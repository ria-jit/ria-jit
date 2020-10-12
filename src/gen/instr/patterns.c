//
// Created by jhne on 10/3/20.
//

#include <gen/instr/patterns.h>
#include <gen/translate.h>
#include <util/typedefs.h>
#include <util/util.h>


/**
 * how to create a pattern:
 *
 * ----1: elements:-----
 * Element fields:
 *  mnem : mnem
 *  rs1, rs2, rd :
 *      rs1_h1 means, that rs1 of the instruction
 *      has to be the same as in the instruction at
 *      the position stored in h1. (block_cache[pattern_start + h1]).
 *      rs2, rd work the same. all are also available with h2.
 *
 *      Example:
 *      pattern_element jsjfbefjb[] = {
 *          {.....},
 *          {.....},
 *          {ADDI, rd_h1, DONT_CARE, rs1_h2, 1, 0, 1, 32}
 *      };
 *
 *      In this example, rs1 of the 3rd instruction has to be
 *      the same register as rd of the second instruction,
 *      and rd has to be the same register as rs1 of the first instruction.
 *      The immediate has to be 32.
 *
 *      NOTE: not all options are implemented for every field yet
 *            (to avoid unnecessary comparisons in matcher).
 *            Do so in the switch-case under the corresponding comments (e.g. "rs1 match") in optimize.c
 *
 *  h1, h2 : helper variables
 *
 *  imm :
 *      0 = dont care ;
 *      1 = value has to be equal to imm_value ;
 *      2 = value has to be equal to immediate of (block_cache[pattern_start + h1])
 *
 * ----2: emitter function:-----
 * Emits the x86 equivalent of the pattern.
 *
 * ----3: entry in patterns[]:-----
 *
 * */


const pattern_element p_0_elem[] = {
        {LUI,  DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LD,   rd_h1,     DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDI, rd_h1,     DONT_CARE, rd_h1,     1, 0, 0, 0},
        {SD,   rd_h2,     rd_h1,     DONT_CARE, 1, 0, 2, 1}
};

const pattern_element p_1_elem[] = {
        {LUI,   DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LW,    rd_h1,     DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDIW, rd_h1,     DONT_CARE, rd_h1,     1, 0, 0, 0},
        {SW,    rd_h2,     rd_h1,     DONT_CARE, 1, 0, 2, 1}
};

const pattern_element p_2_elem[] = {
        {AUIPC, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDI,  rd_h1,     DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LW,    rd_h1,     DONT_CARE, DONT_CARE, 1, 0, 0, 0},
        {ADDIW, rd_h1,     DONT_CARE, DONT_CARE, 2, 0, 0, 0},
        {SW,    rd_h1,     rd_h2,     DONT_CARE, 0, 3, 2, 2}
};

const pattern_element p_3_elem[] = {
        {AUIPC, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDI, rd_h1, DONT_CARE, rd_h1, 0, 0, 0, 0}
};

const pattern_element p_4_elem[] = {
        {AUIPC, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LW, rd_h1, DONT_CARE, rd_h1, 0, 0, 0, 0}
};

const pattern_element p_5_elem[] = {
        {AUIPC, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LD, rd_h1, DONT_CARE, rd_h1, 0, 0, 0, 0}
};

const pattern_element p_6_elem[] = {
        {SLLI, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 1, 32},    //rd == rs1???
        {SRLI, rd_h1, DONT_CARE, rd_h1, 0, 0, 1, 32}
};

