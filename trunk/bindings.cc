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
 * $Id: bindings.cc,v 6.1 2003-07-13 15:50:02 lorens Exp $
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

/* Iterator for variable sets. */
typedef VariableSet::const_iterator VariableSetIter;


/* ====================================================================== */
/* VariableChain */

typedef CollectibleChain<StepVariable> VariableChain;


/* ====================================================================== */
/* Varset */

/*
 * Variable codesignation, and non-codesignation.
 */
struct Varset {
  /* Constructs a varset. */
  Varset(Object constant, const VariableChain* cd_set,
	 const VariableChain* ncd_set)
    : constant_(constant), cd_set_(cd_set), ncd_set_(ncd_set) {
    VariableChain::register_use(cd_set_);
    VariableChain::register_use(ncd_set_);
  }

  /* Constructs a varset. */
  Varset(const Varset& vs)
    : constant_(vs.constant_), cd_set_(vs.cd_set_), ncd_set_(vs.ncd_set_) {
    VariableChain::register_use(cd_set_);
    VariableChain::register_use(ncd_set_);
  }

  /* Deletes this varset. */
  ~Varset() {
    VariableChain::unregister_use(cd_set_);
    VariableChain::unregister_use(ncd_set_);
  }

  /* The constant of this varset, or NULL. */
  Object constant() const { return constant_; }

  /* The codesignation list. */
  const VariableChain* cd_set() const { return cd_set_; }

  /* The non-codesignation list. */
  const VariableChain* ncd_set() const { return ncd_set_; }

  /* Checks if this varset includes the given name. */
  bool includes(Object name) const {
    return constant() == name;
  }

  /* Checks if this varset includes the given variable. */
  bool includes(Variable var, size_t step_id) const {
    for (const VariableChain* vc = cd_set(); vc != NULL; vc = vc->tail) {
      if (vc->head.first == var && vc->head.second == step_id) {
	return true;
      }
    }
    return false;
  }

  /* Checks if this varset excludes the given variable. */
  bool excludes(Variable var, size_t step_id) const {
    for (const VariableChain* vc = ncd_set(); vc != NULL; vc = vc->tail) {
      if (vc->head.first == var && vc->head.second == step_id) {
	return true;
      }
    }
    return false;
  }

  /* Returns the varset obtained by adding the given name to this
     varset, or NULL if the name is inconsistent with the current
     constant. */
  const Varset* add(const VarsetChain*& vsc, Object name) const {
    if (constant() != Object(NULL_TERM)) {
      return (constant() == name) ? this : NULL;
    } else {
      vsc = new VarsetChain(Varset(name, cd_set(), ncd_set()), vsc);
      return &vsc->head;
    }
  }

  /* Returns the varset obtained by adding the given term to this
     varset, or NULL if the term is excluded from this varset. */
  const Varset* add(const VarsetChain*& vsc, Term t, size_t step_id) const {
    if (is_object(t)) {
      return add(vsc, t);
    } else if (excludes(t, step_id)) {
      return NULL;
    } else {
      vsc = new VarsetChain(Varset(constant(),
				   new VariableChain(std::make_pair(t,
								    step_id),
						     cd_set()),
				   ncd_set()),
			    vsc);
      return &vsc->head;
    }
  }

  /* Returns the varset obtained by adding the given variable to the
     non-codesignation list of this varset; N.B. assumes that the
     variable is not included in the varset already. */
  const Varset* restrict(const VarsetChain*& vsc,
			 Variable var, size_t step_id) const {
    vsc = new VarsetChain(Varset(constant(),
				 cd_set(),
				 new VariableChain(std::make_pair(var,
								  step_id),
						   ncd_set())),
			  vsc);
    return &vsc->head;
  }

