/*
 * $Id: plans.cc,v 1.23 2001-10-06 15:00:15 lorens Exp $
 */
#include <queue>
#include <hash_set>
#include <algorithm>
#include <climits>
#include <cassert>
#include "plans.h"
#include "parameters.h"
#include "costgraph.h"


/* Id of goal step. */
const size_t Plan::GOAL_ID = UINT_MAX;

struct AchievesMap : public gc,
		     hash_map<const Formula*, ActionList, hash<const Formula*>,
		     equal_to<const Formula*>, container_alloc> {
};


/* Planning parameters. */
static Parameters params;
/* Verbosity. */
static int verbosity = 0;
/* Number of visited plans. */
size_t num_visited_plans = 0;
/* Number of generated plans. */
size_t num_generated_plans = 0;
/* Domain of problem currently being solved. */
static const Domain* domain = NULL;
/* Maps ground atomic formulas to fully instantiated actions. */
static AchievesMap achieves;
/* Maps predicates to actions. */
static hash_map<string, ActionList> achieves_pred;
/* Maps negated predicates to actions. */
static hash_map<string, ActionList> achieves_neg_pred;
/* Maps ground atomic formulas to heuristic cost. */
static hash_map<const Formula*, Cost> atom_cost;


/*
 * Abstract reason.
 */
struct Reason : public Printable {
  /* Checks if this reason involves the given link. */
  virtual bool involves(const Link& link) const {
    return false;
  }

  /* Checks if this reason involves the given step. */
  virtual bool involves(const Step& step) const {
    return false;
  }
};


/*
 * Reason attached to elements of the initial plan.
 */
struct InitReason : public Reason {
protected:
  /* Prints this reason on the given stream. */
  virtual void print(ostream& os) const {
    os << "#<InitReason>";
  }
};


/*
 * Reason attached to elements added along with a step.
 */
struct AddStepReason : public Reason {
  /* Id of added step. */
  const size_t step_id;

  /* Constructs an AddStep reason. */
  AddStepReason(size_t step_id)
    : step_id(step_id) {
  }

  /* Checks if this reason involves the given step. */
  virtual bool involves(const Step& step) const {
    return step_id == step.id;
  }

protected:
  /* Prints this reason on the given stream. */
  virtual void print(ostream& os) const {
    os << "#<AddStepReason " << step_id << ">";
  }
};


/*
 * Reason attached to elements needed to establish a link.
 */
struct EstablishReason : public Reason {
  /* Established link */
  const Link& link;

  /* Constructs an Establish reason. */
  EstablishReason(const Link& link)
    : link(link) {
  }

  /* Checks if this reason involves the given link. */
  virtual bool involves(const Link& link) const {
    return &this->link == &link;
  }

protected:
  /* Prints this reason on the given stream. */
  virtual void print(ostream& os) const {
    os << "#<EstablishReason " << link << ">";
  }
};


/*
 * Reason attached to elements needed to protect a link.
 */
struct ProtectReason : public Reason {
  /* Protected link. */
  const Link& link;
  /* Id of threatening step. */
  const size_t step_id;

  /* Constructs a Protect reason. */
  ProtectReason(const Link& link, size_t step_id)
    : link(link), step_id(step_id) {
  }

  /* Checks if this reason involves the given link. */
  virtual bool involves(const Link& link) const {
    return &this->link == &link;
  }

  /* Checks if this reason involves the given step. */
  virtual bool involves(const Step& step) const {
    return step_id == step.id;
  }

protected:
  /* Prints this reason on the given stream. */
  virtual void print(ostream& os) const {
    os << "#<ProtectReason " << link << " step " << step_id << ">";
  }
};


struct less<const Plan*> {
  /* Should return true if p1 is worse than p2 */
  bool operator()(const Plan* p1, const Plan* p2) {
    int d = p1->primary_rank() - p2->primary_rank();
    if (d == 0) {
      d = p1->secondary_rank() - p2->secondary_rank();
      if (d == 0) {
	return p1->tertiary_rank() > p2->tertiary_rank();
      } else {
	return d > 0;
      }
    } else {
      return d > 0;
    }
  }
};


/* Prints this causal link. */
void Link::print(ostream& os) const {
  os << "#<LINK " << from_id << ' ' << condition << ' ' << to_id << '>';
}


/* Adds atomic goal to chain of open conditions, and returns true if
   and only if the goal is consistent. */
static bool add_open_condition(const OpenConditionChain*& open_conds,
			       size_t& num_open_conds,
			       const Formula& goal, size_t step_id,
			       const Reason& reason, const LinkChain* links) {
  /*
   * Add goal as open condition.
   */
  const OpenCondition* open_cond;
  const Atom* atom = dynamic_cast<const Atom*>(&goal);
  if (atom != NULL) {
    open_cond = new PredicateOpenCondition(goal, step_id, reason,
					   atom->predicate);
  } else {
    const Negation* negation = dynamic_cast<const Negation*>(&goal);
    if (negation != NULL) {
      open_cond = new PredicateOpenCondition(goal, step_id, reason,
					     negation->atom.predicate, true);
    } else {
      open_cond = new OpenCondition(goal, step_id, reason);
    }
  }
  open_conds = new OpenConditionChain(open_cond, open_conds);
  num_open_conds++;
  return true;
}


