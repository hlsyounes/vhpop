/*
 * Copyright (C) 2002 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: plans.cc,v 3.12 2002-03-25 00:45:05 lorens Exp $
 */
#include "plans.h"
#include "heuristics.h"
#include "bindings.h"
#include "reasons.h"
#include "problems.h"
#include "domains.h"
#include "formulas.h"
#include "requirements.h"
#include "parameters.h"
#include "debug.h"
#include <queue>
#include <stack>
#include <algorithm>
#include <typeinfo>
#include <climits>
#include <sys/time.h>
#define __USE_ISOC99 1
#include <cmath>


/*
 * Mapping of predicate names to actions.
 */
struct PredicateActionsMap
  : public hash_multimap<string, const ActionSchema*> {
};

/* Iterator for PredicateActionsMap. */
typedef PredicateActionsMap::const_iterator PredicateActionsMapIter;


/* Planning parameters. */
static const Parameters* params;
/* Domain of problem currently being solved. */
static const Domain* domain = NULL;
/* Planning graph. */
static const PlanningGraph* planning_graph;
/* Maps predicates to actions. */
static PredicateActionsMap achieves_pred;
/* Maps negated predicates to actions. */
static PredicateActionsMap achieves_neg_pred;
/* Whether last flaw was a static predicate. */
static bool static_pred_flaw;


/* ====================================================================== */
/* Link */

/* Constructs a causal link. */
Link::Link(size_t from_id, StepTime effect_time,
	   const OpenCondition& open_cond)
  : from_id_(from_id), effect_time_(effect_time), to_id_(open_cond.step_id()),
    condition_(open_cond.literal()) {
#ifdef TRANSFORMATIONAL
  reason_ = &open_cond.reason();
#endif
}


/* Returns the reason for the link. */
const Reason& Link::reason() const {
#ifdef TRANSFORMATIONAL
  return *reason_;
#else
  return Reason::DUMMY;
#endif
}


/* Equality operator for links. */
bool operator==(const Link& l1, const Link& l2) {
  return &l1 == &l2;
}


/* ====================================================================== */
/* Step */

/* Constructs a step. */
Step::Step(size_t id, const EffectList& effects, const Reason& reason)
  : id_(id), action_(NULL), effects_(&effects.instantiation(id)) {
#ifdef TRANSFORMATIONAL
  reason_ = &reason;
#endif
}


/* Constructs a step instantiated from an action. */
Step::Step(size_t id, const Action& action, const Reason& reason)
  : id_(id), action_(&action),
    effects_(&((typeid(action) == typeid(ActionSchema))
	       ? action.effects.instantiation(id) : action.effects)) {
#ifdef TRANSFORMATIONAL
  reason_ = &reason;
#endif
}


/* Returns the reasons. */
const Reason& Step::reason() const {
#ifdef TRANSFORMATIONAL
  return *reason_;
#else
  return Reason::DUMMY;
#endif
}


/* Sets the reason for this step. */
void Step::set_reason(const Reason& reason) {
#ifdef TRANSFORMATIONAL
  reason_ = &reason;
#endif
}


/* Returns a formula representing this step. */
const Atom* Step::step_formula() const {
  if (action() == NULL) {
    return NULL;
  } else {
    return &((typeid(*action()) == typeid(ActionSchema))
	     ? action()->action_formula().instantiation(id())
	     : action()->action_formula());
  }
}


/* ====================================================================== */
/* Plan */

#ifdef DEBUG
extern size_t created_plans;
extern size_t deleted_plans;
#endif

/*
 * Less than function object for plan pointers.
 */
struct less<const Plan*>
  : public binary_function<const Plan*, const Plan*, bool> {
  bool operator()(const Plan* p1, const Plan* p2) const {
    return *p1 < *p2;
  }
};


/*
 * A plan queue.
 */
struct PlanQueue : public priority_queue<const Plan*, vector<const Plan*>,
		   less<const Plan*> > {
};


/* Id of goal step. */
const size_t Plan::GOAL_ID = UINT_MAX;


/* Adds atomic goal to chain of open conditions, and returns true if
  and only if the goal is consistent. */
static bool add_goal(const OpenConditionChain*& open_conds,
		     size_t& num_open_conds, BindingList& new_bindings,
		     const Formula& goal, size_t step_id,
		     const Reason& reason, bool test_only = false) {
  if (goal.tautology()) {
    return true;
  } else if (goal.contradiction()) {
    return false;
  }
  deque<const Formula*> goals(1, &goal);
  while (!goals.empty()) {
    const Formula* goal = goals.back();
    goals.pop_back();
    const Conjunction* conj = dynamic_cast<const Conjunction*>(goal);
    if (conj != NULL) {
      const FormulaList& gs = conj->conjuncts;
      for (FormulaListIter fi = gs.begin(); fi != gs.end(); fi++) {
	if (params->reverse_open_conditions) {
	  goals.push_front(*fi);
	} else {
	  goals.push_back(*fi);
	}
      }
    } else {
      const Literal* literal = dynamic_cast<const Literal*>(goal);
      if (literal != NULL) {
	if (!test_only) {
	  open_conds =
	    new OpenConditionChain(OpenCondition(step_id, *goal, reason),
				   open_conds);
	}
	num_open_conds++;
      } else {
	const Disjunction* disj = dynamic_cast<const Disjunction*>(goal);
	if (disj != NULL) {
	  if (!test_only) {
	    open_conds =
	      new OpenConditionChain(OpenCondition(step_id, *goal, reason),
				     open_conds);
	  }
	  num_open_conds++;
	} else {
	  const BindingLiteral* bl = dynamic_cast<const BindingLiteral*>(goal);
	  if (bl != NULL) {
	    bool is_eq = (typeid(*bl) == typeid(Equality));
	    const Binding* b = NULL;
	    const Variable* var = dynamic_cast<const Variable*>(&bl->term1);
	    if (var != NULL) {
	      /* The first term is a variable. */
	      if (is_eq) {
		b = new EqualityBinding(*var, bl->term2, reason);
	      } else {
		b = new InequalityBinding(*var, bl->term2, reason);
	      }
	    } else {
	      var = dynamic_cast<const Variable*>(&bl->term2);
	      if (var != NULL) {
		/* The second term is a variable. */
		if (is_eq) {
		  b = new EqualityBinding(*var, bl->term1, reason);
		} else {
		  b = new InequalityBinding(*var, bl->term1, reason);
		}
	      } else if ((is_eq && bl->term1 != bl->term2)
			 || (!is_eq && bl->term1 == bl->term2)) {
		/* Both terms are names, and the binding is inconsistent. */
		return false;
	      }
	    }
	    if (b != NULL) {
#ifdef BRANCH_ON_INEQUALITY
	      const Inequality* neq = dynamic_cast<const Inequality*>(bl);
	      if (params->domain_constraints
		  && neq != NULL && typeid(bl->term1) == typeid(bl->term2)) {
		/* Both terms are variables, so handle specially. */
		if (!test_only) {
		  open_conds =
		    new OpenConditionChain(OpenCondition(step_id, *neq,
							 reason),
					   open_conds);
		  num_open_conds++;
		}
	      } else {
		new_bindings.push_back(b);
	      }
#else
	      new_bindings.push_back(b);
#endif
	    }
	  } else {
	    const ExistsFormula* exists =
	      dynamic_cast<const ExistsFormula*>(goal);
	    if (exists != NULL) {
	      if (params->reverse_open_conditions) {
		goals.push_front(&exists->body);
	      } else {
		goals.push_back(&exists->body);
	      }
	    } else if (dynamic_cast<const ForallFormula*>(goal) != NULL) {
	      throw Unimplemented("adding universally quantified goal");
	    } else {
	      throw Unimplemented("unknown kind of goal");
	    }
	  }
	}
      }
    }
  }
  return true;
}


/* Fills the provided list with actions that achieves the given
   literal. */
static void applicable_actions(ActionList& actions, const Literal& literal) {
 if (params->ground_actions) {
    planning_graph->achieves_formula(actions, literal);
  } else if (typeid(literal) == typeid(Atom)) {
    pair<PredicateActionsMapIter, PredicateActionsMapIter> bounds =
      achieves_pred.equal_range(literal.predicate());
    for (PredicateActionsMapIter pai = bounds.first;
	 pai != bounds.second; pai++) {
      actions.push_back((*pai).second);
    }
  } else {
    pair<PredicateActionsMapIter, PredicateActionsMapIter> bounds =
      achieves_neg_pred.equal_range(literal.predicate());
    for (PredicateActionsMapIter pai = bounds.first;
	 pai != bounds.second; pai++) {
      actions.push_back((*pai).second);
    }
  }
}


