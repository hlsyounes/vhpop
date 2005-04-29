/*
 * PDDL terms.
 *
 * Copyright (C) 2002-2005 Carnegie Mellon University
 *
 * This file is part of VHPOP.
 *
 * VHPOP is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * VHPOP is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VHPOP; if not, write to the Free Software Foundation,
 * Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * $Id: terms.cc,v 6.3 2005-04-29 09:30:22 lorens Exp $
 */
#include "terms.h"
#include <typeinfo>


/* ====================================================================== */
/* Term */

/* Converts this term to an object.  Fails if the term is not an
   object. */
Object Term::as_object() const {
  if (object()) {
    return Object(index_);
  } else {
    throw std::bad_cast();
  }
}


/* Converts this term to a variable.  Fails if the term is not a
   variable. */
Variable Term::as_variable() const {
  if (variable()) {
    return Variable(index_);
  } else {
    throw std::bad_cast();
  }
}


/* Output operator for terms. */
std::ostream& operator<<(std::ostream& os, const Term& t) {
  if (t.object()) {
    os << TermTable::names_[t.index_];
  } else {
    os << "?v" << -t.index_;
  }
  return os;
}


/* ====================================================================== */
/* TermTable */

/* Object names. */
std::vector<std::string> TermTable::names_;
/* Object types. */
TypeList TermTable::object_types_;
/* Variable types. */
TypeList TermTable::variable_types_;


/* Deletes this term table. */
TermTable::~TermTable() {
  for (std::map<Type, const ObjectList*>::const_iterator oi =
	 compatible_.begin();
       oi != compatible_.end(); oi++) {
    delete (*oi).second;
  }
}


/* Adds a variable with the given name and type to this term table
   and returns the variable. */
Variable TermTable::add_variable(const Type& type) {
  variable_types_.push_back(type);
  return Variable(-variable_types_.size());
}


/* Sets the type of the given term. */
void TermTable::set_type(const Term& term, const Type& type) {
  if (term.object()) {
    object_types_[term.index_] = type;
  } else {
    variable_types_[-term.index_ - 1] = type;
  }
}


/* Returns the type of the given term. */
const Type& TermTable::type(const Term& term) {
  if (term.object()) {
    return object_types_[term.index_];
  } else {
    return variable_types_[-term.index_ - 1];
  }
}


/* Adds an object with the given name and type to this term table
   and returns the object. */
const Object& TermTable::add_object(const std::string& name,
				    const Type& type) {
  std::pair<std::map<std::string, Object>::const_iterator, bool> oi =
    objects_.insert(std::make_pair(name, Object(names_.size())));
  names_.push_back(name);
  object_types_.push_back(type);
  return (*oi.first).second;
}


/* Returns the object with the given name, or 0 if no object with
   the given name exists. */
const Object* TermTable::find_object(const std::string& name) const {
  std::map<std::string, Object>::const_iterator oi = objects_.find(name);
  if (oi != objects_.end()) {
    return &(*oi).second;
  } else if (parent_ != 0) {
    return parent_->find_object(name);
  } else {
    return 0;
  }
}


/* Returns a list with objects that are compatible with the given
   type. */
const ObjectList& TermTable::compatible_objects(const Type& type) const {
  std::map<Type, const ObjectList*>::const_iterator oi =
    compatible_.find(type);
  if (oi != compatible_.end()) {
    return *(*oi).second;
  } else {
    ObjectList* objects;
    if (parent_ != 0) {
      objects = new ObjectList(parent_->compatible_objects(type));
    } else {
      objects = new ObjectList();
    }
    for (std::map<std::string, Object>::const_iterator oi = objects_.begin();
	 oi != objects_.end(); oi++) {
      const Object& o = (*oi).second;
      if (TypeTable::subtype(TermTable::type(o), type)) {
	objects->push_back(o);
      }
    }
    return *objects;
  }
}


/* Output operator for term tables. */
std::ostream& operator<<(std::ostream& os, const TermTable& t) {
  if (t.parent_ != 0) {
    os << *t.parent_;
  }
  for (std::map<std::string, Object>::const_iterator oi = t.objects_.begin();
       oi != t.objects_.end(); oi++) {
    const Object& o = (*oi).second;
    os << std::endl << "  " << o;
    os << " - " << TermTable::type(o);
  }
  return os;
}
