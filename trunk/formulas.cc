/*
 * $Id: formulas.cc,v 1.7 2001-08-18 21:15:07 lorens Exp $
 */
#include <typeinfo>
#include "formulas.h"
#include "domains.h"
#include "problems.h"
#include "bindings.h"


/*
 * A substitutes binary predicate.
 */
struct Substitutes
  : public binary_function<const Substitution*, const Variable*, bool> {
  /* Checks if the given substitution involves the given variable. */
  bool operator()(const Substitution* s, const Variable* v) const {
    return s->var == *v;
  }
};


/*
 * Instantiated variable.
 */
struct StepVar : public Variable {
protected:
  /* Prints this instantiated variable on the given stream. */
  virtual void print(ostream& os) const;

  /* Checks if this term equals the given term. */
  virtual bool equals(const Term& t) const;

private:
  /* The id of the step that this variable belongs to. */
  size_t id;

  /* Constructs an instantiated variable. */
  StepVar(const Variable& var, size_t id)
    : Variable(var), id(id) {
  }

  friend const Variable& Variable::instantiation(size_t id) const;
};


/*
 * Class representing the true formula.
 */
struct TrueFormula : public Formula {
  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(size_t id) const {
    return *this;
  }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const {
    return *this;
  }

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const {
    return *this;
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 0;
  }

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const {
    os << "TRUE";
  }

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const {
    return this == &f;
  }

  /* Returns a negation of this formula. */
  virtual const Formula& negation() const {
    return FALSE;
  }

private:
  TrueFormula() {
  }

  friend struct Formula;
};


/*
 * Class representing the false formula.
 */
struct FalseFormula : public Formula {
  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(size_t id) const {
    return *this;
  }

  /* Returns an instantiation of this formula. */
  virtual const Formula& instantiation(const SubstitutionList& subst,
				       const Problem& problem) const {
    return *this;
  }

  /* Returns this formula subject to the given substitutions. */
  virtual const Formula& substitution(const SubstitutionList& subst) const {
    return *this;
  }

  /* Roughly corresponds to the number of open conditions this formula
     will give rise to. */
  virtual size_t cost() const {
    return 0;
  }

protected:
  /* Prints this formula on the given stream. */
  virtual void print(ostream& os) const {
    os << "FALSE";
  }

  /* Checks if this formula equals the given formula. */
  virtual bool equals(const Formula& f) const {
    return this == &f;
  }

  /* Returns a negation of this formula. */
  virtual const Formula& negation() const {
    return TRUE;
  }

private:
  FalseFormula() {
  }

  friend struct Formula;
};


/* Prints this substitution on the given stream. */
void Substitution::print(ostream& os) const {
  os << '[' << var << '/' << term << ']';
}


/* Prints this term on the given stream. */
void Term::print(ostream& os) const {
  os << name;
}


/* Returns the hash value of this term. */
size_t Term::hash_value() const {
  return hash<string>()(name);
}


/* Returns an instantiation of this term. */
const Name& Name::instantiation(size_t id) const {
  return *this;
}


/* Returns this term subject to the given substitutions. */
const Name& Name::substitution(const SubstitutionList& subst) const {
  return *this;
}


/* Checks if this term is equivalent to the given term; two terms are
   equivalent if they both are the same name, or if they are variables
   (variable names only matter for equality and not for
   equivalence). */
bool Name::equivalent(const Term& t) const {
  return *this == t;
}


/* Checks if this term equals the given term. */
bool Name::equals(const Term& t) const {
  const Name* nt = dynamic_cast<const Name*>(&t);
  return nt != NULL && name == nt->name;
}


/* Returns an instantiation of this term. */
const Variable& Variable::instantiation(size_t id) const {
  return *(new StepVar(*this, id));
}


/* Returns this term subject to the given substitutions. */
const Term& Variable::substitution(const SubstitutionList& subst) const {
  SubstitutionList::const_iterator si =
    find_if(subst.begin(), subst.end(), bind2nd(Substitutes(), this));
  return (si != subst.end()) ? (*si)->term : *this;
}


/* Checks if this term is equivalent to the given term. */
bool Variable::equivalent(const Term& t) const {
  return dynamic_cast<const Variable*>(&t) != NULL;
}


