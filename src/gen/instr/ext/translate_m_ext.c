//
// Created by flo on 21.05.20.
//

#include "translate_m_ext.h"
#include <util/util.h>

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
void translate_MUL(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate MUL...\n");

    ///rs1 and rd can use same temporary since the original value of rd is not needed.
    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regSrc2 = getRs2(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    //TODO Optimization: Could get rid of one of the memory loads potentially by using a memory operand for the IMUL
    doArithmCommutative(regSrc1, regSrc2, regDest, FE_IMUL64rr);
}

/**
* Translate the MULH instruction.
* MULH, MULHU, and MULHSU perform the same multiplication but return
* the upper XLEN bits of the full 2×XLEN-bit product, for signed×signed, unsigned×unsigned,
* and signed rs1×unsigned rs2 multiplication, respectively.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_MULH(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate MULH...\n");

    ///rs2 and rd can use same temporary since the original value of rd is not needed. (Uses RAX and RDX specifically
    /// because of the IMUL)
    //TODO Optimization: Could also place rs2 in RAX if rs1 is mapped and rs2 is not since order does not matter
    // (overwritten by next suggestion)
    getRs1Into(instr, r_info, FE_AX);
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);


    //TODO Optimization: Could get rid of one of the memory loads potentially by using a memory operand for the IMUL
    err |= fe_enc64(&current, FE_IMUL64r, regSrc2);

    //we want the upper XLEN bits (in RDX) here
    if (regDest != FE_DX) {
        ///rd is mapped so move the result in RDX there.
        err |= fe_enc64(&current, FE_MOV64rr, regDest, FE_DX);
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
void translate_MULHSU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate MULHSU...\n");

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


//    ///Handle special zero reg cases
//    // TODO Wanted/Necessary? Would work for all multiplications, introduced here since this is a "long" instruction).
//    //  Also possible for the dividend and result in division.
//    if (instr->reg_src_1 == x0 || instr->reg_src_2 == x0) {
//        err |= fe_enc64(&current, FE_XOR32rr, regDest, regDest);
//        return;
//    } else if (instr->reg_dest == x0) {
//        err |= fe_enc64(&current, FE_NOP);
//        return;
//    }
    //TODO Optimization: Could also place rs2 in RAX if rs1 is mapped and rs2 is not since order does not matter
    // (overwritten by next suggestion)
    getRs1Into(instr, r_info, FE_AX);
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);
    ///Uses RAX and RDX specifically because of the IMULs input/output.
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);

    //TODO Optimization: Could get rid of one of the memory loads potentially by using a memory operand for the IMUL
    // But we need rs2 again
    err |= fe_enc64(&current, FE_IMUL64r, regSrc2);
    err |= fe_enc64(&current, FE_TEST64rr, regSrc2, regSrc2);

    //insert same forward jump here later
    uint8_t *jmpNoSignBitBuf = current;
    err |= fe_enc64(&current, FE_JNS, (intptr_t) current); //dummy jmp

    ///Special cases for "Sign"-bit in rs2 is set
    ///add signed rs1 to the upper half of the result, if the "sign"-bit in rs2 is set
    if (!r_info->mapped[instr->reg_src_1]) {
        ///temporary rs1 was overwritten, so need to load it again
        err |= fe_enc64(&current, FE_ADD64rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    } else {
        err |= fe_enc64(&current, FE_ADD64rr, FE_DX, r_info->map[instr->reg_src_1]);
    }

    uint8_t *no_sign_bit = current;
    err |= fe_enc64(&jmpNoSignBitBuf, FE_JNS, (intptr_t) no_sign_bit);

    //we want the upper XLEN (in RDX) bits here
    if (regDest != FE_DX) {
        ///rd is mapped so move the result in RDX there.
        err |= fe_enc64(&current, FE_MOV64rr, regDest, FE_DX);
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
void translate_MULHU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate MULHU...\n");

    //TODO Optimization: Could also place rs2 in RAX if rs1 is mapped and rs2 is not since order does not matter
    // (overwritten by next suggestion)
    ///rs2 and rd can use same temporary since the original value of rd is not needed. (Uses RAX and RDX specifically
    /// because of the MULs input/output)
    getRs1Into(instr, r_info, FE_AX);
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);

    //TODO Optimization: Could get rid of one of the memory loads potentially by using a memory operand for the MUL
    err |= fe_enc64(&current, FE_MUL64r, regSrc2);

    //we want the upper XLEN (in RDX) bits here
    if (regDest != FE_DX) {
        ///rd is mapped so move the result in RDX there.
        err |= fe_enc64(&current, FE_MOV64rr, regDest, FE_DX);
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
void translate_DIV(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate DIV...\n");

    ///Uses RCX specifically since IDIV uses RDX:RAX as implicit input
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);

    ///rs1 and rd can use same temporary since the original value of rd is not needed. (Uses RAX specifically because
    /// of the IDIV)
    getRs1Into(instr, r_info, FE_AX);
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_TEST64rr, regSrc2, regSrc2);

    ///Special case for div by zero
    //insert same forward jump here later
    uint8_t *jmpDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current); //dummy jmp

    //do actual divide
    err |= fe_enc64(&current, FE_C_SEP64);
    // Loading rs2 into register always, if not already mapped, should be the better choice, since it is needed more
    // than once.
    err |= fe_enc64(&current, FE_IDIV64r, regSrc2);

    //insert same forward jump here later
    uint8_t *jmpNotDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jmp

    ///Special case for div by zero
    //write forward jump target for divZero jmp
    uint8_t *divZero = current;
    err |= fe_enc64(&jmpDivZeroBuf, FE_JZ, (intptr_t) divZero);

    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, -1); //-1 is all bits set

    //write forward jump target for notDivZero jmp
    uint8_t *notDivZero = current;
    err |= fe_enc64(&jmpNotDivZeroBuf, FE_JMP, (intptr_t) notDivZero);

    if (regDest != FE_AX) {
        ///rd is mapped so move the result in RAX there.
        err |= fe_enc64(&current, FE_MOV64rr, regDest, FE_AX);
    }
}

/**
* Translate the DIVU instruction.
* DIV and DIVU perform an XLEN bits by XLEN bits signed and unsigned integer division of rs1 by
* rs2, rounding towards zero.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_DIVU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate DIVU...\n");

    ///Uses RCX specifically since IDIV uses RDX:RAX as implicit input
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);

    ///rs1 and rd can use same temporary since the original value of rd is not needed. (Uses RAX specifically because
    /// of the IDIV)
    getRs1Into(instr, r_info, FE_AX);
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);
    err |= fe_enc64(&current, FE_TEST64rr, regSrc2, regSrc2);

    ///Special case for div by zero
    //insert same forward jump here later
    uint8_t *jmpDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current); //dummy jmp

    //do actual divide
    err |= fe_enc64(&current, FE_XOR32rr, FE_DX, FE_DX);
    // Loading rs2 into register always, if not already mapped, should be the better choice, since it is needed more
    // than once.
    err |= fe_enc64(&current, FE_DIV64r, regSrc2);

    //insert same forward jump here later
    uint8_t *jmpNotDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jmp

    ///Special case for div by zero
    //write forward jump target for divZero jmp
    uint8_t *divZero = current;
    err |= fe_enc64(&jmpDivZeroBuf, FE_JZ, (intptr_t) divZero);

    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, -1); //-1 is all bits set

    //write forward jump target for notDivZero jmp
    uint8_t *notDivZero = current;
    err |= fe_enc64(&jmpNotDivZeroBuf, FE_JMP, (intptr_t) notDivZero);

    if (regDest != FE_AX) {
        ///rd is mapped so move the result in RAX there.
        err |= fe_enc64(&current, FE_MOV64rr, regDest, FE_AX);
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
void translate_REM(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate REM...\n");

    ///Uses RCX specifically since IDIV uses RDX:RAX as implicit input
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);
    ///Uses RAX specifically because of IDIVs input
    FeReg regSrc1 = getRs1Into(instr, r_info, FE_AX);

    ///Uses RDX specifically because of IDIVs output
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_TEST64rr, regSrc2, regSrc2);

    ///Special case for div by zero
    //insert same forward jump here later
    uint8_t *jmpDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current); //dummy jmp

    //do actual divide
    err |= fe_enc64(&current, FE_C_SEP64);
    // Loading rs2 into register always, if not already mapped, should be the better choice, since it is needed more
    // than once.
    err |= fe_enc64(&current, FE_IDIV64r, regSrc2);

    //insert same forward jump here later
    uint8_t *jmpNotDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jmp

    ///Special case for div by zero
    //write forward jump target for divZero jmp
    uint8_t *divZero = current;
    err |= fe_enc64(&jmpDivZeroBuf, FE_JZ, (intptr_t) divZero);

    err |= fe_enc64(&current, FE_MOV64rr, FE_DX, regSrc1);

    //write forward jump target for notDivZero jmp
    uint8_t *notDivZero = current;
    err |= fe_enc64(&jmpNotDivZeroBuf, FE_JMP, (intptr_t) notDivZero);

    if (regDest != FE_DX) {
        ///rd is mapped so move the result in RDX there.
        err |= fe_enc64(&current, FE_MOV64rr, regDest, FE_DX);
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
void translate_REMU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate REMU...\n");

    ///Uses RCX specifically since DIV uses RDX:RAX as implicit input
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);
    ///Uses RAX specifically because of DIVs input
    FeReg regSrc1 = getRs1Into(instr, r_info, FE_AX);

    ///Uses RDX specifically because of DIVs output
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_TEST64rr, regSrc2, regSrc2);

    ///Special case for div by zero
    //insert same forward jump here later
    uint8_t *jmpDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current); //dummy jmp

    //do actual divide
    err |= fe_enc64(&current, FE_XOR32rr, FE_DX, FE_DX);
    // Loading rs2 into register always, if not already mapped, should be the better choice, since it is needed more
    // than once.
    err |= fe_enc64(&current, FE_DIV64r, regSrc2);

    //insert same forward jump here later
    uint8_t *jmpNotDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jmp

    ///Special case for div by zero
    //write forward jump target for divZero jmp
    uint8_t *divZero = current;
    err |= fe_enc64(&jmpDivZeroBuf, FE_JZ, (intptr_t) divZero);

    err |= fe_enc64(&current, FE_MOV64rr, FE_DX, regSrc1);

    //write forward jump target for notDivZero jmp
    uint8_t *notDivZero = current;
    err |= fe_enc64(&jmpNotDivZeroBuf, FE_JMP, (intptr_t) notDivZero);

    if (regDest != FE_DX) {
        ///rd is mapped so move the result in RDX there.
        err |= fe_enc64(&current, FE_MOV64rr, regDest, FE_DX);
    }
}

/**
* Translate the MULW instruction.
* MULW is an RV64 instruction that multiplies the lower 32 bits of the source registers, placing the
* sign-extension of the lower 32 bits of the result into the destination register.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_MULW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate MULW...\n");

    //TODO Minor optimization: Don't load the full 64bit registers for no reason (upper half is just ignored anyways,
    // so save the instruction prefixes)
    ///rs1 and rd can use same temporary since the original value of rd is not needed.
    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regSrc2 = getRs2(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    //TODO Optimization: Could get rid of one of the memory loads potentially by using a memory operand for the IMUL
    doArithmCommutative(regSrc1, regSrc2, regDest, FE_IMUL32rr);

    err |= fe_enc64(&current, FE_MOVSXr64r32, regDest, regDest);

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
void translate_DIVW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate DIVW...\n");

    //TODO Minor optimization: Don't load the full 64bit registers for no reason (upper half is just ignored anyways,
    // so save the instruction prefixes)
    ///Uses RCX specifically since IDIV uses EDX:EAX as implicit input
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);

    ///rs1 and rd can use same temporary since the original value of rd is not needed. (Uses RAX specifically because
    /// of the IDIV)
    getRs1Into(instr, r_info, FE_AX);
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_TEST32rr, regSrc2, regSrc2);

    ///Special case for div by zero
    //insert same forward jump here later
    uint8_t *jmpDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current); //dummy jmp

    //do actual divide
    err |= fe_enc64(&current, FE_C_SEP32);
    // Loading rs2 into register always, if not already mapped, should be the better choice, since it is needed more
    // than once.
    err |= fe_enc64(&current, FE_IDIV32r, regSrc2);

    //If rd is not mapped this will only be an inplace sign extend otherwise will also do the mov
    err |= fe_enc64(&current, FE_MOVSXr64r32, regDest, FE_AX);

    //insert same forward jump here later
    uint8_t *jmpNotDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jmp

    ///Special case for div by zero
    //write forward jump target for divZero jmp
    uint8_t *divZero = current;
    err |= fe_enc64(&jmpDivZeroBuf, FE_JZ, (intptr_t) divZero);

    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, -1); //-1 is all bits set

    //write forward jump target for notDivZero jmp
    uint8_t *notDivZero = current;
    err |= fe_enc64(&jmpNotDivZeroBuf, FE_JMP, (intptr_t) notDivZero);

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
void translate_DIVUW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate DIVUW...\n");

    //TODO Minor optimization: Don't load the full 64bit registers for no reason (upper half is just ignored anyways,
    // so save the instruction prefixes)
    ///Uses RCX specifically since DIV uses EDX:EAX as implicit input
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);

    ///rs1 and rd can use same temporary since the original value of rd is not needed. (Uses RAX specifically because
    /// of the DIV)
    getRs1Into(instr, r_info, FE_AX);
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_TEST32rr, regSrc2, regSrc2);

    ///Special case for div by zero
    //insert same forward jump here later
    uint8_t *jmpDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current); //dummy jmp

    //do actual divide
    err |= fe_enc64(&current, FE_XOR32rr, FE_DX, FE_DX);
    // Loading rs2 into register always, if not already mapped, should be the better choice, since it is needed more
    // than once.
    err |= fe_enc64(&current, FE_DIV32r, regSrc2);

    //If rd is not mapped this will only be an inplace sign extend otherwise will also do the mov
    err |= fe_enc64(&current, FE_MOVSXr64r32, regDest, FE_AX);

    //insert same forward jump here later
    uint8_t *jmpNotDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jmp

    ///Special case for div by zero
    //write forward jump target for divZero jmp
    uint8_t *divZero = current;
    err |= fe_enc64(&jmpDivZeroBuf, FE_JZ, (intptr_t) divZero);

    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, -1); //-1 is all bits set

    //write forward jump target for notDivZero jmp
    uint8_t *notDivZero = current;
    err |= fe_enc64(&jmpNotDivZeroBuf, FE_JMP, (intptr_t) notDivZero);

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
void translate_REMW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate REMW...\n");

    //TODO Minor optimization: Don't load the full 64bit registers for no reason (upper half is just ignored anyways,
    // so save the instruction prefixes)
    ///Uses RCX specifically since IDIV uses EDX:EAX as implicit input
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);

    ///Uses RAX specifically because of the IDIVs input
    FeReg regSrc1 = getRs1Into(instr, r_info, FE_AX);

    ///Uses RDX specifically because of the IDIVs output
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_TEST32rr, regSrc2, regSrc2);

    ///Special case for div by zero
    //insert same forward jump here later
    uint8_t *jmpDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current); //dummy jmp

    //do actual divide
    err |= fe_enc64(&current, FE_C_SEP32);
    // Loading rs2 into register always, if not already mapped, should be the better choice, since it is needed more
    // than once.
    err |= fe_enc64(&current, FE_IDIV32r, regSrc2);

    //If rd is not mapped this will only be an inplace sign extend otherwise will also do the mov
    err |= fe_enc64(&current, FE_MOVSXr64r32, regDest, FE_DX);

    //insert same forward jump here later
    uint8_t *jmpNotDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jmp

    ///Special case for div by zero
    //write forward jump target for divZero jmp
    uint8_t *divZero = current;
    err |= fe_enc64(&jmpDivZeroBuf, FE_JZ, (intptr_t) divZero);

    err |= fe_enc64(&current, FE_MOVSXr64r32, regDest, regSrc1);

    //write forward jump target for notDivZero jmp
    uint8_t *notDivZero = current;
    err |= fe_enc64(&jmpNotDivZeroBuf, FE_JMP, (intptr_t) notDivZero);

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
void translate_REMUW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate REMUW...\n");

    //TODO Minor optimization: Don't load the full 64bit registers for no reason (upper half is just ignored anyways,
    // so save the instruction prefixes)
    ///Uses RCX specifically since DIV uses EDX:EAX as implicit input
    FeReg regSrc2 = getRs2Into(instr, r_info, FE_CX);
    ///Uses RAX specifically because of the DIVs input
    FeReg regSrc1 = getRs1Into(instr, r_info, FE_AX);

    ///Uses RDX specifically because of the DIVs output
    invalidateReplacement(r_info, FE_DX, true);
    invalidateReplacement(r_info, FE_AX, true);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_TEST32rr, regSrc2, regSrc2);

    ///Special case for div by zero
    //insert same forward jump here later
    uint8_t *jmpDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current); //dummy jmp

    //do actual divide
    err |= fe_enc64(&current, FE_XOR32rr, FE_DX, FE_DX);
    // Loading rs2 into register always, if not already mapped, should be the better choice, since it is needed more
    // than once.
    err |= fe_enc64(&current, FE_DIV32r, regSrc2);

    //If rd is not mapped this will only be an inplace sign extend otherwise will also do the mov
    err |= fe_enc64(&current, FE_MOVSXr64r32, regDest, FE_DX);

    //insert same forward jump here later
    uint8_t *jmpNotDivZeroBuf = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jmp

    ///Special case for div by zero
    //write forward jump target for divZero jmp
    uint8_t *divZero = current;
    err |= fe_enc64(&jmpDivZeroBuf, FE_JZ, (intptr_t) divZero);

    err |= fe_enc64(&current, FE_MOVSXr64r32, regDest, regSrc1);

    //write forward jump target for notDivZero jmp
    uint8_t *notDivZero = current;
    err |= fe_enc64(&jmpNotDivZeroBuf, FE_JMP, (intptr_t) notDivZero);

}
