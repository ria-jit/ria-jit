cmake_minimum_required(VERSION 3.15)
project(DynamicBinaryTranslatorRISCV64->x86-64)

set(CMAKE_C_STANDARD 11)

#Create CMake Constant TRANSLATOR_BASE
set(TRANSLATOR_BASE 0x780000000000)
#Tell compiler and linker that we want to relocate to TRANSLATOR_BASE
add_link_options(-pie -Wl,-Ttext-segment=${TRANSLATOR_BASE})
#Make Constant available as C Makro
add_definitions(-DTRANSLATOR_BASE=${TRANSLATOR_BASE})

add_compile_options(-fno-stack-protector -fPIE -Wall -Wextra -Wpedantic)

#Get a variable containing the meson build type corresponding to the CMake Build type
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(MESON_BUILD_TYPE debug)
elseif (${CMAKE_BUILD_TYPE} MATCHES Release)
    set(MESON_BUILD_TYPE release)
else ()
    set(MESON_BUILD_TYPE debugoptimized)
endif ()

include(ExternalProject)
include(CheckCCompilerFlag)

set(FADEC_PATH_PREFIX lib/fadec)
set(FADEC_GEN_INCLUDES
        ${FADEC_PATH_PREFIX}/fadec-decode-table.inc
        ${FADEC_PATH_PREFIX}/fadec-enc-cases.inc
        ${FADEC_PATH_PREFIX}/fadec-enc-mnems.inc)
set(FADEC_BYPRODUCTS
        ${FADEC_PATH_PREFIX}/${CMAKE_STATIC_LIBRARY_PREFIX}fadec${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${FADECLIB_GEN_INCLUDES})
set(FADECLIB_GEN_INCLUDES ${FADEC_GEN_INCLUDES})
list(TRANSFORM FADECLIB_GEN_INCLUDES PREPEND ${CMAKE_CURRENT_BINARY_DIR}/)

#Add fadec as an external target to our project (uses slightly changed versions of the commands from
#fadec/.github/workflows/ci.yml)
ExternalProject_Add(fadec
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${FADEC_PATH_PREFIX}
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${FADEC_PATH_PREFIX}
        CONFIGURE_COMMAND CFLAGS=-fno-stack-protector meson --buildtype ${MESON_BUILD_TYPE} ${CMAKE_CURRENT_SOURCE_DIR}/${FADEC_PATH_PREFIX}
        BUILD_COMMAND ninja -v
        INSTALL_COMMAND ""
        UPDATE_COMMAND "pwd" #dummy so cmake always tries to update the subproject
        TEST_COMMAND meson test -v
        BUILD_BYPRODUCTS ${FADEC_BYPRODUCTS})

#Will always be regenerated
add_custom_target(
        version
        ${CMAKE_COMMAND} -D SRC=${CMAKE_SOURCE_DIR}/src/util/version.h.in
        -D DST=${CMAKE_BINARY_DIR}/src/util/version.h
        -P ${CMAKE_SOURCE_DIR}/version.cmake
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/src/util/version.h
)

include_directories(src)
include_directories(lib)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/${FADEC_PATH_PREFIX})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

