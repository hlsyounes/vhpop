// Copyright (C) 2002--2005 Carnegie Mellon University
// Copyright (C) 2019 Google Inc
//
// This file is part of VHPOP.
//
// VHPOP is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// VHPOP is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VHPOP; if not, write to the Free Software Foundation,
// Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Partial plans, and their components.

#ifndef PLANS_H
#define PLANS_H

#include "chain.h"
#include "flaws.h"
#include "orderings.h"

struct Parameters;
struct BindingList;
struct Literal;
struct Atom;
struct Negation;
struct Effect;
struct EffectList;
struct Action;
struct Problem;
struct Bindings;
struct ActionEffectMap;
struct FlawSelectionOrder;


/* ====================================================================== */
/* Link */

/*
 * Causal link.
 */
struct Link {
  /* Constructs a causal link. */
  Link(size_t from_id, StepTime effect_time, const OpenCondition& open_cond);

  /* Constructs a causal link. */
  Link(const Link& l);

  /* Deletes this causal link. */
  ~Link();

  /* Returns the id of step that link goes from. */
  size_t from_id() const { return from_id_; }

  /* Returns the time of effect satisfying link. */
  StepTime effect_time() const { return effect_time_; }

  /* Returns the id of step that link goes to. */
  size_t to_id() const { return  to_id_; }

  /* Returns the condition satisfied by link. */
  const Literal& condition() const { return *condition_; }

  /* Returns the time of the condition satisfied by this link. */
  FormulaTime condition_time() const { return condition_time_; }

private:
  /* Id of step that link goes from. */
  size_t from_id_;
  /* Time of effect satisfying link. */
  StepTime effect_time_;
  /* Id of step that link goes to. */
  size_t to_id_;
  /* Condition satisfied by link. */
  const Literal* condition_;
  /* Time of condition satisfied by link. */
  FormulaTime condition_time_;
};

/* Equality operator for links. */
inline bool operator==(const Link& l1, const Link& l2) {
  return &l1 == &l2;
}


/* ====================================================================== */
/* Step */

/*
 * Plan step.
 */
struct Step {
  /* Constructs a step instantiated from an action. */
  Step(size_t id, const Action& action)
    : id_(id), action_(&action) {}

  /* Constructs a step. */
  Step(const Step& s)
    : id_(s.id_), action_(s.action_) {}

  /* Returns the step id. */
  size_t id() const { return id_; }

  /* Returns the action that this step is instantiated from. */
  const Action& action() const { return *action_; }

private:
  /* Step id. */
  size_t id_;
  /* Action that this step is instantiated from. */
  const Action* action_;
};


/* ====================================================================== */
/* Plan */

/*
 * Plan.
 */
struct Plan {
  /* Id of goal step. */
  static const size_t GOAL_ID;

  /* Returns plan for given problem. */
  static const Plan* plan(const Problem& problem, const Parameters& params,
			  bool last_problem);

  /* Cleans up after planning. */
  static void cleanup();

  /* Deletes this plan. */
  ~Plan();

  /* Returns the steps of this plan. */
  const Chain<Step>* steps() const { return steps_; }

  /* Returns the number of unique steps in this plan. */
  size_t num_steps() const { return num_steps_; }
  
  /* Returns the links of this plan. */
  const Chain<Link>* links() const { return links_; }

  /* Returns the number of links in this plan. */
  size_t num_links() const { return num_links_; }

  /* Returns the ordering constraints of this plan. */
  const Orderings& orderings() const { return *orderings_; }

  /* Returns the bindings of this plan. */
  const Bindings* bindings() const;

  /* Returns the potentially threatened links of this plan. */
  const Chain<Unsafe>* unsafes() const { return unsafes_; }

  /* Returns the number of potentially threatened links in this plan. */
  size_t num_unsafes() const { return num_unsafes_; }

  /* Returns the open conditions of this plan. */
  const Chain<OpenCondition>* open_conds() const { return open_conds_; }

  /* Returns the number of open conditions in this plan. */
  size_t num_open_conds() const { return num_open_conds_; }

  /* Returns the mutex threats of this plan. */
  const Chain<MutexThreat>* mutex_threats() const { return mutex_threats_; }

  /* Checks if this plan is complete. */
  bool complete() const;

  /* Returns the primary rank of this plan, where a lower rank
     signifies a better plan. */
  float primary_rank() const;

  /* Returns the serial number of this plan. */
  size_t serial_no() const;

#ifdef DEBUG
  /* Returns the depth of this plan. */
  size_t depth() const { return depth_; }
#endif

  /* Counts the number of refinements for the given threat, and returns
     true iff the number of refinements does not exceed the given
     limit. */
  bool unsafe_refinements(int& refinements, int& separable,
			  int& promotable, int& demotable,
			  const Unsafe& unsafe, int limit) const;

  /* Checks if the given threat is separable. */
  int separable(const Unsafe& unsafe) const;

  /* Checks if the given open condition is threatened. */
  bool unsafe_open_condition(const OpenCondition& open_cond) const;

  /* Counts the number of refinements for the given open condition, and
     returns true iff the number of refinements does not exceed the
     given limit. */
  bool open_cond_refinements(int& refinements, int& addable, int& reusable,
			     const OpenCondition& open_cond, int limit) const;

  /* Counts the number of add-step refinements for the given literal
     open condition, and returns true iff the number of refinements
     does not exceed the given limit. */
  bool addable_steps(int& refinements, const Literal& literal,
		     const OpenCondition& open_cond, int limit) const;

  /* Counts the number of reuse-step refinements for the given literal
     open condition, and returns true iff the number of refinements
     does not exceed the given limit. */
  bool reusable_steps(int& refinements, const Literal& literal,
		      const OpenCondition& open_cond, int limit) const;

private:
  /* List of plans. */
  struct PlanList : public std::vector<const Plan*> {
  };

