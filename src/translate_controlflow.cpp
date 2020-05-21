//
// Created by flo on 21.05.20.
//

#include "translate_controlflow.hpp"
#include <iostream>
#include "register.h"

using namespace asmjit;

/**
 * The following instructions return to the binary translator after writing pc
 */
void translate_JAL(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate JAL should not ever be needed" << std::endl;
}

void translate_JALR(t_risc_instr instr, register_info &r_info) {
    /**
     * The target address is obtained by adding the 12-bit signed I-immediate to the register rs1,
     * then setting the least-significant bit of the result to zero.
     * The address of the instruction following the jump (pc+4)is written to register rd.
     * Register x0 can be used as the destination if the result is not required.
     */
    std::cout << "Translate JALR" << std::endl;

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
    //why??? not aligned to 4 bit boundary would throw exception anyway...
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
            a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), instr.addr + 4);
        }
    }

}

void translate_BEQ(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate BRANCH" << std::endl;
}

void translate_BNE(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate BRANCH" << std::endl;
}

void translate_BLT(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate BRANCH" << std::endl;
}

void translate_BGE(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate BRANCH" << std::endl;
}

void translate_BLTU(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate BRANCH" << std::endl;
}

void translate_BGEU(t_risc_instr instr, register_info r_info) {
    std::cout << "Translate BRANCH" << std::endl;
}