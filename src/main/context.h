//
// Created by Flo Schmidt on 08.09.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_CONTEXT_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_CONTEXT_H

#include <util/typedefs.h>
#include <cache/cache.h>

typedef struct {
    register_info r_info;
    t_cache_loc load_context;
    t_cache_loc save_context;
} context_info;

void load_guest_context(context_info c_info);

void store_guest_context(context_info c_info);

context_info init_map_context(void);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_CONTEXT_H
