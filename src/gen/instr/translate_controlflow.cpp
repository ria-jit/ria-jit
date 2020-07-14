//
// Created by flo on 21.05.20.
//

#include "translate_controlflow.hpp"
#include "runtime/register.h"
#include "translate_arithmetic.hpp"
#include "util/log.h"
#include <fadec/fadec-enc.h>

using namespace asmjit;

inline void translate_controlflow_cmp_rs1_rs2(const t_risc_instr &instr, const register_info &r_info, bool noOrder);
//inline void translate_controlflow_set_pc(const t_risc_instr &instr, const register_info &r_info, Label END, Label NOJUMP);
inline void translate_controlflow_set_pc2(const t_risc_instr &instr, const register_info &r_info, uint8_t *jmpLoc, uint64_t mnem);


void translate_JAL(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate JAL\n");

    t_risc_instr aupicInstr = t_risc_instr{
            instr.addr,
            AUIPC,
            IMMEDIATE,
            x0,
            x0,
            instr.reg_dest,
            4
    };

    translate_AUIPC(aupicInstr, r_info);

    t_risc_addr target = instr.addr + instr.imm;

    t_cache_loc cache_loc = lookup_cache_entry(target);

    if(cache_loc == UNSEEN_CODE || !flag_translate_opt) {
        //afaik the "multiples of two" thing is resolved in parser.c
        if (r_info.mapped[t_risc_reg::pc]) {
            //a->mov(r_info.map[t_risc_reg::pc], (instr.addr + ((int64_t) (instr.imm)))); //cast to sign extend
            err |= fe_enc64(&current, FE_MOV64ri, r_info.map[t_risc_reg::pc], (instr.addr + ((int64_t) (instr.imm)))); //cast to sign extend
        } else {
            //a->mov(x86::qword_ptr(r_info.base + 8 * t_risc_reg::pc), (instr.addr + ((int64_t) (instr.imm)))); //cast to sign extend
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info.base + 8 * t_risc_reg::pc), (instr.addr + ((int64_t) (instr.imm)))); //cast to sign extend
        }
    }
    else {
        log_asm_out("DIRECT JUMP\n");
        //a->jmp((uint64_t)cache_loc);
        err |= fe_enc64(&current, FE_JMP, (intptr_t) cache_loc);
    }
}

void translate_JALR(const t_risc_instr &instr, const register_info &r_info) {
    /**
     * The target address is obtained by adding the 12-bit signed I-immediate to the register rs1,
     * then setting the least-significant bit of the result to zero.
     * The address of the instruction following the jump (pc+4)is written to register rd.
     * Register x0 can be used as the destination if the result is not required.
     */
    log_asm_out("Translate JALR\n");

    //assuming rax is unused, usage information will probably be added to r_info

    ///mov rs1 to temp register
    if (r_info.mapped[instr.reg_src_1]) {
        //a->mov(x86::rax, r_info.map[instr.reg_src_1]);
        err |= fe_enc64(&current, FE_MOV64rr, FE_AX, r_info.map[instr.reg_src_1]);
    } else {
        //a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
    }

    ///add immediate to rs1
    //a->add(x86::rax, instr.imm);
    err |= fe_enc64(&current, FE_ADD64ri, FE_AX, instr.imm);

    ///set last bit to zero
    //why??? not aligned to 4 bit boundary would throw exception anyway…
    //a->and_(x86::rax, -2);
    err |= fe_enc64(&current, FE_AND64ri, -2);

    ///write target addr to pc
    if (r_info.mapped[t_risc_reg::pc]) {
        //a->mov(r_info.map[pc], x86::rax);
        err |= fe_enc64(&current, FE_MOV64rr, r_info.map[t_risc_reg::pc], FE_AX);
    } else {
        //a->mov(x86::ptr(r_info.base + 8 * t_risc_reg::pc), x86::rax);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * t_risc_reg::pc), FE_AX);
    }

    ///write addr of next instruction in rd
    if (instr.reg_dest != t_risc_reg::x0) {
        if (r_info.mapped[instr.reg_dest]) {
            //a->mov(r_info.map[instr.reg_dest], instr.addr + 4);
            err |= fe_enc64(&current, FE_MOV64ri, r_info.map[instr.reg_dest], instr.addr + 4);
        } else {
            //a->mov(x86::qword_ptr(r_info.base + 8 * instr.reg_dest), instr.addr + 4);
            err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), instr.addr + 4);
        }
    }

}