/* Checks if this term equals the given term. */
bool Variable::equals(const Term& t) const {
  const Variable* vt = dynamic_cast<const Variable*>(&t);
  return vt != NULL && name == vt->name && typeid(t) == typeid(Variable);
}


/* Prints this term on the given stream. */
void StepVar::print(ostream& os) const {
  os << name << '(' << id << ')';
}


/* Checks if this term equals the given term. */
bool StepVar::equals(const Term& t) const {
  const StepVar* vt = dynamic_cast<const StepVar*>(&t);
  return vt != NULL && name == vt->name && id == vt->id;
}


/* Returns an instantiation of this term list. */
const TermList& TermList::instantiation(size_t id) const {
  TermList& terms = *(new TermList());
  for (const_iterator ti = begin(); ti != end(); ti++) {
    terms.push_back(&(*ti)->instantiation(id));
  }
  return terms;
}


/* Returns this term list subject to the given substitutions. */
const TermList& TermList::substitution(const SubstitutionList& subst) const {
  TermList& terms = *(new TermList());
  for (const_iterator ti = begin(); ti != end(); ti++) {
    terms.push_back(&(*ti)->substitution(subst));
  }
  return terms;
}


/* Checks if this term list is equivalent to the given term list. */
bool TermList::equivalent(const TermList& terms) const {
  if (size() != terms.size()) {
    return false;
  } else {
    for (const_iterator ti = begin(), tj = terms.begin();
	 ti != end(); ti++, tj++) {
      if (!(*ti)->equivalent(**tj)) {
	return false;
      }
    }
    return true;
  }
}


/* Equality operator for term lists. */
bool TermList::operator==(const TermList& terms) const {
  if (size() != terms.size()) {
    return false;
  } else {
    for (const_iterator ti = begin(), tj = terms.begin();
	 ti != end(); ti++, tj++) {
      if (**ti != **tj) {
	return false;
      }
    }
    return true;
  }
}


/* Inequality operator for term lists. */
bool TermList::operator!=(const TermList& terms) const {
  return !(*this == terms);
}


/* An empty variable list. */
const VariableList& VariableList::EMPTY = *(new VariableList());


/* Checks if this variable list contains the given variable. */
bool VariableList::contains(const Variable& v) const {
  for (const_iterator vi = begin(); vi != end(); vi++) {
    if (**vi == v) {
      return true;
    }
  }
  return false;
}


/* Returns an instantiation of this variable list. */
const VariableList& VariableList::instantiation(size_t id) const {
  VariableList& variables = *(new VariableList());
  for (const_iterator vi = begin(); vi != end(); vi++) {
    variables.push_back(&(*vi)->instantiation(id));
  }
  return variables;
}


/* Equality operator for variable lists. */
bool VariableList::operator==(const VariableList& variables) const {
  if (size() != variables.size()) {
    return false;
  } else {
    for (const_iterator vi = begin(), vj = variables.begin();
	 vi != end(); vi++, vj++) {
      if (**vi != **vj) {
	return false;
      }
    }
    return true;
  }
}


/* Inequality operator for variable lists. */
bool VariableList::operator!=(const VariableList& variables) const {
  return !(*this == variables);
}


/* The true formula. */
const Formula& Formula::TRUE = *(new TrueFormula());
/* The false formula. */
const Formula& Formula::FALSE = *(new FalseFormula());


/* Conjunction operator for formulas. */
const Formula& operator&&(const Formula& f1, const Formula& f2) {
  if (f1 == Formula::FALSE || f2 == Formula::FALSE) {
    return Formula::FALSE;
  } else if (f1 == Formula::TRUE) {
    return f2;
  } else if (f2 == Formula::TRUE) {
    return f1;
  } else {
    FormulaList& conjuncts = *(new FormulaList());
    const Conjunction* c1 = dynamic_cast<const Conjunction*>(&f1);
    if (c1 != NULL) {
      copy(c1->conjuncts.begin(), c1->conjuncts.end(),
	   back_inserter(conjuncts));
    } else {
      conjuncts.push_back(&f1);
    }
    const Conjunction* c2 = dynamic_cast<const Conjunction*>(&f2);
    if (c2 != NULL) {
      copy(c2->conjuncts.begin(), c2->conjuncts.end(),
	   back_inserter(conjuncts));
    } else {
      conjuncts.push_back(&f2);
    }
    return *(new Conjunction(conjuncts));
  }
}


