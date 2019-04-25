#ifndef SCANNER_H
#define SCANNER_H

#undef yyFlexLexer
#include <FlexLexer.h>
#include <fstream>
#include <string>
#include "driver.h"
#include "location.hxx"
#include "parser.hxx"
#define YY_DECL parser::Parser::symbol_type parser::Scanner::lex()

namespace parser {
class Scanner : public yyFlexLexer {
 public:
  Scanner(Driver& driver);

  Parser::symbol_type lex();
  void lex_domain();
  void lex_problem();

  private:
  std::ifstream current_stream;
  location* loc;
  std::string* domain_file;
  std::string* problem_file;
};

}  // namespace parser
#endif /* end of include guard: SCANNER_H */
