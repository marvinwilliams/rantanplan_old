#ifndef PROBLEM_BUILDER_H
#define PROBLEM_BUILDER_H

#include <exception>
#include <sstream>
#include "../model.h"
#include "location.hh"

class SemanticException : public std::runtime_error {
 public:
  SemanticException(const std::string& msg, const yy::location& loc)
      : std::runtime_error{msg}, loc_{loc} {}
  virtual const char* what() const noexcept {
    std::stringstream stream;
    stream << loc_ << ": " << std::runtime_error::what() << '\n';
    return stream.str().c_str();
  }

 private:
  std::string msg_;
  yy::location loc_;
};

class ProblemBuilder {
 public:
  ProblemBuilder(const std::string& domain, const std::string& problem);

  void add_requirement(const std::string& requirement);
  void add_type(const std::string& name, const std::string& supertype);
  void add_constant(const std::string& name, const std::string& type);
  void add_predicate(
      const std::string& name,
      const std::vector<std::pair<std::vector<std::string>, std::string>>
          parameter_list);

  yy::location* current_location;

 private:
  std::string domain_name_;
  std::string problem_name_;
  std::vector<model::Type> types_;
  std::vector<model::Constant> constants_;
  std::vector<model::Predicate> predicates_;
  std::vector<model::Operator> operators;
  std::vector<model::Condition> initialState;
  model::Condition goals;
};

#endif /* end of include guard: PROBLEM_BUILDER_H */