void translate_BEQ(const t_risc_instr &instr, const register_info &r_info) {
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

void translate_BNE(const t_risc_instr &instr, const register_info &r_info) {
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

void translate_BLT(const t_risc_instr &instr, const register_info &r_info) {
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

void translate_BLTU(const t_risc_instr &instr, const register_info &r_info) {
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

void translate_BGE(const t_risc_instr &instr, const register_info &r_info) {
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

void translate_BGEU(const t_risc_instr &instr, const register_info &r_info) {
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



inline void translate_controlflow_cmp_rs1_rs2(const t_risc_instr& instr, const register_info &r_info, bool noOrder) {
    ///compare registers:

    ///rs1 mapped?
    if(r_info.mapped[instr.reg_src_1]) {
        ///everything mapped
        if (r_info.mapped[instr.reg_src_2]) {
            //a->cmp(r_info.map[instr.reg_src_1], r_info.map[instr.reg_src_2]);
            err |= fe_enc64(&current, FE_CMP64rr, r_info.map[instr.reg_src_1], r_info.map[instr.reg_src_2]);
        }
            ///else get rs2 from mem
        else {
            //a->cmp(r_info.map[instr.reg_src_1], x86::ptr(r_info.base + 8 * instr.reg_src_2));
            err |= fe_enc64(&current, FE_CMP64rm, r_info.map[instr.reg_src_1], FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        }
    }
    else {
        ///rs2 mapped && order of compare doesn't matter -> get rs1 from mem
        if(r_info.mapped[instr.reg_src_2] && noOrder) {
            //a->cmp(r_info.map[instr.reg_src_2], x86::ptr(r_info.base + 8 * instr.reg_src_1));
            err |= fe_enc64(&current, FE_CMP64rr, r_info.map[instr.reg_src_2], FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        }
            ///else get both from mem, rs1 in temp register
        else {
            //a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
            //a->cmp(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_2));
            err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
            err |= fe_enc64(&current, FE_CMP64rm, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        }
    }
}

/*
inline void translate_controlflow_set_pc(const t_risc_instr &instr, const register_info &r_info, Label END, Label NOJUMP) {
    ///set pc: BRANCH
    //afaik the "multiples of two" thing is resolved in parser.c
    if(r_info.mapped[t_risc_reg::pc]) {
        //a->mov(r_info.map[t_risc_reg::pc], (instr.addr + ((int64_t)(instr.imm)))); //cast to sign extend
    }
    else {
        a->mov(x86::qword_ptr(r_info.base + 8 * t_risc_reg::pc), (instr.addr + ((int64_t)(instr.imm)))); //cast to sign extend
    }

    a->jmp(END);
    a->bind(NOJUMP);

    ///set pc: NO BRANCH
    if(r_info.mapped[t_risc_reg::pc]) {
        a->mov(r_info.map[t_risc_reg::pc], (instr.addr + 4)); //add 4 for next instr
    }
    else {
        a->mov(x86::qword_ptr(r_info.base + 8 * t_risc_reg::pc), (instr.addr + 4)); //add 4 for next instr
    }

    a->bind(END);
}
*/

inline void translate_controlflow_set_pc2(const t_risc_instr &instr, const register_info &r_info, uint8_t *noJmpLoc, uint64_t jmpMnem) {
    ///set pc: BRANCH
    //afaik the "multiples of two" thing is resolved in parser.c
    if(r_info.mapped[t_risc_reg::pc]) {
        //a->mov(r_info.map[t_risc_reg::pc], (instr.addr + ((int64_t)(instr.imm)))); //cast to sign extend
        err |= fe_enc64(&current, FE_MOV64ri, r_info.map[t_risc_reg::pc], (instr.addr + ((int64_t)(instr.imm))));
    }
    else {
        //a->mov(x86::qword_ptr(r_info.base + 8 * t_risc_reg::pc), (instr.addr + ((int64_t)(instr.imm)))); //cast to sign extend
        err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info.base + 8 * t_risc_reg::pc), (instr.addr + ((int64_t)(instr.imm))));
    }

    //a->jmp(END);
    uint8_t *endJmpLoc = current;
    err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy jump

    //a->bind(NOJUMP);
    err |= fe_enc64(&noJmpLoc, jmpMnem, (intptr_t) current); //replace dummy

    ///set pc: NO BRANCH
    if(r_info.mapped[t_risc_reg::pc]) {
        //a->mov(r_info.map[t_risc_reg::pc], (instr.addr + 4)); //add 4 for next instr
        err |= fe_enc64(&current, FE_MOV64ri, r_info.map[t_risc_reg::pc], (instr.addr + 4));
    }
    else {
        //a->mov(x86::qword_ptr(r_info.base + 8 * t_risc_reg::pc), (instr.addr + 4)); //add 4 for next instr
        err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info.base + 8 * t_risc_reg::pc), (instr.addr + 4));
    }

    //a->bind(END);
    err |= fe_enc64(&endJmpLoc, FE_JMP, (intptr_t) current); //replace dummy
}
