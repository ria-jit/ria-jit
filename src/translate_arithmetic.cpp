//
// Created by flo on 21.05.20.
//

#include "translate_arithmetic.hpp"
#include "register.h"

using namespace asmjit;

/**
 * ADDIW adds the sign-extended 12-bit immediate to register rs1 and produces the
 * proper sign-extension of a 32-bit result in rd.
 * @param instr
 */
void translate_addiw(const t_risc_instr &instr, const register_info &r_info) {
    // mov rd, rs1
    // add rd, instr.imm
    log_verbose("Translate addiw...\n");
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->add(x86::edx, instr.imm);
        a->movsx(r_info.map[instr.reg_dest], x86::edx);
    } else {
        a->mov(x86::edx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->add(x86::edx, instr.imm);
        a->movsx(x86::rax, x86::edx);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
 * SLLI is a logical left shift.
 * The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
 * @param instr
 */
void translate_slli(const t_risc_instr &instr, const register_info &r_info) {
    //mov rd, rs1
    //shl rd, (instr.imm & 0x3F)
    log_verbose("Translate slli...\n");
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->shl(r_info.map[instr.reg_dest], instr.imm & 0b111111);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->shl(x86::rax, instr.imm & 0b111111);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
 * LUI places the 20-bit U-immediate into bits 31-12 of register rd and places zero in the lowest 12 bits.
 * The 32-bit result is sign-extended to 64 bits.
 * @param instr
 */
void translate_lui(const t_risc_instr &instr, const register_info &r_info) {
    //mov rd, extended
    log_verbose("Translate lui...\n");

    //move into register
    if (r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], instr.imm);
    } else {
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), instr.imm);
    }
}

/**
 * ADDI adds the sign-extended 12-bit immediate to register rs1.
 * Overflow is ignored and the result is the low (in our case) 64 bit of the result.
 * The result is stored in rd.
 * @param instr
 */
void translate_addi(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate addi...\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->add(r_info.map[instr.reg_dest], instr.imm);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->add(x86::rax, instr.imm);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the AUIPC instruction.
* AUIPC (add upper immediate to pc) is used to build pc-relative addresses and uses the U-type
* format. AUIPC forms a 32-bit offset from the 20-bit U-immediate, filling in the lowest 12 bits with
* zeros, adds this offset to the address of the AUIPC instruction, then places the result in register
* rd.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_AUIPC(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate AUIPC…\n");

    //TODO Change to use instr.addr instead of pc as pc is only updated on end of basic block AFAIK.
    //add offset to the pc and store in rd
    if (r_info.mapped[instr.reg_dest] && r_info.mapped[pc]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[pc]);
        a->add(r_info.map[instr.reg_dest], instr.imm);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * pc));
        a->add(x86::rax, instr.imm);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SLTI instruction.
* SLTI (set less than immediate) places the value 1 in register rd if register rs1 is less than the sign-
* extended 12-bit immediate when both are treated as signed numbers, else 0 is written to rd.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLTI(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SLTI…\n");

    const Label &not_less = a->newLabel();

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->xor_(r_info.map[instr.reg_dest], r_info.map[instr.reg_dest]);
        a->cmp(r_info.map[instr.reg_src_1], instr.imm);
        a->jnl(not_less);
        a->inc(r_info.map[instr.reg_dest]);
        a->bind(not_less);
    } else {
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), 0);
        a->cmp(x86::qword_ptr(r_info.base + 8 * instr.reg_src_1), instr.imm);
        a->jnl(not_less);
        a->inc(x86::qword_ptr(r_info.base + 8 * instr.reg_dest));
        a->bind(not_less);
    }
}