/* Finds threats to the given link. */
static void link_threats(const UnsafeChain*& unsafes, size_t& num_unsafes,
			 const Link& link, const StepChain* steps,
			 const Orderings& orderings,
			 const Bindings& bindings) {
  hash_set<size_t> seen_steps;
  StepTime lt1 = link.effect_time();
  StepTime lt2 = end_time(link.condition());
  for (const StepChain* sc = steps; sc != NULL; sc = sc->tail) {
    const Step& s = sc->head;
    if (seen_steps.find(s.id()) == seen_steps.end()
	&& (link.from_id() == s.id()
	    || (orderings.possibly_before(link.from_id(), lt1,
					  s.id(), STEP_END)
		&& orderings.possibly_after(link.to_id(), lt2,
					    s.id(), STEP_START)))) {
      seen_steps.insert(s.id());
      const EffectList& effects = s.effects();
      for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
	const Effect& e = **ei;
	StepTime et = end_time(e);
	if ((link.from_id() == s.id() && lt1 == et)
	    || orderings.possibly_before(link.from_id(), lt1, s.id(), et)
	    || orderings.possibly_after(link.to_id(), lt2, s.id(), et)) {
	  if (typeid(link.condition()) == typeid(Negation)) {
	    const AtomList& adds = e.add_list;
	    for (AtomListIter fi = adds.begin(); fi != adds.end(); fi++) {
	      const Atom& atom = **fi;
	      if (bindings.affects(atom, link.condition())) {
		unsafes = new UnsafeChain(Unsafe(link, s.id(), e, atom),
					  unsafes);
		num_unsafes++;
	      }
	    }
	  } else if (!(link.from_id() == s.id() && lt1 == et)) {
	    const NegationList& dels = e.del_list;
	    for (NegationListIter fi = dels.begin(); fi != dels.end(); fi++) {
	      const Negation& neg = **fi;
	      if (bindings.affects(neg, link.condition())) {
		unsafes = new UnsafeChain(Unsafe(link, s.id(), e, neg),
					  unsafes);
		num_unsafes++;
	      }
	    }
	  }
	}
      }
    }
  }
}


/* Finds the threats by the given step. */
static void step_threats(const UnsafeChain*& unsafes, size_t& num_unsafes,
			 const Step& step, const LinkChain* links,
			 const Orderings& orderings,
			 const Bindings& bindings) {
  for (const LinkChain* lc = links; lc != NULL; lc = lc->tail) {
    const Link& l = lc->head;
    StepTime lt1 = l.effect_time();
    StepTime lt2 = end_time(l.condition());
    if (orderings.possibly_before(l.from_id(), lt1, step.id(), STEP_END)
	&& orderings.possibly_after(l.to_id(), lt2, step.id(), STEP_START)) {
      const EffectList& effects = step.effects();
      for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
	const Effect& e = **ei;
	StepTime et = end_time(e);
	if (orderings.possibly_before(l.from_id(), lt1, step.id(), et)
	    && orderings.possibly_after(l.to_id(), lt2, step.id(), et)) {
	  if (typeid(l.condition()) == typeid(Negation)) {
	    const AtomList& adds = e.add_list;
	    for (AtomListIter fi = adds.begin(); fi != adds.end(); fi++) {
	      const Atom& atom = **fi;
	      if (bindings.affects(atom, l.condition())) {
		unsafes = new UnsafeChain(Unsafe(l, step.id(), e, atom),
					  unsafes);
		num_unsafes++;
	      }
	    }
	  } else {
	    const NegationList& dels = e.del_list;
	    for (NegationListIter fi = dels.begin(); fi != dels.end(); fi++) {
	      const Negation& neg = **fi;
	      if (bindings.affects(neg, l.condition())) {
		unsafes = new UnsafeChain(Unsafe(l, step.id(), e, neg),
					  unsafes);
		num_unsafes++;
	      }
	    }
	  }
	}
      }
    }
  }
}


/* Returns the initial plan representing the given problem, or NULL
   if initial conditions or goals of the problem are inconsistent. */
const Plan* Plan::make_initial_plan(const Problem& problem) {
  /* Reason for initial steps in plan. */
  const Reason& init_reason = InitReason::make(*params);

  /*
   * Create goal of problem.
   */
  const Formula* goal;
  if (params->ground_actions) {
    goal = &problem.goal.instantiation(SubstitutionList(), problem);
  } else {
    goal = &problem.goal.instantiation(GOAL_ID);
  }
  /* Reason for open conditions of goal. */
  const Reason& goal_reason = AddStepReason::make(*params, GOAL_ID);
  /* Chain of open conditions. */
  const OpenConditionChain* open_conds = NULL;
  /* Number of open conditions. */
  size_t num_open_conds = 0;
  /* Bindings introduced by goal. */
  BindingList new_bindings;
  /* Add goals as open conditions. */
  if (!add_goal(open_conds, num_open_conds, new_bindings,
		*goal, GOAL_ID, goal_reason)) {
    /* Goals are inconsistent. */
    if (open_conds != NULL) {
      delete open_conds;
    }
    return NULL;
  }
  /* Make chain of initial steps. */
  EffectList& init = *(new EffectList(&problem.init));
  const StepChain* steps =
    new StepChain(Step(GOAL_ID, EffectList::EMPTY, init_reason),
		  new StepChain(Step(0, init, init_reason), NULL));
  /* Variable bindings. */
  const Bindings* bindings = &Bindings::make_bindings(steps, planning_graph);
  if (bindings == NULL) {
    /* Bindings are inconsistent. */
    if (open_conds != NULL) {
      delete open_conds;
    }
    return NULL;
  }
  /* Step orderings. */
  const Orderings* orderings;
  if (domain->requirements.durative_actions) {
    orderings = new TemporalOrderings();
  } else {
    orderings = new BinaryOrderings();
  }
  /* Return initial plan. */
  return new Plan(steps, 0, NULL, 0, *orderings, *bindings,
		  NULL, 0, open_conds, num_open_conds, NULL);
}


