/*
 * $Id: domains.cc,v 1.12 2001-09-03 20:05:08 lorens Exp $
 */
#include "domains.h"
#include "problems.h"
#include "formulas.h"


/* Table of defined domains. */
Domain::DomainMap Domain::domains = Domain::DomainMap();


/* Returns the arity of this predicate. */
size_t Predicate::arity() const {
  return parameters.size();
}


/* Prints this predicate on the given stream. */
void Predicate::print(ostream& os) const {
  os << '(' << name;
  for (VariableList::const_iterator i = parameters.begin();
       i != parameters.end(); i++) {
    os << ' ' << **i;
    if (!(*i)->type.object()) {
      os << " - " << (*i)->type;
    }
  }
  os << ')';
}


/* Constructs an unconditional single effect. */
Effect::Effect(const Formula& add)
  : forall(VariableList::EMPTY), condition(Formula::TRUE),
    add_list(*(new FormulaList(&add))) {
}


/* Constructs an unconditional multiple effect. */
Effect::Effect(const FormulaList& add_list)
  : forall(VariableList::EMPTY), condition(Formula::TRUE), add_list(add_list) {
}


/* Constructs a conditional effect. */
Effect::Effect(const Formula& condition, const FormulaList& add_list)
  : forall(VariableList::EMPTY), condition(condition), add_list(add_list) {
}


/* Constructs a universally quantified unconditional effect. */
Effect::Effect(const VariableList& forall, const FormulaList& add_list)
  : forall(forall), condition(Formula::TRUE), add_list(add_list) {
}


/* Returns an instantiation of this effect. */
const Effect& Effect::instantiation(size_t id) const {
  return *(new Effect(forall.instantiation(id), condition.instantiation(id),
		      add_list.instantiation(id)));
}


/* Returns an instantiation of this effect. */
const Effect& Effect::instantiation(const SubstitutionList& subst,
				    const Problem& problem) const {
  if (forall.empty()) {
    return *(new Effect(forall, condition.instantiation(subst, problem),
			add_list.instantiation(subst, problem)));
  } else {
    SubstitutionList eff_subst;
    for (SubstitutionList::const_iterator i = subst.begin();
	 i != subst.end(); i++) {
      const Substitution& s = **i;
      if (!forall.contains(s.var)) {
	eff_subst.push_back(&s);
      }
    }
    return *(new Effect(forall, condition.instantiation(eff_subst, problem),
			add_list.instantiation(eff_subst, problem)));
  }
}


/* Returns this effect subject to the given substitutions. */
const Effect& Effect::substitution(const SubstitutionList& subst) const {
  if (forall.empty()) {
    return *(new Effect(forall, condition.substitution(subst),
			add_list.substitution(subst)));
  } else {
    SubstitutionList eff_subst;
    for (SubstitutionList::const_iterator i = subst.begin();
	 i != subst.end(); i++) {
      const Substitution& s = **i;
      if (!forall.contains(s.var)) {
	eff_subst.push_back(&s);
      }
    }
    return *(new Effect(forall, condition.substitution(eff_subst),
			add_list.substitution(eff_subst)));
  }
}


/* Fills the provided lists with goals achievable by this effect. */
void Effect::achievable_goals(FormulaList& goals,
			      FormulaList& neg_goals) const {
  add_list.achievable_goals(goals, neg_goals);
}


/* Fills the provided sets with predicates achievable by the
   effect. */
void Effect::achievable_predicates(hash_set<string>& preds,
				   hash_set<string>& neg_preds) const {
  add_list.achievable_predicates(preds, neg_preds);
}


/* Prints this effect on the given stream. */
void Effect::print(ostream& os) const {
  os << '[';
  if (condition != Formula::TRUE) {
    os << condition;
  }
  os << "->";
  if (add_list.size() == 1) {
    os << *add_list.front();
  } else {
    os << "(and";
    for (FLCI i = add_list.begin(); i != add_list.end(); i++) {
      os << ' ' << **i;
    }
    os << ")";
  }
  os << ']';
}


/* Returns an instantiation of this effect list. */
const EffectList& EffectList::instantiation(size_t id) const {
  EffectList& effects = *(new EffectList());
  for (const_iterator i = begin(); i != end(); i++) {
    effects.push_back(&(*i)->instantiation(id));
  }
  return effects;
}


/* Returns an instantiation of this effect list. */
const EffectList& EffectList::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  EffectList& effects = *(new EffectList());
  for (const_iterator i = begin(); i != end(); i++) {
    const Effect& e = (*i)->instantiation(subst, problem);
    if (!(e.condition == Formula::FALSE || e.add_list.empty())) {
      effects.push_back(&e);
    }
  }
  return effects;
}


