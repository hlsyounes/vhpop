/* -*-C++-*- */
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
//
// PDDL parser.

%{
#include <cstdlib>
#include <iostream>
#include <typeinfo>
#include <utility>

#include "domains.h"
#include "formulas.h"
#include "functions.h"
#include "predicates.h"
#include "problems.h"
#include "requirements.h"
#include "terms.h"
#include "types.h"

/* Workaround for bug in Bison 1.35 that disables stack growth. */
#define YYLTYPE_IS_TRIVIAL 1


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

  void insert(const std::string& name, const Variable& v) {
    frames_.back().insert(std::make_pair(name, v));
  }

  const Variable* shallow_find(const std::string& name) const {
    VariableMap::const_iterator vi = frames_.back().find(name);
    if (vi != frames_.back().end()) {
      return &(*vi).second;
    } else {
      return 0;
    }
  }

  const Variable* find(const std::string& name) const {
    for (std::vector<VariableMap>::const_reverse_iterator fi =
	   frames_.rbegin(); fi != frames_.rend(); fi++) {
      VariableMap::const_iterator vi = (*fi).find(name);
      if (vi != (*fi).end()) {
	return &(*vi).second;
      }
    }
    return 0;
  }

private:
  struct VariableMap : public std::map<std::string, Variable> {
  };

  std::vector<VariableMap> frames_;
};


/* The lexer. */
extern int yylex();
/* Current line number. */
extern size_t line_number;
/* Name of current file. */
extern std::string current_file;
/* Level of warnings. */
extern int warning_level;

/* Whether the last parsing attempt succeeded. */
static bool success = true;
/* Current domain. */
static Domain* domain;
/* Domains. */
static std::map<std::string, Domain*> domains;
/* Problem being parsed, or 0 if no problem is being parsed. */
static Problem* problem;
/* Current requirements. */
static Requirements* requirements;
/* Predicate being parsed. */
static const Predicate* predicate;
/* Whether predicate declaration is repeated. */
static bool repeated_predicate;
/* Function being parsed. */
static const Function* function;
/* Whether function declaration is repeated. */
static bool repeated_function;
/* Action being parsed, or 0 if no action is being parsed. */
static ActionSchema* action;
/* Time of current condition. */ 
static FormulaTime formula_time; 
/* Time of current effect. */
static Effect::EffectTime effect_time;
/* Condition for effect being parsed, or 0 if unconditional effect. */
static const Formula* effect_condition; 
/* Current variable context. */
static Context context;
/* Predicate for atomic formula being parsed. */
static const Predicate* atom_predicate;
/* Whether the predicate of the currently parsed atom was undeclared. */
static bool undeclared_atom_predicate;
/* Whether parsing metric fluent. */
static bool metric_fluent;
/* Function for fluent being parsed. */
static const Function* fluent_function;
/* Whether the function of the currently parsed fluent was undeclared. */
static bool undeclared_fluent_function;
/* Paramerers for atomic formula or fluent being parsed. */
static std::vector<Term> term_parameters;
/* Quantified variables for effect or formula being parsed. */
static std::vector<Term> quantified;
/* Kind of name map being parsed. */
static enum { TYPE_KIND, CONSTANT_KIND, OBJECT_KIND, VOID_KIND } name_kind;

/* Outputs an error message. */
static void yyerror(const std::string& s); 
/* Outputs a warning message. */
static void yywarning(const std::string& s);
/* Creates an empty domain with the given name. */
static void make_domain(const std::string* name);
/* Creates an empty problem with the given name. */
static void make_problem(const std::string* name,
			 const std::string* domain_name);
/* Adds :typing to the requirements. */
static void require_typing();
/* Adds :fluents to the requirements. */
static void require_fluents();
/* Adds :disjunctive-preconditions to the requirements. */
static void require_disjunction();
/* Adds :duration-inequalities to the requirements. */
static void require_duration_inequalities();
/* Returns a simple type with the given name. */
static const Type& make_type(const std::string* name);
/* Returns the union of the given types. */
static Type make_type(const std::set<Type>& types);
/* Returns a simple term with the given name. */
static Term make_term(const std::string* name);
/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name);
/* Creates a function with the given name. */
static void make_function(const std::string* name);
/* Creates an action with the given name. */
static void make_action(const std::string* name, bool durative);
/* Adds the current action to the current domain. */ 
static void add_action();
/* Prepares for the parsing of a universally quantified effect. */ 
static void prepare_forall_effect();
/* Prepares for the parsing of a conditional effect. */ 
static void prepare_conditional_effect(const Formula& condition);
/* Adds types, constants, or objects to the current domain or problem. */
static void add_names(const std::vector<const std::string*>* names,
		      const Type& type);
/* Adds variables to the current variable list. */
static void add_variables(const std::vector<const std::string*>* names,
			  const Type& type);
/* Prepares for the parsing of an atomic formula. */ 
static void prepare_atom(const std::string* name);
/* Prepares for the parsing of a fluent. */ 
static void prepare_fluent(const std::string* name);
/* Adds a term with the given name to the current atomic formula. */
static void add_term(const std::string* name);
/* Creates the atomic formula just parsed. */
static const Atom* make_atom();
/* Creates the fluent just parsed. */
static const Fluent* make_fluent();
/* Creates a subtraction. */
static const Expression* make_subtraction(const Expression& term,
					  const Expression* opt_term);