  /* Returns the combination of this and the given varset, or NULL if
     the combination is inconsistent. */
  const Varset* combine(const VarsetChain*& vsc, const Varset& vs) const {
    if (constant() != Object(NULL_TERM) && vs.constant() != Object(NULL_TERM)
	&& constant() != vs.constant()) {
      return NULL;
    }
    Object comb_const =
      (constant() != Object(NULL_TERM)) ? constant() : vs.constant();
    const VariableChain* comb_cd = cd_set();
    for (const VariableChain* vc = vs.cd_set(); vc != NULL; vc = vc->tail) {
      const StepVariable& step_var = vc->head;
      if (excludes(step_var.first, step_var.second)) {
	VariableChain::register_use(comb_cd);
	VariableChain::unregister_use(comb_cd);
	return NULL;
      } else {
	comb_cd = new VariableChain(step_var, comb_cd);
      }
    }
    const VariableChain* comb_ncd = ncd_set();
    for (const VariableChain* vc = vs.ncd_set(); vc != NULL; vc = vc->tail) {
      const StepVariable& step_var = vc->head;
      if (includes(step_var.first, step_var.second)) {
	VariableChain::register_use(comb_cd);
	VariableChain::unregister_use(comb_cd);
	VariableChain::register_use(comb_ncd);
	VariableChain::unregister_use(comb_ncd);
	return NULL;
      } else if (!excludes(step_var.first, step_var.second)) {
	comb_ncd = new VariableChain(step_var, comb_ncd);
      }
    }
    vsc = new VarsetChain(Varset(comb_const, comb_cd, comb_ncd), vsc);
    return &vsc->head;
  }

  /* Returns the varset representing the given equality binding. */
  static const Varset* make_varset(const VarsetChain*& vsc,
				   const Binding& b, bool reverse = false) {
    if (b.equality()) {
      const VariableChain* cd_set =
	new VariableChain(std::make_pair(b.var(), b.var_id()), NULL);
      if (is_variable(b.term())) {
	cd_set = new VariableChain(std::make_pair(b.term(), b.term_id()),
				   cd_set);
	vsc = new VarsetChain(Varset(NULL_TERM, cd_set, NULL), vsc);
      } else {
	vsc = new VarsetChain(Varset(b.term(), cd_set, NULL), vsc);
      }
      return &vsc->head;
    } else {
      Object constant;
      const VariableChain* cd_set;
      const VariableChain* ncd_set;
      if (reverse) {
	if (is_variable(b.term())) {
	  constant = NULL_TERM;
	  cd_set = new VariableChain(std::make_pair(b.term(), b.term_id()),
				     NULL);
	} else {
	  constant = b.term();
	  cd_set = NULL;
	}
	ncd_set = new VariableChain(std::make_pair(b.var(), b.var_id()), NULL);
      } else { /* !reverse */
	if (is_variable(b.term())) {
	  constant = NULL_TERM;
	  cd_set = new VariableChain(std::make_pair(b.var(), b.var_id()),
				     NULL);
	  ncd_set = new VariableChain(std::make_pair(b.term(), b.term_id()),
				      NULL);
	} else {
	  return NULL;
	}
      }
      vsc = new VarsetChain(Varset(constant, cd_set, ncd_set), vsc);
      return &vsc->head;
    }
  }

private:
  /* The constant of this varset, or NULL. */
  Object constant_;
  /* The codesignation list. */
  const VariableChain* cd_set_;
  /* The non-codesignation list. */
  const VariableChain* ncd_set_;
};

/* Output operator for varsets. */
std::ostream& operator<<(std::ostream& os, const Varset& vs) {
  os << "CD{";
  for (const VariableChain* vc = vs.cd_set(); vc != NULL; vc = vc->tail) {
    os << " ?v" << -vc->head.first << '[' << vc->head.second << ']';
  }
  os << " }";
  if (vs.constant() != Object(NULL_TERM)) {
    os << ' ' << vs.constant();
  }
  os << " NCD{";
  for (const VariableChain* vc = vs.ncd_set(); vc != NULL; vc = vc->tail) {
    os << " ?v" << -vc->head.first << '[' << vc->head.second << ']';
  }
  os << " }";
  return os;
}


/* Returns the varset containing the given constant, or NULL if none do. */
static const Varset* find_varset(const VarsetChain* varsets, Object constant) {
  for (const VarsetChain* vsc = varsets; vsc != NULL; vsc = vsc->tail) {
    const Varset& vs = vsc->head;
    if (vs.includes(constant)) {
      return &vs;
    }
  }
  return NULL;
}


