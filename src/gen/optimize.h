//
// Created by Flo Schmidt on 12.09.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_OPTIMIZE_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_OPTIMIZE_H

#include <util/typedefs.h>
#include <main/context.h>

void optimize_instr(t_risc_instr *block_cache, size_t index, size_t len);

void optimize_patterns(t_risc_instr *block_cache, size_t len);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_OPTIMIZE_H
