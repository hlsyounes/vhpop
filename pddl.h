#ifndef BISON_PDDL_H
# define BISON_PDDL_H

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


extern YYSTYPE yylval;

#endif /* not BISON_PDDL_H */
