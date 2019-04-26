#include <iostream>
#include "parser/driver.h"
#include "config.h"

int main(int argc, char *argv[])
{
  std::cout << "This is rantanplan version " << VERSION_MAJOR << "." << VERSION_MINOR << '\n';
  if (argc < 3) {
    std::cerr << "Please provide domain and problem\n";
    return 1;
  }
  parser::Driver driver(argv[1], argv[2]);
  driver.parse();
  return 0;
}
