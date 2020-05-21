//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_M_EXT_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_M_EXT_HPP

#include "util.h"
#include "translate.hpp"

void translate_MUL(t_risc_instr instr, register_info r_info);

void translate_MULH(t_risc_instr instr, register_info r_info);

void translate_MULHSU(t_risc_instr instr, register_info r_info);

void translate_MULHU(t_risc_instr instr, register_info r_info);

void translate_DIV(t_risc_instr instr, register_info r_info);

void translate_DIVU(t_risc_instr instr, register_info r_info);

void translate_REM(t_risc_instr instr, register_info r_info);

void translate_REMU(t_risc_instr instr, register_info r_info);

void translate_MULW(t_risc_instr instr, register_info r_info);

void translate_DIVW(t_risc_instr instr, register_info r_info);

void translate_DIVUW(t_risc_instr instr, register_info r_info);

void translate_REMW(t_risc_instr instr, register_info r_info);

void translate_REMUW(t_risc_instr instr, register_info r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_M_EXT_HPP
