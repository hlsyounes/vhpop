/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 20 "pddl.yy"

#include "requirements.h"
#include "problems.h"
#include "domains.h"
#include "formulas.h"
#include "types.h"
#include "exceptions.h"
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

  void insert(const Variable* v) {
    frames_.back()[v->name()] = v;
  }

  const Variable* shallow_find(const std::string& name) const {
    VariableMap::const_iterator vi = frames_.back().find(name);
    return (vi != frames_.back().end()) ? (*vi).second : NULL;
  }

  const Variable* find(const std::string& name) const {
    for (std::vector<VariableMap>::const_reverse_iterator fi =
	   frames_.rbegin(); fi != frames_.rend(); fi++) {
      VariableMap::const_iterator vi = (*fi).find(name);
      if (vi != (*fi).end()) {
	return (*vi).second;
      }
    }
    return NULL;
  }

private:
  struct VariableMap : public std::map<std::string, const Variable*> {
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
/* Predicate being parsed, or NULL if no predicate is being parsed. */
static Predicate* predicate;
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
/* Returns the simple type with the given name, or NULL if undefined. */
static const SimpleType* find_type(const std::string& name);
/* Returns the constant with the given name, or NULL if undefined. */
static const Name* find_constant(const std::string& name);
/* Returns the predicate with the given name, or NULL if undefined. */
static const Predicate* find_predicate(const std::string& name);
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
static const SimpleType& make_type(const std::string* name);
/* Returns a simple term with the given name. */
static const Term& make_term(const std::string* name);
/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name);
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
static void add_names(const std::vector<std::string>* names, const Type& type);
/* Adds variables to the current variable list. */
static void add_variables(const std::vector<std::string>* names,
			  const Type& type);
/* Adds the current predicate to the current domain. */
static void add_predicate();
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


/* Line 268 of yacc.c  */
#line 240 "pddl.cc"

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
     OBJECT_TOKEN = 298,
     EITHER = 299,
     AT = 300,
     OVER = 301,
     START = 302,
     END = 303,
     ALL = 304,
     MINIMIZE = 305,
     MAXIMIZE = 306,
     TOTAL_TIME = 307,
     LE = 308,
     GE = 309,
     NAME = 310,
     DURATION_VAR = 311,
     VARIABLE = 312,
     NUMBER = 313,
     ILLEGAL_TOKEN = 314
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
#define OBJECT_TOKEN 298
#define EITHER 299
#define AT 300
#define OVER 301
#define START 302
#define END 303
#define ALL 304
#define MINIMIZE 305
#define MAXIMIZE 306
#define TOTAL_TIME 307
#define LE 308
#define GE 309
#define NAME 310
#define DURATION_VAR 311
#define VARIABLE 312
#define NUMBER 313
#define ILLEGAL_TOKEN 314




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 204 "pddl.yy"

  const Formula* formula;
  const Atom* atom;
  const std::string* str;
  std::vector<std::string>* strs;
  const Term* term;
  const Type* type;
  UnionType* types;
  float num;



