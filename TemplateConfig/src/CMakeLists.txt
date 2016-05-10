cmake_minimum_required(VERSION 2.8)

project(TemplateConfig)

set(FLAGS "-std=c++11 -Wall")
find_package(Boost 1.54 REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
include_directories(${INCLUDE_DIRS})

add_custom_target(template_config SOURCES template_config.hpp)

add_executable(demo demo.cpp)
add_executable(hash hash.cpp)
add_executable(init init.cpp)
add_executable(struct struct.cpp)
add_executable(test test.cpp)
