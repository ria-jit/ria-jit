//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_M_EXT_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_M_EXT_HPP

#include "util.h"
#include "translate.hpp"

void translate_MUL(const t_risc_instr &instr, const register_info &r_info);

void translate_MULH(const t_risc_instr &instr, const register_info &r_info);

void translate_MULHSU(const t_risc_instr &instr, const register_info &r_info);

void translate_MULHU(const t_risc_instr &instr, const register_info &r_info);

void translate_DIV(const t_risc_instr &instr, const register_info &r_info);

void translate_DIVU(const t_risc_instr &instr, const register_info &r_info);

void translate_REM(const t_risc_instr &instr, const register_info &r_info);

void translate_REMU(const t_risc_instr &instr, const register_info &r_info);

void translate_MULW(const t_risc_instr &instr, const register_info &r_info);

void translate_DIVW(const t_risc_instr &instr, const register_info &r_info);

void translate_DIVUW(const t_risc_instr &instr, const register_info &r_info);

void translate_REMW(const t_risc_instr &instr, const register_info &r_info);

void translate_REMUW(const t_risc_instr &instr, const register_info &r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_M_EXT_HPP
