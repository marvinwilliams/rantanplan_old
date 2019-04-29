#ifndef SCANNER_H
#define SCANNER_H

#undef yyFlexLexer
#include "driver.h"
#include "parser.hxx"
#include <FlexLexer.h>
#include <fstream>
#include <functional>
#include <string>
#define YY_DECL                                                                \
  parser::Parser::symbol_type parser::Scanner::lex(                            \
      Driver<parser::Scanner, parser::Parser> &driver)

namespace parser {

class Scanner : public yyFlexLexer {
public:
  Scanner(std::istream *stream);

  Parser::symbol_type lex(Driver<parser::Scanner, parser::Parser> &driver);
};

} // namespace parser
#endif /* end of include guard: SCANNER_H */
