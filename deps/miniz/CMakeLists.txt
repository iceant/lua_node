cmake_minimum_required(VERSION 3.0)
project(miniz C)

#add_compile_definitions(MINIZ_NO_STDIO)
#add_compile_definitions(MINIZ_NO_TIME)
add_compile_definitions(MINIZ_NO_ZLIB_COMPATIBLE_NAMES)
add_compile_definitions(MINIZ_NO_ZLIB_APIS)
#add_compile_definitions(MINIZ_NO_DEFLATE_APIS)
#add_compile_definitions(MINIZ_NO_INFLATE_APIS)
#add_compile_definitions(MINIZ_NO_ARCHIVE_APIS)
#add_compile_definitions(MINIZ_NO_ARCHIVE_WRITING_APIS)
#add_compile_definitions(MINIZ_NO_MALLOC)

add_library(${PROJECT_NAME} miniz.c miniz.h)
