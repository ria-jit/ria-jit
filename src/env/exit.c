//
// Created by Flo Schmidt on 19.10.20.
//

#include <common.h>
#include "exit.h"

void panic(t_exit_code reason) {
    dprintf(2, "Fatal error: %s (%u)\n", exit_code_to_str(reason), reason);
    _exit(reason);
}

char *exit_code_to_str(t_exit_code code) {
    switch (code) {
        case EXIT_OK:
            return "EXIT_OK";
        case FAIL_HEAP_ALLOC:
            return "FAIL_HEAP_ALLOC";
        case FAIL_INVALID_PATH:
            return "FAIL_INVALID_PATH";
        case FAIL_MAP_FILE:
            return "FAIL_MAP_FILE";
        case FAIL_INCOMPATIBLE:
            return "FAIL_INCOMPATIBLE";
        case FAIL_ASSEMBLY_ERR:
            return "FAIL_ASSEMBLY_ERR";
        case FAIL_NOT_IMPL:
            return "FAIL_NOT_IMPL";
        case FAIL_INVALID_STATE:
            return "FAIL_INVALID_STATE";
        case FAIL_INVALID_CODE:
            return "FAIL_INVALID_CODE";
        default:
            return "UNKNOWN";
    }
}
