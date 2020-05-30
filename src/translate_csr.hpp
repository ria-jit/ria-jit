//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CSR_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CSR_HPP

#include "util.h"
#include "translate.hpp"

void translate_CSRRW(const t_risc_instr &instr, const register_info &r_info);

void translate_CSRRS(const t_risc_instr &instr, const register_info &r_info);

void translate_CSRRC(const t_risc_instr &instr, const register_info &r_info);

void translate_CSRRWI(const t_risc_instr &instr, const register_info &r_info);

void translate_CSRRSI(const t_risc_instr &instr, const register_info &r_info);

void translate_CSRRCI(const t_risc_instr &instr, const register_info &r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CSR_HPP