/* Creates an equality formula. */
static const Formula* make_equality(const Term* term1, const Term* term2);
/* Creates a negation. */
static const Formula* make_negation(const Formula& negand);
/* Prepares for the parsing of an existentially quantified formula. */
static void prepare_exists();
/* Prepares for the parsing of a universally quantified formula. */
static void prepare_forall();
/* Creates an existentially quantified formula. */
static const Formula* make_exists(const Formula& body);
/* Creates a universally quantified formula. */
static const Formula* make_forall(const Formula& body);
/* Adds the given literal as an effect to the currect action. */
static void add_effect(const Literal& literal);
/* Pops the top-most universally quantified variables. */
static void pop_forall_effect();
/* Adds a timed initial literal to the current problem. */
static void add_init_literal(float time, const Literal& literal);
%}

%token DEFINE DOMAIN_TOKEN PROBLEM
%token REQUIREMENTS TYPES CONSTANTS PREDICATES FUNCTIONS
%token STRIPS TYPING NEGATIVE_PRECONDITIONS DISJUNCTIVE_PRECONDITIONS EQUALITY
%token EXISTENTIAL_PRECONDITIONS UNIVERSAL_PRECONDITIONS
%token QUANTIFIED_PRECONDITIONS CONDITIONAL_EFFECTS FLUENTS ADL
%token DURATIVE_ACTIONS DURATION_INEQUALITIES CONTINUOUS_EFFECTS
%token TIMED_INITIAL_LITERALS
%token ACTION PARAMETERS PRECONDITION EFFECT
%token DURATIVE_ACTION DURATION CONDITION
%token PDOMAIN OBJECTS INIT GOAL METRIC
%token WHEN NOT AND OR IMPLY EXISTS FORALL
%token AT OVER START END ALL
%token MINIMIZE MAXIMIZE TOTAL_TIME
%token NUMBER_TOKEN OBJECT_TOKEN EITHER
%token LE GE NAME DURATION_VAR VARIABLE NUMBER
%token ILLEGAL_TOKEN

%union {
  const Formula* formula;
  const Literal* literal;
  const Atom* atom;
  const Expression* expr;
  const Fluent* fluent;
  const Term* term;
  const Type* type;
  std::set<Type>* types;
  const std::string* str;
  std::vector<const std::string*>* strs;
  float num;
}

%type <formula> da_gd timed_gd timed_gds formula conjuncts disjuncts
%type <literal> name_literal
%type <atom> atomic_name_formula atomic_term_formula
%type <expr> f_exp opt_f_exp ground_f_exp opt_ground_f_exp
%type <fluent> ground_f_head f_head
%type <term> term
%type <strs> name_seq variable_seq
%type <type> type_spec type
%type <types> types
%type <str> type_name predicate init_predicate function name variable
%type <str> DEFINE DOMAIN_TOKEN PROBLEM
%type <str> WHEN NOT AND OR IMPLY EXISTS FORALL
%type <str> NUMBER_TOKEN OBJECT_TOKEN EITHER
%type <str> AT OVER START END ALL
%type <str> MINIMIZE MAXIMIZE TOTAL_TIME
%type <str> NAME DURATION_VAR VARIABLE
%type <num> NUMBER

%%

pddl_file : { success = true; line_number = 1; } domains_and_problems
              { if (!success) YYERROR; }
          ;

domains_and_problems : /* empty */
                     | domains_and_problems domain_def
                     | domains_and_problems problem_def
                     ;


/* ====================================================================== */
/* Domain definitions. */

domain_def : '(' define '(' domain name ')' { make_domain($5); }
               domain_body ')'
           ;

domain_body : /* empty */
            | require_def
            | require_def domain_body2
            | domain_body2
            ;

domain_body2 : types_def
             | types_def domain_body3
             | domain_body3
             ;

domain_body3 : constants_def
             | predicates_def
             | functions_def
             | constants_def domain_body4
             | predicates_def domain_body5
             | functions_def domain_body6
             | structure_defs
             ;

domain_body4 : predicates_def
             | functions_def
             | predicates_def domain_body7
             | functions_def domain_body8
             | structure_defs
             ;

domain_body5 : constants_def
             | functions_def
             | constants_def domain_body7
             | functions_def domain_body9
             | structure_defs
             ;

domain_body6 : constants_def
             | predicates_def
             | constants_def domain_body8
             | predicates_def domain_body9
             | structure_defs
             ;

domain_body7 : functions_def 
             | functions_def structure_defs
             | structure_defs
             ;

domain_body8 : predicates_def
             | predicates_def structure_defs
             | structure_defs
             ;

domain_body9 : constants_def
             | constants_def structure_defs
             | structure_defs
             ;

structure_defs : structure_def
               | structure_defs structure_def
               ;

structure_def : action_def
              ;

require_def : '(' REQUIREMENTS require_keys ')'
            ;

require_keys : require_key
             | require_keys require_key
             ;

