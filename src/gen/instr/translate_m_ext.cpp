//
// Created by flo on 21.05.20.
//

#include "translate_m_ext.hpp"
#include "runtime/register.h"

//shortcut for memory operands
#define FE_MEM_ADDR(addr) FE_MEM(FE_IP, 0, 0, addr - (intptr_t) current)

/**
 * see p. 44 of the RISC-V-Spec
 * The semantics for division by zero and division overflow are summarized in Table 7.1. The quotient
 * of division by zero has all bits set, and the remainder of division by zero equals the dividend. Signed
 * division overflow occurs only when the most-negative integer is divided by −1. The quotient of a
 * signed division with overflow is equal to the dividend, and the remainder is zero. Unsigned division
 * overflow cannot occur.
 */

/**
* Translate the MUL instruction.
* MUL performs an XLEN-bit×XLEN-bit multiplication of rs1 by rs2 and places the lower XLEN bits
* in the destination register.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_MUL(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate MUL…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
        /* a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
         a->imul(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_IMUL64rm, FE_AX, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FE_AX);
        /*a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->imul(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_2));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rax);*/
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
void translate_MULH(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate MULH…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
        /* a->mov(x86::rax, r_info.map[instr.reg_src_1]);
         a->imul(r_info.map[instr.reg_src_2]);
         //we want the upper XLEN bits here
         a->mov(r_info.map[instr.reg_dest], x86::rdx);*/
    } else {

        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_IMUL64m, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FE_DX);
        /*
        a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->imul(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2));
        //we want the upper XLEN bits here
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rdx);*/
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
void translate_MULHSU(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate MULHSU…\n");

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
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
        /* a->mov(x86::rax, r_info.map[instr.reg_src_1]);
         a->imul(r_info.map[instr.reg_src_2]);

         //add signed rs1 to the upper half of the result, if the "sign"-bit in rs2 is set
         a->sar(r_info.map[instr.reg_src_2], 63);
         a->and_(r_info.map[instr.reg_src_1], r_info.map[instr.reg_src_2]);
         a->add(x86::rdx, r_info.map[instr.reg_src_1]);

         //we want the upper XLEN bits here
         a->mov(r_info.map[instr.reg_dest], x86::rdx);*/
    } else {

        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_IMUL64m, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));

        //add signed rs1 to the upper half of the result, if the "sign"-bit in rs2 is set
        err |= fe_enc64(&current, FE_SAR64mi, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2), 63);
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_AND64rm, FE_CX, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_ADD64rr, FE_DX, FE_CX);

        //we want the upper XLEN bits here
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FE_DX);

        /*
        a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->imul(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2));

        //add signed rs1 to the upper half of the result, if the "sign"-bit in rs2 is set
        a->sar(x86::ptr(r_info.base + 8 * instr.reg_src_2), 63);
        a->mov(x86::rcx, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->and_(x86::rcx, x86::ptr(r_info.base + 8 * instr.reg_src_2));
        a->add(x86::rdx, x86::rcx);

        //we want the upper XLEN bits here
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rdx);
         */
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
void translate_MULHU(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate MULHU…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
        /* a->mov(x86::rax, r_info.map[instr.reg_src_1]);
         a->mul(r_info.map[instr.reg_src_2]);
         //we want the upper XLEN bits here
         a->mov(r_info.map[instr.reg_dest], x86::rdx);*/
    } else {

        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MUL64m, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FE_DX);
/*
        a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->mul(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2));
        //we want the upper XLEN bits here
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rdx);*/
    }
}

