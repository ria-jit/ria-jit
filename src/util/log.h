//
// Created by flo on 02.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOG_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOG_H

//error codes for internal reference
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const char *translator_version;

/**
* Verbose logging flags used by the main.c and logging methods.
*/
extern bool flag_log_general;
extern bool flag_log_asm_in;
extern bool flag_log_asm_out;
extern bool flag_log_reg_dump;
extern bool flag_log_cache;
extern bool flag_fail_silently;
extern bool flag_single_step;
extern bool flag_translate_opt;
extern bool flag_do_benchmark;
extern bool flag_do_analyze;

void critical_not_yet_implemented(const char* info);

void not_yet_implemented(const char* info);

void log_general(const char* format, ...);
void log_analyze(const char *format, ...);
void log_asm_in(const char* format, ...);
void log_asm_out(const char* format, ...);
void log_reg_dump(const char* format, ...);
void log_cache(const char* format, ...);
void log_benchmark(const char* format, ...);
void log_print_mem(const char* ptr, int len);

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOG_H
