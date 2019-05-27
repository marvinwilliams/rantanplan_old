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

  bool visit(const parser::ast::AST &) { return true; }
  bool visit(const parser::ast::Domain &Domain) {
    std::cout << "Domain at " << Domain.loc << std::endl;
    return true;
  }
  bool visit(const parser::ast::DomainElement &DomainElement) {
    std::cout << "DomainElement at " << DomainElement.loc << std::endl;
    return true;
  }
  bool visit(const parser::ast::Requirement &Requirement) {
    std::cout << "Requirement at " << Requirement.loc << std::endl;
    return true;
  }
  bool visit(const parser::ast::RequirementsDef &RequirementsDef) {
    std::cout << "RequirementsDef at " << RequirementsDef.loc << std::endl;
    return true;
  }
  bool visit(const parser::ast::Type &Type) {
    std::cout << "Type at " << Type.loc << std::endl;
    return true;
  }
  bool visit(const parser::ast::TypeList &TypeList) {
    std::cout << "TypeList at " << TypeList.loc << std::endl;
    return true;
  }
  bool visit(const parser::ast::TypesDef &TypesDef) {
    std::cout << "TypesDef at " << TypesDef.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::Constant &Constant) {
    std::cout << "Constant at " << Constant.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::ConstantList &ConstantList) {
    std::cout << "ConstantList at " << ConstantList.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::ConstantsDef &ConstantsDef) {
    std::cout << "ConstantsDef at " << ConstantsDef.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::Predicate &Predicate) {
    std::cout << "Predicate at " << Predicate.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::Parameter &Parameter) {
    std::cout << "Parameter at " << Parameter.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::ParameterList &ParameterList) {
    std::cout << "ParameterList at " << ParameterList.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::PredicatesDef &PredicatesDef) {
    std::cout << "PredicatesDef at " << PredicatesDef.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::Condition &Condition) {
    std::cout << "Condition at " << Condition.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::Argument &Argument) {
    std::cout << "Argument at " << Argument.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::PredicateEvaluation &PredicateEvaluation) {
    std::cout << "PredicateEvaluation at " << PredicateEvaluation.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::CompoundCondition &CompoundCondition) {
    std::cout << "CompoundCondition at " << CompoundCondition.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::Conjunction &Conjunction) {
    std::cout << "Conjunction at " << Conjunction.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::Disjunction &Disjunction) {
    std::cout << "Disjunction at " << Disjunction.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::Negation &Negation) {
    std::cout << "Negation at " << Negation.loc << '\n';
    return true;
  }
  bool visit(const parser::ast::ActionDef &ActionDef) {
    std::cout << "ActionDef at " << ActionDef.loc << '\n';
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

  auto result = driver.parse(&domain_in, &problem_in);

  MyVisitor v;
  v.traverse(*driver.ast);
  return result;
}
