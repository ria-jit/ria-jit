//
// Created by flo on 23.06.20.
//

#include "rv64minilibc.h"
#include <stddef.h>

void m_exit(void) {
    asm volatile (
    "addi a0, x0, 0\n\t"
    "addi a7, x0, 93\n\t"
    "ecall"
    ::: "memory"
    );
}

size_t m_strlen(char* str) {
    size_t len;
    for (len = 0; str[len] != '\0'; ++len) {
        ;
    }
    return len;
}

void m_write(int fd, void *buf, size_t len) {
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

void m_putchar(char ch) {
    m_write(1, &ch, 1);
}

void printi(int i) {
    if (i > 9) {
        int rem = i / 10;
        i -= 10 * rem;
        printi(rem);
    }

    m_putchar('0' + i);
}