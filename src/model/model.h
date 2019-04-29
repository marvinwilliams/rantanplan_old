#ifndef MODEL_HH
#define MODEL_HH

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace model {

struct Type {
  std::string name;
  unsigned int id;
  const Type *supertype = nullptr;
};

struct Constant {
  std::string name;
  unsigned int id;
  const Type *type = nullptr;
};

struct Variable {
  std::string name;
  const Type *type = nullptr;
};

struct Argument {
  std::string name;
};

struct Predicate {
  std::string name;
  std::vector<Variable> param_list;
  unsigned int id;
};

struct AtomicCondition {
  const Predicate *predicate;
  std::vector<Argument> argument_list;
  bool negated = false;
};

struct ConditionConjunction;
struct ConditionDisjunction;
struct ConditionNot;
struct ConditionImply;
struct ConditionExists;
struct ConditionForall;
struct ConditionWhen;

using Condition =
    std::variant<AtomicCondition, ConditionConjunction, ConditionDisjunction,
                 ConditionNot, ConditionImply, ConditionExists,
                 ConditionForall, ConditionWhen>;

struct ConditionConjunction {
  std::vector<Condition> condition_list;
};

struct ConditionDisjunction {
  std::vector<Condition> condition_list;
};

struct ConditionNot {
  Condition condition;
};

struct ConditionImply {
  Condition premise;
  Condition conclusion;
};

struct ConditionExists {
  std::vector<Variable> variable_list;
  Condition condition;
};

struct ConditionForall {
  std::vector<Variable> variable_list;
  Condition condition;
};

struct ConditionWhen {
  Condition test_condition;
  Condition effect;
};

struct Operator {
  std::string name;
  std::vector<Variable> param_list;
  Precondition precondition;
  Effect effect;
  unsigned int id;
};

class predicate {
 public:
  predicate(const std::string &name,
            const std::vector<std::pair<std::vector<std::string>, std::string>>
                &parameters)
      : name_(name), parameters_(std::move(parameters)) {}

 private:
  std::string name_;
  std::vector<std::pair<std::vector<std::string>, std::string>> parameters_;
};

class domain {
 public:
  domain(const std::string &name) : name_(name) {}

  void set_requirements(const std::vector<std::string> &requirements) {
    requirements_ = std::move(requirements);
  }

  void set_types(
      const std::vector<std::pair<std::vector<std::string>, std::string>>
          &types) {
    types_ = std::move(types);
  }

  void set_constants(
      const std::vector<std::pair<std::vector<std::string>, std::string>>
          &constants) {
    constants_ = std::move(constants);
  }

  void set_predicates(const std::vector<predicate> &predicates) {
    predicates_ = std::move(predicates);
  }

 private:
  std::string name_;
  std::vector<std::string> requirements_;
  std::vector<std::pair<std::vector<std::string>, std::string>> types_;
  std::vector<std::pair<std::vector<std::string>, std::string>> constants_;
  std::vector<predicate> predicates_;
};

struct condition {
  std::string predicate;
  std::vector<std::string> parameters;
  bool negated;
};

class action {
 public:
  action(const std::string &name,
         const std::vector<std::pair<std::vector<std::string>, std::string>>
             &parameters)
      : name_(name), parameters_(std::move(parameters)) {}
  action() {}

  void set_precondition(const std::vector<condition> &precondition) {
    precondition_ = std::move(precondition);
  }

  void set_effect(const std::vector<condition> &effect) {
    effect_ = std::move(effect);
  }

 private:
  std::string name_;
  std::vector<std::pair<std::vector<std::string>, std::string>> parameters_;
  std::vector<condition> precondition_;
  std::vector<condition> effect_;
};

class problem {
 public:
  problem() {}

 private:
};

}  // namespace model
#endif /* end of include guard: MODEL_HH */
