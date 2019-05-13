%{ /* -*- C++ -*- */
  #include "scanner.h"
  #include "parser.hxx"
  #include <string>
%}

%{
  #define YY_USER_ACTION location_.columns(YYLeng());
%}

%option c++ batch noinput nounput noyywrap

%s domain
%s problem
%x switch_stream

BLANK [ \t\r]
COMMENT ;.*
NAME [[:alpha:]][[:alnum:]_\-]*

%%
%{
  location_.step();
%}

{BLANK}+                location_.step();
{COMMENT}               location_.step();
\n+                     location_.lines(YYLeng()); location_.step();

"("                     return parser::Parser::make_LPAREN(location_);
")"                     return parser::Parser::make_RPAREN(location_);
"-"                     return parser::Parser::make_HYPHEN(location_);
"="                     return parser::Parser::make_EQUALITY(location_);
(?i:"and")              return parser::Parser::make_AND(location_);
(?i:"or")               return parser::Parser::make_OR(location_);
(?i:"not")              return parser::Parser::make_NOT(location_);
"define"                return parser::Parser::make_DEFINE(location_);

<domain>"domain"        return parser::Parser::make_DOMAIN(location_);
<domain>":types"        return parser::Parser::make_TYPES(location_);
<domain>":constants"    return parser::Parser::make_CONSTANTS(location_);
<domain>":predicates"   return parser::Parser::make_PREDICATES(location_);
<domain>":action"       return parser::Parser::make_ACTION(location_);
<domain>":parameters"   return parser::Parser::make_PARAMETERS(location_);
<domain>":precondition" return parser::Parser::make_PRECONDITION(location_);
<domain>":effect"       return parser::Parser::make_EFFECT(location_);

<problem>"problem"      return parser::Parser::make_PROBLEM(location_);
<problem>":domain"      return parser::Parser::make_DOMAIN_REF(location_);
<problem>":objects"     return parser::Parser::make_OBJECTS(location_);
<problem>":init"        return parser::Parser::make_INIT(location_);
<problem>":goal"        return parser::Parser::make_GOAL(location_);

":requirements"         return parser::Parser::make_REQUIREMENTS(location_);
{NAME}                  return parser::Parser::make_NAME(YYText(), location_);
\?{NAME}                return parser::Parser::make_VARIABLE(YYText(), location_);
:{NAME}                 return parser::Parser::make_KEYWORD(YYText(), location_);

<switch_stream><<EOF>>  {
                          location_.initialize(&problem_file_);
                          switch_streams(problem_stream_);
                          BEGIN(problem);
                        }
<switch_stream>.        {
                          throw parser::Parser::syntax_error(
                            location_, "expected end of line, found " + std::string(YYText()));
                        }
<<EOF>>                 {
                          return parser::Parser::make_END(location_);
                        }

.                       {
                          throw parser::Parser::syntax_error(
                            location_, "invalid character: " + std::string(YYText()));
                        }

%%

parser::Scanner::Scanner(const std::string &domain_file,
                         const std::string &problem_file,
                         std::istream *domain_stream,
                         std::istream *problem_stream)
    : yyFlexLexer{domain_stream}, domain_file_{domain_file}, problem_file_{problem_file},
      domain_stream_{domain_stream}, problem_stream_{problem_stream} {
  location_.initialize(&domain_file_);
  BEGIN(domain);
}

void parser::Scanner::domain_end() {
  BEGIN(switch_stream);
}

int yyFlexLexer::yylex() {
  throw std::logic_error("This function must never be called");
}
