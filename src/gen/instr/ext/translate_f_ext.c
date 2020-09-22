//
// Created by Flo Schmidt on 02.09.20.
//

#include "translate_f_ext.h"
#include <fadec/fadec-enc.h>

/**
 * Translate the FLW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FLW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FLW...\n");
    critical_not_yet_implemented("FLW not yet implemented.\n");
}

/**
 * Translate the FSW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSW...\n");
    critical_not_yet_implemented("FSW not yet implemented.\n");
}

/**
 * Translate the FMADDS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMADDS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMADDS...\n");
    critical_not_yet_implemented("FMADDS not yet implemented.\n");
}

/**
 * Translate the FMSUBS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMSUBS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMSUBS...\n");
    critical_not_yet_implemented("FMSUBS not yet implemented.\n");
}

/**
 * Translate the FNMSUBS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FNMSUBS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FNMSUBS...\n");
    critical_not_yet_implemented("FNMSUBS not yet implemented.\n");
}

/**
 * Translate the FNMADDS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FNMADDS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FNMADDS...\n");
    critical_not_yet_implemented("FNMADDS not yet implemented.\n");
}

/**
 * Translate the FADDS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FADDS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FADDS...\n");
    critical_not_yet_implemented("FADDS not yet implemented.\n");
}

/**
 * Translate the FSUBS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSUBS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSUBS...\n");
    critical_not_yet_implemented("FSUBS not yet implemented.\n");
}

/**
 * Translate the FMULS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMULS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMULS...\n");
    critical_not_yet_implemented("FMULS not yet implemented.\n");
}

/**
 * Translate the FDIVS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FDIVS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FDIVS...\n");
    critical_not_yet_implemented("FDIVS not yet implemented.\n");
}

/**
 * Translate the FSQRTS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSQRTS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSQRTS...\n");
    critical_not_yet_implemented("FSQRTS not yet implemented.\n");
}

/**
 * Translate the FSGNJS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSGNJS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSGNJS...\n");
    critical_not_yet_implemented("FSGNJS not yet implemented.\n");
}

/**
 * Translate the FSGNJNS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSGNJNS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSGNJNS...\n");
    critical_not_yet_implemented("FSGNJNS not yet implemented.\n");
}

/**
 * Translate the FSGNJXS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSGNJXS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSGNJXS...\n");
    critical_not_yet_implemented("FSGNJXS not yet implemented.\n");
}

/**
 * Translate the FMINS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMINS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMINS...\n");
    critical_not_yet_implemented("FMINS not yet implemented.\n");
}

/**
 * Translate the FMAXS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMAXS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMAXS...\n");
    critical_not_yet_implemented("FMAXS not yet implemented.\n");
}

/**
 * Translate the FCVTWS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTWS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTWS...\n");
    critical_not_yet_implemented("FCVTWS not yet implemented.\n");
}

/**
 * Translate the FCVTWUS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTWUS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTWUS...\n");
    critical_not_yet_implemented("FCVTWUS not yet implemented.\n");
}

/**
 * Translate the FMVXW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMVXW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMVXW...\n");
    critical_not_yet_implemented("FMVXW not yet implemented.\n");
}

/**
 * Translate the FEQS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FEQS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FEQS...\n");
    critical_not_yet_implemented("FEQS not yet implemented.\n");
}

/**
 * Translate the FLTS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FLTS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FLTS...\n");
    critical_not_yet_implemented("FLTS not yet implemented.\n");
}

/**
 * Translate the FLES instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FLES(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FLES...\n");
    critical_not_yet_implemented("FLES not yet implemented.\n");
}

/**
 * Translate the FCLASSS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCLASSS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCLASSS...\n");
    critical_not_yet_implemented("FCLASSS not yet implemented.\n");
}

/**
 * Translate the FCVTSW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTSW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTSW...\n");
    critical_not_yet_implemented("FCVTSW not yet implemented.\n");
}

/**
 * Translate the FCVTSWU instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTSWU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTSWU...\n");
    critical_not_yet_implemented("FCVTSWU not yet implemented.\n");
}

/**
 * Translate the FMVWX instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMVWX(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMVWX...\n");
    critical_not_yet_implemented("FMVWX not yet implemented.\n");
}

/**
 * Translate the FCVTLS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTLS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTLS...\n");
    critical_not_yet_implemented("FCVTLS not yet implemented.\n");
}

/**
 * Translate the FCVTLUS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTLUS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTLUS...\n");
    critical_not_yet_implemented("FCVTLUS not yet implemented.\n");
}

/**
 * Translate the FCVTSL instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTSL(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTSL...\n");
    critical_not_yet_implemented("FCVTSL not yet implemented.\n");
}

/**
 * Translate the FCVTSLU instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTSLU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTSLU...\n");
    critical_not_yet_implemented("FCVTSLU not yet implemented.\n");
}
