/* -*-C++-*- */
/*
 * PDDL tokenizer.
 *
 * Copyright (C) 2003 Carnegie Mellon University
 * Written by Håkan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: tokens.ll,v 6.3 2003-09-05 16:34:09 lorens Exp $
 */
%{
struct Condition;
struct Formula;
struct Atom;
struct Expression;
struct Application;

#include "terms.h"
#include "types.h"
#include <cctype>
#include <string>
#include <vector>
#include "pddl.h"


/* Current line number. */
size_t line_number;

/* Allocates a string containing the lowercase characters of the given
   C string, and returns the given token. */
static int make_string(const char* s, int token);
/* Makes a number of the given string, and return the NUMBER token. */
static int make_number(const char* s);

%}

%option case-insensitive never-interactive nounput noyywrap

IDENT	[A-Za-z]([A-Za-z0-9\-_])*

%%

define				return make_string(yytext, DEFINE);
domain				return make_string(yytext, DOMAIN_TOKEN);
problem				return make_string(yytext, PROBLEM);
:requirements			return REQUIREMENTS;
:types				return TYPES;
:constants			return CONSTANTS;
:predicates			return PREDICATES;
:functions			return FUNCTIONS;
:strips				return STRIPS;
:typing				return TYPING;
:negative-preconditions		return NEGATIVE_PRECONDITIONS;
:disjunctive-preconditions	return DISJUNCTIVE_PRECONDITIONS;
:equality			return EQUALITY;
:existential-preconditions	return EXISTENTIAL_PRECONDITIONS;
:universal-preconditions	return UNIVERSAL_PRECONDITIONS;
:quantified-preconditions	return QUANTIFIED_PRECONDITIONS;
:conditional-effects		return CONDITIONAL_EFFECTS;
:fluents			return FLUENTS;
:adl				return ADL;
:durative-actions		return DURATIVE_ACTIONS;
:duration-inequalities		return DURATION_INEQUALITIES;
:continuous-effects		return CONTINUOUS_EFFECTS;
:action				return ACTION;
:durative-action		return DURATIVE_ACTION;
:parameters			return PARAMETERS;
:duration			return DURATION;
:precondition			return PRECONDITION;
:condition			return CONDITION;
:effect				return EFFECT;
:domain				return PDOMAIN;
:objects			return OBJECTS;
:init				return INIT;
:goal				return GOAL;
:metric				return METRIC;
number				return make_string(yytext, NUMBER_TOKEN);
object				return make_string(yytext, OBJECT_TOKEN);
either				return make_string(yytext, EITHER);
when				return make_string(yytext, WHEN);
not				return make_string(yytext, NOT);
and				return make_string(yytext, AND);
or				return make_string(yytext, OR);
imply				return make_string(yytext, IMPLY);
exists				return make_string(yytext, EXISTS);
forall				return make_string(yytext, FORALL);
at				return make_string(yytext, AT);
over				return make_string(yytext, OVER);
start				return make_string(yytext, START);
end				return make_string(yytext, END);
all				return make_string(yytext, ALL);
minimize			return make_string(yytext, MINIMIZE);
maximize			return make_string(yytext, MAXIMIZE);
total-time			return make_string(yytext, TOTAL_TIME);
{IDENT}				return make_string(yytext, NAME);
\?duration			return make_string(yytext, DURATION_VAR);
\?{IDENT}			return make_string(yytext, VARIABLE);
[0-9]*\.?[0-9]+			return make_number(yytext);
[()=+\-\*/]			return yytext[0];
\<=				return LE;
>=				return GE;
;.*$				/* comment */
[ \t\r]+			/* whitespace */
\n				line_number++;
.				return ILLEGAL_TOKEN;

%%

/* Allocates a string containing the lowercase characters of the given
   C string, and returns the given token. */
static int make_string(const char* s, int token) {
  std::string* result = new std::string();
  for (const char* p = s; *p != '\0'; p++) {
    *result += tolower(*p);
  }
  yylval.str = result;
  return token;
}


/* Makes a number of the given string, and return the NUMBER token. */
static int make_number(const char* s) {
  yylval.num = atof(s);
  return NUMBER;
}
