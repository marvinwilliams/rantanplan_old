%require "3.3"
%language "c++"
%skeleton "lalr1.cc"

%code requires {
  namespace parser {
    template<typename Scanner, typename Parser>
    class Driver;
    class Scanner;
  }
}

%code top {
  #include "parser.hxx"
  #include "driver.h"
  #include "scanner.h"
  #include "../model/builder.h"
  #include <iostream>
  #ifdef TRACE
  #define YYDEBUG 1
  #endif
  #undef yylex
  #define yylex driver.lex
}

%defines
%locations
%define api.location.file none

%define api.value.type variant
%define api.token.constructor
%define parse.assert
%define parse.error verbose
%define api.token.prefix {TOK_}
%define api.namespace {parser}
%define api.parser.class {Parser}

%parse-param {Driver<Scanner, Parser>& driver}

%token
LPAREN        "("
RPAREN        ")"
HYPHEN        "-"
EQUALITY      "="
AND           "and"
OR            "or"
NOT           "not"
DEFINE        "def"
DOMAIN        "domain"
TYPES         "types"
CONSTANTS     "const"
PREDICATES    "pred"
ACTION        "action"
PARAMETERS    "param"
PRECONDITION  "precond"
EFFECT        "effect"
PROBLEM       "problem"
DOMAIN_REF    "dref"
OBJECTS       "obj"
INIT          "init"
GOAL          "goal"
REQUIREMENTS  "reqs"

END 0         "end of file"

<std::string> NAME      "name"
<std::string> VARIABLE  "var"
<std::string> KEYWORD   "key"
;

%type
domain-def
domain-body
require-def
require-list
types-def
constants-def
predicates-def
predicate-list
predicate-def
action-def
action-body
precondition-def
precondition-body
precondition-list
effect-def
effect-body
effect-list
problem-def
unknown-keyword
<std::vector<std::pair<std::string, std::string>>>  typed-name-list
<std::vector<std::pair<std::string, std::string>>>  typed-var-list
<std::vector<std::string>>                          name-list
<std::vector<std::string>>                          var-list
<std::vector<std::string>>                          param-list

;

%%
%start unit;
unit:
    domain-def problem-def {
      model::Builder builder{"domain", "problem"};
      builder.add_requirement("invalid_requirement", driver.get_location());
    }
;
domain-def:
    "(" "def" "(" "domain" "name" ")" domain-body ")" {
    }
;
domain-body:
    %empty {}
  | domain-body require-def {}
  | domain-body types-def {}
  | domain-body constants-def {}
  | domain-body predicates-def {}
  | domain-body action-def {}
  | domain-body unknown-keyword {}
;
require-def:
    "(" "reqs" require-list ")" {
    }
;
require-list:
    "key" {
    }
  | require-list "key" {
    }
;
types-def:
    "(" "types" typed-name-list ")" {
    }
;
constants-def:
    "(" "const" typed-name-list ")" {
    }
;
predicates-def:
    "(" "pred" predicate-list ")" {
    }
;
predicate-list:
    predicate-def {
    }
  | predicate-list predicate-def {
    }
;
predicate-def:
    "(" "name" typed-var-list ")" {
    }
;
action-def:
    "(" "action" "name" "param" "(" typed-var-list ")" action-body ")" {
    }
;
action-body:
  precondition-def effect-def {
  }
;
precondition-def:
    %empty {}
  | "precond" "(" precondition-body ")" {
    }
;
precondition-body:
    %empty {}
  | "name" param-list {
    }
  | "=" param-list {
    }
  | "and" precondition-list {
    }
  | "or" precondition-list {
    }
  | "not" "(" precondition-body ")" {
    }
;
precondition-list:
    %empty {}
  | precondition-list "(" precondition-body ")" {
    }
;
effect-def:
    %empty {}
  | "effect" "(" effect-body ")" {
    }
;
effect-body:
    %empty {}
  | "name" param-list {
    }
  | "not" "(" "name" param-list ")" {
    }
  | "and" effect-list {
    }
;
effect-list:
    %empty {}
  | effect-list "(" effect-body ")" {
    }
;
problem-def:
    %empty {}
;
unknown-keyword:
    "(" "key" error ")" {
      yyerrok;
    }
;
typed-name-list:
    name-list {
    }
  | name-list "name" "-" "name" typed-name-list {
    }
;
typed-var-list:
    var-list {
    }
  | var-list "var" "-" "name" typed-var-list {
    }
;
name-list:
    %empty {}
  | name-list "name" {
    }
;
var-list:
    %empty {}
  | var-list "var" {
    }
;
param-list:
    %empty {}
  | param-list "name" {
    }
  | param-list "var" {
    }
;
%%

void parser::Parser::error (const location_type& l, const std::string& m)
{
  throw syntax_error(l, m);
}
