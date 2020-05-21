//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_LOADSTORE_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_LOADSTORE_HPP

#include "util.h"
#include "translate.hpp"

void translate_LB(t_risc_instr instr, register_info r_info);

void translate_LH(t_risc_instr instr, register_info r_info);

void translate_LW(t_risc_instr instr, register_info r_info);

void translate_LBU(t_risc_instr instr, register_info r_info);

void translate_LHU(t_risc_instr instr, register_info r_info);

void translate_SB(t_risc_instr instr, register_info r_info);

void translate_SH(t_risc_instr instr, register_info r_info);

void translate_SW(t_risc_instr instr, register_info r_info);

void translate_LWU(t_risc_instr instr, register_info r_info);

void translate_LD(t_risc_instr instr, register_info r_info);

void translate_SD(t_risc_instr instr, register_info r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_LOADSTORE_HPP