/* Disjunction operator for formulas. */
const Formula& operator||(const Formula& f1, const Formula& f2) {
  if (f1 == Formula::TRUE || f2 == Formula::TRUE) {
    return Formula::TRUE;
  } else if (f1 == Formula::FALSE) {
    return f2;
  } else if (f2 == Formula::FALSE) {
    return f1;
  } else {
    FormulaList& disjuncts = *(new FormulaList());
    const Disjunction* d1 = dynamic_cast<const Disjunction*>(&f1);
    if (d1 != NULL) {
      copy(d1->disjuncts.begin(), d1->disjuncts.end(),
	   back_inserter(disjuncts));
    } else {
      disjuncts.push_back(&f1);
    }
    const Disjunction* d2 = dynamic_cast<const Disjunction*>(&f2);
    if (d2 != NULL) {
      copy(d2->disjuncts.begin(), d2->disjuncts.end(),
	   back_inserter(disjuncts));
    } else {
      disjuncts.push_back(&f2);
    }
    return *(new Disjunction(disjuncts));
  }
}


/* Returns an instantiation of this formula list. */
const FormulaList& FormulaList::instantiation(size_t id) const {
  FormulaList& formulas = *(new FormulaList());
  for (const_iterator i = begin(); i != end(); i++) {
    formulas.push_back(&(*i)->instantiation(id));
  }
  return formulas;
}


/* Returns an instantiation of this formula list. */
const FormulaList& FormulaList::instantiation(const SubstitutionList& subst,
					      const Problem& problem) const {
  FormulaList& formulas = *(new FormulaList());
  for (const_iterator i = begin(); i != end(); i++) {
    const Formula& f = (*i)->instantiation(subst, problem);
    if (f == Formula::FALSE) {
      formulas.clear();
      break;
    } else if (f != Formula::TRUE) {
      formulas.push_back(&f);
    }
  }
  return formulas;
}


/* Returns this formula list subject to the given substitutions. */
const FormulaList&
FormulaList::substitution(const SubstitutionList& subst) const {
  FormulaList& formulas = *(new FormulaList());
  for (const_iterator i = begin(); i != end(); i++) {
    formulas.push_back(&(*i)->substitution(subst));
  }
  return formulas;
}


/* Fills the provided list with goals achievable by the formulas in
   this list. */
void FormulaList::achievable_goals(FormulaList& goals) const {
  for (const_iterator i = begin(); i != end(); i++) {
    (*i)->achievable_goals(goals);
  }
}


/* Fills the provided sets with predicates achievable by the formulas
   in this list. */
void FormulaList::achievable_predicates(hash_set<string>& preds,
					hash_set<string>& neg_preds) const {
  for (const_iterator i = begin(); i != end(); i++) {
    (*i)->achievable_predicates(preds, neg_preds);
  }
}


/* Equality operator for formula lists. */
bool FormulaList::operator==(const FormulaList& formulas) const {
  if (size() != formulas.size()) {
    return false;
  } else {
    for (const_iterator i = begin(), j = formulas.begin();
	 i != end(); i++, j++) {
      if (**i != **j) {
	return false;
      }
    }
    return true;
  }
}


/* Inequality operator for formula lists. */
bool FormulaList::operator!=(const FormulaList& formulas) const {
  return !(*this == formulas);
}


/* Returns the negation of this formula list. */
const FormulaList& FormulaList::operator!() const {
  FormulaList& formulas = *(new FormulaList());
  for (const_iterator i = begin(); i != end(); i++) {
    formulas.push_back(&!**i);
  }
  return formulas;
}


