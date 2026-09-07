cmake_minimum_required(VERSION 3.20)
project(ExoticCoreRuntime LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(exotic_core
  src/runtime.cpp
)
target_include_directories(exotic_core PUBLIC include)

add_executable(exotic_core_demo app/main.cpp)
target_link_libraries(exotic_core_demo PRIVATE exotic_core)

include(CTest)
if(BUILD_TESTING)
  add_executable(exotic_core_tests tests/runtime_tests.cpp)
  target_link_libraries(exotic_core_tests PRIVATE exotic_core)
  add_test(NAME exotic_core_tests COMMAND exotic_core_tests)
endif()
