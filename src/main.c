//
// Created by flo on 24.04.20.
//

#include <stdio.h>
#include <stdbool.h>
#include "util.h"
#include "cache.h"
#include "translate.h"
#include "parser.h"

//just temporary - we need some way to control transcoding globally?
bool finalize = false;

//prototypes
int transcode_loop();
t_risc_addr init_entry_pc();
t_risc_addr execute_cached(t_cache_loc loc);
t_cache_loc translate_block(t_risc_addr risc_addr);

int main() {
    printf("Hello World!\n");
    test_parsing();
    transcode_loop();
    test_generation();
    return 0;
}

int transcode_loop() {
    t_risc_addr pc = init_entry_pc();

    init_hash_table();

    while(!finalize) {
        //check our previously translated code
        t_cache_loc cache_loc = lookup_cache_entry(pc);

        //we have not seen this block before
        if (cache_loc == UNSEEN_CODE) {
            cache_loc = translate_block(pc);
        }

        //execute the cached (or now newly generated code) and update the program counter
        pc = execute_cached(cache_loc);

        //tmp
        finalize = true;
    }

    return 0;
}

/**
 * Translate the basic block at the passed RISC-V pc address.
 * @param risc_addr the RISC-V address of the basic block in question
 * @return cache location of the translated code
 */
t_cache_loc translate_block(t_risc_addr risc_addr) {
    not_yet_implemented("Translate Block");
    return 0;
}

/**
 * Execute cached translated code at the passed location.
 * @param loc the cache address of that code
 * @return the program counter value after execution of the translated basic block
 */
t_risc_addr execute_cached(t_cache_loc loc) {
    not_yet_implemented("Execute Cached");
    return 0;
}

t_risc_instr *decode_next() {
    not_yet_implemented("Decode next");
    return NULL;
}

/**
 * Initialize the program counter to the program entry point address.
 * @return initial RISC-V program counter value
 */
t_risc_addr init_entry_pc() {
    not_yet_implemented("Init entry pc");
    return 0;
}