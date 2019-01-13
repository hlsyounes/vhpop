/* -*-C++-*-
 * Copyright (C) 2002--2005 Carnegie Mellon University
 * Copyright (C) 2019 Google Inc
 *
 * This file is part of VHPOP.
 *
 * VHPOP is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * VHPOP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VHPOP; if not, write to the Free Software Foundation,
 * Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * PDDL tokenizer.
 */
%{
struct Condition;
struct Formula;
struct Literal;
struct Atom;
struct Expression;
struct Fluent;

#include "terms.h"
#include "types.h"

#include <cctype>
#include <set>
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
:timed-initial-literals		return TIMED_INITIAL_LITERALS;
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
