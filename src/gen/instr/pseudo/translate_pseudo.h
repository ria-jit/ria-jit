//
// Created by Flo Schmidt on 12.09.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_PSEUDO_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_PSEUDO_H

#include <util/typedefs.h>

void translate_PC_NEXT_INST(t_risc_addr addr, uint64_t reg_base);

void translate_NOP(const t_risc_instr *instr, const register_info *r_info);

void translate_SILENT_NOP(const t_risc_instr *instr, const register_info *r_info);

void translate_MV(const t_risc_instr *instr, const register_info *r_info);

void translate_NOT(const t_risc_instr *instr, const register_info *r_info);

void translate_NEG(const t_risc_instr *instr, const register_info *r_info);

void translate_NEGW(const t_risc_instr *instr, const register_info *r_info);

void translate_SEXTW(const t_risc_instr *instr, const register_info *r_info);

void translate_SEQZ(const t_risc_instr *instr, const register_info *r_info);

void translate_SNEZ(const t_risc_instr *instr, const register_info *r_info);

void translate_SLTZ(const t_risc_instr *instr, const register_info *r_info);

void translate_SGTZ(const t_risc_instr *instr, const register_info *r_info);

void translate_FUSE_AUIPC_ADDI(const t_risc_instr *instr, const register_info *r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_PSEUDO_H
