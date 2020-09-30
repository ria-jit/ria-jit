//
// Created by Flo Schmidt on 08.09.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_CONTEXT_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_CONTEXT_H

#include <util/typedefs.h>
#include <cache/cache.h>

#ifdef __cplusplus
extern "C"{
#endif

#define SWAP_BX FE_MEM_ADDR((intptr_t) get_swap_file() + 8 * 0)
#define SWAP_BP FE_MEM_ADDR((intptr_t) get_swap_file() + 8 * 1)
#define SWAP_R12 FE_MEM_ADDR((intptr_t) get_swap_file() + 8 * 2)
#define SWAP_R13 FE_MEM_ADDR((intptr_t) get_swap_file() + 8 * 3)
#define SWAP_R14 FE_MEM_ADDR((intptr_t) get_swap_file() + 8 * 4)
#define SWAP_R15 FE_MEM_ADDR((intptr_t) get_swap_file() + 8 * 5)

typedef struct {
    register_info *r_info;
    /**
     * Call this with two parameters
     * @param t_cache_loc the location of the block to (possibly) execute.
     * @param bool whether to execute the given block and store back the context.
     */
    t_cache_loc load_execute_save_context;
    t_cache_loc save_context;
} context_info;

void execute_in_guest_context(const context_info *c_info, t_cache_loc loc);

context_info *init_map_context(void);

#ifdef __cplusplus
}
#endif
#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_CONTEXT_H
