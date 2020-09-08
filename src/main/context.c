//
// Created by Flo Schmidt on 08.09.20.
//

#include "context.h"

#include <runtime/register.h>
#include "context.h"
#include <gen/translate.h>

/*
 * Neither finished nor tested.
 * Dynamically generated switching blocks should give us the freedom to change the mapping more flexibly? Input needed.
 */

context_info init_map_context(void) {
    //register mapping as pulled from translate.c

    ///create allocation MAPping
    FeReg register_map[N_REG];
    bool mapped[N_REG];

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
    register_info r_info = {
            register_map,
            mapped,
            (uint64_t) get_gp_reg_file(),
            (uint64_t) get_csr_reg_file()
    };

    //generate switching functions
    t_cache_loc load_context;
    t_cache_loc save_context;

    //generate these dynamically in case we need to modify them
    {
        //context loading
        init_block();

        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 0), FE_R12);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 1), FE_R13);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 2), FE_R14);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 3), FE_R15);

        //load by register mapping
        for (int i = x0; i <= pc; ++i) {
            if (r_info.mapped[i]) {
                err |= fe_enc64(&current, FE_MOV64rm, r_info.map[i], FE_MEM_ADDR(r_info.base + 8 * i));
            }
        }

        load_context = finalize_block(DONT_LINK);
    }

    {
        //context storing
        init_block();

        //save by register mapping
        for (int i = x0; i <= pc; ++i) {
            if (r_info.mapped[i]) {
                err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * i), r_info.map[i]);
            }
        }

        err |= fe_enc64(&current, FE_MOV64rm, FE_R12, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 0));
        err |= fe_enc64(&current, FE_MOV64rm, FE_R13, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 1));
        err |= fe_enc64(&current, FE_MOV64rm, FE_R14, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 2));
        err |= fe_enc64(&current, FE_MOV64rm, FE_R15, FE_MEM_ADDR((intptr_t) get_swap_space() + 8 * 3));

        save_context = finalize_block(DONT_LINK);
    }

    context_info c_info = {
            r_info,
            load_context,
            save_context
    };

    return c_info;
}

/**
 * Load the RISC-V guest program's context by saving our registers and loading the guest registers.
 */
void load_guest_context(context_info c_info) {
    typedef void (*void_asm)(void);
    ((void_asm) c_info.load_context)();
}

/**
 * Storer the RISC-V guest program's context by saving the guest registers and restoring our register contents.
 */
void store_guest_context(context_info c_info) {
    typedef void (*void_asm)(void);
    ((void_asm) c_info.save_context)();
}
