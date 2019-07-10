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
  using value_type = T;

  List(const location &loc)
      : Node{loc},
        elements{std::make_unique<std::vector<std::unique_ptr<value_type>>>()} {
  }

  void add(const location &loc, std::unique_ptr<value_type> element) {
    this->loc += loc;
    elements->push_back(std::move(element));
  }

  std::unique_ptr<std::vector<std::unique_ptr<value_type>>> elements;
};

template <typename T> struct SingleTypeList : Node {
  using element_type = List<T>;

  SingleTypeList(const location &loc, std::unique_ptr<element_type> list,
                 std::optional<Name> type = std::nullopt)
      : Node{loc}, list{std::move(list)}, type{type} {}

  std::unique_ptr<element_type> list;
  std::optional<Name> type;
};

template <typename T> struct TypedList : Node {
  using value_type = SingleTypeList<T>;

  TypedList(const location &loc,
            std::unique_ptr<std::vector<std::unique_ptr<value_type>>> lists)
      : Node{loc}, lists{std::move(lists)} {}

  std::unique_ptr<std::vector<std::unique_ptr<value_type>>> lists;
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
  Predicate(const location &loc, std::unique_ptr<Name> name,
            std::unique_ptr<TypedVariableList> parameters)
      : Node{loc}, name{std::move(name)}, parameters{std::move(parameters)} {}

  std::unique_ptr<Name> name;
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
  PredicateEvaluation(const location &loc, std::unique_ptr<Name> name,
                      std::unique_ptr<ArgumentList> arguments)
      : Node{loc}, name{std::move(name)}, arguments{std::move(arguments)} {}

  std::unique_ptr<Name> name;
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
  ActionDef(const location &loc, std::unique_ptr<Name> name,
            std::unique_ptr<TypedVariableList> parameters,
            std::optional<std::unique_ptr<Precondition>> precondition,
            std::optional<std::unique_ptr<Effect>> effect)
      : Node{loc}, name{std::move(name)}, parameters{std::move(parameters)},
        precondition{std::move(precondition)}, effect{std::move(effect)} {}

  std::unique_ptr<Name> name;
  std::unique_ptr<TypedVariableList> parameters;
  std::optional<std::unique_ptr<Precondition>> precondition;
  std::optional<std::unique_ptr<Effect>> effect;
};

struct ObjectsDef : Node {
  ObjectsDef(const location &loc, std::unique_ptr<TypedNameList> objects)
      : Node{loc}, objects{std::move(objects)} {}

  std::unique_ptr<TypedNameList> objects;
};

struct InitPredicate : Node {
  InitPredicate(const location &loc, std::unique_ptr<Name> name,
                std::unique_ptr<NameList> arguments)
      : Node{loc}, name{std::move(name)}, arguments{std::move(arguments)} {}

  std::unique_ptr<Name> name;
  std::unique_ptr<NameList> arguments;
};

struct InitNegation : Node {
  InitNegation(const location &loc,
               std::unique_ptr<InitPredicate> init_predicate)
      : Node{loc}, init_predicate{std::move(init_predicate)} {}

  std::unique_ptr<InitPredicate> init_predicate;
};

using InitCondition = std::variant<InitPredicate, InitNegation>;

using InitList = detail::List<InitCondition>;

struct InitDef : Node {
  InitDef(const location &loc, std::unique_ptr<InitList> init_predicates)
      : Node{loc}, init_predicates{std::move(init_predicates)} {}

  std::unique_ptr<InitList> init_predicates;
};

struct GoalDef : Node {
  GoalDef(const location &loc, std::unique_ptr<Condition> goal)
      : Node{loc}, goal{std::move(goal)} {}

  std::unique_ptr<Condition> goal;
};

using Element =
    std::variant<RequirementsDef, TypesDef, ConstantsDef, PredicatesDef,
                 ActionDef, ObjectsDef, InitDef, GoalDef>;

struct Domain : Node {
  Domain(const location &loc, std::unique_ptr<Name> name,
         std::unique_ptr<std::vector<std::unique_ptr<Element>>> domain_body)
      : Node{loc}, name{std::move(name)}, domain_body{std::move(domain_body)} {}

  std::unique_ptr<Name> name;
  std::unique_ptr<std::vector<std::unique_ptr<Element>>> domain_body;
};

struct Problem : Node {
  Problem(const location &loc, std::unique_ptr<Name> name,
          std::unique_ptr<Name> domain_ref,
          std::unique_ptr<std::vector<std::unique_ptr<Element>>> problem_body)
      : Node{loc}, name{std::move(name)}, domain_ref{std::move(domain_ref)},
        problem_body{std::move(problem_body)} {}

  std::unique_ptr<Name> name;
  std::unique_ptr<Name> domain_ref;
  std::unique_ptr<std::vector<std::unique_ptr<Element>>> problem_body;
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

  void set_problem(std::unique_ptr<Problem> problem) {
    problem_ = std::move(problem);
  }

  const std::string &get_domain_file() const { return domain_file_; }
  const std::string &get_problem_file() const { return problem_file_; }
  const Domain *get_domain() const { return domain_.get(); }
  const Problem *get_problem() const { return problem_.get(); }

private:
  std::string domain_file_;
  std::string problem_file_;

  std::unique_ptr<Domain> domain_;
  std::unique_ptr<Problem> problem_;
};

} // namespace ast

} // namespace parser

#endif /* end of include guard: AST_H */
