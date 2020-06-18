//
// Created by flo on 24.04.20.
//

#include <common.h>
#include <stdbool.h>
#include "util.h"
#include "cache.h"
#include "translate.hpp"
#include "parser.h"
#include <register.h>
#include "loadElf.h"
#include <getopt.h>

//just temporary - we need some way to control transcoding globally?
bool finalize = false;

//prototypes
int transcode_loop();

t_risc_addr init_entry_pc();

bool execute_cached(t_cache_loc loc);

t_cache_loc translate_block(t_risc_addr risc_addr);

#ifndef TESTING
int main(int argc, char *argv[]) {
    //todo run with -v -f ../test/assembly_hello_world for the hello world test

    int opt_index = 0;
    char *file_path = NULL;

    //read command line options (ex. -f for executable file, -v for verbose logging, etc.)
    while ((opt_index = getopt(argc, argv, ":f:vh")) != -1) {
        switch (opt_index) {
            case 'v':
                verbose = true;
                break;
            case 'f':
                file_path = optarg;
                break;
            case ':':
            case 'h':
            default:
                dprintf(2, "Usage: dynamic-translate -f <filename> [-v][…]\n");
                return 1;
        }
    }

    log_verbose("Command line options:\n");
    log_verbose("Verbose: %d\n", verbose);
    log_verbose("File path: %s\n", file_path);

    if (file_path == NULL) {
        dprintf(2, "Bad. Invalid file path.\n");
        return 2;
    }

    log_verbose("Initializing transcoding...\n");
    transcode_loop(file_path);
    return 0;
}

#endif //TESTING

int start_transcode(const char *file_path){
    log_verbose("extern transcode start!\n");
    verbose = true;
    transcode_loop(file_path);
    return 0;
}

int transcode_loop(const char *file_path) {
    t_risc_elf_map_result result = mapIntoMemory(file_path);
    if(!result.valid){
        dprintf(2, "Bad. Failed to map into memory.\n");
    }

    t_risc_addr next_pc = result.entry;

    //allocate stack
    set_value((t_risc_reg) sp, createStack(0, (char **) "", result));

    init_hash_table();

    set_value(pc,next_pc);

    while (!finalize) {
        //check our previously translated code
        t_cache_loc cache_loc = lookup_cache_entry(next_pc);

        //we have not seen this block before
        if (cache_loc == UNSEEN_CODE) {
            cache_loc = translate_block(next_pc);
            set_cache_entry(next_pc,cache_loc);
        }

        //execute the cached (or now newly generated code) and update the program counter
        if(!execute_cached(cache_loc)) break;

        //store pc from registers in pc
        next_pc = get_value(pc);

        //tmp - programm should exit on syscall
        //finalize = true;
    }

    return 0;
}

/**
 * Moved to translate.cpp
 * Translate the basic block at the passed RISC-V pc address.
 * @param risc_addr the RISC-V address of the basic block in question
 * @return cache location of the translated code
 */
/*t_cache_loc translate_block(t_risc_addr risc_addr) {
    not_yet_implemented("Translate Block");

    return 0;
}*/

/**
 * Execute cached translated code at the passed location.
 * @param loc the cache address of that code
 * @return
 */
bool execute_cached(t_cache_loc loc) {
    log_verbose("Execute Cached...\n");
    typedef void (*void_asm)(void);
    ((void_asm)loc)(); //call asm code

    ///check for illegal x0 values
    if(*get_reg_data() != 0){
        printf("riscV register x0 != 0 after executing block\n");
        printf("Terminating...");
        return false;
    }

    return true;
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