const pattern_element p_7_elem[] = {
        {ADDIW, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {SLLI,  rd_h1,     DONT_CARE, rd_h1,     0, 0, 1, 32},
        {SRLI,  rd_h1,     DONT_CARE, rd_h1,     0, 0, 1, 32}
};

const pattern_element p_8_elem[] = {
        {ADDIW, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 1, 0}
};

const pattern_element p_9_elem[] = {
        {LUI,  DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDI, rd_h1,     DONT_CARE, not_rd_h1, 0, 0, 1, 0},
        {SLLI, not_rd_h1, DONT_CARE, rd_h2,     1, 0, 1, 32},
        {SRLI, rd_h1,     DONT_CARE, rd_h1,     2, 0, 1, 32}
};

const pattern_element p_10_elem[] = {
        {ADDI, x0, DONT_CARE, x0, 0, 0, 1, 0}
};

const pattern_element p_11_elem[] = {
        {ADDI, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 1, 0}
};

const pattern_element p_12_elem[] = {
        {XORI, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 1, -1}
};

const pattern_element p_13_elem[] = {
        {SUB, x0, DONT_CARE, DONT_CARE, 0, 0, 0, 0}
};

const pattern_element p_14_elem[] = {
        {SUBW, x0, DONT_CARE, DONT_CARE, 0, 0, 0, 0}
};

const pattern_element p_15_elem[] = {
        {SLTIU, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 1, 1}
};

const pattern_element p_16_elem[] = {
        {SLTU, x0, DONT_CARE, DONT_CARE, 0, 0, 0, 0}
};

const pattern_element p_17_elem[] = {
        {SLT, DONT_CARE, x0, DONT_CARE, 0, 0, 0, 0}
};

const pattern_element p_18_elem[] = {
        {SLT, x0, DONT_CARE, DONT_CARE, 0, 0, 0, 0}
};

const pattern_element p_19_elem[] = {
        {LUI,  DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDI, rd_h1,     DONT_CARE, rd_h1,     0, 0, 0, 0}
};

void emit_pattern_0(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 0: inc mem64 at 0x%lx\n", instr->addr);

    //TODO Check if address fits into 32bit and use non RIP-relative absolute
    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, (instr->addr + instr->imm));
    err |= fe_enc64(&current, FE_ADD64mi, FE_MEM(FE_AX, 0, 0, 0), instr[2].imm);
}

void emit_pattern_2(const t_risc_instr instrs[static 5], const register_info *r_info) {
    log_asm_out("emit pattern 2: inc m64 at 0x%lx\n", instrs[0].addr);

    t_risc_addr addr = instrs[0].addr + instrs[0].imm + instrs[1].imm + instrs[2].imm;
    if ((int64_t) addr == (int32_t) addr) {
        err |= fe_enc64(&current, FE_ADD32mi, FE_MEM(0, 0, 0, addr), instrs[3].imm);
    } else {
        err |= fe_enc64(&current, FE_MOV64ri, FE_AX, addr);
        err |= fe_enc64(&current, FE_ADD32mi, FE_MEM(FE_AX, 0, 0, 0), instrs[3].imm);
    }
}

void emit_pattern_3(const t_risc_instr instrs[static 2], const register_info *r_info) {
    log_asm_out("emit pattern 3: AUIPC + ADDI at 0x%lx\n", instrs[0].addr);

    if (instrs[1].reg_dest == x0) {
        log_asm_out("pattern 3: addi x0 at %p\n", (void *) current);
        return;
    }

    t_risc_addr value = instrs[0].addr + instrs[0].imm + instrs[1].imm;
    if (r_info->mapped[instrs[1].reg_dest]) {
        err |= fe_enc64(&current, FE_MOV64ri, r_info->map[instrs[0].reg_dest], value);
    } else {
        if ((int64_t) value == (int32_t) value) {
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), value);
        } else {
            err |= fe_enc64(&current, FE_MOV64ri, FE_AX, value);
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), FE_AX);
        }
    }
}

