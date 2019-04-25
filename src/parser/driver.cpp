#include "driver.h"
#include <exception>
#include <fstream>
#include "parser.hxx"
#include "scanner.h"

namespace parser {

Driver::Driver(const std::string& domain_file, const std::string& problem_file)
    : domain_file{domain_file}, problem_file{problem_file} {}

int Driver::parse() {
  Scanner s{*this};
  this->scanner = &s;
  Parser parser{*this};
  return parser.parse();
}

}  // namespace parser
