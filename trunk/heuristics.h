/* -*-C++-*- */
/*
 * Heuristics.
 *
 * $Id: heuristics.h,v 1.17 2002-01-05 13:51:11 lorens Exp $
 */
#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "support.h"
#include "formulas.h"


struct ActionList;
struct Action;
struct Domain;
struct Problem;
struct Bindings;
struct Flaw;
struct Unsafe;
struct OpenCondition;
struct Plan;


/*
 * A heuristic value.
 */
struct HeuristicValue : public Printable, public gc {
  /* A zero heuristic value. */
  static const HeuristicValue ZERO;
  /* A zero cost, unit work, heuristic value. */
  static const HeuristicValue ZERO_COST_UNIT_WORK;
  /* An infinite heuristic value. */
  static const HeuristicValue INFINITE;

  /* Constructs a zero heuristic value. */
  HeuristicValue();

  /* Constructs a heuristic value. */
  HeuristicValue(int max_cost, int max_work, int sum_cost, int sum_work);

  /* Returns the cost according to the MAX heuristic. */
  int max_cost() const;

  /* Returns the work according to the MAX heuristic. */
  int max_work() const;

  /* Returns the cost according to the SUM heurisitc. */
  int sum_cost() const;

  /* Returns the work according to the SUM heuristic. */
  int sum_work() const;

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

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Cost according to MAX heuristic. */
  int max_cost_;
  /* Work according to MAX heuristic. */
  int max_work_;
  /* Cost according to SUM heuristic. */
  int sum_cost_;
  /* Work according to SUM heuristic. */
  int sum_work_;
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


struct ActionDomain;

/*
 * A planning graph.
 */
struct PlanningGraph : public gc {
  /* Constructs a planning graph. */
  PlanningGraph(const Problem& problem, bool domain_constraints);

  /* Returns the heurisitc value of an atom. */
  HeuristicValue heuristic_value(const Atom& atom,
				 const Bindings* bindings = NULL) const;

  /* Returns the heuristic value of a negated atom. */
  HeuristicValue heuristic_value(const Negation& negation,
				 const Bindings* bindings = NULL) const;

  /* Fills the provided list with actions that achieve the given
     formula. */
  void achieves_formula(ActionList& actions, const Literal& f) const;

  /* Returns the parameter domain for the given action, or NULL if the
     parameter domain is empty. */
  const ActionDomain* action_domain(const string& name) const;

private:
  /* Atom value map. */
  struct AtomValueMap
    : public HashMap<const Atom*, HeuristicValue, hash<const Literal*>,
		     equal_to<const Literal*> > {
  };

  /* Iterator for AtomValueMap. */
  typedef AtomValueMap::const_iterator AtomValueMapIter;

  /* Mapping of literals to actions. */
  struct LiteralActionsMap
    : public HashMultimap<const Literal*, const Action*> {
  };

  /* Iterator for LiteralActionsMap. */
  typedef LiteralActionsMap::const_iterator LiteralActionsMapIter;

  /* Mapping of predicate names to ground atoms. */
  struct PredicateAtomsMap : public HashMultimap<string, const Atom*> {
  };

  /* Iterator for PredicateAtomsMap. */
  typedef PredicateAtomsMap::const_iterator PredicateAtomsMapIter;

  /* Mapping of action name to parameter domain. */
  struct ActionDomainMap : public HashMap<string, ActionDomain*> {
  };

  /* Iterator for ActionDomainMap. */
  typedef ActionDomainMap::const_iterator ActionDomainMapIter;

  /* Atom values. */
  AtomValueMap atom_values;
  /* Negated atom values. */
  AtomValueMap negation_values;
  /* Maps formulas to actions that achieve those formulas. */
  LiteralActionsMap achieves;
  /* Maps predicates to ground atoms. */
  PredicateAtomsMap predicate_atoms;
  /* Maps predicates to negated ground atoms. */
  PredicateAtomsMap predicate_negations;
  /* Maps action names to possible parameter lists. */
  ActionDomainMap action_domains;
};


/*
 * An invalid heuristic exception.
 */
struct InvalidHeuristic : public Exception {
  /* Constructs an invalid heuristic exception. */
  InvalidHeuristic(const string& name);
};


/*
 * Heuristic for ranking plans.
 *
 * LIFO gives priority to plans created later.
 * FIFO gives priority to plans created earlier.
 * OC gives priority to plans with few open conditions.
 * UC gives priority to plans with few threatened links.
 * BUC gives priority to plans with no threatened links.
 * S+OC uses h(p) = |S(p)| + w*|OC(p)|.
 * UCPOP uses h(p) = |S(p)| + w*(|OC(p)| + |UC(p)|.
 * SUM_COST uses the additive cost heuristic.
 * SUM_WORK uses the additive work heuristic.
 * SUM uses h(p) = |S(p)| + w*SUM_COST.
 * SUMR is like SUM, but tries to take reuse into account.
 * MAX is an admissible heuristic counting parallel cost.
 */
struct Heuristic : public gc {
  /* Constructs a heuristic from a name. */
  Heuristic(const string& name = "SUM");

  /* Selects a heuristic from a name. */
  Heuristic& operator=(const string& name);

  /* Checks if this heuristic needs a planning graph. */
  bool needs_planning_graph() const;

  /* Fills the provided vector with the ranks for the given plan. */
  void plan_rank(vector<double>& rank, const Plan& plan,
		 double weight, const Domain& domain,
		 const PlanningGraph* planning_graph) const;

private:
  typedef enum { LIFO, FIFO, OC, UC, BUC, S_PLUS_OC, UCPOP,
		 SUM, SUM_COST, SUM_WORK,
		 SUMR, SUMR_COST, SUMR_WORK,
		 MAX, MAX_COST, MAX_WORK } HVal;

  /* The selected heuristics. */
  vector<HVal> h_;
  /* Whether a planning graph is needed by this heuristic. */
  bool needs_pg_;
};


/*
 * An invalid flaw selection order exception.
 */
struct InvalidFlawSelectionOrder : public Exception {
  /* Constructs an invalid flaw selection order exception. */
  InvalidFlawSelectionOrder(const string& name);
};


/*
 * A selection criterion.
 */
struct SelectionCriterion {
  /* A selection order. */
  typedef enum { LIFO, FIFO, RANDOM, LR, MR, NEW, REUSE } OrderType;

  bool non_separable;
  bool separable;
  bool open_cond;
  bool local_open_cond;
  bool static_open_cond;
  int max_refinements;
  OrderType order;
};


/*
 * Flaw selection order.
 */
struct FlawSelectionOrder : public gc {
  /* Constructs a default flaw selection order. */
  FlawSelectionOrder(const string& name = "UCPOP");

  /* Selects a flaw selection order from a name. */
  FlawSelectionOrder& operator=(const string& name);

  /* Checks if this flaw order needs a planning graph. */
  bool needs_planning_graph() const;

  /* Selects a flaw from the flaws of the given plan. */
  const Flaw& select(const Plan& plan, const Domain& domain,
		     const PlanningGraph* pg) const;

private:
  /* A flaw selection. */
  struct FlawSelection {
    const Flaw* flaw;
    int criterion;
    int rank;
    int streak;
  };

  /* Selection criteria. */
  vector<SelectionCriterion> selection_criteria_;
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
		    int first_criterion, int last_criterion) const;

  /* Seaches open conditions for a flaw to select. */
  int select_open_cond(FlawSelection& selection, const Plan& plan,
		       const Domain& domain, const PlanningGraph* pg,
		       int first_criterion, int last_criterion) const;
};


#endif /* HEURISTICS_H */
