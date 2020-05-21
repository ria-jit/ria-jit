//
// Created by flo on 21.05.20.
//

#include "translate_other.hpp"
#include <iostream>
#include "register.h"

/**
* Translate the FENCE instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_FENCE(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate FENCE…" << std::endl;
}

/**
* Translate the ECALL instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ECALL(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate ECALL…" << std::endl;
}

/**
* Translate the EBREAK instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_EBREAK(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate EBREAK…" << std::endl;
}

/**
* Translate the FENCE_I instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_FENCE_I(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate FENCE_I…" << std::endl;
}