/* Line 293 of yacc.c  */
#line 407 "pddl.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 419 "pddl.cc"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
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
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   629

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  67
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  123
/* YYNRULES -- Number of rules.  */
#define YYNRULES  249
/* YYNRULES -- Number of states.  */
#define YYNSTATES  418

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   314

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      60,    61,    65,    63,     2,    64,     2,    66,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    62,     2,     2,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     8,    11,    14,    15,    25,
      26,    28,    31,    33,    35,    38,    40,    43,    47,    49,
      51,    54,    56,    58,    61,    63,    65,    68,    73,    75,
      78,    80,    82,    84,    86,    88,    90,    92,    94,    96,
      98,   100,   102,   104,   106,   107,   113,   114,   120,   125,
     127,   130,   131,   137,   138,   149,   150,   163,   166,   168,
     169,   171,   172,   176,   179,   183,   185,   186,   189,   191,
     196,   202,   208,   214,   215,   218,   220,   225,   226,   229,
     230,   237,   238,   245,   246,   253,   255,   260,   261,   270,
     271,   272,   280,   281,   284,   286,   291,   293,   298,   299,
     302,   304,   309,   310,   319,   320,   327,   328,   331,   332,
     339,   340,   347,   348,   362,   365,   367,   370,   372,   375,
     377,   378,   384,   389,   391,   394,   396,   401,   402,   408,
     409,   412,   414,   417,   418,   424,   430,   432,   434,   440,
     446,   452,   458,   460,   465,   467,   468,   470,   472,   478,
     483,   488,   489,   495,   496,   503,   504,   513,   514,   523,
     524,   527,   528,   531,   532,   538,   539,   542,   545,   547,
     549,   550,   552,   554,   555,   560,   561,   563,   565,   568,
     570,   571,   576,   577,   579,   581,   584,   587,   589,   591,
     596,   598,   600,   603,   606,   608,   610,   612,   614,   616,
     618,   620,   622,   624,   626,   628,   630,   632,   634,   636,
     638,   640,   642,   644,   646,   648,   650,   652,   654,   656,
     658,   660,   662,   664,   666,   668,   670,   672,   674,   676,
     678,   680,   682,   684,   686,   688,   690,   692,   694,   696,
     698,   700,   702,   704,   706,   708,   710,   712,   714,   716
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      68,     0,    -1,    -1,    69,    70,    -1,    -1,    70,    71,
      -1,    70,   124,    -1,    -1,    60,   167,    60,   168,   188,
      61,    72,    73,    61,    -1,    -1,    79,    -1,    79,    74,
      -1,    74,    -1,    82,    -1,    82,    75,    -1,    75,    -1,
      86,    84,    -1,    86,    84,    78,    -1,    76,    -1,    84,
      -1,    84,    77,    -1,    77,    -1,    86,    -1,    86,    78,
      -1,    78,    -1,    90,    -1,    78,    90,    -1,    60,     6,
      80,    61,    -1,    81,    -1,    80,    81,    -1,     7,    -1,
       8,    -1,     9,    -1,    10,    -1,    11,    -1,    12,    -1,
      13,    -1,    14,    -1,    15,    -1,    16,    -1,    17,    -1,
      18,    -1,    19,    -1,    20,    -1,    -1,    60,    21,    83,
     155,    61,    -1,    -1,    60,    22,    85,   155,    61,    -1,
      60,    23,    87,    61,    -1,    88,    -1,    87,    88,    -1,
      -1,    60,   186,    89,   158,    61,    -1,    -1,    60,    24,
     188,    91,    27,    60,   158,    61,    93,    61,    -1,    -1,
      60,    25,   188,    92,    27,    60,   158,    61,    26,   100,
      98,    61,    -1,    95,    94,    -1,    94,    -1,    -1,    97,
      -1,    -1,    28,    96,   144,    -1,    30,   109,    -1,    29,
     103,    99,    -1,    99,    -1,    -1,    30,   117,    -1,   101,
      -1,    60,   172,   102,    61,    -1,    60,    53,   184,    58,
      61,    -1,    60,    54,   184,    58,    61,    -1,    60,    62,
     184,    58,    61,    -1,    -1,   102,   101,    -1,   105,    -1,
      60,   172,   104,    61,    -1,    -1,   104,   105,    -1,    -1,
      60,   179,   181,   106,   144,    61,    -1,    -1,    60,   179,
     182,   107,   144,    61,    -1,    -1,    60,   180,   183,   108,
     144,    61,    -1,   115,    -1,    60,   172,   113,    61,    -1,
      -1,    60,   176,   110,    60,   158,    61,   109,    61,    -1,
      -1,    -1,    60,   170,   111,   144,   112,   114,    61,    -1,
      -1,   113,   109,    -1,   115,    -1,    60,   172,   116,    61,
      -1,   151,    -1,    60,   171,   151,    61,    -1,    -1,   116,
     115,    -1,   121,    -1,    60,   172,   120,    61,    -1,    -1,
      60,   176,   118,    60,   158,    61,   117,    61,    -1,    -1,
      60,   170,   103,   119,   121,    61,    -1,    -1,   120,   117,
      -1,    -1,    60,   179,   181,   122,   114,    61,    -1,    -1,
      60,   179,   182,   123,   114,    61,    -1,    -1,    60,   167,
      60,   169,   188,    61,    60,    31,   188,    61,   125,   126,
      61,    -1,    79,   127,    -1,   127,    -1,   129,   128,    -1,
     128,    -1,   131,   137,    -1,   137,    -1,    -1,    60,    32,
     130,   155,    61,    -1,    60,    33,   132,    61,    -1,   133,
      -1,   132,   133,    -1,   134,    -1,    60,   171,   134,    61,
      -1,    -1,    60,   186,   135,   136,    61,    -1,    -1,   136,
     188,    -1,   138,    -1,   138,   140,    -1,    -1,    60,    34,
     139,   144,    61,    -1,    60,    35,   141,   142,    61,    -1,
      50,    -1,    51,    -1,    60,    63,   142,   142,    61,    -1,
      60,    64,   142,   143,    61,    -1,    60,    65,   142,   142,
      61,    -1,    60,    66,   142,   142,    61,    -1,    58,    -1,
      60,   187,   136,    61,    -1,   187,    -1,    -1,   142,    -1,
     151,    -1,    60,    62,   154,   154,    61,    -1,    60,   171,
     144,    61,    -1,    60,   172,   149,    61,    -1,    -1,    60,
     173,   145,   150,    61,    -1,    -1,    60,   174,   146,   144,
     144,    61,    -1,    -1,    60,   175,   147,    60,   158,    61,
     144,    61,    -1,    -1,    60,   176,   148,    60,   158,    61,
     144,    61,    -1,    -1,   149,   144,    -1,    -1,   150,   144,
      -1,    -1,    60,   186,   152,   153,    61,    -1,    -1,   153,
     188,    -1,   153,   189,    -1,   188,    -1,   189,    -1,    -1,
     160,    -1,   159,    -1,    -1,   159,   164,   157,   158,    -1,
      -1,   156,    -1,   189,    -1,   159,   189,    -1,   163,    -1,
      -1,   163,   164,   161,   162,    -1,    -1,   160,    -1,   188,
      -1,   163,   188,    -1,    64,   165,    -1,   177,    -1,   185,
      -1,    60,   178,   166,    61,    -1,   177,    -1,   185,    -1,
     166,   177,    -1,   166,   185,    -1,     3,    -1,     4,    -1,
       5,    -1,    36,    -1,    37,    -1,    38,    -1,    39,    -1,
      40,    -1,    41,    -1,    42,    -1,    43,    -1,    44,    -1,
      45,    -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,
      56,    -1,     3,    -1,     4,    -1,     5,    -1,    44,    -1,
      45,    -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,
      50,    -1,    51,    -1,    52,    -1,    55,    -1,   185,    -1,
      43,    -1,   188,    -1,     3,    -1,     4,    -1,     5,    -1,
      43,    -1,    44,    -1,    36,    -1,    37,    -1,    38,    -1,
      39,    -1,    40,    -1,    41,    -1,    42,    -1,    45,    -1,
      46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,
      51,    -1,    52,    -1,    55,    -1,    57,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   233,   233,   233,   237,   238,   239,   246,   246,   250,
     251,   252,   253,   256,   257,   258,   261,   262,   263,   266,
     267,   268,   271,   272,   273,   276,   277,   280,   283,   284,
     287,   288,   289,   291,   293,   294,   296,   298,   300,   301,
     302,   303,   304,   306,   310,   310,   314,   314,   318,   325,
     326,   329,   329,   337,   337,   340,   340,   345,   346,   349,
     350,   353,   353,   357,   360,   361,   364,   365,   372,   373,
     377,   382,   387,   391,   392,   400,   401,   404,   405,   408,
     408,   410,   410,   412,   412,   420,   421,   422,   422,   424,
     425,   424,   429,   430,   433,   434,   437,   438,   441,   442,
     445,   446,   447,   447,   449,   449,   453,   454,   457,   457,
     459,   459,   468,   467,   472,   473,   476,   477,   480,   481,
     484,   484,   487,   490,   491,   494,   495,   498,   498,   502,
     503,   506,   507,   510,   510,   514,   517,   518,   521,   522,
     523,   524,   525,   526,   527,   530,   531,   538,   539,   540,
     541,   542,   542,   543,   543,   545,   545,   547,   547,   551,
     552,   555,   556,   559,   559,   567,   568,   569,   572,   573,
     576,   577,   580,   581,   581,   584,   585,   588,   590,   594,
     595,   595,   598,   599,   602,   603,   606,   609,   610,   611,
     614,   615,   616,   617,   620,   623,   626,   629,   632,   635,
     638,   641,   644,   647,   650,   653,   656,   659,   662,   665,
     668,   671,   674,   674,   674,   675,   676,   676,   676,   676,
     676,   677,   677,   677,   678,   681,   682,   685,   688,   688,
     688,   689,   689,   690,   690,   690,   690,   690,   690,   690,
     691,   691,   691,   691,   691,   692,   692,   692,   693,   696
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
  "FORALL", "OBJECT_TOKEN", "EITHER", "AT", "OVER", "START", "END", "ALL",
  "MINIMIZE", "MAXIMIZE", "TOTAL_TIME", "LE", "GE", "NAME", "DURATION_VAR",
  "VARIABLE", "NUMBER", "ILLEGAL_TOKEN", "'('", "')'", "'='", "'+'", "'-'",
  "'*'", "'/'", "$accept", "pddl_file", "$@1", "domains_and_problems",
  "domain_def", "$@2", "domain_body", "domain_body2", "domain_body3",
  "domain_body4", "domain_body5", "actions_def", "require_def",
  "require_keys", "require_key", "types_def", "$@3", "constants_def",
  "$@4", "predicates_def", "atomic_formula_skeletons",
  "atomic_formula_skeleton", "$@5", "action_def", "$@6", "$@7",
  "action_body", "action_body2", "precondition", "$@8", "effect",
  "da_body", "da_body2", "duration_constraint",
  "simple_duration_constraint", "simple_duration_constraints", "da_gd",
  "timed_gds", "timed_gd", "$@9", "$@10", "$@11", "eff_formula", "$@12",
  "$@13", "$@14", "eff_formulas", "one_eff_formula", "term_literal",
  "term_literals", "da_effect", "$@15", "$@16", "da_effects",
  "timed_effect", "$@17", "$@18", "problem_def", "$@19", "problem_body",
  "problem_body2", "problem_body3", "object_decl", "$@20", "init",
  "name_literals", "name_literal", "atomic_name_formula", "$@21", "names",
  "goal_spec", "goal", "$@22", "metric_spec", "optimization",
  "ground_f_exp", "ground_f_exp_opt", "formula", "$@23", "$@24", "$@25",
  "$@26", "conjuncts", "disjuncts", "atomic_term_formula", "$@27", "terms",
  "term", "name_map", "variables", "$@28", "opt_variables", "variable_seq",
  "typed_names", "$@29", "opt_typed_names", "name_seq", "type_spec",
  "type", "types", "define", "domain", "problem", "when", "not", "and",
  "or", "imply", "exists", "forall", "object", "either", "at", "over",
  "start", "end", "all", "duration_var", "type_name", "predicate",
  "function_symbol", "name", "variable", 0
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
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
      40,    41,    61,    43,    45,    42,    47
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    67,    69,    68,    70,    70,    70,    72,    71,    73,
      73,    73,    73,    74,    74,    74,    75,    75,    75,    76,
      76,    76,    77,    77,    77,    78,    78,    79,    80,    80,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    83,    82,    85,    84,    86,    87,
      87,    89,    88,    91,    90,    92,    90,    93,    93,    94,
      94,    96,    95,    97,    98,    98,    99,    99,   100,   100,
     101,   101,   101,   102,   102,   103,   103,   104,   104,   106,
     105,   107,   105,   108,   105,   109,   109,   110,   109,   111,
     112,   109,   113,   113,   114,   114,   115,   115,   116,   116,
     117,   117,   118,   117,   119,   117,   120,   120,   122,   121,
     123,   121,   125,   124,   126,   126,   127,   127,   128,   128,
     130,   129,   131,   132,   132,   133,   133,   135,   134,   136,
     136,   137,   137,   139,   138,   140,   141,   141,   142,   142,
     142,   142,   142,   142,   142,   143,   143,   144,   144,   144,
     144,   145,   144,   146,   144,   147,   144,   148,   144,   149,
     149,   150,   150,   152,   151,   153,   153,   153,   154,   154,
     155,   155,   156,   157,   156,   158,   158,   159,   159,   160,
     161,   160,   162,   162,   163,   163,   164,   165,   165,   165,
     166,   166,   166,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   186,   186,   187,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   189
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     0,     2,     2,     0,     9,     0,
       1,     2,     1,     1,     2,     1,     2,     3,     1,     1,
       2,     1,     1,     2,     1,     1,     2,     4,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     5,     0,     5,     4,     1,
       2,     0,     5,     0,    10,     0,    12,     2,     1,     0,
       1,     0,     3,     2,     3,     1,     0,     2,     1,     4,
       5,     5,     5,     0,     2,     1,     4,     0,     2,     0,
       6,     0,     6,     0,     6,     1,     4,     0,     8,     0,
       0,     7,     0,     2,     1,     4,     1,     4,     0,     2,
       1,     4,     0,     8,     0,     6,     0,     2,     0,     6,
       0,     6,     0,    13,     2,     1,     2,     1,     2,     1,
       0,     5,     4,     1,     2,     1,     4,     0,     5,     0,
       2,     1,     2,     0,     5,     5,     1,     1,     5,     5,
       5,     5,     1,     4,     1,     0,     1,     1,     5,     4,
       4,     0,     5,     0,     6,     0,     8,     0,     8,     0,
       2,     0,     2,     0,     5,     0,     2,     2,     1,     1,
       0,     1,     1,     0,     4,     0,     1,     1,     2,     1,
       0,     4,     0,     1,     1,     2,     2,     1,     1,     4,
       1,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     4,     1,     3,     0,     5,     6,   194,     0,
       0,   195,   196,     0,     0,   228,   229,   230,   233,   234,
     235,   236,   237,   238,   239,   231,   232,   240,   241,   242,
     243,   244,   245,   246,   247,   248,     0,     0,     7,     0,
       9,     0,     0,     0,    12,    15,    18,    21,    24,    10,
      13,    19,    22,    25,     0,     0,    44,    46,     0,     0,
       0,     8,     0,    26,     0,    11,     0,    14,     0,    20,
      22,     0,    23,    16,     0,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,     0,
      28,   170,   170,     0,     0,    49,    53,    55,    17,   112,
      27,    29,     0,   171,   179,   184,     0,   212,   213,   214,
     226,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,    51,    48,    50,     0,     0,     0,    45,     0,
     180,   185,    47,   175,     0,     0,     0,     0,     0,   115,
     117,     0,     0,   119,   131,   204,     0,   186,   187,   188,
     182,   249,   176,     0,   172,   177,   175,   175,   120,     0,
     133,     0,   114,   113,     0,   116,     0,   118,     0,   132,
     205,     0,   183,   181,    52,   173,   178,     0,     0,   170,
       0,     0,   123,   125,     0,     0,     0,   190,   191,   175,
      59,     0,     0,   198,     0,   127,   122,   124,     0,     0,
     147,   136,   137,     0,   189,   192,   193,   174,    61,     0,
       0,    58,    59,    60,     0,   121,     0,     0,   129,   199,
     200,   201,   202,   203,     0,     0,   159,   151,   153,   155,
     157,   163,   134,   142,     0,     0,   144,   227,     0,     0,
      63,    85,    96,    54,    57,     0,    66,    68,   126,     0,
       0,   168,   169,     0,     0,   161,     0,     0,     0,   165,
       0,     0,     0,     0,   129,   135,    62,   197,    89,     0,
      92,    87,     0,     0,     0,    73,     0,     0,     0,    65,
     128,   130,     0,   149,   150,   160,     0,     0,   175,   175,
       0,     0,   145,     0,     0,     0,     0,     0,     0,     0,
       0,   211,     0,     0,     0,     0,     0,    66,    75,     0,
      67,   100,    56,   148,   152,   162,     0,     0,     0,   164,
     166,   167,     0,   146,     0,     0,     0,   143,    90,    97,
      86,    93,   175,     0,     0,     0,     0,    69,    74,   206,
     207,    77,     0,     0,    64,     0,   106,   102,     0,   154,
       0,     0,   138,   139,   140,   141,     0,     0,    70,    71,
      72,     0,   208,   209,    79,    81,   210,    83,   104,     0,
       0,   108,   110,     0,     0,     0,     0,    94,     0,     0,
      76,    78,     0,     0,     0,     0,   101,   107,   175,     0,
       0,   156,   158,    98,    91,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    88,    80,    82,    84,   105,
       0,   109,   111,     0,    95,    99,     0,   103
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     4,     6,    40,    43,    44,    45,    46,
      47,    48,    49,    89,    90,    50,    91,    51,    92,    52,
      94,    95,   133,    53,   125,   126,   210,   211,   212,   238,
     213,   278,   279,   246,   247,   305,   307,   361,   308,   382,
     383,   384,   240,   300,   296,   356,   299,   376,   241,   404,
     310,   370,   385,   369,   311,   389,   390,     7,   127,   138,
     139,   140,   141,   179,   142,   181,   182,   183,   218,   249,
     143,   144,   184,   169,   203,   235,   324,   199,   255,   256,
     257,   258,   254,   286,   200,   259,   290,   250,   102,   152,
     189,   153,   154,   103,   150,   173,   104,   130,   147,   186,
       9,    13,    14,   268,   269,   226,   227,   228,   229,   230,
     148,   171,   342,   343,   364,   365,   367,   302,   121,   231,
     236,   237,   155
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -354
static const yytype_int16 yypact[] =
{
    -354,    38,  -354,  -354,    -6,    56,  -354,  -354,  -354,    15,
      45,  -354,  -354,   465,   465,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,    26,    31,  -354,    40,
      47,    41,    80,    34,  -354,  -354,  -354,  -354,    84,   101,
     105,   107,   138,  -354,   465,   246,  -354,  -354,   144,   465,
     465,  -354,    94,  -354,   135,  -354,   146,  -354,   116,  -354,
      84,   150,    84,    84,    86,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,    12,
    -354,   465,   465,   551,    67,  -354,  -354,  -354,    84,  -354,
    -354,  -354,   149,  -354,   247,  -354,   174,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,  -354,   222,   254,   178,  -354,   145,
    -354,  -354,  -354,   243,   241,   244,    11,   248,   242,  -354,
    -354,   249,   250,  -354,   268,  -354,   285,  -354,  -354,  -354,
     465,  -354,  -354,   273,    -2,  -354,   243,   243,  -354,   276,
    -354,   169,  -354,  -354,   143,  -354,   301,  -354,   320,  -354,
    -354,   574,  -354,  -354,  -354,  -354,  -354,   295,   297,   465,
     538,   122,  -354,  -354,   321,   136,    65,  -354,  -354,   243,
      53,   356,   323,  -354,   325,  -354,  -354,  -354,   275,   329,
    -354,  -354,  -354,   383,  -354,  -354,  -354,  -354,  -354,   331,
     349,  -354,   362,  -354,   351,  -354,   551,   355,  -354,  -354,
    -354,  -354,  -354,  -354,   410,   321,  -354,  -354,  -354,  -354,
    -354,  -354,  -354,  -354,   176,   375,  -354,  -354,   321,   485,
    -354,  -354,  -354,  -354,  -354,    -1,   177,  -354,  -354,   328,
     410,  -354,  -354,   376,   148,  -354,   321,   379,   380,  -354,
     383,   383,   383,   383,  -354,  -354,  -354,  -354,  -354,   382,
    -354,  -354,   361,   361,   361,  -354,   384,   385,   402,  -354,
    -354,  -354,   403,  -354,  -354,  -354,   172,   321,   243,   243,
     302,   383,   383,   383,   383,   357,   321,   551,   405,   183,
     411,  -354,   414,   415,   416,   185,    87,   445,  -354,   100,
    -354,  -354,  -354,  -354,  -354,  -354,   417,   418,   419,  -354,
    -354,  -354,   420,  -354,   421,   422,   423,  -354,  -354,  -354,
    -354,  -354,   243,   424,   425,   426,    35,  -354,  -354,  -354,
    -354,  -354,   182,   427,  -354,   384,  -354,  -354,   182,  -354,
     321,   321,  -354,  -354,  -354,  -354,   431,   432,  -354,  -354,
    -354,   187,  -354,  -354,  -354,  -354,  -354,  -354,  -354,   207,
     434,  -354,  -354,   435,   436,   521,   437,  -354,   331,   224,
    -354,  -354,   321,   321,   321,   439,  -354,  -354,   243,   431,
     431,  -354,  -354,  -354,  -354,   457,   458,   477,   478,   447,
     483,   484,   486,   487,   211,  -354,  -354,  -354,  -354,  -354,
     385,  -354,  -354,   538,  -354,  -354,   488,  -354
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -354,  -354,  -354,  -354,  -354,  -354,  -354,   428,   450,  -354,
     444,   -10,   430,  -354,   461,  -354,  -354,   494,  -354,   500,
    -354,   459,  -354,   -33,  -354,  -354,  -354,   340,  -354,  -354,
    -354,  -354,   253,  -354,   256,  -354,   217,  -354,   202,  -354,
    -354,  -354,  -288,  -354,  -354,  -354,  -354,  -116,  -305,  -354,
    -353,  -354,  -354,  -354,   189,  -354,  -354,  -354,  -354,  -354,
     443,   451,  -354,  -354,  -354,  -354,   429,   397,  -354,   341,
     462,  -354,  -354,  -354,  -354,  -139,  -354,  -220,  -354,  -354,
    -354,  -354,  -354,  -354,  -205,  -354,  -354,   358,   -83,  -354,
    -354,  -154,  -354,   463,  -354,  -354,  -354,   453,  -354,  -354,
    -354,  -354,  -354,   300,  -140,  -232,  -354,  -354,  -354,  -227,
    -138,  -354,  -301,  -354,   263,   264,  -354,     2,  -115,   -87,
     381,   -13,  -144
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      36,    37,   177,   178,   242,   253,   122,   270,   348,   106,
     176,   331,   271,   275,   149,    63,   387,    55,   266,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,   187,   285,   207,   287,   219,     3,    63,
     194,    74,    72,   158,   159,   160,    96,    97,   205,    11,
      12,   377,   272,   273,     5,   151,   188,   416,   225,     8,
      72,   274,   129,    98,   298,    63,   315,   316,   107,   108,
     109,   206,    54,   100,   341,    10,   328,   346,   105,   105,
     252,   208,   347,   209,   377,   377,    55,    38,   272,   273,
     395,   131,    39,   195,   242,    61,   192,   274,   348,   415,
      41,    56,    57,    58,    59,    60,   252,    42,   145,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    59,    60,
     120,   291,   292,   293,   294,   219,   204,    93,   123,   195,
     373,   374,   339,   340,   317,   318,   267,   105,   219,    58,
      59,    60,   223,   393,    62,   339,   321,    99,   107,   108,
     109,   242,   322,   323,   325,   326,    56,    57,    58,    59,
      60,    64,   396,   397,   398,    66,   105,    68,    57,    58,
      59,    60,    57,   242,    59,    60,   159,   160,   357,    15,
      16,    17,   180,   196,   242,   242,   201,   202,   145,   111,
     112,   113,   114,   115,   116,   117,   118,   119,    71,   242,
     120,   158,   159,   160,    93,   146,   276,   277,   198,   284,
     128,   251,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,   362,
     363,    35,   198,   314,   401,   132,   281,   251,   136,   260,
     261,   262,   263,   239,   330,   336,   337,   379,   380,   134,
      15,    16,    17,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,   309,   386,   339,
     340,   413,   414,   402,   403,   303,   304,   320,   107,   108,
     109,   135,   281,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
     151,   156,    35,   163,   157,    15,    16,    17,   161,   164,
     166,   129,   193,   219,   220,   221,   222,   223,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   168,   170,
     120,    15,    16,    17,   174,   160,   180,   224,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,   185,   190,    35,   191,   151,
      15,    16,    17,   319,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,   198,   214,    35,   215,   216,    15,    16,    17,   280,
     232,   239,   209,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
     243,   245,    35,    15,    16,    17,   248,   301,   327,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,   265,   283,    35,   288,
     289,   233,   297,   234,   306,   309,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,   312,   313,    35,   329,   151,    15,    16,
      17,   332,   333,   334,   335,   277,   366,    65,   349,   350,
     351,   352,   353,   354,   355,   358,   359,   360,   107,   108,
     109,   375,   339,   378,   388,    69,   391,   392,   394,   399,
      67,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,   405,   406,
      35,   267,   193,   219,   107,   108,   109,   223,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   407,   408,
     120,   107,   108,   109,   409,   410,    73,   411,   412,   417,
     101,    70,   244,   124,   107,   108,   109,   137,   193,   219,
     344,   338,   368,   381,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   400,   193,   120,   107,   108,   109,
     162,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   217,   165,   120,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   167,   295,   120,   175,   282,   345,
     197,   371,   372,   172,     0,   264,     0,   145,   111,   112,
     113,   114,   115,   116,   117,   118,   119,     0,     0,   120
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-354))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      13,    14,   156,   157,   209,   225,    93,   239,   309,    92,
     154,   299,   239,   245,   129,    48,   369,     6,   238,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,   171,   254,   189,   256,    38,     0,    72,
     180,    54,    52,    32,    33,    34,    59,    60,   186,     4,
       5,   356,    53,    54,    60,    57,   171,   410,   198,     3,
      70,    62,    64,    73,   269,    98,   286,   287,     3,     4,
       5,   186,    31,    61,   306,    60,   296,   309,    91,    92,
     224,    28,   309,    30,   389,   390,     6,    61,    53,    54,
     378,   104,    61,   180,   299,    61,   179,    62,   399,   404,
      60,    21,    22,    23,    24,    25,   250,    60,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    24,    25,
      55,   260,   261,   262,   263,    38,    61,    60,    61,   216,
     350,   351,    45,    46,   288,   289,    36,   150,    38,    23,
      24,    25,    42,   375,    60,    45,   290,    61,     3,     4,
       5,   356,   291,   292,   293,   294,    21,    22,    23,    24,
      25,    60,   382,   383,   384,    60,   179,    60,    22,    23,
      24,    25,    22,   378,    24,    25,    33,    34,   332,     3,
       4,     5,    60,    61,   389,   390,    50,    51,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    60,   404,
      55,    32,    33,    34,    60,    60,    29,    30,    60,    61,
      61,   224,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    47,
      48,    55,    60,    61,   388,    61,   249,   250,    60,    63,
      64,    65,    66,    60,    61,    60,    61,    60,    61,    27,
       3,     4,     5,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    60,    61,    45,
      46,    60,    61,   389,   390,   273,   274,   290,     3,     4,
       5,    27,   295,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      57,    60,    55,    61,    60,     3,     4,     5,    60,    60,
      60,    64,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    60,    44,
      55,     3,     4,     5,    61,    34,    60,    62,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    35,    61,    55,    61,    57,
       3,     4,     5,    61,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    60,    26,    55,    61,    60,     3,     4,     5,    61,
      61,    60,    30,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      61,    60,    55,     3,     4,     5,    61,    56,    61,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    61,    61,    55,    60,
      60,    58,    60,    60,    60,    60,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    61,    61,    55,    61,    57,     3,     4,
       5,    60,    58,    58,    58,    30,    49,    49,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,     3,     4,
       5,    60,    45,    61,    60,    51,    61,    61,    61,    60,
      50,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    61,    61,
      55,    36,    37,    38,     3,     4,     5,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    61,    61,
      55,     3,     4,     5,    61,    61,    52,    61,    61,    61,
      89,    51,   212,    94,     3,     4,     5,   127,    37,    38,
     307,   305,   345,   361,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,   385,    37,    55,     3,     4,     5,
     137,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,   194,   141,    55,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,   142,   264,    55,   154,   250,   309,
     181,   348,   348,   150,    -1,   234,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    -1,    -1,    55
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    68,    69,     0,    70,    60,    71,   124,     3,   167,
      60,     4,     5,   168,   169,     3,     4,     5,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    55,   188,   188,    61,    61,
      72,    60,    60,    73,    74,    75,    76,    77,    78,    79,
      82,    84,    86,    90,    31,     6,    21,    22,    23,    24,
      25,    61,    60,    90,    60,    74,    60,    75,    60,    77,
      86,    60,    78,    84,   188,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    80,
      81,    83,    85,    60,    87,    88,   188,   188,    78,    61,
      61,    81,   155,   160,   163,   188,   155,     3,     4,     5,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      55,   185,   186,    61,    88,    91,    92,   125,    61,    64,
     164,   188,    61,    89,    27,    27,    60,    79,   126,   127,
     128,   129,   131,   137,   138,    43,    60,   165,   177,   185,
     161,    57,   156,   158,   159,   189,    60,    60,    32,    33,
      34,    60,   127,    61,    60,   128,    60,   137,    60,   140,
      44,   178,   160,   162,    61,   164,   189,   158,   158,   130,
      60,   132,   133,   134,   139,    35,   166,   177,   185,   157,
      61,    61,   155,    37,   171,   186,    61,   133,    60,   144,
     151,    50,    51,   141,    61,   177,   185,   158,    28,    30,
      93,    94,    95,    97,    26,    61,    60,   134,   135,    38,
      39,    40,    41,    42,    62,   171,   172,   173,   174,   175,
     176,   186,    61,    58,    60,   142,   187,   188,    96,    60,
     109,   115,   151,    61,    94,    60,   100,   101,    61,   136,
     154,   188,   189,   144,   149,   145,   146,   147,   148,   152,
      63,    64,    65,    66,   187,    61,   144,    36,   170,   171,
     172,   176,    53,    54,    62,   172,    29,    30,    98,    99,
      61,   188,   154,    61,    61,   144,   150,   144,    60,    60,
     153,   142,   142,   142,   142,   136,   111,    60,   151,   113,
     110,    56,   184,   184,   184,   102,    60,   103,   105,    60,
     117,   121,    61,    61,    61,   144,   144,   158,   158,    61,
     188,   189,   142,   142,   143,   142,   142,    61,   144,    61,
      61,   109,    60,    58,    58,    58,    60,    61,   101,    45,
      46,   172,   179,   180,    99,   170,   172,   176,   179,    61,
      61,    61,    61,    61,    61,    61,   112,   158,    61,    61,
      61,   104,    47,    48,   181,   182,    49,   183,   103,   120,
     118,   181,   182,   144,   144,    60,   114,   115,    61,    60,
      61,   105,   106,   107,   108,   119,    61,   117,    60,   122,
     123,    61,    61,   172,    61,   109,   144,   144,   144,    60,
     121,   158,   114,   114,   116,    61,    61,    61,    61,    61,
      61,    61,    61,    60,    61,   115,   117,    61
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
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


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Discard the shifted token.  */
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

