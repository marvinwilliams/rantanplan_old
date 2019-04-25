%require "3.3"
%language "c++"
%skeleton "lalr1.cc"

%code requires {
  //class parser::Driver;
}

%code top {
  #ifdef TRACE
  #define YYDEBUG 1
  #endif
  #include "driver.h"
  #include "scanner.h"
  #undef yylex
  #define yylex driver.scanner->lex
}

%output  "parser.cxx"
%defines "parser.hxx"
%locations
%define api.location.file "location.hxx"

%define api.value.type variant
  //%define api.value.automove
%define api.token.constructor
%define parse.assert
%define parse.error verbose
%define api.token.prefix {TOK_}
%define api.namespace {parser}
%define api.parser.class {Parser}

%parse-param {Driver& driver}

%token
DEFINE        "define"
LPAREN        "("
RPAREN        ")"
AND           "and"
NOT           "not"
HYPHEN        "-"

END 0         "end of file"

%%
%start unit;
unit:
    LPAREN RPAREN {std::cout << "Parsing complete";}
;

%%

void parser::Parser::error (const location_type& l, const std::string& m)
{
  std::cout << "test";
  throw syntax_error(l, m);
}