/**
* Translate the DIV instruction.
* DIV and DIVU perform an XLEN bits by XLEN bits signed and unsigned integer division of rs1 by
* rs2, rounding towards zero.
* For division by zero semantics: see p. 44-45 in the spec.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_DIV(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate DIV…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        //handle division by zero separately
        const Label &div_zero = a->newLabel();
        a->cmp(r_info.map[instr.reg_src_2], 0);
        a->mov(r_info.map[instr.reg_dest], 0xFFFFFFFFFFFFFFFF);
        a->jz(div_zero);

        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(r_info.map[instr.reg_src_2]);
        a->mov(r_info.map[instr.reg_dest], x86::rax);

        a->bind(div_zero);
    } else {
        const Label &div_zero = a->newLabel();
        const Label &not_div_zero = a->newLabel();
        a->cmp(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2), 0);
        a->jnz(not_div_zero);
        a->mov(x86::qword_ptr(r_info.base + 8 * instr.reg_dest), 0xFFFFFFFFFFFFFFFF);
        a->jz(div_zero);
        a->bind(not_div_zero);

        a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rax);

        a->bind(div_zero);
    }
}

/**
* Translate the DIVU instruction.
* DIV and DIVU perform an XLEN bits by XLEN bits signed and unsigned integer division of rs1 by
* rs2, rounding towards zero.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_DIVU(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate DIVU…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        //handle division by zero separately
        const Label &div_zero = a->newLabel();
        a->cmp(r_info.map[instr.reg_src_2], 0);
        a->mov(r_info.map[instr.reg_dest], 0xFFFFFFFFFFFFFFFF);
        a->jz(div_zero);

        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->xor_(x86::rdx, x86::rdx);
        a->div(r_info.map[instr.reg_src_2]);
        a->mov(r_info.map[instr.reg_dest], x86::rax);

        a->bind(div_zero);
    } else {
        const Label &div_zero = a->newLabel();
        const Label &not_div_zero = a->newLabel();
        a->cmp(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2), 0);
        a->jnz(not_div_zero);
        a->mov(x86::qword_ptr(r_info.base + 8 * instr.reg_dest), 0xFFFFFFFFFFFFFFFF);
        a->jz(div_zero);
        a->bind(not_div_zero);

        a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rax);

        a->bind(div_zero);
    }
}

/**
* Translate the REM instruction.
* DIV and DIVU perform an XLEN bits by XLEN bits signed and unsigned integer division of rs1 by
* rs2, rounding towards zero. REM and REMU provide the remainder of the corresponding division
* operation. For REM, the sign of the result equals the sign of the dividend.
* For division by zero semantics: see p. 44-45 in the spec.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_REM(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate REM…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        //handle division by zero separately
        const Label &div_zero = a->newLabel();
        a->cmp(r_info.map[instr.reg_src_2], 0);
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->jz(div_zero);

        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(r_info.map[instr.reg_src_2]);
        a->mov(r_info.map[instr.reg_dest], x86::rdx);

        a->bind(div_zero);
    } else {
        const Label &div_zero = a->newLabel();
        const Label &not_div_zero = a->newLabel();
        a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->cmp(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2), 0);
        a->jnz(not_div_zero);
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rax);
        a->jz(div_zero);
        a->bind(not_div_zero);
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rdx);

        a->bind(div_zero);
    }
}

/**
* Translate the REMU instruction.
* DIV and DIVU perform an XLEN bits by XLEN bits signed and unsigned integer division of rs1 by
* rs2, rounding towards zero. REM and REMU provide the remainder of the corresponding division
* operation. For REM, the sign of the result equals the sign of the dividend.
* For division by zero semantics: see p. 44-45 in the spec.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_REMU(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate REMU…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        //handle division by zero separately
        const Label &div_zero = a->newLabel();
        a->cmp(r_info.map[instr.reg_src_2], 0);
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->jz(div_zero);

        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->xor_(x86::rdx, x86::rdx);
        a->div(r_info.map[instr.reg_src_2]);
        a->mov(r_info.map[instr.reg_dest], x86::rdx);

        a->bind(div_zero);
    } else {
        const Label &div_zero = a->newLabel();
        const Label &not_div_zero = a->newLabel();
        a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->cmp(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2), 0);
        a->jnz(not_div_zero);

        //the mov here does not affect the zero-flag, but we need to mov after comparing in case rs2 == rd
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rax);
        a->jz(div_zero);

        a->bind(not_div_zero);
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::qword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rdx);

        a->bind(div_zero);
    }
}

/**
* Translate the MULW instruction.
* MULW is an RV64 instruction that multiplies the lower 32 bits of the source registers, placing the
* sign-extension of the lower 32 bits of the result into the destination register.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_MULW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate MULW…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
        /* a->mov(x86::rax, r_info.map[instr.reg_src_1]);
         a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
         a->imul(x86::ecx);
         a->movsxd(r_info.map[instr.reg_dest], x86::eax);*/
    } else {

        err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_IMUL32m, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOVSXr64r32, FE_CX, FE_AX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FE_CX);
        /*
        a->mov(x86::eax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->imul(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->movsxd(x86::rcx, x86::eax);
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rcx);
        */
    }
}

