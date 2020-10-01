//
// Created by Flo Schmidt on 02.09.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_D_EXT_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_D_EXT_H

#include "util/log.h"
#include "util/util.h"
#include "gen/translate.h"
#include <util/typedefs.h>

void translate_FLD(const t_risc_instr *instr, const register_info *r_info);
void translate_FSD(const t_risc_instr *instr, const register_info *r_info);
void translate_FMADDD(const t_risc_instr *instr, const register_info *r_info);
void translate_FMSUBD(const t_risc_instr *instr, const register_info *r_info);
void translate_FNMSUBD(const t_risc_instr *instr, const register_info *r_info);
void translate_FNMADDD(const t_risc_instr *instr, const register_info *r_info);
void translate_FADDD(const t_risc_instr *instr, const register_info *r_info);
void translate_FSUBD(const t_risc_instr *instr, const register_info *r_info);
void translate_FMULD(const t_risc_instr *instr, const register_info *r_info);
void translate_FDIVD(const t_risc_instr *instr, const register_info *r_info);
void translate_FSQRTD(const t_risc_instr *instr, const register_info *r_info);
void translate_FSGNJD(const t_risc_instr *instr, const register_info *r_info);
void translate_FSGNJND(const t_risc_instr *instr, const register_info *r_info);
void translate_FSGNJXD(const t_risc_instr *instr, const register_info *r_info);
void translate_FMIND(const t_risc_instr *instr, const register_info *r_info);
void translate_FMAXD(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTSD(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTDS(const t_risc_instr *instr, const register_info *r_info);
void translate_FEQD(const t_risc_instr *instr, const register_info *r_info);
void translate_FLTD(const t_risc_instr *instr, const register_info *r_info);
void translate_FLED(const t_risc_instr *instr, const register_info *r_info);
void translate_FCLASSD(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTWD(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTWUD(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTDW(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTDWU(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTLD(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTLUD(const t_risc_instr *instr, const register_info *r_info);
void translate_FMVXD(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTDL(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTDLU(const t_risc_instr *instr, const register_info *r_info);
void translate_FMVDX(const t_risc_instr *instr, const register_info *r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_D_EXT_H
