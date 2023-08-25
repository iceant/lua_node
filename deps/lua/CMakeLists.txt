cmake_minimum_required(VERSION 3.0)
project(lua C)

################################################################################
#### FETCH
set(FETCHCONTENT_QUIET OFF)
include(FetchContent)
FetchContent_Declare(lua_src
    URL https://www.lua.org/ftp/lua-5.4.6.tar.gz
    URL_HASH SHA256=7d5ea1b9cb6aa0b59ca3dde1c6adcb57ef83a1ba8e5432c0ecd06bf439b3ad88
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

FetchContent_MakeAvailable(lua_src)
FetchContent_GetProperties(lua_src)
if(NOT lua_src_POPULATED)
    # Fetch the content using previously declared details
    FetchContent_Populate(lua_src)

    # Set custom variables, policies, etc.
    # ...


    # Bring the populated content into the build
    add_subdirectory(${lua_src_SOURCE_DIR} ${lua_src_BINARY_DIR})
endif()


set(LUA_SOURCE_DIR ${lua_src_SOURCE_DIR}/src CACHE STRING "")

aux_source_directory(${LUA_SOURCE_DIR} SOURCES)
list(REMOVE_ITEM SOURCES ${LUA_SOURCE_DIR}/lua.c ${LUA_SOURCE_DIR}/luac.c)
foreach (item IN LISTS SOURCES)
    message(STATUS "${PROJECT_NAME} SOURCE: ${item}")
endforeach ()


################################################################################
#### COMPILE


message(STATUS "READLINE_FOUND: ${READLINE_FOUND}")

add_library(${PROJECT_NAME} ${SOURCES})

if(IS_TARGET_OS_FREEBSD)
    find_package(readline)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DLUA_USE_LINUX -DLUA_USE_READLINE)
    target_include_directories(${PROJECT_NAME} PUBLIC "/usr/include/edit")
    target_link_libraries(${PROJECT_NAME} PUBLIC edit)
    if(READLINE_FOUND)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${READLINE_LIBRARY})
    endif ()
endif ()

if(IS_TARGET_OS_LINUX)
    find_package(readline)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DLUA_USE_LINUX)
    target_link_libraries(${PROJECT_NAME} PUBLIC m)
    if(READLINE_FOUND)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${READLINE_LIBRARY})
    endif ()
endif ()

if(IS_TARGET_OS_MACOS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DLUA_USE_MACOSX)
    target_link_libraries(${PROJECT_NAME} PUBLIC readline)
endif ()




################################################################################
#### EXPORT
set(LUA_FOUND 1 CACHE BOOL "Lua Sources Founded")
set(LUA_SOURCES ${SOURCES} CACHE STRING "Lua Sources")
set(LUA_INCLUDE_DIR ${LUA_SOURCE_DIR} CACHE STRING "Lua Include Directory")
set(LUA_LIBRARY ${PROJECT_NAME} CACHE STRING "Lua Library")

message(STATUS "LUA_FOUND: ${LUA_FOUND}")
message(STATUS "LUA_SOURCES: ${LUA_SOURCES}")
message(STATUS "LUA_INCLUDE_DIR: ${LUA_INCLUDE_DIR}")