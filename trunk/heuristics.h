/* -*-C++-*- */
/*
 * Heuristics.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by H�kan L. S. Younes.
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
 * $Id: heuristics.h,v 6.2 2003-07-21 02:21:57 lorens Exp $
 */
#ifndef HEURISTICS_H
#define HEURISTICS_H

#include <config.h>
#include "domains.h"
#include "formulas.h"
#include "exceptions.h"

struct Action;
struct ActionList;
struct Problem;
struct ActionDomain;
struct Bindings;
struct Flaw;
struct Unsafe;
struct OpenCondition;
struct Plan;


/* ====================================================================== */
/* HeuristicValue */

/*
 * A heuristic value.
 */
struct HeuristicValue {
  /* A zero heuristic value. */
  static const HeuristicValue ZERO;
  /* A zero cost, unit work, heuristic value. */
  static const HeuristicValue ZERO_COST_UNIT_WORK;
  /* An infinite heuristic value. */
  static const HeuristicValue INFINITE;

  /* Constructs a zero heuristic value. */
  HeuristicValue();

  /* Constructs a heuristic value. */
  HeuristicValue(int max_cost, int max_work, int add_cost, int add_work);

  /* Returns the cost according to the max heuristic. */
  int max_cost() const { return max_cost_; }

  /* Returns the work according to the max heuristic. */
  int max_work() const { return max_work_; }

  /* Returns the cost according to the additive heurisitc. */
  int add_cost() const { return add_cost_; }

  /* Returns the work according to the additive heuristic. */
  int add_work() const { return add_work_; }

  /* Checks if this heuristic value is zero. */
  bool zero() const;

  /* Checks if this heuristic value is infinite. */
  bool infinite() const;

  /* Adds the given heuristic value to this heuristic value. */
  HeuristicValue& operator+=(const HeuristicValue& v);

  /* Adds the given cost to this heuristic value. */
  void add_cost(int c);

  /* Adds the given work to this heuristic value. */
  void add_work(int w);

private:
  /* Cost according to max heuristic. */
  int max_cost_;
  /* Work according to max heuristic. */
  int max_work_;
  /* Cost according to additive heuristic. */
  int add_cost_;
  /* Work according to additive heuristic. */
  int add_work_;
};

/* Equality operator for heuristic values. */
bool operator==(const HeuristicValue& v1, const HeuristicValue& v2);

/* Inequality operator for heuristic values. */
bool operator!=(const HeuristicValue& v1, const HeuristicValue& v2);

/* Less than operator for heuristic values. */
bool operator<(const HeuristicValue& v1, const HeuristicValue& v2);

/* Greater than operator for heuristic values. */
bool operator>(const HeuristicValue& v1, const HeuristicValue& v2);

/* Less than or equal to operator for heuristic values. */
bool operator<=(const HeuristicValue& v1, const HeuristicValue& v2);

/* Greater than or equal to operator for heuristic values. */
bool operator>=(const HeuristicValue& v1, const HeuristicValue& v2);

/* Returns the componentwise minimum heuristic value, given two
   heuristic values. */
HeuristicValue min(const HeuristicValue& v1, const HeuristicValue& v2);

/* Output operator for heuristic values. */
std::ostream& operator<<(std::ostream& os, const HeuristicValue& v);


/* ====================================================================== */
/* PlanningGraph */

/*
 * A planning graph.
 */
struct PlanningGraph {
  /* Constructs a planning graph. */
  PlanningGraph(const Problem& problem, bool domain_constraints);

  /* Deletes this planning graph. */
  ~PlanningGraph();

  /* Returns the heurisitc value of an atom. */
  HeuristicValue heuristic_value(const Atom& atom, size_t step_id,
				 const Bindings* bindings = NULL) const;

  /* Returns the heuristic value of a negated atom. */
  HeuristicValue heuristic_value(const Negation& negation, size_t step_id,
				 const Bindings* bindings = NULL) const;

  /* Fills the provided list with actions that achieve the given
     formula. */
  void achieves_formula(ActionList& actions, const Literal& f) const;

  /* Returns the parameter domain for the given action, or NULL if the
     parameter domain is empty. */
  const ActionDomain* action_domain(const std::string& name) const;

private:
  /* Atom value map. */
  struct AtomValueMap : public hashing::hash_map<const Atom*, HeuristicValue> {
  };

