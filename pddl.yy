/* -*-C++-*- */
/*
 * PDDL parser.
 *
 * $Id: pddl.yy,v 1.21 2001-12-25 20:11:00 lorens Exp $
 */
%{
#include <utility>
#include <cstdlib>
#include "requirements.h"
#include "domains.h"
#include "problems.h"
#include "formulas.h"


/* The lexer. */
extern int yylex();
/* Current line number. */
extern size_t line_number;
 

static string tostring(unsigned int n);
static void yyerror(const string& s); 
static void yywarning(const string& s);
static const Type* find_type(const string& name);
static const Name* find_constant(const string& name);
static const Predicate* find_predicate(const string& name);
static void add_names(const vector<string>& names,
		      const Type& type = SimpleType::OBJECT);
static void add_predicate(const string& name, const VariableList& parameters);
static void add_action(const ActionSchema& action);
static void add_variable(const Variable& var);
static const pair<AtomList*, NegationList*>& make_add_del(AtomList* adds,
							  NegationList* dels);
static const Atom& make_atom(const string& predicate, const TermList& terms);
static TermList& add_name(TermList& terms, const string& name);
static const Type& make_type(const string& name);


/*
 * Context of free variables.
 */
struct Context {
  void push_frame() {
    frames_.push_back(new VariableMap());
  }

  void pop_frame() {
    frames_.pop_back();
  }

  void insert(const Variable* v) {
    (*frames_.back())[v->name] = v;
  }

  const Variable* shallow_find(const string& name) const {
    VariableMap::const_iterator vi = frames_.back()->find(name);
    return (vi != frames_.back()->end()) ? (*vi).second : NULL;
  }

  const Variable* find(const string& name) const {
    for (Vector<VariableMap*>::const_reverse_iterator fi = frames_.rbegin();
	 fi != frames_.rend(); fi++) {
      VariableMap::const_iterator vi = (*fi)->find(name);
      if (vi != (*fi)->end()) {
	return (*vi).second;
      }
    }
    return NULL;
  }

private:
  struct VariableMap : public HashMap<string, const Variable*> {
  };

  Vector<VariableMap*> frames_;
};


/* Name of current file. */
string current_file; 
/* Level of warnings. */
int warning_level;

static bool success = true;
static const Domain* pdomain;
static string domain_name;
static Requirements* requirements;
static TypeMap* domain_types;
static NameMap* domain_constants;
static PredicateMap* domain_predicates;
static ActionSchemaMap* domain_actions;
static string problem_name;
static NameMap* problem_objects;
static string current_predicate;
static string context;
static string name_map_type;
static NameMap* name_map;
static const Formula* action_precond; 
static const EffectList* action_adds;
static VariableList* variables;
static Context free_variables;
static const VariableList* eff_forall;
static const Effect* problem_init;
static const Formula* problem_goal;
static bool unique_variables = true;
%}

%token DEFINE DOMAIN PROBLEM
%token REQUIREMENTS STRIPS TYPING DISJUNCTIVE_PRECONDITIONS EQUALITY
%token EXISTENTIAL_PRECONDITIONS UNIVERSAL_PRECONDITIONS
%token QUANTIFIED_PRECONDITIONS CONDITIONAL_EFFECTS ADL
%token TYPES CONSTANTS PREDICATES ACTION PARAMETERS PRECONDITION EFFECT
%token PDOMAIN OBJECTS INIT GOAL
%token WHEN NOT AND OR IMPLY EXISTS FORALL
%token EITHER
%token EQUALS NAME VARIABLE
%token ILLEGAL_TOKEN

%union {
  const ActionSchema* action;
  const Formula* formula;
  EffectList* effects;
  const Effect* effect;
  const pair<AtomList*, NegationList*>* add_del_lists;
  FormulaList* formulas;
  TermList* terms;
  const Atom* atom;
  const Name* name;
  VariableList* variables;
  const Variable* variable;
  const string* str;
  vector<string>* strings;
  const Type* type;
}

