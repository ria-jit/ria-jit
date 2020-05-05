cmake_minimum_required(VERSION 3.16)
project(DynamicBinaryTranslatorRISCV64->x86-64)

set(CMAKE_C_STANDARD 99)
set(TRANSLATOR_BASE 0x780000000000)

link_libraries("-pie -Wl,-Ttext-segment=${TRANSLATOR_BASE}")

add_definitions(-DTRANSLATOR_BASE=${TRANSLATOR_BASE})

set(ASMJIT_EMBED TRUE)
add_definitions(-DASMJIT_STATIC)
include(lib/asmjit/CMakeLists.txt)
include_directories(lib/asmjit/src)

add_executable("target" ${ASMJIT_SRC} src/main.c src/parser.c src/parser.h src/gen.c src/gen.h src/util.c src/util.h
        src/cache.c
        src/cache.h src/blockloader.h src/blockloader.c src/translate.cpp src/translate.hpp src/register.c
        src/register.h src/loadElf.c src/loadElf.h)

target_link_libraries("target" ${ASMJIT_DEPS})