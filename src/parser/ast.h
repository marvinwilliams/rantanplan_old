#ifndef AST_H
#define AST_H

#include "location.hxx"
#include <memory>
#include <variant>
#include <vector>

namespace parser {

namespace ast {

struct Node {
  Node(const location &loc) : loc{loc} {}

  location loc;
  virtual ~Node() {}
};

struct Requirement : Node {
  Requirement(const location &loc, const std::string &name)
      : Node{loc}, name{name} {}

  std::string name;
};

struct RequirementsDef : Node {
  RequirementsDef(const location &loc,
                  std::vector<std::unique_ptr<Requirement>> &&requirements)
      : Node{loc}, requirements{std::move(requirements)} {}

  std::vector<std::unique_ptr<Requirement>> requirements;
};

struct Type : Node {
  Type(const location &loc, const std::string &name) : Node{loc}, name{name} {}

  std::string name;
};

struct TypeList : Node {
  TypeList(const location &loc) : Node{loc} {}

  std::vector<std::unique_ptr<Type>> types;
  std::unique_ptr<Type> supertype;
};

struct TypesDef : Node {
  TypesDef(const location &loc) : Node{loc} {}

  std::vector<std::unique_ptr<TypeList>> type_lists;
};

struct Constant : Node {
  Constant(const location &loc) : Node{loc} {}

  std::string name;
};

struct ConstantList : Node {
  ConstantList(const location &loc) : Node{loc} {}

  std::vector<std::unique_ptr<Constant>> constants;
  std::unique_ptr<Type> type;
};

struct ConstantsDef : Node {
  ConstantsDef(const location &loc) : Node{loc} {}

  std::vector<std::unique_ptr<ConstantList>> constant_lists;
};

struct Parameter : Node {
  Parameter(const location &loc) : Node{loc} {}

  std::string name;
};

struct ParameterList : Node {
  ParameterList(const location &loc) : Node{loc} {}

  std::vector<std::unique_ptr<Parameter>> parameters;
  std::unique_ptr<Type> type;
};

struct Predicate : Node {
  Predicate(const location &loc) : Node{loc} {}

  std::string name;
  std::vector<std::unique_ptr<ParameterList>> parameters;
};

struct PredicatesDef : Node {
  PredicatesDef(const location &loc) : Node{loc} {}

  std::vector<std::unique_ptr<Predicate>> predicates;
};

struct Argument : Node {
  Argument(const location &loc) : Node{loc} {}

  std::string name;
};

struct PredicateEvaluation : Node {
  PredicateEvaluation(const location &loc) : Node{loc} {}

  std::string name;
  std::vector<std::unique_ptr<Argument>> arguments;
};

struct Condition;

struct Conjunction : Node {
  Conjunction(const location &loc) : Node{loc} {}

  std::vector<std::unique_ptr<Condition>> conditions;
};

struct Disjunction : Node {
  Disjunction(const location &loc) : Node{loc} {}

  std::vector<std::unique_ptr<Condition>> conditions;
};

struct Negation : Node {
  Negation(const location &loc) : Node{loc} {}

  std::unique_ptr<Condition> condition;
};

struct Condition : Node {
  Condition(const location &loc) : Node{loc} {}

  std::variant<std::unique_ptr<PredicateEvaluation>,
               std::unique_ptr<Conjunction>, std::unique_ptr<Disjunction>,
               std::unique_ptr<Negation>>
      condition;
};

struct ActionDef : Node {
  ActionDef(const location &loc) : Node{loc} {}

  std::string name;
  std::vector<std::unique_ptr<ParameterList>> parameters;
  std::unique_ptr<Condition> precondition;
  std::unique_ptr<Condition> effect;
};

struct Domain : Node {
  using Element = std::variant<RequirementsDef, TypesDef, ConstantsDef,
                               PredicatesDef, ActionDef>;
  Domain(const location &loc) : Node{loc} {}

  std::string name;
  std::vector<std::unique_ptr<Element>> domain_body;
};

class AST {
public:
  AST(const AST &) = delete;
  AST(AST &&other)
      : domain_file{other.domain_file},
        problem_file{other.problem_file}, domain{std::move(other.domain)} {}

  AST(const std::string &domain_file, const std::string &problem_file)
      : domain_file{domain_file}, problem_file{problem_file} {}

  void set_domain(std::unique_ptr<Domain> &&d) { domain = std::move(d); }

  std::string domain_file;
  std::string problem_file;

  std::unique_ptr<Domain> domain;
};

} // namespace ast

} // namespace parser

#endif /* end of include guard: AST_H */
