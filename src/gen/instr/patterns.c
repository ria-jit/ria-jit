//
// Created by jhne on 10/3/20.
//

#include <gen/instr/patterns.h>
#include <gen/translate.h>
#include <util/typedefs.h>
#include <common.h>


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
        {LUI, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LD, rd_h1, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDI, rd_h1, DONT_CARE, rd_h1, 1, 0, 0, 0},
        {SD, rd_h2, rd_h1, DONT_CARE, 1, 0, 2, 1}
};

const pattern_element p_1_elem[] = {
        {LUI, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LW, rd_h1, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDIW, rd_h1, DONT_CARE, rd_h1, 1, 0, 0, 0},
        {SW, rd_h2, rd_h1, DONT_CARE, 1, 0, 2, 1}
};

const pattern_element p_2_elem[] = {
        {AUIPC, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDI, rd_h1, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {LW, rd_h1, DONT_CARE, DONT_CARE, 1, 0, 0, 0},
        {ADDIW, rd_h1, DONT_CARE, DONT_CARE, 2, 0, 0, 0},
        {SW, rd_h1, rd_h2, DONT_CARE, 0, 3, 2, 2}
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
        {ADDIW, DONT_CARE, DONT_CARE, rs1_h1, 0, 0, 0, 0}, //rd != rs1 ???
        {SLLI, rd_h1, DONT_CARE, rd_h1, 0, 0, 1, 32},
        {SRLI, rd_h1, DONT_CARE, rd_h1, 0, 0, 1, 32}
};

const pattern_element p_8_elem[] = {
        {ADDIW, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 1, 0}
};

const pattern_element p_9_elem[] = {
        {LUI, DONT_CARE, DONT_CARE, DONT_CARE, 0, 0, 0, 0},
        {ADDI, rd_h1, DONT_CARE, not_rd_hs1, 0, 0, 1, 0},
        {SLLI, not_rd_hs1, DONT_CARE, rd_h2, 1, 0, 1, 32},
        {SRLI, rd_h1, DONT_CARE, rd_h1, 2, 0, 1, 32}
};


void emit_pattern_0(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 0: inc mem64 at %p\n", instr->addr);

    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, (instr->addr + instr->imm));
    err |= fe_enc64(&current, FE_ADD64mi, FE_MEM(FE_AX, 0, 0, 0), instr[2].imm);
}

void emit_pattern_2(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 2: inc m64 at %p\n", instr->addr);

    //err |= fe_enc64(&current, FE_ADD32mi, FE_MEM_ADDR(instr->addr + instr->imm + instr[1].imm + instr[2].imm), instr[3].imm);
    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, (instr->addr + instr->imm + instr[1].imm + instr[2].imm)),
    err |= fe_enc64(&current, FE_ADD32mi, FE_MEM(FE_AX,0,0,0), instr[3].imm);
}

void emit_pattern_3(const t_risc_instr instrs[static 2], const register_info *r_info) {
    log_asm_out("emit pattern 3: AUIPC + ADDI at %p\n", instrs[0].addr);

    if(instrs[1].reg_dest == x0) {
        log_asm_out("pattern 3: addi x0 at %p\n", current);
        return;
    }

    if(r_info->mapped[instrs[1].reg_dest]) {
        err |= fe_enc64(&current, FE_MOV64ri, r_info->map[instrs[0].reg_dest], (instrs[0].addr + instrs[0].imm + instrs[1].imm));
    } else {
        if((instrs[0].addr + instrs->imm + instrs[1].imm) == (int)(instrs[0].addr + instrs->imm + instrs[1].imm)) {
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), (instrs[0].addr + instrs->imm + instrs[1].imm));
        } else {
            err |= fe_enc64(&current, FE_MOV64ri, FE_AX, (instrs[0].addr + instrs->imm + instrs[1].imm));
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), FE_AX);
        }
    }
}

void emit_pattern_4(const t_risc_instr instrs[static 2], const register_info *r_info) {
    log_asm_out("emit pattern 4: AUIPC + LW at %p\n", instrs[0].addr);

    if(r_info->mapped[instrs[1].reg_dest]) {
        err |= fe_enc64(&current, FE_MOV64ri, FE_AX, (instrs[0].addr + instrs[0].imm + instrs[1].imm));
        err |= fe_enc64(&current, FE_MOVSXr64m32, r_info->map[instrs[1].reg_dest], FE_MEM(FE_AX, 0, 0, 0));
    } else {
        err |= fe_enc64(&current, FE_MOV64ri, FE_AX, (instrs[0].addr + instrs[0].imm + instrs[1].imm));
        err |= fe_enc64(&current, FE_MOVSXr64m32, FE_AX, FE_MEM(FE_AX, 0, 0, 0));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), FE_AX);
    }
}

