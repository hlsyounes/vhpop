/* -*-C++-*- */
/*
 * Partial plans, and their components.
 *
 * $Id: plans.h,v 1.18 2001-10-06 00:34:59 lorens Exp $
 */
#ifndef PLANS_H
#define PLANS_H

#include <vector>
#include <stack>
#include <utility>
#include <typeinfo>
#include "support.h"
#include "domains.h"
#include "problems.h"
#include "bindings.h"


struct Reason;


/*
 * Abstract flaw.
 */
struct Flaw : public Printable {
};


/*
 * Open condition.
 */
struct OpenCondition : public Flaw {
  /* Open condition. */
  const Formula& condition;
  /* Id of step to which this open condition belongs. */
  const size_t step_id;
  /* Reason for open condition. */
  const Reason& reason;

  /* Constructs an open condition. */
  OpenCondition(const Formula& condition, size_t step_id, const Reason& reason)
    : condition(condition), step_id(step_id), reason(reason) {
  }

protected:
  /* Prints this open condition on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Chain of open conditions.
 */
typedef Chain<const OpenCondition*> OpenConditionChain;


/*
 * A predicate open condition.
 */
struct PredicateOpenCondition : public OpenCondition {
  /* Predicate. */
  const string& predicate;
  /* Whether the predicate is negated. */
  const bool negated;

  /* Constructs an open condition. */
  PredicateOpenCondition(const Formula& condition, size_t step_id,
			 const Reason& reason, const string& predicate,
			 bool negated = false)
    : OpenCondition(condition, step_id, reason),
      predicate(predicate), negated(negated) {
  }
};


struct Link;

/*
 * Threatened causal link.
 */
struct Unsafe : public Flaw {
  /* Threatened link. */
  const Link& link;
  /* Id of threatening step. */
  const size_t step_id;
  /* Threatening effect. */
  const Effect& effect;
  /* Specific part of effect that threatens link. */
  const Formula& effect_add;

  /* Constructs a threatened causal link. */
  Unsafe(const Link& link, size_t step_id, const Effect& effect,
	 const Formula& effect_add)
    : link(link), step_id(step_id), effect(effect), effect_add(effect_add) {
  }

protected:
  /* Prints this threatened causal link on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Chain of threatened causal links.
 */
typedef Chain<const Unsafe*> UnsafeChain;


/*
 * Causal link.
 */
struct Link : public Printable {
  /* Id of step that link goes from. */
  const size_t from_id;
  /* Id of step that link goes to. */
  const size_t to_id;
  /* Condition satisfied by link. */
  const Formula& condition;
  /* Reason for link. */
  const Reason& reason;

  /* Constructs a causal link. */
  Link(size_t from_id, const OpenCondition& open_cond)
    : from_id(from_id), to_id(open_cond.step_id),
      condition(open_cond.condition), reason(open_cond.reason) {
  }

protected:
  /* Prints this causal link. */
  virtual void print(ostream& os) const;
};


/*
 * Chain of causal links.
 */
typedef Chain<const Link*> LinkChain;


/*
 * Plan step.
 */
struct Step : public gc {
  /* Step id. */
  const size_t id;
  /* Action formula, or NULL if step is not instantiated from an action. */
  const Atom* const action;
  /* Precondition of step, or TRUE if step has no precondition. */
  const Formula& precondition;
  /* List of effects. */
  const EffectList& effects;
  /* Reason for step. */
  const Reason& reason;

  /* Constructs a step. */
  Step(size_t id, const Formula& precondition, const EffectList& effects,
       const Reason& reason)
    : id(id), action(NULL), precondition(precondition.instantiation(id)),
      effects(effects.instantiation(id)), reason(reason) {
  }

  /* Constructs a step instantiated from an action. */
  Step(size_t id, const Action& action, const Reason& reason)
    : id(id), action((typeid(action) == typeid(ActionSchema))
		     ? &action.action_formula().instantiation(id)
		     : &action.action_formula()),
      precondition((typeid(action) == typeid(ActionSchema))
		   ? action.precondition.instantiation(id)
		   : action.precondition),
      effects((typeid(action) == typeid(ActionSchema))
	      ? action.effects.instantiation(id) : action.effects),
      reason(reason) {
  }

  /* Returns a copy of this step with a new reason. */
  const Step& new_reason(const Reason& reason) const {
    return *(new Step(id, action, precondition, effects, reason));
  }

private:
  /* Constructs a step. */
  Step(size_t id, const Atom* action, const Formula& precondition,
       const EffectList& effects, const Reason& reason)
    : id(id), action(action), precondition(precondition), effects(effects),
      reason(reason) {
  }
};


/*
 * Chain of plan steps.
 */
typedef Chain<const Step*> StepChain;


/*
 * Ordering constraint between plan steps.
 */
struct Ordering : public Printable {
  /* Preceeding step. */
  const size_t before_id;
  /* Succeeding step. */
  const size_t after_id;
  /* Reason for ordering constraint. */
  const Reason& reason;

