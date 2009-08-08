/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

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
     STRIPS = 262,
     TYPING = 263,
     NEGATIVE_PRECONDITIONS = 264,
     DISJUNCTIVE_PRECONDITIONS = 265,
     EQUALITY = 266,
     EXISTENTIAL_PRECONDITIONS = 267,
     UNIVERSAL_PRECONDITIONS = 268,
     QUANTIFIED_PRECONDITIONS = 269,
     CONDITIONAL_EFFECTS = 270,
     FLUENTS = 271,
     ADL = 272,
     DURATIVE_ACTIONS = 273,
     DURATION_INEQUALITIES = 274,
     CONTINUOUS_EFFECTS = 275,
     TYPES = 276,
     CONSTANTS = 277,
     PREDICATES = 278,
     ACTION = 279,
     DURATIVE_ACTION = 280,
     DURATION = 281,
     PARAMETERS = 282,
     PRECONDITION = 283,
     CONDITION = 284,
     EFFECT = 285,
     PDOMAIN = 286,
     OBJECTS = 287,
     INIT = 288,
     GOAL = 289,
     METRIC = 290,
     WHEN = 291,
     NOT = 292,
     AND = 293,
     OR = 294,
     IMPLY = 295,
     EXISTS = 296,
     FORALL = 297,
     EITHER = 298,
     AT = 299,
     OVER = 300,
     START = 301,
     END = 302,
     ALL = 303,
     MINIMIZE = 304,
     MAXIMIZE = 305,
     TOTAL_TIME = 306,
     LE = 307,
     GE = 308,
     NAME = 309,
     DURATION_VAR = 310,
     VARIABLE = 311,
     NUMBER = 312,
     ILLEGAL_TOKEN = 313
   };
#endif
/* Tokens.  */
#define DEFINE 258
#define DOMAIN_TOKEN 259
#define PROBLEM 260
#define REQUIREMENTS 261
#define STRIPS 262
#define TYPING 263
#define NEGATIVE_PRECONDITIONS 264
#define DISJUNCTIVE_PRECONDITIONS 265
#define EQUALITY 266
#define EXISTENTIAL_PRECONDITIONS 267
#define UNIVERSAL_PRECONDITIONS 268
#define QUANTIFIED_PRECONDITIONS 269
#define CONDITIONAL_EFFECTS 270
#define FLUENTS 271
#define ADL 272
#define DURATIVE_ACTIONS 273
#define DURATION_INEQUALITIES 274
#define CONTINUOUS_EFFECTS 275
#define TYPES 276
#define CONSTANTS 277
#define PREDICATES 278
#define ACTION 279
#define DURATIVE_ACTION 280
#define DURATION 281
#define PARAMETERS 282
#define PRECONDITION 283
#define CONDITION 284
#define EFFECT 285
#define PDOMAIN 286
#define OBJECTS 287
#define INIT 288
#define GOAL 289
#define METRIC 290
#define WHEN 291
#define NOT 292
#define AND 293
#define OR 294
#define IMPLY 295
#define EXISTS 296
#define FORALL 297
#define EITHER 298
#define AT 299
#define OVER 300
#define START 301
#define END 302
#define ALL 303
#define MINIMIZE 304
#define MAXIMIZE 305
#define TOTAL_TIME 306
#define LE 307
#define GE 308
#define NAME 309
#define DURATION_VAR 310
#define VARIABLE 311
#define NUMBER 312
#define ILLEGAL_TOKEN 313




/* Copy the first part of user declarations.  */
#line 21 "pddl.yy"

#include <typeinfo>
#include <utility>
#include <cstdlib>
#include "mathport.h"
#include "requirements.h"
#include "problems.h"
#include "domains.h"
#include "formulas.h"
#include "types.h"


/* The lexer. */
extern int yylex();
/* Current line number. */
extern size_t line_number;
 

static string tostring(unsigned int n);
static void yyerror(const string& s); 
static void yywarning(const string& s);
static const SimpleType* find_type(const string& name);
static const Name* find_constant(const string& name);
static const Predicate* find_predicate(const string& name);
static void add_names(const vector<string>& names,
		      const Type& type = SimpleType::OBJECT);
static void add_predicate(const Predicate& predicate);
static void add_action(const ActionSchema& action);
static void add_variable(const string& name,
			 const Type& type = SimpleType::OBJECT);
static const pair<AtomList*, NegationList*>& make_add_del(AtomList* adds,
							  NegationList* dels);
static const Atom& make_atom(const string& predicate, const TermList& terms);
static TermList& add_name(TermList& terms, const string& name);
static const SimpleType& make_type(const string& name);


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
    frames_.back()[v->name()] = v;
  }

  const Variable* shallow_find(const string& name) const {
    VariableMap::const_iterator vi = frames_.back().find(name);
    return (vi != frames_.back().end()) ? (*vi).second : NULL;
  }

  const Variable* find(const string& name) const {
    for (vector<VariableMap>::const_reverse_iterator fi = frames_.rbegin();
	 fi != frames_.rend(); fi++) {
      VariableMap::const_iterator vi = (*fi).find(name);
      if (vi != (*fi).end()) {
	return (*vi).second;
      }
    }
    return NULL;
  }

private:
  struct VariableMap : public __gnu_cxx::hash_map<string, const Variable*> {
  };

  vector<VariableMap> frames_;
};


/* Name of current file. */
string current_file; 
/* Level of warnings. */
int warning_level;

