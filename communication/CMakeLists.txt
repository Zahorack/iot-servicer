cmake_minimum_required(VERSION 3.13)

project(communication)

add_subdirectory(socket)
add_subdirectory(protocol)

file(GLOB_RECURSE files "src/*.cpp")
add_library(communication OBJECT ${files})
target_include_directories(communication PUBLIC include)

target_link_libraries(communication socket)
target_link_libraries(communication protocol)
