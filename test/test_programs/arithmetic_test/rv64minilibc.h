//
// Created by flo on 23.06.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_RV64MINILIBC_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_RV64MINILIBC_H

#include <stddef.h>
#include <stdint.h>

void m_putchar(char ch);

void m_write(int fd, void *buf, size_t len);

void m_exit(void);

size_t m_strlen(char* str);

void printi(int n);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_RV64MINILIBC_H
