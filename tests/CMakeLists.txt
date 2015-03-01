cmake_minimum_required (VERSION 2.8.1)

if(POLICY CMP0042)
cmake_policy(SET CMP0042 NEW)
endif()

include_directories (../include)

project (tunnel_test CXX)

file (GLOB headers "*.h")
file (GLOB sources "*.cpp")

set (DIRS "src")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1y")
endif()

add_executable (tunnel_simple src/simple.cpp)
add_executable (tunnel_parallel src/parallel.cpp)
add_executable (tunnel_iter src/iter.cpp)
add_executable (tunnel_iterator src/iterator.cpp)
add_executable (tunnel_closed src/close.cpp)

target_link_libraries (tunnel_simple)
target_link_libraries (tunnel_iter)
target_link_libraries (tunnel_iterator)
target_link_libraries (tunnel_closed)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries (tunnel_parallel pthread)
else()
    target_link_libraries (tunnel_parallel)
endif()


set_target_properties(tunnel_simple
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target"
)

set_target_properties(tunnel_parallel
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target"
)

set_target_properties(tunnel_iter
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target"
)

set_target_properties(tunnel_iterator
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target"
)

set_target_properties(tunnel_closed
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target"
)