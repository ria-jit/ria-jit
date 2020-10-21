//
// Created by flo on 02.05.20.
//

#include "log.h"
#include "typedefs.h"
#include <env/flags.h>
#include <common.h>
#include <stdbool.h>
#include <util/version.h>
#include <env/exit.h>

/**
 * Version number of our translator. Keep up to date - see GitLab releases.
 */
const char *const translator_version = VERSION;

void critical_not_yet_implemented(const char *info, ...) {
    va_list args;
    va_start(args, info);

    if (flag_fail_silently) {
        dprintf(1, "Warning: ");
        vdprintf(1, info, args);
        dprintf(1, " - not yet implemented\n");
    } else {
        //fail fast, so write to stderr, then quit
        dprintf(2, "Critical: ");
        vdprintf(2, info, args);
        dprintf(2, " - not yet implemented\n");
        panic(FAIL_NOT_IMPL);
        //exited
    }

    va_end(args);
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

void log_syscall(const char *format, ...) {
    if (flag_log_syscall) {
        va_list args;
        va_start(args, format);
        printf("[syscall] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_analyze(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[analyze] ");
    vdprintf(1, format, args);
    va_end(args);
}

void log_asm_in(const char *format, ...) {
    if (flag_log_asm_in) {
        va_list args;
        va_start(args, format);
        printf("[asm-in] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_asm_out(const char *format, ...) {
    if (flag_log_asm_out) {
        va_list args;
        va_start(args, format);
        printf("[asm-out] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_reg_dump(const char *format, ...) {
    if (flag_log_reg_dump) {
        va_list args;
        va_start(args, format);
        printf("[reg-dump] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_context(const char *format, ...) {
    if (flag_log_context) {
        va_list args;
        va_start(args, format);
        printf("[context] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_cache(const char *format, ...) {
    if (flag_log_cache) {
        va_list args;
        va_start(args, format);
        printf("[cache] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_benchmark(const char *format, ...) {
    if (flag_do_benchmark) {
        va_list args;
        va_start(args, format);
        printf("[benchmark] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_profile(const char *format, ...) {
    if (flag_do_profile) {
        va_list args;
        va_start(args, format);
        printf("[profile] ");
        vdprintf(1, format, args);
        va_end(args);
        return;
    }
}

void log_print_mem(const char *ptr, long int len) {
    char buffer[2];
    for (const char *ptri = ptr; ptri < ptr + len; ptri++) {

        buffer[0] = "0123456789abcdef"[((*ptri) >> 4) & 0xf];
        buffer[1] = "0123456789abcdef"[(*ptri) & 0xf];
        //buffer[2] = ' ';

        write(1, buffer, 2);
    }
}

void invalid_error_handler(int32_t errorcode, int32_t raw_instr, t_risc_addr addr) {
    dprintf(2, "Critical: tried to execute invalid code 0x%x at %p\n"
               "error: %s\n", raw_instr, (void *) addr, errorcode_to_string(errorcode));
    panic(FAIL_INVALID_CODE);
}
