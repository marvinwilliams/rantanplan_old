#ifndef DRIVER_H
#define DRIVER_H

#include "ast.h"
#include "parser.hxx"
#include "scanner.h"
#include <exception>
#include <fstream>
#include <memory>
#include <optional>
#include <string>

namespace parser {

std::optional<ast::AST> parse(std::string *domain_file,
                              std::string *problem_file) {
  std::ifstream domain_stream{*domain_file};
  if (!domain_stream) {
    throw std::system_error(errno, std::system_category(),
                            "failed to open " + *domain_file);
  }
  std::ifstream problem_stream{*problem_file};
  if (!problem_stream) {
    throw std::system_error(errno, std::system_category(),
                            "failed to open " + *problem_file);
  }
  Scanner scanner{domain_file, problem_file, &domain_stream, &problem_stream};
  ast::AST ast(*domain_file, *problem_file);
  Parser parser{scanner, ast};
  //   parser.set_debug_level(1);
  if (parser.parse() == 0) {
    return ast;
  } else {
    return std::nullopt;
  }
}

} // namespace parser

#endif /* end of include guard: DRIVER_H */
