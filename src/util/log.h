//
// Created by flo on 02.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOG_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOG_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const char *const translator_version;

void not_yet_implemented(const char *info, ...)
__attribute__((__format__(printf, 1, 2)));

void critical_not_yet_implemented(const char *info, ...)
__attribute__((__format__(printf, 1, 2)));

void log_general(const char *format, ...)
__attribute__((__format__(printf, 1, 2)));

void log_syscall(const char *format, ...)
__attribute__((__format__(printf, 1, 2)));

void log_analyze(const char *format, ...)
__attribute__((__format__(printf, 1, 2)));

void log_asm_in(const char *format, ...)
__attribute__((__format__(printf, 1, 2)));

void log_asm_out(const char *format, ...)
__attribute__((__format__(printf, 1, 2)));

void log_reg_dump(const char *format, ...)
__attribute__((__format__(printf, 1, 2)));

void log_context(const char *format, ...)
__attribute__((__format__(printf, 1, 2)));

void log_cache(const char *format, ...)
__attribute__((__format__(printf, 1, 2)));

void log_benchmark(const char *format, ...)
__attribute__((__format__(printf, 1, 2)));

void log_profile(const char *format, ...)
__attribute__((__format__(printf, 1, 2)));

void log_print_mem(const char *ptr, long int len);

void invalid_error_handler(int32_t errorcode, int32_t raw_instr, t_risc_addr addr);

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOG_H
