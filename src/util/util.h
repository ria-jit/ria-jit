//
// Created by flo on 06.07.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H

#include <fadec/fadec-enc.h>
#include <gen/translate.h>

#ifdef __cplusplus
extern "C" {
#endif

//Apparently not included in the headers on my version.
#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x200000
#endif

#define FAIL_HEAP_ALLOC 0x1000

/*
 * Helper functions to extract FeRegs without handling every mapping case.
 */
static inline FeReg getRs1(const t_risc_instr *instr, const register_info *r_info, const FeReg replacement) {
    //No need to load the zero from memory, just create the zero in replacement register and move on.
    if (instr->reg_src_1 == x0) {
        err |= fe_enc64(&current, FE_XOR32rr, replacement, replacement);
        return replacement;
    }
    if (!r_info->mapped[instr->reg_src_1]) {
        err |= fe_enc64(&current, FE_MOV64rm, replacement, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
        return replacement;
    } else {
        return r_info->map[instr->reg_src_1];
    }
}

static inline FeReg getRs2(const t_risc_instr *instr, const register_info *r_info, const FeReg replacement) {
    //No need to load the zero from memory, just create the zero in replacement register and move on.
    if (instr->reg_src_2 == x0) {
        err |= fe_enc64(&current, FE_XOR32rr, replacement, replacement);
        return replacement;
    }
    if (!r_info->mapped[instr->reg_src_2]) {
        err |= fe_enc64(&current, FE_MOV64rm, replacement, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));
        return replacement;
    } else {
        return r_info->map[instr->reg_src_2];
    }
}

static inline FeReg getRd(const t_risc_instr *instr, const register_info *r_info, const FeReg replacement) {
    return !r_info->mapped[instr->reg_dest] ? replacement : r_info->map[instr->reg_dest];
}

static inline void storeRd(const t_risc_instr *instr, const register_info *r_info, const FeReg regDest) {
    if (instr->reg_dest != x0 && !r_info->mapped[instr->reg_dest]) {
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), regDest);
    }
}


#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TYPEDEFS_H
