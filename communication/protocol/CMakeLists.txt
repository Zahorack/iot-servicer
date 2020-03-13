cmake_minimum_required(VERSION 3.13)
file(GLOB_RECURSE files "src/*.cpp")

project(protocol)

add_library(protocol OBJECT  ${files})
target_include_directories(protocol PUBLIC include)

target_link_libraries(protocol utilities)


