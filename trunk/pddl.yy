/* -*-C++-*- */
/*
 * PDDL parser.
 *
 * $Id: pddl.yy,v 1.3 2001-07-29 18:14:50 lorens Exp $
 */
%{
#include <utility>
#include <stdlib.h>
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
		      const Type& type = SimpleType::OBJECT_TYPE);
static void add_predicate(const string& name, const VariableList& parameters);
static void add_action(const ActionSchema& action);
static void add_variable(const Variable& var);
static const Formula& make_atomic_formula(const string& predicate,
					  const TermList& terms);


/*
 * Context of free variables.
 */
struct Context {
  void push_frame() {
    frames_.push_back(VariableMap());
  }

  void pop_frame() {
    frames_.pop_back();
  }

  void insert(const Variable* v) {
    (frames_.back())[v->name] = v;
  }

  const Variable* shallow_find(const string& name) const {
    VariableMap::const_iterator i = frames_.back().find(name);
    return (i != frames_.back().end()) ? (*i).second : NULL;
  }

  const Variable* find(const string& name) const {
    for (vector<VariableMap>::const_reverse_iterator i = frames_.rbegin();
	 i != frames_.rend(); i++) {
      VariableMap::const_iterator j = (*i).find(name);
      if (j != (*i).end()) {
	return (*j).second;
      }
    }
    return NULL;
  }

private:
  typedef hash_map<string, const Variable*, hash<string>, equal_to<string>,
    container_alloc> VariableMap;

  vector<VariableMap> frames_;
};


/* Level of warnings. */
int warning_level;

static bool success = true;
static const Domain* pdomain;
static string domain_name;
static Requirements* requirements;
static TypeMap* domain_types;
static NameMap* domain_constants;
static PredicateMap* domain_predicates;
static ActionMap* domain_actions;
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
%token NAME VARIABLE
%token ILLEGAL_TOKEN

%union {
  const ActionSchema* action;
  const Formula* formula;
  EffectList* effects;
  const Effect* effect;
  FormulaList* formulas;
  TermList* terms;
  const AtomicFormula* atomic_formula;
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
%type <formulas> atomic_effs term_literals
%type <formula> term_literal
%type <effect> init
%type <formulas> atomic_name_formulas
%type <formula> atomic_name_formula
%type <terms> names
%type <formula> goals
%type <formulas> goal_list
%type <formula> goal
%type <formulas> formulas
%type <formula> formula
%type <formula> atomic_term_formula
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

domain_or_problem : domain
                  | problem
                  ;


/*
 * Domains
 */

domain : '(' DEFINE '(' DOMAIN NAME ')'
           {
	     pdomain = NULL;
	     domain_name = *$5;
	     requirements = new Requirements();
	     domain_types = new TypeMap();
	     domain_constants = new NameMap();
	     domain_predicates = new PredicateMap();
	     domain_actions = new ActionMap();
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
                { requirements->typing = true; }
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
		  yywarning("assuming ':typing' requirement.");
		}
		context = "domain '" + domain_name + "'";
		name_map_type = "type";
	      }
            name_map ')'
          ;