require_key : STRIPS { requirements->strips = true; }
            | TYPING { requirements->typing = true; }
            | NEGATIVE_PRECONDITIONS
                { requirements->negative_preconditions = true; }
            | DISJUNCTIVE_PRECONDITIONS
                { requirements->disjunctive_preconditions = true; }
            | EQUALITY { requirements->equality = true; }
            | EXISTENTIAL_PRECONDITIONS
                { requirements->existential_preconditions = true; }
            | UNIVERSAL_PRECONDITIONS
                { requirements->universal_preconditions = true; }
            | QUANTIFIED_PRECONDITIONS
                { requirements->quantified_preconditions(); }
            | CONDITIONAL_EFFECTS { requirements->conditional_effects = true; }
            | FLUENTS { requirements->fluents = true; }
            | ADL { requirements->adl(); }
            | DURATIVE_ACTIONS { requirements->durative_actions = true; }
            | DURATION_INEQUALITIES
                { requirements->duration_inequalities = true; }
            | CONTINUOUS_EFFECTS
                { yyerror("`:continuous-effects' not supported"); }
            | TIMED_INITIAL_LITERALS
                {
		  requirements->durative_actions = true;
		  requirements->timed_initial_literals = true;
		}
            ;

types_def : '(' TYPES { require_typing(); name_kind = TYPE_KIND; }
              typed_names ')' { name_kind = VOID_KIND; }
          ;

constants_def : '(' CONSTANTS { name_kind = CONSTANT_KIND; } typed_names ')'
                  { name_kind = VOID_KIND; }
              ;

predicates_def : '(' PREDICATES predicate_decls ')'
               ;

functions_def : '(' FUNCTIONS { require_fluents(); } function_decls ')'
              ;


/* ====================================================================== */
/* Predicate and function declarations. */

predicate_decls : /* empty */
                | predicate_decls predicate_decl
                ;

predicate_decl : '(' predicate { make_predicate($2); } variables ')'
                   { predicate = 0; }
               ;

function_decls : /* empty */
               | function_decl_seq
               | function_decl_seq function_type_spec function_decls
               ;

function_decl_seq : function_decl
                  | function_decl_seq function_decl
                  ;

function_type_spec : '-' { require_typing(); } function_type
                   ;

function_decl : '(' function { make_function($2); } variables ')'
                  { function = 0; }
              ;


/* ====================================================================== */
/* Actions. */

action_def : '(' ACTION name { make_action($3, false); }
               parameters action_body ')' { add_action(); }
           | '(' DURATIVE_ACTION name { make_action($3, true); }
               parameters DURATION duration_constraint da_body ')'
               { add_action(); }
           ;

parameters : /* empty */
           | PARAMETERS '(' variables ')'
           ;

action_body : precondition action_body2
            | action_body2
            ;

action_body2 : /* empty */
             | effect
             ;

precondition : PRECONDITION { formula_time = AT_START; } formula
                 { action->set_condition(*$3); }
             ;

effect : EFFECT { effect_time = Effect::AT_END; } eff_formula
       ;

da_body : CONDITION da_gd da_body2 { action->set_condition(*$2); }
        | da_body2
        ;

da_body2 : /* empty */
         | EFFECT da_effect
         ;


/* ====================================================================== */
/* Duration constraints. */

duration_constraint : simple_duration_constraint
                    | '(' and simple_duration_constraints ')'
                        { require_duration_inequalities(); }
                    ;

simple_duration_constraint : '(' LE duration_var f_exp ')'
                               {
				 require_duration_inequalities();
				 action->set_max_duration(*$4);
			       }
                           | '(' GE duration_var f_exp ')'
                               {
				 require_duration_inequalities();
				 action->set_min_duration(*$4);
			       }
                           | '(' '=' duration_var f_exp ')'
                               { action->set_duration(*$4); }
                           ;

simple_duration_constraints : /* empty */
                            | simple_duration_constraints
                                simple_duration_constraint
                            ;


/* ====================================================================== */
/* Goals with time annotations. */

da_gd : timed_gd
      | '(' and timed_gds ')' { $$ = $3; }
      ;

timed_gds : /* empty */ { $$ = &Formula::TRUE; }
          | timed_gds timed_gd { $$ = &(*$1 && *$2); }
          ;

timed_gd : '(' at start { formula_time = AT_START; } formula ')' { $$ = $5; }
         | '(' at end { formula_time = AT_END; } formula ')' { $$ = $5; }
         | '(' over all { formula_time = OVER_ALL; } formula ')' { $$ = $5; }
         ;


/* ====================================================================== */
/* Effect formulas. */

eff_formula : term_literal
            | '(' and eff_formulas ')'
            | '(' forall { prepare_forall_effect(); }
                '(' variables ')' eff_formula ')' { pop_forall_effect(); }
            | '(' when { formula_time = AT_START; } formula
                { prepare_conditional_effect(*$4); }
                one_eff_formula ')' { effect_condition = 0; }
            ;

eff_formulas : /* empty */
             | eff_formulas eff_formula
             ;

one_eff_formula : term_literal
                | '(' and term_literals ')'
                ;

term_literal : atomic_term_formula { add_effect(*$1); }
             | '(' not atomic_term_formula ')'
                 { add_effect(Negation::make(*$3)); }
             ;

