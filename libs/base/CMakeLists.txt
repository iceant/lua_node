cmake_minimum_required(VERSION 3.0)
project(lua_node_base C)

################################################################################
#### OPTIONS
option(IS_TARGET_OS_WINDOWS "Windows Target OS" OFF)
option(IS_TARGET_OS_LINUX "Linux Target OS" OFF)
option(IS_TARGET_OS_MACOS "MacOS Target OS" OFF)
option(IS_TARGET_OS_FREEBSD "FreeBSD Target OS" OFF)

message(STATUS "IS_TARGET_OS_WINDOWS: ${IS_TARGET_OS_WINDOWS}")
message(STATUS "IS_TARGET_OS_LINUX: ${IS_TARGET_OS_LINUX}")
message(STATUS "IS_TARGET_OS_MACOS: ${IS_TARGET_OS_MACOS}")
message(STATUS "IS_TARGET_OS_FREEBSD: ${IS_TARGET_OS_FREEBSD}")

if(IS_TARGET_OS_WINDOWS)
    add_compile_definitions(IS_TARGET_OS_WINDOWS)
endif ()

if(IS_TARGET_OS_LINUX)
    add_compile_definitions(IS_TARGET_OS_LINUX)
endif ()

if(IS_TARGET_OS_MACOS)
    add_compile_definitions(IS_TARGET_OS_MACOS)
endif ()

if(IS_TARGET_OS_FREEBSD)
    add_compile_definitions(IS_TARGET_OS_FREEBSD)
endif ()

################################################################################
#### CONFIG
configure_file(ln_config.h.in ln_config.h)

################################################################################
#### INCLUDE
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

################################################################################
#### SOURCES
file(GLOB SOURCES *.c)
list(FILTER SOURCES EXCLUDE REGEX \\.t\\.c)
foreach (item IN LISTS SOURCES)
    message(STATUS "${PROJECT_NAME} SOURCE: ${item}")
endforeach ()

add_library(${PROJECT_NAME} ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/ln_config.h)

################################################################################
#### TEST
file(GLOB TEST_SOURCES *.t.c)
foreach (item IN LISTS TEST_SOURCES)
    get_filename_component(TestUnit ${item} NAME_WE)
    message(STATUS "${PROJECT_NAME} TEST UNIT: ${TestUnit} - ${item}")
    add_executable(${TestUnit}.t ${item})
    target_link_libraries(${TestUnit}.t ${PROJECT_NAME})
endforeach ()

################################################################################
#### MAIN_SOURCES
file(GLOB MAIN_SOURCES *.m.c)
foreach (item IN LISTS MAIN_SOURCES)
    get_filename_component(MainUnit ${item} NAME_WE)
    message(STATUS "${PROJECT_NAME} MAIN PROGRAM: ${MainUnit} - ${item}")
    add_executable(${MainUnit} ${item})
    target_link_libraries(${MainUnit} ${PROJECT_NAME})
endforeach ()

################################################################################
#### EXPORT
set(LUA_NODE_BASE_FOUND 1 CACHE BOOL "" FORCE)
set(LUA_NODE_BASE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "" FORCE)
set(LUA_NODE_BASE_LIBRARY ${PROJECT_NAME} CACHE STRING "" FORCE)


