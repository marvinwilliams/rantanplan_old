%{ /* -*- C++ -*- */
  #include "scanner.h"
  #include "parser.hxx"
  #include <string>
%}

%{
  #define YY_USER_ACTION loc_.columns(YYLeng());
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
  loc_.step();
%}

{BLANK}+                loc_.step();
{COMMENT}               loc_.step();
\n+                     loc_.lines(YYLeng()); loc_.step();

"("                     return parser::Parser::make_LPAREN(loc_);
")"                     return parser::Parser::make_RPAREN(loc_);
"-"                     return parser::Parser::make_HYPHEN(loc_);
"="                     return parser::Parser::make_EQUALITY(loc_);
(?i:"and")              return parser::Parser::make_AND(loc_);
(?i:"or")               return parser::Parser::make_OR(loc_);
(?i:"not")              return parser::Parser::make_NOT(loc_);
"define"                return parser::Parser::make_DEFINE(loc_);

<domain>"domain"        return parser::Parser::make_DOMAIN(loc_);
<domain>":types"        return parser::Parser::make_TYPES(loc_);
<domain>":constants"    return parser::Parser::make_CONSTANTS(loc_);
<domain>":predicates"   return parser::Parser::make_PREDICATES(loc_);
<domain>":action"       return parser::Parser::make_ACTION(loc_);
<domain>":parameters"   return parser::Parser::make_PARAMETERS(loc_);
<domain>":precondition" return parser::Parser::make_PRECONDITION(loc_);
<domain>":effect"       return parser::Parser::make_EFFECT(loc_);

<problem>"problem"      return parser::Parser::make_PROBLEM(loc_);
<problem>":domain"      return parser::Parser::make_DOMAIN_REF(loc_);
<problem>":objects"     return parser::Parser::make_OBJECTS(loc_);
<problem>":init"        return parser::Parser::make_INIT(loc_);
<problem>":goal"        return parser::Parser::make_GOAL(loc_);

":requirements"         return parser::Parser::make_REQUIREMENTS(loc_);
{NAME}                  return parser::Parser::make_NAME(YYText(), loc_);
\?{NAME}                return parser::Parser::make_VARIABLE(YYText(), loc_);
:{NAME}                 return parser::Parser::make_KEYWORD(YYText(), loc_);

<switch_stream><<EOF>>  {
                          loc_.initialize(problem_file_);
                          switch_streams(problem_stream_);
                          BEGIN(problem);
                        }
<switch_stream>.        {
                          throw parser::Parser::syntax_error(
                            loc_, "expected end of line, found " + std::string(YYText()));
                        }
<<EOF>>                 {
                          return parser::Parser::make_END(loc_);
                        }

.                       {
                          throw parser::Parser::syntax_error(
                            loc_, "invalid character: " + std::string(YYText()));
                        }

%%

parser::Scanner::Scanner(std::string *domain_file,
                         std::string *problem_file,
                         std::istream *domain_stream,
                         std::istream *problem_stream)
    : yyFlexLexer{domain_stream}, domain_file_{domain_file}, problem_file_{problem_file},
      domain_stream_{domain_stream}, problem_stream_{problem_stream} {
  loc_.initialize(domain_file_);
  BEGIN(domain);
}

void parser::Scanner::domain_end() {
  BEGIN(switch_stream);
}

int yyFlexLexer::yylex() {
  throw std::logic_error("This function must never be called");
}
