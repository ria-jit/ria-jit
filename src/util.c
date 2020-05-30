//
// Created by flo on 02.05.20.
//

#include "util.h"
#include <stdio.h>
#include <stdbool.h>

bool verbose = false;

void not_yet_implemented(const char* info) {
    printf("%s - not yet implemented\n", info);
}

/**
 * Log the passed message to stdout if the verbose flag is set.
 * An added benefit here is that this unifies all printf/std::cout calls to one method.
 * Avoiding libc calls is thus easier.
 * @param message the message (including newline) to log.
 */
void log_verbose(const char *message) {
    if (verbose) printf("%s", message);
}
