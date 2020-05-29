//
// Created by flo on 21.05.20.
//

#include "translate_m_ext.hpp"
#include <iostream>
#include "register.h"

using namespace asmjit;

//todo: implement the correct division edge case semantics as per p. 44 of the RISC-V spec

/**
* Translate the MUL instruction.
* MUL performs an XLEN-bit×XLEN-bit multiplication of rs1 by rs2 and places the lower XLEN bits
* in the destination register.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_MUL(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate MUL…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->imul(r_info.map[instr.reg_src_2]);
        //we only need the lower XLEN = 64 bits of the result
        a->mov(r_info.map[instr.reg_dest], x86::rax);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->imul(x86::ptr(r_info.base, 8 * instr.reg_src_2));
        //we only need the lower XLEN = 64 bits of the result
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the MULH instruction.
* MULH, MULHU, and MULHSU perform the same multiplication but return
* the upper XLEN bits of the full 2×XLEN-bit product, for signed×signed, unsigned×unsigned,
* and signed rs1×unsigned rs2 multiplication, respectively.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_MULH(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate MULH…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->imul(r_info.map[instr.reg_src_2]);
        //we want the upper XLEN bits here
        a->mov(r_info.map[instr.reg_dest], x86::rdx);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->imul(x86::ptr(r_info.base, 8 * instr.reg_src_2));
        //we want the upper XLEN bits here
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rdx);
    }
}

/**
* Translate the MULHSU instruction.
* MULH, MULHU, and MULHSU perform the same multiplication but return
* the upper XLEN bits of the full 2×XLEN-bit product, for signed×signed, unsigned×unsigned,
* and signed rs1×unsigned rs2 multiplication, respectively.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_MULHSU(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate MULHSU…" << std::endl;

    /*
     * For signed × unsigned multiplication, we cannot directly use MUL or IMUL (unsigned² and signed², respectively).
     * We do not touch the signed operand (here rs1), and imagine the unsigned operand (rs2) in signed form.
     * This would mean adding back the sign-bit with twice the sign-weight, if it is set:
     * A XLEN = 64-bit wide number would have the "signed" conversion of
     *      signed = unsigned + 2 * (2^63) * sign-bit
     * We essentially "add back" or "compensate" the sign-bit.
     * This means, for multiplication of signed rs1 by unsigned rs2, we calculate
     *      imul(rs1, rs2) + 2^(64) * rs2-sign-bit * rs1.
     * We treat both registers as signed in the imul, and then add back the quantity we have "lost"
     * by treating rs2 as unsigned.
     */
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->imul(r_info.map[instr.reg_src_2]);

        //add signed rs1 to the upper half of the result, if the "sign"-bit in rs2 is set
        a->sar(r_info.map[instr.reg_src_2], 63);
        a->and_(r_info.map[instr.reg_src_1], r_info.map[instr.reg_src_2]);
        a->add(x86::rdx, r_info.map[instr.reg_src_1]);

        //we want the upper XLEN bits here
        a->mov(r_info.map[instr.reg_dest], x86::rdx);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->imul(x86::ptr(r_info.base, 8 * instr.reg_src_2));

        //add signed rs1 to the upper half of the result, if the "sign"-bit in rs2 is set
        a->sar(x86::ptr(r_info.base, 8 * instr.reg_src_2), 63);
        a->mov(x86::rcx, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->and_(x86::rcx, x86::ptr(r_info.base, 8 * instr.reg_src_2));
        a->add(x86::rdx, x86::rcx);

        //we want the upper XLEN bits here
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rdx);
    }
}

