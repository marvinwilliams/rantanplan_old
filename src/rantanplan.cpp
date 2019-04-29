#include "config.h"
#include "model/builder.h"
#include "parser/driver.h"
#include "parser/parser.hxx"
#include "parser/scanner.h"
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "This is rantanplan version " << VERSION_MAJOR << "."
            << VERSION_MINOR << '\n';
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " DOMAIN PROBLEM "
              << "[OPTION]..." << '\n';
    return 1;
  }
  std::string domain_in{argv[1]};
  std::string problem_in{argv[2]};
  parser::Driver<parser::Scanner, parser::Parser> driver;
  try {
    driver.parse(domain_in, problem_in);
  } catch (const std::system_error &e) {
    std::cerr << e.what() << '\n';
    return 1;
  } catch (const parser::Parser::syntax_error &e) {
    std::cerr << e.location << ": " << e.what() << '\n';
    return 1;
  } catch (const model::Builder::semantic_error &e) {
    std::cerr << e.location << ": " << e.what() << '\n';
    return 1;
  }
  return 0;
}