static const Variable DURATION_VARIABLE = Variable("?duration");
static bool success = true;
static const Domain* pdomain;
static Domain* domain;
static Requirements* requirements;
static Predicate* predicate = NULL;
static Problem* problem;
static string current_predicate;
static string context;
static enum { TYPE_MAP, CONSTANT_MAP, OBJECT_MAP } name_map_type;
static const Formula* action_precond; 
static const EffectList* action_effs;
static pair<float, float> action_duration;
static Formula::FormulaTime formula_time;
static VariableList* variables;
static Context free_variables;


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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 137 "pddl.yy"
{
  const ActionSchema* action;
  const Formula* formula;
  EffectList* effects;
  const Effect* effect;
  const std::pair<AtomList*, NegationList*>* add_del_lists;
  FormulaList* formulas;
  TermList* terms;
  const Atom* atom;
  const Name* name;
  VariableList* variables;
  const Variable* variable;
  const std::string* str;
  float num;
  std::vector<std::string>* strings;
  UnionType* utype;
  const Type* type;
}
/* Line 187 of yacc.c.  */
#line 331 "pddl.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 344 "pddl.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
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
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   592

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  66
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  121
/* YYNRULES -- Number of rules.  */
#define YYNRULES  262
/* YYNRULES -- Number of states.  */
#define YYNSTATES  430

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   313

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      59,    60,    64,    62,     2,    63,     2,    65,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    61,     2,     2,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     8,    11,    13,    15,    16,
      26,    27,    29,    32,    34,    36,    39,    41,    44,    48,
      50,    52,    55,    57,    59,    62,    64,    66,    69,    74,
      76,    79,    81,    83,    85,    87,    89,    91,    93,    95,
      97,    99,   101,   103,   105,   107,   108,   114,   115,   121,
     126,   128,   131,   132,   138,   139,   150,   151,   162,   165,
     167,   168,   170,   171,   175,   178,   180,   185,   186,   195,
     196,   203,   204,   207,   209,   214,   215,   218,   220,   225,
     229,   233,   235,   236,   239,   241,   246,   252,   258,   264,
     265,   268,   270,   275,   276,   279,   280,   287,   288,   295,
     296,   303,   305,   310,   311,   320,   326,   327,   330,   331,
     338,   339,   346,   347,   348,   363,   366,   368,   371,   373,
     376,   378,   379,   385,   386,   392,   394,   397,   399,   404,
     405,   411,   412,   415,   418,   420,   422,   425,   426,   432,
     438,   440,   442,   448,   454,   460,   466,   468,   473,   475,
     476,   478,   479,   482,   484,   489,   494,   499,   504,   510,
     511,   520,   521,   530,   531,   537,   538,   541,   544,   545,
     547,   549,   550,   555,   556,   558,   560,   563,   564,   566,
     567,   570,   572,   573,   578,   579,   581,   583,   586,   588,
     591,   594,   596,   601,   603,   605,   607,   609,   611,   613,
     615,   617,   619,   621,   623,   625,   627,   629,   631,   633,
     635,   637,   639,   641,   643,   645,   647,   649,   651,   653,
     655,   657,   659,   661,   663,   665,   667,   669,   671,   673,
     675,   677,   679,   681,   683,   685,   687,   689,   691,   693,
     695,   697,   699,   701,   703,   705,   707,   709,   711,   713,
     715,   717,   719,   721,   723,   725,   727,   729,   731,   733,
     735,   737,   739
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      67,     0,    -1,    -1,    68,    69,    -1,    -1,    69,    70,
      -1,    71,    -1,   123,    -1,    -1,    59,   168,    59,    71,
     186,    60,    72,    73,    60,    -1,    -1,    79,    -1,    79,
      74,    -1,    74,    -1,    82,    -1,    82,    75,    -1,    75,
      -1,    86,    84,    -1,    86,    84,    78,    -1,    76,    -1,
      84,    -1,    84,    77,    -1,    77,    -1,    86,    -1,    86,
      78,    -1,    78,    -1,    90,    -1,    78,    90,    -1,    59,
       6,    80,    60,    -1,    81,    -1,    80,    81,    -1,     7,
      -1,     8,    -1,     9,    -1,    10,    -1,    11,    -1,    12,
      -1,    13,    -1,    14,    -1,    15,    -1,    16,    -1,    17,
      -1,    18,    -1,    19,    -1,    20,    -1,    -1,    59,    21,
      83,   153,    60,    -1,    -1,    59,    22,    85,   153,    60,
      -1,    59,    23,    87,    60,    -1,    88,    -1,    87,    88,
      -1,    -1,    59,   184,    89,   158,    60,    -1,    -1,    59,
      24,   186,    91,    27,    59,   158,    60,    93,    60,    -1,
      -1,    59,    25,   186,    92,    27,    59,   158,    60,   105,
      60,    -1,    95,    94,    -1,    94,    -1,    -1,    97,    -1,
      -1,    28,    96,   147,    -1,    30,    98,    -1,   104,    -1,
      59,   171,   101,    60,    -1,    -1,    59,   175,    99,    59,
     158,    60,    98,    60,    -1,    -1,    59,   169,   100,   147,
     102,    60,    -1,    -1,   101,    98,    -1,   104,    -1,    59,
     171,   103,    60,    -1,    -1,   103,   104,    -1,   150,    -1,
      59,   170,   150,    60,    -1,    26,   108,   106,    -1,    29,
     111,   107,    -1,   107,    -1,    -1,    30,   117,    -1,   109,
      -1,    59,   171,   110,    60,    -1,    59,    52,   182,    57,
      60,    -1,    59,    53,   182,    57,    60,    -1,    59,    61,
     182,    57,    60,    -1,    -1,   110,   109,    -1,   113,    -1,
      59,   171,   112,    60,    -1,    -1,   112,   113,    -1,    -1,
      59,   177,   179,   114,   147,    60,    -1,    -1,    59,   177,
     180,   115,   147,    60,    -1,    -1,    59,   178,   181,   116,
     147,    60,    -1,   120,    -1,    59,   171,   119,    60,    -1,
      -1,    59,   175,   118,    59,   158,    60,   117,    60,    -1,
      59,   169,   111,   120,    60,    -1,    -1,   119,   117,    -1,
      -1,    59,   177,   179,   121,   102,    60,    -1,    -1,    59,
     177,   180,   122,   102,    60,    -1,    -1,    -1,    59,   168,
      59,   123,   186,    60,   124,    59,    31,   186,    60,   125,
     126,    60,    -1,    79,   127,    -1,   127,    -1,   129,   128,
      -1,   128,    -1,   131,   138,    -1,   138,    -1,    -1,    59,
      32,   130,   153,    60,    -1,    -1,    59,    33,   132,   133,
      60,    -1,   134,    -1,   133,   134,    -1,   135,    -1,    59,
     170,   135,    60,    -1,    -1,    59,   184,   136,   137,    60,
      -1,    -1,   137,   186,    -1,   139,   142,    -1,   139,    -1,
     140,    -1,   139,   140,    -1,    -1,    59,    34,   141,   147,
      60,    -1,    59,    35,   143,   144,    60,    -1,    49,    -1,
      50,    -1,    59,    62,   144,   144,    60,    -1,    59,    63,
     144,   145,    60,    -1,    59,    64,   144,   144,    60,    -1,
      59,    65,   144,   144,    60,    -1,    57,    -1,    59,   185,
     137,    60,    -1,   185,    -1,    -1,   144,    -1,    -1,   146,
     147,    -1,   150,    -1,    59,    61,   152,    60,    -1,    59,
     170,   147,    60,    -1,    59,   171,   146,    60,    -1,    59,
     172,   146,    60,    -1,    59,   173,   147,   147,    60,    -1,
      -1,    59,   174,   148,    59,   158,    60,   147,    60,    -1,
      -1,    59,   175,   149,    59,   158,    60,   147,    60,    -1,
      -1,    59,   184,   151,   152,    60,    -1,    -1,   152,   186,
      -1,   152,   183,    -1,    -1,   154,    -1,   157,    -1,    -1,
     157,   166,   155,   156,    -1,    -1,   154,    -1,   186,    -1,
     157,   186,    -1,    -1,   159,    -1,    -1,   160,   161,    -1,
     164,    -1,    -1,   164,   166,   162,   163,    -1,    -1,   161,
      -1,   183,    -1,   164,   183,    -1,   184,    -1,   165,   184,
      -1,    63,   167,    -1,   184,    -1,    59,   176,   165,    60,
      -1,     3,    -1,     4,    -1,     5,    -1,    36,    -1,    37,
      -1,    38,    -1,    39,    -1,    40,    -1,    41,    -1,    42,
      -1,    43,    -1,    44,    -1,    45,    -1,    46,    -1,    47,
      -1,    48,    -1,    55,    -1,    55,    -1,    56,    -1,    54,
      -1,     3,    -1,     4,    -1,     5,    -1,    44,    -1,    45,
      -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,
      -1,    54,    -1,     3,    -1,     4,    -1,     5,    -1,    36,
      -1,    37,    -1,    38,    -1,    39,    -1,    40,    -1,    41,
      -1,    42,    -1,    43,    -1,    44,    -1,    45,    -1,    46,
      -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,    51,
      -1,    54,    -1,     3,    -1,     4,    -1,     5,    -1,    36,
      -1,    37,    -1,    38,    -1,    39,    -1,    40,    -1,    41,
      -1,    42,    -1,    43,    -1,    44,    -1,    45,    -1,    46,
      -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,    51,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   184,   184,   184,   187,   188,   191,   192,   201,   200,
     212,   213,   214,   215,   218,   219,   220,   223,   224,   225,
     228,   229,   230,   233,   234,   235,   238,   239,   242,   245,
     246,   249,   251,   256,   258,   260,   262,   264,   266,   268,
     270,   272,   274,   276,   278,   283,   282,   295,   294,   301,
     304,   305,   309,   308,   326,   325,   340,   339,   363,   364,
     367,   368,   371,   371,   375,   378,   384,   387,   386,   418,
     418,   440,   441,   444,   445,   450,   451,   462,   464,   468,
     471,   472,   475,   476,   479,   480,   490,   502,   514,   526,
     527,   530,   531,   540,   541,   544,   544,   546,   546,   548,
     548,   552,   553,   556,   555,   587,   610,   611,   615,   614,
     623,   622,   638,   642,   637,   661,   662,   665,   666,   669,
     671,   676,   675,   683,   682,   694,   695,   706,   708,   713,
     712,   719,   720,   724,   731,   740,   741,   745,   744,   753,
     756,   757,   760,   761,   762,   763,   764,   765,   766,   769,
     770,   778,   779,   782,   784,   804,   820,   827,   838,   847,
     846,   860,   859,   875,   874,   881,   882,   884,   897,   898,
     901,   902,   902,   906,   907,   910,   912,   916,   923,   926,
     926,   936,   945,   944,   962,   963,   966,   968,   972,   973,
     976,   979,   980,   983,   986,   989,   992,   995,   998,  1001,
    1004,  1007,  1010,  1013,  1016,  1019,  1022,  1025,  1028,  1031,
    1034,  1035,  1038,  1039,  1040,  1041,  1042,  1043,  1044,  1045,
    1046,  1047,  1048,  1051,  1052,  1053,  1054,  1055,  1056,  1057,
    1058,  1059,  1060,  1061,  1062,  1063,  1064,  1065,  1066,  1067,
    1068,  1069,  1070,  1073,  1074,  1075,  1076,  1077,  1078,  1079,
    1080,  1081,  1082,  1083,  1084,  1085,  1086,  1087,  1088,  1089,
    1090,  1091,  1092
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DEFINE", "DOMAIN_TOKEN", "PROBLEM",
  "REQUIREMENTS", "STRIPS", "TYPING", "NEGATIVE_PRECONDITIONS",
  "DISJUNCTIVE_PRECONDITIONS", "EQUALITY", "EXISTENTIAL_PRECONDITIONS",
  "UNIVERSAL_PRECONDITIONS", "QUANTIFIED_PRECONDITIONS",
  "CONDITIONAL_EFFECTS", "FLUENTS", "ADL", "DURATIVE_ACTIONS",
  "DURATION_INEQUALITIES", "CONTINUOUS_EFFECTS", "TYPES", "CONSTANTS",
  "PREDICATES", "ACTION", "DURATIVE_ACTION", "DURATION", "PARAMETERS",
  "PRECONDITION", "CONDITION", "EFFECT", "PDOMAIN", "OBJECTS", "INIT",
  "GOAL", "METRIC", "WHEN", "NOT", "AND", "OR", "IMPLY", "EXISTS",
  "FORALL", "EITHER", "AT", "OVER", "START", "END", "ALL", "MINIMIZE",
  "MAXIMIZE", "TOTAL_TIME", "LE", "GE", "NAME", "DURATION_VAR", "VARIABLE",
  "NUMBER", "ILLEGAL_TOKEN", "'('", "')'", "'='", "'+'", "'-'", "'*'",
  "'/'", "$accept", "pddl_file", "@1", "domain_or_problems",
  "domain_or_problem", "domain", "@2", "domain_body", "domain_body2",
  "domain_body3", "domain_body4", "domain_body5", "action_defs",
  "require_def", "require_keys", "require_key", "types_def", "@3",
  "constants_def", "@4", "predicates_def", "atomic_formula_skeletons",
  "atomic_formula_skeleton", "@5", "action_def", "@6", "@7", "action_body",
  "action_body2", "precondition", "@8", "effect", "eff_formula", "@9",
  "@10", "eff_formulas", "one_eff_formula", "term_literals",
  "term_literal", "da_body", "da_body2", "da_body3", "duration_constraint",
  "simple_duration_constraint", "simple_duration_constraints", "da_gd",
  "timed_gds", "timed_gd", "@11", "@12", "@13", "da_effect", "@14",
  "da_effects", "timed_effect", "@15", "@16", "problem", "@17", "@18",
  "problem_body", "problem_body2", "problem_body3", "object_decl", "@19",
  "init", "@20", "name_literals", "name_literal", "atomic_name_formula",
  "@21", "names", "goals", "goal_list", "goal", "@22", "metric_spec",
  "optimization", "ground_f_exp", "ground_f_exp_opt", "formulas",
  "formula", "@23", "@24", "atomic_term_formula", "@25", "terms",
  "name_map", "typed_names", "@26", "opt_typed_names", "name_seq",
  "opt_variables", "variables", "@27", "vars", "@28", "opt_vars",
  "variable_seq", "types", "type_spec", "type", "define", "when", "not",
  "and", "or", "imply", "exists", "forall", "either", "at", "over",
  "start", "end", "all", "duration_var", "variable", "predicate",
  "function_symbol", "name", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,    40,
      41,    61,    43,    45,    42,    47
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    66,    68,    67,    69,    69,    70,    70,    72,    71,
      73,    73,    73,    73,    74,    74,    74,    75,    75,    75,
      76,    76,    76,    77,    77,    77,    78,    78,    79,    80,
      80,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    83,    82,    85,    84,    86,
      87,    87,    89,    88,    91,    90,    92,    90,    93,    93,
      94,    94,    96,    95,    97,    98,    98,    99,    98,   100,
      98,   101,   101,   102,   102,   103,   103,   104,   104,   105,
     106,   106,   107,   107,   108,   108,   109,   109,   109,   110,
     110,   111,   111,   112,   112,   114,   113,   115,   113,   116,
     113,   117,   117,   118,   117,   117,   119,   119,   121,   120,
     122,   120,   124,   125,   123,   126,   126,   127,   127,   128,
     128,   130,   129,   132,   131,   133,   133,   134,   134,   136,
     135,   137,   137,   138,   138,   139,   139,   141,   140,   142,
     143,   143,   144,   144,   144,   144,   144,   144,   144,   145,
     145,   146,   146,   147,   147,   147,   147,   147,   147,   148,
     147,   149,   147,   151,   150,   152,   152,   152,   153,   153,
     154,   155,   154,   156,   156,   157,   157,   158,   158,   160,
     159,   161,   162,   161,   163,   163,   164,   164,   165,   165,
     166,   167,   167,   168,    71,   123,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   183,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     0,     2,     1,     1,     0,     9,
       0,     1,     2,     1,     1,     2,     1,     2,     3,     1,
       1,     2,     1,     1,     2,     1,     1,     2,     4,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     5,     0,     5,     4,
       1,     2,     0,     5,     0,    10,     0,    10,     2,     1,
       0,     1,     0,     3,     2,     1,     4,     0,     8,     0,
       6,     0,     2,     1,     4,     0,     2,     1,     4,     3,
       3,     1,     0,     2,     1,     4,     5,     5,     5,     0,
       2,     1,     4,     0,     2,     0,     6,     0,     6,     0,
       6,     1,     4,     0,     8,     5,     0,     2,     0,     6,
       0,     6,     0,     0,    14,     2,     1,     2,     1,     2,
       1,     0,     5,     0,     5,     1,     2,     1,     4,     0,
       5,     0,     2,     2,     1,     1,     2,     0,     5,     5,
       1,     1,     5,     5,     5,     5,     1,     4,     1,     0,
       1,     0,     2,     1,     4,     4,     4,     4,     5,     0,
       8,     0,     8,     0,     5,     0,     2,     2,     0,     1,
       1,     0,     4,     0,     1,     1,     2,     0,     1,     0,
       2,     1,     0,     4,     0,     1,     1,     2,     1,     2,
       2,     1,     4,     1,     1,     1,     1,     1,     1,     1,
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
static const yytype_uint16 yydefact[] =
{
       2,     0,     4,     1,     3,   194,   195,     0,     5,     6,
       7,   193,     0,     0,     0,     0,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   243,     0,     0,     8,   112,
      10,     0,     0,     0,    13,    16,    19,    22,    25,    11,
      14,    20,    23,    26,     0,     0,    45,    47,     0,     0,
       0,     9,     0,    27,     0,    12,     0,    15,     0,    21,
      23,     0,    24,    17,     0,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,     0,
      29,   168,   168,     0,     0,    50,    54,    56,    18,     0,
      28,    30,     0,   169,   170,   175,     0,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   212,    52,    49,
      51,     0,     0,   113,    46,     0,   171,   176,    48,   179,
       0,     0,     0,     0,   190,   191,   173,     0,   178,     0,
     179,   179,     0,     0,     0,   116,   118,     0,     0,   120,
     134,   135,   203,     0,   174,   172,    53,   210,   211,   180,
     181,   186,     0,     0,   121,   123,   137,     0,   115,   114,
       0,   117,     0,   119,     0,   136,   133,     0,   188,   182,
     187,    60,     0,   168,     0,     0,     0,   192,   189,   184,
      62,     0,     0,    59,    60,    61,     0,     0,     0,     0,
       0,   125,   127,     0,     0,   153,   140,   141,     0,   185,
     183,     0,     0,    64,    65,    77,    55,    58,     0,    82,
      84,    57,   122,   197,     0,   129,   124,   126,   198,   199,
     200,   201,   202,   165,     0,   151,   151,     0,   159,   161,
     163,   138,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   223,   146,     0,     0,   148,    63,   196,    69,     0,
      71,    67,     0,     0,     0,    89,     0,     0,    79,    81,
       0,     0,   131,     0,     0,     0,     0,     0,     0,     0,
     165,     0,     0,     0,     0,   131,   139,     0,     0,     0,
       0,     0,   209,     0,     0,     0,     0,     0,    82,    91,
       0,    83,   101,   128,     0,   154,   167,   166,   155,   156,
     152,   157,     0,   179,   179,     0,     0,   149,     0,     0,
       0,     0,    78,    66,    72,   179,     0,     0,     0,     0,
      85,    90,   204,   205,    93,     0,     0,    80,     0,   106,
     103,     0,   130,   132,   158,     0,     0,   164,     0,   150,
       0,     0,     0,   147,     0,     0,    73,     0,    86,    87,
      88,     0,   206,   207,    95,    97,   208,    99,     0,     0,
       0,   108,   110,     0,     0,   142,   143,   144,   145,    75,
      70,     0,     0,    92,    94,     0,     0,     0,     0,     0,
     102,   107,   179,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   105,     0,     0,     0,   160,   162,     0,    74,
      76,    68,    96,    98,   100,     0,   109,   111,     0,   104
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     4,     8,     9,    40,    43,    44,    45,
      46,    47,    48,    49,    89,    90,    50,    91,    51,    92,
      52,    94,    95,   129,    53,   121,   122,   192,   193,   194,
     211,   195,   213,   301,   297,   300,   365,   407,   214,   197,
     278,   279,   219,   220,   306,   308,   371,   309,   395,   396,
     397,   311,   380,   379,   312,   403,   404,    10,    41,   132,
     144,   145,   146,   147,   183,   148,   184,   200,   201,   202,
     282,   314,   149,   150,   151,   185,   176,   208,   264,   360,
     285,   320,   288,   289,   205,   290,   283,   102,   103,   136,
     155,   104,   137,   138,   139,   159,   189,   210,   160,   177,
     126,   134,    12,   268,   269,   235,   236,   237,   238,   239,
     153,   345,   346,   374,   375,   377,   303,   161,   240,   265,
     105
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -363
static const yytype_int16 yypact[] =
{
    -363,    22,  -363,  -363,    24,  -363,  -363,    33,  -363,  -363,
    -363,  -363,   -16,    24,   444,   444,  -363,  -363,  -363,  -363,
    -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,
    -363,  -363,  -363,  -363,  -363,  -363,    -9,    25,  -363,  -363,
      45,    59,   166,    76,  -363,  -363,  -363,  -363,    89,    97,
      99,   125,   137,  -363,   144,   572,  -363,  -363,   150,   444,
     444,  -363,    34,  -363,   156,  -363,   103,  -363,    92,  -363,
      89,   128,    89,    89,   444,  -363,  -363,  -363,  -363,  -363,
    -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,   459,
    -363,   444,   444,   458,    48,  -363,  -363,  -363,    89,   138,
    -363,  -363,   143,  -363,   190,  -363,   161,  -363,  -363,  -363,
    -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,
    -363,   198,   225,  -363,  -363,   406,  -363,  -363,  -363,   200,
     202,   203,   204,   222,  -363,  -363,   444,   207,  -363,    67,
     200,   200,     8,   209,   210,  -363,  -363,   212,   213,  -363,
     214,  -363,  -363,   458,  -363,  -363,  -363,  -363,  -363,  -363,
      -6,  -363,   215,   216,  -363,  -363,  -363,   135,  -363,  -363,
     112,  -363,   232,  -363,   139,  -363,  -363,   116,  -363,  -363,
    -363,    -3,   248,   444,   218,   235,   133,  -363,  -363,    67,
    -363,   236,   239,  -363,   271,  -363,   247,   249,   250,   524,
     140,  -363,  -363,    93,   252,  -363,  -363,  -363,   379,  -363,
    -363,   235,   496,  -363,  -363,  -363,  -363,  -363,   -22,   172,
    -363,  -363,  -363,  -363,   254,  -363,  -363,  -363,  -363,  -363,
    -363,  -363,  -363,  -363,   235,  -363,  -363,   235,  -363,  -363,
    -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,
    -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,  -363,
    -363,  -363,  -363,    30,   255,  -363,  -363,  -363,  -363,   258,
    -363,  -363,   253,   253,   253,  -363,   259,   260,  -363,  -363,
     458,   261,  -363,   242,   262,   146,   148,   235,   264,   265,
    -363,   379,   379,   379,   379,  -363,  -363,   235,   458,   266,
     153,   268,  -363,   257,   263,   274,   155,    65,   277,  -363,
     113,  -363,  -363,  -363,   325,  -363,  -363,  -363,  -363,  -363,
    -363,  -363,   272,   200,   200,   300,   379,   379,   379,   379,
     354,   275,  -363,  -363,  -363,   200,   273,   292,   293,   -29,
    -363,  -363,  -363,  -363,  -363,   196,   287,  -363,   259,  -363,
    -363,   196,  -363,  -363,  -363,   317,   318,  -363,   320,  -363,
     321,   326,   327,  -363,   510,   328,  -363,   329,  -363,  -363,
    -363,   159,  -363,  -363,  -363,  -363,  -363,  -363,   347,   189,
     348,  -363,  -363,   235,   235,  -363,  -363,  -363,  -363,  -363,
    -363,   236,   206,  -363,  -363,   235,   235,   235,   281,   352,
    -363,  -363,   200,   275,   275,   353,   371,   195,   372,   374,
     375,   377,  -363,   380,   381,   382,  -363,  -363,   524,  -363,
    -363,  -363,  -363,  -363,  -363,   260,  -363,  -363,   383,  -363
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -363,  -363,  -363,  -363,  -363,   426,  -363,  -363,   395,   396,
    -363,   394,   -26,   332,  -363,   368,  -363,  -363,   407,  -363,
     445,  -363,   364,  -363,    14,  -363,  -363,  -363,   303,  -363,
    -363,  -363,  -291,  -363,  -363,  -363,  -147,  -363,  -316,  -363,
    -363,   201,  -363,   205,  -363,   162,  -363,   145,  -363,  -363,
    -363,  -362,  -363,  -363,   142,  -363,  -363,   504,  -363,  -363,
    -363,   378,   376,  -363,  -363,  -363,  -363,  -363,   322,   294,
    -363,   229,   387,  -363,   386,  -363,  -363,  -363,  -273,  -363,
     289,  -173,  -363,  -363,  -187,  -363,   240,   -82,   390,  -363,
    -363,  -363,  -138,  -363,  -363,   342,  -363,  -363,  -363,  -363,
     389,  -363,  -363,   227,  -151,  -205,  -363,  -363,  -363,  -204,
    -363,  -299,  -363,   188,   211,  -363,   -15,  -154,   -88,   288,
     -14
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -178
static const yytype_int16 yytable[] =
{
      36,    37,   162,   163,   215,   118,   180,   270,   271,   334,
     106,   351,   204,   275,    55,   366,   228,   401,   326,   327,
     328,   329,     3,   272,   273,   190,    72,   191,     5,     6,
     272,   273,   274,   242,   243,   244,    11,   135,   266,   274,
     164,   165,   166,    13,    72,    96,    97,    98,   224,   157,
     158,    38,   234,   358,   359,   361,   362,   125,    59,    60,
      99,   284,    63,   428,   287,   178,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   299,     7,   261,    39,    63,   366,   366,   188,
     127,   420,   291,   292,   293,   294,   107,   108,   109,   351,
     408,   198,   344,   228,    42,   349,   350,    93,   119,   342,
     343,   225,    63,   215,   322,    58,    59,    60,    54,   107,
     108,   109,   157,   158,   331,    57,    58,    59,    60,   316,
     223,   228,   229,   230,   231,   232,    61,   110,   111,   112,
     113,   114,   115,   116,   215,   165,   166,   117,    62,   267,
      57,   228,    59,    60,   233,   232,    64,   342,    66,   389,
     110,   111,   112,   113,   114,   115,   116,   164,   165,   166,
     117,   316,    55,   166,   186,    74,   187,    56,    57,    58,
      59,    60,   206,   207,    68,   355,   356,    56,    57,    58,
      59,    60,   225,    16,    17,    18,    71,   367,   123,   199,
     226,   276,   277,   124,   215,   203,   319,   203,   321,    93,
     405,   406,   212,   333,   339,   340,   215,   215,   392,   393,
     215,   128,   409,   410,   411,   130,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,   372,   373,    35,    16,    17,    18,   310,   400,
     342,   343,   131,   125,   418,   419,   414,   415,   304,   305,
    -177,   140,   141,   142,   413,   152,   166,   156,   167,   317,
     169,   170,   172,   174,   196,   181,   182,   199,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,   203,   212,    35,   157,   158,   216,
     353,   191,   315,    16,    17,    18,   218,   277,   302,   221,
     222,   317,   241,   280,   336,   296,   353,   298,   307,   310,
     337,   313,   318,   323,   324,   342,   332,   335,    16,    17,
      18,   338,   354,   368,   364,   376,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,   369,   370,    35,   157,   158,    16,    17,    18,
     357,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,   383,   384,    35,
     385,   386,   242,   243,   244,   352,   387,   388,   390,   391,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,   398,   402,    35,   107,
     108,   109,   412,   416,   363,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   417,   421,   261,   422,   423,   262,   424,   263,    14,
     425,   426,   427,   429,    65,    69,    67,    16,    17,    18,
     110,   111,   112,   113,   114,   115,   116,   101,   120,    73,
     117,   107,   108,   109,   143,   133,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    70,   217,    35,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   347,
     378,   341,   117,   107,   108,   109,   394,    15,   281,   100,
     399,   168,   227,   171,   330,   286,   154,   107,   108,   109,
     325,   209,   267,   223,   228,   173,   175,   348,   232,   381,
     110,   111,   112,   113,   114,   115,   116,   223,   228,   179,
     117,   295,     0,     0,   110,   111,   112,   113,   114,   115,
     116,   223,   382,     0,   117,     0,     0,     0,   110,   111,
     112,   113,   114,   115,   116,     0,     0,     0,   117,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88
};

static const yytype_int16 yycheck[] =
{
      14,    15,   140,   141,   191,    93,   160,   212,   212,   300,
      92,   310,   185,   218,     6,   331,    38,   379,   291,   292,
     293,   294,     0,    52,    53,    28,    52,    30,     4,     5,
      52,    53,    61,     3,     4,     5,     3,   125,   211,    61,
      32,    33,    34,    59,    70,    59,    60,    73,   199,    55,
      56,    60,   203,   326,   327,   328,   329,    63,    24,    25,
      74,   234,    48,   425,   237,   153,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,   269,    59,    54,    60,    72,   403,   404,   177,
     104,   407,    62,    63,    64,    65,     3,     4,     5,   398,
     391,   183,   307,    38,    59,   310,   310,    59,    60,    44,
      45,   199,    98,   300,   287,    23,    24,    25,    59,     3,
       4,     5,    55,    56,   297,    22,    23,    24,    25,   283,
      37,    38,    39,    40,    41,    42,    60,    44,    45,    46,
      47,    48,    49,    50,   331,    33,    34,    54,    59,    36,
      22,    38,    24,    25,    61,    42,    59,    44,    59,   364,
      44,    45,    46,    47,    48,    49,    50,    32,    33,    34,
      54,   325,     6,    34,    35,    31,    60,    21,    22,    23,
      24,    25,    49,    50,    59,   323,   324,    21,    22,    23,
      24,    25,   280,     3,     4,     5,    59,   335,    60,    59,
      60,    29,    30,    60,   391,    59,    60,    59,    60,    59,
     383,   384,    59,    60,    59,    60,   403,   404,    59,    60,
     407,    60,   395,   396,   397,    27,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    46,    47,    54,     3,     4,     5,    59,    60,
      44,    45,    27,    63,    59,    60,   403,   404,   273,   274,
      60,    59,    59,    59,   402,    43,    34,    60,    59,   283,
      60,    59,    59,    59,    26,    60,    60,    59,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    59,    59,    54,    55,    56,    60,
     314,    30,    60,     3,     4,     5,    59,    30,    55,    60,
      60,   325,    60,    59,    57,    60,   330,    59,    59,    59,
      57,    60,    60,    59,    59,    44,    60,    59,     3,     4,
       5,    57,    60,    60,    59,    48,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    60,    60,    54,    55,    56,     3,     4,     5,
      60,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    60,    60,    54,
      60,    60,     3,     4,     5,    60,    60,    60,    60,    60,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    59,    59,    54,     3,
       4,     5,    60,    60,    60,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    60,    60,    54,    60,    60,    57,    60,    59,    13,
      60,    60,    60,    60,    49,    51,    50,     3,     4,     5,
      44,    45,    46,    47,    48,    49,    50,    89,    94,    52,
      54,     3,     4,     5,   132,    59,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    51,   194,    54,     3,
       4,     5,    44,    45,    46,    47,    48,    49,    50,   308,
     348,   306,    54,     3,     4,     5,   371,    13,   224,    60,
     378,   143,   200,   147,   295,   236,   136,     3,     4,     5,
     290,   189,    36,    37,    38,   148,   150,   310,    42,   351,
      44,    45,    46,    47,    48,    49,    50,    37,    38,   160,
      54,   263,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    37,   351,    -1,    54,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    -1,    -1,    -1,    54,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    67,    68,     0,    69,     4,     5,    59,    70,    71,
     123,     3,   168,    59,    71,   123,     3,     4,     5,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    54,   186,   186,    60,    60,
      72,   124,    59,    73,    74,    75,    76,    77,    78,    79,
      82,    84,    86,    90,    59,     6,    21,    22,    23,    24,
      25,    60,    59,    90,    59,    74,    59,    75,    59,    77,
      86,    59,    78,    84,    31,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    80,
      81,    83,    85,    59,    87,    88,   186,   186,    78,   186,
      60,    81,   153,   154,   157,   186,   153,     3,     4,     5,
      44,    45,    46,    47,    48,    49,    50,    54,   184,    60,
      88,    91,    92,    60,    60,    63,   166,   186,    60,    89,
      27,    27,   125,    59,   167,   184,   155,   158,   159,   160,
      59,    59,    59,    79,   126,   127,   128,   129,   131,   138,
     139,   140,    43,   176,   154,   156,    60,    55,    56,   161,
     164,   183,   158,   158,    32,    33,    34,    59,   127,    60,
      59,   128,    59,   138,    59,   140,   142,   165,   184,   166,
     183,    60,    60,   130,   132,   141,    35,    60,   184,   162,
      28,    30,    93,    94,    95,    97,    26,   105,   153,    59,
     133,   134,   135,    59,   147,   150,    49,    50,   143,   161,
     163,    96,    59,    98,   104,   150,    60,    94,    59,   108,
     109,    60,    60,    37,   170,   184,    60,   134,    38,    39,
      40,    41,    42,    61,   170,   171,   172,   173,   174,   175,
     184,    60,     3,     4,     5,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    54,    57,    59,   144,   185,   147,    36,   169,   170,
     171,   175,    52,    53,    61,   171,    29,    30,   106,   107,
      59,   135,   136,   152,   147,   146,   146,   147,   148,   149,
     151,    62,    63,    64,    65,   185,    60,   100,    59,   150,
     101,    99,    55,   182,   182,   182,   110,    59,   111,   113,
      59,   117,   120,    60,   137,    60,   183,   186,    60,    60,
     147,    60,   147,    59,    59,   152,   144,   144,   144,   144,
     137,   147,    60,    60,    98,    59,    57,    57,    57,    59,
      60,   109,    44,    45,   171,   177,   178,   107,   169,   171,
     175,   177,    60,   186,    60,   158,   158,    60,   144,   144,
     145,   144,   144,    60,    59,   102,   104,   158,    60,    60,
      60,   112,    46,    47,   179,   180,    48,   181,   111,   119,
     118,   179,   180,    60,    60,    60,    60,    60,    60,   171,
      60,    60,    59,    60,   113,   114,   115,   116,    59,   120,
      60,   117,    59,   121,   122,   147,   147,   103,    98,   147,
     147,   147,    60,   158,   102,   102,    60,    60,    59,    60,
     104,    60,    60,    60,    60,    60,    60,    60,   117,    60
};

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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
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
    while (YYID (0))
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
  YYUSE (yyvaluep);

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
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

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

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
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

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
#line 184 "pddl.yy"
    { line_number = 1; }
    break;

  case 3:
#line 184 "pddl.yy"
    { if (!success) YYERROR; }
    break;

  case 6:
#line 191 "pddl.yy"
    { context = ""; }
    break;

  case 7:
#line 192 "pddl.yy"
    { context = ""; }
    break;

  case 8:
#line 201 "pddl.yy"
    {
	     pdomain = NULL;
	     domain = new Domain(*(yyvsp[(5) - (6)].str));
	     delete (yyvsp[(5) - (6)].str);
	     requirements = &domain->requirements;
	     context = " in domain `" + domain->name() + "'";
	     problem = NULL;
	   }
    break;

  case 26:
#line 238 "pddl.yy"
    { add_action(*(yyvsp[(1) - (1)].action)); }
    break;

  case 27:
#line 239 "pddl.yy"
    { add_action(*(yyvsp[(2) - (2)].action)); }
    break;

  case 31:
#line 250 "pddl.yy"
    { requirements->strips = true; }
    break;

  case 32:
#line 252 "pddl.yy"
    {
		  requirements->typing = true;
		  domain->add_type(SimpleType::OBJECT);
		}
    break;

  case 33:
#line 257 "pddl.yy"
    { requirements->negative_preconditions = true; }
    break;

  case 34:
#line 259 "pddl.yy"
    { requirements->disjunctive_preconditions = true; }
    break;

  case 35:
#line 261 "pddl.yy"
    { requirements->equality = true; }
    break;

  case 36:
#line 263 "pddl.yy"
    { requirements->existential_preconditions = true; }
    break;

  case 37:
#line 265 "pddl.yy"
    { requirements->universal_preconditions = true; }
    break;

  case 38:
#line 267 "pddl.yy"
    { requirements->quantified_preconditions(); }
    break;

  case 39:
#line 269 "pddl.yy"
    { requirements->conditional_effects = true; }
    break;

  case 40:
#line 271 "pddl.yy"
    { throw Unimplemented("`:fluents' not supported"); }
    break;

  case 41:
#line 273 "pddl.yy"
    { requirements->adl(); }
    break;

  case 42:
#line 275 "pddl.yy"
    { requirements->durative_actions = true; }
    break;

  case 43:
#line 277 "pddl.yy"
    { requirements->duration_inequalities = true; }
    break;

  case 44:
#line 279 "pddl.yy"
    { throw Unimplemented("`:continuous-effects' not supported"); }
    break;

  case 45:
#line 283 "pddl.yy"
    {
		if (!requirements->typing) {
		  domain->add_type(SimpleType::OBJECT);
		  yywarning("assuming `:typing' requirement");
		  requirements->typing = true;
		}
		name_map_type = TYPE_MAP;
	      }
    break;

  case 47:
#line 295 "pddl.yy"
    {
		    name_map_type = CONSTANT_MAP;
		  }
    break;

  case 52:
#line 309 "pddl.yy"
    {
			      predicate = new Predicate(*(yyvsp[(2) - (2)].str));
			      delete (yyvsp[(2) - (2)].str);
			    }
    break;

  case 53:
#line 314 "pddl.yy"
    {
			      add_predicate(*predicate);
			      predicate = NULL;
			    }
    break;

  case 54:
#line 326 "pddl.yy"
    {
		 context = (" in action `" + *(yyvsp[(3) - (3)].str) + "' of domain `"
			    + domain->name() + "'");
		 free_variables.push_frame();
	       }
    break;

  case 55:
#line 332 "pddl.yy"
    {
		 free_variables.pop_frame();
		 (yyval.action) =
		   new ActionSchema(*(yyvsp[(3) - (10)].str), *(yyvsp[(7) - (10)].variables), *action_precond,
				    action_effs->strengthen(*action_precond));
		 delete (yyvsp[(3) - (10)].str);
	       }
    break;

  case 56:
#line 340 "pddl.yy"
    {
		 if (!requirements->durative_actions) {
		   yywarning("assuming `:durative-actions' requirement");
		   requirements->durative_actions = true;
		 }
		 context = (" in action `" + *(yyvsp[(3) - (3)].str) + "' of domain `"
			    + domain->name() + "'");
		 free_variables.push_frame();
		 free_variables.insert(&DURATION_VARIABLE);
		 action_duration = make_pair(0.0f, INFINITY);
	       }
    break;

  case 57:
#line 352 "pddl.yy"
    {
		 free_variables.pop_frame();
		 (yyval.action) =
		   new ActionSchema(*(yyvsp[(3) - (10)].str), *(yyvsp[(7) - (10)].variables), *action_precond,
				    action_effs->strengthen(*action_precond),
				    action_duration.first,
				    action_duration.second);
		 delete (yyvsp[(3) - (10)].str);
	       }
    break;

  case 58:
#line 363 "pddl.yy"
    { action_precond = (yyvsp[(1) - (2)].formula); }
    break;

  case 59:
#line 364 "pddl.yy"
    {action_precond = &Formula::TRUE; }
    break;

  case 60:
#line 367 "pddl.yy"
    { action_effs = &EffectList::EMPTY; }
    break;

  case 61:
#line 368 "pddl.yy"
    { action_effs = (yyvsp[(1) - (1)].effects); }
    break;

  case 62:
#line 371 "pddl.yy"
    { formula_time = Formula::OVER_ALL; }
    break;

  case 63:
#line 372 "pddl.yy"
    { (yyval.formula) = (yyvsp[(3) - (3)].formula); }
    break;

  case 64:
#line 375 "pddl.yy"
    { (yyval.effects) = (yyvsp[(2) - (2)].effects); }
    break;

  case 65:
#line 379 "pddl.yy"
    {
		  (yyval.effects) = new EffectList(new Effect(*(yyvsp[(1) - (1)].add_del_lists)->first, *(yyvsp[(1) - (1)].add_del_lists)->second,
						 Effect::AT_END));
		  delete (yyvsp[(1) - (1)].add_del_lists);
		}
    break;

  case 66:
#line 385 "pddl.yy"
    { (yyval.effects) = (yyvsp[(3) - (4)].effects); }
    break;

  case 67:
#line 387 "pddl.yy"
    {
		  if (!requirements->conditional_effects) {
		    yywarning("assuming `:conditional-effects' requirement");
		    requirements->conditional_effects = true;
		  }
		  free_variables.push_frame();
		}
    break;

  case 68:
#line 395 "pddl.yy"
    {
		  free_variables.pop_frame();
		  if ((yyvsp[(5) - (8)].variables)->empty()) {
		    (yyval.effects) = (yyvsp[(7) - (8)].effects);
		  } else {
		    for (size_t i = 0; i < (yyvsp[(7) - (8)].effects)->size(); i++) {
		      const Effect& e = *(*(yyvsp[(7) - (8)].effects))[i];
		      if (e.forall().empty()) {
			(*(yyvsp[(7) - (8)].effects))[i] = new Effect(*(yyvsp[(5) - (8)].variables), e.condition(),
					      e.add_list(), e.del_list(),
					      e.when());
		      } else {
			VariableList& forall = *(new VariableList(*(yyvsp[(5) - (8)].variables)));
			copy(e.forall().begin(), e.forall().end(),
			     back_inserter(forall));
			(*(yyvsp[(7) - (8)].effects))[i] = new Effect(forall, e.condition(),
					      e.add_list(), e.del_list(),
					      e.when());
		      }
		    }
		    (yyval.effects) = (yyvsp[(7) - (8)].effects);
		  }
		}
    break;

  case 69:
#line 418 "pddl.yy"
    { formula_time = Formula::OVER_ALL; }
    break;

  case 70:
#line 420 "pddl.yy"
    {
		  if (!requirements->conditional_effects) {
		    yywarning("assuming `:conditional-effects' requirement");
		    requirements->conditional_effects = true;
		  }
		  if ((yyvsp[(4) - (6)].formula)->tautology()) {
		    (yyval.effects) = new EffectList(new Effect(*(yyvsp[(5) - (6)].add_del_lists)->first, *(yyvsp[(5) - (6)].add_del_lists)->second,
						   Effect::AT_END));
		  } else if ((yyvsp[(4) - (6)].formula)->contradiction()) {
		    (yyval.effects) = new EffectList();
		  } else {
		    (yyval.effects) = new EffectList(new Effect(*(yyvsp[(4) - (6)].formula),
						   *(yyvsp[(5) - (6)].add_del_lists)->first, *(yyvsp[(5) - (6)].add_del_lists)->second,
						   Effect::AT_END));
		  }
		  delete (yyvsp[(5) - (6)].add_del_lists);
		}
    break;

  case 71:
#line 440 "pddl.yy"
    { (yyval.effects) = new EffectList(); }
    break;

  case 72:
#line 442 "pddl.yy"
    { copy((yyvsp[(2) - (2)].effects)->begin(), (yyvsp[(2) - (2)].effects)->end(), back_inserter(*(yyvsp[(1) - (2)].effects))); (yyval.effects) = (yyvsp[(1) - (2)].effects); }
    break;

  case 74:
#line 446 "pddl.yy"
    { (yyval.add_del_lists) = (yyvsp[(3) - (4)].add_del_lists); }
    break;

  case 75:
#line 450 "pddl.yy"
    { (yyval.add_del_lists) = &make_add_del(new AtomList(), new NegationList()); }
    break;

  case 76:
#line 452 "pddl.yy"
    {
		    copy((yyvsp[(2) - (2)].add_del_lists)->first->begin(), (yyvsp[(2) - (2)].add_del_lists)->first->end(),
			 back_inserter(*(yyvsp[(1) - (2)].add_del_lists)->first));
		    copy((yyvsp[(2) - (2)].add_del_lists)->second->begin(), (yyvsp[(2) - (2)].add_del_lists)->second->end(),
			 back_inserter(*(yyvsp[(1) - (2)].add_del_lists)->second));
		    (yyval.add_del_lists) = (yyvsp[(1) - (2)].add_del_lists);
		    delete (yyvsp[(2) - (2)].add_del_lists);
		  }
    break;

  case 77:
#line 463 "pddl.yy"
    { (yyval.add_del_lists) = &make_add_del(new AtomList((yyvsp[(1) - (1)].atom)), new NegationList()); }
    break;

  case 78:
#line 465 "pddl.yy"
    { (yyval.add_del_lists) = &make_add_del(new AtomList(), new NegationList((yyvsp[(3) - (4)].atom))); }
    break;

  case 80:
#line 471 "pddl.yy"
    { action_precond = (yyvsp[(2) - (3)].formula); }
    break;

  case 81:
#line 472 "pddl.yy"
    { action_precond = &Formula::TRUE; }
    break;

  case 82:
#line 475 "pddl.yy"
    { action_effs = &EffectList::EMPTY; }
    break;

  case 83:
#line 476 "pddl.yy"
    { action_effs = (yyvsp[(2) - (2)].effects); }
    break;

  case 85:
#line 481 "pddl.yy"
    {
			  if (!requirements->duration_inequalities) {
			    yywarning("assuming `:duration-inequalities' "
				      "requirement");
			    requirements->duration_inequalities = true;
			  }
			}
    break;

  case 86:
#line 491 "pddl.yy"
    {
				 if (!requirements->duration_inequalities) {
				   yywarning("assuming "
					     "`:duration-inequalities' "
					     "requirement");
				   requirements->duration_inequalities = true;
				 }
				 if ((yyvsp[(4) - (5)].num) < action_duration.second) {
				   action_duration.second = (yyvsp[(4) - (5)].num);
				 }
			       }
    break;

  case 87:
#line 503 "pddl.yy"
    {
				 if (!requirements->duration_inequalities) {
				   yywarning("assuming "
					     "`:duration-inequalities' "
					     "requirement");
				   requirements->duration_inequalities = true;
				 }
				 if (action_duration.first < (yyvsp[(4) - (5)].num)) {
				   action_duration.first = (yyvsp[(4) - (5)].num);
				 }
			       }
    break;

  case 88:
#line 515 "pddl.yy"
    {
				 if (action_duration.first <= (yyvsp[(4) - (5)].num)
				     && (yyvsp[(4) - (5)].num) <= action_duration.second) {
				   action_duration.first =
				     action_duration.second = (yyvsp[(4) - (5)].num);
				 } else {
				   action_duration = make_pair(1.0f, -1.0f);
				 }
			       }
    break;

  case 92:
#line 532 "pddl.yy"
    {
	    (yyval.formula) = &Formula::TRUE;
	    for (FormulaListIter fi = (yyvsp[(3) - (4)].formulas)->begin(); fi != (yyvsp[(3) - (4)].formulas)->end(); fi++) {
	      (yyval.formula) = &(*(yyval.formula) && **fi);
	    }
	  }
    break;

  case 93:
#line 540 "pddl.yy"
    { (yyval.formulas) = new FormulaList(); }
    break;

  case 94:
#line 541 "pddl.yy"
    { (yyvsp[(1) - (2)].formulas)->push_back((yyvsp[(2) - (2)].formula)); (yyval.formulas) = (yyvsp[(1) - (2)].formulas); }
    break;

  case 95:
#line 544 "pddl.yy"
    { formula_time = Formula::AT_START; }
    break;

  case 96:
#line 545 "pddl.yy"
    { (yyval.formula) = (yyvsp[(5) - (6)].formula); }
    break;

  case 97:
#line 546 "pddl.yy"
    { formula_time = Formula::AT_END; }
    break;

  case 98:
#line 547 "pddl.yy"
    { (yyval.formula) = (yyvsp[(5) - (6)].formula); }
    break;

  case 99:
#line 548 "pddl.yy"
    { formula_time = Formula::OVER_ALL; }
    break;

  case 100:
#line 549 "pddl.yy"
    { (yyval.formula) = (yyvsp[(5) - (6)].formula); }
    break;

  case 102:
#line 554 "pddl.yy"
    { (yyval.effects) = (yyvsp[(3) - (4)].effects); }
    break;

  case 103:
#line 556 "pddl.yy"
    {
		if (!requirements->conditional_effects) {
		  yywarning("assuming `:conditional-effects' requirement");
		  requirements->conditional_effects = true;
		}
		free_variables.push_frame();
	      }
    break;

  case 104:
#line 564 "pddl.yy"
    {
		free_variables.pop_frame();
		if ((yyvsp[(5) - (8)].variables)->empty()) {
		  (yyval.effects) = (yyvsp[(7) - (8)].effects);
		} else {
		  for (size_t i = 0; i < (yyvsp[(7) - (8)].effects)->size(); i++) {
		    const Effect& e = *(*(yyvsp[(7) - (8)].effects))[i];
		    if (e.forall().empty()) {
		      (*(yyvsp[(7) - (8)].effects))[i] = new Effect(*(yyvsp[(5) - (8)].variables), e.condition(),
					    e.add_list(), e.del_list(),
					    e.when());
		    } else {
		      VariableList& forall = *(new VariableList(*(yyvsp[(5) - (8)].variables)));
		      copy(e.forall().begin(), e.forall().end(),
			   back_inserter(forall));
		      (*(yyvsp[(7) - (8)].effects))[i] = new Effect(forall, e.condition(),
					    e.add_list(), e.del_list(),
					    e.when());
		    }
		  }
		  (yyval.effects) = (yyvsp[(7) - (8)].effects);
		}
	      }
    break;

  case 105:
#line 588 "pddl.yy"
    {
		if (!requirements->conditional_effects) {
		  yywarning("assuming `:conditional-effects' requirement");
		  requirements->conditional_effects = true;
		}
		if ((yyvsp[(3) - (5)].formula)->tautology()) {
		  (yyval.effects) = (yyvsp[(4) - (5)].effects);
		} else if ((yyvsp[(3) - (5)].formula)->contradiction()) {
		  (yyval.effects) = new EffectList();
		} else {
		  for (size_t i = 0; i < (yyvsp[(4) - (5)].effects)->size(); i++) {
		    const Effect& e = *(*(yyvsp[(4) - (5)].effects))[i];
		    (*(yyvsp[(4) - (5)].effects))[i] = new Effect(e.forall(), *(yyvsp[(3) - (5)].formula),
					  e.add_list(), e.del_list(),
					  e.when());
		  }
		  (yyval.effects) = (yyvsp[(4) - (5)].effects);
		}
	      }
    break;

  case 106:
#line 610 "pddl.yy"
    { (yyval.effects) = new EffectList(); }
    break;

  case 107:
#line 612 "pddl.yy"
    { copy((yyvsp[(2) - (2)].effects)->begin(), (yyvsp[(2) - (2)].effects)->end(), back_inserter(*(yyvsp[(1) - (2)].effects))); (yyval.effects) = (yyvsp[(1) - (2)].effects); }
    break;

  case 108:
#line 615 "pddl.yy"
    { formula_time = Formula::AT_START; }
    break;

  case 109:
#line 617 "pddl.yy"
    {
		   (yyval.effects) = new EffectList(new Effect(*(yyvsp[(5) - (6)].add_del_lists)->first, *(yyvsp[(5) - (6)].add_del_lists)->second,
						  Effect::AT_START));
		   delete (yyvsp[(5) - (6)].add_del_lists);
		 }
    break;

  case 110:
#line 623 "pddl.yy"
    { formula_time = Formula::AT_END; }
    break;

  case 111:
#line 625 "pddl.yy"
    {
		   (yyval.effects) = new EffectList(new Effect(*(yyvsp[(5) - (6)].add_del_lists)->first, *(yyvsp[(5) - (6)].add_del_lists)->second,
						  Effect::AT_END));
		   delete (yyvsp[(5) - (6)].add_del_lists);
		 }
    break;

  case 112:
#line 638 "pddl.yy"
    {
	      context = " in problem `" + *(yyvsp[(5) - (6)].str) + "'";
	    }
    break;

  case 113:
#line 642 "pddl.yy"
    {
	      domain = NULL;
	      pdomain = Domain::find(*(yyvsp[(10) - (11)].str));
	      delete (yyvsp[(10) - (11)].str);
	      if (pdomain != NULL) {
		requirements = new Requirements(pdomain->requirements);
	      } else {
		yyerror("undeclared domain used");
		requirements = new Requirements();
	      }
	      problem = new Problem(*(yyvsp[(5) - (11)].str), *pdomain);
	      delete (yyvsp[(5) - (11)].str);
	    }
    break;

  case 114:
#line 656 "pddl.yy"
    {
	      delete requirements;
	    }
    break;

  case 119:
#line 670 "pddl.yy"
    { problem->set_init(*(yyvsp[(1) - (2)].effect)); problem->set_goal(*(yyvsp[(2) - (2)].formula)); }
    break;

  case 120:
#line 672 "pddl.yy"
    { problem->set_goal(*(yyvsp[(1) - (1)].formula)); }
    break;

  case 121:
#line 676 "pddl.yy"
    {
		  name_map_type = OBJECT_MAP;
		}
    break;

  case 123:
#line 683 "pddl.yy"
    {
	   context =
	     " in initial conditions of problem `" + problem->name() + "'";
	 }
    break;

  case 124:
#line 688 "pddl.yy"
    {
	   (yyval.effect) = new Effect(*(yyvsp[(4) - (5)].add_del_lists)->first, *(yyvsp[(4) - (5)].add_del_lists)->second, Effect::AT_END);
	   delete (yyvsp[(4) - (5)].add_del_lists);
	 }
    break;

  case 126:
#line 696 "pddl.yy"
    {
		    copy((yyvsp[(2) - (2)].add_del_lists)->first->begin(), (yyvsp[(2) - (2)].add_del_lists)->first->end(),
			 back_inserter(*(yyvsp[(1) - (2)].add_del_lists)->first));
		    copy((yyvsp[(2) - (2)].add_del_lists)->second->begin(), (yyvsp[(2) - (2)].add_del_lists)->second->end(),
			 back_inserter(*(yyvsp[(1) - (2)].add_del_lists)->second));
		    (yyval.add_del_lists) = (yyvsp[(1) - (2)].add_del_lists);
		    delete (yyvsp[(2) - (2)].add_del_lists);
		  }
    break;

  case 127:
#line 707 "pddl.yy"
    { (yyval.add_del_lists) = &make_add_del(new AtomList((yyvsp[(1) - (1)].atom)), new NegationList()); }
    break;

  case 128:
#line 709 "pddl.yy"
    { (yyval.add_del_lists) = &make_add_del(new AtomList(), new NegationList()); }
    break;

  case 129:
#line 713 "pddl.yy"
    { current_predicate = *(yyvsp[(2) - (2)].str); }
    break;

  case 130:
#line 715 "pddl.yy"
    { (yyval.atom) = &make_atom(*(yyvsp[(2) - (5)].str), *(yyvsp[(4) - (5)].terms)); delete (yyvsp[(2) - (5)].str); }
    break;

  case 131:
#line 719 "pddl.yy"
    { (yyval.terms) = new TermList(); }
    break;

  case 132:
#line 721 "pddl.yy"
    { (yyval.terms) = &add_name(*(yyvsp[(1) - (2)].terms), *(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); }
    break;

  case 133:
#line 725 "pddl.yy"
    {
	    (yyval.formula) = &Formula::TRUE;
	    for (FormulaListIter fi = (yyvsp[(1) - (2)].formulas)->begin(); fi != (yyvsp[(1) - (2)].formulas)->end(); fi++) {
	      (yyval.formula) = &(*(yyval.formula) && **fi);
	    }
	  }
    break;

  case 134:
#line 732 "pddl.yy"
    {
	    (yyval.formula) = &Formula::TRUE;
	    for (FormulaListIter fi = (yyvsp[(1) - (1)].formulas)->begin(); fi != (yyvsp[(1) - (1)].formulas)->end(); fi++) {
	      (yyval.formula) = &(*(yyval.formula) && **fi);
	    }
	  }
    break;

  case 135:
#line 740 "pddl.yy"
    { (yyval.formulas) = new FormulaList((yyvsp[(1) - (1)].formula)); }
    break;

  case 136:
#line 741 "pddl.yy"
    { (yyvsp[(1) - (2)].formulas)->push_back((yyvsp[(2) - (2)].formula)); (yyval.formulas) = (yyvsp[(1) - (2)].formulas); }
    break;

  case 137:
#line 745 "pddl.yy"
    {
	   context = " in goal of problem `" + problem->name() + "'";
	   formula_time = Formula::AT_START;
	 }
    break;

  case 138:
#line 750 "pddl.yy"
    { (yyval.formula) = (yyvsp[(4) - (5)].formula); }
    break;

  case 140:
#line 756 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 141:
#line 757 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 146:
#line 764 "pddl.yy"
    {}
    break;

  case 147:
#line 765 "pddl.yy"
    { delete (yyvsp[(2) - (4)].str); }
    break;

  case 148:
#line 766 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 151:
#line 778 "pddl.yy"
    { (yyval.formulas) = new FormulaList(); }
    break;

  case 152:
#line 779 "pddl.yy"
    { (yyvsp[(1) - (2)].formulas)->push_back((yyvsp[(2) - (2)].formula)); (yyval.formulas) = (yyvsp[(1) - (2)].formulas); }
    break;

  case 153:
#line 783 "pddl.yy"
    { (yyval.formula) = (yyvsp[(1) - (1)].atom); }
    break;

  case 154:
#line 785 "pddl.yy"
    {
	      if (!requirements->equality) {
		yywarning("assuming `:equality' requirement");
		requirements->equality = true;
	      }
	      if ((yyvsp[(3) - (4)].terms)->size() != 2) {
		yyerror(tostring((yyvsp[(3) - (4)].terms)->size()) + " parameter"
			+ std::string(((yyvsp[(3) - (4)].terms)->size() == 1) ? "" : "s")
			+ " passed to predicate `=' expecting 2");
	      }
	      const Term& t1 = *(*(yyvsp[(3) - (4)].terms))[0];
	      const Term& t2 = *(*(yyvsp[(3) - (4)].terms))[1];
	      if (t1.type().subtype(t2.type())
		  || t2.type().subtype(t1.type())) {
		(yyval.formula) = new Equality(t1, t2);
	      } else {
		(yyval.formula) = &Formula::FALSE;
	      }
	    }
    break;

  case 155:
#line 805 "pddl.yy"
    {
	      if (typeid(*(yyvsp[(3) - (4)].formula)) == typeid(Atom)) {
		if (!requirements->negative_preconditions) {
		  yywarning("assuming `:negative-preconditions' "
			    "requirement");
		  requirements->negative_preconditions = true;
		}
	      } else if (!requirements->disjunctive_preconditions
			 && typeid(*(yyvsp[(3) - (4)].formula)) != typeid(Equality)) {
		yywarning("assuming `:disjunctive-preconditions' "
			  "requirement");
		requirements->disjunctive_preconditions = true;
	      }
	      (yyval.formula) = &!*(yyvsp[(3) - (4)].formula);
	    }
    break;

  case 156:
#line 821 "pddl.yy"
    {
	      (yyval.formula) = &Formula::TRUE;
	      for (FormulaListIter fi = (yyvsp[(3) - (4)].formulas)->begin(); fi != (yyvsp[(3) - (4)].formulas)->end(); fi++) {
		(yyval.formula) = &(*(yyval.formula) && **fi);
	      }
	    }
    break;

  case 157:
#line 828 "pddl.yy"
    {
	      if (!requirements->disjunctive_preconditions) {
		yywarning("assuming `:disjunctive-preconditions' requirement");
		requirements->disjunctive_preconditions = true;
	      }
	      (yyval.formula) = &Formula::FALSE;
	      for (FormulaListIter fi = (yyvsp[(3) - (4)].formulas)->begin(); fi != (yyvsp[(3) - (4)].formulas)->end(); fi++) {
		(yyval.formula) = &(*(yyval.formula) || **fi);
	      }
	    }
    break;

  case 158:
#line 839 "pddl.yy"
    {
	      if (!requirements->disjunctive_preconditions) {
		yywarning("assuming `:disjunctive-preconditions' requirement");
		requirements->disjunctive_preconditions = true;
	      }
	      (yyval.formula) = &(!*(yyvsp[(3) - (5)].formula) || *(yyvsp[(4) - (5)].formula));
	    }
    break;

  case 159:
#line 847 "pddl.yy"
    {
	      if (!requirements->existential_preconditions) {
		yywarning("assuming `:existential-preconditions' requirement");
		requirements->existential_preconditions = true;
	      }
	      free_variables.push_frame();
	    }
    break;

  case 160:
#line 855 "pddl.yy"
    {
	      free_variables.pop_frame();
	      (yyval.formula) = (yyvsp[(5) - (8)].variables)->empty() ? (yyvsp[(7) - (8)].formula) : new ExistsFormula(*(yyvsp[(5) - (8)].variables), *(yyvsp[(7) - (8)].formula));
	    }
    break;

  case 161:
#line 860 "pddl.yy"
    {
	      if (!requirements->universal_preconditions) {
		yywarning("assuming `:universal-preconditions' requirement");
		requirements->universal_preconditions = true;
	      }
	      free_variables.push_frame();
	    }
    break;

  case 162:
#line 868 "pddl.yy"
    {
	      free_variables.pop_frame();
	      (yyval.formula) = (yyvsp[(5) - (8)].variables)->empty() ? (yyvsp[(7) - (8)].formula) : new ForallFormula(*(yyvsp[(5) - (8)].variables), *(yyvsp[(7) - (8)].formula));
	    }
    break;

  case 163:
#line 875 "pddl.yy"
    { current_predicate = *(yyvsp[(2) - (2)].str); }
    break;

  case 164:
#line 877 "pddl.yy"
    { (yyval.atom) = &make_atom(*(yyvsp[(2) - (5)].str), *(yyvsp[(4) - (5)].terms)); delete (yyvsp[(2) - (5)].str); }
    break;

  case 165:
#line 881 "pddl.yy"
    { (yyval.terms) = new TermList(); }
    break;

  case 166:
#line 883 "pddl.yy"
    { (yyval.terms) = &add_name(*(yyvsp[(1) - (2)].terms), *(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); }
    break;

  case 167:
#line 885 "pddl.yy"
    {
	    const Variable* v = free_variables.find(*(yyvsp[(2) - (2)].str));
	    if (v == NULL) {
	      yyerror("free variable `" + *(yyvsp[(2) - (2)].str) + "'");
	      v = new Variable(*(yyvsp[(2) - (2)].str));
	    }
	    (yyvsp[(1) - (2)].terms)->push_back(v);
	    (yyval.terms) = (yyvsp[(1) - (2)].terms);
	    delete (yyvsp[(2) - (2)].str);
	  }
    break;

  case 170:
#line 901 "pddl.yy"
    { add_names(*(yyvsp[(1) - (1)].strings)); delete (yyvsp[(1) - (1)].strings); }
    break;

  case 171:
#line 902 "pddl.yy"
    { add_names(*(yyvsp[(1) - (2)].strings), *(yyvsp[(2) - (2)].type)); delete (yyvsp[(1) - (2)].strings); }
    break;

  case 175:
#line 911 "pddl.yy"
    { (yyval.strings) = new std::vector<std::string>(1, *(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); }
    break;

  case 176:
#line 912 "pddl.yy"
    { (yyvsp[(1) - (2)].strings)->push_back(*(yyvsp[(2) - (2)].str)); (yyval.strings) = (yyvsp[(1) - (2)].strings); delete (yyvsp[(2) - (2)].str); }
    break;

  case 177:
#line 916 "pddl.yy"
    {
		    if (predicate == NULL) {
		      (yyval.variables) = new VariableList();
		    } else {
		      (yyval.variables) = NULL;
		    }
		  }
    break;

  case 179:
#line 926 "pddl.yy"
    {
              if (predicate == NULL) {
		variables = new VariableList();
	      } else {
		variables = NULL;
	      }
            }
    break;

  case 180:
#line 933 "pddl.yy"
    { (yyval.variables) = variables; }
    break;

  case 181:
#line 937 "pddl.yy"
    {
	   for (std::vector<std::string>::const_iterator si = (yyvsp[(1) - (1)].strings)->begin();
		si != (yyvsp[(1) - (1)].strings)->end(); si++) {
	     add_variable(*si);
	   }
	   delete (yyvsp[(1) - (1)].strings);
	 }
    break;

  case 182:
#line 945 "pddl.yy"
    {
	   const UnionType* ut = dynamic_cast<const UnionType*>((yyvsp[(2) - (2)].type));
	   for (std::vector<std::string>::const_iterator si = (yyvsp[(1) - (2)].strings)->begin();
		si != (yyvsp[(1) - (2)].strings)->end(); si++) {
	     /* Duplicate type if it is a union type so that every
		variable has its own copy. */
	     const Type* t = (ut != NULL) ? new UnionType(*ut) : (yyvsp[(2) - (2)].type);
	     add_variable(*si, *t);
	   }
	   if (ut != NULL) {
	     delete ut;
	   }
	   delete (yyvsp[(1) - (2)].strings);
	 }
    break;

  case 186:
#line 967 "pddl.yy"
    { (yyval.strings) = new std::vector<std::string>(1, *(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); }
    break;

  case 187:
#line 969 "pddl.yy"
    { (yyvsp[(1) - (2)].strings)->push_back(*(yyvsp[(2) - (2)].str)); (yyval.strings) = (yyvsp[(1) - (2)].strings); delete (yyvsp[(2) - (2)].str); }
    break;

  case 188:
#line 972 "pddl.yy"
    { (yyval.utype) = new UnionType(make_type(*(yyvsp[(1) - (1)].str))); delete (yyvsp[(1) - (1)].str); }
    break;

  case 189:
#line 973 "pddl.yy"
    { (yyval.utype) = (yyvsp[(1) - (2)].utype); (yyval.utype)->add(make_type(*(yyvsp[(2) - (2)].str))); delete (yyvsp[(2) - (2)].str); }
    break;

  case 190:
#line 976 "pddl.yy"
    { (yyval.type) = (yyvsp[(2) - (2)].type); }
    break;

  case 191:
#line 979 "pddl.yy"
    { (yyval.type) = &make_type(*(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); }
    break;

  case 192:
#line 980 "pddl.yy"
    { (yyval.type) = &UnionType::simplify(*(yyvsp[(3) - (4)].utype)); }
    break;

  case 193:
#line 983 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 194:
#line 986 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 195:
#line 989 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 196:
#line 992 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 197:
#line 995 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 198:
#line 998 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 199:
#line 1001 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 200:
#line 1004 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 201:
#line 1007 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 202:
#line 1010 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 203:
#line 1013 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 204:
#line 1016 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 205:
#line 1019 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 206:
#line 1022 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 207:
#line 1025 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 208:
#line 1028 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 209:
#line 1031 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;


/* Line 1267 of yacc.c.  */
#line 3129 "pddl.cc"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
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
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1095 "pddl.yy"


/*
 * Converts an unsigned integer to a string.
 */
static std::string tostring(unsigned int n) {
  std::string result;
  while (n > 0) {
    result = char(n % 10 + '0') + result;
    n /= 10;
  }
  return (result.length() == 0) ? "0" : result;
}


/*
 * Outputs an error message.
 */
static void yyerror(const std::string& s) {
  cerr << PACKAGE << ':' << current_file << ':' << line_number << ": "
       << s << context << endl;
  success = false;
}


/*
 * Outputs a warning.
 */
static void yywarning(const std::string& s) {
  if (warning_level > 0) {
    cerr << PACKAGE << ':' << current_file << ':' << line_number << ": "
	 << s << context << endl;
    if (warning_level > 1) {
      success = false;
    }
  }
}


/*
 * Returns the type with the given name, or NULL if it is undefined.
 */
static const SimpleType* find_type(const std::string& name) {
  if (pdomain != NULL) {
    return pdomain->find_type(name);
  } else if (domain != NULL) {
    return domain->find_type(name);
  } else {
    return NULL;
  }
}


/*
 * Returns the constant or object with the given name, or NULL if it
 * is undefined.  */
static const Name* find_constant(const std::string& name) {
  const Name* c = NULL;
  if (pdomain != NULL) {
    c = pdomain->find_constant(name);
  }
  if (c == NULL && domain != NULL) {
    c = domain->find_constant(name);
  }
  if (c == NULL && problem != NULL) {
    c = problem->find_object(name);
  }
  return c;
}


/*
 * Returns the predicate with the given name, or NULL if it is
 * undefined.
 */
static const Predicate* find_predicate(const std::string& name) {
  if (pdomain != NULL) {
    return pdomain->find_predicate(name);
  } else if (domain != NULL) {
    return domain->find_predicate(name);
  } else {
    return NULL;
  }
}


/*
 * Adds types, constants, or objects to the current domain or problem.
 */
static void add_names(const std::vector<std::string>& names, const Type& type) {
  const UnionType* ut = dynamic_cast<const UnionType*>(&type);
  for (std::vector<std::string>::const_iterator si = names.begin();
       si != names.end(); si++) {
    /* Duplicate type if it is a union type so that every name has its
       own copy. */
    const Type* t = (ut != NULL) ? new UnionType(*ut) : &type;
    const std::string& s = *si;
    if (name_map_type == TYPE_MAP) {
      if (find_type(s) == NULL) {
	domain->add_type(*(new SimpleType(s, *t)));
      } else {
	yywarning("ignoring repeated declaration of type `" + s + "'");
      }
    } else if (name_map_type == CONSTANT_MAP) {
      const Name* old_name = domain->find_constant(s);
      if (old_name != NULL) {
	domain->add_constant(*(new Name(s, UnionType::add(old_name->type(),
							  *t))));
	delete old_name;
	if (ut != NULL) {
	  delete t;
	}
      } else {
	domain->add_constant(*(new Name(s, *t)));
      }
    } else {
      if (pdomain->find_constant(s) != NULL) {
	yywarning("ignoring declaration of object `" + s
		  + "' previously declared as constant");
      } else {
	const Name* old_name = problem->find_object(s);
	if (old_name == NULL) {
	  problem->add_object(*(new Name(s, *t)));
	} else {
	  problem->add_object(*(new Name(s,
					 UnionType::add(old_name->type(),
							*t))));
	  delete old_name;
	  if (ut != NULL) {
	    delete t;
	  }
	}
      }
    }
  }
  if (ut != NULL) {
    delete ut;
  }
}


/*
 * Adds a predicate to the current domain.
 */
static void add_predicate(const Predicate& predicate) {
  if (find_predicate(predicate.name()) == NULL) {
    if (find_type(predicate.name()) == NULL) {
      domain->add_predicate(predicate);
    } else {
      yywarning("ignoring declaration of predicate `" + predicate.name()
		+ "' in domain `" + domain->name()
		+ "' previously declared as type");
      delete &predicate;
    }
  } else {
    yywarning("ignoring repeated declaration of predicate `" + predicate.name()
	      + "' in domain `" + domain->name() + "'");
    delete &predicate;
  }
}


/*
 * Adds an action schema to the current domain.
 */
static void add_action(const ActionSchema& action) {
  if (domain->find_action(action.name()) != NULL) {
    yywarning("ignoring repeated declaration of action `" + action.name()
	      + "' in domain `" + domain->name() + "'");
    delete &action;
  } else {
    domain->add_action(action);
  }
}


/*
 * Adds a variable to the current context.
 */
static void add_variable(const std::string& name, const Type& type) {
  if (predicate != NULL) {
    predicate->add_parameter(type);
  } else {
    if (free_variables.shallow_find(name) != NULL) {
      yyerror("repetition of parameter `" + name + "'");
    } else if (free_variables.find(name) != NULL) {
      yyerror("shadowing parameter `" + name + "'");
    }
    const Variable* var = new Variable(name, type);
    free_variables.insert(var);
    variables->push_back(var);
  }
}


/*
 * Creates an add/delete list pair.
 */
static const std::pair<AtomList*, NegationList*>& make_add_del(
    AtomList* adds,
    NegationList* dels) {
  return *(new std::pair<AtomList*, NegationList*>(adds, dels));
}


/*
 * Creates a formula (predicate terms[0] ...).
 */
static const Atom& make_atom(const std::string& predicate,
                             const TermList& terms) {
  const Predicate* p = find_predicate(predicate);
  if (p == NULL) {
    if (find_type(predicate) != NULL) {
      if (terms.size() != 1) {
	yyerror(tostring(terms.size())
		+ "parameters passed to type predicate `" + predicate + "'");
      }
    } else if (domain != NULL) {
      Predicate* new_p = new Predicate(predicate);
      for (size_t i = 0; i < terms.size(); i++) {
	new_p->add_parameter(SimpleType::OBJECT);
      }
      domain->add_predicate(*new_p);
      yywarning("implicit declaration of predicate `" + predicate + "'");
    } else {
      yyerror("undeclared predicate `" + predicate + "' used");
    }
  } else if (p->arity() != terms.size()) {
    yyerror(tostring(terms.size()) + " parameter"
	    + std::string((terms.size() == 1) ? "" : "s")
	    + " passed to predicate `" + predicate
	    + "' expecting " + tostring(p->arity()));
  }
  return *(new Atom(predicate, terms, formula_time));
}


/*
 * Adds a name to the given name term list.
 */
static TermList& add_name(TermList& terms, const std::string& name) {
  const Name* c = find_constant(name);
  if (c == NULL) {
    const Predicate* predicate = find_predicate(current_predicate);
    if (predicate == NULL || predicate->arity() <= terms.size()) {
      c = new Name(name, SimpleType::OBJECT);
    } else {
      c = new Name(name, predicate->type(terms.size()));
    }
    if (domain != NULL) {
      domain->add_constant(*c);
      yywarning("implicit declaration of constant `" + name + "'");
    } else {
      problem->add_object(*c);
      yywarning("implicit declaration of object `" + name + "'");
    }
  }
  terms.push_back(c);
  return terms;
}


static const SimpleType& make_type(const std::string& name) {
  const SimpleType* t = find_type(name);
  if (t == NULL) {
    const SimpleType& st = *(new SimpleType(name));
    if (domain != NULL) {
      domain->add_type(st);
      yywarning("implicit declaration of type `" + name + "'");
    } else {
      yyerror("undeclared type `" + name + "' used");
    }
    return st;
  } else {
    return *t;
  }
}

