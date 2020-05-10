//
// Created by flo on 02.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H

//error codes for internal reference
#include <stdint.h>

#define FAIL_HEAP_ALLOC 0x1000

//the RISC-V mnemonic of the instruction
typedef enum {
    //---RV32I---
    LUI, //load upper Imm
    AUIPC, //register = Imm

    //control flow
    JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU,

    //load & store
    LBW, LBU, LHU, SB, SH, SW,

    //Arithmetic
    ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,

    //??????
    FENCE, FENCE_I,

    //---RV64I---
    //load & store
    LWU, LD, SD,

    //Arithmetic
    //SLLI,
    //SRLI,
    //SRAI,
    ADDIW, SLLIW, SRLIW, SRAIW, ADDW, SUBW, SLLW, SRLW, SRAW,

    //---RV32M---
    MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU,

    //---RV64M---
    MULW, DIVW, DIVUW, REMW, REMUW

} t_risc_mnem;

//general purpose registers (x1 is ret addr, x2 is sp by convention)
typedef enum {
    x0, //x0 is hardwired to constant 0
    x1, x2, x3, x4, x5, x6, x7, x8, x9,
    x10, x11, x12, x13, x14, x15, x16, x17, x18, x19,
    x20, x21, x22, x23, x24, x25, x26, x27, x28, x29,
    x30, x31,
    pc
} t_risc_reg;

//register value type
typedef uint64_t t_risc_reg_val;

//RISC-V operation types (for later optimization)
typedef enum {
    REG_REG, IMMEDIATE, UPPER_IMMEDIATE, STORE, BRANCH, JUMP
} t_risc_optype;

//carry immediate values in the instruction struct
typedef int* t_risc_imm;

//carry a pointer to the raw instruction in the struct (could be expanded)
typedef int* t_risc_raw_instr;

//riscV address
typedef unsigned long int t_risc_addr;

typedef struct {
    t_risc_addr addr;
    t_risc_mnem mnem;
    t_risc_optype optype;
    t_risc_reg reg_src_1;
    t_risc_reg reg_src_2;
    t_risc_reg reg_dest;
    t_risc_imm imm;
    t_risc_raw_instr raw_bytes;
} t_risc_instr;

int parse_jump_immediate(t_risc_instr* instr);

void not_yet_implemented();

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H