void emit_pattern_4(const t_risc_instr instrs[static 2], const register_info *r_info) {
    log_asm_out("emit pattern 4: AUIPC + LW at 0x%lx\n", instrs[0].addr);

    t_risc_addr addr = instrs[0].addr + instrs[0].imm + instrs[1].imm;
    if (r_info->mapped[instrs[1].reg_dest]) {
        if ((int64_t) addr == (int32_t) addr) {
            err |= fe_enc64(&current, FE_MOVSXr64m32, r_info->map[instrs[1].reg_dest], FE_MEM(0, 0, 0, addr));
        } else {
            err |= fe_enc64(&current, FE_MOV64ri, FE_AX, addr);
            err |= fe_enc64(&current, FE_MOVSXr64m32, r_info->map[instrs[1].reg_dest], FE_MEM(FE_AX, 0, 0, 0));
        }
    } else {
        if ((int64_t) addr == (int32_t) addr) {
            err |= fe_enc64(&current, FE_MOVSXr64m32, FE_AX, FE_MEM(0, 0, 0, addr));
        } else {
            err |= fe_enc64(&current, FE_MOV64ri, FE_AX, addr);
            err |= fe_enc64(&current, FE_MOVSXr64m32, FE_AX, FE_MEM(FE_AX, 0, 0, 0));
        }
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), FE_AX);
    }
}

void emit_pattern_5(const t_risc_instr instrs[static 2], const register_info *r_info) {
    log_asm_out("emit pattern 5: AUIPC + LD at 0x%lx\n", instrs[0].addr);

    t_risc_addr addr = instrs[0].addr + instrs[0].imm + instrs[1].imm;
    if (r_info->mapped[instrs[1].reg_dest]) {
        if ((int64_t) addr == (int32_t) addr) {
            err |= fe_enc64(&current, FE_MOV64rm, r_info->map[instrs[1].reg_dest], FE_MEM(0, 0, 0, addr));
        } else {
            err |= fe_enc64(&current, FE_MOV64ri, FE_AX, addr);
            err |= fe_enc64(&current, FE_MOV64rm, r_info->map[instrs[1].reg_dest], FE_MEM(FE_AX, 0, 0, 0));
        }
    } else {
        if ((int64_t) addr == (int32_t) addr) {
            err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM(0, 0, 0, addr));
        } else {
            err |= fe_enc64(&current, FE_MOV64ri, FE_AX, addr);
            err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM(FE_AX, 0, 0, 0));
        }
    }
}

void emit_pattern_6(const t_risc_instr instrs[static 2], const register_info *r_info) {
    log_asm_out("emit pattern 6: SLLI +  SRLI at 0x%lx\n", instrs[0].addr);

    if (r_info->mapped[instrs[0].reg_src_1]) {
        if (r_info->mapped[instrs[1].reg_dest]) {
            //sets high 32 to zero
            err |= fe_enc64(&current, FE_MOV32rr, r_info->map[instrs[1].reg_dest], r_info->map[instrs[0].reg_src_1]);
        } else {
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), 0);

            err |= fe_enc64(&current, FE_MOV32mr, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest),
                            r_info->map[instrs[0].reg_src_1]);
        }
    } else if (r_info->mapped[instrs[1].reg_dest]) {
        err |= fe_enc64(&current, FE_MOV32rm, r_info->map[instrs[1].reg_dest],
                        FE_MEM_ADDR(r_info->base + 8 * instrs[0].reg_src_1));  //sets high 32 to zero
    } else {
        //sets high 32 to zero
        err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instrs[0].reg_src_1));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), FE_AX);
    }
}

void emit_pattern_7(const t_risc_instr instr[static 3], const register_info *r_info) {
    log_asm_out("emit pattern 7: ADDIW + SLLI + SRLI at 0x%lx\n", instr[0].addr);

    FeReg regDest = getRd(instr, r_info, FIRST_REG);
    FeReg regSrc1 = getRs1(instr, r_info, regDest);

    if (regSrc1 != regDest) {
        err |= fe_enc64(&current, FE_MOV32rr, regDest, regSrc1);
    }

    err |= fe_enc64(&current, FE_ADD32ri, regDest, instr[0].imm);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the SEXTW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_8_SEXTW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 8: ADDIW 32b sign extension at 0x%lx\n", instr[0].addr);

    if (r_info->mapped[instr->reg_src_1]) {
        if (r_info->mapped[instr->reg_dest]) {
            err |= fe_enc64(&current, FE_MOVSXr64r32, r_info->map[instr->reg_dest], r_info->map[instr->reg_src_1]);
        } else {
            err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, r_info->map[instr->reg_src_1]);
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_AX);
        }
    } else {
        if (r_info->mapped[instr->reg_dest]) {
            err |= fe_enc64(&current, FE_MOVSXr64m32, r_info->map[instr->reg_dest],
                            FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
        } else {
            err |= fe_enc64(&current, FE_MOVSXr64m32, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_AX);
        }
    }
}

