cmake_minimum_required(VERSION 3.10)
project(Revolt.c LANGUAGES C)

function(revolt_have_h target header define)
    include(CheckIncludeFiles)

    check_include_files(${header} ${define})
    if (${define})
        set(${define} ON)
        target_compile_definitions(${target} PRIVATE ${define}=1)
    endif()
endfunction()

function(revolt_have_fn target fn define)
    include(CheckFunctionExists)

    check_function_exists(${fn} ${define})
    if (${define})
        set(${define} ON)
        target_compile_definitions(${target} PRIVATE ${define}=1)
    endif()
endfunction()

# Sources
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/*.h)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/ext)

set(LIB_NAME revoltc)
option(REVOLT_BUILD_SHARED "Build shared libraries" ON)
option(REVOLT_BUILD_STATIC "Build static libraries" OFF)

# Shared Lib
if(${REVOLT_BUILD_SHARED} OR NOT ${REVOLT_BUILD_STATIC})
    set(REVOLT_BUILD_SHARED ON)
    set(LIB_SHARED ${LIB_NAME}_shared)
    list(APPEND BUILD_LIBS ${LIB_SHARED})

    add_library(${LIB_SHARED} SHARED ${SOURCES} ${HEADERS})
    add_library(${PROJECT_NAME}::${LIB_SHARED} ALIAS ${LIB_SHARED})
    set_property(TARGET ${LIB_SHARED} PROPERTY POSITION_INDEPENDENT_CODE 1)

    add_library(${PROJECT_NAME}::${LIB_NAME} ALIAS ${LIB_SHARED})
endif()

# Static Lib
if(${REVOLT_BUILD_STATIC} OR NOT ${REVOLT_BUILD_SHARED})
    set(REVOLT_BUILD_STATIC ON)
    set(LIB_STATIC ${LIB_NAME}_static)
    list(APPEND BUILD_LIBS ${LIB_STATIC})

    add_library(${LIB_STATIC} STATIC ${SOURCES} ${HEADERS})
    add_library(${PROJECT_NAME}::${LIB_STATIC} ALIAS ${LIB_STATIC})

    if(NOT ${REVOLT_BUILD_SHARED})
        add_library(${PROJECT_NAME}::${LIB_NAME} ALIAS ${LIB_STATIC})
    endif()
endif()

foreach(BULD_LIB IN LISTS BUILD_LIBS)
    target_compile_definitions(${BULD_LIB} PRIVATE REVOLTC_BUILD=1)

    option(REVOLT_BUILD_STRICT "Force C89/C90 without extensions" OFF)
    if(REVOLT_BUILD_STRICT)
        set_target_properties(
            ${BULD_LIB} PROPERTIES
            C_STANDARD 90
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
            C_VISIBILITY_PRESET hidden
        )
    else()
        revolt_have_h(${BULD_LIB} stddef.h HAVE_STDDEF_H)
        revolt_have_h(${BULD_LIB} stdint.h HAVE_STDINT_H)
        revolt_have_h(${BULD_LIB} time.h HAVE_TIME_H)
        revolt_have_h(${BULD_LIB} unistd.h HAVE_UNISTD_H)
        revolt_have_fn(${BULD_LIB} snprintf HAVE_SNPRINTF)
        revolt_have_fn(${BULD_LIB} vsprintf HAVE_VSPRINTF)
        revolt_have_fn(${BULD_LIB} vsnprintf HAVE_VSNPRINTF)

        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            if(UNIX)
                target_compile_definitions(${BULD_LIB} PRIVATE _POSIX_C_SOURCE=199506L)
                target_compile_definitions(${BULD_LIB} PRIVATE _XOPEN_SOURCE=500)
            endif()
            set_target_properties(
                ${BULD_LIB} PROPERTIES
                C_STANDARD 90
                C_STANDARD_REQUIRED ON
                C_EXTENSIONS ON
                C_VISIBILITY_PRESET hidden
            )
        elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
            if(UNIX)
                target_compile_definitions(${BULD_LIB} PRIVATE _POSIX_C_SOURCE=200809L)
                target_compile_definitions(${BULD_LIB} PRIVATE _XOPEN_SOURCE=600)
            endif()
            set_target_properties(
                ${BULD_LIB} PROPERTIES
                C_STANDARD 11
                C_STANDARD_REQUIRED OFF
                C_EXTENSIONS ON
                C_VISIBILITY_PRESET hidden
            )
        else()
            message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
        endif()
    endif()

    # pthread
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${BULD_LIB} PRIVATE Threads::Threads)

    # CURL
    find_package(CURL REQUIRED)
    target_link_libraries(${BULD_LIB} PUBLIC CURL::libcurl)

    # JSON Lib
    option(REVOLT_WITH_CJSON "Use cJSON library" OFF)
    option(REVOLT_WITH_YYJSON "Use yyjson library" OFF)

    if(REVOLT_WITH_CJSON AND REVOLT_WITH_YYJSON)
        message(FATAL_ERROR "You cannot use multiple JSON librarys. Please choose only one.")
    elseif(NOT REVOLT_WITH_CJSON AND NOT REVOLT_WITH_YYJSON)
        set(REVOLT_WITH_CJSON ON)
    endif()

    if(REVOLT_WITH_YYJSON)
        find_package(yyjson REQUIRED)
        target_link_libraries(${BULD_LIB} PRIVATE yyjson)
        target_compile_definitions(${BULD_LIB} PRIVATE REVOLTC_WITH_YYJSON=1)
    else()
        find_package(cJSON REQUIRED)
        target_link_libraries(${BULD_LIB} PRIVATE cjson)
        target_compile_definitions(${BULD_LIB} PRIVATE REVOLTC_WITH_CJSON=1)
    endif()
endforeach()

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
    DIRECTORY ext/hedley
    DESTINATION include
)

if(NOT HAVE_STDINT_H)
install(
    DIRECTORY ext/pstdint
    DESTINATION include
)
endif()

install(
    TARGETS
        ${LIB_SHARED}
        ${LIB_STATIC}
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

message(STATUS "${PROJECT_NAME} Config")
message(STATUS "${PROJECT_NAME}: CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")
message(STATUS "${PROJECT_NAME}: REVOLT_BUILD_STRICT ${REVOLT_BUILD_STRICT}")
message(STATUS "${PROJECT_NAME}: REVOLT_BUILD_SHARED ${REVOLT_BUILD_SHARED}")
message(STATUS "${PROJECT_NAME}: REVOLT_BUILD_STATIC ${REVOLT_BUILD_STATIC}")
message(STATUS "${PROJECT_NAME}: REVOLT_WITH_CJSON ${REVOLT_WITH_CJSON}")
message(STATUS "${PROJECT_NAME}: REVOLT_WITH_YYJSON ${REVOLT_WITH_YYJSON}")