  /* Iterator for AtomValueMap. */
  typedef AtomValueMap::const_iterator AtomValueMapIter;

  /* Mapping of literals to actions. */
  struct LiteralActionsMap
    : public hashing::hash_multimap<const Literal*, const GroundAction*> {
  };

  /* Iterator for LiteralActionsMap. */
  typedef LiteralActionsMap::const_iterator LiteralActionsMapIter;

  /* Mapping of predicate names to ground atoms. */
  struct PredicateAtomsMap
    : public hashing::hash_multimap<Predicate, const Atom*> {
  };

  /* Iterator for PredicateAtomsMap. */
  typedef PredicateAtomsMap::const_iterator PredicateAtomsMapIter;

  /* Mapping of action name to parameter domain. */
  struct ActionDomainMap : public std::map<std::string, ActionDomain*> {
  };

  /* Iterator for ActionDomainMap. */
  typedef ActionDomainMap::const_iterator ActionDomainMapIter;

  /* Atom values. */
  AtomValueMap atom_values_;
  /* Negated atom values. */
  AtomValueMap negation_values_;
  /* Maps formulas to actions that achieve those formulas. */
  LiteralActionsMap achieves_;
  /* Maps predicates to ground atoms. */
  PredicateAtomsMap predicate_atoms_;
  /* Maps predicates to negated ground atoms. */
  PredicateAtomsMap predicate_negations_;
  /* Maps action names to possible parameter lists. */
  ActionDomainMap action_domains_;

  /* Finds an element in a LiteralActionsMap. */
  LiteralActionsMapIter find(const LiteralActionsMap& m,
			     const Literal &l, const Action& a) const;
};


/* ====================================================================== */
/* InvalidHeuristic */

/*
 * An invalid heuristic exception.
 */
struct InvalidHeuristic : public Exception {
  /* Constructs an invalid heuristic exception. */
  InvalidHeuristic(const std::string& name);
};


/* ====================================================================== */
/* Heuristic */

/*
 * Heuristic for ranking plans.
 *
 * LIFO gives priority to plans created later.
 * FIFO gives priority to plans created earlier.
 * OC gives priority to plans with few open conditions.
 * UC gives priority to plans with few threatened links.
 * BUC gives priority to plans with no threatened links.
 * S+OC uses h(p) = |S(p)| + w*|OC(p)|.
 * UCPOP uses h(p) = |S(p)| + w*(|OC(p)| + |UC(p)|).
 * ADD_COST uses the additive cost heuristic.
 * ADD_WORK uses the additive work heuristic.
 * ADD uses h(p) = |S(p)| + w*ADD_COST.
 * ADDR is like ADD, but tries to take reuse into account.
 * MAX is an admissible heuristic counting parallel cost.
 * MAXR is like MAX, but thries to take reuse into account.
 */
struct Heuristic {
  /* Constructs a heuristic from a name. */
  Heuristic(const std::string& name = "UCPOP");

  /* Selects a heuristic from a name. */
  Heuristic& operator=(const std::string& name);

  /* Checks if this heuristic needs a planning graph. */
  bool needs_planning_graph() const;

  /* Fills the provided vector with the ranks for the given plan. */
  void plan_rank(std::vector<float>& rank, const Plan& plan,
		 float weight, const Domain& domain,
		 const PlanningGraph* planning_graph) const;

private:
  /* Heuristics. */
  typedef enum { LIFO, FIFO, OC, UC, BUC, S_PLUS_OC, UCPOP,
		 ADD, ADD_COST, ADD_WORK,
		 ADDR, ADDR_COST, ADDR_WORK,
		 MAX, MAX_COST, MAX_WORK,
		 MAXR, MAXR_COST, MAXR_WORK } HVal;

  /* The selected heuristics. */
  std::vector<HVal> h_;
  /* Whether a planning graph is needed by this heuristic. */
  bool needs_pg_;
};


/* ====================================================================== */
/* InvalidFlawSelectionOrder */

/*
 * An invalid flaw selection order exception.
 */
