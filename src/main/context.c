//
// Created by Flo Schmidt on 08.09.20.
//

#include "context.h"

#include <runtime/register.h>
#include <gen/translate.h>
#include <common.h>
#include <linux/mman.h>
#include <util/util.h>

/*
 * Neither finished nor tested.
 * Dynamically generated switching blocks should give us the freedom to change the mapping more flexibly? Input needed.
 */

context_info *init_map_context(void) {
    //register mapping as pulled from translate.c

    //create register allocation mapping
    FeReg *register_map = mmap(NULL,
                               N_REG * sizeof(FeReg),
                               PROT_READ | PROT_WRITE,
                               MAP_ANONYMOUS | MAP_PRIVATE,
                               -1,
                               0);

    if (BAD_ADDR(register_map)) {
        dprintf(2, "Failed to allocate register_map for context. Error %li", -(intptr_t) register_map);
        _exit(FAIL_HEAP_ALLOC);
    }

    bool *mapped = mmap(NULL,
                        N_REG * sizeof(bool),
                        PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE,
                        -1,
                        0);

    if (BAD_ADDR(mapped)) {
        dprintf(2, "Failed to allocate mapped for context. Error %li", -(intptr_t) mapped);
        _exit(FAIL_HEAP_ALLOC);
    }

    //fill boolean array with 0
    for (int i = 0; i < N_REG; ++i) {
        mapped[i] = false;
    }


    //todo be aware of the registers claimed by the individual translator functions.
    // atomics temporarily replace into AX, DX, R8, CX

#define map_reg(reg_risc, reg_x86)          \
    ({                                      \
        register_map[reg_risc] = reg_x86;   \
        mapped[reg_risc] = true;            \
    })                                      \

    map_reg(ra, FE_DI);
    map_reg(sp, FE_SI);
    map_reg(a0, FE_R15);
    map_reg(a1, FE_R9);
    map_reg(a2, FE_R10);
    map_reg(a3, FE_R11);
    map_reg(a4, FE_R12);
    map_reg(a5, FE_R13);
    map_reg(a6, FE_R14);
//    map_reg(a7, FE_R15);

#undef map_reg

    //notice: risc reg x0 will need special treatment

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
        _exit(FAIL_HEAP_ALLOC);
    }

    r_info->map = register_map;
    r_info->mapped = mapped;
    r_info->base = (uint64_t) get_gp_reg_file();
    r_info->csr_base = (uint64_t) get_csr_reg_file();

    //generate switching functions
    t_cache_loc load_execute_save_context;
    t_cache_loc save_context;

    //generate these dynamically in case we need to modify them
    {
        //context storing
        init_block();
        log_general("Generating context storing block…\n");

        //save by register mapping
        for (int i = x0; i <= pc; ++i) {
            if (r_info->mapped[i]) {
                err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * i), r_info->map[i]);
            }
        }

        err |= fe_enc64(&current, FE_MOV64rm, FE_R12, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 0));
        err |= fe_enc64(&current, FE_MOV64rm, FE_R13, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 1));
        err |= fe_enc64(&current, FE_MOV64rm, FE_R14, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 2));
        err |= fe_enc64(&current, FE_MOV64rm, FE_R15, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 3));

        save_context = finalize_block(DONT_LINK);
    }

    {
        //context loading
        init_block();
        log_general("Generating context executing block…\n");

        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 0), FE_R12);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 1), FE_R13);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 2), FE_R14);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 3), FE_R15);

        err |= fe_enc64(&current, FE_MOV64rr, FIRST_REG, FE_DI);
        err |= fe_enc64(&current, FE_MOV64rr, SECOND_REG, FE_SI);

        //load by register mapping
        for (int i = x0; i <= pc; ++i) {
            if (r_info->mapped[i]) {
                err |= fe_enc64(&current, FE_MOV64rm, r_info->map[i], FE_MEM_ADDR(r_info->base + 8 * i));
            }
        }
        err |= fe_enc64(&current, FE_TEST32rr, SECOND_REG, SECOND_REG);

        uint8_t *jmpBuf = current;
        err |= fe_enc64(&current, FE_JZ, (intptr_t) current);

        err |= fe_enc64(&current, FE_CALLr, FIRST_REG);

        ///Tail call save_context
        err |= fe_enc64(&current, FE_JMP, (intptr_t) save_context);

        err |= fe_enc64(&jmpBuf, FE_JZ, (intptr_t) current);

        load_execute_save_context = finalize_block(DONT_LINK);
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
        _exit(FAIL_HEAP_ALLOC);
    }

    c_info->r_info = r_info;
    c_info->load_execute_save_context = load_execute_save_context;
    c_info->save_context = save_context;

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

