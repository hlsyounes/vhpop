/* -*-C++-*- */
/*
 * PDDL parser.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by H�kan L. S. Younes.
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
 * $Id: pddl.yy,v 6.1 2003-07-13 16:14:11 lorens Exp $
 */
%{
#include "requirements.h"
#include "problems.h"
#include "domains.h"
#include "formulas.h"
#include "types.h"
#include "exceptions.h"
#include "mathport.h"
#include <typeinfo>
#include <utility>
#include <cstdlib>
#include <iostream>


/* Workaround for bug in Bison 1.35 that disables stack growth. */
#define YYLTYPE_IS_TRIVIAL 1

/* The lexer. */
extern int yylex();
/* Current line number. */
extern size_t line_number;


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


/* Name of current file. */
std::string current_file; 
/* Level of warnings. */
int warning_level;

/* Whether the last parsing attempt succeeded. */
static bool success = true;
/* Domain being parsed, or NULL if no domain is being parsed. */
static Domain* domain;
/* Problem being parsed, or NULL if no problem is being parsed. */
static Problem* problem;
/* Domain of problem being parsed, or NULL if no problem is being parsed. */
static const Domain* pdomain;
/* Current requirements. */
static Requirements* requirements;
/* Predicate being parsed. */
static Predicate predicate;
/* Whether a predicate is being parsed. */
static bool parsing_predicate;
/* Whether predicate declaration is repeated. */
static bool repeated_predicate;
/* Action being parsed, or NULL if no action is being parsed. */
static ActionSchema* action;
/* Effect being parsed, or NULL if no effect is being parsed. */
static Effect* effect;
/* Condition for effect being parsed, or NULL if unconditional effect. */
static const Formula* effect_condition;
/* Quantified variables for effect or formula being parsed. */
VariableList quantified;
/* Atom being parsed, or NULL if no atom is being parsed. */
static Atom* atom;
/* Time stap of current formula. */
static Formula::FormulaTime formula_time;
/* Current variable context. */
static Context context;
/* Kind of name map being parsed. */
static enum { TYPE_MAP, CONSTANT_MAP, OBJECT_MAP, NOTHING } name_map_kind;

/* Outputs an error message. */
static void yyerror(const std::string& s); 
/* Outputs a warning message. */
static void yywarning(const std::string& s);
/* Returns the type with the given name.  If no type with the given
   name exists, false is returned in the second part of the
   result. */
static std::pair<Type, bool> find_type(const std::string& name);
/* Returns the predicate with the given name.  If no predicate with
   the given name exists, false is returned in the second part of the
   result. */
static std::pair<Predicate, bool> find_predicate(const std::string& name);
/* Returns the constant with the given name, or NULL if undefined. */
static std::pair<Object, bool> find_constant(const std::string& name);
/* Creates an empty domain with the given name. */
static void make_domain(const std::string* name);
/* Creates an empty problem with the given name. */
static void make_problem(const std::string* name,
			 const std::string* domain_name);
/* Adds :typing to the requirements. */
static void require_typing();
/* Adds :duration-inequalities to the requirements. */
static void require_duration_inequalities();
/* Returns a simple type with the given name. */
static Type make_type(const std::string* name);
/* Returns the union of the given types. */
static Type make_type(const TypeSet& types);
/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name);
/* Returns a simple term with the given name. */
static Term make_term(const std::string* name);
/* Creates an action with the given name. */
static void make_action(const std::string* name, bool durative);
/* Prepares for the parsing of a universally quantified effect. */ 
static void prepare_forall_effect();
/* Prepares for the parsing of a conditional effect. */ 
static void prepare_conditional_effect(const Formula* condition);
/* Creates an equality formula. */
static const Formula* make_equality(const Term& t1, const Term& t2);
/* Creates a negation. */
static const Formula* make_negation(const Formula& f);
/* Prepares for the parsing of a disjunction. */
static void prepare_disjunction();
/* Prepares for the parsing of an existentially quantified formula. */
static void prepare_exists();
/* Prepares for the parsing of a universally quantified formula. */
static void prepare_forall();
/* Creates an existentially quantified formula. */
static const Formula* make_exists(const Formula& body);
/* Creates a universally quantified formula. */
static const Formula* make_forall(const Formula& body);
/* Adds types, constants, or objects to the current domain or problem. */
static void add_names(const std::vector<std::string>* names, Type type);
/* Adds variables to the current variable list. */
static void add_variables(const std::vector<std::string>* names, Type type);
/* Adds the current action to the current domain. */ 
static void add_action();
/* Adds the given atom to the add list of the current effect. */
static void add_positive(const Atom& atom);
/* Adds the given atom to the delete list of the current effect. */
static void add_negative(const Atom& atom);
/* Adds the current effect to the currect action. */
static void add_effect();
/* Adds the current universally quantified effect to the currect action. */
static void add_forall_effect();
/* Adds the current conditional effect to the currect action. */
static void add_conditional_effect();
/* Prepares for the parsning of an atomic formula. */ 
static void prepare_atom(const std::string* name);
/* Adds a term with the given name to the current atomic formula. */
static void add_term(const std::string* name);
/* Creates the atomic formula just parsed. */
static const Atom* make_atom();
%}

