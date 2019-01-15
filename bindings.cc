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

#include "bindings.h"

#include <limits.h>
#include <algorithm>
#include <limits>
#include <typeinfo>

#include "debug.h"
#include "domains.h"
#include "formulas.h"
#include "heuristics.h"
#include "plans.h"
#include "problems.h"
#include "refcount.h"
#include "types.h"

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
  Varset(const Object* constant, const Chain<StepVariable>* cd_set,
	 const Chain<StepVariable>* ncd_set, const Type& type)
    : constant_((constant != 0) ? new Object(*constant) : 0),
      cd_set_(cd_set), ncd_set_(ncd_set), type_(type) {
    RCObject::ref(cd_set_);
    RCObject::ref(ncd_set_);
  }

  /* Constructs a varset. */
  Varset(const Varset& vs)
    : constant_((vs.constant_ != 0) ? new Object(*vs.constant_) : 0),
      cd_set_(vs.cd_set_), ncd_set_(vs.ncd_set_), type_(vs.type_) {
    RCObject::ref(cd_set_);
    RCObject::ref(ncd_set_);
  }

  /* Deletes this varset. */
  ~Varset() {
    if (constant_ != 0) {
      delete constant_;
    }
    RCObject::destructive_deref(cd_set_);
    RCObject::destructive_deref(ncd_set_);
  }

  /* Pointer to the constant of this varset, or 0. */
  const Object* constant() const { return constant_; }

  /* The codesignation list. */
  const Chain<StepVariable>* cd_set() const { return cd_set_; }

  /* The non-codesignation list. */
  const Chain<StepVariable>* ncd_set() const { return ncd_set_; }

  /* Checks if this varset includes the given object. */
  bool includes(const Object& obj) const {
    return constant() != 0 && *constant() == obj;
  }

  /* Checks if this varset includes the given variable. */
  bool includes(const Variable& var, size_t step_id) const {
    for (const Chain<StepVariable>* vc = cd_set(); vc != 0; vc = vc->tail) {
      if (vc->head.first == var && vc->head.second == step_id) {
	return true;
      }
    }
    return false;
  }

  /* Checks if this varset excludes the given variable. */
  bool excludes(const Variable& var, size_t step_id) const {
    for (const Chain<StepVariable>* vc = ncd_set(); vc != 0; vc = vc->tail) {
      if (vc->head.first == var && vc->head.second == step_id) {
	return true;
      }
    }
    return false;
  }

  /* Returns the varset obtained by adding the given object to this
     varset, or 0 if the object is excluded from this varset. */
  const Varset* add(const Chain<Varset>*& vsc, const Object& obj) const {
    if (constant() != 0) {
      return (*constant() == obj) ? this : 0;
    } else {
      const Type& ot = TermTable::type(obj);
      if (TypeTable::subtype(ot, type_)) {
	vsc = new Chain<Varset>(Varset(&obj, cd_set(), ncd_set(), ot), vsc);
	return &vsc->head;
      } else {
	return 0;
      }
    }
  }

  /* Returns the varset obtained by adding the given variable to this
     varset, or 0 if the variable is excluded from this varset. */
  const Varset* add(const Chain<Varset>*& vsc, const Variable& var,
		    size_t step_id) const {
    if (excludes(var, step_id)) {
      return 0;
    } else {
      const Type* tt;
      if (constant() != 0) {
	if (!TypeTable::subtype(type_, TermTable::type(var))) {
	  return 0;
	}
	tt = &type_;
      } else {
	tt = TypeTable::most_specific(type_, TermTable::type(var));
	if (tt == 0) {
	  return 0;
	}
      }
      const Chain<StepVariable>* new_cd =
	new Chain<StepVariable>(std::make_pair(var, step_id), cd_set());
      vsc = new Chain<Varset>(Varset(constant(), new_cd, ncd_set(), *tt), vsc);
      return &vsc->head;
    }
  }

  /* Returns the varset obtained by adding the given term to this
     varset, or 0 if the term is excluded from this varset. */
  const Varset* add(const Chain<Varset>*& vsc, const Term& term,
		    size_t step_id) const {
    if (term.object()) {
      return add(vsc, term.as_object());
    } else {
      return add(vsc, term.as_variable(), step_id);
    }
  }

  /* Returns the varset obtained by adding the given variable to the
     non-codesignation list of this varset; N.B. assumes that the
     variable is not included in the varset already. */
  const Varset* restrict(const Chain<Varset>*& vsc,
			 const Variable& var, size_t step_id) const {
    const Chain<StepVariable>* new_ncd =
      new Chain<StepVariable>(std::make_pair(var, step_id), ncd_set());
    vsc = new Chain<Varset>(Varset(constant(), cd_set(), new_ncd, type_), vsc);
    return &vsc->head;
  }

  /* Returns the combination of this and the given varset, or 0 if
     the combination is inconsistent. */
  const Varset* combine(const Chain<Varset>*& vsc, const Varset& vs) const {
    const Object* comb_obj;
    const Type* tt;
    if (constant() != 0) {
      if (vs.constant() != 0) {
	if (constant() != vs.constant()) {
	  return 0;
	}
      } else if (!TypeTable::subtype(type_, vs.type_)) {
	return 0;
      }
      comb_obj = constant();
      tt = &type_;
    } else if (vs.constant() != 0) {
      if (!TypeTable::subtype(vs.type_, type_)) {
	return 0;
      }
      comb_obj = vs.constant();
      tt = &vs.type_;
    } else {
      comb_obj = 0;
      tt = TypeTable::most_specific(type_, vs.type_);
      if (tt == 0) {
	return 0;
      }
    }
    const Chain<StepVariable>* comb_cd = cd_set();
    for (const Chain<StepVariable>* vc = vs.cd_set(); vc != 0; vc = vc->tail) {
      const StepVariable& step_var = vc->head;
      if (excludes(step_var.first, step_var.second)) {
	RCObject::ref(comb_cd);
	RCObject::destructive_deref(comb_cd);
	return 0;
      } else {
	comb_cd = new Chain<StepVariable>(step_var, comb_cd);
      }
    }
    const Chain<StepVariable>* comb_ncd = ncd_set();
    for (const Chain<StepVariable>* vc = vs.ncd_set();
	 vc != 0; vc = vc->tail) {
      const StepVariable& step_var = vc->head;
      if (includes(step_var.first, step_var.second)) {
	RCObject::ref(comb_cd);
	RCObject::destructive_deref(comb_cd);
	RCObject::ref(comb_ncd);
	RCObject::destructive_deref(comb_ncd);
	return 0;
      } else if (!excludes(step_var.first, step_var.second)) {
	comb_ncd = new Chain<StepVariable>(step_var, comb_ncd);
      }
    }
    vsc = new Chain<Varset>(Varset(comb_obj, comb_cd, comb_ncd, *tt), vsc);
    return &vsc->head;
  }

  /* Returns the varset representing the given equality binding. */
  static const Varset* make(const Chain<Varset>*& vsc, const Binding& b,
			    bool reverse = false) {
    if (b.equality()) {
      const Chain<StepVariable>* cd_set =
	new Chain<StepVariable>(std::make_pair(b.var(), b.var_id()), 0);
      if (b.term().object()) {
	Object obj = b.term().as_object();
	vsc = new Chain<Varset>(Varset(&obj, cd_set, 0,
				       TermTable::type(b.term())),
				vsc);
      } else {
	const Type* tt = TypeTable::most_specific(TermTable::type(b.var()),
						  TermTable::type(b.term()));
	if (tt == 0) {
	  RCObject::ref(cd_set);
	  RCObject::destructive_deref(cd_set);
	  return 0;
	}
	cd_set = new Chain<StepVariable>(std::make_pair(b.term().as_variable(),
							b.term_id()),
					 cd_set);
	vsc = new Chain<Varset>(Varset(0, cd_set, 0, *tt), vsc);
      }
      return &vsc->head;
    } else {
      if (reverse) {
	const Chain<StepVariable>* ncd_set =
	  new Chain<StepVariable>(std::make_pair(b.var(), b.var_id()), 0);
	if (b.term().object()) {
	  Object obj = b.term().as_object();
	  vsc = new Chain<Varset>(Varset(&obj, 0, ncd_set,
					 TermTable::type(b.term())),
				  vsc);
	} else {
	  Variable var = b.term().as_variable();
	  const Chain<StepVariable>* cd_set =
	    new Chain<StepVariable>(std::make_pair(var, b.term_id()), 0);
	  vsc = new Chain<Varset>(Varset(0, cd_set, ncd_set,
					 TermTable::type(b.term())),
				  vsc);
	}
	return &vsc->head;
      } else { /* !reverse */
	if (b.term().object()) {
	  return 0;
	} else {
	  Variable var = b.term().as_variable();
	  const Chain<StepVariable>* cd_set =
	    new Chain<StepVariable>(std::make_pair(b.var(), b.var_id()), 0);
	  const Chain<StepVariable>* ncd_set =
	    new Chain<StepVariable>(std::make_pair(var, b.term_id()), 0);
	  vsc = new Chain<Varset>(Varset(0, cd_set, ncd_set,
					 TermTable::type(b.var())),
				  vsc);
	  return &vsc->head;
	}
      }
    }
  }

