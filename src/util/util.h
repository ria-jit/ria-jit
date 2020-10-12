//
// Created by flo on 06.07.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H

#include <fadec/fadec-enc.h>
#include <gen/translate.h>
#include <runtime/register.h>

#ifdef __cplusplus
extern "C" {
#endif

//Apparently not included in the headers on my version.
#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x200000
#endif

#define FAIL_HEAP_ALLOC 0x1000

//add an access to reg to the profiler's data
#define RECORD_PROFILER(reg) err |= fe_enc64(&current, FE_INC64m, FE_MEM_ADDR((uint64_t) get_usage_file() + 8 * (reg)))

/*
 * Helper functions to extract FeRegs without handling every mapping case.
 */
static inline FeReg getRs1(const t_risc_instr *instr, const register_info *r_info, const FeReg replacement) {
    //log register access to profile if requested
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_src_1);
    }

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
    //log register access to profile if requested
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_src_2);
    }

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
    //log register access to profile if requested
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_dest);
    }

    return !r_info->mapped[instr->reg_dest] ? replacement : r_info->map[instr->reg_dest];
}

static inline void storeRd(const t_risc_instr *instr, const register_info *r_info, const FeReg regDest) {
    if (instr->reg_dest != x0 && !r_info->mapped[instr->reg_dest]) {
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), regDest);
    }
}

static inline void doArithmCommutative(FeReg regSrc1, FeReg regSrc2, FeReg regDest, uint64_t arithmMnem) {
    if (regDest == regSrc1) {
        ///mov into rd can be omitted when using rs1 as destination
        err |= fe_enc64(&current, arithmMnem, regSrc1, regSrc2);
    } else if (regDest == regSrc2) {
        ///mov into rd can be omitted when using rs2 as destination
        err |= fe_enc64(&current, arithmMnem, regSrc2, regSrc1);
    } else {
        ///mov first to not touch rs1 in case it is mapped to a x86 register and needed afterwards.
        err |= fe_enc64(&current, FE_MOV64rr, regDest, regSrc1);
        err |= fe_enc64(&current, arithmMnem, regDest, regSrc2);
    }
}

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TYPEDEFS_H
