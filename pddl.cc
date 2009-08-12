/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DEFINE = 258,
     DOMAIN_TOKEN = 259,
     PROBLEM = 260,
     REQUIREMENTS = 261,
     TYPES = 262,
     CONSTANTS = 263,
     PREDICATES = 264,
     FUNCTIONS = 265,
     STRIPS = 266,
     TYPING = 267,
     NEGATIVE_PRECONDITIONS = 268,
     DISJUNCTIVE_PRECONDITIONS = 269,
     EQUALITY = 270,
     EXISTENTIAL_PRECONDITIONS = 271,
     UNIVERSAL_PRECONDITIONS = 272,
     QUANTIFIED_PRECONDITIONS = 273,
     CONDITIONAL_EFFECTS = 274,
     FLUENTS = 275,
     ADL = 276,
     DURATIVE_ACTIONS = 277,
     DURATION_INEQUALITIES = 278,
     CONTINUOUS_EFFECTS = 279,
     TIMED_INITIAL_LITERALS = 280,
     ACTION = 281,
     PARAMETERS = 282,
     PRECONDITION = 283,
     EFFECT = 284,
     DURATIVE_ACTION = 285,
     DURATION = 286,
     CONDITION = 287,
     PDOMAIN = 288,
     OBJECTS = 289,
     INIT = 290,
     GOAL = 291,
     METRIC = 292,
     WHEN = 293,
     NOT = 294,
     AND = 295,
     OR = 296,
     IMPLY = 297,
     EXISTS = 298,
     FORALL = 299,
     AT = 300,
     OVER = 301,
     START = 302,
     END = 303,
     ALL = 304,
     MINIMIZE = 305,
     MAXIMIZE = 306,
     TOTAL_TIME = 307,
     NUMBER_TOKEN = 308,
     OBJECT_TOKEN = 309,
     EITHER = 310,
     LE = 311,
     GE = 312,
     NAME = 313,
     DURATION_VAR = 314,
     VARIABLE = 315,
     NUMBER = 316,
     ILLEGAL_TOKEN = 317
   };
#endif
#define DEFINE 258
#define DOMAIN_TOKEN 259
#define PROBLEM 260
#define REQUIREMENTS 261
#define TYPES 262
#define CONSTANTS 263
#define PREDICATES 264
#define FUNCTIONS 265
#define STRIPS 266
#define TYPING 267
#define NEGATIVE_PRECONDITIONS 268
#define DISJUNCTIVE_PRECONDITIONS 269
#define EQUALITY 270
#define EXISTENTIAL_PRECONDITIONS 271
#define UNIVERSAL_PRECONDITIONS 272
#define QUANTIFIED_PRECONDITIONS 273
#define CONDITIONAL_EFFECTS 274
#define FLUENTS 275
#define ADL 276
#define DURATIVE_ACTIONS 277
#define DURATION_INEQUALITIES 278
#define CONTINUOUS_EFFECTS 279
#define TIMED_INITIAL_LITERALS 280
#define ACTION 281
#define PARAMETERS 282
#define PRECONDITION 283
#define EFFECT 284
#define DURATIVE_ACTION 285
#define DURATION 286
#define CONDITION 287
#define PDOMAIN 288
#define OBJECTS 289
#define INIT 290
#define GOAL 291
#define METRIC 292
#define WHEN 293
#define NOT 294
#define AND 295
#define OR 296
#define IMPLY 297
#define EXISTS 298
#define FORALL 299
#define AT 300
#define OVER 301
#define START 302
#define END 303
#define ALL 304
#define MINIMIZE 305
#define MAXIMIZE 306
#define TOTAL_TIME 307
#define NUMBER_TOKEN 308
#define OBJECT_TOKEN 309
#define EITHER 310
#define LE 311
#define GE 312
#define NAME 313
#define DURATION_VAR 314
#define VARIABLE 315
#define NUMBER 316
#define ILLEGAL_TOKEN 317




