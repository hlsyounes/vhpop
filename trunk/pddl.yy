/* -*-C++-*- */
/*
 * PDDL parser.
 *
 * Copyright (C) 2002 Carnegie Mellon University
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
 * $Id: pddl.yy,v 3.1 2002-03-10 14:35:03 lorens Exp $
 */
%{
#include "requirements.h"
#include "problems.h"
#include "domains.h"
#include "formulas.h"
#include "types.h"
#include <typeinfo>
#include <utility>
#include <cstdlib>
#define __USE_ISOC99 1
#include <cmath>


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
static const EffectList* action_effs;
static pair<float, float> action_duration;
static Formula::FormulaTime formula_time;
static VariableList* variables;
static Context free_variables;
static const Effect* problem_init;
static const Formula* problem_goal;
static bool unique_variables = true;
%}

%token DEFINE DOMAIN PROBLEM
%token REQUIREMENTS
%token STRIPS TYPING NEGATIVE_PRECONDITIONS DISJUNCTIVE_PRECONDITIONS EQUALITY
%token EXISTENTIAL_PRECONDITIONS UNIVERSAL_PRECONDITIONS
%token QUANTIFIED_PRECONDITIONS CONDITIONAL_EFFECTS FLUENTS ADL
%token DURATIVE_ACTIONS DURATION_INEQUALITIES CONTINUOUS_EFFECTS
%token TYPES CONSTANTS PREDICATES
%token ACTION DURATIVE_ACTION DURATION PARAMETERS PRECONDITION CONDITION EFFECT
%token PDOMAIN OBJECTS INIT GOAL METRIC
%token WHEN NOT AND OR IMPLY EXISTS FORALL
%token EITHER
%token AT OVER START END ALL
%token MINIMIZE MAXIMIZE TOTAL_TIME
%token LE GE NAME DURATION_VAR VARIABLE NUMBER
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
  float num;
  vector<string>* strings;
  const Type* type;
}

%type <action> action_def
%type <formula> precondition da_gd timed_gd
%type <effects> effect eff_formula eff_formulas
%type <effects> da_effect da_effects timed_effect
%type <add_del_lists> one_eff_formula term_literals term_literal
%type <effect> init
%type <add_del_lists> name_literals name_literal
%type <terms> names
%type <formula> goals
%type <formulas> goal_list
%type <formula> goal
%type <formulas> timed_gds formulas
%type <formula> formula
%type <atom> atomic_term_formula atomic_name_formula
%type <terms> terms
%type <variables> opt_variables variables
%type <strings> name_seq variable_seq
%type <type> types
%type <type> type_spec type
%type <str> predicate function_symbol name NAME variable DURATION_VAR VARIABLE
%type <num> NUMBER
%type <str> DEFINE DOMAIN PROBLEM
%type <str> WHEN NOT AND OR IMPLY EXISTS FORALL EITHER
%type <str> AT OVER START END ALL
%type <str> MINIMIZE MAXIMIZE TOTAL_TIME

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

domain : '(' DEFINE '(' DOMAIN name ')'
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
            | require_def
            | require_def domain_body2
            | domain_body2
            ;

domain_body2 : types_def
             | types_def domain_body3
             | domain_body3
             ;

domain_body3 : predicates_def constants_def
             | predicates_def constants_def action_defs
             | domain_body4
             ;

domain_body4 : constants_def
             | constants_def domain_body5
             | domain_body5
             ;

domain_body5 : predicates_def
             | predicates_def action_defs
             | action_defs
             ;

action_defs : action_def             { add_action(*$1); }
            | action_defs action_def { add_action(*$2); }
            ;

require_def : '(' REQUIREMENTS require_keys ')'
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
            | NEGATIVE_PRECONDITIONS
                { requirements->negative_preconditions = true; }
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
            | FLUENTS
                { throw Unimplemented("`:fluents' not supported"); }
            | ADL
                { requirements->adl(); }
            | DURATIVE_ACTIONS
                { requirements->durative_actions = true; }
            | DURATION_INEQUALITIES
                { requirements->duration_inequalities = true; }
            | CONTINUOUS_EFFECTS
                { throw Unimplemented("`:continuous-effects' not supported"); }
            ;

types_def : '(' TYPES
              {
		if (!requirements->typing) {
		  domain_types->insert(make_pair(SimpleType::OBJECT.name,
						 &SimpleType::OBJECT));
		  yywarning("assuming `:typing' requirement");
		  requirements->typing = true;
		}
		name_map_type = "type";
	      }
            name_map ')'
          ;

