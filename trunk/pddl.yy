/* -*-C++-*- */
/*
 * PDDL parser.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: pddl.yy,v 6.9 2003-12-05 23:17:07 lorens Exp $
 */
%{
#include "requirements.h"
#include "problems.h"
#include "domains.h"
#include "formulas.h"
#include "types.h"
#include "mathport.h"
#include <typeinfo>
#include <utility>
#include <cstdlib>
#include <iostream>


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

  void insert(const std::string& name, Variable v) {
    frames_.back()[name] = v;
  }

  std::pair<Variable, bool> shallow_find(const std::string& name) const {
    VariableMap::const_iterator vi = frames_.back().find(name);
    if (vi != frames_.back().end()) {
      return std::make_pair((*vi).second, true);
    } else {
      return std::make_pair(0, false);
    }
  }

  std::pair<Variable, bool> find(const std::string& name) const {
    for (std::vector<VariableMap>::const_reverse_iterator fi =
	   frames_.rbegin(); fi != frames_.rend(); fi++) {
      VariableMap::const_iterator vi = (*fi).find(name);
      if (vi != (*fi).end()) {
	return std::make_pair((*vi).second, true);
      }
    }
    return std::make_pair(0, false);
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
/* Problem being parsed, or NULL if no problem is being parsed. */
static Problem* problem;
/* Current requirements. */
static Requirements* requirements;
/* Predicate being parsed. */
static Predicate predicate;
/* Whether a predicate is being parsed. */
static bool parsing_predicate;
/* Whether predicate declaration is repeated. */
static bool repeated_predicate;
/* Function being parsed. */
static Function function;
/* Whether a function is being parsed. */
static bool parsing_function;
/* Whether function declaration is repeated. */
static bool repeated_function;
/* Action being parsed, or NULL if no action is being parsed. */
static ActionSchema* action;
/* Time of current effect. */
static Effect::EffectTime effect_time;
/* Condition for effect being parsed, or NULL if unconditional effect. */
static const Condition* effect_condition; 
/* Current variable context. */
static Context context;
/* Predicate for atomic formula being parsed. */
static Predicate atom_predicate;
/* Whether the predicate of the currently parsed atom was undeclared. */
static bool undeclared_atom_predicate;
/* Whether parsing metric fluent. */
static bool metric_fluent;
/* Function for function application being parsed. */
static Function appl_function;
/* Whether the function of the currently parsed application was undeclared. */
static bool undeclared_appl_function;
/* Paramerers for atomic formula or function application being parsed. */
static TermList term_parameters;
/* Whether parsing an atom. */
static bool parsing_atom;
/* Whether parsing a function application. */
static bool parsing_application;
/* Quantified variables for effect or formula being parsed. */
static VariableList quantified;
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
static Type make_type(const std::string* name);
/* Returns the union of the given types. */
static Type make_type(const TypeSet& types);
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
static void prepare_conditional_effect(const Condition& condition);
/* Adds types, constants, or objects to the current domain or problem. */
static void add_names(const std::vector<const std::string*>* names, Type type);
/* Adds variables to the current variable list. */
static void add_variables(const std::vector<const std::string*>* names,
			  Type type);
/* Prepares for the parsing of an atomic formula. */ 
static void prepare_atom(const std::string* name);
/* Prepares for the parsing of a function application. */ 
static void prepare_application(const std::string* name);
/* Adds a term with the given name to the current atomic formula. */
static void add_term(const std::string* name);
/* Creates the atomic formula just parsed. */
static const Atom* make_atom();
/* Creates the function application just parsed. */
static const Application* make_application();
/* Creates a subtraction. */
static const Expression* make_subtraction(const Expression& term,
					  const Expression* opt_term);
/* Creates an equality formula. */
static const Formula* make_equality(Term term1, Term term2);
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
  const Condition* condition;
  const Formula* formula;
  const Literal* literal;
  const Atom* atom;
  const Expression* expr;
  const Application* appl;
  Term term;
  Type type;
  TypeSet* types;
  const std::string* str;
  std::vector<const std::string*>* strs;
  float num;
}

%type <condition> da_gd timed_gd timed_gds
%type <formula> formula conjuncts disjuncts
%type <literal> name_literal
%type <atom> atomic_name_formula atomic_term_formula
%type <expr> f_exp opt_f_exp ground_f_exp opt_ground_f_exp
%type <appl> ground_f_head f_head
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
                   { parsing_predicate = false; }
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
                  { parsing_function = false; }
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

precondition : PRECONDITION formula
                 { action->set_condition(Condition::make(*$2, OVER_ALL)); }
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

timed_gds : /* empty */ { $$ = &Condition::TRUE; }
          | timed_gds timed_gd { $$ = &(*$1 && *$2); }
          ;

timed_gd : '(' at start formula ')'
             { $$ = &Condition::make(*$4, AT_START); }
         | '(' at end formula ')'
             { $$ = &Condition::make(*$4, AT_END); }
         | '(' over all formula ')'
             { $$ = &Condition::make(*$4, OVER_ALL); }
         ;


/* ====================================================================== */
/* Effect formulas. */

eff_formula : term_literal
            | '(' and eff_formulas ')'
            | '(' forall { prepare_forall_effect(); }
                '(' variables ')' eff_formula ')' { pop_forall_effect(); }
            | '(' when formula
                { prepare_conditional_effect(Condition::make(*$3, OVER_ALL)); }
                one_eff_formula ')' { effect_condition = NULL; }
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
              timed_effect ')' { effect_condition = NULL; }
          ;

da_effects : /* empty */
           | da_effects da_effect
           ;

timed_effect : '(' at start { effect_time = Effect::AT_START; }
                 one_eff_formula ')'
             | '(' at end { effect_time = Effect::AT_END; }
                 one_eff_formula ')'
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

formula : atomic_term_formula { $$ = $1; }
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

opt_f_exp : /* empty */ { $$ = NULL; }
          | f_exp
          ;

f_head : '(' function { prepare_application($2); } terms ')'
           { $$ = make_application(); }
       | function { prepare_application($1); $$ = make_application(); }
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

opt_ground_f_exp : /* empty */ { $$ = NULL; }
                 | ground_f_exp
                 ;

ground_f_head : '(' function { prepare_application($2); } names ')'
                  { $$ = make_application(); }
              | function { prepare_application($1); $$ = make_application(); }
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

term : name { $$ = make_term($1); }
     | variable { $$ = make_term($1); }
     ;

variables : /* empty */
          | variable_seq { add_variables($1, OBJECT_TYPE); }
          | variable_seq type_spec { add_variables($1, $2); } variables
          ;

variable_seq : variable { $$ = new std::vector<const std::string*>(1, $1); }
             | variable_seq variable { $$ = $1; $$->push_back($2); }
             ;

typed_names : /* empty */
            | name_seq { add_names($1, OBJECT_TYPE); }
            | name_seq type_spec { add_names($1, $2); } typed_names
            ;

name_seq : name { $$ = new std::vector<const std::string*>(1, $1); }
         | name_seq name { $$ = $1; $$->push_back($2); }
         ;

type_spec : '-' { require_typing(); } type { $$ = $3; }
          ;

type : object { $$ = OBJECT_TYPE; }
     | type_name { $$ = make_type($1); }
     | '(' either types ')' { $$ = make_type(*$3); delete $3; }
     ;

types : object { $$ = new TypeSet(); }
      | type_name { $$ = new TypeSet(); $$->insert(make_type($1)); }
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
  problem = NULL;
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
static Type make_type(const std::string* name) {
  std::pair<Type, bool> t = domain->types().find_type(*name);
  if (!t.second) {
    t.first = domain->types().add_type(*name);
    if (name_kind != TYPE_KIND) {
      yywarning("implicit declaration of type `" + *name + "'");
    }
  }
  delete name;
  return t.first;
}


/* Returns the union of the given types. */
static Type make_type(const TypeSet& types) {
  return domain->types().add_type(types);
}


/* Returns a simple term with the given name. */
static Term make_term(const std::string* name) {
  if ((*name)[0] == '?') {
    std::pair<Variable, bool> v = context.find(*name);
    if (!v.second) {
      if (problem != NULL) {
	v.first = problem->terms().add_variable(OBJECT_TYPE);
      } else {
	v.first = domain->terms().add_variable(OBJECT_TYPE);
      }
      context.insert(*name, v.first);
      yyerror("free variable `" + *name + "' used");
    }
    delete name;
    return v.first;
  } else {
    TermTable& terms = (problem != NULL) ? problem->terms() : domain->terms();
    const PredicateTable& predicates = domain->predicates();
    std::pair<Object, bool> o = terms.find_object(*name);
    if (!o.second) {
      size_t n = term_parameters.size();
      if (parsing_atom && predicates.arity(atom_predicate) > n) {
	o.first = terms.add_object(*name,
				   predicates.parameter(atom_predicate, n));
      } else {
	o.first = terms.add_object(*name, OBJECT_TYPE);
      }
    }
    delete name;
    return o.first;
  }
}


/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name) {
  repeated_predicate = false;
  std::pair<Predicate, bool> p = domain->predicates().find_predicate(*name);
  if (!p.second) {
    p.first = domain->predicates().add_predicate(*name);
  } else {
    repeated_predicate = true;
    yywarning("ignoring repeated declaration of predicate `" + *name + "'");
  }
  predicate = p.first;
  parsing_predicate = true;
  delete name;
}


