#include "builder.h"

namespace model {

Builder::Builder(const std::string &domain, const std::string &problem)
    : domain_name_{domain}, problem_name_{problem} {}

void Builder::add_requirement(const std::string &requirement,
                         const parser::Parser::location_type &location) {
  throw semantic_error(location, "semantic error, invalid requirement");
}
} // namespace model