%type <action> action_def
%type <formula> precondition
%type <effects> effect eff_formula one_eff_formulas
%type <effect> one_eff_formula atomic_effs_forall_body
%type <add_del_lists> atomic_effs term_literals term_literal
%type <effect> init
%type <add_del_lists> atomic_name_formulas atomic_name_formula
%type <terms> names
%type <formula> goals
%type <formulas> goal_list
%type <formula> goal
%type <formulas> formulas
%type <formula> formula
%type <atom> atomic_term_formula
%type <str> predicate
%type <terms> terms
%type <variables> opt_variables variables
%type <strings> name_seq variable_seq
%type <type> types
%type <type> type_spec type
%type <str> NAME VARIABLE

%%

pddl_file : { line_number = 1; } domain_or_problems { if (!success) YYERROR; }
          ;

domain_or_problems : /* empty */
                   | domain_or_problems domain_or_problem
                   ;

domain_or_problem : domain  { context = ""; }
                  | problem { context = ""; }
                  ;


/*
 * Domains
 */

domain : '(' DEFINE '(' DOMAIN NAME ')'
           {
	     pdomain = NULL;
	     domain_name = *$5;
	     context = " in domain `" + domain_name + "'";
	     requirements = new Requirements();
	     domain_types = new TypeMap();
	     domain_constants = new NameMap();
	     domain_predicates = new PredicateMap();
	     domain_actions = new ActionSchemaMap();
	     problem_objects = NULL;
	   }
         domain_body ')'
           {
	     new Domain(domain_name, *requirements, *domain_types,
			*domain_constants, *domain_predicates,
			*domain_actions);
	   }
       ;

domain_body : /* empty */
            | '(' require_def
            | '(' require_def domain_body2
            | domain_body2
            ;

domain_body2 : '(' types_def
             | '(' types_def domain_body3
             | domain_body3
             ;

domain_body3 : '(' constants_def
             | '(' constants_def domain_body4
             | domain_body4
             ;

domain_body4 : '(' predicates_def
             | '(' predicates_def action_defs
             | action_defs
             ;

action_defs : action_def             { add_action(*$1); }
            | action_defs action_def { add_action(*$2); }
            ;

require_def : REQUIREMENTS require_keys ')'
            ;

require_keys : require_key
             | require_keys require_key
             ;

require_key : STRIPS
                { requirements->strips = true; }
            | TYPING
                {
		  requirements->typing = true;
		  domain_types->insert(make_pair(SimpleType::OBJECT.name,
						 &SimpleType::OBJECT));
		}
            | DISJUNCTIVE_PRECONDITIONS
                { requirements->disjunctive_preconditions = true; }
	    | EQUALITY
                { requirements->equality = true; }
            | EXISTENTIAL_PRECONDITIONS
                { requirements->existential_preconditions = true; }
            | UNIVERSAL_PRECONDITIONS
                { requirements->universal_preconditions = true; }
            | QUANTIFIED_PRECONDITIONS
                { requirements->quantified_preconditions(); }
            | CONDITIONAL_EFFECTS
                { requirements->conditional_effects = true; }
            | ADL
                { requirements->adl(); }
            ;

types_def : TYPES
              {
		if (!requirements->typing) {
		  domain_types->insert(make_pair(SimpleType::OBJECT.name,
						 &SimpleType::OBJECT));
		  yywarning("assuming `:typing' requirement.");
		}
		name_map_type = "type";
	      }
            name_map ')'
          ;

constants_def : CONSTANTS
                  {
		    name_map_type = "constant";
		    name_map = domain_constants;
		  }
                name_map ')'
              ;

predicates_def : PREDICATES atomic_formula_skeletons ')'
               ;

atomic_formula_skeletons : atomic_formula_skeleton
                         | atomic_formula_skeletons atomic_formula_skeleton
                         ;

atomic_formula_skeleton : '(' predicate
                            { unique_variables = false; }
                          opt_variables ')'
                            {
			      add_predicate(*$2, *$4);
			      unique_variables = true;
			    }
                        ;


/*
 * Actions
 */

action_def : '(' ACTION NAME PARAMETERS
               { free_variables.push_frame(); }
             '(' opt_variables ')'
               {
		 context = (" in action `" + *$3 + "' of domain `"
			    + domain_name + "'");
	       }
             action_body ')'
               {
		 free_variables.pop_frame();
		 $$ = new ActionSchema(*$3, *$7, *action_precond,
				       *action_adds);
	       }
           ;

action_body : precondition effect
                { action_precond = $1; action_adds = $2; }
            | effect
                { action_precond = &Formula::TRUE; action_adds = $1; }
            ;

