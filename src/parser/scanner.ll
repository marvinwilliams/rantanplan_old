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

<domain><<EOF>> {
                  read_problem_();
                  BEGIN(problem);
                }
<<EOF>>         {
                  return parser::Parser::make_END(*loc);
                }

%%

parser::Scanner::Scanner(std::istream* stream, std::function<void(Scanner &>) read_problem) : yyFlexLexer{stream}, read_problem_{read_problem} {
  BEGIN(domain);
}

int yyFlexLexer::yylex() {
    throw std::logic_error(
        "This function must never be called");
}
