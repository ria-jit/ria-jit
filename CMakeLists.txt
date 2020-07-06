cmake_minimum_required(VERSION 3.16)
project(DynamicBinaryTranslatorRISCV64->x86-64)

set(CMAKE_C_STANDARD 99)

#Create CMake Constant TRANSLATOR_BASE
set(TRANSLATOR_BASE 0x780000000000)
#Tell compiler and linker that we want to relocate to TRANSLATOR_BASE
link_libraries("-pie -Wl,-Ttext-segment=${TRANSLATOR_BASE}")
#Make Constant available as C Makro
add_definitions(-DTRANSLATOR_BASE=${TRANSLATOR_BASE})

add_compile_options(-fPIE -Wall -Wextra) ##TODO pedantic removed because ASMJit

#Get a variable containing the meson build type corresponding to the CMake Build type
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(MESON_BUILD_TYPE debug)
elseif (${CMAKE_BUILD_TYPE} MATCHES Release)
    set(MESON_BUILD_TYPE release)
else ()
    set(MESON_BUILD_TYPE debugoptimized)
endif ()

include(ExternalProject)

#Add fadec as an external target to our project (uses slightly changed versions of the commands from
#fadec/.github/workflows/ci.yml)
ExternalProject_Add(fadec
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/fadec
        BINARY_DIR ${CMAKE_BINARY_DIR}/lib/fadec
        CONFIGURE_COMMAND meson --buildtype ${MESON_BUILD_TYPE} ${CMAKE_SOURCE_DIR}/lib/fadec
        BUILD_COMMAND ninja -v
        INSTALL_COMMAND ""
        TEST_COMMAND meson test -v)

include_directories(src)
include_directories(lib)

#Include the AsmJit library for x86 code-generation
set(ASMJIT_EMBED TRUE)
add_definitions(-DASMJIT_STATIC)
include(lib/asmjit/CMakeLists.txt)
include_directories(lib/asmjit/src)

#include the googletest testing framework
add_subdirectory(lib/googletest-master)
include_directories(lib/googletest-master/googletest/include)
include_directories(lib/googletest-master/googlemock/include)
add_executable("test" ${ASMJIT_SRC} src/main/main.c src/main/main.h test/unit_tests/test_main.cpp src/parser/parser.c src/parser/parser.h
        src/util/log.c src/util/log.h src/cache/cache.c src/cache/cache.h src/gen/translate.cpp src/gen/translate.hpp src/runtime/register.c src/runtime/register.h
        src/elf/loadElf.c src/elf/loadElf.h src/gen/instr/translate_arithmetic.cpp src/gen/instr/translate_arithmetic.hpp
        src/gen/instr/translate_controlflow.cpp src/gen/instr/translate_controlflow.hpp src/gen/instr/translate_csr.cpp src/gen/instr/translate_csr.hpp
        src/gen/instr/translate_loadstore.cpp src/gen/instr/translate_loadstore.hpp src/gen/instr/translate_m_ext.cpp src/gen/instr/translate_m_ext.hpp
        src/gen/instr/translate_other.cpp src/gen/instr/translate_other.hpp src/runtime/emulateEcall.cpp src/runtime/emulateEcall.hpp lib/common.h
        lib/minilibc.c src/util/typedefs.h test/unit_tests/test_register.cpp
        test/unit_tests/test_translate_basic.cpp test/unit_tests/test_parser.cpp test/unit_tests/test_cache.cpp
        test/unit_tests/test_asmjit_ptr.cpp src/util/typedefs.c test/unit_tests/test_faenc_experiments.cpp)
target_link_libraries("test" gtest gtest_main)
target_compile_definitions(test PUBLIC TESTING)

#define our translator's target
add_executable("translator" ${ASMJIT_SRC} src/main/main.c src/main/main.h src/parser/parser.c src/parser/parser.h src/util/log.c src/util/log.h src/util/util.h
        src/cache/cache.c src/cache/cache.h src/gen/translate.cpp src/gen/translate.hpp src/runtime/register.c src/runtime/register.h src/elf/loadElf.c
        src/elf/loadElf.h src/gen/instr/translate_arithmetic.cpp src/gen/instr/translate_arithmetic.hpp src/gen/instr/translate_controlflow.cpp
        src/gen/instr/translate_controlflow.hpp src/gen/instr/translate_csr.cpp src/gen/instr/translate_csr.hpp src/gen/instr/translate_loadstore.cpp
        src/gen/instr/translate_loadstore.hpp src/gen/instr/translate_m_ext.cpp src/gen/instr/translate_m_ext.hpp src/gen/instr/translate_other.cpp
        src/gen/instr/translate_other.hpp src/runtime/emulateEcall.cpp src/runtime/emulateEcall.hpp lib/common.h
        lib/minilibc.c src/util/typedefs.h src/util/typedefs.c src/util/analyze.c src/util/analyze.h)
#Make sure fadec is built before translator or test
add_dependencies(translator fadec)
add_dependencies(test fadec)
#Link to  the built fadec library (Needs to be done like this since external targets are of type UTILITY and so can't
#be linked as libraries)
ExternalProject_get_property(fadec BINARY_DIR)
find_library(FADEC_LIB NAMES fadec PATHS ${BINARY_DIR})
target_link_libraries("translator" ${FADEC_LIB})
target_link_libraries("test" ${FADEC_LIB})
#link our translator with asmjit dependencies
target_link_libraries("translator" ${ASMJIT_DEPS})
target_link_libraries("test" ${ASMJIT_DEPS})

