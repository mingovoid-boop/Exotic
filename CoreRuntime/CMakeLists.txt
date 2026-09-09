cmake_minimum_required(VERSION 3.20)
project(ExoticCoreRuntime LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  sqlite_amalgamation
  URL https://www.sqlite.org/2026/sqlite-amalgamation-3530400.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(sqlite_amalgamation)

add_library(exotic_sqlite STATIC
  ${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c
)
target_include_directories(exotic_sqlite PUBLIC ${sqlite_amalgamation_SOURCE_DIR})
target_compile_definitions(exotic_sqlite PRIVATE SQLITE_THREADSAFE=1 SQLITE_OMIT_LOAD_EXTENSION=1)

add_library(exotic_core
  src/runtime.cpp
  src/free_agent.cpp
  src/free_agent_service.cpp
  src/agent_store.cpp
  src/idle_cognition.cpp
  src/operator_sessions.cpp
)
target_include_directories(exotic_core PUBLIC include)
target_link_libraries(exotic_core PUBLIC exotic_sqlite)

add_executable(exotic_core_demo app/main.cpp)
target_link_libraries(exotic_core_demo PRIVATE exotic_core)

add_executable(exotic_free_agent_server app/free_agent_server.cpp)
target_link_libraries(exotic_free_agent_server PRIVATE exotic_core)
if(WIN32)
  target_link_libraries(exotic_free_agent_server PRIVATE ws2_32)
endif()

include(CTest)
if(BUILD_TESTING)
  add_executable(exotic_core_tests tests/runtime_tests.cpp)
  target_link_libraries(exotic_core_tests PRIVATE exotic_core)
  add_test(NAME exotic_core_tests COMMAND exotic_core_tests)

  add_executable(exotic_free_agent_tests tests/free_agent_tests.cpp)
  target_link_libraries(exotic_free_agent_tests PRIVATE exotic_core)
  add_test(NAME exotic_free_agent_tests COMMAND exotic_free_agent_tests)

  add_executable(exotic_free_agent_service_tests tests/free_agent_service_tests.cpp)
  target_link_libraries(exotic_free_agent_service_tests PRIVATE exotic_core)
  add_test(NAME exotic_free_agent_service_tests COMMAND exotic_free_agent_service_tests)

  add_executable(exotic_agent_store_tests tests/agent_store_tests.cpp)
  target_link_libraries(exotic_agent_store_tests PRIVATE exotic_core)
  add_test(NAME exotic_agent_store_tests COMMAND exotic_agent_store_tests)
endif()