private:
  /* Pointer to the constant of this varset, or 0. */
  const Object* constant_;
  /* The codesignation list. */
  const Chain<StepVariable>* cd_set_;
  /* The non-codesignation list. */
  const Chain<StepVariable>* ncd_set_;
  /* The most specific type of any variable in this set. */
  Type type_;
};


/* Returns the varset containing the given object, or 0 if none do. */
static const Varset* find_varset(const Chain<Varset>* varsets,
				 const Object& obj) {
  for (const Chain<Varset>* vsc = varsets; vsc != 0; vsc = vsc->tail) {
    const Varset& vs = vsc->head;
    if (vs.includes(obj)) {
      return &vs;
    }
  }
  return 0;
}


/* Returns the varset containing the given variable, or 0 if none do. */
static const Varset* find_varset(const Chain<Varset>* varsets,
				 const Variable& var, size_t step_id) {
  for (const Chain<Varset>* vsc = varsets; vsc != 0; vsc = vsc->tail) {
    const Varset& vs = vsc->head;
    if (vs.includes(var, step_id)) {
      return &vs;
    }
  }
  return 0;
}


/* Returns the varset containing the given term, or 0 if none do. */
static const Varset* find_varset(const Chain<Varset>* varsets,
				 const Term& term, size_t step_id) {
  if (term.object()) {
    return find_varset(varsets, term.as_object());
  } else {
    return find_varset(varsets, term.as_variable(), step_id);
  }
}


