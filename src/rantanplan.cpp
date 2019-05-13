#include "config.h"
#include "parser/driver.h"
#include "parser/parser.hxx"
#include "parser/scanner.h"
#include "parser/visitor.h"
#include <iostream>

namespace parser {
namespace ast {
class MyVisitor : public Visitor<MyVisitor> {
public:
  using Visitor<MyVisitor>::traverse;
  using Visitor<MyVisitor>::visit;

  bool visit(const Domain& domain) {
    std::cout << "Domain: " << domain.name << std::endl;
    return true;
  }

  bool visit(const Requirement &req) {
    std::cout << "Found requirement: " << req.name << std::endl;
    return true;
  }
};
} // namespace ast
} // namespace parser

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
  parser::Driver driver;

  auto result = driver.parse(domain_in, problem_in);

  parser::ast::MyVisitor v;
  v.traverse(*driver.ast);
  return result;
}