term_literals : /* empty */
              | term_literals term_literal
              ;

da_effect : timed_effect
          | '(' and da_effects ')'
          | '(' forall { prepare_forall_effect(); }
              '(' variables ')' da_effect ')' { pop_forall_effect(); }
          | '(' when da_gd { prepare_conditional_effect(*$3); }
              timed_effect ')' { effect_condition = 0; }
          ;

da_effects : /* empty */
           | da_effects da_effect
           ;

timed_effect : '(' at start
                 { effect_time = Effect::AT_START; formula_time = AT_START; }
                 a_effect ')'
             | '(' at end
                 { effect_time = Effect::AT_END; formula_time = AT_END; }
                 a_effect ')'
             ;

a_effect : term_literal
         | '(' and a_effects ')'
         | '(' forall { prepare_forall_effect(); }
             '(' variables ')' a_effect ')' { pop_forall_effect(); }
         | '(' when formula { prepare_conditional_effect(*$3); }
             one_eff_formula ')' { effect_condition = 0; }
         ;

a_effects : /* empty */
          | a_effects a_effect
          ;


/* ====================================================================== */
/* Problem definitions. */

problem_def : '(' define '(' problem name ')' '(' PDOMAIN name ')'
                { make_problem($5, $9); } problem_body ')'
                { delete requirements; }
            ;

problem_body : require_def problem_body2
             | problem_body2
             ;

problem_body2 : object_decl problem_body3
              | problem_body3
              ;

problem_body3 : init goal_spec
              | goal_spec
              ;

object_decl : '(' OBJECTS { name_kind = OBJECT_KIND; } typed_names ')'
                { name_kind = VOID_KIND; }
            ;

init : '(' INIT init_elements ')'
     ;

init_elements : /* empty */
              | init_elements init_element
              ;

init_element : '(' init_predicate { prepare_atom($2); } names ')'
                 { problem->add_init_atom(*make_atom()); }
             | '(' AT { prepare_atom($2); } names ')'
                 { problem->add_init_atom(*make_atom()); }
             | '(' not atomic_name_formula ')'
                 { Formula::register_use($3); Formula::unregister_use($3); }
             | '(' '=' ground_f_head NUMBER ')'
                 { problem->add_init_value(*$3, $4); }
             | '(' at NUMBER name_literal ')'
                 { add_init_literal($3, *$4); }
             ;

goal_spec : goal
          | goal metric_spec
          ;

goal : '(' GOAL formula ')' { problem->set_goal(*$3); }
     ;

metric_spec : '(' METRIC maximize { metric_fluent = true; } ground_f_exp ')'
                { problem->set_metric(*$5, true); metric_fluent = false; }
            | '(' METRIC minimize { metric_fluent = true; } ground_f_exp ')'
                { problem->set_metric(*$5); metric_fluent = false; }
            ;


/* ====================================================================== */
/* Formulas. */

formula : atomic_term_formula { $$ = &TimedLiteral::make(*$1, formula_time); }
        | '(' '=' term term ')' { $$ = make_equality($3, $4); }
        | '(' not formula ')' { $$ = make_negation(*$3); }
        | '(' and conjuncts ')' { $$ = $3; }
        | '(' or { require_disjunction(); } disjuncts ')' { $$ = $4; }
        | '(' imply { require_disjunction(); } formula formula ')'
            { $$ = &(!*$4 || *$5); }
        | '(' exists { prepare_exists(); } '(' variables ')' formula ')'
            { $$ = make_exists(*$7); }
        | '(' forall { prepare_forall(); } '(' variables ')' formula ')'
            { $$ = make_forall(*$7); }
        ;

conjuncts : /* empty */ { $$ = &Formula::TRUE; }
          | conjuncts formula { $$ = &(*$1 && *$2); }
          ;

disjuncts : /* empty */ { $$ = &Formula::FALSE; }
          | disjuncts formula { $$ = &(*$1 || *$2); }
          ;

atomic_term_formula : '(' predicate { prepare_atom($2); } terms ')'
                        { $$ = make_atom(); }
                    ;

atomic_name_formula : '(' predicate { prepare_atom($2); } names ')'
                        { $$ = make_atom(); }
                    ;

name_literal : atomic_name_formula { $$ = $1; }
             | '(' not atomic_name_formula ')' { $$ = &Negation::make(*$3); }
             ;


/* ====================================================================== */
/* Function expressions. */

f_exp : NUMBER { $$ = new Value($1); }
      | '(' '+' f_exp f_exp ')' { $$ = &Addition::make(*$3, *$4); }
      | '(' '-' f_exp opt_f_exp ')' { $$ = make_subtraction(*$3, $4); }
      | '(' '*' f_exp f_exp ')' { $$ = &Multiplication::make(*$3, *$4); }
      | '(' '/' f_exp f_exp ')' { $$ = &Division::make(*$3, *$4); }
      | f_head { $$ = $1; }
      ;

opt_f_exp : /* empty */ { $$ = 0; }
          | f_exp
          ;

f_head : '(' function { prepare_fluent($2); } terms ')'
           { $$ = make_fluent(); }
       | function { prepare_fluent($1); $$ = make_fluent(); }
       ;

