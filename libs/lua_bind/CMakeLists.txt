cmake_minimum_required(VERSION 3.0)
project(lua_bind C)

set(CMAKE_C_STANDARD 11)


################################################################################
#### OPTIONS

################################################################################
#### CONFIG


################################################################################
#### INCLUDE

message(STATUS "LUA_NODE_BASE_INCLUDE_DIR: ${LUA_NODE_BASE_INCLUDE_DIR}")
message(STATUS "LUA_NODE_BASE_LIBRARY: ${LUA_NODE_BASE_LIBRARY}")
message(STATUS "LUA_INCLUDE_DIR: ${LUA_INCLUDE_DIR}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${LUA_INCLUDE_DIR})
include_directories(${LUA_NODE_BASE_INCLUDE_DIR})

################################################################################
#### SOURCES
file(GLOB SOURCES *.c)
list(FILTER SOURCES EXCLUDE REGEX \\.t\\.c)
foreach (item IN LISTS SOURCES)
    message(STATUS "${PROJECT_NAME} SOURCE: ${item}")
endforeach ()

add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LUA_NODE_BASE_LIBRARY} ${LUA_LIBRARY})

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
