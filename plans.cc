/*
 * $Id: plans.cc,v 1.38 2001-12-29 19:08:42 lorens Exp $
 */
#include <queue>
#include <hash_set>
#include <algorithm>
#include <typeinfo>
#include <climits>
#include <cassert>
#include <sys/time.h>
#include "plans.h"
#include "orderings.h"
#include "flaws.h"
#include "reasons.h"
#include "problems.h"
#include "formulas.h"
#include "parameters.h"
#include "debug.h"


//#define INEQUALITY_AS_BRANCHING

/* Planning parameters. */
static Parameters params;
/* Domain of problem currently being solved. */
static const Domain* domain = NULL;
/* Maps predicates to actions. */
static hash_map<string, ActionList> achieves_pred;
/* Maps negated predicates to actions. */
static hash_map<string, ActionList> achieves_neg_pred;
/* Planning graph. */
static const PlanningGraph* planning_graph;
/* Whether last flaw was a static predicate. */
static bool static_pred_flaw = false;


/*
 * A plan queue.
 */
struct PlanQueue : public priority_queue<const Plan*, Vector<const Plan*>,
		   less<const LessThanComparable*> > {
};


/* Constructs a causal link. */
Link::Link(size_t from_id, const LiteralOpenCondition& open_cond)
  : from_id(from_id), to_id(open_cond.step_id),
    condition(open_cond.literal), reason(open_cond.reason) {
}


/* Prints this causal link. */
void Link::print(ostream& os) const {
  os << "#<LINK " << from_id << ' ' << condition << ' ' << to_id << '>';
}


/* Constructs a step. */
Step::Step(size_t id, const Formula& precondition, const EffectList& effects,
	   const Reason& reason)
  : id(id), action(NULL), precondition(precondition.instantiation(id)),
    effects(effects.instantiation(id)), reason(reason), formula(NULL) {
}


/* Constructs a step instantiated from an action. */
Step::Step(size_t id, const Action& action, const Reason& reason)
  : id(id), action(&action),
    precondition((typeid(action) == typeid(ActionSchema))
		 ? action.precondition.instantiation(id)
		 : action.precondition),
    effects((typeid(action) == typeid(ActionSchema))
	    ? action.effects.instantiation(id) : action.effects),
    reason(reason), formula(NULL) {
}


/* Returns a formula representing this step. */
const Atom* Step::step_formula() const {
  if (action == NULL) {
    return NULL;
  } else if (formula == NULL) {
    formula = &((typeid(*action) == typeid(ActionSchema))
		? action->action_formula().instantiation(id)
		: action->action_formula());
  }
  return formula;
}


/* Id of goal step. */
const size_t Plan::GOAL_ID = UINT_MAX;


/* Adds atomic goal to chain of open conditions, and returns true if
  and only if the goal is consistent. */
static bool add_goal(const OpenConditionChain*& open_conds,
		     size_t& num_open_conds, BindingList& new_bindings,
		     const Formula& goal, size_t step_id,
		     const Reason& reason) {
  if (goal.tautology()) {
    return true;
  } else if (goal.contradiction()) {
    return false;
  }
  deque<const Formula*, container_alloc> goals(1, &goal);
  while (!goals.empty()) {
    const Formula* goal = goals.back();
    goals.pop_back();
    const OpenCondition* open_cond = NULL;
    const Conjunction* conj = dynamic_cast<const Conjunction*>(goal);
    if (conj != NULL) {
      const FormulaList& gs = conj->conjuncts;
      for (FormulaListIter fi = gs.begin(); fi != gs.end(); fi++) {
#ifndef REVERSE_OPEN_CONDITIONS
	goals.push_back(*fi);
#else
	goals.push_front(*fi);
#endif
      }
    } else {
      const Literal* literal = dynamic_cast<const Literal*>(goal);
      if (literal != NULL) {
	open_cond = new LiteralOpenCondition(*literal, step_id, reason);
      } else {
	const Disjunction* disj = dynamic_cast<const Disjunction*>(goal);
	if (disj != NULL) {
	  open_cond = new DisjunctiveOpenCondition(*disj, step_id, reason);
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
#ifndef BRANCH_ON_INEQUALITY
	      new_bindings.push_back(b);
#else
	      const Inequality* neq = dynamic_cast<const Inequality*>(bl);
	      if (neq != NULL && typeid(bl->term1) == typeid(bl->term2)) {
		/* Both terms are variables, so handle specially. */
		open_cond = new InequalityOpenCondition(*neq, step_id, reason);
	      } else {
		new_bindings.push_back(b);
	      }
#endif
	    }
	  } else if (dynamic_cast<const QuantifiedFormula*>(goal) != NULL) {
	    throw Unimplemented("adding quantified goal");
	  } else {
	    throw Unimplemented("unknown kind of goal");
	  }
	}
      }
    }
    if (open_cond != NULL) {
      open_conds = new OpenConditionChain(open_cond, open_conds);
      num_open_conds++;
    }
  }
  return true;
}


/* Returns the initial plan representing the given problem, or NULL
   if goals of problem are inconsistent. */
const Plan* Plan::make_initial_plan(const Problem& problem) {
  /* Reason for initial steps in plan. */
  const Reason& init_reason = InitReason::make(params);

  /*
   * Create step representing initial conditions of problem.
   */
  EffectList& init = *(new EffectList(&problem.init));
  /* Initial step. */
  const Step& init_step = *(new Step(0, Formula::TRUE, init, init_reason));

  /*
   * Create step representing goal of problem.
   */
  /* Goal step. */
  const Step& goal_step =
    *(new Step(Plan::GOAL_ID, problem.goal, *(new EffectList()), init_reason));
  /* Reason for open conditions of goal. */
  const Reason& goal_reason = AddStepReason::make(params, goal_step.id);
  /* Chain of open conditions. */
  const OpenConditionChain* open_conds = NULL;
  /* Number of open conditions. */
  size_t num_open_conds = 0;
  /* Bindings introduced by goal. */
  BindingList new_bindings;
  /* Add goals as open conditions. */
  if (!add_goal(open_conds, num_open_conds, new_bindings,
		goal_step.precondition, goal_step.id, goal_reason)) {
    /* Goals are inconsistent. */
    return NULL;
  }
  /* Make chain of steps. */
  const StepChain* steps = new StepChain(&goal_step,
					 new StepChain(&init_step, NULL));
  /* Variable bindings. */
  const Bindings* bindings = &Bindings::make_bindings(steps, planning_graph);
  if (bindings == NULL) {
    /* Bindings are inconsistent. */
    return NULL;
  }
  /* Return initial plan. */
  return new Plan(steps, 2, 0, NULL, 0, NULL, 0, open_conds, num_open_conds,
		  *bindings, *(new Orderings()), NULL);
}


