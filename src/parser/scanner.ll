%{ /* -*- C++ -*- */
  #include "scanner.h"
  #include <string>
  #include "driver.h"
  #include "parser.hxx"
%}

%{
  #define YY_USER_ACTION loc.columns(YYLeng());
%}

%option c++ batch noinput nounput noyywrap

%s domain
%s problem

BLANK [ \t\r]
COMMENT ;.*
NAME [[:alpha:]][[:alnum:]_\-]*

%%
%{
  parser::Parser::location_type &loc = driver.get_location();
  loc.step();
%}

{BLANK}+                loc.step();
{COMMENT}               loc.step();
\n+                     loc.lines(YYLeng()); loc.step();

"("                     return parser::Parser::make_LPAREN(loc);
")"                     return parser::Parser::make_RPAREN(loc);
"-"                     return parser::Parser::make_HYPHEN(loc);
"="                     return parser::Parser::make_EQUALITY(loc);
(?i:"and")              return parser::Parser::make_AND(loc);
(?i:"or")               return parser::Parser::make_OR(loc);
(?i:"not")              return parser::Parser::make_NOT(loc);
"define"                return parser::Parser::make_DEFINE(loc);

<domain>"domain"        return parser::Parser::make_DOMAIN(loc);
<domain>":types"        return parser::Parser::make_TYPES(loc);
<domain>":constants"    return parser::Parser::make_CONSTANTS(loc);
<domain>":predicates"   return parser::Parser::make_PREDICATES(loc);
<domain>":action"       return parser::Parser::make_ACTION(loc);
<domain>":parameters"   return parser::Parser::make_PARAMETERS(loc);
<domain>":precondition" return parser::Parser::make_PRECONDITION(loc);
<domain>":effect"       return parser::Parser::make_EFFECT(loc);

<problem>"problem"      return parser::Parser::make_PROBLEM(loc);
<problem>":domain"      return parser::Parser::make_DOMAIN_REF(loc);
<problem>":objects"     return parser::Parser::make_OBJECTS(loc);
<problem>":init"        return parser::Parser::make_INIT(loc);
<problem>":goal"        return parser::Parser::make_GOAL(loc);

":requirements"         return parser::Parser::make_REQUIREMENTS(loc);
{NAME}                  return parser::Parser::make_NAME(YYText(), loc);
\?{NAME}                return parser::Parser::make_VARIABLE(YYText(), loc);
:{NAME}                 return parser::Parser::make_KEYWORD(YYText(), loc);

.                       {
                          throw parser::Parser::syntax_error(
                          loc, "invalid character: " + std::string(YYText()));
                        }

<domain><<EOF>>         {
                          driver.switch_streams();
                          BEGIN(problem);
                        }
<<EOF>>                 {
                          return parser::Parser::make_END(loc);
                        }

%%

parser::Scanner::Scanner(std::istream* stream) : yyFlexLexer{stream} {
  BEGIN(domain);
}

int yyFlexLexer::yylex() {
    throw std::logic_error(
        "This function must never be called");
}
