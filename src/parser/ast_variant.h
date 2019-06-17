#ifndef AST_H
#define AST_H

#include "location.hxx"
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace parser {

namespace ast {

// The basic node for the ast contains only a location
struct Node {
  location loc;
  virtual ~Node() {}

protected:
  Node(const location &loc) : loc{loc} {}
};

struct Name : Node {
  Name(const location &loc, std::string name) : Node{loc}, name{name} {}

  std::string name;
};

struct Variable : Node {
  Variable(const location &loc, std::string variable)
      : Node{loc}, variable{variable} {}

  std::string variable;
};

using Argument = std::variant<Name, Variable>;

struct Requirement : Node {
  Requirement(const location &loc, const std::string &name)
      : Node{loc}, name{name} {}

  std::string name;
};

namespace detail {

template <typename T> struct List : Node {
  List(const location &loc)
      : Node{loc}, elements{
                       std::make_unique<std::vector<std::unique_ptr<T>>>()} {}

  void add(const location &loc, std::unique_ptr<T> element) {
    this->loc += loc;
    elements->push_back(std::move(element));
  }

  std::unique_ptr<std::vector<std::unique_ptr<T>>> elements;
};

template <typename T> struct SingleTypeList : Node {
  SingleTypeList(const location &loc, std::unique_ptr<List<T>> list,
                 std::optional<Name> type = std::nullopt)
      : Node{loc}, list{std::move(list)}, type{type} {}

  std::unique_ptr<List<T>> list;
  std::optional<Name> type;
};

template <typename T> struct TypedList : Node {
  TypedList(const location &loc)
      : Node{loc}, lists{std::make_unique<
                       std::vector<std::unique_ptr<SingleTypeList<T>>>>()} {}

  void add(std::unique_ptr<SingleTypeList<T>> list) {
    loc += list->loc;
    lists->push_back(std::move(list));
  }

  std::unique_ptr<std::vector<std::unique_ptr<SingleTypeList<T>>>> lists;
};

} // namespace detail

using NameList = detail::List<Name>;
using VariableList = detail::List<Variable>;
using RequirementsList = detail::List<Requirement>;
using ArgumentList = detail::List<Argument>;
using SingleTypedNameList = detail::SingleTypeList<Name>;
using SingleTypedVariableList = detail::SingleTypeList<Variable>;
using TypedNameList = detail::TypedList<Name>;
using TypedVariableList = detail::TypedList<Variable>;

struct RequirementsDef : Node {
  RequirementsDef(const location &loc,
                  std::unique_ptr<RequirementsList> requirements)
      : Node{loc}, requirements{std::move(requirements)} {}

  std::unique_ptr<RequirementsList> requirements;
};

struct TypesDef : Node {
  TypesDef(const location &loc, std::unique_ptr<TypedNameList> type_list)
      : Node{loc}, type_list{std::move(type_list)} {}

  std::unique_ptr<TypedNameList> type_list;
};

struct ConstantsDef : Node {
  ConstantsDef(const location &loc,
               std::unique_ptr<TypedVariableList> constant_list)
      : Node{loc}, constant_list{std::move(constant_list)} {}

  std::unique_ptr<TypedVariableList> constant_list;
};

struct Predicate : Node {
  Predicate(const location &loc, const std::string &name,
            std::unique_ptr<TypedVariableList> parameters)
      : Node{loc}, name{name}, parameters{std::move(parameters)} {}

  std::string name;
  std::unique_ptr<TypedVariableList> parameters;
};

using PredicateList = detail::List<Predicate>;

struct PredicatesDef : Node {
  PredicatesDef(const location &loc,
                std::unique_ptr<PredicateList> predicate_list)
      : Node{loc}, predicate_list{std::move(predicate_list)} {}

  std::unique_ptr<PredicateList> predicate_list;
};

struct PredicateEvaluation;
struct Conjunction;
struct Disjunction;
struct Negation;

using Condition = std::variant<std::monostate, PredicateEvaluation, Conjunction,
                               Disjunction, Negation>;

using ConditionList = detail::List<Condition>;

struct PredicateEvaluation : Node {
  PredicateEvaluation(const location &loc, const std::string &name,
                      std::unique_ptr<ArgumentList> arguments)
      : Node{loc}, name{name}, arguments{std::move(arguments)} {}

  std::string name;
  std::unique_ptr<ArgumentList> arguments;
};

struct Conjunction : Node {
  Conjunction(const location &loc, std::unique_ptr<ConditionList> conditions)
      : Node{loc}, conditions{std::move(conditions)} {}

  std::unique_ptr<ConditionList> conditions;
};

struct Disjunction : Node {
  Disjunction(const location &loc, std::unique_ptr<ConditionList> conditions)
      : Node{loc}, conditions{std::move(conditions)} {}

  std::unique_ptr<ConditionList> conditions;
};

struct Negation : Node {
  Negation(const location &loc, std::unique_ptr<Condition> condition)
      : Node{loc}, condition{std::move(condition)} {}

  std::unique_ptr<Condition> condition;
};

struct Precondition : Node {
  Precondition(const location &loc, std::unique_ptr<Condition> precondition)
      : Node{loc}, precondition{std::move(precondition)} {}

  std::unique_ptr<Condition> precondition;
};

struct Effect : Node {
  Effect(const location &loc, std::unique_ptr<Condition> effect)
      : Node{loc}, effect{std::move(effect)} {}

  std::unique_ptr<Condition> effect;
};

struct ActionDef : Node {
  ActionDef(const location &loc, const std::string &name,
            std::unique_ptr<TypedVariableList> parameters,
            std::optional<std::unique_ptr<Precondition>> precondition,
            std::optional<std::unique_ptr<Effect>> effect)
      : Node{loc}, name{name}, parameters{std::move(parameters)},
        precondition{std::move(precondition)}, effect{std::move(effect)} {}

  std::string name;
  std::unique_ptr<TypedVariableList> parameters;
  std::optional<std::unique_ptr<Precondition>> precondition;
  std::optional<std::unique_ptr<Effect>> effect;
};

using DomainElement = std::variant<RequirementsDef, TypesDef, ConstantsDef,
                                   PredicatesDef, ActionDef>;

struct Domain : Node {
  Domain(
      const location &loc, const std::string &name,
      std::unique_ptr<std::vector<std::unique_ptr<DomainElement>>> domain_body)
      : Node{loc}, name{name}, domain_body{std::move(domain_body)} {}

  std::string name;
  std::unique_ptr<std::vector<std::unique_ptr<DomainElement>>> domain_body;
};

/* The AST is built while parsing. It abstracts the entire input and can later
 * be traversed by a visitor. Most constructors only take unique pointers so
 * that the AST must be built inplace */
class AST {
public:
  AST(const AST &) = delete;
  AST(AST &&other)
      : domain_file_{other.domain_file_},
        problem_file_{other.problem_file_}, domain_{std::move(other.domain_)} {
    other.domain_file_ = "";
    other.problem_file_ = "";
  }
  AST(const std::string &domain_file, const std::string &problem_file)
      : domain_file_{domain_file}, problem_file_{problem_file} {}

  void set_domain(std::unique_ptr<Domain> domain) {
    domain_ = std::move(domain);
  }

  const std::string &get_domain_file() const { return domain_file_; }
  const std::string &get_problem_file() const { return problem_file_; }
  const Domain *get_domain() const { return domain_.get(); }

private:
  std::string domain_file_;
  std::string problem_file_;

  std::unique_ptr<Domain> domain_;
};

} // namespace ast

} // namespace parser

#endif /* end of include guard: AST_H */
