//
// Created by flo on 09.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_HPP

#include "cache.h"

#ifdef __cplusplus
extern "C" {
#endif

void test_generation();

//basic block translation management
void init_block();
t_cache_loc finalize_block();

///basic block translation
//t_cache_loc translate_block(t_risc_addr risc_addr);

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_HPP