  /* Constructs an ordering constraint. */
  Ordering(size_t before_id, size_t after_id, const Reason& reason)
    : before_id(before_id), after_id(after_id), reason(reason) {
  }

protected:
  /* Prints this ordering constraint on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Chain of ordering constraints.
 */
typedef Chain<const Ordering*> OrderingChain;


/*
 * Collection of ordering constraints.
 */
struct Orderings : public Printable {
  /* Constructs an empty ordering collection. */
  Orderings()
    : orderings_(NULL), size_(0) {
  }

  /* Constructs an ordering collection. */
  Orderings(const StepChain* steps, const OrderingChain* orderings);

  /* Checks if the first step is ordered before the second step. */
  bool before(size_t id1, size_t id2) const;

  /* Checks if the first step is ordered after the second step. */
  bool after(size_t id1, size_t id2) const;

  /* Checks if the first step could be ordered before the second step. */
  bool possibly_before(size_t id1, size_t id2) const;

  /* Checks if the first step could be ordered after the second step. */
  bool possibly_after(size_t id1, size_t id2) const;

  /* Returns the the ordering collection with the given additions. */
  const Orderings& refine(const Ordering& new_ordering,
			  const Step* new_step = NULL) const;

  /* Returns the ordering constraints making up this collection. */
  const OrderingChain* orderings() const {
    return orderings_;
  }

protected:
  /* Prints this ordering collection on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* A step id map. */
  typedef hash_map<size_t, size_t> IdMap;

  /* The ordering constraints making up this collection. */
  const OrderingChain* orderings_;
  /* Number of steps. */
  size_t size_;
  /* Maps step ids to positions in the matrix below. */
  IdMap id_map1_;
  /* Maps positions in the matrix below to step ids */
  vector<size_t> id_map2_;
  /* Matrix representing the transitive closure of the ordering constraints. */
  vector<vector<bool> > order_;

  /* Updates the transitive closure given new ordering constraints. */
  void fill_transitive(const OrderingChain* orderings);

  /* Updates the transitive closure given a new ordering constraint. */
  void fill_transitive(const Ordering& ordering);
};


/*
 * Hash function for open condition pointers.
 */
struct hash<const OpenCondition*> {
  size_t operator()(const OpenCondition* oc) const {
    return h(reinterpret_cast<int>(oc));
  }

private:
  hash<int> h;
};


struct Parameters;
struct CostGraph;

/*
 * Plan.
 */
struct Plan : public Printable {
  /* Id of goal step. */
  static const size_t GOAL_ID;

  /* Returns plan for given problem. */
  static const Plan* plan(const Problem& problem, const Parameters& params,
			  int verbosity);

  /* Checks if this plan is complete. */
  bool complete() const;

  /* Returns the primary rank of this plan, where a lower rank
     signifies a better plan. */
  int primary_rank() const;

  /* Returns the secondary rank of this plan, where a lower rank
     signifies a better plan. */
  int secondary_rank() const;

  /* Returns the tertiary rank of this plan, where a lower rank
     signifies a better plan. */
  int tertiary_rank() const;

  /* Returns the number of open conditions of this plan. */
  size_t num_open_conds() const {
    return num_open_conds_;
  }

protected:
  /* Prints this object on the given stream */
  virtual void print(ostream& os) const;

private:
  /* List of plans. */
  typedef vector<const Plan*, container_alloc> PlanList;
  /* Type of plan. */
  typedef enum { NORMAL_PLAN, INTERMEDIATE_PLAN, TRANSFORMED_PLAN } PlanType;