constants_def : '(' CONSTANTS
                  {
		    name_map_type = "constant";
		    name_map = domain_constants;
		  }
                name_map ')'
              ;

predicates_def : '(' PREDICATES atomic_formula_skeletons ')'
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

action_def : '(' ACTION name
               {
		 context = (" in action `" + *$3 + "' of domain `"
			    + domain_name + "'");
		 free_variables.push_frame();
	       }
             PARAMETERS '(' opt_variables ')' action_body ')'
               {
		 free_variables.pop_frame();
		 $$ = new ActionSchema(*$3, *$7, *action_precond,
				       *action_effs);
	       }
           | '(' DURATIVE_ACTION name
               {
		 if (!requirements->durative_actions) {
		   yywarning("assuming `:durative-actions' requirement");
		   requirements->durative_actions = true;
		 }
		 context = (" in action `" + *$3 + "' of domain `"
			    + domain_name + "'");
		 free_variables.push_frame();
		 free_variables.insert(new Variable("?duration"));
		 action_duration = make_pair(0.0f, INFINITY);
	       }
             PARAMETERS '(' opt_variables ')' da_body ')'
               {
		 free_variables.pop_frame();
		 $$ = new ActionSchema(*$3, *$7, *action_precond,
				       *action_effs, action_duration.first,
				       action_duration.second);
	       }
           ;

action_body : precondition action_body2 { action_precond = $1; }
            | action_body2              {action_precond = &Formula::TRUE; }
            ;

action_body2 : /* empty */ { action_effs = &EffectList::EMPTY; }
             | effect      { action_effs = $1; }
            ;

precondition : PRECONDITION { formula_time = Formula::AT_START; }
               formula { $$ = $3; }
             ;

effect : EFFECT eff_formula { $$ = $2; }
       ;

eff_formula : term_literal
                {
		  $$ = new EffectList(new Effect(*$1->first, *$1->second,
						 Effect::AT_END));
		}
            | '(' AND eff_formulas ')'
                { $$ = $3; }
            | '(' FORALL
                {
		  if (!requirements->conditional_effects) {
		    yywarning("assuming `:conditional-effects' requirement");
		    requirements->conditional_effects = true;
		  }
		  free_variables.push_frame();
		}
              '(' opt_variables ')' eff_formula ')'
                {
		  free_variables.pop_frame();
		  if ($5->empty()) {
		    $$ = $7;
		  } else {
		    for (size_t i = 0; i < $7->size(); i++) {
		      const Effect& e = *(*$7)[i];
		      if (e.forall.empty()) {
			(*$7)[i] = new Effect(*$5, e.condition,
					      e.add_list, e.del_list, e.when);
		      } else {
			VariableList& forall = *(new VariableList(*$5));
			copy(e.forall.begin(), e.forall.end(),
			     back_inserter(forall));
			(*$7)[i] = new Effect(forall, e.condition,
					      e.add_list, e.del_list, e.when);
		      }
		    }
		    $$ = $7;
		  }
		}
            | '(' WHEN { formula_time = Formula::AT_START; }
              formula one_eff_formula ')'
                {
		  if (!requirements->conditional_effects) {
		    yywarning("assuming `:conditional-effects' requirement");
		    requirements->conditional_effects = true;
		  }
		  if ($4->tautology()) {
		    $$ = new EffectList(new Effect(*$5->first, *$5->second,
						   Effect::AT_END));
		  } else if ($4->contradiction()) {
		    $$ = new EffectList();
		  } else {
		    $$ = new EffectList(new Effect(*$4,
						   *$5->first, *$5->second,
						   Effect::AT_END));
		  }
		}
            ;

eff_formulas : /* empty */
                 { $$ = new EffectList(); }
             | eff_formulas eff_formula
                 { copy($2->begin(), $2->end(), back_inserter(*$1)); $$ = $1; }

one_eff_formula : term_literal
                | '(' AND term_literals ')'
                    { $$ = $3; }
                ;

term_literals : /* empty */
                  { $$ = &make_add_del(new AtomList(), new NegationList()); }
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

da_body : DURATION duration_constraint da_body2
        ;

da_body2 : CONDITION da_gd da_body3 { action_precond = $2; }
         | da_body3                 { action_precond = &Formula::TRUE; }
         ;