void emit_pattern_5(const t_risc_instr instrs[static 2], const register_info *r_info) {
    log_asm_out("emit pattern 5: AUIPC + LD at %p\n", instrs[0].addr);

    if(r_info->mapped[instrs[1].reg_dest]) {
        err |= fe_enc64(&current, FE_MOV64ri, FE_AX, (instrs[0].addr + instrs[0].imm + instrs[1].imm));
        err |= fe_enc64(&current, FE_MOV64rm, r_info->map[instrs[1].reg_dest], FE_MEM(FE_AX, 0, 0, 0));
    } else {
        err |= fe_enc64(&current, FE_MOV64ri, FE_AX, (instrs[0].addr + instrs[0].imm + instrs[1].imm));
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM(FE_AX, 0, 0, 0));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), FE_AX);
    }
}

void emit_pattern_6(const t_risc_instr instrs[static 2], const register_info *r_info) {
    log_asm_out("emit pattern 6: SLLI +  SRLI at %p\n", instrs->addr);

    if(r_info->mapped[instrs[0].reg_src_1]) {
        if(r_info->mapped[instrs[1].reg_dest]) {
            err |= fe_enc64(&current, FE_MOV32rr, r_info->map[instrs[1].reg_dest], r_info->map[instrs[0].reg_src_1]);   //sets high 32 to zero
        } else {
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), 0);

            err |= fe_enc64(&current, FE_MOV32mr,
                            FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), r_info->map[instrs[0].reg_src_1]);
        }
    } else if(r_info->mapped[instrs[1].reg_dest]){
        err |= fe_enc64(&current, FE_MOV32rm, r_info->map[instrs[1].reg_dest],
                        FE_MEM_ADDR(r_info->base + 8 * instrs[0].reg_src_1));  //sets high 32 to zero
    } else {
        err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instrs[0].reg_src_1)); //sets high 32 to zero
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest), FE_AX);
    }
}

void emit_pattern_7(const t_risc_instr instr[static 3], const register_info *r_info) {
    log_asm_out("emit pattern 7: ADDIW + SLLI + SRLI at %p\n", instr[0].addr);

    if(r_info->mapped[instr[0].reg_src_1]) {
        err |= fe_enc64(&current, FE_ADD32ri, r_info->map[instr[0].reg_src_1], instr[0].imm);
    } else {
        err |= fe_enc64(&current, FE_ADD64mi,
                        FE_MEM_ADDR(r_info->base + 8 * r_info->map[instr[0].reg_src_1]), instr[0].imm);
        err |= fe_enc64(&current, FE_MOV32mi,
                        FE_MEM_ADDR(r_info->base + 8 * r_info->map[instr[0].reg_src_1] + 4), 0);
    }
}

void emit_pattern_8(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("emit pattern 8: ADDIW 32b sign extension at %p\n", instr[0].addr);

    if(r_info->mapped[instr->reg_src_1]) {
        if(r_info->mapped[instr->reg_dest]) {
            err |= fe_enc64(&current, FE_MOVSXr64r32, r_info->map[instr->reg_dest], r_info->map[instr->reg_src_1]);
        } else {
            err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, r_info->map[instr->reg_src_1]);
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_AX);
        }
    } else {
        if(r_info->mapped[instr->reg_dest]) {
            err |= fe_enc64(&current, FE_MOVSXr64m32, r_info->map[instr->reg_dest], FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
        } else {
            err |= fe_enc64(&current, FE_MOVSXr64m32, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_AX);
        }
    }
}

void emit_pattern_9(const t_risc_instr instrs[static 4], const register_info *r_info) {
    printf("emit pattern 9: LUI + ADDIW + SLLI + SRLI at %p\n", instrs[0].addr);

    if(r_info->mapped[instrs[1].reg_dest]) {
        err |= fe_enc64(&current, FE_MOV64ri, r_info->map[instrs[1].reg_dest], instrs[0].imm);
    } else {
        err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * instrs[1].reg_dest));
    }

    emit_pattern_6(&instrs[2], r_info);
}

//order = length, descending
//order is important because longer patterns can contain shorter ones,
//and would never match if the shorter pattern would be tested before the longer one
//(and if the shorter pattern is contained at the beginning).
const pattern patterns[] = {
        {p_2_elem, 5, &emit_pattern_2}, //inc mem64
        {p_9_elem, 4, &emit_pattern_9}, //LUI + ADDI + SRLI + SLLI
        {p_7_elem, 3, &emit_pattern_7}, //ADDIW + SLLI + SRLI
        {p_3_elem, 2, &emit_pattern_3}, //AUIPC + ADDI
        {p_4_elem, 2, &emit_pattern_4}, //AUIPC + LW
        {p_5_elem, 2, &emit_pattern_5}, //AUIPC + LD
        {p_6_elem, 2, &emit_pattern_6}, //SLLI + SRLI
        {p_8_elem, 1, &emit_pattern_8}, //ADDIW SX only

        {0, 0, 0},      //stopper

        //unused
        {p_0_elem, 4, &emit_pattern_0},
        {p_1_elem, 4, &emit_pattern_0},
        {}
};

void init_patterns() {

}