struct InvalidFlawSelectionOrder : public Exception {
  /* Constructs an invalid flaw selection order exception. */
  InvalidFlawSelectionOrder(const std::string& name);
};


/* ====================================================================== */
/* SelectionCriterion */

/*
 * A selection criterion.
 *
 * The specification of a selection criterion more or less follows the
 * notation of (Pollack, Joslin, and Paolucci 1997).  Their LC is here
 * called LR (least refinements) because we use LC to denote least
 * heuristic cost.  While not mentioned by Pollack et al., we have
 * implemented MR and REUSE for completeness.  These are the opposites
 * of LR and NEW, respectively.  We define four completely new
 * tie-breaking strategies based on heuristic evaluation functions.
 * These are LC (least cost), MC (most cost), LW (least work), and MW
 * (most work).  It is required that a heuristic is specified in
 * conjunction with the use of any of these four strategies.  Finally,
 * we introduce three new flaw types.  These are 't' for static open
 * conditions, 'u' for unsafe open conditions, and 'l' for local open
 * conditions.  All three select subsets of 'o', so {t,o}, {u,o}, and
 * {t,o} reduce to {o}.
 */
struct SelectionCriterion {
  /* A selection order. */
  typedef enum { LIFO, FIFO, RANDOM, LR, MR,
		 NEW, REUSE, LC, MC, LW, MW } OrderType;
  /* A heuristic. */
  typedef enum { ADD, MAX } RankHeuristic;

  /* Whether this criterion applies to non-separable threats. */
  bool non_separable;
  /* Whether this criterion applies to separable threats. */
  bool separable;
  /* Whether this criterion applies to open conditions. */
  bool open_cond;
  /* Whether this criterion applies to local open conditions. */
  bool local_open_cond;
  /* Whether this criterion applies to static open conditions. */
  bool static_open_cond;
  /* Whether this criterion applies to unsafe open conditions. */
  bool unsafe_open_cond;
  /* The maximum number of refinements allowed for a flaw that this
     criterion applies to. */
  int max_refinements;
  /* The ordering criterion. */
  OrderType order;
  /* Heuristic used to rank open conditions (if applicable). */
  RankHeuristic heuristic;
  /* Whether the above heuristic should take reuse into account. */
  bool reuse;
};


/* ====================================================================== */
/* FlawSelectionOrder */

/*
 * Flaw selection order.
 *
 * This is basically a list of selection criteria.
 */
struct FlawSelectionOrder {
  /* Constructs a default flaw selection order. */
  FlawSelectionOrder(const std::string& name = "UCPOP");

  /* Selects a flaw selection order from a name. */
  FlawSelectionOrder& operator=(const std::string& name);

  /* Checks if this flaw order needs a planning graph. */
  bool needs_planning_graph() const;

  /* Selects a flaw from the flaws of the given plan. */
  const Flaw& select(const Plan& plan, const Problem& problem,
		     const PlanningGraph* pg) const;

private:
  /* A flaw selection. */
  struct FlawSelection {
    /* The selected flaw. */
    const Flaw* flaw;
    /* Index of criterion used to select this flaw. */
    int criterion;
    /* Rank of this flaw if selected by a ranking criterion. */
    int rank;
    /* Counts the length of a streak, for use with random order. */
    int streak;
  };

  /* Selection criteria. */
  std::vector<SelectionCriterion> selection_criteria_;
  /* Whether a planning graph is needed by this flaw selection order. */
  bool needs_pg_;
  /* Index of the first selection criterion involving threats. */
  int first_unsafe_criterion_;
  /* Index of the last selection criterion involving threats. */
  int last_unsafe_criterion_;
  /* Index of the first selection criterion involving open conditions. */
  int first_open_cond_criterion_;
  /* Index of the last selection criterion involving open conditions. */
  int last_open_cond_criterion_;

  /* Seaches threats for a flaw to select. */
  int select_unsafe(FlawSelection& selection, const Plan& plan,
		    const Problem& problem,
		    int first_criterion, int last_criterion) const;

  /* Seaches open conditions for a flaw to select. */
  int select_open_cond(FlawSelection& selection, const Plan& plan,
		       const Problem& problem, const PlanningGraph* pg,
		       int first_criterion, int last_criterion) const;
};


#endif /* HEURISTICS_H */
