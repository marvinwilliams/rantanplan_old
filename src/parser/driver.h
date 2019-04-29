#ifndef DRIVER_H
#define DRIVER_H

#include <exception>
#include <fstream>
#include <memory>
#include <string>

namespace parser {

template <typename Scanner, typename Parser> class Driver {
public:
  using location_type = typename Parser::location_type;
  using symbol_type = typename Parser::symbol_type;

  Driver() {}

  int parse(std::string& domain_in, std::string& problem_in) {
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
    location_type location;
    location.initialize(&domain_in);
    Scanner scanner{&domain_stream};
    Parser parser{*this};
    Context ctx{scanner,       parser,         domain_in, problem_in,
                domain_stream, problem_stream, location};
    context = &ctx;
    int result = parser.parse();
    context = nullptr;
    return result;
  }

  void switch_streams() {
    context->location.initialize(&context->problem_in);
    context->scanner.switch_streams(&context->problem_stream);
  }

  symbol_type lex() {
    return context->scanner.lex(*this);
  }

  location_type &get_location() { return context->location; }

private:
  struct Context {
    Scanner &scanner;
    Parser &parser;
    std::string &domain_in;
    std::string &problem_in;
    std::ifstream &domain_stream;
    std::ifstream &problem_stream;
    location_type &location;
  };

  Context *context;
};

} // namespace parser

#endif /* end of include guard: DRIVER_H */