/**
* Translate the SLTIU instruction.
* SLTIU (set less than immediate unsigned) places the value 1 in register rd if register rs1 is less than the sign-
* extended 12-bit immediate when both are treated as unsigned numbers, else 0 is written to rd.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLTIU(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SLTIU…\n");

    const Label &not_below = a->newLabel();

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->xor_(r_info.map[instr.reg_dest], r_info.map[instr.reg_dest]);
        a->cmp(r_info.map[instr.reg_src_1], instr.imm);
        a->jnb(not_below);
        a->inc(r_info.map[instr.reg_dest]);
        a->bind(not_below);
    } else {
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), 0);
        a->cmp(x86::qword_ptr(r_info.base + 8 * instr.reg_src_1), instr.imm);
        a->jnb(not_below);
        a->inc(x86::qword_ptr(r_info.base + 8 * instr.reg_src_1));
        a->bind(not_below);
    }
}

/**
* Translate the XORI instruction.
* Performs a bitwise XOR on the register rs1 and the sign-extended 12-bit immediate
* and places the result into rd.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_XORI(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate XORI…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], r_info.mapped[instr.reg_src_1]);
        a->xor_(r_info.map[instr.reg_dest], instr.imm);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->xor_(x86::rax, instr.imm);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the ORI instruction.
* Performs a bitwise OR on the register rs1 and the sign-extended 12-bit immediate
* and places the result into rd.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ORI(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate ORI…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], r_info.mapped[instr.reg_src_1]);
        a->or_(r_info.map[instr.reg_dest], instr.imm);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->or_(x86::rax, instr.imm);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the ANDI instruction.
* Performs a bitwise AND on the register rs1 and the sign-extended 12-bit immediate
* and places the result into rd.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ANDI(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate ANDI…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], r_info.mapped[instr.reg_src_1]);
        a->and_(r_info.map[instr.reg_dest], instr.imm);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->and_(x86::rax, instr.imm);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SRLI instruction. SRLI is a logical right shift.
* The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRLI(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SRLI…\n");
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->shr(r_info.map[instr.reg_dest], instr.imm & 0b111111);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->shr(x86::rax, instr.imm & 0b111111);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SRAI instruction. SRAI is an arithmetic right shift.
* The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRAI(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SRAI…\n");
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->sar(r_info.map[instr.reg_dest], instr.imm & 0b111111);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->sar(x86::rax, instr.imm & 0b111111);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the ADD instruction.
* ADD performs the addition of rs1 and rs2. Overflows
* are ignored and the low XLEN bits of results are written to the destination rd.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ADD(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate ADD…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->add(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->add(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SUB instruction.
* SUB performs the subtraction of rs2 from rs1. Overflows
* are ignored and the low XLEN bits of results are written to the destination rd.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SUB(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SUB…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->sub(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->sub(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SLL instruction.
* SLL, SRL, and SRA perform logical left, logical right, and arithmetic right shifts on the value in
* register rs1 by the shift amount held in the lower 5 bits of register rs2.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLL(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SLL…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->shl(r_info.map[instr.reg_dest], x86::cl);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->mov(x86::rcx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->and_(x86::rcx, 0b11111);
        a->shl(x86::rax, x86::cl);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SLT instruction.
* SLT and SLTU perform signed and unsigned compares respectively, writing 1 to rd if rs1 < rs2, 0 otherwise.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLT(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SLT…\n");

    const Label &not_less = a->newLabel();

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->xor_(r_info.map[instr.reg_dest], r_info.map[instr.reg_dest]);
        a->cmp(r_info.map[instr.reg_src_1], r_info.map[instr.reg_src_2]);
        a->jnl(not_less);
        a->inc(r_info.map[instr.reg_dest]);
        a->bind(not_less);
    } else {
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), 0);
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->cmp(x86::qword_ptr(r_info.base + 8 * instr.reg_src_1), x86::rax);
        a->jnl(not_less);
        a->inc(x86::qword_ptr(r_info.base + 8 * instr.reg_dest));
        a->bind(not_less);
    }
}

/**
* Translate the SLTU instruction.
* SLT and SLTU perform signed and unsigned compares respectively, writing 1 to rd if rs1 < rs2, 0 otherwise.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLTU(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SLTU…\n");

    const Label &not_below = a->newLabel();

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->xor_(r_info.map[instr.reg_dest], r_info.map[instr.reg_dest]);
        a->cmp(r_info.map[instr.reg_src_1], r_info.map[instr.reg_src_2]);
        a->jnb(not_below);
        a->inc(r_info.map[instr.reg_dest]);
        a->bind(not_below);
    } else {
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), 0);
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->cmp(x86::qword_ptr(r_info.base + 8 * instr.reg_src_1), x86::rax);
        a->jnb(not_below);
        a->inc(x86::qword_ptr(r_info.base + 8 * instr.reg_dest));
        a->bind(not_below);
    }
}

/**
* Translate the XOR instruction.
* XOR performs the bitwise logical XOR of rs1 and rs2.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_XOR(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate XOR…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->xor_(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->xor_(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SRL instruction.
* SLL, SRL, and SRA perform logical left, logical right, and arithmetic right shifts on the value in
* register rs1 by the shift amount held in the lower 5 bits of register rs2.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRL(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SRL…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->shr(r_info.map[instr.reg_dest], x86::cl);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->mov(x86::rcx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->and_(x86::rcx, 0b11111);
        a->shr(x86::rax, x86::cl);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SRA instruction.
* SLL, SRL, and SRA perform logical left, logical right, and arithmetic right shifts on the value in
* register rs1 by the shift amount held in the lower 5 bits of register rs2.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRA(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SRA…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->sar(r_info.map[instr.reg_dest], x86::cl);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->mov(x86::rcx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->and_(x86::rcx, 0b11111);
        a->sar(x86::rax, x86::cl);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the OR instruction.
* OR performs the bitwise logical OR of rs1 and rs2.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_OR(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate OR…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->or_(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->or_(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the AND instruction.
* AND performs the bitwise logical AND of rs1 and rs2.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_AND(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate AND…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->and_(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);
    } else {
        a->mov(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->and_(x86::rax, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SLLIW instruction. It is defined analogous to SLLI, but operates on 32-bit values.
* The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLLIW(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SLLIW…\n");
    //shift left the 32-bit value
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->shl(x86::edx, instr.imm & 0b111111);
        a->movsx(x86::rax, x86::edx);
        a->mov(r_info.map[instr.reg_dest], x86::rax);
    } else {
        a->mov(x86::rdx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->shl(x86::edx, instr.imm & 0b111111);
        a->movsx(x86::rax, x86::edx);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SRLIW instruction. It is defined analogous to SRLI, but operates on 32-bit values.
* The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRLIW(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SRLIW…\n");
    //shift right the 32-bit value
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->shr(x86::edx, instr.imm & 0b111111);
        a->movsx(x86::rax, x86::edx);
        a->mov(r_info.map[instr.reg_dest], x86::rax);
    } else {
        a->mov(x86::rdx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->shr(x86::edx, instr.imm & 0b111111);
        a->movsx(x86::rax, x86::edx);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SRAIW instruction. It is defined analogous to SRAI, but operates on 32-bit values.
* The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRAIW(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SRAIW…\n");
    //shift right the 32-bit value
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->sar(x86::edx, instr.imm & 0b111111);
        a->movsx(x86::rax, x86::edx);
        a->mov(r_info.map[instr.reg_dest], x86::rax);
    } else {
        a->mov(x86::rdx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->sar(x86::edx, instr.imm & 0b111111);
        a->movsx(x86::rax, x86::edx);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the ADDW instruction.
* ADDW and SUBW are RV64I-only instructions that are defined analogously to ADD and SUB
* but operate on 32-bit values and produce signed 32-bit results. Overflows are ignored, and the low
* 32-bits of the result is sign-extended to 64-bits and written to the destination register.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ADDW(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate ADDW…\n");
    /*
     * todo verify!
     * Right now, we add in 64-bit registers, take the lower 32-bit and sign extend that to XLEN.
     */
    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->add(x86::rdx, r_info.map[instr.reg_src_2]);
        a->movsx(r_info.map[instr.reg_dest], x86::edx);
    } else {
        a->mov(x86::edx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->add(x86::edx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->movsx(x86::rax, x86::edx);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SUBW instruction.
* ADDW and SUBW are RV64I-only instructions that are defined analogously to ADD and SUB
* but operate on 32-bit values and produce signed 32-bit results. Overflows are ignored, and the low
* 32-bits of the result is sign-extended to 64-bits and written to the destination register.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SUBW(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SUBW…\n");
    /*
     * todo verify!
     * Right now, we add in 64-bit registers, take the lower 32-bit and sign extend that to XLEN.
     */
    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->sub(x86::rdx, r_info.map[instr.reg_src_2]);
        a->movsx(r_info.map[instr.reg_dest], x86::edx);
    } else {
        a->mov(x86::edx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->sub(x86::edx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->movsx(x86::rax, x86::edx);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SLLW instruction.
* SLLW, SRLW, and SRAW are RV64I-only instructions that are analogously defined but operate
* on 32-bit values and produce signed 32-bit results. The shift amount is given by rs2[4:0]
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
* @see SLL, SRL, SRA
*/
void translate_SLLW(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SLLW…\n");

    //todo verify (see above)
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->shl(x86::edx, x86::cl);
        a->movsx(r_info.map[instr.reg_dest], x86::edx);
    } else {
        //shift in 32-bit register, then write-back
        a->mov(x86::rdx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->mov(x86::rcx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->and_(x86::rcx, 0b11111);
        a->shl(x86::edx, x86::cl);
        a->movsx(x86::rax, x86::edx);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SRLW instruction.
* SLLW, SRLW, and SRAW are RV64I-only instructions that are analogously defined but operate
* on 32-bit values and produce signed 32-bit results. The shift amount is given by rs2[4:0]
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
* @see SLL, SRL, SRA
*/
void translate_SRLW(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SRLW…\n");

    //todo verify (see above)
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->shr(x86::edx, x86::cl);
        a->movsx(r_info.map[instr.reg_dest], x86::edx);
    } else {
        //shift in 32-bit register, then write-back
        a->mov(x86::rdx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->mov(x86::rcx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->and_(x86::rcx, 0b11111);
        a->shr(x86::edx, x86::cl);
        a->movsx(x86::rax, x86::edx);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}

/**
* Translate the SRAW instruction.
* SLLW, SRLW, and SRAW are RV64I-only instructions that are analogously defined but operate
* on 32-bit values and produce signed 32-bit results. The shift amount is given by rs2[4:0]
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
* @see SLL, SRL, SRA
*/
void translate_SRAW(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate SRAW…\n");

    //todo verify (see above)
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->sar(x86::edx, x86::cl);
        a->movsx(r_info.map[instr.reg_dest], x86::edx);
    } else {
        //shift in 32-bit register, then write-back
        a->mov(x86::rdx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_1));
        a->mov(x86::rcx, x86::ptr_abs(r_info.base, 8 * instr.reg_src_2));
        a->and_(x86::rcx, 0b11111);
        a->sar(x86::edx, x86::cl);
        a->movsx(x86::rax, x86::edx);
        a->mov(x86::ptr_abs(r_info.base, 8 * instr.reg_dest), x86::rax);
    }
}