precondition : PRECONDITION formula { $$ = $2; }
             ;

effect : EFFECT eff_formula { $$ = $2; }
       ;

eff_formula : one_eff_formula
                { $$ = new EffectList($1); }
            | '(' AND one_eff_formulas one_eff_formula ')'
                { $3->push_back($4); $$ = $3; }
            ;

one_eff_formulas : one_eff_formula
                     { $$ = new EffectList($1); }
                 | one_eff_formulas one_eff_formula
                     { $1->push_back($2); $$ = $1; }
                 ;

one_eff_formula : term_literal
                    { $$ = new Effect(*$1->first, *$1->second); }
                | '(' WHEN formula atomic_effs ')'
                    {
		      if (!requirements->conditional_effects) {
			yywarning("assuming `:conditional-effects' "
				  "requirement.");
		      }
		      $$ = new Effect(*$3, *$4->first, *$4->second);
		    }
                | '(' FORALL 
                    {
		      if (!requirements->conditional_effects) {
			yywarning("assuming `:conditional-effects' "
				  "requirement.");
		      }
		      free_variables.push_frame();
		    }
                  '(' variables ')'
                    { eff_forall = $5; }
		  atomic_effs_forall_body ')'
                    {
		      free_variables.pop_frame();
		      $$ = $8;
		    }
                ;

atomic_effs_forall_body : atomic_effs
                            {
			      $$ = new Effect(*eff_forall,
					      *$1->first, *$1->second);
			    }
                        | '(' WHEN formula atomic_effs ')'
                            {
			      $$ = new Effect(*eff_forall, *$3,
					      *$4->first, *$4->second);
			    }
                        ;

atomic_effs : term_literal
            | '(' AND term_literals term_literal ')'
                {
		  copy($4->first->begin(), $4->first->end(),
		       back_inserter(*$3->first));
		  copy($4->second->begin(), $4->second->end(),
		       back_inserter(*$3->second));
		  $$ = $3;
		}
            ;

term_literals : term_literal
              | term_literals term_literal
                  {
		    copy($2->first->begin(), $2->first->end(),
			 back_inserter(*$1->first));
		    copy($2->second->begin(), $2->second->end(),
			 back_inserter(*$1->second));
		    $$ = $1;
		  }
              ;

term_literal : atomic_term_formula
                 { $$ = &make_add_del(new AtomList($1), new NegationList()); }
             | '(' NOT atomic_term_formula ')'
                 { $$ = &make_add_del(new AtomList(), new NegationList($3)); }
             ;


/*
 * Problems
 */

problem : '(' DEFINE '(' PROBLEM NAME ')' '(' PDOMAIN NAME ')'
            {
	      pdomain = Domain::find(*$9);
	      problem_name = *$5;
	      context = " in problem `" + problem_name + "'";
	      if (pdomain != NULL) {
		requirements = new Requirements(pdomain->requirements);
	      } else {
		yyerror("undeclared domain used");
		requirements = new Requirements();
	      }
	      domain_types = NULL;
	      domain_constants = NULL;
	      domain_predicates = NULL;
	      problem_objects = new NameMap();
	    }
          problem_body ')'
            {
	      new Problem(problem_name, *pdomain, *problem_objects,
			  *problem_init, *problem_goal);
	    }
        ;

problem_body : '(' object_decl problem_body2
             | problem_body2
             ;

problem_body2 : '(' init goals
                  { problem_init = $2; problem_goal = $3; }
              | goals
                  {
		    problem_init = new Effect(*(new AtomList()),
					      *(new NegationList()));
		    problem_goal = $1;
		  }
              ;

object_decl : OBJECTS
                {
		  name_map_type = "object";
		  name_map = problem_objects;
		}
              name_map ')'
            ;

init : INIT
         {
	   context =
	     " in initial conditions of problem `" + problem_name + "'";
	 }
       atomic_name_formulas ')'
         { $$ = new Effect(*$3->first, *$3->second); }
     ;

atomic_name_formulas : atomic_name_formula
                     | atomic_name_formulas atomic_name_formula
                         {
			   copy($2->first->begin(), $2->first->end(),
				back_inserter(*$1->first));
			   $$ = $1;
			 }
                     ;