da_body3 : /* empty */      { action_effs = &EffectList::EMPTY; }
         | EFFECT da_effect { action_effs = $2; }
         ;

duration_constraint : simple_duration_constraint
                    | '(' AND simple_duration_constraints ')'
                        {
			  if (!requirements->duration_inequalities) {
			    yywarning("assuming `:duration-inequalities' "
				      "requirement");
			    requirements->duration_inequalities = true;
			  }
			}
                    ;

simple_duration_constraint : '(' LE DURATION_VAR NUMBER ')'
                               {
				 if (!requirements->duration_inequalities) {
				   yywarning("assuming "
					     "`:duration-inequalities' "
					     "requirement");
				   requirements->duration_inequalities = true;
				 }
				 if ($4 < action_duration.second) {
				   action_duration.second = $4;
				 }
			       }
                           | '(' GE DURATION_VAR NUMBER ')'
                               {
				 if (!requirements->duration_inequalities) {
				   yywarning("assuming "
					     "`:duration-inequalities' "
					     "requirement");
				   requirements->duration_inequalities = true;
				 }
				 if (action_duration.first < $4) {
				   action_duration.first = $4;
				 }
			       }
                           | '(' '=' DURATION_VAR NUMBER ')'
                               {
				 if (action_duration.first <= $4
				     && $4 <= action_duration.second) {
				   action_duration.first =
				     action_duration.second = $4;
				 } else {
				   action_duration = make_pair(1.0f, -1.0f);
				 }
			       }
                           ;

simple_duration_constraints : /* empty */
                            | simple_duration_constraints
                              simple_duration_constraint

da_gd : timed_gd
      | '(' AND timed_gds ')'
          {
	    $$ = &Formula::TRUE;
	    for (FormulaListIter fi = $3->begin(); fi != $3->end(); fi++) {
	      $$ = &(*$$ && **fi);
	    }
	  }
      ;

timed_gds : /* empty */        { $$ = new FormulaList(); }
          | timed_gds timed_gd { $1->push_back($2); $$ = $1; }
          ;

timed_gd : '(' AT START { formula_time = Formula::AT_START; } formula ')'
             { $$ = $5; }
         | '(' AT END { formula_time = Formula::AT_END; } formula ')'
             { $$ = $5; }
         | '(' OVER ALL { formula_time = Formula::OVER_ALL; } formula ')'
             { $$ = $5; }
         ;

da_effect : timed_effect
          | '(' AND da_effects ')'
              { $$ = $3; }
          | '(' FORALL
              {
		if (!requirements->conditional_effects) {
		  yywarning("assuming `:conditional-effects' requirement");
		  requirements->conditional_effects = true;
		}
		free_variables.push_frame();
	      }
            '(' opt_variables ')' da_effect ')'
              {
		free_variables.pop_frame();
		if ($5->empty()) {
		  $$ = $7;
		} else {
		  for (size_t i = 0; i < $7->size(); i++) {
		    const Effect& e = *(*$7)[i];
		    if (e.forall.empty()) {
		      (*$7)[i] = new Effect(*$5, e.condition,
					    e.add_list, e.del_list, e.when);
		    } else {
		      VariableList& forall = *(new VariableList(*$5));
		      copy(e.forall.begin(), e.forall.end(),
			   back_inserter(forall));
		      (*$7)[i] = new Effect(forall, e.condition,
					    e.add_list, e.del_list, e.when);
		    }
		  }
		  $$ = $7;
		}
	      }
          | '(' WHEN da_gd timed_effect ')'
              {
		if (!requirements->conditional_effects) {
		  yywarning("assuming `:conditional-effects' requirement");
		  requirements->conditional_effects = true;
		}
		if ($3->tautology()) {
		  $$ = $4;
		} else if ($3->contradiction()) {
		  $$ = new EffectList();
		} else {
		  for (size_t i = 0; i < $4->size(); i++) {
		    const Effect& e = *(*$4)[i];
		    (*$4)[i] = new Effect(e.forall, *$3,
					  e.add_list, e.del_list, e.when);
		  }
		  $$ = $4;
		}
	      }
          ;

da_effects : /* empty */
               { $$ = new EffectList(); }
           | da_effects da_effect
               { copy($2->begin(), $2->end(), back_inserter(*$1)); $$ = $1; }

timed_effect : '(' AT START one_eff_formula ')'
                 {
		   $$ = new EffectList(new Effect(*$4->first, *$4->second,
						  Effect::AT_START));
		 }
             | '(' AT END one_eff_formula ')'
                 {
		   $$ = new EffectList(new Effect(*$4->first, *$4->second,
						  Effect::AT_END));
		 }
             ;


