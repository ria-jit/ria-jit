//
// Created by flo on 10.05.20.
//

#include "register.h"

/**
 * The raw register contents stored in memory.
 * Access will be relatively expensive, so optimisation is needed.
 * contents[0] is undefined, as x0 is hardwired to 0 -
 * it is left unused in order to avoid off-by-one errors in indexing.
 */
t_risc_reg_val gp_file[N_REG];

/**
 * Contents of the CSR registers stored in memory.
 * For allocated addresses see the enum type t_risc_csr_reg.
 */
t_risc_reg_val csr_file[N_CSR];

/**
 * Swap space for the 7 callee-saved registers we are using for loading the context.
 * Keeping the values here avoids stack consistency issues.
 * See context.c for reference.
 */
uint64_t swap_file[7];

/**
 * Usage array for profiler.
 * Used to count register accesses during program execution.
 */
uint64_t usage[N_REG];

t_risc_reg_val *get_gp_reg_file(void) {
    return gp_file;
}

t_risc_reg_val *get_csr_reg_file(void) {
    return csr_file;
}

uint64_t *get_swap_file(void) {
    return swap_file;
}

uint64_t *get_usage_file(void) {
    return usage;
}

/**
 * Get the value currently in the passed register.
 * @param reg the register to lookup
 * @return value in register reg
 */
t_risc_reg_val get_value(t_risc_reg reg) {
    if (reg == x0) {
        //an access to x0 always yields 0
        return 0;
    } else {
        return gp_file[reg];
    }

}

/**
 * Set the value of the passed register.
 * @param reg the register to update
 * @param val the new value
 */
void set_value(t_risc_reg reg, t_risc_reg_val val) {
    //a write to x0 is ignored, hardwired zero
    if (reg != x0) {
        gp_file[reg] = val;
    }
}

/**
 * Dump the contents of the register file.
 */
void dump_gp_registers(void) {
    log_reg_dump("Register file contents:\n");

    //dump all registers:
    for (t_risc_reg i = x0; i < pc; ++i) {
        log_reg_dump("%s/%s\t0x%lx\n", reg_to_string(i), reg_to_alias(i), get_value(i));
    }

    //nice pc output
    log_reg_dump("pc\t\t0x%lx\n", get_value(pc));
}

/**
 * Dump the profiler register usage data.
 */
void dump_profiler_data(void) {
    log_profile("Register hits (unsorted):\n");
    log_profile("==============\n");

    //dump for all registers
    //unsorted for easier statistical usage of the data
    for (size_t i = x0; i <= x31; i++) {
        log_profile("%s (%s): %li\n", reg_to_string(i), reg_to_alias(i), usage[i]);
    }

    //ranked by usage
    int regRanked[N_REG];
    for (int i = 0; i < N_REG; i++) {
        regRanked[i] = i;
    }
    ///insertion sort:
    {
        int key, j;
        for (int i = 1; i < N_REG; i++) {
            key = regRanked[i];
            j = i - 1;

            ///move move elements with index < i && element > i one to the left
            while (j >= 0 && usage[regRanked[j]] < usage[key]) {
                regRanked[j + 1] = regRanked[j];
                j--;
            }

            ///insert former element i to correct position
            regRanked[j + 1] = key;
        }
    }

    log_profile("Register hits (ranked):\n");
    log_profile("==============\n");
    for (size_t i = x0; i <= x31; i++) {
        //when we hit zero, we've listed all hits
        if (usage[regRanked[i]] == 0) break;
        log_profile("%s (%s): %li\n", reg_to_string(regRanked[i]), reg_to_alias(regRanked[i]), usage[regRanked[i]]);
    }
}
