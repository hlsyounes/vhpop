/* -*-C++-*- */
/*
 * Heuristics.
 *
 * $Id: heuristics.h,v 1.3 2001-08-20 04:11:09 lorens Exp $
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
 * LIFO/FIFO order works with all heuristics.  Hardest/easiest first
 * can be combined with LIFO/FIFO order, but works only with
 * heuristics that can estimate cost for open conditions.
 */
struct FlawSelectionOrder {
  /* Constructs a default flaw selection order. */
  FlawSelectionOrder()
    : lifo_(true), difficulty_(0) {
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

  /* Selects hardest first order. */
  void set_hardest_first() {
    difficulty_ = 1;
  }

  /* Checks if hardest first order is selected. */
  bool hardest_first() const {
    return difficulty_ > 0;
  }

  /* Selects easiest first order. */
  void set_easiest_first() {
    difficulty_ = -1;
  }

  /* Checks if easiest first order is selected. */
  bool easiest_first() const {
    return difficulty_ < 0;
  }

private:
  /* Whether to select flaws in LIFO order or FIFO order. */
  bool lifo_;
  /* Hardest first (> 0), easiest first (< 0), or no preference (== 0). */
  int difficulty_;
};

#endif /* HEURISTICS_H */
