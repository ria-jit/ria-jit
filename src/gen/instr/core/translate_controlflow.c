//
// Created by flo on 21.05.20.
//

#include "translate_controlflow.h"
#include <gen/instr/core/translate_arithmetic.h>
#include <util/log.h>
#include <fadec/fadec-enc.h>
#include <cache/return_stack.h>
#include <common.h>

static inline void
translate_controlflow_cmp_rs1_rs2(const t_risc_instr *instr, const register_info *r_info, bool noOrder);

//inline void translate_controlflow_set_pc(const t_risc_instr *instr, const register_info *r_info, Label END, Label NOJUMP);

static inline void
translate_controlflow_set_pc2(const t_risc_instr *instr, const register_info *r_info, uint8_t *jmpLoc, uint64_t mnem);


void translate_JAL(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate JAL\n");

    ///push to return stack
    if(flag_translate_opt) {
        t_risc_addr ret_target = instr->addr + 4;

        t_cache_loc cache_loc;

        if ((cache_loc = lookup_cache_entry(ret_target)) == UNSEEN_CODE) {
            printf("translate_JAL: flag_translate_op is enabled, but return target is not in cache! riscv: %p\n", instr->addr);
            goto NOT_CACHED;
        }

        err |= fe_enc64(&current, FE_MOV64ri, FE_DI, instr->addr + 4);
        err |= fe_enc64(&current, FE_MOV64ri, FE_SI, cache_loc);
        err |= fe_enc64(&current, FE_CALL, &rs_push);

        NOT_CACHED:;
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

    if (!flag_translate_opt || (cache_loc = lookup_cache_entry(target)) == UNSEEN_CODE ||
            cache_loc == (t_cache_loc) 1) {
        //afaik the "multiples of two" thing is resolved in parser.c

        ///write chainEnd to be chained by chainer
        if (flag_translate_opt) {
            err |= fe_enc64(&current, FE_LEA64rm, FE_AX, FE_MEM(FE_IP, 0, 0, 0));
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((uint64_t) &chain_end), FE_AX);
        }

        if (r_info->mapped[pc]) {
            //a->mov(r_info->map[pc], (instr->addr + ((int64_t) (instr->imm)))); //cast to sign extend
            err |= fe_enc64(&current, FE_MOV64ri, r_info->map[pc],
                            instr->addr + (int64_t) instr->imm); //cast to sign extend
        } else {
            //a->mov(x86::qword_ptr(r_info->base + 8 * pc), (instr->addr + ((int64_t) (instr->imm)))); //cast to sign extend
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * pc),
                            instr->addr + (int64_t) instr->imm); //cast to sign extend
        }
    } else {
        log_asm_out("DIRECT JUMP JAL\n");
        //a->jmp((uint64_t)cache_loc);
        err |= fe_enc64(&current, FE_JMP, (intptr_t) cache_loc);
    }
}

