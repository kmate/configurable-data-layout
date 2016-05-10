cmake_minimum_required(VERSION 2.8)

project(MacroConfig)

set(FLAGS "-std=c99 -Wall")
find_package(Boost 1.54 REQUIRED)
set(DEFINITIONS "-DBOOST_PP_VARIADICS=1")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}")
add_definitions(${DEFINITIONS})
include_directories(${INCLUDE_DIRS})

add_custom_target(macro_config SOURCES macro_config.h)

add_executable(alloc alloc.c)
add_executable(hash hash.c)
add_executable(struct struct.c)
add_executable(struct_named_config struct_named_config.c)
add_executable(vla vla.c)

macro(add_test_command sourceName)

add_custom_command(
  OUTPUT ${sourceName}.out
  COMMAND "${CMAKE_C_COMPILER}" -E -P "${CMAKE_SOURCE_DIR}/${sourceName}.c"
    -I "${INCLUDE_DIRS}" ${DEFINITIONS} > ${sourceName}.out
  COMMAND "${CMAKE_COMMAND}" -E compare_files "${CMAKE_SOURCE_DIR}/${sourceName}.out.expected" ${sourceName}.out
  MAIN_DEPENDENCY "${CMAKE_SOURCE_DIR}/${sourceName}.c"
  COMMENT "Preprocessing ${sourceName}.c"
  VERBATIM
)

endmacro(add_test_command)

add_test_command(test)
add_test_command(test_named_config)

add_custom_target(
  run_tests ALL
  DEPENDS test.out test_named_config.out
  VERBATIM
)
