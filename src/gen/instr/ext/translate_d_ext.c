//
// Created by Flo Schmidt on 02.09.20.
//

#include "translate_d_ext.h"
#include <fadec/fadec-enc.h>

#define D_SIGN_BIT_MASK (0x1 << 63) //left most bit

/**
 * Translate the FLD instruction.
 * Description: load a double-precision floating-point value from memory into floating-point register rd
 * preserve NaN payload
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FLD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FLD...\n");
    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);
    err |= fe_enc64(&current, FE_SSE_MOVSDrm,regDest, FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm));
    setFpReg(regDest, r_info, FIRST_FP_REG);
}

/**
 * Translate the FSD instruction.
 * Description: store a double-precision floating-point value from floating-point register rs2 into memory
 * preserve NaN payload
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSD...\n");
    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, FIRST_FP_REG);
    err |= fe_enc64(&current, FE_SSE_MOVSDmr,FE_MEM(regSrc1, 0, 0, instr->op_field.op.imm), regSrc2);
}

/**
 * Translate the FMADDD instruction.
 * Description: rs1 x rs2 + rs3
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMADDD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMADDD...\n");
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, SECOND_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, FIRST_FP_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    //TODO could be optimized by reducing moves
    //move src1 to SECOND_FP_REG if src1 was mapped
    if (regSrc1 != SECOND_FP_REG) {
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, SECOND_FP_REG, regSrc1); //TODO check encoding
    }

    //multiply rs2
    err |= fe_enc64(&current, FE_SSE_MULSDrr, SECOND_FP_REG, regSrc2);

    //move rs3 into dest
    FeReg regSrc3 = getFpReg(instr->op_field.f_op.reg_src_3, r_info, regDest);
    if (regSrc3 != regDest) {
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, regSrc3); //TODO check encoding
    }

    //add multiply result
    err |= fe_enc64(&current, FE_SSE_ADDSDrr, regDest, SECOND_FP_REG);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FMSUBD instruction.
 * Description:  rs1 x rs2 - rs3
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMSUBD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMSUBD...\n");
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, SECOND_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, FIRST_FP_REG);

    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, SECOND_FP_REG);

    //multiply rs1 and rs2
    //move src1 to SECOND_FP_REG if src1 was mapped
    if (regSrc1 != SECOND_FP_REG) {
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, SECOND_FP_REG, regSrc1); //TODO check encoding
    }

    err |= fe_enc64(&current, FE_SSE_MULSDrr, SECOND_FP_REG, regSrc2);

    FeReg regSrc3 = getFpReg(instr->op_field.f_op.reg_src_3, r_info, FIRST_FP_REG);

    //if src3 is already in dest save it
    if (regDest == regSrc3) {
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, FIRST_FP_REG, regSrc3); //TODO check encoding
        regSrc3 = FIRST_FP_REG;
    }


    //move multiply result in SECOND_FP_REG to regDest
    if (regSrc1 != FIRST_FP_REG) {
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, SECOND_FP_REG); //TODO check encoding
    }

    //subtract rs3
    err |= fe_enc64(&current, FE_SSE_SUBSDrr, regDest, regSrc3);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FNMSUBD instruction.
 * Description: -(rs1 x rs2) + rs3
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FNMSUBD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FNMSUBD...\n");
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, SECOND_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, FIRST_FP_REG);

    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    //move src1 to SECOND_FP_REG if src1 was mapped
    if (regSrc1 != SECOND_FP_REG) {
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, SECOND_FP_REG, regSrc1); //TODO check encoding
    }

    //multiply rs1 and rs2
    err |= fe_enc64(&current, FE_SSE_MULSDrr, SECOND_FP_REG, regSrc2);

    //FIRST_FP_REG is now free again
    FeReg regSrc3 = getFpReg(instr->op_field.f_op.reg_src_3, r_info, FIRST_FP_REG);

    //if src3 is not already in dest move it
    if (regDest == regSrc3) {
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, regSrc3); //TODO check encoding
        regSrc3 = FIRST_FP_REG;
    }

    //subtract multiply result
    err |= fe_enc64(&current, FE_SSE_SUBSDrr, regDest, SECOND_FP_REG);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FNMADDD instruction.
 * Description -(rs1 x rs2) - rs3
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FNMADDD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FNMADDD...\n");
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, SECOND_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, FIRST_FP_REG);

    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    //multiply rs1 and rs2
    //move src1 to SECOND_FP_REG if src1 was mapped
    if (regSrc1 != SECOND_FP_REG) {
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, SECOND_FP_REG, regSrc1); //TODO check encoding
    }

    err |= fe_enc64(&current, FE_SSE_MULSDrr, SECOND_FP_REG, regSrc2);

    //negate by subtracting
    err |= fe_enc64(&current, FE_SSE_XORPDrr, FIRST_FP_REG, FIRST_FP_REG);
    err |= fe_enc64(&current, FE_SSE_SUBSDrr, FIRST_FP_REG, SECOND_FP_REG);

    FeReg regSrc3 = getFpReg(instr->op_field.f_op.reg_src_3, r_info, SECOND_FP_REG);
    //if src3 is already in dest save it
    if (regDest == regSrc3) {
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, SECOND_FP_REG, regSrc3); //TODO check encoding
        regSrc3 = SECOND_FP_REG;
    }


    //move negated multiply result in FIRST_FP_REG to regDest
    if (regDest != FIRST_FP_REG) {
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, FIRST_FP_REG); //TODO check encoding
    }

    //subtract rs3
    err |= fe_enc64(&current, FE_SSE_SUBSDrr, regDest, regSrc3);
    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FADDD instruction.
 * Description: rs1 + rs2
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FADDD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FADDD...\n");
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    doFpArithmCommutative(regSrc1, regSrc2, regDest, FE_SSE_ADDSDrr);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FSUBD instruction.
 * Description: rs1 - rs2
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSUBD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSUBD...\n");
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    if (instr->op_field.op.reg_dest == instr->op_field.op.reg_src_2 &&
            instr->op_field.op.reg_dest == instr->op_field.op.reg_src_1) {

        err |= fe_enc64(&current, FE_SSE_XORPDrr, regDest,
                        regDest); // SUB same, same, same is equivalent to zeroing
    } else {
        //save rs2
        FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);
        if (regSrc2 == regDest) { //save src2 if necessary
            err |= fe_enc64(&current, FE_SSE_MOVSDrr, SECOND_FP_REG, regSrc2); //TODO check encoding
            regSrc2 = SECOND_FP_REG;
        }
        //load first operand into regDest
        FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, regDest);
        if (regSrc1 != regDest) { // move if src1 was mapped
            err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, regSrc1); //TODO check encoding
        }
        err |= fe_enc64(&current, FE_SSE_SUBSDrr, regDest, regSrc2);
    }

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FMULD instruction.
 * Description: rs1 x rs2
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMULD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMULD...\n");

    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    doFpArithmCommutative(regSrc1, regSrc2, regDest, FE_SSE_MULSDrr);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FDIVD instruction.
 * Description: rs1 / rs2
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FDIVD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FDIVD...\n");
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);
    if (regDest == regSrc2) {
        //happens only if both are mapped and the same
        //save rs2
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, SECOND_FP_REG, regSrc2); //TODO check encoding
        regSrc2 = SECOND_FP_REG;
    }

    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, regDest); //load src1 into destination
    if (regSrc1 != regDest) {
        //src1 was mapped => move into regDest
        err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest,
                        regSrc1); //TODO check encoding needs to be F3 0F 11 and not 10
    }
    err |= fe_enc64(&current, FE_SSE_DIVSDrr, regDest, regSrc2);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FSQRTD instruction.
 * Description: compute the square root of rs1
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSQRTD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSQRTD...\n");
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    err |= fe_enc64(&current, FE_SSE_SQRTSDrr, regDest, regSrc1);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FSGNJD instruction.
 * Description: takes all bits but sign from rs1, sign bit is that from rs2
 * naive approach by first moving into normal register, doing bit artihmetic and then putting it back
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSGNJD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSGNJD...\n");
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    if (instr->op_field.op.reg_src_1 == instr->op_field.op.reg_src_2) {
        //simple move
        if (regSrc1 != regDest) {
            //move to regDest
            err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, regSrc1);
        }
    } else {
        FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);

        err |= fe_enc64(&current, FE_SSE_MOVQrr, FIRST_REG, regSrc1);
        err |= fe_enc64(&current, FE_SSE_MOVQrr, FE_R8, regSrc2);

        //load bitmask
        err |= fe_enc64(&current, FE_MOV64ri, SECOND_REG, 0x1);
        err |= fe_enc64(&current, FE_ROR64ri, SECOND_REG, 0x1);

        //mask sign from second
        err |= fe_enc64(&current, FE_AND64rr, FE_R8, SECOND_REG);

        //remove sign from first
        err |= fe_enc64(&current, FE_BTR64ri, FIRST_REG, 63);

        //merge
        err |= fe_enc64(&current, FE_OR64rr, FIRST_REG, FE_R8);
        //move back to fp_reg
        err |= fe_enc64(&current, FE_SSE_MOVQrr, regDest, FIRST_REG);
    }

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FSGNJND instruction.
 * Description: takes all bits but sign from rs1, sign bit is the opposite of rs2's
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSGNJND(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSGNJND...\n");
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);


    err |= fe_enc64(&current, FE_SSE_MOVQrr, FIRST_REG, regSrc1);
    err |= fe_enc64(&current, FE_SSE_MOVQrr, FE_R8, regSrc2);

    //load bitmask
    err |= fe_enc64(&current, FE_MOV64ri, SECOND_REG, 0x1);
    err |= fe_enc64(&current, FE_ROR64ri, SECOND_REG, 0x1);

    //remove sign from first
    err |= fe_enc64(&current, FE_BTR64ri, FIRST_REG, 63);

    //flip second (negate sign)
    err |= fe_enc64(&current, FE_NOT64r, FE_R8);


    //mask sign from second
    err |= fe_enc64(&current, FE_AND64rr, FE_R8, SECOND_REG);
    //merge
    err |= fe_enc64(&current, FE_OR64rr, FIRST_REG, FE_R8);
    //move back to fp_reg
    err |= fe_enc64(&current, FE_SSE_MOVQrr, regDest, FIRST_REG);


    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FSGNJXD instruction.
 * Description: takes all bits but sign from rs1, sign bit is the xor of rs1's and rs2's
 * may be used for e.g. FABS, to compute the absolute of a value FSGNJXS ry, rx, rx
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FSGNJXD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FSGNJXD...\n");
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);


    err |= fe_enc64(&current, FE_SSE_MOVQrr, FIRST_REG, regSrc1);
    err |= fe_enc64(&current, FE_SSE_MOVQrr, FE_R8, regSrc2);

    //load bitmask
    err |= fe_enc64(&current, FE_MOV64ri, SECOND_REG, 0x1);
    err |= fe_enc64(&current, FE_ROR64ri, SECOND_REG, 0x1);

    //xor first and second sign
    err |= fe_enc64(&current, FE_XOR64rr, FE_R8, FIRST_REG);

    //remove sign from first
    err |= fe_enc64(&current, FE_BTR64ri, FIRST_REG, 63);

    //mask sign from second
    err |= fe_enc64(&current, FE_AND64rr, FE_R8, SECOND_REG);
    //merge
    err |= fe_enc64(&current, FE_OR64rr, FIRST_REG, FE_R8);
    //move back to fp_reg
    err |= fe_enc64(&current, FE_SSE_MOVQrr, regDest, FIRST_REG);


    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FMIND instruction.
 * Description: compute min of rs1, rs2 store in rd, if only one is NaN, the result is not NaN
 * if one of the inputs is NaN NV Exception (Invalid Operation is set)
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMIND(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMIND...\n");
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    if (regSrc1 == regSrc2) { // we don't need any comparison in this case, because they are the same
        if (regDest != regSrc1) {
            //move src1 in dest
            err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, regSrc1); //TODO check encoding
        }
    } else {
        if (regSrc2 == regDest) {//rs2 needs to be first operand, thus, check rs1
            //check if rs1 is nan
            err |= fe_enc64(&current, FE_SSE_UCOMISDrr, regSrc1, regSrc1);

            uint8_t *jmpBufNaN = current;
            err |= fe_enc64(&current, FE_JP, (intptr_t) current); //dummy

            err |= fe_enc64(&current, FE_SSE_MINSDrr, regDest, regSrc1); //if regSrc2 is NaN regSrc1 will be returned

            err |= fe_enc64(&jmpBufNaN, FE_JP, (intptr_t) current);
            //branch if rs1 is nan, nothing to do as src2 is already in dest
        } else {
            //check if rs2 is nan
            err |= fe_enc64(&current, FE_SSE_UCOMISDrr, regSrc2, regSrc2);

            uint8_t *jmpBufNaN = current;
            err |= fe_enc64(&current, FE_JP, (intptr_t) current); //dummy

            //move src1 in dest
            if (regSrc1 != regDest) {
                err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, regSrc1); //TODO check encoding
            }
            err |= fe_enc64(&current, FE_SSE_MINSDrr, regDest, regSrc2); //if regSrc1 is NaN regSrc2 will be returned

            //check if rs1 is already in dest, in this case we don't need the jump
            if (regSrc1 == regDest) {
                err |= fe_enc64(&jmpBufNaN, FE_JP, (intptr_t) current);
            } else {

                uint8_t *jmpBufEnd = current;
                err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy
                err |= fe_enc64(&jmpBufNaN, FE_JP, (intptr_t) current);

                err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, regSrc1); //TODO check encoding
                err |= fe_enc64(&jmpBufEnd, FE_JMP, (intptr_t) current);
            }
        }
    }


    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FMAXD instruction.
 * Description: compute max of rs1, rs2 store in rd, if only one is NaN, the result is not NaN
 * if one of the inputs is NaN NV Exception (Invalid Operation is set)
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMAXD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMAXD...\n");
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    if (regSrc1 == regSrc2) { // we don't need any comparison in this case, because they are the same
        if (regDest != regSrc1) {
            //move src1 in dest
            err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, regSrc1); //TODO check encoding
        }
    } else {
        if (regSrc2 == regDest) {//rs2 needs to be first operand, thus, check rs1
            //check if rs1 is nan
            err |= fe_enc64(&current, FE_SSE_UCOMISDrr, regSrc1, regSrc1);

            uint8_t *jmpBufNaN = current;
            err |= fe_enc64(&current, FE_JP, (intptr_t) current); //dummy

            err |= fe_enc64(&current, FE_SSE_MAXSDrr, regDest, regSrc1); //if regSrc2 is NaN regSrc1 will be returned

            err |= fe_enc64(&jmpBufNaN, FE_JP, (intptr_t) current);
            //branch if rs1 is nan, nothing to do as src2 is already in dest
        } else {
            //check if rs2 is nan
            err |= fe_enc64(&current, FE_SSE_UCOMISDrr, regSrc2, regSrc2);

            uint8_t *jmpBufNaN = current;
            err |= fe_enc64(&current, FE_JP, (intptr_t) current); //dummy

            //move src1 in dest
            if (regSrc1 != regDest) {
                err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, regSrc1); //TODO check encoding
            }
            err |= fe_enc64(&current, FE_SSE_MAXSDrr, regDest, regSrc2); //if regSrc1 is NaN regSrc2 will be returned

            //check if rs1 is already in dest, in this case we don't need the jump
            if (regSrc1 == regDest) {
                err |= fe_enc64(&jmpBufNaN, FE_JP, (intptr_t) current);
            } else {

                uint8_t *jmpBufEnd = current;
                err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy
                err |= fe_enc64(&jmpBufNaN, FE_JP, (intptr_t) current);

                err |= fe_enc64(&current, FE_SSE_MOVSDrr, regDest, regSrc1); //TODO check encoding
                err |= fe_enc64(&jmpBufEnd, FE_JMP, (intptr_t) current);
            }
        }
    }


    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FCVTSD instruction.
 * Description: convert single-precision float in rs1 to double-precision float in dest
 * @param instr the RISC-V instruction to translate
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTSD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTSD...\n");

    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    err |= fe_enc64(&current, FE_SSE_CVTSS2SDrr, regDest, regSrc1);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FCVTDS instruction.
 * Description: convert double-precision float in rs1 to single-precision float in dest
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTDS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTDS...\n");

    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    err |= fe_enc64(&current, FE_SSE_CVTSD2SSrr, regDest, regSrc1);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the FEQD instruction.
 * Description: check if rs1 = rs2, write 1 to the integer register rd if the condition holds, 0 otherwise
 * the result is zero if either operand is NaN and Invalid operation exception is set
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FEQD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FEQD...\n");

    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_XOR64rr, regDest, regDest);
    err |= fe_enc64(&current, FE_SSE_COMISDrr, regSrc1, regSrc2);
    err |= fe_enc64(&current, FE_MOV64ri, SECOND_REG, 0);
    err |= fe_enc64(&current, FE_SETNP8r, regDest);
    err |= fe_enc64(&current, FE_CMOVNZ64rr, regDest, SECOND_REG);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the FLTD instruction.
 * Description: check if rs1 < rs2, write 1 to the integer register rd if the condition holds, 0 otherwise
 * the result is zero if either operand is NaN and Invalid operation exception is set
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FLTD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FLTD...\n");

    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_XOR64rr, regDest, regDest);
    err |= fe_enc64(&current, FE_SSE_COMISDrr, regSrc2, regSrc1);
    err |= fe_enc64(&current, FE_SETA8r, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the FLED instruction.
 * Description: check if rs1 <= rs2, write 1 to the integer register rd if the condition holds, 0 otherwise
 * the result is zero if either operand is NaN and Invalid operation exception is set
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FLED(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FLED...\n");
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regSrc2 = getFpReg(instr->op_field.op.reg_src_2, r_info, SECOND_FP_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_XOR64rr, regDest, regDest);
    err |= fe_enc64(&current, FE_SSE_COMISDrr, regSrc2, regSrc1);
    err |= fe_enc64(&current, FE_SETA8r, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the FCLASSD instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCLASSD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCLASSD...\n");
    critical_not_yet_implemented("FCLASSD not yet implemented.\n");
}

/**
 * Translate the FCVTWD instruction.
 * Description: convert double fp number in flp register rs1 to signed 32-bit int into int register rd
 * if the value is not representable it is clipped an the invalid flag is set
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTWD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTWD...\n");

    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_SSE_CVTSD2SI32rr, regDest, regSrc1);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the FCVTWUD instruction.
 * Description: convert double fp number in flp register rs1 to unsigned 32-bit int into int register rd
 * if the value is not representable it is clipped an the invalid flag is set
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTWUD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTWUD...\n");

    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_SSE_CVTSD2SI64rr, regDest, regSrc1);
    err |= fe_enc64(&current, FE_MOV32rr, regDest, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the FCVTDW instruction.
 * Description: convert signed 32-bit int from rs1 into double  flp in fp rd
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTDW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTDW...\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    err |= fe_enc64(&current, FE_SSE_XORPDrr, regDest, regDest); //needs to be done because of cvtsi2ss design
    err |= fe_enc64(&current, FE_SSE_CVTSI2SD32rr, regDest, regSrc1);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FCVTDWU instruction.
 * Description: convert unsigned 32-bit int from rs1 into double  flp in fp rd
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTDWU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTDWU...\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);
    //zero upper bits by moving into FIRST_REG
    err |= fe_enc64(&current, FE_MOV32rr, FIRST_REG, regSrc1);

    err |= fe_enc64(&current, FE_SSE_XORPDrr, regDest, regDest); //needs to be done because of cvtsi2ss design
    err |= fe_enc64(&current, FE_SSE_CVTSI2SD64rr, regDest,
                    FIRST_REG); //use 64 bit convert to get a unsigned conversion

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FCVTLD instruction.
 * Description: convert a double fp number in fp register rs1 to a signed 64-bit int in rd
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTLD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTLD...\n");

    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_SSE_CVTSD2SI64rr, regDest, regSrc1);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the FCVTLUD instruction.
 * Description: convert a double fp number in fp register rs1 to a unsigned 64-bit int in rd
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTLUD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTLUD...\n");

    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    //constant saved here
    const unsigned long C0 = 1138753536;
    //load const into register
    err |= fe_enc64(&current, FE_MOV64ri, SECOND_REG, C0);
    err |= fe_enc64(&current, FE_SSE_MOVQrr, SECOND_FP_REG, SECOND_REG);

    err |= fe_enc64(&current, FE_SSE_COMISDrr, regSrc1, SECOND_FP_REG);
    uint8_t *jmpBufCOM = current;
    err |= fe_enc64(&current, FE_JNC, (intptr_t) current); //dummy

    err |= fe_enc64(&current, FE_SSE_CVTSD2SI64rr, regDest, regSrc1);

    uint8_t *jmpBufEnd = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy


    err |= fe_enc64(&jmpBufCOM, FE_JNC, (intptr_t) current);
    err |= fe_enc64(&current, FE_SSE_SUBSDrr, regSrc1, SECOND_FP_REG);
    err |= fe_enc64(&current, FE_SSE_CVTSD2SI64rr, regDest, regSrc1);
    err |= fe_enc64(&current, FE_BTC64ri, regDest, 63);
    err |= fe_enc64(&jmpBufEnd, FE_JMP, (intptr_t) current);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the FMVXD instruction.
 * Description: move ths fp value in fp register rs1 unchanged to 64bits in rd
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMVXD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMVXD...\n");
    FeReg regSrc1 = getFpReg(instr->op_field.op.reg_src_1, r_info, FIRST_FP_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_SSE_MOVQrr, regDest, regSrc1);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the FCVTDL instruction.
 * Description: convert a signed 64-bit int in rs1 to a double fp number in fp register rd
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTDL(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTDL...\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    err |= fe_enc64(&current, FE_SSE_XORPDrr, regDest, regDest); //needs to be done because of cvtsi2ss design
    err |= fe_enc64(&current, FE_SSE_CVTSI2SD64rr, regDest, regSrc1);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FCVTDLU instruction.
 * Description: convert a unsigned 64-bit int in rs1 to a double fp number in fp register rd
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FCVTDLU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FCVTDLU...\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    err |= fe_enc64(&current, FE_SSE_XORPDrr, regDest, regDest); //needs to be done because of cvtsi2ss design
    err |= fe_enc64(&current, FE_TEST64rr, regSrc1, regSrc1);

    uint8_t *jmpBufSign = current;
    err |= fe_enc64(&current, FE_JS, (intptr_t) current); //dummy
    err |= fe_enc64(&current, FE_SSE_CVTSI2SD64rr, regDest, regSrc1);

    uint8_t *jmpBufEnd = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy

    //set jump address
    err |= fe_enc64(&jmpBufSign, FE_JS, (intptr_t) current);

    if (regSrc1 != FIRST_REG) {
        err |= fe_enc64(&current, FE_MOV64rr, FIRST_REG, regSrc1);
    }
    err |= fe_enc64(&current, FE_MOV64rr, SECOND_REG, regSrc1);
    err |= fe_enc64(&current, FE_AND32ri, FIRST_REG, 0x1);
    err |= fe_enc64(&current, FE_SHR64ri, SECOND_REG, 0x1);
    err |= fe_enc64(&current, FE_OR64rr, SECOND_REG, FIRST_REG);
    err |= fe_enc64(&current, FE_SSE_CVTSI2SD64rr, regDest, SECOND_REG);
    err |= fe_enc64(&current, FE_SSE_ADDSDrr, regDest, regDest);


    //set jump address
    err |= fe_enc64(&jmpBufEnd, FE_JMP, (intptr_t) current);


    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}

/**
 * Translate the FMVDX instruction.
 * Description: move the lower 64-bits from int register rs1 to the fp register rd
 * the bits don't get changed
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_FMVDX(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate FMVDX...\n");
    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getFpRegNoLoad(instr->op_field.op.reg_dest, r_info, FIRST_FP_REG);

    err |= fe_enc64(&current, FE_SSE_MOVQrr, regDest, regSrc1);

    setFpReg(instr->op_field.op.reg_dest, r_info, regDest);
}
