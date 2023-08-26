################################################################################
## OS
##  - IS_WINDOWS
##  - IS_LINUX
##  - IS_MACOS

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(IS_LINUX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_compile_definitions(IS_MACOS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(IS_WINDOWS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    add_compile_definitions(IS_FREEBSD)
elseif(CMAKE_SYSTEM_NAME STREQUAL "AIX")
    add_compile_definitions(IS_AIX)
else()
    message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
endif()

################################################################################
## COMPILER
##  - IS_INTEL_C_COMPILER
##  - IS_GNU_C_COMPILER
##  - IS_MSVC_C_COMPILER


if(CMAKE_C_COMPILER_ID MATCHES Intel)
    add_compile_definitions(IS_INTEL_C_COMPILER)
elseif (CMAKE_C_COMPILER_ID MATCHES GNU)
    add_compile_definitions(IS_GNU_C_COMPILER)
elseif (CMAKE_C_COMPILER_ID MATCHES PGI)
    add_compile_definitions(IS_PGI_C_COMPILER)
elseif (CMAKE_C_COMPILER_ID MATCHES XL)
    add_compile_definitions(IS_XL_C_COMPILER)
else()
    message(STATUS "CMAKE_C_COMPILER_ID: ${CMAKE_C_COMPILER_ID}")
endif()


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_compile_definitions(IS_64BIT)
    message(STATUS "Target is 64 bits")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    add_compile_definitions(IS_32BIT)
    message(STATUS "Target is 32 bits")
else ()
    message(STATUS "CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")
endif()

if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i386")
    message(STATUS "i386 architecture detected")
    add_compile_definitions(IS_CPU_386)
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i686")
    message(STATUS "i686 architecture detected")
    add_compile_definitions(IS_CPU_686)
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    message(STATUS "x86_64 architecture detected")
    add_compile_definitions(IS_CPU_X86_64)
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm")
    message(STATUS "arm architecture detected")
    add_compile_definitions(IS_CPU_ARM)
else()
    message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()



foreach(key
        IN ITEMS
        NUMBER_OF_LOGICAL_CORES
        NUMBER_OF_PHYSICAL_CORES
        HOSTNAME
        FQDN
        TOTAL_VIRTUAL_MEMORY
        AVAILABLE_VIRTUAL_MEMORY
        TOTAL_PHYSICAL_MEMORY
        AVAILABLE_PHYSICAL_MEMORY
        IS_64BIT
        HAS_FPU
        HAS_MMX
        HAS_MMX_PLUS
        HAS_SSE
        HAS_SSE2
        HAS_SSE_FP
        HAS_SSE_MMX
        HAS_AMD_3DNOW
        HAS_AMD_3DNOW_PLUS
        HAS_IA64
        HAS_SERIAL_NUMBER
        PROCESSOR_SERIAL_NUMBER
        PROCESSOR_NAME
        PROCESSOR_DESCRIPTION
        OS_NAME
        OS_RELEASE
        OS_VERSION
        OS_PLATFORM
)
    cmake_host_system_information(RESULT _${key} QUERY ${key})
    message(STATUS "${key}: ${_key}")
endforeach()