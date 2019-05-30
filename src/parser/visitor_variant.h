#ifndef VISITOR_H
#define VISITOR_H

#include "ast_variant.h"
#include <type_traits>

namespace parser {

namespace ast {

template <typename Derived> class Visitor {
public:
  Visitor() : condition_visitor{get_derived()} {}

  bool traverse(const AST &ast) {
    if (!ast.get_domain()) {
      return true;
    }
    return get_derived().traverse(*ast.get_domain()) &&
           get_derived().visit(ast);
  }

  bool traverse(const Domain &domain) {
    return traverse_(domain.domain_body) && get_derived().visit(domain);
  }

  bool traverse(const DomainElement &domain_element) {
    if (const RequirementsDef *req_ptr =
            dynamic_cast<const RequirementsDef *>(&domain_element);
        req_ptr != nullptr) {
      if (!get_derived().traverse(*req_ptr)) {
        return false;
      }
    } else if (const TypesDef *types_ptr =
                   dynamic_cast<const TypesDef *>(&domain_element);
               types_ptr != nullptr) {
      if (!get_derived().traverse(*types_ptr)) {
        return false;
      }
    } else if (const ConstantsDef *const_ptr =
                   dynamic_cast<const ConstantsDef *>(&domain_element);
               const_ptr != nullptr) {
      if (!get_derived().traverse(*const_ptr)) {
        return false;
      }
    } else if (const PredicatesDef *pred_ptr =
                   dynamic_cast<const PredicatesDef *>(&domain_element);
               pred_ptr != nullptr) {
      if (!get_derived().traverse(*pred_ptr)) {
        return false;
      }
    } else if (const ActionDef *action_ptr =
                   dynamic_cast<const ActionDef *>(&domain_element);
               action_ptr != nullptr) {
      if (!get_derived().traverse(*action_ptr)) {
        return false;
      }
    }
    return get_derived().visit(domain_element);
  }

  bool traverse(const Requirement &requirement) {
    return get_derived().visit(requirement);
  }

  bool traverse(const RequirementsDef &requirements_def) {
    return traverse_(requirements_def.requirements) &&
           get_derived().visit(requirements_def);
  }

  bool traverse(const Type &type) { return get_derived().visit(type); }

  bool traverse(const TypeList &type_list) {
    return traverse_(type_list.types) && get_derived().visit(type_list);
  }

  bool traverse(const TypesDef &types_def) {
    return traverse_(types_def.type_lists) && get_derived().visit(types_def);
  }

  bool traverse(const Constant &constant) {
    return get_derived().visit(constant);
  }

  bool traverse(const ConstantList &constant_list) {
    return traverse_(constant_list.constants) &&
           get_derived().visit(constant_list);
  }

  bool traverse(const ConstantsDef &constants_def) {
    return traverse_(constants_def.constant_lists) &&
           get_derived().visit(constants_def);
  }

  bool traverse(const Predicate &predicate) {
    return traverse_(predicate.parameters) && get_derived().visit(predicate);
  }

  bool traverse(const Parameter &parameter) {
    return get_derived().visit(parameter);
  }

  bool traverse(const ParameterList &parameter_list) {
    return traverse_(parameter_list.parameters) &&
           get_derived().visit(parameter_list);
  }

  bool traverse(const PredicatesDef &predicates_def) {
    return traverse_(predicates_def.predicates) &&
           get_derived().visit(predicates_def);
  }

  bool traverse(const Condition &condition) {
    return std::visit(condition_visitor, condition.element) &&
           get_derived().visit(condition);
  }

  bool traverse(const Argument &argument) {
    return get_derived().visit(argument);
  }

  bool traverse(const PredicateEvaluation &predicate) {
    return traverse_(predicate.arguments) && get_derived().visit(predicate);
  }

  bool traverse(const Conjunction &conjunction) {
    return traverse_(conjunction.conditions) &&
           get_derived().visit(conjunction);
  }

  bool traverse(const Disjunction &disjunction) {
    return traverse_(disjunction.conditions) &&
           get_derived().visit(disjunction);
  }

  bool traverse(const Negation &negation) {
    return get_derived().traverse(*negation.condition) &&
           get_derived().visit(negation);
  }

  bool traverse(const ActionDef &action_def) {
    if (!traverse_(action_def.parameters)) {
      return false;
    }
    if (!get_derived().traverse(*action_def.precondition)) {
      return false;
    }
    if (!get_derived().traverse(*action_def.effect)) {
      return false;
    }
    return get_derived().visit(action_def);
  }

  bool visit(const AST &) { return true; }
  bool visit(const Domain &) { return true; }
  bool visit(const DomainElement &) { return true; }
  bool visit(const Requirement &) { return true; }
  bool visit(const RequirementsDef &) { return true; }
  bool visit(const Type &) { return true; }
  bool visit(const TypeList &) { return true; }
  bool visit(const TypesDef &) { return true; }
  bool visit(const Constant &) { return true; }
  bool visit(const ConstantList &) { return true; }
  bool visit(const ConstantsDef &) { return true; }
  bool visit(const Predicate &) { return true; }
  bool visit(const Parameter &) { return true; }
  bool visit(const ParameterList &) { return true; }
  bool visit(const PredicatesDef &) { return true; }
  bool visit(const Condition &) { return true; }
  bool visit(const Argument &) { return true; }
  bool visit(const PredicateEvaluation &) { return true; }
  bool visit(const Conjunction &) { return true; }
  bool visit(const Disjunction &) { return true; }
  bool visit(const Negation &) { return true; }
  bool visit(const ActionDef &) { return true; }

  virtual ~Visitor() {}

private:
  Derived &get_derived() { return *static_cast<Derived *>(this); }

  struct ConditionVisitor {
    ConditionVisitor(Derived &derived) : derived{derived} {}
    bool operator()(const std::monostate &) { return true; }
    bool operator()(
        const std::unique_ptr<PredicateEvaluation> &predicate_evaluation) {
      return derived.traverse(*predicate_evaluation);
    }
    bool operator()(const std::unique_ptr<Conjunction> &conjunction) {
      return derived.traverse(*conjunction);
    }
    bool operator()(const std::unique_ptr<Disjunction> &disjunction) {
      return derived.traverse(*disjunction);
    }
    bool operator()(const std::unique_ptr<Negation> &negation) {
      return derived.traverse(*negation);
    }

    Derived &derived;
  };

  template <typename Container> bool traverse_(const Container &container) {
    for (auto &c : container) {
      if (!get_derived().traverse(*c)) {
      }
    }
    return true;
  }

  ConditionVisitor condition_visitor;
};

} // namespace ast

} // namespace parser

#endif /* end of include guard: VISITOR_H */
