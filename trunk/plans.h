/* -*-C++-*- */
/*
 * Partial plans, and their components.
 *
 * $Id: plans.h,v 1.27 2001-12-26 18:51:38 lorens Exp $
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


struct Link;
struct Step;

/*
 * Abstract reason.
 */
struct Reason : public Printable, public gc {
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
 * Abstract flaw.
 */
struct Flaw : public Printable, public gc {
};


/*
 * Open condition.
 */
struct OpenCondition : public Flaw {
  /* Id of step to which this open condition belongs. */
  const size_t step_id;
  /* Reason for open condition. */
  const Reason& reason;

  /* Constructs an open condition. */
  OpenCondition(size_t step_id, const Reason& reason);

  /* Returns the open condition. */
  virtual const Formula& condition() const = 0;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const = 0;

protected:
  /* Prints this open condition on the given stream. */
  virtual void print(ostream& os) const;
};


/*
 * Chain of open conditions.
 */
typedef Chain<const OpenCondition*> OpenConditionChain;


/*
 * A literal open condition.
 */
struct LiteralOpenCondition : public OpenCondition {
  /* Literal. */
  const Literal& literal;

  /* Constructs a literal open condition. */
  LiteralOpenCondition(const Literal& condition, size_t step_id,
		       const Reason& reason);

  /* Returns the open condition. */
  virtual const Formula& condition() const;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const;
};


/*
 * A formula open condition.
 */
struct FormulaOpenCondition : public OpenCondition {
  /* Constructs a formula open condition. */
  FormulaOpenCondition(const Formula& condition, size_t step_id,
		       const Reason& reason);

  /* Returns the open condition. */
  virtual const Formula& condition() const;

  /* Checks if this is a static open condition. */
  virtual bool is_static(const Domain& domain) const;

private:
  /* The open condition. */
  const Formula& condition_;
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
  const Literal& effect_add;

  /* Constructs a threatened causal link. */
  Unsafe(const Link& link, size_t step_id, const Effect& effect,
	 const Literal& effect_add)
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
struct Link : public Printable, public gc {
  /* Id of step that link goes from. */
  const size_t from_id;
  /* Id of step that link goes to. */
  const size_t to_id;
  /* Condition satisfied by link. */
  const Literal& condition;
  /* Reason for link. */
  const Reason& reason;

  /* Constructs a causal link. */
  Link(size_t from_id, const LiteralOpenCondition& open_cond)
    : from_id(from_id), to_id(open_cond.step_id),
      condition(open_cond.literal), reason(open_cond.reason) {
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
  /* Action, or NULL if step is not instantiated from an action. */
  const Action* const action;
  /* Precondition of step, or TRUE if step has no precondition. */
  const Formula& precondition;
  /* List of effects. */
  const EffectList& effects;
  /* Reason for step. */
  const Reason& reason;

  /* Constructs a step. */
  Step(size_t id, const Formula& precondition, const EffectList& effects,
       const Reason& reason);

  /* Constructs a step instantiated from an action. */
  Step(size_t id, const Action& action, const Reason& reason);

  /* Returns a copy of this step with a new reason. */
  const Step& new_reason(const Reason& reason) const {
    return *(new Step(id, action, precondition, effects, reason, formula));
  }

  /* Returns a formula representing this step. */
  const Atom* step_formula() const;

private:
  /* Constructs a step. */
  Step(size_t id, const Action* action, const Formula& precondition,
       const EffectList& effects, const Reason& reason, const Atom* formula)
    : id(id), action(action), precondition(precondition), effects(effects),
      reason(reason), formula(formula) {
  }

  /* Atomic representation of this step. */
  mutable const Atom* formula;
};


/*
 * Chain of plan steps.
 */
typedef Chain<const Step*> StepChain;


/*
 * Ordering constraint between plan steps.
 */
struct Ordering : public Printable, public gc {
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
struct Orderings : public Printable, public gc {
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
struct Plan : public LessThanComparable, public Printable, public gc {
  /* Id of goal step. */
  static const size_t GOAL_ID;

  /* Returns plan for given problem. */
  static const Plan* plan(const Problem& problem, const Parameters& params);

  /* Checks if this plan is complete. */
  bool complete() const;