/**
* Translate the MULHU instruction.
* MULH, MULHU, and MULHSU perform the same multiplication but return
* the upper XLEN bits of the full 2×XLEN-bit product, for signed×signed, unsigned×unsigned,
* and signed rs1×unsigned rs2 multiplication, respectively.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_MULHU(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate MULHU…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->mul(r_info.map[instr.reg_src_2]);
        //we want the upper XLEN bits here
        a->mov(r_info.map[instr.reg_dest], x86::rdx);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->mul(x86::ptr(r_info.base, 8 * instr.reg_src_2));
        //we want the upper XLEN bits here
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rdx);
    }
}

/**
* Translate the DIV instruction.
* DIV and DIVU perform an XLEN bits by XLEN bits signed and unsigned integer division of rs1 by
* rs2, rounding towards zero.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_DIV(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate DIV…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(r_info.map[instr.reg_src_2]);
        a->mov(r_info.map[instr.reg_dest], x86::rax);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::ptr(r_info.base, 8 * instr.reg_src_2));
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the DIVU instruction.
* DIV and DIVU perform an XLEN bits by XLEN bits signed and unsigned integer division of rs1 by
* rs2, rounding towards zero.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_DIVU(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate DIVU…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->xor_(x86::rdx, x86::rdx);
        a->div(r_info.map[instr.reg_src_2]);
        a->mov(r_info.map[instr.reg_dest], x86::rax);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::ptr(r_info.base, 8 * instr.reg_src_2));
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the REM instruction.
* DIV and DIVU perform an XLEN bits by XLEN bits signed and unsigned integer division of rs1 by
* rs2, rounding towards zero. REM and REMU provide the remainder of the corresponding division
* operation. For REM, the sign of the result equals the sign of the dividend.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_REM(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate REM…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(r_info.map[instr.reg_src_2]);
        a->mov(r_info.map[instr.reg_dest], x86::rdx);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::ptr(r_info.base, 8 * instr.reg_src_2));
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rdx);
    }
}

/**
* Translate the REMU instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_REMU(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate REMU…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->xor_(x86::rdx, x86::rdx);
        a->div(r_info.map[instr.reg_src_2]);
        a->mov(r_info.map[instr.reg_dest], x86::rdx);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::ptr(r_info.base, 8 * instr.reg_src_2));
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rdx);
    }
}

/**
* Translate the MULW instruction.
* MULW is an RV64 instruction that multiplies the lower 32 bits of the source registers, placing the
* sign-extension of the lower 32 bits of the result into the destination register.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_MULW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate MULW…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->imul(x86::ecx);
        a->movsx(r_info.map[instr.reg_dest], x86::eax);
    } else {
        a->mov(x86::eax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->imul(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->movsx(x86::rcx, x86::eax);
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rcx);
    }
}

/**
* Translate the DIVW instruction.
* DIVW and DIVUW are RV64 instructions that divide the lower 32 bits of rs1 by the lower 32
* bits of rs2, treating them as signed and unsigned integers respectively, placing the 32-bit quotient
* in rd, sign-extended to 64 bits.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_DIVW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate DIVW…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::ecx);
        a->movsx(r_info.map[instr.reg_dest], x86::eax);
    } else {
        a->mov(x86::eax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->movsx(x86::rcx, x86::eax);
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rcx);
    }
}

/**
* Translate the DIVUW instruction.
* DIVW and DIVUW are RV64 instructions that divide the lower 32 bits of rs1 by the lower 32
* bits of rs2, treating them as signed and unsigned integers respectively, placing the 32-bit quotient
* in rd, sign-extended to 64 bits.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_DIVUW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate DIVUW…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::ecx);
        a->movsx(r_info.map[instr.reg_dest], x86::eax);
    } else {
        a->mov(x86::eax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->movsx(x86::rcx, x86::eax);
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rcx);
    }
}

/**
* Translate the REMW instruction.
* DIVW and DIVUW are RV64 instructions that divide the lower 32 bits of rs1 by the lower 32
* bits of rs2, treating them as signed and unsigned integers respectively, placing the 32-bit quotient
* in rd, sign-extended to 64 bits. REMW and REMUW are RV64 instructions that provide the
* corresponding signed and unsigned remainder operations respectively.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_REMW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate REMW…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::ecx);
        a->movsx(r_info.map[instr.reg_dest], x86::edx);
    } else {
        a->mov(x86::eax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->movsx(x86::rcx, x86::edx);
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rcx);
    }
}

/**
* Translate the REMUW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_REMUW(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate REMUW…" << std::endl;

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::ecx);
        a->movsx(r_info.map[instr.reg_dest], x86::edx);
    } else {
        a->mov(x86::eax, x86::ptr(r_info.base, 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->movsx(x86::rcx, x86::edx);
        a->mov(x86::ptr(r_info.base, 8 * instr.reg_dest), x86::rcx);
    }
}
