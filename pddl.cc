/* A Bison parser, made from pddl.yy
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	DEFINE	257
# define	DOMAIN_TOKEN	258
# define	PROBLEM	259
# define	REQUIREMENTS	260
# define	STRIPS	261
# define	TYPING	262
# define	NEGATIVE_PRECONDITIONS	263
# define	DISJUNCTIVE_PRECONDITIONS	264
# define	EQUALITY	265
# define	EXISTENTIAL_PRECONDITIONS	266
# define	UNIVERSAL_PRECONDITIONS	267
# define	QUANTIFIED_PRECONDITIONS	268
# define	CONDITIONAL_EFFECTS	269
# define	FLUENTS	270
# define	ADL	271
# define	DURATIVE_ACTIONS	272
# define	DURATION_INEQUALITIES	273
# define	CONTINUOUS_EFFECTS	274
# define	TYPES	275
# define	CONSTANTS	276
# define	PREDICATES	277
# define	ACTION	278
# define	DURATIVE_ACTION	279
# define	DURATION	280
# define	PARAMETERS	281
# define	PRECONDITION	282
# define	CONDITION	283
# define	EFFECT	284
# define	PDOMAIN	285
# define	OBJECTS	286
# define	INIT	287
# define	GOAL	288
# define	METRIC	289
# define	WHEN	290
# define	NOT	291
# define	AND	292
# define	OR	293
# define	IMPLY	294
# define	EXISTS	295
# define	FORALL	296
# define	OBJECT_TOKEN	297
# define	EITHER	298
# define	AT	299
# define	OVER	300
# define	START	301
# define	END	302
# define	ALL	303
# define	MINIMIZE	304
# define	MAXIMIZE	305
# define	TOTAL_TIME	306
# define	LE	307
# define	GE	308
# define	NAME	309
# define	DURATION_VAR	310
# define	VARIABLE	311
# define	NUMBER	312
# define	ILLEGAL_TOKEN	313

#line 21 "pddl.yy"

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

#line 206 "pddl.yy"
#ifndef YYSTYPE
typedef union {
  const Formula* formula;
  const Atom* atom;
  const std::string* str;
  std::vector<std::string>* strs;
  const Term* term;
  const Type* type;
  UnionType* types;
  float num;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		418
#define	YYFLAG		-32768
#define	YYNTBASE	67

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 313 ? yytranslate[x] : 189)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     5,     8,    11,    12,    22,    23,
      25,    28,    30,    32,    35,    37,    40,    44,    46,    48,
      51,    53,    55,    58,    60,    62,    65,    70,    72,    75,
      77,    79,    81,    83,    85,    87,    89,    91,    93,    95,
      97,    99,   101,   103,   104,   110,   111,   117,   122,   124,
     127,   128,   134,   135,   146,   147,   160,   163,   165,   166,
     168,   169,   173,   176,   180,   182,   183,   186,   188,   193,
     199,   205,   211,   212,   215,   217,   222,   223,   226,   227,
     234,   235,   242,   243,   250,   252,   257,   258,   267,   268,
     269,   277,   278,   281,   283,   288,   290,   295,   296,   299,
     301,   306,   307,   316,   317,   324,   325,   328,   329,   336,
     337,   344,   345,   359,   362,   364,   367,   369,   372,   374,
     375,   381,   386,   388,   391,   393,   398,   399,   405,   406,
     409,   411,   414,   415,   421,   427,   429,   431,   437,   443,
     449,   455,   457,   462,   464,   465,   467,   469,   475,   480,
     485,   486,   492,   493,   500,   501,   510,   511,   520,   521,
     524,   525,   528,   529,   535,   536,   539,   542,   544,   546,
     547,   549,   551,   552,   557,   558,   560,   562,   565,   567,
     568,   573,   574,   576,   578,   581,   584,   586,   588,   593,
     595,   597,   600,   603,   605,   607,   609,   611,   613,   615,
     617,   619,   621,   623,   625,   627,   629,   631,   633,   635,
     637,   639,   641,   643,   645,   647,   649,   651,   653,   655,
     657,   659,   661,   663,   665,   667,   669,   671,   673,   675,
     677,   679,   681,   683,   685,   687,   689,   691,   693,   695,
     697,   699,   701,   703,   705,   707,   709,   711,   713
};
static const short yyrhs[] =
{
      -1,    68,    69,     0,     0,    69,    70,     0,    69,   123,
       0,     0,    60,   166,    60,   167,   187,    61,    71,    72,
      61,     0,     0,    78,     0,    78,    73,     0,    73,     0,
      81,     0,    81,    74,     0,    74,     0,    85,    83,     0,
      85,    83,    77,     0,    75,     0,    83,     0,    83,    76,
       0,    76,     0,    85,     0,    85,    77,     0,    77,     0,
      89,     0,    77,    89,     0,    60,     6,    79,    61,     0,
      80,     0,    79,    80,     0,     7,     0,     8,     0,     9,
       0,    10,     0,    11,     0,    12,     0,    13,     0,    14,
       0,    15,     0,    16,     0,    17,     0,    18,     0,    19,
       0,    20,     0,     0,    60,    21,    82,   154,    61,     0,
       0,    60,    22,    84,   154,    61,     0,    60,    23,    86,
      61,     0,    87,     0,    86,    87,     0,     0,    60,   185,
      88,   157,    61,     0,     0,    60,    24,   187,    90,    27,
      60,   157,    61,    92,    61,     0,     0,    60,    25,   187,
      91,    27,    60,   157,    61,    26,    99,    97,    61,     0,
      94,    93,     0,    93,     0,     0,    96,     0,     0,    28,
      95,   143,     0,    30,   108,     0,    29,   102,    98,     0,
      98,     0,     0,    30,   116,     0,   100,     0,    60,   171,
     101,    61,     0,    60,    53,   183,    58,    61,     0,    60,
      54,   183,    58,    61,     0,    60,    62,   183,    58,    61,
       0,     0,   101,   100,     0,   104,     0,    60,   171,   103,
      61,     0,     0,   103,   104,     0,     0,    60,   178,   180,
     105,   143,    61,     0,     0,    60,   178,   181,   106,   143,
      61,     0,     0,    60,   179,   182,   107,   143,    61,     0,
     114,     0,    60,   171,   112,    61,     0,     0,    60,   175,
     109,    60,   157,    61,   108,    61,     0,     0,     0,    60,
     169,   110,   143,   111,   113,    61,     0,     0,   112,   108,
       0,   114,     0,    60,   171,   115,    61,     0,   150,     0,
      60,   170,   150,    61,     0,     0,   115,   114,     0,   120,
       0,    60,   171,   119,    61,     0,     0,    60,   175,   117,
      60,   157,    61,   116,    61,     0,     0,    60,   169,   102,
     118,   120,    61,     0,     0,   119,   116,     0,     0,    60,
     178,   180,   121,   113,    61,     0,     0,    60,   178,   181,
     122,   113,    61,     0,     0,    60,   166,    60,   168,   187,
      61,    60,    31,   187,    61,   124,   125,    61,     0,    78,
     126,     0,   126,     0,   128,   127,     0,   127,     0,   130,
     136,     0,   136,     0,     0,    60,    32,   129,   154,    61,
       0,    60,    33,   131,    61,     0,   132,     0,   131,   132,
       0,   133,     0,    60,   170,   133,    61,     0,     0,    60,
     185,   134,   135,    61,     0,     0,   135,   187,     0,   137,
       0,   137,   139,     0,     0,    60,    34,   138,   143,    61,
       0,    60,    35,   140,   141,    61,     0,    50,     0,    51,
       0,    60,    63,   141,   141,    61,     0,    60,    64,   141,
     142,    61,     0,    60,    65,   141,   141,    61,     0,    60,
      66,   141,   141,    61,     0,    58,     0,    60,   186,   135,
      61,     0,   186,     0,     0,   141,     0,   150,     0,    60,
      62,   153,   153,    61,     0,    60,   170,   143,    61,     0,
      60,   171,   148,    61,     0,     0,    60,   172,   144,   149,
      61,     0,     0,    60,   173,   145,   143,   143,    61,     0,
       0,    60,   174,   146,    60,   157,    61,   143,    61,     0,
       0,    60,   175,   147,    60,   157,    61,   143,    61,     0,
       0,   148,   143,     0,     0,   149,   143,     0,     0,    60,
     185,   151,   152,    61,     0,     0,   152,   187,     0,   152,
     188,     0,   187,     0,   188,     0,     0,   159,     0,   158,
       0,     0,   158,   163,   156,   157,     0,     0,   155,     0,
     188,     0,   158,   188,     0,   162,     0,     0,   162,   163,
     160,   161,     0,     0,   159,     0,   187,     0,   162,   187,
       0,    64,   164,     0,   176,     0,   184,     0,    60,   177,
     165,    61,     0,   176,     0,   184,     0,   165,   176,     0,
     165,   184,     0,     3,     0,     4,     0,     5,     0,    36,
       0,    37,     0,    38,     0,    39,     0,    40,     0,    41,
       0,    42,     0,    43,     0,    44,     0,    45,     0,    46,
       0,    47,     0,    48,     0,    49,     0,    56,     0,     3,
       0,     4,     0,     5,     0,    44,     0,    45,     0,    46,
       0,    47,     0,    48,     0,    49,     0,    50,     0,    51,
       0,    52,     0,    55,     0,   184,     0,    43,     0,   187,
       0,     3,     0,     4,     0,     5,     0,    43,     0,    44,
       0,    36,     0,    37,     0,    38,     0,    39,     0,    40,
       0,    41,     0,    42,     0,    45,     0,    46,     0,    47,
       0,    48,     0,    49,     0,    50,     0,    51,     0,    52,
       0,    55,     0,    57,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   235,   235,   239,   240,   241,   248,   248,   252,   253,
     254,   255,   258,   259,   260,   263,   264,   265,   268,   269,
     270,   273,   274,   275,   278,   279,   282,   285,   286,   289,
     290,   291,   293,   295,   296,   298,   300,   302,   303,   304,
     305,   306,   308,   312,   312,   316,   316,   320,   327,   328,
     331,   331,   339,   339,   342,   342,   347,   348,   351,   352,
     355,   355,   359,   362,   363,   366,   367,   374,   375,   379,
     384,   389,   393,   394,   402,   403,   406,   407,   410,   410,
     412,   412,   414,   414,   422,   423,   424,   424,   426,   426,
     426,   431,   432,   435,   436,   439,   440,   443,   444,   447,
     448,   449,   449,   451,   451,   455,   456,   459,   459,   461,
     461,   469,   469,   474,   475,   478,   479,   482,   483,   486,
     486,   489,   492,   493,   496,   497,   500,   500,   504,   505,
     508,   509,   512,   512,   516,   519,   520,   523,   524,   525,
     526,   527,   528,   529,   532,   533,   540,   541,   542,   543,
     544,   544,   545,   545,   547,   547,   549,   549,   553,   554,
     557,   558,   561,   561,   569,   570,   571,   574,   575,   578,
     579,   582,   583,   583,   586,   587,   590,   592,   596,   597,
     597,   600,   601,   604,   605,   608,   611,   612,   613,   616,
     617,   618,   619,   622,   625,   628,   631,   634,   637,   640,
     643,   646,   649,   652,   655,   658,   661,   664,   667,   670,
     673,   676,   676,   676,   677,   678,   678,   678,   678,   678,
     679,   679,   679,   680,   683,   684,   687,   690,   690,   690,
     691,   691,   692,   692,   692,   692,   692,   692,   692,   693,
     693,   693,   693,   693,   694,   694,   694,   695,   698
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "DEFINE", "DOMAIN_TOKEN", "PROBLEM", 
  "REQUIREMENTS", "STRIPS", "TYPING", "NEGATIVE_PRECONDITIONS", 
  "DISJUNCTIVE_PRECONDITIONS", "EQUALITY", "EXISTENTIAL_PRECONDITIONS", 
  "UNIVERSAL_PRECONDITIONS", "QUANTIFIED_PRECONDITIONS", 
  "CONDITIONAL_EFFECTS", "FLUENTS", "ADL", "DURATIVE_ACTIONS", 
  "DURATION_INEQUALITIES", "CONTINUOUS_EFFECTS", "TYPES", "CONSTANTS", 
  "PREDICATES", "ACTION", "DURATIVE_ACTION", "DURATION", "PARAMETERS", 
  "PRECONDITION", "CONDITION", "EFFECT", "PDOMAIN", "OBJECTS", "INIT", 
  "GOAL", "METRIC", "WHEN", "NOT", "AND", "OR", "IMPLY", "EXISTS", 
  "FORALL", "OBJECT_TOKEN", "EITHER", "AT", "OVER", "START", "END", "ALL", 
  "MINIMIZE", "MAXIMIZE", "TOTAL_TIME", "LE", "GE", "NAME", 
  "DURATION_VAR", "VARIABLE", "NUMBER", "ILLEGAL_TOKEN", "'('", "')'", 
  "'='", "'+'", "'-'", "'*'", "'/'", "pddl_file", "@1", 
  "domains_and_problems", "domain_def", "@2", "domain_body", 
  "domain_body2", "domain_body3", "domain_body4", "domain_body5", 
  "actions_def", "require_def", "require_keys", "require_key", 
  "types_def", "@3", "constants_def", "@4", "predicates_def", 
  "atomic_formula_skeletons", "atomic_formula_skeleton", "@5", 
  "action_def", "@6", "@7", "action_body", "action_body2", "precondition", 
  "@8", "effect", "da_body", "da_body2", "duration_constraint", 
  "simple_duration_constraint", "simple_duration_constraints", "da_gd", 
  "timed_gds", "timed_gd", "@9", "@10", "@11", "eff_formula", "@12", 
  "@13", "@14", "eff_formulas", "one_eff_formula", "term_literal", 
  "term_literals", "da_effect", "@15", "@16", "da_effects", 
  "timed_effect", "@17", "@18", "problem_def", "@19", "problem_body", 
  "problem_body2", "problem_body3", "object_decl", "@20", "init", 
  "name_literals", "name_literal", "atomic_name_formula", "@21", "names", 
  "goal_spec", "goal", "@22", "metric_spec", "optimization", 
  "ground_f_exp", "ground_f_exp_opt", "formula", "@23", "@24", "@25", 
  "@26", "conjuncts", "disjuncts", "atomic_term_formula", "@27", "terms", 
  "term", "name_map", "variables", "@28", "opt_variables", "variable_seq", 
  "typed_names", "@29", "opt_typed_names", "name_seq", "type_spec", 
  "type", "types", "define", "domain", "problem", "when", "not", "and", 
  "or", "imply", "exists", "forall", "object", "either", "at", "over", 
  "start", "end", "all", "duration_var", "type_name", "predicate", 
  "function_symbol", "name", "variable", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    68,    67,    69,    69,    69,    71,    70,    72,    72,
      72,    72,    73,    73,    73,    74,    74,    74,    75,    75,
      75,    76,    76,    76,    77,    77,    78,    79,    79,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    82,    81,    84,    83,    85,    86,    86,
      88,    87,    90,    89,    91,    89,    92,    92,    93,    93,
      95,    94,    96,    97,    97,    98,    98,    99,    99,   100,
     100,   100,   101,   101,   102,   102,   103,   103,   105,   104,
     106,   104,   107,   104,   108,   108,   109,   108,   110,   111,
     108,   112,   112,   113,   113,   114,   114,   115,   115,   116,
     116,   117,   116,   118,   116,   119,   119,   121,   120,   122,
     120,   124,   123,   125,   125,   126,   126,   127,   127,   129,
     128,   130,   131,   131,   132,   132,   134,   133,   135,   135,
     136,   136,   138,   137,   139,   140,   140,   141,   141,   141,
     141,   141,   141,   141,   142,   142,   143,   143,   143,   143,
     144,   143,   145,   143,   146,   143,   147,   143,   148,   148,
     149,   149,   151,   150,   152,   152,   152,   153,   153,   154,
     154,   155,   156,   155,   157,   157,   158,   158,   159,   160,
     159,   161,   161,   162,   162,   163,   164,   164,   164,   165,
     165,   165,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   185,   185,   186,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   188
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     0,     2,     2,     0,     9,     0,     1,
       2,     1,     1,     2,     1,     2,     3,     1,     1,     2,
       1,     1,     2,     1,     1,     2,     4,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     5,     0,     5,     4,     1,     2,
       0,     5,     0,    10,     0,    12,     2,     1,     0,     1,
       0,     3,     2,     3,     1,     0,     2,     1,     4,     5,
       5,     5,     0,     2,     1,     4,     0,     2,     0,     6,
       0,     6,     0,     6,     1,     4,     0,     8,     0,     0,
       7,     0,     2,     1,     4,     1,     4,     0,     2,     1,
       4,     0,     8,     0,     6,     0,     2,     0,     6,     0,
       6,     0,    13,     2,     1,     2,     1,     2,     1,     0,
       5,     4,     1,     2,     1,     4,     0,     5,     0,     2,
       1,     2,     0,     5,     5,     1,     1,     5,     5,     5,
       5,     1,     4,     1,     0,     1,     1,     5,     4,     4,
       0,     5,     0,     6,     0,     8,     0,     8,     0,     2,
       0,     2,     0,     5,     0,     2,     2,     1,     1,     0,
       1,     1,     0,     4,     0,     1,     1,     2,     1,     0,
       4,     0,     1,     1,     2,     2,     1,     1,     4,     1,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     3,     2,     0,     4,     5,   193,     0,     0,   194,
     195,     0,     0,   227,   228,   229,   232,   233,   234,   235,
     236,   237,   238,   230,   231,   239,   240,   241,   242,   243,
     244,   245,   246,   247,     0,     0,     6,     0,     8,     0,
       0,     0,    11,    14,    17,    20,    23,     9,    12,    18,
      21,    24,     0,     0,    43,    45,     0,     0,     0,     7,
       0,    25,     0,    10,     0,    13,     0,    19,    21,     0,
      22,    15,     0,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     0,    27,   169,
     169,     0,     0,    48,    52,    54,    16,   111,    26,    28,
       0,   170,   178,   183,     0,   211,   212,   213,   225,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
      50,    47,    49,     0,     0,     0,    44,     0,   179,   184,
      46,   174,     0,     0,     0,     0,     0,   114,   116,     0,
       0,   118,   130,   203,     0,   185,   186,   187,   181,   248,
     175,     0,   171,   176,   174,   174,   119,     0,   132,     0,
     113,   112,     0,   115,     0,   117,     0,   131,   204,     0,
     182,   180,    51,   172,   177,     0,     0,   169,     0,     0,
     122,   124,     0,     0,     0,   189,   190,   174,    58,     0,
       0,   197,     0,   126,   121,   123,     0,     0,   146,   135,
     136,     0,   188,   191,   192,   173,    60,     0,     0,    57,
      58,    59,     0,   120,     0,     0,   128,   198,   199,   200,
     201,   202,     0,     0,   158,   150,   152,   154,   156,   162,
     133,   141,     0,     0,   143,   226,     0,     0,    62,    84,
      95,    53,    56,     0,    65,    67,   125,     0,     0,   167,
     168,     0,     0,   160,     0,     0,     0,   164,     0,     0,
       0,     0,   128,   134,    61,   196,    88,     0,    91,    86,
       0,     0,     0,    72,     0,     0,     0,    64,   127,   129,
       0,   148,   149,   159,     0,     0,   174,   174,     0,     0,
     144,     0,     0,     0,     0,     0,     0,     0,     0,   210,
       0,     0,     0,     0,     0,    65,    74,     0,    66,    99,
      55,   147,   151,   161,     0,     0,     0,   163,   165,   166,
       0,   145,     0,     0,     0,   142,    89,    96,    85,    92,
     174,     0,     0,     0,     0,    68,    73,   205,   206,    76,
       0,     0,    63,     0,   105,   101,     0,   153,     0,     0,
     137,   138,   139,   140,     0,     0,    69,    70,    71,     0,
     207,   208,    78,    80,   209,    82,   103,     0,     0,   107,
     109,     0,     0,     0,     0,    93,     0,     0,    75,    77,
       0,     0,     0,     0,   100,   106,   174,     0,     0,   155,
     157,    97,    90,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    87,    79,    81,    83,   104,     0,   108,
     110,     0,    94,    98,     0,   102,     0,     0,     0
};

static const short yydefgoto[] =
{
     416,     1,     2,     4,    38,    41,    42,    43,    44,    45,
      46,    47,    87,    88,    48,    89,    49,    90,    50,    92,
      93,   131,    51,   123,   124,   208,   209,   210,   236,   211,
     276,   277,   244,   245,   303,   305,   359,   306,   380,   381,
     382,   238,   298,   294,   354,   297,   374,   239,   402,   308,
     368,   383,   367,   309,   387,   388,     5,   125,   136,   137,
     138,   139,   177,   140,   179,   180,   181,   216,   247,   141,
     142,   182,   167,   201,   233,   322,   197,   253,   254,   255,
     256,   252,   284,   198,   257,   288,   248,   100,   150,   187,
     151,   152,   101,   148,   171,   102,   128,   145,   184,     7,
      11,    12,   266,   267,   224,   225,   226,   227,   228,   146,
     169,   340,   341,   362,   363,   365,   300,   119,   229,   234,
     235,   153
};

static const short yypact[] =
{
  -32768,-32768,   -22,    51,-32768,-32768,-32768,    15,    45,-32768,
  -32768,   465,   465,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,    26,    31,-32768,    40,    47,    28,
      80,    34,-32768,-32768,-32768,-32768,    84,   101,   105,   107,
     138,-32768,   465,   246,-32768,-32768,   144,   465,   465,-32768,
      94,-32768,   135,-32768,   146,-32768,   116,-32768,    84,   150,
      84,    84,    86,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,    12,-32768,   465,
     465,   551,    67,-32768,-32768,-32768,    84,-32768,-32768,-32768,
     149,-32768,   247,-32768,   174,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,   222,   254,   178,-32768,   145,-32768,-32768,
  -32768,   243,   241,   244,    11,   248,   242,-32768,-32768,   249,
     250,-32768,   268,-32768,   285,-32768,-32768,-32768,   465,-32768,
  -32768,   273,    -2,-32768,   243,   243,-32768,   276,-32768,   169,
  -32768,-32768,   143,-32768,    38,-32768,   300,-32768,-32768,   574,
  -32768,-32768,-32768,-32768,-32768,   294,   295,   465,   538,   122,
  -32768,-32768,   298,   136,    65,-32768,-32768,   243,    53,   355,
     321,-32768,   324,-32768,-32768,-32768,   275,   329,-32768,-32768,
  -32768,   383,-32768,-32768,-32768,-32768,-32768,   325,   330,-32768,
     362,-32768,   350,-32768,   551,   356,-32768,-32768,-32768,-32768,
  -32768,-32768,   410,   298,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,   176,   375,-32768,-32768,   298,   485,-32768,-32768,
  -32768,-32768,-32768,    -1,   177,-32768,-32768,   328,   410,-32768,
  -32768,   376,   148,-32768,   298,   351,   379,-32768,   383,   383,
     383,   383,-32768,-32768,-32768,-32768,-32768,   380,-32768,-32768,
     360,   360,   360,-32768,   382,   384,   402,-32768,-32768,-32768,
     403,-32768,-32768,-32768,   172,   298,   243,   243,   302,   383,
     383,   383,   383,   357,   298,   551,   405,   183,   385,-32768,
     413,   414,   415,   185,    87,   444,-32768,   100,-32768,-32768,
  -32768,-32768,-32768,-32768,   416,   417,   418,-32768,-32768,-32768,
     419,-32768,   420,   421,   422,-32768,-32768,-32768,-32768,-32768,
     243,   423,   424,   425,    35,-32768,-32768,-32768,-32768,-32768,
     182,   426,-32768,   382,-32768,-32768,   182,-32768,   298,   298,
  -32768,-32768,-32768,-32768,   427,   430,-32768,-32768,-32768,   187,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,   207,   432,-32768,
  -32768,   433,   434,   521,   435,-32768,   325,   224,-32768,-32768,
     298,   298,   298,   437,-32768,-32768,   243,   427,   427,-32768,
  -32768,-32768,-32768,   438,   439,   457,   458,   431,   477,   478,
     483,   484,   211,-32768,-32768,-32768,-32768,-32768,   384,-32768,
  -32768,   538,-32768,-32768,   486,-32768,   493,   498,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,-32768,   499,   500,-32768,   501,
      -8,   428,-32768,   462,-32768,-32768,   502,-32768,   508,-32768,
     459,-32768,   -31,-32768,-32768,-32768,   352,-32768,-32768,-32768,
  -32768,   255,-32768,   258,-32768,   220,-32768,   215,-32768,-32768,
  -32768,  -286,-32768,-32768,-32768,-32768,  -114,  -303,-32768,  -351,
  -32768,-32768,-32768,   197,-32768,-32768,-32768,-32768,-32768,   456,
     453,-32768,-32768,-32768,-32768,   429,   412,-32768,   343,   467,
  -32768,-32768,-32768,-32768,  -137,-32768,  -218,-32768,-32768,-32768,
  -32768,-32768,-32768,  -203,-32768,-32768,   361,   -81,-32768,-32768,
    -152,-32768,   463,-32768,-32768,-32768,   460,-32768,-32768,-32768,
  -32768,-32768,   303,  -138,  -230,-32768,-32768,-32768,  -225,  -136,
  -32768,  -299,-32768,   267,   269,-32768,     4,  -113,   -85,   395,
     -11,  -142
};


#define	YYLAST		629


static const short yytable[] =
{
      34,    35,   175,   176,   240,   251,   120,   268,   346,   104,
     174,   329,   269,   273,   147,    61,   385,    53,   264,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,   185,   283,   205,   285,   217,     3,    61,
     192,    72,    70,   156,   157,   158,    94,    95,   203,     9,
      10,   375,   270,   271,     6,   149,   186,   414,   223,    52,
      70,   272,   127,    96,   296,    61,   313,   314,   105,   106,
     107,   204,   158,    98,   339,     8,   326,   344,   103,   103,
     250,   206,   345,   207,   375,   375,    53,    36,   270,   271,
     393,   129,    37,   193,   240,    59,   190,   272,   346,   413,
      39,    54,    55,    56,    57,    58,   250,    40,   143,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    57,    58,
     118,   289,   290,   291,   292,   217,   202,    91,   121,   193,
     371,   372,   337,   338,   315,   316,   265,   103,   217,    56,
      57,    58,   221,   391,    60,   337,   319,    97,   105,   106,
     107,   240,   320,   321,   323,   324,    54,    55,    56,    57,
      58,    62,   394,   395,   396,    64,   103,    66,    55,    56,
      57,    58,    55,   240,    57,    58,   157,   158,   355,    13,
      14,    15,   178,   194,   240,   240,   199,   200,   143,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    69,   240,
     118,   156,   157,   158,    91,   144,   274,   275,   196,   282,
     126,   249,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,   360,
     361,    33,   196,   312,   399,   130,   279,   249,   134,   258,
     259,   260,   261,   237,   328,   334,   335,   377,   378,   132,
      13,    14,    15,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,   307,   384,   337,
     338,   411,   412,   400,   401,   301,   302,   318,   105,   106,
     107,   133,   279,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
     149,   154,    33,   161,   155,    13,    14,    15,   159,   162,
     164,   127,   191,   217,   218,   219,   220,   221,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   166,   168,
     118,    13,    14,    15,   172,   183,   178,   222,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,   188,   189,    33,   196,   149,
      13,    14,    15,   317,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,   212,   213,    33,   214,   237,    13,    14,    15,   278,
     230,   241,   207,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
     243,   286,    33,    13,    14,    15,   299,   246,   325,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,   263,   281,    33,   287,
     295,   231,   304,   232,   307,   330,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,   310,   311,    33,   327,   149,    13,    14,
      15,   331,   332,   333,   275,   364,   337,   347,   348,   349,
     350,   351,   352,   353,   356,   357,   358,   373,   105,   106,
     107,   376,   386,   417,   389,   390,   392,   397,   418,   403,
     404,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,   405,   406,
      33,   265,   191,   217,   105,   106,   107,   221,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   407,   408,
     118,   105,   106,   107,   409,   410,    63,   415,    65,    99,
      67,   122,    71,   135,   105,   106,   107,    68,   191,   217,
     342,   336,   242,   366,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   379,   191,   118,   105,   106,   107,
     398,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   160,   163,   118,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   215,   293,   118,   165,   195,   280,
     343,   170,   173,   369,     0,   370,     0,   143,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   262,     0,   118
};

static const short yycheck[] =
{
      11,    12,   154,   155,   207,   223,    91,   237,   307,    90,
     152,   297,   237,   243,   127,    46,   367,     6,   236,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,   169,   252,   187,   254,    38,    60,    70,
     178,    52,    50,    32,    33,    34,    57,    58,   184,     4,
       5,   354,    53,    54,     3,    57,   169,   408,   196,    31,
      68,    62,    64,    71,   267,    96,   284,   285,     3,     4,
       5,   184,    34,    61,   304,    60,   294,   307,    89,    90,
     222,    28,   307,    30,   387,   388,     6,    61,    53,    54,
     376,   102,    61,   178,   297,    61,   177,    62,   397,   402,
      60,    21,    22,    23,    24,    25,   248,    60,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    24,    25,
      55,   258,   259,   260,   261,    38,    61,    60,    61,   214,
     348,   349,    45,    46,   286,   287,    36,   148,    38,    23,
      24,    25,    42,   373,    60,    45,   288,    61,     3,     4,
       5,   354,   289,   290,   291,   292,    21,    22,    23,    24,
      25,    60,   380,   381,   382,    60,   177,    60,    22,    23,
      24,    25,    22,   376,    24,    25,    33,    34,   330,     3,
       4,     5,    60,    61,   387,   388,    50,    51,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    60,   402,
      55,    32,    33,    34,    60,    60,    29,    30,    60,    61,
      61,   222,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    47,
      48,    55,    60,    61,   386,    61,   247,   248,    60,    63,
      64,    65,    66,    60,    61,    60,    61,    60,    61,    27,
       3,     4,     5,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    60,    61,    45,
      46,    60,    61,   387,   388,   271,   272,   288,     3,     4,
       5,    27,   293,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      57,    60,    55,    61,    60,     3,     4,     5,    60,    60,
      60,    64,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    60,    44,
      55,     3,     4,     5,    61,    35,    60,    62,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    61,    61,    55,    60,    57,
       3,     4,     5,    61,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    26,    61,    55,    60,    60,     3,     4,     5,    61,
      61,    61,    30,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      60,    60,    55,     3,     4,     5,    56,    61,    61,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    61,    61,    55,    60,
      60,    58,    60,    60,    60,    60,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    61,    61,    55,    61,    57,     3,     4,
       5,    58,    58,    58,    30,    49,    45,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    60,     3,     4,
       5,    61,    60,     0,    61,    61,    61,    60,     0,    61,
      61,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    61,    61,
      55,    36,    37,    38,     3,     4,     5,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    61,    61,
      55,     3,     4,     5,    61,    61,    47,    61,    48,    87,
      49,    92,    50,   125,     3,     4,     5,    49,    37,    38,
     305,   303,   210,   343,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,   359,    37,    55,     3,     4,     5,
     383,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,   135,   139,    55,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,   192,   262,    55,   140,   179,   248,
     307,   148,   152,   346,    -1,   346,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,   232,    -1,    55
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
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
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
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
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


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
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
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
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


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
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
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

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

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
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
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
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
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

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 235 "pddl.yy"
{ success = true; line_number = 1; }
    break;
case 2:
#line 236 "pddl.yy"
{ if (!success) YYERROR; }
    break;
case 6:
#line 248 "pddl.yy"
{ make_domain(yyvsp[-1].str); }
    break;
case 29:
#line 289 "pddl.yy"
{ requirements->strips = true; }
    break;
case 30:
#line 290 "pddl.yy"
{ requirements->typing = true; }
    break;
case 31:
#line 292 "pddl.yy"
{ requirements->negative_preconditions = true; }
    break;
case 32:
#line 294 "pddl.yy"
{ requirements->disjunctive_preconditions = true; }
    break;
case 33:
#line 295 "pddl.yy"
{ requirements->equality = true; }
    break;
case 34:
#line 297 "pddl.yy"
{ requirements->existential_preconditions = true; }
    break;
case 35:
#line 299 "pddl.yy"
{ requirements->universal_preconditions = true; }
    break;
case 36:
#line 301 "pddl.yy"
{ requirements->quantified_preconditions(); }
    break;
case 37:
#line 302 "pddl.yy"
{ requirements->conditional_effects = true; }
    break;
case 38:
#line 303 "pddl.yy"
{ throw Exception("`:fluents' not supported"); }
    break;
case 39:
#line 304 "pddl.yy"
{ requirements->adl(); }
    break;
case 40:
#line 305 "pddl.yy"
{ requirements->durative_actions = true; }
    break;
case 41:
#line 307 "pddl.yy"
{ requirements->duration_inequalities = true; }
    break;
case 42:
#line 309 "pddl.yy"
{ throw Exception("`:continuous-effects' not supported"); }
    break;
case 43:
#line 312 "pddl.yy"
{ require_typing(); }
    break;
case 44:
#line 313 "pddl.yy"
{ name_map_kind = NOTHING; }
    break;
case 45:
#line 316 "pddl.yy"
{ name_map_kind = CONSTANT_MAP; }
    break;
case 46:
#line 317 "pddl.yy"
{ name_map_kind = NOTHING; }
    break;
case 50:
#line 331 "pddl.yy"
{ make_predicate(yyvsp[0].str); }
    break;
case 51:
#line 332 "pddl.yy"
{ add_predicate(); }
    break;
case 52:
#line 339 "pddl.yy"
{ make_action(yyvsp[0].str, false); }
    break;
case 53:
#line 341 "pddl.yy"
{ add_action(); }
    break;
case 54:
#line 342 "pddl.yy"
{ make_action(yyvsp[0].str, true); }
    break;
case 55:
#line 344 "pddl.yy"
{ add_action(); }
    break;
case 60:
#line 355 "pddl.yy"
{ formula_time = Formula::OVER_ALL; }
    break;
case 61:
#line 356 "pddl.yy"
{ action->set_precondition(*yyvsp[0].formula); }
    break;
case 62:
#line 359 "pddl.yy"
{ add_effect(); }
    break;
case 63:
#line 362 "pddl.yy"
{ action->set_precondition(*yyvsp[-1].formula); }
    break;
case 66:
#line 367 "pddl.yy"
{ add_effect(); }
    break;
case 68:
#line 376 "pddl.yy"
{ require_duration_inequalities(); }
    break;
case 69:
#line 380 "pddl.yy"
{
				 require_duration_inequalities();
				 action->set_max_duration(yyvsp[-1].num);
			       }
    break;
case 70:
#line 385 "pddl.yy"
{
				 require_duration_inequalities();
				 action->set_min_duration(yyvsp[-1].num);
			       }
    break;
case 71:
#line 390 "pddl.yy"
{ action->set_duration(yyvsp[-1].num); }
    break;
case 75:
#line 403 "pddl.yy"
{ yyval.formula = yyvsp[-1].formula; }
    break;
case 76:
#line 406 "pddl.yy"
{ yyval.formula = &Formula::TRUE; }
    break;
case 77:
#line 407 "pddl.yy"
{ yyval.formula = &(*yyvsp[-1].formula && *yyvsp[0].formula); }
    break;
case 78:
#line 410 "pddl.yy"
{ formula_time = Formula::AT_START; }
    break;
case 79:
#line 411 "pddl.yy"
{ yyval.formula = yyvsp[-1].formula; }
    break;
case 80:
#line 412 "pddl.yy"
{ formula_time = Formula::AT_END; }
    break;
case 81:
#line 413 "pddl.yy"
{ yyval.formula = yyvsp[-1].formula; }
    break;
case 82:
#line 414 "pddl.yy"
{ formula_time = Formula::OVER_ALL; }
    break;
case 83:
#line 415 "pddl.yy"
{ yyval.formula = yyvsp[-1].formula; }
    break;
case 86:
#line 424 "pddl.yy"
{ prepare_forall_effect(); }
    break;
case 87:
#line 425 "pddl.yy"
{ add_forall_effect(); }
    break;
case 88:
#line 426 "pddl.yy"
{ formula_time = Formula::OVER_ALL; }
    break;
case 89:
#line 427 "pddl.yy"
{ prepare_conditional_effect(yyvsp[0].formula); }
    break;
case 90:
#line 428 "pddl.yy"
{ add_conditional_effect(); }
    break;
case 95:
#line 439 "pddl.yy"
{ add_positive(*yyvsp[0].atom); }
    break;
case 96:
#line 440 "pddl.yy"
{ add_negative(*yyvsp[-1].atom); }
    break;
case 101:
#line 449 "pddl.yy"
{ prepare_forall_effect(); }
    break;
case 102:
#line 450 "pddl.yy"
{ add_forall_effect(); }
    break;
case 103:
#line 451 "pddl.yy"
{ prepare_conditional_effect(yyvsp[0].formula); }
    break;
case 104:
#line 452 "pddl.yy"
{ add_conditional_effect(); }
    break;
case 107:
#line 459 "pddl.yy"
{ formula_time = Formula::AT_START; }
    break;
case 108:
#line 460 "pddl.yy"
{ add_effect(); }
    break;
case 109:
#line 461 "pddl.yy"
{ formula_time = Formula::AT_END; }
    break;
case 110:
#line 462 "pddl.yy"
{ add_effect(); }
    break;
case 111:
#line 470 "pddl.yy"
{ make_problem(yyvsp[-5].str, yyvsp[-1].str); }
    break;
case 112:
#line 471 "pddl.yy"
{ delete requirements; }
    break;
case 119:
#line 486 "pddl.yy"
{ name_map_kind = OBJECT_MAP; }
    break;
case 124:
#line 496 "pddl.yy"
{ problem->add_init(*yyvsp[0].atom); }
    break;
case 126:
#line 500 "pddl.yy"
{ prepare_atom(yyvsp[0].str); }
    break;
case 127:
#line 501 "pddl.yy"
{ yyval.atom = make_atom(); }
    break;
case 129:
#line 505 "pddl.yy"
{ add_term(yyvsp[0].str); }
    break;
case 132:
#line 512 "pddl.yy"
{ formula_time = Formula::AT_START; }
    break;
case 133:
#line 513 "pddl.yy"
{ problem->set_goal(*yyvsp[-1].formula); }
    break;
case 135:
#line 519 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 136:
#line 520 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 141:
#line 527 "pddl.yy"
{}
    break;
case 142:
#line 528 "pddl.yy"
{ delete yyvsp[-2].str; }
    break;
case 143:
#line 529 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 146:
#line 540 "pddl.yy"
{ yyval.formula = yyvsp[0].atom; }
    break;
case 147:
#line 541 "pddl.yy"
{ yyval.formula = make_equality(*yyvsp[-2].term, *yyvsp[-1].term); }
    break;
case 148:
#line 542 "pddl.yy"
{ yyval.formula = make_negation(*yyvsp[-1].formula); }
    break;
case 149:
#line 543 "pddl.yy"
{ yyval.formula = yyvsp[-1].formula; }
    break;
case 150:
#line 544 "pddl.yy"
{ prepare_disjunction(); }
    break;
case 151:
#line 544 "pddl.yy"
{ yyval.formula = yyvsp[-1].formula; }
    break;
case 152:
#line 545 "pddl.yy"
{ prepare_disjunction(); }
    break;
case 153:
#line 546 "pddl.yy"
{ yyval.formula = &(!*yyvsp[-2].formula || *yyvsp[-1].formula); }
    break;
case 154:
#line 547 "pddl.yy"
{ prepare_exists(); }
    break;
case 155:
#line 548 "pddl.yy"
{ yyval.formula = make_exists(*yyvsp[-1].formula); }
    break;
case 156:
#line 549 "pddl.yy"
{ prepare_forall(); }
    break;
case 157:
#line 550 "pddl.yy"
{ yyval.formula = make_forall(*yyvsp[-1].formula); }
    break;
case 158:
#line 553 "pddl.yy"
{ yyval.formula = &Formula::TRUE; }
    break;
case 159:
#line 554 "pddl.yy"
{ yyval.formula = &(*yyvsp[-1].formula && *yyvsp[0].formula); }
    break;
case 160:
#line 557 "pddl.yy"
{ yyval.formula = &Formula::FALSE; }
    break;
case 161:
#line 558 "pddl.yy"
{ yyval.formula = &(*yyvsp[-1].formula || *yyvsp[0].formula); }
    break;
case 162:
#line 561 "pddl.yy"
{ prepare_atom(yyvsp[0].str); }
    break;
case 163:
#line 562 "pddl.yy"
{ yyval.atom = make_atom(); }
    break;
case 165:
#line 570 "pddl.yy"
{add_term(yyvsp[0].str); }
    break;
case 166:
#line 571 "pddl.yy"
{add_term(yyvsp[0].str); }
    break;
case 167:
#line 574 "pddl.yy"
{ yyval.term = &make_term(yyvsp[0].str); }
    break;
case 168:
#line 575 "pddl.yy"
{ yyval.term = &make_term(yyvsp[0].str); }
    break;
case 171:
#line 582 "pddl.yy"
{ add_variables(yyvsp[0].strs, Type::OBJECT); }
    break;
case 172:
#line 583 "pddl.yy"
{ add_variables(yyvsp[-1].strs, *yyvsp[0].type); }
    break;
case 176:
#line 591 "pddl.yy"
{ yyval.strs = new std::vector<std::string>(1, *yyvsp[0].str); delete yyvsp[0].str; }
    break;
case 177:
#line 593 "pddl.yy"
{ yyval.strs = yyvsp[-1].strs; yyval.strs->push_back(*yyvsp[0].str); delete yyvsp[0].str; }
    break;
case 178:
#line 596 "pddl.yy"
{ add_names(yyvsp[0].strs, Type::OBJECT); }
    break;
case 179:
#line 597 "pddl.yy"
{ add_names(yyvsp[-1].strs, *yyvsp[0].type); }
    break;
case 183:
#line 604 "pddl.yy"
{ yyval.strs = new std::vector<std::string>(1, *yyvsp[0].str); delete yyvsp[0].str; }
    break;
case 184:
#line 605 "pddl.yy"
{ yyval.strs = yyvsp[-1].strs; yyval.strs->push_back(*yyvsp[0].str); delete yyvsp[0].str; }
    break;
case 185:
#line 608 "pddl.yy"
{ yyval.type = yyvsp[0].type; }
    break;
case 186:
#line 611 "pddl.yy"
{ yyval.type = &Type::OBJECT; }
    break;
case 187:
#line 612 "pddl.yy"
{ yyval.type = &make_type(yyvsp[0].str); }
    break;
case 188:
#line 613 "pddl.yy"
{ yyval.type = &UnionType::simplify(*yyvsp[-1].types); }
    break;
case 189:
#line 616 "pddl.yy"
{ yyval.types = new UnionType(); }
    break;
case 190:
#line 617 "pddl.yy"
{ yyval.types = new UnionType(make_type(yyvsp[0].str)); }
    break;
case 191:
#line 618 "pddl.yy"
{ yyval.types = yyvsp[-1].types; }
    break;
case 192:
#line 619 "pddl.yy"
{ yyval.types = yyvsp[-1].types; yyval.types->add(make_type(yyvsp[0].str)); }
    break;
case 193:
#line 622 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 194:
#line 625 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 195:
#line 628 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 196:
#line 631 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 197:
#line 634 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 198:
#line 637 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 199:
#line 640 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 200:
#line 643 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 201:
#line 646 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 202:
#line 649 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 203:
#line 652 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 204:
#line 655 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 205:
#line 658 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 206:
#line 661 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 207:
#line 664 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 208:
#line 667 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 209:
#line 670 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
case 210:
#line 673 "pddl.yy"
{ delete yyvsp[0].str; }
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 701 "pddl.yy"



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