atomic_name_formula : '(' predicate
                        { current_predicate = *$2; }
                      names ')'
                        {
			  AtomList* adds = new AtomList(&make_atom(*$2, *$4));
			  $$ = &make_add_del(adds, new NegationList());
			}
                    ;

names : /* empty */
          { $$ = new TermList(); }
      | names NAME
          { $$ = &add_name(*$1, *$2); }
      ;

goals : goal_list
          {
	    $$ = &Formula::TRUE;
	    for (FormulaListIter fi = $1->begin(); fi != $1->end(); fi++) {
	      $$ = &(*$$ && **fi);
	    }
	  }
      ;

goal_list : goal           { $$ = new FormulaList($1); }
          | goal_list goal { $1->push_back($2); $$ = $1; }
          ;

goal : '(' GOAL
         { context = " in goal of problem `" + problem_name + "'"; }
       formula ')'
         { $$ = $4; }
     ;


/*
 * Formulas
 */

formulas : formula          { $$ = new FormulaList($1); }
         | formulas formula { $1->push_back($2); $$ = $1; }
         ;

formula : atomic_term_formula
            { $$ = $1 }
        | '(' EQUALS terms ')'
            {
	      if (!requirements->equality) {
		yywarning("assuming `:equality' requirement.");
	      }
	      if ($3->size() != 2) {
		yyerror(tostring($3->size()) + " parameter"
			+ string(($3->size() == 1) ? "" : "s")
			+ " passed to predicate `=' expecting 2");
	      }
	      $$ = new Equality(*(*$3)[0], *(*$3)[1]);
	    }
        | '(' NOT formula ')'
            {
	      if (!requirements->disjunctive_preconditions
		  && dynamic_cast<const Equality*>($3) == NULL) {
		yywarning("assuming `:disjunctive-preconditions' "
			  "requirement.");
	      }
	      $$ = &!*$3;
	    }
        | '(' AND formulas formula ')'
            {
	      $$ = &Formula::TRUE;
	      for (FormulaListIter fi = $3->begin(); fi != $3->end(); fi++) {
		$$ = &(*$$ && **fi);
	      }
	      $$ = &(*$$ && *$4);
	    }
        | '(' OR formulas formula ')'
            {
	      if (!requirements->disjunctive_preconditions) {
		yywarning("assuming `:disjunctive-preconditions' "
			  "requirement.");
	      }
	      $$ = &Formula::FALSE;
	      for (FormulaListIter fi = $3->begin(); fi != $3->end(); fi++) {
		$$ = &(*$$ || **fi);
	      }
	      $$ = &(*$$ || *$4);
	    }
        | '(' IMPLY formula formula ')'
            {
	      if (!requirements->disjunctive_preconditions) {
		yywarning("assuming `:disjunctive-preconditions' "
			  "requirement.");
	      }
	      $$ = &(!*$3 || *$4);
	    }
        | '(' EXISTS
            {
	      if (!requirements->existential_preconditions) {
		yywarning("assuming `:existential-preconditions' "
			  "requirement.");
	      }
	      free_variables.push_frame();
	    }
	  '(' variables ')' formula ')'
            {
	      free_variables.pop_frame();
	      $$ = new ExistsFormula(*$5, *$7);
	    }
        | '(' FORALL
            {
	      if (!requirements->universal_preconditions) {
		yywarning("assuming `:universal-preconditions' requirement.");
	      }
	      free_variables.push_frame();
	    }
	  '(' variables ')' formula ')'
            {
	      free_variables.pop_frame();
	      $$ = new ForallFormula(*$5, *$7);
	    }
        ;

atomic_term_formula : '(' predicate
                        { current_predicate = *$2; }
                      terms ')'
                        { $$ = &make_atom(*$2, *$4); }
                    ;

predicate : NAME
          ;

terms : /* empty */
          { $$ = new TermList(); }
      | terms NAME
          { $$ = &add_name(*$1, *$2); }
      | terms VARIABLE
          {
	    const Variable* v = free_variables.find(*$2);
	    if (v == NULL) {
	      yyerror("free variable `" + *$2 + "'");
	      v = new Variable(*$2);
	    }
	    $1->push_back(v);
	    $$ = $1;
	  }
      ;

name_map : /* empty */
         | typed_names
	 ;

typed_names : name_seq { add_names(*$1); }
            | name_seq type_spec { add_names(*$1, *$2); } opt_typed_names
            ;

