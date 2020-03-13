cmake_minimum_required(VERSION 3.13)
file(GLOB_RECURSE files "src/*.cpp")

project(utilities)

add_library(utilities OBJECT  ${files})
target_include_directories(utilities PUBLIC include)