%token DEFINE DOMAIN_TOKEN PROBLEM
%token REQUIREMENTS
%token STRIPS TYPING NEGATIVE_PRECONDITIONS DISJUNCTIVE_PRECONDITIONS EQUALITY
%token EXISTENTIAL_PRECONDITIONS UNIVERSAL_PRECONDITIONS
%token QUANTIFIED_PRECONDITIONS CONDITIONAL_EFFECTS FLUENTS ADL
%token DURATIVE_ACTIONS DURATION_INEQUALITIES CONTINUOUS_EFFECTS
%token TYPES CONSTANTS PREDICATES
%token ACTION DURATIVE_ACTION DURATION PARAMETERS PRECONDITION CONDITION EFFECT
%token PDOMAIN OBJECTS INIT GOAL METRIC
%token WHEN NOT AND OR IMPLY EXISTS FORALL
%token OBJECT_TOKEN EITHER
%token AT OVER START END ALL
%token MINIMIZE MAXIMIZE TOTAL_TIME
%token LE GE NAME DURATION_VAR VARIABLE NUMBER
%token ILLEGAL_TOKEN

%union {
  const Formula* formula;
  const Atom* atom;
  const std::string* str;
  std::vector<std::string>* strs;
  Term term;
  Type type;
  TypeSet* types;
  float num;
}

%type <formula> da_gd timed_gd timed_gds
%type <formula> formula conjuncts disjuncts
%type <atom> atomic_term_formula atomic_name_formula
%type <strs> name_seq variable_seq
%type <term> term
%type <type> type_spec type
%type <types> types
%type <str> type_name predicate function_symbol name variable
%type <str> DEFINE DOMAIN_TOKEN PROBLEM
%type <str> WHEN NOT AND OR IMPLY EXISTS FORALL
%type <str> OBJECT_TOKEN EITHER
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
/* Domains. */

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

domain_body3 : predicates_def constants_def
             | predicates_def constants_def actions_def
             | domain_body4
             ;

domain_body4 : constants_def
             | constants_def domain_body5
             | domain_body5
             ;

domain_body5 : predicates_def
             | predicates_def actions_def
             | actions_def
             ;

actions_def : action_def
            | actions_def action_def
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
            | FLUENTS { throw Exception("`:fluents' not supported"); }
            | ADL { requirements->adl(); }
            | DURATIVE_ACTIONS { requirements->durative_actions = true; }
            | DURATION_INEQUALITIES
                { requirements->duration_inequalities = true; }
            | CONTINUOUS_EFFECTS
                { throw Exception("`:continuous-effects' not supported"); }
            ;

types_def : '(' TYPES { require_typing(); } name_map ')'
              { name_map_kind = NOTHING; }
          ;

constants_def : '(' CONSTANTS { name_map_kind = CONSTANT_MAP; } name_map ')'
                  { name_map_kind = NOTHING; }
              ;