/* Returns plan for given problem. */
const Plan* Plan::plan(const Problem& problem, const Parameters& p,
		       bool last_problem) {
  /* Set planning parameters. */
  params = &p;
  /* Set current domain. */
  domain = &problem.domain;

  /*
   * Initialize planning graph and maps from predicates to actions.
   */
  if (params->ground_actions || params->domain_constraints
      || params->heuristic.needs_planning_graph()
      || params->flaw_order.needs_planning_graph()) {
    planning_graph = new PlanningGraph(problem, params->domain_constraints);
  } else {
    planning_graph = NULL;
  }
  if (!params->ground_actions) {
    achieves_pred.clear();
    achieves_neg_pred.clear();
    for (ActionSchemaMapIter ai = domain->actions().begin();
	 ai != domain->actions().end(); ai++) {
      const ActionSchema* as = (*ai).second;
      if (params->domain_constraints && !params->keep_static_preconditions) {
	as = &as->strip_static(*domain);
      }
      hash_set<string> preds;
      hash_set<string> neg_preds;
      as->achievable_predicates(preds, neg_preds);
      for (hash_set<string>::const_iterator si = preds.begin();
	   si != preds.end(); si++) {
	achieves_pred.insert(make_pair(*si, as));
      }
      for (hash_set<string>::const_iterator si = neg_preds.begin();
	   si != neg_preds.end(); si++) {
	achieves_neg_pred.insert(make_pair(*si, as));
      }
    }
  }
  static_pred_flaw = false;

  /* Number of visited plan. */
  size_t num_visited_plans = 0;
  /* Number of generated plans. */
  size_t num_generated_plans = 0;
  /* Number of static preconditions encountered. */
  size_t num_static = 0;

  /* Queue of pending plans. */
  PlanQueue plans;
  /* Construct the initial plan. */
  const Plan* initial_plan = make_initial_plan(problem);
  const Plan* current_plan = initial_plan;
  if (current_plan != NULL) {
    current_plan->id_ = 0;
    num_generated_plans++;
  }

  /* Variable for progress bar (number of generated plans). */
  size_t last_dot = 0;
  /* Variable for progress bar (time). */
  size_t last_hash = 0;

  /*
   * Search for complete plan.
   */
  float f_limit;
  if (current_plan != NULL
      && params->search_algorithm == Parameters::IDA_STAR) {
    f_limit = current_plan->primary_rank();
  } else {
    f_limit = INFINITY;
  }
  do {
    float next_f_limit = INFINITY;
    while (current_plan != NULL && !current_plan->complete()) {
      if (num_generated_plans - num_static >= params->search_limit) {
	/* Search limit exceeded. */
	break;
      }
      struct itimerval timer;
      getitimer(ITIMER_PROF, &timer);
      double t = 1000000.9
	- (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
      if (t >= 60.0*params->time_limit) {
	/* Time limit exceeded. */
	break;
      }

      /*
       * Visiting a new plan.
       */
      num_visited_plans++;
      if (verbosity == 1) {
	while (num_generated_plans - num_static - last_dot >= 1000) {
	  cout << '.';
	  last_dot += 1000;
	}
	while (t - 60.0*last_hash >= 60.0) {
	  cout << '#';
	  last_hash++;
	}
      }
      if (verbosity > 1) {
	cout << endl << (num_visited_plans - num_static) << ": "
	     << "!!!!CURRENT PLAN (id " << current_plan->id_ << ")"
	     << " with rank (" << current_plan->primary_rank();
	for (size_t ri = 1; ri < current_plan->rank_.size(); ri++) {
	  cout << ',' << current_plan->rank_[ri];
	}
	cout << ")" << endl
	     << *current_plan << endl;
      }
      /* List of children to current plan. */
      PlanList refinements;
      /* Get plan refinements. */
      current_plan->refinements(refinements);
      /* Add children to queue of pending plans. */
      bool added = false;
      for (PlanListIter pi = refinements.begin();
	   pi != refinements.end(); pi++) {
	const Plan& new_plan = **pi;
	/* N.B. Must set id before computing rank, because it may be used. */
	new_plan.id_ = num_generated_plans;
	if (!isinf(new_plan.primary_rank())
	    && num_generated_plans - num_static < params->search_limit) {
	  if (params->search_algorithm == Parameters::IDA_STAR
	      && new_plan.primary_rank() > f_limit) {
	    next_f_limit = min(next_f_limit, new_plan.primary_rank());
	    delete &new_plan;
	    continue;
	  }
	  if (!added && static_pred_flaw) {
	    num_static++;
	  }
	  added = true;
	  plans.push(&new_plan);
	  num_generated_plans++;
	  if (verbosity > 2) {
	    cout << endl << "####CHILD (id " << new_plan.id_ << ")"
		 << " with rank (" << new_plan.primary_rank();
	    for (size_t ri = 1; ri < new_plan.rank_.size(); ri++) {
	      cout << ',' << new_plan.rank_[ri];
	    }
	    cout << "):" << endl
		 << new_plan << endl;
	  }
	}
      }

      /*
       * Process next plan.
       */
      do {
	if (!params->transformational && current_plan != initial_plan) {
	  delete current_plan;
	}
	if (plans.empty()) {
	  /* Problem lacks solution. */
	  current_plan = NULL;
	} else {
	  current_plan = plans.top();
	  plans.pop();
	}
      } while (current_plan != NULL && current_plan->duplicate());
      if (params->search_algorithm == Parameters::HILL_CLIMBING) {
	/* Discard the rest of the plan queue. */
	while (!plans.empty()) {
	  delete plans.top();
	  plans.pop();
	}
      }
    }
    if (current_plan != NULL && current_plan->complete()) {
      break;
    }
    f_limit = next_f_limit;
    if (!isinf(f_limit)) {
      /* Restart search. */
      if (current_plan != NULL && current_plan != initial_plan) {
	delete current_plan;
      }
      current_plan = initial_plan;
      while (!plans.empty()) {
	delete plans.top();
	plans.pop();
      }
    }
  } while (!isinf(f_limit));
  if (verbosity > 0) {
    /*
     * Print statistics.
     */
    cout << endl << "Plans generated: " << num_generated_plans;
    if (num_static > 0) {
      cout << " [" << (num_generated_plans - num_static) << "]";
    }
    cout << endl << "Plans visited: " << num_visited_plans;
    if (num_static > 0) {
      cout << " [" << (num_visited_plans - num_static) << "]";
    }
    cout << endl;
  }
  if (current_plan != initial_plan) {
    delete initial_plan;
  }
  /* Discard the rest of the plan queue. */
  if (!last_problem) {
    while (!plans.empty()) {
      delete plans.top();
      plans.pop();
    }
  }
  /* Return last plan, or NULL if problem does not have a solution. */
  return current_plan;
}


/* Constructs a plan. */
Plan::Plan(const StepChain* steps, size_t num_steps,
	   const LinkChain* links, size_t num_links,
	   const Orderings& orderings, const Bindings& bindings,
	   const UnsafeChain* unsafes, size_t num_unsafes,
	   const OpenConditionChain* open_conds, size_t num_open_conds,
	   const Plan* parent, PlanType type)
  : steps_(steps), num_steps_(num_steps),
    links_(links), num_links_(num_links),
    orderings_(&orderings), bindings_(&bindings),
    unsafes_(unsafes), num_unsafes_(num_unsafes),
    open_conds_(open_conds), num_open_conds_(num_open_conds) {
  StepChain::register_use(steps);
  LinkChain::register_use(links);
  Collectible::register_use(&orderings);
  Collectible::register_use(&bindings);
  UnsafeChain::register_use(unsafes);
  OpenConditionChain::register_use(open_conds);
#ifdef DEBUG
  created_plans++;
  depth_ = (parent != NULL) ? parent->depth() + 1 : 0;
#endif
#ifdef TRANSFORMATIONAL
  if (params->transformational) {
    parent_ = ((parent != NULL && parent->type_ == INTERMEDIATE_PLAN)
	       ? parent->parent_ : parent);
  } else {
    parent_ = NULL;
  }
  type_ = ((parent != NULL && parent->type_ == INTERMEDIATE_PLAN)
	   ? TRANSFORMED_PLAN : type);
#endif
  if (parent != NULL) {
    if (steps != NULL && steps->head.id() < GOAL_ID) {
      high_step_id_ = max(parent->high_step_id_, steps->head.id());
    } else {
      high_step_id_ = parent->high_step_id_;
    }
  } else {
    high_step_id_ = 0;
  }
}


/* Deletes this plan. */
Plan::~Plan() {
#ifdef DEBUG
  deleted_plans++;
#endif
  StepChain::unregister_use(steps_);
  LinkChain::unregister_use(links_);
  Collectible::unregister_use(orderings_);
  Collectible::unregister_use(bindings_);
  UnsafeChain::unregister_use(unsafes_);
  OpenConditionChain::unregister_use(open_conds_);
}


/* Returns the bindings of this plan. */
const Bindings* Plan::bindings() const {
  return params->ground_actions ? NULL : bindings_;
}


/* Checks if this plan is complete. */
bool Plan::complete() const {
  return unsafes() == NULL && open_conds() == NULL;
}


/* Returns the primary rank of this plan, where a lower rank
   signifies a better plan. */
float Plan::primary_rank() const {
  if (rank_.empty()) {
    params->heuristic.plan_rank(rank_, *this, params->weight, *domain,
				planning_graph);
  }
  return rank_[0];
}


/* Returns the serial number of this plan. */
size_t Plan::serial_no() const {
  return id_;
}


/* Returns the next flaw to work on. */
const Flaw& Plan::get_flaw() const {
  const Flaw& flaw = params->flaw_order.select(*this, *domain, planning_graph);
  if (!params->ground_actions) {
    const OpenCondition* open_cond = dynamic_cast<const OpenCondition*>(&flaw);
    static_pred_flaw = (open_cond != NULL && open_cond->is_static(*domain));
  }
  return flaw;
}


/* Returns the refinements for the next flaw to work on. */
void Plan::refinements(PlanList& plans) const {
  const Flaw& flaw = get_flaw();
  if (verbosity > 1) {
    cout << endl << "handle " << flaw << endl;
  }
  const Unsafe* unsafe = dynamic_cast<const Unsafe*>(&flaw);
  if (unsafe != NULL) {
    handle_unsafe(plans, *unsafe);
  } else {
    const OpenCondition* open_cond = dynamic_cast<const OpenCondition*>(&flaw);
    if (open_cond != NULL) {
      handle_open_condition(plans, *open_cond);
    } else {
      throw Unimplemented("unknown kind of flaw");
    }
  }
}


/* Handles an unsafe link. */
void Plan::handle_unsafe(PlanList& plans, const Unsafe& unsafe) const {
  size_t num_prev_plans = plans.size();
  const Link& link = unsafe.link();
  StepTime lt1 = link.effect_time();
  StepTime lt2 = end_time(link.condition());
  StepTime et = end_time(unsafe.effect());
  if (((link.from_id() == unsafe.step_id() && lt1 == et)
       || (orderings().possibly_before(link.from_id(), lt1,
				       unsafe.step_id(), et)
	   && orderings().possibly_after(link.to_id(), lt2,
				       unsafe.step_id(), et)))
      && bindings_->affects(unsafe.effect_add(), link.condition())) {
    separate(plans, unsafe);
    promote(plans, unsafe);
    demote(plans, unsafe);
    if (num_prev_plans == plans.size()) {
      if (params->transformational) {
	if (verbosity > 2) {
	  cout << endl << "++++DEAD END:" << endl << *this << endl;
	}
	relink(plans, unsafe.link());
	if (verbosity > 2) {
	  for (size_t i = num_prev_plans; i < plans.size(); i++) {
	    cout << "^^^^Transformed plan" << endl << *plans[i] << endl;
	    plans[i]->duplicate();
	  }
	}
      }
    }
  } else {
    /* bogus flaw */
    plans.push_back(new Plan(steps(), num_steps(), links(), num_links(),
			     orderings(), *bindings_,
			     unsafes()->remove(unsafe), num_unsafes() - 1,
			     open_conds(), num_open_conds(), this));
  }
}


/* Checks if the given threat is separable. */
int Plan::separable(const Unsafe& unsafe) const {
  SubstitutionList unifier;
  const Link& link = unsafe.link();
  StepTime lt1 = link.effect_time();
  StepTime lt2 = end_time(link.condition());
  StepTime et = end_time(unsafe.effect());
  if (((link.from_id() == unsafe.step_id() && lt1 == et)
       || (orderings().possibly_before(link.from_id(), lt1,
				       unsafe.step_id(), et)
	   && orderings().possibly_after(link.to_id(), lt2,
				       unsafe.step_id(), et)))
      && bindings_->affects(unifier,
			    unsafe.effect_add(), link.condition())) {
    const VariableList& effect_forall = unsafe.effect().forall;
    const Formula* goal = &Formula::FALSE;
    for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
      const Substitution& subst = *si;
      if (!member(effect_forall.begin(), effect_forall.end(), &subst.var())) {
	const Inequality& neq = *new Inequality(subst.var(), subst.term());
	if (bindings_->consistent_with(neq)) {
	  goal = &(*goal || neq);
	}
      }
    }
    const Formula& effect_cond = unsafe.effect().condition;
    if (!effect_cond.tautology()) {
      if (!effect_forall.empty()) {
	SubstitutionList forall_subst;
	for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
	  const Substitution& subst = *si;
	  if (member(effect_forall.begin(), effect_forall.end(),
		     &subst.var())) {
	    forall_subst.push_back(subst);
	  }
	}
	goal = &(*goal || !effect_cond.substitution(forall_subst));
      } else {
	goal = &(*goal || !effect_cond);
      }
    }
    const OpenConditionChain* new_open_conds = NULL;
    size_t new_num_open_conds = 0;
    BindingList new_bindings;
    if (add_goal(new_open_conds, new_num_open_conds, new_bindings, *goal,
		 unsafe.step_id(), Reason::DUMMY, true)) {
      if (bindings_->add(new_bindings, true) != NULL) {
	return 1;
      }
    }
  } else {
    /* Threat is no longer active. */
    return -1;
  }
  return 0;
}


