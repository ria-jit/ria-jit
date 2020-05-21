//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CONTROLFLOW_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CONTROLFLOW_H

#include "util.h"
#include "translate.hpp"

void translate_JAL(t_risc_instr instr, register_info r_info);

void translate_JALR(t_risc_instr instr, register_info &r_info);

void translate_BEQ(t_risc_instr instr, register_info r_info);

void translate_BNE(t_risc_instr instr, register_info r_info);

void translate_BLT(t_risc_instr instr, register_info r_info);

void translate_BGE(t_risc_instr instr, register_info r_info);

void translate_BLTU(t_risc_instr instr, register_info r_info);

void translate_BGEU(t_risc_instr instr, register_info r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CONTROLFLOW_H
