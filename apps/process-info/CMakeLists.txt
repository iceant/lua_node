cmake_minimum_required(VERSION 3.0)
project(process-info C)

set(CMAKE_C_STANDARD 11)


################################################################################
#### OPTIONS

################################################################################
#### CONFIG
configure_file(config.h.in config.h)

################################################################################
#### INCLUDE
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

################################################################################
#### SOURCES
file(GLOB SOURCES *.c)
list(FILTER SOURCES EXCLUDE REGEX \\.t\\.c)
foreach (item IN LISTS SOURCES)
    message(STATUS "${PROJECT_NAME} SOURCE: ${item}")
endforeach ()

add_library(${PROJECT_NAME} ${SOURCES})
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
    add_executable(${MainUnit}.m ${item})
    target_link_libraries(${MainUnit}.m ${PROJECT_NAME})
endforeach ()
