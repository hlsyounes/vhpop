/* -*-C++-*- */
/*
 * PDDL tokenizer.
 *
 * $Id: tokens.ll,v 1.1 2001-05-03 15:43:40 lorens Exp $
 */
%{
#include "formulas.h"
#include "domains.h"
#include "pddl.cc.h"

unsigned int line_number;
%}

%option never-interactive nounput

%%

[()\-]                       return yytext[0];
define                       return DEFINE;
domain                       return DOMAIN;
problem                      return PROBLEM;
:requirements                return REQUIREMENTS;
:strips                      return REQSTRIPS;
:adl                         return REQADL;
:typing                      return TYPING;
:equality                    return EQUALITY;
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
[A-Za-z]([A-Za-z0-9\-_])*    { yylval.str = new (GC) string(tolower(yytext));
                               return NAME; }
=                            { yylval.str = new (GC) string(tolower(yytext));
                               return NAME; }
\?[A-Za-z]([A-Z0-9a-z\-_])*  { yylval.str = new (GC) string(tolower(yytext));
			       return VARIABLE; }
;.*$                         /* comment */
[ \t\r]+                     /* whitespace */
\n                           line_number++;
.                            return ILLEGAL_TOKEN;

%%
