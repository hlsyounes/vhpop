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
 * $Id: bindings.cc,v 6.6 2003-08-28 16:48:25 lorens Exp $
 */
#include "bindings.h"
#include "plans.h"
#include "heuristics.h"
#include "domains.h"
#include "formulas.h"
#include "types.h"
#include "debug.h"
#include <typeinfo>


/* ====================================================================== */
/* StepVariable */

typedef std::pair<Variable, size_t> StepVariable;


/* ====================================================================== */
/* VariableSet */

/*
 * A set of variables.
 */
struct VariableSet : public std::set<StepVariable> {
};


/* ====================================================================== */
/* Varset */

/*
 * Variable codesignation, and non-codesignation.
 */
struct Varset {
  /* Constructs a varset. */
  Varset(Object constant, const Chain<StepVariable>* cd_set,
	 const Chain<StepVariable>* ncd_set)
    : constant_(constant), cd_set_(cd_set), ncd_set_(ncd_set) {
    Chain<StepVariable>::register_use(cd_set_);
    Chain<StepVariable>::register_use(ncd_set_);
  }

  /* Constructs a varset. */
  Varset(const Varset& vs)
    : constant_(vs.constant_), cd_set_(vs.cd_set_), ncd_set_(vs.ncd_set_) {
    Chain<StepVariable>::register_use(cd_set_);
    Chain<StepVariable>::register_use(ncd_set_);
  }

  /* Deletes this varset. */
  ~Varset() {
    Chain<StepVariable>::unregister_use(cd_set_);
    Chain<StepVariable>::unregister_use(ncd_set_);
  }

  /* The constant of this varset, or NULL. */
  Object constant() const { return constant_; }

  /* The codesignation list. */
  const Chain<StepVariable>* cd_set() const { return cd_set_; }

  /* The non-codesignation list. */
  const Chain<StepVariable>* ncd_set() const { return ncd_set_; }

  /* Checks if this varset includes the given name. */
  bool includes(Object name) const { return constant() == name; }

  /* Checks if this varset includes the given variable. */
  bool includes(Variable var, size_t step_id) const {
    for (const Chain<StepVariable>* vc = cd_set(); vc != NULL; vc = vc->tail) {
      if (vc->head.first == var && vc->head.second == step_id) {
	return true;
      }
    }
    return false;
  }

  /* Checks if this varset excludes the given variable. */
  bool excludes(Variable var, size_t step_id) const {
    for (const Chain<StepVariable>* vc = ncd_set();
	 vc != NULL; vc = vc->tail) {
      if (vc->head.first == var && vc->head.second == step_id) {
	return true;
      }
    }
    return false;
  }

  /* Returns the varset obtained by adding the given name to this
     varset, or NULL if the name is inconsistent with the current
     constant. */
  const Varset* add(const Chain<Varset>*& vsc, Object name) const {
    if (constant() != Object(NULL_TERM)) {
      return (constant() == name) ? this : NULL;
    } else {
      vsc = new Chain<Varset>(Varset(name, cd_set(), ncd_set()), vsc);
      return &vsc->head;
    }
  }

  /* Returns the varset obtained by adding the given term to this
     varset, or NULL if the term is excluded from this varset. */
  const Varset* add(const Chain<Varset>*& vsc, Term t, size_t step_id) const {
    if (is_object(t)) {
      return add(vsc, t);
    } else if (excludes(t, step_id)) {
      return NULL;
    } else {
      const Chain<StepVariable>* new_cd =
	new Chain<StepVariable>(std::make_pair(t, step_id), cd_set());
      vsc = new Chain<Varset>(Varset(constant(), new_cd, ncd_set()), vsc);
      return &vsc->head;
    }
  }

  /* Returns the varset obtained by adding the given variable to the
     non-codesignation list of this varset; N.B. assumes that the
     variable is not included in the varset already. */
  const Varset* restrict(const Chain<Varset>*& vsc,
			 Variable var, size_t step_id) const {
    const Chain<StepVariable>* new_ncd =
      new Chain<StepVariable>(std::make_pair(var, step_id), ncd_set());
    vsc = new Chain<Varset>(Varset(constant(), cd_set(), new_ncd), vsc);
    return &vsc->head;
  }

  /* Returns the combination of this and the given varset, or NULL if
     the combination is inconsistent. */
  const Varset* combine(const Chain<Varset>*& vsc, const Varset& vs) const {
    if (constant() != Object(NULL_TERM) && vs.constant() != Object(NULL_TERM)
	&& constant() != vs.constant()) {
      return NULL;
    }
    Object comb_const =
      (constant() != Object(NULL_TERM)) ? constant() : vs.constant();
    const Chain<StepVariable>* comb_cd = cd_set();
    for (const Chain<StepVariable>* vc = vs.cd_set();
	 vc != NULL; vc = vc->tail) {
      const StepVariable& step_var = vc->head;
      if (excludes(step_var.first, step_var.second)) {
	Chain<StepVariable>::register_use(comb_cd);
	Chain<StepVariable>::unregister_use(comb_cd);
	return NULL;
      } else {
	comb_cd = new Chain<StepVariable>(step_var, comb_cd);
      }
    }
    const Chain<StepVariable>* comb_ncd = ncd_set();
    for (const Chain<StepVariable>* vc = vs.ncd_set();
	 vc != NULL; vc = vc->tail) {
      const StepVariable& step_var = vc->head;
      if (includes(step_var.first, step_var.second)) {
	Chain<StepVariable>::register_use(comb_cd);
	Chain<StepVariable>::unregister_use(comb_cd);
	Chain<StepVariable>::register_use(comb_ncd);
	Chain<StepVariable>::unregister_use(comb_ncd);
	return NULL;
      } else if (!excludes(step_var.first, step_var.second)) {
	comb_ncd = new Chain<StepVariable>(step_var, comb_ncd);
      }
    }
    vsc = new Chain<Varset>(Varset(comb_const, comb_cd, comb_ncd), vsc);
    return &vsc->head;
  }