/* Line 1806 of yacc.c  */
#line 233 "pddl.yy"
    { success = true; line_number = 1; }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 234 "pddl.yy"
    { if (!success) YYERROR; }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 246 "pddl.yy"
    { make_domain((yyvsp[(5) - (6)].str)); }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 287 "pddl.yy"
    { requirements->strips = true; }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 288 "pddl.yy"
    { requirements->typing = true; }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 290 "pddl.yy"
    { requirements->negative_preconditions = true; }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 292 "pddl.yy"
    { requirements->disjunctive_preconditions = true; }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 293 "pddl.yy"
    { requirements->equality = true; }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 295 "pddl.yy"
    { requirements->existential_preconditions = true; }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 297 "pddl.yy"
    { requirements->universal_preconditions = true; }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 299 "pddl.yy"
    { requirements->quantified_preconditions(); }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 300 "pddl.yy"
    { requirements->conditional_effects = true; }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 301 "pddl.yy"
    { throw Exception("`:fluents' not supported"); }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 302 "pddl.yy"
    { requirements->adl(); }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 303 "pddl.yy"
    { requirements->durative_actions = true; }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 305 "pddl.yy"
    { requirements->duration_inequalities = true; }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 307 "pddl.yy"
    { throw Exception("`:continuous-effects' not supported"); }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 310 "pddl.yy"
    { require_typing(); }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 311 "pddl.yy"
    { name_map_kind = NOTHING; }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 314 "pddl.yy"
    { name_map_kind = CONSTANT_MAP; }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 315 "pddl.yy"
    { name_map_kind = NOTHING; }
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 329 "pddl.yy"
    { make_predicate((yyvsp[(2) - (2)].str)); }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 330 "pddl.yy"
    { add_predicate(); }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 337 "pddl.yy"
    { make_action((yyvsp[(3) - (3)].str), false); }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 339 "pddl.yy"
    { add_action(); }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 340 "pddl.yy"
    { make_action((yyvsp[(3) - (3)].str), true); }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 342 "pddl.yy"
    { add_action(); }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 353 "pddl.yy"
    { formula_time = Formula::OVER_ALL; }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 354 "pddl.yy"
    { action->set_precondition(*(yyvsp[(3) - (3)].formula)); }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 357 "pddl.yy"
    { add_effect(); }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 360 "pddl.yy"
    { action->set_precondition(*(yyvsp[(2) - (3)].formula)); }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 365 "pddl.yy"
    { add_effect(); }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 374 "pddl.yy"
    { require_duration_inequalities(); }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 378 "pddl.yy"
    {
				 require_duration_inequalities();
				 action->set_max_duration((yyvsp[(4) - (5)].num));
			       }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 383 "pddl.yy"
    {
				 require_duration_inequalities();
				 action->set_min_duration((yyvsp[(4) - (5)].num));
			       }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 388 "pddl.yy"
    { action->set_duration((yyvsp[(4) - (5)].num)); }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 401 "pddl.yy"
    { (yyval.formula) = (yyvsp[(3) - (4)].formula); }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 404 "pddl.yy"
    { (yyval.formula) = &Formula::TRUE; }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 405 "pddl.yy"
    { (yyval.formula) = &(*(yyvsp[(1) - (2)].formula) && *(yyvsp[(2) - (2)].formula)); }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 408 "pddl.yy"
    { formula_time = Formula::AT_START; }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 409 "pddl.yy"
    { (yyval.formula) = (yyvsp[(5) - (6)].formula); }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 410 "pddl.yy"
    { formula_time = Formula::AT_END; }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 411 "pddl.yy"
    { (yyval.formula) = (yyvsp[(5) - (6)].formula); }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 412 "pddl.yy"
    { formula_time = Formula::OVER_ALL; }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 413 "pddl.yy"
    { (yyval.formula) = (yyvsp[(5) - (6)].formula); }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 422 "pddl.yy"
    { prepare_forall_effect(); }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 423 "pddl.yy"
    { add_forall_effect(); }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 424 "pddl.yy"
    { formula_time = Formula::OVER_ALL; }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 425 "pddl.yy"
    { prepare_conditional_effect((yyvsp[(4) - (4)].formula)); }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 426 "pddl.yy"
    { add_conditional_effect(); }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 437 "pddl.yy"
    { add_positive(*(yyvsp[(1) - (1)].atom)); }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 438 "pddl.yy"
    { add_negative(*(yyvsp[(3) - (4)].atom)); }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 447 "pddl.yy"
    { prepare_forall_effect(); }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 448 "pddl.yy"
    { add_forall_effect(); }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 449 "pddl.yy"
    { prepare_conditional_effect((yyvsp[(3) - (3)].formula)); }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 450 "pddl.yy"
    { add_conditional_effect(); }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 457 "pddl.yy"
    { formula_time = Formula::AT_START; }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 458 "pddl.yy"
    { add_effect(); }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 459 "pddl.yy"
    { formula_time = Formula::AT_END; }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 460 "pddl.yy"
    { add_effect(); }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 468 "pddl.yy"
    { make_problem((yyvsp[(5) - (10)].str), (yyvsp[(9) - (10)].str)); }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 469 "pddl.yy"
    { delete requirements; }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 484 "pddl.yy"
    { name_map_kind = OBJECT_MAP; }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 494 "pddl.yy"
    { problem->add_init(*(yyvsp[(1) - (1)].atom)); }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 498 "pddl.yy"
    { prepare_atom((yyvsp[(2) - (2)].str)); }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 499 "pddl.yy"
    { (yyval.atom) = make_atom(); }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 503 "pddl.yy"
    { add_term((yyvsp[(2) - (2)].str)); }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 510 "pddl.yy"
    { formula_time = Formula::AT_START; }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 511 "pddl.yy"
    { problem->set_goal(*(yyvsp[(4) - (5)].formula)); }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 517 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 518 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 525 "pddl.yy"
    {}
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 526 "pddl.yy"
    { delete (yyvsp[(2) - (4)].str); }
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 527 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 538 "pddl.yy"
    { (yyval.formula) = (yyvsp[(1) - (1)].atom); }
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 539 "pddl.yy"
    { (yyval.formula) = make_equality(*(yyvsp[(3) - (5)].term), *(yyvsp[(4) - (5)].term)); }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 540 "pddl.yy"
    { (yyval.formula) = make_negation(*(yyvsp[(3) - (4)].formula)); }
    break;

  case 150:

/* Line 1806 of yacc.c  */
#line 541 "pddl.yy"
    { (yyval.formula) = (yyvsp[(3) - (4)].formula); }
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 542 "pddl.yy"
    { prepare_disjunction(); }
    break;

  case 152:

/* Line 1806 of yacc.c  */
#line 542 "pddl.yy"
    { (yyval.formula) = (yyvsp[(4) - (5)].formula); }
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 543 "pddl.yy"
    { prepare_disjunction(); }
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 544 "pddl.yy"
    { (yyval.formula) = &(!*(yyvsp[(4) - (6)].formula) || *(yyvsp[(5) - (6)].formula)); }
    break;

  case 155:

/* Line 1806 of yacc.c  */
#line 545 "pddl.yy"
    { prepare_exists(); }
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 546 "pddl.yy"
    { (yyval.formula) = make_exists(*(yyvsp[(7) - (8)].formula)); }
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 547 "pddl.yy"
    { prepare_forall(); }
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 548 "pddl.yy"
    { (yyval.formula) = make_forall(*(yyvsp[(7) - (8)].formula)); }
    break;

  case 159:

/* Line 1806 of yacc.c  */
#line 551 "pddl.yy"
    { (yyval.formula) = &Formula::TRUE; }
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 552 "pddl.yy"
    { (yyval.formula) = &(*(yyvsp[(1) - (2)].formula) && *(yyvsp[(2) - (2)].formula)); }
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 555 "pddl.yy"
    { (yyval.formula) = &Formula::FALSE; }
    break;

  case 162:

