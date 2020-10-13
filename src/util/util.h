//
// Created by flo on 06.07.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H

#include <fadec/fadec-enc.h>
#include <gen/translate.h>
#include <runtime/register.h>
#include <common.h>

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

static inline FeReg getRegForIndex(size_t index) {
    switch (index) {
        case 0:
            return FIRST_REG;
        case 1:
            return SECOND_REG;
        case 2:
            return THIRD_REG;
        default:
            dprintf(2, "Bad. Invalid replacement register index %li.", index);
            _exit(index);
    }
}

static inline size_t getIndexForReg(FeReg replacement) {
    switch (replacement) {
        case FIRST_REG:
            return 0;
        case SECOND_REG:
            return 1;
        case THIRD_REG:
            return 2;
        default:
            dprintf(2, "Bad. Invalid replacement register index %u.", replacement);
            _exit(replacement);
    }
}

/**
 * Invalidates and writes back the passed replacement register.
 * Used in order to free the register for specific loads, and writes back the current content to the register file.
 * It also frees the register for future usage - be sure to invalidate after loading the registers, otherwise your
 * free register might be overwritten (as it will be marked as free and clean, so it will be the first one chosen for
 * loading a new unmapped register).
 * @param r_info the static register mapping and dynamic allocation info
 * @param replacement the selected replacement to invalidate and write back
 */
static inline void invalidateReplacement(const register_info *r_info, FeReg replacement) {
    size_t index = getIndexForReg(replacement);

    t_risc_reg currentContent = r_info->replacement_content[index];
    
    //write back to register file
    if (currentContent != x0 && currentContent != INVALID_REG) {
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * currentContent), replacement);
    }

    //set as empty and clean
    r_info->replacement_content[index] = INVALID_REG;
    r_info->replacement_recency[index] = 0;
}

/**
 * See above. Invalidates and cleans all replacement registers.
 * @param r_info containing the dynamic allocation info
 */
static inline void invalidateAllReplacements(const register_info *r_info) {
    invalidateReplacement(r_info, FIRST_REG);
    invalidateReplacement(r_info, SECOND_REG);
    invalidateReplacement(r_info, THIRD_REG);
}

/**
 * Load all replacement registers according to r_info.
 * Intended to be called on block boundaries (i.e. in finalize_block);
 * however, it may be sensible to do this context-wide.
 * @param r_info the register info containing the dynamic allocation specification
 */
static inline void loadReplacements(const register_info *r_info) {
    for (size_t i = 0; i < N_REPLACE; i++) {
        t_risc_reg content = r_info->replacement_content[i];
        FeReg replacement = getRegForIndex(i);

        if (content == x0) {
            err |= fe_enc64(&current, FE_XOR64rr, replacement, replacement);
        } else if (content == INVALID_REG) {
            continue;
        } else {
            err |= fe_enc64(&current,
                            FE_MOV64rm,
                            replacement,
                            FE_MEM_ADDR(r_info->base + 8 * content));
        }
    }
}

/**
 * Store all replacement registers according to r_info.
 * Intended to be called on block boundaries (i.e. in finalize_block);
 * however, it may be sensible to do this context-wide.
 * @param r_info the register info containing the dynamic allocation specification
 */
static inline void storeReplacements(const register_info *r_info) {
    for (size_t i = 0; i < N_REPLACE; i++) {
        t_risc_reg content = r_info->replacement_content[i];
        FeReg replacement = getRegForIndex(i);

        if (content == x0 || content == INVALID_REG) {
            continue;
        } else {
            err |= fe_enc64(&current,
                            FE_MOV64mr,
                            FE_MEM_ADDR(r_info->base + 8 * content),
                            replacement);
        }
    }
}

/**
 * Load the requested RISC-V register into a replacement GPR and return that.
 * Mark the load in the register_info contents for future instruction loads (--> cache register in GPR).
 * @param r_info the static register mapping and dynamic allocation info
 * @param requested the RISC-V register requested by the instruction
 * @param requireValue when loading the register, load the value from the register file if true, else leave undefined.
 *                     Can be used when loading a destination register into memory.
 * @return the x86 register that contains the requested replacement
 */
static inline FeReg loadIntoReplacement(const register_info *r_info, const t_risc_reg requested, bool requireValue) {
    //increment access recency
    *r_info->current_recency += 1;

    //check the replacements if the register is already present - if so, just return that
    for (size_t i = 0; i < N_REPLACE; i++) {
        if (r_info->replacement_content[i] == requested) {
            //note access recency for register and return
            r_info->replacement_recency[i] = *r_info->current_recency;
            FeReg activeReplacement = getRegForIndex(i);
            
            //zero register if x0 is requested, as it could have been written to previously
            //  checking whether zeroing is needed would likely be a bigger overhead than the single cycle XOR
            if (requested == x0) {
                err |= fe_enc64(&current, FE_XOR64rr, activeReplacement, activeReplacement);
            }
            
            return activeReplacement;
        }
    }

    //it is not already present, so we load it into the least recently used replacement register (lowest age)
    //find minimum of recency (or some clean register with value 0)
    size_t min = 0;
    for (size_t i = 1; i < N_REPLACE; i++) {
        if (r_info->current_recency[i] < r_info->current_recency[min]) {
            min = i;
        }
    }

    //write back that register to the file, if there is a valid value present
    t_risc_reg currentlyPresent = r_info->replacement_content[min];
    FeReg selectedReplacement = getRegForIndex(min);
    if (currentlyPresent != x0 && currentlyPresent != INVALID_REG) {
        err |= fe_enc64(&current,
                        FE_MOV64mr,
                        FE_MEM_ADDR(r_info->base + 8 * currentlyPresent),
                        selectedReplacement);
    }
    
    //note current replacement contents
    r_info->replacement_content[min] = requested;
    r_info->replacement_recency[min] = *r_info->current_recency;
    
    //load the requested register into the selected replacement
    if (requireValue) {
        if (requested != x0) {
            err |= fe_enc64(&current, FE_MOV64rm, selectedReplacement, FE_MEM_ADDR(r_info->base + 8 * requested));
        } else {
            err |= fe_enc64(&current, FE_XOR64rr, selectedReplacement, selectedReplacement);
        }
    }

    //register can now be used
    return selectedReplacement;
}

static inline FeReg getRs1(const t_risc_instr *instr, const register_info *r_info) {
    //log register access to profile if requested
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_src_1);
    }

    //either return the mapped register, or load into a replacement
    if (!r_info->mapped[instr->reg_src_1]) {
        return loadIntoReplacement(r_info, instr->reg_src_1, true);
    } else {
        return r_info->map[instr->reg_src_1];
    }
}

static inline FeReg getRs2(const t_risc_instr *instr, const register_info *r_info) {
    //log register access to profile if requested
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_src_2);
    }

    //either return the mapped register, or load into a replacement
    if (!r_info->mapped[instr->reg_src_2]) {
        return loadIntoReplacement(r_info, instr->reg_src_2, true);
    } else {
        return r_info->map[instr->reg_src_2];
    }
}

static inline FeReg getRd(const t_risc_instr *instr, const register_info *r_info) {
    //log register access to profile if requested
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_dest);
    }

    if (!r_info->mapped[instr->reg_dest]) {
        //"load" the destination register without reading the previous value from the register file, as it will be
        //  overwritten by the instruction that follows this load
        return loadIntoReplacement(r_info, instr->reg_dest, false);
    } else {
        return r_info->map[instr->reg_dest];
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
