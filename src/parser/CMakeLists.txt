find_package(FLEX)
find_package(BISON)

set(bison_compile_flags
  -Dapi.location.file="${CMAKE_CURRENT_BINARY_DIR}/location.hxx"
  )

set(bison_compile_flags
  "${bison_compile_flags} --warnings=all"
  )

FLEX_TARGET(scanner scanner.ll "${CMAKE_CURRENT_BINARY_DIR}/scanner.cxx")
BISON_TARGET(parser parser.yy "${CMAKE_CURRENT_BINARY_DIR}/parser.cxx"
  DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/parser.hxx"
  COMPILE_FLAGS ${bison_compile_flags})


ADD_FLEX_BISON_DEPENDENCY(scanner parser)

add_library(parser STATIC ${FLEX_scanner_OUTPUTS} ${BISON_parser_OUTPUTS})

target_include_directories(parser PRIVATE ".")
target_include_directories(parser PRIVATE "ast")
target_include_directories(parser PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

