/*
 * $Id: bindings.cc,v 1.4 2001-10-06 00:34:53 lorens Exp $
 */
#include "bindings.h"


typedef Chain<const Variable*> VariableChain;


/*
 * Variable codesignation, and non-codesignation.
 */
struct Varset : public gc {
  /* The constant of this varset, or NULL. */
  const Name* const constant;

  /* Checks if this varset includes the given name. */
  bool includes(const Name& name) const {
    return constant != NULL && *constant == name;
  }

  /* Checks if this varset includes the given variable. */
  bool includes(const Variable& var) const {
    for (const VariableChain* vars = cd_set; vars != NULL; vars = vars->tail) {
      if (*vars->head == var) {
	return true;
      }
    }
    return false;
  }

  /* Checks if this varset excludes the given variable. */
  bool excludes(const Variable& var) const {
    for (const VariableChain* vars = ncd_set;
	 vars != NULL; vars = vars->tail) {
      if (*vars->head == var) {
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
    for (const VariableChain* cd = vs.cd_set; cd != NULL; cd = cd->tail) {
      if (excludes(*cd->head)) {
	return NULL;
      } else {
	comb_cd = new VariableChain(cd->head, comb_cd);
      }
    }
    const VariableChain* comb_ncd = ncd_set;
    for (const VariableChain* ncd = vs.ncd_set; ncd != NULL; ncd = ncd->tail) {
      if (includes(*ncd->head)) {
	return NULL;
      } else if (!excludes(*ncd->head)) {
	comb_ncd = new VariableChain(ncd->head, comb_ncd);
      }
    }
    return new Varset(comb_const, comb_cd, comb_ncd);
  }

  /* Returns the varset representing the given equality binding. */
  static const Varset* make_varset(const EqualityBinding& eq) {
    const VariableChain* cd_set = new VariableChain(&eq.variable, NULL);
    const Variable* v2 = dynamic_cast<const Variable*>(&eq.term);
    if (v2 != NULL) {
      cd_set = new VariableChain(v2, cd_set);
      return new Varset(NULL, cd_set, NULL);
    } else {
      const Name& n = dynamic_cast<const Name&>(eq.term);
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
      const Variable* v2 = dynamic_cast<const Variable*>(&neq.term);
      if (v2 != NULL) {
	constant = NULL;
	cd_set = new VariableChain(v2, NULL);
      } else {
	const Name& n = dynamic_cast<const Name&>(neq.term);
	constant = &n;
	cd_set = NULL;
      }
      ncd_set = new VariableChain(&neq.variable, NULL);
    } else {
      const Variable* v2 = dynamic_cast<const Variable*>(&neq.term);
      if (v2 != NULL) {
	constant = NULL;
	cd_set = new VariableChain(&neq.variable, NULL);
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

private:
  /* The codesignation list. */
  const VariableChain* const cd_set;
  /* The non-codesignation list. */
  const VariableChain* const ncd_set;
};


/* Prints this equality binding on the given stream. */
void EqualityBinding::print(ostream& os) const {
  os << variable << '=' << term;
}


/* Prints this inequality binding on the given stream. */
void InequalityBinding::print(ostream& os) const {
  os << variable << "!=" << term;
}


/* Returns the varset containing the given constant, or NULL if none do. */
static const Varset* find_varset(const VarsetChain* varsets,
				 const Name& constant) {
  for (const VarsetChain* vs = varsets; vs != NULL; vs = vs->tail) {
    if (vs->head->includes(constant)) {
      return vs->head;
    }
  }
  return NULL;
}


/* Returns the varset containing the given variable, or NULL if none do. */
static const Varset* find_varset(const VarsetChain* varsets,
				 const Variable& var) {
  for (const VarsetChain* vs = varsets; vs != NULL; vs = vs->tail) {
    if (vs->head->includes(var)) {
      return vs->head;
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


/* Creates a collection of variable bindings with the given equality
   and inequality bindings. */
const Bindings* Bindings::make_bindings(const BindingChain* equalities,
					const BindingChain* inequalities) {
  BindingList bindings;
  for (const BindingChain* ec = equalities; ec != NULL; ec = ec->tail) {
    bindings.push_back(ec->head);
  }
  for (const BindingChain* ic = inequalities; ic != NULL; ic = ic->tail) {
    bindings.push_back(ic->head);
  }
  return Bindings().add(bindings);
}


/* Returns an instantiation of the given formula, where bound
   variables have been substituted for the value they are bound
   to. */
const Formula& Bindings::instantiation(const Formula& f) const {
  const Atom* atom = dynamic_cast<const Atom*>(&f);
  if (atom != NULL) {
    TermList& inst_terms = *(new TermList());
    const TermList& terms = atom->terms;
    for (TermList::const_iterator i = terms.begin(); i != terms.end(); i++) {
      inst_terms.push_back(&binding(**i));
    }
    return *(new Atom(atom->predicate, inst_terms));
  }
  const Negation* negation = dynamic_cast<const Negation*>(&f);
  if (negation != NULL) {
    return !instantiation(negation->atom);
  }
  const Conjunction* conjunction = dynamic_cast<const Conjunction*>(&f);
  if (conjunction != NULL) {
    const Formula* inst_conjunction = &Formula::TRUE;
    const FormulaList& conjuncts = conjunction->conjuncts;
    for (FormulaList::const_iterator i = conjuncts.begin();
	 i != conjuncts.end(); i++) {
      inst_conjunction = &(*inst_conjunction && instantiation(**i));
    }
    return *inst_conjunction;
  }
  const Disjunction* disjunction = dynamic_cast<const Disjunction*>(&f);
  if (disjunction != NULL) {
    const Formula* inst_disjunction = &Formula::FALSE;
    const FormulaList& disjuncts = disjunction->disjuncts;
    for (FormulaList::const_iterator i = disjuncts.begin();
	 i != disjuncts.end(); i++) {
      inst_disjunction = &(*inst_disjunction || instantiation(**i));
    }
    return *inst_disjunction;
  }
  const Equality* eq = dynamic_cast<const Equality*>(&f);
  if (eq != NULL) {
    return *(new Equality(binding(eq->term1), binding(eq->term2)));
  }
  const Inequality* neq = dynamic_cast<const Inequality*>(&f);
  if (neq != NULL) {
    return *(new Inequality(binding(neq->term1), binding(neq->term2)));
  }
  throw Unimplemented("Bindings::instantiation");
}


/* Returns the binding for the given term, or the term itself if it
   is unbound. */
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


/* Checks if one of the given formulas is the negation of the other,
   and the atomic formulas can be unified. */
bool Bindings::affects(const Formula& f1, const Formula& f2) const {
  SubstitutionList dummy;
  return affects(dummy, f1, f2);
}


/* Checks if one of the given formulas is the negation of the other,
   and the atomic formulas can be unified; the most general unifier
   is added to the provided substitution list. */
bool Bindings::affects(SubstitutionList& mgu,
		       const Formula& f1, const Formula& f2) const {
  const Negation* negation = dynamic_cast<const Negation*>(&f1);
  if (negation != NULL) {
    return unify(mgu, negation->atom, f2);
  } else {
    negation = dynamic_cast<const Negation*>(&f2);
    if (negation != NULL) {
      return unify(mgu, f1, negation->atom);
    } else {
      return false;
    }
  }
}


/* Checks if the given formulas can be unified. */
bool Bindings::unify(const Formula& f1, const Formula& f2) const {
  SubstitutionList dummy;
  return unify(dummy, f1, f2);
}

/* Checks if the given formulas can be unified; the most general
   unifier is added to the provided substitution list. */
bool Bindings::unify(SubstitutionList& mgu,
		     const Formula& f1, const Formula& f2) const {
  /*
   * Extract the atomic formulas if the given formulas are negations.
   */
  const Formula* fp1 = &f1;
  const Formula* fp2 = &f2;
  const Negation* negation1 = dynamic_cast<const Negation*>(fp1);
  if (negation1 != NULL) {
    const Negation* negation2 = dynamic_cast<const Negation*>(fp2);
    if (negation2 == NULL) {
      /* One is negation, but not the other. */
      return false;
    } else {
      /* Both are negations. */
      fp1 = negation1;
      fp2 = negation2;
    }
  }
  const Atom& atom1 = dynamic_cast<const Atom&>(*fp1);
  const Atom& atom2 = dynamic_cast<const Atom&>(*fp2);
  if (atom1.predicate != atom2.predicate) {
    /* The predicates do not match. */
    return false;
  }

  /*
   * Try to unify the terms of the atomic formulas.
   */
  /* Varsets resulting from unification. */
  const VarsetChain* mgu_varsets = varsets_;
  /* Terms of the first atom. */
  const TermList& terms1 = atom1.terms;
  /* Terms of the second atom. */
  const TermList& terms2 = atom2.terms;

  /*
   * Unify one pair of terms at a time.
   */
  for (TermList::const_iterator i = terms1.begin(), j = terms2.begin();
       i != terms1.end() || j != terms2.end(); i++, j++) {
    if (i == terms1.end() || j == terms2.end()) {
      /* The term lists are of different length. */
      return false;
    } else {
      /*
       * Try to unify a pair of terms.
       */
      const Name* name1 = dynamic_cast<const Name*>(*i);
      if (name1 != NULL) {
	/* The first term is a name. */
	const Name* name2 = dynamic_cast<const Name*>(*j);
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
	  const Variable& var2 = dynamic_cast<const Variable&>(**j);
	  /* Varset for first term. */
	  const Varset* vs1 = find_varset(mgu_varsets, *name1);
	  /* Varset for second term. */
	  const Varset* vs2 = find_varset(mgu_varsets, var2);
	  /* Combined varset, or NULL if terms cannot be unified. */
	  const Varset* comb;
	  if (vs1 != NULL || vs2 != NULL) {
	    /* At least one of the terms is already bound. */
	    if (vs1 != vs2) {
	      /* The terms are not yet bound to eachother. */
	      if (vs1 == NULL) {
		/* The first term is unbound, so add it to the varset
                   of the second. */
		comb = vs2->add(*name1);
	      } else if (vs2 == NULL) {
		/* The second term is unbound, so add it to the varset
                   of the first. */
		comb = vs1->add(var2);
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
	    comb = new Varset(name1, new VariableChain(&var2, NULL), NULL);
	  }
	  if (comb == NULL) {
	    /* Unification is inconsistent with current bindings. */
	    return false;
	  } else {
	    /* Unification is consistent. */
	    if (comb != vs1) {
	      /* Combined varset is new, so add it to the chain of varsets. */
	      mgu_varsets = new VarsetChain(comb, mgu_varsets);
	    }
	    /* Add unification to most general unifier. */
	    mgu.push_back(new Substitution(var2, *name1));
	  }
	}
      } else {
	/* The first term is a variable. */
	const Variable& var1 = dynamic_cast<const Variable&>(**i);
	const Name* name2 = dynamic_cast<const Name*>(*j);
	if (name2 != NULL) {
	  /*
	   * The first term is a variable and the second is a name.
	   */
	  /* Varset for first term. */
	  const Varset* vs1 = find_varset(mgu_varsets, var1);
	  /* Varset for second term. */
	  const Varset* vs2 = find_varset(mgu_varsets, *name2);
	  /* Combined varset, or NULL if terms cannot be unified. */
	  const Varset* comb;
	  if (vs1 != NULL || vs2 != NULL) {
	    /* At least one of the terms is already bound. */
	    if (vs1 != vs2) {
	      /* The terms are not yet bound to eachother. */
	      if (vs1 == NULL) {
		/* The first term is unbound, so add it to the varset
                   of the second. */
		comb = vs2->add(var1);
	      } else if (vs2 == NULL) {
		/* The second term is unbound, so add it to the varset
                   of the first. */
		comb = vs1->add(*name2);
	      } else {
		/* Both terms are bound, so combine their varsets. */
		comb = vs1->combine(*vs2);
	      }
	    } else {
	      /* The terms are already bound to each other. */
	      comb = vs1;
	    }
	  } else {
	    /* None of the terms are already bound. */
	    comb = new Varset(name2, new VariableChain(&var1, NULL), NULL);
	  }
	  if (comb == NULL) {
	    /* Unification is inconsistent with current bindings. */
	    return false;
	  } else {
	    /* Unification is consistent. */
	    if (comb != vs1) {
	      /* Combined varset is new, so add it to the chain of varsets. */
	      mgu_varsets = new VarsetChain(comb, mgu_varsets);
	    }
	    /* Add unification to most general unifier. */
	    mgu.push_back(new Substitution(var1, *name2));
	  }
	} else {
	  /*
	   * Both terms are variables.
	   */
	  const Variable& var2 = dynamic_cast<const Variable&>(**j);
	  /* Varset for first term. */
	  const Varset* vs1 = find_varset(mgu_varsets, var1);
	  /* Varset for second term. */
	  const Varset* vs2 = find_varset(mgu_varsets, var2);
	  /* Combined varset, or NULL if terms cannot be unified. */
	  const Varset* comb;
	  if (vs1 != NULL || vs2 != NULL) {
	    /* At least one of the terms is already bound. */
	    if (vs1 != vs2) {
	      /* The terms are not yet bound to eachother. */
	      if (vs1 == NULL) {
		/* The first term is unbound, so add it to the varset
                   of the second. */
		comb = vs2->add(var1);
	      } else if (vs2 == NULL) {
		/* The second term is unbound, so add it to the varset
                   of the first. */
		comb = vs1->add(var2);
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
	    const VariableChain* vars = new VariableChain(&var1, NULL);
	    if (var1 != var2) {
	      vars = new VariableChain(&var2, vars);
	    }
	    comb = new Varset(NULL, vars, NULL);
	  }
	  if (comb == NULL) {
	    /* Unification is inconsistent with current bindings. */
	    return false;
	  } else {
	    /* Unification is consistent. */
	    if (comb != vs1) {
	      /* Combined varset is new, so add it to the chain of varsets. */
	      mgu_varsets = new VarsetChain(comb, mgu_varsets);
	    }
	    if (var1 != var2) {
	      /* Add unification to most general unifier. */
	      mgu.push_back(new Substitution(var1, var2));
	    }
	  }
	}
      }
    }
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
  /* Varset for term. */
  const Varset* vs2 = find_varset(varsets_, *term);
  return vs2 == NULL || vs2->includes(*var) || !vs2->excludes(*var);
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
  /* Varset for term. */
  const Varset* vs2 = find_varset(varsets_, *term);
  return vs2 == NULL || !vs2->includes(*var) || vs2->excludes(*var);
}


/* Returns the binding collection obtained by adding the given
   bindings to this binding collection, or NULL if the new bindings
   are inconsistent with the current. */
const Bindings* Bindings::add(const BindingList& new_bindings) const {
  /* Equality bindings for new binding collection. */
  const BindingChain* equalities = this->equalities;
  /* Inequality bindings for new binding collection. */
  const BindingChain* inequalities = this->inequalities;
  /* Varsets for new binding collection */
  const VarsetChain* varsets = varsets_;

  /*
   * Add new bindings one at a time.
   */
  for (BindingList::const_iterator i = new_bindings.begin();
       i != new_bindings.end(); i++) {
    const EqualityBinding* eq = dynamic_cast<const EqualityBinding*>(*i);
    if (eq != NULL) {
      /*
       * Adding equality binding.
       */
      /* Varset for variable. */
      const Varset* vs1 = find_varset(varsets, eq->variable);
      /* Varset for term. */
      const Varset* vs2 = find_varset(varsets, eq->term);
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
	    comb = vs2->add(eq->variable);
	  } else if (vs2 == NULL) {
	    /* The second term is unbound, so add it to the varset of
               the first. */
	    comb = vs1->add(eq->term);
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
	}
	/* Add binding to chain of equality bindings. */
	equalities = new BindingChain(eq, equalities);
      }
    } else {
      /*
       * Adding inequality binding.
       */
      const InequalityBinding& neq =
	dynamic_cast<const InequalityBinding&>(**i);
      /* Varset for variable. */
      const Varset* vs1 = find_varset(varsets, neq.variable);
      /* Varset for term. */
      const Varset* vs2 = find_varset(varsets, neq.term);
      if (vs1 == vs2) {
	/* The terms are already bound to eachother. */
	return NULL;
      } else {
	/* The terms are not bound to eachother. */
	if (vs1 == NULL) {
	  /* The first term is unbound, so create a new varset for it. */
	  vs1 = Varset::make_varset(neq);
	} else {
	  const Variable* v2 = dynamic_cast<const Variable*>(&neq.term);
	  if (v2 != NULL) {
	    /* The second term is a variable. */
	    if (vs1->excludes(*v2)) {
	      /* The second term is already separated from the first. */
	      vs1 = NULL;
	    } else {
	      /* Separate the second term from the first. */
	      vs1 = vs1->restrict(*v2);
	    }
	  } else {
	    /* The second term is a name, so the terms are separated
               in the varset for the second term. */
	    vs1 = NULL;
	  }
	}
	if (vs2 == NULL) {
	  /* The second term is unbound, so create a new varset for it. */
	  vs2 = Varset::make_varset(neq, true);
	} else if (vs2->excludes(neq.variable)) {
	  /* The first term is already separated from the second. */
	  vs2 = NULL;
	} else {
	  /* Separate the first term from the second. */
	  vs2 = vs2->restrict(neq.variable);
	}
	if (vs1 != NULL) {
	  /* The second term was not separated from the first already. */
	  varsets = new VarsetChain(vs1, varsets);
	}
	if (vs2 != NULL) {
	  /* The first term was not separated from the second already. */
	  varsets = new VarsetChain(vs2, varsets);
	}
      }
      /* Add binding to chain of inequality bindings. */
      inequalities = new BindingChain(&neq, inequalities);
    }
  }
  /* New bindings are consistent with the current bindings. */
  return new Bindings(equalities, inequalities, varsets);
}


/* Prints this binding collection on the given stream. */
void Bindings::print(ostream& os) const {
  os << "{";
  for (const BindingChain* b = equalities; b != NULL; b = b->tail) {
    os << ' ' << *b->head;
  }
  for (const BindingChain* b = inequalities; b != NULL; b = b->tail) {
    os << ' ' << *b->head;
  }
  os << " }";
}