/* Returns plan for given problem. */
const Plan* Plan::plan(const Problem& problem, const Parameters& p) {
  /* Set planning parameters. */
  params = p;
  /* Set current domain. */
  domain = &problem.domain;

  achieves_pred.clear();
  achieves_neg_pred.clear();
  if (params.ground_actions || params.domain_constraints
      || params.heuristic.needs_planning_graph()
      || params.flaw_order.needs_planning_graph()) {
    planning_graph = new PlanningGraph(problem);
  }
  if (!params.ground_actions) {
    for (ActionSchemaMapIter ai = domain->actions.begin();
	 ai != domain->actions.end(); ai++) {
      const ActionSchema* action = (*ai).second;
      if (params.domain_constraints) {
#ifdef INEQUALITY_AS_BRANCHING
	action = &action->strip_static(*domain);
#else
	action = &action->strip_equality();
#endif
      }
      hash_set<string> preds;
      hash_set<string> neg_preds;
      action->achievable_predicates(preds, neg_preds);
      for (hash_set<string>::const_iterator j = preds.begin();
	   j != preds.end(); j++) {
	achieves_pred[*j].push_back(action);
      }
      for (hash_set<string>::const_iterator j = neg_preds.begin();
	   j != neg_preds.end(); j++) {
	achieves_neg_pred[*j].push_back(action);
      }
    }
  }

  /* Number of visited plan. */
  size_t num_visited_plans = 0;
  /* Number of generated plans. */
  size_t num_generated_plans = 0;
  /* Number of static preconditions encountered. */
  size_t num_static = 0;

  /* Queue of pending plans. */
  PlanQueue plans;
  /* Construct the initial plan. */
  const Plan* current_plan = make_initial_plan(problem);
  current_plan->id = 0;
  num_generated_plans++;
  size_t last_dot = 0;
  size_t last_hash = 0;
  /*
   * Search for complete plan.
   */
  while (current_plan != NULL && !current_plan->complete()) {
    if (num_generated_plans - num_static >= params.search_limit) {
      /* Search limit exceeded. */
      break;
    }
    struct itimerval timer;
    getitimer(ITIMER_PROF, &timer);
    double t = 1000000.9
      - (timer.it_value.tv_sec + timer.it_value.tv_usec*1e-6);
    if (t >= 60.0*params.time_limit) {
      /* Time limit exceeded. */
      break;
    }
    /*
     * This is a new plan.
     */
    num_visited_plans++;
    if (verbosity == 1) {
      if (num_generated_plans - num_static - last_dot >= 1000) {
	cout << '.';
	last_dot = 1000*((num_generated_plans - num_static) / 1000);
      }
      if (t - 60.0*last_hash >= 60.0) {
	cout << '#';
	last_hash = size_t(t/60.0);
      }
    }
    if (verbosity > 1) {
      cout << endl << (num_visited_plans - num_static) << ": "
	   << "!!!!CURRENT PLAN (id " << current_plan->id << ")"
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
    for (PlanList::const_iterator i = refinements.begin();
	 i != refinements.end(); i++) {
      (*i)->id = num_generated_plans;
      if ((*i)->primary_rank() < INT_MAX) {
	added = true;
	plans.push(*i);
	num_generated_plans++;
	if (verbosity > 2) {
	  cout << endl << "####CHILD (id " << (*i)->id << ")"
	       << " with rank (" << (*i)->primary_rank();
	  for (size_t ri = 1; ri < (*i)->rank_.size(); ri++) {
	    cout << ',' << (*i)->rank_[ri];
	  }
	  cout << "):" << endl
	       << **i << endl;
	}
      }
    }
    if (added && static_pred_flaw) {
      num_static++;
    }
    /* Process next plan. */
    do {
      if (plans.empty()) {
	/* Problem lacks solution. */
	current_plan = NULL;
      } else {
	current_plan = plans.top();
	plans.pop();
      }
    } while (current_plan != NULL && current_plan->duplicate());
#ifdef HILLCLIMB_TRANSFORMATIONAL
    if (params.transformational) {
      plans = PlanQueue();
    }
#endif
  }
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
  /* Return last plan, or NULL if problem does not have a solution. */
  return current_plan;
}


/* Checks if this plan is complete. */
bool Plan::complete() const {
  return unsafes_ == NULL && open_conds_ == NULL;
}


/* Returns the primary rank of this plan, where a lower rank
   signifies a better plan. */
double Plan::primary_rank() const {
  if (rank_.empty()) {
    params.heuristic.plan_rank(rank_, *this, params.weight, planning_graph);
  }
  return rank_[0];
}


const Flaw& Plan::get_flaw() const {
  const Flaw& flaw =
    params.flaw_order.select(unsafes_, open_conds_, planning_graph, *domain,
			     (params.ground_actions ? NULL : &bindings_));
  const OpenCondition* open_cond = dynamic_cast<const OpenCondition*>(&flaw);
  static_pred_flaw = (open_cond != NULL && open_cond->is_static(*domain));
  return flaw;
}


void Plan::refinements(PlanList& new_plans) const {
  const Flaw& flaw = get_flaw();
  if (verbosity > 1) {
    cout << endl << "handle " << flaw << endl;
  }
  const Unsafe* unsafe = dynamic_cast<const Unsafe*>(&flaw);
  if (unsafe != NULL) {
    handle_unsafe(new_plans, *unsafe);
  } else {
    const OpenCondition* open_cond = dynamic_cast<const OpenCondition*>(&flaw);
    if (open_cond != NULL) {
      handle_open_condition(new_plans, *open_cond);
    }
  }
}

void Plan::handle_unsafe(PlanList& new_plans, const Unsafe& unsafe) const {
  size_t num_prev_plans = new_plans.size();
  if (orderings_.possibly_before(unsafe.link.from_id, unsafe.step_id) &&
      orderings_.possibly_after(unsafe.link.to_id, unsafe.step_id) &&
      bindings_.affects(unsafe.effect_add, unsafe.link.condition)) {
    demote(new_plans, unsafe);
    promote(new_plans, unsafe);
    separate(new_plans, unsafe);
    if (num_prev_plans == new_plans.size()) {
      if (params.transformational) {
	if (verbosity > 2) {
	  cout << endl << "++++DEAD END:" << endl << *this << endl;
	}
#if 1
	relink(new_plans, unsafe.link);
	if (verbosity > 2) {
	  for (size_t i = num_prev_plans; i < new_plans.size(); i++) {
	    cout << "^^^^Transformed plan" << endl << *new_plans[i] << endl;
	    new_plans[i]->duplicate();
	  }
	}
#else
	for (const LinkChain* l = links_; l != NULL; l = l->tail) {
	  relink(new_plans, *l->head);
	}
#endif
      }
    }
  } else {
    /* bogus flaw */
    const Plan* p =
      new Plan(steps_, num_steps_, high_step_id_, links_, num_links_,
	       unsafes_->remove(&unsafe), num_unsafes_ - 1, open_conds_,
	       num_open_conds_, bindings_, orderings_, this);
    new_plans.push_back(p);
  }
}

void Plan::separate(PlanList& new_plans, const Unsafe& unsafe) const {
  SubstitutionList unifier;
  bindings_.affects(unifier, unsafe.effect_add, unsafe.link.condition);
  const VariableList& effect_forall = unsafe.effect.forall;
  const Formula* goal = &Formula::FALSE;
  for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
    const Substitution& s = **si;
    if (!member(effect_forall.begin(), effect_forall.end(), &s.var)) {
      const Inequality& neq = *new Inequality(s.var, s.term);
      if (bindings_.consistent_with(neq)) {
	goal = &(*goal || neq);
      }
    }
  }
  const Formula& effect_cond = unsafe.effect.condition;
  if (!effect_cond.tautology()) {
    const Formula* cond_goal;
    if (!effect_forall.empty()) {
      SubstitutionList forall_subst;
      for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
	const Substitution& s = **si;
	if (member(effect_forall.begin(), effect_forall.end(), &s.var)) {
	  forall_subst.push_back(&s);
	}
      }
      cond_goal = &effect_cond.substitution(forall_subst);
    } else {
      cond_goal = &effect_cond;
    }
    goal = &(*goal || *cond_goal);
  }
  if (!goal->contradiction()) {
    const OpenConditionChain* open_conds = open_conds_;
    size_t num_open_conds = num_open_conds_;
    BindingList new_bindings;
    const Reason& protect_reason =
      ProtectReason::make(params, unsafe.link, unsafe.step_id);
    if (add_goal(open_conds, num_open_conds, new_bindings, *goal,
		 unsafe.step_id, protect_reason)) {
      const Bindings* bindings = bindings_.add(new_bindings);
      if (bindings != NULL) {
	const UnsafeChain* unsafes = unsafes_->remove(&unsafe);
	size_t num_unsafes = num_unsafes_ - 1;
	const Plan* p =
	  new Plan(steps_, num_steps_, high_step_id_, links_, num_links_,
		   unsafes, num_unsafes, open_conds, num_open_conds, *bindings,
		   orderings_, this);
	new_plans.push_back(p);
      }
    }
  }
}

