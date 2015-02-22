cmake_minimum_required (VERSION 2.8.1)
cmake_policy(SET CMP0042 NEW)

include_directories (../include)

project (tunnel_test CXX)

file (GLOB headers "*.h")
file (GLOB sources "*.c")

set (DIRS "src")

foreach (dir ${DIRS})
   file (GLOB h_${dir} "${dir}/*.h")
   file (GLOB s_${dir} "${dir}/*.cpp")
   source_group (${dir} FILES ${s_${dir}} ${h_${dir}})
   set (sources ${sources} ${s_${dir}})
   set (header ${headers} ${h_${dir}})
endforeach ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu1y")
endif()

add_executable (tunnel_test ${sources})

target_link_libraries (tunnel_test)

set_target_properties(tunnel_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target"
)