//
// Created by Flo Schmidt on 08.09.20.
//

#include "context.h"
#include <env/flags.h>

#include <runtime/register.h>
#include <gen/translate.h>
#include <common.h>
#include <linux/mman.h>
#include <util/util.h>
#include <env/opt.h>

/*
 * Dynamically generated switching blocks should give us the freedom to change the mapping more flexibly.
 */

context_info *init_map_context(bool floatBinary) {
    //register mapping as pulled from translate.c
    log_context("Initializing context...\n");

    /**
     * Allocation for general purpose register mapping.
     */
    FeReg *gp_map = mmap(NULL,
                         N_REG * sizeof(FeReg),
                         PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_PRIVATE,
                         -1,
                         0);

    if (BAD_ADDR(gp_map)) {
        dprintf(2, "Failed to allocate gp_map for context. Error %li", -(intptr_t) gp_map);
        panic(FAIL_HEAP_ALLOC);
    }

    bool *gp_mapped = mmap(NULL,
                           N_REG * sizeof(bool),
                           PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_PRIVATE,
                           -1,
                           0);

    if (BAD_ADDR(gp_mapped)) {
        dprintf(2, "Failed to allocate gp_mapped for context. Error %li", -(intptr_t) gp_mapped);
        panic(FAIL_HEAP_ALLOC);
    }

    //fill boolean array with 0
    for (int i = 0; i < N_REG; ++i) {
        gp_mapped[i] = false;
    }

    /**
     * Allocation for floating point register mapping.
     */
    FeReg *fp_map = mmap(NULL,
                         N_REG * sizeof(FeReg),
                         PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_PRIVATE,
                         -1,
                         0);

    if (BAD_ADDR(fp_map)) {
        dprintf(2, "Failed to allocate fp_map for context. Error %li", -(intptr_t) fp_map);
        panic(FAIL_HEAP_ALLOC);
    }

    bool *fp_mapped = mmap(NULL,
                           N_REG * sizeof(bool),
                           PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_PRIVATE,
                           -1,
                           0);

    if (BAD_ADDR(fp_mapped)) {
        dprintf(2, "Failed to allocate fp_mapped for context. Error %li", -(intptr_t) fp_mapped);
        panic(FAIL_HEAP_ALLOC);
    }

    //fill boolean array with 0
    for (int i = 0; i < N_REG; ++i) {
        fp_mapped[i] = false;
    }

    /**
     * Allocate memory for the dynamic translate-time replacement mapping.
     * These struct contents are used to lazily load values into replacement registers at translate time.
     */
    t_risc_reg *replacement_content = mmap(NULL,
                                           N_REPLACE * sizeof(t_risc_reg),
                                           PROT_READ | PROT_WRITE,
                                           MAP_ANONYMOUS | MAP_PRIVATE,
                                           -1,
                                           0);

    if (BAD_ADDR(replacement_content)) {
        dprintf(2, "Failed to allocate replacement_content for context. Error %li", -(intptr_t) replacement_content);
        panic(FAIL_HEAP_ALLOC);
    }

    //fill with INVALID_REG to indicate unused
    for (size_t i = 0; i < N_REPLACE; i++) {
        replacement_content[i] = INVALID_REG;
    }

    uint64_t *replacement_recency = mmap(NULL,
                                         N_REPLACE * sizeof(uint64_t),
                                         PROT_READ | PROT_WRITE,
                                         MAP_ANONYMOUS | MAP_PRIVATE,
                                         -1,
                                         0);

    if (BAD_ADDR(replacement_recency)) {
        dprintf(2, "Failed to allocate replacement_recency for context. Error %li", -(intptr_t) replacement_recency);
        panic(FAIL_HEAP_ALLOC);
    }

    //fill with 0 to indicate clean (maybe not necessary?)
    for (size_t i = 0; i < N_REPLACE; i++) {
        replacement_recency[i] = 0;
    }


    //Allocating here to keep r_info const in all translator functions.
    //I am, however, aware this is not pretty. Maybe refactor in the future? todo?
    uint64_t *current_recency = mmap(NULL,
                                     sizeof(uint64_t),
                                     PROT_READ | PROT_WRITE,
                                     MAP_ANONYMOUS | MAP_PRIVATE,
                                     -1,
                                     0);

    if (BAD_ADDR(current_recency)) {
        dprintf(2, "Failed to allocate current_recency for context. Error %li", -(intptr_t) current_recency);
        panic(FAIL_HEAP_ALLOC);
    }

    //start count (across blocks?) at 1
    *current_recency = 1;


    /**
     * General-purpose register mapping
     * ================================
     * Any register mapping needs to take the translator functions into account.
     * They temporarily replace into AX, DX and CX for arithmetics (e.g. imul, shifts),
     * as well as CX as a scratch register for the atomics.
     * FIRST_REG and SECOND_REG are #defined as AX and DX.
     * Instructions may also replace into the SP, assuming they take care and save the register accordingly.
     * As soon as this is implemented, all other x86-GPRs must be considered callee-saved
     * when used inside instruction translations, as the mapping requires them to keep their value.
     * So, for the registers available to the mapping, see the following:
     * Reserved: AX, DX, CX (also, SP without great care)
     * May be used: BX, BP, SI, DI, R8, R9, R10, R11, R12, R13, R14, R15
     * Of which are callee-saved: BX, BP, R12, R13, R14, R15
     */
#define map_gp_reg(reg_risc, reg_x86)          \
    {                                          \
        gp_map[reg_risc] = reg_x86;            \
        gp_mapped[reg_risc] = true;            \
    }

    /**
     * We capture approximately 85 % of the register hits when we map the following registers:
     * (by order of access frequency)
     * x15, x14, x13, x10, x8, x2, x12, x11, x9,  x1,  x17, x18
     * a5,  a4,  a3,  a0,  fp, sp, a2,  a1,  s1,  ra,  a7,  s2
     *                             into
     * BX,  BP,  SI,  DI,  R8, R9, R10, R11, R12, R13, R14, R15
     */
    map_gp_reg(a5, FE_BX)
    map_gp_reg(a4, FE_BP)
    map_gp_reg(a3, FE_SI)
    map_gp_reg(a0, FE_DI)
    map_gp_reg(fp, FE_R8)
    map_gp_reg(sp, FE_R9)
    map_gp_reg(a2, FE_R10)
    map_gp_reg(a1, FE_R11)
    map_gp_reg(s1, FE_R12)
    map_gp_reg(ra, FE_R13)
    map_gp_reg(a7, FE_R14)
    map_gp_reg(s2, FE_R15)

#undef map_gp_reg


    /**
     * Floating point register mapping
     * ===============================
     */
#define map_fp_reg(reg_risc, reg_x86)          \
    {                                       \
        fp_map[reg_risc] = reg_x86;   \
        fp_mapped[reg_risc] = true;            \
    }                                          \

    /**
     * We capture approximately 81,4 % of the register hits when we map the following registers:
     * (by order of access frequency)
     * f15, f14, f9, f13, f12, f10, f11, f1, f0, f3,  f2, f31, f20, f24
     *                             into
     * XMM2 - XMM15
     */
    //can't map FE_XMM0 and FE_XMM1 as they are used as replacement
    map_fp_reg(f15, FE_XMM2)
    map_fp_reg(f14, FE_XMM3)
    map_fp_reg(f9, FE_XMM4)
    map_fp_reg(f13, FE_XMM5)
    map_fp_reg(f12, FE_XMM6)
    map_fp_reg(f10, FE_XMM7)
    map_fp_reg(f11, FE_XMM8)
    map_fp_reg(f1, FE_XMM9)
    map_fp_reg(f0, FE_XMM10)
    map_fp_reg(f3, FE_XMM11)
    map_fp_reg(f2, FE_XMM12)
    map_fp_reg(f31, FE_XMM13)
    map_fp_reg(f20, FE_XMM14)
    map_fp_reg(f24, FE_XMM15)

#undef map_fp_reg

    //log context setup
    if (flag_log_context) {
        log_context("Static general-purpose mapping contents:\n");
        for (t_risc_reg reg = x0; reg <= pc; reg++) {
            if (gp_mapped[reg]) {
                log_context("%s/%s --> %s\n",
                            gp_to_string(reg),
                            gp_to_alias(reg),
                            reg_x86_to_string(gp_map[reg]));
            }
        }

        log_context("Static floating point mapping contents:\n");
        for (t_risc_fp_reg reg = f0; reg <= f31; reg++) {
            if (fp_mapped[reg]) {
                log_context("%s/%s --> %s\n",
                            fp_to_string(reg),
                            fp_to_alias(reg),
                            reg_x86_to_string(fp_map[reg]));
            }
        }
    }

    ///create info struct
    register_info *r_info =
            mmap(NULL,
                 sizeof(register_info),
                 PROT_READ | PROT_WRITE,
                 MAP_ANONYMOUS | MAP_PRIVATE,
                 -1,
                 0);

    if (BAD_ADDR(r_info)) {
        dprintf(2, "Failed to allocate r_info for context. Error %li", -(intptr_t) r_info);
        panic(FAIL_HEAP_ALLOC);
    }

    r_info->gp_map = gp_map;
    r_info->gp_mapped = gp_mapped;
    r_info->fp_map = fp_map;
    r_info->fp_mapped = fp_mapped;
    r_info->base = (uint64_t) get_gp_reg_file();
    r_info->csr_base = (uint64_t) get_csr_reg_file();
    r_info->replacement_content = replacement_content;
    r_info->replacement_recency = replacement_recency;
    r_info->current_recency = current_recency;
    r_info->fp_base = (uint64_t) get_fp_reg_file();

    //generate switching functions
    t_cache_loc load_execute_save_context;
    t_cache_loc save_context;

    //generate these dynamically in case we need to modify them
    {
        //context storing
        init_block(r_info, false);
        log_general("Generating context storing block...\n");

        if (floatBinary) {
            //check floatRegsLoaded
            /*err |= fe_enc64(&current, FE_CMP8mi, FE_MEM_ADDR((intptr_t) &floatRegsLoaded), 0); //zero if not loaded

            uint8_t *jmpBuf = current;
            err |= fe_enc64(&current, FE_JZ, (intptr_t) current);*/

            //save by register mapping fp
            for (int i = f0; i <= f31; ++i) {
                if (r_info->fp_mapped[i]) {
                    err |= fe_enc64(&current, FE_SSE_MOVSDmr, FE_MEM_ADDR(r_info->fp_base + 8 * i), r_info->fp_map[i]);
                }
            }
        }

        /*
        //clear flag
        err |= fe_enc64(&current, FE_MOV8mi, FE_MEM_ADDR((intptr_t) &floatRegsLoaded), 0); //zero if not loaded

        //write jump
        err |= fe_enc64(&jmpBuf, FE_JZ, (intptr_t) current);*/

        //save by register mapping gp
        for (int i = x0; i <= pc; ++i) {
            if (r_info->gp_mapped[i]) {
                err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * i), r_info->gp_map[i]);
            }
        }

        //load back callee-saved host registers BX, BP, R12, R13, R14, R15
        err |= fe_enc64(&current, FE_MOV64rm, FE_BX, SWAP_BX);
        err |= fe_enc64(&current, FE_MOV64rm, FE_BP, SWAP_BP);
        err |= fe_enc64(&current, FE_MOV64rm, FE_R12, SWAP_R12);
        err |= fe_enc64(&current, FE_MOV64rm, FE_R13, SWAP_R13);
        err |= fe_enc64(&current, FE_MOV64rm, FE_R14, SWAP_R14);
        err |= fe_enc64(&current, FE_MOV64rm, FE_R15, SWAP_R15);

        save_context = finalize_block(DONT_LINK, r_info);
    }

    {
        //context loading
        init_block(r_info, false);
        log_general("Generating context executing block...\n");

        //store callee-saved host registers BX, BP, R12, R13, R14, R15
        err |= fe_enc64(&current, FE_MOV64mr, SWAP_BX, FE_BX);
        err |= fe_enc64(&current, FE_MOV64mr, SWAP_BP, FE_BP);
        err |= fe_enc64(&current, FE_MOV64mr, SWAP_R12, FE_R12);
        err |= fe_enc64(&current, FE_MOV64mr, SWAP_R13, FE_R13);
        err |= fe_enc64(&current, FE_MOV64mr, SWAP_R14, FE_R14);
        err |= fe_enc64(&current, FE_MOV64mr, SWAP_R15, FE_R15);

        //move function arguments to scratch registers (would be overwritten by following guest context load)
        err |= fe_enc64(&current, FE_MOV64rr, FIRST_REG, FE_DI);
        err |= fe_enc64(&current, FE_MOV64rr, SECOND_REG, FE_SI);

        //load by register mapping
        for (int i = x0; i <= pc; ++i) {
            if (r_info->gp_mapped[i]) {
                err |= fe_enc64(&current, FE_MOV64rm, r_info->gp_map[i], FE_MEM_ADDR(r_info->base + 8 * i));
            }
        }
        if (floatBinary) {
            //load by fp register mapping
            for (int i = f0; i <= f31; ++i) {
                if (r_info->fp_mapped[i]) {
                    err |= fe_enc64(&current, FE_SSE_MOVSDrm, r_info->fp_map[i], FE_MEM_ADDR(r_info->fp_base + 8 * i));
                }
            }
        }

        err |= fe_enc64(&current, FE_TEST32rr, SECOND_REG, SECOND_REG);

        uint8_t *jmpBuf = current;
        err |= fe_enc64(&current, FE_JZ, (intptr_t) current);

        err |= fe_enc64(&current, FE_CALLr, FIRST_REG);

        ///Tail call save_context
        err |= fe_enc64(&current, FE_JMP, (intptr_t) save_context);

        err |= fe_enc64(&jmpBuf, FE_JZ, (intptr_t) current);

        load_execute_save_context = finalize_block(DONT_LINK, r_info);
    }

    //create context info struct
    context_info *c_info = mmap(NULL,
                                sizeof(context_info),
                                PROT_READ | PROT_WRITE,
                                MAP_ANONYMOUS | MAP_PRIVATE,
                                -1,
                                0);

    if (BAD_ADDR(c_info)) {
        dprintf(2, "Failed to allocate c_info for context. Error %li", -(intptr_t) c_info);
        panic(FAIL_HEAP_ALLOC);
    }

    c_info->r_info = r_info;
    c_info->load_execute_save_context = load_execute_save_context;
    c_info->save_context = save_context;
    if (perfFd >= 0) {
        dprintf(perfFd, "%lx %lx context_switch_load_execute\n", (uintptr_t) load_execute_save_context, 4096lu);
        dprintf(perfFd, "%lx %lx context_switch_save\n", (uintptr_t) save_context, 4096lu);
    }

    return c_info;
}

/**
 * Loads the RISC-V guest program's context, executes the translated block at the given address and stores the context
 * back.
 * <p>
 * Saves the x86 registers, loads the mapped RISC-V registers, executes the given block, stores the mapped RISC-V
 * registers and then restores the x86 registers.
 *
 * @param c_info the context_info to apply to (un)map the registers.
 * @param loc the cached block to execute.
 */
void execute_in_guest_context(const context_info *c_info, t_cache_loc loc) {
    typedef void (*void_asm)(t_cache_loc, bool);
    ((void_asm) c_info->load_execute_save_context)(loc, true);
}

