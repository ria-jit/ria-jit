//
// Created by flo on 23.06.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_RV64MINILIBC_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_RV64MINILIBC_H

#include <stddef.h>
#include <stdint.h>

void __putchar(char ch);

void __write(int fd, void *buf, size_t len);

void __exit(void);

size_t __strlen(char* str);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_RV64MINILIBC_H