void emit_pattern_9(const t_risc_instr instrs[static 4], const register_info *r_info) {
    log_asm_out("emit pattern 9: LUI + ADDIW + SLLI + SRLI at 0x%lx\n", instrs[0].addr);

    if (r_info->mapped[instrs[1].reg_dest]) {
        err |= fe_enc64(&current, FE_MOV64ri, r_info->map[instrs[1].reg_dest], instrs[0].imm);
    } else {
        err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest));
    }

    emit_pattern_6(&instrs[2], r_info);
}

/**
 * Translate the NOP instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_10_NOP(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 10: ADDI as NOP at 0x%lx\n", instr->addr);
    err |= fe_enc64(&current, FE_NOP);
}

/**
 * Translate the MV instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_11_MV(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 11: ADDI as MV at 0x%lx\n", instr->addr);

    FeReg regDest = getRd(instr, r_info, FIRST_REG);
    FeReg regSrc1 = getRs1(instr, r_info, regDest);

    if (regDest != regSrc1 && regDest != FIRST_REG) {
        err |= fe_enc64(&current, FE_MOV64rr, regDest, regSrc1);
    }

    storeRd(instr, r_info, regSrc1);
}

/**
 * Translate the NOT instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_12_NOT(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 12: XORI as NOT at 0x%lx\n", instr->addr);

    FeReg regDest = getRd(instr, r_info, FIRST_REG);
    FeReg regSrc1 = getRs1(instr, r_info, regDest);

    if (regDest != regSrc1) {
        err |= fe_enc64(&current, FE_MOV64rr, regDest, regSrc1);
    }
    err |= fe_enc64(&current, FE_NOT64r, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the NEG instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_13_NEG(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 13: SUB as NEG at 0x%lx\n", instr->addr);

    FeReg regDest = getRd(instr, r_info, FIRST_REG);
    FeReg regSrc2 = getRs2(instr, r_info, regDest);

    if (regDest != regSrc2) {
        err |= fe_enc64(&current, FE_MOV64rr, regDest, regSrc2);
    }
    err |= fe_enc64(&current, FE_NEG64r, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the NEGW instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_14_NEGW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 14: SUBW as NEGW at 0x%lx\n", instr->addr);

    FeReg regDest = getRd(instr, r_info, FIRST_REG);
    FeReg regSrc2 = getRs2(instr, r_info, regDest);

    if (regDest != regSrc2) {
        err |= fe_enc64(&current, FE_MOV32rr, regDest, regSrc2);
    }
    err |= fe_enc64(&current, FE_NEG32r, regDest);
    err |= fe_enc64(&current, FE_MOVSXr64r32, regDest, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the SEQZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_15_SEQZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 15: SLTIU as SEQZ at 0x%lx\n", instr->addr);

    FeReg regDest = getRd(instr, r_info, FIRST_REG);
    FeReg regSrc1 = getRs1(instr, r_info, regDest);

    err |= fe_enc64(&current, FE_TEST64rr, regSrc1, regSrc1);
    err |= fe_enc64(&current, FE_SETZ8r, regDest);
    //Implicit zero extension to 64bit
    err |= fe_enc64(&current, FE_MOVZXr32r8, regDest, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the SNEZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_16_SNEZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 16: SLTU as SNEZ at 0x%lx\n", instr->addr);

    FeReg regDest = getRd(instr, r_info, FIRST_REG);
    FeReg regSrc2 = getRs2(instr, r_info, regDest);

    err |= fe_enc64(&current, FE_TEST64rr, regSrc2, regSrc2);
    err |= fe_enc64(&current, FE_SETNZ8r, regDest);
    //Implicit zero extension to 64bit
    err |= fe_enc64(&current, FE_MOVZXr32r8, regDest, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the SLTZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_17_SLTZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 17: SLT as SLTZ at 0x%lx\n", instr->addr);

    FeReg regDest = getRd(instr, r_info, FIRST_REG);
    FeReg regSrc1 = getRs1(instr, r_info, regDest);

    if (regSrc1 != regDest) {
        err |= fe_enc64(&current, FE_MOV64rr, regDest, regSrc1);
    }
    err |= fe_enc64(&current, FE_SHR64ri, 63);


    storeRd(instr, r_info, regDest);
}

/**
 * Translate the SGTZ instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_18_SGTZ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 18: SLT as SGTZ at 0x%lx\n", instr->addr);

    FeReg regDest = getRd(instr, r_info, FIRST_REG);
    FeReg regSrc2 = getRs2(instr, r_info, regDest);

    err |= fe_enc64(&current, FE_TEST64rr, regSrc2, regSrc2);
    err |= fe_enc64(&current, FE_SETG8r, regDest);
    //Implicit zero extension to 64bit
    err |= fe_enc64(&current, FE_MOVZXr32r8, regDest, regDest);

    storeRd(instr, r_info, regDest);
}

/**
 * Translate the LI instruction.
 * Loads the value in the immediate field into the destination register.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void emit_pattern_19_LI(const t_risc_instr *instrs, const register_info *r_info) {
    log_asm_out("emit pattern 19: LUI ADDI as LI at 0x%lx\n", instrs->addr);

    FeReg regDest = getRd(instrs, r_info, FIRST_REG);

    err |= fe_enc64(&current, FE_MOV64ri, regDest, instrs[0].imm + instrs[1].imm);

    storeRd(instrs, r_info, regDest);
}


//order = length, descending
//order is important because longer patterns can contain shorter ones,
//and would never match if the shorter pattern would be tested before the longer one
//(and if the shorter pattern is contained at the beginning).
const pattern patterns[] = {
        {p_2_elem,  5, &emit_pattern_2},  //inc mem64
        {p_9_elem,  4, &emit_pattern_9},  //LUI + ADDI + SRLI + SLLI
        {p_7_elem,  3, &emit_pattern_7},  //ADDIW + SLLI + SRLI
        {p_3_elem,  2, &emit_pattern_3},  //AUIPC + ADDI
        {p_4_elem,  2, &emit_pattern_4},  //AUIPC + LW
        {p_5_elem,  2, &emit_pattern_5},  //AUIPC + LD
        {p_6_elem,  2, &emit_pattern_6},  //SLLI + SRLI
        {p_19_elem, 2, &emit_pattern_19_LI}, //LUI + ADDI
        {p_8_elem,  1, &emit_pattern_8_SEXTW},  //ADDIW SX only
        {p_10_elem, 1, &emit_pattern_10_NOP}, //ADDI NOP
        {p_11_elem, 1, &emit_pattern_11_MV}, //ADDI MV
        {p_12_elem, 1, &emit_pattern_12_NOT}, //XORI NOT
        {p_13_elem, 1, &emit_pattern_13_NEG}, //SUB NEG
        {p_14_elem, 1, &emit_pattern_14_NEGW}, //SUBW NEGW
        {p_15_elem, 1, &emit_pattern_15_SEQZ}, //SLTIU SEQZ
        {p_16_elem, 1, &emit_pattern_16_SNEZ}, //SLTU SNEZ
        {p_17_elem, 1, &emit_pattern_17_SLTZ}, //SLT SLTZ gcc seems to emit slti rd, rs, 0 instead
        {p_18_elem, 1, &emit_pattern_18_SGTZ}, //SLT SGTZ


        {0,         0, 0},      //stopper

        //unused
        {p_0_elem,  4, &emit_pattern_0},
        {p_1_elem,  4, &emit_pattern_0},
        {0}
};
