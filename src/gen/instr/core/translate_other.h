//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_OTHER_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_OTHER_H

#include "util/log.h"
#include "gen/translate.h"
#include <util/typedefs.h>

void translate_FENCE(const t_risc_instr *instr, const register_info *r_info);

void translate_ECALL(const t_risc_instr *instr, const register_info *r_info, const context_info *c_info);

void translate_EBREAK(const t_risc_instr *instr, const register_info *r_info);

void translate_FENCE_I(const t_risc_instr *instr, const register_info *r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_OTHER_H
