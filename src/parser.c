//
// Created by noah on 29.04.20.
// Based on RISC-V-Spec.pdf ../documentation/RISC-V-Spec.pdf
// For register assembly conventions look at page 137
//
#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "parser.h"

#define N_OPCODE 32 //inst[6:2] 5 bit 2⁵ = 32

typedef int *t_inst; //maybe replace with some struct with meta info later
typedef void t_parse_result; //maybe some more return information later

// extract rd register number bit[11:7]
static inline int extract_rd(int instr) { return instr >> 7 & 0b11111; }

// extract rs1 register number bit[19:15]
static inline int extract_rs1(int instr) { return instr >> 15 & 0b11111; }

// extract rs2 register number bit[24:20]
static inline int extract_rs2(int instr) { return instr >> 20 & 0b11111; }

// extract func3 bit [14:12]
static inline int extract_func3(int instr) { return instr >> 12 & 0b111; }

// extract big_shamt bit[25:20]
static inline int extract_big_shamt(int instr) { return instr >> 20 & 0b111111; }

// extract small_shamt bit[24:20] -> basically the same as rs2
static inline int extract_small_shamt(int instr) { return instr >> 20 & 0b11111; }

// extract U-Type immediate bit[31:12] -> mask lower 12 bit [11:0] with zeros
static inline int extract_imm_U(int instr) { return instr & ~(0xfff); }

// extract I-Type immediate bit[31:20]
static inline int extract_imm_I(int instr) { return instr >> 20; } //sign extend!

// extract S-Type immediate bit[31:25] + [11:7] => 7bits + 5 bits
// TODO need sign extend?
static inline int extract_imm_S(int instr) { return (instr >> 20 & 0b1111111) | (instr >> 7 & 0b11111); }

// extract J-Type immediate bits[31:12] order: [20|10:1|11|19:12]
// only for jumps so no sign extend?
// [20] => [31], [10:1] => [30:21], [11] => [20], [19:12] => [19:12]
static inline int extract_imm_J(int instr) {
    return (instr & 0xff000) | (instr >> (20 - 11) & (1 << 11)) | (instr >> 11 & (1 << 20)) |
           (instr >> (30 - 10) & 0b11111111110);
}

// extract B-Type immediate bits[31:25],[11:7] order: [12|10:5],[4:1|11]
static inline int extract_imm_B(int instr) {
    return (instr >> (31 - 12) & (1 << 12)) | (instr << (11 - 7) & (1 << 11) |
           (instr >> (30 - 10) & 0b11111100000) | (instr >> (11 - 4) & 0b11110));
}

//function prototypes
t_parse_result parse_LOAD(t_inst instruction);

t_parse_result parse_LOAD_FP(t_inst instruction);

t_parse_result parse_MISC_MEM(t_inst instruction);

t_parse_result parse_OP_IMM(t_inst instruction);

t_parse_result parse_AUIPC(t_inst instruction);

t_parse_result parse_OP_IMM_32(t_inst instruction);

t_parse_result parse_STORE(t_inst instruction);

t_parse_result parse_STORE_FP(t_inst instruction);

t_parse_result parse_AMO(t_inst instruction);

t_parse_result parse_OP(t_inst instruction);

t_parse_result parse_LUI(t_inst instruction);

t_parse_result parse_OP_32(t_inst instruction);

t_parse_result parse_MADD(t_inst instruction);

t_parse_result parse_MSUB(t_inst instruction);

t_parse_result parse_NMSUB(t_inst instruction);

t_parse_result parse_NMADD(t_inst instruction);

t_parse_result parse_OP_FP(t_inst instruction);

t_parse_result parse_BRANCH(t_inst instruction);

t_parse_result parse_JALR(t_inst instruction);

t_parse_result parse_JAL(t_inst instruction);

t_parse_result parse_SYSTEM(t_inst instruction);

void test_parsing() {
    /*
     * dissassembly of li a0, 0xDEADBEEF
     *  00038537                lui     a0,0x38
     *  ab75051b                addiw   a0,a0,-1353
     *  00e51513                slli    a0,a0,0xe
     *  eef50513                addi    a0,a0,-273 # 37eef <__global_pointer$+0x26667>
     */

    unsigned int *memory = (unsigned int *) malloc(0x10);
    //fill memory with the data we have little endian so 00038537 should be 37 85 38 00 in memory
    memory[0] = 0x38537;
    memory[1] = 0xab75051b;
    memory[2] = 0xe51513;
    memory[3] = 0xeef50513;

    printf("First line of memory %#010x, first byte: %#x\n", memory[0], ((unsigned char *) memory)[0]);

    for (int i = 0; i < 4; i++) {
        parse_instruction(&memory[i]);
    }
}

void parse_instruction(t_inst instruction) {
    // print out the line to parse in grouped binary as in the spec
    printf("Parsing: %#010x\n", *instruction);
    //extract opcode bits[6:2]
    t_opcodes opcode = *instruction >> 2 & 0x1f;
    switch(opcode){
        case OP_LUI:
            parse_LUI(instruction);
            break;
        case OP_OP_IMM_32:
            parse_OP_IMM_32(instruction);
            break;
        case OP_OP_IMM:
            parse_OP_IMM(instruction);
            break;
        default:
            not_yet_implemented("OPCODE");
    }
}


void not_implemented(t_inst instruction) {
    printf("Error when trying to parse \"%#010x\", instruction not implemented!", *instruction);
}

t_parse_result parse_LUI(t_inst instruction) {
    //extract imm [31:12] 20 bits
    // fills rd with the upper 20 bits and fills lower 12 bits with zeros
    printf("LUI rd: %d, imm32: %#010x\n", extract_rd(*instruction), extract_imm_U(*instruction));
}

t_parse_result parse_LOAD(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_LOAD_FP(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_MISC_MEM(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_OP_IMM(t_inst instruction) {
    switch (extract_func3(*instruction)) {
        case 0: { //ADDI
            printf("ADDI rd %d, rs1 %d, imm %d\n", extract_rd(*instruction), extract_rs1(*instruction),
                   extract_imm_I(*instruction));
            break;
        }
        case 1: { //SLLI opcode and func3 are unique
            printf("SLLI rd %d, rs1 %d, shamt %d\n", extract_rd(*instruction), extract_rs1(*instruction),
                   extract_big_shamt(*instruction));
            break;
        }
        default: {
            not_implemented(instruction);
        }
    }
}

t_parse_result parse_AUIPC(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_OP_IMM_32(t_inst instruction) {
    // extract func 3 bits[14:12]
    switch (extract_func3(*instruction)) {
        case 0: {
            //extract imm bits[31:20] 12 bits we need sign extension!
            int imm = *instruction >> 20;
            printf("ADDIW rd: %d, rs1 %d, imm %d\n", extract_rd(*instruction), extract_rs1(*instruction), imm);
            break;
        }
        default: {
            not_implemented(instruction);
            break;
        }
    }
}

t_parse_result parse_STORE(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_STORE_FP(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_AMO(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_OP(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_OP_32(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_MADD(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_MSUB(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_NMSUB(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_NMADD(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_OP_FP(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_BRANCH(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_JALR(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_JAL(t_inst instruction) {
    not_implemented(instruction);
}

t_parse_result parse_SYSTEM(t_inst instruction) {
    not_implemented(instruction);
}

