/*
 * $Id: domains.cc,v 1.25 2001-10-30 16:01:47 lorens Exp $
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


/* Prints this object on the given stream. */
void Predicate::print(ostream& os) const {
  os << '(' << name;
  for (VarListIter vi = parameters.begin(); vi != parameters.end(); vi++) {
    os << ' ' << **vi;
    if (!(*vi)->type.object()) {
      os << " - " << (*vi)->type;
    }
  }
  os << ')';
}


/* Constructs an unconditional effect. */
Effect::Effect(const AtomList& add_list, const NegationList& del_list)
  : forall(VariableList::EMPTY), condition(Formula::TRUE),
    add_list(add_list), del_list(del_list) {
}


/* Constructs a conditional effect. */
Effect::Effect(const Formula& condition,
	       const AtomList& add_list, const NegationList& del_list)
  : forall(VariableList::EMPTY), condition(condition),
    add_list(add_list), del_list(del_list) {
}


/* Constructs a universally quantified unconditional effect. */
Effect::Effect(const VariableList& forall,
	       const AtomList& add_list, const NegationList& del_list)
  : forall(forall), condition(Formula::TRUE),
    add_list(add_list), del_list(del_list) {
}


/* Returns an instantiation of this effect. */
const Effect& Effect::instantiation(size_t id) const {
  return *(new Effect(forall.instantiation(id), condition.instantiation(id),
		      add_list.instantiation(id), del_list.instantiation(id)));
}


