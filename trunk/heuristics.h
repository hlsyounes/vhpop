/* -*-C++-*- */
/*
 * Heuristics.
 *
 * $Id: heuristics.h,v 1.4 2001-09-03 20:06:16 lorens Exp $
 */
#ifndef HEURISTICS_H
#define HEURISTICS_H


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
  /* Constructs a default heuristic. */
  Heuristic() :
    h_(SUMR) {
  }

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
 * Heuristic cost.
 */
struct Cost {
  /* Total estimated cost. */
  int cost;
  /* Estimated remaining effort. */
  int work;

  /* Constructs a zero cost object. */
  Cost()
    : cost(0), work(0) {
  }

  /* Constructs a cost object. */
  Cost(int cost, int work)
    : cost(cost), work(work) {
  }

  /* Checks if this cost equals the given cost. */
  bool operator==(const Cost& c) const {
    return cost == c.cost && work == c.work;
  }

  /* Checks if this cost is less than the given cost. */
  bool operator<(const Cost& c) const {
    return cost < c.cost || (cost == c.cost && work < c.work);
  }

  /* Checks if this cost is greater than the given cost. */
  bool operator>(const Cost& c) const {
    return cost > c.cost || (cost == c.cost && work > c.work);
  }

  /* Checks if this cost is at most the given cost. */
  bool operator<=(const Cost& c) const {
    return !(*this > c);
  }

  /* Checks if this cost is at least the given cost. */
  bool operator>=(const Cost& c) const {
    return !(*this < c);
  }

  /* Adds the given cost to this cost. */
  Cost& operator+=(const Cost& c) {
    cost += c.cost;
    work += c.work;
    return *this;
  }
};


/*
 * Flaw selection order.
 *
 * LIFO/FIFO order works with all heuristics.  Other orders can be
 * combined with LIFO/FIFO order, but work only with heuristics that
 * can estimate cost for open conditions.
 */
struct FlawSelectionOrder {
private:
  typedef enum { NONE, MC, LC, MW, LW } FVal;

public:
  /* Constructs a default flaw selection order. */
  FlawSelectionOrder()
    : lifo_(true), mode_(NONE) {
  }

  /* Selects LIFO order. */
  void set_lifo() {
    lifo_ = true;
  }

  /* Checks if LIFO order is selected. */
  bool lifo() const {
    return lifo_;
  }

  /* Selects FIFO order. */
  void set_fifo() {
    lifo_ = false;
  }

  /* Checks if FIFO order is selected. */
  bool fifo() const {
    return !lifo_;
  }

  /* Selects most cost first order. */
  void set_most_cost_first() {
    mode_ = MC;
  }

  /* Checks if most cost first order is selected. */
  bool most_cost_first() const {
    return mode_ == MC;
  }

  /* Selects least cost first order. */
  void set_least_cost_first() {
    mode_ = LC;
  }

  /* Checks if least cost first order is selected. */
  bool least_cost_first() const {
    return mode_ == LC;
  }

  /* Selects most work first order. */
  void set_most_work_first() {
    mode_ = MW;
  }

  /* Checks if most work first order is selected. */
  bool most_work_first() const {
    return mode_ == MW;
  }

  /* Selects least work first order. */
  void set_least_work_first() {
    mode_ = LW;
  }

  /* Checks if least work first order is selected. */
  bool least_work_first() const {
    return mode_ == LW;
  }

private:
  /* Whether to select flaws in LIFO order or FIFO order. */
  bool lifo_;
  /* Flaw selection mode. */
  FVal mode_;
};

#endif /* HEURISTICS_H */
