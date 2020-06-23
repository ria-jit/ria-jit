//
// Created by flo on 21.05.20.
//

#include "translate_controlflow.hpp"
#include "register.h"

using namespace asmjit;

inline void translate_controlflow_cmp_rs1_rs2(const t_risc_instr &instr, const register_info &r_info, bool noOrder);
inline void translate_controlflow_set_pc(const t_risc_instr &instr, const register_info &r_info, Label END, Label NOJUMP);


void translate_JAL(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate JAL should not ever be needed\n");
}

void translate_JALR(const t_risc_instr &instr, const register_info &r_info) {
    /**
     * The target address is obtained by adding the 12-bit signed I-immediate to the register rs1,
     * then setting the least-significant bit of the result to zero.
     * The address of the instruction following the jump (pc+4)is written to register rd.
     * Register x0 can be used as the destination if the result is not required.
     */
    log_verbose("Translate JALR\n");

    //assuming rax is unused, usage information will probably be added to r_info

    ///mov rs1 to temp register
    if (r_info.mapped[instr.reg_src_1]) {
        a->mov(x86::rax, r_info.map[instr.reg_src_1]);
    } else {
        a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
    }

    ///add immediate to rs1
    a->add(x86::rax, instr.imm);

    ///set last bit to zero
    //why??? not aligned to 4 bit boundary would throw exception anyway…
    a->and_(x86::rax, -2);

    ///write target addr to pc
    if (r_info.mapped[t_risc_reg::pc]) {
        a->mov(r_info.map[pc], x86::rax);
    } else {
        a->mov(x86::ptr(r_info.base + 8 * t_risc_reg::pc), x86::rax);
    }

    ///write addr of next instruction in rd
    if (instr.reg_dest != t_risc_reg::x0) {
        if (r_info.mapped[instr.reg_dest]) {
            a->mov(r_info.map[instr.reg_dest], instr.addr + 4);
        } else {
            a->mov(x86::qword_ptr(r_info.base + 8 * instr.reg_dest), instr.addr + 4);
        }
    }

}

void translate_BEQ(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate BRANCH\n");
    //does "The 12-bit B-immediate encodes signed offsets in multiples of 2" already account for the always-zero LSB????

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, true);

    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->jne(NOJUMP);

    ///set pc
    translate_controlflow_set_pc(instr, r_info, END, NOJUMP);
}

void translate_BNE(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate BRANCH\n");
    //does "The 12-bit B-immediate encodes signed offsets in multiples of 2" already account for the always-zero LSB????

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, true);

    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->je(NOJUMP);

    ///set pc
    translate_controlflow_set_pc(instr, r_info, END, NOJUMP);
}

void translate_BLT(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate BRANCH\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, false);

    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->jl(NOJUMP);

    ///set pc
    translate_controlflow_set_pc(instr, r_info, END, NOJUMP);
}

void translate_BLTU(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate BRANCH\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, false);

    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->jb(NOJUMP);

    ///set pc
    translate_controlflow_set_pc(instr, r_info, END, NOJUMP);
}

void translate_BGE(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate BRANCH\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, false);

    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->jge(NOJUMP);

    ///set pc
    translate_controlflow_set_pc(instr, r_info, END, NOJUMP);
}

void translate_BGEU(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate BRANCH\n");

    ///compare registers:
    translate_controlflow_cmp_rs1_rs2(instr, r_info, false);

    ///"jump":
    Label END = a->newLabel();
    Label NOJUMP = a-> newLabel();

    ///skip setting pc if !branch
    a->jae(NOJUMP);

    ///set pc
    translate_controlflow_set_pc(instr, r_info, END, NOJUMP);
}



inline void translate_controlflow_cmp_rs1_rs2(const t_risc_instr& instr, const register_info &r_info, bool noOrder) {
    ///compare registers:

    ///rs1 mapped?
    if(r_info.mapped[instr.reg_src_1]) {
        ///everything mapped
        if (r_info.mapped[instr.reg_src_2]) {
            a->cmp(r_info.map[instr.reg_src_1], r_info.map[instr.reg_src_2]);
        }
            ///else get rs2 from mem
        else {
            a->cmp(r_info.map[instr.reg_src_1], x86::ptr(r_info.base + 8 * instr.reg_src_2));
        }
    }
    else {
        ///rs2 mapped && order of compare doesn't matter -> get rs1 from mem
        if(r_info.mapped[instr.reg_src_2] && noOrder) {
            a->cmp(r_info.map[instr.reg_src_2], x86::ptr(r_info.base + 8 * instr.reg_src_1));
        }
            ///else get both from mem, rs1 in temp register
        else {
            a->mov(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_1));
            a->cmp(x86::rax, x86::ptr(r_info.base + 8 * instr.reg_src_2));
        }
    }
}

inline void translate_controlflow_set_pc(const t_risc_instr &instr, const register_info &r_info, Label END, Label NOJUMP) {
    ///set pc: BRANCH
    //afaik the "multiples of two" thing is resolved in parser.c
    if(r_info.mapped[t_risc_reg::pc]) {
        a->mov(r_info.map[t_risc_reg::pc], (instr.addr + ((int64_t)(instr.imm)))); //cast to sign extend
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