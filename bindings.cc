/*
 * Copyright (C) 2002 Carnegie Mellon University
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
 * $Id: bindings.cc,v 3.9 2002-03-24 22:03:21 lorens Exp $
 */
#include <typeinfo>
#include "bindings.h"
#include "plans.h"
#include "reasons.h"
#include "heuristics.h"
#include "domains.h"
#include "formulas.h"
#include "types.h"


const VariableSet& VariableSet::EMPTY = *(new VariableSet());


typedef Chain<const Variable*> VariableChain;


/*
 * Variable codesignation, and non-codesignation.
 */
struct Varset {
  /* The constant of this varset, or NULL. */
  const Name* const constant;
  /* The codesignation list. */
  const VariableChain* const cd_set;
  /* The non-codesignation list. */
  const VariableChain* const ncd_set;

  /* Checks if this varset includes the given name. */
  bool includes(const Name& name) const {
    return constant != NULL && *constant == name;
  }

  /* Checks if this varset includes the given variable. */
  bool includes(const Variable& var) const {
    for (const VariableChain* vc = cd_set; vc != NULL; vc = vc->tail) {
      if (*vc->head == var) {
	return true;
      }
    }
    return false;
  }

  /* Checks if this varset excludes the given variable. */
  bool excludes(const Variable& var) const {
    for (const VariableChain* vc = ncd_set; vc != NULL; vc = vc->tail) {
      if (*vc->head == var) {
	return true;
      }
    }
    return false;
  }

  /* Returns the varset obtained by adding the given name to this
     varset, or NULL if the name is inconsistent with the current
     constant. */
  const Varset* add(const Name& name) const {
    if (constant != NULL) {
      return (*constant == name) ? this : NULL;
    } else {
      return new Varset(&name, cd_set, ncd_set);
    }
  }

  /* Returns the varset obtained by adding the given variable to this
     varset, or NULL if the variable is excluded from this varset. */
  const Varset* add(const Variable& var) const {
    if (excludes(var)) {
      return NULL;
    }
    return new Varset(constant, new VariableChain(&var, cd_set), ncd_set);
  }

  /* Returns the varset obtained by adding the given term to this
     varset, or NULL if the term is excluded from this varset. */
  const Varset* add(const Term& t) const {
    const Name* name = dynamic_cast<const Name*>(&t);
    if (name != NULL) {
      return add(*name);
    } else {
      const Variable& var = dynamic_cast<const Variable&>(t);
      return add(var);
    }
  }

  /* Returns the varset obtained by adding the given variable to the
     non-codesignation list of this varset; N.B. assumes that the
     variable is not included in the varset already. */
  const Varset* restrict(const Variable& var) const {
    return new Varset(constant, cd_set, new VariableChain(&var, ncd_set));
  }

  /* Returns the combination of this and the given varset, or NULL if
     the combination is inconsistent. */
  const Varset* combine(const Varset& vs) const {
    if (constant != NULL && vs.constant != NULL &&
	*constant != *vs.constant) {
      return NULL;
    }
    const Name* comb_const = (constant != NULL) ? constant : vs.constant;
    const VariableChain* comb_cd = cd_set;
    for (const VariableChain* vc = vs.cd_set; vc != NULL; vc = vc->tail) {
      const Variable& var = *vc->head;
      if (excludes(var)) {
	return NULL;
      } else {
	comb_cd = new VariableChain(&var, comb_cd);
      }
    }
    const VariableChain* comb_ncd = ncd_set;
    for (const VariableChain* vc = vs.ncd_set; vc != NULL; vc = vc->tail) {
      const Variable& var = *vc->head;
      if (includes(var)) {
	return NULL;
      } else if (!excludes(var)) {
	comb_ncd = new VariableChain(&var, comb_ncd);
      }
    }
    return new Varset(comb_const, comb_cd, comb_ncd);
  }

  /* Returns the varset representing the given equality binding. */
  static const Varset* make_varset(const EqualityBinding& eq) {
    const VariableChain* cd_set = new VariableChain(&eq.var(), NULL);
    const Variable* v2 = dynamic_cast<const Variable*>(&eq.term());
    if (v2 != NULL) {
      cd_set = new VariableChain(v2, cd_set);
      return new Varset(NULL, cd_set, NULL);
    } else {
      const Name& n = dynamic_cast<const Name&>(eq.term());
      return new Varset(&n, cd_set, NULL);
    }
  }

  /* Returns the varset representing the given inequality binding. */
  static const Varset* make_varset(const InequalityBinding& neq,
				   bool reverse = false) {
    const Name* constant;
    const VariableChain* cd_set;
    const VariableChain* ncd_set;
    if (reverse) {
      const Variable* v2 = dynamic_cast<const Variable*>(&neq.term());
      if (v2 != NULL) {
	constant = NULL;
	cd_set = new VariableChain(v2, NULL);
      } else {
	const Name& n = dynamic_cast<const Name&>(neq.term());
	constant = &n;
	cd_set = NULL;
      }
      ncd_set = new VariableChain(&neq.var(), NULL);
    } else {
      const Variable* v2 = dynamic_cast<const Variable*>(&neq.term());
      if (v2 != NULL) {
	constant = NULL;
	cd_set = new VariableChain(&neq.var(), NULL);
	ncd_set = new VariableChain(v2, NULL);
      } else {
	return NULL;
      }
    }
    return new Varset(constant, cd_set, ncd_set);
  }

