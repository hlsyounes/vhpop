/* -*-C++-*- */
/*
 * PDDL parser.
 *
 * $Id: pddl.yy,v 1.1 2001-05-03 15:43:14 lorens Exp $
 */
%{
#include <utility>
#include <stdlib.h>
#include "domains.h"
#include "problems.h"
#include "formulas.h"

extern int yylex();

static void yyerror(const char* s);
static void yyerror(const string& s); 
static const Name* find_constant(const string& name);
static const Type* find_type(const string& name);
static const Predicate* find_predicate(const string& name);
static void add_variable(const Variable& var);

extern unsigned int line_number;


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


static bool success = true;
static const Domain* pdomain;
static string domain_name;
static TypeMap* domain_types;
static const NameMap* domain_constants;
static PredicateMap* domain_predicates;
static const ActionMap* domain_actions;
static Domain::ActionStyle action_style;
static bool support_equality;
static const Formula* action_precond; 
static const EffectList* action_adds;
static NameMap* name_map;
static VariableList* variables;
static Context free_variables;
static const VariableList* eff_forall;
static string problem_name;
static const NameMap* problem_objects = NULL;
static const Effect* problem_init;
static const Formula* problem_goal;
static bool unique_variables = true;
static string name_map_type;
static string context;
%}

%token DEFINE DOMAIN PROBLEM
%token REQUIREMENTS REQSTRIPS REQADL TYPING EQUALITY
%token TYPES CONSTANTS PREDICATES ACTION PARAMETERS PRECONDITION EFFECT
%token PDOMAIN OBJECTS INIT GOAL
%token WHEN NOT AND OR IMPLY EXISTS FORALL
%token EITHER
%token NAME VARIABLE
%token ILLEGAL_TOKEN

%union {
  ActionMap* actions;
  const Action* action;
  const Formula* formula;
  EffectList* effects;
  const Effect* effect;
  FormulaList* formulas;
  TermList* terms;
  const AtomicFormula* atomic_formula;
  const Name* name;
  const Term* term;
  NameMap* name_map;
  VariableList* variables;
  const Variable* variable;
  const string* str;
  vector<string>* strings;
  TypeList* types;
  const Type* type;
}

%type <actions> action_defs
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
%type <term> term
%type <name_map> name_map
%type <variables> opt_variables variables
%type <strings> name_seq variable_seq
%type <types> types
%type <type> type_spec type
%type <str> NAME VARIABLE

%%

