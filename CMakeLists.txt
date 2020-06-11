cmake_minimum_required(VERSION 3.16)
project(DynamicBinaryTranslatorRISCV64->x86-64)

set(CMAKE_C_STANDARD 99)

#Create CMake Constant TRANSLATOR_BASE
set(TRANSLATOR_BASE 0x780000000000)
#Tell compiler and linker that we want to relocate to TRANSLATOR_BASE
link_libraries("-pie -Wl,-Ttext-segment=${TRANSLATOR_BASE}")
#Make Constant available as C Makro
add_definitions(-DTRANSLATOR_BASE=${TRANSLATOR_BASE})

include_directories(src)

#Include the AsmJit library for x86 code-generation
set(ASMJIT_EMBED TRUE)
add_definitions(-DASMJIT_STATIC)
include(lib/asmjit/CMakeLists.txt)
include_directories(lib/asmjit/src)

#include the googletest testing framework
add_subdirectory(lib/googletest-master)
include_directories(lib/googletest-master/googletest/include)
include_directories(lib/googletest-master/googlemock/include)
add_executable("test" ${ASMJIT_SRC} src/main.c src/main.h test/test_main.cpp src/parser.c src/parser.h src/util.c src/util.h
        src/cache.c src/cache.h src/translate.cpp src/translate.hpp src/register.c src/register.h src/loadElf.c
        src/loadElf.h src/translate_arithmetic.cpp src/translate_arithmetic.hpp src/translate_controlflow.cpp
        src/translate_controlflow.hpp src/translate_csr.cpp src/translate_csr.hpp src/translate_loadstore.cpp
        src/translate_loadstore.hpp src/translate_m_ext.cpp src/translate_m_ext.hpp src/translate_other.cpp
        src/translate_other.hpp lib/common.h lib/minilibc.c test/test_register.cpp test/test_loadElf.cpp test/test_translate_basic.cpp test/test_parser.cpp test/test_cache.cpp test/test_hello_world.cpp)
target_link_libraries("test" gtest gtest_main)
target_compile_definitions(test PUBLIC TESTING)

#define our translator's target
add_executable("translator" ${ASMJIT_SRC} src/main.c src/parser.c src/parser.h src/util.c src/util.h
        src/cache.c src/cache.h src/translate.cpp src/translate.hpp src/register.c src/register.h src/loadElf.c
        src/loadElf.h src/translate_arithmetic.cpp src/translate_arithmetic.hpp src/translate_controlflow.cpp
        src/translate_controlflow.hpp src/translate_csr.cpp src/translate_csr.hpp src/translate_loadstore.cpp
        src/translate_loadstore.hpp src/translate_m_ext.cpp src/translate_m_ext.hpp src/translate_other.cpp
        src/translate_other.hpp lib/common.h lib/minilibc.c)

#link our translator with asmjit dependencies
target_link_libraries("translator" ${ASMJIT_DEPS})
target_link_libraries("test" ${ASMJIT_DEPS})

add_compile_options(-Wall -Wextra -pedantic)