predicates_def : '(' PREDICATES atomic_formula_skeletons ')'
               ;


/* ====================================================================== */
/* Predicates. */

atomic_formula_skeletons : atomic_formula_skeleton
                         | atomic_formula_skeletons atomic_formula_skeleton
                         ;

atomic_formula_skeleton : '(' predicate { make_predicate($2); }
                            opt_variables ')' { parsing_predicate = false; }
                        ;


/* ====================================================================== */
/* Actions. */

action_def : '(' ACTION name { make_action($3, false); }
               PARAMETERS '(' opt_variables ')' action_body ')'
               { add_action(); }
           | '(' DURATIVE_ACTION name { make_action($3, true); }
               PARAMETERS '(' opt_variables ')' DURATION duration_constraint
               da_body ')' { add_action(); }
           ;

action_body : precondition action_body2
            | action_body2
            ;

action_body2 : /* empty */
             | effect
             ;

precondition : PRECONDITION { formula_time = Formula::OVER_ALL; }
                 formula { action->set_precondition(*$3); }
             ;

effect : EFFECT eff_formula { add_effect(); }
       ;

da_body : CONDITION da_gd da_body2 { action->set_precondition(*$2); }
        | da_body2
        ;

da_body2 : /* empty */
         | EFFECT da_effect { add_effect(); }
         ;


/* ====================================================================== */
/* Duration constraints. */

duration_constraint : simple_duration_constraint
                    | '(' and simple_duration_constraints ')'
                        { require_duration_inequalities(); }
                    ;

simple_duration_constraint : '(' LE duration_var NUMBER ')'
                               {
				 require_duration_inequalities();
				 action->set_max_duration($4);
			       }
                           | '(' GE duration_var NUMBER ')'
                               {
				 require_duration_inequalities();
				 action->set_min_duration($4);
			       }
                           | '(' '=' duration_var NUMBER ')'
                               { action->set_duration($4); }
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

timed_gd : '(' at start { formula_time = Formula::AT_START; } formula ')'
             { $$ = $5; }
         | '(' at end { formula_time = Formula::AT_END; } formula ')'
             { $$ = $5; }
         | '(' over all { formula_time = Formula::OVER_ALL; } formula ')'
             { $$ = $5; }
         ;


/* ====================================================================== */
/* Effect formulas. */

eff_formula : term_literal
            | '(' and eff_formulas ')'
            | '(' forall { prepare_forall_effect(); }
                '(' opt_variables ')' eff_formula ')' { add_forall_effect(); }
            | '(' when { formula_time = Formula::OVER_ALL; }
                formula { prepare_conditional_effect($4); } one_eff_formula ')'
                { add_conditional_effect(); }
            ;

eff_formulas : /* empty */
             | eff_formulas eff_formula
             ;

one_eff_formula : term_literal
                | '(' and term_literals ')'
                ;

term_literal : atomic_term_formula { add_positive(*$1); }
             | '(' not atomic_term_formula ')' { add_negative(*$3); }
             ;

term_literals : /* empty */
              | term_literals term_literal
              ;

da_effect : timed_effect
          | '(' and da_effects ')'
          | '(' forall { prepare_forall_effect(); }
              '(' opt_variables ')' da_effect ')' { add_forall_effect(); }
          | '(' when da_gd { prepare_conditional_effect($3); } timed_effect ')'
              { add_conditional_effect(); }
          ;

da_effects : /* empty */
           | da_effects da_effect
           ;

timed_effect : '(' at start { formula_time = Formula::AT_START; }
                 one_eff_formula ')' { add_effect(); }
             | '(' at end { formula_time = Formula::AT_END; }
                 one_eff_formula ')' { add_effect(); }
             ;


/* ====================================================================== */
/* Problems. */

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

object_decl : '(' OBJECTS { name_map_kind = OBJECT_MAP; } name_map ')'
            ;

init : '(' INIT name_literals ')'
     ;

name_literals : name_literal
              | name_literals name_literal
              ;

name_literal : atomic_name_formula { problem->add_init(*$1); }
             | '(' not atomic_name_formula ')'
             ;

