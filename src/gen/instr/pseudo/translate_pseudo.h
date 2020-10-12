//
// Created by Flo Schmidt on 12.09.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_PSEUDO_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_PSEUDO_H

#include <util/typedefs.h>

void translate_PC_NEXT_INST(t_risc_addr addr, uint64_t reg_base);

void translate_SILENT_NOP(const t_risc_instr *instr, const register_info *r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_PSEUDO_H