  /* Returns the varset representing the given equality binding. */
  static const Varset* make(const Chain<Varset>*& vsc, const Binding& b,
			    bool reverse = false) {
    if (b.equality()) {
      const Chain<StepVariable>* cd_set =
	new Chain<StepVariable>(std::make_pair(b.var(), b.var_id()), NULL);
      if (is_variable(b.term())) {
	cd_set = new Chain<StepVariable>(std::make_pair(b.term(), b.term_id()),
					 cd_set);
	vsc = new Chain<Varset>(Varset(NULL_TERM, cd_set, NULL), vsc);
      } else {
	vsc = new Chain<Varset>(Varset(b.term(), cd_set, NULL), vsc);
      }
      return &vsc->head;
    } else {
      Object constant;
      const Chain<StepVariable>* cd_set;
      const Chain<StepVariable>* ncd_set;
      if (reverse) {
	if (is_variable(b.term())) {
	  constant = NULL_TERM;
	  cd_set = new Chain<StepVariable>(std::make_pair(b.term(),
							  b.term_id()),
					   NULL);
	} else {
	  constant = b.term();
	  cd_set = NULL;
	}
	ncd_set = new Chain<StepVariable>(std::make_pair(b.var(), b.var_id()),
					  NULL);
      } else { /* !reverse */
	if (is_variable(b.term())) {
	  constant = NULL_TERM;
	  cd_set = new Chain<StepVariable>(std::make_pair(b.var(), b.var_id()),
					   NULL);
	  ncd_set = new Chain<StepVariable>(std::make_pair(b.term(),
							   b.term_id()),
					    NULL);
	} else {
	  return NULL;
	}
      }
      vsc = new Chain<Varset>(Varset(constant, cd_set, ncd_set), vsc);
      return &vsc->head;
    }
  }

private:
  /* The constant of this varset, or NULL. */
  Object constant_;
  /* The codesignation list. */
  const Chain<StepVariable>* cd_set_;
  /* The non-codesignation list. */
  const Chain<StepVariable>* ncd_set_;
};


/* Returns the varset containing the given constant, or NULL if none do. */
static const Varset* find_varset(const Chain<Varset>* varsets,
				 Object constant) {
  for (const Chain<Varset>* vsc = varsets; vsc != NULL; vsc = vsc->tail) {
    const Varset& vs = vsc->head;
    if (vs.includes(constant)) {
      return &vs;
    }
  }
  return NULL;
}


/* Returns the varset containing the given term, or NULL if none do. */
static const Varset* find_varset(const Chain<Varset>* varsets,
				 Term t, size_t step_id) {
  if (is_object(t)) {
    return find_varset(varsets, t);
  } else {
    for (const Chain<Varset>* vsc = varsets; vsc != NULL; vsc = vsc->tail) {
      const Varset& vs = vsc->head;
      if (vs.includes(t, step_id)) {
	return &vs;
      }
    }
    return NULL;
  }
}


/* ====================================================================== */
/* StepDomain */

/*
 * A step domain.
 */
struct StepDomain {
  /* Constructs a step domain. */
  StepDomain(size_t id, const VariableList& parameters,
	     const ActionDomain& domain)
    : id_(id), parameters_(&parameters), domain_(&domain) {
    ActionDomain::register_use(domain_);
  }

  /* Constructs a step domain. */
  StepDomain(const StepDomain& sd)
    : id_(sd.id_), parameters_(sd.parameters_), domain_(sd.domain_) {
    ActionDomain::register_use(domain_);
  }

  /* Deletes this step domain. */
  ~StepDomain() {
    ActionDomain::unregister_use(domain_);
  }

  /* Returns the step id. */
  size_t id() const { return id_; }

  /* Returns the step parameters. */
  const VariableList& parameters() const { return *parameters_; }

  /* Returns the parameter domains. */
  const ActionDomain& domain() const { return *domain_; }

  /* Returns the index of the variable in this step domain, or -1 if
     the variable is not included. */
  int index_of(Variable v) const {
    VariableList::const_iterator vi = find(parameters().begin(),
					   parameters().end(), v);
    return (vi != parameters().end()) ? vi - parameters().begin() : -1;
  }

  /* Checks if this step domain includes the given name in the given
     column. */
  bool includes(Object name, size_t column) const {
    for (TupleList::const_iterator ti = domain().tuples().begin();
	 ti != domain().tuples().end(); ti++) {
      if ((**ti)[column] == name) {
	return true;
      }
    }
    return false;
  }

  /* Returns the set of names from the given column. */
  const NameSet& projection(size_t column) const {
    return domain().projection(column);
  }

  /* Returns the size of the projection of the given column. */
  const size_t projection_size(size_t column) const {
    return domain().projection_size(column);
  }