/**
* Translate the DIVW instruction.
* DIVW and DIVUW are RV64 instructions that divide the lower 32 bits of rs1 by the lower 32
* bits of rs2, treating them as signed and unsigned integers respectively, placing the 32-bit quotient
* in rd, sign-extended to 64 bits.
* For division by zero semantics: see p. 44-45 in the spec.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_DIVW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate DIVW…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        //handle division by zero separately
        const Label &div_zero = a->newLabel();
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->cmp(x86::ecx, 0);
        a->mov(r_info.map[instr.reg_dest], 0xFFFFFFFFFFFFFFFF);
        a->jz(div_zero);

        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::ecx);
        a->movsxd(r_info.map[instr.reg_dest], x86::eax);

        a->bind(div_zero);
    } else {
        const Label &div_zero = a->newLabel();
        const Label &not_div_zero = a->newLabel();
        a->cmp(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2), 0);
        a->jnz(not_div_zero);
        a->mov(x86::dword_ptr(r_info.base + 8 * instr.reg_dest), 0xFFFFFFFFFFFFFFFF);
        a->jz(div_zero);

        a->bind(not_div_zero);
        a->mov(x86::eax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->movsxd(x86::rcx, x86::eax);
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rcx);

        a->bind(div_zero);
    }
}

/**
* Translate the DIVUW instruction.
* DIVW and DIVUW are RV64 instructions that divide the lower 32 bits of rs1 by the lower 32
* bits of rs2, treating them as signed and unsigned integers respectively, placing the 32-bit quotient
* in rd, sign-extended to 64 bits.
* For division by zero semantics: see p. 44-45 in the spec.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_DIVUW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate DIVUW…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        //handle division by zero separately
        const Label &div_zero = a->newLabel();
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->cmp(x86::ecx, 0);
        a->mov(r_info.map[instr.reg_dest], 0xFFFFFFFFFFFFFFFF);
        a->jz(div_zero);

        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::ecx);
        a->movsxd(r_info.map[instr.reg_dest], x86::eax);

        a->bind(div_zero);
    } else {
        const Label &div_zero = a->newLabel();
        const Label &not_div_zero = a->newLabel();
        a->cmp(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2), 0);
        a->jnz(not_div_zero);
        a->mov(x86::qword_ptr(r_info.base + 8 * instr.reg_dest), 0xFFFFFFFFFFFFFFFF);
        a->jz(div_zero);

        a->bind(not_div_zero);
        a->mov(x86::eax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->movsxd(x86::rcx, x86::eax);
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rcx);

        a->bind(div_zero);
    }
}

/**
* Translate the REMW instruction.
* DIVW and DIVUW are RV64 instructions that divide the lower 32 bits of rs1 by the lower 32
* bits of rs2, treating them as signed and unsigned integers respectively, placing the 32-bit quotient
* in rd, sign-extended to 64 bits. REMW and REMUW are RV64 instructions that provide the
* corresponding signed and unsigned remainder operations respectively.
* For division by zero semantics: see p. 44-45 in the spec.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_REMW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate REMW…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        //handle division by zero separately
        const Label &div_zero = a->newLabel();
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->cmp(x86::ecx, 0);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->jz(div_zero);

        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::ecx);
        a->movsxd(r_info.map[instr.reg_dest], x86::edx);

        a->bind(div_zero);
    } else {
        const Label &div_zero = a->newLabel();
        const Label &not_div_zero = a->newLabel();
        a->cmp(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2), 0);
        a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->jnz(not_div_zero);
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rax);
        a->jz(div_zero);

        a->bind(not_div_zero);
        a->xor_(x86::rdx, x86::rdx);
        a->idiv(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->movsxd(x86::rcx, x86::edx);
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rcx);

        a->bind(div_zero);
    }
}

/**
* Translate the REMUW instruction.
* DIVW and DIVUW are RV64 instructions that divide the lower 32 bits of rs1 by the lower 32
* bits of rs2, treating them as signed and unsigned integers respectively, placing the 32-bit quotient
* in rd, sign-extended to 64 bits. REMW and REMUW are RV64 instructions that provide the
* corresponding signed and unsigned remainder operations respectively.
* For division by zero semantics: see p. 44-45 in the spec.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_REMUW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate REMUW…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        //handle division by zero separately
        const Label &div_zero = a->newLabel();
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->cmp(x86::ecx, 0);
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->jz(div_zero);

        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::ecx);
        a->movsxd(r_info.map[instr.reg_dest], x86::edx);

        a->bind(div_zero);
    } else {
        const Label &div_zero = a->newLabel();
        const Label &not_div_zero = a->newLabel();

        a->cmp(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2), 0);
        a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->jnz(not_div_zero);
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rax);
        a->jz(div_zero);

        a->bind(not_div_zero);
        a->xor_(x86::rdx, x86::rdx);
        a->div(x86::dword_ptr(r_info.base + 8 * instr.reg_src_2));
        a->movsxd(x86::rcx, x86::edx);
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), x86::rcx);

        a->bind(div_zero);
    }
}
