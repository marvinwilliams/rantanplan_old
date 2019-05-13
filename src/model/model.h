#ifndef MODEL_H
#define MODEL_H

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

} // namespace model
#endif /* end of include guard: MODEL_H */
