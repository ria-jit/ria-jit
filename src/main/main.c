//
// Created by flo on 24.04.20.
//

#include <common.h>
#include <stdbool.h>
#include "util/log.h"
#include <runtime/emulateEcall.hpp>
#include "cache/cache.h"
#include "gen/translate.hpp"
#include <runtime/register.h>
#include "elf/loadElf.h"
#include "util/analyze.h"
#include <getopt.h>

//just temporary - we need some way to control transcoding globally?
bool finalize = false;

//prototypes
int transcode_loop(const char *file_path, int guestArgc, char **guestArgv);

bool execute_cached(t_cache_loc loc);

#ifndef TESTING
int main(int argc, char *argv[]) {
    int opt_index = 0;
    char *file_path = NULL;
    bool doAnalyze = false;
    int fileIndex;

    //read command line options (ex. -f for executable file, -v for verbose logging, etc.)
    while((opt_index = getopt(argc, argv, ":f:mavgiorcshd")) != -1) {
        switch(opt_index) {
            case 'a':
                doAnalyze = true;
                break;
            case 'v':
                flag_log_general = true;
                flag_log_asm_in = true;
                flag_log_asm_out = true;
                flag_log_reg_dump = false; //don't do register dump with the verbose option by default
                flag_log_cache = true;
                flag_translate_opt = false;
                break;
            case 'g':
                flag_log_general = true;
                break;
            case 'i':
                flag_log_asm_in = true;
                break;
            case 'o':
                flag_log_asm_out = true;
                break;
            case 'r':
                flag_log_reg_dump = true;
                break;
            case 'c':
                flag_log_cache = true;
                break;
            case 'f':
                file_path = optarg;
                fileIndex = optind - 1;
                break;
            case 's':
                flag_fail_silently = true;
                break;
            case 'd':
                flag_single_step = true;
                break;
            case 'm':
                flag_translate_opt = true;
                break;
            case ':':
            case 'h':
            default:
                dprintf(2,
                        "Usage: dynamic-translate -f <filename> <option(s)>\n\t-v\tBe more verbose. Does not dump "
                        "register file. (equivalent to -gioc)\n"
                        "\t-g\tDisplay general verbose info\n\t-i\tDisplay parsed RISC-V input assembly\n"
                        "\t-o\tDisplay translated output x86 assembly\n"
                        "\t-r\tDump registers on basic block boundaries\n"
                        "\t-c\tDisplay cache info\n"
                        "\t-s\tFail silently for some  error conditions. Allows continued execution, but the client "
                        "program may enter undefined states.\n"
                        "\t-d\tEnable Single stepping mode. Each instruction will be its own block.\n");
                return 1;
        }
    }

    if(doAnalyze){
        analyze(file_path);
        return 0;
    }

    log_general("Command line options:\n");
    log_general("General verbose: %d\n", flag_log_general);
    log_general("Input assembly: %d\n", flag_log_asm_in);
    log_general("Output assembly: %d\n", flag_log_asm_out);
    log_general("Register dump: %d\n", flag_log_reg_dump);
    log_general("Cache info: %d\n", flag_log_cache);
    log_general("Fail silently: %d\n", flag_fail_silently);
    log_general("Single stepping: %d\n", flag_single_step);
    log_general("translate opt: %d\n", flag_translate_opt);
    log_general("File path: %s\n", file_path);

    if (file_path == NULL) {
        dprintf(2, "Bad. Invalid file path.\n");
        return 2;
    }

    log_general("Initializing transcoding...\n");

    char *temp = argv[optind - 1];
    argv[optind - 1] = file_path;
    argv[fileIndex] = temp;

    int guestArgc = argc - optind + 1;
    char **guestArgv = argv + (optind - 1);
    return transcode_loop(file_path, guestArgc, guestArgv);

}

#endif //TESTING

int start_transcode(const char *file_path) {
    log_general("extern transcode start!\n");
    return transcode_loop(file_path, 0, NULL);
}

int transcode_loop(const char *file_path, int guestArgc, char **guestArgv) {
    t_risc_elf_map_result result = mapIntoMemory(file_path);
    if (!result.valid) {
        dprintf(2, "Bad. Failed to map into memory.\n");
        return 1;
    }

    setupBrk(result.dataEnd);

    t_risc_addr next_pc = result.entry;

    //allocate stack
    char *string = "";
    t_risc_addr stackAddr = createStack(guestArgc, guestArgv, result);
    if (!stackAddr) {
        return 1;
    }

    set_value((t_risc_reg) sp, stackAddr);

    init_hash_table();

    set_value(pc,next_pc);

    //debugging output
    if (flag_log_reg_dump) {
        dump_registers();
    }

    while(!finalize) {
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
    ((void_asm)loc)(); //call asm code

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
