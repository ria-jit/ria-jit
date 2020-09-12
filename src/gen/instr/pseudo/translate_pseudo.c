//
// Created by Flo Schmidt on 12.09.20.
//

#include <gen/translate.h>
#include "translate_pseudo.h"

///set the pc to next addr after inst
/**
 * Translate the PC_NEXT_INST pseudo instruction that is inserted when a block would overflow our buffer.
 * Emits instructions that sets the PC RISC_V register in our gp_reg_file to the given address.
 * @param addr the address the risc PC reg should have at the end of the current block (the start address of the next
 *        block)
 * @param reg_base the base_address of the gp_reg_file.
 */
void translate_PC_NEXT_INST(const t_risc_addr addr, uint64_t reg_base) {
    ///set pc
    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, addr);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(reg_base + 8 * pc), FE_AX);
}

/**
 * Translate the NOP instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_NOP(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate NOP…\n");
    critical_not_yet_implemented("NOP not yet implemented.\n");
}

/**
 * Translate the SILENT_NOP instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SILENT_NOP(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SILENT_NOP…\n");
    critical_not_yet_implemented("SILENT_NOP not yet implemented.\n");
}

/**
 * Translate the MV instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_MV(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate MV…\n");
    critical_not_yet_implemented("MV not yet implemented.\n");
}

/**
 * Translate the NOT instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_NOT(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate NOT…\n");
    critical_not_yet_implemented("NOT not yet implemented.\n");
}

/**
 * Translate the NEG instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_NEG(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate NEG…\n");
    critical_not_yet_implemented("NEG not yet implemented.\n");
}

/**
 * Translate the NEGW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_NEGW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate NEGW…\n");
    critical_not_yet_implemented("NEGW not yet implemented.\n");
}

/**
 * Translate the SEXTW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SEXTW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SEXTW…\n");
    critical_not_yet_implemented("SEXTW not yet implemented.\n");
}

/**
 * Translate the SEQZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SEQZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SEQZ…\n");
    critical_not_yet_implemented("SEQZ not yet implemented.\n");
}

/**
 * Translate the SNEZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SNEZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SNEZ…\n");
    critical_not_yet_implemented("SNEZ not yet implemented.\n");
}

/**
 * Translate the SLTZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SLTZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SLTZ…\n");
    critical_not_yet_implemented("SLTZ not yet implemented.\n");
}

/**
 * Translate the SGTZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SGTZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SGTZ…\n");
    critical_not_yet_implemented("SGTZ not yet implemented.\n");
}