ground_f_exp : NUMBER { $$ = new Value($1); }
             | '(' '+' ground_f_exp ground_f_exp ')'
                 { $$ = &Addition::make(*$3, *$4); }
             | '(' '-' ground_f_exp opt_ground_f_exp ')'
                 { $$ = make_subtraction(*$3, $4); }
             | '(' '*' ground_f_exp ground_f_exp ')'
                 { $$ = &Multiplication::make(*$3, *$4); }
             | '(' '/' ground_f_exp ground_f_exp ')'
                 { $$ = &Division::make(*$3, *$4); }
             | ground_f_head { $$ = $1; }
             ;

opt_ground_f_exp : /* empty */ { $$ = 0; }
                 | ground_f_exp
                 ;

ground_f_head : '(' function { prepare_fluent($2); } names ')'
                  { $$ = make_fluent(); }
              | function { prepare_fluent($1); $$ = make_fluent(); }
              ;


/* ====================================================================== */
/* Terms and types. */

terms : /* empty */
      | terms name { add_term($2); }
      | terms variable { add_term($2); }
      ;

names : /* empty */
      | names name { add_term($2); }
      ;

term : name { $$ = new Term(make_term($1)); }
     | variable { $$ = new Term(make_term($1)); }
     ;

variables : /* empty */
          | variable_seq { add_variables($1, TypeTable::OBJECT); }
          | variable_seq type_spec { add_variables($1, *$2); delete $2; }
              variables
          ;

variable_seq : variable { $$ = new std::vector<const std::string*>(1, $1); }
             | variable_seq variable { $$ = $1; $$->push_back($2); }
             ;

typed_names : /* empty */
            | name_seq { add_names($1, TypeTable::OBJECT); }
            | name_seq type_spec { add_names($1, *$2); delete $2; } typed_names
            ;

name_seq : name { $$ = new std::vector<const std::string*>(1, $1); }
         | name_seq name { $$ = $1; $$->push_back($2); }
         ;

type_spec : '-' { require_typing(); } type { $$ = $3; }
          ;

type : object { $$ = new Type(TypeTable::OBJECT); }
     | type_name { $$ = new Type(make_type($1)); }
     | '(' either types ')' { $$ = new Type(make_type(*$3)); delete $3; }
     ;

types : object { $$ = new std::set<Type>(); }
      | type_name { $$ = new std::set<Type>(); $$->insert(make_type($1)); }
      | types object { $$ = $1; }
      | types type_name { $$ = $1; $$->insert(make_type($2)); }
      ;

function_type : number
              ;


/* ====================================================================== */
/* Tokens. */

define : DEFINE { delete $1; }
       ;

domain : DOMAIN_TOKEN { delete $1; }
       ;

problem : PROBLEM { delete $1; }
        ;

when : WHEN { delete $1; }
     ;

not : NOT { delete $1; }
    ;

and : AND { delete $1; }
    ;

or : OR { delete $1; }
   ;

imply : IMPLY { delete $1; }
      ;

exists : EXISTS { delete $1; }
       ;

forall : FORALL { delete $1; }
       ;

at : AT { delete $1; }
   ;

over : OVER { delete $1; }
     ;

start : START { delete $1; }
      ;

end : END { delete $1; }
    ;

all : ALL { delete $1; }
    ;

duration_var : DURATION_VAR { delete $1; }
             ;

minimize : MINIMIZE { delete $1; }
         ;

maximize : MAXIMIZE { delete $1; }
         ;

number : NUMBER_TOKEN { delete $1; }
       ;

object : OBJECT_TOKEN { delete $1; }
       ;

either : EITHER { delete $1; }
       ;

type_name : DEFINE | DOMAIN_TOKEN | PROBLEM
          | EITHER
          | AT | OVER | START | END | ALL
          | MINIMIZE | MAXIMIZE | TOTAL_TIME
          | NAME
          ;

predicate : type_name
          | OBJECT_TOKEN | NUMBER_TOKEN
          ;

init_predicate : DEFINE | DOMAIN_TOKEN | PROBLEM
               | EITHER
               | OVER | START | END | ALL
               | MINIMIZE | MAXIMIZE | TOTAL_TIME
               | NAME
               | OBJECT_TOKEN | NUMBER_TOKEN
               ;

function : name
         ;

name : DEFINE | DOMAIN_TOKEN | PROBLEM
     | NUMBER_TOKEN | OBJECT_TOKEN | EITHER
     | WHEN | NOT | AND | OR | IMPLY | EXISTS | FORALL
     | AT | OVER | START | END | ALL
     | MINIMIZE | MAXIMIZE | TOTAL_TIME
     | NAME
     ;

variable : VARIABLE
         ;

%%

/* Outputs an error message. */
static void yyerror(const std::string& s) {
  std::cerr << PACKAGE ":" << current_file << ':' << line_number << ": " << s
	    << std::endl;
  success = false;
}


/* Outputs a warning. */
static void yywarning(const std::string& s) {
  if (warning_level > 0) {
    std::cerr << PACKAGE ":" << current_file << ':' << line_number << ": " << s
	      << std::endl;
    if (warning_level > 1) {
      success = false;
    }
  }
}