constants_def : CONSTANTS
                  {
		    context = "domain '" + domain_name + "'";
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
               {
		 free_variables.push_frame();
	       }
             '(' opt_variables ')'
               {
		 context = ("action '" + *$3 + "' of domain '" +
			    domain_name + "'");
	       }
             action_body ')'
               {
		 free_variables.pop_frame();
		 $$ = new ActionSchema(*$3, *$7, action_precond, *action_adds);
	       }
           ;

action_body : precondition effect { action_precond = $1; action_adds = $2; }
            | effect              { action_precond = NULL; action_adds = $1; }
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
                    { $$ = new Effect(*$1); }
                | '(' WHEN formula atomic_effs ')'
                    {
		      if (!requirements->conditional_effects) {
			yywarning("assuming ':conditional-effects' "
				  "requirement.");
		      }
		      $$ = new Effect($3, *$4);
		    }
                | '(' FORALL 
                    {
		      if (!requirements->conditional_effects) {
			yywarning("assuming ':conditional-effects' "
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
			      $$ = new Effect(*eff_forall, NULL, *$1);
			    }
                        | '(' WHEN formula atomic_effs ')'
                            {
			      $$ = new Effect(*eff_forall, $3, *$4);
			    }
                        ;

atomic_effs : term_literal
                { $$ = new FormulaList($1); }
            | '(' AND term_literals term_literal ')'
                { $3->push_back($4); $$ = $3; }
            ;

term_literals : term_literal
                  { $$ = new FormulaList($1); }
              | term_literals term_literal
                  { $1->push_back($2); $$ = $1; }
              ;

term_literal : atomic_term_formula
                 { $$ = $1; }
             | '(' NOT atomic_term_formula ')'
                 { $$ = &$3->negation(); }
             ;


/*
 * Problems
 */

problem : '(' DEFINE '(' PROBLEM NAME ')' '(' PDOMAIN NAME ')'
            {
	      pdomain = Domain::find(*$9);
	      if (pdomain != NULL) {
		requirements = new Requirements(pdomain->requirements);
	      } else {
		yyerror("undeclared domain used in problem '" + *$5 + "'");
		requirements = new Requirements();
	      }
	      domain_types = NULL;
	      domain_constants = NULL;
	      domain_predicates = NULL;
	      problem_name = *$5;
	      problem_objects = new NameMap();
	    }
          problem_body ')'
            {
	      new Problem(problem_name, *pdomain, *problem_objects,
			  problem_init, *problem_goal);
	    }
        ;

problem_body : object_decl problem_body2
             ;

problem_body2 : '(' init goals { problem_init = $2; problem_goal = $3; }
              | goals          { problem_init = NULL; problem_goal = $1; }
              ;

object_decl : '(' OBJECTS
                {
		  context = "problem '" + problem_name + "'";
		  name_map_type = "object";
		  name_map = problem_objects;
		}
              name_map ')'
            ;

init : INIT
         {
	   context = "initial conditions of problem '" + problem_name + "'";
	 }
       atomic_name_formulas ')'
         { $$ = new Effect(*$3); }
     ;

atomic_name_formulas : atomic_name_formula
                         { $$ = new FormulaList($1); }
                     | atomic_name_formulas atomic_name_formula
                         { $1->push_back($2); $$ = $1; }
                     ;

atomic_name_formula : '(' predicate
                        { current_predicate = *$2; }
                      names ')'
                        { $$ = &make_atomic_formula(*$2, *$4); }
                    ;

names : /* empty */
          { $$ = new TermList(); }
      | names NAME
          {
	    const Name* c = find_constant(*$2);
	    if (c == NULL) {
	      const Predicate* predicate = find_predicate(current_predicate);
	      if (predicate == NULL || predicate->arity() <= $1->size()) {
		c = new Name(*$2);
	      } else {
		c = new Name(*$2, predicate->parameters[$1->size()]->type);
	      }
	      (*problem_objects)[*$2] = c;
	      yywarning("implicit declaration of object '" + *$2 +
			"' in " + context);
	    }
	    $1->push_back(c);
	    $$ = $1;
	  }
      ;

goals : goal_list
          {
	    if ($1->size() > 2) {
	      $$ = new Conjunction(*$1);
	    } else {
	      $$ = $1->front();
	    }
	  }
      ;

goal_list : goal           { $$ = new FormulaList($1); }
          | goal_list goal { $1->push_back($2); $$ = $1; }
          ;

goal : '(' GOAL
         { context = "goal of problem '" + problem_name + "'"; }
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
        | '(' NOT formula ')'
            {
	      if (!requirements->disjunctive_preconditions) {
		yywarning("assuming ':disjunctive-preconditions' "
			  "requirement.");
	      }
	      $$ = &$3->negation();
	    }
        | '(' AND formulas formula ')'
            { $3->push_back($4); $$ = new Conjunction(*$3); }
        | '(' OR formulas formula ')'
            {
	      if (!requirements->disjunctive_preconditions) {
		yywarning("assuming ':disjunctive-preconditions' "
			  "requirement.");
	      }
	      $3->push_back($4);
	      $$ = new Disjunction(*$3);
	    }
        | '(' IMPLY formula formula ')'
            {
	      if (!requirements->disjunctive_preconditions) {
		yywarning("assuming ':disjunctive-preconditions' "
			  "requirement.");
	      }
	      FormulaList& disjuncts = *(new FormulaList());
	      disjuncts.push_back(&$3->negation());
	      disjuncts.push_back($4);
	      $$ = new Disjunction(disjuncts);
	    }
        | '(' EXISTS
            {
	      if (!requirements->existential_preconditions) {
		yywarning("assuming ':existential-preconditions' "
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
		yywarning("assuming ':universal-preconditions' requirement.");
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
                        { $$ = &make_atomic_formula(*$2, *$4); }
                    ;

predicate : NAME
          ;

terms : /* empty */
          { $$ = new TermList(); }
      | terms NAME
          {
	    const Name* c = find_constant(*$2);
	    if (c == NULL) {
	      const Predicate* predicate = find_predicate(current_predicate);
	      if (predicate == NULL || predicate->arity() <= $1->size()) {
		c = new Name(*$2);
	      } else {
		c = new Name(*$2, predicate->parameters[$1->size()]->type);
	      }
	      if (domain_constants != NULL) {
		(*domain_constants)[*$2] = c;
		yywarning("implicit declaration of constant '" + *$2 +
			  "' in " + context);
	      } else {
		(*problem_objects)[*$2] = c;
		yywarning("implicit declaration of object '" + *$2 +
			  "' in " + context);
	      }
	    }
	    $1->push_back(c);
	    $$ = $1;
	  }
      | terms VARIABLE
          {
	    const Variable* v = free_variables.find(*$2);
	    if (v == NULL) {
	      yyerror("free variable '" + *$2 + "'  in " + context);
	      v = new Variable(*$2);
	    }
	    $1->push_back(v);
	    $$ = $1;
	  }
      ;

name_map : /* empty */
         | typed_names
	 ;

typed_names : name_seq           { add_names(*$1); }
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
	   for (vector<string>::const_iterator i = $1->begin();
		i != $1->end(); i++) {
	     add_variable(*(new Variable(*i)));
	   }
	 }
     | variable_seq type_spec 
         {
	   for (vector<string>::const_iterator i = $1->begin();
		i != $1->end(); i++) {
	     add_variable(*(new Variable(*i, *$2)));
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

type : NAME
         {
	   const Type* t = find_type(*$1);
	   if (t == NULL) {
	     const SimpleType* st = new SimpleType(*$1);
	     $$ = st;
	     if (domain_types != NULL) {
	       (*domain_types)[*$1] = st;
	       yywarning("implicit declaration of type '" + *$1 +
			 "' in " + context);
	     } else {
	       yyerror("undeclared type '" + *$1 + "' used in " + context);
	     }
	   } else {
	     $$ = t;
	   }
	 }
     | '(' EITHER types ')'
         { $$ = $3; }
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
  cerr << "line " << line_number << ": " << s << endl;
  success = false;
}


/*
 * Outputs a warning.
 */
static void yywarning(const string& s) {
  if (warning_level > 0) {
    cerr << "line " << line_number << ": " << s << endl;
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
    TypeMap::const_iterator i = domain_types->find(name);
    return (i != domain_types->end()) ? (*i).second : NULL;
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
    NameMap::const_iterator i = domain_constants->find(name);
    c = (i != domain_constants->end()) ? (*i).second : NULL;
  }
  if (c == NULL && problem_objects != NULL) {
    NameMap::const_iterator i = problem_objects->find(name);
    c = (i != problem_objects->end()) ? (*i).second : NULL;
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
    PredicateMap::const_iterator i = domain_predicates->find(name);
    return (i != domain_predicates->end()) ? (*i).second : NULL;
  } else {
    return NULL;
  }
}


/*
 * Adds types, constants, or objects to the current domain or problem.
 */
static void add_names(const vector<string>& names, const Type& type) {
  for (vector<string>::const_iterator i = names.begin();
       i != names.end(); i++) {
    if (name_map_type == "type") {
      if (find_type(*i) == NULL) {
	(*domain_types)[*i] = new SimpleType(*i, type);
      } else {
	yywarning("ignoring repeated declaration of " + name_map_type +
		  " '" + *i + "' in " + context);
      }
    } else {
      NameMap::const_iterator ni = name_map->find(*i);
      if (ni == name_map->end()) {
	(*name_map)[*i] = new Name(*i, type);
      } else {
	(*name_map)[*i] = new Name(*i, (*ni).second->type + type);
      }
    }
  }
}


/*
 * Adds a predicate to the current domain.
 */
static void add_predicate(const string& name, const VariableList& parameters) {
  if (find_predicate(name) == NULL) {
    (*domain_predicates)[name] = new Predicate(name, parameters);
  } else {
    yywarning("ignoring repeated declaration of predicate '" + name +
	      "' in domain '" + domain_name + "'");
  }
}


/*
 * Adds an action schema to the current domain.
 */
static void add_action(const ActionSchema& action) {
  if (domain_actions->find(action.name) != domain_actions->end()) {
    yywarning("ignoring repeated declaration of action '" + action.name +
	      "' in domain '" + domain_name + "'");
  } else {
    (*domain_actions)[action.name] = &action;
  }
}


/*
 * Adds a variable to the current context.
 */
static void add_variable(const Variable& var) {
  if (unique_variables) {
    if (free_variables.shallow_find(var.name) != NULL) {
      yyerror("repetition of parameter '" + var.name + "' in " + context);
    } else if (free_variables.find(var.name) != NULL) {
      yyerror("shadowing parameter '" + var.name + "' in " + context);
    }
    free_variables.insert(&var);
  }
  variables->push_back(&var);
}


/*
 * Creates a formula (predicate terms[0] ...).
 */
static const Formula& make_atomic_formula(const string& predicate,
					  const TermList& terms) {
  if (predicate == "=") {
    if (!requirements->equality) {
      yywarning("assuming ':equality' requirement.");
    }
    if (terms.size() == 2) {
      return *(new Equality(*terms[0], *terms[1]));
    } else {
      yyerror(tostring(terms.size()) + " parameter" +
	      string((terms.size() == 1) ? "" : "s") +
	      " passed to predicate '=' expecting 2 in " + context);
    }
  } else {
    const Predicate* p = find_predicate(predicate);
    if (p == NULL) {
      if (domain_predicates != NULL) {
	VariableList& params = *(new VariableList());
	for (size_t i = 0; i < terms.size(); i++) {
	  params.push_back(new Variable("?x" + tostring(i + 1)));
	}
	(*domain_predicates)[predicate] = new Predicate(predicate, params);
	yywarning("implicit declaration of predicate '" + predicate +
		  "' in " + context);
      } else {
	yyerror("undeclared predicate '" + predicate + "' used in " + context);
      }
    } else if (p->arity() != terms.size()) {
      yyerror(tostring(terms.size()) + " parameter" +
	      string((terms.size() == 1) ? "" : "s") +
	      " passed to predicate '" + predicate +
	      "' expecting " + tostring(p->arity()) + " in " + context);
    }
  }
  return *(new AtomicFormula(predicate, terms));
}
