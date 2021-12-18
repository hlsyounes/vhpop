/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 23 "pddl.yy"

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

#line 262 "pddl.cc"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_PDDL_HH_INCLUDED
# define YY_YY_PDDL_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    REQUIREMENTS = 258,            /* REQUIREMENTS  */
    TYPES = 259,                   /* TYPES  */
    CONSTANTS = 260,               /* CONSTANTS  */
    PREDICATES = 261,              /* PREDICATES  */
    FUNCTIONS = 262,               /* FUNCTIONS  */
    STRIPS = 263,                  /* STRIPS  */
    TYPING = 264,                  /* TYPING  */
    NEGATIVE_PRECONDITIONS = 265,  /* NEGATIVE_PRECONDITIONS  */
    DISJUNCTIVE_PRECONDITIONS = 266, /* DISJUNCTIVE_PRECONDITIONS  */
    EQUALITY = 267,                /* EQUALITY  */
    EXISTENTIAL_PRECONDITIONS = 268, /* EXISTENTIAL_PRECONDITIONS  */
    UNIVERSAL_PRECONDITIONS = 269, /* UNIVERSAL_PRECONDITIONS  */
    QUANTIFIED_PRECONDITIONS = 270, /* QUANTIFIED_PRECONDITIONS  */
    CONDITIONAL_EFFECTS = 271,     /* CONDITIONAL_EFFECTS  */
    FLUENTS = 272,                 /* FLUENTS  */
    ADL = 273,                     /* ADL  */
    DURATIVE_ACTIONS = 274,        /* DURATIVE_ACTIONS  */
    DURATION_INEQUALITIES = 275,   /* DURATION_INEQUALITIES  */
    CONTINUOUS_EFFECTS = 276,      /* CONTINUOUS_EFFECTS  */
    TIMED_INITIAL_LITERALS = 277,  /* TIMED_INITIAL_LITERALS  */
    ACTION = 278,                  /* ACTION  */
    PARAMETERS = 279,              /* PARAMETERS  */
    PRECONDITION = 280,            /* PRECONDITION  */
    EFFECT = 281,                  /* EFFECT  */
    DURATIVE_ACTION = 282,         /* DURATIVE_ACTION  */
    DURATION = 283,                /* DURATION  */
    CONDITION = 284,               /* CONDITION  */
    PDOMAIN = 285,                 /* PDOMAIN  */
    OBJECTS = 286,                 /* OBJECTS  */
    INIT = 287,                    /* INIT  */
    GOAL = 288,                    /* GOAL  */
    METRIC = 289,                  /* METRIC  */
    LE = 290,                      /* LE  */
    GE = 291,                      /* GE  */
    ILLEGAL_TOKEN = 292,           /* ILLEGAL_TOKEN  */
    DEFINE = 293,                  /* DEFINE  */
    DOMAIN_TOKEN = 294,            /* DOMAIN_TOKEN  */
    PROBLEM = 295,                 /* PROBLEM  */
    WHEN = 296,                    /* WHEN  */
    NOT = 297,                     /* NOT  */
    AND = 298,                     /* AND  */
    OR = 299,                      /* OR  */
    IMPLY = 300,                   /* IMPLY  */
    EXISTS = 301,                  /* EXISTS  */
    FORALL = 302,                  /* FORALL  */
    AT = 303,                      /* AT  */
    OVER = 304,                    /* OVER  */
    START = 305,                   /* START  */
    END = 306,                     /* END  */
    ALL = 307,                     /* ALL  */
    MINIMIZE = 308,                /* MINIMIZE  */
    MAXIMIZE = 309,                /* MAXIMIZE  */
    TOTAL_TIME = 310,              /* TOTAL_TIME  */
    NUMBER_TOKEN = 311,            /* NUMBER_TOKEN  */
    OBJECT_TOKEN = 312,            /* OBJECT_TOKEN  */
    EITHER = 313,                  /* EITHER  */
    NAME = 314,                    /* NAME  */
    DURATION_VAR = 315,            /* DURATION_VAR  */
    VARIABLE = 316,                /* VARIABLE  */
    NUMBER = 317                   /* NUMBER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define REQUIREMENTS 258
#define TYPES 259
#define CONSTANTS 260
#define PREDICATES 261
#define FUNCTIONS 262
#define STRIPS 263
#define TYPING 264
#define NEGATIVE_PRECONDITIONS 265
#define DISJUNCTIVE_PRECONDITIONS 266
#define EQUALITY 267
#define EXISTENTIAL_PRECONDITIONS 268
#define UNIVERSAL_PRECONDITIONS 269
#define QUANTIFIED_PRECONDITIONS 270
#define CONDITIONAL_EFFECTS 271
#define FLUENTS 272
#define ADL 273
#define DURATIVE_ACTIONS 274
#define DURATION_INEQUALITIES 275
#define CONTINUOUS_EFFECTS 276
#define TIMED_INITIAL_LITERALS 277
#define ACTION 278
#define PARAMETERS 279
#define PRECONDITION 280
#define EFFECT 281
#define DURATIVE_ACTION 282
#define DURATION 283
#define CONDITION 284
#define PDOMAIN 285
#define OBJECTS 286
#define INIT 287
#define GOAL 288
#define METRIC 289
#define LE 290
#define GE 291
#define ILLEGAL_TOKEN 292
#define DEFINE 293
#define DOMAIN_TOKEN 294
#define PROBLEM 295
#define WHEN 296
#define NOT 297
#define AND 298
#define OR 299
#define IMPLY 300
#define EXISTS 301
#define FORALL 302
#define AT 303
#define OVER 304
#define START 305
#define END 306
#define ALL 307
#define MINIMIZE 308
#define MAXIMIZE 309
#define TOTAL_TIME 310
#define NUMBER_TOKEN 311
#define OBJECT_TOKEN 312
#define EITHER 313
#define NAME 314
#define DURATION_VAR 315
#define VARIABLE 316
#define NUMBER 317

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 226 "pddl.yy"

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

#line 453 "pddl.cc"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PDDL_HH_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_REQUIREMENTS = 3,               /* REQUIREMENTS  */
  YYSYMBOL_TYPES = 4,                      /* TYPES  */
  YYSYMBOL_CONSTANTS = 5,                  /* CONSTANTS  */
  YYSYMBOL_PREDICATES = 6,                 /* PREDICATES  */
  YYSYMBOL_FUNCTIONS = 7,                  /* FUNCTIONS  */
  YYSYMBOL_STRIPS = 8,                     /* STRIPS  */
  YYSYMBOL_TYPING = 9,                     /* TYPING  */
  YYSYMBOL_NEGATIVE_PRECONDITIONS = 10,    /* NEGATIVE_PRECONDITIONS  */
  YYSYMBOL_DISJUNCTIVE_PRECONDITIONS = 11, /* DISJUNCTIVE_PRECONDITIONS  */
  YYSYMBOL_EQUALITY = 12,                  /* EQUALITY  */
  YYSYMBOL_EXISTENTIAL_PRECONDITIONS = 13, /* EXISTENTIAL_PRECONDITIONS  */
  YYSYMBOL_UNIVERSAL_PRECONDITIONS = 14,   /* UNIVERSAL_PRECONDITIONS  */
  YYSYMBOL_QUANTIFIED_PRECONDITIONS = 15,  /* QUANTIFIED_PRECONDITIONS  */
  YYSYMBOL_CONDITIONAL_EFFECTS = 16,       /* CONDITIONAL_EFFECTS  */
  YYSYMBOL_FLUENTS = 17,                   /* FLUENTS  */
  YYSYMBOL_ADL = 18,                       /* ADL  */
  YYSYMBOL_DURATIVE_ACTIONS = 19,          /* DURATIVE_ACTIONS  */
  YYSYMBOL_DURATION_INEQUALITIES = 20,     /* DURATION_INEQUALITIES  */
  YYSYMBOL_CONTINUOUS_EFFECTS = 21,        /* CONTINUOUS_EFFECTS  */
  YYSYMBOL_TIMED_INITIAL_LITERALS = 22,    /* TIMED_INITIAL_LITERALS  */
  YYSYMBOL_ACTION = 23,                    /* ACTION  */
  YYSYMBOL_PARAMETERS = 24,                /* PARAMETERS  */
  YYSYMBOL_PRECONDITION = 25,              /* PRECONDITION  */
  YYSYMBOL_EFFECT = 26,                    /* EFFECT  */
  YYSYMBOL_DURATIVE_ACTION = 27,           /* DURATIVE_ACTION  */
  YYSYMBOL_DURATION = 28,                  /* DURATION  */
  YYSYMBOL_CONDITION = 29,                 /* CONDITION  */
  YYSYMBOL_PDOMAIN = 30,                   /* PDOMAIN  */
  YYSYMBOL_OBJECTS = 31,                   /* OBJECTS  */
  YYSYMBOL_INIT = 32,                      /* INIT  */
  YYSYMBOL_GOAL = 33,                      /* GOAL  */
  YYSYMBOL_METRIC = 34,                    /* METRIC  */
  YYSYMBOL_LE = 35,                        /* LE  */
  YYSYMBOL_GE = 36,                        /* GE  */
  YYSYMBOL_ILLEGAL_TOKEN = 37,             /* ILLEGAL_TOKEN  */
  YYSYMBOL_DEFINE = 38,                    /* DEFINE  */
  YYSYMBOL_DOMAIN_TOKEN = 39,              /* DOMAIN_TOKEN  */
  YYSYMBOL_PROBLEM = 40,                   /* PROBLEM  */
  YYSYMBOL_WHEN = 41,                      /* WHEN  */
  YYSYMBOL_NOT = 42,                       /* NOT  */
  YYSYMBOL_AND = 43,                       /* AND  */
  YYSYMBOL_OR = 44,                        /* OR  */
  YYSYMBOL_IMPLY = 45,                     /* IMPLY  */
  YYSYMBOL_EXISTS = 46,                    /* EXISTS  */
  YYSYMBOL_FORALL = 47,                    /* FORALL  */
  YYSYMBOL_AT = 48,                        /* AT  */
  YYSYMBOL_OVER = 49,                      /* OVER  */
  YYSYMBOL_START = 50,                     /* START  */
  YYSYMBOL_END = 51,                       /* END  */
  YYSYMBOL_ALL = 52,                       /* ALL  */
  YYSYMBOL_MINIMIZE = 53,                  /* MINIMIZE  */
  YYSYMBOL_MAXIMIZE = 54,                  /* MAXIMIZE  */
  YYSYMBOL_TOTAL_TIME = 55,                /* TOTAL_TIME  */
  YYSYMBOL_NUMBER_TOKEN = 56,              /* NUMBER_TOKEN  */
  YYSYMBOL_OBJECT_TOKEN = 57,              /* OBJECT_TOKEN  */
  YYSYMBOL_EITHER = 58,                    /* EITHER  */
  YYSYMBOL_NAME = 59,                      /* NAME  */
  YYSYMBOL_DURATION_VAR = 60,              /* DURATION_VAR  */
  YYSYMBOL_VARIABLE = 61,                  /* VARIABLE  */
  YYSYMBOL_NUMBER = 62,                    /* NUMBER  */
  YYSYMBOL_63_ = 63,                       /* '('  */
  YYSYMBOL_64_ = 64,                       /* ')'  */
  YYSYMBOL_65_ = 65,                       /* '-'  */
  YYSYMBOL_66_ = 66,                       /* '='  */
  YYSYMBOL_67_ = 67,                       /* '+'  */
  YYSYMBOL_68_ = 68,                       /* '*'  */
  YYSYMBOL_69_ = 69,                       /* '/'  */
  YYSYMBOL_YYACCEPT = 70,                  /* $accept  */
  YYSYMBOL_pddl_file = 71,                 /* pddl_file  */
  YYSYMBOL_72_1 = 72,                      /* $@1  */
  YYSYMBOL_domains_and_problems = 73,      /* domains_and_problems  */
  YYSYMBOL_domain_def = 74,                /* domain_def  */
  YYSYMBOL_75_2 = 75,                      /* $@2  */
  YYSYMBOL_domain_body = 76,               /* domain_body  */
  YYSYMBOL_domain_body2 = 77,              /* domain_body2  */
  YYSYMBOL_domain_body3 = 78,              /* domain_body3  */
  YYSYMBOL_domain_body4 = 79,              /* domain_body4  */
  YYSYMBOL_domain_body5 = 80,              /* domain_body5  */
  YYSYMBOL_domain_body6 = 81,              /* domain_body6  */
  YYSYMBOL_domain_body7 = 82,              /* domain_body7  */
  YYSYMBOL_domain_body8 = 83,              /* domain_body8  */
  YYSYMBOL_domain_body9 = 84,              /* domain_body9  */
  YYSYMBOL_structure_defs = 85,            /* structure_defs  */
  YYSYMBOL_structure_def = 86,             /* structure_def  */
  YYSYMBOL_require_def = 87,               /* require_def  */
  YYSYMBOL_require_keys = 88,              /* require_keys  */
  YYSYMBOL_require_key = 89,               /* require_key  */
  YYSYMBOL_types_def = 90,                 /* types_def  */
  YYSYMBOL_91_3 = 91,                      /* $@3  */
  YYSYMBOL_constants_def = 92,             /* constants_def  */
  YYSYMBOL_93_4 = 93,                      /* $@4  */
  YYSYMBOL_predicates_def = 94,            /* predicates_def  */
  YYSYMBOL_functions_def = 95,             /* functions_def  */
  YYSYMBOL_96_5 = 96,                      /* $@5  */
  YYSYMBOL_predicate_decls = 97,           /* predicate_decls  */
  YYSYMBOL_predicate_decl = 98,            /* predicate_decl  */
  YYSYMBOL_99_6 = 99,                      /* $@6  */
  YYSYMBOL_function_decls = 100,           /* function_decls  */
  YYSYMBOL_function_decl_seq = 101,        /* function_decl_seq  */
  YYSYMBOL_function_type_spec = 102,       /* function_type_spec  */
  YYSYMBOL_103_7 = 103,                    /* $@7  */
  YYSYMBOL_function_decl = 104,            /* function_decl  */
  YYSYMBOL_105_8 = 105,                    /* $@8  */
  YYSYMBOL_action_def = 106,               /* action_def  */
  YYSYMBOL_107_9 = 107,                    /* $@9  */
  YYSYMBOL_108_10 = 108,                   /* $@10  */
  YYSYMBOL_parameters = 109,               /* parameters  */
  YYSYMBOL_action_body = 110,              /* action_body  */
  YYSYMBOL_action_body2 = 111,             /* action_body2  */
  YYSYMBOL_precondition = 112,             /* precondition  */
  YYSYMBOL_113_11 = 113,                   /* $@11  */
  YYSYMBOL_effect = 114,                   /* effect  */
  YYSYMBOL_115_12 = 115,                   /* $@12  */
  YYSYMBOL_da_body = 116,                  /* da_body  */
  YYSYMBOL_da_body2 = 117,                 /* da_body2  */
  YYSYMBOL_duration_constraint = 118,      /* duration_constraint  */
  YYSYMBOL_simple_duration_constraint = 119, /* simple_duration_constraint  */
  YYSYMBOL_simple_duration_constraints = 120, /* simple_duration_constraints  */
  YYSYMBOL_da_gd = 121,                    /* da_gd  */
  YYSYMBOL_timed_gds = 122,                /* timed_gds  */
  YYSYMBOL_timed_gd = 123,                 /* timed_gd  */
  YYSYMBOL_124_13 = 124,                   /* $@13  */
  YYSYMBOL_125_14 = 125,                   /* $@14  */
  YYSYMBOL_126_15 = 126,                   /* $@15  */
  YYSYMBOL_eff_formula = 127,              /* eff_formula  */
  YYSYMBOL_128_16 = 128,                   /* $@16  */
  YYSYMBOL_129_17 = 129,                   /* $@17  */
  YYSYMBOL_130_18 = 130,                   /* $@18  */
  YYSYMBOL_eff_formulas = 131,             /* eff_formulas  */
  YYSYMBOL_one_eff_formula = 132,          /* one_eff_formula  */
  YYSYMBOL_term_literal = 133,             /* term_literal  */
  YYSYMBOL_term_literals = 134,            /* term_literals  */
  YYSYMBOL_da_effect = 135,                /* da_effect  */
  YYSYMBOL_136_19 = 136,                   /* $@19  */
  YYSYMBOL_137_20 = 137,                   /* $@20  */
  YYSYMBOL_da_effects = 138,               /* da_effects  */
  YYSYMBOL_timed_effect = 139,             /* timed_effect  */
  YYSYMBOL_140_21 = 140,                   /* $@21  */
  YYSYMBOL_141_22 = 141,                   /* $@22  */
  YYSYMBOL_a_effect = 142,                 /* a_effect  */
  YYSYMBOL_143_23 = 143,                   /* $@23  */
  YYSYMBOL_144_24 = 144,                   /* $@24  */
  YYSYMBOL_a_effects = 145,                /* a_effects  */
  YYSYMBOL_problem_def = 146,              /* problem_def  */
  YYSYMBOL_147_25 = 147,                   /* $@25  */
  YYSYMBOL_problem_body = 148,             /* problem_body  */
  YYSYMBOL_problem_body2 = 149,            /* problem_body2  */
  YYSYMBOL_problem_body3 = 150,            /* problem_body3  */
  YYSYMBOL_object_decl = 151,              /* object_decl  */
  YYSYMBOL_152_26 = 152,                   /* $@26  */
  YYSYMBOL_init = 153,                     /* init  */
  YYSYMBOL_init_elements = 154,            /* init_elements  */
  YYSYMBOL_init_element = 155,             /* init_element  */
  YYSYMBOL_156_27 = 156,                   /* $@27  */
  YYSYMBOL_157_28 = 157,                   /* $@28  */
  YYSYMBOL_goal_spec = 158,                /* goal_spec  */
  YYSYMBOL_goal = 159,                     /* goal  */
  YYSYMBOL_metric_spec = 160,              /* metric_spec  */
  YYSYMBOL_161_29 = 161,                   /* $@29  */
  YYSYMBOL_162_30 = 162,                   /* $@30  */
  YYSYMBOL_formula = 163,                  /* formula  */
  YYSYMBOL_164_31 = 164,                   /* $@31  */
  YYSYMBOL_165_32 = 165,                   /* $@32  */
  YYSYMBOL_166_33 = 166,                   /* $@33  */
  YYSYMBOL_167_34 = 167,                   /* $@34  */
  YYSYMBOL_conjuncts = 168,                /* conjuncts  */
  YYSYMBOL_disjuncts = 169,                /* disjuncts  */
  YYSYMBOL_atomic_term_formula = 170,      /* atomic_term_formula  */
  YYSYMBOL_171_35 = 171,                   /* $@35  */
  YYSYMBOL_atomic_name_formula = 172,      /* atomic_name_formula  */
  YYSYMBOL_173_36 = 173,                   /* $@36  */
  YYSYMBOL_name_literal = 174,             /* name_literal  */
  YYSYMBOL_f_exp = 175,                    /* f_exp  */
  YYSYMBOL_opt_f_exp = 176,                /* opt_f_exp  */
  YYSYMBOL_f_head = 177,                   /* f_head  */
  YYSYMBOL_178_37 = 178,                   /* $@37  */
  YYSYMBOL_ground_f_exp = 179,             /* ground_f_exp  */
  YYSYMBOL_opt_ground_f_exp = 180,         /* opt_ground_f_exp  */
  YYSYMBOL_ground_f_head = 181,            /* ground_f_head  */
  YYSYMBOL_182_38 = 182,                   /* $@38  */
  YYSYMBOL_terms = 183,                    /* terms  */
  YYSYMBOL_names = 184,                    /* names  */
  YYSYMBOL_term = 185,                     /* term  */
  YYSYMBOL_variables = 186,                /* variables  */
  YYSYMBOL_187_39 = 187,                   /* $@39  */
  YYSYMBOL_variable_seq = 188,             /* variable_seq  */
  YYSYMBOL_typed_names = 189,              /* typed_names  */
  YYSYMBOL_190_40 = 190,                   /* $@40  */
  YYSYMBOL_name_seq = 191,                 /* name_seq  */
  YYSYMBOL_type_spec = 192,                /* type_spec  */
  YYSYMBOL_193_41 = 193,                   /* $@41  */
  YYSYMBOL_type = 194,                     /* type  */
  YYSYMBOL_types = 195,                    /* types  */
  YYSYMBOL_function_type = 196,            /* function_type  */
  YYSYMBOL_define = 197,                   /* define  */
  YYSYMBOL_domain = 198,                   /* domain  */
  YYSYMBOL_problem = 199,                  /* problem  */
  YYSYMBOL_when = 200,                     /* when  */
  YYSYMBOL_not = 201,                      /* not  */
  YYSYMBOL_and = 202,                      /* and  */
  YYSYMBOL_or = 203,                       /* or  */
  YYSYMBOL_imply = 204,                    /* imply  */
  YYSYMBOL_exists = 205,                   /* exists  */
  YYSYMBOL_forall = 206,                   /* forall  */
  YYSYMBOL_at = 207,                       /* at  */
  YYSYMBOL_over = 208,                     /* over  */
  YYSYMBOL_start = 209,                    /* start  */
  YYSYMBOL_end = 210,                      /* end  */
  YYSYMBOL_all = 211,                      /* all  */
  YYSYMBOL_duration_var = 212,             /* duration_var  */
  YYSYMBOL_minimize = 213,                 /* minimize  */
  YYSYMBOL_maximize = 214,                 /* maximize  */
  YYSYMBOL_number = 215,                   /* number  */
  YYSYMBOL_object = 216,                   /* object  */
  YYSYMBOL_either = 217,                   /* either  */
  YYSYMBOL_type_name = 218,                /* type_name  */
  YYSYMBOL_predicate = 219,                /* predicate  */
  YYSYMBOL_init_predicate = 220,           /* init_predicate  */
  YYSYMBOL_function = 221,                 /* function  */
  YYSYMBOL_name = 222,                     /* name  */
  YYSYMBOL_variable = 223                  /* variable  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   978

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  154
/* YYNRULES -- Number of rules.  */
#define YYNRULES  332
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  545

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   317


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      63,    64,    68,    67,     2,    65,     2,    69,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    66,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   260,   260,   260,   264,   265,   266,   273,   273,   277,
     278,   279,   280,   283,   284,   285,   288,   289,   290,   291,
     292,   293,   294,   297,   298,   299,   300,   301,   304,   305,
     306,   307,   308,   311,   312,   313,   314,   315,   318,   319,
     320,   323,   324,   325,   328,   329,   330,   333,   334,   337,
     340,   343,   344,   347,   348,   349,   351,   353,   354,   356,
     358,   360,   361,   362,   363,   364,   366,   368,   375,   375,
     379,   379,   383,   386,   386,   393,   394,   397,   397,   401,
     402,   403,   406,   407,   410,   410,   413,   413,   421,   421,
     423,   423,   428,   429,   432,   433,   436,   437,   440,   440,
     444,   444,   447,   448,   451,   452,   459,   460,   464,   469,
     474,   478,   479,   487,   488,   491,   492,   495,   495,   496,
     496,   497,   497,   504,   505,   506,   506,   508,   509,   508,
     513,   514,   517,   518,   521,   522,   526,   527,   530,   531,
     532,   532,   534,   534,   538,   539,   543,   542,   546,   545,
     550,   551,   552,   552,   554,   554,   558,   559,   567,   566,
     571,   572,   575,   576,   579,   580,   583,   583,   587,   590,
     591,   594,   594,   596,   596,   598,   600,   602,   606,   607,
     610,   613,   613,   615,   615,   623,   624,   625,   626,   627,
     627,   628,   628,   630,   630,   632,   632,   636,   637,   640,
     641,   644,   644,   648,   648,   652,   653,   660,   661,   662,
     663,   664,   665,   668,   669,   672,   672,   674,   677,   678,
     680,   682,   684,   686,   689,   690,   693,   693,   695,   702,
     703,   704,   707,   708,   711,   712,   715,   716,   717,   717,
     721,   722,   725,   726,   727,   727,   730,   731,   734,   734,
     737,   738,   739,   742,   743,   744,   745,   748,   755,   758,
     761,   764,   767,   770,   773,   776,   779,   782,   785,   788,
     791,   794,   797,   800,   803,   806,   809,   812,   815,   818,
     818,   818,   819,   820,   820,   820,   820,   820,   821,   821,
     821,   822,   825,   826,   826,   829,   829,   829,   830,   831,
     831,   831,   831,   832,   832,   832,   833,   834,   834,   837,
     840,   840,   840,   841,   841,   841,   842,   842,   842,   842,
     842,   842,   842,   843,   843,   843,   843,   843,   844,   844,
     844,   845,   848
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "REQUIREMENTS",
  "TYPES", "CONSTANTS", "PREDICATES", "FUNCTIONS", "STRIPS", "TYPING",
  "NEGATIVE_PRECONDITIONS", "DISJUNCTIVE_PRECONDITIONS", "EQUALITY",
  "EXISTENTIAL_PRECONDITIONS", "UNIVERSAL_PRECONDITIONS",
  "QUANTIFIED_PRECONDITIONS", "CONDITIONAL_EFFECTS", "FLUENTS", "ADL",
  "DURATIVE_ACTIONS", "DURATION_INEQUALITIES", "CONTINUOUS_EFFECTS",
  "TIMED_INITIAL_LITERALS", "ACTION", "PARAMETERS", "PRECONDITION",
  "EFFECT", "DURATIVE_ACTION", "DURATION", "CONDITION", "PDOMAIN",
  "OBJECTS", "INIT", "GOAL", "METRIC", "LE", "GE", "ILLEGAL_TOKEN",
  "DEFINE", "DOMAIN_TOKEN", "PROBLEM", "WHEN", "NOT", "AND", "OR", "IMPLY",
  "EXISTS", "FORALL", "AT", "OVER", "START", "END", "ALL", "MINIMIZE",
  "MAXIMIZE", "TOTAL_TIME", "NUMBER_TOKEN", "OBJECT_TOKEN", "EITHER",
  "NAME", "DURATION_VAR", "VARIABLE", "NUMBER", "'('", "')'", "'-'", "'='",
  "'+'", "'*'", "'/'", "$accept", "pddl_file", "$@1",
  "domains_and_problems", "domain_def", "$@2", "domain_body",
  "domain_body2", "domain_body3", "domain_body4", "domain_body5",
  "domain_body6", "domain_body7", "domain_body8", "domain_body9",
  "structure_defs", "structure_def", "require_def", "require_keys",
  "require_key", "types_def", "$@3", "constants_def", "$@4",
  "predicates_def", "functions_def", "$@5", "predicate_decls",
  "predicate_decl", "$@6", "function_decls", "function_decl_seq",
  "function_type_spec", "$@7", "function_decl", "$@8", "action_def", "$@9",
  "$@10", "parameters", "action_body", "action_body2", "precondition",
  "$@11", "effect", "$@12", "da_body", "da_body2", "duration_constraint",
  "simple_duration_constraint", "simple_duration_constraints", "da_gd",
  "timed_gds", "timed_gd", "$@13", "$@14", "$@15", "eff_formula", "$@16",
  "$@17", "$@18", "eff_formulas", "one_eff_formula", "term_literal",
  "term_literals", "da_effect", "$@19", "$@20", "da_effects",
  "timed_effect", "$@21", "$@22", "a_effect", "$@23", "$@24", "a_effects",
  "problem_def", "$@25", "problem_body", "problem_body2", "problem_body3",
  "object_decl", "$@26", "init", "init_elements", "init_element", "$@27",
  "$@28", "goal_spec", "goal", "metric_spec", "$@29", "$@30", "formula",
  "$@31", "$@32", "$@33", "$@34", "conjuncts", "disjuncts",
  "atomic_term_formula", "$@35", "atomic_name_formula", "$@36",
  "name_literal", "f_exp", "opt_f_exp", "f_head", "$@37", "ground_f_exp",
  "opt_ground_f_exp", "ground_f_head", "$@38", "terms", "names", "term",
  "variables", "$@39", "variable_seq", "typed_names", "$@40", "name_seq",
  "type_spec", "$@41", "type", "types", "function_type", "define",
  "domain", "problem", "when", "not", "and", "or", "imply", "exists",
  "forall", "at", "over", "start", "end", "all", "duration_var",
  "minimize", "maximize", "number", "object", "either", "type_name",
  "predicate", "init_predicate", "function", "name", "variable", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-426)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-269)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -426,    32,  -426,  -426,   -43,    39,  -426,  -426,  -426,     7,
       5,  -426,  -426,   794,   794,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,    36,    56,  -426,
      90,    92,   149,    78,    93,  -426,  -426,   134,  -426,   143,
     145,   153,   155,   179,  -426,   794,   956,  -426,  -426,  -426,
    -426,   794,   794,  -426,    20,  -426,   131,  -426,   157,  -426,
     182,  -426,   134,   181,   189,    66,  -426,   134,   181,   190,
     120,  -426,   134,   189,   190,   122,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,   329,  -426,   794,   794,    23,   191,  -426,  -426,    69,
    -426,   134,   134,    28,  -426,   134,   134,  -426,   180,  -426,
     134,   134,  -426,  -426,  -426,  -426,  -426,   130,   458,  -426,
     156,   882,  -426,  -426,   794,   173,     4,  -426,   232,   232,
     134,   134,   134,   194,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
     191,  -426,   197,   123,   236,    47,   202,   203,  -426,  -426,
     205,   207,  -426,   208,   744,   794,   211,   211,   217,  -426,
     211,  -426,  -426,   212,  -426,   240,  -426,   214,  -426,  -426,
     215,   108,  -426,  -426,    62,  -426,   241,  -426,   245,  -426,
    -426,   222,  -426,  -426,  -426,  -426,  -426,   218,    81,  -426,
     221,  -426,  -426,  -426,   223,   215,   227,  -426,  -426,    -8,
      30,  -426,   794,    87,   400,   231,  -426,   112,  -426,   904,
    -426,  -426,  -426,  -426,  -426,  -426,   816,  -426,  -426,  -426,
     226,   226,  -426,   226,  -426,   233,   238,   242,  -426,   244,
     429,  -426,  -426,  -426,  -426,  -426,  -426,  -426,   770,   215,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,   643,  -426,  -426,   211,  -426,  -426,   239,  -426,  -426,
    -426,   670,   670,   670,   114,   174,  -426,  -426,    54,   249,
    -426,  -426,  -426,  -426,  -426,  -426,   219,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,   722,   247,
     250,  -426,   770,  -426,  -426,   252,   127,  -426,   215,   248,
     251,  -426,   696,   696,  -426,  -426,  -426,  -426,   215,   882,
     253,   136,   255,  -426,   331,   257,  -426,  -426,   259,   260,
       2,  -426,  -426,  -426,   238,  -426,  -426,   160,  -426,  -426,
     160,   267,  -426,  -426,   794,   263,  -426,   882,   264,   266,
    -426,   268,  -426,  -426,  -426,   162,   215,   211,   211,   486,
    -426,   368,   288,  -426,   290,  -426,  -426,  -426,  -426,   211,
     670,   670,   670,   670,  -426,  -426,  -426,  -426,  -426,   164,
     292,  -426,  -426,  -426,  -426,   166,  -426,  -426,  -426,  -426,
     540,  -426,   293,  -426,  -426,   860,  -426,   294,   567,  -426,
    -426,  -426,   295,   296,   297,  -426,  -426,  -426,   696,   696,
     696,   696,  -426,  -426,   299,   300,   670,   670,   670,   670,
    -426,   302,  -426,  -426,   211,   304,   304,   183,  -426,  -426,
     215,   215,   215,  -426,  -426,  -426,  -426,  -426,   247,  -426,
    -426,  -426,   215,   215,   696,   696,   696,   696,   838,   327,
    -426,   227,  -426,   328,   330,   337,   338,   513,   256,   339,
     340,   816,  -426,   364,   365,   366,   367,   370,   594,   621,
     377,   396,   397,  -426,   398,   399,   401,   406,  -426,  -426,
     408,  -426,  -426,  -426,  -426,  -426,  -426,   233,   215,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,   175,  -426,   409,  -426,   177,
     305,   860,  -426,  -426,  -426,   299,  -426,  -426,   211,   410,
     425,  -426,   304,   426,  -426
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     4,     1,     3,     0,     5,     6,   258,     0,
       0,   259,   260,     0,     0,   310,   311,   312,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   313,   314,   315,   331,     0,     0,     7,
       0,     9,     0,     0,     0,    12,    15,    22,    47,    10,
      13,    16,    17,    18,    49,     0,     0,    68,    70,    75,
      73,     0,     0,     8,     0,    48,     0,    11,     0,    14,
       0,    19,    27,    23,    24,     0,    20,    32,    28,    29,
       0,    21,    37,    33,    34,     0,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    51,   242,   242,     0,    79,    88,    90,     0,
      25,    40,    38,     0,    26,    43,    41,    30,     0,    31,
      46,    44,    35,    36,   158,    50,    52,     0,   243,   246,
       0,     0,    72,    76,     0,     0,    80,    82,    92,    92,
      39,    42,    45,     0,    69,   248,   244,   247,    71,   279,
     280,   281,   283,   284,   285,   286,   287,   288,   289,   290,
     294,   293,   282,   291,   292,    77,    86,   309,    74,    84,
      79,    83,     0,    96,     0,     0,     0,     0,   161,   163,
       0,     0,   165,   178,     0,   242,   236,   236,     0,    81,
     236,    98,   100,     0,    95,    96,    97,     0,   166,   169,
       0,     0,   160,   159,     0,   162,     0,   164,     0,   179,
     277,     0,   249,   250,   251,   245,   332,     0,   237,   240,
       0,   276,    85,   257,     0,     0,     0,    89,    94,     0,
     104,   106,   242,     0,     0,     0,   185,     0,   278,     0,
      78,   238,   241,    87,    93,    99,     0,   101,   123,   134,
       0,     0,   263,     0,   111,     0,     0,     0,   103,     0,
       0,   168,   170,   262,   264,   265,   266,   267,     0,     0,
     197,   189,   191,   193,   195,   201,   180,   274,   275,   183,
     181,     0,   253,   254,   236,   261,   127,     0,   130,   125,
     273,     0,     0,     0,     0,     0,   105,   138,     0,   104,
     113,    91,   167,   295,   296,   297,   173,   299,   300,   301,
     302,   303,   304,   305,   308,   307,   298,   306,     0,     0,
       0,   171,     0,   234,   235,     0,     0,   199,     0,     0,
       0,   229,     0,     0,   252,   255,   256,   239,     0,     0,
       0,     0,     0,   207,     0,     0,   212,   217,     0,     0,
       0,   107,   112,   268,     0,   144,   140,     0,   269,   115,
       0,     0,   102,   232,     0,     0,   228,     0,     0,     0,
     232,     0,   187,   188,   198,     0,     0,   236,   236,     0,
     218,     0,     0,   223,     0,   128,   135,   124,   131,   236,
       0,     0,     0,     0,   215,   108,   109,   110,   142,     0,
       0,   270,   271,   146,   148,     0,   117,   119,   272,   121,
       0,   226,     0,   203,   175,     0,   205,     0,     0,   186,
     190,   200,     0,     0,     0,   202,   230,   231,     0,     0,
       0,     0,   184,   182,     0,     0,   213,     0,     0,     0,
     229,     0,   139,   145,   236,     0,     0,     0,   114,   116,
       0,     0,     0,   174,   233,   232,   176,   232,     0,   177,
     172,   192,     0,     0,   224,     0,     0,     0,     0,     0,
     132,     0,   214,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   150,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   225,     0,     0,     0,     0,   136,   129,
       0,   209,   208,   210,   211,   216,   143,     0,     0,   156,
     152,   147,   149,   118,   120,   122,   227,   204,   206,   194,
     196,   220,   219,   221,   222,     0,   126,     0,   154,     0,
       0,     0,   133,   137,   141,     0,   151,   157,   236,     0,
       0,   155,     0,     0,   153
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -426,  -426,  -426,  -426,  -426,  -426,  -426,   278,   280,  -426,
    -426,  -426,   285,   273,   311,   210,    41,   289,  -426,   390,
    -426,  -426,   -27,  -426,   -20,   -12,  -426,  -426,  -426,  -426,
     322,  -426,  -426,  -426,   357,  -426,  -426,  -426,  -426,   355,
    -426,   323,  -426,  -426,  -426,  -426,  -426,   220,  -426,   228,
    -426,   167,  -426,   115,  -426,  -426,  -426,  -327,  -426,  -426,
    -426,  -426,    11,  -220,  -426,  -383,  -426,  -426,  -426,   107,
    -426,  -426,  -425,  -426,  -426,  -426,  -426,  -426,  -426,   373,
     393,  -426,  -426,  -426,  -426,  -426,  -426,  -426,   394,  -426,
    -426,  -426,  -426,  -216,  -426,  -426,  -426,  -426,  -426,  -426,
    -222,  -426,  -351,  -426,  -426,  -263,  -426,  -426,  -426,  -297,
    -426,   258,  -426,   161,  -346,   281,  -185,  -426,  -426,   -87,
    -426,  -426,   382,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -283,  -219,  -223,  -426,  -426,  -426,  -233,  -252,  -426,   269,
     270,  -426,  -145,  -426,  -426,  -426,  -217,  -426,  -165,  -124,
    -426,  -131,   -13,  -208
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,     4,     6,    41,    44,    45,    46,    71,
      76,    81,   110,   114,   119,    47,    48,    49,   101,   102,
      50,   103,    51,   104,    52,    53,   106,   105,   133,   186,
     135,   136,   170,   188,   137,   187,    54,   138,   139,   173,
     193,   194,   195,   225,   196,   226,   257,   258,   230,   231,
     294,   299,   405,   300,   450,   451,   452,   247,   342,   338,
     434,   341,   469,   482,   525,   296,   400,   441,   399,   297,
     445,   446,   483,   530,   535,   529,     7,   143,   177,   178,
     179,   180,   232,   181,   233,   262,   370,   363,   182,   183,
     209,   333,   332,   235,   327,   328,   329,   330,   326,   375,
     236,   331,   368,   457,   417,   345,   473,   346,   440,   382,
     494,   383,   455,   379,   410,   322,   217,   284,   218,   127,
     185,   128,   146,   184,   212,   281,   222,     9,    13,    14,
     286,   287,   254,   271,   272,   273,   274,   357,   361,   403,
     404,   409,   291,   279,   280,   223,   213,   239,   164,   275,
     321,   347,   167,   219
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      37,    38,   220,   166,   249,   224,   248,   165,   320,   245,
     242,   270,   354,   289,   388,   269,   443,   130,   416,   214,
       5,   484,   282,   288,   418,    78,    83,   250,   251,   348,
     349,    73,     3,    84,    59,   252,   384,   250,   251,    74,
      79,   319,    85,    61,    11,    12,   360,    62,   107,   108,
      56,    61,   121,   325,   116,    62,   255,   121,   253,   256,
     324,   112,   356,   116,   335,   340,   112,   134,   253,   169,
      10,    58,   355,    60,   283,   359,    60,     8,   198,   199,
     200,    56,    57,    58,    59,    60,   131,   132,    65,    61,
     129,   129,    61,    62,   199,   200,    62,   252,   215,   337,
      39,    61,   353,   358,   537,    62,   292,   490,   293,   488,
     374,   489,   376,    65,   324,   147,   336,   543,    65,   249,
      40,   248,   385,    65,   527,    58,    59,   436,   437,   438,
     439,   464,   465,   466,   467,    57,    58,    59,    60,   198,
     199,   200,   216,    61,   500,   259,   145,    62,   191,   192,
     260,   261,    65,    42,    61,    43,    65,    63,    62,   421,
     422,    65,    58,    59,    60,   277,   278,   493,   495,   496,
     497,   427,   129,   472,   474,   475,   476,   350,   351,    55,
      61,    65,    65,    65,    62,    58,   124,   366,    59,    60,
     234,   373,   423,   424,   144,   360,   458,    64,   508,   246,
     387,   366,   366,    61,   435,    61,    66,    62,    68,    62,
     401,   402,   249,   394,   470,   285,    70,   252,    75,   129,
     148,   267,   353,   249,   249,   234,   420,   295,   442,   447,
     448,   353,   358,   411,   485,   486,   487,   168,   531,   532,
     481,   536,    80,   413,   109,   498,   491,   492,   510,   249,
     411,   248,   113,   118,   134,   323,   172,   175,   509,   480,
     190,    72,    77,    82,   197,   201,   192,   203,   204,   427,
     206,   208,   216,   221,   200,   255,   227,   229,   234,   237,
     238,  -268,   240,   111,   115,   243,   290,   244,   111,   120,
     246,   413,   528,   115,   120,   276,   295,   366,   366,   366,
     366,   298,   339,   249,   353,   533,   301,   249,   302,   323,
     367,   377,   369,   249,   378,   470,   372,   386,   389,   408,
     249,   395,   140,   396,   397,   412,   141,    67,   414,   415,
      69,   142,   419,   366,   366,   366,   366,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   432,   540,   433,   444,   122,   456,   459,   461,
     462,   463,   468,   117,   471,   478,   426,   481,   538,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,   499,   501,   125,   502,   123,   390,   454,   391,   392,
     393,   503,   504,   506,   507,   454,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,   511,   512,
     513,   514,   176,   428,   515,   429,   430,   431,   149,   150,
     151,   518,   263,   252,   264,   265,   266,   267,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     519,   520,   521,   522,   426,   523,   268,   303,   304,   305,
     524,   263,   526,   534,   541,   454,   454,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   542,
     544,   126,   189,   171,   174,   318,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,   228,   362,
     449,   398,   352,   145,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,   539,   216,   479,   202,
     425,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,   205,   216,   207,   365,   505,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
     241,   477,     0,   371,   453,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,     0,   406,
     407,   460,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,     0,     0,     0,   516,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,   149,   150,   151,     0,   517,     0,     0,     0,     0,
       0,   152,   153,   154,   155,   156,   157,   158,   159,     0,
     210,   162,   163,     0,     0,     0,     0,   334,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,     0,   343,   344,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,     0,   380,   381,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,   149,   150,   151,   364,     0,     0,     0,     0,
       0,     0,   152,   153,   154,   155,   156,   157,   158,   159,
       0,   210,   162,   163,     0,     0,     0,   211,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
       0,   216,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,   149,   150,   151,   285,   263,   252,
       0,     0,     0,   267,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   149,   150,   151,     0,
     263,   252,     0,     0,     0,     0,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   149,   150,
     151,     0,   263,     0,     0,     0,     0,     0,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     149,   150,   151,     0,     0,     0,     0,     0,     0,     0,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   149,   150,   151,     0,     0,     0,     0,     0,
       0,     0,   152,   153,   154,   155,   156,   157,   158,   159,
       0,   210,   162,   163,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100
};

