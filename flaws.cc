#include "plans.h"
#include "formulas.h"


/* ====================================================================== */
/* OpenCondition */

/* Constructs an open condition. */
OpenCondition::OpenCondition(size_t step_id, const Reason& reason)
  : step_id(step_id), reason(reason) {}


/* Prints this open condition on the given stream. */
void OpenCondition::print(ostream& os) const {
  os << "#<OPEN " << condition() << " step " << step_id << ">";
}


/* ====================================================================== */
/* LiteralOpenCondition */

/* Constructs a literal open condition. */
LiteralOpenCondition::LiteralOpenCondition(const Literal& condition,
					   size_t step_id,
					   const Reason& reason)
  : OpenCondition(step_id, reason), literal(condition) {}


/* Returns the open condition. */
const Formula& LiteralOpenCondition::condition() const {
  return literal;
}


/* Checks if this is a static open condition. */
bool LiteralOpenCondition::is_static(const Domain& domain) const {
  return domain.static_predicate(literal.predicate());
}


/* ====================================================================== */
/* FormulaOpenCondition */

/* Constructs a formula open condition. */
FormulaOpenCondition::FormulaOpenCondition(const Formula& condition,
					   size_t step_id,
					   const Reason& reason)
  : OpenCondition(step_id, reason), condition_(condition) {}


/* Returns the open condition. */
const Formula& FormulaOpenCondition::condition() const {
  return condition_;
}


/* Checks if this is a static open condition. */
bool FormulaOpenCondition::is_static(const Domain& domain) const {
  return false;
}


/* ====================================================================== */

/* Prints this threatened causal link on the given stream. */
void Unsafe::print(ostream& os) const {
  os << "#<UNSAFE " << link << " step " << step_id << ">";
}