  /* Constructs a varset. */
  Varset(const Name* constant, const VariableChain* cd_set,
	 const VariableChain* ncd_set)
    : constant(constant), cd_set(cd_set), ncd_set(ncd_set) {
  }
};

/* Output operator for varsets. */
ostream& operator<<(ostream& os, const Varset& vs) {
  os << "CD{";
  for (const VariableChain* vc = vs.cd_set; vc != NULL; vc = vc->tail) {
    os << ' ' << *vc->head;
  }
  os << " }";
  if (vs.constant != NULL) {
    os << ' ' << *vs.constant;
  }
  os << " NCD{";
  for (const VariableChain* vc = vs.ncd_set; vc != NULL; vc = vc->tail) {
    os << ' ' << *vc->head;
  }
  os << " }";
  return os;
}


/*
 * A step domain.
 */
struct StepDomain : public Printable {
  const size_t id;
  const VariableList& parameters;
  const ActionDomain& domain;

  StepDomain(size_t id, const VariableList& parameters,
	     const ActionDomain& domain)
    : id(id), parameters(parameters), domain(domain) {
  }

  /* Returns the number of columns in this domain. */
  size_t width() const {
    return parameters.size();
  }

  /* Returns the index of the variable in this step domain, or -1 if
     the variable is not included. */
  int index_of(const Variable& v) const {
    VarListIter i = find_if(parameters.begin(), parameters.end(),
			    bind1st(equal_to<const EqualityComparable*>(),
				    &v));
    return (i != parameters.end()) ? i - parameters.begin() : -1;
  }

  /* Checks if this step domain includes the given name in the given
     column. */
  bool includes(const Name& name, size_t column) const {
    const NameSet& names = domain.projection(column);
    return names.find(&name) != names.end();
  }

  /* Returns the set of names from the given column. */
  const NameSet& projection(size_t column) const {
    return domain.projection(column);
  }

  /* Returns the size of the projection of the given column. */
  const size_t projection_size(size_t column) const {
    return domain.projection_size(column);
  }

  /* Returns a domain where the given column has been restricted to
     the given name, or NULL if this would leave an empty domain. */
  const StepDomain* restrict(const Name& name, size_t column) const {
    const ActionDomain* ad = domain.restrict(name, column);
    if (ad == NULL) {
      return NULL;
    } else if (ad->size() == domain.size()) {
      return this;
    } else {
      return new StepDomain(id, parameters, *ad);
    }
  }

  /* Returns a domain where the given column has been restricted to
     the given set of names, or NULL if this would leave an empty
     domain. */
  const StepDomain* restrict(const NameSet& names, size_t column) const {
    const ActionDomain* ad = domain.restrict(names, column);
    if (ad == NULL) {
      return NULL;
    } else if (ad->size() == domain.size()) {
      return this;
    } else {
      return new StepDomain(id, parameters, *ad);
    }
  }

  /* Returns a domain where the given column exclues the given name,
     or NULL if this would leave an empty domain. */
  const StepDomain* exclude(const Name& name, size_t column) const {
    const ActionDomain* ad = domain.exclude(name, column);
    if (ad == NULL) {
      return NULL;
    } else if (ad->size() == domain.size()) {
      return this;
    } else {
      return new StepDomain(id, parameters, *ad);
    }
  }

protected:
  /* Prints this object on the given stream. */
  virtual void print(ostream& os) const {
    os << "<";
    for (VarListIter vi = parameters.begin(); vi != parameters.end(); vi++) {
      if (vi != parameters.begin()) {
	os << ' ';
      }
      os << **vi;
    }
    os << "> in " << domain;
  }
};


/* Returns the varset containing the given constant, or NULL if none do. */
static const Varset* find_varset(const VarsetChain* varsets,
				 const Name& constant) {
  for (const VarsetChain* vsc = varsets; vsc != NULL; vsc = vsc->tail) {
    const Varset* vs = vsc->head;
    if (vs->includes(constant)) {
      return vs;
    }
  }
  return NULL;
}


/* Returns the varset containing the given variable, or NULL if none do. */
static const Varset* find_varset(const VarsetChain* varsets,
				 const Variable& var) {
  for (const VarsetChain* vsc = varsets; vsc != NULL; vsc = vsc->tail) {
    const Varset* vs = vsc->head;
    if (vs->includes(var)) {
      return vs;
    }
  }
  return NULL;
}


/* Returns the varset containing the given term, or NULL if none do. */
static const Varset* find_varset(const VarsetChain* varsets, const Term& t) {
  const Name* name = dynamic_cast<const Name*>(&t);
  if (name != NULL) {
    return find_varset(varsets, *name);
  } else {
    const Variable& var = dynamic_cast<const Variable&>(t);
    return find_varset(varsets, var);
  }
}