/* Creates a function with the given name. */
static void make_function(const std::string* name) {
  if (*name == TOTAL_TIME_NAME) {
    yywarning("ignoring declaration of reserved function `total-time'");
    repeated_function = true;
    function = TOTAL_TIME_FUNCTION;
    parsing_function = true;
  } else {
    repeated_function = false;
    std::pair<Function, bool> f = domain->functions().find_function(*name);
    if (!f.second) {
      f.first = domain->functions().add_function(*name);
    } else {
      repeated_function = true;
      yywarning("ignoring repeated declaration of function `" + *name + "'");
    }
    function = f.first;
    parsing_function = true;
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
  if (domain->find_action(action->name()) == NULL) {
    action->strengthen_effects(*domain);
    domain->add_action(*action);
  } else {
    yywarning("ignoring repeated declaration of action `"
	      + action->name() + "'");
    delete action;
  }
  action = NULL;
}


/* Prepares for the parsing of a universally quantified effect. */ 
static void prepare_forall_effect() {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
  context.push_frame();
  quantified.push_back(NULL_TERM);
}


/* Prepares for the parsing of a conditional effect. */ 
static void prepare_conditional_effect(const Condition& condition) {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
  effect_condition = &condition;
}


/* Adds types, constants, or objects to the current domain or problem. */
static void add_names(const std::vector<const std::string*>* names,
		      Type type) {
  for (std::vector<const std::string*>::const_iterator si = names->begin();
       si != names->end(); si++) {
    const std::string* s = *si;
    if (name_kind == TYPE_KIND) {
      if (*s == OBJECT_NAME) {
	yywarning("ignoring declaration of reserved type `object'");
      } else if (*s == NUMBER_NAME) {
	yywarning("ignoring declaration of reserved type `number'");
      } else {
	std::pair<Type, bool> t = domain->types().find_type(*s);
	if (!t.second) {
	  t.first = domain->types().add_type(*s);
	}
	if (!domain->types().add_supertype(t.first, type)) {
	  yyerror("cyclic type hierarchy");
	}
      }
    } else if (name_kind == CONSTANT_KIND) {
      std::pair<Object, bool> o = domain->terms().find_object(*s);
      if (!o.second) {
	domain->terms().add_object(*s, type);
      } else {
	TypeSet components;
	domain->types().components(components, domain->terms().type(o.first));
	components.insert(type);
	domain->terms().set_type(o.first, make_type(components));
      }
    } else { /* name_kind == OBJECT_KIND */
      if (domain->terms().find_object(*s).second) {
	yywarning("ignoring declaration of object `" + *s
		  + "' previously declared as constant");
      } else {
	std::pair<Object, bool> o = problem->terms().find_object(*s);
	if (!o.second) {
	  problem->terms().add_object(*s, type);
	} else {
	  TypeSet components;
	  domain->types().components(components,
				     problem->terms().type(o.first));
	  components.insert(type);
	  problem->terms().set_type(o.first, make_type(components));
	}
      }
    }
    delete s;
  }
  delete names;
}


/* Adds variables to the current variable list. */
static void add_variables(const std::vector<const std::string*>* names,
			  Type type) {
  for (std::vector<const std::string*>::const_iterator si = names->begin();
       si != names->end(); si++) {
    const std::string* s = *si;
    if (parsing_predicate) {
      if (!repeated_predicate) {
	domain->predicates().add_parameter(predicate, type);
      }
    } else if (parsing_function) {
      if (!repeated_function) {
	domain->functions().add_parameter(function, type);
      }
    } else {
      if (context.shallow_find(*s).second) {
	yyerror("repetition of parameter `" + *s + "'");
      } else if (context.find(*s).second) {
	yywarning("shadowing parameter `" + *s + "'");
      }
      Variable var;
      if (problem != NULL) {
	var = problem->terms().add_variable(type);
      } else {
	var = domain->terms().add_variable(type);
      }
      context.insert(*s, var);
      if (!quantified.empty()) {
	quantified.push_back(var);
      } else { /* action != NULL */
	action->add_parameter(var);
      }
    }
    delete s;
  }
  delete names;
}


/* Prepares for the parsing of an atomic formula. */ 
static void prepare_atom(const std::string* name) {
  std::pair<Predicate, bool> p = domain->predicates().find_predicate(*name);
  if (!p.second) {
    atom_predicate = domain->predicates().add_predicate(*name);
    undeclared_atom_predicate = true;
    if (problem != NULL) {
      yywarning("undeclared predicate `" + *name + "' used");
    } else {
      yywarning("implicit declaration of predicate `" + *name + "'");
    }
  } else {
    atom_predicate = p.first;
    undeclared_atom_predicate = false;
  }
  term_parameters.clear();
  parsing_atom = true;
  delete name;
}


/* Prepares for the parsing of a function application. */ 
static void prepare_application(const std::string* name) {
  if (*name == TOTAL_TIME_NAME) {
    appl_function = TOTAL_TIME_FUNCTION;
    undeclared_appl_function = false;
    if (!metric_fluent) {
      yyerror("reserved function `total-time' not allowed here");
    }
  } else {
    std::pair<Function, bool> f = domain->functions().find_function(*name);
    if (!f.second) {
      appl_function = domain->functions().add_function(*name);
      undeclared_appl_function = true;
      if (problem != NULL) {
	yywarning("undeclared function `" + *name + "' used");
      } else {
	yywarning("implicit declaration of function `" + *name + "'");
      }
    } else {
      appl_function = f.first;
      undeclared_appl_function = false;
    }
    require_fluents();
  }
  term_parameters.clear();
  parsing_application = true;
  delete name;
}


/* Adds a term with the given name to the current atomic formula. */
static void add_term(const std::string* name) {
  Term term = make_term(name);
  const TermTable& terms =
    (problem != NULL) ? problem->terms() : domain->terms();
  if (parsing_atom) {
    PredicateTable& predicates = domain->predicates();
    size_t n = term_parameters.size();
    if (undeclared_atom_predicate) {
      predicates.add_parameter(atom_predicate, terms.type(term));
    } else if (predicates.arity(atom_predicate) > n
	       && !domain->types().subtype(terms.type(term),
					   predicates.parameter(atom_predicate,
								n))) {
      yyerror("type mismatch");
    }
  } else if (parsing_application) {
    FunctionTable& functions = domain->functions();
    size_t n = term_parameters.size();
    if (undeclared_appl_function) {
      functions.add_parameter(appl_function, terms.type(term));
    } else if (functions.arity(appl_function) > n
	       && !domain->types().subtype(terms.type(term),
					   functions.parameter(appl_function,
							       n))) {
      yyerror("type mismatch");
    }
  }
  term_parameters.push_back(term);
}


/* Creates the atomic formula just parsed. */
static const Atom* make_atom() {
  size_t n = term_parameters.size();
  if (domain->predicates().arity(atom_predicate) < n) {
    yyerror("too many parameters passed to predicate `"
	    + domain->predicates().name(atom_predicate) + "'");
  } else if (domain->predicates().arity(atom_predicate) > n) {
    yyerror("too few parameters passed to predicate `"
	    + domain->predicates().name(atom_predicate) + "'");
  }
  parsing_atom = false;
  return &Atom::make(atom_predicate, term_parameters);
}


/* Creates the function application just parsed. */
static const Application* make_application() {
  size_t n = term_parameters.size();
  if (domain->functions().arity(appl_function) < n) {
    yyerror("too many parameters passed to function `"
	    + domain->functions().name(appl_function) + "'");
  } else if (domain->functions().arity(appl_function) > n) {
    yyerror("too few parameters passed to function `"
	    + domain->functions().name(appl_function) + "'");
  }
  parsing_application = false;
  return &Application::make(appl_function, term_parameters);
}


/* Creates a subtraction. */
static const Expression* make_subtraction(const Expression& term,
					  const Expression* opt_term) {
  if (opt_term != NULL) {
    return &Subtraction::make(term, *opt_term);
  } else {
    return &Subtraction::make(*new Value(0), term);
  }
}


/* Creates an equality formula. */
static const Formula* make_equality(Term term1, Term term2) {
  if (!requirements->equality) {
    yywarning("assuming `:equality' requirement");
    requirements->equality = true;
  }
  const TermTable& terms =
    (problem != NULL) ? problem->terms() : domain->terms();
  if (domain->types().subtype(terms.type(term1), terms.type(term2))
      || domain->types().subtype(terms.type(term2), terms.type(term1))) {
    return &Equality::make(term1, term2);
  } else {
    return &Formula::FALSE;
  }
}


/* Creates a negated formula. */
static const Formula* make_negation(const Formula& negand) {
  if (typeid(negand) == typeid(Atom)) {
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
  quantified.push_back(NULL_TERM);
}


/* Prepares for the parsing of a universally quantified formula. */
static void prepare_forall() {
  if (!requirements->universal_preconditions) {
    yywarning("assuming `:universal-preconditions' requirement");
    requirements->universal_preconditions = true;
  }
  context.push_frame();
  quantified.push_back(NULL_TERM);
}


/* Creates an existentially quantified formula. */
static const Formula* make_exists(const Formula& body) {
  context.pop_frame();
  size_t m = quantified.size() - 1;
  size_t n = m;
  while (is_variable(quantified[n])) {
    n--;
  }
  if (n < m) {
    if (body.tautology() || body.contradiction()) {
      quantified.resize(n);
      return &body;
    } else {
      Exists& exists = *new Exists();
      for (size_t i = n + 1; i <= m; i++) {
	exists.add_parameter(quantified[i]);
      }
      exists.set_body(body);
      quantified.resize(n);
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
  while (is_variable(quantified[n])) {
    n--;
  }
  if (n < m) {
    if (body.tautology() || body.contradiction()) {
      quantified.resize(n);
      return &body;
    } else {
      Forall& forall = *new Forall();
      for (size_t i = n + 1; i <= m; i++) {
	forall.add_parameter(quantified[i]);
      }
      forall.set_body(body);
      quantified.resize(n);
      return &forall;
    }
  } else {
    quantified.pop_back();
    return &body;
  }
}


/* Adds the current effect to the currect action. */
static void add_effect(const Literal& literal) {
  domain->predicates().make_dynamic(literal.predicate());
  Effect* effect = new Effect(literal, effect_time);
  for (VariableList::const_iterator vi = quantified.begin();
       vi != quantified.end(); vi++) {
    if (is_variable(*vi)) {
      effect->add_parameter(*vi);
    }
  }
  if (effect_condition != NULL) {
    effect->set_condition(*effect_condition);
  }
  action->add_effect(*effect);
}


/* Pops the top-most universally quantified variables. */
static void pop_forall_effect() {
  context.pop_frame();
  size_t n = quantified.size() - 1;
  while (is_variable(quantified[n])) {
    n--;
  }
  quantified.resize(n);
}


/* Adds a timed initial literal to the current problem. */
static void add_init_literal(float time, const Literal& literal) {
  problem->add_init_literal(time, literal);
  if (time > 0.0f) {
    domain->predicates().make_dynamic(literal.predicate());
  }
}
