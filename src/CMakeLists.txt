configure_file(
  "config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h"
  )

add_executable(rantanplan
  rantanplan.cpp
  )

target_include_directories(rantanplan PRIVATE ".")
target_include_directories(rantanplan PRIVATE "parser")
target_include_directories(rantanplan PRIVATE "model")
target_include_directories(rantanplan PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(rantanplan PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/parser")

add_subdirectory("parser")

target_link_libraries(rantanplan parser)

install(TARGETS rantanplan DESTINATION "${PROJECT_SOURCE_DIR}/bin")