  /* Returns a domain where the given column has been restricted to
     the given name, or NULL if this would leave an empty domain. */
  const StepDomain* restrict(const Chain<StepDomain>*& sdc,
			     Object name, size_t column) const {
    const ActionDomain* ad = domain().restrict(name, column);
    if (ad == NULL) {
      return NULL;
    } else if (ad == &domain()) {
      return this;
    } else {
      sdc = new Chain<StepDomain>(StepDomain(id(), parameters(), *ad), sdc);
      return &sdc->head;
    }
  }

  /* Returns a domain where the given column has been restricted to
     the given set of names, or NULL if this would leave an empty
     domain. */
  const StepDomain* restrict(const Chain<StepDomain>*& sdc,
			     const NameSet& names, size_t column) const {
    const ActionDomain* ad = domain().restrict(names, column);
    if (ad == NULL) {
      return NULL;
    } else if (ad == &domain()) {
      return this;
    } else {
      sdc = new Chain<StepDomain>(StepDomain(id(), parameters(), *ad), sdc);
      return &sdc->head;
    }
  }

  /* Returns a domain where the given column exclues the given name,
     or NULL if this would leave an empty domain. */
  const StepDomain* exclude(const Chain<StepDomain>*& sdc,
			    Object name, size_t column) const {
    const ActionDomain* ad = domain().exclude(name, column);
    if (ad == NULL) {
      return NULL;
    } else if (ad == &domain()) {
      return this;
    } else {
      sdc = new Chain<StepDomain>(StepDomain(id(), parameters(), *ad), sdc);
      return &sdc->head;
    }
  }

  /* Prints this object on the given stream. */
  void print(std::ostream& os, const TermTable& terms) const;

private:
  /* The id of the step. */
  size_t id_;
  /* Parameters of the step. */
  const VariableList* parameters_;
  /* Domain of the parameters. */
  const ActionDomain* domain_;
};


/* Prints this object on the given stream. */
void StepDomain::print(std::ostream& os, const TermTable& terms) const {
  os << "<";
  for (VariableList::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    terms.print_term(os, *vi);
    os << '(' << id() << ')';
  }
  os << "> in ";
  domain().print(os, terms);
}


/* Returns the step domain containing the given variable and the
   column of the variable, or NULL no step domain contains the
   variable. */
static std::pair<const StepDomain*, size_t>
find_step_domain(const Chain<StepDomain>* step_domains,
		 Variable var, size_t step_id) {
  if (step_id > 0) {
    for (const Chain<StepDomain>* sd = step_domains;
	 sd != NULL; sd = sd->tail) {
      const StepDomain& step_domain = sd->head;
      if (step_domain.id() == step_id) {
	int column = step_domain.index_of(var);
	if (column >= 0) {
	  return std::make_pair(&step_domain, column);
	} else {
	  break;
	}
      }
    }
  }
  return std::pair<const StepDomain*, size_t>(NULL, 0);
}


/* ====================================================================== */
/* ActionDomain */

/* Constructs an action domain with a single tuple. */
ActionDomain::ActionDomain(const ObjectList& tuple)
  : ref_count_(0) {
#ifdef DEBUG_MEMORY
    created_action_domains++;
#endif
  add(tuple);
}


/* Deletes this action domain. */
ActionDomain::~ActionDomain() {
#ifdef DEBUG_MEMORY
  deleted_action_domains++;
#endif
  for (ProjectionMap::const_iterator pi = projections_.begin();
       pi != projections_.end(); pi++) {
    delete (*pi).second;
  }
}


/* Adds a tuple to this domain. */
void ActionDomain::add(const ObjectList& tuple) {
  tuples_.push_back(&tuple);
}


/* Returns the set of names from the given column. */
const NameSet& ActionDomain::projection(size_t column) const {
  ProjectionMap::const_iterator pi = projections_.find(column);
  if (pi != projections_.end()) {
    return *(*pi).second;
  } else {
    NameSet* projection = new NameSet();
    for (TupleList::const_iterator ti = tuples().begin();
	 ti != tuples().end(); ti++) {
      const ObjectList& tuple = **ti;
      projection->insert(tuple[column]);
    }
    projections_.insert(std::make_pair(column, projection));
    return *projection;
  }
}


/* Returns the size of the projection of the given column. */
const size_t ActionDomain::projection_size(size_t column) const {
  return projection(column).size();
}


/* Returns a domain where the given column has been restricted to
   the given name, or NULL if this would leave an empty domain. */
const ActionDomain* ActionDomain::restrict(Object name, size_t column) const {
  ActionDomain* new_domain = NULL;
  for (TupleList::const_iterator ti = tuples().begin();
       ti != tuples().end(); ti++) {
    const ObjectList& tuple = **ti;
    if (tuple[column] == name) {
      if (new_domain == NULL) {
	new_domain = new ActionDomain(tuple);
      } else {
	new_domain->add(tuple);
      }
    }
  }
  if (new_domain != NULL && new_domain->size() == size()) {
    ActionDomain::register_use(new_domain);
    ActionDomain::unregister_use(new_domain);
    return this;
  } else {
    return new_domain;
  }
}


/* Returns a domain where the given column has been restricted to
   the given set of names, or NULL if this would leave an empty
   domain. */
