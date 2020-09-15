//
// Created by Flo Schmidt on 14.07.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_A_EXT_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_A_EXT_H

#include "util/log.h"
#include "gen/translate.h"
#include <util/typedefs.h>

void translate_LRW(const t_risc_instr *instr, const register_info *r_info);

void translate_SCW(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOSWAPW(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOADDW(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOXORW(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOANDW(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOORW(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOMINW(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOMAXW(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOMINUW(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOMAXUW(const t_risc_instr *instr, const register_info *r_info);

void translate_LRD(const t_risc_instr *instr, const register_info *r_info);

void translate_SCD(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOSWAPD(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOADDD(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOXORD(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOANDD(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOORD(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOMIND(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOMAXD(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOMINUD(const t_risc_instr *instr, const register_info *r_info);

void translate_AMOMAXUD(const t_risc_instr *instr, const register_info *r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_A_EXT_H
