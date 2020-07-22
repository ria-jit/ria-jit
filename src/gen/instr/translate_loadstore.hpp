//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_LOADSTORE_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_LOADSTORE_HPP

#include "util/log.h"
#include "gen/translate.hpp"
#include <util/typedefs.h>

void translate_LB(const t_risc_instr &instr, const register_info &r_info);

void translate_LH(const t_risc_instr &instr, const register_info &r_info);

void translate_LW(const t_risc_instr &instr, const register_info &r_info);

void translate_LBU(const t_risc_instr &instr, const register_info &r_info);

void translate_LHU(const t_risc_instr &instr, const register_info &r_info);

void translate_SB(const t_risc_instr &instr, const register_info &r_info);

void translate_SH(const t_risc_instr &instr, const register_info &r_info);

void translate_SW(const t_risc_instr &instr, const register_info &r_info);

void translate_LWU(const t_risc_instr &instr, const register_info &r_info);

void translate_LD(const t_risc_instr &instr, const register_info &r_info);

void translate_SD(const t_risc_instr &instr, const register_info &r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_LOADSTORE_HPP
