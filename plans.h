/* -*-C++-*- */
/*
 * Partial plans, and their components.
 *
 * $Id: plans.h,v 1.34 2002-01-01 17:59:56 lorens Exp $
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
  /* Chain of steps (could contain same step several times, if it is
     in plan for more than one reason). */
  const StepChain* const steps;
  /* Number of unique steps in plan. */
  const size_t num_steps;
  /* Chain of causal links. */
  const LinkChain* const links;
  /* Number of causal links. */
  const size_t num_links;
  /* Chain of potentially threatened links. */
  const UnsafeChain* const unsafes;
  /* Number of potentially threatened links. */
  const size_t num_unsafes;
  /* Chain of open conditions. */
  const OpenConditionChain* const open_conds;
  /* Number of open conditions. */
  const size_t num_open_conds;

  /* Returns plan for given problem. */
  static const Plan* plan(const Problem& problem, const Parameters& params);

  /* Checks if this plan is complete. */
  bool complete() const;

  /* Returns the bindings of this plan. */
  const Bindings* bindings() const;

  /* Returns the primary rank of this plan, where a lower rank
     signifies a better plan. */
  double primary_rank() const;

  /* Returns the serial number of this plan. */
  size_t serial_no() const;

  /* Fills the provided list with possible refinements that repair the
     given flaw. */
  void refinements(PlanList& plans, const Flaw& flaw) const;

protected:
  /* Checks if this object is less than the given object. */
  virtual bool less(const LessThanComparable& o) const;

  /* Prints this object on the given stream */
  virtual void print(ostream& os) const;

private:
  /* Type of plan. */
  typedef enum { NORMAL_PLAN, INTERMEDIATE_PLAN, TRANSFORMED_PLAN } PlanType;

  /* Highest step id that has been used so far. */
  size_t high_step_id_;
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
  mutable size_t id_;

  /* Returns the initial plan representing the given problem, or NULL
     if goals of problem are inconsistent. */
  static const Plan* make_initial_plan(const Problem& problem);

  /* Constructs a plan. */
  Plan(const StepChain* steps, size_t num_steps,
       const LinkChain* links, size_t num_links,
       const UnsafeChain* unsafes, size_t num_unsafes,
       const OpenConditionChain* open_conds, size_t num_open_conds,
       const Bindings& bindings, const Orderings& orderings,
       const Plan* parent, PlanType type = NORMAL_PLAN);

  /* Returns the next flaw to work on. */
  const Flaw& get_flaw() const;

  /* Returns the refinements for the next flaw to work on. */
  void refinements(PlanList& plans) const;

  /* Handles an unsafe link. */
  void handle_unsafe(PlanList& plans, const Unsafe& unsafe) const;

  /* Handles an unsafe link through separation. */
  void separate(PlanList& new_plans, const Unsafe& unsafe) const;

  /* Handles an unsafe link through demotion. */
  void demote(PlanList& new_plans, const Unsafe& unsafe) const;

  /* Handles an unsafe link through promotion. */
  void promote(PlanList& new_plans, const Unsafe& unsasfe) const;

  /* Adds a plan to the given plan list with an ordering added. */
  void new_ordering(PlanList& plans, size_t before_id, size_t after_id,
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