/* Creates an empty domain with the given name. */
static void make_domain(const std::string* name) {
  domain = new Domain(*name);
  domains[*name] = domain;
  requirements = &domain->requirements;
  problem = 0;
  delete name;
}


/* Creates an empty problem with the given name. */
static void make_problem(const std::string* name,
			 const std::string* domain_name) {
  std::map<std::string, Domain*>::const_iterator di =
    domains.find(*domain_name);
  if (di != domains.end()) {
    domain = (*di).second;
  } else {
    domain = new Domain(*domain_name);
    domains[*domain_name] = domain;
    yyerror("undeclared domain `" + *domain_name + "' used");
  }
  requirements = new Requirements(domain->requirements);
  problem = new Problem(*name, *domain);
  delete name;
  delete domain_name;
}


/* Adds :typing to the requirements. */
static void require_typing() {
  if (!requirements->typing) {
    yywarning("assuming `:typing' requirement");
    requirements->typing = true;
  }
}


/* Adds :fluents to the requirements. */
static void require_fluents() {
  if (!requirements->fluents) {
    yywarning("assuming `:fluents' requirement");
    requirements->fluents = true;
  }
}


/* Adds :disjunctive-preconditions to the requirements. */
static void require_disjunction() {
  if (!requirements->disjunctive_preconditions) {
    yywarning("assuming `:disjunctive-preconditions' requirement");
    requirements->disjunctive_preconditions = true;
  }
}


/* Adds :duration-inequalities to the requirements. */
static void require_duration_inequalities() {
  if (!requirements->duration_inequalities) {
    yywarning("assuming `:duration-inequalities' requirement");
    requirements->duration_inequalities = true;
  }
}


/* Returns a simple type with the given name. */
static const Type& make_type(const std::string* name) {
  const Type* t = domain->types().find_type(*name);
  if (t == 0) {
    t = &domain->types().add_type(*name);
    if (name_kind != TYPE_KIND) {
      yywarning("implicit declaration of type `" + *name + "'");
    }
  }
  delete name;
  return *t;
}


/* Returns the union of the given types. */
static Type make_type(const std::set<Type>& types) {
  return TypeTable::union_type(types);
}

/* Returns a simple term with the given name. */
static Term make_term(const std::string* name) {
  if ((*name)[0] == '?') {
    const Variable* vp = context.find(*name);
    if (vp != 0) {
      delete name;
      return *vp;
    } else {
      Variable v = TermTable::add_variable(TypeTable::OBJECT);
      context.insert(*name, v);
      yyerror("free variable `" + *name + "' used");
      delete name;
      return v;
    }
  } else {
    TermTable& terms = (problem != 0) ? problem->terms() : domain->terms();
    const Object* o = terms.find_object(*name);
    if (o == 0) {
      size_t n = term_parameters.size();
      if (atom_predicate != 0
	  && PredicateTable::parameters(*atom_predicate).size() > n) {
	const Type& t = PredicateTable::parameters(*atom_predicate)[n];
	o = &terms.add_object(*name, t);
      } else {
	o = &terms.add_object(*name, TypeTable::OBJECT);
      }
      yywarning("implicit declaration of object `" + *name + "'");
    }
    delete name;
    return *o;
  }
}


/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name) {
  predicate = domain->predicates().find_predicate(*name);
  if (predicate == 0) {
    repeated_predicate = false;
    predicate = &domain->predicates().add_predicate(*name);
  } else {
    repeated_predicate = true;
    yywarning("ignoring repeated declaration of predicate `" + *name + "'");
  }
  delete name;
}


/* Creates a function with the given name. */
static void make_function(const std::string* name) {
  repeated_function = false;
  function = domain->functions().find_function(*name);
  if (function == 0) {
    function = &domain->functions().add_function(*name);
  } else {
    repeated_function = true;
    if (*name == "total-time") {
      yywarning("ignoring declaration of reserved function `" + *name + "'");
    } else {
      yywarning("ignoring repeated declaration of function `" + *name + "'");
    }
  }
  delete name;
}


/* Creates an action with the given name. */
static void make_action(const std::string* name, bool durative) {
  if (durative) {
    if (!requirements->durative_actions) {
      yywarning("assuming `:durative-actions' requirement");
      requirements->durative_actions = true;
    }
  }
  context.push_frame();
  action = new ActionSchema(*name, durative);
  delete name;
}


/* Adds the current action to the current domain. */
static void add_action() {
  context.pop_frame();
  if (domain->find_action(action->name()) == 0) {
    action->strengthen_effects(*domain);
    domain->add_action(*action);
  } else {
    yywarning("ignoring repeated declaration of action `"
	      + action->name() + "'");
    delete action;
  }
  action = 0;
}


/* Prepares for the parsing of a universally quantified effect. */ 
static void prepare_forall_effect() {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
  context.push_frame();
  quantified.push_back(Term(0));
}


/* Prepares for the parsing of a conditional effect. */ 
static void prepare_conditional_effect(const Formula& condition) {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
  effect_condition = &condition;
}