void translate_JALR(const t_risc_instr *instr, const register_info *r_info) {
    /**
     * The target address is obtained by adding the 12-bit signed I-immediate to the register rs1,
     * then setting the least-significant bit of the result to zero.
     * The address of the instruction following the jump (pc+4)is written to register rd.
     * Register x0 can be used as the destination if the result is not required.
     */
    log_asm_out("Translate JALR\n");

    //assuming rax is unused, usage information will probably be added to r_info

    ///1: compute target address

    ///mov rs1 to temp register
    if (r_info->mapped[instr->reg_src_1]) {
        //a->mov(x86::rax, r_info->map[instr->reg_src_1]);
        err |= fe_enc64(&current, FE_MOV64rr, FE_AX, r_info->map[instr->reg_src_1]);
    } else {
        //a->mov(x86::rax, x86::ptr(r_info->base + 8 * instr->reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    }

    ///add immediate to rs1
    //a->add(x86::rax, instr->imm);
    err |= fe_enc64(&current, FE_ADD64ri, FE_AX, instr->imm);

    ///set last bit to zero
    //why??? not aligned to 4 bit boundary would throw exception anyway…
    //a->and_(x86::rax, -2);
    err |= fe_enc64(&current, FE_AND64ri, FE_AX, -2);


    ///2: check return stack

    if(flag_translate_opt) {
        err |= fe_enc64(&current, FE_MOV64rr, FE_DI, FE_AX);    //function argument
        err |= fe_enc64(&current, FE_PUSHr, FE_AX);             //save target address
        err |= fe_enc64(&current, FE_CALL, &rs_pop_check);       //call rs_easy pop
        err |= fe_enc64(&current, FE_CMP64ri, FE_AX, 0);        //cmp
        uint8_t *jmpLoc = current;
        err |= fe_enc64(&current, FE_JZ, current);              //dummy jump
        err |= fe_enc64(&current, FE_ADD64ri, FE_SP, 8);        //remove saved target address
        err |= fe_enc64(&current, FE_JMPr, FE_AX);              //jmp to next block (ret)
        err |= fe_enc64(&jmpLoc, FE_JZ, current);               //replace dummy
        err |= fe_enc64(&current, FE_POPr, FE_AX);              //restore saved target address
    }


    ///3: prepare return to transcode loop

    ///write target addr to pc
    if (r_info->mapped[pc]) {
        //a->mov(r_info->map[pc], x86::rax);
        err |= fe_enc64(&current, FE_MOV64rr, r_info->map[pc], FE_AX);
    } else {
        //a->mov(x86::ptr(r_info->base + 8 * pc), x86::rax);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * pc), FE_AX);
    }

    ///write addr of next instruction in rd
    if (instr->reg_dest != x0) {
        if (r_info->mapped[instr->reg_dest]) {
            //a->mov(r_info->map[instr->reg_dest], instr->addr + 4);
            err |= fe_enc64(&current, FE_MOV64ri, r_info->map[instr->reg_dest], instr->addr + 4);
        } else {
            //a->mov(x86::qword_ptr(r_info->base + 8 * instr->reg_dest), instr->addr + 4);
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), instr->addr + 4);
        }
    }

}

void translate_BEQ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BEQ\n");
    //does "The 12-bit B-immediate encodes signed offsets in multiples of 2" already account for the always-zero LSB????

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, true);

    /*
    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->jne(NOJUMP);
     */

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
    translate_controlflow_cmp_rs1_rs2(instr, r_info, true);

    /*
    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->je(NOJUMP);
    */

    ///dummy jump
    uint8_t *jump_b = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_b, FE_JZ);
}

void translate_BLT(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BLT\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, false);

    /*
    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->jge(NOJUMP);
    */

    ///dummy jump
    uint8_t *jump_b = current;
    err |= fe_enc64(&current, FE_JGE, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_b, FE_JGE);
}

void translate_BLTU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BLTU\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, false);

    /*
    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->jae(NOJUMP);
    */

    ///dummy jump
    uint8_t *jump_b = current;
    err |= fe_enc64(&current, FE_JNC, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_b, FE_JNC);
}

void translate_BGE(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BGE\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, false);

    /*
    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->jl(NOJUMP);
    */

    ///dummy jump
    uint8_t *jump_l = current;
    err |= fe_enc64(&current, FE_JL, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_l, FE_JL);
}

void translate_BGEU(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BGEU\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, false);

    /*
    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->jb(NOJUMP);
    */

    ///dummy jump
    uint8_t *jump_b = current;
    err |= fe_enc64(&current, FE_JC, (intptr_t) current);

    ///set pc
    translate_controlflow_set_pc2(instr, r_info, jump_b, FE_JC);
}


static inline void
translate_controlflow_cmp_rs1_rs2(const t_risc_instr *instr, const register_info *r_info, bool noOrder) {
    ///compare registers:

    ///rs1 mapped?
    if (r_info->mapped[instr->reg_src_1]) {
        ///everything mapped
        if (r_info->mapped[instr->reg_src_2]) {
            //a->cmp(r_info->map[instr->reg_src_1], r_info->map[instr->reg_src_2]);
            err |= fe_enc64(&current, FE_CMP64rr, r_info->map[instr->reg_src_1], r_info->map[instr->reg_src_2]);
        }
            ///else get rs2 from mem
        else {
            //a->cmp(r_info->map[instr->reg_src_1], x86::ptr(r_info->base + 8 * instr->reg_src_2));
            err |= fe_enc64(&current, FE_CMP64rm, r_info->map[instr->reg_src_1],
                            FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));
        }
    } else {
        ///rs2 mapped && order of compare doesn't matter -> get rs1 from mem
        if (r_info->mapped[instr->reg_src_2] && noOrder) {
            //a->cmp(r_info->map[instr->reg_src_2], x86::ptr(r_info->base + 8 * instr->reg_src_1));
            err |= fe_enc64(&current, FE_CMP64rm, r_info->map[instr->reg_src_2],
                            FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
        }
            ///else get both from mem, rs1 in temp register
        else {
            //a->mov(x86::rax, x86::ptr(r_info->base + 8 * instr->reg_src_1));
            //a->cmp(x86::rax, x86::ptr(r_info->base + 8 * instr->reg_src_2));
            err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
            err |= fe_enc64(&current, FE_CMP64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));
        }
    }
}

