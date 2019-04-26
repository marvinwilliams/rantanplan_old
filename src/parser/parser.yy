%require "3.3"
%language "c++"
%skeleton "lalr1.cc"

%code requires {
  namespace parser {
    class Driver;
  }
}

%code top {
  #include <iostream>
  #include "driver.h"
  #include "scanner.h"
  #ifdef TRACE
  #define YYDEBUG 1
  #endif
  #undef yylex
  #define yylex driver.scanner->lex
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
    LPAREN RPAREN {std::cout << "Parsing complete\n";}
;
%%

void parser::Parser::error (const location_type& l, const std::string& m)
{
  throw syntax_error(l, m);
}
