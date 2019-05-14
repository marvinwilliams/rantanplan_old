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
  RequirementsDef(const location &loc, std::vector<Requirement> &&requirements)
      : Node{loc}, requirements{std::move(requirements)} {}

  std::vector<Requirement> requirements;
};

struct Type : Node {
  Type(const location &loc, const std::string &name) : Node{loc}, name{name} {}

  std::string name;
};

struct TypeList : Node {
  TypeList(const location &loc, std::vector<Type> &&types,
           std::unique_ptr<Type> &&supertype)
      : Node{loc}, types{std::move(types)}, supertype{std::move(supertype)} {}

  std::vector<Type> types;
  std::unique_ptr<Type> supertype;
};

struct TypesDef : Node {
  TypesDef(const location &loc, std::vector<TypeList> &&type_lists)
      : Node{loc}, type_lists{std::move(type_lists)} {}

  std::vector<TypeList> type_lists;
};

struct Constant : Node {
  Constant(const location &loc, const std::string &name)
      : Node{loc}, name{name} {}

  std::string name;
};

struct ConstantList : Node {
  ConstantList(const location &loc, std::vector<Constant> &&constants,
               std::unique_ptr<Type> &&type)
      : Node{loc}, constants{std::move(constants)}, type{std::move(type)} {}

  std::vector<Constant> constants;
  std::unique_ptr<Type> type;
};

struct ConstantsDef : Node {
  ConstantsDef(const location &loc, std::vector<ConstantList> &&constant_lists)
      : Node{loc}, constant_lists{std::move(constant_lists)} {}

  std::vector<ConstantList> constant_lists;
};

struct Parameter : Node {
  Parameter(const location &loc, const std::string &name)
      : Node{loc}, name{name} {}

  std::string name;
};

struct ParameterList : Node {
  ParameterList(const location &loc, std::vector<Parameter> &&parameters,
                std::unique_ptr<Type> &&type)
      : Node{loc}, parameters{std::move(parameters)}, type{std::move(type)} {}

  std::vector<Parameter> parameters;
  std::unique_ptr<Type> type;
};

struct Predicate : Node {
  Predicate(const location &loc, const std::string &name,
            std::vector<ParameterList> &&parameters)
      : Node{loc}, name{name}, parameters{std::move(parameters)} {}

  std::string name;
  std::vector<ParameterList> parameters;
};

struct PredicatesDef : Node {
  PredicatesDef(const location &loc, std::vector<Predicate> &&predicates)
      : Node{loc}, predicates{std::move(predicates)} {}

  std::vector<Predicate> predicates;
};

struct Argument : Node {
  Argument(const location &loc, const std::string &name)
      : Node{loc}, name{name} {}

  std::string name;
};

struct PredicateEvaluation : Node {
  PredicateEvaluation(const location &loc, const std::string &name,
                      std::vector<Argument> &&arguments)
      : Node{loc}, name{name}, arguments{std::move(arguments)} {}

  std::string name;
  std::vector<Argument> arguments;
};

struct Condition;

struct Conjunction : Node {
  Conjunction(const location &loc, std::vector<Condition> &&conditions)
      : Node{loc}, conditions{std::move(conditions)} {}

  std::vector<Condition> conditions;
};

struct Disjunction : Node {
  Disjunction(const location &loc, std::vector<Condition> &&conditions)
      : Node{loc}, conditions{std::move(conditions)} {}

  std::vector<Condition> conditions;
};

struct Negation : Node {
  Negation(const location &loc, std::unique_ptr<Condition> &&condition)
      : Node{loc}, condition{std::move(condition)} {}

  std::unique_ptr<Condition> condition;
};

struct Condition : Node {
  template <typename ConditionType>
  Condition(const location &loc, std::unique_ptr<ConditionType> &&condition)
      : Node{loc}, condition{std::move(condition)} {}

  std::variant<std::unique_ptr<PredicateEvaluation>,
               std::unique_ptr<Conjunction>, std::unique_ptr<Disjunction>,
               std::unique_ptr<Negation>>
      condition;
};

struct ActionDef : Node {
  ActionDef(const location &loc, const std::string &name,
            std::vector<ParameterList> &&parameters,
            std::unique_ptr<Condition> &&precondition,
            std::unique_ptr<Condition> &&effect)
      : Node{loc}, name{name}, parameters{std::move(parameters)},
        precondition{std::move(precondition)}, effect{std::move(effect)} {}

  std::string name;
  std::vector<ParameterList> parameters;
  std::unique_ptr<Condition> precondition;
  std::unique_ptr<Condition> effect;
};

struct Domain : Node {
  using Element = std::variant<RequirementsDef, TypesDef, ConstantsDef,
                               PredicatesDef, ActionDef>;
  Domain(const location &loc, const std::string &name,
         std::vector<Element> &&domain_body)
      : Node{loc}, name{name}, domain_body{std::move(domain_body)} {}

  std::string name;
  std::vector<Element> domain_body;
};

class AST {
public:
  AST(const AST &) = delete;
  AST(AST &&other)
      : domain_file{other.domain_file},
        problem_file{other.problem_file}, domain{std::move(other.domain)} {
    other.domain_file = "";
    other.problem_file = "";
  }
  AST(const std::string &domain_file, const std::string &problem_file,
      std::unique_ptr<Domain> &&domain)
      : domain_file{domain_file}, problem_file{problem_file}, domain{std::move(
                                                                  domain)} {}

  std::string domain_file;
  std::string problem_file;

  std::unique_ptr<Domain> domain;
};

} // namespace ast

} // namespace parser

#endif /* end of include guard: AST_H */
