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
/* Line 1318 of yacc.c.  */
#line 175 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



