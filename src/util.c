//
// Created by flo on 02.05.20.
//

#include "util.h"
#include <common.h>
#include <stdbool.h>

bool verbose = false;

void not_yet_implemented(const char *info) {
    log_verbose("%s - not yet implemented\n", info);
}

/**
 * Log the passed message to stdout if the verbose flag is set.
 * An added benefit here is that this unifies all printf/std::cout calls to one method.
 * Avoiding libc calls is thus easier.
 * Format strings can be used in the same way as they can be when using printf directly.
 * @param message the message (including newline) to log.
 */
void log_verbose(const char *format, ...) {
    if (verbose) {
        va_list args;
        va_start(args, format);
        printf("[verbose] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}