/*
 * Problems
 */

problem : '(' DEFINE '(' PROBLEM name ')' 
            {
	      problem_name = *$5;
	      context = " in problem `" + problem_name + "'";
	    }
          '(' PDOMAIN name ')'
            {
	      pdomain = Domain::find(*$10);
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

problem_body : require_def problem_body2
             | problem_body2
             ;

problem_body2 : object_decl problem_body3
              | problem_body3
              ;

problem_body3 : init goals
                  { problem_init = $1; problem_goal = $2; }
              | goals
                  {
		    problem_init = new Effect(*(new AtomList()),
					      *(new NegationList()),
					      Effect::AT_END);
		    problem_goal = $1;
		  }
              ;

object_decl : '(' OBJECTS
                {
		  name_map_type = "object";
		  name_map = problem_objects;
		}
              name_map ')'
            ;

init : '(' INIT
         {
	   context =
	     " in initial conditions of problem `" + problem_name + "'";
	 }
       name_literals ')'
         { $$ = new Effect(*$4->first, *$4->second, Effect::AT_END); }
     ;

name_literals : name_literal
              | name_literals name_literal
                  {
		    copy($2->first->begin(), $2->first->end(),
			 back_inserter(*$1->first));
		    copy($2->second->begin(), $2->second->end(),
			 back_inserter(*$1->second));
		    $$ = $1;
		  }
              ;

name_literal : atomic_name_formula
                 { $$ = &make_add_del(new AtomList($1), new NegationList()); }
             | '(' NOT atomic_name_formula ')'
                 { $$ = &make_add_del(new AtomList(), new NegationList()); }
             ;

atomic_name_formula : '(' predicate
                        { current_predicate = *$2; }
                      names ')'
                        { $$ = &make_atom(*$2, *$4); }
                    ;

names : /* empty */
          { $$ = new TermList(); }
      | names name
          { $$ = &add_name(*$1, *$2); }
      ;

goals : goal_list metric_spec
          {
	    $$ = &Formula::TRUE;
	    for (FormulaListIter fi = $1->begin(); fi != $1->end(); fi++) {
	      $$ = &(*$$ && **fi);
	    }
	  }
      | goal_list
          {
	    $$ = &Formula::TRUE;
	    for (FormulaListIter fi = $1->begin(); fi != $1->end(); fi++) {
	      $$ = &(*$$ && **fi);
	    }
	  }
      ;

goal_list : goal { $$ = new FormulaList($1); }
          | goal_list goal { $1->push_back($2); $$ = $1; }
          ;

goal : '(' GOAL
         {
	   context = " in goal of problem `" + problem_name + "'";
	   formula_time = Formula::AT_START;
	 }
       formula ')'
         { $$ = $4; }
     ;

metric_spec : '(' METRIC optimization ground_f_exp ')'
            ;

optimization : MINIMIZE {}
             | MAXIMIZE {}
             ;

ground_f_exp : '(' '+' ground_f_exp ground_f_exp ')'
             | '(' '-' ground_f_exp ground_f_exp_opt ')'
             | '(' '*' ground_f_exp ground_f_exp ')'
             | '(' '/' ground_f_exp ground_f_exp ')'
             | NUMBER {}
             | '(' function_symbol names ')'
             | TOTAL_TIME {}
             | function_symbol {}
             ;

ground_f_exp_opt : /* empty */
                 | ground_f_exp
                 ;


/*
 * Formulas
 */

formulas : /* empty */      { $$ = new FormulaList(); }
         | formulas formula { $1->push_back($2); $$ = $1; }
         ;

formula : atomic_term_formula
            { $$ = $1 }
        | '(' '=' terms ')'
            {
	      if (!requirements->equality) {
		yywarning("assuming `:equality' requirement");
		requirements->equality = true;
	      }
	      if ($3->size() != 2) {
		yyerror(tostring($3->size()) + " parameter"
			+ string(($3->size() == 1) ? "" : "s")
			+ " passed to predicate `=' expecting 2");
	      }
	      const Term& t1 = *(*$3)[0];
	      const Term& t2 = *(*$3)[1];
	      if (t1.type.subtype(t2.type) || t2.type.subtype(t1.type)) {
		$$ = new Equality(t1, t2);
	      } else {
		$$ = &Formula::FALSE;
	      }
	    }
        | '(' NOT formula ')'
            {
	      if (typeid(*$3) == typeid(Atom)) {
		if (!requirements->negative_preconditions) {
		  yywarning("assuming `:negative-preconditions' "
			    "requirement");
		  requirements->negative_preconditions = true;
		}
	      } else if (!requirements->disjunctive_preconditions
			 && typeid(*$3) != typeid(Equality)) {
		yywarning("assuming `:disjunctive-preconditions' "
			  "requirement");
		requirements->disjunctive_preconditions = true;
	      }
	      $$ = &!*$3;
	    }
        | '(' AND formulas ')'
            {
	      $$ = &Formula::TRUE;
	      for (FormulaListIter fi = $3->begin(); fi != $3->end(); fi++) {
		$$ = &(*$$ && **fi);
	      }
	    }
        | '(' OR formulas ')'
            {
	      if (!requirements->disjunctive_preconditions) {
		yywarning("assuming `:disjunctive-preconditions' requirement");
		requirements->disjunctive_preconditions = true;
	      }
	      $$ = &Formula::FALSE;
	      for (FormulaListIter fi = $3->begin(); fi != $3->end(); fi++) {
		$$ = &(*$$ || **fi);
	      }
	    }
        | '(' IMPLY formula formula ')'
            {
	      if (!requirements->disjunctive_preconditions) {
		yywarning("assuming `:disjunctive-preconditions' requirement");
		requirements->disjunctive_preconditions = true;
	      }
	      $$ = &(!*$3 || *$4);
	    }
        | '(' EXISTS
            {
	      if (!requirements->existential_preconditions) {
		yywarning("assuming `:existential-preconditions' requirement");
		requirements->existential_preconditions = true;
	      }
	      free_variables.push_frame();
	    }
	  '(' opt_variables ')' formula ')'
            {
	      free_variables.pop_frame();
	      $$ = $5->empty() ? $7 : new ExistsFormula(*$5, *$7);
	    }
        | '(' FORALL
            {
	      if (!requirements->universal_preconditions) {
		yywarning("assuming `:universal-preconditions' requirement");
		requirements->universal_preconditions = true;
	      }
	      free_variables.push_frame();
	    }
	  '(' opt_variables ')' formula ')'
            {
	      free_variables.pop_frame();
	      $$ = $5->empty() ? $7 : new ForallFormula(*$5, *$7);
	    }
        ;

atomic_term_formula : '(' predicate
                        { current_predicate = *$2; }
                      terms ')'
                        { $$ = &make_atom(*$2, *$4); }
                    ;

terms : /* empty */
          { $$ = new TermList(); }
      | terms name
          { $$ = &add_name(*$1, *$2); }
      | terms variable
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

name_seq : name          { $$ = new vector<string>(1, *$1); }
         | name_seq name { $1->push_back(*$2); $$ = $1; }
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

variable_seq : variable              { $$ = new vector<string>(1, *$1); }
             | variable_seq variable { $1->push_back(*$2); $$ = $1; }
             ;

types : type
      | types type { $$ = &(*$1 + *$2); }
      ;

type_spec : '-' type { $$ = $2; }
          ;

type : predicate            { $$ = &make_type(*$1); }
     | '(' EITHER types ')' { $$ = $3; }
     ;

variable : DURATION_VAR
         | VARIABLE
         ;

predicate : NAME
          | DEFINE
          | DOMAIN
          | PROBLEM
          | AT
          | OVER
          | START
          | END
          | ALL
          | MINIMIZE
          | MAXIMIZE
          ;

function_symbol : NAME
                | DEFINE
                | DOMAIN
                | PROBLEM
                | WHEN
                | NOT
                | AND
                | OR
                | IMPLY
                | EXISTS
                | FORALL
                | EITHER
                | AT
                | OVER
                | START
                | END
                | ALL
                | MINIMIZE
                | MAXIMIZE
                | TOTAL_TIME
                ;

name : NAME
     | DEFINE
     | DOMAIN
     | PROBLEM
     | WHEN
     | NOT
     | AND
     | OR
     | IMPLY
     | EXISTS
     | FORALL
     | EITHER
     | AT
     | OVER
     | START
     | END
     | ALL
     | MINIMIZE
     | MAXIMIZE
     | TOTAL_TIME
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
    PredicateMapIter pi = domain_predicates->find(name);
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
  return *(new Atom(predicate, terms, formula_time));
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
