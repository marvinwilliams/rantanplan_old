#ifndef DRIVER_H
#define DRIVER_H

#include <fstream>
#include <string>
#include "location.hxx"

namespace parser {

class Scanner;

class Driver {
 public:
  Driver(const std::string& domain_file, const std::string& problem_file);

  int parse();

  location loc;
  std::string domain_file;
  std::string problem_file;
  Scanner* scanner;
};

}  // namespace parser

#endif /* end of include guard: DRIVER_H */
