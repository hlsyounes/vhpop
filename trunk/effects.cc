/*
 * Copyright (C) 2003 Carnegie Mellon University
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
 * $Id: effects.cc,v 6.1 2003-07-28 21:40:32 lorens Exp $
 */
#include "effects.h"
#include "bindings.h"
#include "problems.h"
#include "formulas.h"
#include <stack>


/* ====================================================================== */
/* Effect */

/* Constructs an effect. */
Effect::Effect(const Literal& literal, EffectTime when)
  : condition_(&Condition::TRUE), link_condition_(&Condition::TRUE),
    literal_(&literal), when_(when) {
  Condition::register_use(condition_);
  Condition::register_use(link_condition_);
  Formula::register_use(literal_);
}


/* Deletes this effect. */
Effect::~Effect() {
  Condition::unregister_use(condition_);
  Condition::unregister_use(link_condition_);
  Formula::unregister_use(literal_);
}


/* Adds a universally quantified variable to this effect. */
void Effect::add_parameter(Variable parameter) {
  parameters_.push_back(parameter);
}


/* Sets the condition of this effect. */
void Effect::set_condition(const Condition& condition) {
  if (condition_ != &condition) {
    Condition::unregister_use(condition_);
    condition_ = &condition;
    Condition::register_use(condition_);
  }
}


/* Sets the link condition of this effect. */
void Effect::set_link_condition(const Condition& link_condition) const {
  if (link_condition_ != &link_condition) {
    Condition::unregister_use(link_condition_);
    link_condition_ = &link_condition;
    Condition::register_use(link_condition_);
  }
}


/* Tests if this effect quantifies the given variable. */
bool Effect::quantifies(Variable variable) const {
  return (find(parameters_.begin(), parameters_.end(), variable)
	  != parameters_.end());
}


/* Fills the provided list with instantiations of this effect. */
void Effect::instantiations(EffectList& effects, size_t& useful,
			    const SubstitutionMap& subst,
			    const Problem& problem) const {
  size_t n = arity();
  if (n == 0) {
    const Condition& inst_cond = condition().instantiation(subst, problem);
    if (!inst_cond.contradiction()) {
      const Effect* inst_effect = instantiation(subst, problem, inst_cond);
      effects.push_back(inst_effect);
      if (!inst_effect->link_condition().contradiction()) {
	useful++;
      }
    }
  } else {
    SubstitutionMap args(subst);
    std::vector<ObjectList> arguments(n, ObjectList());
    std::vector<ObjectList::const_iterator> next_arg;
    for (size_t i = 0; i < n; i++) {
      problem.compatible_objects(arguments[i],
				 problem.domain().terms().type(parameter(i)));
      if (arguments[i].empty()) {
	return;
      }
      next_arg.push_back(arguments[i].begin());
    }
    std::stack<const Condition*> conds;
    conds.push(&condition().instantiation(args, problem));
    Condition::register_use(conds.top());
    for (size_t i = 0; i < n; ) {
      args.insert(std::make_pair(parameter(i), *next_arg[i]));
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameter(i), *next_arg[i]));
      const Condition& inst_cond = conds.top()->instantiation(pargs, problem);
      conds.push(&inst_cond);
      Condition::register_use(conds.top());
      if (i + 1 == n || inst_cond.contradiction()) {
	if (!inst_cond.contradiction()) {
	  const Effect* inst_effect = instantiation(args, problem, inst_cond);
	  effects.push_back(inst_effect);
	  if (!inst_effect->link_condition().contradiction()) {
	    useful++;
	  }
	}
	for (int j = i; j >= 0; j--) {
	  Condition::unregister_use(conds.top());
	  conds.pop();
	  args.erase(parameter(j));
	  next_arg[j]++;
	  if (next_arg[j] == arguments[j].end()) {
	    if (j == 0) {
	      i = n;
	      break;
	    } else {
	      next_arg[j] = arguments[j].begin();
	    }
	  } else {
	    i = j;
	    break;
	  }
	}
      } else {
	i++;
      }
    }
    while (!conds.empty()) {
      Condition::unregister_use(conds.top());
      conds.pop();
    }
  }
}


/* Returns an instantiation of this effect. */
const Effect* Effect::instantiation(const SubstitutionMap& args,
				    const Problem& problem,
				    const Condition& condition) const {
  Effect* inst_eff = new Effect(literal().substitution(args), when());
  inst_eff->set_condition(condition);
  inst_eff->set_link_condition(link_condition().instantiation(args, problem));
  return inst_eff;
}


/* Prints this effect on the given stream. */
void Effect::print(std::ostream& os, const PredicateTable& predicates,
		   const TermTable& terms) const {
  os << '(';
  for (VariableList::const_iterator vi = parameters_.begin();
       vi != parameters_.end(); vi++) {
    terms.print_term(os, *vi);
    os << ' ';
  }
  switch (when()) {
  case Effect::AT_START:
    os << "at start ";
    break;
  case Effect::AT_END:
    os << "at end ";
    break;
  }
  os << '[';
  condition().print(os, predicates, terms, 0, Bindings());
  os << ',';
  link_condition().print(os, predicates, terms, 0, Bindings());
  os << "->";
  literal().print(os, predicates, terms, 0, Bindings());
  os << ']' << ')';
}
