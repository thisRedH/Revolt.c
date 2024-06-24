cmake_minimum_required(VERSION 3.10)

project(playground VERSION 0.1.0)

include_directories(${CMAKE_CURRENT_LIST_DIR})
file(GLOB_RECURSE SOURCES main.c)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(Revolt.c REQUIRED)
target_link_libraries(${PROJECT_NAME} Revolt.c::revolt)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:Revolt.c::revolt> $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
