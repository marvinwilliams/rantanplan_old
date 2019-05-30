#ifndef AST_H
#define AST_H

#include "location.hxx"
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace parser {

namespace ast {

struct Node {
  location loc;
  virtual ~Node() {}

protected:
  Node(const location &loc) : loc{loc} {}
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
  TypeList(const location &loc, std::vector<std::unique_ptr<Type>> &&types,
           std::unique_ptr<Type> &&supertype)
      : Node{loc}, types{std::move(types)}, supertype{std::move(supertype)} {}
  TypeList(const location &loc, std::vector<std::unique_ptr<Type>> &&types)
      : Node{loc}, types{std::move(types)} {}

  std::vector<std::unique_ptr<Type>> types;
  std::optional<std::unique_ptr<Type>> supertype;
};

struct TypesDef : Node {
  TypesDef(const location &loc,
           std::vector<std::unique_ptr<TypeList>> &&type_lists)
      : Node{loc}, type_lists{std::move(type_lists)} {}

  std::vector<std::unique_ptr<TypeList>> type_lists;
};

struct Constant : Node {
  Constant(const location &loc, const std::string &name)
      : Node{loc}, name{name} {}

  std::string name;
};

struct ConstantList : Node {
  ConstantList(const location &loc,
               std::vector<std::unique_ptr<Constant>> &&constants,
               std::unique_ptr<Type> &&type)
      : Node{loc}, constants{std::move(constants)}, type{std::move(type)} {}
  ConstantList(const location &loc,
               std::vector<std::unique_ptr<Constant>> &&constants)
      : Node{loc}, constants{std::move(constants)} {}

  std::vector<std::unique_ptr<Constant>> constants;
  std::optional<std::unique_ptr<Type>> type;
};

struct ConstantsDef : Node {
  ConstantsDef(const location &loc,
               std::vector<std::unique_ptr<ConstantList>> &&constant_lists)
      : Node{loc}, constant_lists{std::move(constant_lists)} {}

  std::vector<std::unique_ptr<ConstantList>> constant_lists;
};

struct Parameter : Node {
  Parameter(const location &loc, const std::string &name)
      : Node{loc}, name{name} {}

  std::string name;
};

struct ParameterList : Node {
  ParameterList(const location &loc,
                std::vector<std::unique_ptr<Parameter>> &&parameters,
                std::unique_ptr<Type> &&type)
      : Node{loc}, parameters{std::move(parameters)}, type{std::move(type)} {}
  ParameterList(const location &loc,
                std::vector<std::unique_ptr<Parameter>> &&parameters)
      : Node{loc}, parameters{std::move(parameters)} {}

  std::vector<std::unique_ptr<Parameter>> parameters;
  std::optional<std::unique_ptr<Type>> type;
};

struct Predicate : Node {
  Predicate(const location &loc, const std::string &name,
            std::vector<std::unique_ptr<ParameterList>> &&parameters)
      : Node{loc}, name{name}, parameters{std::move(parameters)} {}

  std::string name;
  std::vector<std::unique_ptr<ParameterList>> parameters;
};

struct PredicatesDef : Node {
  PredicatesDef(const location &loc,
                std::vector<std::unique_ptr<Predicate>> &&predicates)
      : Node{loc}, predicates{std::move(predicates)} {}

  std::vector<std::unique_ptr<Predicate>> predicates;
};

struct Argument : Node {
  Argument(const location &loc, const std::string &name)
      : Node{loc}, name{name} {}

  std::string name;
};

struct PredicateEvaluation : Node {
  PredicateEvaluation(const location &loc, const std::string &name,
                      std::vector<std::unique_ptr<Argument>> &&arguments)
      : Node{loc}, name{name}, arguments{std::move(arguments)} {}

  std::string name;
  std::vector<std::unique_ptr<Argument>> arguments;
};

struct Condition;

struct Conjunction : Node {
  Conjunction(const location &loc,
              std::vector<std::unique_ptr<Condition>> &&conditions)
      : Node{loc}, conditions{std::move(conditions)} {}

  std::vector<std::unique_ptr<Condition>> conditions;
};

struct Disjunction : Node {
  Disjunction(const location &loc,
              std::vector<std::unique_ptr<Condition>> &&conditions)
      : Node{loc}, conditions{std::move(conditions)} {}

  std::vector<std::unique_ptr<Condition>> conditions;
};

struct Negation : Node {
  Negation(const location &loc, std::unique_ptr<Condition> &&condition)
      : Node{loc}, condition{std::move(condition)} {}

  std::unique_ptr<Condition> condition;
};

using ConditionElement =
    std::variant<std::monostate, std::unique_ptr<PredicateEvaluation>,
                 std::unique_ptr<Conjunction>, std::unique_ptr<Disjunction>,
                 std::unique_ptr<Negation>>;

struct Condition : Node {
  Condition(const location &loc, ConditionElement &&element)
      : Node{loc}, element{std::move(element)} {}
  Condition(const location &loc) : Node{loc} {}

  ConditionElement element;
};

struct ActionDef : Node {
  ActionDef(const location &loc, const std::string &name,
            std::vector<std::unique_ptr<ParameterList>> &&parameters,
            std::unique_ptr<Condition> &&precondition,
            std::unique_ptr<Condition> &&effect)
      : Node{loc}, name{name}, parameters{std::move(parameters)},
        precondition{std::move(precondition)}, effect{std::move(effect)} {}

  std::string name;
  std::vector<std::unique_ptr<ParameterList>> parameters;
  std::unique_ptr<Condition> precondition;
  std::unique_ptr<Condition> effect;
};

using DomainElement =
    std::variant<std::unique_ptr<RequirementsDef>, std::unique_ptr<TypesDef>,
                 std::unique_ptr<ConstantsDef>, std::unique_ptr<PredicatesDef>,
                 std::unique_ptr<ActionDef>>;

struct DomainBody : Node {
  DomainBody(const location & loc, std::unique_ptr<DomainElement>&& element
}

struct Domain : Node {
  Domain(const location &loc, const std::string &name,
         std::vector<DomainElement> &&domain_body)
      : Node{loc}, name{name}, domain_body{std::move(domain_body)} {}

  std::string name;
  std::vector<DomainElement> domain_body;
};

class AST {
public:
  AST(const AST &) = delete;
  AST(AST &&other)
      : domain_file{other.domain_file},
        problem_file{other.problem_file}, domain_{std::move(other.domain_)} {
    other.domain_file = "";
    other.problem_file = "";
  }
  AST(const std::string &domain_file, const std::string &problem_file)
      : domain_file{domain_file}, problem_file{problem_file} {}

  void set_domain(std::unique_ptr<Domain> &&domain) {
    domain_ = std::move(domain);
  }

  const Domain *get_domain() const { return domain_.get(); }

  std::string domain_file;
  std::string problem_file;

private:
  std::unique_ptr<Domain> domain_;
};

} // namespace ast

} // namespace parser

#endif /* end of include guard: AST_H */
