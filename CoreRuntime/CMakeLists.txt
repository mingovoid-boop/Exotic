cmake_minimum_required(VERSION 3.20)
project(ExoticCoreRuntime LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(exotic_core
  src/runtime.cpp
  src/crypto.cpp
  src/status_api.cpp
  src/free_agent.cpp
)
target_include_directories(exotic_core PUBLIC include)

add_executable(exotic_core_demo app/main.cpp)
target_link_libraries(exotic_core_demo PRIVATE exotic_core)

add_executable(exotic_core_status app/status_server.cpp)
target_link_libraries(exotic_core_status PRIVATE exotic_core)
if(WIN32)
  target_link_libraries(exotic_core_status PRIVATE ws2_32)
endif()

include(CTest)
if(BUILD_TESTING)
  add_executable(exotic_core_tests tests/runtime_tests.cpp)
  target_link_libraries(exotic_core_tests PRIVATE exotic_core)
  add_test(NAME exotic_core_tests COMMAND exotic_core_tests)

  add_executable(exotic_status_api_tests tests/status_api_tests.cpp)
  target_link_libraries(exotic_status_api_tests PRIVATE exotic_core)
  add_test(NAME exotic_status_api_tests COMMAND exotic_status_api_tests)

  add_executable(exotic_free_agent_tests tests/free_agent_tests.cpp)
  target_link_libraries(exotic_free_agent_tests PRIVATE exotic_core)
  add_test(NAME exotic_free_agent_tests COMMAND exotic_free_agent_tests)
endif()
