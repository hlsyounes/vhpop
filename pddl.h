/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
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

/* Line 2068 of yacc.c  */
#line 204 "pddl.yy"

  const Formula* formula;
  const Atom* atom;
  const std::string* str;
  std::vector<std::string>* strs;
  const Term* term;
  const Type* type;
  UnionType* types;
  float num;



/* Line 2068 of yacc.c  */
#line 181 "pddl.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