/* Handles an unsafe link through separation. */
void Plan::separate(PlanList& plans, const Unsafe& unsafe) const {
  SubstitutionList unifier;
  bindings_->affects(unifier, unsafe.effect_add(), unsafe.link().condition());
  const VariableList& effect_forall = unsafe.effect().forall;
  const Formula* goal = &Formula::FALSE;
  for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
    const Substitution& subst = *si;
    if (!member(effect_forall.begin(), effect_forall.end(), &subst.var())) {
      const Inequality& neq = *new Inequality(subst.var(), subst.term());
      if (bindings_->consistent_with(neq)) {
	goal = &(*goal || neq);
      }
    }
  }
  const Formula& effect_cond = unsafe.effect().condition;
  if (!effect_cond.tautology()) {
    if (!effect_forall.empty()) {
      SubstitutionList forall_subst;
      for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
	const Substitution& subst = *si;
	if (member(effect_forall.begin(), effect_forall.end(), &subst.var())) {
	  forall_subst.push_back(subst);
	}
      }
      goal = &(*goal || !effect_cond.substitution(forall_subst));
    } else {
      goal = &(*goal || !effect_cond);
    }
  }
  const OpenConditionChain* new_open_conds = open_conds();
  size_t new_num_open_conds = num_open_conds();
  BindingList new_bindings;
  const Reason& protect_reason =
    ProtectReason::make(*params, unsafe.link(), unsafe.step_id());
  bool added = add_goal(new_open_conds, new_num_open_conds, new_bindings,
			*goal, unsafe.step_id(), protect_reason);
  OpenConditionChain::register_use(new_open_conds);
  if (added) {
    const Bindings* bindings = bindings_->add(new_bindings);
    if (bindings != NULL) {
      plans.push_back(new Plan(steps(), num_steps(), links(), num_links(),
			       orderings(), *bindings,
			       unsafes()->remove(unsafe), num_unsafes() - 1,
			       new_open_conds, new_num_open_conds, this));
    }
  }
  OpenConditionChain::unregister_use(new_open_conds);
}


/* Checsk if the given threat is demotable. */
int Plan::demotable(const Unsafe& unsafe) const {
  const Link& link = unsafe.link();
  StepTime lt1 = link.effect_time();
  StepTime et = end_time(unsafe.effect());
  return orderings().possibly_before(unsafe.step_id(), et,
				     link.from_id(), lt1) ? 1 : 0;
}


/* Handles an unsafe link through demotion. */
void Plan::demote(PlanList& plans, const Unsafe& unsafe) const {
  const Link& link = unsafe.link();
  StepTime lt1 = link.effect_time();
  StepTime et = end_time(unsafe.effect());
  if (orderings().possibly_before(unsafe.step_id(), et, link.from_id(), lt1)) {
    new_ordering(plans, unsafe.step_id(), et, link.from_id(), lt1, unsafe);
  }
}


/* Checks if the given threat is promotable. */
int Plan::promotable(const Unsafe& unsafe) const {
  const Link& link = unsafe.link();
  StepTime lt2 = end_time(link.condition());
  StepTime et = end_time(unsafe.effect());
  return orderings().possibly_before(link.to_id(), lt2,
				     unsafe.step_id(), et) ? 1 : 0;
}


/* Handles an unsafe link through promotion. */
void Plan::promote(PlanList& plans, const Unsafe& unsafe) const {
  const Link& link = unsafe.link();
  StepTime lt2 = end_time(link.condition());
  StepTime et = end_time(unsafe.effect());
  if (orderings().possibly_before(link.to_id(), lt2, unsafe.step_id(), et)) {
    new_ordering(plans, link.to_id(), lt2, unsafe.step_id(), et, unsafe);
  }
}


/* Adds a plan to the given plan list with an ordering added. */
void Plan::new_ordering(PlanList& plans, size_t before_id, StepTime t1,
			size_t after_id, StepTime t2,
			const Unsafe& unsafe) const {
  const Reason& protect_reason =
    ProtectReason::make(*params, unsafe.link(), unsafe.step_id());
  const Orderings* new_orderings =
    orderings().refine(Ordering(before_id, t1, after_id, t2, protect_reason));
  if (new_orderings != NULL) {
    plans.push_back(new Plan(steps(), num_steps(), links(), num_links(),
			     *new_orderings, *bindings_,
			     unsafes()->remove(unsafe), num_unsafes() - 1,
			     open_conds(), num_open_conds(), this));
  }
}


/* Checks if the given open conditions is threatened. */
bool Plan::unsafe_open_condition(const OpenCondition& open_cond) const {
  const Literal* literal = open_cond.literal();
  if (literal != NULL) {
    const Literal& goal = *literal;
    StepTime gt = end_time(goal);
    hash_set<size_t> seen_steps;
    for (const StepChain* sc = steps(); sc != NULL; sc = sc->tail) {
      const Step& s = sc->head;
      if (seen_steps.find(s.id()) == seen_steps.end()
	  && orderings().possibly_before(s.id(), STEP_START,
					 open_cond.step_id(), gt)) {
	seen_steps.insert(s.id());
	const EffectList& effects = s.effects();
	for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
	  const Effect& e = **ei;
	  StepTime et = end_time(e);
	  if (orderings().possibly_before(s.id(), et,
					  open_cond.step_id(), gt)) {
	    if (typeid(goal) == typeid(Negation)) {
	      const AtomList& adds = e.add_list;
	      for (AtomListIter fi = adds.begin(); fi != adds.end(); fi++) {
		if (bindings_->affects(**fi, goal)) {
		  return true;
		}
	      }
	    } else {
	      const NegationList& dels = e.del_list;
	      for (NegationListIter fi = dels.begin();
		   fi != dels.end(); fi++) {
		if (bindings_->affects(**fi, goal)) {
		  return true;
		}
	      }
	    }
	  }
	}
      }
    }
  }
  return false;
}


/* Handles an open condition. */
void Plan::handle_open_condition(PlanList& plans,
				 const OpenCondition& open_cond) const {
  const Literal* literal = open_cond.literal();
  if (literal != NULL) {
    add_step(plans, *literal, open_cond);
    reuse_step(plans, *literal, open_cond);
  } else {
    const Disjunction* disj = open_cond.disjunction();
    if (disj != NULL) {
      handle_disjunction(plans, *disj, open_cond);
    } else {
      const Inequality* neq = open_cond.inequality();
      if (neq != NULL) {
	handle_inequality(plans, *neq, open_cond);
      } else {
	throw Unimplemented("unknown kind of open condition");
      }
    }
  }
}


/* Counts the number of refinements for the given disjunctive open
   condition. */
int Plan::disjunction_refinements(const Disjunction& disj,
				  size_t step_id) const {
  int count = 0;
  const FormulaList& disjuncts = disj.disjuncts;
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    BindingList new_bindings;
    const OpenConditionChain* new_open_conds = NULL;
    size_t new_num_open_conds = 0;
    if (add_goal(new_open_conds, new_num_open_conds, new_bindings, **fi,
		 step_id, Reason::DUMMY, true)) {
      if (bindings_->add(new_bindings, true) != NULL) {
	count++;
      }
    }
  }
  return count;
}


/* Handles a disjunctive open condition. */
void
Plan::handle_disjunction(PlanList& plans, const Disjunction& disj,
			 const OpenCondition& open_cond) const {
  const FormulaList& disjuncts = disj.disjuncts;
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    BindingList new_bindings;
    const OpenConditionChain* new_open_conds = open_conds()->remove(open_cond);
    size_t new_num_open_conds = num_open_conds() - 1;
    bool added = add_goal(new_open_conds, new_num_open_conds, new_bindings,
			  **fi, open_cond.step_id(), open_cond.reason());
    OpenConditionChain::register_use(new_open_conds);
    if (added) {
      const Bindings* bindings = bindings_->add(new_bindings);
      if (bindings != NULL) {
	plans.push_back(new Plan(steps(), num_steps(), links(), num_links(),
				 orderings(), *bindings,
				 unsafes(), num_unsafes(),
				 new_open_conds, new_num_open_conds, this));
      }
    }
    OpenConditionChain::unregister_use(new_open_conds);
  }
}