/* Returns this effect list subject to the given substitutions. */
const EffectList&
EffectList::substitution(const SubstitutionList& subst) const {
  EffectList& effects = *(new EffectList());
  for (const_iterator i = begin(); i != end(); i++) {
    effects.push_back(&(*i)->substitution(subst));
  }
  return effects;
}


/* Fills the provided lists with goals achievable by the effect in
   this list. */
void EffectList::achievable_goals(FormulaList& goals,
				  FormulaList& neg_goals) const {
  for (const_iterator i = begin(); i != end(); i++) {
    (*i)->achievable_goals(goals, neg_goals);
  }
}


/* Fills the provided sets with predicates achievable by the effects
   in this list. */
void EffectList::achievable_predicates(hash_set<string>& preds,
				       hash_set<string>& neg_preds) const {
  for (const_iterator i = begin(); i != end(); i++) {
    (*i)->achievable_predicates(preds, neg_preds);
  }
}


/* Constructs an action. */
Action::Action(const Formula& precondition, const EffectList& effects)
  : precondition(precondition), effects(effects) {
}


/* Fills the provided lists with goals achievable by this action. */
void Action::achievable_goals(FormulaList& goals,
			      FormulaList& neg_goals) const {
  effects.achievable_goals(goals, neg_goals);
}


/* Fills the provided sets with predicates achievable by this
   action. */
void Action::achievable_predicates(hash_set<string>& preds,
				   hash_set<string>& neg_preds) const {
  effects.achievable_predicates(preds, neg_preds);
}


/* Returns a formula representing this action. */
const AtomicFormula& ActionSchema::action_formula(size_t id) const {
  TermList& terms = *(new TermList());
  for (VariableList::const_iterator i = parameters.begin();
       i != parameters.end(); i++) {
    terms.push_back(&(*i)->instantiation(id));
  }
  return *(new AtomicFormula(name, terms));
}


/* Fills the provided action list with all instantiations of this
   action. */
