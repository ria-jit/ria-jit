//
// Created by flo on 24.04.20.
//

#include <common.h>
#include <stdbool.h>
#include <util/log.h>
#include <runtime/emulateEcall.h>
#include <cache/cache.h>
#include <gen/translate.h>
#include <runtime/register.h>
#include <elf/loadElf.h>
#include <util/tools/analyze.h>
#include <env/opt.h>
#include <util/tools/perf.h>
#include <cache/return_stack.h>

//just temporary - we need some way to control transcoding globally?
bool finalize = false;

//prototypes
int transcode_loop(const char *file_path, int guestArgc, char **guestArgv);

bool execute_cached(t_cache_loc loc);

#ifndef TESTING

int main(int argc, char *argv[]) {
    t_opt_parse_result options = parse_cmd_arguments(argc, argv);

    //command line parsing failed, cannot execute anything
    if (options.status != 0) return options.status;

    if (flag_do_analyze) {
        analyze(options.file_path);
        return 0;
    }

    log_general("Initializing transcoding...\n");

    char *temp = argv[options.last_optind];
    argv[options.last_optind] = options.file_path;
    argv[options.file_index] = temp;

    int guestArgc = argc - options.last_optind;
    char **guestArgv = argv + (options.last_optind);
    int ret = transcode_loop(options.file_path, guestArgc, guestArgv);
    return ret;
}

#endif //TESTING

int transcode_loop(const char *file_path, int guestArgc, char **guestArgv) {
    t_risc_elf_map_result result = mapIntoMemory(file_path);
    if (!result.valid) {
        dprintf(2, "Bad. Failed to map into memory.\n");
        return 1;
    }

    setupBrk(result.dataEnd);

    t_risc_addr next_pc = result.entry;

    //allocate stack
    t_risc_addr stackAddr = createStack(guestArgc, guestArgv, result);
    if (!stackAddr) {
        return 1;
    }

    set_value((t_risc_reg) sp, stackAddr);

    init_hash_table();
    init_return_stack();

    set_value(pc, next_pc);

    //debugging output
    if (flag_log_reg_dump) {
        dump_registers();
    }

    //benchmark if necessary
    struct timespec begin;
    if (flag_do_benchmark) {
        begin = begin_measure();
    }

    while (!finalize) {
        //check our previously translated code
        t_cache_loc cache_loc = lookup_cache_entry(next_pc);

        //we have not seen this block before
        if (cache_loc == UNSEEN_CODE) {
            cache_loc = translate_block(next_pc);
            set_cache_entry(next_pc, cache_loc);
        }

        ///chain last block to current (if chainable)
        chain(cache_loc);

        //execute the cached (or now newly generated code) and update the program counter
        if (!execute_cached(cache_loc)) break;
        //printf("chain_end: %p\ncache_loc: %p\n\n", chain_end, cache_loc);

        //store pc from registers in pc
        next_pc = get_value(pc);
    }

    //finalize benchmark if necessary
    if (flag_do_benchmark) {
        end_display_measure(&begin);
    }

    return guest_exit_status;
}

/**
 * Execute cached translated code at the passed location.
 * @param loc the cache address of that code
 * @return
 */
bool execute_cached(t_cache_loc loc) {
    if (flag_log_cache) {
        log_cache("Execute block at %p, cache loc %p\n", get_value(pc), loc);
    }

    typedef void (*void_asm)(void);
    ((void_asm) loc)(); //call asm code

    //dump registers to the log
    if (flag_log_reg_dump) {
        dump_registers();
    }

    ///check for illegal x0 values
    if (*get_reg_data() != 0) {
        dprintf(2, "riscV register x0 != 0 after executing block\n");
        dprintf(2, "Terminating...");
        return false;
    }

    return true;
}
