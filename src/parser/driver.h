#ifndef DRIVER_H
#define DRIVER_H

#include "parser.hxx"
#include <fstream>
#include <memory>
#include <string>

namespace parser {

class Scanner;

class Driver {
public:
  Driver();

  int parse(std::string domain_in, std::string problem_in);

  Scanner *scanner;
};

} // namespace parser

#endif /* end of include guard: DRIVER_H */
