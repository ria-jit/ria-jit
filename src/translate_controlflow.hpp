//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CONTROLFLOW_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CONTROLFLOW_HPP

#include "util.h"
#include "translate.hpp"

void translate_JAL(const t_risc_instr &instr, const register_info &r_info);

void translate_JALR(const t_risc_instr &instr, const register_info &r_info);

void translate_BEQ(const t_risc_instr &instr, const register_info &r_info);

void translate_BNE(const t_risc_instr &instr, const register_info &r_info);

void translate_BLT(const t_risc_instr &instr, const register_info &r_info);

void translate_BGE(const t_risc_instr &instr, const register_info &r_info);

void translate_BLTU(const t_risc_instr &instr, const register_info &r_info);

void translate_BGEU(const t_risc_instr &instr, const register_info &r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CONTROLFLOW_HPP