/* Line 1806 of yacc.c  */
#line 556 "pddl.yy"
    { (yyval.formula) = &(*(yyvsp[(1) - (2)].formula) || *(yyvsp[(2) - (2)].formula)); }
    break;

  case 163:

/* Line 1806 of yacc.c  */
#line 559 "pddl.yy"
    { prepare_atom((yyvsp[(2) - (2)].str)); }
    break;

  case 164:

/* Line 1806 of yacc.c  */
#line 560 "pddl.yy"
    { (yyval.atom) = make_atom(); }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 568 "pddl.yy"
    {add_term((yyvsp[(2) - (2)].str)); }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 569 "pddl.yy"
    {add_term((yyvsp[(2) - (2)].str)); }
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 572 "pddl.yy"
    { (yyval.term) = &make_term((yyvsp[(1) - (1)].str)); }
    break;

  case 169:

/* Line 1806 of yacc.c  */
#line 573 "pddl.yy"
    { (yyval.term) = &make_term((yyvsp[(1) - (1)].str)); }
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 580 "pddl.yy"
    { add_variables((yyvsp[(1) - (1)].strs), Type::OBJECT); }
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 581 "pddl.yy"
    { add_variables((yyvsp[(1) - (2)].strs), *(yyvsp[(2) - (2)].type)); }
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 589 "pddl.yy"
    { (yyval.strs) = new std::vector<std::string>(1, *(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); }
    break;

  case 178:

/* Line 1806 of yacc.c  */
#line 591 "pddl.yy"
    { (yyval.strs) = (yyvsp[(1) - (2)].strs); (yyval.strs)->push_back(*(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); }
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 594 "pddl.yy"
    { add_names((yyvsp[(1) - (1)].strs), Type::OBJECT); }
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 595 "pddl.yy"
    { add_names((yyvsp[(1) - (2)].strs), *(yyvsp[(2) - (2)].type)); }
    break;

  case 184:

/* Line 1806 of yacc.c  */
#line 602 "pddl.yy"
    { (yyval.strs) = new std::vector<std::string>(1, *(yyvsp[(1) - (1)].str)); delete (yyvsp[(1) - (1)].str); }
    break;

  case 185:

/* Line 1806 of yacc.c  */
#line 603 "pddl.yy"
    { (yyval.strs) = (yyvsp[(1) - (2)].strs); (yyval.strs)->push_back(*(yyvsp[(2) - (2)].str)); delete (yyvsp[(2) - (2)].str); }
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 606 "pddl.yy"
    { (yyval.type) = (yyvsp[(2) - (2)].type); }
    break;

  case 187:

/* Line 1806 of yacc.c  */
#line 609 "pddl.yy"
    { (yyval.type) = &Type::OBJECT; }
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 610 "pddl.yy"
    { (yyval.type) = &make_type((yyvsp[(1) - (1)].str)); }
    break;

  case 189:

/* Line 1806 of yacc.c  */
#line 611 "pddl.yy"
    { (yyval.type) = &UnionType::simplify(*(yyvsp[(3) - (4)].types)); }
    break;

  case 190:

/* Line 1806 of yacc.c  */
#line 614 "pddl.yy"
    { (yyval.types) = new UnionType(); }
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 615 "pddl.yy"
    { (yyval.types) = new UnionType(make_type((yyvsp[(1) - (1)].str))); }
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 616 "pddl.yy"
    { (yyval.types) = (yyvsp[(1) - (2)].types); }
    break;

  case 193:

/* Line 1806 of yacc.c  */
#line 617 "pddl.yy"
    { (yyval.types) = (yyvsp[(1) - (2)].types); (yyval.types)->add(make_type((yyvsp[(2) - (2)].str))); }
    break;

  case 194:

/* Line 1806 of yacc.c  */
#line 620 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 623 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 196:

/* Line 1806 of yacc.c  */
#line 626 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 629 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 198:

/* Line 1806 of yacc.c  */
#line 632 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 199:

/* Line 1806 of yacc.c  */
#line 635 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 200:

/* Line 1806 of yacc.c  */
#line 638 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 201:

/* Line 1806 of yacc.c  */
#line 641 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 202:

/* Line 1806 of yacc.c  */
#line 644 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 203:

/* Line 1806 of yacc.c  */
#line 647 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 204:

/* Line 1806 of yacc.c  */
#line 650 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 205:

/* Line 1806 of yacc.c  */
#line 653 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 206:

/* Line 1806 of yacc.c  */
#line 656 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 207:

/* Line 1806 of yacc.c  */
#line 659 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 208:

/* Line 1806 of yacc.c  */
#line 662 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 209:

/* Line 1806 of yacc.c  */
#line 665 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 668 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;

  case 211:

/* Line 1806 of yacc.c  */
#line 671 "pddl.yy"
    { delete (yyvsp[(1) - (1)].str); }
    break;



/* Line 1806 of yacc.c  */
#line 3028 "pddl.cc"
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
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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
      if (!yypact_value_is_default (yyn))
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
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



/* Line 2067 of yacc.c  */
#line 699 "pddl.yy"



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


/* Returns the type with the given name, or NULL if undefined. */
static const SimpleType* find_type(const std::string& name) {
  if (pdomain != NULL) {
    return pdomain->find_type(name);
  } else if (domain != NULL) {
    return domain->find_type(name);
  } else {
    return NULL;
  }
}


/* Returns the constant with the given name, or NULL if undefined.  */
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


/* Returns the predicate with the given name, or NULL if undefined. */
static const Predicate* find_predicate(const std::string& name) {
  if (pdomain != NULL) {
    return pdomain->find_predicate(name);
  } else if (domain != NULL) {
    return domain->find_predicate(name);
  } else {
    return NULL;
  }
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
static const SimpleType& make_type(const std::string* name) {
  const SimpleType* t = find_type(*name);
  if (t == NULL) {
    SimpleType* st = new SimpleType(*name, Type::OBJECT);
    if (domain != NULL) {
      domain->add_type(*st);
      if (name_map_kind != TYPE_MAP) {
	yywarning("implicit declaration of type `" + *name + "'");
      }
    } else {
      yyerror("undeclared type `" + *name + "' used");
    }
    t = st;
  }
  delete name;
  return *t;
}


/* Returns a simple term with the given name. */
static const Term& make_term(const std::string* name) {
  if ((*name)[0] == '?') {
    const Variable* var = context.find(*name);
    if (var == NULL) {
      var = new Variable(*name, Type::OBJECT);
      context.insert(var);
      yyerror("free variable `" + *name + "' used");
    }
    delete name;
    return *var;
  } else {
    const Name* c = find_constant(*name);
    if (c == NULL) {
      Name* new_c;
      size_t n = atom->terms().size();
      if (atom != NULL && atom->predicate().arity() > n) {
	new_c = new Name(*name, atom->predicate().type(n));
      } else {
	new_c = new Name(*name, Type::OBJECT);
      }
      if (domain != NULL) {
	domain->add_constant(*new_c);
	yywarning("implicit declaration of constant `" + *name + "'");
      } else {
	problem->add_object(*new_c);
	yywarning("implicit declaration of object `" + *name + "'");
      }
      c = new_c;
    }
    delete name;
    return *c;
  }
}


/* Creates a predicate with the given name. */
static void make_predicate(const std::string* name) {
  predicate = new Predicate(*name);
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


/* Prepares for the parsing of a universally quantified effect. */ 
static void prepare_forall_effect() {
  if (!requirements->conditional_effects) {
    yywarning("assuming `:conditional-effects' requirement");
    requirements->conditional_effects = true;
  }
  add_effect();
  context.push_frame();
  quantified.push_back(NULL);
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
  if (t1.type().subtype(t2.type()) || t2.type().subtype(t1.type())) {
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
  quantified.push_back(NULL);
}


/* Prepares for the parsing of a universally quantified formula. */
static void prepare_forall() {
  if (!requirements->universal_preconditions) {
    yywarning("assuming `:universal-preconditions' requirement");
    requirements->universal_preconditions = true;
  }
  context.push_frame();
  quantified.push_back(NULL);
}


/* Creates an existentially quantified formula. */
static const Formula* make_exists(const Formula& body) {
  context.pop_frame();
  size_t m = quantified.size() - 1;
  size_t n = m;
  while (quantified[n] != NULL) {
    n--;
  }
  if (n < m) {
    ExistsFormula& exists = *(new ExistsFormula());
    for (size_t i = n + 1; i <= m; i++) {
      exists.add_parameter(*quantified[i]);
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
  while (quantified[n] != NULL) {
    n--;
  }
  if (n < m) {
    ForallFormula& forall = *(new ForallFormula());
    for (size_t i = n + 1; i <= m; i++) {
      forall.add_parameter(*quantified[i]);
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
static void add_names(const std::vector<std::string>* names,
		      const Type& type) {
  const UnionType* ut = dynamic_cast<const UnionType*>(&type);
  for (std::vector<std::string>::const_iterator si = names->begin();
       si != names->end(); si++) {
    /* Duplicate type if it is a union type so that every name has its
       own copy. */
    const Type* t = (ut != NULL) ? new UnionType(*ut) : &type;
    const std::string& s = *si;
    if (name_map_kind == TYPE_MAP) {
      if (s != "object") {
	SimpleType* old_type = domain->find_type(s);
	if (old_type != NULL) {
	  if (!old_type->add_supertype(*t)) {
	    yyerror("cyclic type hierarchy");
	  }
	  if (ut != NULL) {
	    delete t;
	  }
	} else {
	  domain->add_type(*(new SimpleType(s, *t)));
	}
      } else {
	yywarning("ignoring declaration of reserved type `object'");
      }
    } else if (name_map_kind == CONSTANT_MAP) {
      Name* old_name = domain->find_constant(s);
      if (old_name != NULL) {
	old_name->add_type(*t);
	if (ut != NULL) {
	  delete t;
	}
      } else {
	domain->add_constant(*(new Name(s, *t)));
      }
    } else { /* name_map_kind == OBJECT_MAP */
      if (pdomain->find_constant(s) != NULL) {
	yywarning("ignoring declaration of object `" + s
		  + "' previously declared as constant");
      } else {
	Name* old_name = problem->find_object(s);
	if (old_name != NULL) {
	  old_name->add_type(*t);
	  if (ut != NULL) {
	    delete t;
	  }
	} else {
	  problem->add_object(*(new Name(s, *t)));
	}
      }
    }
  }
  if (ut != NULL) {
    delete ut;
  }
  delete names;
}


/* Adds variables to the current variable list. */
static void add_variables(const std::vector<std::string>* names,
			  const Type& type) {
  const UnionType* ut = dynamic_cast<const UnionType*>(&type);
  for (std::vector<std::string>::const_iterator si = names->begin();
       si != names->end(); si++) {
    /* Duplicate type if it is a union type so that every name has its
       own copy. */
    const Type* t = (ut != NULL) ? new UnionType(*ut) : &type;
    const std::string& s = *si;
    if (predicate != NULL) {
      predicate->add_parameter(*t);
    } else {
      if (context.shallow_find(s) != NULL) {
	yyerror("repetition of parameter `" + s + "'");
      } else if (context.find(s) != NULL) {
	yyerror("shadowing parameter `" + s + "'");
      }
      const Variable* var = new Variable(s, *t);
      context.insert(var);
      if (!quantified.empty()) {
	quantified.push_back(var);
      } else if (action != NULL) {
	action->add_parameter(*var);
      } else {
	yyerror("where do these variables go?");
      }
    }
  }
  if (ut != NULL) {
    delete ut;
  }
  delete names;
}


/* Adds the current predicate to the current domain. */
static void add_predicate() {
  if (find_predicate(predicate->name()) == NULL) {
    domain->add_predicate(*predicate);
  } else {
    yywarning("ignoring repeated declaration of predicate `"
	      + predicate->name() + "'");
    delete predicate;
  }
  predicate = NULL;
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
    for (VarListIter vi = quantified.begin(); vi != quantified.end(); vi++) {
      if (*vi != NULL) {
	effect->add_forall(**vi);
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
  while (quantified[n] != NULL) {
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
  const Predicate* p = find_predicate(*name);
  if (p == NULL) {
    p = predicate = new Predicate(*name);
    if (domain != NULL) {
      domain->add_predicate(*predicate);
      yywarning("implicit declaration of predicate `" + *name + "'");
    } else {
      yyerror("undeclared predicate `" + *name + "' used");
    }
  }
  atom = new Atom(*p, formula_time);
  delete name;
}


/* Adds a term with the given name to the current atomic formula. */
static void add_term(const std::string* name) {
  const Term& term = make_term(name);
  size_t n = atom->terms().size();
  if (atom->predicate().arity() > n
      && !term.type().subtype(atom->predicate().type(n))) {
    yyerror("type mismatch");
  }
  atom->add_term(term);
}


/* Creates the atomic formula just parsed. */
static const Atom* make_atom() {
  if (atom->predicate().arity() < atom->terms().size()) {
    yyerror("too many parameters passed to predicate `"
	    + atom->predicate().name() + "'");
  } else if (atom->predicate().arity() > atom->terms().size()) {
    yyerror("too few parameters passed to predicate `"
	    + atom->predicate().name() + "'");
  }
  const Atom* a = atom;
  atom = NULL;
  return a;
}

