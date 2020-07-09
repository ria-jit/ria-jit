//
// Created by flo on 21.05.20.
//

#include "translate_arithmetic.hpp"
#include "runtime/register.h"
#include <fadec/fadec-enc.h>

//shortcut for memory operands
#define FE_BASE_MEM(base) FE_MEM(base, 0, 0, 0)

/**
 * ADDIW adds the sign-extended 12-bit immediate to register rs1 and produces the
 * proper sign-extension of a 32-bit result in rd.
 * @param instr
 */
void translate_addiw(const t_risc_instr &instr, const register_info &r_info) {
    // mov rd, rs1
    // add rd, instr.imm
    log_asm_out("Translate addiw…\n");
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->add(x86::edx, instr.imm);
        a->movsxd(r_info.map[instr.reg_dest], x86::edx);*/
    } else {
        err |= fe_enc64(&current, FE_MOV32rm, FE_DX, FE_MEM(r_info.base + 8 * instr.reg_src_1, 0, 0, 0));
        err |= fe_enc64(&current, FE_ADD32ri, FE_DX, instr.imm);
        err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, FE_DX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(r_info.base + 8 * instr.reg_dest, 0, 0, 0), FE_AX);
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
    log_asm_out("Translate slli…\n");
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->shl(r_info.map[instr.reg_dest], instr.imm & 0b111111);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64mr, FE_AX, FE_MEM(r_info.base + 8 * instr.reg_src_1, 0, 0, 0));
        err |= fe_enc64(&current, FE_SHL64ri, FE_AX, instr.imm & 0b111111);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(r_info.base + 8 * instr.reg_dest, 0, 0, 0), FE_AX);
    }
}

/**
 * LUI places the 20-bit U-immediate into bits 31-12 of register rd and places zero in the lowest 12 bits.
 * The 32-bit result is sign-extended to 64 bits.
 * @param instr
 */
void translate_lui(const t_risc_instr &instr, const register_info &r_info) {
    //mov rd, extended
    log_asm_out("Translate lui…\n");

    //move into register
    if (r_info.mapped[instr.reg_dest]) {
        //a->mov(r_info.map[instr.reg_dest], instr.imm);
    } else {
        err |= fe_enc64(&current, FE_MOV64mi, FE_MEM(r_info.base + 8 * instr.reg_dest, 0, 0, 0), instr.imm);
    }
}

/**
 * ADDI adds the sign-extended 12-bit immediate to register rs1.
 * Overflow is ignored and the result is the low (in our case) 64 bit of the result.
 * The result is stored in rd.
 * @param instr
 */
void translate_addi(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate addi…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->add(r_info.map[instr.reg_dest], instr.imm);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM(r_info.base + 8 * instr.reg_src_1, 0, 0, 0));
        err |= fe_enc64(&current, FE_ADD64ri, FE_AX, instr.imm);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(r_info.base + 8 * instr.reg_dest, 0, 0, 0), FE_AX);
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
    log_asm_out("Translate AUIPC…\n");

    if (instr.reg_dest != t_risc_reg::x0) {
        //add offset to the pc and store in rd
        if (r_info.mapped[instr.reg_dest]) {
            /*a->mov(r_info.map[instr.reg_dest], instr.addr);
            a->add(r_info.map[instr.reg_dest], instr.imm);*/
        } else {
            err |= fe_enc64(&current, FE_MOV64ri, FE_AX, instr.addr);
            err |= fe_enc64(&current, FE_ADD64ri, FE_AX, instr.imm);
            err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(r_info.base + 8 * instr.reg_dest, 0, 0, 0), FE_AX);
        }
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
    log_asm_out("Translate SLTI…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->xor_(r_info.map[instr.reg_dest], r_info.map[instr.reg_dest]);
        a->cmp(r_info.map[instr.reg_src_1], instr.imm);
        a->jnl(not_less);
        a->inc(r_info.map[instr.reg_dest]);
        a->bind(not_less);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64mi, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), 0);
        err |= fe_enc64(&current, FE_CMP64mi, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1), instr.imm);

        //insert forward jump here later
        uint8_t *jmp_buf = current;
        *(current++) = 0x90; //there must be a nicer way to do this :)
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;

        err |= fe_enc64(&current, FE_INC64m, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest));

        //write forward jump target when now known
        auto not_less = (uintptr_t) current;
        err |= fe_enc64(&jmp_buf, FE_JGE, (intptr_t) not_less);
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
    log_asm_out("Translate SLTIU…\n");

    //const Label &not_below = a->newLabel();
    //const Label &below = a->newLabel();

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->cmp(r_info.map[instr.reg_src_1], instr.imm);
        a->jnb(not_below);
        a->inc(r_info.map[instr.reg_dest]);
        a->jmp(below);
        a->bind(not_below);
        a->xor_(r_info.map[instr.reg_dest], r_info.map[instr.reg_dest]);
        a->bind(below);*/
    } else {
        err |= fe_enc64(&current, FE_CMP64mi, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1), instr.imm);
        uint8_t *jnb_buffer = current;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;

        err |= fe_enc64(&current, FE_INC64m, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest));

        uint8_t *jmp_buffer = current;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;

        //not_below <- jnb:
        err |= fe_enc64(&jnb_buffer, FE_JNC, (intptr_t) current);
        err |= fe_enc64(&current, FE_MOV64mi, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), 0);

        //below <- jmp:
        err |= fe_enc64(&jmp_buffer, FE_JMP | FE_JMPL, (intptr_t) current);
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
    log_asm_out("Translate XORI…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->xor_(r_info.map[instr.reg_dest], instr.imm);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_XOR64ri, FE_AX, instr.imm);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate ORI…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->or_(r_info.map[instr.reg_dest], instr.imm);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_OR64ri, FE_AX, instr.imm);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate ANDI…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->and_(r_info.map[instr.reg_dest], instr.imm);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_AND64ri, FE_AX, instr.imm);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
    }
}

