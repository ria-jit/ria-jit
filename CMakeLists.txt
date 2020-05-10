cmake_minimum_required(VERSION 3.16)
project(DynamicBinaryTranslatorRISCV64->x86-64)

set(CMAKE_C_STANDARD 99)

set(ASMJIT_EMBED TRUE)
add_definitions(-DASMJIT_STATIC)
include(lib/asmjit/CMakeLists.txt)
include_directories(lib/asmjit/src)

add_executable("target" ${ASMJIT_SRC} src/main.c src/parser.c src/parser.h src/gen.c src/gen.h src/util.c src/util.h
        src/cache.c
        src/cache.h src/blockloader.h src/blockloader.c src/translate.cpp src/translate.h src/register.c src/register.h)

target_link_libraries("target" ${ASMJIT_DEPS})