/* -*-C++-*- */
/*
 * PDDL tokenizer.
 *
 * Copyright (C) 2002 Carnegie Mellon University
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
 * $Id: tokens.ll,v 2.1 2002-01-30 22:43:32 lorens Exp $
 */
%{
struct Type;
struct TermList;
struct Name;
struct Variable;
struct VariableList;
struct Formula;
struct FormulaList;
struct Atom;
struct AtomList;
struct NegationList;
struct Effect;
struct EffectList;
struct ActionSchema;

#include <cctype>
#include "support.h"
#include "pddl.cc.h"


/* Current line number. */
size_t line_number;

static int make_string(const char* s, int token);
%}

%option case-insensitive never-interactive nounput

%%

[()\-]                       return yytext[0];
define                       return make_string(yytext, DEFINE);
domain                       return make_string(yytext, DOMAIN);
problem                      return make_string(yytext, PROBLEM);
:requirements                return REQUIREMENTS;
:strips                      return STRIPS;
:typing                      return TYPING;
:negative-preconditions      return NEGATIVE_PRECONDITIONS;
:disjunctive-preconditions   return DISJUNCTIVE_PRECONDITIONS;
:equality                    return EQUALITY;
:existential-preconditions   return EXISTENTIAL_PRECONDITIONS;
:universal-preconditions     return UNIVERSAL_PRECONDITIONS;
:quantified-preconditions    return QUANTIFIED_PRECONDITIONS;
:conditional-effects         return CONDITIONAL_EFFECTS;
:fluents                     return FLUENTS;
:adl                         return ADL;
:durative-actions            return DURATIVE_ACTIONS;
:duration-inequalities       return DURATION_INEQUALITIES;
:continuous-effects          return CONTINUOUS_EFFECTS;
:types                       return TYPES;
:constants                   return CONSTANTS;
:predicates                  return PREDICATES;
:action                      return ACTION;
:parameters                  return PARAMETERS;
:precondition                return PRECONDITION;
:effect                      return EFFECT;
:domain                      return PDOMAIN;
:objects                     return OBJECTS;
:init                        return INIT;
:goal                        return GOAL;
when                         return make_string(yytext, WHEN);
not                          return make_string(yytext, NOT);
and                          return make_string(yytext, AND);
or                           return make_string(yytext, OR);
imply                        return make_string(yytext, IMPLY);
exists                       return make_string(yytext, EXISTS);
forall                       return make_string(yytext, FORALL);
either                       return make_string(yytext, EITHER);
[A-Za-z]([A-Za-z0-9\-_])*    return make_string(yytext, NAME);
=                            return EQUALS;
\?[A-Za-z]([A-Z0-9a-z\-_])*  return make_string(yytext, VARIABLE);
;.*$                         /* comment */
[ \t\r]+                     /* whitespace */
\n                           line_number++;
.                            return ILLEGAL_TOKEN;

%%

/* Allocates a string containing the lowercase characters of the given
   C string, and returns the given token. */
static int make_string(const char* s, int token) {
  string* result = new (GC) string();
  for (const char* p = s; *p != '\0'; p++) {
    *result += tolower(*p);
  }
  yylval.str = result;
  return token;
}
