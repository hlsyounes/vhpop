/* -*-C++-*- */
/*
 * Partial plans, and their components.
 *
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
 * $Id: plans.h,v 3.5 2002-03-21 22:49:51 lorens Exp $
 */
#ifndef PLANS_H
#define PLANS_H

#include "support.h"
#include "chain.h"
#include "flaws.h"
#include "orderings.h"

struct Parameters;
struct SubstitutionList;
struct Literal;
struct Atom;
struct Negation;
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
struct Bindings;


/* ====================================================================== */
/* Link */

/*
 * Causal link.
 */
struct Link {
  /* Constructs a causal link. */
  Link(size_t from_id, StepTime effect_time,
       const LiteralOpenCondition& open_cond);

  /* Returns the id of step that link goes from. */
  size_t from_id() const { return from_id_; }

  /* Returns the time of effect satisfying link. */
  StepTime effect_time() const { return effect_time_; }

  /* Returns the id of step that link goes to. */
  size_t to_id() const { return  to_id_; }

  /* Returns the condition satisfied by link. */
  const Literal& condition() const { return *condition_; }

  /* Returns the reason for the link. */
  const Reason& reason() const;

private:
  /* Id of step that link goes from. */
  size_t from_id_;
  /* Time of effect satisfying link. */
  StepTime effect_time_;
  /* Id of step that link goes to. */
  size_t to_id_;
  /* Condition satisfied by link. */
  const Literal* condition_;
#ifdef TRANSFORMATIONAL
  /* Reason for link. */
  const Reason* reason_;
#endif
};


/* ====================================================================== */
/* LinkChain */

/*
 * Chain of causal links.
 */
typedef Chain<Link> LinkChain;


/* ====================================================================== */
/* Step */

/*
 * Plan step.
 */
struct Step {
  /* Constructs a step. */
  Step(size_t id, const EffectList& effects, const Reason& reason);

  /* Constructs a step instantiated from an action. */
  Step(size_t id, const Action& action, const Reason& reason);

  /* Returns the step id. */
  size_t id() const { return id_; }

  /* Returns the action that this step was instantiated from, or NULL
     if step was not instantiated from an action. */
  const Action* action() const { return action_; }

  /* Returns the effects of this step. */
  const EffectList& effects() const { return *effects_; }

  /* Returns the reasons. */
  const Reason& reason() const;

  /* Sets the reason for this step. */
  void set_reason(const Reason& reason);

  /* Returns a formula representing this step. */
  const Atom* step_formula() const;

private:
  /* Step id. */
  size_t id_;
  /* Action, or NULL if step is not instantiated from an action. */
  const Action* action_;
  /* List of effects. */
  const EffectList* effects_;
#ifdef TRANSFORMATIONAL
  /* Reason for step. */
  const Reason* reason_;
#endif
};


/* ====================================================================== */
/* StepChain */

/*
 * Chain of plan steps.
 */
typedef Chain<Step> StepChain;


/* ====================================================================== */
/* Plan */

/*
 * Plan.
 */
struct Plan {
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
  /* Ordering constraints of this plan. */
  const Orderings& orderings;

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

  /* Checks if the given threat is separable. */
  int separable(const Unsafe& unsafe) const;

  /* Checks if the given threat is demotable. */
  int demotable(const Unsafe& unsafe) const;

  /* Checks if the given threat is promotable. */
  int promotable(const Unsafe& unsafe) const;

  /* Checks if the given open conditions is threatened. */
  bool unsafe_open_condition(const OpenCondition& open_cond) const;

  /* Counts the number of refinements for the given disjunctive open
     condition. */
  int disjunction_refinements(const DisjunctiveOpenCondition& open_cond) const;

  /* Counts the number of refinements for the given inequality open
     condition. */
  int inequality_refinements(const InequalityOpenCondition& open_cond) const;

  /* Counts the number of add-step refinements for the given literal
     open condition, and returns true iff the number of refinements
     does not exceed the given limit. */
  bool addable_steps(int& refinements,
		     const LiteralOpenCondition& open_cond, int limit) const;

  /* Counts the number of reuse-step refinements for the given literal
     open condition, and returns true iff the number of refinements
     does not exceed the given limit. */
  bool reusable_steps(int& refinements,
		      const LiteralOpenCondition& open_cond, int limit) const;

private:
  /* List of plans. */
  struct PlanList : public vector<const Plan*> {
  };