/* Returns the step domain containing the given variable, or NULL if
   none does. */
static pair<const StepDomain*, size_t>
find_step_domain(const StepDomainChain* step_domains, const Variable& var) {
  const StepVar* sv = dynamic_cast<const StepVar*>(&var);
  if (sv != NULL) {
    size_t id = sv->id;
    for (const StepDomainChain* sd = step_domains; sd != NULL; sd = sd->tail) {
      const StepDomain& step_domain = *sd->head;
      if (step_domain.id == id) {
	int column = step_domain.index_of(var);
	if (column >= 0) {
	  return make_pair(&step_domain, column);
	} else {
	  break;
	}
      }
    }
  }
  return pair<const StepDomain*, size_t>(NULL, 0);
}


/* ====================================================================== */
/* Binding */

/* Constructs an abstract variable binding. */
Binding::Binding(const Variable& var, const Term& term, const Reason& reason)
  : var_(&var), term_(&term) {
#ifdef TRANSFORMATIONAL
  reason_ = & reason;
#endif
}


/* Returns the reason for this binding. */
const Reason& Binding::reason() const {
#ifdef TRANSFORMATIONAL
  return *reason_;
#else
  return Reason::DUMMY;
#endif
}


/* ====================================================================== */
/* EqualityBinding */

/* Constructs an equality binding from the given substitution. */
EqualityBinding::EqualityBinding(const Substitution& s, const Reason& reason)
  : Binding(s.var(), s.term(), reason) {
}


/* Construct an equality binding, binding the given variable to the
   given term. */
EqualityBinding::EqualityBinding(const Variable& var, const Term& term,
				 const Reason& reason)
  : Binding(var, term, reason) {}


/* Prints this equality binding on the given stream. */
void EqualityBinding::print(ostream& os) const {
  os << var() << '=' << term();
}


/* ====================================================================== */
/* InequalityBinding */

/* Constructs an inequality binding from the given substitution. */
InequalityBinding::InequalityBinding(const Substitution& s,
				     const Reason& reason)
  : Binding(s.var(), s.term(), reason) {
}


/* Constructs an inequality binding, separating the given variable
   from the given term. */
InequalityBinding::InequalityBinding(const Variable& var, const Term& term,
				     const Reason& reason)
  : Binding(var, term, reason) {}


/* Prints this inequality binding on the given stream. */
void InequalityBinding::print(ostream& os) const {
  os << var() << "!=" << term();
}


/* ====================================================================== */
/* ActionDomain */

/* Constructs an action domain with a single tuple. */
ActionDomain::ActionDomain(const NameList& tuple) {
  for (size_t i = 0; i < tuple.size(); i++) {
    projections.push_back(new NameSet());
  }
  add(tuple);
}


/* Number of tuples. */
size_t ActionDomain::size() const {
  return tuples.size();
}


/* Adds a tuple to this domain. */
void ActionDomain::add(const NameList& tuple) {
  tuples.push_back(&tuple);
  for (size_t i = 0; i < tuple.size(); i++) {
    projections[i]->insert(tuple[i]);
  }
}


/* Returns the set of names from the given column. */
const NameSet& ActionDomain::projection(size_t column) const {
  return *projections[column];
}


/* Returns the size of the projection of the given column. */
const size_t ActionDomain::projection_size(size_t column) const {
  return projections[column]->size();
}


/* Returns a domain where the given column has been restricted to
   the given name, or NULL if this would leave an empty domain. */
const ActionDomain* ActionDomain::restrict(const Name& name,
					   size_t column) const {
  ActionDomain* new_domain = NULL;
  for (TupleListIter ti = tuples.begin(); ti != tuples.end(); ti++) {
    const NameList& tuple = **ti;
    if (*tuple[column] == name) {
      if (new_domain == NULL) {
	new_domain = new ActionDomain(tuple);
      } else {
	new_domain->add(tuple);
      }
    }
  }
  return new_domain;
}


/* Returns a domain where the given column has been restricted to
   the given set of names, or NULL if this would leave an empty
   domain. */
const ActionDomain* ActionDomain::restrict(const NameSet& names,
					   size_t column) const {
  ActionDomain* new_domain = NULL;
  for (TupleListIter ti = tuples.begin(); ti != tuples.end(); ti++) {
    const NameList& tuple = **ti;
    if (names.find(tuple[column]) != names.end()) {
      if (new_domain == NULL) {
	new_domain = new ActionDomain(tuple);
      } else {
	new_domain->add(tuple);
      }
    }
  }
  return new_domain;
}


/* Returns a domain where the given column exclues the given name,
   or NULL if this would leave an empty domain. */
