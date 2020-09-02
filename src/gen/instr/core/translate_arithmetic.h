//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_ARITHMETIC_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_ARITHMETIC_H

#include "util/log.h"
#include "gen/translate.h"
#include <util/typedefs.h>

void translate_addi(const t_risc_instr *instr, const register_info *r_info);

void translate_lui(const t_risc_instr *instr, const register_info *r_info);

void translate_slli(const t_risc_instr *instr, const register_info *r_info);

void translate_addiw(const t_risc_instr *instr, const register_info *r_info);

void translate_AUIPC(const t_risc_instr *instr, const register_info *r_info);

void translate_SLTI(const t_risc_instr *instr, const register_info *r_info);

void translate_SLTIU(const t_risc_instr *instr, const register_info *r_info);

void translate_XORI(const t_risc_instr *instr, const register_info *r_info);

void translate_ORI(const t_risc_instr *instr, const register_info *r_info);

void translate_ANDI(const t_risc_instr *instr, const register_info *r_info);

void translate_SRLI(const t_risc_instr *instr, const register_info *r_info);

void translate_SRAI(const t_risc_instr *instr, const register_info *r_info);

void translate_ADD(const t_risc_instr *instr, const register_info *r_info);

void translate_SUB(const t_risc_instr *instr, const register_info *r_info);

void translate_SLL(const t_risc_instr *instr, const register_info *r_info);

void translate_SLT(const t_risc_instr *instr, const register_info *r_info);

void translate_SLTU(const t_risc_instr *instr, const register_info *r_info);

void translate_XOR(const t_risc_instr *instr, const register_info *r_info);

void translate_SRL(const t_risc_instr *instr, const register_info *r_info);

void translate_SRA(const t_risc_instr *instr, const register_info *r_info);

void translate_OR(const t_risc_instr *instr, const register_info *r_info);

void translate_AND(const t_risc_instr *instr, const register_info *r_info);

void translate_SLLIW(const t_risc_instr *instr, const register_info *r_info);

void translate_SRLIW(const t_risc_instr *instr, const register_info *r_info);

void translate_SRAIW(const t_risc_instr *instr, const register_info *r_info);

void translate_ADDW(const t_risc_instr *instr, const register_info *r_info);

void translate_SUBW(const t_risc_instr *instr, const register_info *r_info);

void translate_SLLW(const t_risc_instr *instr, const register_info *r_info);

void translate_SRLW(const t_risc_instr *instr, const register_info *r_info);

void translate_SRAW(const t_risc_instr *instr, const register_info *r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_ARITHMETIC_H
