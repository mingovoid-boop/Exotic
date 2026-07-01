file(GLOB_RECURSE EXOTIC_SOURCES foundation/src/*.cpp runtime/src/*.cpp graph/src/*.cpp memory/src/*.cpp observer/src/*.cpp alignment/src/*.cpp prediction/src/*.cpp execution/src/*.cpp learning/src/*.cpp relationship/src/*.cpp)

add_library(Exotic STATIC ${EXOTIC_SOURCES})

target_include_directories(Exotic PUBLIC foundation/include runtime/include graph/include memory/include observer/include alignment/include prediction/include execution/include learning/include relationship/include sdk/include)

add_executable(HelloExotic ${CMAKE_SOURCE_DIR}/sdk/examples/HelloExotic.cpp)
target_link_libraries(HelloExotic PRIVATE Exotic)

