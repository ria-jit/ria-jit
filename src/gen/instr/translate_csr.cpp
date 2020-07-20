//
// Created by flo on 21.05.20.
//

#include "translate_csr.hpp"
#include "runtime/register.h"

/**
* Translate the CSRRW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate CSRRW…\n");
    critical_not_yet_implemented("CSSRW not implemented");
}

/**
* Translate the CSRRS instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRS(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate CSRRS…\n");
    critical_not_yet_implemented("CSRRS not implemented");
}

/**
* Translate the CSRRC instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRC(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate CSRRC…\n");
    critical_not_yet_implemented("CSRRC not implemented");
}

/**
* Translate the CSRRWI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRWI(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate CSRRWI…\n");
    critical_not_yet_implemented("CSRRWI not implemented");
}

/**
* Translate the CSRRSI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRSI(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate CSRRSI…\n");
    critical_not_yet_implemented("CSRRSI not implemented");
}

/**
* Translate the CSRRCI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRCI(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate CSRRCI…\n");
    critical_not_yet_implemented("CSRRCI not implemented");
}