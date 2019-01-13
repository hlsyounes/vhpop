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

#include "actions.h"

#include <limits>
#include <stack>
#include <typeinfo>

#include "problems.h"
#include "refcount.h"
#include "types.h"

size_t Action::next_id = 0;

Action::Action(const std::string& name, bool durative)
    : id_(next_id++),
      name_(name),
      condition_(&Formula::TRUE),
      durative_(durative),
      min_duration_(new Value(0.0f)),
      max_duration_(
          new Value(durative ? std::numeric_limits<float>::infinity() : 0.0f)) {
  Formula::register_use(condition_);
  RCObject::ref(min_duration_);
  RCObject::ref(max_duration_);
}

Action::~Action() {
  Formula::unregister_use(condition_);
  RCObject::destructive_deref(min_duration_);
  RCObject::destructive_deref(max_duration_);
  for (EffectList::const_iterator ei = effects().begin(); ei != effects().end();
       ei++) {
    delete *ei;
  }
}

void Action::set_condition(const Formula& condition) {
  if (condition_ != &condition) {
    Formula::register_use(&condition);
    Formula::unregister_use(condition_);
    condition_ = &condition;
  }
}

void Action::add_effect(const Effect& effect) { effects_.push_back(&effect); }

void Action::set_min_duration(const Expression& min_duration) {
  const Expression& md = Maximum::make(*min_duration_, min_duration);
  if (&md != min_duration_) {
    RCObject::ref(&md);
    RCObject::destructive_deref(min_duration_);
    min_duration_ = &md;
  }
}

void Action::set_max_duration(const Expression& max_duration) {
  const Expression& md = Minimum::make(*max_duration_, max_duration);
  if (&md != max_duration_) {
    RCObject::ref(&md);
    RCObject::destructive_deref(max_duration_);
    max_duration_ = &md;
  }
}

void Action::set_duration(const Expression& duration) {
  set_min_duration(duration);
  set_max_duration(duration);
}

void Action::strengthen_effects(const Domain& domain) {
  // Separate negative effects from positive effects occuring at the same time.
  for (size_t i = 0; i < effects_.size(); i++) {
    const Effect& ei = *effects_[i];
    if (typeid(ei.literal()) == typeid(Negation)) {
      const Negation& neg = dynamic_cast<const Negation&>(ei.literal());
      const Formula* cond = &Formula::TRUE;
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
            // Only separate two effects with same universally quantified
            // variables.
            BindingList mgu;
            if (Bindings::unifiable(mgu, neg.atom(), 1, ej.literal(), 1)) {
              const Formula* sep = &Formula::FALSE;
              for (BindingList::const_iterator si = mgu.begin();
                   si != mgu.end(); si++) {
                const Binding& subst = *si;
                sep = &(*sep || Inequality::make(subst.var(), subst.term()));
              }
              cond = &(*cond && (*sep || !ej.condition()));
            }
          }
        }
      }
      if (!cond->tautology()) {
        ei.set_link_condition(*cond);
      }
    }
  }

  // Separate effects from conditions asserted at the same time.
  for (size_t i = 0; i < effects_.size(); i++) {
    const Effect& ei = *effects_[i];
    ei.set_link_condition(ei.link_condition() &&
                          condition().separator(ei, domain));
  }
}

ActionSchema::ActionSchema(const std::string& name, bool durative)
    : Action(name, durative) {}

void ActionSchema::add_parameter(Variable var) { parameters_.push_back(var); }

void ActionSchema::instantiations(std::vector<const GroundAction*>& actions,
                                  const Problem& problem) const {
  size_t n = parameters().size();
  if (n == 0) {
    const GroundAction* inst_action =
        instantiation(std::map<Variable, Term>(), problem, condition());
    if (inst_action != NULL) {
      actions.push_back(inst_action);
    }
  } else {
    std::map<Variable, Term> args;
    std::vector<const std::vector<Object>*> arguments(n);
    std::vector<std::vector<Object>::const_iterator> next_arg;
    for (size_t i = 0; i < n; i++) {
      const Type& t = TermTable::type(parameters()[i]);
      arguments[i] = &problem.terms().compatible_objects(t);
      if (arguments[i]->empty()) {
        return;
      }
      next_arg.push_back(arguments[i]->begin());
    }
    std::stack<const Formula*> conds;
    conds.push(&condition());
    Formula::register_use(conds.top());
    for (size_t i = 0; i < n;) {
      args.insert(std::make_pair(parameters()[i], *next_arg[i]));
      std::map<Variable, Term> pargs;
      pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
      const Formula& inst_cond = conds.top()->instantiation(pargs, problem);
      conds.push(&inst_cond);
      Formula::register_use(conds.top());
      if (i + 1 == n || inst_cond.contradiction()) {
        if (!inst_cond.contradiction()) {
          const GroundAction* inst_action =
              instantiation(args, problem, inst_cond);
          if (inst_action != NULL) {
            actions.push_back(inst_action);
          }
        }
        for (int j = i; j >= 0; j--) {
          Formula::unregister_use(conds.top());
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
      Formula::unregister_use(conds.top());
      conds.pop();
    }
  }
}

const GroundAction* ActionSchema::instantiation(
    const std::map<Variable, Term>& args, const Problem& problem,
    const Formula& condition) const {
  EffectList inst_effects;
  size_t useful = 0;
  for (EffectList::const_iterator ei = effects().begin(); ei != effects().end();
       ei++) {
    (*ei)->instantiations(inst_effects, useful, args, problem);
  }
  if (useful > 0) {
    GroundAction& ga = *new GroundAction(name(), durative());
    size_t n = parameters().size();
    for (size_t i = 0; i < n; i++) {
      std::map<Variable, Term>::const_iterator si = args.find(parameters()[i]);
      ga.add_argument((*si).second.as_object());
    }
    ga.set_condition(condition);
    for (EffectList::const_iterator ei = inst_effects.begin();
         ei != inst_effects.end(); ei++) {
      ga.add_effect(**ei);
    }
    ga.set_min_duration(
        min_duration().instantiation(args, problem.init_values()));
    ga.set_max_duration(
        max_duration().instantiation(args, problem.init_values()));
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

void ActionSchema::print(std::ostream& os) const {
  os << "  " << name();
  os << std::endl << "    parameters:";
  for (std::vector<Variable>::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    os << ' ' << *vi;
  }
  if (durative()) {
    os << std::endl
       << "    duration: [" << min_duration() << ',' << max_duration() << "]";
  }
  os << std::endl << "    condition: ";
  condition().print(os, 0, Bindings::EMPTY);
  os << std::endl << "    effect: (and";
  for (EffectList::const_iterator ei = effects().begin(); ei != effects().end();
       ei++) {
    os << ' ';
    (*ei)->print(os);
  }
  os << ")";
}

void ActionSchema::print(std::ostream& os, size_t step_id,
                         const Bindings& bindings) const {
  os << '(' << name();
  for (std::vector<Variable>::const_iterator ti = parameters().begin();
       ti != parameters().end(); ti++) {
    os << ' ';
    bindings.print_term(os, *ti, step_id);
  }
  os << ')';
}

GroundAction::GroundAction(const std::string& name, bool durative)
    : Action(name, durative) {}

void GroundAction::add_argument(Object arg) { arguments_.push_back(arg); }

void GroundAction::print(std::ostream& os, size_t step_id,
                         const Bindings& bindings) const {
  os << '(' << name();
  for (std::vector<Object>::const_iterator ni = arguments().begin();
       ni != arguments().end(); ni++) {
    os << ' ' << *ni;
  }
  os << ')';
}