const ActionDomain* ActionDomain::exclude(const Name& name,
					  size_t column) const {
  ActionDomain* new_domain = NULL;
  for (TupleListIter ti = tuples.begin(); ti != tuples.end(); ti++) {
    const NameList& tuple = **ti;
    if (*tuple[column] != name) {
      if (new_domain == NULL) {
	new_domain = new ActionDomain(tuple);
      } else {
	new_domain->add(tuple);
      }
    }
  }
  return new_domain;
}


/* Prints this object on the given stream. */
void ActionDomain::print(ostream& os) const {
  os << '{';
  for (TupleListIter ti = tuples.begin(); ti != tuples.end(); ti++) {
    if (ti != tuples.begin()) {
      os << ' ';
    }
    os << '<';
    const NameList& tuple = **ti;
    for (NameListIter ni = tuple.begin(); ni != tuple.end(); ni++) {
      if (ni != tuple.begin()) {
	os << ' ';
      }
      os << **ni;
    }
    os << '>';
  }
  os << '}';
}


/* ====================================================================== */
/* Bindings */

/* Creates a binding collection with parameter constrains if pg is
   not NULL, or an empty binding collection otherwise. */
const Bindings& Bindings::make_bindings(const StepChain* steps,
					const PlanningGraph* pg) {
  const StepDomainChain* step_domains = NULL;
  hash_set<size_t> seen_steps;
  for (const StepChain* sc = steps; sc != NULL; sc = sc->tail) {
    const Step& step = sc->head;
    if (step.action() != NULL
	&& seen_steps.find(step.id()) == seen_steps.end()) {
      seen_steps.insert(step.id());
      const ActionSchema* action =
	dynamic_cast<const ActionSchema*>(step.action());
      if (action != NULL && !action->parameters.empty()) {
	const ActionDomain* domain = pg->action_domain(action->name);
	if (domain != NULL) {
	  const StepDomain* step_domain =
	    new StepDomain(step.id(),
			   action->parameters.instantiation(step.id()),
			   *domain);
	  step_domains = new StepDomainChain(step_domain, step_domains);
	}
      }
    }
  }
  return *(new Bindings(NULL, 0, step_domains, NULL, NULL));
}


/* Creates a collection of variable bindings with the given equality
   and inequality bindings. */
const Bindings* Bindings::make_bindings(const StepChain* steps,
					const PlanningGraph* pg,
					const BindingChain* equalities,
					const BindingChain* inequalities) {
  const Bindings* b = &make_bindings(steps, pg);
  BindingList bindings;
  for (const BindingChain* bc = equalities; bc != NULL; bc = bc->tail) {
    bindings.push_back(bc->head);
  }
  for (const BindingChain* bc = inequalities; bc != NULL; bc = bc->tail) {
    bindings.push_back(bc->head);
  }
  const Bindings* new_b = b->add(bindings);
  if (new_b != b) {
    delete b;
  }
  return new_b;
}


/* Checks if the given formulas can be unified. */
bool Bindings::unifiable(const Literal& l1, const Literal& l2) {
#ifdef DEBUG
  created_collectibles--;
  deleted_collectibles--;
#endif
  return Bindings(NULL, 0, NULL, NULL, NULL).unify(l1, l2);
}


/* Checks if the given formulas can be unified; the most general
   unifier is added to the provided substitution list. */
bool Bindings::unifiable(SubstitutionList& mgu,
			 const Literal& l1, const Literal& l2) {
#ifdef DEBUG
  created_collectibles--;
  deleted_collectibles--;
#endif
  return Bindings(NULL, 0, NULL, NULL, NULL).unify(mgu, l1, l2);
}


/* Constructs a binding collection. */
Bindings::Bindings(const VarsetChain* varsets, size_t high_step,
		   const StepDomainChain* step_domains,
		   const BindingChain* equalities,
		   const BindingChain* inequalities)
  : varsets_(varsets), high_step_(high_step), step_domains_(step_domains) {
#ifdef TRANSFORMATIONAL
  equalities_ = equalities;
  inequalities_ = inequalities;
#endif
}


/* Returns the equality bindings. */
const BindingChain* Bindings::equalities() const {
#ifdef TRANSFORMATIONAL
  return equalities_;
#else
  return NULL;
#endif
}


/* Return the inequality bindings. */
const BindingChain* Bindings::inequalities() const {
#ifdef TRANSFORMATIONAL
  return inequalities_;
#else
  return NULL;
#endif
}


/* Returns the binding for the given term, or the term itself if it is
     not bound to a single name. */
const Term& Bindings::binding(const Term& t) const {
  const Variable* var = dynamic_cast<const Variable*>(&t);
  if (var != NULL) {
    const Varset* vs = find_varset(varsets_, *var);
    if (vs != NULL && vs->constant != NULL) {
      return *vs->constant;
    }
  }
  return t;
}


/* Returns the domain for the given step variable. */
const NameSet* Bindings::domain(const Variable& v) const {
  pair<const StepDomain*, size_t> sd = find_step_domain(step_domains_, v);
  return (sd.first != NULL) ? &sd.first->projection(sd.second) : NULL;
}


/* Checks if one of the given formulas is the negation of the other,
   and the atomic formulas can be unified. */
