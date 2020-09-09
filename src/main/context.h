//
// Created by Flo Schmidt on 08.09.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_CONTEXT_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_CONTEXT_H

#include <util/typedefs.h>
#include <cache/cache.h>

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

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_CONTEXT_H