const ActionDomain* ActionDomain::restrict(const NameSet& names,
					   size_t column) const {
  ActionDomain* new_domain = NULL;
  for (TupleList::const_iterator ti = tuples().begin();
       ti != tuples().end(); ti++) {
    const ObjectList& tuple = **ti;
    if (names.find(tuple[column]) != names.end()) {
      if (new_domain == NULL) {
	new_domain = new ActionDomain(tuple);
      } else {
	new_domain->add(tuple);
      }
    }
  }
  if (new_domain != NULL && new_domain->size() == size()) {
    ActionDomain::register_use(new_domain);
    ActionDomain::unregister_use(new_domain);
    return this;
  } else {
    return new_domain;
  }
}


/* Returns a domain where the given column exclues the given name,
   or NULL if this would leave an empty domain. */
const ActionDomain* ActionDomain::exclude(Object name, size_t column) const {
  ActionDomain* new_domain = NULL;
  for (TupleList::const_iterator ti = tuples().begin();
       ti != tuples().end(); ti++) {
    const ObjectList& tuple = **ti;
    if (tuple[column] != name) {
      if (new_domain == NULL) {
	new_domain = new ActionDomain(tuple);
      } else {
	new_domain->add(tuple);
      }
    }
  }
  if (new_domain != NULL && new_domain->size() == size()) {
    ActionDomain::register_use(new_domain);
    ActionDomain::unregister_use(new_domain);
    return this;
  } else {
    return new_domain;
  }
}


/* Prints this object on the given stream. */
void ActionDomain::print(std::ostream& os, const TermTable& terms) const {
  os << '{';
  for (TupleList::const_iterator ti = tuples().begin();
       ti != tuples().end(); ti++) {
    if (ti != tuples().begin()) {
      os << ' ';
    }
    os << '<';
    const ObjectList& tuple = **ti;
    for (ObjectList::const_iterator ni = tuple.begin();
	 ni != tuple.end(); ni++) {
      if (ni != tuple.begin()) {
	os << ' ';
      }
      terms.print_term(os, *ni);
    }
    os << '>';
  }
  os << '}';
}


/* ====================================================================== */
/* Bindings */

/* Empty bindings. */
const Bindings Bindings::EMPTY = Bindings();

/* Checks if the given formulas can be unified. */
bool Bindings::unifiable(const Literal& l1, size_t id1,
			 const Literal& l2, size_t id2) {
  BindingList dummy;
  return unifiable(dummy, l1, id1, l2, id2);
}


/* Checks if the given formulas can be unified; the most general
   unifier is added to the provided substitution list. */
bool Bindings::unifiable(BindingList& mgu,
			 const Literal& l1, size_t id1,
			 const Literal& l2, size_t id2) {
  return EMPTY.unify(mgu, l1, id1, l2, id2);
}


/* Constructs an empty binding collection. */
Bindings::Bindings()
  : varsets_(NULL), high_step_(0), step_domains_(NULL), ref_count_(1) {
}


/* Constructs a binding collection. */
Bindings::Bindings(const Chain<Varset>* varsets, size_t high_step,
		   const Chain<StepDomain>* step_domains)
  : varsets_(varsets), high_step_(high_step), step_domains_(step_domains),
    ref_count_(0) {
#ifdef DEBUG_MEMORY
  created_bindings++;
#endif
  Chain<Varset>::register_use(varsets_);
  Chain<StepDomain>::register_use(step_domains_);
}


/* Deletes this binding collection. */
Bindings::~Bindings() {
#ifdef DEBUG_MEMORY
  deleted_bindings++;
#endif
  Chain<Varset>::unregister_use(varsets_);
  Chain<StepDomain>::unregister_use(step_domains_);
}


/* Returns the binding for the given term, or the term itself if it is
     not bound to a single name. */
Term Bindings::binding(Term t, size_t step_id) const {
  if (is_variable(t)) {
    const Varset* vs =
      (step_id <= high_step_) ? find_varset(varsets_, t, step_id) : NULL;
    if (vs != NULL && vs->constant() != Object(NULL_TERM)) {
      return vs->constant();
    }
  }
  return t;
}


/* Returns the domain for the given step variable. */
const NameSet* Bindings::domain(Variable v, size_t step_id) const {
  std::pair<const StepDomain*, size_t> sd =
    find_step_domain(step_domains_, v, step_id);
  return (sd.first != NULL) ? &sd.first->projection(sd.second) : NULL;
}


/* Checks if one of the given formulas is the negation of the other,
   and the atomic formulas can be unified. */
bool Bindings::affects(const Literal& l1, size_t id1,
		       const Literal& l2, size_t id2) const {
  BindingList dummy;
  return affects(dummy, l1, id1, l2, id2);
}


/* Checks if one of the given formulas is the negation of the other,
   and the atomic formulas can be unified; the most general unifier
   is added to the provided substitution list. */
bool Bindings::affects(BindingList& mgu, const Literal& l1, size_t id1,
		       const Literal& l2, size_t id2) const {
  const Negation* negation = dynamic_cast<const Negation*>(&l1);
  if (negation != NULL) {
    return unify(mgu, l2, id2, negation->atom(), id1);
  } else {
    negation = dynamic_cast<const Negation*>(&l2);
    if (negation != NULL) {
      return unify(mgu, negation->atom(), id2, l1, id1);
    } else {
      return false;
    }
  }
}


