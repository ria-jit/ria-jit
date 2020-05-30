//
// Created by flo on 21.05.20.
//

#include "translate_csr.hpp"
#include <iostream>
#include "register.h"

using namespace asmjit;

/**
* Translate the CSRRW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRW(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate CSRRW…" << std::endl;
}

/**
* Translate the CSRRS instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRS(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate CSRRS…" << std::endl;
}

/**
* Translate the CSRRC instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRC(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate CSRRC…" << std::endl;
}

/**
* Translate the CSRRWI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRWI(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate CSRRWI…" << std::endl;
}

/**
* Translate the CSRRSI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRSI(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate CSRRSI…" << std::endl;
}

/**
* Translate the CSRRCI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRCI(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate CSRRCI…" << std::endl;
}