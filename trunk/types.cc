/*
 * $Id: types.cc,v 1.9 2001-12-25 19:23:02 lorens Exp $
 */
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


/* ====================================================================== */
/* Type */

/* Checks if this type is the object type. */
bool Type::object() const {
  return this == &SimpleType::OBJECT;
}


/* Type union. */
const Type& operator+(const Type& t1, const Type& t2) {
  return t1.add(t2);
}


/* Type subtraction. */
const Type& operator-(const Type& t1, const Type& t2) {
  return t1.subtract(t2);
}


/* ====================================================================== */
/* SimpleType */

/* The object type. */
const SimpleType& SimpleType::OBJECT = *(new SimpleType("object"));


/* Constructs a simple type with the given name. */
SimpleType::SimpleType(const string& name, const Type& supertype)
  : name(name), supertype(name == "object" ? *this : supertype) {}


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
      return member_if(ut.types.begin(), ut.types.end(),
		       bind1st(Subtype(), this));
    }
  }
}


/* Checks if this object is less than the given object. */
bool SimpleType::less(const LessThanComparable& o) const {
  const SimpleType& st = dynamic_cast<const SimpleType&>(o);
  return name < st.name;
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
	sort(new_types.begin(), new_types.end(),
	     std::less<const LessThanComparable*>());
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


/* ====================================================================== */
/* UnionType */

/* Constructs the type that is the union of the given types.
   N.B. Assumes type list is sorted. */
UnionType::UnionType(const TypeList& types)
  : types(types) {}


/* Checks if this type is a subtype of the given type. */
bool UnionType::subtype(const Type& t) const {
  return member_if(types.begin(), types.end(), bind2nd(Subtype(), &t));
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
	sort(new_types.begin(), new_types.end(),
	     less<const LessThanComparable*>());
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
    sort(new_types.begin(), new_types.end(),
	 less<const LessThanComparable*>());
    if (new_types.empty()) {
      return SimpleType::OBJECT;
    } else if (new_types.size() == 1) {
      return *new_types.back();
    } else {
      sort(new_types.begin(), new_types.end(),
	   less<const LessThanComparable*>());
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
      sort(new_types.begin(), new_types.end(),
	   less<const LessThanComparable*>());
      return *(new UnionType(new_types));
    }
  }
}


/* ====================================================================== */
/* TypeList */

/* Constructs an empty type list. */
TypeList::TypeList() {}


/* Constructs a type list with a single type. */
TypeList::TypeList(const SimpleType* type)
  : Vector<const SimpleType*>(1, type) {}