/* Checks if the given formulas can be unified. */
bool Bindings::unify(const Literal& l1, size_t id1,
		     const Literal& l2, size_t id2) const {
  BindingList dummy;
  return unify(dummy, l1, id1, l2, id2);
}


/* Checks if the given formulas can be unified; the most general
   unifier is added to the provided substitution list. */
bool Bindings::unify(BindingList& mgu,
		     const Literal& l1, size_t id1,
		     const Literal& l2, size_t id2) const {
  if (typeid(l1) != typeid(l2)) {
    /* Not the same type of literal. */
    return false;
  } else if (l1.predicate() != l2.predicate()) {
    /* The predicates do not match. */
    return false;
  } else if (id1 == 0 && id2 == 0) {
    /* Both literals are fully instantiated. */
    return &l1 == &l2;
  } else if (id1 == 0 || id2 == 0) {
    /* One of the literals is fully instantiated. */
    const Literal* ll;
    const Literal* lg;
    size_t idl;
    if (id1 == 0) {
      ll = &l2;
      lg = &l1;
      idl = id2;
    } else {
      ll = &l1;
      lg = &l2;
      idl = id1;
    }
    SubstitutionMap bind;
    size_t n = ll->arity();
    for (size_t i = 0; i < n; i++) {
      Term t1 = ll->term(i);
      Object o2 = lg->term(i);
      if (is_object(t1)) {
	if (Object(t1) != o2) {
	  return false;
	}
      } else {
	SubstitutionMap::const_iterator b = bind.find(t1);
	if (b != bind.end()) {
	  if ((*b).second != o2) {
	    return false;
	  }
	} else {
	  Term bt = binding(t1, idl);
	  if (is_object(bt)) {
	    if (Object(bt) != o2) {
	      return false;
	    }
	  } else {
	    mgu.push_back(Binding(t1, idl, o2, 0, true));
	  }
	  bind.insert(std::make_pair(t1, o2));
	}
      }
    }
  } else {
    /*
     * Try to unify the terms of the literals.
     */
    /* Number of terms for the first literal. */
    size_t n = l1.arity();
    for (size_t i = 0; i < n; i++) {
      /*
       * Try to unify a pair of terms.
       */
      Term term1 = l1.term(i);
      Term term2 = l2.term(i);
      if (is_object(term1)) {
	/* The first term is a name. */
	if (is_object(term2)) {
	  /*
	   * Both terms are names.
	   */
	  if (term1 != term2) {
	    /* The two terms are different names. */
	    return false;
	  }
	} else {
	  /*
	   * The first term is a name and the second is a variable.
	   */
	  mgu.push_back(Binding(term2, id2, term1, 0, true));
	}
      } else {
	/* The first term is a variable. */
	mgu.push_back(Binding(term1, id1, term2, id2, true));
      }
    }
    if (add(mgu, true) == NULL) {
      /* Unification is inconsistent with current bindings. */
      return false;
    }
  }
  /* Successful unification. */
  return true;
}


/* Checks if the given equality is consistent with the current
   bindings. */
bool Bindings::consistent_with(const Equality& eq, size_t step_id) const {
  Variable var;
  size_t var_id;
  Term term;
  size_t term_id;
  if (is_variable(eq.term1())) {
    var = eq.term1();
    var_id = eq.step_id1(step_id);
    term = eq.term2();
    term_id = eq.step_id2(step_id);
  } else if (is_variable(eq.term2())) {
    var = eq.term2();
    var_id = eq.step_id2(step_id);
    term = eq.term1();
    term_id = eq.step_id1(step_id);
  } else {
    return eq.term1() == eq.term2();
  }
  const Varset* vs =
    (term_id <= high_step_) ? find_varset(varsets_, term, term_id) : NULL;
  if (vs == NULL || vs->includes(var, var_id)) {
    return true;
  } else if (vs->excludes(var, var_id)) {
    return false;
  } else if (vs->constant() != Object(NULL_TERM)) {
    std::pair<const StepDomain*, size_t> sd =
      find_step_domain(step_domains_, var, var_id);
    if (sd.first != NULL) {
      return sd.first->includes(vs->constant(), sd.second);
    }
  }
  return true;
}


/* Checks if the given inequality is consistent with the current
   bindings. */
bool Bindings::consistent_with(const Inequality& neq, size_t step_id) const {
  Variable var;
  size_t var_id;
  Term term;
  size_t term_id;
  if (is_variable(neq.term1())) {
    var = neq.term1();
    var_id = neq.step_id1(step_id);
    term = neq.term2();
    term_id = neq.step_id2(step_id);
  } else if (is_variable(neq.term2())) {
    var = neq.term2();
    var_id = neq.step_id2(step_id);
    term = neq.term1();
    term_id = neq.step_id1(step_id);
  } else {
    return neq.term1() != neq.term2();
  }
  const Varset* vs =
    (term_id <= high_step_) ? find_varset(varsets_, term, term_id) : NULL;
  return (vs == NULL
	  || !vs->includes(var, var_id) || vs->excludes(var, var_id));
}


/* Adds bindings to the list as determined by difference between the
   given step domains */
