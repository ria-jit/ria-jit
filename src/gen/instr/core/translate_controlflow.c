//
// Created by flo on 21.05.20.
//

#include "translate_controlflow.h"
#include <env/flags.h>
#include <gen/instr/core/translate_arithmetic.h>
#include <util/log.h>
#include <fadec/fadec-enc.h>
#include <cache/return_stack.h>
#include <common.h>
#include <util/util.h>

#include <parser/parser.h>

static inline void
translate_controlflow_cmp_rs1_rs2(const t_risc_instr *instr, const register_info *r_info);

//inline void translate_controlflow_set_pc(const t_risc_instr *instr, const register_info *r_info, Label END, Label NOJUMP);

static inline void
translate_controlflow_set_pc2(const t_risc_instr *instr, const register_info *r_info, uint8_t *jmpLoc, uint64_t mnem);


void translate_JAL(const t_risc_instr *instr, const register_info *r_info, const context_info *c_info) {
    log_asm_out("Translate JAL\n");

    //add rd access to profiler
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_dest);
    }

    ///push to return stack
    if (flag_translate_opt_ras && (instr->reg_dest == x1 || instr->reg_dest == x5)) {
        rs_emit_push(instr, r_info);
    }

    ///set rd
    t_risc_instr aupicInstr = (t_risc_instr) {
            instr->addr,
            AUIPC,
            IMMEDIATE,
            x0,
            x0,
            instr->reg_dest,
            4
    };

    translate_AUIPC(&aupicInstr, r_info);

    ///jump...
    t_risc_addr target = instr->addr + instr->imm;

    t_cache_loc cache_loc;

    //potentially write back the register used by the translated AUIPC instruction above
    invalidateAllReplacements(r_info);

    if (!flag_translate_opt_chain || (cache_loc = lookup_cache_entry(target)) == UNSEEN_CODE ||
            cache_loc == (t_cache_loc) 1) {
        //afaik the "multiples of two" thing is resolved in parser.c

        ///write chainEnd to be chained by chainer
        if (flag_translate_opt_chain) {
            err |= fe_enc64(&current, FE_LEA64rm, FE_AX, FE_MEM(FE_IP, 0, 0, 0));
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((uint64_t) &chain_end), FE_AX);
        }

        ///set pc
        if (r_info->mapped[pc]) {
            err |= fe_enc64(&current, FE_MOV64ri, r_info->map[pc],
                            instr->addr + instr->imm);
        } else {
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * pc),
                            instr->addr + instr->imm);
        }
    } else {
        log_asm_out("DIRECT JUMP JAL\n");
        err |= fe_enc64(&current, FE_JMP, (intptr_t) cache_loc);
    }
}

