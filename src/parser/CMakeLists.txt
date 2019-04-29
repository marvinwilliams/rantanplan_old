find_package(FLEX)
find_package(BISON)

FLEX_TARGET(scanner scanner.ll "${CMAKE_CURRENT_BINARY_DIR}/scanner.cxx")
BISON_TARGET(parser parser.yy "${CMAKE_CURRENT_BINARY_DIR}/parser.cxx"
  DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/parser.hxx")

ADD_FLEX_BISON_DEPENDENCY(scanner parser)

add_library(parser STATIC ${FLEX_scanner_OUTPUTS} ${BISON_parser_OUTPUTS} ../model/builder.h ../model/builder.cpp)

target_include_directories(parser PRIVATE ".")
target_include_directories(parser PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