/* Copy the first part of user declarations.  */
#line 21 "pddl.yy"

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
static TermList term_parameters;
/* Quantified variables for effect or formula being parsed. */
static TermList quantified;
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


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 227 "pddl.yy"
typedef union YYSTYPE {
  const Formula* formula;
  const Literal* literal;
  const Atom* atom;
  const Expression* expr;
  const Fluent* fluent;
  const Term* term;
  const Type* type;
  TypeSet* types;
  const std::string* str;
  std::vector<const std::string*>* strs;
  float num;
} YYSTYPE;
/* Line 190 of yacc.c.  */
#line 403 "pddl.cc"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 213 of yacc.c.  */
#line 415 "pddl.cc"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1061

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  154
/* YYNRULES -- Number of rules. */
#define YYNRULES  332
/* YYNRULES -- Number of states. */
#define YYNSTATES  545

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   317

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     4,     7,     8,    11,    14,    15,    25,
      26,    28,    31,    33,    35,    38,    40,    42,    44,    46,
      49,    52,    55,    57,    59,    61,    64,    67,    69,    71,
      73,    76,    79,    81,    83,    85,    88,    91,    93,    95,
      98,   100,   102,   105,   107,   109,   112,   114,   116,   119,
     121,   126,   128,   131,   133,   135,   137,   139,   141,   143,
     145,   147,   149,   151,   153,   155,   157,   159,   161,   162,
     168,   169,   175,   180,   181,   187,   188,   191,   192,   198,
     199,   201,   205,   207,   210,   211,   215,   216,   222,   223,
     231,   232,   242,   243,   248,   251,   253,   254,   256,   257,
     261,   262,   266,   270,   272,   273,   276,   278,   283,   289,
     295,   301,   302,   305,   307,   312,   313,   316,   317,   324,
     325,   332,   333,   340,   342,   347,   348,   357,   358,   359,
     367,   368,   371,   373,   378,   380,   385,   386,   389,   391,
     396,   397,   406,   407,   414,   415,   418,   419,   426,   427,
     434,   436,   441,   442,   451,   452,   459,   460,   463,   464,
     478,   481,   483,   486,   488,   491,   493,   494,   500,   505,
     506,   509,   510,   516,   517,   523,   528,   534,   540,   542,
     545,   550,   551,   558,   559,   566,   568,   574,   579,   584,
     585,   591,   592,   599,   600,   609,   610,   619,   620,   623,
     624,   627,   628,   634,   635,   641,   643,   648,   650,   656,
     662,   668,   674,   676,   677,   679,   680,   686,   688,   690,
     696,   702,   708,   714,   716,   717,   719,   720,   726,   728,
     729,   732,   735,   736,   739,   741,   743,   744,   746,   747,
     752,   754,   757,   758,   760,   761,   766,   768,   771,   772,
     776,   778,   780,   785,   787,   789,   792,   795,   797,   799,
     801,   803,   805,   807,   809,   811,   813,   815,   817,   819,
     821,   823,   825,   827,   829,   831,   833,   835,   837,   839,
     841,   843,   845,   847,   849,   851,   853,   855,   857,   859,
     861,   863,   865,   867,   869,   871,   873,   875,   877,   879,
     881,   883,   885,   887,   889,   891,   893,   895,   897,   899,
     901,   903,   905,   907,   909,   911,   913,   915,   917,   919,
     921,   923,   925,   927,   929,   931,   933,   935,   937,   939,
     941,   943,   945
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
      71,     0,    -1,    -1,    72,    73,    -1,    -1,    73,    74,
      -1,    73,   146,    -1,    -1,    63,   197,    63,   198,   222,
      64,    75,    76,    64,    -1,    -1,    87,    -1,    87,    77,
      -1,    77,    -1,    90,    -1,    90,    78,    -1,    78,    -1,
      92,    -1,    94,    -1,    95,    -1,    92,    79,    -1,    94,
      80,    -1,    95,    81,    -1,    85,    -1,    94,    -1,    95,
      -1,    94,    82,    -1,    95,    83,    -1,    85,    -1,    92,
      -1,    95,    -1,    92,    82,    -1,    95,    84,    -1,    85,
      -1,    92,    -1,    94,    -1,    92,    83,    -1,    94,    84,
      -1,    85,    -1,    95,    -1,    95,    85,    -1,    85,    -1,
      94,    -1,    94,    85,    -1,    85,    -1,    92,    -1,    92,
      85,    -1,    85,    -1,    86,    -1,    85,    86,    -1,   106,
      -1,    63,     6,    88,    64,    -1,    89,    -1,    88,    89,
      -1,    11,    -1,    12,    -1,    13,    -1,    14,    -1,    15,
      -1,    16,    -1,    17,    -1,    18,    -1,    19,    -1,    20,
      -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,    25,
      -1,    -1,    63,     7,    91,   189,    64,    -1,    -1,    63,
       8,    93,   189,    64,    -1,    63,     9,    97,    64,    -1,
      -1,    63,    10,    96,   100,    64,    -1,    -1,    97,    98,
      -1,    -1,    63,   219,    99,   186,    64,    -1,    -1,   101,
      -1,   101,   102,   100,    -1,   104,    -1,   101,   104,    -1,
      -1,    65,   103,   196,    -1,    -1,    63,   221,   105,   186,
      64,    -1,    -1,    63,    26,   222,   107,   109,   110,    64,
      -1,    -1,    63,    30,   222,   108,   109,    31,   118,   116,
      64,    -1,    -1,    27,    63,   186,    64,    -1,   112,   111,
      -1,   111,    -1,    -1,   114,    -1,    -1,    28,   113,   163,
      -1,    -1,    29,   115,   127,    -1,    32,   121,   117,    -1,
     117,    -1,    -1,    29,   135,    -1,   119,    -1,    63,   202,
     120,    64,    -1,    63,    56,   212,   175,    64,    -1,    63,
      57,   212,   175,    64,    -1,    63,    66,   212,   175,    64,
      -1,    -1,   120,   119,    -1,   123,    -1,    63,   202,   122,
      64,    -1,    -1,   122,   123,    -1,    -1,    63,   207,   209,
     124,   163,    64,    -1,    -1,    63,   207,   210,   125,   163,
      64,    -1,    -1,    63,   208,   211,   126,   163,    64,    -1,
     133,    -1,    63,   202,   131,    64,    -1,    -1,    63,   206,
     128,    63,   186,    64,   127,    64,    -1,    -1,    -1,    63,
     200,   129,   163,   130,   132,    64,    -1,    -1,   131,   127,
      -1,   133,    -1,    63,   202,   134,    64,    -1,   170,    -1,
      63,   201,   170,    64,    -1,    -1,   134,   133,    -1,   139,
      -1,    63,   202,   138,    64,    -1,    -1,    63,   206,   136,
      63,   186,    64,   135,    64,    -1,    -1,    63,   200,   121,
     137,   139,    64,    -1,    -1,   138,   135,    -1,    -1,    63,
     207,   209,   140,   142,    64,    -1,    -1,    63,   207,   210,
     141,   142,    64,    -1,   133,    -1,    63,   202,   145,    64,
      -1,    -1,    63,   206,   143,    63,   186,    64,   142,    64,
      -1,    -1,    63,   200,   163,   144,   132,    64,    -1,    -1,
     145,   142,    -1,    -1,    63,   197,    63,   199,   222,    64,
      63,    33,   222,    64,   147,   148,    64,    -1,    87,   149,
      -1,   149,    -1,   151,   150,    -1,   150,    -1,   153,   158,
      -1,   158,    -1,    -1,    63,    34,   152,   189,    64,    -1,
      63,    35,   154,    64,    -1,    -1,   154,   155,    -1,    -1,
      63,   220,   156,   184,    64,    -1,    -1,    63,    45,   157,
     184,    64,    -1,    63,   201,   172,    64,    -1,    63,    66,
     181,    61,    64,    -1,    63,   207,    61,   174,    64,    -1,
     159,    -1,   159,   160,    -1,    63,    36,   163,    64,    -1,
      -1,    63,    37,   214,   161,   179,    64,    -1,    -1,    63,
      37,   213,   162,   179,    64,    -1,   170,    -1,    63,    66,
     185,   185,    64,    -1,    63,   201,   163,    64,    -1,    63,
     202,   168,    64,    -1,    -1,    63,   203,   164,   169,    64,
      -1,    -1,    63,   204,   165,   163,   163,    64,    -1,    -1,
      63,   205,   166,    63,   186,    64,   163,    64,    -1,    -1,
      63,   206,   167,    63,   186,    64,   163,    64,    -1,    -1,
     168,   163,    -1,    -1,   169,   163,    -1,    -1,    63,   219,
     171,   183,    64,    -1,    -1,    63,   219,   173,   184,    64,
      -1,   172,    -1,    63,   201,   172,    64,    -1,    61,    -1,
      63,    67,   175,   175,    64,    -1,    63,    65,   175,   176,
      64,    -1,    63,    68,   175,   175,    64,    -1,    63,    69,
     175,   175,    64,    -1,   177,    -1,    -1,   175,    -1,    -1,
      63,   221,   178,   183,    64,    -1,   221,    -1,    61,    -1,
      63,    67,   179,   179,    64,    -1,    63,    65,   179,   180,
      64,    -1,    63,    68,   179,   179,    64,    -1,    63,    69,
     179,   179,    64,    -1,   181,    -1,    -1,   179,    -1,    -1,
      63,   221,   182,   184,    64,    -1,   221,    -1,    -1,   183,
     222,    -1,   183,   223,    -1,    -1,   184,   222,    -1,   222,
      -1,   223,    -1,    -1,   188,    -1,    -1,   188,   192,   187,
     186,    -1,   223,    -1,   188,   223,    -1,    -1,   191,    -1,
      -1,   191,   192,   190,   189,    -1,   222,    -1,   191,   222,
      -1,    -1,    65,   193,   194,    -1,   216,    -1,   218,    -1,
      63,   217,   195,    64,    -1,   216,    -1,   218,    -1,   195,
     216,    -1,   195,   218,    -1,   215,    -1,     3,    -1,     4,
      -1,     5,    -1,    38,    -1,    39,    -1,    40,    -1,    41,
      -1,    42,    -1,    43,    -1,    44,    -1,    45,    -1,    46,
      -1,    47,    -1,    48,    -1,    49,    -1,    59,    -1,    50,
      -1,    51,    -1,    53,    -1,    54,    -1,    55,    -1,     3,
      -1,     4,    -1,     5,    -1,    55,    -1,    45,    -1,    46,
      -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,    51,
      -1,    52,    -1,    58,    -1,   218,    -1,    54,    -1,    53,
      -1,     3,    -1,     4,    -1,     5,    -1,    55,    -1,    46,
      -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,    51,
      -1,    52,    -1,    58,    -1,    54,    -1,    53,    -1,   222,
      -1,     3,    -1,     4,    -1,     5,    -1,    53,    -1,    54,
      -1,    55,    -1,    38,    -1,    39,    -1,    40,    -1,    41,
      -1,    42,    -1,    43,    -1,    44,    -1,    45,    -1,    46,
      -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,    51,
      -1,    52,    -1,    58,    -1,    60,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   261,   261,   261,   265,   266,   267,   274,   274,   278,
     279,   280,   281,   284,   285,   286,   289,   290,   291,   292,
     293,   294,   295,   298,   299,   300,   301,   302,   305,   306,
     307,   308,   309,   312,   313,   314,   315,   316,   319,   320,
     321,   324,   325,   326,   329,   330,   331,   334,   335,   338,
     341,   344,   345,   348,   349,   350,   352,   354,   355,   357,
     359,   361,   362,   363,   364,   365,   367,   369,   376,   376,
     380,   380,   384,   387,   387,   394,   395,   398,   398,   402,
     403,   404,   407,   408,   411,   411,   414,   414,   422,   422,
     424,   424,   429,   430,   433,   434,   437,   438,   441,   441,
     445,   445,   448,   449,   452,   453,   460,   461,   465,   470,
     475,   479,   480,   488,   489,   492,   493,   496,   496,   497,
     497,   498,   498,   505,   506,   507,   507,   509,   510,   509,
     514,   515,   518,   519,   522,   523,   527,   528,   531,   532,
     533,   533,   535,   535,   539,   540,   544,   543,   547,   546,
     551,   552,   553,   553,   555,   555,   559,   560,   568,   567,
     572,   573,   576,   577,   580,   581,   584,   584,   588,   591,
     592,   595,   595,   597,   597,   599,   601,   603,   607,   608,
     611,   614,   614,   616,   616,   624,   625,   626,   627,   628,
     628,   629,   629,   631,   631,   633,   633,   637,   638,   641,
     642,   645,   645,   649,   649,   653,   654,   661,   662,   663,
     664,   665,   666,   669,   670,   673,   673,   675,   678,   679,
     681,   683,   685,   687,   690,   691,   694,   694,   696,   703,
     704,   705,   708,   709,   712,   713,   716,   717,   718,   718,
     722,   723,   726,   727,   728,   728,   731,   732,   735,   735,
     738,   739,   740,   743,   744,   745,   746,   749,   756,   759,
     762,   765,   768,   771,   774,   777,   780,   783,   786,   789,
     792,   795,   798,   801,   804,   807,   810,   813,   816,   819,
     819,   819,   820,   821,   821,   821,   821,   821,   822,   822,
     822,   823,   826,   827,   827,   830,   830,   830,   831,   832,
     832,   832,   832,   833,   833,   833,   834,   835,   835,   838,
     841,   841,   841,   842,   842,   842,   843,   843,   843,   843,
     843,   843,   843,   844,   844,   844,   844,   844,   845,   845,
     845,   846,   849
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DEFINE", "DOMAIN_TOKEN", "PROBLEM",
  "REQUIREMENTS", "TYPES", "CONSTANTS", "PREDICATES", "FUNCTIONS",
  "STRIPS", "TYPING", "NEGATIVE_PRECONDITIONS",
  "DISJUNCTIVE_PRECONDITIONS", "EQUALITY", "EXISTENTIAL_PRECONDITIONS",
  "UNIVERSAL_PRECONDITIONS", "QUANTIFIED_PRECONDITIONS",
  "CONDITIONAL_EFFECTS", "FLUENTS", "ADL", "DURATIVE_ACTIONS",
  "DURATION_INEQUALITIES", "CONTINUOUS_EFFECTS", "TIMED_INITIAL_LITERALS",
  "ACTION", "PARAMETERS", "PRECONDITION", "EFFECT", "DURATIVE_ACTION",
  "DURATION", "CONDITION", "PDOMAIN", "OBJECTS", "INIT", "GOAL", "METRIC",
  "WHEN", "NOT", "AND", "OR", "IMPLY", "EXISTS", "FORALL", "AT", "OVER",
  "START", "END", "ALL", "MINIMIZE", "MAXIMIZE", "TOTAL_TIME",
  "NUMBER_TOKEN", "OBJECT_TOKEN", "EITHER", "LE", "GE", "NAME",
  "DURATION_VAR", "VARIABLE", "NUMBER", "ILLEGAL_TOKEN", "'('", "')'",
  "'-'", "'='", "'+'", "'*'", "'/'", "$accept", "pddl_file", "@1",
  "domains_and_problems", "domain_def", "@2", "domain_body",
  "domain_body2", "domain_body3", "domain_body4", "domain_body5",
  "domain_body6", "domain_body7", "domain_body8", "domain_body9",
  "structure_defs", "structure_def", "require_def", "require_keys",
  "require_key", "types_def", "@3", "constants_def", "@4",
  "predicates_def", "functions_def", "@5", "predicate_decls",
  "predicate_decl", "@6", "function_decls", "function_decl_seq",
  "function_type_spec", "@7", "function_decl", "@8", "action_def", "@9",
  "@10", "parameters", "action_body", "action_body2", "precondition",
  "@11", "effect", "@12", "da_body", "da_body2", "duration_constraint",
  "simple_duration_constraint", "simple_duration_constraints", "da_gd",
  "timed_gds", "timed_gd", "@13", "@14", "@15", "eff_formula", "@16",
  "@17", "@18", "eff_formulas", "one_eff_formula", "term_literal",
  "term_literals", "da_effect", "@19", "@20", "da_effects", "timed_effect",
  "@21", "@22", "a_effect", "@23", "@24", "a_effects", "problem_def",
  "@25", "problem_body", "problem_body2", "problem_body3", "object_decl",
  "@26", "init", "init_elements", "init_element", "@27", "@28",
  "goal_spec", "goal", "metric_spec", "@29", "@30", "formula", "@31",
  "@32", "@33", "@34", "conjuncts", "disjuncts", "atomic_term_formula",
  "@35", "atomic_name_formula", "@36", "name_literal", "f_exp",
  "opt_f_exp", "f_head", "@37", "ground_f_exp", "opt_ground_f_exp",
  "ground_f_head", "@38", "terms", "names", "term", "variables", "@39",
  "variable_seq", "typed_names", "@40", "name_seq", "type_spec", "@41",
  "type", "types", "function_type", "define", "domain", "problem", "when",
  "not", "and", "or", "imply", "exists", "forall", "at", "over", "start",
  "end", "all", "duration_var", "minimize", "maximize", "number", "object",
  "either", "type_name", "predicate", "init_predicate", "function", "name",
  "variable", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,    40,    41,    45,    61,    43,    42,    47
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
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

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
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

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
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
     263,     0,     0,     0,   111,     0,     0,     0,   103,     0,
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

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     1,     2,     4,     6,    41,    44,    45,    46,    71,
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

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -426
static const short int yypact[] =
{
    -426,    20,  -426,  -426,   -35,    37,  -426,  -426,  -426,     7,
      54,  -426,  -426,   876,   876,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,   -14,    23,  -426,
      57,    85,    59,    70,    62,  -426,  -426,    90,  -426,    92,
      94,   122,   131,   134,  -426,   876,  1036,  -426,  -426,  -426,
    -426,   876,   876,  -426,    25,  -426,   128,  -426,   154,  -426,
     179,  -426,    90,   145,   158,    63,  -426,    90,   145,   162,
      75,  -426,    90,   158,   162,    82,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,   818,  -426,   876,   876,    18,   181,  -426,  -426,    17,
    -426,    90,    90,   177,  -426,    90,    90,  -426,   117,  -426,
      90,    90,  -426,  -426,  -426,  -426,  -426,   142,   505,  -426,
     190,   960,  -426,  -426,   876,   196,    43,  -426,   237,   237,
      90,    90,    90,   202,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
     181,  -426,   203,    74,   236,    32,   205,   206,  -426,  -426,
     208,   217,  -426,   223,   227,   876,   235,   235,   234,  -426,
     235,  -426,  -426,   232,  -426,   272,  -426,   239,  -426,  -426,
     241,   143,  -426,  -426,   106,  -426,   270,  -426,   271,  -426,
    -426,   255,  -426,  -426,  -426,  -426,  -426,   247,   -28,  -426,
     248,  -426,  -426,  -426,   250,   241,   253,  -426,  -426,    29,
      65,  -426,   876,    87,   446,   254,  -426,   115,  -426,   988,
    -426,  -426,  -426,  -426,  -426,  -426,   904,  -426,  -426,  -426,
    -426,   258,   258,   258,  -426,   256,   260,   257,  -426,   261,
     468,  -426,  -426,  -426,  -426,  -426,  -426,  -426,   846,   241,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,   703,  -426,  -426,   235,  -426,  -426,   264,  -426,  -426,
    -426,   731,   731,   731,   127,   182,  -426,  -426,    -1,   295,
    -426,  -426,  -426,  -426,  -426,  -426,   267,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,   820,   266,
     269,  -426,   846,  -426,  -426,   268,   136,  -426,   241,   277,
     278,  -426,   759,   759,  -426,  -426,  -426,  -426,   241,   960,
     279,   147,   281,  -426,   334,   282,  -426,  -426,   283,   284,
      83,  -426,  -426,  -426,   260,  -426,  -426,   168,  -426,  -426,
     168,   293,  -426,  -426,   876,   288,  -426,   960,   286,   289,
    -426,   287,  -426,  -426,  -426,   165,   241,   235,   235,   533,
    -426,   390,   290,  -426,   291,  -426,  -426,  -426,  -426,   235,
     731,   731,   731,   731,  -426,  -426,  -426,  -426,  -426,   174,
     294,  -426,  -426,  -426,  -426,   176,  -426,  -426,  -426,  -426,
     591,  -426,   292,  -426,  -426,   365,  -426,   296,   619,  -426,
    -426,  -426,   297,   298,   299,  -426,  -426,  -426,   759,   759,
     759,   759,  -426,  -426,   301,   303,   731,   731,   731,   731,
    -426,   302,  -426,  -426,   235,   308,   308,   172,  -426,  -426,
     241,   241,   241,  -426,  -426,  -426,  -426,  -426,   266,  -426,
    -426,  -426,   241,   241,   759,   759,   759,   759,   932,   326,
    -426,   253,  -426,   327,   332,   336,   342,   561,   300,   343,
     344,   904,  -426,   345,   357,   358,   360,   361,   647,   675,
     362,   363,   382,  -426,   383,   388,   389,   392,  -426,  -426,
     396,  -426,  -426,  -426,  -426,  -426,  -426,   256,   241,  -426,
    -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -426,  -426,  -426,  -426,  -426,   178,  -426,   397,  -426,   189,
     335,   365,  -426,  -426,  -426,   301,  -426,  -426,   235,   398,
     399,  -426,   308,   401,  -426
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -426,  -426,  -426,  -426,  -426,  -426,  -426,   309,   404,  -426,
    -426,  -426,   391,   276,   384,   210,    41,   323,  -426,   366,
    -426,  -426,   -27,  -426,   -20,   -17,  -426,  -426,  -426,  -426,
     304,  -426,  -426,  -426,   341,  -426,  -426,  -426,  -426,   331,
    -426,   285,  -426,  -426,  -426,  -426,  -426,   180,  -426,   184,
    -426,   129,  -426,    76,  -426,  -426,  -426,  -327,  -426,  -426,
    -426,  -426,   -53,  -220,  -426,  -383,  -426,  -426,  -426,    61,
    -426,  -426,  -425,  -426,  -426,  -426,  -426,  -426,  -426,   329,
     347,  -426,  -426,  -426,  -426,  -426,  -426,  -426,   322,  -426,
    -426,  -426,  -426,  -216,  -426,  -426,  -426,  -426,  -426,  -426,
    -222,  -426,  -351,  -426,  -426,  -263,  -426,  -426,  -426,  -297,
    -426,   166,  -426,    66,  -346,   207,  -185,  -426,  -426,   -87,
    -426,  -426,   306,  -426,  -426,  -426,  -426,  -426,  -426,  -426,
    -283,  -219,  -223,  -426,  -426,  -426,  -233,  -252,  -426,   151,
     170,  -426,     4,  -426,  -426,  -426,  -217,  -426,  -165,  -124,
    -426,  -131,   -13,  -208
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -269
static const short int yytable[] =
{
      37,    38,   220,   166,   249,   224,   248,   165,   320,   245,
     242,   270,   354,   289,   388,   269,   443,   130,   416,   214,
       3,   484,   282,   288,   418,    78,    83,    60,     5,   348,
     349,    73,   216,    84,    74,    79,   384,   145,    56,   250,
       8,   319,    85,    61,   353,   358,   360,    62,   107,   108,
      39,    61,   121,   325,   116,    62,   112,   121,    11,    12,
     324,   112,   356,   116,   335,   340,   198,   199,   200,   250,
      10,    58,   355,    60,   283,   359,    56,    57,    58,    59,
      60,   131,   132,    58,    59,   251,   252,    40,    65,    61,
     129,   129,    55,    62,   255,   253,    61,   256,   215,   337,
      62,    61,   191,   192,   537,    62,   134,   490,   169,   488,
     374,   489,   376,    65,   324,   147,   336,   543,    65,   249,
      42,   248,   385,    65,   527,    58,    63,   436,   437,   438,
     439,   464,   465,   466,   467,    57,    58,    59,    60,   251,
     252,   199,   200,    61,   500,   259,   124,    62,    43,   253,
     260,   261,    65,    64,    61,    66,    65,    68,    62,   421,
     422,    65,    58,    59,    60,   277,   278,   493,   495,   496,
     497,   427,   129,   472,   474,   475,   476,   198,   199,   200,
      61,    65,    65,    65,    62,    70,    59,   366,    59,    60,
     350,   351,   423,   424,    75,   360,   458,    80,   508,   234,
     373,   366,   366,    61,   435,    61,   144,    62,   109,    62,
     246,   387,   249,   394,   470,   401,   402,   353,   358,   129,
     285,   113,   250,   249,   249,   118,   267,   353,   234,   420,
     149,   150,   151,   411,   485,   486,   487,   295,   442,   447,
     448,   531,   532,   413,   134,   498,   491,   492,   510,   249,
     411,   248,   481,   536,   148,   323,   292,   293,   509,   480,
     168,    72,    77,    82,   172,   175,   190,   197,   201,   427,
     203,   204,   152,   153,   154,   155,   156,   157,   158,   159,
     206,   210,   162,   111,   115,   163,   208,   221,   111,   120,
     211,   413,   528,   115,   120,   216,   227,   366,   366,   366,
     366,   192,   229,   249,   234,   533,   200,   249,   237,   323,
     238,   240,   243,   249,   244,   470,   246,   290,   276,   295,
     249,   301,   140,   298,   255,   302,   141,   339,  -268,   367,
     369,   142,   372,   366,   366,   366,   366,    15,    16,    17,
     377,   378,   408,   386,   389,   353,   395,   396,   397,   412,
     414,   419,   415,   540,   432,   433,   456,   444,    67,   122,
     459,   461,   462,   463,   468,   478,   426,   471,   149,   150,
     151,   481,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
     499,   501,    36,    15,    16,    17,   502,   454,   538,   390,
     503,   391,   392,   393,   263,   454,   504,   506,   507,   511,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   512,   513,   163,   514,   515,   518,   519,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,   520,   521,    36,   149,
     150,   151,   522,   523,    69,   428,   524,   429,   430,   431,
     526,   534,   541,   542,   426,   544,   176,   126,   123,   117,
     174,   303,   304,   305,   189,   454,   454,   171,   352,   362,
     228,   449,   539,   398,   365,   263,   250,   264,   265,   266,
     267,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   479,   207,   163,   202,   477,   263,    15,    16,
      17,   406,   268,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   241,     0,   317,   205,     0,   371,
     407,     0,     0,     0,   318,     0,    15,    16,    17,     0,
       0,     0,     0,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,     0,     0,    36,    15,    16,    17,     0,     0,     0,
     145,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,     0,
       0,    36,     0,   216,    15,    16,    17,   425,     0,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
       0,   216,    15,    16,    17,   505,     0,     0,     0,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
      15,    16,    17,     0,     0,   453,     0,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,     0,     0,    36,    15,    16,
      17,     0,     0,   460,     0,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,     0,     0,    36,   149,   150,   151,     0,
       0,   516,     0,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,     0,     0,    36,    15,    16,    17,     0,     0,   517,
       0,     0,     0,     0,     0,     0,     0,     0,   152,   153,
     154,   155,   156,   157,   158,   159,     0,   210,   162,     0,
       0,   163,    15,    16,    17,     0,     0,   334,     0,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,     0,    36,
       0,     0,   343,     0,   344,     0,     0,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,     0,     0,    36,     0,     0,
     380,     0,   381,    15,    16,    17,     0,     0,     0,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,     0,     0,     0,     0,     0,    15,
      16,    17,     0,     0,     0,     0,     0,     0,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,     0,     0,    36,    15,
      16,    17,   125,   364,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,     0,     0,    36,     0,   216,   149,   150,   151,
       0,     0,     0,     0,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,     0,     0,    36,   149,   150,   151,     0,     0,
       0,     0,   285,   263,   250,     0,     0,     0,   267,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
       0,     0,   163,   149,   150,   151,     0,     0,     0,     0,
       0,   263,   250,     0,     0,     0,     0,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,     0,     0,
     163,   149,   150,   151,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,     0,     0,   163,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   152,   153,   154,   155,   156,   157,   158,
     159,     0,   210,   162,     0,     0,   163,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100
};

static const short int yycheck[] =
{
      13,    14,   187,   134,   226,   190,   226,   131,   260,   225,
     218,   234,   295,   246,   341,   234,   399,   104,   369,   184,
       0,   446,   239,   246,   370,    52,    53,    10,    63,   292,
     293,    51,    60,    53,    51,    52,   333,    65,     6,    40,
       3,   260,    55,    26,    45,    46,   298,    30,    61,    62,
      64,    26,    79,   269,    74,    30,    73,    84,     4,     5,
     268,    78,   295,    83,   281,   287,    34,    35,    36,    40,
      63,     8,   295,    10,   239,   298,     6,     7,     8,     9,
      10,    63,    64,     8,     9,    56,    57,    64,    47,    26,
     103,   104,    33,    30,    29,    66,    26,    32,   185,   284,
      30,    26,    28,    29,   529,    30,    63,   458,    65,   455,
     326,   457,   328,    72,   322,   128,   281,   542,    77,   341,
      63,   341,   338,    82,   507,     8,    64,   390,   391,   392,
     393,   428,   429,   430,   431,     7,     8,     9,    10,    56,
      57,    35,    36,    26,   471,   232,    64,    30,    63,    66,
      63,    64,   111,    63,    26,    63,   115,    63,    30,   375,
     376,   120,     8,     9,    10,    50,    51,   464,   465,   466,
     467,   379,   185,   436,   437,   438,   439,    34,    35,    36,
      26,   140,   141,   142,    30,    63,     9,   318,     9,    10,
      63,    64,   377,   378,    63,   447,   415,    63,   481,    63,
      64,   332,   333,    26,   389,    26,    64,    30,    63,    30,
      63,    64,   434,   344,   434,    47,    48,    45,    46,   232,
      38,    63,    40,   445,   446,    63,    44,    45,    63,    64,
       3,     4,     5,   364,   450,   451,   452,    63,    64,    63,
      64,    63,    64,   367,    63,   468,   462,   463,   481,   471,
     381,   471,    63,    64,    64,   268,   252,   253,   481,   444,
      64,    51,    52,    53,    27,    63,    63,    31,    63,   477,
      64,    63,    45,    46,    47,    48,    49,    50,    51,    52,
      63,    54,    55,    73,    74,    58,    63,    53,    78,    79,
      63,   415,   508,    83,    84,    60,    64,   428,   429,   430,
     431,    29,    63,   525,    63,   525,    36,   529,    37,   322,
      55,    64,    64,   535,    64,   535,    63,    59,    64,    63,
     542,    64,   112,    63,    29,    64,   116,    63,    61,    63,
      61,   121,    64,   464,   465,   466,   467,     3,     4,     5,
      63,    63,    49,    64,    63,    45,    64,    64,    64,    61,
      64,    64,    63,   538,    64,    64,    64,    63,    49,    83,
      64,    64,    64,    64,    63,    63,   379,    64,     3,     4,
       5,    63,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      64,    64,    58,     3,     4,     5,    64,   410,    63,    65,
      64,    67,    68,    69,    39,   418,    64,    64,    64,    64,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    64,    64,    58,    64,    64,    64,    64,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    64,    64,    58,     3,
       4,     5,    64,    64,    50,    65,    64,    67,    68,    69,
      64,    64,    64,    64,   477,    64,   143,   101,    84,    78,
     139,     3,     4,     5,   170,   488,   489,   136,   294,   299,
     195,   405,   535,   354,   318,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,   441,   181,    58,   176,   440,    39,     3,     4,
       5,   360,    66,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,   218,    -1,    58,   180,    -1,   322,
     360,    -1,    -1,    -1,    66,    -1,     3,     4,     5,    -1,
      -1,    -1,    -1,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    58,     3,     4,     5,    -1,    -1,    -1,
      65,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      -1,    58,    -1,    60,     3,     4,     5,    64,    -1,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    58,
      -1,    60,     3,     4,     5,    64,    -1,    -1,    -1,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    58,
       3,     4,     5,    -1,    -1,    64,    -1,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,     3,     4,
       5,    -1,    -1,    64,    -1,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    58,     3,     4,     5,    -1,
      -1,    64,    -1,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    58,     3,     4,     5,    -1,    -1,    64,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    46,
      47,    48,    49,    50,    51,    52,    -1,    54,    55,    -1,
      -1,    58,     3,     4,     5,    -1,    -1,    64,    -1,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    58,
      -1,    -1,    61,    -1,    63,    -1,    -1,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      61,    -1,    63,     3,     4,     5,    -1,    -1,    -1,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    -1,    -1,    58,     3,
       4,     5,    64,    63,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    -1,    -1,    58,    -1,    60,     3,     4,     5,
      -1,    -1,    -1,    -1,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    -1,    -1,    58,     3,     4,     5,    -1,    -1,
      -1,    -1,    38,    39,    40,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,     3,     4,     5,    -1,    -1,    -1,    -1,
      -1,    39,    40,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
      58,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    -1,    -1,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    -1,    54,    55,    -1,    -1,    58,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    71,    72,     0,    73,    63,    74,   146,     3,   197,
      63,     4,     5,   198,   199,     3,     4,     5,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    58,   222,   222,    64,
      64,    75,    63,    63,    76,    77,    78,    85,    86,    87,
      90,    92,    94,    95,   106,    33,     6,     7,     8,     9,
      10,    26,    30,    64,    63,    86,    63,    77,    63,    78,
      63,    79,    85,    94,    95,    63,    80,    85,    92,    95,
      63,    81,    85,    92,    94,   222,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    88,    89,    91,    93,    97,    96,   222,   222,    63,
      82,    85,    95,    63,    83,    85,    94,    82,    63,    84,
      85,    92,    83,    84,    64,    64,    89,   189,   191,   222,
     189,    63,    64,    98,    63,   100,   101,   104,   107,   108,
      85,    85,    85,   147,    64,    65,   192,   222,    64,     3,
       4,     5,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    58,   218,   219,   221,   222,    64,    65,
     102,   104,    27,   109,   109,    63,    87,   148,   149,   150,
     151,   153,   158,   159,   193,   190,    99,   105,   103,   100,
      63,    28,    29,   110,   111,   112,   114,    31,    34,    35,
      36,    63,   149,    64,    63,   150,    63,   158,    63,   160,
      54,    63,   194,   216,   218,   189,    60,   186,   188,   223,
     186,    53,   196,   215,   186,   113,   115,    64,   111,    63,
     118,   119,   152,   154,    63,   163,   170,    37,    55,   217,
      64,   192,   223,    64,    64,   163,    63,   127,   133,   170,
      40,    56,    57,    66,   202,    29,    32,   116,   117,   189,
      63,    64,   155,    39,    41,    42,    43,    44,    66,   201,
     202,   203,   204,   205,   206,   219,    64,    50,    51,   213,
     214,   195,   216,   218,   187,    38,   200,   201,   202,   206,
      59,   212,   212,   212,   120,    63,   135,   139,    63,   121,
     123,    64,    64,     3,     4,     5,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    58,    66,   201,
     207,   220,   185,   222,   223,   163,   168,   164,   165,   166,
     167,   171,   162,   161,    64,   216,   218,   186,   129,    63,
     170,   131,   128,    61,    63,   175,   177,   221,   175,   175,
      63,    64,   119,    45,   200,   202,   206,   207,    46,   202,
     207,   208,   117,   157,    63,   181,   221,    63,   172,    61,
     156,   185,    64,    64,   163,   169,   163,    63,    63,   183,
      61,    63,   179,   181,   179,   163,    64,    64,   127,    63,
      65,    67,    68,    69,   221,    64,    64,    64,   121,   138,
     136,    47,    48,   209,   210,   122,   209,   210,    49,   211,
     184,   221,    61,   219,    64,    63,   172,   174,   184,    64,
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

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  yyvsp[0] = yylval;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 261 "pddl.yy"
    { success = true; line_number = 1; }
    break;

  case 3:
#line 262 "pddl.yy"
    { if (!success) YYERROR; }
    break;

  case 7:
#line 274 "pddl.yy"
    { make_domain((yyvsp[-1].str)); }
    break;

  case 53:
#line 348 "pddl.yy"
    { requirements->strips = true; }
    break;

  case 54:
#line 349 "pddl.yy"
    { requirements->typing = true; }
    break;

  case 55:
#line 351 "pddl.yy"
    { requirements->negative_preconditions = true; }
    break;

  case 56:
#line 353 "pddl.yy"
    { requirements->disjunctive_preconditions = true; }
    break;

  case 57:
#line 354 "pddl.yy"
    { requirements->equality = true; }
    break;

  case 58:
#line 356 "pddl.yy"
    { requirements->existential_preconditions = true; }
    break;

  case 59:
#line 358 "pddl.yy"
    { requirements->universal_preconditions = true; }
    break;

  case 60:
#line 360 "pddl.yy"
    { requirements->quantified_preconditions(); }
    break;

  case 61:
#line 361 "pddl.yy"
    { requirements->conditional_effects = true; }
    break;

  case 62:
#line 362 "pddl.yy"
    { requirements->fluents = true; }
    break;

  case 63:
#line 363 "pddl.yy"
    { requirements->adl(); }
    break;

  case 64:
#line 364 "pddl.yy"
    { requirements->durative_actions = true; }
    break;

  case 65:
#line 366 "pddl.yy"
    { requirements->duration_inequalities = true; }
    break;

  case 66:
#line 368 "pddl.yy"
    { yyerror("`:continuous-effects' not supported"); }
    break;

  case 67:
#line 370 "pddl.yy"
    {
		  requirements->durative_actions = true;
		  requirements->timed_initial_literals = true;
		}
    break;

  case 68:
#line 376 "pddl.yy"
    { require_typing(); name_kind = TYPE_KIND; }
    break;

  case 69:
#line 377 "pddl.yy"
    { name_kind = VOID_KIND; }
    break;

  case 70:
#line 380 "pddl.yy"
    { name_kind = CONSTANT_KIND; }
    break;

  case 71:
#line 381 "pddl.yy"
    { name_kind = VOID_KIND; }
    break;

  case 73:
#line 387 "pddl.yy"
    { require_fluents(); }
    break;

  case 77:
#line 398 "pddl.yy"
    { make_predicate((yyvsp[0].str)); }
    break;

  case 78:
#line 399 "pddl.yy"
    { predicate = 0; }
    break;

  case 84:
#line 411 "pddl.yy"
    { require_typing(); }
    break;

  case 86:
#line 414 "pddl.yy"
    { make_function((yyvsp[0].str)); }
    break;

  case 87:
#line 415 "pddl.yy"
    { function = 0; }
    break;

  case 88:
#line 422 "pddl.yy"
    { make_action((yyvsp[0].str), false); }
    break;

  case 89:
#line 423 "pddl.yy"
    { add_action(); }
    break;

  case 90:
#line 424 "pddl.yy"
    { make_action((yyvsp[0].str), true); }
    break;

  case 91:
#line 426 "pddl.yy"
    { add_action(); }
    break;

  case 98:
#line 441 "pddl.yy"
    { formula_time = AT_START; }
    break;

  case 99:
#line 442 "pddl.yy"
    { action->set_condition(*(yyvsp[0].formula)); }
    break;

  case 100:
#line 445 "pddl.yy"
    { effect_time = Effect::AT_END; }
    break;

  case 102:
#line 448 "pddl.yy"
    { action->set_condition(*(yyvsp[-1].formula)); }
    break;

  case 107:
#line 462 "pddl.yy"
    { require_duration_inequalities(); }
    break;

  case 108:
#line 466 "pddl.yy"
    {
				 require_duration_inequalities();
				 action->set_max_duration(*(yyvsp[-1].expr));
			       }
    break;

  case 109:
#line 471 "pddl.yy"
    {
				 require_duration_inequalities();
				 action->set_min_duration(*(yyvsp[-1].expr));
			       }
    break;

  case 110:
#line 476 "pddl.yy"
    { action->set_duration(*(yyvsp[-1].expr)); }
    break;

  case 114:
#line 489 "pddl.yy"
    { (yyval.formula) = (yyvsp[-1].formula); }
    break;

  case 115:
#line 492 "pddl.yy"
    { (yyval.formula) = &Formula::TRUE; }
    break;

  case 116:
#line 493 "pddl.yy"
    { (yyval.formula) = &(*(yyvsp[-1].formula) && *(yyvsp[0].formula)); }
    break;

  case 117:
#line 496 "pddl.yy"
    { formula_time = AT_START; }
    break;

  case 118:
#line 496 "pddl.yy"
    { (yyval.formula) = (yyvsp[-1].formula); }
    break;

  case 119:
#line 497 "pddl.yy"
    { formula_time = AT_END; }
    break;

  case 120:
#line 497 "pddl.yy"
    { (yyval.formula) = (yyvsp[-1].formula); }
    break;

  case 121:
#line 498 "pddl.yy"
    { formula_time = OVER_ALL; }
    break;

  case 122:
#line 498 "pddl.yy"
    { (yyval.formula) = (yyvsp[-1].formula); }
    break;

  case 125:
#line 507 "pddl.yy"
    { prepare_forall_effect(); }
    break;

  case 126:
#line 508 "pddl.yy"
    { pop_forall_effect(); }
    break;

  case 127:
#line 509 "pddl.yy"
    { formula_time = AT_START; }
    break;

  case 128:
#line 510 "pddl.yy"
    { prepare_conditional_effect(*(yyvsp[0].formula)); }
    break;

  case 129:
#line 511 "pddl.yy"
    { effect_condition = 0; }
    break;

  case 134:
#line 522 "pddl.yy"
    { add_effect(*(yyvsp[0].atom)); }
    break;

  case 135:
#line 524 "pddl.yy"
    { add_effect(Negation::make(*(yyvsp[-1].atom))); }
    break;

  case 140:
#line 533 "pddl.yy"
    { prepare_forall_effect(); }
    break;

  case 141:
#line 534 "pddl.yy"
    { pop_forall_effect(); }
    break;

  case 142:
#line 535 "pddl.yy"
    { prepare_conditional_effect(*(yyvsp[0].formula)); }
    break;

  case 143:
#line 536 "pddl.yy"
    { effect_condition = 0; }
    break;

  case 146:
#line 544 "pddl.yy"
    { effect_time = Effect::AT_START; formula_time = AT_START; }
    break;

  case 148:
#line 547 "pddl.yy"
    { effect_time = Effect::AT_END; formula_time = AT_END; }
    break;

  case 152:
#line 553 "pddl.yy"
    { prepare_forall_effect(); }
    break;

  case 153:
#line 554 "pddl.yy"
    { pop_forall_effect(); }
    break;

  case 154:
#line 555 "pddl.yy"
    { prepare_conditional_effect(*(yyvsp[0].formula)); }
    break;

  case 155:
#line 556 "pddl.yy"
    { effect_condition = 0; }
    break;

  case 158:
#line 568 "pddl.yy"
    { make_problem((yyvsp[-5].str), (yyvsp[-1].str)); }
    break;

  case 159:
#line 569 "pddl.yy"
    { delete requirements; }
    break;

  case 166:
#line 584 "pddl.yy"
    { name_kind = OBJECT_KIND; }
    break;

  case 167:
#line 585 "pddl.yy"
    { name_kind = VOID_KIND; }
    break;

  case 171:
#line 595 "pddl.yy"
    { prepare_atom((yyvsp[0].str)); }
    break;

  case 172:
#line 596 "pddl.yy"
    { problem->add_init_atom(*make_atom()); }
    break;

  case 173:
#line 597 "pddl.yy"
    { prepare_atom((yyvsp[0].str)); }
    break;

  case 174:
#line 598 "pddl.yy"
    { problem->add_init_atom(*make_atom()); }
    break;

  case 175:
#line 600 "pddl.yy"
    { Formula::register_use((yyvsp[-1].atom)); Formula::unregister_use((yyvsp[-1].atom)); }
    break;

  case 176:
#line 602 "pddl.yy"
    { problem->add_init_value(*(yyvsp[-2].fluent), (yyvsp[-1].num)); }
    break;

  case 177:
#line 604 "pddl.yy"
    { add_init_literal((yyvsp[-2].num), *(yyvsp[-1].literal)); }
    break;

  case 180:
#line 611 "pddl.yy"
    { problem->set_goal(*(yyvsp[-1].formula)); }
    break;

  case 181:
#line 614 "pddl.yy"
    { metric_fluent = true; }
    break;

  case 182:
#line 615 "pddl.yy"
    { problem->set_metric(*(yyvsp[-1].expr), true); metric_fluent = false; }
    break;

  case 183:
#line 616 "pddl.yy"
    { metric_fluent = true; }
    break;

  case 184:
#line 617 "pddl.yy"
    { problem->set_metric(*(yyvsp[-1].expr)); metric_fluent = false; }
    break;

  case 185:
#line 624 "pddl.yy"
    { (yyval.formula) = &TimedLiteral::make(*(yyvsp[0].atom), formula_time); }
    break;

  case 186:
#line 625 "pddl.yy"
    { (yyval.formula) = make_equality((yyvsp[-2].term), (yyvsp[-1].term)); }
    break;

  case 187:
#line 626 "pddl.yy"
    { (yyval.formula) = make_negation(*(yyvsp[-1].formula)); }
    break;

  case 188:
#line 627 "pddl.yy"
    { (yyval.formula) = (yyvsp[-1].formula); }
    break;

  case 189:
#line 628 "pddl.yy"
    { require_disjunction(); }
    break;

  case 190:
#line 628 "pddl.yy"
    { (yyval.formula) = (yyvsp[-1].formula); }
    break;

  case 191:
#line 629 "pddl.yy"
    { require_disjunction(); }
    break;

  case 192:
#line 630 "pddl.yy"
    { (yyval.formula) = &(!*(yyvsp[-2].formula) || *(yyvsp[-1].formula)); }
    break;

  case 193:
#line 631 "pddl.yy"
    { prepare_exists(); }
    break;

  case 194:
#line 632 "pddl.yy"
    { (yyval.formula) = make_exists(*(yyvsp[-1].formula)); }
    break;

  case 195:
#line 633 "pddl.yy"
    { prepare_forall(); }
    break;

  case 196:
#line 634 "pddl.yy"
    { (yyval.formula) = make_forall(*(yyvsp[-1].formula)); }
    break;

  case 197:
#line 637 "pddl.yy"
    { (yyval.formula) = &Formula::TRUE; }
    break;

  case 198:
#line 638 "pddl.yy"
    { (yyval.formula) = &(*(yyvsp[-1].formula) && *(yyvsp[0].formula)); }
    break;

  case 199:
#line 641 "pddl.yy"
    { (yyval.formula) = &Formula::FALSE; }
    break;

  case 200:
#line 642 "pddl.yy"
    { (yyval.formula) = &(*(yyvsp[-1].formula) || *(yyvsp[0].formula)); }
    break;

  case 201:
#line 645 "pddl.yy"
    { prepare_atom((yyvsp[0].str)); }
    break;

  case 202:
#line 646 "pddl.yy"
    { (yyval.atom) = make_atom(); }
    break;

  case 203:
#line 649 "pddl.yy"
    { prepare_atom((yyvsp[0].str)); }
    break;

  case 204:
#line 650 "pddl.yy"
    { (yyval.atom) = make_atom(); }
    break;

  case 205:
#line 653 "pddl.yy"
    { (yyval.literal) = (yyvsp[0].atom); }
    break;

  case 206:
#line 654 "pddl.yy"
    { (yyval.literal) = &Negation::make(*(yyvsp[-1].atom)); }
    break;

  case 207:
#line 661 "pddl.yy"
    { (yyval.expr) = new Value((yyvsp[0].num)); }
    break;

  case 208:
#line 662 "pddl.yy"
    { (yyval.expr) = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
    break;

  case 209:
#line 663 "pddl.yy"
    { (yyval.expr) = make_subtraction(*(yyvsp[-2].expr), (yyvsp[-1].expr)); }
    break;

  case 210:
#line 664 "pddl.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
    break;

  case 211:
#line 665 "pddl.yy"
    { (yyval.expr) = &Division::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
    break;

  case 212:
#line 666 "pddl.yy"
    { (yyval.expr) = (yyvsp[0].fluent); }
    break;

  case 213:
#line 669 "pddl.yy"
    { (yyval.expr) = 0; }
    break;

  case 215:
#line 673 "pddl.yy"
    { prepare_fluent((yyvsp[0].str)); }
    break;

  case 216:
#line 674 "pddl.yy"
    { (yyval.fluent) = make_fluent(); }
    break;

  case 217:
#line 675 "pddl.yy"
    { prepare_fluent((yyvsp[0].str)); (yyval.fluent) = make_fluent(); }
    break;

  case 218:
#line 678 "pddl.yy"
    { (yyval.expr) = new Value((yyvsp[0].num)); }
    break;

  case 219:
#line 680 "pddl.yy"
    { (yyval.expr) = &Addition::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
    break;

  case 220:
#line 682 "pddl.yy"
    { (yyval.expr) = make_subtraction(*(yyvsp[-2].expr), (yyvsp[-1].expr)); }
    break;

  case 221:
#line 684 "pddl.yy"
    { (yyval.expr) = &Multiplication::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
    break;

  case 222:
#line 686 "pddl.yy"
    { (yyval.expr) = &Division::make(*(yyvsp[-2].expr), *(yyvsp[-1].expr)); }
    break;

  case 223:
#line 687 "pddl.yy"
    { (yyval.expr) = (yyvsp[0].fluent); }
    break;

  case 224:
#line 690 "pddl.yy"
    { (yyval.expr) = 0; }
    break;

  case 226:
#line 694 "pddl.yy"
    { prepare_fluent((yyvsp[0].str)); }
    break;

  case 227:
#line 695 "pddl.yy"
    { (yyval.fluent) = make_fluent(); }
    break;

  case 228:
#line 696 "pddl.yy"
    { prepare_fluent((yyvsp[0].str)); (yyval.fluent) = make_fluent(); }
    break;

  case 230:
#line 704 "pddl.yy"
    { add_term((yyvsp[0].str)); }
    break;

  case 231:
#line 705 "pddl.yy"
    { add_term((yyvsp[0].str)); }
    break;

  case 233:
#line 709 "pddl.yy"
    { add_term((yyvsp[0].str)); }
    break;

  case 234:
#line 712 "pddl.yy"
    { (yyval.term) = new Term(make_term((yyvsp[0].str))); }
    break;

  case 235:
#line 713 "pddl.yy"
    { (yyval.term) = new Term(make_term((yyvsp[0].str))); }
    break;

  case 237:
#line 717 "pddl.yy"
    { add_variables((yyvsp[0].strs), TypeTable::OBJECT); }
    break;

  case 238:
#line 718 "pddl.yy"
    { add_variables((yyvsp[-1].strs), *(yyvsp[0].type)); delete (yyvsp[0].type); }
    break;

  case 240:
#line 722 "pddl.yy"
    { (yyval.strs) = new std::vector<const std::string*>(1, (yyvsp[0].str)); }
    break;

  case 241:
#line 723 "pddl.yy"
    { (yyval.strs) = (yyvsp[-1].strs); (yyval.strs)->push_back((yyvsp[0].str)); }
    break;

  case 243:
#line 727 "pddl.yy"
    { add_names((yyvsp[0].strs), TypeTable::OBJECT); }
    break;

  case 244:
#line 728 "pddl.yy"
    { add_names((yyvsp[-1].strs), *(yyvsp[0].type)); delete (yyvsp[0].type); }
    break;

  case 246:
#line 731 "pddl.yy"
    { (yyval.strs) = new std::vector<const std::string*>(1, (yyvsp[0].str)); }
    break;

  case 247:
#line 732 "pddl.yy"
    { (yyval.strs) = (yyvsp[-1].strs); (yyval.strs)->push_back((yyvsp[0].str)); }
    break;

  case 248:
#line 735 "pddl.yy"
    { require_typing(); }
    break;

  case 249:
#line 735 "pddl.yy"
    { (yyval.type) = (yyvsp[0].type); }
    break;

  case 250:
#line 738 "pddl.yy"
    { (yyval.type) = new Type(TypeTable::OBJECT); }
    break;

  case 251:
#line 739 "pddl.yy"
    { (yyval.type) = new Type(make_type((yyvsp[0].str))); }
    break;

  case 252:
#line 740 "pddl.yy"
    { (yyval.type) = new Type(make_type(*(yyvsp[-1].types))); delete (yyvsp[-1].types); }
    break;

  case 253:
#line 743 "pddl.yy"
    { (yyval.types) = new TypeSet(); }
    break;

  case 254:
#line 744 "pddl.yy"
    { (yyval.types) = new TypeSet(); (yyval.types)->insert(make_type((yyvsp[0].str))); }
    break;

  case 255:
#line 745 "pddl.yy"
    { (yyval.types) = (yyvsp[-1].types); }
    break;

  case 256:
#line 746 "pddl.yy"
    { (yyval.types) = (yyvsp[-1].types); (yyval.types)->insert(make_type((yyvsp[0].str))); }
    break;

  case 258:
#line 756 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 259:
#line 759 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 260:
#line 762 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 261:
#line 765 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 262:
#line 768 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 263:
#line 771 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 264:
#line 774 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 265:
#line 777 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 266:
#line 780 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 267:
#line 783 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 268:
#line 786 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 269:
#line 789 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 270:
#line 792 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 271:
#line 795 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 272:
#line 798 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 273:
#line 801 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 274:
#line 804 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 275:
#line 807 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 276:
#line 810 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 277:
#line 813 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;

  case 278:
#line 816 "pddl.yy"
    { delete (yyvsp[0].str); }
    break;


    }

/* Line 1037 of yacc.c.  */
#line 2835 "pddl.cc"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {

		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 yydestruct ("Error: popping",
                             yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yydestruct ("Error: discarding lookahead",
              yytoken, &yylval);
  yychar = YYEMPTY;
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 852 "pddl.yy"


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
static Type make_type(const TypeSet& types) {
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
	TypeSet components;
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
	  TypeSet components;
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
      const TypeList& params = PredicateTable::parameters(*atom_predicate);
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
      const TypeList& params = FunctionTable::parameters(*fluent_function);
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
  for (TermList::const_iterator vi = quantified.begin();
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

