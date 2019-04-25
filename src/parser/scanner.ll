%{ /* -*- C++ -*- */
  #include "scanner.h"
  #include <string>
  #include "driver.h"
  #include "parser.hxx"
%}

%{
  #define YY_USER_ACTION loc->columns(yyleng);
%}

%option c++ batch noinput nounput noyywrap
%option outfile="scanner.cxx"

%s domain
%s problem

NAME [[:alpha:]][[:alnum:]_\-]*
REQUIREMENT :[[:alpha:]][[:alpha:]\-]*
COMMENT ;.*
BLANK [ \t\r]

%%
%{
  loc->step();
%}
{BLANK}+        loc->step();
{COMMENT}       loc->step();
\n+             loc->lines(yyleng); loc->step();
"define"        return parser::Parser::make_DEFINE(*loc);
"("             return parser::Parser::make_LPAREN(*loc);
")"             return parser::Parser::make_RPAREN(*loc);
(?i:"and")      return parser::Parser::make_AND(*loc);
(?i:"not")      return parser::Parser::make_NOT(*loc);
"-"             return parser::Parser::make_HYPHEN(*loc);

.               {
                  throw parser::Parser::syntax_error(
                  *loc, "invalid character: " + std::string(yytext));
                }

%{
//<domain><<EOF>> {
//                  lex_problem();
//                }
%}
<<EOF>>         {
                  return parser::Parser::make_END(*loc);
                }

%%

parser::Scanner::Scanner(Driver& driver) : loc{&driver.loc}, domain_file{&driver.domain_file}, problem_file{&driver.problem_file} {
  lex_domain();
}

void parser::Scanner::lex_domain() {
  current_stream.open(*domain_file);
  if (!current_stream) {
    throw std::system_error(errno, std::system_category(),
                            "failed to open " + *domain_file);
  }
  loc->initialize(domain_file);
  switch_streams(&current_stream);
  BEGIN(domain);
}

void parser::Scanner::lex_problem() {
  current_stream.close();
  current_stream.open(*problem_file);
  if (!current_stream) {
    throw std::system_error(errno, std::system_category(),
                            "failed to open " + *problem_file);
  }
  loc->initialize(problem_file);
  switch_streams(&current_stream);
  BEGIN(problem);
}

int yyFlexLexer::yylex() {
    throw std::logic_error(
        "This function must never be called");
}
