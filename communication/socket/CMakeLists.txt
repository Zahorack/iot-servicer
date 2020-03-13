cmake_minimum_required(VERSION 3.13)
file(GLOB_RECURSE files "src/*.cpp")

project(socket)

add_library(socket OBJECT  ${files})
target_include_directories(socket PUBLIC include)

target_link_libraries(socket ws2_32)
if(WIN32)
    target_link_libraries(socket wsock32 ws2_32)
endif()
