/* -*-C++-*- */
/*
 * Heuristics.
 *
 * $Id: heuristics.h,v 1.7 2001-10-16 19:31:35 lorens Exp $
 */
#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "support.h"
#include "chain.h"


struct Formula;
struct Atom;
struct Negation;
struct ActionList;
struct Action;
struct Problem;
struct Bindings;
struct OpenCondition;


/*
 * An invalid heuristic exception.
 */
struct InvalidHeuristic : public Exception {
  /* Constructs an invalid heuristic exception. */
  InvalidHeuristic(const string& name)
    : Exception("invalid heuristic `" + name + "'") {
  }
};


/*
 * Heuristic.
 *
 * The MAX heuristic estimates the parallel cost of a plan.  The SUM
 * heuristic estimates the sequential cost of a plan.  The SUMR
 * heuristic is a variant of the SUM heuristic, trying to be more
 * clever about reuse of steps.  The UCPOP is from the UCPOP planner.
 */
struct Heuristic {
private:
  typedef enum { MAX, SUM, SUMR, UCPOP } HVal;

public:
  /* Constructs a heuristic from a name. */
  Heuristic(const string& name = "SUMR") {
    *this = name;
  }

  /* Selects a heuristic from a name. */
  Heuristic& operator=(const string& name);

  /* Selects the MAX heuristic. */
  void set_max() {
    h_ = MAX;
  }

  /* Checks if the MAX heuristic is selected. */
  bool max() const {
    return h_ == MAX;
  }

  /* Selects the SUM heuristic. */
  void set_sum() {
    h_ = SUM;
  }

  /* Checks if the SUM (or SUMR) heuristic is selected. */
  bool sum() const {
    return h_ == SUM || h_ == SUMR;
  }

  /* Selects the SUMR heuristic. */
  void set_sum_reuse() {
    h_ = SUMR;
  }

  /* Checks if the SUMR heuristic is selected. */
  bool sum_reuse() const {
    return h_ == SUMR;
  }

  /* Selects the UCPOP heuristic. */
  void set_ucpop() {
    h_ = UCPOP;
  }

  /* Checks if the UCPOP heuristic is selected. */
  bool ucpop() const {
    return h_ == UCPOP;
  }

private:
  /* The selected heuristic. */
  HVal h_;
};


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
  HeuristicValue()
    : max_cost_(0), max_work_(0), sum_cost_(0), sum_work_(0) {
  }

  /* Constructs a heuristic value. */
  HeuristicValue(int max_cost, int max_work, int sum_cost, int sum_work)
    : max_cost_(max_cost), max_work_(max_work),
      sum_cost_(sum_cost), sum_work_(sum_work) {
  }

  /* Returns the cost according to the MAX heuristic. */
  int max_cost() const {
    return max_cost_;
  }

  /* Returns the work according to the MAX heuristic. */
  int max_work() const {
    return max_work_;
  }

  /* Returns the cost according to the SUM heurisitc. */
  int sum_cost() const {
    return sum_cost_;
  }

  /* Returns the work according to the SUM heuristic. */
  int sum_work() const {
    return sum_work_;
  }

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


/*
 * A planning graph.
 */
struct PlanningGraph : public gc {
  /* Constructs a planning graph. */
  PlanningGraph(const Problem& problem);

  /* Returns the heurisitc value of an atom. */
  HeuristicValue heuristic_value(const Atom& atom,
				 const Bindings* bindings = NULL) const;

  /* Returns the heuristic value of a negated atom. */
  HeuristicValue heuristic_value(const Negation& negation,
				 const Bindings* bindings = NULL) const;

  /* Fills the provided list with actions that achieve the given
     formula. */
  void achieves_formula(ActionList& actions, const Formula& f) const;

private:
  /* Atom value map. */
  struct AtomValueMap
    : public HashMap<const Atom*, HeuristicValue, hash<const Hashable*>,
		     equal_to<const EqualityComparable*> > {
  };

  /* Iterator for AtomValueMap. */
  typedef AtomValueMap::const_iterator AtomValueMapIter;

  /* Mapping of formulas to actions. */
  struct FormulaActionsMap
    : public HashMultimap<const Formula*, const Action*, hash<const Hashable*>,
			  equal_to<const EqualityComparable*> > {
  };

  /* Iterator for FormulaActionsMap. */
  typedef FormulaActionsMap::const_iterator FormulaActionsMapIter;

  /* Mapping of predicate names to ground atoms. */
  struct PredicateAtomsMap : public HashMultimap<string, const Atom*> {
  };

  /* Iterator for PredicateAtomsMap. */
  typedef PredicateAtomsMap::const_iterator PredicateAtomsMapIter;

  /* Atom values. */
  AtomValueMap atom_values;
  /* Negated atom values. */
  AtomValueMap negation_values;
  /* Maps formulas to actions that achieve those formulas. */
  FormulaActionsMap achieves;
  /* Maps predicates to ground atoms. */
  PredicateAtomsMap predicate_atoms;
  /* Maps predicates to negated ground atoms. */
  PredicateAtomsMap predicate_negations;
};


/*
 * An invalid flaw selection order exception.
 */
struct InvalidFlawSelectionOrder : public Exception {
  /* Constructs an invalid flaw selection order exception. */
  InvalidFlawSelectionOrder(const string& name)
    : Exception("invalid flaw selection order `" + name + "'") {
  }
};


/*
 * Flaw selection order.
 *
 * LIFO/FIFO/RANDOM order works with all heuristics.  Other orders can
 * be combined with LIFO/FIFO/RANDOM order, but work only with
 * heuristics that can estimate cost for open conditions.
 */
struct FlawSelectionOrder : public Printable, public gc {
private:
  typedef enum { UNSPEC, MAX, SUM } FlawHeuristic;
  typedef enum { NONE, COST, WORK } PrimaryOrder;
  typedef enum { MOST, LEAST } PrimaryExtreme;
  typedef enum { LIFO, FIFO, RANDOM } SecondaryOrder;

public:
  /* Constructs a default flaw selection order. */
  FlawSelectionOrder(const string& name = "LIFO")
    : heuristic_(UNSPEC), primary_(NONE), extreme_(MOST), secondary_(LIFO) {
    *this = name;
  }

  /* Selects a flaw selection order from a name. */
  FlawSelectionOrder& operator=(const string& name);

  /* Sets the flaw value heuristic, but only if it is unspecified. */
  void set_heuristic(const Heuristic& h);

  /* Selects an open condition from the given list. */
  const OpenCondition* select(const Chain<const OpenCondition*>* open_conds,
			      const PlanningGraph& pg,
			      const Bindings* bindings = NULL) const;

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const;

private:
  /* Heuristic used for computing flaw value. */
  FlawHeuristic heuristic_;
  /* Primary flaw selection order. */
  PrimaryOrder primary_;
  /* Extreme for primary order. */
  PrimaryExtreme extreme_;
  /* Secondary order. */
  SecondaryOrder secondary_;
};

#endif /* HEURISTICS_H */