bool Bindings::affects(const Literal& l1, const Literal& l2) const {
  SubstitutionList dummy;
  return affects(dummy, l1, l2);
}


/* Checks if one of the given formulas is the negation of the other,
   and the atomic formulas can be unified; the most general unifier
   is added to the provided substitution list. */
bool Bindings::affects(SubstitutionList& mgu,
		       const Literal& l1, const Literal& l2) const {
  const Negation* negation = dynamic_cast<const Negation*>(&l1);
  if (negation != NULL) {
    return unify(mgu, l2, negation->atom);
  } else {
    negation = dynamic_cast<const Negation*>(&l2);
    if (negation != NULL) {
      return unify(mgu, negation->atom, l1);
    } else {
      return false;
    }
  }
}


/* Checks if the given formulas can be unified. */
bool Bindings::unify(const Literal& l1, const Literal& l2) const {
  SubstitutionList dummy;
  return unify(dummy, l1, l2);
}

/* Checks if the given formulas can be unified; the most general
   unifier is added to the provided substitution list. */
bool Bindings::unify(SubstitutionList& mgu,
		     const Literal& l1, const Literal& l2) const {
  if (typeid(l1) != typeid(l2)) {
    /* Not the same type of literal. */
    return false;
  }

  if (l1.predicate() != l2.predicate()) {
    /* The predicates do not match. */
    return false;
  }

  /*
   * Try to unify the terms of the literals.
   */
  /* Terms of the first literal. */
  const TermList& terms1 = l1.terms();
  /* Terms of the second literal. */
  const TermList& terms2 = l2.terms();
  if (terms1.size() != terms2.size()) {
    /* Term lists of different size. */
    return false;
  }
  BindingList bl;
  for (TermListIter ti = terms1.begin(), tj = terms2.begin();
       ti != terms1.end(); ti++, tj++) {
    /*
     * Try to unify a pair of terms.
     */
    const Term& term1 = **ti;
    const Term& term2 = **tj;
    const Name* name1 = dynamic_cast<const Name*>(&term1);
    if (name1 != NULL) {
      /* The first term is a name. */
      const Name* name2 = dynamic_cast<const Name*>(&term2);
      if (name2 != NULL) {
	/*
	 * Both terms are names.
	 */
	if (*name1 != *name2) {
	  /* The two terms are different names. */
	  return false;
	}
      } else {
	/*
	 * The first term is a name and the second is a variable.
	 */
	const Variable& var2 = dynamic_cast<const Variable&>(term2);
	if (!term1.type().subtype(term2.type())) {
	  /* Incompatible term types. */
	  return false;
	}
	bl.push_back(new EqualityBinding(var2, *name1, Reason::DUMMY));
      }
    } else {
      /* The first term is a variable. */
      const Variable& var1 = dynamic_cast<const Variable&>(term1);
      const Name* name2 = dynamic_cast<const Name*>(&term2);
      if ((name2 != NULL && !term2.type().subtype(term1.type()))
	  || (name2 == NULL && !(term1.type().subtype(term2.type())
				 || term2.type().subtype(term1.type())))) {
	/* Incompatible term types. */
	return false;
      }
      bl.push_back(new EqualityBinding(var1, term2, Reason::DUMMY));
    }
  }
  if (add(bl, true) == NULL) {
    /* Unification is inconsistent with current bindings. */
    return false;
  }
  for (BindingListIter bi = bl.begin(); bi != bl.end(); bi++) {
    /* Add unification to most general unifier. */
    const Binding& b = **bi;
    mgu.push_back(Substitution(b.var(), b.term()));
  }
  /* Successful unification. */
  return true;
}


/* Checks if the given equality is consistent with the current
   bindings. */
bool Bindings::consistent_with(const Equality& eq) const {
  const Variable* var = dynamic_cast<const Variable*>(&eq.term1);
  const Term* term;
  if (var != NULL) {
    term = &eq.term2;
  } else {
    var = dynamic_cast<const Variable*>(&eq.term2);
    if (var != NULL) {
      term = &eq.term1;
    } else {
      return eq.term1 == eq.term2;
    }
  }
  const Varset* vs2 = find_varset(varsets_, *term);
  if (vs2 == NULL || vs2->includes(*var)) {
    return true;
  } else if (vs2->excludes(*var)) {
    return false;
  } else if (vs2->constant != NULL) {
    pair<const StepDomain*, size_t> sd = find_step_domain(step_domains_, *var);
    if (sd.first != NULL) {
      return sd.first->includes(*vs2->constant, sd.second);
    }
  }
  return true;
}


/* Checks if the given inequality is consistent with the current
   bindings. */
bool Bindings::consistent_with(const Inequality& neq) const {
  const Variable* var = dynamic_cast<const Variable*>(&neq.term1);
  const Term* term;
  if (var != NULL) {
    term = &neq.term2;
  } else {
    var = dynamic_cast<const Variable*>(&neq.term2);
    if (var != NULL) {
      term = &neq.term1;
    } else {
      return neq.term1 != neq.term2;
    }
  }
  const Varset* vs2 = find_varset(varsets_, *term);
  return vs2 == NULL || !vs2->includes(*var) || vs2->excludes(*var);
}


