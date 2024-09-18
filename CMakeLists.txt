cmake_minimum_required(VERSION 3.10)

project(Revolt.c LANGUAGES C)

option(REVOLT_BUILD_SHARED "Build shared libraries." OFF)
message(STATUS "${PROJECT_NAME}: REVOLT_BUILD_SHARED ${REVOLT_BUILD_SHARED}")

# Sources
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/*.h)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/ext)

set(CMAKE_C_FLAGS "-pedantic-errors -ansi -Wextra -Wall -Wuninitialized")

# Lib
if(${REVOLT_BUILD_SHARED})
    set(BULD_LIB_NAME revolt)
    add_library(${BULD_LIB_NAME} SHARED ${SOURCES} ${HEADERS})
    set_property(TARGET ${BULD_LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
else()
    set(BULD_LIB_NAME revolt_static)
    add_library(${BULD_LIB_NAME} STATIC ${SOURCES} ${HEADERS})
endif()
add_library(${PROJECT_NAME}::${BULD_LIB_NAME} ALIAS ${BULD_LIB_NAME})

set_target_properties(
    ${BULD_LIB_NAME} PROPERTIES
    C_STANDARD 90
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
    C_VISIBILITY_PRESET hidden
)

target_compile_definitions(${BULD_LIB_NAME} PRIVATE REVOLTC_BUILD)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${BULD_LIB_NAME} PRIVATE Threads::Threads)

find_package(CURL REQUIRED)
target_link_libraries(${BULD_LIB_NAME} PUBLIC CURL::libcurl)

find_package(cJSON REQUIRED)
target_link_libraries(${BULD_LIB_NAME} PRIVATE cjson)

# Config
set(TARGETS_EXPORT_NAME ${PROJECT_NAME}Targets)
set(CONFIG_NAME ${PROJECT_NAME}Config)
set(CONFIG_VERSION_NAME ${PROJECT_NAME}ConfigVersion)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${CONFIG_NAME}.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_NAME}.cmake"
    INSTALL_DESTINATION lib/cmake/revolt
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_VERSION_NAME}.cmake"
    VERSION 0.1.0 #TODO ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Install
install(
    DIRECTORY include/revolt
    DESTINATION include
)

install(
    DIRECTORY ext/pstdint
    DESTINATION include
)

install(
    DIRECTORY ext/hedley
    DESTINATION include
)

install(
    TARGETS
        ${BULD_LIB_NAME}
    EXPORT ${TARGETS_EXPORT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_NAME}.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_VERSION_NAME}.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)

install(
    EXPORT ${TARGETS_EXPORT_NAME}
    FILE ${TARGETS_EXPORT_NAME}.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)
