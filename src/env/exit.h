//
// Created by Flo Schmidt on 19.10.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_EXIT_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_EXIT_H

typedef enum {
    EXIT_OK,
    FAIL_HEAP_ALLOC,
    FAIL_INVALID_PATH,
    FAIL_MAP_FILE,
    FAIL_INCOMPATIBLE,
    FAIL_ASSEMBLY_ERR,
    FAIL_NOT_IMPL,
    FAIL_INVALID_STATE,
    FAIL_INVALID_CODE
} t_exit_code;

__attribute__((noreturn))
void panic(t_exit_code reason);

char *exit_code_to_str(t_exit_code exit);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_EXIT_H