atomic_name_formula : '(' predicate { prepare_atom($2); } names ')'
                        { $$ = make_atom(); }
                    ;

names : /* empty */
      | names name { add_term($2); }
      ;

goal_spec : goal
          | goal metric_spec
          ;

goal : '(' GOAL { formula_time = Formula::AT_START; } formula ')'
         { problem->set_goal(*$4); }
     ;

metric_spec : '(' METRIC optimization ground_f_exp ')'
            ;

optimization : MINIMIZE { delete $1; }
             | MAXIMIZE { delete $1; }
             ;

ground_f_exp : '(' '+' ground_f_exp ground_f_exp ')'
             | '(' '-' ground_f_exp ground_f_exp_opt ')'
             | '(' '*' ground_f_exp ground_f_exp ')'
             | '(' '/' ground_f_exp ground_f_exp ')'
             | NUMBER {}
             | '(' function_symbol names ')' { delete $2; }
             | function_symbol { delete $1; }
             ;

ground_f_exp_opt : /* empty */
                 | ground_f_exp
                 ;


/* ====================================================================== */
/* Formulas. */

formula : atomic_term_formula { $$ = $1; }
        | '(' '=' term term ')' { $$ = make_equality($3, $4); }
        | '(' not formula ')' { $$ = make_negation(*$3); }
        | '(' and conjuncts ')' { $$ = $3; }
        | '(' or { prepare_disjunction(); } disjuncts ')' { $$ = $4; }
        | '(' imply { prepare_disjunction(); } formula formula ')'
            { $$ = &(!*$4 || *$5); }
        | '(' exists { prepare_exists(); } '(' opt_variables ')' formula ')'
            { $$ = make_exists(*$7); }
        | '(' forall { prepare_forall(); } '(' opt_variables ')' formula ')'
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


/* ====================================================================== */
/* Terms and types. */

terms : /* empty */
      | terms name {add_term($2); }
      | terms variable {add_term($2); }
      ;

term : name { $$ = make_term($1); }
     | variable { $$ = make_term($1); }
     ;

name_map : /* empty */
         | typed_names
	 ;

variables : variable_seq { add_variables($1, OBJECT_TYPE); }
          | variable_seq type_spec { add_variables($1, $2); } opt_variables
          ;

opt_variables : /* empty */
              | variables
              ;

variable_seq : variable
                 { $$ = new std::vector<std::string>(1, *$1); delete $1; }
             | variable_seq variable
                 { $$ = $1; $$->push_back(*$2); delete $2; }
             ;

typed_names : name_seq { add_names($1, OBJECT_TYPE); }
            | name_seq type_spec { add_names($1, $2); } opt_typed_names
            ;

opt_typed_names : /* empty */
                | typed_names
                ;

name_seq : name { $$ = new std::vector<std::string>(1, *$1); delete $1; }
         | name_seq name { $$ = $1; $$->push_back(*$2); delete $2; }
         ;

type_spec : '-' type { $$ = $2; }
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

object : OBJECT_TOKEN { delete $1; }
       ;

either : EITHER { delete $1; }
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

type_name : DEFINE | DOMAIN_TOKEN | PROBLEM
          | EITHER
          | AT | OVER | START | END | ALL
          | MINIMIZE | MAXIMIZE | TOTAL_TIME
          | NAME
          ;

predicate : type_name
          | OBJECT_TOKEN
          ;

function_symbol : name
                ;

name : DEFINE | DOMAIN_TOKEN | PROBLEM
     | OBJECT_TOKEN | EITHER
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


/* Returns the type with the given name.  If no type with the given
   name exists, false is returned in the second part of the
   result. */
static std::pair<Type, bool> find_type(const std::string& name) {
  if (domain != NULL) {
    return domain->types().find_type(name);
  } else { /* pdomain != NULL */
    return pdomain->types().find_type(name);
  }
}


/* Returns the predicate with the given name.  If no predicate with
   the given name exists, false is returned in the second part of the
   result. */
