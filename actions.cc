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
 * $Id: actions.cc,v 6.6 2003-12-05 21:01:56 lorens Exp $
 */
#include "actions.h"
#include "bindings.h"
#include "problems.h"
#include "mathport.h"
#include <stack>
#include <typeinfo>


/* ====================================================================== */
/* Action */

/* Next action id. */
size_t Action::next_id = 0;


/* Constructs an action with the given name. */
Action::Action(const std::string& name, bool durative)
  : id_(next_id++), name_(name), condition_(&Condition::TRUE),
    durative_(durative), min_duration_(new Value(0.0f)),
    max_duration_(new Value(durative ? INFINITY : 0.0f)) {
  Condition::register_use(condition_);
  Expression::register_use(min_duration_);
  Expression::register_use(max_duration_);
}


/* Deletes this action. */
Action::~Action() {
  Condition::unregister_use(condition_);
  Expression::unregister_use(min_duration_);
  Expression::unregister_use(max_duration_);
  for (EffectList::const_iterator ei = effects().begin();
       ei != effects().end(); ei++) {
    delete *ei;
  }
}


/* Sets the condition for this action. */
void Action::set_condition(const Condition& condition) {
  if (condition_ != &condition) {
    Condition::unregister_use(condition_);
    condition_ = &condition;
    Condition::register_use(condition_);
  }
}


/* Adds an effect to this action. */
void Action::add_effect(const Effect& effect) {
  effects_.push_back(&effect);
}


/* Sets the minimum duration for this action. */
void Action::set_min_duration(const Expression& min_duration) {
  const Expression& md = Maximum::make(*min_duration_, min_duration);
  if (&md != min_duration_) {
    Expression::unregister_use(min_duration_);
    min_duration_ = &md;
    Expression::register_use(min_duration_);
  }
}


/* Sets the maximum duration for this action. */
void Action::set_max_duration(const Expression& max_duration) {
  const Expression& md = Minimum::make(*max_duration_, max_duration);
  if (&md != max_duration_) {
    Expression::unregister_use(max_duration_);
    max_duration_ = &md;
    Expression::register_use(max_duration_);
  }
}


/* Sets the duration for this action. */
void Action::set_duration(const Expression& duration) {
  set_min_duration(duration);
  set_max_duration(duration);
}


/* "Strengthens" the effects of this action. */
void Action::strengthen_effects(const Domain& domain) {
  /*
   * Separate negative effects from positive effects occuring at the
   * same time.
   */
  for (size_t i = 0; i < effects_.size(); i++) {
    const Effect& ei = *effects_[i];
    if (typeid(ei.literal()) == typeid(Negation)) {
      const Negation& neg = dynamic_cast<const Negation&>(ei.literal());
      const Condition* cond = &Condition::TRUE;
      for (EffectList::const_iterator j = effects_.begin();
	   j != effects_.end() && !cond->contradiction(); j++) {
	const Effect& ej = **j;
	if (ei.when() == ej.when() && typeid(ej.literal()) == typeid(Atom)) {
	  bool diff_param = ei.arity() != ej.arity();
	  for (size_t pi = 0; pi < ei.arity() && !diff_param; pi++) {
	    if (ei.parameter(pi) != ej.parameter(pi)) {
	      diff_param = true;
	    }
	  }
	  if (!diff_param) {
	    /* Only separate two effects with same universally
	       quantified variables. */
	    BindingList mgu;
	    if (Bindings::unifiable(mgu, neg.atom(), 1, ej.literal(), 1,
				    domain.types(), domain.terms())) {
	      const Formula* sep = &Formula::FALSE;
	      for (BindingList::const_iterator si = mgu.begin();
		   si != mgu.end(); si++) {
		const Binding& subst = *si;
		sep = &(*sep || Inequality::make(subst.var(), subst.term()));
	      }
	      cond = &(*cond && (Condition::make(*sep, AT_START)
				 || !ej.condition()));
	    }
	  }
	}
      }
      if (!cond->tautology()) {
	ei.set_link_condition(*cond);
      }
    }
  }

  /*
   * Separate effects from conditions asserted at the same time.
   */
  for (size_t i = 0; i < effects_.size(); i++) {
    const Effect& ei = *effects_[i];
    const Literal& literal = ei.literal();
    const Formula* cond = &condition().over_all().separator(literal, domain);
    ei.set_link_condition(ei.link_condition()
			  && Condition::make(*cond, OVER_ALL));
    if (ei.when() != Effect::AT_END) {
      cond = &condition().at_start().separator(literal, domain);
      ei.set_link_condition(ei.link_condition()
			    && Condition::make(*cond, AT_START));
    }
    if (ei.when() != Effect::AT_START) {
      cond = &condition().at_end().separator(literal, domain);
      ei.set_link_condition(ei.link_condition()
			    && Condition::make(*cond, AT_END));
    }
  }
}


/* ====================================================================== */
/* ActionSchema */

/* Constructs an action schema with the given name. */
ActionSchema::ActionSchema(const std::string& name, bool durative)
  : Action(name, durative) {}


/* Adds a parameter to this action schema. */
void ActionSchema::add_parameter(Variable var) {
  parameters_.push_back(var);
}


/* Fills the provided action list with all instantiations of this
   action schema. */
