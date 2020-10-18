//
// Created by Flo Schmidt on 18.10.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_FLAGS_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_FLAGS_H

/**
* Verbose logging flags used by the main.c and logging methods.
*/
extern bool flag_log_general;
extern bool flag_log_syscall;
extern bool flag_log_asm_in;
extern bool flag_log_asm_out;
extern bool flag_verbose_disassembly;
extern bool flag_log_reg_dump;
extern bool flag_log_cache;
extern bool flag_log_cache_contents;
extern bool flag_log_context;
extern bool flag_fail_silently;
extern bool flag_single_step;
extern bool flag_translate_opt;
extern bool flag_translate_opt_ras;
extern bool flag_translate_opt_chain;
extern bool flag_translate_opt_jump;
extern bool flag_translate_opt_fusion;
extern bool flag_do_benchmark;
extern bool flag_do_analyze_mnem;
extern bool flag_do_analyze_reg;
extern bool flag_do_analyze_pattern;
extern bool flag_do_profile;

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/typedefs.h>

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_FLAGS_H