static void add_domain_bindings(BindingList& bindings,
				const StepDomain& old_sd,
				const StepDomain& new_sd,
				size_t ex_column = UINT_MAX) {
  for (size_t c = 0; c < old_sd.parameters().size(); c++) {
    if (c != ex_column && new_sd.projection_size(c) == 1
	&& old_sd.projection_size(c) > 1) {
      bindings.push_back(Binding(new_sd.parameters()[c], new_sd.id(),
				 *new_sd.projection(c).begin(), 0, true));
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

  /* Varsets for new binding collection */
  const Chain<Varset>* varsets = varsets_;
  /* Highest step id of variable in varsets. */
  size_t high_step = high_step_;
  /* Variables above previous high step. */
  VariableSet high_step_vars;
  /* Step domains for new binding collection */
  const Chain<StepDomain>* step_domains = step_domains_;

  BindingList new_binds(new_bindings);
  /*
   * Add new bindings one at a time.
   */
  for (size_t i = 0; i < new_binds.size(); i++) {
    /*
     * N.B. Make a copy of the binding instead of just saving a
     * reference, because new_binds can be expanded in the loop in
     * which case the reference may become invalid.
     */
    const Binding bind = new_binds[i];
    if (bind.equality()) {
      /*
       * Adding equality binding.
       */
      /* Varset for variable. */
      const Varset* vs1;
      StepVariable sv(bind.var(), bind.var_id());
      if (bind.var_id() <= high_step_
	  || high_step_vars.find(sv) != high_step_vars.end()) {
	vs1 = find_varset(varsets, bind.var(), bind.var_id());
      } else {
	if (bind.var_id() > high_step) {
	  high_step = bind.var_id();
	}
	high_step_vars.insert(sv);
	vs1 = NULL;
      }
      /* Varset for term. */
      const Varset* vs2;
      sv = std::make_pair(bind.term(), bind.term_id());
      if (!is_variable(sv.first) || bind.term_id() <= high_step_
	  || high_step_vars.find(sv) != high_step_vars.end()) {
	vs2 = find_varset(varsets, bind.term(), bind.term_id());
      } else {
	if (bind.term_id() > high_step) {
	  high_step = bind.term_id();
	}
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
	    comb = vs2->add(varsets, bind.var(), bind.var_id());
	  } else if (vs2 == NULL) {
	    /* The second term is unbound, so add it to the varset of
               the first. */
	    comb = vs1->add(varsets, bind.term(), bind.term_id());
	  } else {
	    /* Both terms are bound, so combine their varsets. */
	    comb = vs1->combine(varsets, *vs2);
	  }
	} else {
	  /* The terms are already bound to eachother. */
	  comb = vs1;
	}
      } else {
	/* None of the terms are already bound. */
	comb = Varset::make(varsets, bind);
      }
      if (comb == NULL) {
	/* Binding is inconsistent with current bindings. */
	Chain<Varset>::register_use(varsets);
	Chain<Varset>::unregister_use(varsets);
	Chain<StepDomain>::register_use(step_domains);
	Chain<StepDomain>::unregister_use(step_domains);
	return NULL;
      } else {
	/* Binding is consistent with current bindings. */
	if (comb != vs1) {
	  /* Combined varset is new, so add it to the chain of varsets. */
	  Object name = comb->constant();
	  /* Restrict step domain for all codesignated variables. */
	  const NameSet* intersection = NULL;
	  bool new_intersection = false;
	  const Chain<StepVariable>* vc = NULL;
	  int phase = 0;
	  while (phase < 4 || (intersection != NULL && phase < 8)) {
	    Variable var = NULL_TERM;
	    size_t var_id = 0;
	    switch (phase) {
	    case 0:
	    case 4:
	      if (vs1 == NULL) {
		var = bind.var();
		var_id = bind.var_id();
		phase += 2;
	      } else if (vs1->constant() == Object(NULL_TERM)) {
		vc = vs1->cd_set();
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
		var = vc->head.first;
		var_id = vc->head.second;
		vc = vc->tail;
	      } else {
		var = NULL_TERM;
		phase++;
	      }
	      break;
	    case 2:
	    case 6:
	      if (vs2 == NULL) {
		var = bind.term();
		var_id = bind.term_id();
		phase += 2;
	      } else if (vs2->constant() == Object(NULL_TERM)) {
		vc = vs2->cd_set();
		phase++;
	      } else {
		phase += 2;
	      }
	      break;
	    }
	    if (is_variable(var)) {
	      /* Step domain for variable. */
	      std::pair<const StepDomain*, size_t> sd =
		find_step_domain(step_domains, var, var_id);
	      if (sd.first != NULL) {
		if (name != Object(NULL_TERM)) {
		  const StepDomain* new_sd =
		    sd.first->restrict(step_domains, name, sd.second);
		  if (new_sd == NULL) {
		    /* Domain became empty. */
		    Chain<Varset>::register_use(varsets);
		    Chain<Varset>::unregister_use(varsets);
		    Chain<StepDomain>::register_use(step_domains);
		    Chain<StepDomain>::unregister_use(step_domains);
		    if (new_intersection) {
		      delete intersection;
		    }
		    return NULL;
		  }
		  if (sd.first != new_sd) {
		    add_domain_bindings(new_binds, *sd.first, *new_sd,
					sd.second);
		  }
		} else {
		  if (phase > 4) {
		    const StepDomain* new_sd =
		      sd.first->restrict(step_domains,
					 *intersection, sd.second);
		    if (new_sd == NULL) {
		      /* Domain became empty. */
		      Chain<Varset>::register_use(varsets);
		      Chain<Varset>::unregister_use(varsets);
		      Chain<StepDomain>::register_use(step_domains);
		      Chain<StepDomain>::unregister_use(step_domains);
		      if (new_intersection) {
			delete intersection;
		      }
		      return NULL;
		    }
		    if (sd.first != new_sd) {
		      add_domain_bindings(new_binds, *sd.first, *new_sd);
		    }
		  } else if (intersection == NULL) {
		    intersection = &sd.first->projection(sd.second);
		  } else {
		    NameSet* cut = new NameSet();
		    const NameSet& set2 = sd.first->projection(sd.second);
		    set_intersection(intersection->begin(),
				     intersection->end(),
				     set2.begin(), set2.end(),
				     inserter(*cut, cut->begin()));
		    if (new_intersection) {
		      delete intersection;
		    }
		    intersection = cut;
		    new_intersection = true;
		    if (intersection->empty()) {
		      /* Domain became empty. */
		      Chain<Varset>::register_use(varsets);
		      Chain<Varset>::unregister_use(varsets);
		      Chain<StepDomain>::register_use(step_domains);
		      Chain<StepDomain>::unregister_use(step_domains);
		      if (new_intersection) {
			delete intersection;
		      }
		      return NULL;
		    }
		  }
		}
	      }
	    }
	  }
	  if (new_intersection) {
	    delete intersection;
	  }
	}
      }
    } else {
      /*
       * Adding inequality binding.
       */
      /* Varset for variable. */
      const Varset* vs1;
      StepVariable sv(bind.var(), bind.var_id());
      if (bind.var_id() <= high_step_
	  || high_step_vars.find(sv) != high_step_vars.end()) {
	vs1 = find_varset(varsets, bind.var(), bind.var_id());
      } else {
	if (bind.var_id() > high_step) {
	  high_step = bind.var_id();
	}
	high_step_vars.insert(sv);
	vs1 = NULL;
      }
      /* Varset for term. */
      const Varset* vs2;
      sv = std::make_pair(bind.term(), bind.term_id());
      if (!is_variable(sv.first) || bind.term_id() <= high_step_
	  || high_step_vars.find(sv) != high_step_vars.end()) {
	vs2 = find_varset(varsets, bind.term(), bind.term_id());
      } else {
	if (bind.term_id() > high_step) {
	  high_step = bind.term_id();
	}
	high_step_vars.insert(sv);
	vs2 = NULL;
      }
      if (vs1 != NULL && vs2 != NULL && vs1 == vs2) {
	/* The terms are already bound to eachother. */
	Chain<Varset>::register_use(varsets);
	Chain<Varset>::unregister_use(varsets);
	Chain<StepDomain>::register_use(step_domains);
	Chain<StepDomain>::unregister_use(step_domains);
	return NULL;
      } else {
	/* The terms are not bound to eachother. */
	bool separate1 = true;
	bool separate2 = true;
	if (vs1 == NULL) {
	  /* The first term is unbound, so create a new varset for it. */
	  vs1 = Varset::make(varsets, bind);
	} else {
	  if (is_variable(bind.term())) {
	    /* The second term is a variable. */
	    if (vs1->excludes(bind.term(), bind.term_id())) {
	      /* The second term is already separated from the first. */
	      separate1 = false;
	    } else {
	      /* Separate the second term from the first. */
	      vs1 = vs1->restrict(varsets, bind.term(), bind.term_id());
	    }
	  } else {
	    /* The second term is a name, so the terms are separated
               in the varset for the second term. */
	    separate1 = false;
	  }
	}
	if (vs2 == NULL) {
	  /* The second term is unbound, so create a new varset for it. */
	  vs2 = Varset::make(varsets, bind, true);
	} else if (vs2->excludes(bind.var(), bind.var_id())) {
	  /* The first term is already separated from the second. */
	  separate2 = false;
	} else {
	  /* Separate the first term from the second. */
	  vs2 = vs2->restrict(varsets, bind.var(), bind.var_id());
	}
	if (separate1 && vs1 != NULL) {
	  /* The second term was not separated from the first already. */
	  if (vs1->constant() != Object(NULL_TERM) && vs2 != NULL) {
	    for (const Chain<StepVariable>* vc = vs2->cd_set();
		 vc != NULL; vc = vc->tail) {
	      std::pair<const StepDomain*, size_t> sd =
		find_step_domain(step_domains,
				 vc->head.first, vc->head.second);
	      if (sd.first != NULL) {
		const StepDomain* new_sd =
		  sd.first->exclude(step_domains, vs1->constant(), sd.second);
		if (new_sd == NULL) {
		  /* Domain became empty. */
		  Chain<Varset>::register_use(varsets);
		  Chain<Varset>::unregister_use(varsets);
		  Chain<StepDomain>::register_use(step_domains);
		  Chain<StepDomain>::unregister_use(step_domains);
		  return NULL;
		}
		if (sd.first != new_sd) {
		  add_domain_bindings(new_binds, *sd.first, *new_sd);
		}
	      }
	    }
	  }
	}
	if (separate2 && vs2 != NULL) {
	  /* The first term was not separated from the second already. */
	  if (vs2->constant() != Object(NULL_TERM)) {
	    const Chain<StepVariable>* vc =
	      (vs1 != NULL) ? vs1->cd_set() : NULL;
	    Variable var = (vc != NULL) ? vc->head.first : bind.var();
	    size_t var_id = (vc != NULL) ? vc->head.second : bind.var_id();
	    while (is_variable(var)) {
	      std::pair<const StepDomain*, size_t> sd =
		find_step_domain(step_domains, var, var_id);
	      if (sd.first != NULL) {
		const StepDomain* new_sd =
		  sd.first->exclude(step_domains, vs2->constant(), sd.second);
		if (new_sd == NULL) {
		  /* Domain became empty. */
		  Chain<Varset>::register_use(varsets);
		  Chain<Varset>::unregister_use(varsets);
		  Chain<StepDomain>::register_use(step_domains);
		  Chain<StepDomain>::unregister_use(step_domains);
		  return NULL;
		}
		if (sd.first != new_sd) {
		  add_domain_bindings(new_binds, *sd.first, *new_sd);
		}
	      }
	      vc = (vs1 != NULL) ? vc->tail : NULL;
	      var = (vc != NULL) ? vc->head.first : NULL_TERM;
	      var_id = (vc != NULL) ? vc->head.second : 0;
	    }
	  }
	}
      }
    }
  }
  /* New bindings are consistent with the current bindings. */
  if (test_only
      || (varsets == varsets_ && high_step == high_step_
	  && step_domains == step_domains_)) {
    Chain<Varset>::register_use(varsets);
    Chain<Varset>::unregister_use(varsets);
    Chain<StepDomain>::register_use(step_domains);
    Chain<StepDomain>::unregister_use(step_domains);
    return this;
  } else {
    return new Bindings(varsets, high_step, step_domains);
  }
}


