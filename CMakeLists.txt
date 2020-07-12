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
        test/unit_tests/test_asmjit_ptr.cpp)
target_link_libraries("test" gtest gtest_main)
target_compile_definitions(test PUBLIC TESTING)

#define our translator's target
add_executable("translator" ${ASMJIT_SRC} src/main/main.c src/main/main.h src/parser/parser.c src/parser/parser.h src/util/log.c src/util/log.h src/util/util.h
        src/cache/cache.c src/cache/cache.h src/gen/translate.cpp src/gen/translate.hpp src/runtime/register.c src/runtime/register.h src/elf/loadElf.c
        src/elf/loadElf.h src/gen/instr/translate_arithmetic.cpp src/gen/instr/translate_arithmetic.hpp src/gen/instr/translate_controlflow.cpp
        src/gen/instr/translate_controlflow.hpp src/gen/instr/translate_csr.cpp src/gen/instr/translate_csr.hpp src/gen/instr/translate_loadstore.cpp
        src/gen/instr/translate_loadstore.hpp src/gen/instr/translate_m_ext.cpp src/gen/instr/translate_m_ext.hpp src/gen/instr/translate_other.cpp
        src/gen/instr/translate_other.hpp src/runtime/emulateEcall.cpp src/runtime/emulateEcall.hpp lib/common.h lib/minilibc.c src/util/typedefs.h src/util/analyze.c src/util/analyze.h src/util/typedefs.c)

#link our translator with asmjit dependencies
target_link_libraries("translator" ${ASMJIT_DEPS})
target_link_libraries("test" ${ASMJIT_DEPS})

