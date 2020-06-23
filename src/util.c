//
// Created by flo on 02.05.20.
//

#include "util.h"
#include <common.h>
#include <stdbool.h>

bool flag_log_general = false;
bool flag_log_asm_in = false;
bool flag_log_asm_out = false;
bool flag_log_reg_dump = false;
bool flag_log_cache = false;

void not_yet_implemented(const char *info) {
    log_general("%s - not yet implemented\n", info);
}

/**
 * Log the passed message to stdout if the verbose flag is set.
 * An added benefit here is that this unifies all printf/std::cout calls to one method.
 * Avoiding libc calls is thus easier.
 * Format strings can be used in the same way as they can be when using printf directly.
 * This is also true for all other logging functions.
 * @param message the message (including newline) to log.
 */
void log_general(const char *format, ...) {
    if (flag_log_general) {
        va_list args;
        va_start(args, format);
        printf("[general] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_asm_in(const char* format, ...) {
    if (flag_log_asm_in) {
        va_list args;
        va_start(args, format);
        printf("[asm-in] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_asm_out(const char* format, ...) {
    if (flag_log_asm_out) {
        va_list args;
        va_start(args, format);
        printf("[asm-out] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_reg_dump(const char* format, ...) {
    if (flag_log_reg_dump) {
        va_list args;
        va_start(args, format);
        printf("[reg-dump] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_cache(const char* format, ...) {
    if (flag_log_cache) {
        va_list args;
        va_start(args, format);
        printf("[cache] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_print_mem(const char* ptr, int len) {
    char buffer[2];
    for (const char *ptri = ptr; ptri < ptr + len; ptri++) {

        buffer[0] = "0123456789abcdef"[((*ptri) >> 4) & 0xf];
        buffer[1] = "0123456789abcdef"[(*ptri) & 0xf];
        //buffer[2] = ' ';

        write(1, buffer, 2);
    }
}