pddl_file : { line_number = 1; } domain_or_problems
              { if (!success) YYERROR; }
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
	     action_style = Domain::STRIPS;
	     support_equality = false;
	     domain_name = *$5;
	     domain_types = new TypeMap();
	     (*domain_types)[string("object")] = &SimpleType::OBJECT_TYPE;
	     domain_constants = NULL;
	     domain_predicates = new PredicateMap();
	     domain_actions = NULL;
	   }
         domain_body ')'
           {
	     if (domain_constants == NULL) {
	       domain_constants = new NameMap();
	     }
	     if (domain_actions == NULL) {
	       domain_actions = new ActionMap();
	     }
	     new Domain(domain_name, action_style, *domain_types,
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
                 { domain_actions = $3; }
             | action_defs
                 { domain_actions = $1; }
             ;

action_defs : action_def
                { $$ = new ActionMap(); (*$$)[$1->name] = $1; }
            | action_defs action_def
                {
		  if ($1->find($2->name) != $1->end()) {
		    yyerror("multiple definitions of action '" + $2->name +
			    "' in domain '" + domain_name + "'");
		  } else {
		    (*$1)[$2->name] = $2;
		  }
		  $$ = $1;
		}
            ;

require_def : REQUIREMENTS require_keys ')'
            ;

require_keys : require_key
             | require_keys require_key
             ;

require_key : REQSTRIPS { action_style = Domain::STRIPS; }
            | REQADL    { action_style = Domain::ADL; }
            | TYPING
	    | EQUALITY  { support_equality = true; }
            ;

types_def : TYPES
              {
		name_map_type = "type";
		context = "domain '" + domain_name + "'";
	      }
            name_map ')'
          ;

constants_def : CONSTANTS
                  {
		    name_map_type = "constant";
		    context = "domain '" + domain_name + "'";
		  }
                name_map ')'
                  { domain_constants = $3; }
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
			      unique_variables = true;
			      if (domain_predicates->find(*$2) !=
				  domain_predicates->end()) {
				yyerror("multiple declarations of "
					"predicate '" + *$2 +
					"' in domain '" + domain_name + "'");
			      } else {
				(*domain_predicates)[*$2] =
				  new Predicate(*$2, *$4);
			      }
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
		 $$ = new Action(*$3, *$7, action_precond, *action_adds);
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
		      if (action_style == Domain::STRIPS) {
			yyerror("conditional "
				"not allowed in STRIPS-style effects.");
		      }
		      $$ = new Effect($3, *$4);
		    }
                | '(' FORALL 
                    {
		      if (action_style == Domain::STRIPS) {
			yyerror("universal quantification "
				"not allowed in STRIPS-style effects.");
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
	      if (pdomain == NULL) {
		yyerror("undefined domain used in problem '" + *$5 + "'");
		YYERROR;
	      }
	      action_style = pdomain->action_style;
	      problem_name = *$5;
	    }
          problem_body ')'
            {
	      new Problem(problem_name, *pdomain, *problem_objects,
			  problem_init, *problem_goal);
	      problem_objects = NULL;
	    }
        ;

problem_body : object_decl problem_body2
             ;

problem_body2 : '(' init goals { problem_init = $2; problem_goal = $3; }
              | goals          { problem_init = NULL; problem_goal = $1; }
              ;

object_decl : '(' OBJECTS
                {
		  name_map_type = "object";
		  context = "problem '" + problem_name + "'";
		}
              name_map ')'
                { problem_objects = $4; }
            ;

init : INIT
         {
	   context = ("initial conditions of problem '" + problem_name + "'");
	 }
       atomic_name_formulas ')'
         { $$ = new Effect(*$3); }
     ;

atomic_name_formulas : atomic_name_formula
                         { $$ = new FormulaList($1); }
                     | atomic_name_formulas atomic_name_formula
                         { $1->push_back($2); $$ = $1; }
                     ;

atomic_name_formula : '(' predicate names ')'
                        {
			  if (support_equality && *$2 == "=") {
			    if ($3->size() != 2) {
			      yyerror($3->size() + " parameter" +
				      string(($3->size() == 1) ? "" : "s") +
				      " passed to predicate '=' expecting 2"
				      " in " + context);
			    }
			    $$ = new Equality(*(*$3)[0], *(*$3)[1]);
			  } else {
			    const Predicate* p =
			      pdomain->find_predicate(*$2);
			    if (p == NULL) {
			      yyerror("undeclared predicate '" + *$2 +
				      "' used in " + context);
			    } else if (p->arity() != $3->size()) {
			      yyerror($3->size() + " parameter" +
				      string(($3->size() == 1) ? "" : "s") +
				      " passed to predicate '" + *$2 +
				      "' expecting " + tostring(p->arity()) +
				      " in " + context);
			    }
			    $$ = new AtomicFormula(*$2, *$3);
			  }
			}
                    ;

names : /* empty */
          { $$ = new TermList(); }
      | names NAME
          {
	    const Name* c = find_constant(*$2);
	    if (c == NULL) {
	      yyerror("undeclared constant '" + *$2 + "' used in " + context);
	      $1->push_back(new Name(*$2));
	    } else {
	      $1->push_back(c);
	    }
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
	      if (action_style == Domain::STRIPS) {
		yyerror("negation not allowed in STRIPS-style preconditions.");
	      }
	      $$ = &$3->negation();
	    }
        | '(' AND formulas formula ')'
            { $3->push_back($4); $$ = new Conjunction(*$3); }
        | '(' OR formulas formula ')'
            {
	      if (action_style == Domain::STRIPS) {
		yyerror("disjunction "
			"not allowerd in STRIPS-style preconditions.");
	      }
	      $3->push_back($4);
	      $$ = new Disjunction(*$3);
	    }
        | '(' IMPLY formula formula ')'
            {
	      if (action_style == Domain::STRIPS) {
		yyerror("implication "
			"not allowed in STRIPS-style preconditions.");
	      }
	      FormulaList& disjuncts = *(new FormulaList());
	      disjuncts.push_back(&$3->negation());
	      disjuncts.push_back($4);
	      $$ = new Disjunction(disjuncts);
	    }
        | '(' EXISTS
            {
	      if (action_style == Domain::STRIPS) {
		yyerror("existential quantification "
			"not allowed in STRIPS-style preconditions.");
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
	      if (action_style == Domain::STRIPS) {
		yyerror("universal quantification "
			"not allowed in STRIPS-style preconditions.");
	      }
	      free_variables.push_frame();
	    }
	  '(' variables ')' formula ')'
            {
	      free_variables.pop_frame();
	      $$ = new ForallFormula(*$5, *$7);
	    }
        ;

atomic_term_formula : '(' predicate terms ')'
                        {
			  if (support_equality && *$2 == "=") {
			    if ($3->size() != 2) {
			      yyerror($3->size() + " parameter" +
				      string(($3->size() == 1) ? "" : "s") +
				      " passed to predicate '=' expecting 2"
				      " in " + context);
			    }
			    $$ = new Equality(*(*$3)[0], *(*$3)[1]);
			  } else {
			    const Predicate* p = find_predicate(*$2);
			    if (p == NULL) {
			      yyerror("undeclared predicate '" + *$2 +
				      "' used in " + context);
			    } else if (p->arity() != $3->size()) {
			      yyerror($3->size() + " parameter" +
				      string(($3->size() == 1) ? "" : "s") +
				      " passed to predicate '" + *$2 +
				      "' expecting " + tostring(p->arity()) +
				      " in " + context);
			    }
			    $$ = new AtomicFormula(*$2, *$3);
			  }
			}
                    ;

predicate : NAME
          ;

terms : /* empty */ { $$ = new TermList(); }
      | terms term  { $1->push_back($2); $$ = $1; }
      ;

term : NAME
         {
	   const Name* c = find_constant(*$1);
	   if (c == NULL) {
	     yyerror("undeclared constant '" + *$1 + "' used in " + context);
	     $$ = new Name(*$1);
	   } else {
	     $$ = c;
	   }
	 }
     | VARIABLE
         {
	   const Variable* v = free_variables.find(*$1);
	   if (v == NULL) {
	     yyerror("undeclared variable '" + *$1 + "' used in " +
		     context);
	     $$ = new Variable(*$1);
	   } else {
	     $$ = v;
	   }
	 }
     ;

name_map : /* empty */ { $$ = new NameMap(); }
         | { name_map = new NameMap(); } typed_names { $$ = name_map; }
	 ;

typed_names : name_seq
                {
		  for (vector<string>::const_iterator i = $1->begin();
		       i != $1->end(); i++) {
		    if (name_map_type == "type") {
		      if (domain_types->find(*i) != domain_types->end()) {
			yyerror("multiple declarations of " + name_map_type +
				" '" + *i + "' in " + context);
		      } else {
			(*domain_types)[*i] = new SimpleType(*i);
		      }
		    } else {
		      if (name_map->find(*i) != name_map->end()) {
			yyerror("multiple declarations of " + name_map_type +
				" '" + *i + "' in " + context);
		      } else {
			(*name_map)[*i] = new Name(*i);
		      }
		    }
		  }
		}
            | name_seq type_spec
                {
		  for (vector<string>::const_iterator i = $1->begin();
		       i != $1->end(); i++) {
		    if (name_map_type == "type") {
		      if (domain_types->find(*i) != domain_types->end()) {
			yyerror("multiple declarations of " + name_map_type +
				" '" + *i + "' in " + context);
		      } else {
			(*domain_types)[*i] = new SimpleType(*i, *$2);
		      }
		    } else {
		      if (name_map->find(*i) != name_map->end()) {
			yyerror("multiple declarations of " + name_map_type +
				" '" + *i + "' in " + context);
		      } else {
			(*name_map)[*i] = new Name(*i, *$2);
		      }
		    }
		  }
		}
	      opt_typed_names
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

types : type        { $$ = new TypeList(1, $1); }
      | types type  { $1->push_back($2); $$ = $1; }
      ;

type_spec : '-' type    { $$ = $2; }
          ;

type : NAME
         {
	   const Type* t = find_type(*$1);
	   if (t == NULL) {
	     yyerror("undeclared type '" + *$1 + "' used in " + context);
	     $$ = &SimpleType::OBJECT_TYPE;
	   } else {
	     $$ = t;
	   }
	 }
     | '(' EITHER types ')'
         { $$ = new UnionType(*$3); }
     ;

%%

static void yyerror(const char* s) {
  cerr << "line " << line_number << ": " << s << endl;
  success = false;
}

static void yyerror(const string& s) {
  cerr << "line " << line_number << ": " << s << endl;
  success = false;
}

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

static const Type* find_type(const string& name) {
  TypeMap::const_iterator i = domain_types->find(name);
  return (i != domain_types->end()) ? (*i).second : NULL;
}

static const Predicate* find_predicate(const string& name) {
  if (pdomain != NULL) {
    return pdomain->find_predicate(name);
  } else {
    PredicateMap::const_iterator i = domain_predicates->find(name);
    return (i != domain_predicates->end()) ? (*i).second : NULL;
  }
}

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