/* Counts the number of refinements for the given inequality open
   condition. */
int Plan::inequality_refinements(const Inequality& neq) const {
  int count = 0;
  const StepVar& v1 = dynamic_cast<const StepVar&>(neq.term1);
  const StepVar& v2 = dynamic_cast<const StepVar&>(neq.term2);
  const NameSet* d1 = bindings_->domain(v1);
  const NameSet* d2 = bindings_->domain(v2);
  if (d1 == NULL || d2 == NULL) {
    /* Domain constraints not found for both variables. */
    return 0;
  }

  /*
   * Branch on the variable with the smallest domain.
   */
  const StepVar* var1;
  const StepVar* var2;
  const NameSet* var_domain;
  if (d1->size() < d2->size()) {
    var1 = &v1;
    var2 = &v2;
    var_domain = d1;
  } else {
    var1 = &v2;
    var2 = &v1;
    var_domain = d2;
  }
  for (NameSetIter ni = var_domain->begin(); ni != var_domain->end(); ni++) {
    const Name& name = **ni;
    BindingList new_bindings;
    new_bindings.push_back(new EqualityBinding(*var1, name, Reason::DUMMY));
    new_bindings.push_back(new InequalityBinding(*var2, name, Reason::DUMMY));
    if (bindings_->add(new_bindings, true) != NULL) {
      count++;
    }
  }
  return count;
}

  
/* Handles inequality open condition. */
void Plan::handle_inequality(PlanList& plans, const Inequality& neq,
			     const OpenCondition& open_cond) const {
  const StepVar& v1 = dynamic_cast<const StepVar&>(neq.term1);
  const StepVar& v2 = dynamic_cast<const StepVar&>(neq.term2);
  const NameSet* d1 = bindings_->domain(v1);
  const NameSet* d2 = bindings_->domain(v2);
  if (d1 == NULL || d2 == NULL) {
    /* Domain constraints not found for both variables. */
    return;
  }

  /*
   * Branch on the variable with the smallest domain.
   */
  const StepVar* var1;
  const StepVar* var2;
  const NameSet* var_domain;
  if (d1->size() < d2->size()) {
    var1 = &v1;
    var2 = &v2;
    var_domain = d1;
  } else {
    var1 = &v2;
    var2 = &v1;
    var_domain = d2;
  }
  for (NameSetIter ni = var_domain->begin(); ni != var_domain->end(); ni++) {
    const Name& name = **ni;
    BindingList new_bindings;
    new_bindings.push_back(new EqualityBinding(*var1, name,
					       open_cond.reason()));
    new_bindings.push_back(new InequalityBinding(*var2, name,
						 open_cond.reason()));
    const Bindings* bindings = bindings_->add(new_bindings);
    if (bindings != NULL) {
      plans.push_back(new Plan(steps(), num_steps(), links(), num_links(),
			       orderings(), *bindings,
			       unsafes(), num_unsafes(),
			       open_conds()->remove(open_cond),
			       num_open_conds() - 1, this));
    }
  }
}


/* Counts the number of add-step refinements for the given literal
   open condition, and returns true iff the number of refinements
   does not exceed the given limit. */
bool Plan::addable_steps(int& refinements, const Literal& literal,
			 size_t step_id, int limit) const {
  int count = 0;
  ActionList actions;
  applicable_actions(actions, literal);
  if (!actions.empty()) {
    for (ActionListIter ai = actions.begin(); ai != actions.end(); ai++) {
      const Action& action = **ai;
      if (!count_new_links(count, high_step_id_ + 1, action, action.effects,
			   literal, step_id, limit)) {
	return false;
      }
    }
  }
  refinements = count;
  return count;
}


/* Handles a literal open condition by adding a new step. */
void Plan::add_step(PlanList& plans, const Literal& literal,
		    const OpenCondition& open_cond) const {
  ActionList actions;
  applicable_actions(actions, literal);
  if (!actions.empty()) {
    size_t step_id = high_step_id_ + 1;
    for (ActionListIter ai = actions.begin(); ai != actions.end(); ai++) {
      new_link(plans, Step(step_id, **ai, Reason::DUMMY), literal, open_cond);
    }
  }
}


/* Counts the number of reuse-step refinements for the given literal
   open condition, and returns true iff the number of refinements
   does not exceed the given limit. */
bool Plan::reusable_steps(int& refinements, const Literal& literal,
			  size_t step_id, int limit) const {
  int count = 0;
  hash_set<size_t> seen_steps;
  StepTime gt = start_time(literal);
  for (const StepChain* sc = steps(); sc != NULL; sc = sc->tail) {
    const Step& step = sc->head;
    if (seen_steps.find(step.id()) == seen_steps.end() &&
	orderings().possibly_before(step.id(), STEP_START, step_id, gt)) {
      seen_steps.insert(step.id());
      if (!count_new_links(count, step.id(), *step.action(), step.effects(),
			   literal, step_id, limit)) {
	return false;
      }
    }
  }
  refinements = count;
  return true;
}


/* Handles a literal open condition by reusing an existing step. */
void Plan::reuse_step(PlanList& plans, const Literal& literal,
		      const OpenCondition& open_cond) const {
  hash_set<size_t> seen_steps;
  StepTime gt = start_time(literal);
  for (const StepChain* sc = steps(); sc != NULL; sc = sc->tail) {
    const Step& step = sc->head;
    if (seen_steps.find(step.id()) == seen_steps.end() &&
	orderings().possibly_before(step.id(), STEP_START,
				    open_cond.step_id(), gt)) {
      seen_steps.insert(step.id());
      new_link(plans, step, literal, open_cond);
    }
  }
}


/* Counts the number of new links that can be established between
   the given effects and the open condition, and returns true iff
   the number of refinements does not exceed the given limit. */
bool Plan::count_new_links(int& count, size_t step_id, const Action& action,
			   const EffectList& effects,
			   const Literal& literal, size_t oc_step_id,
			   int limit) const {
  if (step_id == 0) {
    const Negation* negation = dynamic_cast<const Negation*>(&literal);
    if (negation != NULL) {
      count += cw_link_possible(effects, *negation);
      if (count > limit) {
	return false;
      }
    }
  }
  const Literal& goal = literal;
  StepTime gt = start_time(goal);
  for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
    const Effect& effect = **ei;
    StepTime et = end_time(effect);
    if (step_id > high_step_id_
	|| orderings().possibly_before(step_id, et, oc_step_id, gt)) {
      if (typeid(goal) == typeid(Atom)) {
	const AtomList& adds = effect.add_list;
	for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
	  SubstitutionList mgu;
	  if (bindings_->unify(mgu, goal, **gi)) {
	    count += link_possible(step_id, action, effect, mgu);
	    if (count > limit) {
	      return false;
	    }
	  }
	}
      } else {
	const NegationList& dels = effect.del_list;
	for (NegationListIter gi = dels.begin(); gi != dels.end(); gi++) {
	  SubstitutionList mgu;
	  if (bindings_->unify(mgu, goal, **gi)) {
	    count += link_possible(step_id, action, effect, mgu);
	    if (count > limit) {
	      return false;
	    }
	  }
	}
      }
    }
  }
  return true;
}


/* Adds plans to the given plan list with a link from the given step
   to the given open condition added. */
void Plan::new_link(PlanList& plans, const Step& step, const Literal& literal,
		    const OpenCondition& open_cond) const {
  if (step.id() == 0 && typeid(literal) == typeid(Negation)) {
    new_cw_link(plans, step, literal, open_cond);
  }
  const Literal& goal = literal;
  StepTime gt = start_time(goal);
  const EffectList& effs = step.effects();
  for (EffectListIter ei = effs.begin(); ei != effs.end(); ei++) {
    const Effect& effect = **ei;
    StepTime et = end_time(effect);
    if (step.id() > high_step_id_
	|| orderings().possibly_before(step.id(), et,
				       open_cond.step_id(), gt)) {
      const LinkChain* new_links =
	new LinkChain(Link(step.id(), et, open_cond), links());
      LinkChain::register_use(new_links);
      const Link& link = new_links->head;
      const Reason& reason = EstablishReason::make(*params, link);
      if (typeid(goal) == typeid(Atom)) {
	const AtomList& adds = effect.add_list;
	for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
	  SubstitutionList mgu;
	  if (bindings_->unify(mgu, goal, **gi)) {
	    const Plan* new_plan = make_link(step, effect, literal, open_cond,
					     new_links, reason, mgu);
	    if (new_plan != NULL) {
	      plans.push_back(new_plan);
	    }
	  }
	}
      } else {
	const NegationList& dels = effect.del_list;
	for (NegationListIter gi = dels.begin(); gi != dels.end(); gi++) {
	  SubstitutionList mgu;
	  if (bindings_->unify(mgu, goal, **gi)) {
	    const Plan* new_plan = make_link(step, effect, literal, open_cond,
					     new_links, reason, mgu);
	    if (new_plan != NULL) {
	      plans.push_back(new_plan);
	    }
	  }
	}
      }
      LinkChain::unregister_use(new_links);
    }
  }
}


/* Checks if a new link be established between the given effects and
   the open condition using the closed world assumption. */
