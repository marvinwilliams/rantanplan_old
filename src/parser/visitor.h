#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"

namespace parser {

namespace ast {

template <typename Derived>
class Visitor {
 public:
  Derived *get_derived() { return static_cast<Derived *>(this); }

  bool traverse(const AST &ast);
  bool traverse(const Domain &Domain);
  bool traverse(const Domain::Element &element);
  bool traverse(const RequirementsDef &requirements_def);
  bool traverse(const Requirement &);
  bool traverse(const Type &);
  bool traverse(const TypeList &);
  bool traverse(const TypesDef &);
  bool traverse(const Constant &);
  bool traverse(const ConstantsDef &);
  bool traverse(const PredicatesDef &);
  bool traverse(const ActionDef &);

  bool visit(const AST &) { return true; }
  bool visit(const Domain &) { return true; }
  bool visit(const Domain::Element &) { return true; }
  bool visit(const RequirementsDef &) { return true; }
  bool visit(const Requirement &) { return true; }
  bool visit(const Type &) { return true; }
  bool visit(const TypeList &) { return true; }
  bool visit(const Constant &) { return true; }

  struct DomainBodyVisitor {
    DomainBodyVisitor(Derived *visitor) : visitor{visitor} {}

    template <typename E>
    bool operator()(const E &elem) {
      return visitor->traverse(elem);
    }

    Derived *visitor;
  };

  virtual ~Visitor() {}
};

template <typename Derived>
bool Visitor<Derived>::traverse(const AST &ast) {
  if (!get_derived()->traverse(*ast.domain.get())) {
    return false;
  }
  return get_derived()->visit(ast);
}

template <typename Derived>
bool Visitor<Derived>::traverse(const Domain &domain) {
  for (auto &elem : domain.domain_body) {
    if (!get_derived()->traverse(*elem)) {
      return false;
    }
  }
  return get_derived()->visit(domain);
}

template <typename Derived>
bool Visitor<Derived>::traverse(const Domain::Element &element) {
  if (!std::visit(Visitor<Derived>::DomainBodyVisitor{get_derived()},
                  element)) {
    return false;
  }
  return get_derived()->visit(element);
}

template <typename Derived>
bool Visitor<Derived>::traverse(const RequirementsDef &requirements_def) {
  for (auto &r : requirements_def.requirements) {
    if (!get_derived()->traverse(*r)) {
      return false;
    }
  }
  return get_derived()->visit(requirements_def);
}

template <typename Derived>
bool Visitor<Derived>::traverse(const Requirement &requirement) {
  return get_derived()->visit(requirement);
}

template <typename Derived>
bool Visitor<Derived>::traverse(const Type &type) {
  return get_derived()->visit(type);
}

template <typename Derived>
bool Visitor<Derived>::traverse(const TypeList &type_list) {
  for (auto &t : type_list.types) {
    if (!get_derived()->traverse(t)) {
      return false;
    }
  }
  return get_derived()->visit(type_list);
}

template <typename Derived>
bool Visitor<Derived>::traverse(const Constant &constant) {
  return get_derived()->visit(constant);
}

template <typename Derived>
bool Visitor<Derived>::traverse(const PredicatesDef &action) {
  return true;
}

template <typename Derived>
bool Visitor<Derived>::traverse(const ConstantsDef &action) {
  return true;
}

template <typename Derived>
bool Visitor<Derived>::traverse(const TypesDef &action) {
  return true;
}

template <typename Derived>
bool Visitor<Derived>::traverse(const ActionDef &action) {
  return true;
}

}  // namespace ast

}  // namespace parser

#endif /* end of include guard: VISITOR_H */
