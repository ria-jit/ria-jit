//
// Created by Flo Schmidt on 27.07.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_OPT_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_OPT_H
extern int perfFd;

typedef struct {
    int status;
    char *file_path;
    int last_optind;
} t_opt_parse_result;

t_opt_parse_result parse_cmd_arguments(int argc, char **argv);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_OPT_H
