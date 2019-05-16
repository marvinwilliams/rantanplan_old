#include "config.h"
#include "parser/driver.h"
#include "parser/parser.hxx"
#include "parser/scanner.h"
#include "parser/visitor.h"
#include <iostream>

class MyVisitor : public parser::ast::Visitor<MyVisitor> {
public:
  using Visitor<MyVisitor>::traverse;
  using Visitor<MyVisitor>::visit;

  bool visit(const parser::ast::Type& type) {
    std::cout << "Type " << type.name << " at " << type.loc << std::endl;
    return true;
  }

  bool visit(const parser::ast::TypesDef&) {
    std::cout << "TypesDef!!" << std::endl;
    return true;
  }

  bool visit(const parser::ast::Domain& domain) {
    std::cout << "Domain: " << domain.name << std::endl;
    return true;
  }

  bool visit(const parser::ast::Requirement &req) {
    std::cout << "Found requirement: " << req.name << " at " << req.loc.begin.column <<  std::endl;
    return true;
  }
};

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

  MyVisitor v;
  v.traverse(*driver.ast);
  return result;
}
