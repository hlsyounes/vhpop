/*
 * $Id: types.cc,v 1.1 2001-07-29 18:12:15 lorens Exp $
 */
#include <algorithm>
#include "types.h"


/* The object type. */
const SimpleType& SimpleType::OBJECT_TYPE = *(new SimpleType("object"));


/* Checks if this type is object type. */
bool Type::object() const {
  return this == &SimpleType::OBJECT_TYPE;
}


/* Checks if this type is a subtype of the given type. */
bool SimpleType::subtype(const Type& t) const {
  if (t.object()) {
    return true;
  } else {
    const SimpleType* st = dynamic_cast<const SimpleType*>(&t);
    if (st != NULL) {
      return name == st->name || (!object() && supertype.subtype(t));
    } else {
      const UnionType& ut = dynamic_cast<const UnionType&>(t);
      for (TypeList::const_iterator i = ut.types.begin();
	   i != ut.types.end(); i++) {
	if (subtype(**i)) {
	  return true;
	}
      }
      return false;
    }
  }
}


/* Returns the union of this type and the given type. */
const Type& SimpleType::add(const Type& t) const {
  if (t.object()) {
    return *this;
  } else if (object()) {
    return t;
  } else {
    const SimpleType* st = dynamic_cast<const SimpleType*>(&t);
    if (st != NULL) {
      if (*this == t) {
	return *this;
      } else {
	TypeList& types = *(new TypeList(1, this));
	types.push_back(st);
	return *(new UnionType(types));
      }
    } else {
      return t + *this;
    }
  }
}


/* Removes the given type from this type. */
const Type& SimpleType::subtract(const Type& t) const {
  if (t.object()) {
    return *this;
  } else {
    const SimpleType* st = dynamic_cast<const SimpleType*>(&t);
    if (st != NULL) {
      return (*this != t) ? *this : OBJECT_TYPE;
    } else {
      const UnionType& ut = dynamic_cast<const UnionType&>(t);
      if (find(ut.types.begin(), ut.types.end(), *this) != ut.types.end()) {
	return OBJECT_TYPE;
      } else {
	return *this;
      }
    }
  }
}


/* Prints this type on the given stream. */
void SimpleType::print(ostream& os) const {
  os << name;
}


/* Checks if this type is a subtype of the given type. */
bool UnionType::subtype(const Type& t) const {
  for (TypeList::const_iterator i = types.begin(); i != types.end(); i++) {
    if ((*i)->subtype(t)) {
      return true;
    }
  }
  return false;
}


/* Returns the union of this type and the given type. */
const Type& UnionType::add(const Type& t) const {
  if (t.object()) {
    return *this;
  } else {
    const SimpleType* st = dynamic_cast<const SimpleType*>(&t);
    if (st != NULL) {
      if (find(types.begin(), types.end(), *st) != types.end()) {
	return *this;
      } else {
	TypeList& new_types = *(new TypeList(types));
	new_types.push_back(st);
	return *(new UnionType(new_types));
      }
    } else {
      const UnionType& ut = dynamic_cast<const UnionType&>(t);
      TypeList& new_types = *(new TypeList(types));
      for (TypeList::const_iterator i = ut.types.begin();
	   i != ut.types.end(); i++) {
	const SimpleType& st = **i;
	if (find(types.begin(), types.end(), st) == types.end()) {
	  new_types.push_back(&st);
	}
      }
      return *(new UnionType(new_types));
    }
  }
}


/* Removes the given type from this type. */
const Type& UnionType::subtract(const Type& t) const {
  if (t.object()) {
    return *this;
  } else {
    const SimpleType* st = dynamic_cast<const SimpleType*>(&t);
    if (st != NULL) {
      if (find(types.begin(), types.end(), *st) == types.end()) {
	return *this;
      } else {
	TypeList& new_types = *(new TypeList(types));
	TypeList::iterator ti = find(new_types.begin(), new_types.end(), *st);
	new_types.erase(ti);
	if (new_types.size() == 1) {
	  return *new_types.back();
	} else {
	  return *(new UnionType(new_types));
	}
      }
    } else {
      const UnionType& ut = dynamic_cast<const UnionType&>(t);
      TypeList& new_types = *(new TypeList(types));
      for (TypeList::const_iterator i = ut.types.begin();
	   i != ut.types.end(); i++) {
	TypeList::iterator ti = find(new_types.begin(), new_types.end(), **i);
	new_types.erase(ti);
      }
      if (new_types.empty()) {
	return SimpleType::OBJECT_TYPE;
      } else if (new_types.size() == 1) {
	return *new_types.back();
      } else {
	return *(new UnionType(new_types));
      }
    }
  }
}


/* Prints this type on the given stream. */
void UnionType::print(ostream& os) const {
  os << "(either";
  for (TypeList::const_iterator i = types.begin(); i != types.end(); i++) {
    os << ' ' << **i;
  }
  os << ")";
}