/* Returns the binding collection obtained by adding the constraints
   associated with the given step to this binding collection, or
   NULL if the new binding collection would be inconsistent. */
const Bindings* Bindings::add(size_t step_id, const Action& step_action,
			      const PlanningGraph& pg, bool test_only) const {
  const ActionSchema* action = dynamic_cast<const ActionSchema*>(&step_action);
  if (action == NULL || action->parameters().empty()) {
    return this;
  }
  const ActionDomain* domain = pg.action_domain(action->name());
  if (domain == NULL) {
    return NULL;
  }
  const Chain<StepDomain>* step_domains =
    new Chain<StepDomain>(StepDomain(step_id, action->parameters(), *domain),
			  step_domains_);
  const Chain<Varset>* varsets = varsets_;
  size_t high_step = high_step_;
  const StepDomain& step_domain = step_domains->head;
  for (size_t c = 0; c < step_domain.parameters().size(); c++) {
    if (step_domain.projection_size(c) == 1) {
      const Chain<StepVariable>* cd_set =
	new Chain<StepVariable>(std::make_pair(step_domain.parameters()[c],
					       step_domain.id()),
				NULL);
      varsets = new Chain<Varset>(Varset(*step_domain.projection(c).begin(),
					 cd_set, NULL),
				  varsets);
      if (step_id > high_step) {
	high_step = step_id;
      }
    }
  }
  if (test_only
      || (varsets == varsets_ && high_step == high_step_
	  && step_domains == step_domains_)) {
    Chain<Varset>::register_use(varsets);
    Chain<Varset>::unregister_use(varsets);
    Chain<StepDomain>::register_use(step_domains);
    Chain<StepDomain>::unregister_use(step_domains);
    return this;
  } else {
    return new Bindings(varsets, high_step, step_domains);
  }
}