  /* Iterator for plan lists. */
  typedef PlanList::const_iterator PlanListIter;

  /* Type of plan. */
  typedef enum { NORMAL_PLAN, INTERMEDIATE_PLAN, TRANSFORMED_PLAN } PlanType;

  /* Highest step id that has been used so far. */
  size_t high_step_id_;
  /* Binding constraints of this plan. */
  const Bindings& bindings_;
  /* Rank of this plan. */
  mutable vector<double> rank_;
  /* Plan id (serial number). */
  mutable size_t id_;
#ifdef TRANSFORMATIONAL
  /* Parent plan. */
  const Plan* const parent_;
  /* Plan type. */
  const PlanType type_;
#endif

  /* Returns the initial plan representing the given problem, or NULL
     if goals of problem are inconsistent. */
  static const Plan* make_initial_plan(const Problem& problem);

  /* Constructs a plan. */
  Plan(const StepChain* steps, size_t num_steps,
       const LinkChain* links, size_t num_links,
       const UnsafeChain* unsafes, size_t num_unsafes,
       const OpenConditionChain* open_conds, size_t num_open_conds,
       const Orderings& orderings, const Bindings& bindings,
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
  void new_ordering(PlanList& plans, size_t before_id, StepTime t1,
		    size_t after_id, StepTime t2,
		    const Unsafe& unsafe) const;

  /* Handles an open condition. */
  void handle_open_condition(PlanList& plans,
			     const OpenCondition& open_cond) const;

  /* Handles a disjunctive open condition. */
  void handle_disjunction(PlanList& plans,
			  const DisjunctiveOpenCondition& open_cond) const;

  /* Handles an inequality open condition. */
  void handle_inequality(PlanList& plans,
			 const InequalityOpenCondition& open_cond) const;

  /* Handles a literal open condition by adding a new step. */
  void add_step(PlanList& plans,
		const LiteralOpenCondition& open_cond) const;

  /* Handles a literal open condition by reusing an existing step. */
  void reuse_step(PlanList& plans,
		  const LiteralOpenCondition& open_cond) const;

  /* Counts the number of new links that can be established between
     the given effects and the open condition, and returns true iff
     the number of refinements does not exceed the given limit. */
  bool count_new_links(int& count, size_t step_id, const Action& action,
		       const EffectList& effects,
		       const LiteralOpenCondition& open_cond,
		       int limit) const;

  /* Adds plans to the given plan list with a link from the given step
     to the given open condition added. */
  void new_link(PlanList& plans, const Step& step,
		const LiteralOpenCondition& open_cond) const;

  /* Checks if a new link be established between the given effects and
     the open condition using the closed world assumption. */
  int cw_link_possible(const EffectList& effects,
		       const Negation& negation) const;

  /* Adds plans to the given plan list with a link from the given step
     to the given open condition added using the closed world
     assumption. */
  void new_cw_link(PlanList& plans, const Step& step,
		   const LiteralOpenCondition& open_cond) const;

  /* Checks if a new link can be established between the given effect
     and the open condition. */
  int link_possible(size_t step_id, const Action& action,
		    const Effect& effect,
		    const LiteralOpenCondition& open_cond,
		    const SubstitutionList& unifier) const;

  /* Returns a plan with a link added from the given effect to the
     given open condition. */
  const Plan* make_link(const Step& step, const Effect& effect,
			const LiteralOpenCondition& open_cond,
			const LinkChain* new_links, const Reason& reason,
			const SubstitutionList& unifier) const;

  /* Adds plans to the given plan list with the given link removed and
     the resulting open condition relinked. */
  void relink(PlanList& new_plans, const Link& link) const;

  /* Adds plans to the given plan list with the given link removed and
     the resulting open condition relinked. */
  pair<const Plan*, const OpenCondition*> unlink(const Link& link) const;

  /* Checks if this plan is a duplicate of a previous plan. */
  bool duplicate() const;

  /* Checks if this plan is equivalent to the given plan. */
  bool equivalent(const Plan& p) const;

  friend bool operator<(const Plan& p1, const Plan& p2);
};

/* Less than operator for plans. */
bool operator<(const Plan& p1, const Plan& p2);

/* Output operator for plans. */
ostream& operator<<(ostream& os, const Plan& p);


#endif /* PLANS_H */
