//
// Created by flo on 02.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_CACHE_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_CACHE_H

#define UNSEEN_CODE (void*) 0

#include "util/log.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *t_cache_loc;

extern uint8_t *chain_end;

//cache entries for translated code segments
typedef struct {
    //the full RISC-V pc address
    t_risc_addr risc_addr;
    //cache location of that code segment
    t_cache_loc cache_loc;
} t_cache_entry;

void init_hash_table(void);

size_t hash(t_risc_addr risc_addr);

size_t find_lin_slot(t_risc_addr risc_addr);

t_cache_loc lookup_cache_entry(t_risc_addr risc_addr);

void set_cache_entry(t_risc_addr risc_addr, t_cache_loc cache_loc);

void print_values(void);

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_CACHE_H
