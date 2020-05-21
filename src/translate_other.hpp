//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_OTHER_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_OTHER_HPP

#include "util.h"
#include "translate.hpp"

void translate_FENCE(t_risc_instr instr, register_info r_info);

void translate_ECALL(t_risc_instr instr, register_info r_info);

void translate_EBREAK(t_risc_instr instr, register_info r_info);

void translate_FENCE_I(t_risc_instr instr, register_info r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_OTHER_HPP
