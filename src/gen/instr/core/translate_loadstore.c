//
// Created by flo on 21.05.20.
//

#include "translate_loadstore.h"
#include <util/util.h>

/**
 * Translate the LB instruction.
 * The LB instruction loads a 8-bit value from memory and sign extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LB(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate LB...\n");

    ///Can use same reg since temporary rs1 is not needed afterwards.
    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_MOVSXr64m8, regDest, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm));
}

/**
 * Translate the LH instruction.
 * The LH instruction loads a 16-bit value from memory and sign extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LH(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate LH...\n");

    ///Can use same reg since temporary rs1 is not needed afterwards.
    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_MOVSXr64m16, regDest, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm));
}

/**
 * Translate the LW instruction.
 * The LW instruction loads a 32-bit value from memory and sign extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate LW...\n");

    ///Can use same reg since temporary rs1 is not needed afterwards.
    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_MOVSXr64m32, regDest, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm));
}

/**
 * Translate the LBU instruction.
 * The LBU instruction loads a 8-bit value from memory and zero extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LBU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate LBU...\n");

    ///Can use same reg since temporary rs1 is not needed afterwards.
    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_MOVZXr32m8, regDest, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm));
}

/**
 * Translate the LHU instruction.
 * The LHU instruction loads a 16-bit value from memory and zero extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LHU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate LHU...\n");

    ///Can use same reg since temporary rs1 is not needed afterwards.
    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_MOVZXr32m16, regDest, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm));
}

/**
 * Translate the SB instruction.
 * The SB instruction stores the low 8 bits from rs2 into memory. The effective address is obtained by adding register
 * rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SB(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SB...\n");

    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regSrc2 = getRs2(instr, r_info);

    err |= fe_enc64(&current, FE_MOV8mr, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm), regSrc2);
}

/**
 * Translate the SH instruction.
 * The SH instruction stores the low 16 bits from rs2 into memory. The effective address is obtained by adding register
 * rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SH(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SH...\n");

    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regSrc2 = getRs2(instr, r_info);

    err |= fe_enc64(&current, FE_MOV16mr, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm), regSrc2);
}

/**
 * Translate the SW instruction.
 * The SH instruction stores the low 32 bits from rs2 into memory. The effective address is obtained by adding register
 * rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SW...\n");

    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regSrc2 = getRs2(instr, r_info);

    err |= fe_enc64(&current, FE_MOV32mr, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm), regSrc2);
}

/**
 * Translate the LWU instruction.
 * The LWU instruction loads a 32-bit value from memory and zero extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LWU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate LWU...\n");

    ///Can use same reg since temporary rs1 is not needed afterwards.
    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    ///All instructions with 32bit register targets on x86-64 automatically zero extend. Hence there is no movzx r64,
    /// r/m32. Instead you use mov r32, r/m32


    err |= fe_enc64(&current, FE_MOV32rm, regDest, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm));
}

/**
 * Translate the LD instruction.
 * The LD instruction loads a 64-bit value from memory into register rd. The effective address is obtained by adding
 * register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate LD...\n");

    ///Can use same reg since temporary rs1 is not needed afterwards.
    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    err |= fe_enc64(&current, FE_MOV64rm, regDest, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm));
}

/**
 * Translate the SD instruction.
 * The SD instruction stores the 64-bit value from rs2 into memory. The effective address is obtained by adding register
 * rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SD...\n");

    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regSrc2 = getRs2(instr, r_info);

    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm), regSrc2);
}