set(TRANSLATOR_SOURCES
        src/main/main.c src/main/main.h
        src/main/context.c src/main/context.h
        src/parser/parser.c src/parser/parser.h
        src/cache/cache.c src/cache/cache.h
        src/cache/return_stack.h src/cache/return_stack.c
        src/runtime/register.c src/runtime/register.h
        src/runtime/emulateEcall.c src/runtime/emulateEcall.h
        src/elf/loadElf.c src/elf/loadElf.h
        src/gen/translate.c src/gen/translate.h
        src/gen/instr/ext/translate_a_ext.c src/gen/instr/ext/translate_a_ext.h
        src/gen/instr/core/translate_arithmetic.c src/gen/instr/core/translate_arithmetic.h
        src/gen/instr/core/translate_controlflow.c src/gen/instr/core/translate_controlflow.h
        src/gen/instr/core/translate_csr.c src/gen/instr/core/translate_csr.h
        src/gen/instr/core/translate_loadstore.c src/gen/instr/core/translate_loadstore.h
        src/gen/instr/ext/translate_m_ext.c src/gen/instr/ext/translate_m_ext.h
        src/gen/instr/core/translate_other.c src/gen/instr/core/translate_other.h
        src/gen/instr/ext/translate_f_ext.c src/gen/instr/ext/translate_f_ext.h
        src/gen/instr/ext/translate_d_ext.c src/gen/instr/ext/translate_d_ext.h
        src/gen/dispatch.c src/gen/dispatch.h
        lib/common.h lib/minilibc.c
        src/util/log.c src/util/log.h
        src/util/tools/analyze.c src/util/tools/analyze.h
        src/util/util.h
        src/util/typedefs.c src/util/typedefs.h
        src/env/opt.c src/env/opt.h
        src/util/tools/perf.c src/util/tools/perf.h
        lib/ryu/ryucommon.h
        lib/ryu/d2fixed.c
        lib/ryu/d2fixed_full_table.h
        lib/ryu/d2s_intrinsics.h
        lib/ryu/digit_table.h
        lib/ryu/ryu.h
        src/gen/instr/pseudo/translate_pseudo.c src/gen/instr/pseudo/translate_pseudo.h
        src/gen/optimize.c src/gen/optimize.h
        src/gen/instr/patterns.h src/gen/instr/patterns.c
        ${CMAKE_CURRENT_BINARY_DIR}/src/util/version.h
        src/env/flags.c src/env/flags.h
        src/env/exit.c src/env/exit.h)

set(UNIT_TESTS_SOURCES
        test/unit_tests/test_main.cpp
        test/unit_tests/test_register.cpp
        test/unit_tests/test_parser_basic.cpp
        test/unit_tests/test_cache.cpp
        test/unit_tests/test_faenc_experiments.cpp
        test/unit_tests/test_amo_ext.cpp
        test/unit_tests/test_arithm.cpp
        test/unit_tests/float_tests/test_float_arithm.cpp
        test/unit_tests/float_tests/test_float_fused_arithm.cpp
        test/unit_tests/float_tests/test_float_con.cpp
        test/unit_tests/double_tests/test_double_arithm.cpp
        test/unit_tests/double_tests/test_double_fused_arithm.cpp
        test/unit_tests/double_tests/test_double_con.cpp
        test/unit_tests/double_tests/test_double_gp_to_fp.cpp
        test/unit_tests/double_tests/test_double_fp_to_gp.cpp
        test/unit_tests/float_tests/test_float_gp_to_fp.cpp
        test/unit_tests/float_tests/test_float_fp_to_gp.cpp
        test/unit_tests/test_parser_param.cpp)

#include the googletest testing framework
add_subdirectory(lib/googletest-master)
include_directories(lib/googletest-master/googletest/include)
include_directories(lib/googletest-master/googlemock/include)
add_executable("test" ${TRANSLATOR_SOURCES} ${UNIT_TESTS_SOURCES})
target_link_libraries("test" gtest gtest_main)
target_compile_definitions(test PUBLIC TESTING)

#define our translator's target
add_executable("translator" ${TRANSLATOR_SOURCES})

#Link to the built fadec library (Needs to be done like this since external targets are of type UTILITY and so can't
#be linked as libraries directly)
add_library(fadecLib IMPORTED STATIC)
ExternalProject_get_property(fadec BINARY_DIR)
set_target_properties(fadecLib PROPERTIES
        IMPORTED_LOCATION "${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}fadec${CMAKE_STATIC_LIBRARY_SUFFIX}")
add_dependencies(fadecLib fadec)

#Make sure fadecLib is built before translator or test
add_dependencies(translator fadecLib version)
add_dependencies(test fadecLib version)

target_link_libraries("translator" fadecLib)
target_link_libraries("test" fadecLib)

check_c_compiler_flag(-static-pie HAS_STATIC_PIE)

#Only use no stdlib in the actual translator since gtest needs it.
target_link_options(translator PRIVATE -nostdlib -lgcc)

if (HAS_STATIC_PIE)
    target_link_options(translator PRIVATE -static-pie)
else ()
    target_link_options(translator PRIVATE -Wl,-static -Wl,-pie -Wl,--no-dynamic-linker -Wl,-z,text)
endif ()

target_compile_definitions(translator PRIVATE -DNO_STDLIB)