opt_typed_names : /* empty */
                | typed_names
                ;

name_seq : NAME          { $$ = new vector<string>(1, *$1); }
         | name_seq NAME { $1->push_back(*$2); $$ = $1; }
         ;

opt_variables : /* empty */ { $$ = new VariableList(); }
              | variables
	      ;

variables : { variables = new VariableList(); } vars { $$ = variables; }
          ;

vars : variable_seq
         {
	   for (vector<string>::const_iterator si = $1->begin();
		si != $1->end(); si++) {
	     add_variable(*(new Variable(*si)));
	   }
	 }
     | variable_seq type_spec 
         {
	   for (vector<string>::const_iterator si = $1->begin();
		si != $1->end(); si++) {
	     add_variable(*(new Variable(*si, *$2)));
	   }
	 }
       opt_vars
     ;

opt_vars : /* empty */
         | vars
         ;

variable_seq : VARIABLE              { $$ = new vector<string>(1, *$1); }
             | variable_seq VARIABLE { $1->push_back(*$2); $$ = $1; }
             ;

types : type
      | types type { $$ = &(*$1 + *$2); }
      ;

type_spec : '-' type { $$ = $2; }
          ;

type : NAME                 { $$ = &make_type(*$1); }
     | '(' EITHER types ')' { $$ = $3; }
     ;

%%

/*
 * Converts an unsigned integer to a string.
 */
static string tostring(unsigned int n) {
  string result;
  while (n > 0) {
    result = char(n % 10 + '0') + result;
    n /= 10;
  }
  return (result.length() == 0) ? "0" : result;
}


/*
 * Outputs an error message.
 */
static void yyerror(const string& s) {
  cerr << PROGRAM_NAME << ':' << current_file << ':' << line_number << ": "
       << s << context << endl;
  success = false;
}


/*
 * Outputs a warning.
 */
static void yywarning(const string& s) {
  if (warning_level > 0) {
    cerr << PROGRAM_NAME << ':' << current_file << ':' << line_number << ": "
	 << s << context << endl;
    if (warning_level > 1) {
      success = false;
    }
  }
}


/*
 * Returns the type with the given name, or NULL if it is undefined.
 */
static const Type* find_type(const string& name) {
  if (pdomain != NULL) {
    return pdomain->find_type(name);
  } else if (domain_types != NULL) {
    TypeMapIter ti = domain_types->find(name);
    return (ti != domain_types->end()) ? (*ti).second : NULL;
  } else {
    return NULL;
  }
}


/*
 * Returns the constant or object with the given name, or NULL if it
 * is undefined.  */
static const Name* find_constant(const string& name) {
  const Name* c = NULL;
  if (pdomain != NULL) {
    c = pdomain->find_constant(name);
  }
  if (c == NULL && domain_constants != NULL) {
    NameMapIter ni = domain_constants->find(name);
    c = (ni != domain_constants->end()) ? (*ni).second : NULL;
  }
  if (c == NULL && problem_objects != NULL) {
    NameMapIter ni = problem_objects->find(name);
    c = (ni != problem_objects->end()) ? (*ni).second : NULL;
  }
  return c;
}


/*
 * Returns the predicate with the given name, or NULL if it is
 * undefined.
 */
static const Predicate* find_predicate(const string& name) {
  if (pdomain != NULL) {
    return pdomain->find_predicate(name);
  } else if (domain_predicates != NULL) {
    PredicateMap::const_iterator pi = domain_predicates->find(name);
    return (pi != domain_predicates->end()) ? (*pi).second : NULL;
  } else {
    return NULL;
  }
}


/*
 * Adds types, constants, or objects to the current domain or problem.
 */
static void add_names(const vector<string>& names, const Type& type) {
  for (vector<string>::const_iterator si = names.begin();
       si != names.end(); si++) {
    const string& s = *si;
    if (name_map_type == "type") {
      if (find_type(s) == NULL) {
	domain_types->insert(make_pair(s, new SimpleType(s, type)));
      } else {
	yywarning("ignoring repeated declaration of " + name_map_type
		  + " `" + s + "'");
      }
    } else {
      NameMapIter ni = name_map->find(s);
      if (ni == name_map->end()) {
	if (pdomain != NULL && pdomain->find_constant(s) != NULL) {
	  yywarning("ignoring declaration of object `" + s
		    + "' previously declared as constant");
	} else {
	  name_map->insert(make_pair(s, new Name(s, type)));
	}
      } else {
	(*name_map)[s] = new Name(s, (*ni).second->type + type);
      }
    }
  }
}


