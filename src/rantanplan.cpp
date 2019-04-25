#include "parser/driver.h"
#include "config.h"

int main(int argc, char *argv[])
{
  parser::Driver driver(argv[1], argv[2]);
  driver.parse();
  return 0;
}