/* ====================================================================== */
/* StepDomain */

/*
 * A step domain.
 */
struct StepDomain {
  /* Constructs a step domain. */
  StepDomain(size_t id, const std::vector<Variable>& parameters,
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
  const std::vector<Variable>& parameters() const { return *parameters_; }

  /* Returns the parameter domains. */
  const ActionDomain& domain() const { return *domain_; }

  /* Returns the index of the variable in this step domain, or -1 if
     the variable is not included. */
  int index_of(const Variable& var) const {
    std::vector<Variable>::const_iterator vi =
        find(parameters().begin(), parameters().end(), var);
    return (vi != parameters().end()) ? vi - parameters().begin() : -1;
  }

  /* Checks if this step domain includes the given object in the given
     column. */
  bool includes(const Object& obj, size_t column) const {
    for (TupleList::const_iterator ti = domain().tuples().begin();
	 ti != domain().tuples().end(); ti++) {
      if ((**ti)[column] == obj) {
	return true;
      }
    }
    return false;
  }

  /* Returns the set of objects from the given column. */
  const NameSet& projection(size_t column) const {
    return domain().projection(column);
  }

  /* Returns the size of the projection of the given column. */
  const size_t projection_size(size_t column) const {
    return domain().projection_size(column);
  }

  /* Returns a domain where the given column has been restricted to
     the given object, or 0 if this would leave an empty domain. */
  const StepDomain* restrict(const Chain<StepDomain>*& sdc,
			     const Object& obj, size_t column) const {
    const ActionDomain* ad = domain().restrict(obj, column);
    if (ad == 0) {
      return 0;
    } else if (ad == &domain()) {
      return this;
    } else {
      sdc = new Chain<StepDomain>(StepDomain(id(), parameters(), *ad), sdc);
      return &sdc->head;
    }
  }

  /* Returns a domain where the given column has been restricted to
     the given set of objects, or 0 if this would leave an empty
     domain. */
  const StepDomain* restrict(const Chain<StepDomain>*& sdc,
			     const NameSet& names, size_t column) const {
    const ActionDomain* ad = domain().restrict(names, column);
    if (ad == 0) {
      return 0;
    } else if (ad == &domain()) {
      return this;
    } else {
      sdc = new Chain<StepDomain>(StepDomain(id(), parameters(), *ad), sdc);
      return &sdc->head;
    }
  }

  /* Returns a domain where the given column exclues the given object,
     or 0 if this would leave an empty domain. */
  const StepDomain* exclude(const Chain<StepDomain>*& sdc,
			    const Object& obj, size_t column) const {
    const ActionDomain* ad = domain().exclude(obj, column);
    if (ad == 0) {
      return 0;
    } else if (ad == &domain()) {
      return this;
    } else {
      sdc = new Chain<StepDomain>(StepDomain(id(), parameters(), *ad), sdc);
      return &sdc->head;
    }
  }

  /* Prints this object on the given stream. */
  void print(std::ostream& os) const;

private:
  /* The id of the step. */
  size_t id_;
  /* Parameters of the step. */
  const std::vector<Variable>* parameters_;
  /* Domain of the parameters. */
  const ActionDomain* domain_;
};


/* Prints this object on the given stream. */
void StepDomain::print(std::ostream& os) const {
  os << "<";
  for (std::vector<Variable>::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    if (vi != parameters().begin()) {
      os << ' ';
    }
    os << *vi << '(' << id() << ')';
  }
  os << "> in ";
  domain().print(os);
}


/* Returns the step domain containing the given variable and the
   column of the variable, or 0 no step domain contains the
   variable. */
static std::pair<const StepDomain*, size_t>
find_step_domain(const Chain<StepDomain>* step_domains,
		 const Variable& var, size_t step_id) {
  if (step_id > 0) {
    for (const Chain<StepDomain>* sd = step_domains; sd != 0; sd = sd->tail) {
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
  return std::pair<const StepDomain*, size_t>(0, 0);
}


/* ====================================================================== */
/* ActionDomain */

/* Constructs an action domain with a single tuple. */
ActionDomain::ActionDomain(const std::vector<Object>& tuple) : ref_count_(0) {
  add(tuple);
}

/* Deletes this action domain. */
ActionDomain::~ActionDomain() {
  for (ProjectionMap::const_iterator pi = projections_.begin();
       pi != projections_.end(); pi++) {
    delete (*pi).second;
  }
}


/* Adds a tuple to this domain. */
void ActionDomain::add(const std::vector<Object>& tuple) {
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
      const std::vector<Object>& tuple = **ti;
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
   the given object, or 0 if this would leave an empty domain. */
const ActionDomain* ActionDomain::restrict(const Object& obj,
					   size_t column) const {
  ActionDomain* new_domain = 0;
  for (TupleList::const_iterator ti = tuples().begin();
       ti != tuples().end(); ti++) {
    const std::vector<Object>& tuple = **ti;
    if (tuple[column] == obj) {
      if (new_domain == 0) {
	new_domain = new ActionDomain(tuple);
      } else {
	new_domain->add(tuple);
      }
    }
  }
  if (new_domain != 0 && new_domain->size() == size()) {
    ActionDomain::register_use(new_domain);
    ActionDomain::unregister_use(new_domain);
    return this;
  } else {
    return new_domain;
  }
}


/* Returns a domain where the given column has been restricted to
   the given set of names, or 0 if this would leave an empty
   domain. */
const ActionDomain* ActionDomain::restrict(const NameSet& names,
					   size_t column) const {
  ActionDomain* new_domain = 0;
  for (TupleList::const_iterator ti = tuples().begin();
       ti != tuples().end(); ti++) {
    const std::vector<Object>& tuple = **ti;
    if (names.find(tuple[column]) != names.end()) {
      if (new_domain == 0) {
	new_domain = new ActionDomain(tuple);
      } else {
	new_domain->add(tuple);
      }
    }
  }
  if (new_domain != 0 && new_domain->size() == size()) {
    ActionDomain::register_use(new_domain);
    ActionDomain::unregister_use(new_domain);
    return this;
  } else {
    return new_domain;
  }
}


/* Returns a domain where the given column exclues the given object,
   or 0 if this would leave an empty domain. */
const ActionDomain* ActionDomain::exclude(const Object& obj,
					  size_t column) const {
  ActionDomain* new_domain = 0;
  for (TupleList::const_iterator ti = tuples().begin();
       ti != tuples().end(); ti++) {
    const std::vector<Object>& tuple = **ti;
    if (tuple[column] != obj) {
      if (new_domain == 0) {
	new_domain = new ActionDomain(tuple);
      } else {
	new_domain->add(tuple);
      }
    }
  }
  if (new_domain != 0 && new_domain->size() == size()) {
    ActionDomain::register_use(new_domain);
    ActionDomain::unregister_use(new_domain);
    return this;
  } else {
    return new_domain;
  }
}


/* Prints this object on the given stream. */
void ActionDomain::print(std::ostream& os) const {
  os << '{';
  for (TupleList::const_iterator ti = tuples().begin();
       ti != tuples().end(); ti++) {
    if (ti != tuples().begin()) {
      os << ' ';
    }
    os << '<';
    const std::vector<Object>& tuple = **ti;
    for (std::vector<Object>::const_iterator ni = tuple.begin();
         ni != tuple.end(); ni++) {
      if (ni != tuple.begin()) {
	os << ' ';
      }
      os << *ni;
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
  : varsets_(0), high_step_(0), step_domains_(0), ref_count_(1) {
}


/* Constructs a binding collection. */
Bindings::Bindings(const Chain<Varset>* varsets, size_t high_step,
		   const Chain<StepDomain>* step_domains)
  : varsets_(varsets), high_step_(high_step), step_domains_(step_domains),
    ref_count_(0) {
  RCObject::ref(varsets_);
  RCObject::ref(step_domains_);
}


/* Deletes this binding collection. */
Bindings::~Bindings() {
  RCObject::destructive_deref(varsets_);
  RCObject::destructive_deref(step_domains_);
}


/* Returns the binding for the given term, or the term itself if it is
     not bound to a single object. */
Term Bindings::binding(const Term& term, size_t step_id) const {
  if (term.variable()) {
    const Varset* vs =
      ((step_id <= high_step_)
       ? find_varset(varsets_, term.as_variable(), step_id) : 0);
    if (vs != 0 && vs->constant() != 0) {
      return *vs->constant();
    }
  }
  return term;
}


/* Returns the domain for the given step variable. */
const NameSet& Bindings::domain(const Variable& var, size_t step_id,
				const Problem& problem) const {
  std::pair<const StepDomain*, size_t> sd =
    find_step_domain(step_domains_, var, step_id);
  if (sd.first != 0) {
    return sd.first->projection(sd.second);
  } else {
    const std::vector<Object>& objects =
        problem.terms().compatible_objects(TermTable::type(var));
    NameSet* names = new NameSet();
    names->insert(objects.begin(), objects.end());
    const Varset* vs =
      (step_id <= high_step_) ? find_varset(varsets_, var, step_id) : 0;
    if (vs != 0) {
      for (const Chain<StepVariable>* vc = vs->ncd_set();
	   vc != 0; vc = vc->tail) {
	const StepVariable& sv = vc->head;
	const Varset* vs2 = ((sv.second <= high_step_)
			     ? find_varset(varsets_, sv.first, sv.second) : 0);
	if (vs2 != 0 && vs2->constant() != 0) {
	  names->erase(*vs2->constant());
	}
      }
    }
    return *names;
  }
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
  if (negation != 0) {
    return unify(mgu, l2, id2, negation->atom(), id1);
  } else {
    negation = dynamic_cast<const Negation*>(&l2);
    if (negation != 0) {
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
bool Bindings::unify(BindingList& mgu, const Literal& l1, size_t id1,
		     const Literal& l2, size_t id2) const {
  if (l1.id() > 0 && l2.id() > 0) {
    /* Both literals are fully instantiated. */
    return &l1 == &l2;
  } else if (typeid(l1) != typeid(l2)) {
    /* Not the same type of literal. */
    return false;
  } else if (l1.predicate() != l2.predicate()) {
    /* The predicates do not match. */
    return false;
  } else if (l1.id() > 0 || l2.id() > 0) {
    /* One of the literals is fully instantiated. */
    const Literal* ll;
    const Literal* lg;
    size_t idl;
    if (l1.id() > 0) {
      ll = &l2;
      lg = &l1;
      idl = id2;
    } else {
      ll = &l1;
      lg = &l2;
      idl = id1;
    }
    std::map<Variable, Term> bind;
    size_t n = ll->arity();
    for (size_t i = 0; i < n; i++) {
      const Term& term1 = ll->term(i);
      Object obj2 = lg->term(i).as_object();
      if (term1.object()) {
	if (term1 != obj2) {
	  return false;
	}
      } else {
	Variable var1 = term1.as_variable();
        std::map<Variable, Term>::const_iterator b = bind.find(var1);
        if (b != bind.end()) {
	  if ((*b).second != obj2) {
	    return false;
	  }
	} else {
	  Term bt = binding(term1, idl);
	  if (bt.object()) {
	    if (bt != obj2) {
	      return false;
	    }
	  } else {
	    if (!TypeTable::subtype(TermTable::type(obj2),
				    TermTable::type(term1))) {
	      return false;
	    }
	    mgu.push_back(Binding(var1, idl, obj2, 0, true));
	  }
	  bind.insert(std::make_pair(var1, obj2));
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
      const Term& term1 = l1.term(i);
      const Term& term2 = l2.term(i);
      if (term1.object()) {
	/* The first term is an object. */
	if (term2.object()) {
	  /*
	   * Both terms are objects.
	   */
	  if (term1 != term2) {
	    /* The two terms are different objects. */
	    return false;
	  }
	} else {
	  /*
	   * The first term is an object and the second is a variable.
	   */
	  if (!TypeTable::subtype(TermTable::type(term1),
				  TermTable::type(term2))) {
	    /* Incompatible term types. */
	    return false;
	  }
	  mgu.push_back(Binding(term2.as_variable(), id2, term1, 0, true));
	}
      } else {
	/*
	 * The first term is a variable.
	 */
	if (term2.object()) {
	  if (!TypeTable::subtype(TermTable::type(term2),
				  TermTable::type(term1))) {
	    /* Incompatible term types. */
	    return false;
	  }
	} else if (!TypeTable::compatible(TermTable::type(term1),
					  TermTable::type(term2))) {
	  /* Incompatible term types. */
	  return false;
	}
	mgu.push_back(Binding(term1.as_variable(), id1, term2, id2, true));
      }
    }
  }
  if (add(mgu, true) == 0) {
    /* Unification is inconsistent with current bindings. */
    return false;
  } else {
    /* Successful unification. */
    return true;
  }
}


/* Checks if the given equality is consistent with the current
   bindings. */
bool Bindings::consistent_with(const Equality& eq, size_t step_id) const {
  size_t var_id = eq.step_id1(step_id);
  size_t term_id = eq.step_id2(step_id);
  const Varset* vs =
    (term_id <= high_step_) ? find_varset(varsets_, eq.term(), term_id) : 0;
  if (vs == 0 || vs->includes(eq.variable(), var_id)) {
    return true;
  } else if (vs->excludes(eq.variable(), var_id)) {
    return false;
  } else if (vs->constant() != 0) {
    std::pair<const StepDomain*, size_t> sd =
      find_step_domain(step_domains_, eq.variable(), var_id);
    if (sd.first != 0) {
      return sd.first->includes(*vs->constant(), sd.second);
    }
  }
  return true;
}


/* Checks if the given inequality is consistent with the current
   bindings. */
bool Bindings::consistent_with(const Inequality& neq, size_t step_id) const {
  size_t var_id = neq.step_id1(step_id);
  size_t term_id = neq.step_id2(step_id);
  const Varset* vs =
    (term_id <= high_step_) ? find_varset(varsets_, neq.term(), term_id) : 0;
  return (vs == 0
	  || !vs->includes(neq.variable(), var_id)
	  || vs->excludes(neq.variable(), var_id));
}


/* Adds bindings to the list as determined by difference between the
   given step domains */
static void add_domain_bindings(BindingList& bindings,
				const StepDomain& old_sd,
				const StepDomain& new_sd,
				size_t ex_column = std::numeric_limits<unsigned int>::max()) {
  for (size_t c = 0; c < old_sd.parameters().size(); c++) {
    if (c != ex_column && new_sd.projection_size(c) == 1
	&& old_sd.projection_size(c) > 1) {
      bindings.push_back(Binding(new_sd.parameters()[c], new_sd.id(),
				 *new_sd.projection(c).begin(), 0, true));
    }
  }
}


/* Returns the binding collection obtained by adding the given
   bindings to this binding collection, or 0 if the new bindings
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
	vs1 = 0;
      }
      /* Varset for term. */
      const Varset* vs2;
      if (bind.term().object()) {
	vs2 = find_varset(varsets, bind.term().as_object());
      } else {
	StepVariable sv(bind.term().as_variable(), bind.term_id());
	if (bind.term_id() <= high_step_
	    || high_step_vars.find(sv) != high_step_vars.end()) {
	  vs2 = find_varset(varsets, sv.first, bind.term_id());
	} else {
	  if (bind.term_id() > high_step) {
	    high_step = bind.term_id();
	  }
	  high_step_vars.insert(sv);
	  vs2 = 0;
	}
      }
      /* Combined varset, or 0 if binding is inconsistent with
         current bindings. */
      const Varset* comb;
      if (vs1 != 0 || vs2 != 0) {
	/* At least one of the terms is already bound. */
	if (vs1 != vs2) {
	  /* The terms are not yet bound to eachother. */
	  if (vs1 == 0) {
	    /* The first term is unbound, so add it to the varset of
               the second. */
	    comb = vs2->add(varsets, bind.var(), bind.var_id());
	  } else if (vs2 == 0) {
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
      if (comb == 0) {
	/* Binding is inconsistent with current bindings. */
	RCObject::ref(varsets);
	RCObject::destructive_deref(varsets);
	RCObject::ref(step_domains);
	RCObject::destructive_deref(step_domains);
	return 0;
      } else {
	/* Binding is consistent with current bindings. */
	if (comb != vs1) {
	  /* Combined varset is new, so add it to the chain of varsets. */
	  const Object* obj = comb->constant();
	  /* Restrict step domain for all codesignated variables. */
	  const NameSet* intersection = 0;
	  bool new_intersection = false;
	  const Chain<StepVariable>* vc = 0;
	  int phase = 0;
	  while (phase < 4 || (intersection != 0 && phase < 8)) {
	    const Variable* var = 0;
	    size_t var_id = 0;
	    switch (phase) {
	    case 0:
	    case 4:
	      if (vs1 == 0) {
		var = new Variable(bind.var());
		var_id = bind.var_id();
		phase += 2;
	      } else if (vs1->constant() == 0) {
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
	      if (vc != 0) {
		var = new Variable(vc->head.first);
		var_id = vc->head.second;
		vc = vc->tail;
	      } else {
		var = 0;
		phase++;
	      }
	      break;
	    case 2:
	    case 6:
	      if (vs2 == 0) {
		var = (bind.term().variable()
		       ? new Variable(bind.term().as_variable()) : 0);
		var_id = bind.term_id();
		phase += 2;
	      } else if (vs2->constant() == 0) {
		vc = vs2->cd_set();
		phase++;
	      } else {
		phase += 2;
	      }
	      break;
	    }
	    if (var != 0) {
	      /* Step domain for variable. */
	      std::pair<const StepDomain*, size_t> sd =
		find_step_domain(step_domains, *var, var_id);
	      delete var;
	      if (sd.first != 0) {
		if (obj != 0) {
		  const StepDomain* new_sd =
		    sd.first->restrict(step_domains, *obj, sd.second);
		  if (new_sd == 0) {
		    /* Domain became empty. */
		    RCObject::ref(varsets);
		    RCObject::destructive_deref(varsets);
		    RCObject::ref(step_domains);
		    RCObject::destructive_deref(step_domains);
		    if (new_intersection) {
		      delete intersection;
		    }
		    return 0;
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
		    if (new_sd == 0) {
		      /* Domain became empty. */
		      RCObject::ref(varsets);
		      RCObject::destructive_deref(varsets);
		      RCObject::ref(step_domains);
		      RCObject::destructive_deref(step_domains);
		      if (new_intersection) {
			delete intersection;
		      }
		      return 0;
		    }
		    if (sd.first != new_sd) {
		      add_domain_bindings(new_binds, *sd.first, *new_sd);
		    }
		  } else if (intersection == 0) {
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
		      RCObject::ref(varsets);
		      RCObject::destructive_deref(varsets);
		      RCObject::ref(step_domains);
		      RCObject::destructive_deref(step_domains);
		      if (new_intersection) {
			delete intersection;
		      }
		      return 0;
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
	vs1 = 0;
      }
      /* Varset for term. */
      const Varset* vs2;
      if (bind.term().object()) {
	vs2 = find_varset(varsets, bind.term().as_object());
      } else {
	StepVariable sv(bind.term().as_variable(), bind.term_id());
	if (bind.term_id() <= high_step_
	    || high_step_vars.find(sv) != high_step_vars.end()) {
	  vs2 = find_varset(varsets, sv.first, bind.term_id());
	} else {
	  if (bind.term_id() > high_step) {
	    high_step = bind.term_id();
	  }
	  high_step_vars.insert(sv);
	  vs2 = 0;
	}
      }
      if (vs1 != 0 && vs2 != 0 && vs1 == vs2) {
	/* The terms are already bound to eachother. */
	RCObject::ref(varsets);
	RCObject::destructive_deref(varsets);
	RCObject::ref(step_domains);
	RCObject::destructive_deref(step_domains);
	return 0;
      } else {
	/* The terms are not bound to eachother. */
	bool separate1 = true;
	bool separate2 = true;
	if (vs1 == 0) {
	  /* The first term is unbound, so create a new varset for it. */
	  vs1 = Varset::make(varsets, bind);
	} else {
	  if (bind.term().variable()) {
	    /* The second term is a variable. */
	    Variable var = bind.term().as_variable();
	    if (vs1->excludes(var, bind.term_id())) {
	      /* The second term is already separated from the first. */
	      separate1 = false;
	    } else {
	      /* Separate the second term from the first. */
	      vs1 = vs1->restrict(varsets, var, bind.term_id());
	    }
	  } else {
	    /* The second term is an object, so the terms are separated
               in the varset for the second term. */
	    separate1 = false;
	  }
	}
	if (vs2 == 0) {
	  /* The second term is unbound, so create a new varset for it. */
	  vs2 = Varset::make(varsets, bind, true);
	} else if (vs2->excludes(bind.var(), bind.var_id())) {
	  /* The first term is already separated from the second. */
	  separate2 = false;
	} else {
	  /* Separate the first term from the second. */
	  vs2 = vs2->restrict(varsets, bind.var(), bind.var_id());
	}
	if (separate1 && vs1 != 0) {
	  /* The second term was not separated from the first already. */
	  if (vs1->constant() != 0 && vs2 != 0) {
	    for (const Chain<StepVariable>* vc = vs2->cd_set();
		 vc != 0; vc = vc->tail) {
	      std::pair<const StepDomain*, size_t> sd =
		find_step_domain(step_domains,
				 vc->head.first, vc->head.second);
	      if (sd.first != 0) {
		const StepDomain* new_sd =
		  sd.first->exclude(step_domains, *vs1->constant(), sd.second);
		if (new_sd == 0) {
		  /* Domain became empty. */
		  RCObject::ref(varsets);
		  RCObject::destructive_deref(varsets);
		  RCObject::ref(step_domains);
		  RCObject::destructive_deref(step_domains);
		  return 0;
		}
		if (sd.first != new_sd) {
		  add_domain_bindings(new_binds, *sd.first, *new_sd);
		}
	      }
	    }
	  }
	}
	if (separate2 && vs2 != 0) {
	  /* The first term was not separated from the second already. */
	  if (vs2->constant() != 0) {
	    const Chain<StepVariable>* vc = (vs1 != 0) ? vs1->cd_set() : 0;
	    const Variable* var = (vc != 0) ? &vc->head.first : &bind.var();
	    size_t var_id = (vc != 0) ? vc->head.second : bind.var_id();
	    while (var != 0) {
	      std::pair<const StepDomain*, size_t> sd =
		find_step_domain(step_domains, *var, var_id);
	      if (sd.first != 0) {
		const StepDomain* new_sd =
		  sd.first->exclude(step_domains, *vs2->constant(), sd.second);
		if (new_sd == 0) {
		  /* Domain became empty. */
		  RCObject::ref(varsets);
		  RCObject::destructive_deref(varsets);
		  RCObject::ref(step_domains);
		  RCObject::destructive_deref(step_domains);
		  return 0;
		}
		if (sd.first != new_sd) {
		  add_domain_bindings(new_binds, *sd.first, *new_sd);
		}
	      }
	      vc = (vs1 != 0) ? vc->tail : 0;
	      var = (vc != 0) ? &vc->head.first : 0;
	      var_id = (vc != 0) ? vc->head.second : 0;
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
    RCObject::ref(varsets);
    RCObject::destructive_deref(varsets);
    RCObject::ref(step_domains);
    RCObject::destructive_deref(step_domains);
    return this;
  } else {
    return new Bindings(varsets, high_step, step_domains);
  }
}


/* Returns the binding collection obtained by adding the constraints
   associated with the given step to this binding collection, or
   0 if the new binding collection would be inconsistent. */
const Bindings* Bindings::add(size_t step_id, const Action& step_action,
			      const PlanningGraph& pg, bool test_only) const {
  const ActionSchema* action = dynamic_cast<const ActionSchema*>(&step_action);
  if (action == 0 || action->parameters().empty()) {
    return this;
  }
  const ActionDomain* domain = pg.action_domain(action->name());
  if (domain == 0) {
    return 0;
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
				0);
      Type type = TermTable::type(step_domain.parameters()[c]);
      varsets = new Chain<Varset>(Varset(&*step_domain.projection(c).begin(),
					 cd_set, 0, type),
				  varsets);
      if (step_id > high_step) {
	high_step = step_id;
      }
    }
  }
  if (test_only
      || (varsets == varsets_ && high_step == high_step_
	  && step_domains == step_domains_)) {
    RCObject::ref(varsets);
    RCObject::destructive_deref(varsets);
    RCObject::ref(step_domains);
    RCObject::destructive_deref(step_domains);
    return this;
  } else {
    return new Bindings(varsets, high_step, step_domains);
  }
}


/* Prints this object on the given stream. */
void Bindings::print(std::ostream& os) const {
  std::map<size_t, std::vector<Variable> > seen_vars;
  std::vector<Object> seen_objs;
  for (const Chain<Varset>* vsc = varsets_; vsc != 0; vsc = vsc->tail) {
    const Varset& vs = vsc->head;
    if (vs.cd_set() != 0) {
      const Chain<StepVariable>* vc = vs.cd_set();
      if (find(seen_vars[vc->head.second].begin(),
	       seen_vars[vc->head.second].end(), vc->head.first)
	  != seen_vars[vc->head.second].end()) {
	continue;
      }
      os << std::endl << "{";
      for (; vc != 0; vc = vc->tail) {
	const StepVariable& step_var = vc->head;
	os << ' ' << step_var.first << '(' << step_var.second << ')';
	seen_vars[step_var.second].push_back(step_var.first);
      }
      os << " }";
      if (vs.constant() != 0) {
	os << " == ";
      }
    }
    if (vs.constant() != 0) {
      const Object& obj = *vs.constant();
      if (find(seen_objs.begin(), seen_objs.end(), obj) != seen_objs.end()) {
	continue;
      }
      if (vs.cd_set() == 0) {
	os << std::endl;
      }
      os << obj;
      seen_objs.push_back(obj);
    }
    if (vs.ncd_set() != 0) {
      os << " != {";
      for (const Chain<StepVariable>* vc =
	     vs.ncd_set(); vc != 0; vc = vc->tail) {
	os << ' ' << vc->head.first << '(' << vc->head.second << ')';
      }
      os << " }";
    }
  }
  std::set<size_t> seen_steps;
  for (const Chain<StepDomain>* sd = step_domains_; sd != 0; sd = sd->tail) {
    if (seen_steps.find(sd->head.id()) == seen_steps.end()) {
      seen_steps.insert(sd->head.id());
      os << std::endl;
      sd->head.print(os);
    }
  }
}


/* Prints the given term on the given stream. */
void Bindings::print_term(std::ostream& os,
			  const Term& term, size_t step_id) const {
  Term t = binding(term, step_id);
  os << t;
  if (t.variable()) {
    os << '(' << step_id << ')';
  }
}
