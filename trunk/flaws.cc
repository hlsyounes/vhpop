#include "plans.h"
#include "formulas.h"


/* Prints this open condition on the given stream. */
void OpenCondition::print(ostream& os) const {
  os << "#<OPEN " << condition << " step " << step_id << ">";
}


/* Prints this threatened causal link on the given stream. */
void Unsafe::print(ostream& os) const {
  os << "#<UNSAFE " << link << " step " << step_id << ">";
}