/* Adds atomic goal to chain of open conditions, and returns true if
   and only if the goal is consistent. */
static bool add_goal(const OpenConditionChain*& open_conds,
		     size_t& num_open_conds, BindingList& new_bindings,
		     const Formula& goal, size_t step_id, const Reason& reason,
		     const LinkChain* links = NULL) {
  if (goal == Formula::TRUE) {
    return true;
  } else if (goal == Formula::FALSE) {
    return false;
  }
  size_t num_bindings = 0;
  stack<const Formula*, deque<const Formula*, container_alloc> > goals;
  goals.push(&goal);
  while(!goals.empty()) {
    const Formula* goal = goals.top();
    goals.pop();
    const Equality* eq = dynamic_cast<const Equality*>(goal);
    if (eq != NULL) {
      const Variable* var = dynamic_cast<const Variable*>(&eq->term1);
      if (var != NULL) {
	new_bindings.push_back(new EqualityBinding(*var, eq->term2, reason));
	num_bindings++;
      } else {
	var = dynamic_cast<const Variable*>(&eq->term2);
	if (var != NULL) {
	  new_bindings.push_back(new EqualityBinding(*var, eq->term1, reason));
	  num_bindings++;
	} else {
	  if (eq->term1 != eq->term2) {
	    return false;
	  }
	}
      }
    } else {
      const Inequality* neq = dynamic_cast<const Inequality*>(goal);
      if (neq != NULL) {
	const Variable* var = dynamic_cast<const Variable*>(&neq->term1);
	if (var != NULL) {
	  new_bindings.push_back(new InequalityBinding(*var, neq->term2,
						       reason));
	  num_bindings++;
	} else {
	  var = dynamic_cast<const Variable*>(&neq->term2);
	  if (var != NULL) {
	    new_bindings.push_back(new InequalityBinding(*var, neq->term1,
							 reason));
	    num_bindings++;
	  } else {
	    if (neq->term1 == neq->term2) {
	      return false;
	    }
	  }
	}
      } else {
	const Conjunction* conjunction =
	  dynamic_cast<const Conjunction*>(goal);
	if (conjunction != NULL) {
	  const FormulaList& gs = conjunction->conjuncts;
	  for (FLCI i = gs.begin(); i != gs.end(); i++) {
	    goals.push(*i);
	  }
	} else {
	  const ForallFormula* forall =
	    dynamic_cast<const ForallFormula*>(goal);
	  if (forall != NULL) {
	    // handle FORALL
	    throw Unimplemented("adding universally quantified goal");
	  } else {
	    const ExistsFormula* exists =
	      dynamic_cast<const ExistsFormula*>(goal);
	    if (exists != NULL) {
	      // handle EXISTS
	      throw Unimplemented("adding existentially quantified goal");
	    } else {
	      if (!add_open_condition(open_conds, num_open_conds, *goal,
				      step_id, reason, links)) {
		return false;
	      }
	    }
	  }
	}
      }
    }
  }
  return true;
}


/* Returns a chain of unsafes with all obsolete unsafes in the given
   chain removed, and sets num_unsafes to the remaining number of
   unsafes. */
static const UnsafeChain*
remove_obsolete_unsafes(const UnsafeChain* unsafes, size_t& num_unsafes,
			const Orderings& orderings, const Bindings& bindings) {
  if (unsafes == NULL) {
    return NULL;
  } else {
    const UnsafeChain* tail =
      remove_obsolete_unsafes(unsafes->tail, num_unsafes, orderings, bindings);
    const Unsafe* u = unsafes->head;
    if (!(orderings.possibly_before(u->link.from_id, u->step_id) &&
	  orderings.possibly_after(u->link.to_id, u->step_id) &&
	  bindings.affects(u->effect_add, u->link.condition))) {
      num_unsafes--;
      return tail;
    } else {
      return new UnsafeChain(u, tail);
    }
  }
}


