/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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
#line 228 "pddl.yy"

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

#line 205 "pddl.hh"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PDDL_HH_INCLUDED  */
