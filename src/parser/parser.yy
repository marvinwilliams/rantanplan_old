%require "3.3"
%language "c++"
%skeleton "lalr1.cc"

%code requires {
  namespace parser {
    class Scanner;
  }
  #include "ast.h"
}

%code provides {
}

%code top {
  #include "parser.hxx"
  #include "scanner.h"
  #include "ast.h"
  #include <iostream>
  #undef yylex
  #define yylex scanner.lex
}

%defines
%locations
%define api.value.type variant
%define api.token.constructor
%define parse.assert
%define parse.error verbose
%define api.token.prefix {TOK_}
%define api.namespace {parser}
%define api.parser.class {Parser}

%parse-param {Scanner& scanner} {ast::AST& ast}

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
<std::unique_ptr<ast::Domain>> domain-def
<std::vector<ast::Domain::Element>> domain-body
<std::unique_ptr<ast::Domain::Element>> require-def
<std::vector<ast::Requirement>> require-list
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
problem-body
object-def
init-def
init-list
goal-def
<std::vector<std::pair<std::vector<std::string>, std::string>>>  typed-name-list
<std::vector<std::pair<std::string, std::string>>>  typed-var-list
<std::vector<std::string>>                          name-list
<std::vector<std::string>>                          var-list
<std::vector<std::string>>                          param-list
;

%%
%start unit;
unit:
    domain-def {
      scanner.domain_end();
    }
    problem-def {
    }
    {
      ast.set_domain(std::move($[domain-def]));
    }
;
domain-def:
    "(" DEFINE "(" DOMAIN NAME ")" domain-body ")" {
      $$ = std::make_unique<ast::Domain>(@$, $[NAME], std::move($[domain-body]));
    }
;
domain-body:
    %empty {}
  | domain-body require-def {
      $1.push_back(std::move($[require-def]));
      $$ = std::move($1);
    }
  | domain-body types-def {
      $$ = std::move($1);
    }
  | domain-body constants-def {
      $$ = std::move($1);
    }
  | domain-body predicates-def {
      $$ = std::move($1);
    }
  | domain-body action-def {
      $$ = std::move($1);
    }
;
require-def:
    "(" REQUIREMENTS require-list ")" {
      $$ = ast::Domain::Element{ast::RequirementsDef{@$, std::move($[require-list])}};
    }
;
require-list:
    KEYWORD {
      $$.push_back(std::make_unique<ast::Requirement>(@$, $[KEYWORD]));
    }
  | require-list KEYWORD {
      $1.push_back(std::make_unique<ast::Requirement>(@$, $[KEYWORD]));
      $$ = std::move($1);
    }
;
types-def:
    "(" TYPES typed-name-list[types] ")" {
    }
;
constants-def:
    "(" CONSTANTS typed-name-list ")" {
    }
;
predicates-def:
    "(" PREDICATES predicate-list ")" {
    }
;
predicate-list:
    predicate-def {
    }
  | predicate-list predicate-def {
    }
;
predicate-def:
    "(" NAME typed-var-list ")" {
    }
;
action-def:
    "(" ACTION NAME PARAMETERS "(" typed-var-list ")" action-body ")" {
    }
;
action-body:
  precondition-def effect-def {
  }
;
precondition-def:
    %empty {}
  | PRECONDITION "(" precondition-body ")" {
    }
;
precondition-body:
    %empty {}
  | NAME param-list {
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
  | EFFECT "(" effect-body ")" {
    }
;
effect-body:
    %empty {}
  | NAME param-list {
    }
  | "not" "(" NAME param-list ")" {
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
    "(" DEFINE "(" PROBLEM NAME ")" "(" DOMAIN_REF NAME ")" problem-body ")" {
    }
;
problem-body:
    %empty {}
  | problem-body require-def {}
  | problem-body object-def {}
  | problem-body init-def {}
  | problem-body goal-def {}
;
object-def:
    "(" OBJECTS typed-name-list ")" {
    }
;
init-def:
    "(" INIT init-list ")" {
    }
;
init-list:
    %empty {}
  | init-list "(" NAME name-list ")" {
    }
  | init-list "(" "not" "(" NAME name-list ")" ")" {
  }
;
goal-def:
    "(" GOAL "(" precondition-body ")" ")" {
    }
;
typed-name-list:
    name-list {
    }
  | name-list[types] NAME[type] "-" NAME[supertype] typed-name-list[typelist] {
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
  | name-list NAME {
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
  std::cerr << l << ": " << m << '\n';
}
