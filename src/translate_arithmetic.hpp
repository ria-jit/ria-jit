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

void translate_AUIPC(t_risc_instr instr, register_info r_info);

void translate_SLTI(t_risc_instr instr, register_info r_info);

void translate_SLTIU(t_risc_instr instr, register_info r_info);

void translate_XORI(t_risc_instr instr, register_info r_info);

void translate_ORI(t_risc_instr instr, register_info r_info);

void translate_ANDI(t_risc_instr instr, register_info r_info);

void translate_SRLI(t_risc_instr instr, register_info r_info);

void translate_SRAI(t_risc_instr instr, register_info r_info);

void translate_ADD(t_risc_instr instr, register_info r_info);

void translate_SUB(t_risc_instr instr, register_info r_info);

void translate_SLL(t_risc_instr instr, register_info r_info);

void translate_SLT(t_risc_instr instr, register_info r_info);

void translate_SLTU(t_risc_instr instr, register_info r_info);

void translate_XOR(t_risc_instr instr, register_info r_info);

void translate_SRL(t_risc_instr instr, register_info r_info);

void translate_SRA(t_risc_instr instr, register_info r_info);

void translate_OR(t_risc_instr instr, register_info r_info);

void translate_AND(t_risc_instr instr, register_info r_info);

void translate_SLLIW(t_risc_instr instr, register_info r_info);

void translate_SRLIW(t_risc_instr instr, register_info r_info);

void translate_SRAIW(t_risc_instr instr, register_info r_info);

void translate_ADDW(t_risc_instr instr, register_info r_info);

void translate_SUBW(t_risc_instr instr, register_info r_info);

void translate_SLLW(t_risc_instr instr, register_info r_info);

void translate_SRLW(t_risc_instr instr, register_info r_info);

void translate_SRAW(t_risc_instr instr, register_info r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_ARITHMETIC_HPP
