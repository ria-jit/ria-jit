//
// Created by Flo Schmidt on 02.09.20.
//

#include "translate_d_ext.h"
#include <fadec/fadec-enc.h>

/**
 * Translate the FLD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FLD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FLD...\n");
    critical_not_yet_implemented("FLD not yet implemented.\n");
}

/**
 * Translate the FSD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSD...\n");
    critical_not_yet_implemented("FSD not yet implemented.\n");
}

/**
 * Translate the FMADDD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMADDD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMADDD...\n");
    critical_not_yet_implemented("FMADDD not yet implemented.\n");
}

/**
 * Translate the FMSUBD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMSUBD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMSUBD...\n");
    critical_not_yet_implemented("FMSUBD not yet implemented.\n");
}

/**
 * Translate the FNMSUBD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FNMSUBD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FNMSUBD...\n");
    critical_not_yet_implemented("FNMSUBD not yet implemented.\n");
}

/**
 * Translate the FNMADDD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FNMADDD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FNMADDD...\n");
    critical_not_yet_implemented("FNMADDD not yet implemented.\n");
}

/**
 * Translate the FADDD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FADDD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FADDD...\n");
    critical_not_yet_implemented("FADDD not yet implemented.\n");
}

/**
 * Translate the FSUBD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSUBD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSUBD...\n");
    critical_not_yet_implemented("FSUBD not yet implemented.\n");
}

/**
 * Translate the FMULD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMULD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMULD...\n");
    critical_not_yet_implemented("FMULD not yet implemented.\n");
}

/**
 * Translate the FDIVD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FDIVD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FDIVD...\n");
    critical_not_yet_implemented("FDIVD not yet implemented.\n");
}

/**
 * Translate the FSQRTD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSQRTD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSQRTD...\n");
    critical_not_yet_implemented("FSQRTD not yet implemented.\n");
}

/**
 * Translate the FSGNJD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSGNJD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSGNJD...\n");
    critical_not_yet_implemented("FSGNJD not yet implemented.\n");
}

/**
 * Translate the FSGNJND instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSGNJND(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSGNJND...\n");
    critical_not_yet_implemented("FSGNJND not yet implemented.\n");
}

/**
 * Translate the FSGNJXD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSGNJXD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSGNJXD...\n");
    critical_not_yet_implemented("FSGNJXD not yet implemented.\n");
}

/**
 * Translate the FMIND instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMIND(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMIND...\n");
    critical_not_yet_implemented("FMIND not yet implemented.\n");
}

/**
 * Translate the FMAXD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMAXD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMAXD...\n");
    critical_not_yet_implemented("FMAXD not yet implemented.\n");
}

/**
 * Translate the FCVTSD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTSD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTSD...\n");
    critical_not_yet_implemented("FCVTSD not yet implemented.\n");
}

/**
 * Translate the FCVTDS instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTDS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTDS...\n");
    critical_not_yet_implemented("FCVTDS not yet implemented.\n");
}

/**
 * Translate the FEQD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FEQD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FEQD...\n");
    critical_not_yet_implemented("FEQD not yet implemented.\n");
}

/**
 * Translate the FLTD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FLTD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FLTD...\n");
    critical_not_yet_implemented("FLTD not yet implemented.\n");
}

/**
 * Translate the FLED instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FLED(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FLED...\n");
    critical_not_yet_implemented("FLED not yet implemented.\n");
}

/**
 * Translate the FCLASSD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCLASSD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCLASSD...\n");
    critical_not_yet_implemented("FCLASSD not yet implemented.\n");
}

/**
 * Translate the FCVTWD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTWD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTWD...\n");
    critical_not_yet_implemented("FCVTWD not yet implemented.\n");
}

/**
 * Translate the FCVTWUD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTWUD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTWUD...\n");
    critical_not_yet_implemented("FCVTWUD not yet implemented.\n");
}

/**
 * Translate the FCVTDW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTDW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTDW...\n");
    critical_not_yet_implemented("FCVTDW not yet implemented.\n");
}

/**
 * Translate the FCVTDWU instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTDWU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTDWU...\n");
    critical_not_yet_implemented("FCVTDWU not yet implemented.\n");
}

/**
 * Translate the FCVTLD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTLD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTLD...\n");
    critical_not_yet_implemented("FCVTLD not yet implemented.\n");
}

/**
 * Translate the FCVTLUD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTLUD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTLUD...\n");
    critical_not_yet_implemented("FCVTLUD not yet implemented.\n");
}

/**
 * Translate the FMVXD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMVXD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMVXD...\n");
    critical_not_yet_implemented("FMVXD not yet implemented.\n");
}

/**
 * Translate the FCVTDL instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTDL(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTDL...\n");
    critical_not_yet_implemented("FCVTDL not yet implemented.\n");
}

/**
 * Translate the FCVTDLU instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTDLU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTDLU...\n");
    critical_not_yet_implemented("FCVTDLU not yet implemented.\n");
}

/**
 * Translate the FMVDX instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMVDX(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMVDX...\n");
    critical_not_yet_implemented("FMVDX not yet implemented.\n");
}