/* Returns plan for given problem. */
const Plan* Plan::plan(const Problem& problem, const Parameters& p, int v) {
  /* Set planning parameters. */
  params = p;
  /* Set verbosity. */
  verbosity = v;
  /* Set current domain. */
  domain = &problem.domain;
  /* Extract list of actions */
  ActionList all_actions;
  achieves.clear();
  achieves_pred.clear();
  achieves_neg_pred.clear();
  if (params.ground_actions || !params.heuristic.ucpop()) {
    ActionList inst_actions;
    problem.instantiated_actions(inst_actions);
    if (verbosity > 0) {
      cout << endl << inst_actions.size() << " instantiated actions." << endl;
    }
    params.heuristic.compute_cost(atom_cost, problem, inst_actions);
    if (verbosity > 1) {
      for (hash_map<const Formula*, Cost>::const_iterator ci =
	     atom_cost.begin(); ci != atom_cost.end(); ci++) {
	cout << "cost of " << *(*ci).first << " is (" << (*ci).second.cost
	     << ", " << (*ci).second.work << ")" << endl;
      }
    }
    for (ActionList::const_iterator i = inst_actions.begin();
	 i != inst_actions.end(); i++) {
      if ((*i)->precondition.cost(atom_cost, params.heuristic).cost >= 0) {
	all_actions.push_back(*i);
      }
    }
  }
  if (params.ground_actions) {
    for (ActionList::const_iterator i = all_actions.begin();
	 i != all_actions.end(); i++) {
      AtomList add_list;
      NegationList del_list;
      (*i)->achievable_goals(add_list, del_list);
      for (AtomListIter gi = add_list.begin(); gi != add_list.end(); gi++) {
	achieves[*gi].push_back(*i);
      }
      for (NegationListIter gi = del_list.begin();
	   gi != del_list.end(); gi++) {
	achieves[*gi].push_back(*i);
      }
    }
    if (verbosity > 0) {
      cout << achieves.size() << " achievable atoms." << endl;
      cout << endl << all_actions.size() << " performable actions." << endl;
      if (verbosity > 2) {
	for (ActionList::const_iterator i = all_actions.begin();
	     i != all_actions.end(); i++) {
	  if (verbosity > 3) {
	    cout << "  " << **i << endl;
	  } else {
	    cout << "  " << (*i)->action_formula() << endl;
	  }
	}
	cout << endl;
      }
    }
  } else {
    all_actions.clear();
    for (ActionMap::const_iterator i = domain->actions.begin();
	 i != domain->actions.end(); i++) {
      all_actions.push_back((*i).second);
    }
    for (ActionList::const_iterator i = all_actions.begin();
	 i != all_actions.end(); i++) {
      hash_set<string> preds;
      hash_set<string> neg_preds;
      (*i)->achievable_predicates(preds, neg_preds);
      for (hash_set<string>::const_iterator j = preds.begin();
	   j != preds.end(); j++) {
	achieves_pred[*j].push_back(*i);
      }
      for (hash_set<string>::const_iterator j = neg_preds.begin();
	   j != neg_preds.end(); j++) {
	achieves_neg_pred[*j].push_back(*i);
      }
    }
  }
  /* Reset number of visited plan. */
  num_visited_plans = 0;
  /* Reset number of generated plans. */
  num_generated_plans = 0;

  /* Queue of pending plans. */
  priority_queue<const Plan*, PlanList> plans;
  /* Construct the initial plan. */
  const Plan* current_plan = make_initial_plan(problem);
  current_plan->id = 0;
  num_generated_plans++;
  /*
   * Search for complete plan.
   */
  while (current_plan != NULL && !current_plan->complete()) {
    if (num_generated_plans >= params.search_limit) {
      /* Search limit reached. */
      break;
    }
    /*
     * This is a new plan.
     */
    num_visited_plans++;
    if (verbosity > 1) {
      cout << endl << "!!!!CURRENT PLAN (id " << current_plan->id << ")"
	   << " with rank " << current_plan->primary_rank() << ','
	   << current_plan->secondary_rank() << endl;
      cout << *current_plan << endl;
    }
    /* List of children to current plan. */
    PlanList refinements;
    /* Get plan refinements. */
    current_plan->refinements(refinements);
    /* Add children to queue of pending plans. */
    for (PlanList::const_iterator i = refinements.begin();
	 i != refinements.end(); i++) {
      if ((*i)->primary_rank() < INT_MAX) {
	(*i)->id = num_generated_plans;
	plans.push(*i);
	num_generated_plans++;
	if (verbosity > 2) {
	  cout << endl << "####CHILD (id " << (*i)->id << ")"
	       << " with rank " << (*i)->primary_rank()
	       << ',' << (*i)->secondary_rank() << ':' << endl
	       << **i << endl;
	}
      }
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
    if (params.transformational) {
      plans = priority_queue<const Plan*, PlanList>();
    }
  }
  if (verbosity > 0) {
    /*
     * Print statistics.
     */
    cout << endl << "Plans visited: " << num_visited_plans;
    cout << endl << "Plans generated: " << num_generated_plans << endl;
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
int Plan::primary_rank() const {
  h_rank();
  return rank1_;
}


/* Returns the secondary rank of this plan, where a lower rank
   signifies a better plan. */
int Plan::secondary_rank() const {
  if (complete()) {
    return -1;
  } else {
    h_rank();
    return rank2_;
  }
}


/* Returns the tertiary rank of this plan, where a lower rank
   signifies a better plan. */
int Plan::tertiary_rank() const {
#if 0
  return num_unsafes_;
#else
  return 0;
#endif
}


/* Returns the initial plan representing the given problem, or NULL
   if goals of problem are inconsistent. */
const Plan* Plan::make_initial_plan(const Problem& problem) {
  /* Reason for initial steps in plan. */
  const Reason& init_reason = *(new InitReason());

  /*
   * Create step representing initial conditions of problem.
   */
  EffectList& init = *(new EffectList());
  if (problem.init != NULL) {
    /* Add initial conditions of problem as effects. */
    init.push_back(problem.init);
  }
  /* Initial step. */
  const Step& init_step = *(new Step(0, Formula::TRUE, init, init_reason));

  /*
   * Create step representing goal of problem.
   */
  /* Goal step. */
  const Step& goal_step =
    *(new Step(Plan::GOAL_ID, problem.goal, *(new EffectList()), init_reason));
  /* Reason for open conditions of goal. */
  const Reason& goal_reason = *(new AddStepReason(goal_step.id));
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
  const Bindings* bindings = Bindings().add(new_bindings);
  if (bindings == NULL) {
    /* Bindings are inconsistent. */
    return NULL;
  }
  /* Return initial plan. */
  return new Plan(steps, 2, 0, NULL, 0, NULL, 0, open_conds, num_open_conds,
		  NULL, *bindings, *(new Orderings()), NULL);
}


const Flaw& Plan::get_flaw() const {
  if (unsafes_ != NULL) {
    return *unsafes_->head;
  } else {
    if (!params.heuristic.ucpop()) {
      h_rank();
      if (params.flaw_order.most_cost_first()) {
	if (most_cost_open_cond_ != NULL) {
	  return *most_cost_open_cond_;
	}
      } else if (params.flaw_order.least_cost_first()) {
	if (least_cost_open_cond_ != NULL) {
	  return *least_cost_open_cond_;
	}
      } else if (params.flaw_order.most_work_first()) {
	if (most_work_open_cond_ != NULL) {
	  return *most_work_open_cond_;
	}
      } else if (params.flaw_order.least_work_first()) {
	if (least_work_open_cond_ != NULL) {
	  return *least_work_open_cond_;
	}
      } else if (params.flaw_order.most_linkable_first()) {
	if (most_linkable_open_cond_ != NULL) {
	  return *most_linkable_open_cond_;
	}
      } else if (params.flaw_order.least_linkable_first()) {
	if (least_linkable_open_cond_ != NULL) {
	  return *least_linkable_open_cond_;
	}
      }
    }
    if (params.flaw_order.fifo()) {
      for (const OpenConditionChain* oc = open_conds_; ; oc = oc->tail) {
	if (oc->tail == NULL) {
	  return *oc->head;
	}
      }
    }
    return *open_conds_->head;
  }
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
    separate(new_plans, unsafe);
    demote(new_plans, unsafe);
    promote(new_plans, unsafe);
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
	       num_open_conds_, open_conds_, bindings_, orderings_, this);
    new_plans.push_back(p);
  }
}

void Plan::separate(PlanList& new_plans, const Unsafe& unsafe) const {
  SubstitutionList unifier;
  bindings_.affects(unifier, unsafe.effect_add, unsafe.link.condition);
  const VariableList& effect_forall = unsafe.effect.forall;
  const Formula* goal = &Formula::FALSE;
  for (SubstitutionList::const_iterator i = unifier.begin();
       i != unifier.end(); i++) {
    const Substitution& s = **i;
    if (!effect_forall.contains(s.var)) {
      const Inequality& neq = *new Inequality(s.var, s.term);
      if (bindings_.consistent_with(neq)) {
	goal = &(*goal || neq);
      }
    }
  }
  const Formula& effect_cond = unsafe.effect.condition;
  if (effect_cond != Formula::TRUE) {
    const Formula* cond_goal;
    if (!effect_forall.empty()) {
      SubstitutionList forall_subst;
      for (SubstitutionList::const_iterator i = unifier.begin();
	   i != unifier.end(); i++) {
	const Substitution& s = **i;
	if (effect_forall.contains(s.var)) {
	  forall_subst.push_back(&s);
	}
      }
      cond_goal = &effect_cond.substitution(forall_subst);
    } else {
      cond_goal = &effect_cond;
    }
    goal = &(*goal || *cond_goal);
  }
  if (*goal != Formula::FALSE) {
    const OpenConditionChain* open_conds = open_conds_;
    size_t num_open_conds = num_open_conds_;
    BindingList new_bindings;
    const Reason& protect_reason =
      *(new ProtectReason(unsafe.link, unsafe.step_id));
    if (add_goal(open_conds, num_open_conds, new_bindings, *goal,
		 unsafe.step_id, protect_reason, links_)) {
      const Bindings* bindings = bindings_.add(new_bindings);
      if (bindings != NULL) {
	const UnsafeChain* unsafes = unsafes_->remove(&unsafe);
	size_t num_unsafes = num_unsafes_ - 1;
#ifdef DELETE_OBSOLETE_UNSAFES
	unsafes = remove_obsolete_unsafes(unsafes, num_unsafes,
					  orderings_, *bindings);
#endif
	const Plan* p =
	  new Plan(steps_, num_steps_, high_step_id_, links_, num_links_,
		   unsafes, num_unsafes, open_conds, num_open_conds,
		   open_conds_, *bindings, orderings_, this);
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
      *(new ProtectReason(unsafe.link, unsafe.step_id));
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
      *(new ProtectReason(unsafe.link, unsafe.step_id));
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
#ifdef DELETE_OBSOLETE_UNSAFES
  unsafes = remove_obsolete_unsafes(unsafes, num_unsafes,
				    orderings, bindings_);
#endif
  const Plan* new_plan =
    new Plan(steps_, num_steps_, high_step_id_, links_, num_links_,
	     unsafes, num_unsafes, open_conds_, num_open_conds_,
	     open_conds_, bindings_, orderings, this);
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
  const AddStepReason* sr =
    dynamic_cast<const AddStepReason*>(&open_cond.reason);
  if (sr != NULL) {
    return find_step(steps, sr->step_id) != NULL;
  } else {
    const EstablishReason* er =
      dynamic_cast<const EstablishReason*>(&open_cond.reason);
    if (er != NULL) {
      return links->contains(&er->link);
    } else {
      return false;
    }
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
      const OpenCondition* open_cond;
      const Atom* atom = dynamic_cast<const Atom*>(&l.condition);
      if (atom != NULL) {
	open_cond = new PredicateOpenCondition(l.condition, l.to_id, l.reason,
					       atom->predicate);
      } else {
	const Negation* negation = dynamic_cast<const Negation*>(&l.condition);
	if (negation != NULL) {
	  open_cond = new PredicateOpenCondition(l.condition, l.to_id,
						 l.reason,
						 negation->atom.predicate,
						 true);
	} else {
	  open_cond = new OpenCondition(l.condition, l.to_id, l.reason);
	}
      }
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
  /* N.B. old_open_conds is set open_conds.  This disables early
     linking of open conditions added during unlinking. */
  const Plan* plan =
    new Plan(steps, num_steps, high_step_id_, links, num_links,
	     unsafes, num_unsafes, open_conds, num_open_conds, open_conds,
	     *(Bindings::make_bindings(equalities, inequalities)),
	     *(new Orderings(steps, orderings)), this, INTERMEDIATE_PLAN);
  return pair<const Plan*, const OpenCondition*>(plan, link_cond);
}

void Plan::handle_open_condition(PlanList& new_plans,
				 const OpenCondition& open_cond) const {
  const Disjunction* disjunction =
    dynamic_cast<const Disjunction*>(&open_cond.condition);
  if (disjunction != NULL) {
    handle_disjunction(new_plans, open_cond);
  } else {
    const PredicateOpenCondition& poc =
      dynamic_cast<const PredicateOpenCondition&>(open_cond);
    add_step(new_plans, poc);
    reuse_step(new_plans, poc);
  }
}

void Plan::handle_disjunction(PlanList& new_plans,
			      const OpenCondition& open_cond) const {
  const Disjunction& disjunction =
    dynamic_cast<const Disjunction&>(open_cond.condition);
  const FormulaList& disjuncts = disjunction.disjuncts;
  for (FLCI g = disjuncts.begin(); g != disjuncts.end(); g++) {
    BindingList new_bindings;
    const OpenConditionChain* open_conds = open_conds_->remove(&open_cond);
    const OpenConditionChain* old_open_conds = open_conds;
    size_t num_open_conds = num_open_conds_ - 1;
    if (add_goal(open_conds, num_open_conds, new_bindings, **g,
		 open_cond.step_id, open_cond.reason, links_)) {
      const Bindings* bindings;
      bindings = bindings_.add(new_bindings);
      if (bindings != NULL) {
	const UnsafeChain* unsafes = unsafes_;
	size_t num_unsafes = num_unsafes_;
#ifdef DELETE_OBSOLETE_UNSAFES
	unsafes = remove_obsolete_unsafes(unsafes, num_unsafes,
					  orderings_, *bindings);
#endif
	const Plan* new_plan =
	  new Plan(steps_, num_steps_, high_step_id_, links_, num_links_,
		   unsafes, num_unsafes, open_conds, num_open_conds,
		   old_open_conds, *bindings, orderings_, this);
	new_plans.push_back(new_plan);
      }
    }
  }
}

void Plan::add_step(PlanList& new_plans,
		    const PredicateOpenCondition& open_cond) const {
  ActionList actions;
  AchievesMap::const_iterator fali = achieves.find(&open_cond.condition);
  if (fali != achieves.end()) {
    copy((*fali).second.begin(), (*fali).second.end(), back_inserter(actions));
  }
  if (!open_cond.negated) {
    hash_map<string, ActionList>::const_iterator sali =
      achieves_pred.find(open_cond.predicate);
    if (sali != achieves_pred.end()) {
      copy((*sali).second.begin(), (*sali).second.end(),
	   back_inserter(actions));
    }
  } else {
    hash_map<string, ActionList>::const_iterator sali =
      achieves_neg_pred.find(open_cond.predicate);
    if (sali != achieves_neg_pred.end()) {
      copy((*sali).second.begin(), (*sali).second.end(),
	   back_inserter(actions));
    }
  }
  size_t step_id = high_step_id_ + 1;
  if (!actions.empty()) {
    const Link& link = *(new Link(step_id, open_cond));
    const Reason& establish_reason = *(new EstablishReason(link));
    size_t num_prev_plans = new_plans.size();
    for (ActionList::const_iterator i = actions.begin();
	 i != actions.end(); i++) {
      const Step& step = *(new Step(step_id, **i, establish_reason));
      new_link(new_plans, step, open_cond, link, establish_reason);
      if (new_plans.size() > num_prev_plans) {
	// link preconditions of step, and add that plan
	const Plan& p = *new_plans.back();
	if (verbosity > 3) {
	  cout << "new open conditions:" << endl;
	  for (const OpenConditionChain* oc = p.open_conds_;
	       oc != p.old_open_conds_; oc = oc->tail) {
	    cout << "  " << *oc->head << endl;
	  }
	}
      }
    }
  }
}

void Plan::reuse_step(PlanList& new_plans,
		      const PredicateOpenCondition& open_cond) const {
  hash_set<size_t> seen_steps;
  for (const StepChain* steps = steps_; steps != NULL; steps = steps->tail) {
    const Step& step = *steps->head;
    if (seen_steps.find(step.id) == seen_steps.end() &&
	orderings_.possibly_before(step.id, open_cond.step_id)) {
      seen_steps.insert(step.id);
      const Link& link = *(new Link(step.id, open_cond));
      const Reason& establish_reason = *(new EstablishReason(link));
      new_link(new_plans, step, open_cond, link, establish_reason);
    }
  }
}

bool Plan::new_link(PlanList& new_plans, const Step& step,
		    const PredicateOpenCondition& open_cond, const Link& link,
		    const Reason& reason) const {
  size_t prev_num_plans = new_plans.size();
  if (step.id == 0 &&
      dynamic_cast<const Negation*>(&open_cond.condition) != NULL) {
    new_cw_link(new_plans, step, open_cond, link, reason);
  }
  const Formula& goal = open_cond.condition;
  const EffectList& effs = step.effects;
  for (EffectList::const_iterator i = effs.begin(); i != effs.end(); i++) {
    const Effect& effect = **i;
    if (open_cond.negated) {
      const NegationList& dels = effect.del_list;
      for (NegationListIter gi = dels.begin(); gi != dels.end(); gi++) {
	SubstitutionList mgu;
	if (bindings_.unify(mgu, **gi, goal)) {
	  const Plan* new_plan =
	    make_link(step, effect, open_cond, link, reason, mgu);
	  if (new_plan != NULL) {
	    new_plans.push_back(new_plan);
	  }
	}
      }
    } else {
      const AtomList& adds = effect.add_list;
      for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
	SubstitutionList mgu;
	if (bindings_.unify(mgu, **gi, goal)) {
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
		       const PredicateOpenCondition& open_cond,
		       const Link& link,
		       const Reason& establish_reason) const {
  const Negation& negation =
    dynamic_cast<const Negation&>(open_cond.condition);
  const Atom& goal = negation.atom;
  const Formula* goals = &Formula::TRUE;
  const EffectList& effs = step.effects;
  for (EffectList::const_iterator i = effs.begin(); i != effs.end(); i++) {
    const Effect& effect = **i;
    const AtomList& adds = effect.add_list;
    for (AtomListIter gi = adds.begin(); gi != adds.end(); gi++) {
      SubstitutionList mgu;
      if (bindings_.unify(mgu, **gi, goal)) {
	if (mgu.empty()) {
	  return;
	}
	const Formula* binds = &Formula::FALSE;
	for (SubstitutionList::const_iterator i = mgu.begin();
	     i != mgu.end(); i++) {
	  const Substitution& s = **i;
	  binds = &(*binds || *(new Inequality(s.var, s.term)));
	}
	goals = &(*goals && *binds);
      }
    }
  }
  const Formula* bgoal = (*goals != Formula::TRUE) ? goals : NULL;
  const LinkChain* links = new LinkChain(&link, links_);
  BindingList new_bindings;
  const OpenConditionChain* open_conds = open_conds_->remove(&open_cond);
  const OpenConditionChain* old_open_conds = open_conds;
  size_t num_open_conds = num_open_conds_ - 1;
  if (!add_goal(open_conds, num_open_conds, new_bindings, *bgoal, step.id,
		establish_reason, links)) {
    return;
  }
  const Bindings* bindings = bindings_.add(new_bindings);
  if (bindings == NULL) {
    return;
  }
  const UnsafeChain* unsafes = unsafes_;
  size_t num_unsafes = num_unsafes_;
#ifdef DELETE_OBSOLETE_UNSAFES
  unsafes = remove_obsolete_unsafes(unsafes, num_unsafes,
				    orderings_, *bindings);
#endif
  const Plan* new_plan =
    new Plan(steps_, num_steps_, high_step_id_, links, num_links_ + 1,
	     unsafes, num_unsafes, open_conds, num_open_conds,
	     old_open_conds, *bindings, orderings_, this, NORMAL_PLAN);
  new_plans.push_back(new_plan);
}

const Plan* Plan::make_link(const Step& step, const Effect& effect,
			    const PredicateOpenCondition& open_cond,
			    const Link& link, const Reason& establish_reason,
			    const SubstitutionList& unifier) const {
  const LinkChain* links = new LinkChain(&link, links_);
  const VariableList& effect_forall = effect.forall;
  BindingList new_bindings;
  for (SubstitutionList::const_iterator i = unifier.begin();
       i != unifier.end(); i++) {
    const Substitution& s = **i;
    if (!effect_forall.contains(s.var)) {
      new_bindings.push_back(new EqualityBinding(s, establish_reason));
    }
  }
  const OpenConditionChain* open_conds = open_conds_->remove(&open_cond);
  const OpenConditionChain* old_open_conds = open_conds;
  size_t num_open_conds = num_open_conds_ - 1;
  const Formula* cond_goal = NULL;
  if (effect.condition != Formula::TRUE) {
    if (!effect_forall.empty()) {
      SubstitutionList forall_subst;
      for (SubstitutionList::const_iterator i = unifier.begin();
	   i != unifier.end(); i++) {
	const Substitution& s = **i;
	if (effect_forall.contains(s.var)) {
	  forall_subst.push_back(&s);
	}
      }
      cond_goal = &effect.condition.substitution(forall_subst);
    } else {
      cond_goal = &effect.condition;
    }
    if (!add_goal(open_conds, num_open_conds, new_bindings, *cond_goal,
		  step.id, establish_reason, links)) {
      return NULL;
    }
  }
  const Reason* step_reason = NULL;
  if (step.id > high_step_id_) {
    step_reason = new AddStepReason(step.id);
    if (!add_goal(open_conds, num_open_conds, new_bindings, step.precondition,
		  step.id, *step_reason, links)) {
      return NULL;
    }
  }
  const Bindings* bindings;
  bindings = bindings_.add(new_bindings);
  if (bindings == NULL) {
    return NULL;
  }
  StepChain* steps = new StepChain(&step, steps_);
  size_t num_steps = num_steps_;
  size_t high_step_id = high_step_id_;
  if (step.id > high_step_id_) {
    num_steps++;
    high_step_id = step.id;
  }
  const Ordering& new_ordering =
    *(new Ordering(step.id, open_cond.step_id, establish_reason));
  const Orderings& orderings = orderings_.refine(new_ordering, &step);
  const UnsafeChain* unsafes = unsafes_;
  size_t num_unsafes = num_unsafes_;
#ifdef DELETE_OBSOLETE_UNSAFES
  unsafes = remove_obsolete_unsafes(unsafes, num_unsafes,
				    orderings, *bindings);
#endif
  hash_set<size_t> seen_steps;
  for (const StepChain* ss = steps_; ss != NULL; ss = ss->tail) {
    const Step& s = *ss->head;
    if (seen_steps.find(s.id) == seen_steps.end() &&
	orderings.possibly_before(link.from_id, s.id) &&
	orderings.possibly_after(link.to_id, s.id)) {
      seen_steps.insert(s.id);
      const EffectList& effects = s.effects;
      for (EffectList::const_iterator e = effects.begin();
	   e != effects.end(); e++) {
	const AtomList& adds = (*e)->add_list;
	for (AtomListIter f = adds.begin(); f != adds.end(); f++) {
	  if (bindings->affects(**f, link.condition)) {
	    unsafes = new UnsafeChain(new Unsafe(link, s.id, **e, **f),
				      unsafes);
	    num_unsafes++;
	  }
	}
	const NegationList& dels = (*e)->del_list;
	for (NegationListIter f = dels.begin(); f != dels.end(); f++) {
	  if (bindings->affects(**f, link.condition)) {
	    unsafes = new UnsafeChain(new Unsafe(link, s.id, **e, **f),
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
	for (EffectList::const_iterator e = effects.begin();
	     e != effects.end(); e++) {
	  const AtomList& adds = (*e)->add_list;
	  for (AtomListIter f = adds.begin(); f != adds.end(); f++) {
	    if (bindings->affects(**f, l.condition)) {
	      unsafes = new UnsafeChain(new Unsafe(l, step.id, **e, **f),
					unsafes);
	      num_unsafes++;
	    }
	  }
	  const NegationList& dels = (*e)->del_list;
	  for (NegationListIter f = dels.begin(); f != dels.end(); f++) {
	    if (bindings->affects(**f, l.condition)) {
	      unsafes = new UnsafeChain(new Unsafe(l, step.id, **e, **f),
					unsafes);
	      num_unsafes++;
	    }
	  }
	}
      }
    }
  }
  return new Plan(steps, num_steps, high_step_id, links, num_links_ + 1,
		  unsafes, num_unsafes, open_conds, num_open_conds,
		  old_open_conds, *bindings, orderings, this, NORMAL_PLAN);
}

void Plan::print(ostream& os) const {
  const Step* init;
  const Step* goal;
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
      os << ' ' << bindings_.instantiation(*(*s)->action);
    }
  } else {
    os << "Initial  :";
    const EffectList& effects = init->effects;
    for (EffectList::const_iterator i = effects.begin();
	 i != effects.end(); i++) {
      const AtomList& fs = (*i)->add_list;
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
      os << " : " << bindings_.instantiation(*(*s)->action);
      for (const LinkChain* links = links_;
	   links != NULL; links = links->tail) {
	if (links->head->to_id == (*s)->id) {
	  os << endl << "           " << links->head->from_id;
	  if (links->head->from_id < 10) {
	    os << ' ';
	  }
	  os << " -> " << bindings_.instantiation(links->head->condition);
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
	     << bindings_.instantiation(oc->head->condition);
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
	os << endl << "           ?? -> " << oc->head->condition;
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
	  steps1[id] = s->head->action;
	} else {
	  steps1[id] = &bindings_.instantiation(*s->head->action);
	}
      }
    }
    FormulaMap steps2;
    for (const StepChain* s = p.steps_; s != NULL; s = s->tail) {
      size_t id = s->head->id;
      if (s->head->action != NULL && steps2.find(id) == steps2.end()) {
	if (params.ground_actions) {
	  steps2[id] = s->head->action;
	} else {
	  steps2[id] = &p.bindings_.instantiation(*s->head->action);
	}
      }
    }
    // try to find mapping between steps so that bindings and orderings match
    hash_map<size_t, size_t> step_map;
    return matching_plans(step_map, steps1, steps2, steps1.begin());
  }
  return false;
}

void Plan::h_rank() const {
  if (rank1_ >= 0) {
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
  if (verbosity > 3) {
    cout << "@@@@cost graph:" << endl << cg << endl;
    cg.cost(goal_node);
    cout << "@@@@cost graph:" << endl << cg << endl;
  }
  pair<int, int> cost = cg.cost(goal_node);
  rank1_ = cost.first;
  rank2_ = cost.second;// + num_open_conds_;
  int high_cost = -1;
  int low_cost = INT_MAX;
  int high_work = -1;
  int low_work = INT_MAX;
  int high_link = -1;
  int low_link = INT_MAX;
  for (const OpenConditionChain* oc = open_conds_; oc != NULL; oc = oc->tail) {
    Cost c = oc->head->condition.cost(atom_cost, params.heuristic);
    if (c.cost > high_cost
	|| (c.cost == high_cost && params.flaw_order.fifo())) {
      most_cost_open_cond_ = oc->head;
      high_cost = c.cost;
    }
    if (c.cost < low_cost
	|| (c.cost == low_cost && params.flaw_order.fifo())) {
      least_cost_open_cond_ = oc->head;
      low_cost = c.cost;
    }
    if (c.work > high_work
	|| (c.work == high_work && params.flaw_order.fifo())) {
      most_work_open_cond_ = oc->head;
      high_work = c.work;
    }
    if (c.work < low_work
	|| (c.work == low_work && params.flaw_order.fifo())) {
      least_work_open_cond_ = oc->head;
      low_work = c.work;
    }
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
  }
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
    } else if (params.heuristic.sum()) {
      step_node = cg.add_sum_node();
    }
    const Step* step = find_step(steps_, step_id);
    assert(step != NULL);
    step_nodes[step->id] = step_node;
    if (verbosity > 3) {
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
	if (verbosity > 3) {
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
	for (EffectList::const_iterator i = effs.begin();
	     i != effs.end(); i++) {
	  const AtomList& adds = (*i)->add_list;
	  for (AtomListIter j = adds.begin(); j != adds.end(); j++) {
	    if (bindings_.unify(**j, cond)) {
	      return 1;
	    }
	  }
	}
      }
    }
  }
  Cost c = cond.cost(atom_cost, params.heuristic);
  if (c.infinite() && !params.ground_actions) {
    const Atom* atom = dynamic_cast<const Atom*>(&cond);
    if (atom != NULL) {
      if (domain->static_predicate(atom->predicate)) {
	c.cost = 0;
	c.work = 1;
      } else {
	c.cost = 0;
	c.work = 0;
	int nc = 0;
	for (hash_map<const Formula*, Cost>::const_iterator ci =
	       atom_cost.begin(); ci != atom_cost.end(); ci++) {
	  if (bindings_.unify(*(*ci).first, cond)) {
#ifdef MAX_COST_LEAST_COMMITMENT
	    if ((*ci).second > c) {
	      c = (*ci).second;
	      nc = 1;
	    }
#else /* average cost */
	    c += (*ci).second;
	    nc++;
#endif
	  }
	}
	if (nc > 0) {
	  c.cost /= nc;
	  c.work /= nc;
	} else {
	  c = Cost::INFINITE;
	}
      }
    }
  }
  size_t cond_node =
    cg.add_node(params.weight*c.cost, params.weight*c.work);
  f_nodes[&cond] = cond_node;
  return cond_node;
}