int Plan::cw_link_possible(const EffectList& effects,
			   const Negation& negation) const {
  const Atom& goal = negation.atom;
  const Formula* goals = &Formula::TRUE;
  for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
    const Effect& effect = **ei;
    const AtomList& adds = effect.add_list;
    for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
      SubstitutionList mgu;
      if (bindings_->unify(mgu, goal, **gi)) {
	if (mgu.empty()) {
	  /* Impossible to separate goal and initial condition. */
	  return 0;
	}
	const Formula* binds = &Formula::FALSE;
	for (SubstListIter si = mgu.begin(); si != mgu.end(); si++) {
	  const Substitution& subst = *si;
	  binds = &(*binds || *(new Inequality(subst.var(), subst.term())));
	}
	goals = &(*goals && *binds);
      }
    }
  }
  BindingList new_bindings;
  const OpenConditionChain* new_open_conds = NULL;
  size_t new_num_open_conds = 0;
  if (add_goal(new_open_conds, new_num_open_conds, new_bindings, *goals,
	       0, Reason::DUMMY, true)) {
    if (bindings_->add(new_bindings, true) != NULL) {
      return 1;
    }
  }
  return 0;
}


/* Adds plans to the given plan list with a link from the given step
   to the given open condition added using the closed world
   assumption. */
void Plan::new_cw_link(PlanList& plans, const Step& step,
		       const Literal& literal,
		       const OpenCondition& open_cond) const {
  const Negation& negation = dynamic_cast<const Negation&>(literal);
  const Atom& goal = negation.atom;
  const Formula* goals = &Formula::TRUE;
  const EffectList& effs = step.effects();
  for (EffectListIter ei = effs.begin(); ei != effs.end(); ei++) {
    const Effect& effect = **ei;
    const AtomList& adds = effect.add_list;
    for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
      SubstitutionList mgu;
      if (bindings_->unify(mgu, goal, **gi)) {
	if (mgu.empty()) {
	  /* Impossible to separate goal and initial condition. */
	  return;
	}
	const Formula* binds = &Formula::FALSE;
	for (SubstListIter si = mgu.begin(); si != mgu.end(); si++) {
	  const Substitution& subst = *si;
	  binds = &(*binds || *(new Inequality(subst.var(), subst.term())));
	}
	goals = &(*goals && *binds);
      }
    }
  }
  const LinkChain* new_links =
    new LinkChain(Link(step.id(), STEP_END, open_cond), links());
  LinkChain::register_use(new_links);
  const Link& link = new_links->head;
  const Reason& reason = EstablishReason::make(*params, link);
  BindingList new_bindings;
  const OpenConditionChain* new_open_conds = open_conds()->remove(open_cond);
  size_t new_num_open_conds = num_open_conds() - 1;
  bool added = add_goal(new_open_conds, new_num_open_conds, new_bindings,
			*goals, step.id(), reason);
  OpenConditionChain::register_use(new_open_conds);
  if (added) {
    const Bindings* bindings = bindings_->add(new_bindings);
    if (bindings != NULL) {
      const UnsafeChain* new_unsafes = unsafes();
      size_t new_num_unsafes = num_unsafes();
      link_threats(new_unsafes, new_num_unsafes, link, steps(), orderings(),
		   *bindings);
      plans.push_back(new Plan(steps(), num_steps(),
			       new_links, num_links() + 1,
			       orderings(), *bindings,
			       new_unsafes, new_num_unsafes,
			       new_open_conds, new_num_open_conds, this));
    }
  }
  OpenConditionChain::unregister_use(new_open_conds);
  LinkChain::unregister_use(new_links);
}


/* Checks if a new link can be established between the given effect
   and the open condition. */
int Plan::link_possible(size_t step_id, const Action& action,
			const Effect& effect,
			const SubstitutionList& unifier) const {
  /*
   * Add bindings needed to unify effect and goal.
   */
  const VariableList& effect_forall = effect.forall;
  BindingList new_bindings;
  for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
    const Substitution& subst = *si;
    if (!member(effect_forall.begin(), effect_forall.end(), &subst.var())) {
      new_bindings.push_back(new EqualityBinding(subst, Reason::DUMMY));
    }
  }

  /*
   * If the effect is conditional, add condition as goal.
   */
  const OpenConditionChain* new_open_conds = NULL;
  size_t new_num_open_conds = 0;
  const Formula* cond_goal = NULL;
  if (!effect.condition.tautology()) {
    if (!effect_forall.empty()) {
      SubstitutionList forall_subst;
      for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
	const Substitution& subst = *si;
	if (member(effect_forall.begin(), effect_forall.end(), &subst.var())) {
	  forall_subst.push_back(subst);
	}
      }
      cond_goal = &effect.condition.substitution(forall_subst);
    } else {
      cond_goal = &effect.condition;
    }
    if (!add_goal(new_open_conds, new_num_open_conds, new_bindings, *cond_goal,
		  step_id, Reason::DUMMY, true)) {
      return 0;
    }
  }

  /*
   * See if this is a new step.
   */
  const Bindings* bindings = bindings_;
  if (step_id > high_step_id_) {
    if (!add_goal(new_open_conds, new_num_open_conds, new_bindings,
		  action.precondition, step_id, Reason::DUMMY, true)) {
      return 0;
    }
    if (params->domain_constraints) {
      bindings = bindings->add(step_id, &action, *planning_graph);
      if (bindings == NULL) {
	return 0;
      }
    }
  }
  const Bindings* tmp_bindings = bindings->add(new_bindings, true);
  if (bindings != bindings_) {
    delete bindings;
  }
  return (tmp_bindings != NULL) ? 1 : 0;
}


/* Returns a plan with a link added from the given effect to the
   given open condition. */
const Plan* Plan::make_link(const Step& step, const Effect& effect,
			    const Literal& literal,
			    const OpenCondition& open_cond,
			    const LinkChain* new_links, const Reason& reason,
			    const SubstitutionList& unifier) const {
  /*
   * Add bindings needed to unify effect and goal.
   */
  const VariableList& effect_forall = effect.forall;
  BindingList new_bindings;
  for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
    const Substitution& subst = *si;
    if (!member(effect_forall.begin(), effect_forall.end(), &subst.var())) {
      new_bindings.push_back(new EqualityBinding(subst, reason));
    }
  }

  /*
   * If the effect is conditional, add condition as goal.
   */
  const OpenConditionChain* new_open_conds = open_conds()->remove(open_cond);
  size_t new_num_open_conds = num_open_conds() - 1;
  const Formula* cond_goal = NULL;
  if (!effect.condition.tautology()) {
    if (!effect_forall.empty()) {
      SubstitutionList forall_subst;
      for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
	const Substitution& subst = *si;
	if (member(effect_forall.begin(), effect_forall.end(), &subst.var())) {
	  forall_subst.push_back(subst);
	}
      }
      cond_goal = &effect.condition.substitution(forall_subst);
    } else {
      cond_goal = &effect.condition;
    }
    if (!add_goal(new_open_conds, new_num_open_conds, new_bindings,
		  *cond_goal, step.id(), reason)) {
      return NULL;
    }
  }

  /*
   * See if this is a new step.
   */
  const Bindings* bindings = bindings_;
  const StepChain* new_steps = steps();
  size_t new_num_steps = num_steps();
  if (step.id() > high_step_id_) {
    const Reason& step_reason = AddStepReason::make(*params, step.id());
    const Formula* precondition;
    if (typeid(*step.action()) == typeid(ActionSchema)) {
      precondition = &step.action()->precondition.instantiation(step.id());
    } else {
      precondition = & step.action()->precondition;
    }
    if (!add_goal(new_open_conds, new_num_open_conds, new_bindings,
		  *precondition, step.id(), step_reason)) {
      return NULL;
    }
    if (params->domain_constraints) {
      bindings = bindings->add(step.id(), step.action(), *planning_graph);
      if (bindings == NULL) {
	return NULL;
      }
    }
    if (params->transformational) {
      StepChain* tmp = new StepChain(step, new_steps);
      tmp->head.set_reason(reason);
      new_steps = tmp;
    } else {
      new_steps = new StepChain(step, new_steps);
    }
    new_num_steps++;
  } else if (params->transformational) {
    StepChain* tmp = new StepChain(step, new_steps);
    tmp->head.set_reason(reason);
    new_steps = tmp;
  }
  const Bindings* tmp_bindings = bindings->add(new_bindings);
  if (tmp_bindings != bindings && bindings != bindings_) {
    delete bindings;
  }
  if (tmp_bindings == NULL) {
    return NULL;
  }
  bindings = tmp_bindings;
  StepTime et = end_time(effect);
  StepTime gt = start_time(literal);
  const Orderings* new_orderings =
    orderings().refine(Ordering(step.id(), et,
				open_cond.step_id(), gt, reason),
		       step);
  if (new_orderings == NULL) {
    if (bindings != bindings_) {
      delete bindings;
    }
    return NULL;
  }

  /*
   * Find any threats to the newly established link.
   */
  const UnsafeChain* new_unsafes = unsafes();
  size_t new_num_unsafes = num_unsafes();
  link_threats(new_unsafes, new_num_unsafes, new_links->head, new_steps,
	       *new_orderings, *bindings);

  /*
   * If this is a new step, find links it threatens.
   */
  if (step.id() > high_step_id_) {
    step_threats(new_unsafes, new_num_unsafes, step, links(), *new_orderings,
		 *bindings);
  }

  /* Return the new plan. */
  return new Plan(new_steps, new_num_steps,
		  new_links, num_links() + 1,
		  *new_orderings, *bindings,
		  new_unsafes, new_num_unsafes,
		  new_open_conds, new_num_open_conds, this);
}