/**
* Translate the SRLI instruction. SRLI is a logical right shift.
* The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRLI(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SRLI…\n");
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->shr(r_info.map[instr.reg_dest], instr.imm & 0b111111);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64mr, FE_AX, FE_MEM(r_info.base + 8 * instr.reg_src_1, 0, 0, 0));
        err |= fe_enc64(&current, FE_SHR64ri, FE_AX, instr.imm & 0b111111);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(r_info.base + 8 * instr.reg_dest, 0, 0, 0), FE_AX);
    }
}

/**
* Translate the SRAI instruction. SRAI is an arithmetic right shift.
* The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRAI(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SRAI…\n");
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->sar(r_info.map[instr.reg_dest], instr.imm & 0b111111);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64mr, FE_AX, FE_MEM(r_info.base + 8 * instr.reg_src_1, 0, 0, 0));
        err |= fe_enc64(&current, FE_SAR64ri, FE_AX, instr.imm & 0b111111);
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(r_info.base + 8 * instr.reg_dest, 0, 0, 0), FE_AX);
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
    log_asm_out("Translate ADD…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->add(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_ADD64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate SUB…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->sub(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_SUB64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate SLL…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->shl(r_info.map[instr.reg_dest], x86::cl);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_AND64ri, FE_CX, 0b11111);
        err |= fe_enc64(&current, FE_SHL64rr, FE_AX, FE_CX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
    }
}

/**
* Translate the SLT instruction.
* SLT and SLTU perform signed and unsigned compares respectively, writing 1 to rd if rs1 < rs2, 0 otherwise.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLT(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SLT…\n");

    //const Label &not_less = a->newLabel();
    //const Label &less = a->newLabel();

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        /*a->cmp(r_info.map[instr.reg_src_1], r_info.map[instr.reg_src_2]);
        a->jnl(not_less);
        a->inc(r_info.map[instr.reg_dest]);
        a->jmp(less);
        a->bind(not_less);
        a->xor_(r_info.map[instr.reg_dest], r_info.map[instr.reg_dest]);
        a->bind(less);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_CMP64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1), FE_AX);

        //jnl not_less
        uint8_t *jnl_buf = current;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;

        err |= fe_enc64(&current, FE_INC64m, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest));

        //jmp less
        uint8_t *jmp_buf = current;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;

        //not_less:
        err |= fe_enc64(&jnl_buf, FE_JGE, (intptr_t) current);

        err |= fe_enc64(&current, FE_MOV64mi, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), 0);

        //less:
        err |= fe_enc64(&jmp_buf, FE_JMP, (intptr_t) current);
    }
}

/**
* Translate the SLTU instruction.
* SLT and SLTU perform signed and unsigned compares respectively, writing 1 to rd if rs1 < rs2, 0 otherwise.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLTU(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SLTU…\n");

    //const Label &not_below = a->newLabel();
    //const Label &below = a->newLabel();

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        /*a->cmp(r_info.map[instr.reg_src_1], r_info.map[instr.reg_src_2]);
        a->jnb(not_below);
        a->inc(r_info.map[instr.reg_dest]);
        a->jmp(below);
        a->bind(not_below);
        a->xor_(r_info.map[instr.reg_dest], r_info.map[instr.reg_dest]);
        a->bind(below);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_CMP64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1), FE_AX);

        //jnl not_below
        uint8_t *jnb_buf = current;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;

        err |= fe_enc64(&current, FE_INC64m, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest));

        //jmp below
        uint8_t *jmp_buf = current;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;
        *(current++) = 0x90;

        //not_below:
        err |= fe_enc64(&jnb_buf, FE_JNC, (intptr_t) current);

        err |= fe_enc64(&current, FE_MOV64mi, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), 0);

        //below:
        err |= fe_enc64(&jmp_buf, FE_JMP, (intptr_t) current);
    }
}

/**
* Translate the XOR instruction.
* XOR performs the bitwise logical XOR of rs1 and rs2.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_XOR(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate XOR…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->xor_(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_XOR64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate SRL…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->shr(r_info.map[instr.reg_dest], x86::cl);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_AND64ri, FE_CX, 0b11111);
        err |= fe_enc64(&current, FE_SHR64rr, FE_AX, FE_CX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate SRA…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->sar(r_info.map[instr.reg_dest], x86::cl);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_AND64ri, FE_CX, 0b11111);
        err |= fe_enc64(&current, FE_SAR64rr, FE_AX, FE_CX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
    }
}

/**
* Translate the OR instruction.
* OR performs the bitwise logical OR of rs1 and rs2.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_OR(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate OR…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->or_(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_OR64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
    }
}

/**
* Translate the AND instruction.
* AND performs the bitwise logical AND of rs1 and rs2.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_AND(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate AND…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        /*a->mov(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_1]);
        a->and_(r_info.map[instr.reg_dest], r_info.map[instr.reg_src_2]);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_AND64rm, FE_AX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
    }
}

/**
* Translate the SLLIW instruction. It is defined analogous to SLLI, but operates on 32-bit values.
* The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SLLIW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SLLIW…\n");
    //shift left the 32-bit value
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->shl(x86::edx, instr.imm & 0b111111);
        a->movsxd(x86::rax, x86::edx);
        a->mov(r_info.map[instr.reg_dest], x86::rax);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_SHL32ri, FE_DX, instr.imm & 0b111111);
        err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, FE_DX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
    }
}

/**
* Translate the SRLIW instruction. It is defined analogous to SRLI, but operates on 32-bit values.
* The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRLIW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SRLIW…\n");
    //shift right the 32-bit value
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->shr(x86::edx, instr.imm & 0b111111);
        a->movsxd(x86::rax, x86::edx);
        a->mov(r_info.map[instr.reg_dest], x86::rax);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_SHR32ri, FE_DX, instr.imm & 0b111111);
        err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, FE_DX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
    }
}

/**
* Translate the SRAIW instruction. It is defined analogous to SRAI, but operates on 32-bit values.
* The operand to be shifted is in rs1, the shift amount is in the lower 6 bits of the I-immediate field.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_SRAIW(const t_risc_instr &instr, const register_info &r_info) {
    log_asm_out("Translate SRAIW…\n");
    //shift right the 32-bit value
    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->sar(x86::edx, instr.imm & 0b111111);
        a->movsxd(x86::rax, x86::edx);
        a->mov(r_info.map[instr.reg_dest], x86::rax);*/
    } else {
        err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_SAR32ri, FE_DX, instr.imm & 0b111111);
        err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, FE_DX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate ADDW…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        /*a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->add(x86::rdx, r_info.map[instr.reg_src_2]);
        a->movsxd(r_info.map[instr.reg_dest], x86::edx);*/
    } else {
        err |= fe_enc64(&current, FE_MOV32rm, FE_DX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_ADD32rm, FE_DX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, FE_DX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate SUBW…\n");

    if (r_info.mapped[instr.reg_dest] && r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2]) {
        /*a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->sub(x86::rdx, r_info.map[instr.reg_src_2]);
        a->movsxd(r_info.map[instr.reg_dest], x86::edx);*/
    } else {
        err |= fe_enc64(&current, FE_MOV32rm, FE_DX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_SUB32rm, FE_DX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, FE_DX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate SLLW…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->shl(x86::edx, x86::cl);
        a->movsxd(r_info.map[instr.reg_dest], x86::edx);*/
    } else {
        //shift in 32-bit register, then write-back
        err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_AND64ri, FE_CX, 0b11111);
        err |= fe_enc64(&current, FE_SHL32rr, FE_DX, FE_CX);
        err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, FE_DX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate SRLW…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->shr(x86::edx, x86::cl);
        a->movsxd(r_info.map[instr.reg_dest], x86::edx);*/
    } else {
        //shift in 32-bit register, then write-back
        err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_AND64ri, FE_CX, 0b11111);
        err |= fe_enc64(&current, FE_SHR32rr, FE_DX, FE_CX);
        err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, FE_DX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
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
    log_asm_out("Translate SRAW…\n");

    if (r_info.mapped[instr.reg_src_1] && r_info.mapped[instr.reg_src_2] && r_info.mapped[instr.reg_dest]) {
        /*a->mov(x86::rdx, r_info.map[instr.reg_src_1]);
        a->mov(x86::rcx, r_info.map[instr.reg_src_2]);
        a->and_(x86::rcx, 0b11111);
        a->sar(x86::edx, x86::cl);
        a->movsxd(r_info.map[instr.reg_dest], x86::edx);*/
    } else {
        //shift in 32-bit register, then write-back
        err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_1));
        err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_BASE_MEM(r_info.base + 8 * instr.reg_src_2));
        err |= fe_enc64(&current, FE_AND64ri, FE_CX, 0b11111);
        err |= fe_enc64(&current, FE_SAR32rr, FE_DX, FE_CX);
        err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, FE_DX);
        err |= fe_enc64(&current, FE_MOV64mr, FE_BASE_MEM(r_info.base + 8 * instr.reg_dest), FE_AX);
    }
}
