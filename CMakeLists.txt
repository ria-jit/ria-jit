cmake_minimum_required(VERSION 3.16)
project(DynamicBinaryTranslatorRISCV64->x86-64 C)

set(CMAKE_C_STANDARD 99)

add_executable("target" src/main.c src/parser.c src/parser.h)