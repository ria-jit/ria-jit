//
// Created by Flo Schmidt on 12.09.20.
//

#include <gen/translate.h>
#include <util/util.h>
#include "translate_pseudo.h"

///set the pc to next addr after inst
/**
 * Translate the PC_NEXT_INST pseudo instruction that is inserted when a block would overflow our buffer.
 * Emits instructions that sets the PC RISC_V register in our gp_reg_file to the given address.
 * @param addr the address the risc PC reg should have at the end of the current block (the start address of the next
 *        block)
 * @param reg_base the base_address of the gp_reg_file.
 */
void translate_PC_NEXT_INST(const t_risc_addr addr, uint64_t reg_base) {
    ///write chainEnd to be chained by chainer
    if (flag_translate_opt) {
        err |= fe_enc64(&current, FE_LEA64rm, FE_AX, FE_MEM(FE_IP, 0, 0, 0));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((uint64_t) &chain_end), FE_AX);
    }

    ///set pc
    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, addr);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(reg_base + 8 * pc), FE_AX);
}

/**
 * Translate the NOP instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_NOP(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate NOP…\n");
    err |= fe_enc64(&current, FE_NOP);
}

/**
 * Translate the SILENT_NOP instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SILENT_NOP(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SILENT_NOP…\n");
    //Intentionally left blank. Can be used in order to ignore parts of the block cache prior to translation.
}

/**
 * Translate the MV instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_MV(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate MV…\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_MOV64rr, regDest, regSrc1);
    storeRd(instr, r_info, regDest);
}

/**
 * Translate the NOT instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_NOT(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate NOT…\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_MOV64rr, regDest, regSrc1);
    err |= fe_enc64(&current, FE_NOT64r, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the NEG instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_NEG(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate NEG…\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_MOV64rr, regDest, regSrc1);
    err |= fe_enc64(&current, FE_NEG64r, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the NEGW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_NEGW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate NEGW…\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_MOV32rr, regDest, regSrc1);
    err |= fe_enc64(&current, FE_NEG32r, regDest);
    err |= fe_enc64(&current, FE_MOVSXr64r32, regDest, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the SEXTW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SEXTW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SEXTW…\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_MOVSXr64r32, regDest, regSrc1);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the SEQZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SEQZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SEQZ…\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_CMP64ri, regSrc1, 0);
    err |= fe_enc64(&current, FE_SETZ8r, regDest);
    err |= fe_enc64(&current, FE_AND64ri, regDest, 0x1);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the SNEZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SNEZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SNEZ…\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_CMP64ri, regSrc1, 0);
    err |= fe_enc64(&current, FE_SETNZ8r, regDest);
    err |= fe_enc64(&current, FE_AND64ri, regDest, 0x1);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the SLTZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SLTZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SLTZ…\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_CMP64ri, regSrc1, 0);
    err |= fe_enc64(&current, FE_SETL8r, regDest);
    err |= fe_enc64(&current, FE_AND64ri, regDest, 0x1);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the SGTZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SGTZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SGTZ…\n");

    FeReg regSrc1 = getRs1(instr, r_info, FIRST_REG);
    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_CMP64ri, regSrc1, 0);
    err |= fe_enc64(&current, FE_SETG8r, regDest);
    err |= fe_enc64(&current, FE_AND64ri, regDest, 0x1);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the LI instruction.
 * Loads the value in the immediate field into the destination register.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LI(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate LI…\n");

    FeReg regDest = getRd(instr, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_MOV64ri, regDest, instr->imm);

    storeRd(instr, r_info, regDest);
}
