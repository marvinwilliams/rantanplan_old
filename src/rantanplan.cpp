#include "config.h"
#include "parser/driver.h"
#include "parser/parser.hxx"
#include "parser/scanner.h"
#include "parser/visitor_variant.h"
#include <iostream>

using namespace parser::ast;
class MyVisitor : public parser::visitor::Visitor<MyVisitor> {
public:
  using Visitor<MyVisitor>::traverse;
  using Visitor<MyVisitor>::visit_begin;
  using Visitor<MyVisitor>::visit_end;

  bool visit_begin(const Domain &a) {
    std::cout << "Domain: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const DomainElement &a) {
    std::cout << "DomainElement!" << '\n';
    return true;
  }
  bool visit_begin(const RequirementsDef &a) {
    std::cout << "RequirementsDef: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const TypesDef &a) {
    std::cout << "TypesDef: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const ConstantsDef &a) {
    std::cout << "ConstantsDef: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const PredicatesDef &a) {
    std::cout << "PredicatesDef: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const ActionDef &a) {
    std::cout << "ActionDef: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const Precondition &a) {
    std::cout << "Precondition: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const Effect &a) {
    std::cout << "Effect: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const ConditionList &a) {
    std::cout << "ConditionList: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const PredicateList &a) {
    std::cout << "PredicateList: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const TypedNameList &a) {
    std::cout << "TypedNameList: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const TypedVariableList &a) {
    std::cout << "TypedVariableList: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const RequirementsList &a) {
    std::cout << "RequirementsList: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const ArgumentList &a) {
    std::cout << "ArgumentList: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const NameList &a) {
    std::cout << "NameList: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const VariableList &a) {
    std::cout << "VariableList: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const Predicate &a) {
    std::cout << "Predicate: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const Condition &a) { std::cout << "Condition!" << '\n'; }
  bool visit_begin(const PredicateEvaluation &a) {
    return true;
    std::cout << "PredicateEvaluation: " << a.loc << '\n';
  }
  bool visit_begin(const Conjunction &a) {
    return true;
    std::cout << "Conjunction: " << a.loc << '\n';
  }
  bool visit_begin(const Disjunction &a) {
    return true;
    std::cout << "Disjunction: " << a.loc << '\n';
  }
  bool visit_begin(const Negation &a) {
    return true;
    std::cout << "Negation: " << a.loc << '\n';
  }
  bool visit_begin(const Requirement &a) {
    return true;
    std::cout << "Requirement: " << a.loc << '\n';
  }
  bool visit_begin(const Argument &a) {
    std::cout << "Argument!" << '\n';
    return true;
  }
  bool visit_begin(const Name &a) {
    std::cout << "Name: " << a.loc << '\n';
    return true;
  }
  bool visit_begin(const Variable &a) {
    std::cout << "Variable: " << a.loc << '\n';
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
