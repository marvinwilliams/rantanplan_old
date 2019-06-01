#ifndef AST_H
#define AST_H

#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include "location.hxx"

namespace parser {

namespace ast {

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

template <typename T>
struct List : Node {
  List(const location &loc, std::vector<T> &&names)
      : Node{loc}, names{std::move(names)} {}

  std::vector<T> names;
};

template <typename T>
struct SingleTypeList : Node {
  SingleTypeList(const location &loc, List<T> &&list,
                 std::optional<Name> &&type = std::nullopt)
      : Node{loc}, list{std::move(list)}, type{type} {}

  List<T> list;
  std::optional<Name> type;
};

template <typename T>
struct TypedList : Node {
  TypedList(const location &loc, std::vector<SingleTypeList<T>> &&lists)
      : Node{loc}, lists{std::move(lists)} {}

  std::vector<SingleTypeList<T>> lists;
};

using ArgumentList = List<Variable>;
using ParameterList = TypedList<Name>;
using TypeList = TypedList<Name>;
using ConstantList = TypedList<Name>;

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

struct TypesDef : Node {
  TypesDef(const location &loc, std::unique_ptr<TypedList> &&type_list)
      : Node{loc}, type_list{std::move(type_list)} {}

  std::unique_ptr<TypedList> type_list;
};

struct ConstantsDef : Node {
  ConstantsDef(const location &loc, std::unique_ptr<TypedList> &&constant_list)
      : Node{loc}, constant_list{std::move(constant_list)} {}

  std::unique_ptr<TypedList> constant_list;
};

struct ParameterList : Node {
  TypedNameList(const location &loc, std::vector<NameList> &&name_lists,
                std::optional<Name> &&type = std::nullopt)
      : Node{loc}, name_lists{std::move(name_lists)}, type{type} {}

  std::vector<NameList> name_lists;
  std::optional<Name> type;
};

struct ParameterList : Node {
  ParameterList(const location &loc,
                std::vector<std::unique_ptr<Parameter>> &&parameters,
                std::unique_ptr<Type> &&type)
      : Node{loc}, parameters{std::move(parameters)}, type{std::move(type)} {}

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
      : Node{loc},
        name{name},
        parameters{std::move(parameters)},
        precondition{std::move(precondition)},
        effect{std::move(effect)} {}

  std::string name;
  std::vector<std::unique_ptr<ParameterList>> parameters;
  std::unique_ptr<Condition> precondition;
  std::unique_ptr<Condition> effect;
};

struct Domain : Node {
  using DomainElement =
      std::variant<std::unique_ptr<RequirementsDef>, std::unique_ptr<TypesDef>,
                   std::unique_ptr<ConstantsDef>,
                   std::unique_ptr<PredicatesDef>, std::unique_ptr<ActionDef>>;
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
        problem_file{other.problem_file},
        domain_{std::move(other.domain_)} {
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

namespace detail {

template <typename ElementType>
struct GenericElement : Node {
  GenericElement(const location &loc, ElementType &&element)
      : Node{loc}, element{std::move(element)} {}

  ElementType element;
};

template <typename ElementType>
struct GenericList : Node {
  GenericList(const location &loc,
              std::vector<GenericElement<ElementType>> &&elements)
      : Node{loc}, elements{std::move(elements)} {}

  std::vector<GenericElement<ElementType>> elements;
};

template <typename T1, typename T2>
struct GenericPair : Node {
  GenericPair(const location &loc, T1 &&first, T2 &&second)
      : Node{loc}, first{std::move(first)}, second{std::move(second)} {}

  T1 first;
  T2 second;
};

template <typename ListType, typename ElemType>
std::vector<std::unique_ptr<ListType>> convert_list_of_typed_lists(
    const ListOfTypedLists &list) {
  std::vector<std::unique_ptr<ListType>> converted_list;
  for (auto &typed_list : list.typed_lists) {
    std::vector<std::unique_ptr<ElemType>> inner_list;
    for (auto &elem : typed_list.string_list.elements) {
      inner_list.push_back(
          std::make_unique<ElemType>(ElemType(elem.first, elem.second)));
    }
    converted_list.push_back(std::make_unique<ListType>(
        typed_list.loc, std::move(inner_list),
        std::make_unique<ast::Type>(typed_list.type.loc,
                                    typed_list.type.name)));
  }
  std::vector<std::unique_ptr<ElemType>> untyped_list;
  for (auto &elem : list.untyped_list.elements) {
    untyped_list.push_back(
        std::make_unique<ElemType>(ElemType(elem.first, elem.second)));
  }
  converted_list.push_back(std::make_unique<ListType>(list.untyped_list.loc,
                                                      std::move(untyped_list)));
  return converted_list;
}

}  // namespace detail

using StringList = detail::GenericList<std::string>;
using TypedList =
    detail::GenericPair<StringList,
                        std::optional<detail::GenericElement<std::string>>>;
using ListOfTypedLists = detail::GenericList<TypedList>;

}  // namespace ast

}  // namespace parser

#endif /* end of include guard: AST_H */
