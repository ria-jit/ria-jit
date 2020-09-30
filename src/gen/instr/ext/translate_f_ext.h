//
// Created by Flo Schmidt on 02.09.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_F_EXT_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_F_EXT_H

#include "util/log.h"
#include "util/util.h"
#include "gen/translate.h"
#include <util/typedefs.h>

void translate_FLW(const t_risc_instr *instr, const register_info *r_info);
void translate_FSW(const t_risc_instr *instr, const register_info *r_info);
void translate_FMADDS(const t_risc_instr *instr, const register_info *r_info);
void translate_FMSUBS(const t_risc_instr *instr, const register_info *r_info);
void translate_FNMSUBS(const t_risc_instr *instr, const register_info *r_info);
void translate_FNMADDS(const t_risc_instr *instr, const register_info *r_info);
void translate_FADDS(const t_risc_instr *instr, const register_info *r_info);
void translate_FSUBS(const t_risc_instr *instr, const register_info *r_info);
void translate_FMULS(const t_risc_instr *instr, const register_info *r_info);
void translate_FDIVS(const t_risc_instr *instr, const register_info *r_info);
void translate_FSQRTS(const t_risc_instr *instr, const register_info *r_info);
void translate_FSGNJS(const t_risc_instr *instr, const register_info *r_info);
void translate_FSGNJNS(const t_risc_instr *instr, const register_info *r_info);
void translate_FSGNJXS(const t_risc_instr *instr, const register_info *r_info);
void translate_FMINS(const t_risc_instr *instr, const register_info *r_info);
void translate_FMAXS(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTWS(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTWUS(const t_risc_instr *instr, const register_info *r_info);
void translate_FMVXW(const t_risc_instr *instr, const register_info *r_info);
void translate_FEQS(const t_risc_instr *instr, const register_info *r_info);
void translate_FLTS(const t_risc_instr *instr, const register_info *r_info);
void translate_FLES(const t_risc_instr *instr, const register_info *r_info);
void translate_FCLASSS(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTSW(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTSWU(const t_risc_instr *instr, const register_info *r_info);
void translate_FMVWX(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTLS(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTLUS(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTSL(const t_risc_instr *instr, const register_info *r_info);
void translate_FCVTSLU(const t_risc_instr *instr, const register_info *r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_F_EXT_H
