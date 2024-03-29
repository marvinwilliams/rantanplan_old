%require "3.3"
%language "c++"
%skeleton "lalr1.cc"

%code requires {
  #include "ast.h"
  #include "location.hxx"

  namespace parser {
  class Scanner;
  }
}

%code provides {
}

%code top {
  #include "parser.hxx"
  #include "scanner.h"
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

END 0         "eof"

<std::string> NAME        "name"
<std::string> VARIABLE    "var"
<std::string> REQUIREMENT "req"
;

%type
<std::unique_ptr<ast::Domain>> domain-def
<std::unique_ptr<std::vector<std::unique_ptr<ast::Element>>>> domain-body
<std::unique_ptr<ast::Element>> require-def
<std::unique_ptr<ast::RequirementsList>> require-list
<std::unique_ptr<ast::Element>> types-def
<std::unique_ptr<ast::Element>> constants-def
<std::unique_ptr<ast::Element>> predicates-def
<std::unique_ptr<ast::PredicateList>> predicate-list
<std::unique_ptr<ast::Predicate>> predicate-def
<std::unique_ptr<ast::Element>> action-def
<std::pair<std::optional<std::unique_ptr<ast::Precondition>>, std::optional<std::unique_ptr<ast::Effect>>>> action-body
<std::optional<std::unique_ptr<ast::Precondition>>> precondition-def
<std::unique_ptr<ast::Condition>> precondition-body
<std::unique_ptr<ast::ConditionList>> precondition-list
<std::optional<std::unique_ptr<ast::Effect>>> effect-def
<std::unique_ptr<ast::Condition>> effect-body
<std::unique_ptr<ast::ConditionList>> effect-list
<std::unique_ptr<ast::Problem>> problem-def
<std::unique_ptr<std::vector<std::unique_ptr<ast::Element>>>> problem-body
<std::unique_ptr<ast::Element>> objects-def
<std::unique_ptr<ast::Element>> init-def
<std::unique_ptr<ast::InitList>> init-list
<std::unique_ptr<ast::Element>> goal-def
<std::unique_ptr<ast::ArgumentList>> argument-list
<std::unique_ptr<ast::TypedNameList>> typed-name-list
<std::unique_ptr<ast::TypedVariableList>> typed-var-list
<std::unique_ptr<std::vector<std::unique_ptr<ast::TypedNameList::value_type>>>> single-typed-name-lists
<std::unique_ptr<std::vector<std::unique_ptr<ast::TypedVariableList::value_type>>>> single-typed-variable-lists
<std::unique_ptr<ast::NameList>> empty-or-name-list
<std::unique_ptr<ast::NameList>> name-list
<std::unique_ptr<ast::VariableList>> var-list
;

%%
%start unit;
unit:
    domain-def {
      scanner.domain_end();
      ast.set_domain(std::move($[domain-def]));
    }
    problem-def {
      ast.set_problem(std::move($[problem-def]));
    }
;
domain-def:
    "(" DEFINE "(" DOMAIN NAME ")" domain-body ")" {
      $$ = std::make_unique<ast::Domain>(@$, std::make_unique<ast::Name>(@[NAME], $[NAME]), std::move($[domain-body]));
    }
;
domain-body:
    %empty {
      $$ = std::make_unique<std::vector<std::unique_ptr<ast::Element>>>();
    }
  | domain-body require-def {
      $$ = std::move($1);
      $$->push_back(std::move($[require-def]));
    }
  | domain-body types-def {
      $$ = std::move($1);
      $$->push_back(std::move($[types-def]));
    }
  | domain-body constants-def {
      $$ = std::move($1);
      $$->push_back(std::move($[constants-def]));
    }
  | domain-body predicates-def {
      $$ = std::move($1);
      $$->push_back(std::move($[predicates-def]));
    }
  | domain-body action-def {
      $$ = std::move($1);
      $$->push_back(std::move($[action-def]));
    }
;
require-def:
    "(" REQUIREMENTS require-list ")" {
      $$ = std::make_unique<ast::Element>(ast::RequirementsDef{@$, std::move($[require-list])});
    }
;
require-list:
    REQUIREMENT {
      $$ = std::make_unique<ast::RequirementsList>(@$);
      $$->add(@$, std::make_unique<ast::Requirement>(@$, $[REQUIREMENT]));
    }
  | require-list REQUIREMENT {
      $$ = std::move($1);
      $$->add(@[REQUIREMENT], std::make_unique<ast::Requirement>(@[REQUIREMENT], $[REQUIREMENT]));
    }
;
types-def:
    "(" TYPES typed-name-list[types-list] ")" {
      $$ = std::make_unique<ast::Element>(ast::TypesDef{@$, std::move($[types-list])});
    }
;
constants-def:
    "(" CONSTANTS typed-name-list[constants-list] ")" {
      $$ = std::make_unique<ast::Element>(ast::TypesDef{@$, std::move($[constants-list])});
    }
;
predicates-def:
    "(" PREDICATES predicate-list ")" {
      $$ = std::make_unique<ast::Element>(ast::PredicatesDef{@$, std::move($[predicate-list])});
    }
;
predicate-list:
    predicate-def {
      $$ = std::make_unique<ast::PredicateList>(@$);
      $$->add(@$, std::move($[predicate-def]));
    }
  | predicate-list predicate-def {
      $$ = std::move($1);
      $$->add(@[predicate-def], std::move($[predicate-def]));
    }
;
predicate-def:
    "(" NAME typed-var-list[parameter-list] ")" {
      $$ = std::make_unique<ast::Predicate>(@$, std::make_unique<ast::Name>(@[NAME], $[NAME]), std::move($[parameter-list]));
    }
;
action-def:
    "(" ACTION NAME PARAMETERS "(" typed-var-list[parameter-list] ")" action-body ")" {
      $$ = std::make_unique<ast::Element>(ast::ActionDef{@$, std::make_unique<ast::Name>(@[NAME], $[NAME]), std::move($[parameter-list]), std::move($[action-body].first), std::move($[action-body].second)});
    }
;
action-body:
  precondition-def effect-def {
    $$ = std::make_pair(std::move($[precondition-def]), std::move($[effect-def]));
  }
;
precondition-def:
    %empty {
      $$ = std::nullopt;
    }
  | PRECONDITION "(" precondition-body ")" {
      $$ = std::make_unique<ast::Precondition>(@$, std::move($[precondition-body]));
    }
;
precondition-body:
    %empty {
      $$ = std::make_unique<ast::Condition>();
    }
  | NAME argument-list {
      $$ = std::make_unique<ast::Condition>(ast::PredicateEvaluation{@$, std::make_unique<ast::Name>(@[NAME], $[NAME]), std::move($[argument-list])});
    }
  | "=" argument-list {
      $$ = std::make_unique<ast::Condition>(ast::PredicateEvaluation{@$, std::make_unique<ast::Name>(@1, "="), std::move($[argument-list])});
    }
  | "and" precondition-list {
      $$ = std::make_unique<ast::Condition>(ast::Conjunction{@$, std::move($[precondition-list])});
    }
  | "or" precondition-list {
      $$ = std::make_unique<ast::Condition>(ast::Disjunction{@$, std::move($[precondition-list])});
    }
  | "not" "(" precondition-body[nested-body] ")" {
      $$ = std::make_unique<ast::Condition>(ast::Negation{@$, std::move($[nested-body])});
    }
;
precondition-list:
    %empty {
      $$ = std::make_unique<ast::ConditionList>(@$);
    }
  | precondition-list "(" precondition-body ")" {
      $$ = std::move($1);
      $$->add(@[precondition-body], std::move($[precondition-body]));
    }
;
effect-def:
    %empty {
      $$ = std::nullopt;
    }
  | EFFECT "(" effect-body ")" {
      $$ = std::make_unique<ast::Effect>(@$, std::move($[effect-body]));
    }
;
effect-body:
    %empty {
      $$ = std::make_unique<ast::Condition>();
    }
  | NAME argument-list {
      $$ = std::make_unique<ast::Condition>(ast::PredicateEvaluation{@$, std::make_unique<ast::Name>(@[NAME], $[NAME]), std::move($[argument-list])});
    }
  | "and" effect-list {
      $$ = std::make_unique<ast::Condition>(ast::Conjunction{@$, std::move($[effect-list])});
    }
  | "not" "("[start] NAME argument-list ")"[end] {
      auto predicate_evaluation = std::make_unique<ast::Condition>(ast::PredicateEvaluation{@[start] + @[end], std::make_unique<ast::Name>(@[NAME], $[NAME]), std::move($[argument-list])});
      $$ = std::make_unique<ast::Condition>(ast::Negation{@$, std::move(predicate_evaluation)});
    }
;
effect-list:
    %empty {
      $$ = std::make_unique<ast::ConditionList>(@$);
    }
  | effect-list "(" effect-body ")" {
      $$ = std::move($1);
      $$->add(@[effect-body], std::move($[effect-body]));
    }
;
problem-def:
    "(" DEFINE "(" PROBLEM NAME[problem-name] ")" "(" DOMAIN_REF NAME[ref-name] ")" problem-body ")" {
      $$ = std::make_unique<ast::Problem>(@$, std::make_unique<ast::Name>(@[problem-name], $[problem-name]), std::make_unique<ast::Name>(@[ref-name], $[ref-name]), std::move($[problem-body]));
    }
;
problem-body:
    %empty {
      $$ = std::make_unique<std::vector<std::unique_ptr<ast::Element>>>();
    }
  | problem-body require-def {
      $$ = std::move($1);
      $$->push_back(std::move($[require-def]));
    }
  | problem-body objects-def {
      $$ = std::move($1);
      $$->push_back(std::move($[objects-def]));
  }
  | problem-body init-def {
      $$ = std::move($1);
      $$->push_back(std::move($[init-def]));
  }
  | problem-body goal-def {
      $$ = std::move($1);
      $$->push_back(std::move($[goal-def]));
  }
;
objects-def:
    "(" OBJECTS typed-name-list[objects-list] ")" {
      $$ = std::make_unique<ast::Element>(ast::ObjectsDef{@$, std::move($[objects-list])});
    }
;
init-def:
    "(" INIT init-list ")" {
      $$ = std::make_unique<ast::Element>(ast::InitDef{@$, std::move($[init-list])});
    }
;
init-list:
    %empty {
      $$ = std::make_unique<ast::InitList>(@$);
    }
  | init-list "("[start] NAME empty-or-name-list[argument-list] ")"[end] {
      $$ = std::move($1);
      auto predicate = ast::InitPredicate{@[start] + @[end], std::make_unique<ast::Name>(@[NAME], $[NAME]), std::move($[argument-list])};
      $$->add(@[start] + @[end], std::make_unique<ast::InitCondition>(std::move(predicate)));
    }
  | init-list "("[start] "not" "("[inner-start] NAME empty-or-name-list[argument-list] ")"[inner-end] ")"[end] {
      $$ = std::move($1);
      auto predicate = std::make_unique<ast::InitPredicate>(@[inner-start] + @[inner-end], std::make_unique<ast::Name>(@[NAME], $[NAME]), std::move($[argument-list]));
      $$->add(@[start] + @[end], std::make_unique<ast::InitCondition>(ast::InitNegation{@[start] + @[end], std::move(predicate)}));
    }
;
goal-def:
    "(" GOAL "(" precondition-body[goal] ")" ")" {
      $$ = std::make_unique<ast::Element>(ast::GoalDef{@$, std::move($[goal])});
    }
;
argument-list:
    %empty {
      $$ = std::make_unique<ast::ArgumentList>(@$);
    }
  | argument-list NAME {
      $$ = std::move($1);
      $$->add(@[NAME], std::make_unique<ast::Argument>(ast::Name{@[NAME], $[NAME]}));
    }
  | argument-list VARIABLE {
      $$ = std::move($1);
      $$->add(@[VARIABLE], std::make_unique<ast::Argument>(ast::Variable{@[VARIABLE], $[VARIABLE]}));
    }
;
typed-name-list:
    single-typed-name-lists[lists] {
      $$ = std::make_unique<ast::TypedNameList>(@$, std::move($[lists]));
    }
  | single-typed-name-lists[lists] name-list {
      auto list = std::make_unique<ast::TypedNameList::value_type>(@[name-list], std::move($[name-list]));
      $[lists]->push_back(std::move(list));
      $$ = std::make_unique<ast::TypedNameList>(@$, std::move($[lists]));
    }
;
typed-var-list:
    single-typed-variable-lists[lists] {
      $$ = std::make_unique<ast::TypedVariableList>(@$, std::move($[lists]));
    }
  | single-typed-variable-lists[lists] var-list {
      auto list = std::make_unique<ast::TypedVariableList::value_type>(@[var-list], std::move($[var-list]));
      $[lists]->push_back(std::move(list));
      $$ = std::make_unique<ast::TypedVariableList>(@$, std::move($[lists]));
    }
;
single-typed-name-lists:
    %empty{
      $$ = std::make_unique<std::vector<std::unique_ptr<ast::TypedNameList::value_type>>>();
    }
  | single-typed-name-lists name-list "-" NAME[type] {
      auto type = ast::Name{@[type], $[type]};
      auto list = std::make_unique<ast::TypedNameList::value_type>(@[name-list] + @[type], std::move($[name-list]), type);
      $$ = std::move($1);
      $$->push_back(std::move(list));
    }
;
single-typed-variable-lists:
    %empty{
      $$ = std::make_unique<std::vector<std::unique_ptr<ast::TypedVariableList::value_type>>>();
    }
  | single-typed-variable-lists var-list "-" NAME[type] {
      auto type = ast::Name{@[type], $[type]};
      auto list = std::make_unique<ast::TypedVariableList::value_type>(@[var-list] + @[type], std::move($[var-list]), type);
      $$ = std::move($1);
      $$->push_back(std::move(list));
    }
;
empty-or-name-list:
    %empty {
      $$ = std::make_unique<ast::NameList>(@$);
    }
  | name-list {
      $$ = std::move($1);
    }
;
name-list:
    NAME {
      $$ = std::make_unique<ast::NameList>(@$);
      $$->add(@[NAME], std::make_unique<ast::Name>(@[NAME], $[NAME]));
    }
  | name-list NAME {
      $$ = std::move($1);
      $$->add(@[NAME], std::make_unique<ast::Name>(@[NAME], $[NAME]));
    }
;
var-list:
    VARIABLE {
      $$ = std::make_unique<ast::VariableList>(@$);
      $$->add(@[VARIABLE], std::make_unique<ast::Variable>(@[VARIABLE], $[VARIABLE]));
    }
  | var-list VARIABLE {
      $$ = std::move($1);
      $$->add(@[VARIABLE], std::make_unique<ast::Variable>(@[VARIABLE], $[VARIABLE]));
    }
;
%%

void parser::Parser::error (const location& loc, const std::string& msg)
{
  std::cerr << loc << ": " << msg << '\n';
}

