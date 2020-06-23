//
// Created by flo on 23.06.20.
//

#include "rv64minilibc.h"
#include <stdbool.h>

void init(char number);

void check_condition(bool cond);

/**
 * Compile with:
 * riscv64-unknown-linux-gnu-gcc -o arithm compiled_test_arithm.c rv64minilibc.c rv64minilibc.h -fPIE -static -march=rv64i -mabi=lp64 -nostdlib
 * Reference run: qemu-riscv64 arithm
 * or ./translator -v -f arithm.
 *
 * Makefile for compile:
 * arithmetic: compiled_test_arithm.c rv64minilibc.c rv64minilibc.h
	riscv64-unknown-linux-gnu-gcc -o arithm compiled_test_arithm.c rv64minilibc.c rv64minilibc.h -fPIE -static -march=rv64i -mabi=lp64 -nostdlib
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {
    init('1');
    size_t i = 4;
    i /= 2;
    check_condition(i == 2);

    init('2');
    size_t j = 4;
    j <<= 2;
    check_condition(j == 16);

    init('3');
    size_t m = 5;
    m /= 2;
    check_condition(m == 2);

    //this produces undefined reference to `__udivdi3' during compile because of nostdlib?
    /*init('4');
    size_t n = 256;
    size_t o = 128;
    n += o;
    n *= 10;
    n /= 19;
    check_condition(n == 202);*/

    __exit();
}

void init(char number) {
    __write(1, "Test ", 5);
    __putchar(number);
    __write(1, "... ", 4);
}

void check_condition(bool cond) {
    if (cond) {
        __write(1, "PASSED\n", 7);
    } else {
        __write(1, "FAILED\n", 7);
    }
}