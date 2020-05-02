//
// Created by flo on 02.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_CACHE_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_CACHE_H

#define UNSEEN_CODE -1

typedef unsigned int t_risc_addr;
typedef int t_cache_loc;

//cache entries for translated code segments
typedef struct {
    //the full RISC-V pc address
    t_risc_addr risc_addr;
    //cache location of that code segment
    t_cache_loc cache_loc;
} t_cache_entry;

void init_hash_table();
unsigned int hash(t_risc_addr risc_addr);
unsigned int find_lin_slot(t_risc_addr risc_addr);
t_cache_loc lookup_cache_entry(t_risc_addr risc_addr);
void set_cache_entry(t_risc_addr risc_addr, t_cache_loc cache_loc);
void print_values();

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_CACHE_H
