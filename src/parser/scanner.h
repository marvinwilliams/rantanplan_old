#ifndef SCANNER_H
#define SCANNER_H

#undef yyFlexLexer
#include "driver.h"
#include "parser.hxx"
#include <FlexLexer.h>
#include <fstream>
#include <functional>
#include <string>
#define YY_DECL parser::Parser::symbol_type parser::Scanner::lex()

namespace parser {

class Scanner : public yyFlexLexer {
public:
  Scanner(std::ifstream *stream, std::function<void(Scanner &)> read_problem);

  Parser::symbol_type lex();

private:
  std::function<void(Scanner &)> read_problem_;
  location *loc;
};

} // namespace parser
#endif /* end of include guard: SCANNER_H */
