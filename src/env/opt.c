//
// Created by Flo Schmidt on 27.07.20.
//

#include <stddef.h>
#include <util/log.h>
#include <common.h>
#include "opt.h"
#include <env/flags.h>

int perfFd = -1;

static int open_perfmap(void) {
    int pid = getpid();

    char filename[32];
    snprintf(filename, sizeof(filename), "/tmp/perf-%u.map", pid);

    return open(filename, O_CREAT | O_TRUNC | O_NOFOLLOW | O_WRONLY | O_CLOEXEC, 0600);
}

t_opt_parse_result parse_cmd_arguments(int argc, char **argv) {
    char opt_char;
    int optind = 1;
    char *file_path = NULL;

    t_opt_parse_result parse_result;

    //handle no arguments passed
    if (argc <= 1) goto HELP;

    NEXT:
    while (argv[optind] != NULL && strcmp(argv[optind], "--") != 0) {
        //Argument not an option string (not starting with '-' or only '-')
        if (strncmp(argv[optind], "-", 1) != 0 || strlen(argv[optind]) < 2) {
            goto HELP;
        }
        optind++;
        int opt_str_index = 1;
        char *option_string;
        while ((opt_char = argv[optind - 1][opt_str_index]) != '\0') {
            switch (opt_char) {
                ///long options
                case '-':
                    option_string = argv[optind - 1] + 2;
                    if (strncmp(option_string, "log=", 4) == 0) {
                        option_string += 4;
                        do {
                            if (strncmp(option_string, "general", 7) == 0) {
                                flag_log_general = true;
                                option_string += 7;
                            } else if (strncmp(option_string, "asm_in", 6) == 0) {
                                flag_log_asm_in = true;
                                option_string += 6;
                            } else if (strncmp(option_string, "asm_out", 7) == 0) {
                                flag_log_asm_out = true;
                                flag_verbose_disassembly = true;
                                option_string += 7;
                            } else if (strncmp(option_string, "reg", 3) == 0) {
                                flag_log_reg_dump = true;
                                option_string += 3;
                            } else if (strncmp(option_string, "cache", 5) == 0) {
                                flag_log_cache = true;
                                option_string += 5;
                                if (strncmp(option_string, "-contents", 9) == 0) {
                                    flag_log_cache_contents = true;
                                    option_string += 9;
                                }
                            } else if (strncmp(option_string, "strace", 6) == 0) {
                                flag_log_syscall = true;
                                option_string += 6;
                            } else if (strncmp(option_string, "verbose_disasm", 11) == 0) {
                                flag_verbose_disassembly = true;
                                option_string += 11;
                            } else if (strncmp(option_string, "context", 7) == 0) {
                                flag_log_context = true;
                                option_string += 7;
                            } else {
                                if (strncmp(option_string, "help", 4) != 0) {
                                    dprintf(2, "Warning: Unknown logging category %s...\n", option_string);
                                }
                                printf("Logging categories: --log=...\n"
                                       "\tgeneral\t\t\tGeneral logging that fits no other categories.\n"
                                       "\tasm_in\t\t\tShow parsed and raw RISC-V instructions.\n"
                                       "\tasm_out\t\t\tShow generated x86 code.\n"
                                       "\tverbose_disasm\tSeparate instruction translations in output assembly.\n"
                                       "\treg\t\t\t\tDump register contents after every block (warning: lots of logs).\n"
                                       "\tcontext\t\t\tLog execution context and mapped registers for instructions.\n"
                                       "\tcache\t\t\tLog events involving the block cache.\n"
                                       "\tcache-contents\tLog cache contents after every cache update (implies cache).\n"
                                       "\tstrace\t\t\tLog all emulated syscalls.\n");
                                parse_result.status = 1;
                                return parse_result;
                            }
                        } while (*(option_string++) == ',');
                    } else if (strncmp(option_string, "optimize=", 9) == 0) {
                        option_string += 9;
                        do {
                            if (strncmp(option_string, "no-general", 10) == 0) {
                                option_string += 10;
                                flag_translate_opt = true;
                            } else if (strncmp(option_string, "no-ras", 6) == 0) {
                                option_string += 6;
                                flag_translate_opt_ras = false;
                            } else if (strncmp(option_string, "no-chain", 8) == 0) {
                                option_string += 8;
                                flag_translate_opt_chain = false;
                            } else if (strncmp(option_string, "no-jump", 7) == 0) {
                                option_string += 7;
                                flag_translate_opt_jump = false;
                                //flag_translate_opt_ras = false;
                            } else if (strncmp(option_string, "no-fusion", 9) == 0) {
                                option_string += 9;
                                flag_translate_opt_fusion = false;
                            } else if (strncmp(option_string, "singlestep", 10) == 0) {
                                option_string += 10;
                                flag_single_step = true;
                            } else if (strncmp(option_string, "none", 4) == 0) {
                                option_string += 4;
                                flag_translate_opt_ras = false;
                                flag_translate_opt_chain = false;
                                flag_translate_opt_jump = false;
                                flag_translate_opt_fusion = false;
                                flag_translate_opt = false;
                            } else {
                                if (strncmp(option_string, "help", 4) == 0) {
                                    dprintf(2, "Warning: Unknown optimization option %s...\n", option_string);
                                }
                                printf("Optimization options: --optimize=...\n"
                                       "\tno-general\t\tDisable general optimizations.\n"
                                       "\tno-ras\t\t\tDisable return address stack.\n"
                                       "\tno-chain\t\tDisable block chaining.\n"
                                       "\tno-jump\t\t\tDisable recursive translation of jump targets (implies no-ras).\n"
                                       "\tno-fusion\t\tDisable macro opcode fusion/conversion\n"
                                       "\tnone\t\t\tAll of the above.\n"
                                       "\tsinglestep\t\tEnable single stepping mode.\n"
                                       "\t\t\t\t\tTranslates each RISC-V instruction into its own block.\n");
                                parse_result.status = 1;
                                return parse_result;
                            }
                        } while (*(option_string++) == ',');
                    } else if (strncmp(option_string, "perf", 4) == 0) {
                        perfFd = open_perfmap();
                    } else if (strncmp(option_string, "help", 4) == 0) {
                        goto HELP;
                    } else if (strncmp(option_string, "version", 7) == 0) {
                        goto VERSION;
                    } else if (strncmp(option_string, "file", 4) == 0) {
                        goto FILE;
                    } else if (strncmp(option_string, "benchmark", 9) == 0) {
                        flag_do_benchmark = true;
                    } else if (strncmp(option_string, "profile", 7) == 0) {
                        flag_do_profile = true;
                    } else if (strncmp(option_string, "fail-silently", 13) == 0) {
                        goto FAIL_SILENTLY;
                    } else if (strncmp(option_string, "analyze-all", 11) == 0) {
                        flag_do_analyze_mnem = true;
                        flag_do_analyze_reg = true;
                        flag_do_analyze_pattern = true;
                    } else if (strncmp(option_string, "analyze-mnem", 12) == 0) {
                        flag_do_analyze_mnem = true;
                    } else if (strncmp(option_string, "analyze-reg", 11) == 0) {
                        flag_do_analyze_reg = true;
                    } else if (strncmp(option_string, "analyze-pattern", 15) == 0) {
                        flag_do_analyze_pattern = true;
                    }
                    goto NEXT;
                case 'a':
                    flag_do_analyze_mnem = true;
                    flag_do_analyze_reg = true;
                    flag_do_analyze_pattern = true;
                    break;
                case 'v':
                VERSION:
                    printf("RISC-V -> x86-64 Dynamic Binary Translator %s\n", translator_version);
                    parse_result.status = 1;
                    return parse_result;
                case 'g':
                    flag_log_general = true;
                    flag_log_syscall = true;
                    break;
                case 'i':
                    flag_log_asm_in = true;
                    break;
                case 'o':
                    flag_log_asm_out = true;
                    flag_verbose_disassembly = true;
                    break;
                case 'r':
                    flag_log_reg_dump = true;
                    break;
                case 'c':
                    flag_log_cache = true;
                    break;
                case 'f':
                FILE:
                    file_path = argv[optind];
                    goto END_PARSING;
                case 's':
                FAIL_SILENTLY:
                    flag_fail_silently = true;
                    break;
                case 'd':
                    flag_single_step = true;
                    break;
                case 'm':
                    flag_translate_opt = false;
                    flag_translate_opt_jump = false;
                    flag_translate_opt_chain = false;
                    flag_translate_opt_ras = false;
                    flag_translate_opt_fusion = false;
                    break;
                case 'b':
                    flag_do_benchmark = true;
                    break;
                case 'p':
                    flag_do_profile = true;
                    break;
                case ':':
                case 'h':
                default:
                HELP:
                    dprintf(1, "RISC-V -> x86-64 Dynamic Binary Translator %s\n", translator_version);
                    dprintf(1,
                            "Usage: ./translator [translator option(s)] -f <filename> [guest options]\n"
                            "\n"
                            "Options:\n"
                            "\t-v, --version\n"
                            "\t\tShow translator version.\n"
                            "\t-f, --file <executable>\n"
                            "\t\tSpecify executable. All options after the file path are passed to the guest.\n"
                            "\t-a, --analyze-all\n"
                            "\t--analyze-mnem, --analyze-reg, --analyze-pattern\n"
                            "\t\tAnalyze the binary. Does not execute the guest program.\n"
                            "\t\tInspects passed program binary and shows the selected statistics.\n"
                            "\t-b, --benchmark\n"
                            "\t\tBenchmark execution. Times the execution of the program,\n"
                            "\t\texcluding mapping the binary into memory.\n"
                            "\t-p, --profile\n"
                            "\t\tProfile register usage. Display dynamic register usage statistics.\n"
                            "\t--perf\n"
                            "\t\tLog the generated blocks to /tmp/perf-<pid>.map for externally profiling\n"
                            "\t\tthe execution in perf.\n"
                            "\t-s, --fail-silently\n"
                            "\t\tFail silently for some error conditions.\n"
                            "\t\tAllows continued execution, but the client "
                            "program may enter undefined states.\n"
                            "\t-h, --help\n"
                            "\t\tShow this help.\n"
                            "\n"
                            "Logging:\n"
                            "\t--log=category,[...]\n"
                            "\t\tEnable logging for certain categories. See --log=help for more info.\n"
                            "\t-g\tDisplay general verbose info (--log=general,strace)\n"
                            "\t-i\tDisplay parsed RISC-V input assembly (--log=asm_in)\n"
                            "\t-o\tDisplay translated output x86 assembly (--log=asm_out,verbose_disasm)\n"
                            "\t-r\tDump registers on basic block boundaries (--log=reg)\n"
                            "\t-c\tDisplay cache info (--log=cache)\n"
                            "\n"
                            "Optimization:\n"
                            "\t--optimize=category,[...]\n"
                            "\t\tDisable certain optimization categories. See --optimize=help for more info.\n"
                            "\t-d\tEnable Single stepping mode.\n"
                            "\t\tEach instruction will be its own block. (--optimize=singlestep)\n"
                            "\t-m\tDisable all translation optimization features. (--optimize=none)\n"
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
    log_general("Logging: general %d, asm_in %d, asm_out %d, reg %d, cache %d, cache-contents %d, strace %d, verbose-disassembly %d, context %d\n",
                flag_log_general, flag_log_asm_in, flag_log_asm_out, flag_log_reg_dump, flag_log_cache,
                flag_log_cache_contents, flag_log_syscall, flag_verbose_disassembly, flag_log_context);
    log_general("Fail silently: %d\n", flag_fail_silently);
    log_general("Single stepping: %d\n", flag_single_step);
    log_general("Translate opt: general %d, ras %d, chaining %d, recurse jumps %d, singlestep %d\n", flag_translate_opt,
                flag_translate_opt_ras, flag_translate_opt_chain, flag_translate_opt_jump, flag_single_step);
    log_general("Do analyze: mnem %d, reg %d, pattern %d\n", flag_do_analyze_mnem, flag_do_analyze_reg, flag_do_analyze_pattern);
    log_general("Do benchmarking: %d\n", flag_do_benchmark);
    log_general("Do profiling: %d\n", flag_do_profile);
    log_general("File path: %s\n", file_path);

    if (file_path == NULL) {
        dprintf(2, "Error: File path not specified or invalid.\n");
        parse_result.status = 2;
        return parse_result;
    }

    //we're fine, fill struct and return
    parse_result.status = 0;
    parse_result.file_path = file_path;
    parse_result.last_optind = optind;
    return parse_result;
}
