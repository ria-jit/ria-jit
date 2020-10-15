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

#define bool_str(b) (b ? "yes" : "no")

//todo: pull these into a specific error code file which better manages them and consolidates all _exit()s
#define FAIL_HEAP_ALLOC 0x1000

//add an access to reg to the profiler's data
#define RECORD_PROFILER(reg) err |= fe_enc64(&current, FE_INC64m, FE_MEM_ADDR((uint64_t) get_usage_file() + 8 * (reg)))

/*
 * Helper functions to extract FeRegs without handling every mapping case.
 */

/**
 * Convert the r_info index to the respective FeReg.
 * @param index the r_info index
 * @return corresponding register
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

/**
 * Convert the FeReg into the corresponding r_info index.
 * @param replacement the register
 * @return the corresponding index
 */
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
 * Only use when managing the dynamic replacement allocation manually.
 * @param r_info the static register mapping and dynamic allocation info
 * @param replacement the selected replacement to invalidate and write back
 */
static inline void invalidateReplacement(const register_info *r_info, FeReg replacement, bool writeback) {
    size_t index = getIndexForReg(replacement);
    t_risc_reg currentContent = r_info->replacement_content[index];
    log_context("Invalidating replacement %s with content %s...\n",
                reg_x86_to_string(replacement),
                reg_to_string(currentContent));

    //write back to register file
    if (writeback && currentContent != x0 && currentContent != INVALID_REG) {
        log_context("Writing back %s from %s...\n",
                    reg_to_string(currentContent),
                    reg_x86_to_string(replacement));
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
    invalidateReplacement(r_info, FIRST_REG, true);
    invalidateReplacement(r_info, SECOND_REG, true);
    invalidateReplacement(r_info, THIRD_REG, true);
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
            log_context("Loading x0 into %s...\n", reg_x86_to_string(replacement));
            err |= fe_enc64(&current, FE_XOR32rr, replacement, replacement);
        } else if (content == INVALID_REG) {
            continue;
        } else {
            log_context("Loading %s into %s...\n", reg_to_string(content), reg_x86_to_string(replacement));
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
            log_context("Writing back %s from %s...\n", reg_to_string(content), reg_x86_to_string(replacement));
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
 *                     Can be used when loading a destination register into memory, the value of which will be
 *                     overwritten by the instruction anyways.
 * @return the x86 register that contains the requested replacement
 */
static inline FeReg loadIntoReplacement(const register_info *r_info, const t_risc_reg requested, bool requireValue) {
    log_context("Loading %s into replacement (requireValue %s)...\n", reg_to_string(requested), bool_str(requireValue));

    //increment access recency
    *r_info->current_recency += 1;

    //check the replacements if the register is already present - if so, just return that
    for (size_t i = 0; i < N_REPLACE; i++) {
        if (r_info->replacement_content[i] == requested) {
            //note access recency for register and return
            r_info->replacement_recency[i] = *r_info->current_recency;
            FeReg activeReplacement = getRegForIndex(i);
            log_context("Already present in %s.\n", reg_x86_to_string(activeReplacement));

            //zero register if x0 is requested, as it could have been written to previously
            //  checking whether zeroing is needed would likely be a bigger overhead than the single cycle XOR
            if (requested == x0) {
                log_context("Zeroing %s for requested x0...\n", reg_x86_to_string(activeReplacement));
                err |= fe_enc64(&current, FE_XOR32rr, activeReplacement, activeReplacement);
            }

            return activeReplacement;
        }
    }

    //it is not already present, so we load it into the least recently used replacement register (lowest age)
    //find minimum of recency (or some clean register with value 0)
    size_t min = 0;
    for (size_t i = 1; i < N_REPLACE; i++) {
        if (r_info->replacement_recency[i] < r_info->replacement_recency[min]) {
            min = i;
        }
    }


    //write back that register to the file, if there is a valid value present
    t_risc_reg currentlyPresent = r_info->replacement_content[min];
    FeReg selectedReplacement = getRegForIndex(min);
    log_context("Selected %s for %s (oldest or free).\n", reg_x86_to_string(selectedReplacement), reg_to_string(requested));
    if (currentlyPresent != x0 && currentlyPresent != INVALID_REG) {
        err |= fe_enc64(&current,
                        FE_MOV64mr,
                        FE_MEM_ADDR(r_info->base + 8 * currentlyPresent),
                        selectedReplacement);
        log_context("Writing back %s from %s...\n", reg_to_string(currentlyPresent),
                    reg_x86_to_string(selectedReplacement));
    }

    //note current replacement contents
    r_info->replacement_content[min] = requested;
    r_info->replacement_recency[min] = *r_info->current_recency;

    //load the requested register into the selected replacement
    if (requireValue) {
        log_context("Loading %s into %s...\n", reg_to_string(requested), reg_x86_to_string(selectedReplacement));
        if (requested != x0) {
            err |= fe_enc64(&current, FE_MOV64rm, selectedReplacement, FE_MEM_ADDR(r_info->base + 8 * requested));
        } else {
            err |= fe_enc64(&current, FE_XOR32rr, selectedReplacement, selectedReplacement);
        }
    }

    //register can now be used
    return selectedReplacement;
}

/**
 * Load a register into a specific replacement register.
 * Used for instructions that require a specific target register for translation (i.e. CX in shifting).
 * If the requested destination register is free, it will be directly loaded.
 * If not, a write-back will be performed. 
 * In cases where the register is already mapped, the value will be exchanged to the requested destination.
 * @param r_info the static register mapping and dynamic allocation info
 * @param candidate the requested RISC-V register to map
 * @param destination the specifically requested destination register
 * @return the destination register passed as parameter, for cleaner code
 */
static inline FeReg loadIntoSpecific(const register_info *r_info, t_risc_reg candidate, FeReg destination) {
    log_context("Loading %s into specific replacement %s...\n", reg_to_string(candidate), reg_x86_to_string(destination));

    //increment access recency and get index
    *r_info->current_recency += 1;
    int8_t index = (int8_t) getIndexForReg(destination);

    //check if the candidate is already mapped
    bool alreadyMapped = false;
    int8_t presentIndex = -1;
    for (size_t i = 0; i < N_REPLACE; i++) {
        //check for already mapped
        if (r_info->replacement_content[i] == candidate) {
            alreadyMapped = true;
            presentIndex = i;
            break;
        }
    }

    if (alreadyMapped) {
        //if we already mapped it and it's in the correct spot, we're done
        if (index == presentIndex) {
            log_context("Register %s was already in %s.\n", reg_to_string(candidate), reg_x86_to_string(destination));
            return destination;
        } else {
            //we have to shuffle the registers around (switch info as well as values), so
            // we exchange the requested and present registers
            if (r_info->replacement_recency[index] != 0) {
                //we need to keep this value, so exchange
                log_context("Requested %s occupied. Swapping with %s...\n", reg_x86_to_string(destination),
                            reg_x86_to_string(getRegForIndex(presentIndex)));
                err |= fe_enc64(&current, FE_XCHG64rr, destination, getRegForIndex(presentIndex));
            } else {
                //the current value does not matter, so move
                log_context("Requested %s empty. Getting value from %s...\n", reg_x86_to_string(destination),
                            reg_x86_to_string(getRegForIndex(presentIndex)));
                err |= fe_enc64(&current, FE_MOV64rr, destination, getRegForIndex(presentIndex));
            }

            //exchange the r_info values
            uint64_t tmp_recency = r_info->replacement_recency[index];
            r_info->replacement_recency[index] = r_info->replacement_recency[presentIndex];
            r_info->replacement_recency[presentIndex] = tmp_recency;
            uint64_t tmp_content = r_info->replacement_content[index];
            r_info->replacement_content[index] = r_info->replacement_content[presentIndex];
            r_info->replacement_content[presentIndex] = tmp_content;

            //mark as young, so it won't get overwritten in case of misuse
            r_info->replacement_recency[index] = *r_info->current_recency;
        }
    } else {
        //if it's not mapped, we need to load it into this specific register
        // (if it's dirty, we potentially need to write back)
        if (r_info->replacement_recency[index] != 0 &&
                (r_info->replacement_content[index] != x0 || r_info->replacement_content[index] != INVALID_REG)) {
            //write back if we don't have x0 or invalid in there
            log_context("Writing back %s from %s to free specific replacement...\n",
                        reg_to_string(r_info->replacement_content[index]),
                        reg_x86_to_string(destination));
            err |= fe_enc64(&current,
                            FE_MOV64mr,
                            FE_MEM_ADDR(r_info->base + 8 * r_info->replacement_content[index]),
                            destination);
        }

        //load the register into the correct spot
        log_context("Loading %s into %s...\n",
                    reg_to_string(candidate),
                    reg_x86_to_string(destination));
        err |= fe_enc64(&current,
                        FE_MOV64rm,
                        destination,
                        FE_MEM_ADDR(r_info->base + 8 * candidate));

        //note the load
        r_info->replacement_content[index] = candidate;
        r_info->replacement_recency[index] = *r_info->current_recency;
    }

    return destination;
}

/**
 * Get or load the rs1 of the passed instruction.
 * This call may load the value from the register file in memory.
 * @param instr the instruction in question
 * @param r_info current register info
 * @return the x86 register containing the requested rs1
 */
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

/**
 * Get or load the rs2 of the passed instruction.
 * This call may load the value from the register file in memory.
 * @param instr the instruction in question
 * @param r_info current register info
 * @return the x86 register containing the requested rs2
 */
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

/**
 * Get or load the rd of the passed instruction.
 * This call may load the value from the register file in memory.
 * @param instr the instruction in question
 * @param r_info current register info
 * @return the x86 register containing the requested rd
 */
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

/**
 * Get or load the rs1 of the passed instruction, hinted with a hardware register.
 * This call may load the value from the register file in memory.
 * The requested rs1 will be in the passed into register after this call.
 * It will either be loaded into the register when not statically mapped,
 * or it will be replaced into the register by writing back the current contents.
 * @param instr the instruction in question
 * @param r_info current register info
 * @param into destination for the load
 * @return the x86 register containing the requested rs1
 */
static inline FeReg getRs1Into(const t_risc_instr *instr, const register_info *r_info, FeReg into) {
    //log register access to profile if requested
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_src_1);
    }

    //either return the mapped register, or load into the replacement
    if (!r_info->mapped[instr->reg_src_1]) {
        return loadIntoSpecific(r_info, instr->reg_src_1, into);
    } else if (r_info->map[instr->reg_src_1] == into) {
        //it is already statically mapped into the correct register, so we're done
        return r_info->map[instr->reg_src_1];
    } else {
        //it is statically mapped, but into the wrong register
        invalidateReplacement(r_info, into, true);

        //move over to the correct replacement and note
        err |= fe_enc64(&current, FE_MOV64rr, into, r_info->map[instr->reg_src_1]);
        *r_info->current_recency += 1;
        r_info->replacement_recency[getIndexForReg(into)] = *r_info->current_recency;
        r_info->replacement_content[getIndexForReg(into)] = instr->reg_src_1;
        return into;
    }
}

