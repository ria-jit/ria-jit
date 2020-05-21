//
// Created by flo on 21.05.20.
//

#include "translate_arithmetic.hpp"
#include <iostream>
#include "register.h"

using namespace asmjit;

/**
 * ADDIW adds the sign-extended 12-bit immediate to register rs1 and produces the
 * proper sign-extension of a 32-bit result in rd.
 * @param instr
 */
void translate_addiw(t_risc_instr instr, register_info r_info) {
    // mov rd, rs1
    // add rd, instr.imm

    std::cout << "Translate addiw...\n";

    //sign-extend the immediate to 32-bit
    uint64_t imm = instr.imm & 0x00000FFF;
    if (0x00000800 & imm) {
        imm += 0xFFFFF000;
    }

    //add 32-bit, sign-extend to 64-bit and write back
    auto reg_base = reinterpret_cast<uint64_t>(get_reg_data());
    a->mov(x86::edx, x86::ptr(reg_base, 8 * instr.reg_src_1));
    a->add(x86::edx, imm);
    a->movsx(x86::rax, x86::edx);
    a->mov(x86::ptr(reg_base, 8 * instr.reg_dest), x86::rax);
}

/**
 * SLLI is a logical left shift.
 * The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
 * @param instr
 */
void translate_slli(t_risc_instr instr, register_info r_info) {
    //mov rd, rs1
    //shl rd, (instr.imm & 0x3F)
    std::cout << "Translate slli...\n";
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->shl(r_info.map[instr.reg_dest], instr.imm & 0b111111);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->shl(x86::rax, instr.imm & 0b111111);
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
 * LUI places the 20-bit U-immediate into bits 31-12 of register rd and places zero in the lowest 12 bits.
 * The 32-bit result is sign-extended to 64 bits.
 * @param instr
 */
void translate_lui(t_risc_instr instr, register_info r_info) {
    //mov rd, extended
    std::cout << "Translate lui...\n";

    //prepare immediate (20-bit into 31-12, sign extend to 64)
    uint64_t prepared = instr.imm;

    //shift left by 12-bits so as to leave the lower 12 bits zero
    prepared <<= 12;

    //sign-extend to 64-bit if the sign-bit is set
    if (0x80000000 & prepared) prepared |= 0xFFFFFFFF00000000;

    //move into register
    if (r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], prepared);
    } else {
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), prepared);
    }
}

/**
 * ADDI adds the sign-extended 12-bit immediate to register rs1.
 * Overflow is ignored and the result is the low (in our case) 64 bit of the result.
 * The result is stored in rd.
 * @param instr
 */
void translate_addi(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate addi...\n";

    //prepare immediate (sign-extend to 64-bit)
    uint64_t prepared = instr.imm;
    if (0x800 & prepared) prepared |= 0xFFFFFFFFFFFFF000;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->add(r_info.map[instr.reg_dest], prepared);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->add(x86::rax, prepared);
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the AUIPC instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_AUIPC(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate AUIPC…" << std::endl;
}

/**
* Translate the SLTI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLTI(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SLTI…" << std::endl;
}

/**
* Translate the SLTIU instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLTIU(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SLTIU…" << std::endl;
}

/**
* Translate the XORI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_XORI(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate XORI…" << std::endl;
}

/**
* Translate the ORI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ORI(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate ORI…" << std::endl;
}

/**
* Translate the ANDI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ANDI(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate ANDI…" << std::endl;
}

/**
* Translate the SRLI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRLI(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SRLI…" << std::endl;
}

/**
* Translate the SRAI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRAI(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SRAI…" << std::endl;
}

/**
* Translate the ADD instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ADD(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate ADD…" << std::endl;
}

/**
* Translate the SUB instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SUB(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SUB…" << std::endl;
}

/**
* Translate the SLL instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLL(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SLL…" << std::endl;
}

/**
* Translate the SLT instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLT(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SLT…" << std::endl;
}

/**
* Translate the SLTU instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLTU(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SLTU…" << std::endl;
}

/**
* Translate the XOR instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_XOR(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate XOR…" << std::endl;
}

/**
* Translate the SRL instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRL(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SRL…" << std::endl;
}

/**
* Translate the SRA instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRA(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SRA…" << std::endl;
}

/**
* Translate the OR instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_OR(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate OR…" << std::endl;
}

/**
* Translate the AND instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_AND(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate AND…" << std::endl;
}

/**
* Translate the SLLIW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLLIW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SLLIW…" << std::endl;
}

/**
* Translate the SRLIW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRLIW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SRLIW…" << std::endl;
}

/**
* Translate the SRAIW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRAIW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SRAIW…" << std::endl;
}

/**
* Translate the ADDW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ADDW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate ADDW…" << std::endl;
}

/**
* Translate the SUBW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SUBW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SUBW…" << std::endl;
}

/**
* Translate the SLLW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLLW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SLLW…" << std::endl;
}

/**
* Translate the SRLW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRLW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SRLW…" << std::endl;
}

/**
* Translate the SRAW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRAW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate SRAW…" << std::endl;
}