/* Fills the provided list with instantiations of this effect. */
void Effect::instantiations(EffectList& effects, const SubstitutionList& subst,
			    const Problem& problem) const {
  if (forall.empty()) {
    const Formula& new_condition = condition.instantiation(subst, problem);
    if (new_condition != Formula::FALSE) {
      effects.push_back(new Effect(new_condition,
				   add_list.substitution(subst),
				   del_list.substitution(subst)));
    }
  } else {
    SubstitutionList args;
    for (SubstListIter si = subst.begin(); si != subst.end(); si++) {
      const Substitution& s = **si;
      if (!forall.contains(s.var)) {
	args.push_back(&s);
      }
    }
    Vector<NameList*> arguments;
    Vector<NameListIter> next_arg;
    for (VarListIter vi = forall.begin(); vi != forall.end(); vi++) {
      arguments.push_back(new NameList());
      problem.compatible_objects(*arguments.back(), (*vi)->type);
      if (arguments.back()->empty()) {
	return;
      }
      next_arg.push_back(arguments.back()->begin());
    }
    size_t n = forall.size();
    for (size_t i = 0; i < n; ) {
      args.push_back(new Substitution(*forall[i], **next_arg[i]));
      const Formula& new_condition = condition.instantiation(args, problem);
      if (i + 1 == n || new_condition == Formula::FALSE) {
	if (new_condition != Formula::FALSE) {
	  effects.push_back(new Effect(new_condition,
				       add_list.substitution(args),
				       del_list.substitution(args)));
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
}


/* Returns this effect subject to the given substitutions. */
const Effect& Effect::substitution(const SubstitutionList& subst) const {
  if (forall.empty()) {
    return *(new Effect(forall, condition.substitution(subst),
			add_list.substitution(subst),
			del_list.substitution(subst)));
  } else {
    SubstitutionList eff_subst;
    for (SubstListIter si = subst.begin(); si != subst.end(); si++) {
      const Substitution& s = **si;
      if (!forall.contains(s.var)) {
	eff_subst.push_back(&s);
      }
    }
    return *(new Effect(forall, condition.substitution(eff_subst),
			add_list.substitution(eff_subst),
			del_list.substitution(eff_subst)));
  }
}


/* Fills the provided sets with predicates achievable by the
   effect. */
void Effect::achievable_predicates(hash_set<string>& preds,
				   hash_set<string>& neg_preds) const {
  for (AtomListIter gi = add_list.begin(); gi != add_list.end(); gi++) {
    preds.insert((*gi)->predicate);
  }
  for (NegationListIter gi = del_list.begin(); gi != del_list.end(); gi++) {
    neg_preds.insert((*gi)->atom.predicate);
  }
}


/* Prints this object on the given stream. */
void Effect::print(ostream& os) const {
  os << '[';
  if (condition != Formula::TRUE) {
    os << condition;
  }
  os << "->";
  if (add_list.size() + del_list.size() == 1) {
    if (!add_list.empty()) {
      os << *add_list.front();
    } else {
      os << *del_list.front();
    }
  } else {
    os << "(and";
    for (AtomListIter gi = add_list.begin(); gi != add_list.end(); gi++) {
      os << ' ' << **gi;
    }
    for (NegationListIter gi = del_list.begin(); gi != del_list.end(); gi++) {
      os << ' ' << **gi;
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
  for (const_iterator ei = begin(); ei != end(); ei++) {
    (*ei)->instantiations(effects, subst, problem);
  }
  for (const_iterator ei = effects.begin(); ei != effects.end(); ei++) {
    if ((*ei)->condition == Formula::TRUE) {
      AtomList add_list = (*ei)->add_list;
      for (const_iterator ej = effects.begin(); ej != effects.end(); ej++) {
	if ((*ej)->condition == Formula::TRUE) {
	  NegationList del_list = (*ej)->del_list;
	  for (NegationListIter gi = del_list.begin();
	       gi != del_list.end(); gi++) {
	    if (find_if(add_list.begin(), add_list.end(),
			bind1st(equal_to<const EqualityComparable*>(),
				&(*gi)->atom))
		!= add_list.end()) {
	      /* conflicting effects */
	      effects.clear();
	      return effects;
	    }
	  }
	}
      }
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


/* Fills the provided sets with predicates achievable by the effects
   in this list. */
void EffectList::achievable_predicates(hash_set<string>& preds,
				       hash_set<string>& neg_preds) const {
  for (const_iterator i = begin(); i != end(); i++) {
    (*i)->achievable_predicates(preds, neg_preds);
  }
}


/* Fills the provided sets with predicates achievable by this
   action. */
void Action::achievable_predicates(hash_set<string>& preds,
				   hash_set<string>& neg_preds) const {
  effects.achievable_predicates(preds, neg_preds);
}


/* Returns a formula representing this action. */
const Atom& ActionSchema::action_formula() const {
  TermList& terms = *(new TermList());
  for (VarListIter vi = parameters.begin(); vi != parameters.end(); vi++) {
    terms.push_back(*vi);
  }
  return *(new Atom(name, terms));
}


/* Fills the provided action list with all instantiations of this
   action schema. */
void ActionSchema::instantiations(GroundActionList& actions,
				  const Problem& problem) const {
  Vector<NameList*> arguments;
  Vector<NameListIter> next_arg;
  for (VarListIter vi = parameters.begin(); vi != parameters.end(); vi++) {
    arguments.push_back(new NameList());
    problem.compatible_objects(*arguments.back(), (*vi)->type);
    if (arguments.back()->empty()) {
      return;
    }
    next_arg.push_back(arguments.back()->begin());
  }
  SubstitutionList args;
  size_t n = parameters.size();
  for (size_t i = 0; i < n; ) {
    args.push_back(new Substitution(*parameters[i], **next_arg[i]));
    const Formula& new_precond = precondition.instantiation(args, problem);
    if (i + 1 == n || new_precond == Formula::FALSE) {
      if (new_precond != Formula::FALSE) {
	const EffectList& new_effects = effects.instantiation(args, problem);
	if (!new_effects.empty()) {
	  /* consistent instantiation */
	  NameList& names = *(new NameList());
	  for (SubstListIter si = args.begin(); si != args.end(); si++) {
	    const Name& name = dynamic_cast<const Name&>((*si)->term);
	    names.push_back(&name);
	  }
	  actions.push_back(new GroundAction(name, names, new_precond,
					     new_effects));
	}
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


/* Returns this action schema with all static preconditions assumed
   true. */
const ActionSchema& ActionSchema::strip_static(const Domain& domain) const {
  return *(new ActionSchema(name, parameters,
			    precondition.strip_static(domain), effects));
}


/* Returns this action schema with all equality/inequality
   preconditions assumed true. */
const ActionSchema& ActionSchema::strip_equality() const {
  return *(new ActionSchema(name, parameters,
			    precondition.strip_equality(), effects));
}


/* Prints this object on the given stream. */
void ActionSchema::print(ostream& os) const {
  os << '(' << name << " (";
  for (VarListIter vi = parameters.begin(); vi != parameters.end(); vi++) {
    if (vi != parameters.begin()) {
      os << ' ';
    }
    os << **vi;
    if (!(*vi)->type.object()) {
      os << " - " << (*vi)->type;
    }
  }
  os << ") ";
  if (precondition != Formula::TRUE) {
    os << precondition;
  } else {
    os << "nil";
  }
  os << " (";
  for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
    if (ei != effects.begin()) {
      os << ' ';
    }
    os << **ei;
  }
  os << ")" << ')';
}


/* Returns a formula representing this action. */
const Atom& GroundAction::action_formula() const {
  if (formula == NULL) {
    TermList& terms = *(new TermList());
    copy(arguments.begin(), arguments.end(), back_inserter(terms));
    formula = new Atom(name, terms);
  }
  return *formula;
}


/* Prints this object on the given stream. */
void GroundAction::print(ostream& os) const {
  os << '(' << name << " (";
  for (NameListIter ni = arguments.begin(); ni != arguments.end(); ni++) {
    if (ni != arguments.begin()) {
      os << ' ';
    }
    os << **ni;
  }
  os << ") ";
  if (precondition != Formula::TRUE) {
    os << precondition;
  } else {
    os << "nil";
  }
  os << " (";
  for (EffectListIter ei = effects.begin(); ei != effects.end(); ei++) {
    if (ei != effects.begin()) {
      os << ' ';
    }
    os << **ei;
  }
  os << ")" << ')';
}


/* Returns the type with the given name, or NULL if it is
   undefined. */
const Type* Domain::find_type(const string& name) const {
  TypeMapIter ti = types.find(name);
  return (ti != types.end()) ? (*ti).second : NULL;
}


/* Returns the constant with the given name, or NULL if it is
   undefined. */
const Name* Domain::find_constant(const string& name) const {
  NameMapIter ni = constants.find(name);
  return (ni != constants.end()) ? (*ni).second : NULL;
}


/* Returns the predicate with the given name, or NULL if it is
   undefined. */
const Predicate* Domain::find_predicate(const string& name) const {
  PredicateMapIter pi = predicates.find(name);
  return (pi != predicates.end()) ? (*pi).second : NULL;
}


/* Returns the action with the given name, or NULL if it is
   undefined. */
const ActionSchema* Domain::find_action(const string& name) const {
  ActionSchemaMapIter ai = actions.find(name);
  return (ai != actions.end()) ? (*ai).second : NULL;
}


/* Fills the provided name list with constants that are compatible
   with the given type. */
void Domain::compatible_constants(NameList& constants, const Type& t) const {
  for (NameMapIter ni = this->constants.begin();
       ni != this->constants.end(); ni++) {
    const Name& name = *(*ni).second;
    if (name.type.subtype(t)) {
      constants.push_back(&name);
    }
  }
}


/* Returns a set of static predicates. */
hash_set<string> Domain::static_predicates(const PredicateMap& predicates,
					   const ActionSchemaMap& actions) {
  hash_set<string> static_preds;
  hash_set<string> achievable_preds;
  for (ActionSchemaMapIter ai = actions.begin(); ai != actions.end(); ai++) {
    (*ai).second->achievable_predicates(achievable_preds, achievable_preds);
  }
  for (PredicateMapIter pi = predicates.begin();
       pi != predicates.end(); pi++) {
    const string& p = (*pi).first;
    if (achievable_preds.find(p) == achievable_preds.end()) {
      static_preds.insert(p);
    }
  }
  return static_preds;
}


/* Prints this object on the given stream. */
void Domain::print(ostream& os) const {
  os << "name: " << name;
  os << endl << "types:";
  for (TypeMapIter ti = types.begin(); ti != types.end(); ti++) {
    if (!(*ti).second->object()) {
      os << ' ' << *(*ti).second;
      if (!(*ti).second->supertype.object()) {
	os << " - " << (*ti).second->supertype;
      }
    }
  }
  os << endl << "constants:";
  for (NameMapIter ni = constants.begin(); ni != constants.end(); ni++) {
    os << ' ' << *(*ni).second;
    if (!(*ni).second->type.object()) {
      os << " - " << (*ni).second->type;
    }
  }
  os << endl << "predicates:";
  for (PredicateMapIter pi = predicates.begin();
       pi != predicates.end(); pi++) {
    os << endl << "  " << *(*pi).second;
  }
  os << endl << "actions:";
  for (ActionSchemaMapIter ai = actions.begin(); ai != actions.end(); ai++) {
    os << endl << "  " << *(*ai).second;
  }
}
