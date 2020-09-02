//
// Created by flo on 09.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_H

#include <cache/cache.h>
#include <util/typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <fadec/fadec-enc.h>

//shortcut for memory operands
#define FE_MEM_ADDR(addr) FE_MEM(FE_IP, 0, 0, addr - (intptr_t) current)

///chainLink options
#define LINK_NULL 0
#define LINK_ADDR 1
#define DONT_LINK 2

/**
 * Register information for the translator functions.
 */
typedef struct {
    FeReg *map;
    bool *mapped;
    uint64_t base;
} register_info;

extern uint8_t *current;
extern int err;

void save_risc_registers(register_info r_info);

//basic block translation management
void init_block();
t_cache_loc finalize_block(int chainLinkOp);

///basic block translation
t_cache_loc translate_block(t_risc_addr risc_addr);

///chaining
void chain(t_cache_loc target);

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_H
