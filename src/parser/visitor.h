#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"
#include <type_traits>

namespace parser {

namespace ast {

template <typename Derived> class Visitor {
public:
  Derived *get_derived() { return static_cast<Derived *>(this); }

  bool traverse(const AST &ast) {
    if (!ast.get_domain()) {
      return true;
    }
    if (!get_derived()->traverse(*ast.get_domain())) {
      return false;
    }
    return get_derived()->visit(ast);
  }

  bool traverse(const Domain &domain) {
    if (!traverse_(domain.domain_body)) {
      return false;
    }
    return get_derived()->visit(domain);
  }

  bool traverse(const DomainElement &domain_element) {
    if (const RequirementsDef *req_ptr =
            dynamic_cast<const RequirementsDef *>(&domain_element);
        req_ptr != nullptr) {
      if (!get_derived()->traverse(*req_ptr)) {
        return false;
      }
    } else if (const TypesDef *types_ptr =
                   dynamic_cast<const TypesDef *>(&domain_element);
               types_ptr != nullptr) {
      if (!get_derived()->traverse(*types_ptr)) {
        return false;
      }
    } else if (const ConstantsDef *const_ptr =
                   dynamic_cast<const ConstantsDef *>(&domain_element);
               const_ptr != nullptr) {
      if (!get_derived()->traverse(*const_ptr)) {
        return false;
      }
    } else if (const PredicatesDef *pred_ptr =
                   dynamic_cast<const PredicatesDef *>(&domain_element);
               pred_ptr != nullptr) {
      if (!get_derived()->traverse(*pred_ptr)) {
        return false;
      }
    } else if (const ActionDef *action_ptr =
                   dynamic_cast<const ActionDef *>(&domain_element);
               action_ptr != nullptr) {
      if (!get_derived()->traverse(*action_ptr)) {
        return false;
      }
    }
    return get_derived()->visit(domain_element);
  }

  bool traverse(const Requirement &requirement) {
    return get_derived()->visit(requirement);
  }

  bool traverse(const RequirementsDef &requirements_def) {
    if (!traverse_(requirements_def.requirements)) {
      return false;
    }
    return get_derived()->visit(requirements_def);
  }

  bool traverse(const Type &type) { return get_derived()->visit(type); }

  bool traverse(const TypeList &type_list) {
    if (!traverse_(type_list.types)) {
      return false;
    }
    return get_derived()->visit(type_list);
  }

  bool traverse(const TypesDef &types_def) {
    if (!traverse_(types_def.type_lists)) {
      return false;
    }
    return get_derived()->visit(types_def);
  }

  bool traverse(const Constant &constant) {
    return get_derived()->visit(constant);
  }

  bool traverse(const ConstantList &constant_list) {
    if (!traverse_(constant_list.constants)) {
      return false;
    }
    return get_derived()->visit(constant_list);
  }

  bool traverse(const ConstantsDef &constants_def) {
    if (!traverse_(constants_def.constant_lists)) {
      return false;
    }
    return get_derived()->visit(constants_def);
  }

  bool traverse(const Predicate &predicate) {
    if (!traverse_(predicate.parameters)) {
      return false;
    }
    return get_derived()->visit(predicate);
  }

  bool traverse(const Parameter &parameter) {
    return get_derived()->visit(parameter);
  }

  bool traverse(const ParameterList &parameter_list) {
    if (!traverse_(parameter_list.parameters)) {
      return false;
    }
    return get_derived()->visit(parameter_list);
  }

  bool traverse(const PredicatesDef &predicates_def) {
    if (!traverse_(predicates_def.predicates)) {
      return false;
    }
    return get_derived()->visit(predicates_def);
  }

  bool traverse(const Condition &condition) {
    if (const PredicateEvaluation *predeval_ptr =
            dynamic_cast<const PredicateEvaluation *>(&condition);
        predeval_ptr != nullptr) {
      if (!get_derived()->traverse(*predeval_ptr)) {
        return false;
      }
    } else if (const CompoundCondition *compcond_ptr =
                   dynamic_cast<const CompoundCondition *>(&condition);
               compcond_ptr != nullptr) {
      if (!get_derived()->traverse(*compcond_ptr)) {
        return false;
      }
    }
    return get_derived()->visit(condition);
  }

  bool traverse(const Argument &argument) {
    return get_derived()->visit(argument);
  }

  bool traverse(const PredicateEvaluation &predicate) {
    if (!traverse_(predicate.arguments)) {
      return false;
    }
    return get_derived()->visit(predicate);
  }

  bool traverse(const CompoundCondition &condition) {
    if (const Conjunction *and_ptr =
            dynamic_cast<const Conjunction *>(&condition);
        and_ptr != nullptr) {
      if (!get_derived()->traverse(*and_ptr)) {
        return false;
      }
    } else if (const Disjunction *or_ptr =
                   dynamic_cast<const Disjunction *>(&condition);
               or_ptr != nullptr) {
      if (!get_derived()->traverse(*or_ptr)) {
        return false;
      }
    } else if (const Negation *not_ptr =
                   dynamic_cast<const Negation *>(&condition);
               not_ptr != nullptr) {
      if (!get_derived()->traverse(*not_ptr)) {
        return false;
      }
    }
    return get_derived()->visit(condition);
  }

  bool traverse(const Conjunction &conjunction) {
    if (!traverse_(conjunction.conditions)) {
      return false;
    }
    return get_derived()->visit(conjunction);
  }

  bool traverse(const Disjunction &disjunction) {
    if (!traverse_(disjunction.conditions)) {
      return false;
    }
    return get_derived()->visit(disjunction);
  }

  bool traverse(const Negation &negation) {
    if (!get_derived()->traverse(*negation.condition)) {
      return false;
    }
    return get_derived()->visit(negation);
  }

  bool traverse(const ActionDef &action_def) {
    if (!traverse_(action_def.parameters)) {
      return false;
    }
    if (!get_derived()->traverse(*action_def.precondition)) {
      return false;
    }
    if (!get_derived()->traverse(*action_def.effect)) {
      return false;
    }
    return get_derived()->visit(action_def);
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
  bool visit(const CompoundCondition &) { return true; }
  bool visit(const Conjunction &) { return true; }
  bool visit(const Disjunction &) { return true; }
  bool visit(const Negation &) { return true; }
  bool visit(const ActionDef &) { return true; }

  virtual ~Visitor() {}

private:
  template <typename Container> bool traverse_(const Container &container) {
    for (auto &c : container) {
      if (!get_derived()->traverse(*c)) {
        return false;
      }
    }
    return true;
  }
};

} // namespace ast

} // namespace parser

#endif /* end of include guard: VISITOR_H */