static std::pair<Predicate, bool> find_predicate(const std::string& name) {
  if (domain != NULL) {
    return domain->predicates().find_predicate(name);
  } else { /* pdomain != NULL */
    return pdomain->predicates().find_predicate(name);
  }
}


/* Returns the constant with the given name, or NULL if undefined.  */
static std::pair<Object, bool> find_constant(const std::string& name) {
  std::pair<Object, bool> c(0, false);
  if (pdomain != NULL) {
    c = pdomain->terms().find_object(name);
  }
  if (!c.second && domain != NULL) {
    c = domain->terms().find_object(name);
  }
  if (!c.second && problem != NULL) {
    c = problem->terms().find_object(name);
  }
  return c;
}


/* Creates an empty domain with the given name. */
static void make_domain(const std::string* name) {
  pdomain = NULL;
  problem = NULL;
  domain = new Domain(*name);
  requirements = &domain->requirements;
  delete name;
}


/* Creates an empty problem with the given name. */
static void make_problem(const std::string* name,
			 const std::string* domain_name) {
  domain = NULL;
  pdomain = Domain::find(*domain_name);
  if (pdomain != NULL) {
    requirements = new Requirements(pdomain->requirements);
  } else {
    pdomain = new Domain(*domain_name);
    requirements = new Requirements();
    yyerror("undeclared domain `" + *domain_name + "' used");
  }
  problem = new Problem(*name, *pdomain);
  delete name;
  delete domain_name;
}



/* Adds :typing to the requirements. */
static void require_typing() {
  if (!requirements->typing) {
    yywarning("assuming `:typing' requirement");
    requirements->typing = true;
  }
  name_map_kind = TYPE_MAP;
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
  std::pair<Type, bool> t = find_type(*name);
  if (!t.second) {
    if (domain != NULL) {
      t.first = domain->types().add_type(*name);
      if (name_map_kind != TYPE_MAP) {
	yywarning("implicit declaration of type `" + *name + "'");
      }
    } else {
      t.first = pdomain->types().add_type(*name);
      yyerror("undeclared type `" + *name + "' used");
    }
  }
  delete name;
  return t.first;
}


/* Returns the union of the given types. */
static Type make_type(const TypeSet& types) {
  const Domain& d = *((domain != NULL) ? domain : pdomain);
  return d.types().add_type(types);
}


