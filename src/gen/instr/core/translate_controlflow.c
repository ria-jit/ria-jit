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
translate_controlflow_cmp_rs1_rs2(const t_risc_instr *instr, const register_info *r_info);

//inline void translate_controlflow_set_pc(const t_risc_instr *instr, const register_info *r_info, Label END, Label NOJUMP);

static inline void
translate_controlflow_set_pc2(const t_risc_instr *instr, const register_info *r_info, uint8_t *jmpLoc, uint64_t mnem);


void translate_JAL(const t_risc_instr *instr, const register_info *r_info, const context_info *c_info) {
    log_asm_out("Translate JAL\n");

    ///push to return stack
    rs_emit_push(instr);

    /*
    if (flag_translate_opt && (instr->reg_dest == x1 || instr->reg_dest == x5)) {
        t_risc_addr ret_target = instr->addr + 4;

        t_cache_loc cache_loc;

        if ((cache_loc = lookup_cache_entry(ret_target)) == UNSEEN_CODE) {  // 1 ???
            printf("translate_JAL: flag_translate_op is enabled, but return target is not in cache! riscv: %p\n",
                   instr->addr);
            goto NOT_CACHED;
        }

        //asm rs_push   -----------
        err |= fe_enc64(&current, FE_MOV16rm, FE_AX, FE_MEM_ADDR((uint64_t) &rs_front));
        err |= fe_enc64(&current, FE_ADD64ri, FE_AX, 1);                                         //next field
        err |= fe_enc64(&current, FE_AND64ri, FE_AX, 0x3f);                                      //mod 64
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((uint64_t) &rs_front), FE_AX);    //save rs_front
        err |= fe_enc64(&current, FE_SHL64ri, FE_AX, 1);                                         //multiply by 2,struct array...
        err |= fe_enc64(&current, FE_MOV64rm, FE_BX, FE_MEM_ADDR((uint64_t) &r_stack));     //get base
        err |= fe_enc64(&current, FE_MOV64ri, FE_CX, instr->addr + 4);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(FE_BX, 8, FE_AX, 0), FE_CX);         //save risc adddr
        err |= fe_enc64(&current, FE_MOV64ri, FE_CX, (uintptr_t) cache_loc);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(FE_BX, 8, FE_AX, 8), FE_CX);         //save x86 addr
        err |= fe_enc64(&current, FE_SHR64ri, FE_AX, 1);                                         //revert multiplying by 2
        err |= fe_enc64(&current, FE_CMP64rm, FE_AX, FE_MEM_ADDR((uint64_t) &rs_back));     //front reached back?
        uint8_t *rs_full_jmp = current;
        err |= fe_enc64(&current, FE_JNZ, (intptr_t) current);                                   //dummy
        err |= fe_enc64(&current, FE_ADD64mi, FE_MEM_ADDR((uint64_t) &rs_back), 1);         //move back ahead
        err |= fe_enc64(&current, FE_AND64mi, FE_MEM_ADDR((uint64_t) &rs_back), 0x3f);      //mod 64
        err |= fe_enc64(&rs_full_jmp, FE_JNZ, (intptr_t) current);                               //replace dummy

        //------------------

        /*
        //emit c_info->save_context();
        err |= fe_enc64(&current, FE_CALL, (intptr_t) c_info->save_context);
        err |= fe_enc64(&current, FE_MOV64ri, FE_DI, instr->addr + 4);
        err |= fe_enc64(&current, FE_MOV64ri, FE_SI, cache_loc);
        err |= fe_enc64(&current, FE_CALL, &rs_push);
        //emit c_info->load_execute_save_context(*, false); //* means value does not matter, false means load without execute
        err |= fe_enc64(&current, FE_XOR32rr, FE_SI, FE_SI);
        err |= fe_enc64(&current, FE_CALL, (intptr_t) c_info->load_execute_save_context);
         // /

        NOT_CACHED:;
    }
    */

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

        ///set pc
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

void translate_JALR(const t_risc_instr *instr, const register_info *r_info, const context_info *c_info) {
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

        if(instr->reg_dest == x1 || instr->reg_dest == x5) {
            if(instr->reg_src_1 == x1 || instr->reg_src_1 == x5) {
                if(instr->reg_dest == instr->reg_src_1) {
                    ///push
                    rs_emit_push(instr);
                } else {
                    ///pop and push
                    rs_emit_pop_RAX();
                    rs_emit_push(instr);
                }
            } else {
                ///push
                //rs_emit_push(instr);  //segfault???
            }
        } else {
            if(instr->reg_src_1 == x1 || instr->reg_src_1 == x5) {
                ///pop
                rs_emit_pop_RAX();
            } else {
                ///none
            }
        }

        /*

        //RAX: target

        ///stack empty
        err |= fe_enc64(&current, FE_MOV64rm, FE_BX, FE_MEM_ADDR((intptr_t) &rs_front));    //get front
        err |= fe_enc64(&current, FE_CMP64rm, FE_BX, FE_MEM_ADDR((intptr_t) &rs_back));     //front == back?
        uint8_t *nullJMPempty = current;
        err |= fe_enc64(&current, FE_JZ, current);  //dummy

        //RAX: target
        //RBX: front

        ///hit
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_MEM_ADDR((intptr_t) &r_stack)); //get base
        err |= fe_enc64(&current, FE_SHL64ri, FE_BX, 1);
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_MEM(FE_CX, 8, FE_BX, 0)); //get risc addr from stack
        err |= fe_enc64(&current, FE_CMP64rr, FE_AX, FE_CX); //stack addr == target?
        uint8_t *nullJMPmiss = current;
        err |= fe_enc64(&current, FE_JNZ, current); //dummy
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_MEM_ADDR((intptr_t) &r_stack)); //get base  //DUPLICATE
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_MEM(FE_CX, 8, FE_BX, 8)); // load x86 target
        err |= fe_enc64(&current, FE_SHR64ri, FE_BX, 1);
        err |= fe_enc64(&current, FE_ADD64ri, FE_BX, 63); //-1 mod 64
        err |= fe_enc64(&current, FE_AND64ri, FE_BX, 0x3f); //-1 mod 64
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((intptr_t) &rs_front), FE_BX); //save rs_front
        err |= fe_enc64(&current, FE_JMPr, FE_CX); //jmp to nect block

        err |= fe_enc64(&nullJMPempty, FE_JZ, current);
        err |= fe_enc64(&nullJMPmiss, FE_JNZ, current);


        //*/

        /*
        //emit c_info->save_context();
        err |= fe_enc64(&current, FE_CALL, (intptr_t) c_info->save_context);



        err |= fe_enc64(&current, FE_MOV64rr, FE_DI, FE_AX);    //function argument
        err |= fe_enc64(&current, FE_PUSHr, FE_AX);             //save target address
        err |= fe_enc64(&current, FE_CALL, &rs_pop_check);      //call rs_easy pop
        err |= fe_enc64(&current, FE_PUSHr, FE_AX);             //save jump address


        //emit c_info->load_execute_save_context(*, false); //* means value does not matter, false means load without execute
        err |= fe_enc64(&current, FE_XOR32rr, FE_SI, FE_SI);
        err |= fe_enc64(&current, FE_CALL, (intptr_t) c_info->load_execute_save_context);


        err |= fe_enc64(&current, FE_POPr, FE_AX);
        err |= fe_enc64(&current, FE_CMP64ri, FE_AX, 0);        //cmp
        uint8_t *jmpLoc = current;
        err |= fe_enc64(&current, FE_JZ, current);              //dummy jump
        err |= fe_enc64(&current, FE_ADD64ri, FE_SP, 8);        //remove saved target address
        err |= fe_enc64(&current, FE_JMPr, FE_AX);              //jmp to next block (ret)
        err |= fe_enc64(&jmpLoc, FE_JZ, current);               //replace dummy



        //emit c_info->load_execute_save_context(*, false); //* means value does not matter, false means load without execute
        err |= fe_enc64(&current, FE_XOR32rr, FE_SI, FE_SI);
        err |= fe_enc64(&current, FE_CALL, (intptr_t) c_info->load_execute_save_context);
        err |= fe_enc64(&current, FE_POPr, FE_AX);              //restore saved target address
        //*/
     }


    ///3: prepare return to transcode loop

    ///write target addr to pc
    if (r_info->mapped[pc]) {
        //a->mov(r_info->map[pc], x86::rax);
        err |= fe_enc64(&current, FE_MOV64rr, r_info->map[pc], FE_AX);
    } else {
        //a->mov(x86::chainptr(r_info->base + 8 * pc), x86::rax);
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

    ///write chainEnd to be chained by chainer
    if (flag_translate_opt) {
        err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR((uint64_t) &chain_end), 0);
    }

}

void translate_BEQ(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate BRANCH BEQ\n");
    //does "The 12-bit B-immediate encodes signed offsets in multiples of 2" already account for the always-zero LSB????

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

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
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

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
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

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
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

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
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

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
    translate_controlflow_cmp_rs1_rs2(instr, r_info);

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
translate_controlflow_cmp_rs1_rs2(const t_risc_instr *instr, const register_info *r_info) {
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
        if (r_info->mapped[instr->reg_src_2]) {

            err |= fe_enc64(&current, FE_CMP64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1),
                            r_info->map[instr->reg_src_2]);

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