static const yytype_int16 yycheck[] =
{
      13,    14,   187,   134,   226,   190,   226,   131,   260,   225,
     218,   234,   295,   246,   341,   234,   399,   104,   369,   184,
      63,   446,   239,   246,   370,    52,    53,    35,    36,   292,
     293,    51,     0,    53,     6,    43,   333,    35,    36,    51,
      52,   260,    55,    23,    39,    40,   298,    27,    61,    62,
       3,    23,    79,   269,    74,    27,    26,    84,    66,    29,
     268,    73,   295,    83,   281,   287,    78,    63,    66,    65,
      63,     5,   295,     7,   239,   298,     7,    38,    31,    32,
      33,     3,     4,     5,     6,     7,    63,    64,    47,    23,
     103,   104,    23,    27,    32,    33,    27,    43,   185,   284,
      64,    23,    48,    49,   529,    27,   251,   458,   253,   455,
     326,   457,   328,    72,   322,   128,   281,   542,    77,   341,
      64,   341,   338,    82,   507,     5,     6,   390,   391,   392,
     393,   428,   429,   430,   431,     4,     5,     6,     7,    31,
      32,    33,    61,    23,   471,   232,    65,    27,    25,    26,
      63,    64,   111,    63,    23,    63,   115,    64,    27,   375,
     376,   120,     5,     6,     7,    53,    54,   464,   465,   466,
     467,   379,   185,   436,   437,   438,   439,    63,    64,    30,
      23,   140,   141,   142,    27,     5,    64,   318,     6,     7,
      63,    64,   377,   378,    64,   447,   415,    63,   481,    63,
      64,   332,   333,    23,   389,    23,    63,    27,    63,    27,
      50,    51,   434,   344,   434,    41,    63,    43,    63,   232,
      64,    47,    48,   445,   446,    63,    64,    63,    64,    63,
      64,    48,    49,   364,   450,   451,   452,    64,    63,    64,
      63,    64,    63,   367,    63,   468,   462,   463,   481,   471,
     381,   471,    63,    63,    63,   268,    24,    63,   481,   444,
      63,    51,    52,    53,    28,    63,    26,    64,    63,   477,
      63,    63,    61,    56,    33,    26,    64,    63,    63,    34,
      58,    62,    64,    73,    74,    64,    60,    64,    78,    79,
      63,   415,   508,    83,    84,    64,    63,   428,   429,   430,
     431,    63,    63,   525,    48,   525,    64,   529,    64,   322,
      63,    63,    62,   535,    63,   535,    64,    64,    63,    52,
     542,    64,   112,    64,    64,    62,   116,    49,    64,    63,
      50,   121,    64,   464,   465,   466,   467,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    64,   538,    64,    63,    83,    64,    64,    64,
      64,    64,    63,    78,    64,    63,   379,    63,    63,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    64,    64,    64,    64,    84,    65,   410,    67,    68,
      69,    64,    64,    64,    64,   418,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    64,    64,
      64,    64,   143,    65,    64,    67,    68,    69,    38,    39,
      40,    64,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      64,    64,    64,    64,   477,    64,    66,    38,    39,    40,
      64,    42,    64,    64,    64,   488,   489,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    64,
      64,   101,   170,   136,   139,    66,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,   195,   299,
     405,   354,   294,    65,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,   535,    61,   441,   176,
      64,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,   180,    61,   181,   318,    64,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
     218,   440,    -1,   322,    64,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,   360,
     360,    64,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    -1,    -1,    64,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    38,    39,    40,    -1,    64,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      57,    58,    59,    -1,    -1,    -1,    -1,    64,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    38,    39,    40,    63,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    57,    58,    59,    -1,    -1,    -1,    63,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    61,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    38,    39,    40,    41,    42,    43,
      -1,    -1,    -1,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    38,    39,    40,    -1,
      42,    43,    -1,    -1,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    38,    39,
      40,    -1,    42,    -1,    -1,    -1,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    57,    58,    59,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    71,    72,     0,    73,    63,    74,   146,    38,   197,
      63,    39,    40,   198,   199,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,   222,   222,    64,
      64,    75,    63,    63,    76,    77,    78,    85,    86,    87,
      90,    92,    94,    95,   106,    30,     3,     4,     5,     6,
       7,    23,    27,    64,    63,    86,    63,    77,    63,    78,
      63,    79,    85,    94,    95,    63,    80,    85,    92,    95,
      63,    81,    85,    92,    94,   222,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    88,    89,    91,    93,    97,    96,   222,   222,    63,
      82,    85,    95,    63,    83,    85,    94,    82,    63,    84,
      85,    92,    83,    84,    64,    64,    89,   189,   191,   222,
     189,    63,    64,    98,    63,   100,   101,   104,   107,   108,
      85,    85,    85,   147,    64,    65,   192,   222,    64,    38,
      39,    40,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,   218,   219,   221,   222,    64,    65,
     102,   104,    24,   109,   109,    63,    87,   148,   149,   150,
     151,   153,   158,   159,   193,   190,    99,   105,   103,   100,
      63,    25,    26,   110,   111,   112,   114,    28,    31,    32,
      33,    63,   149,    64,    63,   150,    63,   158,    63,   160,
      57,    63,   194,   216,   218,   189,    61,   186,   188,   223,
     186,    56,   196,   215,   186,   113,   115,    64,   111,    63,
     118,   119,   152,   154,    63,   163,   170,    34,    58,   217,
      64,   192,   223,    64,    64,   163,    63,   127,   133,   170,
      35,    36,    43,    66,   202,    26,    29,   116,   117,   189,
      63,    64,   155,    42,    44,    45,    46,    47,    66,   201,
     202,   203,   204,   205,   206,   219,    64,    53,    54,   213,
     214,   195,   216,   218,   187,    41,   200,   201,   202,   206,
      60,   212,   212,   212,   120,    63,   135,   139,    63,   121,
     123,    64,    64,    38,    39,    40,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    66,   201,
     207,   220,   185,   222,   223,   163,   168,   164,   165,   166,
     167,   171,   162,   161,    64,   216,   218,   186,   129,    63,
     170,   131,   128,    62,    63,   175,   177,   221,   175,   175,
      63,    64,   119,    48,   200,   202,   206,   207,    49,   202,
     207,   208,   117,   157,    63,   181,   221,    63,   172,    62,
     156,   185,    64,    64,   163,   169,   163,    63,    63,   183,
      62,    63,   179,   181,   179,   163,    64,    64,   127,    63,
      65,    67,    68,    69,   221,    64,    64,    64,   121,   138,
     136,    50,    51,   209,   210,   122,   209,   210,    52,   211,
     184,   221,    62,   219,    64,    63,   172,   174,   184,    64,
      64,   163,   163,   186,   186,    64,   222,   223,    65,    67,
      68,    69,    64,    64,   130,   186,   175,   175,   175,   175,
     178,   137,    64,   135,    63,   140,   141,    63,    64,   123,
     124,   125,   126,    64,   222,   182,    64,   173,   201,    64,
      64,    64,    64,    64,   179,   179,   179,   179,    63,   132,
     133,    64,   175,   176,   175,   175,   175,   183,    63,   139,
     186,    63,   133,   142,   142,   163,   163,   163,   184,   184,
     172,   163,   163,   179,   180,   179,   179,   179,   202,    64,
     127,    64,    64,    64,    64,    64,    64,    64,   200,   202,
     206,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,   134,    64,   135,   163,   145,
     143,    63,    64,   133,    64,   144,    64,   142,    63,   132,
     186,    64,    64,   142,    64
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    70,    72,    71,    73,    73,    73,    75,    74,    76,
      76,    76,    76,    77,    77,    77,    78,    78,    78,    78,
      78,    78,    78,    79,    79,    79,    79,    79,    80,    80,
      80,    80,    80,    81,    81,    81,    81,    81,    82,    82,
      82,    83,    83,    83,    84,    84,    84,    85,    85,    86,
      87,    88,    88,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    91,    90,
      93,    92,    94,    96,    95,    97,    97,    99,    98,   100,
     100,   100,   101,   101,   103,   102,   105,   104,   107,   106,
     108,   106,   109,   109,   110,   110,   111,   111,   113,   112,
     115,   114,   116,   116,   117,   117,   118,   118,   119,   119,
     119,   120,   120,   121,   121,   122,   122,   124,   123,   125,
     123,   126,   123,   127,   127,   128,   127,   129,   130,   127,
     131,   131,   132,   132,   133,   133,   134,   134,   135,   135,
     136,   135,   137,   135,   138,   138,   140,   139,   141,   139,
     142,   142,   143,   142,   144,   142,   145,   145,   147,   146,
     148,   148,   149,   149,   150,   150,   152,   151,   153,   154,
     154,   156,   155,   157,   155,   155,   155,   155,   158,   158,
     159,   161,   160,   162,   160,   163,   163,   163,   163,   164,
     163,   165,   163,   166,   163,   167,   163,   168,   168,   169,
     169,   171,   170,   173,   172,   174,   174,   175,   175,   175,
     175,   175,   175,   176,   176,   178,   177,   177,   179,   179,
     179,   179,   179,   179,   180,   180,   182,   181,   181,   183,
     183,   183,   184,   184,   185,   185,   186,   186,   187,   186,
     188,   188,   189,   189,   190,   189,   191,   191,   193,   192,
     194,   194,   194,   195,   195,   195,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   219,   219,   219,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   221,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   223
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     0,     2,     2,     0,     9,     0,
       1,     2,     1,     1,     2,     1,     1,     1,     1,     2,
       2,     2,     1,     1,     1,     2,     2,     1,     1,     1,
       2,     2,     1,     1,     1,     2,     2,     1,     1,     2,
       1,     1,     2,     1,     1,     2,     1,     1,     2,     1,
       4,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     5,
       0,     5,     4,     0,     5,     0,     2,     0,     5,     0,
       1,     3,     1,     2,     0,     3,     0,     5,     0,     7,
       0,     9,     0,     4,     2,     1,     0,     1,     0,     3,
       0,     3,     3,     1,     0,     2,     1,     4,     5,     5,
       5,     0,     2,     1,     4,     0,     2,     0,     6,     0,
       6,     0,     6,     1,     4,     0,     8,     0,     0,     7,
       0,     2,     1,     4,     1,     4,     0,     2,     1,     4,
       0,     8,     0,     6,     0,     2,     0,     6,     0,     6,
       1,     4,     0,     8,     0,     6,     0,     2,     0,    13,
       2,     1,     2,     1,     2,     1,     0,     5,     4,     0,
       2,     0,     5,     0,     5,     4,     5,     5,     1,     2,
       4,     0,     6,     0,     6,     1,     5,     4,     4,     0,
       5,     0,     6,     0,     8,     0,     8,     0,     2,     0,
       2,     0,     5,     0,     5,     1,     4,     1,     5,     5,
       5,     5,     1,     0,     1,     0,     5,     1,     1,     5,
       5,     5,     5,     1,     0,     1,     0,     5,     1,     0,
       2,     2,     0,     2,     1,     1,     0,     1,     0,     4,
       1,     2,     0,     1,     0,     4,     1,     2,     0,     3,
       1,     1,     4,     1,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 260 "pddl.yy"
            { success = true; line_number = 1; }
#line 2172 "pddl.cc"
    break;

  case 3: /* pddl_file: $@1 domains_and_problems  */
#line 261 "pddl.yy"
              { if (!success) YYERROR; }
#line 2178 "pddl.cc"
    break;

  case 7: /* $@2: %empty  */
#line 273 "pddl.yy"
                                            { make_domain((yyvsp[-1].str)); }
#line 2184 "pddl.cc"
    break;

  case 53: /* require_key: STRIPS  */
#line 347 "pddl.yy"
                     { requirements->strips = true; }
#line 2190 "pddl.cc"
    break;

  case 54: /* require_key: TYPING  */
#line 348 "pddl.yy"
                     { requirements->typing = true; }
#line 2196 "pddl.cc"
    break;

  case 55: /* require_key: NEGATIVE_PRECONDITIONS  */
#line 350 "pddl.yy"
                { requirements->negative_preconditions = true; }
#line 2202 "pddl.cc"
    break;

  case 56: /* require_key: DISJUNCTIVE_PRECONDITIONS  */
#line 352 "pddl.yy"
                { requirements->disjunctive_preconditions = true; }
#line 2208 "pddl.cc"
    break;

  case 57: /* require_key: EQUALITY  */
#line 353 "pddl.yy"
                       { requirements->equality = true; }
#line 2214 "pddl.cc"
    break;

  case 58: /* require_key: EXISTENTIAL_PRECONDITIONS  */
#line 355 "pddl.yy"
                { requirements->existential_preconditions = true; }
#line 2220 "pddl.cc"
    break;

  case 59: /* require_key: UNIVERSAL_PRECONDITIONS  */
#line 357 "pddl.yy"
                { requirements->universal_preconditions = true; }
#line 2226 "pddl.cc"
    break;

  case 60: /* require_key: QUANTIFIED_PRECONDITIONS  */
#line 359 "pddl.yy"
                { requirements->quantified_preconditions(); }
#line 2232 "pddl.cc"
    break;

  case 61: /* require_key: CONDITIONAL_EFFECTS  */
#line 360 "pddl.yy"
                                  { requirements->conditional_effects = true; }
#line 2238 "pddl.cc"
    break;

  case 62: /* require_key: FLUENTS  */
#line 361 "pddl.yy"
                      { requirements->fluents = true; }
#line 2244 "pddl.cc"
    break;

  case 63: /* require_key: ADL  */
#line 362 "pddl.yy"
                  { requirements->adl(); }
#line 2250 "pddl.cc"
    break;

  case 64: /* require_key: DURATIVE_ACTIONS  */
#line 363 "pddl.yy"
                               { requirements->durative_actions = true; }
#line 2256 "pddl.cc"
    break;

  case 65: /* require_key: DURATION_INEQUALITIES  */
#line 365 "pddl.yy"
                { requirements->duration_inequalities = true; }
#line 2262 "pddl.cc"
    break;

  case 66: /* require_key: CONTINUOUS_EFFECTS  */
#line 367 "pddl.yy"
                { yyerror("`:continuous-effects' not supported"); }
#line 2268 "pddl.cc"
    break;

  case 67: /* require_key: TIMED_INITIAL_LITERALS  */
#line 369 "pddl.yy"
                {
		  requirements->durative_actions = true;
		  requirements->timed_initial_literals = true;
		}
#line 2277 "pddl.cc"
    break;

  case 68: /* $@3: %empty  */
#line 375 "pddl.yy"
                      { require_typing(); name_kind = TYPE_KIND; }
#line 2283 "pddl.cc"
    break;

  case 69: /* types_def: '(' TYPES $@3 typed_names ')'  */
#line 376 "pddl.yy"
                              { name_kind = VOID_KIND; }
#line 2289 "pddl.cc"
    break;

  case 70: /* $@4: %empty  */
#line 379 "pddl.yy"
                              { name_kind = CONSTANT_KIND; }
#line 2295 "pddl.cc"
    break;

  case 71: /* constants_def: '(' CONSTANTS $@4 typed_names ')'  */
#line 380 "pddl.yy"
                  { name_kind = VOID_KIND; }
#line 2301 "pddl.cc"
    break;

  case 73: /* $@5: %empty  */
#line 386 "pddl.yy"
                              { require_fluents(); }
#line 2307 "pddl.cc"
    break;

  case 77: /* $@6: %empty  */
#line 397 "pddl.yy"
                               { make_predicate((yyvsp[0].str)); }
#line 2313 "pddl.cc"
    break;

  case 78: /* predicate_decl: '(' predicate $@6 variables ')'  */
#line 398 "pddl.yy"
                   { predicate = 0; }
#line 2319 "pddl.cc"
    break;

  case 84: /* $@7: %empty  */
#line 410 "pddl.yy"
                         { require_typing(); }
#line 2325 "pddl.cc"
    break;

  case 86: /* $@8: %empty  */
#line 413 "pddl.yy"
                             { make_function((yyvsp[0].str)); }
#line 2331 "pddl.cc"
    break;

  case 87: /* function_decl: '(' function $@8 variables ')'  */
#line 414 "pddl.yy"
                  { function = 0; }
#line 2337 "pddl.cc"
    break;

  case 88: /* $@9: %empty  */
#line 421 "pddl.yy"
                             { make_action((yyvsp[0].str), false); }
#line 2343 "pddl.cc"
    break;

  case 89: /* action_def: '(' ACTION name $@9 parameters action_body ')'  */
#line 422 "pddl.yy"
                                          { add_action(); }
#line 2349 "pddl.cc"
    break;

  case 90: /* $@10: %empty  */
#line 423 "pddl.yy"
                                      { make_action((yyvsp[0].str), true); }
#line 2355 "pddl.cc"
    break;

  case 91: /* action_def: '(' DURATIVE_ACTION name $@10 parameters DURATION duration_constraint da_body ')'  */
#line 425 "pddl.yy"
               { add_action(); }
#line 2361 "pddl.cc"
    break;

  case 98: /* $@11: %empty  */
#line 440 "pddl.yy"
                            { formula_time = AT_START; }
#line 2367 "pddl.cc"
    break;

  case 99: /* precondition: PRECONDITION $@11 formula  */
#line 441 "pddl.yy"
                 { action->set_condition(*(yyvsp[0].formula)); }
#line 2373 "pddl.cc"
    break;

  case 100: /* $@12: %empty  */
#line 444 "pddl.yy"
                { effect_time = Effect::AT_END; }
#line 2379 "pddl.cc"
    break;

  case 102: /* da_body: CONDITION da_gd da_body2  */
#line 447 "pddl.yy"
                                   { action->set_condition(*(yyvsp[-1].formula)); }
#line 2385 "pddl.cc"
    break;

  case 107: /* duration_constraint: '(' and simple_duration_constraints ')'  */
#line 461 "pddl.yy"
                        { require_duration_inequalities(); }
#line 2391 "pddl.cc"
    break;

  case 108: /* simple_duration_constraint: '(' LE duration_var f_exp ')'  */
#line 465 "pddl.yy"
                               {
				 require_duration_inequalities();
				 action->set_max_duration(*(yyvsp[-1].expr));
			       }
#line 2400 "pddl.cc"
    break;

  case 109: /* simple_duration_constraint: '(' GE duration_var f_exp ')'  */
#line 470 "pddl.yy"
                               {
				 require_duration_inequalities();
				 action->set_min_duration(*(yyvsp[-1].expr));
			       }
#line 2409 "pddl.cc"
    break;

  case 110: /* simple_duration_constraint: '(' '=' duration_var f_exp ')'  */
#line 475 "pddl.yy"
                               { action->set_duration(*(yyvsp[-1].expr)); }
#line 2415 "pddl.cc"
    break;

  case 114: /* da_gd: '(' and timed_gds ')'  */
#line 488 "pddl.yy"
                              { (yyval.formula) = (yyvsp[-1].formula); }
#line 2421 "pddl.cc"
    break;

  case 115: /* timed_gds: %empty  */
#line 491 "pddl.yy"
                        { (yyval.formula) = &Formula::TRUE; }
#line 2427 "pddl.cc"
    break;

  case 116: /* timed_gds: timed_gds timed_gd  */
#line 492 "pddl.yy"
                               { (yyval.formula) = &(*(yyvsp[-1].formula) && *(yyvsp[0].formula)); }
#line 2433 "pddl.cc"
    break;

  case 117: /* $@13: %empty  */
#line 495 "pddl.yy"
                        { formula_time = AT_START; }
#line 2439 "pddl.cc"
    break;

  case 118: /* timed_gd: '(' at start $@13 formula ')'  */
#line 495 "pddl.yy"
                                                                 { (yyval.formula) = (yyvsp[-1].formula); }
#line 2445 "pddl.cc"
    break;

  case 119: /* $@14: %empty  */
#line 496 "pddl.yy"
                      { formula_time = AT_END; }
#line 2451 "pddl.cc"
    break;

  case 120: /* timed_gd: '(' at end $@14 formula ')'  */
#line 496 "pddl.yy"
                                                             { (yyval.formula) = (yyvsp[-1].formula); }
#line 2457 "pddl.cc"
    break;

  case 121: /* $@15: %empty  */
#line 497 "pddl.yy"
                        { formula_time = OVER_ALL; }
#line 2463 "pddl.cc"
    break;

  case 122: /* timed_gd: '(' over all $@15 formula ')'  */
#line 497 "pddl.yy"
                                                                 { (yyval.formula) = (yyvsp[-1].formula); }
#line 2469 "pddl.cc"
    break;

  case 125: /* $@16: %empty  */
#line 506 "pddl.yy"
                         { prepare_forall_effect(); }
#line 2475 "pddl.cc"
    break;

  case 126: /* eff_formula: '(' forall $@16 '(' variables ')' eff_formula ')'  */
#line 507 "pddl.yy"
                                                  { pop_forall_effect(); }
#line 2481 "pddl.cc"
    break;

  case 127: /* $@17: %empty  */
#line 508 "pddl.yy"
                       { formula_time = AT_START; }
#line 2487 "pddl.cc"
    break;

  case 128: /* $@18: %empty  */
#line 509 "pddl.yy"
                { prepare_conditional_effect(*(yyvsp[0].formula)); }
#line 2493 "pddl.cc"
    break;

  case 129: /* eff_formula: '(' when $@17 formula $@18 one_eff_formula ')'  */
#line 510 "pddl.yy"
                                    { effect_condition = 0; }
#line 2499 "pddl.cc"
    break;

  case 134: /* term_literal: atomic_term_formula  */
#line 521 "pddl.yy"
                                   { add_effect(*(yyvsp[0].atom)); }
#line 2505 "pddl.cc"
    break;

  case 135: /* term_literal: '(' not atomic_term_formula ')'  */
#line 523 "pddl.yy"
                 { add_effect(Negation::make(*(yyvsp[-1].atom))); }
#line 2511 "pddl.cc"
    break;

  case 140: /* $@19: %empty  */
#line 532 "pddl.yy"
                       { prepare_forall_effect(); }
#line 2517 "pddl.cc"
    break;

  case 141: /* da_effect: '(' forall $@19 '(' variables ')' da_effect ')'  */
#line 533 "pddl.yy"
                                              { pop_forall_effect(); }
#line 2523 "pddl.cc"
    break;

  case 142: /* $@20: %empty  */
#line 534 "pddl.yy"
                           { prepare_conditional_effect(*(yyvsp[0].formula)); }
#line 2529 "pddl.cc"
    break;

  case 143: /* da_effect: '(' when da_gd $@20 timed_effect ')'  */
#line 535 "pddl.yy"
                               { effect_condition = 0; }
#line 2535 "pddl.cc"
    break;

  case 146: /* $@21: %empty  */
#line 543 "pddl.yy"
                 { effect_time = Effect::AT_START; formula_time = AT_START; }
#line 2541 "pddl.cc"
    break;

  case 148: /* $@22: %empty  */
#line 546 "pddl.yy"
                 { effect_time = Effect::AT_END; formula_time = AT_END; }
#line 2547 "pddl.cc"
    break;

  case 152: /* $@23: %empty  */
#line 552 "pddl.yy"
                      { prepare_forall_effect(); }
#line 2553 "pddl.cc"
    break;

  case 153: /* a_effect: '(' forall $@23 '(' variables ')' a_effect ')'  */
#line 553 "pddl.yy"
                                            { pop_forall_effect(); }
#line 2559 "pddl.cc"
    break;

  case 154: /* $@24: %empty  */
#line 554 "pddl.yy"
                            { prepare_conditional_effect(*(yyvsp[0].formula)); }
#line 2565 "pddl.cc"
    break;

  case 155: /* a_effect: '(' when formula $@24 one_eff_formula ')'  */
#line 555 "pddl.yy"
                                 { effect_condition = 0; }
#line 2571 "pddl.cc"
    break;

  case 158: /* $@25: %empty  */
#line 567 "pddl.yy"
                { make_problem((yyvsp[-5].str), (yyvsp[-1].str)); }
#line 2577 "pddl.cc"
    break;

  case 159: /* problem_def: '(' define '(' problem name ')' '(' PDOMAIN name ')' $@25 problem_body ')'  */
#line 568 "pddl.yy"
                { delete requirements; }
#line 2583 "pddl.cc"
    break;

  case 166: /* $@26: %empty  */
#line 583 "pddl.yy"
                          { name_kind = OBJECT_KIND; }
#line 2589 "pddl.cc"
    break;

  case 167: /* object_decl: '(' OBJECTS $@26 typed_names ')'  */
#line 584 "pddl.yy"
                { name_kind = VOID_KIND; }
#line 2595 "pddl.cc"
    break;

  case 171: /* $@27: %empty  */
#line 594 "pddl.yy"
                                  { prepare_atom((yyvsp[0].str)); }
#line 2601 "pddl.cc"
    break;

  case 172: /* init_element: '(' init_predicate $@27 names ')'  */
#line 595 "pddl.yy"
                 { problem->add_init_atom(*make_atom()); }
#line 2607 "pddl.cc"
    break;

  case 173: /* $@28: %empty  */
#line 596 "pddl.yy"
                      { prepare_atom((yyvsp[0].str)); }
#line 2613 "pddl.cc"
    break;

  case 174: /* init_element: '(' AT $@28 names ')'  */
#line 597 "pddl.yy"
                 { problem->add_init_atom(*make_atom()); }
#line 2619 "pddl.cc"
    break;

  case 175: /* init_element: '(' not atomic_name_formula ')'  */
#line 599 "pddl.yy"
                 { Formula::register_use((yyvsp[-1].atom)); Formula::unregister_use((yyvsp[-1].atom)); }
#line 2625 "pddl.cc"
    break;

  case 176: /* init_element: '(' '=' ground_f_head NUMBER ')'  */
#line 601 "pddl.yy"
                 { problem->add_init_value(*(yyvsp[-2].fluent), (yyvsp[-1].num)); }
#line 2631 "pddl.cc"
    break;

  case 177: /* init_element: '(' at NUMBER name_literal ')'  */
#line 603 "pddl.yy"
                 { add_init_literal((yyvsp[-2].num), *(yyvsp[-1].literal)); }
#line 2637 "pddl.cc"
    break;

  case 180: /* goal: '(' GOAL formula ')'  */
#line 610 "pddl.yy"
                            { problem->set_goal(*(yyvsp[-1].formula)); }
#line 2643 "pddl.cc"
    break;

  case 181: /* $@29: %empty  */
#line 613 "pddl.yy"
                                  { metric_fluent = true; }
#line 2649 "pddl.cc"
    break;

  case 182: /* metric_spec: '(' METRIC maximize $@29 ground_f_exp ')'  */
#line 614 "pddl.yy"
                { problem->set_metric(*(yyvsp[-1].expr), true); metric_fluent = false; }
#line 2655 "pddl.cc"
    break;

  case 183: /* $@30: %empty  */
#line 615 "pddl.yy"
                                  { metric_fluent = true; }
#line 2661 "pddl.cc"
    break;

  case 184: /* metric_spec: '(' METRIC minimize $@30 ground_f_exp ')'  */
#line 616 "pddl.yy"
                { problem->set_metric(*(yyvsp[-1].expr)); metric_fluent = false; }
#line 2667 "pddl.cc"
    break;

  case 185: /* formula: atomic_term_formula  */
#line 623 "pddl.yy"
                              { (yyval.formula) = &TimedLiteral::make(*(yyvsp[0].atom), formula_time); }
#line 2673 "pddl.cc"
    break;

  case 186: /* formula: '(' '=' term term ')'  */
#line 624 "pddl.yy"
                                { (yyval.formula) = make_equality((yyvsp[-2].term), (yyvsp[-1].term)); }
#line 2679 "pddl.cc"
    break;

  case 187: /* formula: '(' not formula ')'  */
#line 625 "pddl.yy"
                              { (yyval.formula) = make_negation(*(yyvsp[-1].formula)); }
#line 2685 "pddl.cc"
    break;

  case 188: /* formula: '(' and conjuncts ')'  */
#line 626 "pddl.yy"
                                { (yyval.formula) = (yyvsp[-1].formula); }
#line 2691 "pddl.cc"
    break;

  case 189: /* $@31: %empty  */
#line 627 "pddl.yy"
                 { require_disjunction(); }
#line 2697 "pddl.cc"
    break;

  case 190: /* formula: '(' or $@31 disjuncts ')'  */
#line 627 "pddl.yy"
                                                          { (yyval.formula) = (yyvsp[-1].formula); }
#line 2703 "pddl.cc"
    break;

  case 191: /* $@32: %empty  */
#line 628 "pddl.yy"
                    { require_disjunction(); }
#line 2709 "pddl.cc"
    break;

  case 192: /* formula: '(' imply $@32 formula formula ')'  */
#line 629 "pddl.yy"
            { (yyval.formula) = &(!*(yyvsp[-2].formula) || *(yyvsp[-1].formula)); }
#line 2715 "pddl.cc"
    break;

  case 193: /* $@33: %empty  */
#line 630 "pddl.yy"
                     { prepare_exists(); }
#line 2721 "pddl.cc"
    break;

  case 194: /* formula: '(' exists $@33 '(' variables ')' formula ')'  */
#line 631 "pddl.yy"
            { (yyval.formula) = make_exists(*(yyvsp[-1].formula)); }
#line 2727 "pddl.cc"
    break;

  case 195: /* $@34: %empty  */
#line 632 "pddl.yy"
                     { prepare_forall(); }
#line 2733 "pddl.cc"
    break;

  case 196: /* formula: '(' forall $@34 '(' variables ')' formula ')'  */
#line 633 "pddl.yy"
            { (yyval.formula) = make_forall(*(yyvsp[-1].formula)); }
#line 2739 "pddl.cc"
    break;

  case 197: /* conjuncts: %empty  */
#line 636 "pddl.yy"
                        { (yyval.formula) = &Formula::TRUE; }
#line 2745 "pddl.cc"
    break;

  case 198: /* conjuncts: conjuncts formula  */
#line 637 "pddl.yy"
                              { (yyval.formula) = &(*(yyvsp[-1].formula) && *(yyvsp[0].formula)); }
#line 2751 "pddl.cc"
    break;

  case 199: /* disjuncts: %empty  */
#line 640 "pddl.yy"
                        { (yyval.formula) = &Formula::FALSE; }
#line 2757 "pddl.cc"
    break;

  case 200: /* disjuncts: disjuncts formula  */
#line 641 "pddl.yy"
                              { (yyval.formula) = &(*(yyvsp[-1].formula) || *(yyvsp[0].formula)); }
#line 2763 "pddl.cc"
    break;

  case 201: /* $@35: %empty  */
#line 644 "pddl.yy"
                                    { prepare_atom((yyvsp[0].str)); }
#line 2769 "pddl.cc"
    break;

  case 202: /* atomic_term_formula: '(' predicate $@35 terms ')'  */
#line 645 "pddl.yy"
                        { (yyval.atom) = make_atom(); }
#line 2775 "pddl.cc"
    break;

  case 203: /* $@36: %empty  */
#line 648 "pddl.yy"
                                    { prepare_atom((yyvsp[0].str)); }
#line 2781 "pddl.cc"
    break;

  case 204: /* atomic_name_formula: '(' predicate $@36 names ')'  */
#line 649 "pddl.yy"
                        { (yyval.atom) = make_atom(); }
#line 2787 "pddl.cc"
    break;

  case 205: /* name_literal: atomic_name_formula  */
#line 652 "pddl.yy"
                                   { (yyval.literal) = (yyvsp[0].atom); }
#line 2793 "pddl.cc"
    break;

  case 206: /* name_literal: '(' not atomic_name_formula ')'  */
#line 653 "pddl.yy"
                                               { (yyval.literal) = &Negation::make(*(yyvsp[-1].atom)); }
#line 2799 "pddl.cc"
    break;

  case 207: /* f_exp: NUMBER  */
#line 660 "pddl.yy"
               { (yyval.expr) = new Value((yyvsp[0].num)); }
#line 2805 "pddl.cc"
    break;

  case 208: /* f_exp: '(' '+' f_exp f_exp ')'  */
#line 661 "pddl.yy"
                                { (yyval.expr) = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2811 "pddl.cc"
    break;

  case 209: /* f_exp: '(' '-' f_exp opt_f_exp ')'  */
#line 662 "pddl.yy"
                                    { (yyval.expr) = make_subtraction(*(yyvsp[-2].expr), (yyvsp[-1].expr)); }
#line 2817 "pddl.cc"
    break;

  case 210: /* f_exp: '(' '*' f_exp f_exp ')'  */
#line 663 "pddl.yy"
                                { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2823 "pddl.cc"
    break;

  case 211: /* f_exp: '(' '/' f_exp f_exp ')'  */
#line 664 "pddl.yy"
                                { (yyval.expr) = &Division::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2829 "pddl.cc"
    break;

  case 212: /* f_exp: f_head  */
#line 665 "pddl.yy"
               { (yyval.expr) = (yyvsp[0].fluent); }
#line 2835 "pddl.cc"
    break;

  case 213: /* opt_f_exp: %empty  */
#line 668 "pddl.yy"
                        { (yyval.expr) = 0; }
#line 2841 "pddl.cc"
    break;

  case 215: /* $@37: %empty  */
#line 672 "pddl.yy"
                      { prepare_fluent((yyvsp[0].str)); }
#line 2847 "pddl.cc"
    break;

  case 216: /* f_head: '(' function $@37 terms ')'  */
#line 673 "pddl.yy"
           { (yyval.fluent) = make_fluent(); }
#line 2853 "pddl.cc"
    break;

  case 217: /* f_head: function  */
#line 674 "pddl.yy"
                  { prepare_fluent((yyvsp[0].str)); (yyval.fluent) = make_fluent(); }
#line 2859 "pddl.cc"
    break;

  case 218: /* ground_f_exp: NUMBER  */
#line 677 "pddl.yy"
                      { (yyval.expr) = new Value((yyvsp[0].num)); }
#line 2865 "pddl.cc"
    break;

  case 219: /* ground_f_exp: '(' '+' ground_f_exp ground_f_exp ')'  */
#line 679 "pddl.yy"
                 { (yyval.expr) = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2871 "pddl.cc"
    break;

  case 220: /* ground_f_exp: '(' '-' ground_f_exp opt_ground_f_exp ')'  */
#line 681 "pddl.yy"
                 { (yyval.expr) = make_subtraction(*(yyvsp[-2].expr), (yyvsp[-1].expr)); }
#line 2877 "pddl.cc"
    break;

  case 221: /* ground_f_exp: '(' '*' ground_f_exp ground_f_exp ')'  */
#line 683 "pddl.yy"
                 { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2883 "pddl.cc"
    break;

  case 222: /* ground_f_exp: '(' '/' ground_f_exp ground_f_exp ')'  */
#line 685 "pddl.yy"
                 { (yyval.expr) = &Division::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
#line 2889 "pddl.cc"
    break;

  case 223: /* ground_f_exp: ground_f_head  */
#line 686 "pddl.yy"
                             { (yyval.expr) = (yyvsp[0].fluent); }
#line 2895 "pddl.cc"
    break;

  case 224: /* opt_ground_f_exp: %empty  */
#line 689 "pddl.yy"
                               { (yyval.expr) = 0; }
#line 2901 "pddl.cc"
    break;

  case 226: /* $@38: %empty  */
#line 693 "pddl.yy"
                             { prepare_fluent((yyvsp[0].str)); }
#line 2907 "pddl.cc"
    break;

  case 227: /* ground_f_head: '(' function $@38 names ')'  */
#line 694 "pddl.yy"
                  { (yyval.fluent) = make_fluent(); }
#line 2913 "pddl.cc"
    break;

  case 228: /* ground_f_head: function  */
#line 695 "pddl.yy"
                         { prepare_fluent((yyvsp[0].str)); (yyval.fluent) = make_fluent(); }
#line 2919 "pddl.cc"
    break;

  case 230: /* terms: terms name  */
#line 703 "pddl.yy"
                   { add_term((yyvsp[0].str)); }
#line 2925 "pddl.cc"
    break;

  case 231: /* terms: terms variable  */
#line 704 "pddl.yy"
                       { add_term((yyvsp[0].str)); }
#line 2931 "pddl.cc"
    break;

  case 233: /* names: names name  */
#line 708 "pddl.yy"
                   { add_term((yyvsp[0].str)); }
#line 2937 "pddl.cc"
    break;

  case 234: /* term: name  */
#line 711 "pddl.yy"
            { (yyval.term) = new Term(make_term((yyvsp[0].str))); }
#line 2943 "pddl.cc"
    break;

  case 235: /* term: variable  */
#line 712 "pddl.yy"
                { (yyval.term) = new Term(make_term((yyvsp[0].str))); }
#line 2949 "pddl.cc"
    break;

  case 237: /* variables: variable_seq  */
#line 716 "pddl.yy"
                         { add_variables((yyvsp[0].strs), TypeTable::OBJECT); }
#line 2955 "pddl.cc"
    break;

  case 238: /* $@39: %empty  */
#line 717 "pddl.yy"
                                   { add_variables((yyvsp[-1].strs), *(yyvsp[0].type)); delete (yyvsp[0].type); }
#line 2961 "pddl.cc"
    break;

  case 240: /* variable_seq: variable  */
#line 721 "pddl.yy"
                        { (yyval.strs) = new std::vector<const std::string*>(1, (yyvsp[0].str)); }
#line 2967 "pddl.cc"
    break;

  case 241: /* variable_seq: variable_seq variable  */
#line 722 "pddl.yy"
                                     { (yyval.strs) = (yyvsp[-1].strs); (yyval.strs)->push_back((yyvsp[0].str)); }
#line 2973 "pddl.cc"
    break;

  case 243: /* typed_names: name_seq  */
#line 726 "pddl.yy"
                       { add_names((yyvsp[0].strs), TypeTable::OBJECT); }
#line 2979 "pddl.cc"
    break;

  case 244: /* $@40: %empty  */
#line 727 "pddl.yy"
                                 { add_names((yyvsp[-1].strs), *(yyvsp[0].type)); delete (yyvsp[0].type); }
#line 2985 "pddl.cc"
    break;

  case 246: /* name_seq: name  */
#line 730 "pddl.yy"
                { (yyval.strs) = new std::vector<const std::string*>(1, (yyvsp[0].str)); }
#line 2991 "pddl.cc"
    break;

  case 247: /* name_seq: name_seq name  */
#line 731 "pddl.yy"
                         { (yyval.strs) = (yyvsp[-1].strs); (yyval.strs)->push_back((yyvsp[0].str)); }
#line 2997 "pddl.cc"
    break;

  case 248: /* $@41: %empty  */
#line 734 "pddl.yy"
                { require_typing(); }
#line 3003 "pddl.cc"
    break;

  case 249: /* type_spec: '-' $@41 type  */
#line 734 "pddl.yy"
                                           { (yyval.type) = (yyvsp[0].type); }
#line 3009 "pddl.cc"
    break;

  case 250: /* type: object  */
#line 737 "pddl.yy"
              { (yyval.type) = new Type(TypeTable::OBJECT); }
#line 3015 "pddl.cc"
    break;

  case 251: /* type: type_name  */
#line 738 "pddl.yy"
                 { (yyval.type) = new Type(make_type((yyvsp[0].str))); }
#line 3021 "pddl.cc"
    break;

  case 252: /* type: '(' either types ')'  */
#line 739 "pddl.yy"
                            { (yyval.type) = new Type(make_type(*(yyvsp[-1].types))); delete (yyvsp[-1].types); }
#line 3027 "pddl.cc"
    break;

  case 253: /* types: object  */
#line 742 "pddl.yy"
               { (yyval.types) = new std::set<Type>(); }
#line 3033 "pddl.cc"
    break;

  case 254: /* types: type_name  */
#line 743 "pddl.yy"
                  { (yyval.types) = new std::set<Type>(); (yyval.types)->insert(make_type((yyvsp[0].str))); }
#line 3039 "pddl.cc"
    break;

  case 255: /* types: types object  */
#line 744 "pddl.yy"
                     { (yyval.types) = (yyvsp[-1].types); }
#line 3045 "pddl.cc"
    break;

  case 256: /* types: types type_name  */
#line 745 "pddl.yy"
                        { (yyval.types) = (yyvsp[-1].types); (yyval.types)->insert(make_type((yyvsp[0].str))); }
#line 3051 "pddl.cc"
    break;

  case 258: /* define: DEFINE  */
#line 755 "pddl.yy"
                { delete (yyvsp[0].str); }
#line 3057 "pddl.cc"
    break;

  case 259: /* domain: DOMAIN_TOKEN  */
#line 758 "pddl.yy"
                      { delete (yyvsp[0].str); }
#line 3063 "pddl.cc"
    break;

  case 260: /* problem: PROBLEM  */
#line 761 "pddl.yy"
                  { delete (yyvsp[0].str); }
#line 3069 "pddl.cc"
    break;

  case 261: /* when: WHEN  */
#line 764 "pddl.yy"
            { delete (yyvsp[0].str); }
#line 3075 "pddl.cc"
    break;

  case 262: /* not: NOT  */
#line 767 "pddl.yy"
          { delete (yyvsp[0].str); }
#line 3081 "pddl.cc"
    break;

  case 263: /* and: AND  */
#line 770 "pddl.yy"
          { delete (yyvsp[0].str); }
#line 3087 "pddl.cc"
    break;

  case 264: /* or: OR  */
#line 773 "pddl.yy"
        { delete (yyvsp[0].str); }
#line 3093 "pddl.cc"
    break;

  case 265: /* imply: IMPLY  */
#line 776 "pddl.yy"
              { delete (yyvsp[0].str); }
#line 3099 "pddl.cc"
    break;

  case 266: /* exists: EXISTS  */
#line 779 "pddl.yy"
                { delete (yyvsp[0].str); }
#line 3105 "pddl.cc"
    break;

  case 267: /* forall: FORALL  */
#line 782 "pddl.yy"
                { delete (yyvsp[0].str); }
#line 3111 "pddl.cc"
    break;

  case 268: /* at: AT  */
#line 785 "pddl.yy"
        { delete (yyvsp[0].str); }
#line 3117 "pddl.cc"
    break;

  case 269: /* over: OVER  */
#line 788 "pddl.yy"
            { delete (yyvsp[0].str); }
#line 3123 "pddl.cc"
    break;

  case 270: /* start: START  */
#line 791 "pddl.yy"
              { delete (yyvsp[0].str); }
#line 3129 "pddl.cc"
    break;

  case 271: /* end: END  */
#line 794 "pddl.yy"
          { delete (yyvsp[0].str); }
#line 3135 "pddl.cc"
    break;

  case 272: /* all: ALL  */
#line 797 "pddl.yy"
          { delete (yyvsp[0].str); }
#line 3141 "pddl.cc"
    break;

  case 273: /* duration_var: DURATION_VAR  */
#line 800 "pddl.yy"
                            { delete (yyvsp[0].str); }
#line 3147 "pddl.cc"
    break;

  case 274: /* minimize: MINIMIZE  */
#line 803 "pddl.yy"
                    { delete (yyvsp[0].str); }
#line 3153 "pddl.cc"
    break;

  case 275: /* maximize: MAXIMIZE  */
#line 806 "pddl.yy"
                    { delete (yyvsp[0].str); }
#line 3159 "pddl.cc"
    break;

  case 276: /* number: NUMBER_TOKEN  */
#line 809 "pddl.yy"
                      { delete (yyvsp[0].str); }
#line 3165 "pddl.cc"
    break;

  case 277: /* object: OBJECT_TOKEN  */
#line 812 "pddl.yy"
                      { delete (yyvsp[0].str); }
#line 3171 "pddl.cc"
    break;

  case 278: /* either: EITHER  */
#line 815 "pddl.yy"
                { delete (yyvsp[0].str); }
#line 3177 "pddl.cc"
    break;


#line 3181 "pddl.cc"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 851 "pddl.yy"


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