/* Adds bindings to the list as determined by difference between the
   given step domains */
static void add_domain_bindings(BindingList& bindings,
				const StepDomain& old_sd,
				const StepDomain& new_sd,
				size_t ex_column = UINT_MAX) {
  for (size_t c = 0; c < old_sd.width(); c++) {
    if (c != ex_column && new_sd.projection_size(c) == 1
	&& old_sd.projection_size(c) > 1) {
      const EqualityBinding* new_eq =
	new EqualityBinding(*new_sd.parameters[c],
			    **new_sd.projection(c).begin(), Reason::DUMMY);
      bindings.push_back(new_eq);
    }
  }
}


/* Returns the binding collection obtained by adding the given
   bindings to this binding collection, or NULL if the new bindings
   are inconsistent with the current. */
const Bindings* Bindings::add(const BindingList& new_bindings,
			      bool test_only) const {
  if (new_bindings.empty()) {
    /* No new bindings. */
    return this;
  }

  /* Equality bindings for new binding collection. */
  const BindingChain* equalities = this->equalities();
  /* Inequality bindings for new binding collection. */
  const BindingChain* inequalities = this->inequalities();
  /* Varsets for new binding collection */
  const VarsetChain* varsets = varsets_;
  /* Highest step id of variable in varsets. */
  size_t high_step = high_step_;
  /* Variables above previous high step. */
  VariableSet high_step_vars;
  /* Step domains for new binding collection */
  const StepDomainChain* step_domains = step_domains_;

  BindingList new_binds(new_bindings);
  /*
   * Add new bindings one at a time.
   */
  for (size_t i = 0; i < new_binds.size(); i++) {
    const Binding* bind = new_binds[i];
    const EqualityBinding* eq = dynamic_cast<const EqualityBinding*>(bind);
    if (eq != NULL) {
      /*
       * Adding equality binding.
       */
      /* Varset for variable. */
      const Varset* vs1;
      const StepVar* sv = dynamic_cast<const StepVar*>(&eq->var());
      if (sv == NULL || sv->id <= high_step_
	  || high_step_vars.find(sv) != high_step_vars.end()) {
	vs1 = find_varset(varsets, eq->var());
      } else {
	high_step = sv->id;
	high_step_vars.insert(sv);
	vs1 = NULL;
      }
      /* Varset for term. */
      const Varset* vs2;
      sv = dynamic_cast<const StepVar*>(&eq->term());
      if (sv == NULL || sv->id <= high_step_
	  || high_step_vars.find(sv) != high_step_vars.end()) {
	vs2 = find_varset(varsets, eq->term());
      } else {
	high_step = sv->id;
	high_step_vars.insert(sv);
	vs2 = NULL;
      }
      /* Combined varset, or NULL if binding is inconsistent with
         current bindings. */
      const Varset* comb;
      if (vs1 != NULL || vs2 != NULL) {
	/* At least one of the terms is already bound. */
	if (vs1 != vs2) {
	  /* The terms are not yet bound to eachother. */
	  if (vs1 == NULL) {
	    /* The first term is unbound, so add it to the varset of
               the second. */
	    comb = vs2->add(eq->var());
	  } else if (vs2 == NULL) {
	    /* The second term is unbound, so add it to the varset of
               the first. */
	    comb = vs1->add(eq->term());
	  } else {
	    /* Both terms are bound, so combine their varsets. */
	    comb = vs1->combine(*vs2);
	  }
	} else {
	  /* The terms are already bound to eachother. */
	  comb = vs1;
	}
      } else {
	/* None of the terms are already bound. */
	comb = Varset::make_varset(*eq);
      }
      if (comb == NULL) {
	/* Binding is inconsistent with current bindings. */
	return NULL;
      } else {
	/* Binding is consistent with current bindings. */
	if (comb != vs1) {
	  /* Combined varset is new, so add it to the chain of varsets. */
	  varsets = new VarsetChain(comb, varsets);
	  const Name* name = comb->constant;
	  /* Restrict step domain for all codesignated variables. */
	  const NameSet* intersection = NULL;
	  const VariableChain* vc = NULL;
	  int phase = 0;
	  while (phase < 4 || (intersection != NULL && phase < 8)) {
	    const Variable* var = NULL;
	    switch (phase) {
	    case 0:
	    case 4:
	      if (vs1 == NULL) {
		var = &eq->var();
		phase += 2;
	      } else if (vs1->constant == NULL) {
		vc = vs1->cd_set;
		phase++;
	      } else {
		phase += 2;
	      }
	      break;
	    case 1:
	    case 3:
	    case 5:
	    case 7:
	      if (vc != NULL) {
		var = vc->head;
		vc = vc->tail;
	      } else {
		var = NULL;
		phase++;
	      }
	      break;
	    case 2:
	    case 6:
	      if (vs2 == NULL) {
		var = dynamic_cast<const Variable*>(&eq->term());
		phase += 2;
	      } else if (vs2->constant == NULL) {
		vc = vs2->cd_set;
		phase++;
	      } else {
		phase += 2;
	      }
	      break;
	    }
	    if (var != NULL) {
	      /* Step domain for variable. */
	      pair<const StepDomain*, size_t> sd =
		find_step_domain(step_domains, *var);
	      if (sd.first != NULL) {
		if (name != NULL) {
		  const StepDomain* new_sd =
		    sd.first->restrict(*name, sd.second);
		  if (new_sd == NULL) {
		    /* Domain became empty. */
		    return NULL;
		  }
		  if (sd.first != new_sd) {
		    add_domain_bindings(new_binds, *sd.first, *new_sd,
					sd.second);
		    step_domains = new StepDomainChain(new_sd, step_domains);
		  }
		} else {
		  if (phase > 4) {
		    const StepDomain* new_sd =
		      sd.first->restrict(*intersection, sd.second);
		    if (new_sd == NULL) {
		      /* Domain became empty. */
		      return NULL;
		    }
		    if (sd.first != new_sd) {
		      add_domain_bindings(new_binds, *sd.first, *new_sd);
		      step_domains = new StepDomainChain(new_sd, step_domains);
		    }
		  } else if (intersection == NULL) {
		    intersection = &sd.first->projection(sd.second);
		  } else {
		    NameSet* cut = new NameSet();
		    const NameSet& set2 = sd.first->projection(sd.second);
		    set_intersection(intersection->begin(),
				     intersection->end(),
				     set2.begin(), set2.end(),
				     inserter(*cut, cut->begin()),
				     less<const LessThanComparable*>());
		    intersection = cut;
		    if (intersection->empty()) {
		      /* Domain became empty. */
		      return NULL;
		    }
		  }
		}
	      }
	    }
	  }
	}
#if TRANSFORMATIONAL
	/* Add binding to chain of equality bindings. */
	if (!eq->reason().dummy()) {
	  equalities = new BindingChain(eq, equalities);
	}
#endif
      }
    } else {
      /*
       * Adding inequality binding.
       */
      const InequalityBinding& neq =
	dynamic_cast<const InequalityBinding&>(*bind);
      /* Varset for variable. */
      const Varset* vs1;
      const StepVar* sv = dynamic_cast<const StepVar*>(&neq.var());
      if (sv == NULL || sv->id <= high_step_
	  || high_step_vars.find(sv) != high_step_vars.end()) {
	vs1 = find_varset(varsets, neq.var());
      } else {
	high_step = sv->id;
	high_step_vars.insert(sv);
	vs1 = NULL;
      }
      /* Varset for term. */
      const Varset* vs2;
      sv = dynamic_cast<const StepVar*>(&neq.term());
      if (sv == NULL || sv->id <= high_step_
	  || high_step_vars.find(sv) != high_step_vars.end()) {
	vs2 = find_varset(varsets, neq.term());
      } else {
	high_step = sv->id;
	high_step_vars.insert(sv);
	vs2 = NULL;
      }
      if (vs1 != NULL && vs2 != NULL && vs1 == vs2) {
	/* The terms are already bound to eachother. */
	return NULL;
      } else {
	/* The terms are not bound to eachother. */
	bool separate1 = true;
	bool separate2 = true;
	if (vs1 == NULL) {
	  /* The first term is unbound, so create a new varset for it. */
	  vs1 = Varset::make_varset(neq);
	} else {
	  const Variable* v2 = dynamic_cast<const Variable*>(&neq.term());
	  if (v2 != NULL) {
	    /* The second term is a variable. */
	    if (vs1->excludes(*v2)) {
	      /* The second term is already separated from the first. */
	      separate1 = false;
	    } else {
	      /* Separate the second term from the first. */
	      vs1 = vs1->restrict(*v2);
	    }
	  } else {
	    /* The second term is a name, so the terms are separated
               in the varset for the second term. */
	    separate1 = false;
	  }
	}
	if (vs2 == NULL) {
	  /* The second term is unbound, so create a new varset for it. */
	  vs2 = Varset::make_varset(neq, true);
	} else if (vs2->excludes(neq.var())) {
	  /* The first term is already separated from the second. */
	  separate2 = false;
	} else {
	  /* Separate the first term from the second. */
	  vs2 = vs2->restrict(neq.var());
	}
	if (separate1 && vs1 != NULL) {
	  /* The second term was not separated from the first already. */
	  varsets = new VarsetChain(vs1, varsets);
	  if (vs1->constant != NULL && vs2 != NULL) {
	    for (const VariableChain* vc = vs2->cd_set;
		 vc != NULL; vc = vc->tail) {
	      pair<const StepDomain*, size_t> sd =
		find_step_domain(step_domains, *vc->head);
	      if (sd.first != NULL) {
		const StepDomain* new_sd =
		  sd.first->exclude(*vs1->constant, sd.second);
		if (new_sd == NULL) {
		  /* Domain became empty. */
		  return NULL;
		}
		if (sd.first != new_sd) {
		  add_domain_bindings(new_binds, *sd.first, *new_sd);
		  step_domains = new StepDomainChain(new_sd, step_domains);
		}
	      }
	    }
	  }
	}
	if (separate2 && vs2 != NULL) {
	  /* The first term was not separated from the second already. */
	  varsets = new VarsetChain(vs2, varsets);
	  if (vs2->constant != NULL) {
	    const VariableChain* vc = (vs1 != NULL) ? vs1->cd_set : NULL;
	    const Variable* var = (vc != NULL) ? vc->head : &neq.var();
	    while (var != NULL) {
	      pair<const StepDomain*, size_t> sd =
		find_step_domain(step_domains, *var);
	      if (sd.first != NULL) {
		const StepDomain* new_sd =
		  sd.first->exclude(*vs2->constant, sd.second);
		if (new_sd == NULL) {
		  /* Domain became empty. */
		  return NULL;
		}
		if (sd.first != new_sd) {
		  add_domain_bindings(new_binds, *sd.first, *new_sd);
		  step_domains = new StepDomainChain(new_sd, step_domains);
		}
	      }
	      vc = (vs1 != NULL) ? vc->tail : NULL;
	      var = (vc != NULL) ? vc->head : NULL;
	    }
	  }
	}
      }
#ifdef TRANSFORMATIONAL
      /* Add binding to chain of inequality bindings. */
      if (!neq.reason().dummy()) {
	inequalities = new BindingChain(&neq, inequalities);
      }
#endif
    }
  }
  /* New bindings are consistent with the current bindings. */
  if (test_only) {
    return this;
  } else {
    return new Bindings(varsets, high_step, step_domains,
			equalities, inequalities);
  }
}