void Plan::demote(PlanList& new_plans, const Unsafe& unsafe) const {
  size_t before_id = unsafe.step_id;
  size_t after_id = unsafe.link.from_id;
  if (orderings_.possibly_before(before_id, after_id)) {
    const Reason& protect_reason =
      ProtectReason::make(params, unsafe.link, unsafe.step_id);
    new_ordering(new_plans,
		 *(new Ordering(before_id, after_id, protect_reason)),
		 unsafe);
  }
}

void Plan::promote(PlanList& new_plans, const Unsafe& unsafe) const {
  size_t before_id = unsafe.link.to_id;
  size_t after_id = unsafe.step_id;
  if (orderings_.possibly_before(before_id, after_id)) {
    const Reason& protect_reason =
      ProtectReason::make(params, unsafe.link, unsafe.step_id);
    new_ordering(new_plans,
		 *(new Ordering(before_id, after_id, protect_reason)),
		 unsafe);
  }
}

void Plan::new_ordering(PlanList& new_plans, const Ordering& ordering,
			const Unsafe& unsafe) const {
  const Orderings& orderings = orderings_.refine(ordering);
  const UnsafeChain* unsafes = unsafes_->remove(&unsafe);
  size_t num_unsafes = num_unsafes_ - 1;
  const Plan* new_plan =
    new Plan(steps_, num_steps_, high_step_id_, links_, num_links_,
	     unsafes, num_unsafes, open_conds_, num_open_conds_, bindings_,
	     orderings, this);
  new_plans.push_back(new_plan);
}

static const Step* find_step(const StepChain* steps, size_t id) {
  if (steps == NULL) {
    return NULL;
  } else if (steps->head->id == id) {
    return steps->head;
  } else {
    return find_step(steps->tail, id);
  }
}

static bool
valid_open_condition(const OpenCondition& open_cond,
		     const StepChain* steps, const LinkChain* links) {
  if (find_step(steps, open_cond.step_id) == NULL) {
    return false;
  } else {
    const EstablishReason* er =
      dynamic_cast<const EstablishReason*>(&open_cond.reason);
    return (er != NULL) ? links->contains(&er->link) : false;
  }
}

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
typedef stack<const Link*, deque<const Link*, container_alloc> > LinkStack;

/*
 * A stack of steps.
 */
