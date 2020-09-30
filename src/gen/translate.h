//
// Created by flo on 09.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_H

#include <cache/cache.h>
#include <util/typedefs.h>
#include <fadec/fadec-enc.h>
#include <main/context.h>

#ifdef __cplusplus
extern "C" {
#endif

//shortcut for memory operands
#define FE_MEM_ADDR(addr) FE_MEM(FE_IP, 0, 0, (addr) - (intptr_t) current)

///chainLink options
#define LINK_NULL 0
#define LINK_ADDR 1
#define DONT_LINK 2

#define FIRST_REG FE_AX
#define SECOND_REG FE_DX
#define THIRD_REG FE_CX
#define FIRST_FP_REG FE_XMM0
#define SECOND_FP_REG FE_XMM1

extern uint8_t *current;
extern int err;

//basic block translation management
void init_block(register_info *r_info);

t_cache_loc finalize_block(int chainLinkOp, const register_info *r_info);

///basic block translation
t_cache_loc translate_block(t_risc_addr risc_addr, const context_info *c_info);

t_cache_loc
translate_block_instructions(t_risc_instr block_cache[], int instructions_in_block, const context_info *c_info);

///chaining
void chain(t_cache_loc target);

void setupInstrMem();

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_H