/* Returns an instantiation of this formula. */
const AtomicFormula& AtomicFormula::instantiation(size_t id) const {
  return *(new AtomicFormula(predicate, terms.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& AtomicFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  const Formula& f = substitution(subst);
  if (problem.domain.static_predicate(predicate)) {
    if (problem.init != NULL) {
      const FormulaList& adds = problem.init->add_list;
      for (FLCI fi = adds.begin(); fi != adds.end(); fi++) {
	if (Bindings().unify(f, **fi)) {
	  return TRUE;
	}
      }
    }
    return FALSE;
  } else {
    return f;
  }
}


/* Returns this formula subject to the given substitutions. */
const Formula&
AtomicFormula::substitution(const SubstitutionList& subst) const {
  return *(new AtomicFormula(predicate, terms.substitution(subst)));
}


/* Checks if this formula negates the given formula. */
bool AtomicFormula::negates(const Formula& f) const {
  const Negation* negation = dynamic_cast<const Negation*>(&f);
  return negation != NULL && negation->negates(*this);
}


/* Fills the provided list with goals achievable by this formula. */
void AtomicFormula::achievable_goals(FormulaList& goals) const {
  goals.push_back(this);
}


/* Fills the provided sets with predicates achievable by this
   formula. */
void AtomicFormula::achievable_predicates(hash_set<string>& preds,
					  hash_set<string>& neg_preds) const {
  for (TermList::const_iterator ti = terms.begin(); ti != terms.end(); ti++) {
    if (typeid(**ti) != typeid(Name)) {
      preds.insert(predicate);
      return;
    }
  }
}


/* Prints this formula on the given stream. */
void AtomicFormula::print(ostream& os) const {
  os << '(' << predicate;
  for (TermList::const_iterator i = terms.begin(); i != terms.end(); i++) {
    os << ' ' << **i;
  }
  os << ')';
}


/* Checks if this formula equals the given formula. */
bool AtomicFormula::equals(const Formula& f) const {
  const AtomicFormula* atom = dynamic_cast<const AtomicFormula*>(&f);
  return atom != NULL && predicate == atom->predicate && terms == atom->terms;
}


/* Returns the negation of this formula. */
const Formula& AtomicFormula::negation() const {
  return *(new Negation(*this));
}


/* Returns the hash value of this formula. */
size_t AtomicFormula::hash_value() const {
  return hash<string>()(predicate) + hash<TermList>()(terms);
}


/* Returns an instantiation of this formula. */
const Negation& Negation::instantiation(size_t id) const {
  return *(new Negation(atom.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& Negation::instantiation(const SubstitutionList& subst,
				       const Problem& problem) const {
  return !atom.instantiation(subst, problem);
}


/* Returns this formula subject to the given substitutions. */
const Formula& Negation::substitution(const SubstitutionList& subst) const {
  return !atom.substitution(subst);
}


/* Checks if this formula negates the given formula. */
bool Negation::negates(const Formula& f) const {
  return atom == f;
}


/* Fills the provided list with goals achievable by this formula. */
void Negation::achievable_goals(FormulaList& goals) const {
  goals.push_back(this);
}


/* Fills the provided sets with predicates achievable by this
   formula. */
void Negation::achievable_predicates(hash_set<string>& preds,
				     hash_set<string>& neg_preds) const {
  atom.achievable_predicates(neg_preds, preds);
}


/* Prints this formula on the given stream. */
void Negation::print(ostream& os) const {
  os << "(not " << atom << ")";
}


/* Checks if this formula equals the given formula. */
bool Negation::equals(const Formula& f) const {
  const Negation* negation = dynamic_cast<const Negation*>(&f);
  return negation != NULL && atom == negation->atom;
}


/* Returns the negation of this formula. */
const Formula& Negation::negation() const {
  return atom;
}


/* Returns the hash value of this formula. */
size_t Negation::hash_value() const {
  return 2*hash<Formula>()(atom);
}


/* Returns the instantiation of this formula. */
const Equality& Equality::instantiation(size_t id) const {
  const Term& t1 = term1.instantiation(id);
  const Term& t2 = term2.instantiation(id);
  return (&t1 == &term1 && &t2 == &term2) ? *this : *(new Equality(t1, t2));
}


/* Returns an instantiation of this formula. */
const Formula& Equality::instantiation(const SubstitutionList& subst,
				       const Problem& problem) const {
  return substitution(subst);
}


/* Returns this formula subject to the given substitutions. */
const Formula& Equality::substitution(const SubstitutionList& subst) const {
  const Term& t1 = term1.substitution(subst);
  const Term& t2 = term2.substitution(subst);
  if (typeid(t1) == typeid(Name) && typeid(t2) == typeid(Name)) {
    return (t1 == t2) ? TRUE : FALSE;
  } else {
    return (&t1 == &term1 && &t2 == &term2) ? *this : *(new Equality(t1, t2));
  }
}


/* Prints this formula on the given stream. */
void Equality::print(ostream& os) const {
  os << "(= " << term1 << ' ' << term2 << ")";
}


/* Checks if this formula equals the given formula. */
bool Equality::equals(const Formula& f) const {
  const Equality* eq = dynamic_cast<const Equality*>(&f);
  return eq != NULL && term1 == eq->term1 && term2 == eq->term2;
}


/* Returns the negation of this formula. */
const Formula& Equality::negation() const {
  return *(new Inequality(term1, term2));
}


/* Returns an instantiation of this formula. */
const Inequality& Inequality::instantiation(size_t id) const {
  const Term& t1 = term1.instantiation(id);
  const Term& t2 = term2.instantiation(id);
  return (&t1 == &term1 && &t2 == &term2) ? *this : *(new Inequality(t1, t2));
}


/* Returns an instantiation of this formula. */
const Formula& Inequality::instantiation(const SubstitutionList& subst,
					 const Problem& problem) const {
  return substitution(subst);
}


/* Returns this formula subject to the given substitutions. */
const Formula& Inequality::substitution(const SubstitutionList& subst) const {
  const Term& t1 = term1.substitution(subst);
  const Term& t2 = term2.substitution(subst);
  if (typeid(t1) == typeid(Name) && typeid(t2) == typeid(Name)) {
    return (t1 != t2) ? TRUE : FALSE;
  } else {
    return ((&t1 == &term1 && &t2 == &term2)
	    ? *this : *(new Inequality(t1, t2)));
  }
}


/* Prints this formula on the given stream. */
void Inequality::print(ostream& os) const {
  os << "(not (= " << term1 << ' ' << term2 << "))";
}


/* Checks if this formula equals the given formula. */
bool Inequality::equals(const Formula& f) const {
  const Inequality* neq = dynamic_cast<const Inequality*>(&f);
  return neq != NULL && term1 == neq->term1 && term2 == neq->term2;
}


/* Returns the negation of this formula. */
const Formula& Inequality::negation() const {
  return *(new Equality(term1, term2));
}


/* Returns an instantiation of this formula. */
const Conjunction& Conjunction::instantiation(size_t id) const {
  return *(new Conjunction(conjuncts.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& Conjunction::instantiation(const SubstitutionList& subst,
					  const Problem& problem) const {
  const Formula* c = &TRUE;
  for (FLCI fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    c = &(*c && (*fi)->instantiation(subst, problem));
  }
  return *c;
}


/* Returns this formula subject to the given substitutions. */
const Formula& Conjunction::substitution(const SubstitutionList& subst) const {
  const Formula* c = &TRUE;
  for (FLCI fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    c = &(*c && (*fi)->substitution(subst));
  }
  return *c;
}


/* Fills the provided list with goals achievable by this formula. */
void Conjunction::achievable_goals(FormulaList& goals) const {
  conjuncts.achievable_goals(goals);
}


/* Fills the provided sets with predicates achievable by this
   formula. */
void Conjunction::achievable_predicates(hash_set<string>& preds,
					hash_set<string>& neg_preds) const {
  conjuncts.achievable_predicates(preds, neg_preds);
}


/* Prints this formula on the given stream. */
void Conjunction::print(ostream& os) const {
  os << "(and";
  for (FLCI i = conjuncts.begin(); i != conjuncts.end(); i++) {
    os << ' ' << **i;
  }
  os << ")";
}


/* Checks if this formula equals the given formula. */
bool Conjunction::equals(const Formula& f) const {
  const Conjunction* conjunction = dynamic_cast<const Conjunction*>(&f);
  return conjunction != NULL && conjuncts == conjunction->conjuncts;
}


/* Returns the negation of this formula. */
const Formula& Conjunction::negation() const {
  const Formula* d = &FALSE;
  for (FLCI fi = conjuncts.begin(); fi != conjuncts.end(); fi++) {
    d = &(*d || !**fi);
  }
  return *d;
}


/* Returns an instantiation of this formula. */
const Disjunction& Disjunction::instantiation(size_t id) const {
  return *(new Disjunction(disjuncts.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& Disjunction::instantiation(const SubstitutionList& subst,
					  const Problem& problem) const {
  const Formula* d = &TRUE;
  for (FLCI fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    d = &(*d || (*fi)->instantiation(subst, problem));
  }
  return *d;
}


/* Returns this formula subject to the given substitutions. */
const Formula& Disjunction::substitution(const SubstitutionList& subst) const {
  const Formula* d = &FALSE;
  for (FLCI fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    d = &(*d || (*fi)->substitution(subst));
  }
  return *d;
}


/* Fills the provided list with goals achievable by this formula. */
void Disjunction::achievable_goals(FormulaList& goals) const {
  disjuncts.achievable_goals(goals);
}


/* Fills the provided sets with predicates achievable by this
   formula. */
void Disjunction::achievable_predicates(hash_set<string>& preds,
					hash_set<string>& neg_preds) const {
  disjuncts.achievable_predicates(preds, neg_preds);
}


/* Prints this formula on the given stream. */
void Disjunction::print(ostream& os) const {
  os << "(or";
  for (FLCI i = disjuncts.begin(); i != disjuncts.end(); i++) {
    os << ' ' << **i;
  }
  os << ")";
}


/* Checks if this formula equals the given formula. */
bool Disjunction::equals(const Formula& f) const {
  const Disjunction* disjunction = dynamic_cast<const Disjunction*>(&f);
  return disjunction != NULL && disjuncts == disjunction->disjuncts;
}


/* Returns the negation of this formula. */
const Formula& Disjunction::negation() const {
  const Formula* c = &TRUE;
  for (FLCI fi = disjuncts.begin(); fi != disjuncts.end(); fi++) {
    c = &(*c && !**fi);
  }
  return *c;
}


/* Fills the provided sets with predicates achievable by this
   formula. */
void
QuantifiedFormula::achievable_predicates(hash_set<string>& preds,
					 hash_set<string>& neg_preds) const {
  body.achievable_predicates(preds, neg_preds);
}


/* Returns an instantiation of this formula. */
const ExistsFormula& ExistsFormula::instantiation(size_t id) const {
  return *(new ExistsFormula(parameters.instantiation(id),
			     body.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& ExistsFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  const Formula& b = body.instantiation(subst, problem);
  return (b == FALSE || b == TRUE) ? b : *(new ExistsFormula(parameters, b));
}


/* Returns this formula subject to the given substitutions. */
const Formula&
ExistsFormula::substitution(const SubstitutionList& subst) const {
  const Formula& b = body.substitution(subst);
  return (b == FALSE || b == TRUE) ? b : *(new ExistsFormula(parameters, b));
}


/* Prints this formula on the given stream. */
void ExistsFormula::print(ostream& os) const {
  os << "(exists (";
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
  os << ") " << body << ")";
}


/* Checks if this formula equals the given formula. */
bool ExistsFormula::equals(const Formula& f) const {
  const ExistsFormula* exists = dynamic_cast<const ExistsFormula*>(&f);
  return (exists != NULL
	  && parameters == exists->parameters && body == exists->body);
}


/* Returns the negation of this formula. */
const Formula& ExistsFormula::negation() const {
  return *(new ForallFormula(parameters, !body));
}


/* Returns an instantiation of this formula. */
const ForallFormula& ForallFormula::instantiation(size_t id) const {
  return *(new ForallFormula(parameters.instantiation(id),
			     body.instantiation(id)));
}


/* Returns an instantiation of this formula. */
const Formula& ForallFormula::instantiation(const SubstitutionList& subst,
					    const Problem& problem) const {
  const Formula& b = body.instantiation(subst, problem);
  return (b == FALSE || b == TRUE) ? b : *(new ForallFormula(parameters, b));
}


/* Returns this formula subject to the given substitutions. */
const Formula&
ForallFormula::substitution(const SubstitutionList& subst) const {
  const Formula& b = body.substitution(subst);
  return (b == FALSE || b == TRUE) ? b : *(new ForallFormula(parameters, b));
}


/* Prints this formula on the given stream. */
void ForallFormula::print(ostream& os) const {
  os << "(forall (";
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
  os << ") " << body << ")";
}


/* Checks if this formula equals the given formula. */
bool ForallFormula::equals(const Formula& f) const {
  const ForallFormula* forall = dynamic_cast<const ForallFormula*>(&f);
  return (forall != NULL
	  && parameters == forall->parameters && body == forall->body);
}


/* Returns the negation of this formula. */
const Formula& ForallFormula::negation() const {
  return *(new ExistsFormula(parameters, !body));
}