  /* Returns the serial number of this plan. */
  size_t serial_no() const;

  /* Returns the number of steps of this plan. */
  size_t num_steps() const;

  /* Returns the open conditions of this plan. */
  const OpenConditionChain* open_conds() const;

  /* Returns the number of open conditions of this plan. */
  size_t num_open_conds() const;

  /* Returns the number of unsafe links of this plan. */
  size_t num_unsafes() const;

  /* Returns the bindings of this plan. */
  const Bindings* bindings() const;

protected:
  /* Checks if this object is less than the given object. */
  virtual bool less(const LessThanComparable& o) const;

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
  /* Number of static open conditions. */
  const size_t num_static_;
  /* Binding constraints of this plan. */
  const Bindings& bindings_;
  /* Ordering constraints of this plan. */
  const Orderings& orderings_;
  /* Parent plan. */
  const Plan* const parent_;
  /* Plan type. */
  const PlanType type_;
  /* Rank of this plan. */
  mutable vector<double> rank_;
  /* Plan id (serial number). */
  mutable size_t id;

  /* Returns the initial plan representing the given problem, or NULL
     if goals of problem are inconsistent. */
  static const Plan* make_initial_plan(const Problem& problem);

  /* Constructs a plan. */
  Plan(const StepChain* steps, size_t num_steps, size_t high_id,
       const LinkChain* links, size_t num_links,
       const UnsafeChain* unsafes, size_t num_unsafes,
       const OpenConditionChain* open_conds, size_t num_open_conds,
       size_t num_static, const Bindings& bindings, const Orderings& orderings,
       const Plan* parent, PlanType type = NORMAL_PLAN)
    : steps_(steps), num_steps_(num_steps), high_step_id_(high_id),
      links_(links), num_links_(num_links),
      unsafes_(unsafes), num_unsafes_(num_unsafes),
      open_conds_(open_conds), num_open_conds_(num_open_conds),
      num_static_(num_static), bindings_(bindings), orderings_(orderings),
#if 0
      parent_((parent != NULL && parent->type_ == INTERMEDIATE_PLAN) ?
	      parent->parent_ : parent),
#else
      parent_(NULL),
#endif
      type_((parent != NULL && parent->type_ == INTERMEDIATE_PLAN) ?
	    TRANSFORMED_PLAN : type) {}

  /* Returns the primary rank of this plan, where a lower rank
     signifies a better plan. */
  double primary_rank() const;

  const Flaw& get_flaw() const;

  void refinements(PlanList& new_plans) const;

  void handle_unsafe(PlanList& new_plans, const Unsafe& unsafe) const;

  void separate(PlanList& new_plans, const Unsafe& unsafe) const;

  void demote(PlanList& new_plans, const Unsafe& unsafe) const;

  void promote(PlanList& new_plans, const Unsafe& unsasfe) const;

  void new_ordering(PlanList& new_plans, const Ordering& ordering,
		    const Unsafe& unsafe) const;

  void relink(PlanList& new_plans, const Link& link) const;

  pair<const Plan*, const OpenCondition*> unlink(const Link& link) const;

  void handle_open_condition(PlanList& new_plans,
			     const OpenCondition& open_cond) const;

  void handle_disjunction(PlanList& new_plans,
			  const OpenCondition& open_cond) const;

  void handle_inequality(PlanList& new_plans,
			 const OpenCondition& open_cond) const;

  void add_step(PlanList& new_plans,
		const LiteralOpenCondition& open_cond) const;

  void link_preconditions(PlanList& new_plans) const;

  void reuse_step(PlanList& new_plans,
		  const LiteralOpenCondition& open_cond) const;

  bool new_link(PlanList& new_plans, const Step& step,
		const LiteralOpenCondition& open_cond, const Link& link,
		const Reason& establish_reason) const;

  void new_cw_link(PlanList& new_plans, const Step& step,
		   const LiteralOpenCondition& open_cond, const Link& link,
		   const Reason& establish_reason) const;

  const Plan* make_link(const Step& step, const Effect& effect,
			const LiteralOpenCondition& open_cond,
			const Link& link, const Reason& establish_reason,
			const SubstitutionList& unifier) const;

  bool duplicate() const;

  bool equivalent(const Plan& p) const;
};

#endif /* PLANS_H */
