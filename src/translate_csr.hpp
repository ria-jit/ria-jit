//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CSR_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CSR_HPP

#include "util.h"
#include "translate.hpp"

void translate_CSRRW(t_risc_instr instr, register_info r_info);

void translate_CSRRS(t_risc_instr instr, register_info r_info);

void translate_CSRRC(t_risc_instr instr, register_info r_info);

void translate_CSRRWI(t_risc_instr instr, register_info r_info);

void translate_CSRRSI(t_risc_instr instr, register_info r_info);

void translate_CSRRCI(t_risc_instr instr, register_info r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_CSR_HPP