typedef stack<const Step*, deque<const Step*, container_alloc> > StepStack;

/* Returns a chain of unsafes with all unsafes in the given chain
   involving the given link removed. */
static const UnsafeChain*
remove_unsafes(const UnsafeChain* unsafes,
	       size_t& num_unsafes, const Link& link) {
  if (unsafes == NULL) {
    return NULL;
  } else {
    const UnsafeChain* tail = remove_unsafes(unsafes->tail, num_unsafes, link);
    if (&unsafes->head->link == &link) {
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
    if (unsafes->head->step_id == step.id) {
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
    const OpenCondition& open_cond = *open_conds->head;
    if (open_cond.reason.involves(link)) {
      num_open_conds--;
      return tail;
    } else {
      return new OpenConditionChain(&open_cond, tail);
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
    const OpenCondition& open_cond = *open_conds->head;
    if (open_cond.step_id == step.id) {
      num_open_conds--;
      return tail;
    } else {
      return new OpenConditionChain(&open_cond, tail);
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
    if (steps->head->reason.involves(link)) {
      if (find_step(steps->tail, steps->head->id) == NULL) {
	exposed_steps.push(steps->head);
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
    if (links->head->from_id == step.id || links->head->to_id == step.id) {
      exposed_links.push(links->head);
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
    if (orderings->head->reason.involves(link)) {
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
    if (orderings->head->reason.involves(step)) {
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
    if (bindings->head->reason.involves(link)) {
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
    if (bindings->head->reason.involves(step)) {
      return tail;
    } else {
      return new BindingChain(bindings->head, tail);
    }
  }
}

pair<const Plan*, const OpenCondition*> Plan::unlink(const Link& link) const {
  const OpenCondition* link_cond = NULL;
  const StepChain* steps = steps_;
  size_t num_steps = num_steps_;
  const UnsafeChain* unsafes = unsafes_;
  size_t num_unsafes = num_unsafes_;
  const OpenConditionChain* open_conds = open_conds_;
  size_t num_open_conds = num_open_conds_;
  const BindingChain* equalities = bindings_.equalities;
  const BindingChain* inequalities = bindings_.inequalities;
  const OrderingChain* orderings = orderings_.orderings();
  const LinkChain* links = links_;
  size_t num_links = num_links_;
  LinkStack exposed_links;
  StepStack exposed_steps;
  exposed_links.push(&link);
  while (!(exposed_links.empty() && exposed_steps.empty())) {
    if (!exposed_links.empty() && links != NULL) {
      const Link& l = *exposed_links.top();
      exposed_links.pop();
      /* remove exposed link */
      links = links->remove(&l);
      num_links--;
      /* remove flaws involving link */
      unsafes = remove_unsafes(unsafes, num_unsafes, l);
      open_conds = remove_open_conditions(open_conds, num_open_conds, l);
      /* add open condition, if still valid */
      const OpenCondition* open_cond =
	new LiteralOpenCondition(l.condition, l.to_id, l.reason);
      if (&l == &link) {
	link_cond = open_cond;
      }
      if (valid_open_condition(*open_cond, steps, links)) {
	open_conds = new OpenConditionChain(open_cond, open_conds);
	num_open_conds++;
      }
      /* remove any reason involving link for steps */
      steps = remove_steps(exposed_steps, steps, l);
      /* remove any reason involving link for orderings */
      orderings = remove_orderings(orderings, l);
      /* remove any reason involving link for bindings */
      equalities = remove_bindings(equalities, l);
      inequalities = remove_bindings(inequalities, l);
      /* remove links to conditions that were threatened by this link */
      for (const LinkChain* lc = links; lc != NULL; lc = lc->tail) {
	if (lc->head->reason.involves(link)) {
	  exposed_links.push(lc->head);
	}
      }
    } else if (!exposed_steps.empty() && steps != NULL) {
      const Step& s = *exposed_steps.top();
      exposed_steps.pop();
      /* decrease number of steps */
      num_steps--;
      /* remove links involving step */
      links = remove_links(exposed_links, links, s);
      /* remove flaws involving step */
      unsafes = remove_unsafes(unsafes, num_unsafes, s);
      open_conds = remove_open_conditions(open_conds, num_open_conds, s);
      /* remove any reason involving step for orderings */
      orderings = remove_orderings(orderings, s);
      /* remove any reason involving step for bindings */
      equalities = remove_bindings(equalities, s);
      inequalities = remove_bindings(inequalities, s);
    }
  }
  assert(link_cond != NULL);
  const Plan* plan =
    new Plan(steps, num_steps, high_step_id_, links, num_links,
	     unsafes, num_unsafes, open_conds, num_open_conds,
	     *(Bindings::make_bindings(steps, planning_graph,
				       equalities, inequalities)),
	     *(new Orderings(steps, orderings)), this, INTERMEDIATE_PLAN);
  return pair<const Plan*, const OpenCondition*>(plan, link_cond);
}


void Plan::handle_open_condition(PlanList& new_plans,
				 const OpenCondition& open_cond) const {
  const LiteralOpenCondition* loc =
    dynamic_cast<const LiteralOpenCondition*>(&open_cond);
  if (loc != NULL) {
    add_step(new_plans, *loc);
    reuse_step(new_plans, *loc);
  } else {
    const DisjunctiveOpenCondition* disjoc =
      dynamic_cast<const DisjunctiveOpenCondition*>(&open_cond);
    if (disjoc != NULL) {
      handle_disjunction(new_plans, *disjoc);
    } else {
      const InequalityOpenCondition* neqoc =
	dynamic_cast<const InequalityOpenCondition*>(&open_cond);
      if (neqoc != NULL) {
	handle_inequality(new_plans, *neqoc);
      } else {
	throw Unimplemented("unknown kind of open condition");
      }
    }
  }
}

void
Plan::handle_disjunction(PlanList& new_plans,
			 const DisjunctiveOpenCondition& open_cond) const {
  const Disjunction& disjunction = open_cond.disjunction;
  const FormulaList& disjuncts = disjunction.disjuncts;
  for (FormulaListIter fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    BindingList new_bindings;
    const OpenConditionChain* open_conds = open_conds_->remove(&open_cond);
    size_t num_open_conds = num_open_conds_ - 1;
    if (add_goal(open_conds, num_open_conds, new_bindings, **fi,
		 open_cond.step_id, open_cond.reason)) {
      const Bindings* bindings = bindings_.add(new_bindings);
      if (bindings != NULL) {
	const Plan* new_plan =
	  new Plan(steps_, num_steps_, high_step_id_, links_, num_links_,
		   unsafes_, num_unsafes_, open_conds, num_open_conds,
		   *bindings, orderings_, this);
	new_plans.push_back(new_plan);
      }
    }
  }
}


/* Handles inequality constraint between two variables. */
void Plan::handle_inequality(PlanList& new_plans,
			     const InequalityOpenCondition& open_cond) const {
  const Inequality& neq = open_cond.neq;
  const StepVar& v1 = dynamic_cast<const StepVar&>(neq.term1);
  const StepVar& v2 = dynamic_cast<const StepVar&>(neq.term2);
  const NameSet* d1 = bindings_.domain(v1);
  const NameSet* d2 = bindings_.domain(v2);
  if (d1 == NULL || d2 == NULL) {
    return;
  }
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
  const OpenConditionChain* open_conds = open_conds_->remove(&open_cond);
  size_t num_open_conds = num_open_conds_ - 1;
  for (NameSetIter ni = var_domain->begin(); ni != var_domain->end(); ni++) {
    const Name& name = **ni;
    BindingList new_bindings;
    new_bindings.push_back(new EqualityBinding(*var1, name, open_cond.reason));
    new_bindings.push_back(new InequalityBinding(*var2, name,
						 open_cond.reason));
    const Bindings* bindings = bindings_.add(new_bindings);
    if (bindings != NULL) {
      new_plans.push_back(new Plan(steps_, num_steps_, high_step_id_, links_,
				   num_links_, unsafes_, num_unsafes_,
				   open_conds, num_open_conds, *bindings,
				   orderings_, this));
    }
  }
}


void Plan::add_step(PlanList& new_plans,
		    const LiteralOpenCondition& open_cond) const {
  ActionList actions;
  if (params.ground_actions) {
    planning_graph->achieves_formula(actions, open_cond.literal);
  } else if (typeid(open_cond.literal) == typeid(Atom)) {
    hash_map<string, ActionList>::const_iterator sali =
      achieves_pred.find(open_cond.literal.predicate());
    if (sali != achieves_pred.end()) {
      copy((*sali).second.begin(), (*sali).second.end(),
	   back_inserter(actions));
    }
  } else {
    hash_map<string, ActionList>::const_iterator sali =
      achieves_neg_pred.find(open_cond.literal.predicate());
    if (sali != achieves_neg_pred.end()) {
      copy((*sali).second.begin(), (*sali).second.end(),
	   back_inserter(actions));
    }
  }
  size_t step_id = high_step_id_ + 1;
  if (!actions.empty()) {
    const Link& link = *(new Link(step_id, open_cond));
    const Reason& establish_reason = EstablishReason::make(params, link);
    for (ActionListIter ai = actions.begin(); ai != actions.end(); ai++) {
      const Step& step = *(new Step(step_id, **ai, establish_reason));
      new_link(new_plans, step, open_cond, link, establish_reason);
    }
  }
}

void Plan::reuse_step(PlanList& new_plans,
		      const LiteralOpenCondition& open_cond) const {
  hash_set<size_t> seen_steps;
  for (const StepChain* steps = steps_; steps != NULL; steps = steps->tail) {
    const Step& step = *steps->head;
    if (seen_steps.find(step.id) == seen_steps.end() &&
	orderings_.possibly_before(step.id, open_cond.step_id)) {
      seen_steps.insert(step.id);
      const Link& link = *(new Link(step.id, open_cond));
      const Reason& establish_reason = EstablishReason::make(params, link);
      new_link(new_plans, step, open_cond, link, establish_reason);
    }
  }
}

bool Plan::new_link(PlanList& new_plans, const Step& step,
		    const LiteralOpenCondition& open_cond, const Link& link,
		    const Reason& reason) const {
  size_t prev_num_plans = new_plans.size();
  if (step.id == 0 && typeid(open_cond.literal) == typeid(Negation)) {
    new_cw_link(new_plans, step, open_cond, link, reason);
  }
  const Literal& goal = open_cond.literal;
  const EffectList& effs = step.effects;
  for (EffectListIter ei = effs.begin(); ei != effs.end(); ei++) {
    const Effect& effect = **ei;
    if (typeid(goal) == typeid(Atom)) {
      const AtomList& adds = effect.add_list;
      for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
	SubstitutionList mgu;
	if (bindings_.unify(mgu, goal, **gi)) {
	  const Plan* new_plan =
	    make_link(step, effect, open_cond, link, reason, mgu);
	  if (new_plan != NULL) {
	    new_plans.push_back(new_plan);
	  }
	}
      }
    } else {
      const NegationList& dels = effect.del_list;
      for (NegationListIter gi = dels.begin(); gi != dels.end(); gi++) {
	SubstitutionList mgu;
	if (bindings_.unify(mgu, goal, **gi)) {
	  const Plan* new_plan =
	    make_link(step, effect, open_cond, link, reason, mgu);
	  if (new_plan != NULL) {
	    new_plans.push_back(new_plan);
	  }
	}
      }
    }
  }
  return new_plans.size() > prev_num_plans;
}

void Plan::new_cw_link(PlanList& new_plans, const Step& step,
		       const LiteralOpenCondition& open_cond,
		       const Link& link,
		       const Reason& establish_reason) const {
  const Negation& negation =
    dynamic_cast<const Negation&>(open_cond.condition());
  const Atom& goal = negation.atom;
  const Formula* goals = &Formula::TRUE;
  const EffectList& effs = step.effects;
  for (EffectListIter ei = effs.begin(); ei != effs.end(); ei++) {
    const Effect& effect = **ei;
    const AtomList& adds = effect.add_list;
    for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
      SubstitutionList mgu;
      if (bindings_.unify(mgu, goal, **gi)) {
	if (mgu.empty()) {
	  return;
	}
	const Formula* binds = &Formula::FALSE;
	for (SubstListIter si = mgu.begin(); si != mgu.end(); si++) {
	  const Substitution& s = **si;
	  binds = &(*binds || *(new Inequality(s.var, s.term)));
	}
	goals = &(*goals && *binds);
      }
    }
  }
  const Formula* bgoal = (!goals->tautology()) ? goals : NULL;
  const LinkChain* links = new LinkChain(&link, links_);
  BindingList new_bindings;
  const OpenConditionChain* open_conds = open_conds_->remove(&open_cond);
  size_t num_open_conds = num_open_conds_ - 1;
  if (!add_goal(open_conds, num_open_conds, new_bindings, *bgoal, step.id,
		establish_reason)) {
    return;
  }
  const Bindings* bindings = bindings_.add(new_bindings);
  if (bindings == NULL) {
    return;
  }
  const UnsafeChain* unsafes = unsafes_;
  size_t num_unsafes = num_unsafes_;
  const Plan* new_plan =
    new Plan(steps_, num_steps_, high_step_id_, links, num_links_ + 1,
	     unsafes, num_unsafes, open_conds, num_open_conds, *bindings,
	     orderings_, this, NORMAL_PLAN);
  new_plans.push_back(new_plan);
}

const Plan* Plan::make_link(const Step& step, const Effect& effect,
			    const LiteralOpenCondition& open_cond,
			    const Link& link, const Reason& establish_reason,
			    const SubstitutionList& unifier) const {
  const LinkChain* links = new LinkChain(&link, links_);
  const VariableList& effect_forall = effect.forall;
  BindingList new_bindings;
  for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
    const Substitution& s = **si;
    if (!member(effect_forall.begin(), effect_forall.end(), &s.var)) {
      new_bindings.push_back(new EqualityBinding(s, establish_reason));
    }
  }
  const OpenConditionChain* open_conds = open_conds_->remove(&open_cond);
  size_t num_open_conds = num_open_conds_ - 1;
  const Formula* cond_goal = NULL;
  if (!effect.condition.tautology()) {
    if (!effect_forall.empty()) {
      SubstitutionList forall_subst;
      for (SubstListIter si = unifier.begin(); si != unifier.end(); si++) {
	const Substitution& s = **si;
	if (member(effect_forall.begin(), effect_forall.end(), &s.var)) {
	  forall_subst.push_back(&s);
	}
      }
      cond_goal = &effect.condition.substitution(forall_subst);
    } else {
      cond_goal = &effect.condition;
    }
    if (!add_goal(open_conds, num_open_conds, new_bindings, *cond_goal,
		  step.id, establish_reason)) {
      return NULL;
    }
  }
  if (step.id > high_step_id_) {
    const Reason& step_reason = AddStepReason::make(params, step.id);
    if (!add_goal(open_conds, num_open_conds, new_bindings, step.precondition,
		  step.id, step_reason)) {
      return NULL;
    }
  }
  const Bindings* bindings = &bindings_;
  size_t num_steps = num_steps_;
  size_t high_step_id = high_step_id_;
  if (step.id > high_step_id_) {
    num_steps++;
    high_step_id = step.id;
    if (params.domain_constraints) {
      bindings = bindings->add(step, *planning_graph);
      if (bindings == NULL) {
	return NULL;
      }
    }
  }
  bindings = bindings->add(new_bindings);
  if (bindings == NULL) {
    return NULL;
  }
  StepChain* steps = new StepChain(&step, steps_);
  const Ordering& new_ordering =
    *(new Ordering(step.id, open_cond.step_id, establish_reason));
  const Orderings& orderings = orderings_.refine(new_ordering, step.id);
  const UnsafeChain* unsafes = unsafes_;
  size_t num_unsafes = num_unsafes_;
  hash_set<size_t> seen_steps;
  for (const StepChain* ss = steps_; ss != NULL; ss = ss->tail) {
    const Step& s = *ss->head;
    if (seen_steps.find(s.id) == seen_steps.end() &&
	orderings.possibly_before(link.from_id, s.id) &&
	orderings.possibly_after(link.to_id, s.id)) {
      seen_steps.insert(s.id);
      const EffectList& effects = s.effects;
      for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
	const Effect& e = **ei;
	const AtomList& adds = e.add_list;
	for (AtomListIter f = adds.begin(); f != adds.end(); f++) {
	  if (bindings->affects(**f, link.condition)) {
	    unsafes = new UnsafeChain(new Unsafe(link, s.id, e, **f),
				      unsafes);
	    num_unsafes++;
	  }
	}
	const NegationList& dels = e.del_list;
	for (NegationListIter f = dels.begin(); f != dels.end(); f++) {
	  if (bindings->affects(**f, link.condition)) {
	    unsafes = new UnsafeChain(new Unsafe(link, s.id, e, **f),
				      unsafes);
	    num_unsafes++;
	  }
	}
      }
    }
  }
  if (step.id > high_step_id_) {
    for (const LinkChain* ls = links_; ls != NULL; ls = ls->tail) {
      const Link& l = *ls->head;
      if (orderings.possibly_before(l.from_id, step.id) &&
	  orderings.possibly_after(l.to_id, step.id)) {
	const EffectList& effects = step.effects;
	for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
	  const Effect& e = **ei;
	  const AtomList& adds = e.add_list;
	  for (AtomListIter f = adds.begin(); f != adds.end(); f++) {
	    if (bindings->affects(**f, l.condition)) {
	      unsafes = new UnsafeChain(new Unsafe(l, step.id, e, **f),
					unsafes);
	      num_unsafes++;
	    }
	  }
	  const NegationList& dels = e.del_list;
	  for (NegationListIter f = dels.begin(); f != dels.end(); f++) {
	    if (bindings->affects(**f, l.condition)) {
	      unsafes = new UnsafeChain(new Unsafe(l, step.id, e, **f),
					unsafes);
	      num_unsafes++;
	    }
	  }
	}
      }
    }
  }
  return new Plan(steps, num_steps, high_step_id, links, num_links_ + 1,
		  unsafes, num_unsafes, open_conds, num_open_conds, *bindings,
		  orderings, this, NORMAL_PLAN);
}


/* Returns the serial number of this plan. */
size_t Plan::serial_no() const {
  return id;
}


/* Returns the number of steps of this plan. */
size_t Plan::num_steps() const {
  return num_steps_ - 2;
}


/* Returns the open conditions of this plan. */
const OpenConditionChain* Plan::open_conds() const {
  return open_conds_;
}


/* Returns the number of open conditions of this plan. */
size_t Plan::num_open_conds() const {
  return num_open_conds_;
}


/* Returns the number of unsafe links of this plan. */
size_t Plan::num_unsafes() const {
  return num_unsafes_;
}


/* Returns the bindings of this plan. */
const Bindings* Plan::bindings() const {
  return params.ground_actions ? NULL : &bindings_;
}


/* Checks if this object is less than the given object. */
bool Plan::less(const LessThanComparable& o) const {
  const Plan& p = dynamic_cast<const Plan&>(o);
  if (rank_.empty()) {
    params.heuristic.plan_rank(rank_, *this, params.weight, planning_graph);
  }
  if (p.rank_.empty()) {
    params.heuristic.plan_rank(rank_, p, params.weight, planning_graph);
  }
  for (size_t i = 0; i < rank_.size(); i++) {
    double diff = rank_[i] - p.rank_[i];
    if (diff > 0.0) {
      return true;
    } else if (diff < 0.0) {
      return false;
    }
  }
  return false;
}


void Plan::print(ostream& os) const {
  const Step* init = NULL;
  const Step* goal = NULL;
  vector<const Step*, container_alloc> ordered_steps;
  hash_set<size_t> seen_steps;
  for (const StepChain* steps = steps_; steps != NULL; steps = steps->tail) {
    if (steps->head->id == 0) {
      init = steps->head;
    } else if (steps->head->id == GOAL_ID) {
      goal = steps->head;
    } else if (seen_steps.find(steps->head->id) == seen_steps.end()) {
      seen_steps.insert(steps->head->id);
      vector<const Step*>::iterator pos = ordered_steps.begin();
      for (; pos != ordered_steps.end(); pos++) {
	if (orderings_.before(steps->head->id, (*pos)->id)) {
	  break;
	}
      }
      ordered_steps.insert(pos, steps->head);
    }
  }
  assert(init != NULL && goal != NULL);
  if (verbosity < 2) {
    os << (num_steps_ - 2);
    for (vector<const Step*>::const_iterator s = ordered_steps.begin();
	 s != ordered_steps.end(); s++) {
      os << ' ' << (*s)->step_formula()->instantiation(bindings_);
    }
  } else {
    os << "Initial  :";
    const EffectList& effects = init->effects;
    for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
      const AtomList& fs = (*ei)->add_list;
      for (AtomListIter j = fs.begin(); j != fs.end(); j++) {
	os << ' ' << **j;
      }
    }
    for (vector<const Step*>::const_iterator s = ordered_steps.begin();
	 s != ordered_steps.end(); s++) {
      os << endl << endl << "Step " << (*s)->id;
      if ((*s)->id < 100) {
	if ((*s)->id < 10) {
	  os << ' ';
	}
	os << ' ';
      }
      os << " : " << (*s)->step_formula()->instantiation(bindings_);
      for (const LinkChain* links = links_;
	   links != NULL; links = links->tail) {
	if (links->head->to_id == (*s)->id) {
	  os << endl << "           " << links->head->from_id;
	  if (links->head->from_id < 10) {
	    os << ' ';
	  }
	  os << " -> " << links->head->condition.instantiation(bindings_);
	  for (const UnsafeChain* unsafes = unsafes_;
	       unsafes != NULL; unsafes = unsafes->tail) {
	    if (&unsafes->head->link == links->head) {
	      os << " <" << unsafes->head->step_id << ">";
	    }
	  }
	}
      }
      for (const OpenConditionChain* oc = open_conds_;
	   oc != NULL; oc = oc->tail) {
	if (oc->head->step_id == (*s)->id) {
	  os << endl << "           ?? -> "
	     << oc->head->condition().instantiation(bindings_);
	}
      }
    }
    os << endl << endl << "Goal     : " << goal->precondition;
    for (const LinkChain* links = links_; links != NULL; links = links->tail) {
      if (links->head->to_id == goal->id) {
	os << endl << "           " << links->head->from_id;
	if (links->head->from_id < 10) {
	  os << ' ';
	}
	os << " -> " << links->head->condition;
	for (const UnsafeChain* unsafes = unsafes_;
	     unsafes != NULL; unsafes = unsafes->tail) {
	  if (&unsafes->head->link == links->head) {
	    os << " <" << unsafes->head->step_id << ">";
	  }
	}
      }
    }
    for (const OpenConditionChain* oc = open_conds_;
	 oc != NULL; oc = oc->tail) {
      if (oc->head->step_id == goal->id) {
	os << endl << "           ?? -> " << oc->head->condition();
      }
    }
    os << endl << "bindings = " << bindings_;
    os << endl << "orderings = " << orderings_;
  }
}

bool Plan::duplicate() const {
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
  return false;
}

typedef hash_map<size_t, const Formula*, hash<size_t>,
  equal_to<size_t>, container_alloc> FormulaMap;

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

bool Plan::equivalent(const Plan& p) const {
  if (num_steps_ == p.num_steps_ && num_open_conds_ == p.num_open_conds_
      && num_links_ == p.num_links_) {
    if (verbosity > 2) {
      cout << "matching number of steps, etc..." << endl;
    }
    // instantiate steps in this and p
    FormulaMap steps1;
    for (const StepChain* s = steps_; s != NULL; s = s->tail) {
      size_t id = s->head->id;
      if (s->head->action != NULL && steps1.find(id) == steps1.end()) {
	if (params.ground_actions) {
	  steps1[id] = s->head->step_formula();
	} else {
	  steps1[id] = &s->head->step_formula()->instantiation(bindings_);
	}
      }
    }
    FormulaMap steps2;
    for (const StepChain* s = p.steps_; s != NULL; s = s->tail) {
      size_t id = s->head->id;
      if (s->head->action != NULL && steps2.find(id) == steps2.end()) {
	if (params.ground_actions) {
	  steps2[id] = s->head->step_formula();
	} else {
	  steps2[id] = &s->head->step_formula()->instantiation(p.bindings_);
	}
      }
    }
    // try to find mapping between steps so that bindings and orderings match
    hash_map<size_t, size_t> step_map;
    return matching_plans(step_map, steps1, steps2, steps1.begin());
  }
  return false;
}

#if 0
void Plan::h_rank() const {
  if (rank1_ >= 0) {
    return;
  }
  if (params.heuristic.oc()) {
    rank1_ = num_steps_ + num_open_conds_;
    rank2_ = 0;
    return;
  }
  if (params.heuristic.ucpop()) {
    rank1_ = num_steps_ + num_open_conds_ + num_unsafes_;
    rank2_ = 0;
    return;
  }
  CostGraph cg;
  cg.add_node(0, 1);
  hash_map<size_t, size_t> step_nodes;
  hash_map<const Formula*, size_t> f_nodes;
  size_t goal_node = make_node(cg, step_nodes, f_nodes, GOAL_ID);
  if (params.heuristic.max()) {
    for (const OrderingChain* ords = orderings_.orderings();
	 ords != NULL; ords = ords->tail) {
      cg.set_distance(step_nodes[ords->head->after_id],
		      step_nodes[ords->head->before_id], 1);
    }
  }
  if (verbosity > 4) {
    cout << "@@@@cost graph:" << endl << cg << endl;
    cg.cost(goal_node);
    cout << "@@@@cost graph:" << endl << cg << endl;
  }
  pair<int, int> cost = cg.cost(goal_node);
  rank1_ = cost.first;
  rank2_ = cost.second;
#if 0
  AchievesMap::const_iterator fali = achieves.find(&oc->head->condition);
  if (fali != achieves.end()) {
    int l = (*fali).second.size();
    if (l > high_link
	|| (l == high_link && params.flaw_order.fifo())) {
      most_linkable_open_cond_ = oc->head;
      high_link = l;
    }
    if (l < low_link
	|| (l == low_link && params.flaw_order.fifo())) {
      least_linkable_open_cond_ = oc->head;
      low_link = l;
    }
  }
#endif
}


size_t Plan::make_node(CostGraph& cg, hash_map<size_t, size_t>& step_nodes,
		       hash_map<const Formula*, size_t>& f_nodes,
		       size_t step_id) const {
  hash_map<size_t, size_t>::const_iterator s = step_nodes.find(step_id);
  if (s != step_nodes.end()) {
    /* node already exists */
    return (*s).second;
  } else {
    /* create new node */
    size_t step_node;
    if (params.heuristic.max()) {
      step_node = cg.add_max_node();
    } else {
      step_node = cg.add_sum_node();
    }
    const Step* step = find_step(steps_, step_id);
    assert(step != NULL);
    step_nodes[step->id] = step_node;
    if (verbosity > 4) {
      cout << "step " << step->id << " is node " << step_node << endl;
    }
    for (const LinkChain* links = links_; links != NULL; links = links->tail) {
      if (links->head->to_id == step->id) {
	if (links->head->from_id == 0) {
	  cg.set_distance(step_node, 0, 0);
	} else {
	  if (!params.heuristic.sum()
	      || step_nodes.find(links->head->from_id) == step_nodes.end()) {
	    size_t prec_node =
	      make_node(cg, step_nodes, f_nodes, links->head->from_id);
	    cg.set_distance(step_node, prec_node, 1);
	  }
	}
      }
    }
    for (const OpenConditionChain* oc = open_conds_;
	 oc != NULL; oc = oc->tail) {
      if (oc->head->step_id == step->id) {
	size_t prec_node = make_node(cg, step_nodes, f_nodes,
				     oc->head->condition, oc->head->step_id);
	cg.set_distance(step_node, prec_node, 0);
	if (verbosity > 4) {
	  cout << "open condition " << *oc->head << " is node " << prec_node
	       << endl;
	}
      }
    }
    return step_node;
  }
}


/* Returns a cost graph node for the given formula. */
size_t Plan::make_node(CostGraph& cg, hash_map<size_t, size_t>& step_nodes,
		       hash_map<const Formula*, size_t>& f_nodes,
		       const Formula& cond, size_t step_id) const {
  if (params.heuristic.sum_reuse()) {
    hash_map<const Formula*, size_t>::const_iterator f = f_nodes.find(&cond);
    if (f != f_nodes.end()) {
      return 1;
    }
    // try to reuse step
    hash_set<size_t> seen_steps;
    for (const StepChain* steps = steps_; steps != NULL; steps = steps->tail) {
      const Step& step = *steps->head;
      if (step.id != 0 && seen_steps.find(step.id) == seen_steps.end()
	  && orderings_.possibly_before(step.id, step_id)) {
	seen_steps.insert(step.id);
	const EffectList& effs = step.effects;
	for (EffectListIter ei = effs.begin(); ei != effs.end(); ei++) {
	  const AtomList& adds = (*ei)->add_list;
	  for (AtomListIter j = adds.begin(); j != adds.end(); j++) {
	    if (bindings_.unify(cond, **j)) {
	      return 1;
	    }
	  }
	}
      }
    }
  }
  HeuristicValue v = cond.heuristic_value(*planning_graph,
					  (params.ground_actions
					   ? NULL : &bindings_));
  int cost;
  int work;
  if (params.heuristic.max()) {
    cost = v.max_cost();
    work = v.max_work();
  } else {
    cost = v.sum_cost();
    work = v.sum_work();
  }
  size_t cond_node = cg.add_node(params.weight*cost, work);
  f_nodes[&cond] = cond_node;
  return cond_node;
}
#endif