/**
 * Get or load the rs2 of the passed instruction, hinted with a hardware register.
 * This call may load the value from the register file in memory.
 * The requested rs2 will be in the passed into register after this call.
 * It will either be loaded into the register when not statically mapped,
 * or it will be replaced into the register by writing back the current contents.
 * @param instr the instruction in question
 * @param r_info current register info
 * @param into destination for the load
 * @return the x86 register containing the requested rs2
 */
static inline FeReg getRs2Into(const t_risc_instr *instr, const register_info *r_info, FeReg into) {
    //log register access to profile if requested
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_src_2);
    }

    //either return the mapped register, or load into the replacement
    if (!r_info->mapped[instr->reg_src_2]) {
        return loadIntoSpecific(r_info, instr->reg_src_2, into);
    } else if (r_info->map[instr->reg_src_2] == into) {
        //it is already statically mapped into the correct register, so we're done
        return r_info->map[instr->reg_src_2];
    } else {
        //it is statically mapped, but into the wrong register
        invalidateReplacement(r_info, into, true);

        //move over to the correct replacement and note
        err |= fe_enc64(&current, FE_MOV64rr, into, r_info->map[instr->reg_src_2]);
        *r_info->current_recency += 1;
        r_info->replacement_recency[getIndexForReg(into)] = *r_info->current_recency;
        r_info->replacement_content[getIndexForReg(into)] = instr->reg_src_2;
        return into;
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

#define SWAP_SCRATCH FE_MEM_ADDR((intptr_t) get_swap_file() + 8 * 6)

static inline void saveScratchReg(FeReg scratch) {
    err |= fe_enc64(&current, FE_MOV64mr, SWAP_SCRATCH, scratch);
}

static inline void loadScratchReg(FeReg scratch) {
    err |= fe_enc64(&current, FE_MOV64rm, scratch, SWAP_SCRATCH);
}

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TYPEDEFS_H
