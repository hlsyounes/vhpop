/* -*-C++-*- */
/*
 * PDDL tokenizer.
 *
 * $Id: tokens.ll,v 1.7 2001-12-23 22:09:13 lorens Exp $
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

#include <string>
#include <vector>
#include <cctype>
#include "pddl.cc.h"
#include "support.h"


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
