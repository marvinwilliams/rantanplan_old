#ifndef BUILDER_H
#define BUILDER_H

#include "../parser/parser.hxx"
//#include "model.h"
#include <exception>
#include <sstream>

namespace model {

class Builder {
public:
  struct semantic_error : public std::runtime_error {
    semantic_error(const parser::Parser::location_type &l, const std::string &m)
        : std::runtime_error{m}, location{l} {}

    parser::Parser::location_type location;
  };

  Builder(const std::string &domain, const std::string &problem);

  void add_requirement(const std::string &requirement,
                       const parser::Parser::location_type &location);
  /* void add_type(const std::string &name, const std::string &supertype); */
  /* void add_constant(const std::string &name, const std::string &type); */
  /* void add_predicate( */
  /*     const std::string &name, */
  /*     const std::vector<std::pair<std::vector<std::string>, std::string>> */
  /*         parameter_list); */

  /* parser::Parser::location_type *current_location; */

private:
  std::string domain_name_;
  std::string problem_name_;
  /* std::vector<model::Type> types_; */
  /* std::vector<model::Constant> constants_; */
  /* std::vector<model::Predicate> predicates_; */
  /* std::vector<model::Operator> operators; */
  /* std::vector<model::Condition> initialState; */
  /* model::Condition goals; */
};

} // namespace model

#endif /* end of include guard: BUILDER_H */