/* Returns the binding collection obtained by adding the constraints
   associated with the given step to this binding collection, or
   NULL if the new binding collection would be inconsistent. */
const Bindings* Bindings::add(size_t step_id, const Action* step_action,
			      const PlanningGraph& pg, bool test_only) const {
  if (step_action == NULL) {
    return this;
  }
  const ActionSchema* action = dynamic_cast<const ActionSchema*>(step_action);
  if (action == NULL || action->parameters.empty()) {
    return this;
  }
  const ActionDomain* domain = pg.action_domain(action->name);
  if (domain == NULL) {
    return NULL;
  }
  const StepDomain* step_domain =
    new StepDomain(step_id, action->parameters.instantiation(step_id),
		   *domain);
  const StepDomainChain* step_domains = new StepDomainChain(step_domain,
							    step_domains_);
  const VarsetChain* varsets = varsets_;
  size_t high_step = high_step_;
  for (size_t c = 0; c < step_domain->width(); c++) {
    if (step_domain->projection_size(c) == 1) {
      const VariableChain* cd_set =
	new VariableChain(step_domain->parameters[c], NULL);
      varsets = new VarsetChain(new Varset(*step_domain->projection(c).begin(),
					   cd_set, NULL), varsets);
      high_step = max(high_step, step_id);
    }
  }
  if (test_only) {
    return this;
  } else {
    return new Bindings(varsets, high_step, step_domains,
			equalities(), inequalities());
  }
}


