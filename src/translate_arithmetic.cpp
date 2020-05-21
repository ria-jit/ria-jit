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