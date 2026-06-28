cmake_minimum_required(VERSION 3.20)

project(Exotic VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(exotic_runtime
    runtime/src/Clock.cpp
    runtime/src/Event.cpp
    runtime/src/EventBus.cpp
    runtime/src/Runtime.cpp
    runtime/src/Scheduler.cpp
    runtime/src/Task.cpp
)

target_include_directories(exotic_runtime PUBLIC
    runtime/include
)

add_executable(hello_runtime
    runtime/examples/HelloRuntime.cpp
)

target_link_libraries(hello_runtime PRIVATE exotic_runtime)
