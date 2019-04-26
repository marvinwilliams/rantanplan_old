#include "driver.h"
#include "parser.hxx"
#include "scanner.h"
#include <exception>
#include <fstream>

namespace parser {

Driver::Driver() {}

int Driver::parse(std::string domain_in, std::string problem_in) {
  std::ifstream domain_stream{domain_in};
  if (!domain_stream) {
    throw std::system_error(errno, std::system_category(),
                            "failed to open " + domain_in);
  }
  std::ifstream problem_stream{problem_in};
  if (!problem_stream) {
    throw std::system_error(errno, std::system_category(),
                            "failed to open " + problem_in);
  }
  location loc;
  loc.initialize(&domain_in);
  auto read_problem = [&loc, &problem_in, &problem_stream](Scanner& s) {
    loc.initialize(&problem_in);
    s.switch_streams(&problem_stream);
  };
  Scanner s{&domain_stream, read_problem};
  scanner = &s;
  Parser parser{*this};
  int result = parser.parse();
  scanner = nullptr;
  return result;
}

} // namespace parser
