cmake_minimum_required(VERSION 3.10)

project(Sandbox)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Sandbox ${SOURCES})

target_link_libraries(Sandbox PRIVATE Argless)
