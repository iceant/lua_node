cmake_minimum_required(VERSION 3.0)
project(miniz C)

add_compile_definitions(MINIZ_NO_ZLIB_COMPATIBLE_NAMES)
#add_compile_definitions(MINIZ_NO_ZLIB_APIS)
#add_compile_definitions(MINIZ_NO_MALLOC)
#add_compile_definitions(MINIZ_NO_STDIO)
#add_compile_definitions(MINIZ_NO_TIME)
#add_compile_definitions(MINIZ_NO_DEFLATE_APIS)
#add_compile_definitions(MINIZ_NO_INFLATE_APIS)
#add_compile_definitions(MINIZ_NO_ARCHIVE_APIS)
#add_compile_definitions(MINIZ_NO_ARCHIVE_WRITING_APIS)

add_library(${PROJECT_NAME} miniz.c miniz.h)

################################################################################
#### VARIABLES
set(MINIZ_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "")
set(MINIZ_LIBRARY ${PROJECT_NAME} CACHE STRING "")