/* Returns the varset containing the given term, or NULL if none do. */
static const Varset* find_varset(const VarsetChain* varsets,
				 Term t, size_t step_id) {
  if (is_object(t)) {
    return find_varset(varsets, t);
  } else {
    for (const VarsetChain* vsc = varsets; vsc != NULL; vsc = vsc->tail) {
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

  /* Returns the number of columns in this domain. */
  size_t width() const { return parameters().size(); }

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
    const NameSet& names = domain().projection(column);
    return names.find(name) != names.end();
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
  const StepDomain* restrict(const StepDomainChain*& sdc,
			     Object name, size_t column) const {
    const ActionDomain* ad = domain().restrict(name, column);
    if (ad == NULL) {
      return NULL;
    } else if (ad == &domain()) {
      return this;
    } else {
      sdc = new StepDomainChain(StepDomain(id(), parameters(), *ad), sdc);
      return &sdc->head;
    }
  }

  /* Returns a domain where the given column has been restricted to
     the given set of names, or NULL if this would leave an empty
     domain. */
  const StepDomain* restrict(const StepDomainChain*& sdc,
			     const NameSet& names, size_t column) const {
    const ActionDomain* ad = domain().restrict(names, column);
    if (ad == NULL) {
      return NULL;
    } else if (ad == &domain()) {
      return this;
    } else {
      sdc = new StepDomainChain(StepDomain(id(), parameters(), *ad), sdc);
      return &sdc->head;
    }
  }

  /* Returns a domain where the given column exclues the given name,
     or NULL if this would leave an empty domain. */
  const StepDomain* exclude(const StepDomainChain*& sdc,
			    Object name, size_t column) const {
    const ActionDomain* ad = domain().exclude(name, column);
    if (ad == NULL) {
      return NULL;
    } else if (ad == &domain()) {
      return this;
    } else {
      sdc = new StepDomainChain(StepDomain(id(), parameters(), *ad), sdc);
      return &sdc->head;
    }
  }

private:
  /* The id of the step. */
  size_t id_;
  /* Parameters of the step. */
  const VariableList* parameters_;
  /* Domain of the parameters. */
  const ActionDomain* domain_;
};

/* Output operator for step domains. */
std::ostream& operator<<(std::ostream& os, const StepDomain& sd) {
  os << "<";
  for (VariableList::const_iterator vi = sd.parameters().begin();
       vi != sd.parameters().end(); vi++) {
    if (vi != sd.parameters().begin()) {
      os << ' ';
    }
    os << "?v" << -*vi << '[' << sd.id() << ']';
  }
  os << "> in " << sd.domain();
  return os;
}


/* Returns the step domain containing the given variable, or NULL if
   none does. */
static std::pair<const StepDomain*, size_t>
find_step_domain(const StepDomainChain* step_domains,
		 Variable var, size_t step_id) {
  if (step_id > 0) {
    for (const StepDomainChain* sd = step_domains; sd != NULL; sd = sd->tail) {
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
/* Binding */

/* Output operator for variable bindings. */
std::ostream& operator<<(std::ostream& os, const Binding& b) {
  return os << b.var() << '(' << b.var_id() << ')'
	    << (b.equality() ? "==" : "!=")
	    << b.term() << '(' << b.term_id() << ')';
}


/* ====================================================================== */
/* NameSet */

/* Constructs a name set. */
NameSet::NameSet()
  : ref_count_(0) {
#ifdef DEBUG_MEMORY
  created_name_sets++;
#endif
}


/* Deletes this name set. */
NameSet::~NameSet() {
#ifdef DEBUG_MEMORY
  deleted_name_sets++;
#endif
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
  for (ProjectionMapIter pi = projections_.begin();
       pi != projections_.end(); pi++) {
    NameSet::unregister_use((*pi).second);
  }
}


/* Number of tuples. */
size_t ActionDomain::size() const {
  return tuples_.size();
}


/* Adds a tuple to this domain. */
void ActionDomain::add(const ObjectList& tuple) {
  tuples_.push_back(&tuple);
}


/* Returns the set of names from the given column. */
const NameSet& ActionDomain::projection(size_t column) const {
  ProjectionMapIter pi = projections_.find(column);
  if (pi != projections_.end()) {
    return *(*pi).second;
  } else {
    NameSet* projection = new NameSet();
    for (TupleListIter ti = tuples_.begin(); ti != tuples_.end(); ti++) {
      const ObjectList& tuple = **ti;
      projection->insert(tuple[column]);
    }
    projections_.insert(std::make_pair(column, projection));
    NameSet::register_use(projection);
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
  for (TupleListIter ti = tuples_.begin(); ti != tuples_.end(); ti++) {
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
  for (TupleListIter ti = tuples_.begin(); ti != tuples_.end(); ti++) {
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
  for (TupleListIter ti = tuples_.begin(); ti != tuples_.end(); ti++) {
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


/* Output operator for action domains. */
std::ostream& operator<<(std::ostream& os, const ActionDomain& ad) {
  os << '{';
  for (ActionDomain::TupleListIter ti = ad.tuples_.begin();
       ti != ad.tuples_.end(); ti++) {
    if (ti != ad.tuples_.begin()) {
      os << ' ';
    }
    os << '<';
    const ObjectList& tuple = **ti;
    for (ObjectList::const_iterator ni = tuple.begin();
	 ni != tuple.end(); ni++) {
      if (ni != tuple.begin()) {
	os << ' ';
      }
      os << *ni;
    }
    os << '>';
  }
  os << '}';
  return os;
}


/* ====================================================================== */
/* Bindings */

/* Creates a collection of variable bindings with the given equality
   and inequality bindings. */
const Bindings* Bindings::make_bindings(const StepChain* steps,
					const PlanningGraph* pg) {
  const StepDomainChain* step_domains = NULL;
  for (const StepChain* sc = steps; sc != NULL; sc = sc->tail) {
    const Step& step = sc->head;
    if (!step.dummy()) {
      const ActionSchema* action =
	dynamic_cast<const ActionSchema*>(&step.action());
      if (action != NULL && !action->parameters().empty()) {
	const ActionDomain* domain = pg->action_domain(action->name());
	if (domain != NULL) {
	  step_domains =
	    new StepDomainChain(StepDomain(step.id(), action->parameters(),
					   *domain),
				step_domains);
	}
      }
    }
  }
  return new Bindings(NULL, 0, step_domains);
}


/* Checks if the given formulas can be unified. */
bool Bindings::unifiable(const Literal& l1, size_t id1,
			 const Literal& l2, size_t id2) {
  BindingList dummy;
  return unifiable(dummy, l1, id1, l2, id2);
}


/* Checks if the given formulas can be unified; the most general
   unifier is added to the provided substitution list. */
bool Bindings::unifiable(BindingList& mgu, const Literal& l1, size_t id1,
			 const Literal& l2, size_t id2) {
#ifdef DEBUG_MEMORY
  created_bindings--;
  deleted_bindings--;
#endif
  return Bindings(NULL, 0, NULL).unify(mgu, l1, id1, l2, id2);
}


/* Constructs an empty binding collection. */
Bindings::Bindings()
  : varsets_(NULL), high_step_(0), step_domains_(NULL) {
#ifdef DEBUG_MEMORY
  created_bindings++;
#endif
}


/* Constructs a binding collection. */
Bindings::Bindings(const VarsetChain* varsets, size_t high_step,
		   const StepDomainChain* step_domains)
  : varsets_(varsets), high_step_(high_step), step_domains_(step_domains),
    ref_count_(0) {
#ifdef DEBUG_MEMORY
  created_bindings++;
#endif
  VarsetChain::register_use(varsets_);
  StepDomainChain::register_use(step_domains_);
}


/* Deletes this binding collection. */
Bindings::~Bindings() {
#ifdef DEBUG_MEMORY
  deleted_bindings++;
#endif
  VarsetChain::unregister_use(varsets_);
  StepDomainChain::unregister_use(step_domains_);
}


/* Returns the binding for the given term, or the term itself if it is
     not bound to a single name. */
Term Bindings::binding(Term t, size_t step_id) const {
  if (is_variable(t)) {
    const Varset* vs = find_varset(varsets_, t, step_id);
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
  for (TermList::const_iterator ti = terms1.begin(), tj = terms2.begin();
       ti != terms1.end(); ti++, tj++) {
    /*
     * Try to unify a pair of terms.
     */
    Term term1 = *ti;
    Term term2 = *tj;
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
	bl.push_back(Binding(term2, id2, term1, 0, true));
      }
    } else {
      /* The first term is a variable. */
      bl.push_back(Binding(term1, id1, term2, id2, true));
    }
  }
  if (add(bl, true) == NULL) {
    /* Unification is inconsistent with current bindings. */
    return false;
  }
  for (BindingListIter bi = bl.begin(); bi != bl.end(); bi++) {
    /* Add unification to most general unifier. */
    const Binding& b = *bi;
    mgu.push_back(Binding(b.var(), b.var_id(), b.term(), b.term_id(),
			  true));
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
  const Varset* vs2 = find_varset(varsets_, term, term_id);
  if (vs2 == NULL || vs2->includes(var, var_id)) {
    return true;
  } else if (vs2->excludes(var, var_id)) {
    return false;
  } else if (vs2->constant() != Object(NULL_TERM)) {
    std::pair<const StepDomain*, size_t> sd =
      find_step_domain(step_domains_, var, var_id);
    if (sd.first != NULL) {
      return sd.first->includes(vs2->constant(), sd.second);
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
  const Varset* vs2 = find_varset(varsets_, term, term_id);
  return (vs2 == NULL
	  || !vs2->includes(var, var_id) || vs2->excludes(var, var_id));
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
	high_step = bind.var_id();
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
	high_step = bind.term_id();
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
	comb = Varset::make_varset(varsets, bind);
      }
      if (comb == NULL) {
	/* Binding is inconsistent with current bindings. */
	VarsetChain::register_use(varsets);
	VarsetChain::unregister_use(varsets);
	StepDomainChain::register_use(step_domains);
	StepDomainChain::unregister_use(step_domains);
	return NULL;
      } else {
	/* Binding is consistent with current bindings. */
	if (comb != vs1) {
	  /* Combined varset is new, so add it to the chain of varsets. */
	  Object name = comb->constant();
	  /* Restrict step domain for all codesignated variables. */
	  const NameSet* intersection = NULL;
	  const VariableChain* vc = NULL;
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
		    VarsetChain::register_use(varsets);
		    VarsetChain::unregister_use(varsets);
		    StepDomainChain::register_use(step_domains);
		    StepDomainChain::unregister_use(step_domains);
		    NameSet::unregister_use(intersection);
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
		      VarsetChain::register_use(varsets);
		      VarsetChain::unregister_use(varsets);
		      StepDomainChain::register_use(step_domains);
		      StepDomainChain::unregister_use(step_domains);
		      NameSet::unregister_use(intersection);
		      return NULL;
		    }
		    if (sd.first != new_sd) {
		      add_domain_bindings(new_binds, *sd.first, *new_sd);
		    }
		  } else if (intersection == NULL) {
		    intersection = &sd.first->projection(sd.second);
		    NameSet::register_use(intersection);
		  } else {
		    NameSet* cut = new NameSet();
		    const NameSet& set2 = sd.first->projection(sd.second);
		    set_intersection(intersection->begin(),
				     intersection->end(),
				     set2.begin(), set2.end(),
				     inserter(*cut, cut->begin()));
		    NameSet::unregister_use(intersection);
		    intersection = cut;
		    NameSet::register_use(intersection);
		    if (intersection->empty()) {
		      /* Domain became empty. */
		      VarsetChain::register_use(varsets);
		      VarsetChain::unregister_use(varsets);
		      StepDomainChain::register_use(step_domains);
		      StepDomainChain::unregister_use(step_domains);
		      NameSet::unregister_use(intersection);
		      return NULL;
		    }
		  }
		}
	      }
	    }
	  }
	  NameSet::unregister_use(intersection);
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
	high_step = bind.var_id();
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
	high_step = bind.term_id();
	high_step_vars.insert(sv);
	vs2 = NULL;
      }
      if (vs1 != NULL && vs2 != NULL && vs1 == vs2) {
	/* The terms are already bound to eachother. */
	VarsetChain::register_use(varsets);
	VarsetChain::unregister_use(varsets);
	StepDomainChain::register_use(step_domains);
	StepDomainChain::unregister_use(step_domains);
	return NULL;
      } else {
	/* The terms are not bound to eachother. */
	bool separate1 = true;
	bool separate2 = true;
	if (vs1 == NULL) {
	  /* The first term is unbound, so create a new varset for it. */
	  vs1 = Varset::make_varset(varsets, bind);
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
	  vs2 = Varset::make_varset(varsets, bind, true);
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
	    for (const VariableChain* vc = vs2->cd_set();
		 vc != NULL; vc = vc->tail) {
	      std::pair<const StepDomain*, size_t> sd =
		find_step_domain(step_domains,
				 vc->head.first, vc->head.second);
	      if (sd.first != NULL) {
		const StepDomain* new_sd =
		  sd.first->exclude(step_domains, vs1->constant(), sd.second);
		if (new_sd == NULL) {
		  /* Domain became empty. */
		  VarsetChain::register_use(varsets);
		  VarsetChain::unregister_use(varsets);
		  StepDomainChain::register_use(step_domains);
		  StepDomainChain::unregister_use(step_domains);
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
	    const VariableChain* vc = (vs1 != NULL) ? vs1->cd_set() : NULL;
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
		  VarsetChain::register_use(varsets);
		  VarsetChain::unregister_use(varsets);
		  StepDomainChain::register_use(step_domains);
		  StepDomainChain::unregister_use(step_domains);
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
    VarsetChain::register_use(varsets);
    VarsetChain::unregister_use(varsets);
    StepDomainChain::register_use(step_domains);
    StepDomainChain::unregister_use(step_domains);
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
  const StepDomainChain* step_domains =
    new StepDomainChain(StepDomain(step_id, action->parameters(), *domain),
			step_domains_);
  const VarsetChain* varsets = varsets_;
  size_t high_step = high_step_;
  const StepDomain& step_domain = step_domains->head;
  for (size_t c = 0; c < step_domain.width(); c++) {
    if (step_domain.projection_size(c) == 1) {
      const VariableChain* cd_set =
	new VariableChain(std::make_pair(step_domain.parameters()[c],
					 step_domain.id()),
			  NULL);
      varsets = new VarsetChain(Varset(*step_domain.projection(c).begin(),
				       cd_set, NULL),
				varsets);
      high_step = std::max(high_step, step_id);
    }
  }
  if (test_only) {
    StepDomainChain::register_use(step_domains);
    StepDomainChain::unregister_use(step_domains);
    return this;
  } else {
    return new Bindings(varsets, high_step, step_domains);
  }
}


/* Output operator for bindings. */
std::ostream& operator<<(std::ostream& os, const Bindings& b) {
  hashing::hash_map<size_t, std::vector<Variable> > seen_vars;
  std::vector<Object> seen_names;
  for (const VarsetChain* vsc = b.varsets_; vsc != NULL; vsc = vsc->tail) {
    const Varset& vs = vsc->head;
    if (vs.cd_set() != NULL) {
      const VariableChain* vc = vs.cd_set();
      if (find(seen_vars[vc->head.second].begin(),
	       seen_vars[vc->head.second].end(),
	       vc->head.first)
	  != seen_vars[vc->head.second].end()) {
	continue;
      }
      os << std::endl << "{";
      for (; vc != NULL; vc = vc->tail) {
	const StepVariable& step_var = vc->head;
	os << ' ' << step_var.first << '(' << step_var.second << ')';
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
      os << name;
      seen_names.push_back(name);
    }
    if (vs.ncd_set() != NULL) {
      os << " != {";
      for (const VariableChain* vc = vs.ncd_set(); vc != NULL; vc = vc->tail) {
	os << ' ' << vc->head.first << '(' << vc->head.second << ')';
      }
      os << " }";
    }
  }
  for (const StepDomainChain* sd = b.step_domains_;
       sd != NULL; sd = sd->tail) {
    os << std::endl << sd->head;
  }
  return os;
}
