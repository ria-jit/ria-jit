//
// Created by Flo Schmidt on 21.10.20.
//

#include <cache/cache.h>
#include "profile.h"

/**
 * Count of cache accesses.
 */
static size_t count_cache_lookups = 0;

/**
 * Usage array for profiler.
 * Used to count general purpose register accesses during program execution.
 */
uint64_t gp_usage[N_REG];

/**
 * Usage array for profiler.
 * Used to count floating point register accesses during program execution.
 */
uint64_t fp_usage[N_FP_REG];

__attribute__((unused))
uint64_t *get_gp_usage_file(void) {
    return gp_usage;
}

__attribute__((unused))
uint64_t *get_fp_usage_file(void) {
    return fp_usage;
}

void profile_cache_access(void) {
    count_cache_lookups++;
}

/**
 * Dump the profiler's cache data.
 */
void dump_cache_stats(void) {
    log_profile("Logged %lu cache lookups, total block count %lu.\n", count_cache_lookups, get_cache_entry_count());
}

/**
 * Dump the profiler register usage data.
 */
void dump_register_stats(void) {
    {
        log_profile("General purpose register hits (unsorted):\n");
        log_profile("==============\n");

        //dump for all registers
        //unsorted for easier statistical usage of the data
        for (size_t i = x0; i <= x31; i++) {
            log_profile("%s (%s): %li\n", gp_to_string(i), gp_to_alias(i), gp_usage[i]);
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
                while (j >= 0 && gp_usage[regRanked[j]] < gp_usage[key]) {
                    regRanked[j + 1] = regRanked[j];
                    j--;
                }

                ///insert former element i to correct position
                regRanked[j + 1] = key;
            }
        }

        log_profile("General purpose register hits (ranked):\n");
        log_profile("==============\n");
        for (size_t i = x0; i <= x31; i++) {
            //when we hit zero, we've listed all hits
            if (gp_usage[regRanked[i]] == 0) break;
            log_profile("%s (%s): %li\n", gp_to_string(regRanked[i]), gp_to_alias(regRanked[i]),
                        gp_usage[regRanked[i]]);
        }
    }

    {
        log_profile("Floating point register hits (unsorted):\n");
        log_profile("==============\n");

        //dump for all registers
        //unsorted for easier statistical usage of the data
        for (size_t i = 0; i < N_FP_REG; i++) {
            log_profile("%s (%s): %li\n", fp_to_string(i), fp_to_alias(i), fp_usage[i]);
        }

        //ranked by usage
        int regRanked[N_FP_REG];
        for (int i = 0; i < N_FP_REG; i++) {
            regRanked[i] = i;
        }
        ///insertion sort:
        {
            int key, j;
            for (int i = 1; i < N_FP_REG; i++) {
                key = regRanked[i];
                j = i - 1;

                ///move move elements with index < i && element > i one to the left
                while (j >= 0 && fp_usage[regRanked[j]] < fp_usage[key]) {
                    regRanked[j + 1] = regRanked[j];
                    j--;
                }

                ///insert former element i to correct position
                regRanked[j + 1] = key;
            }
        }

        log_profile("Floating point register hits (ranked):\n");
        log_profile("==============\n");
        for (size_t i = 0; i < N_FP_REG; i++) {
            //when we hit zero, we've listed all hits
            if (fp_usage[regRanked[i]] == 0) break;
            log_profile("%s (%s): %li\n", fp_to_string(regRanked[i]), fp_to_alias(regRanked[i]),
                        fp_usage[regRanked[i]]);
        }
    }
}