/* Adds types, constants, or objects to the current domain or problem. */
static void add_names(const std::vector<const std::string*>* names,
		      const Type& type) {
  for (std::vector<const std::string*>::const_iterator si = names->begin();
       si != names->end(); si++) {
    const std::string* s = *si;
    if (name_kind == TYPE_KIND) {
      if (*s == TypeTable::OBJECT_NAME) {
	yywarning("ignoring declaration of reserved type `object'");
      } else if (*s == TypeTable::NUMBER_NAME) {
	yywarning("ignoring declaration of reserved type `number'");
      } else {
	const Type* t = domain->types().find_type(*s);
	if (t == 0) {
	  t = &domain->types().add_type(*s);
	}
	if (!TypeTable::add_supertype(*t, type)) {
	  yyerror("cyclic type hierarchy");
	}
      }
    } else if (name_kind == CONSTANT_KIND) {
      const Object* o = domain->terms().find_object(*s);
      if (o == 0) {
	domain->terms().add_object(*s, type);
      } else {
        std::set<Type> components;
	TypeTable::components(components, TermTable::type(*o));
	components.insert(type);
	TermTable::set_type(*o, make_type(components));
      }
    } else { /* name_kind == OBJECT_KIND */
      if (domain->terms().find_object(*s) != 0) {
	yywarning("ignoring declaration of object `" + *s
		  + "' previously declared as constant");
      } else {
	const Object* o = problem->terms().find_object(*s);
	if (o == 0) {
	  problem->terms().add_object(*s, type);
	} else {
          std::set<Type> components;
	  TypeTable::components(components, TermTable::type(*o));
	  components.insert(type);
	  TermTable::set_type(*o, make_type(components));
	}
      }
    }
    delete s;
  }
  delete names;
}


/* Adds variables to the current variable list. */
static void add_variables(const std::vector<const std::string*>* names,
			  const Type& type) {
  for (std::vector<const std::string*>::const_iterator si = names->begin();
       si != names->end(); si++) {
    const std::string* s = *si;
    if (predicate != 0) {
      if (!repeated_predicate) {
	PredicateTable::add_parameter(*predicate, type);
      }
    } else if (function != 0) {
      if (!repeated_function) {
	FunctionTable::add_parameter(*function, type);
      }
    } else {
      if (context.shallow_find(*s) != 0) {
	yyerror("repetition of parameter `" + *s + "'");
      } else if (context.find(*s) != 0) {
	yywarning("shadowing parameter `" + *s + "'");
      }
      Variable var = TermTable::add_variable(type);
      context.insert(*s, var);
      if (!quantified.empty()) {
	quantified.push_back(var);
      } else { /* action != 0 */
	action->add_parameter(var);
      }
    }
    delete s;
  }
  delete names;
}


/* Prepares for the parsing of an atomic formula. */ 
static void prepare_atom(const std::string* name) {
  atom_predicate = domain->predicates().find_predicate(*name);
  if (atom_predicate == 0) {
    atom_predicate = &domain->predicates().add_predicate(*name);
    undeclared_atom_predicate = true;
    if (problem != 0) {
      yywarning("undeclared predicate `" + *name + "' used");
    } else {
      yywarning("implicit declaration of predicate `" + *name + "'");
    }
  } else {
    undeclared_atom_predicate = false;
  }
  term_parameters.clear();
  delete name;
}


/* Prepares for the parsing of a fluent. */ 
static void prepare_fluent(const std::string* name) {
  fluent_function = domain->functions().find_function(*name);
  if (fluent_function == 0) {
    fluent_function = &domain->functions().add_function(*name);
    undeclared_fluent_function = true;
    if (problem != 0) {
      yywarning("undeclared function `" + *name + "' used");
    } else {
      yywarning("implicit declaration of function `" + *name + "'");
    }
  } else {
    undeclared_fluent_function = false;
  }
  if (*name == "total-time") {
    if (!metric_fluent) {
      yyerror("reserved function `" + *name + "' not allowed here");
    }
  } else {
    require_fluents();
  }
  term_parameters.clear();
  delete name;
}


/* Adds a term with the given name to the current atomic formula. */
static void add_term(const std::string* name) {
  Term term = make_term(name);
  if (atom_predicate != 0) {
    size_t n = term_parameters.size();
    if (undeclared_atom_predicate) {
      PredicateTable::add_parameter(*atom_predicate, TermTable::type(term));
    } else {
      const std::vector<Type>& params =
          PredicateTable::parameters(*atom_predicate);
      if (params.size() > n
	  && !TypeTable::subtype(TermTable::type(term), params[n])) {
	yyerror("type mismatch");
      }
    }
  } else if (fluent_function != 0) {
    size_t n = term_parameters.size();
    if (undeclared_fluent_function) {
      FunctionTable::add_parameter(*fluent_function, TermTable::type(term));
    } else {
      const std::vector<Type>& params =
          FunctionTable::parameters(*fluent_function);
      if (params.size() > n
	  && !TypeTable::subtype(TermTable::type(term), params[n])) {
	yyerror("type mismatch");
      }
    }
  }
  term_parameters.push_back(term);
}


