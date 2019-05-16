#ifndef SCANNER_H
#define SCANNER_H

#undef yyFlexLexer
#include "parser.hxx"
#include "location.hxx"
#include <FlexLexer.h>
#include <fstream>
#include <string>
#define YY_DECL parser::Parser::symbol_type parser::Scanner::lex()

namespace parser {

class Scanner : public yyFlexLexer {
public:
  Scanner(const std::string &domain_file, const std::string &problem_file,
          std::istream *domain_stream, std::istream *problem_stream);

  Parser::symbol_type lex();

  void domain_end();

private:
  std::string domain_file_;
  std::string problem_file_;
  std::istream *domain_stream_;
  std::istream *problem_stream_;

  location loc_;
};

} // namespace parser
#endif /* end of include guard: SCANNER_H */
