//
// Created by flo on 23.06.20.
//

#include "rv64minilibc.h"
#include <stddef.h>

void __exit(void) {
    asm volatile (
    "addi a0, x0, 0\n\t"
    "addi a7, x0, 93\n\t"
    "ecall"
    ::: "memory"
    );
}

size_t __strlen(char* str) {
    size_t len;
    for (len = 0; str[len] != '\0'; ++len) {
        ;
    }
    return len;
}

void __write(int fd, void *buf, size_t len) {
    register int file asm("a0");
    file = fd;

    register void* string asm("a1");
    string = buf;

    register size_t length asm("a2");
    length = len;

    asm volatile (
    "addi a7, x0, 64\n\t"
    "ecall"
    ::: "memory"
    );
}

void __putchar(char ch) {
    __write(1, &ch, 1);
}
