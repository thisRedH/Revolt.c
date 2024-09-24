cmake_minimum_required(VERSION 3.10)

project(playground VERSION 0.1.0)

include_directories(${CMAKE_CURRENT_LIST_DIR})
file(GLOB_RECURSE SOURCES main.c)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(Revolt.c REQUIRED)
target_link_libraries(${PROJECT_NAME} Revolt.c::revoltc)
