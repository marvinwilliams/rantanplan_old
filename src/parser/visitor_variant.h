#ifndef VISITOR_H
#define VISITOR_H

#include "ast_variant.h"
#include <type_traits>

namespace parser {

namespace visitor {

using namespace ast;

/*This CRTP-Visitor provides the functionality to traverse the AST. Ther
 * traverse(*) method recursively traverses the AST and calls
 * visit_begin(*)/visit_end(*) on each element it encounters. The boolean return
 * value of both methods indicates wether to continue traversing. You can
 * provide your own traverse(*) and visit_begin(*)/visit_end(*) implementations
 * in the Derived class */
template <typename Derived> class Visitor {
public:
  Visitor() : condition_visitor_{get_derived_()}, domain_element_visitor_{get_derived_()} {}

  bool traverse(const AST &ast) {
    get_derived_().visit_begin(ast);
    if (!ast.get_domain()) {
      return get_derived_().visit_end(ast);
    }
    return get_derived_().traverse(*ast.get_domain()) &&
           get_derived_().visit_end(ast);
  }

  bool traverse(const Domain &domain) {
    return get_derived_().visit_begin(domain) &&
           traverse_(*domain.domain_body) && get_derived_().visit_end(domain);
  }

  bool traverse(const DomainElement &domain_element) {
    return get_derived_().visit_begin(domain_element) &&
           std::visit(domain_element_visitor_, domain_element) &&
           visit_end(domain_element);
  }

  bool traverse(const RequirementsDef &requirements_def) {
    return get_derived_().visit_begin(requirements_def) &&
           get_derived_().traverse(*requirements_def.requirements) &&
           get_derived_().visit_end(requirements_def);
  }

  bool traverse(const TypesDef &types_def) {
    return get_derived_().visit_begin(types_def) &&
           get_derived_().traverse(*types_def.type_list) &&
           get_derived_().visit_end(types_def);
  }

  bool traverse(const ConstantsDef &constants_def) {
    return get_derived_().visit_begin(constants_def) &&
           get_derived_().traverse(*constants_def.constant_list) &&
           get_derived_().visit_end(constants_def);
  }

  bool traverse(const PredicatesDef &predicates_def) {
    return get_derived_().visit_begin(predicates_def) &&
           get_derived_().traverse(*predicates_def.predicate_list) &&
           get_derived_().visit_end(predicates_def);
  }

  bool traverse(const ActionDef &action_def) {
    return get_derived_().visit_begin(action_def) &&
           get_derived_().traverse(*action_def.parameters) &&
           (action_def.precondition
                ? get_derived_().traverse(*action_def.precondition.value())
                : true) &&
           (action_def.effect
                ? get_derived_().traverse(*action_def.effect.value())
                : true) &&
           get_derived_().visit_end(action_def);
  }

  bool traverse(const Precondition &precondition) {
    return get_derived_().visit_begin(precondition) &&
           get_derived_().traverse(*precondition.precondition) &&
           get_derived_().visit_end(precondition);
  }

  bool traverse(const Effect &effect) {
    return get_derived_().visit_begin(effect) &&
           get_derived_().traverse(*effect.effect) &&
           get_derived_().visit_end(effect);
  }

  bool traverse(const ConditionList &condition_list) {
    return get_derived_().visit_begin(condition_list) &&
           traverse_(*condition_list.elements) &&
           get_derived_().visit_end(condition_list);
  }

  bool traverse(const PredicateList &predicate_list) {
    return get_derived_().visit_begin(predicate_list) &&
           traverse_(*predicate_list.elements) &&
           get_derived_().visit_end(predicate_list);
  }

  bool traverse(const TypedNameList &typed_name_list) {
    return get_derived_().visit_begin(typed_name_list) &&
           traverse_(*typed_name_list.lists) &&
           get_derived_().visit_end(typed_name_list);
  }

  bool traverse(const TypedVariableList &typed_variable_list) {
    return get_derived_().visit_begin(typed_variable_list) &&
           traverse_(*typed_variable_list.lists) &&
           get_derived_().visit_end(typed_variable_list);
  }

  bool traverse(const RequirementsList &requirements_list) {
    return get_derived_().visit_begin(requirements_list) &&
           traverse_(*requirements_list.elements) &&
           get_derived_().visit_end(requirements_list);
  }

  bool traverse(const ArgumentList &argument_list) {
    return get_derived_().visit_begin(argument_list) &&
           traverse_(*argument_list.elements) &&
           get_derived_().visit_end(argument_list);
  }

  bool traverse(const NameList &name_list) {
    return get_derived_().visit_begin(name_list) &&
           traverse_(*name_list.elements) &&
           get_derived_().visit_end(name_list);
  }

  bool traverse(const VariableList &variable_list) {
    return get_derived_().visit_begin(variable_list) &&
           traverse_(*variable_list.elements) &&
           get_derived_().visit_end(variable_list);
  }

  bool traverse(const Predicate &predicate) {
    return get_derived_().visit_begin(predicate) &&
           get_derived_().traverse(*predicate.parameters) &&
           get_derived_().visit_end(predicate);
  }

  bool traverse(const Condition &condition) {
    return get_derived_().visit_begin(condition) &&
           std::visit(condition_visitor_, condition);
    get_derived_().visit_end(condition);
  }

  bool traverse(const PredicateEvaluation &predicate_evaluation) {
    return get_derived_().visit_begin(predicate_evaluation) &&
           get_derived_().traverse(*predicate_evaluation.arguments) &&
           get_derived_().visit_end(predicate_evaluation);
  }

  bool traverse(const Conjunction &conjunction) {
    return get_derived_().visit_begin(conjunction) &&
           get_derived_().traverse(*conjunction.conditions) &&
           get_derived_().visit_end(conjunction);
  }

  bool traverse(const Disjunction &disjunction) {
    return get_derived_().visit_begin(disjunction) &&
           get_derived_().traverse(*disjunction.conditions) &&
           get_derived_().visit_end(disjunction);
  }

  bool traverse(const Negation &negation) {
    return get_derived_().visit_begin(negation) &&
           get_derived_().traverse(*negation.condition) &&
           get_derived_().visit_end(negation);
  }

  bool traverse(const Argument &argument) {
    return get_derived_().visit_begin(argument) &&
           (std::holds_alternative<Name>(argument)
                ? get_derived_().traverse(std::get<Name>(argument))
                : get_derived_().traverse(std::get<Variable>(argument))) &&
           get_derived_().visit_end(argument);
  }

  bool traverse(const Requirement &requirement) {
    return get_derived_().visit_begin(requirement) &&
           get_derived_().visit_end(requirement);
  }

  bool traverse(const Name &name) {
    return get_derived_().visit_begin(name) && get_derived_().visit_end(name);
  }

  bool traverse(const Variable &variable) {
    return get_derived_().visit_begin(variable) &&
           get_derived_().visit_end(variable);
  }

  bool traverse(const SingleTypedNameList &single_typed_name_list) {
    return get_derived_().visit_begin(single_typed_name_list) &&
           get_derived_().traverse(*single_typed_name_list.list) &&
           (single_typed_name_list.type
                ? get_derived_().traverse(single_typed_name_list.type.value())
                : true) &&
           get_derived_().visit_end(single_typed_name_list);
  }

  bool traverse(const SingleTypedVariableList &single_typed_variable_list) {
    return get_derived_().visit_begin(single_typed_variable_list) &&
           get_derived_().traverse(*single_typed_variable_list.list) &&
           (single_typed_variable_list.type
                ? get_derived_().traverse(
                      single_typed_variable_list.type.value())
                : true) &&
           get_derived_().visit_end(single_typed_variable_list);
  }

  // Visit functions to be overwritten.
  bool visit_begin(const AST &) { return true; }
  bool visit_begin(const Domain &) { return true; }
  bool visit_begin(const DomainElement &) { return true; }
  bool visit_begin(const RequirementsDef &) { return true; }
  bool visit_begin(const TypesDef &) { return true; }
  bool visit_begin(const ConstantsDef &) { return true; }
  bool visit_begin(const PredicatesDef &) { return true; }
  bool visit_begin(const ActionDef &) { return true; }
  bool visit_begin(const Precondition &) { return true; }
  bool visit_begin(const Effect &) { return true; }
  bool visit_begin(const ConditionList &) { return true; }
  bool visit_begin(const PredicateList &) { return true; }
  bool visit_begin(const TypedNameList &) { return true; }
  bool visit_begin(const TypedVariableList &) { return true; }
  bool visit_begin(const RequirementsList &) { return true; }
  bool visit_begin(const ArgumentList &) { return true; }
  bool visit_begin(const NameList &) { return true; }
  bool visit_begin(const VariableList &) { return true; }
  bool visit_begin(const Predicate &) { return true; }
  bool visit_begin(const Condition &) { return true; }
  bool visit_begin(const PredicateEvaluation &) { return true; }
  bool visit_begin(const Conjunction &) { return true; }
  bool visit_begin(const Disjunction &) { return true; }
  bool visit_begin(const Negation &) { return true; }
  bool visit_begin(const Requirement &) { return true; }
  bool visit_begin(const Argument &) { return true; }
  bool visit_begin(const Name &) { return true; }
  bool visit_begin(const Variable &) { return true; }

  bool visit_end(const AST &) { return true; }
  bool visit_end(const Domain &) { return true; }
  bool visit_end(const DomainElement &) { return true; }
  bool visit_end(const RequirementsDef &) { return true; }
  bool visit_end(const TypesDef &) { return true; }
  bool visit_end(const ConstantsDef &) { return true; }
  bool visit_end(const PredicatesDef &) { return true; }
  bool visit_end(const ActionDef &) { return true; }
  bool visit_end(const Precondition &) { return true; }
  bool visit_end(const Effect &) { return true; }
  bool visit_end(const ConditionList &) { return true; }
  bool visit_end(const PredicateList &) { return true; }
  bool visit_end(const TypedNameList &) { return true; }
  bool visit_end(const TypedVariableList &) { return true; }
  bool visit_end(const RequirementsList &) { return true; }
  bool visit_end(const ArgumentList &) { return true; }
  bool visit_end(const NameList &) { return true; }
  bool visit_end(const VariableList &) { return true; }
  bool visit_end(const Predicate &) { return true; }
  bool visit_end(const Condition &) { return true; }
  bool visit_end(const PredicateEvaluation &) { return true; }
  bool visit_end(const Conjunction &) { return true; }
  bool visit_end(const Disjunction &) { return true; }
  bool visit_end(const Negation &) { return true; }
  bool visit_end(const Requirement &) { return true; }
  bool visit_end(const Argument &) { return true; }
  bool visit_end(const Name &) { return true; }
  bool visit_end(const Variable &) { return true; }

  // Helper, unlikely to be overwritten
  bool visit_begin(const SingleTypedNameList &) { return true; }
  bool visit_begin(const SingleTypedVariableList &) { return true; }

  bool visit_end(const SingleTypedNameList &) { return true; }
  bool visit_end(const SingleTypedVariableList &) { return true; }

  virtual ~Visitor() {}

private:
  Derived &get_derived_() { return *static_cast<Derived *>(this); }

  struct DomainElementVisitor {
    DomainElementVisitor(Derived &derived) : derived{derived} {}
    bool operator()(const RequirementsDef &requirements_def) {
      return derived.traverse(requirements_def);
    }
    bool operator()(const TypesDef &types_def) {
      return derived.traverse(types_def);
    }
    bool operator()(const ConstantsDef &constants_def) {
      return derived.traverse(constants_def);
    }
    bool operator()(const PredicatesDef &predicates_def) {
      return derived.traverse(predicates_def);
    }
    bool operator()(const ActionDef &action_def) {
      return derived.traverse(action_def);
    }

    Derived &derived;
  };

  struct ConditionVisitor {
    ConditionVisitor(Derived &derived) : derived{derived} {}
    bool operator()(const std::monostate &) { return true; }
    bool operator()(const PredicateEvaluation &predicate_evaluation) {
      return derived.traverse(predicate_evaluation);
    }
    bool operator()(const Conjunction &conjunction) {
      return derived.traverse(conjunction);
    }
    bool operator()(const Disjunction &disjunction) {
      return derived.traverse(disjunction);
    }
    bool operator()(const Negation &negation) {
      return derived.traverse(negation);
    }

    Derived &derived;
  };

  template <typename T>
  bool traverse_(const std::vector<std::unique_ptr<T>> &element) {
    for (auto &c : element) {
      if (!get_derived_().traverse(*c)) {
        return false;
      }
    }
    return true;
  }

  ConditionVisitor condition_visitor_;
  DomainElementVisitor domain_element_visitor_;
};

} // namespace visitor
} // namespace parser

#endif /* end of include guard: VISITOR_H */
