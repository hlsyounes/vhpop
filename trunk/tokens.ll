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
 * $Id: tokens.ll,v 1.9 2002-01-25 18:22:56 lorens Exp $
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

static string* tolower(const char* s);
%}

%option case-insensitive never-interactive nounput

%%

[()\-]                       return yytext[0];
define                       return DEFINE;
domain                       return DOMAIN;
problem                      return PROBLEM;
:requirements                return REQUIREMENTS;
:strips                      return STRIPS;
:typing                      return TYPING;
:disjunctive-preconditions   return DISJUNCTIVE_PRECONDITIONS;
:equality                    return EQUALITY;
:existential-preconditions   return EXISTENTIAL_PRECONDITIONS;
:universal-preconditions     return UNIVERSAL_PRECONDITIONS;
:quantified-preconditions    return QUANTIFIED_PRECONDITIONS;
:conditional-effects         return CONDITIONAL_EFFECTS;
:adl                         return ADL;
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
when                         return WHEN;
not                          return NOT;
and                          return AND;
or                           return OR;
imply                        return IMPLY;
exists                       return EXISTS;
forall                       return FORALL;
either                       return EITHER;
[A-Za-z]([A-Za-z0-9\-_])*    { yylval.str = tolower(yytext); return NAME; }
=                            { return EQUALS; }
\?[A-Za-z]([A-Z0-9a-z\-_])*  { yylval.str = tolower(yytext); return VARIABLE; }
;.*$                         /* comment */
[ \t\r]+                     /* whitespace */
\n                           line_number++;
.                            return ILLEGAL_TOKEN;

%%

/* Allocates a string containing the lowercase characters of the given
   C string. */
static string* tolower(const char* s) {
  string* result = new (GC) string();
  for (const char* p = s; *p != '\0'; p++) {
    *result += tolower(*p);
  }
  return result;
}
