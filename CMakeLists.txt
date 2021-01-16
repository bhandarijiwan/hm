cmake_minimum_required(VERSION 3.15.2)

set(project hm)

project(
  ${project}
  VERSION 0.1
  DESCRIPTION "An example project with CMake"
  LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 17)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.10.0
)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 7.0.3
)

FetchContent_MakeAvailable(gtest)
FetchContent_MakeAvailable(fmt)

set(headers lib.h)
set(sources lib.cpp)
add_library(${project} STATIC ${sources} ${headers})
target_link_libraries(${project} PRIVATE fmt::fmt) 
target_compile_options(${project} PRIVATE -Werror -Wall -Wextra)


set(test_project "${project}-test")
set(test_sources libtest.cpp)
add_executable(${test_project} ${test_sources})
target_link_libraries(${test_project} PRIVATE gtest_main gmock_main ${project} fmt::fmt)

add_test(NAME ${test_project} COMMAND ${test_project})