/* -*-C++-*- */
/*
 * Partial plans, and their components.
 *
 * $Id: plans.h,v 1.32 2001-12-30 15:15:28 lorens Exp $
 */
#ifndef PLANS_H
#define PLANS_H

#include "support.h"
#include "chain.h"

struct Parameters;
struct SubstitutionList;
struct Formula;
struct Literal;
struct Atom;
struct Effect;
struct EffectList;
struct Action;
struct Problem;
struct Reason;
struct Flaw;
struct OpenCondition;
struct LiteralOpenCondition;
struct InequalityOpenCondition;
struct DisjunctiveOpenCondition;
struct Unsafe;
struct Ordering;
struct Orderings;
struct Bindings;


/*
 * Chain of open conditions.
 */
typedef Chain<const OpenCondition*> OpenConditionChain;


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
  Link(size_t from_id, const LiteralOpenCondition& open_cond);

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

  /* Returns a formula representing this step. */
  const Atom* step_formula() const;

private:
  /* Atomic representation of this step. */
  mutable const Atom* formula;
};


/*
 * Chain of plan steps.
 */
typedef Chain<const Step*> StepChain;


struct PlanList;

/*
 * Plan.
 */
struct Plan : public LessThanComparable, public Printable, public gc {
  /* Id of goal step. */
  static const size_t GOAL_ID;

  /* Depth of this plan in the search space. */
  const size_t depth;

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
       const Bindings& bindings, const Orderings& orderings,
       const Plan* parent, PlanType type = NORMAL_PLAN)
    : depth((parent != NULL) ? parent->depth + 1 : 0),
      steps_(steps), num_steps_(num_steps), high_step_id_(high_id),
      links_(links), num_links_(num_links),
      unsafes_(unsafes), num_unsafes_(num_unsafes),
      open_conds_(open_conds), num_open_conds_(num_open_conds),
      bindings_(bindings), orderings_(orderings),
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
			  const DisjunctiveOpenCondition& open_cond) const;

  void handle_inequality(PlanList& new_plans,
			 const InequalityOpenCondition& open_cond) const;

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


/*
 * List of plans.
 */
struct PlanList : public Vector<const Plan*> {
};

/* Iterator for plan lists. */
typedef PlanList::const_iterator PlanListIter;


#endif /* PLANS_H */
