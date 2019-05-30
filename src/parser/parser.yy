%require "3.4"
%language "c++"
%skeleton "lalr1.cc"

%code requires {
  #include <optional>
  #include "location.hxx"
  #include "ast_variant.h"

  namespace parser {

  class Scanner;

  struct StringList {
    location loc;
    std::vector<std::pair<location, std::string>> elements;
  };

  struct TypedList {
    location loc;
    StringList string_list;
    ast::Type type;
  };

  struct ListOfTypedLists {
    location loc;
    std::vector<TypedList> typed_lists;
    StringList untyped_list;
  };

  }
}

%code provides {
}

%code top {
  #include "parser.hxx"
  #include "location.hxx"
  #include "scanner.h"
  #include "ast_variant.h"
  #include <iostream>
  #undef yylex
  #define yylex scanner.lex

  namespace parser {

  template <typename ListType, typename ElemType>
  std::vector<std::unique_ptr<ListType>>
  convert_list_of_typed_lists(const ListOfTypedLists &list) {
    std::vector<std::unique_ptr<ListType>> converted_list;
    for (auto &typed_list : list.typed_lists) {
      std::vector<std::unique_ptr<ElemType>> inner_list;
      for (auto &elem : typed_list.string_list.elements) {
        inner_list.push_back(
            std::make_unique<ElemType>(ElemType(elem.first, elem.second)));
      }
      converted_list.push_back(std::make_unique<ListType>(
          typed_list.loc, std::move(inner_list),
          std::make_unique<ast::Type>(typed_list.type.loc, typed_list.type.name)));
    }
    std::vector<std::unique_ptr<ElemType>> untyped_list;
    for (auto &elem : list.untyped_list.elements) {
      untyped_list.push_back(
          std::make_unique<ElemType>(ElemType(elem.first, elem.second)));
    }
    converted_list.push_back(std::make_unique<ListType>(
        list.untyped_list.loc, std::move(untyped_list)));
    return converted_list;
  }

  }
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
<std::vector<ast::DomainElement>> domain-body
<std::unique_ptr<ast::RequirementsDef>> require-def
<std::vector<std::unique_ptr<ast::Requirement>>> require-list
<std::unique_ptr<ast::TypesDef>> types-def
<std::unique_ptr<ast::ConstantsDef>> constants-def
<std::unique_ptr<ast::PredicatesDef>> predicates-def
<std::vector<std::unique_ptr<ast::Predicate>>> predicate-list
<std::unique_ptr<ast::Predicate>> predicate-def
<std::unique_ptr<ast::ActionDef>> action-def
<std::pair<std::unique_ptr<ast::Condition>, std::unique_ptr<ast::Condition>>> action-body
<std::unique_ptr<ast::Condition>> precondition-def
<std::unique_ptr<ast::Condition>> precondition-body
<std::vector<std::unique_ptr<ast::Condition>>> precondition-list
<std::unique_ptr<ast::Condition>> effect-def
<std::unique_ptr<ast::Condition>> effect-body
<std::vector<std::unique_ptr<ast::Condition>>> effect-list
problem-def
problem-body
object-def
init-def
init-list
goal-def
<std::vector<std::unique_ptr<ast::Argument>>> param-list
<ListOfTypedLists> typed-name-list
<ListOfTypedLists> typed-var-list
<StringList> name-list
<StringList> var-list
;

%%
%start unit;
unit:
    domain-def {
      scanner.domain_end();
    }
    problem-def {
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
      $1.push_back(std::move($[types-def]));
      $$ = std::move($1);
    }
  | domain-body constants-def {
      $1.push_back(std::move($[constants-def]));
      $$ = std::move($1);
    }
  | domain-body predicates-def {
      $1.push_back(std::move($[predicates-def]));
      $$ = std::move($1);
    }
  | domain-body action-def {
      $1.push_back(std::move($[action-def]));
      $$ = std::move($1);
    }
;
require-def:
    "(" REQUIREMENTS require-list ")" {
      $$ = std::make_unique<ast::RequirementsDef>(@$, std::move($[require-list]));
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
    "(" TYPES typed-name-list ")" {
      auto type_lists = convert_list_of_typed_lists<ast::TypeList, ast::Type>($[typed-name-list]);
      $$ = std::make_unique<ast::TypesDef>(@$, std::move(type_lists));
    }
;
constants-def:
    "(" CONSTANTS typed-name-list ")" {
      auto constant_lists = convert_list_of_typed_lists<ast::ConstantList, ast::Constant>($[typed-name-list]);
      $$ = std::make_unique<ast::ConstantsDef>(@$, std::move(constant_lists));
    }
;
predicates-def:
    "(" PREDICATES predicate-list ")" {
      $$ = std::make_unique<ast::PredicatesDef>(@$, std::move($[predicate-list]));
    }
;
predicate-list:
    predicate-def {
      $$.push_back(std::move($[predicate-def]));
    }
  | predicate-list predicate-def {
      $1.push_back(std::move($[predicate-def]));
      $$ = std::move($1);
    }
;
predicate-def:
    "(" NAME typed-var-list ")" {
      auto parameter_list = convert_list_of_typed_lists<ast::ParameterList, ast::Parameter>($[typed-var-list]);
      $$ = std::make_unique<ast::Predicate>(@$, $[NAME], std::move(parameter_list));
    }
;
action-def:
    "(" ACTION NAME PARAMETERS "(" typed-var-list ")" action-body ")" {
      auto parameter_list = convert_list_of_typed_lists<ast::ParameterList, ast::Parameter>($[typed-var-list]);
      $$ = std::make_unique<ast::ActionDef>(@$, $[NAME], std::move(parameter_list), std::move($[action-body].first), std::move($[action-body].second));
    }
;
action-body:
  precondition-def effect-def {
    $$ = std::make_pair(std::move($[precondition-def]), std::move($[effect-def]));
  }
;
precondition-def:
    %empty {
      $$ = std::make_unique<ast::Condition>(@$);
    }
  | PRECONDITION "(" precondition-body ")" {
      $$ = std::move($[precondition-body]);
    }
;
precondition-body:
    %empty {
      $$ = std::make_unique<ast::Condition>(@$);
    }
  | NAME param-list {
      $$ = std::make_unique<ast::Condition>(@$, std::make_unique<ast::PredicateEvaluation>(@$, $[NAME], std::move($[param-list])));
    }
  | "=" param-list {
      $$ = std::make_unique<ast::Condition>(@$, std::make_unique<ast::PredicateEvaluation>(@$, "=", std::move($[param-list])));
    }
  | "and" precondition-list {
      $$ = std::make_unique<ast::Condition>(@$, std::make_unique<ast::Conjunction>(@$, std::move($[precondition-list])));
    }
  | "or" precondition-list {
      $$ = std::make_unique<ast::Condition>(@$, std::make_unique<ast::Disjunction>(@$, std::move($[precondition-list])));
    }
  | "not" "(" precondition-body[nested-body] ")" {
      $$ = std::make_unique<ast::Condition>(@$, std::make_unique<ast::Negation>(@$, std::move($[nested-body])));
    }
;
precondition-list:
    %empty {}
  | precondition-list "(" precondition-body ")" {
      $1.push_back(std::move($[precondition-body]));
      $$ = std::move($1);
    }
;
effect-def:
    %empty {
      $$ = std::make_unique<ast::Condition>(@$);
    }
  | EFFECT "(" effect-body ")" {
      $$ = std::move($[effect-body]);
    }
;
effect-body:
    %empty {
      $$ = std::make_unique<ast::Condition>(@$);
    }
  | NAME param-list {
      $$ = std::make_unique<ast::Condition>(@$, std::make_unique<ast::PredicateEvaluation>(@$, $[NAME], std::move($[param-list])));
    }
  | "and" effect-list {
      $$ = std::make_unique<ast::Condition>(@$, std::make_unique<ast::Conjunction>(@$, std::move($[effect-list])));
    }
  | "not" "(" NAME param-list ")" {
      auto condition = std::make_unique<ast::Condition>(@$, std::make_unique<ast::PredicateEvaluation>(@$, $[NAME], std::move($[param-list])));
      $$ = std::make_unique<ast::Condition>(@$, std::make_unique<ast::Negation>(@$, std::move(condition)));
    }
;
effect-list:
    %empty {}
  | effect-list "(" effect-body ")" {
      $1.push_back(std::move($[effect-body]));
      $$ = std::move($1);
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
param-list:
    %empty {}
  | param-list NAME {
      $1.push_back(std::make_unique<ast::Argument>(@[NAME], $[NAME]));
      $$ = std::move($1);
    }
  | param-list VARIABLE {
      $1.push_back(std::make_unique<ast::Argument>(@[VARIABLE], $[VARIABLE]));
      $$ = std::move($1);
    }
;
typed-name-list:
    name-list {
      $$.loc = @$;
      $$.untyped_list = std::move($[name-list]);
    }
  | name-list NAME[last-name] "-" NAME[type] typed-name-list[remaining-list] {
      $[name-list].elements.push_back(std::make_pair(@[last-name], $[last-name]));
      TypedList typed_list{@[name-list] + @[type], std::move($[name-list]), ast::Type(@[type], $[type])};
      $[remaining-list].typed_lists.push_back(std::move(typed_list));
      $$.loc = @$;
      $$.typed_lists = std::move($[remaining-list].typed_lists);
      $$.untyped_list = std::move($[remaining-list].untyped_list);
    }
;
typed-var-list:
    var-list {
      $$.loc = @$;
      $$.untyped_list = std::move($[var-list]);
    }
  | var-list VARIABLE[last-var] "-" NAME[type] typed-var-list[remaining-list] {
      $[var-list].elements.push_back(std::make_pair(@[last-var], $[last-var]));
      TypedList typed_list{@[var-list] + @[type], std::move($[var-list]), ast::Type(@[type], $[type])};
      $[remaining-list].typed_lists.push_back(std::move(typed_list));
      $$.loc = @$;
      $$.typed_lists = std::move($[remaining-list].typed_lists);
      $$.untyped_list = std::move($[remaining-list].untyped_list);
    }
;
name-list:
    %empty {}
  | name-list NAME {
      $$ = std::move($1);
      $$.loc = @$;
      $$.elements.push_back(std::make_pair(@[NAME], $[NAME]));
    }
;
var-list:
    %empty {}
  | var-list VARIABLE {
      $$ = std::move($1);
      $$.loc = @$;
      $$.elements.push_back(std::make_pair(@[VARIABLE], $[VARIABLE]));
    }
;
%%

void parser::Parser::error (const location& loc, const std::string& msg)
{
  std::cerr << loc << ": " << msg << '\n';
}