  /* Chain of steps (could contain same step several times, if it is
     in plan for more than one reason). */
  const StepChain* const steps_;
  /* Number of unique steps in plan. */
  const size_t num_steps_;
  /* Highest step id that has been used so far. */
  const size_t high_step_id_;
  /* Chain of causal links. */
  const LinkChain* const links_;
  /* Number of causal links. */
  const size_t num_links_;
  /* Chain of potentially threatened links. */
  const UnsafeChain* const unsafes_;
  /* Number of potentially threatened links. */
  const size_t num_unsafes_;
  /* Chain of open conditions. */
  const OpenConditionChain* const open_conds_;
  /* Number of open conditions. */
  const size_t num_open_conds_;
  /* Start of old open conditions. */
  const OpenConditionChain* const old_open_conds_;
  /* Binding constraints of this plan. */
  const Bindings& bindings_;
  /* Ordering constraints of this plan. */
  const Orderings& orderings_;
  /* Parent plan. */
  const Plan* const parent_;
  /* Plan type. */
  const PlanType type_;
  /* Rank of this plan. */
  mutable int rank1_;
  mutable int rank2_;
  /* The most costly open condition. */
  mutable const OpenCondition* most_cost_open_cond_;
  /* The least costly open condition. */
  mutable const OpenCondition* least_cost_open_cond_;
  /* The open condition requiring most work. */
  mutable const OpenCondition* most_work_open_cond_;
  /* The open condition requiring least work. */
  mutable const OpenCondition* least_work_open_cond_;
  /* The most linkable open condition. */
  mutable const OpenCondition* most_linkable_open_cond_;
  /* The least linkable open condition. */
  mutable const OpenCondition* least_linkable_open_cond_;
  /* Plan id (for debugging). */
  mutable size_t id;

  /* Returns the initial plan representing the given problem, or NULL
     if goals of problem are inconsistent. */
  static const Plan* make_initial_plan(const Problem& problem);

  /* Constructs a plan. */
  Plan(const StepChain* steps, size_t num_steps, size_t high_id,
       const LinkChain* links, size_t num_links,
       const UnsafeChain* unsafes, size_t num_unsafes,
       const OpenConditionChain* open_conds, size_t num_open_conds,
       const OpenConditionChain* old_open_conds,
       const Bindings& bindings, const Orderings& orderings,
       const Plan* parent, PlanType type = NORMAL_PLAN)
    : steps_(steps), num_steps_(num_steps), high_step_id_(high_id),
      links_(links), num_links_(num_links),
      unsafes_(unsafes), num_unsafes_(num_unsafes),
      open_conds_(open_conds), num_open_conds_(num_open_conds),
      old_open_conds_(old_open_conds),
      bindings_(bindings), orderings_(orderings),
      parent_((parent != NULL && parent->type_ == INTERMEDIATE_PLAN) ?
	      parent->parent_ : parent),
      type_((parent != NULL && parent->type_ == INTERMEDIATE_PLAN) ?
	    TRANSFORMED_PLAN : type),
      rank1_(-1), rank2_(-1),
      most_cost_open_cond_(NULL), least_cost_open_cond_(NULL),
      most_work_open_cond_(NULL), least_work_open_cond_(NULL),
      most_linkable_open_cond_(NULL), least_linkable_open_cond_(NULL) {
  }

  const Flaw& get_flaw() const;

  void refinements(PlanList& new_plans) const;

  void handle_unsafe(PlanList& new_plans, const Unsafe& unsafe) const;

  void separate(PlanList& new_plans, const Unsafe& unsafe) const;

  void demote(PlanList& new_plans, const Unsafe& unsafe) const;

  void promote(PlanList& new_plans, const Unsafe& unsasfe) const;

  void new_ordering(PlanList& new_plans, const Ordering& ordering,
		    const Unsafe& unsafe) const;

  void relink(PlanList& new_plans, const Unsafe& unsafe) const;

  pair<const Plan*, const OpenCondition*> unlink(const Link& link) const;

  void handle_open_condition(PlanList& new_plans,
			     const OpenCondition& open_cond) const;

  void handle_disjunction(PlanList& new_plans,
			  const OpenCondition& open_cond) const;

  void add_step(PlanList& new_plans,
		const PredicateOpenCondition& open_cond) const;

  void link_preconditions(PlanList& new_plans) const;

  void reuse_step(PlanList& new_plans,
		  const PredicateOpenCondition& open_cond) const;

  bool new_link(PlanList& new_plans, const Step& step,
		const OpenCondition& open_cond, const Link& link,
		const Reason& establish_reason) const;

  void new_cw_link(PlanList& new_plans, const Step& step,
		   const OpenCondition& open_cond, const Link& link,
		   const Reason& establish_reason) const;

  const Plan* make_link(const Step& step, const Effect& effect,
			const OpenCondition& open_cond, const Link& link,
			const Reason& establish_reason,
			const SubstitutionList& unifier) const;

  bool duplicate() const;

  bool equivalent(const Plan& p) const;

  void h_rank() const;

  size_t make_node(CostGraph& cg, hash_map<size_t, size_t>& step_nodes,
		   hash_map<const Formula*, size_t>& f_nodes,
		   size_t step_id) const;

  size_t make_node(CostGraph& cg, hash_map<size_t, size_t>& step_nodes,
		   hash_map<const Formula*, size_t>& f_nodes,
		   const Formula& condition, size_t step_id) const;
};

#endif /* PLANS_H */