/*
inline void translate_controlflow_set_pc(const t_risc_instr *instr, const register_info *r_info, Label END, Label NOJUMP) {
    ///set pc: BRANCH
    //afaik the "multiples of two" thing is resolved in parser.c
    if(r_info->mapped[pc]) {
        //a->mov(r_info->map[pc], (instr->addr + ((int64_t)(instr->imm)))); //cast to sign extend
    }
    else {
        a->mov(x86::qword_ptr(r_info->base + 8 * pc), (instr->addr + ((int64_t)(instr->imm)))); //cast to sign extend
    }

    a->jmp(END);
    a->bind(NOJUMP);

    ///set pc: NO BRANCH
    if(r_info->mapped[pc]) {
        a->mov(r_info->map[pc], (instr->addr + 4)); //add 4 for next instr
    }
    else {
        a->mov(x86::qword_ptr(r_info->base + 8 * pc), (instr->addr + 4)); //add 4 for next instr
    }

    a->bind(END);
}
*/

static inline void
translate_controlflow_set_pc2(const t_risc_instr *instr, const register_info *r_info, uint8_t *noJmpLoc,
                              uint64_t jmpMnem) {
    ///set pc: BRANCH
    t_risc_addr target = instr->addr + instr->imm;
    t_cache_loc cache_loc;
    if (flag_translate_opt && (cache_loc = lookup_cache_entry(target)) != UNSEEN_CODE && cache_loc != (t_cache_loc) 1) {
        log_asm_out("DIRECT JUMP BRANCH 1\n");
        err |= fe_enc64(&current, FE_JMP, (intptr_t) cache_loc);
    } else {
        ///write chainEnd to be chained by chainer
        if (flag_translate_opt) {
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

    //a->jmp(END);
    uint8_t *endJmpLoc = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jump

    //a->bind(NOJUMP);
    err |= fe_enc64(&noJmpLoc, jmpMnem, (intptr_t) current); //replace dummy

    ///set pc: NO BRANCH
    target = instr->addr + 4;
    if (flag_translate_opt && (cache_loc = lookup_cache_entry(target)) != UNSEEN_CODE && cache_loc != (t_cache_loc) 1) {
        log_asm_out("DIRECT JUMP BRANCH 2\n");
        err |= fe_enc64(&current, FE_JMP, (intptr_t) cache_loc);
    } else {
        ///write chainEnd to be chained by chainer
        if (flag_translate_opt) {
            err |= fe_enc64(&current, FE_LEA64rm, FE_AX, FE_MEM(FE_IP, 0, 0, 0));
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((uint64_t) &chain_end), FE_AX);
        }

        if (r_info->mapped[pc]) {
            //a->mov(r_info->map[pc], (instr->addr + 4)); //add 4 for next instr
            err |= fe_enc64(&current, FE_MOV64ri, r_info->map[pc], instr->addr + 4);
        } else {
            //a->mov(x86::qword_ptr(r_info->base + 8 * pc), (instr->addr + 4)); //add 4 for next instr
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * pc), instr->addr + 4);
        }
    }

    //a->bind(END);
    err |= fe_enc64(&endJmpLoc, FE_JMP, (intptr_t) current); //replace dummy
}

void translate_INVALID(const t_risc_instr *instr) {
    log_asm_out("Translate INVALID_OP\n");
    ///call error handler
    err |= fe_enc64(&current, FE_MOV64ri, FE_DI, (uint64_t) instr->reg_dest);
    err |= fe_enc64(&current, FE_MOV64ri, FE_SI, (uint64_t) instr->imm);
    err |= fe_enc64(&current, FE_MOV64ri, FE_DX, (uint64_t) instr->addr);
    err |= fe_enc64(&current, FE_AND64ri, FE_SP, 0xFFFFFFFFFFFFFFF0);   //16 byte align
    err |= fe_enc64(&current, FE_SUB64ri, FE_SP, 8); //these 8 byte + return addr from call = 16
    err |= fe_enc64(&current, FE_CALL, (uintptr_t) &invalid_error_handler);
}