  /* Chain of steps. */
  const Chain<Step>* steps_;
  /* Number of unique steps in plan. */
  size_t num_steps_;
  /* Chain of causal links. */
  const Chain<Link>* links_;
  /* Number of causal links. */
  size_t num_links_;
  /* Ordering constraints of this plan. */
  const Orderings* orderings_;
  /* Binding constraints of this plan. */
  const Bindings* bindings_;
  /* Chain of potentially threatened links. */
  const Chain<Unsafe>* unsafes_;
  /* Number of potentially threatened links. */
  size_t num_unsafes_;
  /* Chain of open conditions. */
  const Chain<OpenCondition>* open_conds_;
  /* Number of open conditions. */
  const size_t num_open_conds_;
  /* Chain of mutex threats. */
  const Chain<MutexThreat>* mutex_threats_;
  /* Rank of this plan. */
  mutable std::vector<float> rank_;
  /* Plan id (serial number). */
  mutable size_t id_;
#ifdef DEBUG
  /* Depth of this plan in the search space. */
  size_t depth_;
#endif

  /* Returns the initial plan representing the given problem, or NULL
     if goals of problem are inconsistent. */
  static const Plan* make_initial_plan(const Problem& problem);

  /* Constructs a plan. */
  Plan(const Chain<Step>* steps, size_t num_steps,
       const Chain<Link>* links, size_t num_links,
       const Orderings& orderings, const Bindings& bindings,
       const Chain<Unsafe>* unsafes, size_t num_unsafes,
       const Chain<OpenCondition>* open_conds, size_t num_open_conds,
       const Chain<MutexThreat>* mutex_threats, const Plan* parent);

  /* Returns the next flaw to work on. */
  const Flaw& get_flaw(const FlawSelectionOrder& flaw_order) const;

  /* Returns the refinements for the next flaw to work on. */
  void refinements(PlanList& plans,
		   const FlawSelectionOrder& flaw_order) const;

  /* Handles an unsafe link. */
  void handle_unsafe(PlanList& plans, const Unsafe& unsafe) const;

  /* Handles an unsafe link through separation. */
  int separate(PlanList& plans, const Unsafe& unsafe,
		const BindingList& unifier, bool test_only = false) const;

  /* Handles an unsafe link through demotion. */
  int demote(PlanList& plans, const Unsafe& unsafe,
	     bool test_only = false) const;

  /* Handles an unsafe link through promotion. */
  int promote(PlanList& plans, const Unsafe& unsasfe,
	      bool test_only = false) const;

  /* Adds a plan to the given plan list with an ordering added. */
  void new_ordering(PlanList& plans, size_t before_id, StepTime t1,
		    size_t after_id, StepTime t2,
		    const Unsafe& unsafe) const;

  /* Handles a mutex threat. */
  void handle_mutex_threat(PlanList& plans,
			   const MutexThreat& mutex_threat) const;

  /* Handles a mutex threat through separation. */
  void separate(PlanList& plans, const MutexThreat& mutex_threat,
		const BindingList& unifier) const;

  /* Handles a mutex threat through demotion. */
  void demote(PlanList& plans, const MutexThreat& mutex_threat) const;

  /* Handles a mutex threat through promotion. */
  void promote(PlanList& plans, const MutexThreat& mutex_threat) const;

  /* Adds a plan to the given plan list with an ordering added. */
  void new_ordering(PlanList& plans, size_t before_id, StepTime t1,
		    size_t after_id, StepTime t2,
		    const MutexThreat& mutex_threat) const;

  /* Handles an open condition. */
  void handle_open_condition(PlanList& plans,
			     const OpenCondition& open_cond) const;

  /* Handles a disjunctive open condition. */
  int handle_disjunction(PlanList& plans, const Disjunction& disj,
			 const OpenCondition& open_cond,
			 bool test_only = false) const;

  /* Handles an inequality open condition. */
  int handle_inequality(PlanList& plans, const Inequality& neq,
			const OpenCondition& open_cond,
			bool test_only = false) const;

  /* Handles a literal open condition by adding a new step. */
  void add_step(PlanList& plans, const Literal& literal,
		const OpenCondition& open_cond,
		const ActionEffectMap& achievers) const;

  /* Handles a literal open condition by reusing an existing step. */
  void reuse_step(PlanList& plans, const Literal& literal,
		  const OpenCondition& open_cond,
		  const ActionEffectMap& achievers) const;

  /* Adds plans to the given plan list with a link from the given step
     to the given open condition added. */
  int new_link(PlanList& plans, const Step& step, const Effect& effect,
	       const Literal& literal, const OpenCondition& open_cond,
	       bool test_only = false) const;

  /* Adds plans to the given plan list with a link from the given step
     to the given open condition added using the closed world
     assumption. */
  int new_cw_link(PlanList& plans, const EffectList& effects,
		  const Negation& negation, const OpenCondition& open_cond,
		  bool test_only = false) const;

  /* Returns a plan with a link added from the given effect to the
     given open condition. */
  int make_link(PlanList& plans, const Step& step, const Effect& effect,
		const Literal& literal, const OpenCondition& open_cond,
		const BindingList& unifier, bool test_only = false) const;

  friend bool operator<(const Plan& p1, const Plan& p2);
  friend std::ostream& operator<<(std::ostream& os, const Plan& p);
};

/* Less than operator for plans. */
bool operator<(const Plan& p1, const Plan& p2);

/* Output operator for plans. */
std::ostream& operator<<(std::ostream& os, const Plan& p);


#endif /* PLANS_H */