/* Creates the atomic formula just parsed. */
static const Atom* make_atom() {
  size_t n = term_parameters.size();
  if (PredicateTable::parameters(*atom_predicate).size() < n) {
    yyerror("too many parameters passed to predicate `"
	    + PredicateTable::name(*atom_predicate) + "'");
  } else if (PredicateTable::parameters(*atom_predicate).size() > n) {
    yyerror("too few parameters passed to predicate `"
	    + PredicateTable::name(*atom_predicate) + "'");
  }
  const Atom& atom = Atom::make(*atom_predicate, term_parameters);
  atom_predicate = 0;
  return &atom;
}


/* Creates the fluent just parsed. */
static const Fluent* make_fluent() {
  size_t n = term_parameters.size();
  if (FunctionTable::parameters(*fluent_function).size() < n) {
    yyerror("too many parameters passed to function `"
	    + FunctionTable::name(*fluent_function) + "'");
  } else if (FunctionTable::parameters(*fluent_function).size() > n) {
    yyerror("too few parameters passed to function `"
	    + FunctionTable::name(*fluent_function) + "'");
  }
  const Fluent& fluent = Fluent::make(*fluent_function, term_parameters);
  fluent_function = 0;
  return &fluent;
}


/* Creates a subtraction. */
static const Expression* make_subtraction(const Expression& term,
					  const Expression* opt_term) {
  if (opt_term != 0) {
    return &Subtraction::make(term, *opt_term);
  } else {
    return &Subtraction::make(*new Value(0), term);
  }
}


/* Creates an equality formula. */
static const Formula* make_equality(const Term* term1, const Term* term2) {
  if (!requirements->equality) {
    yywarning("assuming `:equality' requirement");
    requirements->equality = true;
  }
  const Formula& eq = Equality::make(*term1, *term2);
  delete term1;
  delete term2;
  return &eq;
}


/* Creates a negated formula. */
static const Formula* make_negation(const Formula& negand) {
  if (typeid(negand) == typeid(Literal)
      || typeid(negand) == typeid(TimedLiteral)) {
    if (!requirements->negative_preconditions) {
      yywarning("assuming `:negative-preconditions' requirement");
      requirements->negative_preconditions = true;
    }
  } else if (!requirements->disjunctive_preconditions
	     && typeid(negand) != typeid(Equality)) {
    yywarning("assuming `:disjunctive-preconditions' requirement");
    requirements->disjunctive_preconditions = true;
  }
  return &!negand;
}


/* Prepares for the parsing of an existentially quantified formula. */
static void prepare_exists() {
  if (!requirements->existential_preconditions) {
    yywarning("assuming `:existential-preconditions' requirement");
    requirements->existential_preconditions = true;
  }
  context.push_frame();
  quantified.push_back(Term(0));
}


/* Prepares for the parsing of a universally quantified formula. */
static void prepare_forall() {
  if (!requirements->universal_preconditions) {
    yywarning("assuming `:universal-preconditions' requirement");
    requirements->universal_preconditions = true;
  }
  context.push_frame();
  quantified.push_back(Term(0));
}


/* Creates an existentially quantified formula. */
static const Formula* make_exists(const Formula& body) {
  context.pop_frame();
  size_t m = quantified.size() - 1;
  size_t n = m;
  while (quantified[n].variable()) {
    n--;
  }
  if (n < m) {
    if (body.tautology() || body.contradiction()) {
      quantified.resize(n, Term(0));
      return &body;
    } else {
      Exists& exists = *new Exists();
      for (size_t i = n + 1; i <= m; i++) {
	exists.add_parameter(quantified[i].as_variable());
      }
      exists.set_body(body);
      quantified.resize(n, Term(0));
      return &exists;
    }
  } else {
    quantified.pop_back();
    return &body;
  }
}


/* Creates a universally quantified formula. */
static const Formula* make_forall(const Formula& body) {
  context.pop_frame();
  size_t m = quantified.size() - 1;
  size_t n = m;
  while (quantified[n].variable()) {
    n--;
  }
  if (n < m) {
    if (body.tautology() || body.contradiction()) {
      quantified.resize(n, Term(0));
      return &body;
    } else {
      Forall& forall = *new Forall();
      for (size_t i = n + 1; i <= m; i++) {
	forall.add_parameter(quantified[i].as_variable());
      }
      forall.set_body(body);
      quantified.resize(n, Term(0));
      return &forall;
    }
  } else {
    quantified.pop_back();
    return &body;
  }
}


/* Adds the current effect to the currect action. */
static void add_effect(const Literal& literal) {
  PredicateTable::make_dynamic(literal.predicate());
  Effect* effect = new Effect(literal, effect_time);
  for (std::vector<Term>::const_iterator vi = quantified.begin();
       vi != quantified.end(); vi++) {
    if ((*vi).variable()) {
      effect->add_parameter((*vi).as_variable());
    }
  }
  if (effect_condition != 0) {
    effect->set_condition(*effect_condition);
  }
  action->add_effect(*effect);
}


/* Pops the top-most universally quantified variables. */
static void pop_forall_effect() {
  context.pop_frame();
  size_t n = quantified.size() - 1;
  while (quantified[n].variable()) {
    n--;
  }
  quantified.resize(n, Term(0));
}


/* Adds a timed initial literal to the current problem. */
static void add_init_literal(float time, const Literal& literal) {
  problem->add_init_literal(time, literal);
  if (time > 0.0f) {
    PredicateTable::make_dynamic(literal.predicate());
  }
}