/* Prints this binding collection on the given stream. */
void Bindings::print(ostream& os) const {
  Vector<const Term*> seen_terms;
  for (const VarsetChain* vsc = varsets_; vsc != NULL; vsc = vsc->tail) {
    const Varset& vs = *vsc->head;
    if (vs.cd_set != NULL) {
      const VariableChain* vc = vs.cd_set;
      if (member_if(seen_terms.begin(), seen_terms.end(),
		    bind1st(equal_to<const EqualityComparable*>(),
			    vc->head))) {
	continue;
      }
      os << endl << "{";
      for (; vc != NULL; vc = vc->tail) {
	const Variable& var = *vc->head;
	os << ' ' << var;
	seen_terms.push_back(&var);
      }
      os << " }";
      if (vs.constant != NULL) {
	os << " == ";
      }
    }
    if (vs.constant != NULL) {
      const Name& name = *vs.constant;
      if (member_if(seen_terms.begin(), seen_terms.end(),
		    bind1st(equal_to<const EqualityComparable*>(), &name))) {
	continue;
      }
      if (vs.cd_set == NULL) {
	os << endl;
      }
      os << name;
      seen_terms.push_back(&name);
    }
    if (vs.ncd_set != NULL) {
      os << " != {";
      for (const VariableChain* vc = vs.ncd_set; vc != NULL; vc = vc->tail) {
	os << ' ' << *vc->head;
      }
      os << " }";
    }
  }
  hash_set<size_t> seen_steps;
  for (const StepDomainChain* sd = step_domains_; sd != NULL; sd = sd->tail) {
    if (seen_steps.find(sd->head->id) == seen_steps.end()) {
      seen_steps.insert(sd->head->id);
      os << endl << *sd->head;
    }
  }
}