void ActionSchema::instantiations(ActionList& actions,
				  const Problem& problem) const {
  vector<NameList*, container_alloc> arguments;
  vector<NameList::const_iterator> next_arg;
  for (VariableList::const_iterator i = parameters.begin();
       i != parameters.end(); i++) {
    arguments.push_back(new NameList());
    problem.compatible_objects(*arguments.back(), (*i)->type);
    next_arg.push_back(arguments.back()->begin());
  }
  SubstitutionList args;
  size_t n = parameters.size();
  for (size_t i = 0; i < n; ) {
    args.push_back(new Substitution(*parameters[i], **next_arg[i]));
    const Formula& new_precond = precondition.instantiation(args, problem);
    const EffectList& new_effects = effects.instantiation(args, problem);
    if (i + 1 == n || new_precond == Formula::FALSE || new_effects.empty()) {
      if (!(new_precond == Formula::FALSE || new_effects.empty())) {
	/* consistent instantiation */
	TermList& terms = *(new TermList());
	for (SubstitutionList::const_iterator s = args.begin();
	     s != args.end(); s++) {
	  terms.push_back(&(*s)->term);
	}
	actions.push_back(new GroundAction(name, terms, new_precond,
					   new_effects));
      }
      for (int j = i; j >= 0; j--) {
	args.pop_back();
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
}


/* Prints this action on the given stream. */
void ActionSchema::print(ostream& os) const {
  os << '(' << name << " (";
  for (VariableList::const_iterator i = parameters.begin();
       i != parameters.end(); i++) {
    if (i != parameters.begin()) {
      os << ' ';
    }
    os << **i;
    if (!(*i)->type.object()) {
      os << " - " << (*i)->type;
    }
  }
  os << ") ";
  if (precondition != Formula::TRUE) {
    os << precondition;
  } else {
    os << "nil";
  }
  os << " (";
  for (EffectList::const_iterator i = effects.begin();
       i != effects.end(); i++) {
    if (i != effects.begin()) {
      os << ' ';
    }
    os << **i;
  }
  os << ")" << ')';
}


/* Tests if this action equals the given action. */
bool ActionSchema::equals(const Action& a) const {
  const ActionSchema* as = dynamic_cast<const ActionSchema*>(&a);
  return as != NULL && name == as->name;
}


/* Returns the hash value of this action. */
size_t ActionSchema::hash_value() const {
  return hash<string>()(name);
}


/* Constructs a ground action, assuming arguments are names. */
GroundAction::GroundAction(const string& name, const TermList& arguments,
			   const Formula& precondition,
			   const EffectList& effects)
  : Action(precondition, effects),
    formula(*(new AtomicFormula(name, arguments))) {
}


/* Returns a formula representing this action. */
const AtomicFormula& GroundAction::action_formula(size_t id) const {
  return formula;
}


/* Fills the provided action list with all instantiations of this
   action. */
void GroundAction::instantiations(ActionList& actions,
				  const Problem& problem) const {
  actions.push_back(this);
}


/* Prints this action on the given stream. */
void GroundAction::print(ostream& os) const {
  os << '(' << formula.predicate << " (";
  for (TermList::const_iterator i = formula.terms.begin();
       i != formula.terms.end(); i++) {
    if (i != formula.terms.begin()) {
      os << ' ';
    }
    os << **i;
  }
  os << ") ";
  if (precondition != Formula::TRUE) {
    os << precondition;
  } else {
    os << "nil";
  }
  os << " (";
  for (EffectList::const_iterator i = effects.begin();
       i != effects.end(); i++) {
    if (i != effects.begin()) {
      os << ' ';
    }
    os << **i;
  }
  os << ")" << ')';
}


/* Tests if this action equals the given action. */
bool GroundAction::equals(const Action& a) const {
  const GroundAction* ga = dynamic_cast<const GroundAction*>(&a);
  return ga != NULL && formula == ga->formula;
}


/* Returns the hash value of this action. */
size_t GroundAction::hash_value() const {
  return hash<Formula>()(formula);
}


/* Returns the type with the given name, or NULL if it is
   undefined. */
const Type* Domain::find_type(const string& name) const {
  TypeMap::const_iterator i = types.find(name);
  return (i != types.end()) ? (*i).second : NULL;
}


/* Returns the constant with the given name, or NULL if it is
   undefined. */
const Name* Domain::find_constant(const string& name) const {
  NameMap::const_iterator i = constants.find(name);
  return (i != constants.end()) ? (*i).second : NULL;
}


/* Returns the predicate with the given name, or NULL if it is
   undefined. */
const Predicate* Domain::find_predicate(const string& name) const {
  PredicateMap::const_iterator i = predicates.find(name);
  return (i != predicates.end()) ? (*i).second : NULL;
}


/* Returns the action with the given name, or NULL if it is
   undefined. */
const Action* Domain::find_action(const string& name) const {
  ActionMap::const_iterator i = actions.find(name);
  return (i != actions.end()) ? (*i).second : NULL;
}


/* Fills the provided name list with constants that are compatible
   with the given type. */
void Domain::compatible_constants(NameList& constants, const Type& t) const {
  for (NameMap::const_iterator i = this->constants.begin();
       i != this->constants.end(); i++) {
    const Name& name = *(*i).second;
    if (name.type.subtype(t)) {
      constants.push_back(&name);
    }
  }
}


/* Returns a set of static predicates. */
hash_set<string> Domain::static_predicates(const PredicateMap& predicates,
					   const ActionMap& actions) {
  hash_set<string> static_preds;
  hash_set<string> achievable_preds;
  for (ActionMap::const_iterator ai = actions.begin();
       ai != actions.end(); ai++) {
    (*ai).second->achievable_predicates(achievable_preds, achievable_preds);
  }
  for (PredicateMap::const_iterator i = predicates.begin();
       i != predicates.end(); i++) {
    const string& p = (*i).first;
    if (achievable_preds.find(p) == achievable_preds.end()) {
      static_preds.insert(p);
    }
  }
  return static_preds;
}


/* Prints this domain on the given stream. */
void Domain::print(ostream& os) const {
  os << "name: " << name;
  os << endl << "types:";
  for (TypeMap::const_iterator i = types.begin(); i != types.end(); i++) {
    if (!(*i).second->object()) {
      os << ' ' << *(*i).second;
      if (!(*i).second->supertype.object()) {
	os << " - " << (*i).second->supertype;
      }
    }
  }
  os << endl << "constants:";
  for (NameMap::const_iterator i = constants.begin();
       i != constants.end(); i++) {
    os << ' ' << *(*i).second;
    if (!(*i).second->type.object()) {
      os << " - " << (*i).second->type;
    }
  }
  os << endl << "predicates:";
  for (PredicateMap::const_iterator i = predicates.begin();
       i != predicates.end(); i++) {
    os << endl << "  " << *(*i).second;
  }
  os << endl << "actions:";
  for (ActionMap::const_iterator i = actions.begin();
       i != actions.end(); i++) {
    os << endl << "  " << *(*i).second;
  }
}
