/*
 * $Id: types.cc,v 1.5 2001-10-06 22:55:42 lorens Exp $
 */
#include <algorithm>
#include "types.h"


/*
 * A subtype binary predicate.
 */
struct Subtype : public binary_function<const Type*, const Type*, bool> {
  /* Checks if the first type is a subtype of the second type. */
  bool operator()(const Type* t1, const Type* t2) const {
    return t1->subtype(*t2);
  }
};


/*
 * Comparison predicate for simple types.
 */
struct less<const SimpleType*>
  : public binary_function<const SimpleType*, const SimpleType*, bool> {
  bool operator()(const SimpleType* t1, const SimpleType* t2) const {
    return t1->name < t2->name;
  }
};


/* Checks if this type is the object type. */
bool Type::object() const {
  return this == &SimpleType::OBJECT;
}


/* The object type. */
const SimpleType& SimpleType::OBJECT = *(new SimpleType("object"));


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
      return (find_if(ut.types.begin(), ut.types.end(),
		      bind1st(Subtype(), this))
	      != ut.types.end());
    }
  }
}


/* Checks if this object equals the given object. */
bool SimpleType::equals(const EqualityComparable& o) const {
  const SimpleType* st = dynamic_cast<const SimpleType*>(&o);
  return st != NULL && name == st->name;
}


/* Prints this object on the given stream. */
void SimpleType::print(ostream& os) const {
  os << name;
}


/* Returns the union of this type and the given type. */
const Type& SimpleType::add(const Type& t) const {
  const SimpleType* st = dynamic_cast<const SimpleType*>(&t);
  if (st != NULL) {
    if (subtype(t)) {
      return *this;
    } else if (t.subtype(*this)) {
      return t;
    } else {
	TypeList& new_types = *(new TypeList(this));
	new_types.push_back(st);
	sort(new_types.begin(), new_types.end(), less<const SimpleType*>());
	return *(new UnionType(new_types));
    }
  } else {
    const UnionType& ut = dynamic_cast<const UnionType&>(t);
    return ut.add(*this);
  }
}


/* Removes the given type from this type. */
const Type& SimpleType::subtract(const Type& t) const {
  return subtype(t) ? OBJECT : *this;
}


/* Checks if this type is a subtype of the given type. */
bool UnionType::subtype(const Type& t) const {
  return (find_if(types.begin(), types.end(), bind2nd(Subtype(), &t))
	  != types.end());
}


/* Checks if this object equals the given object. */
bool UnionType::equals(const EqualityComparable& o) const {
  const UnionType* ut = dynamic_cast<const UnionType*>(&o);
  return (ut != NULL && types.size() == ut->types.size()
	  && equal(types.begin(), types.end(), ut->types.begin(),
		   equal_to<const EqualityComparable*>()));
}


/* Prints this object on the given stream. */
void UnionType::print(ostream& os) const {
  os << "(either";
  for (TypeListIter ti = types.begin(); ti != types.end(); ti++) {
    os << ' ' << **ti;
  }
  os << ")";
}


/* Returns the union of this type and the given type. */
const Type& UnionType::add(const Type& t) const {
  cout << "UT::add " << *this << ' ' << t << endl;
  const SimpleType* st = dynamic_cast<const SimpleType*>(&t);
  if (st != NULL) {
    if (subtype(t)) {
      return *this;
    } else {
      TypeList& new_types = *(new TypeList());
      remove_copy_if(types.begin(), types.end(), back_inserter(new_types),
		     bind1st(Subtype(), &t));
      new_types.push_back(st);
      if (new_types.size() == 1) {
	return *new_types.back();
      } else {
	sort(new_types.begin(), new_types.end(), less<const SimpleType*>());
	return *(new UnionType(new_types));
      }
    }
  } else {
    const UnionType& ut = dynamic_cast<const UnionType&>(t);
    TypeList& new_types = *(new TypeList());
    remove_copy_if(types.begin(), types.end(), back_inserter(new_types),
		   bind1st(Subtype(), &t));
    remove_copy_if(ut.types.begin(), ut.types.end(), back_inserter(new_types),
		   bind1st(Subtype(), this));
    sort(new_types.begin(), new_types.end(), less<const SimpleType*>());
    if (new_types.empty()) {
      return SimpleType::OBJECT;
    } else if (new_types.size() == 1) {
      return *new_types.back();
    } else {
      sort(new_types.begin(), new_types.end(), less<const SimpleType*>());
      return *(new UnionType(new_types));
    }
  }
}


/* Removes the given type from this type. */
const Type& UnionType::subtract(const Type& t) const {
  if (!subtype(t)) {
    return *this;
  } else {
    TypeList& new_types = *(new TypeList());
    remove_copy_if(types.begin(), types.end(), back_inserter(new_types),
		   bind2nd(Subtype(), &t));
    if (new_types.empty()) {
      return SimpleType::OBJECT;
    } else if (new_types.size() == 1) {
      return *new_types.back();
    } else {
      sort(new_types.begin(), new_types.end(), less<const SimpleType*>());
      return *(new UnionType(new_types));
    }
  }
}