void translate_JALR(const t_risc_instr *instr, const register_info *r_info, const context_info *c_info) {
    /**
     * The target address is obtained by adding the 12-bit signed I-immediate to the register rs1,
     * then setting the least-significant bit of the result to zero.
     * The address of the instruction following the jump (pc+4)is written to register rd.
     * Register x0 can be used as the destination if the result is not required.
     */
    log_asm_out("Translate JALR\n");

    //add rs1, rd access to profiler
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_src_1);
        RECORD_PROFILER(instr->reg_dest);
    }

    ///1: compute target address

    ///mov rs1 to temp register
    invalidateAllReplacements(r_info);
    if (r_info->mapped[instr->reg_src_1]) {
        err |= fe_enc64(&current, FE_MOV64rr, FE_AX, r_info->map[instr->reg_src_1]);
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    }

    ///add immediate to rs1
    err |= fe_enc64(&current, FE_ADD64ri, FE_AX, instr->imm);

    ///set last bit to zero
    //why??? not aligned to 4 bit boundary would throw exception anyway...
    err |= fe_enc64(&current, FE_AND64ri, FE_AX, -2);

    ///2: write addr of next instruction in rd
    if (instr->reg_dest != x0) {
        if (r_info->mapped[instr->reg_dest]) {
            err |= fe_enc64(&current, FE_MOV64ri, r_info->map[instr->reg_dest], instr->addr + 4);
        } else {
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), instr->addr + 4);
        }
    }

    ///3: check return stack
    if (flag_translate_opt_ras) {

        if (instr->reg_dest == x1 || instr->reg_dest == x5) {
            if (instr->reg_src_1 == x1 || instr->reg_src_1 == x5) {
                if (instr->reg_dest == instr->reg_src_1) {
                    ///push
                    rs_emit_push(instr, r_info);
                } else {
                    //not tested
                    ///pop and push
                    rs_emit_pop_RAX(false, r_info);
                    rs_emit_push(instr, r_info);
                    rs_jump_stack(r_info);
                }
            } else {
                ///push
                rs_emit_push(instr, r_info);
            }
        } else {
            if(instr->reg_src_1 == x1 || instr->reg_src_1 == x5) {
                ///pop
                rs_emit_pop_RAX(true, r_info);
            } else {
                ///none
            }
        }
     }


    if (instr->reg_src_2 == 1 && flag_translate_opt_chain) {

        //TODO: more efficient way of obtaining target (without parsing)

        t_risc_instr tmp_p_instr = {};
        tmp_p_instr.addr = instr->addr - 4;

        parse_instruction(&tmp_p_instr);

        t_risc_addr target = tmp_p_instr.addr + tmp_p_instr.imm + instr->imm;

        t_cache_loc cache_loc = 0;
        if((cache_loc = lookup_cache_entry(target)) == UNSEEN_CODE || cache_loc == (t_cache_loc) 1) {
            ///4: write chainEnd to be chained by chainer
            log_asm_out("CHAIN JALR\n");
            err |= fe_enc64(&current, FE_LEA64rm, FE_AX, FE_MEM(FE_IP, 0, 0, 0));
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((uint64_t) &chain_end), FE_AX);
        } else {
            log_asm_out("DIRECT JUMP JALR\n");
            err |= fe_enc64(&current, FE_MOV64ri, FE_CX, cache_loc);
            err |= fe_enc64(&current, FE_JMPr, FE_CX);
        }

    } else {
        ///dont chain
        log_asm_out("DON'T CHAIN JALR\n");
        if (flag_translate_opt_chain) {
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR((uint64_t) &chain_end), 0);
        }
    }

    ///5: write target addr to pc
    if (r_info->mapped[pc]) {
        err |= fe_enc64(&current, FE_MOV64rr, r_info->map[pc], FE_AX);
    } else {
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * pc), FE_AX);
    }

}

void translate_BEQ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BEQ\n");
    //does "The 12-bit B-immediate encodes signed offsets in multiples of 2" already account for the always-zero LSB????

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

    ///dummy jump
    uint8_t *jump_b = current;
    err |= fe_enc64(&current, FE_JNZ, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_b, FE_JNZ);
}

void translate_BNE(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BNE\n");
    //does "The 12-bit B-immediate encodes signed offsets in multiples of 2" already account for the always-zero LSB????

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

    ///dummy jump
    uint8_t *jump_b = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_b, FE_JZ);
}

void translate_BLT(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BLT\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

    ///dummy jump
    uint8_t *jump_b = current;
    err |= fe_enc64(&current, FE_JGE, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_b, FE_JGE);
}

void translate_BLTU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BLTU\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

    ///dummy jump
    uint8_t *jump_b = current;
    err |= fe_enc64(&current, FE_JNC, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_b, FE_JNC);
}

void translate_BGE(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BGE\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

    ///dummy jump
    uint8_t *jump_l = current;
    err |= fe_enc64(&current, FE_JL, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_l, FE_JL);
}

void translate_BGEU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BGEU\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

    ///dummy jump
    uint8_t *jump_b = current;
    err |= fe_enc64(&current, FE_JC, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_b, FE_JC);
}