void ActionSchema::instantiations(GroundActionList& actions,
				  const Problem& problem) const {
  size_t n = parameters().size();
  if (n == 0) {
    const GroundAction* inst_action =
      instantiation(SubstitutionMap(), problem, condition());
    if (inst_action != NULL) {
      actions.push_back(inst_action);
    }
  } else {
    SubstitutionMap args;
    std::vector<const ObjectList*> arguments(n);
    std::vector<ObjectList::const_iterator> next_arg;
    for (size_t i = 0; i < n; i++) {
      Type t = problem.domain().terms().type(parameters()[i]);
      arguments[i] = &problem.compatible_objects(t);
      if (arguments[i]->empty()) {
	return;
      }
      next_arg.push_back(arguments[i]->begin());
    }
    std::stack<const Condition*> conds;
    conds.push(&condition());
    Condition::register_use(conds.top());
    for (size_t i = 0; i < n; ) {
      args.insert(std::make_pair(parameters()[i], *next_arg[i]));
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
      const Condition& inst_cond = conds.top()->instantiation(pargs, problem);
      conds.push(&inst_cond);
      Condition::register_use(conds.top());
      if (i + 1 == n || inst_cond.contradiction()) {
	if (!inst_cond.contradiction()) {
	  const GroundAction* inst_action =
	    instantiation(args, problem, inst_cond);
	  if (inst_action != NULL) {
	    actions.push_back(inst_action);
	  }
	}
	for (int j = i; j >= 0; j--) {
	  Condition::unregister_use(conds.top());
	  conds.pop();
	  args.erase(parameters()[j]);
	  next_arg[j]++;
	  if (next_arg[j] == arguments[j]->end()) {
	    if (j == 0) {
	      i = n;
	      break;
	    } else {
	      next_arg[j] = arguments[j]->begin();
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


/* Returns an instantiation of this action schema. */
const GroundAction*
ActionSchema::instantiation(const SubstitutionMap& args,
			    const Problem& problem,
			    const Condition& condition) const {
  EffectList inst_effects;
  size_t useful = 0;
  for (EffectList::const_iterator ei = effects().begin();
       ei != effects().end(); ei++) {
    (*ei)->instantiations(inst_effects, useful, args, problem);
  }
  if (useful > 0) {
    GroundAction& ga = *new GroundAction(name(), durative());
    size_t n = parameters().size();
    for (size_t i = 0; i < n; i++) {
      SubstitutionMap::const_iterator si = args.find(parameters()[i]);
      ga.add_argument((*si).second);
    }
    ga.set_condition(condition);
    for (EffectList::const_iterator ei = inst_effects.begin();
	 ei != inst_effects.end(); ei++) {
      ga.add_effect(**ei);
    }
    ga.set_min_duration(min_duration().instantiation(args, problem));
    ga.set_max_duration(max_duration().instantiation(args, problem));
    const Value* v1 = dynamic_cast<const Value*>(&ga.min_duration());
    if (v1 != NULL) {
      const Value* v2 = dynamic_cast<const Value*>(&ga.max_duration());
      if (v2 != NULL) {
	if (v1->value() > v2->value()) {
	  delete &ga;
	  return NULL;
	}
      }
    }
    return &ga;
  } else {
    for (EffectList::const_iterator ei = inst_effects.begin();
	 ei != inst_effects.end(); ei++) {
      delete *ei;
    }
    return NULL;
  }
}


/* Prints this action on the given stream. */
void ActionSchema::print(std::ostream& os, const PredicateTable& predicates,
			 const FunctionTable& functions,
			 const TermTable& terms) const {
  os << "  " << name();
  os << std::endl << "    parameters:";
  for (VariableList::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    os << ' ';
    terms.print_term(os, *vi);
  }
  if (durative()) {
    os << std::endl << "    duration: [";
    min_duration().print(os, functions, terms);
    os << ',';
    max_duration().print(os, functions, terms);
    os << "]";
  }
  os << std::endl << "    condition: ";
  condition().print(os, predicates, terms, 0, Bindings::EMPTY);
  os << std::endl << "    effect: (and";
  for (EffectList::const_iterator ei = effects().begin();
       ei != effects().end(); ei++) {
    os << ' ';
    (*ei)->print(os, predicates, terms);
  }
  os << ")";
}


/* Prints this action on the given stream with the given bindings. */
void ActionSchema::print(std::ostream& os, const TermTable& terms,
			 size_t step_id, const Bindings& bindings) const {
  os << '(' << name();
  for (VariableList::const_iterator ti = parameters().begin();
       ti != parameters().end(); ti++) {
    os << ' ';
    terms.print_term(os, *ti, step_id, bindings);
  }
  os << ')';
}


/* ====================================================================== */
/* GroundAction */

/* Constructs a ground action with the given name. */
GroundAction::GroundAction(const std::string& name, bool durative)
  : Action(name, durative) {}


/* Adds an argument to this ground action. */
void GroundAction::add_argument(Object arg) {
  arguments_.push_back(arg);
}


/* Prints this action on the given stream with the given bindings. */
void GroundAction::print(std::ostream& os, const TermTable& terms,
			 size_t step_id, const Bindings& bindings) const {
  os << '(' << name();
  for (ObjectList::const_iterator ni = arguments().begin();
       ni != arguments().end(); ni++) {
    os << ' ';
    terms.print_term(os, *ni, step_id, bindings);
  }
  os << ')';
}
