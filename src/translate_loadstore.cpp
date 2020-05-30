//
// Created by flo on 21.05.20.
//

#include "translate_loadstore.hpp"
#include <iostream>
#include "register.h"

/**
* Translate the LB instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_LB(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate LB…" << std::endl;
}

/**
* Translate the LH instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_LH(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate LH…" << std::endl;
}

/**
* Translate the LW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_LW(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate LW…" << std::endl;
}

/**
* Translate the LBU instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_LBU(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate LBU…" << std::endl;
}

/**
* Translate the LHU instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_LHU(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate LHU…" << std::endl;
}

/**
* Translate the SB instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SB(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate SB…" << std::endl;
}

/**
* Translate the SH instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SH(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate SH…" << std::endl;
}

/**
* Translate the SW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SW(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate SW…" << std::endl;
}

/**
* Translate the LWU instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_LWU(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate LWU…" << std::endl;
}

/**
* Translate the LD instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_LD(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate LD…" << std::endl;
}

/**
* Translate the SD instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SD(const t_risc_instr &instr, const register_info &r_info) {
    std::cout << "Translate SD…" << std::endl;
}