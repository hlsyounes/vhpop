typedef union {
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
  UnionType* utype;
  const Type* type;
} YYSTYPE;
#define	DEFINE	257
#define	DOMAIN_TOKEN	258
#define	PROBLEM	259
#define	REQUIREMENTS	260
#define	STRIPS	261
#define	TYPING	262
#define	NEGATIVE_PRECONDITIONS	263
#define	DISJUNCTIVE_PRECONDITIONS	264
#define	EQUALITY	265
#define	EXISTENTIAL_PRECONDITIONS	266
#define	UNIVERSAL_PRECONDITIONS	267
#define	QUANTIFIED_PRECONDITIONS	268
#define	CONDITIONAL_EFFECTS	269
#define	FLUENTS	270
#define	ADL	271
#define	DURATIVE_ACTIONS	272
#define	DURATION_INEQUALITIES	273
#define	CONTINUOUS_EFFECTS	274
#define	TYPES	275
#define	CONSTANTS	276
#define	PREDICATES	277
#define	ACTION	278
#define	DURATIVE_ACTION	279
#define	DURATION	280
#define	PARAMETERS	281
#define	PRECONDITION	282
#define	CONDITION	283
#define	EFFECT	284
#define	PDOMAIN	285
#define	OBJECTS	286
#define	INIT	287
#define	GOAL	288
#define	METRIC	289
#define	WHEN	290
#define	NOT	291
#define	AND	292
#define	OR	293
#define	IMPLY	294
#define	EXISTS	295
#define	FORALL	296
#define	EITHER	297
#define	AT	298
#define	OVER	299
#define	START	300
#define	END	301
#define	ALL	302
#define	MINIMIZE	303
#define	MAXIMIZE	304
#define	TOTAL_TIME	305
#define	LE	306
#define	GE	307
#define	NAME	308
#define	DURATION_VAR	309
#define	VARIABLE	310
#define	NUMBER	311
#define	ILLEGAL_TOKEN	312


extern YYSTYPE yylval;