/* Adds plans to the given plan list with the given link removed and
   the resulting open condition relinked. */
void Plan::relink(PlanList& new_plans, const Link& link) const {
  pair<const Plan*, const OpenCondition*> p = unlink(link);
  if (verbosity > 2) {
    cout << "!!!!!!!!!!!!!!!!! Unlinked plan !!!!!!!!!!!!!!!!" << endl;
    cout << *p.first << endl;
  }
  p.first->handle_open_condition(new_plans, *p.second);
}


/*
 * A stack of causal links.
 */
typedef stack<const Link*> LinkStack;

/*
 * A stack of steps.
 */
typedef stack<const Step*> StepStack;


/* Returns the first occurance of the step with the given id, or NULL
   if no such step exists. */
static const Step* find_step(const StepChain* steps, size_t id) {
  if (steps == NULL) {
    return NULL;
  } else if (steps->head.id() == id) {
    return &steps->head;
  } else {
    return find_step(steps->tail, id);
  }
}


/* Checks if the given open condition is valid. */
static bool
valid_open_condition(const OpenCondition& open_cond,
		     const StepChain* steps, const LinkChain* links) {
  if (find_step(steps, open_cond.step_id()) == NULL) {
    return false;
  } else {
    const EstablishReason* er =
      dynamic_cast<const EstablishReason*>(&open_cond.reason());
    return (er != NULL) ? links->contains(er->link) : false;
  }
}


/* Returns a chain of unsafes with all unsafes in the given chain
   involving the given link removed. */
static const UnsafeChain*
remove_unsafes(const UnsafeChain* unsafes,
	       size_t& num_unsafes, const Link& link) {
  if (unsafes == NULL) {
    return NULL;
  } else {
    const UnsafeChain* tail = remove_unsafes(unsafes->tail, num_unsafes, link);
    if (unsafes->head.link() == link) {
      num_unsafes--;
      return tail;
    } else {
      return new UnsafeChain(unsafes->head, tail);
    }
  }
}


/* Returns a chain of unsafes with all unsafes in the given chain
   involving the given step removed. */
static const UnsafeChain*
remove_unsafes(const UnsafeChain* unsafes,
	       size_t& num_unsafes, const Step& step) {
  if (unsafes == NULL) {
    return NULL;
  } else {
    const UnsafeChain* tail = remove_unsafes(unsafes->tail, num_unsafes, step);
    if (unsafes->head.step_id() == step.id()) {
      num_unsafes--;
      return tail;
    } else {
      return new UnsafeChain(unsafes->head, tail);
    }
  }
}


/* Returns a chain of open conditions with all open condition in the
   given chain involving the given link removed. */
static const OpenConditionChain*
remove_open_conditions(const OpenConditionChain* open_conds,
		       size_t& num_open_conds, const Link& link) {
  if (open_conds == NULL) {
    return NULL;
  } else {
    const OpenConditionChain* tail =
      remove_open_conditions(open_conds->tail, num_open_conds, link);
    const OpenCondition& open_cond = open_conds->head;
    if (open_cond.reason().involves(link)) {
      num_open_conds--;
      return tail;
    } else {
      return new OpenConditionChain(open_cond, tail);
    }
  }
}


/* Returns a chain of open conditions with all open condition in the
   given chain involving the given step removed. */
static const OpenConditionChain*
remove_open_conditions(const OpenConditionChain* open_conds,
		       size_t& num_open_conds, const Step& step) {
  if (open_conds == NULL) {
    return NULL;
  } else {
    const OpenConditionChain* tail =
      remove_open_conditions(open_conds->tail, num_open_conds, step);
    const OpenCondition& open_cond = open_conds->head;
    if (open_cond.step_id() == step.id()) {
      num_open_conds--;
      return tail;
    } else {
      return new OpenConditionChain(open_cond, tail);
    }
  }
}


/* Returns a chain of steps with all steps in the given chain
   involving the given link removed, and adds exposed steps to the
   provided stack. */
static const StepChain*
remove_steps(StepStack& exposed_steps, const StepChain* steps,
	     const Link& link) {
  if (steps == NULL) {
    return NULL;
  } else {
    const StepChain* tail =
      remove_steps(exposed_steps, steps->tail, link);
    if (steps->head.reason().involves(link)) {
      if (find_step(steps->tail, steps->head.id()) == NULL) {
	exposed_steps.push(&steps->head);
      }
      return tail;
    } else {
      return new StepChain(steps->head, tail);
    }
  }
}


/* Returns a chain of links with all links in the given chain
   involving the given step removed, and adds exposed links to the
   provided stack. */
static const LinkChain*
remove_links(LinkStack& exposed_links, const LinkChain* links,
	     const Step& step) {
  if (links == NULL) {
    return NULL;
  } else {
    const LinkChain* tail =
      remove_links(exposed_links, links->tail, step);
    if (links->head.from_id() == step.id()
	|| links->head.to_id() == step.id()) {
      exposed_links.push(&links->head);
      return tail;
    } else {
      return new LinkChain(links->head, tail);
    }
  }
}


/* Returns a chain of ordering constraints with all ordering
   constraints in the given chain involving the given link removed. */
static const OrderingChain*
remove_orderings(const OrderingChain* orderings, const Link& link) {
  if (orderings == NULL) {
    return NULL;
  } else {
    const OrderingChain* tail = remove_orderings(orderings->tail, link);
    if (orderings->head.reason().involves(link)) {
      return tail;
    } else {
      return new OrderingChain(orderings->head, tail);
    }
  }
}


/* Returns a chain of ordering constraints with all ordering
   constraints in the given chain involving the given step removed. */
static const OrderingChain*
remove_orderings(const OrderingChain* orderings, const Step& step) {
  if (orderings == NULL) {
    return NULL;
  } else {
    const OrderingChain* tail = remove_orderings(orderings->tail, step);
    if (orderings->head.reason().involves(step)) {
      return tail;
    } else {
      return new OrderingChain(orderings->head, tail);
    }
  }
}


/* Returns a chain of binding constraints with all binding constraints
   in the given chain involving the given link removed. */
static const BindingChain*
remove_bindings(const BindingChain* bindings, const Link& link) {
  if (bindings == NULL) {
    return NULL;
  } else {
    const BindingChain* tail = remove_bindings(bindings->tail, link);
    if (bindings->head->reason().involves(link)) {
      return tail;
    } else {
      return new BindingChain(bindings->head, tail);
    }
  }
}


/* Returns a chain of binding constraints with all binding constraints
   in the given chain involving the given step removed. */
static const BindingChain*
remove_bindings(const BindingChain* bindings, const Step& step) {
  if (bindings == NULL) {
    return NULL;
  } else {
    const BindingChain* tail = remove_bindings(bindings->tail, step);
    if (bindings->head->reason().involves(step)) {
      return tail;
    } else {
      return new BindingChain(bindings->head, tail);
    }
  }
}


/* Returns a plan with the given link removed, and also returns the
   resulting open condition. */
pair<const Plan*, const OpenCondition*> Plan::unlink(const Link& link) const {
  const OpenCondition* link_cond = NULL;
  const StepChain* new_steps = steps();
  size_t new_num_steps = num_steps();
  const UnsafeChain* new_unsafes = unsafes();
  size_t new_num_unsafes = num_unsafes();
  const OpenConditionChain* new_open_conds = open_conds();
  size_t new_num_open_conds = num_open_conds();
  const BindingChain* equalities = bindings_->equalities();
  const BindingChain* inequalities = bindings_->inequalities();
  const OrderingChain* new_orderings = orderings().orderings();
  const LinkChain* new_links = links();
  size_t new_num_links = num_links();
  LinkStack exposed_links;
  StepStack exposed_steps;
  exposed_links.push(&link);
  while (!(exposed_links.empty() && exposed_steps.empty())) {
    if (!exposed_links.empty() && new_links != NULL) {
      const Link& l = *exposed_links.top();
      exposed_links.pop();
      /* remove exposed link */
      new_links = new_links->remove(l);
      new_num_links--;
      /* remove flaws involving link */
      new_unsafes = remove_unsafes(new_unsafes, new_num_unsafes, l);
      new_open_conds =
	remove_open_conditions(new_open_conds, new_num_open_conds, l);
      /* add open condition, if still valid */
      const OpenCondition open_cond =
	OpenCondition(l.to_id(), l.condition(), l.reason());
      if (l == link) {
	link_cond = &open_cond;
      }
      if (valid_open_condition(open_cond, new_steps, new_links)) {
	new_open_conds = new OpenConditionChain(open_cond, new_open_conds);
	new_num_open_conds++;
      }
      /* remove any reason involving link for steps */
      new_steps = remove_steps(exposed_steps, new_steps, l);
      /* remove any reason involving link for orderings */
      new_orderings = remove_orderings(new_orderings, l);
      /* remove any reason involving link for bindings */
      equalities = remove_bindings(equalities, l);
      inequalities = remove_bindings(inequalities, l);
      /* remove links to conditions that were threatened by this link */
      for (const LinkChain* lc = new_links; lc != NULL; lc = lc->tail) {
	if (lc->head.reason().involves(link)) {
	  exposed_links.push(&lc->head);
	}
      }
    } else if (!exposed_steps.empty() && new_steps != NULL) {
      const Step& s = *exposed_steps.top();
      exposed_steps.pop();
      /* decrease number of steps */
      new_num_steps--;
      /* remove links involving step */
      new_links = remove_links(exposed_links, new_links, s);
      /* remove flaws involving step */
      new_unsafes = remove_unsafes(new_unsafes, new_num_unsafes, s);
      new_open_conds =
	remove_open_conditions(new_open_conds, new_num_open_conds, s);
      /* remove any reason involving step for orderings */
      new_orderings = remove_orderings(new_orderings, s);
      /* remove any reason involving step for bindings */
      equalities = remove_bindings(equalities, s);
      inequalities = remove_bindings(inequalities, s);
    }
  }
  const Orderings* ords;
  if (domain->requirements.durative_actions) {
    ords = new TemporalOrderings(new_steps, new_orderings);
  } else {
    ords = new BinaryOrderings(new_steps, new_orderings);
  }
  const Plan* plan =
    new Plan(new_steps, new_num_steps, new_links, new_num_links,
	     *ords,
	     *(Bindings::make_bindings(new_steps, planning_graph,
				       equalities, inequalities)),
	     new_unsafes, new_num_unsafes, new_open_conds, new_num_open_conds,
	     this, INTERMEDIATE_PLAN);
  return pair<const Plan*, const OpenCondition*>(plan, link_cond);
}


