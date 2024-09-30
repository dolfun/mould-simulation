cmake_minimum_required(VERSION 3.10)
project(mould-sim)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(extern)
add_subdirectory(src)