/*
 * Adds a predicate to the current domain.
 */
static void add_predicate(const string& name, const VariableList& parameters) {
  if (find_predicate(name) == NULL) {
    if (find_type(name) == NULL) {
      domain_predicates->insert(make_pair(name,
					  new Predicate(name, parameters)));
    } else {
      yywarning("ignoring declaration of predicate `" + name
		+ "' in domain `" + domain_name
		+ "' previously declared as type");
    }
  } else {
    yywarning("ignoring repeated declaration of predicate `" + name
	      + "' in domain `" + domain_name + "'");
  }
}


/*
 * Adds an action schema to the current domain.
 */
static void add_action(const ActionSchema& action) {
  if (domain_actions->find(action.name) != domain_actions->end()) {
    yywarning("ignoring repeated declaration of action `" + action.name
	      + "' in domain `" + domain_name + "'");
  } else {
    domain_actions->insert(make_pair(action.name, &action));
  }
}


/*
 * Adds a variable to the current context.
 */
static void add_variable(const Variable& var) {
  if (unique_variables) {
    if (free_variables.shallow_find(var.name) != NULL) {
      yyerror("repetition of parameter `" + var.name + "'");
    } else if (free_variables.find(var.name) != NULL) {
      yyerror("shadowing parameter `" + var.name + "'");
    }
    free_variables.insert(&var);
  }
  variables->push_back(&var);
}


/*
 * Creates an add/delete list pair.
 */
static const pair<AtomList*, NegationList*>& make_add_del(AtomList* adds,
							  NegationList* dels) {
  return *(new (GC) pair<AtomList*, NegationList*>(adds, dels));
}


/*
 * Creates a formula (predicate terms[0] ...).
 */
static const Atom& make_atom(const string& predicate, const TermList& terms) {
  const Predicate* p = find_predicate(predicate);
  if (p == NULL) {
    if (find_type(predicate) != NULL) {
      if (terms.size() != 1) {
	yyerror(tostring(terms.size())
		+ "parameters passed to type predicate `" + predicate + "'");
      }
    } else if (domain_predicates != NULL) {
      VariableList& params = *(new VariableList());
      for (size_t i = 0; i < terms.size(); i++) {
	params.push_back(new Variable("?x" + tostring(i + 1)));
      }
      domain_predicates->insert(make_pair(predicate,
					  new Predicate(predicate, params)));
      yywarning("implicit declaration of predicate `" + predicate + "'");
    } else {
      yyerror("undeclared predicate `" + predicate + "' used");
    }
  } else if (p->arity() != terms.size()) {
    yyerror(tostring(terms.size()) + " parameter"
	    + string((terms.size() == 1) ? "" : "s")
	    + " passed to predicate `" + predicate
	    + "' expecting " + tostring(p->arity()));
  }
  return *(new Atom(predicate, terms));
}


/*
 * Adds a name to the given name term list.
 */
static TermList& add_name(TermList& terms, const string& name) {
  const Name* c = find_constant(name);
  if (c == NULL) {
    const Predicate* predicate = find_predicate(current_predicate);
    if (predicate == NULL || predicate->arity() <= terms.size()) {
      c = new Name(name, SimpleType::OBJECT);
    } else {
      c = new Name(name, predicate->parameters[terms.size()]->type);
    }
    if (domain_constants != NULL) {
      domain_constants->insert(make_pair(name, c));
      yywarning("implicit declaration of constant `" + name + "'");
    } else {
      problem_objects->insert(make_pair(name, c));
      yywarning("implicit declaration of object `" + name + "'");
    }
  }
  terms.push_back(c);
  return terms;
}


static const Type& make_type(const string& name) {
  const Type* t = find_type(name);
  if (t == NULL) {
    const SimpleType& st = *(new SimpleType(name));
    if (domain_types != NULL) {
      domain_types->insert(make_pair(name, &st));
      yywarning("implicit declaration of type `" + name + "'");
    } else {
      yyerror("undeclared type `" + name + "' used");
    }
    return st;
  } else {
    return *t;
  }
}