/* Checks if this plan is a duplicate of a previous plan. */
bool Plan::duplicate() const {
#ifdef TRANSFORMATIONAL
  if (type_ == TRANSFORMED_PLAN) {
    if (verbosity > 2) {
      cout << "searching for duplicate..." << endl;
    }
    for (const Plan* p = parent_; p != NULL; p = p->parent_) {
      if (equivalent(*p)) {
	if (verbosity > 2) {
	  cout << "matching steps!" << endl;
	}
	return true;
      }
    }
  }
#endif
  return false;
}


/* Maps step ids to formulas. */
struct FormulaMap : public hash_map<size_t, const Formula*> {
};


/* Checks if the steps match for two plans. */
static bool matching_plans(hash_map<size_t, size_t>& step_map,
			   const FormulaMap& steps1, const FormulaMap& steps2,
			   FormulaMap::const_iterator s1) {
  if (s1 == steps1.end()) {
    // try to match orderings, links, and bindings given step_map
    return true; // true if matches
  }
  for (FormulaMap::const_iterator s2 = steps2.begin();
       s2 != steps2.end(); s2++) {
    if (step_map.find((*s2).first) == step_map.end()
	&& (*s1).second->equivalent(*(*s2).second)) {
      step_map[(*s2).first] = (*s1).first;
      if (matching_plans(step_map, steps1, steps2, ++s1)) {
	return true;
      }
      step_map.erase((*s2).first);
    }
  }
  return false;
}


/* Checks if this plan is equivalent to the given plan. */
bool Plan::equivalent(const Plan& p) const {
  if (num_steps() == p.num_steps() && num_open_conds() == p.num_open_conds()
      && num_links() == p.num_links()) {
    if (verbosity > 2) {
      cout << "matching number of steps, etc..." << endl;
    }
    // instantiate steps in this and p
    FormulaMap steps1;
    for (const StepChain* sc = steps(); sc != NULL; sc = sc->tail) {
      size_t id = sc->head.id();
      if (sc->head.action() != NULL && steps1.find(id) == steps1.end()) {
	if (params->ground_actions) {
	  steps1[id] = sc->head.step_formula();
	} else {
	  steps1[id] = &sc->head.step_formula()->instantiation(*bindings_);
	}
      }
    }
    FormulaMap steps2;
    for (const StepChain* sc = p.steps(); sc != NULL; sc = sc->tail) {
      size_t id = sc->head.id();
      if (sc->head.action() != NULL && steps2.find(id) == steps2.end()) {
	if (params->ground_actions) {
	  steps2[id] = sc->head.step_formula();
	} else {
	  steps2[id] = &sc->head.step_formula()->instantiation(*p.bindings_);
	}
      }
    }
    // try to find mapping between steps so that bindings and orderings match
    hash_map<size_t, size_t> step_map;
    return matching_plans(step_map, steps1, steps2, steps1.begin());
  }
  return false;
}


/* Less than operator for plans. */
bool operator<(const Plan& p1, const Plan& p2) {
  float diff = p1.primary_rank() - p2.primary_rank();
  for (size_t i = 1; i < p1.rank_.size() && diff == 0.0; i++) {
    diff = p1.rank_[i] - p2.rank_[i];
  }
  return diff > 0.0;
}


struct StepSorter {
  StepSorter(hash_map<size_t, float>& dist)
    : dist(dist) {}

  bool operator()(const Step* s1, const Step* s2) const {
    return dist[s1->id()] > dist[s2->id()];
  }

  hash_map<size_t, float>& dist;
};


/* Output operator for plans. */
ostream& operator<<(ostream& os, const Plan& p) {
  const Step* init = NULL;
  const Step* goal = NULL;
  const Bindings* bindings = p.bindings();
  vector<const Step*> ordered_steps;
  hash_set<size_t> seen_steps;
  for (const StepChain* sc = p.steps(); sc != NULL; sc = sc->tail) {
    const Step& step = sc->head;
    if (step.id() == 0) {
      init = &step;
    } else if (step.id() == Plan::GOAL_ID) {
      goal = &step;
    } else if (seen_steps.find(step.id()) == seen_steps.end()) {
      seen_steps.insert(step.id());
      ordered_steps.push_back(&step);
    }
  }
  hash_map<size_t, float> start_dist;
  hash_map<size_t, float> end_dist;
  float max_dist = p.orderings().goal_distances(start_dist, end_dist) + 1.0f;
  sort(ordered_steps.begin(), ordered_steps.end(), StepSorter(start_dist));
  if (verbosity < 2) {
    if (verbosity > 0) {
      os << "Number of steps: " << p.num_steps();
    }
    for (vector<const Step*>::const_iterator si = ordered_steps.begin();
	 si != ordered_steps.end(); si++) {
      if (verbosity > 0 || si != ordered_steps.begin()) {
	os << endl;
      }
      const Step& s = **si;
      os << (max_dist - start_dist[s.id()]) << ':';
      if (bindings != NULL) {
	os << s.step_formula()->instantiation(*bindings);
      } else {
	os << *s.step_formula();
      }
      if (s.action()->durative) {
	os << '[' << (start_dist[s.id()] - end_dist[s.id()]) << ']';
      }
    }
  } else {
    os << "Initial  :";
    const EffectList& effects = init->effects();
    for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
      const AtomList& atoms = (*ei)->add_list;
      for (AtomListIter ai = atoms.begin(); ai != atoms.end(); ai++) {
	os << ' ' << **ai;
      }
    }
    ordered_steps.push_back(goal);
    for (vector<const Step*>::const_iterator si = ordered_steps.begin();
	 si != ordered_steps.end(); si++) {
      const Step& step = **si;
      if (step.id() == Plan::GOAL_ID) {
	os << endl << endl << "Goal     : ";
      } else {
	os << endl << endl << "Step " << step.id();
	if (step.id() < 100) {
	  if (step.id() < 10) {
	    os << ' ';
	  }
	  os << ' ';
	}
	os << " : ";
	if (bindings != NULL) {
	  os << step.step_formula()->instantiation(*bindings);
	} else {
	  os << *step.step_formula();
	}
      }
      for (const LinkChain* lc = p.links(); lc != NULL; lc = lc->tail) {
	const Link& link = lc->head;
	if (link.to_id() == step.id()) {
	  os << endl << "          " << link.from_id();
	  if (link.from_id() < 100) {
	    if (link.from_id() < 10) {
	      os << ' ';
	    }
	    os << ' ';
	  }
	  os << " -> ";
	  if (bindings != NULL) {
	    os << link.condition().instantiation(*bindings);
	  } else {
	    os << link.condition();
	  }
	  for (const UnsafeChain* uc = p.unsafes();
	       uc != NULL; uc = uc->tail) {
	    const Unsafe& unsafe = uc->head;
	    if (unsafe.link() == link) {
	      os << " <" << unsafe.step_id() << ">";
	    }
	  }
	}
      }
      for (const OpenConditionChain* occ = p.open_conds();
	   occ != NULL; occ = occ->tail) {
	const OpenCondition& open_cond = occ->head;
	if (open_cond.step_id() == step.id()) {
	  os << endl << "           ?? -> ";
	  if (bindings != NULL) {
	    os << open_cond.condition().instantiation(*bindings);
	  } else {
	    os << open_cond.condition();
	  }
	}
      }
    }
    os << endl << "orderings = " << p.orderings();
    if (bindings != NULL) {
      os << endl << "bindings = " << *bindings;
    }
  }
  return os;
}