/* Prints this object on the given stream. */
void Bindings::print(std::ostream& os, const TermTable& terms) const {
  std::map<size_t, std::vector<Variable> > seen_vars;
  std::vector<Object> seen_names;
  for (const Chain<Varset>* vsc = varsets_; vsc != NULL; vsc = vsc->tail) {
    const Varset& vs = vsc->head;
    if (vs.cd_set() != NULL) {
      const Chain<StepVariable>* vc = vs.cd_set();
      if (find(seen_vars[vc->head.second].begin(),
	       seen_vars[vc->head.second].end(), vc->head.first)
	  != seen_vars[vc->head.second].end()) {
	continue;
      }
      os << std::endl << "{";
      for (; vc != NULL; vc = vc->tail) {
	const StepVariable& step_var = vc->head;
	os << ' ';
	terms.print_term(os, step_var.first);
	os << '(' << step_var.second << ')';
	seen_vars[step_var.second].push_back(step_var.first);
      }
      os << " }";
      if (vs.constant() != Object(NULL_TERM)) {
	os << " == ";
      }
    }
    if (vs.constant() != Object(NULL_TERM)) {
      Object name = vs.constant();
      if (find(seen_names.begin(), seen_names.end(), name)
	  != seen_names.end()) {
	continue;
      }
      if (vs.cd_set() == NULL) {
	os << std::endl;
      }
      terms.print_term(os, name);
      seen_names.push_back(name);
    }
    if (vs.ncd_set() != NULL) {
      os << " != {";
      for (const Chain<StepVariable>* vc =
	     vs.ncd_set(); vc != NULL; vc = vc->tail) {
	os << ' ';
	terms.print_term(os, vc->head.first);
	os << '(' << vc->head.second << ')';
      }
      os << " }";
    }
  }
  std::set<size_t> seen_steps;
  for (const Chain<StepDomain>* sd = step_domains_;
       sd != NULL; sd = sd->tail) {
    if (seen_steps.find(sd->head.id()) == seen_steps.end()) {
      seen_steps.insert(sd->head.id());
      os << std::endl;
      sd->head.print(os, terms);
    }
  }
}