/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name) {
  repeated_predicate = false;
  std::pair<Predicate, bool> p = find_predicate(*name);
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


/* Returns a simple term with the given name. */
static Term make_term(const std::string* name) {
  if ((*name)[0] == '?') {
    std::pair<Variable, bool> var = context.find(*name);
    if (!var.second) {
      if (domain != NULL) {
	var.first = domain->terms().add_variable(OBJECT_TYPE);
      } else {
	var.first = problem->terms().add_variable(OBJECT_TYPE);
      }
      context.insert(*name, var.first);
      yyerror("free variable `" + *name + "' used");
    }
    delete name;
    return var.first;
  } else {
    std::pair<Object, bool> c = find_constant(*name);
    if (!c.second) {
      size_t n = atom->terms().size();
      const Domain& d = *((domain != NULL) ? domain : pdomain);
      if (atom != NULL && d.predicates().arity(atom->predicate()) > n) {
	if (domain != NULL) {
	  c.first =
	    domain->terms().add_object(*name,
				       d.predicates().parameter(atom->predicate(), n));
	} else {
	  c.first =
	    problem->terms().add_object(*name,
				       d.predicates().parameter(atom->predicate(), n));
	}
      } else {
	if (domain != NULL) {
	  c.first = domain->terms().add_object(*name, OBJECT_TYPE);
	} else {
	  c.first = problem->terms().add_object(*name, OBJECT_TYPE);
	}
      }
    }
    delete name;
    return c.first;
  }
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


/* Prepares for the parsing of a universally quantified effect. */ 
static void prepare_forall_effect() {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
  add_effect();
  context.push_frame();
  quantified.push_back(NULL_TERM);
}


/* Prepares for the parsing of a conditional effect. */ 
static void prepare_conditional_effect(const Formula* condition) {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
  add_effect();
  effect_condition = condition;
}


/* Creates an equality formula. */
static const Formula* make_equality(const Term& t1, const Term& t2) {
  if (!requirements->equality) {
    yywarning("assuming `:equality' requirement");
    requirements->equality = true;
  }
  const Domain& d = *((domain != NULL) ? domain : pdomain);
  const TermTable& t = (domain != NULL) ? domain->terms() : problem->terms();
  if (d.types().subtype(t.type(t1), t.type(t2))
      || d.types().subtype(t.type(t2), t.type(t1))) {
    return new Equality(t1, t2);
  } else {
    return &Formula::FALSE;
  }
}


/* Creates a negation. */
static const Formula* make_negation(const Formula& f) {
  if (typeid(f) == typeid(Atom)) {
    if (!requirements->negative_preconditions) {
      yywarning("assuming `:negative-preconditions' requirement");
      requirements->negative_preconditions = true;
    }
  } else if (!requirements->disjunctive_preconditions
	     && typeid(f) != typeid(Equality)) {
    yywarning("assuming `:disjunctive-preconditions' requirement");
    requirements->disjunctive_preconditions = true;
  }
  return &!f;
}


/* Prepares for the parsing of a disjunction. */
static void prepare_disjunction() {
  if (!requirements->disjunctive_preconditions) {
    yywarning("assuming `:disjunctive-preconditions' requirement");
    requirements->disjunctive_preconditions = true;
  }
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
    ExistsFormula& exists = *(new ExistsFormula());
    for (size_t i = n + 1; i <= m; i++) {
      exists.add_parameter(quantified[i]);
    }
    exists.set_body(body);
    quantified.resize(n);
    return &exists;
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
    ForallFormula& forall = *(new ForallFormula());
    for (size_t i = n + 1; i <= m; i++) {
      forall.add_parameter(quantified[i]);
    }
    forall.set_body(body);
    quantified.resize(n);
    return &forall;
  } else {
    quantified.pop_back();
    return &body;
  }
}


/* Adds types, constants, or objects to the current domain or problem. */
static void add_names(const std::vector<std::string>* names, Type type) {
  for (std::vector<std::string>::const_iterator si = names->begin();
       si != names->end(); si++) {
    const std::string& s = *si;
    if (name_map_kind == TYPE_MAP) {
      if (s == OBJECT_NAME) {
	yywarning("ignoring declaration of reserved type `object'");
      } else {
	std::pair<Type, bool> t = domain->types().find_type(s);
	if (!t.second) {
	  t.first = domain->types().add_type(s);
	}
	if (!domain->types().add_supertype(t.first, type)) {
	  yyerror("cyclic type hierarchy");
	}
      }
    } else if (name_map_kind == CONSTANT_MAP) {
      std::pair<Object, bool> old_name = domain->terms().find_object(s);
      if (old_name.second) {
	TypeSet components;
	domain->types().components(components,
				   domain->terms().type(old_name.first));
	components.insert(type);
	domain->terms().set_type(old_name.first, make_type(components));
      } else {
	domain->terms().add_object(s, type);
      }
    } else { /* name_map_kind == OBJECT_MAP */
      if (pdomain->terms().find_object(s).second) {
	yywarning("ignoring declaration of object `" + s
		  + "' previously declared as constant");
      } else {
	std::pair<Object, bool> old_name = problem->terms().find_object(s);
	if (old_name.second) {
	  TypeSet components;
	  pdomain->types().components(components,
				      problem->terms().type(old_name.first));
	  components.insert(type);
	  problem->terms().set_type(old_name.first, make_type(components));
	} else {
	  problem->terms().add_object(s, type);
	}
      }
    }
  }
  delete names;
}


/* Adds variables to the current variable list. */
static void add_variables(const std::vector<std::string>* names, Type type) {
  for (std::vector<std::string>::const_iterator si = names->begin();
       si != names->end(); si++) {
    const std::string& s = *si;
    if (parsing_predicate) {
      if (!repeated_predicate) {
	domain->predicates().add_parameter(predicate, type);
      }
    } else {
      if (context.shallow_find(s).second) {
	yyerror("repetition of parameter `" + s + "'");
      } else if (context.find(s).second) {
	yyerror("shadowing parameter `" + s + "'");
      }
      Variable var;
      if (domain != NULL) {
	var = domain->terms().add_variable(type);
      } else {
	var = problem->terms().add_variable(type);
      }
      context.insert(s, var);
      if (!quantified.empty()) {
	quantified.push_back(var);
      } else if (action != NULL) {
	action->add_parameter(var);
      } else {
	yyerror("where do these variables go?");
      }
    }
  }
  delete names;
}


/* Adds the current action to the current domain. */
static void add_action() {
  context.pop_frame();
  if (domain->find_action(action->name()) == NULL) {
    action->strengthen_effects();
    domain->add_action(*action);
  } else {
    yywarning("ignoring repeated declaration of action `"
	      + action->name() + "'");
    delete action;
  }
  action = NULL;
}


/* Adds the given atom to the add list of the current effect. */
static void add_positive(const Atom& atom) {
  if (effect == NULL) {
    effect = new Effect(formula_time == Formula::AT_START
			? Effect::AT_START : Effect::AT_END);
  }
  effect->add_positive(atom);
}


/* Adds the given atom to the delete list of the current effect. */
static void add_negative(const Atom& atom) {
  if (effect == NULL) {
    effect = new Effect(formula_time == Formula::AT_START
			? Effect::AT_START : Effect::AT_END);
  }
  effect->add_negative(*(new Negation(atom)));
}


/* Adds the current effect to the currect action. */
static void add_effect() {
  if (effect != NULL) {
    for (VariableList::const_iterator vi = quantified.begin();
	 vi != quantified.end(); vi++) {
      if (is_variable(*vi)) {
	effect->add_forall(*vi);
      }
    }
    if (effect_condition != NULL) {
      effect->set_condition(*effect_condition);
    }
    action->add_effect(*effect);
    effect = NULL;
  }
}


/* Adds the current universally quantified effect to the currect action. */
static void add_forall_effect() {
  context.pop_frame();
  add_effect();
  size_t n = quantified.size() - 1;
  while (is_variable(quantified[n])) {
    n--;
  }
  quantified.resize(n);
}


/* Adds the current conditional effect to the currect action. */
static void add_conditional_effect() {
  add_effect();
  effect_condition = NULL;
}


/* Prepares for the parsning of an atomic formula. */ 
static void prepare_atom(const std::string* name) {
  std::pair<Predicate, bool> p = find_predicate(*name);
  if (!p.second) {
    if (domain != NULL) {
      p.first = domain->predicates().add_predicate(*name);
      yywarning("implicit declaration of predicate `" + *name + "'");
    } else {
      p.first = pdomain->predicates().add_predicate(*name);
      yyerror("undeclared predicate `" + *name + "' used");
    }
  }
  atom = new Atom(p.first, formula_time);
  delete name;
}


/* Adds a term with the given name to the current atomic formula. */
static void add_term(const std::string* name) {
  const Term& term = make_term(name);
  size_t n = atom->terms().size();
  const Domain& d = *((domain != NULL) ? domain : pdomain);
  const TermTable& t = (domain != NULL) ? domain->terms() : problem->terms();
  if (d.predicates().arity(atom->predicate()) > n
      && !d.types().subtype(t.type(term),
			    d.predicates().parameter(atom->predicate(), n))) {
    yyerror("type mismatch");
  }
  atom->add_term(term);
}


/* Creates the atomic formula just parsed. */
static const Atom* make_atom() {
  const Domain& d = *((domain != NULL) ? domain : pdomain);
  if (d.predicates().arity(atom->predicate()) < atom->terms().size()) {
    yyerror("too many parameters passed to predicate `"
	    + d.predicates().name(atom->predicate()) + "'");
  } else if (d.predicates().arity(atom->predicate()) > atom->terms().size()) {
    yyerror("too few parameters passed to predicate `"
	    + d.predicates().name(atom->predicate()) + "'");
  }
  const Atom* a = atom;
  atom = NULL;
  return a;
}
