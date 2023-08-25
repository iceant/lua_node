cmake_minimum_required(VERSION 3.0)
project(lua_node_project C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
################################################################################
#### VARIABLES
set(SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})

################################################################################
#### SUB PROJECTS

add_subdirectory(deps/lua)
add_subdirectory(libs/base)
add_subdirectory(libs/lua_bind)