static inline void
translate_controlflow_cmp_rs1_rs2(const t_risc_instr *instr, const register_info *r_info) {

    //add rs1, rs2 access to profiler
    if (flag_do_profile) {
        RECORD_PROFILER(instr->reg_src_1);
        RECORD_PROFILER(instr->reg_src_2);
    }

    ///compare registers:
    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regSrc2 = getRs2(instr, r_info);

    err |= fe_enc64(&current, FE_CMP64rr, regSrc1, regSrc2);

    invalidateAllReplacements(r_info);
}

static inline void
translate_controlflow_set_pc2(const t_risc_instr *instr, const register_info *r_info, uint8_t *noJmpLoc,
                              uint64_t jmpMnem) {
    ///set pc: BRANCH
    t_risc_addr target = instr->addr + instr->imm;
    t_cache_loc cache_loc;
    if (flag_translate_opt_chain && (cache_loc = lookup_cache_entry(target)) != UNSEEN_CODE &&
            cache_loc != (t_cache_loc) 1) {
        log_asm_out("DIRECT JUMP BRANCH 1\n");
        err |= fe_enc64(&current, FE_JMP, (intptr_t) cache_loc);
    } else {
        ///write chainEnd to be chained by chainer
        if (flag_translate_opt_chain) {
            err |= fe_enc64(&current, FE_LEA64rm, FE_AX, FE_MEM(FE_IP, 0, 0, 0));
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((uint64_t) &chain_end), FE_AX);
        }

        if (r_info->mapped[pc]) {
            err |= fe_enc64(&current, FE_MOV64ri, r_info->map[pc], (instr->addr + ((int64_t) (instr->imm))));
        } else {
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * pc),
                            instr->addr + (int64_t) instr->imm);
        }
    }

    uint8_t *endJmpLoc = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jump

    err |= fe_enc64(&noJmpLoc, jmpMnem, (intptr_t) current); //replace dummy

    ///set pc: NO BRANCH
    target = instr->addr + 4;
    if (flag_translate_opt_chain && (cache_loc = lookup_cache_entry(target)) != UNSEEN_CODE &&
            cache_loc != (t_cache_loc) 1) {
        log_asm_out("DIRECT JUMP BRANCH 2\n");
        err |= fe_enc64(&current, FE_JMP, (intptr_t) cache_loc);
    } else {
        ///write chainEnd to be chained by chainer
        if (flag_translate_opt_chain) {
            err |= fe_enc64(&current, FE_LEA64rm, FE_AX, FE_MEM(FE_IP, 0, 0, 0));
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((uint64_t) &chain_end), FE_AX);
        }

        if (r_info->mapped[pc]) {
            err |= fe_enc64(&current, FE_MOV64ri, r_info->map[pc], instr->addr + 4);
        } else {
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * pc), instr->addr + 4);
        }
    }

    err |= fe_enc64(&endJmpLoc, FE_JMP, (intptr_t) current); //replace dummy
}

void translate_INVALID(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate INVALID_OP\n");
    invalidateReplacement(r_info, FE_DX, true);
    ///call error handler
    err |= fe_enc64(&current, FE_MOV64ri, FE_DI, (uint64_t) instr->reg_dest);
    err |= fe_enc64(&current, FE_MOV64ri, FE_SI, (uint64_t) instr->imm);
    err |= fe_enc64(&current, FE_MOV64ri, FE_DX, (uint64_t) instr->addr);
    err |= fe_enc64(&current, FE_AND64ri, FE_SP, 0xFFFFFFFFFFFFFFF0);   //16 byte align
    err |= fe_enc64(&current, FE_SUB64ri, FE_SP, 8); //these 8 byte + return addr from call = 16
    err |= fe_enc64(&current, FE_CALL, (uintptr_t) &invalid_error_handler);
}

