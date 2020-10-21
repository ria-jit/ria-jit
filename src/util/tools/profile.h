//
// Created by Flo Schmidt on 21.10.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_PROFILE_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_PROFILE_H

#include <util/typedefs.h>
#include <util/log.h>

__attribute__((unused))
uint64_t  *get_gp_usage_file(void);

__attribute__((unused))
uint64_t  *get_fp_usage_file(void);

void profile_cache_access(void);

void dump_register_stats(void);

void dump_cache_stats(void);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_PROFILE_H
