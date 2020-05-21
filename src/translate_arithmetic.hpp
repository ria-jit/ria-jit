//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_ARITHMETIC_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_ARITHMETIC_HPP


#include "util.h"
#include "translate.hpp"

void translate_addi(t_risc_instr instr, register_info r_info);

void translate_lui(t_risc_instr instr, register_info r_info);

void translate_slli(t_risc_instr instr, register_info r_info);

void translate_addiw(t_risc_instr instr, register_info r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_ARITHMETIC_HPP
