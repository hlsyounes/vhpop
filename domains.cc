/*
 * $Id: domains.cc,v 1.3 2001-07-29 17:42:03 lorens Exp $
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
  : forall(*(new VariableList())), condition(NULL),
    add_list(*(new FormulaList(&add))) {
}


/* Constructs an unconditional multiple effect. */
Effect::Effect(const FormulaList& add_list)
  : forall(*(new VariableList())), condition(NULL), add_list(add_list) {
}


/* Constructs a conditional effect. */
Effect::Effect(const Formula* condition, const FormulaList& add_list)
  : forall(*(new VariableList())), condition(condition), add_list(add_list) {
}


/* Returns an instantiation of this effect. */
const Effect& Effect::instantiation(size_t id) const {
  const Formula* inst_condition =
    (condition != NULL) ? &condition->instantiation(id) : NULL;
  return *(new Effect(forall.instantiation(id), inst_condition,
		      add_list.instantiation(id)));
}


/* Returns this effect subject to the given substitutions. */
const Effect& Effect::substitution(const SubstitutionList& subst) const {
  if (forall.empty()) {
    const Formula* subst_condition =
      (condition != NULL) ? &condition->substitution(subst) : NULL;
    return *(new Effect(forall, subst_condition,
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
    const Formula* subst_condition =
      (condition != NULL) ? &condition->substitution(eff_subst) : NULL;
    return *(new Effect(forall, subst_condition,
			add_list.substitution(eff_subst)));
  }
}


/* Checks if the add list of this effect matches the given formula. */
bool Effect::matches(const Formula& f) const {
  return add_list.matches(f);
}


/* Checks if the add list of this effect involves the given
   predicate. */
bool Effect::involves(const string& predicate) const {
  return add_list.involves(predicate);
}


/* Prints this effect on the given stream. */
void Effect::print(ostream& os) const {
  os << '[';
  if (condition != NULL) {
    os << *condition;
  }
  os << "->";
  if (add_list.size() == 1) {
    os << *add_list.front();
  } else {
    os << "(and";
    for (FormulaList::const_iterator i = add_list.begin();
	 i != add_list.end(); i++) {
      os << ' ' << **i;
    }
    os << ")";
  }
  os << ']';
}


/* Constructs an action. */
Action::Action(const Formula* precondition, const EffectList& effects)
  : precondition(precondition), effects(effects),
    cost((precondition != NULL) ? precondition->cost() : 0) {
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
  cout << "Instantiating action '" << action_formula(0) << "'" << endl;
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
    if (args.size() > i) {
      args[i] = new Substitution(*parameters[i], **next_arg[i]);
    } else {
      args.push_back(new Substitution(*parameters[i], **next_arg[i]));
    }
    if (i + 1 == n) {
      // create instantiation
      const Formula* new_precond = (precondition != NULL) ?
	&precondition->substitution(args) : NULL;
      if (new_precond == NULL || new_precond->consistent(problem)) {
	// consistent instantiation
	TermList& terms = *(new TermList());
	for (SubstitutionList::const_iterator s = args.begin();
	     s != args.end(); s++) {
	  terms.push_back(&(*s)->term);
	}
	actions.push_back(new GroundAction(name, terms, new_precond,
					   effects.substitution(args)));
	cout << "instantiation: " << actions.back()->action_formula(0) << endl;
      }
      for (int j = i; j >= 0; j--) {
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
  if (precondition != NULL) {
    os << *precondition;
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


/* Constructs a ground action, assuming arguments are names. */
GroundAction::GroundAction(const string& name, const TermList& arguments,
			   const Formula* precondition,
			   const EffectList& effects)
  : Action(precondition, effects),
    formula(*(new AtomicFormula(name, arguments))) {
}


/* Tests if this action equals the given action. */
bool GroundAction::equals(const Action& a) const {
  const GroundAction* ga = dynamic_cast<const GroundAction*>(&a);
  return ga != NULL && formula == ga->formula;
}


/* Returns the hash value of this action. */
size_t GroundAction::hash_value() const {
  // should include arguments in hash
  return hash<string>()(formula.predicate);
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
  if (precondition != NULL) {
    os << *precondition;
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
    if (name.type.compatible(t)) {
      constants.push_back(&name);
    }
  }
}


/* Returns a set of static predicates. */
hash_set<string> Domain::static_predicates(const PredicateMap& predicates,
					   const ActionMap& actions) {
  hash_set<string> preds;
  for (PredicateMap::const_iterator i = predicates.begin();
       i != predicates.end(); i++) {
    const string& p = (*i).first;
    bool is_static = true;
    for (ActionMap::const_iterator j = actions.begin();
	 j != actions.end() && is_static; j++) {
      if ((*j).second->effects.involves(p)) {
	is_static = false;
      }
    }
    if (is_static) {
      preds.insert((*i).first);
    }
  }
  return preds;
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
