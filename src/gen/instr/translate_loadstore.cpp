//
// Created by flo on 21.05.20.
//

#include "translate_loadstore.hpp"
#include "runtime/register.h"

#define FIRST_REG FE_AX
#define SECOND_REG FE_DX

/**
 * Translate the LB instruction.
 * The LB instruction loads a 8-bit value from memory and sign extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LB(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate LB…\n");

    if (r_info.mapped[instr.reg_dest] || r_info.mapped[instr.reg_src_1]) {
//        a->movsx(r_info.map[instr.reg_dest], x86::byte_ptr(r_info.map[instr.reg_src_1], instr.imm));
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->movsx(FIRST_REG, x86::byte_ptr(SECOND_REG, instr.imm));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), FIRST_REG);*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOVSXr64m8, FIRST_REG, FE_MEM(SECOND_REG, 0, 0, instr.imm));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FIRST_REG);
    }
}

/**
 * Translate the LH instruction.
 * The LH instruction loads a 16-bit value from memory and sign extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LH(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate LH…\n");

    if (r_info.mapped[instr.reg_dest] || r_info.mapped[instr.reg_src_1]) {
//        a->movsx(r_info.map[instr.reg_dest], x86::word_ptr(r_info.map[instr.reg_src_1], instr.imm));
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->movsx(FIRST_REG, x86::word_ptr(SECOND_REG, instr.imm));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), FIRST_REG);*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOVSXr64m16, SECOND_REG, FE_MEM(SECOND_REG, 0, 0, instr.imm));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FIRST_REG);
    }
}

/**
 * Translate the LW instruction.
 * The LW instruction loads a 32-bit value from memory and sign extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate LW…\n");

    if (r_info.mapped[instr.reg_dest] || r_info.mapped[instr.reg_src_1]) {
//        a->movsxd(r_info.map[instr.reg_dest], x86::dword_ptr(r_info.map[instr.reg_src_1], instr.imm));
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->movsxd(FIRST_REG, x86::dword_ptr(SECOND_REG, instr.imm));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), FIRST_REG);*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOVSXr64m32, SECOND_REG, FE_MEM(SECOND_REG, 0, 0, instr.imm));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FIRST_REG);
    }
}

/**
 * Translate the LBU instruction.
 * The LBU instruction loads a 8-bit value from memory and zero extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LBU(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate LBU…\n");

    if (r_info.mapped[instr.reg_dest] || r_info.mapped[instr.reg_src_1]) {
//        a->movzx(r_info.map[instr.reg_dest], x86::byte_ptr(r_info.map[instr.reg_src_1], instr.imm));
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->movzx(FIRST_REG, x86::byte_ptr(SECOND_REG, instr.imm));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), FIRST_REG);*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOVZXr64m8, FIRST_REG, FE_MEM(SECOND_REG, 0, 0, instr.imm));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FIRST_REG);
    }
}

/**
 * Translate the LHU instruction.
 * The LHU instruction loads a 16-bit value from memory and zero extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LHU(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate LHU…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1]) {
//        a->movzx(r_info.map[instr.reg_dest], x86::word_ptr(r_info.map[instr.reg_src_1], instr.imm));
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->movzx(FIRST_REG, x86::word_ptr(SECOND_REG, instr.imm));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), FIRST_REG);*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOVZXr64m16, SECOND_REG, FE_MEM(SECOND_REG, 0, 0, instr.imm));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FIRST_REG);
    }
}

/**
 * Translate the SB instruction.
 * The SB instruction stores the low 8 bits from rs2 into memory. The effective address is obtained by adding register
 * rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SB(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SB…\n");

    if (r_info.mapped[instr.reg_src_1] || r_info.mapped[instr.reg_src_2]) {
//        a->mov(x86::ptr(r_info.map[instr.reg_src_1], instr.imm), r_info.map[instr.reg_src_2].r8());
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->mov(FIRST_REG, x86::ptr(r_info.base + 8 * instr.reg_src_2));
        a->mov(x86::ptr(SECOND_REG, instr.imm), FIRST_REG.r8());*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FIRST_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV8mr, FE_MEM(SECOND_REG, 0, 0, instr.imm), FIRST_REG);
    }
}

/**
 * Translate the SH instruction.
 * The SH instruction stores the low 16 bits from rs2 into memory. The effective address is obtained by adding register
 * rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SH(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SH…\n");

    if (r_info.mapped[instr.reg_src_1] || r_info.mapped[instr.reg_src_2]) {
//        a->mov(x86::ptr(r_info.map[instr.reg_src_1], instr.imm), r_info.map[instr.reg_src_2].r16());
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->mov(FIRST_REG, x86::ptr(r_info.base + 8 * instr.reg_src_2));
        a->mov(x86::ptr(SECOND_REG, instr.imm), FIRST_REG.r16());*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FIRST_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV16mr, FE_MEM(SECOND_REG, 0, 0, instr.imm), FIRST_REG);
    }
}

/**
 * Translate the SW instruction.
 * The SH instruction stores the low 32 bits from rs2 into memory. The effective address is obtained by adding register
 * rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SW…\n");

    if (r_info.mapped[instr.reg_src_1] || r_info.mapped[instr.reg_src_2]) {
//        a->mov(x86::ptr(r_info.map[instr.reg_src_1], instr.imm), r_info.map[instr.reg_src_2].r32());
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->mov(FIRST_REG, x86::ptr(r_info.base + 8 * instr.reg_src_2));
        a->mov(x86::ptr(SECOND_REG, instr.imm), FIRST_REG.r32());*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FIRST_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV32mr, FE_MEM(SECOND_REG, 0, 0, instr.imm), FIRST_REG);
    }
}

/**
 * Translate the LWU instruction.
 * The LWU instruction loads a 32-bit value from memory and zero extends it before storing it in register rd. The
 * effective address is obtained by adding register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LWU(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate LWU…\n");

    ///All instructions with 32bit register targets on x86-64 automatically zero extend. Hence there is no movzx r64,
    /// r/m32. Instead you use mov r32, r/m32

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1]) {
//        a->mov(r_info.map[instr.reg_dest].r32(), x86::dword_ptr(r_info.map[instr.reg_src_1], instr.imm));
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->mov(FIRST_REG.r32(), x86::dword_ptr(SECOND_REG, instr.imm));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), FIRST_REG);*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV32rm, SECOND_REG, FE_MEM(SECOND_REG, 0, 0, instr.imm));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FIRST_REG);
    }
}

/**
 * Translate the LD instruction.
 * The LD instruction loads a 64-bit value from memory into register rd. The effective address is obtained by adding
 * register rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LD(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate LD…\n");

    if (r_info.mapped[instr.reg_dest] || r_info.mapped[instr.reg_src_1]) {
//        a->mov(r_info.map[instr.reg_dest], x86::qword_ptr(r_info.map[instr.reg_src_1], instr.imm));
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->mov(FIRST_REG, x86::qword_ptr(SECOND_REG, instr.imm));
        a->mov(x86::ptr(r_info.base + 8 * instr.reg_dest), FIRST_REG);*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM(SECOND_REG, 0, 0, instr.imm));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info.base + 8 * instr.reg_dest), FIRST_REG);
    }
}

/**
 * Translate the SD instruction.
 * The SD instruction stores the 64-bit value from rs2 into memory. The effective address is obtained by adding register
 * rs1 to the sign-extended 12-bit immediate.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SD(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SD…\n");

    if (r_info.mapped[instr.reg_src_1] || r_info.mapped[instr.reg_src_2]) {
//        a->mov(x86::ptr(r_info.map[instr.reg_src_1], instr.imm), r_info.map[instr.reg_src_2]);
        critical_not_yet_implemented("Register mapped instruction type unavailable\n");
    } else {
        /*a->mov(SECOND_REG, x86::ptr(r_info.base + 8 * instr.reg_src_1));
        a->mov(FIRST_REG, x86::ptr(r_info.base + 8 * instr.reg_src_2));
        a->mov(x86::ptr(SECOND_REG, instr.imm), FIRST_REG);*/
        err |= fe_enc64(&current, FE_MOV64rm, SECOND_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FIRST_REG, FE_MEM_ADDR(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(SECOND_REG, 0, 0, instr.imm), FIRST_REG);
    }
}