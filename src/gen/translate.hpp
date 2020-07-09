//
// Created by flo on 09.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_HPP

#include "cache/cache.h"
#include <util/typedefs.h>

#ifdef __cplusplus

#include <asmjit/asmjit.h>

/**
 * Register information for the translator functions.
 */
struct register_info {
    asmjit::x86::Gp *map;
    bool *mapped;
    uint64_t base;
};

//deprecated
extern asmjit::CodeHolder *code;
extern asmjit::x86::Assembler *a;

extern uint8_t *current;
extern int err;

void save_risc_registers(register_info r_info);

extern "C" {
#endif

//basic block translation management
void init_block();
t_cache_loc finalize_block();

///basic block translation
t_cache_loc translate_block(t_risc_addr risc_addr);

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_HPP
