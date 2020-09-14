//
// Created by Flo Schmidt on 27.07.20.
//

#include <stddef.h>
#include <util/log.h>
#include <common.h>
#include "opt.h"

t_opt_parse_result parse_cmd_arguments(int argc, char **argv) {
    char opt_char;
    int optind = 1;
    char *file_path = NULL;

    t_opt_parse_result parse_result;

    //handle no arguments passed
    if (argc <= 1) goto HELP;

    while (argv[optind] != NULL && strcmp(argv[optind], "--") != 0) {
        //Argument not an option string (not starting with '-' or only '-'
        if (strncmp(argv[optind], "-", 1) != 0 || strlen(argv[optind]) < 2) {
            goto HELP;
        }
        optind++;
        int opt_str_index = 1;
        while ((opt_char = argv[optind - 1][opt_str_index]) != '\0') {
            switch (opt_char) {
                case 'a':
                    flag_do_analyze = true;
                    break;
                case 'v':
                    printf("RISC-V -> x86-64 Dynamic Binary Translator v%s\n", translator_version);
                    parse_result.status = 1;
                    return parse_result;
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
                    file_path = argv[optind];
                    goto END_PARSING;
                case 's':
                    flag_fail_silently = true;
                    break;
                case 'd':
                    flag_single_step = true;
                    break;
                case 'm':
                    flag_translate_opt = false;
                    break;
                case 'b':
                    flag_do_benchmark = true;
                    break;
                case ':':
                case 'h':
                default:
                HELP:
                    dprintf(1, "RISC-V -> x86-64 Dynamic Binary Translator v%s\n", translator_version);
                    dprintf(1,
                            "Usage: translator <translator option(s)> -f <filename> <guest options>\n"
                            "\t-v\tShow translator version.\n"
                            "\t-g\tDisplay general verbose info\n"
                            "\t-i\tDisplay parsed RISC-V input assembly\n"
                            "\t-o\tDisplay translated output x86 assembly\n"
                            "\t-r\tDump registers on basic block boundaries\n"
                            "\t-c\tDisplay cache info\n"
                            "\t-s\tFail silently for some  error conditions. Allows continued execution, but the client "
                            "program may enter undefined states.\n"
                            "\t-d\tEnable Single stepping mode. Each instruction will be its own block.\n"
                            "\t-m\tDisable translation optimization features.\n"
                            "\t-a\tAnalyze binary. Inspects passed program binary and shows instruction mnemonics.\n"
                            "\t-b\tBenchmark execution. Times the execution of the program, excluding mapping the binary into memory.\n"
                            "\t-h\tShow this help.\n"
                    );
                    parse_result.status = 1;
                    return parse_result;
            }
            opt_str_index++;
        }
    }
    END_PARSING:

    log_general("Translator version %s\n", translator_version);
    log_general("Command line options:\n");
    log_general("General info: %d\n", flag_log_general);
    log_general("Input assembly: %d\n", flag_log_asm_in);
    log_general("Output assembly: %d\n", flag_log_asm_out);
    log_general("Register dump: %d\n", flag_log_reg_dump);
    log_general("Cache info: %d\n", flag_log_cache);
    log_general("Fail silently: %d\n", flag_fail_silently);
    log_general("Single stepping: %d\n", flag_single_step);
    log_general("Translate opt: %d\n", flag_translate_opt);
    log_general("Do analyze: %d\n", flag_do_analyze);
    log_general("Do benchmarking: %d\n", flag_do_benchmark);
    log_general("File path: %s\n", file_path);

    if (file_path == NULL) {
        dprintf(2, "Bad. Invalid file path.\n");
        parse_result.status = 2;
        return parse_result;
    }

    //we're fine, fill struct and return
    parse_result.status = 0;
    parse_result.file_path = file_path;
    parse_result.last_optind = optind;
    return parse_result;
}
