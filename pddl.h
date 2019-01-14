/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_PDDL_H_INCLUDED
# define YY_YY_PDDL_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
/* Tokens.  */
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 231 "pddl.yy" /* yacc.c:1909  */

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

#line 192 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PDDL_H_INCLUDED  */
