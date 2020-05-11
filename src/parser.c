//
// Created by noah on 29.04.20.
// Based on RISC-V-Spec.pdf ../documentation/RISC-V-Spec.pdf
// For register assembly conventions look at page 137
//
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

#define N_OPCODE 32 //inst[6:2] 5 bit 2⁵ = 32

typedef int *t_inst; //maybe replace with some struct with meta info later
typedef void t_parse_result; //maybe some more return information later

// extract rd register number bit[11:7]
int extract_rd(int instr) { return instr >> 7 & 0b11111; }

// extract rs1 register number bit[19:15]
int extract_rs1(int instr) { return instr >> 15 & 0b11111; }

// extract rs2 register number bit[24:20]
int extract_rs2(int instr) { return instr >> 20 & 0b11111; }

// extract func3 bit [14:12]
int extract_func3(int instr) { return instr >> 12 & 0b111; }

// extract big_shamt bit[25:20]
int extract_big_shamt(int instr) { return instr >> 20 & 0b111111; }

// extract imm_11 bit[31:20]
int extract_imm_11(int instr) { return instr >> 20; } //sign extend!

// lookup table for opcode
t_parse_result (*opcode_lookup[N_OPCODE])(int *instruction);

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

t_parse_result not_implemented(t_inst instruction);

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

    fill_lookuptables();

    for (int i = 0; i < 4; i++) {
        parse_instruction(&memory[i]);
    }
}

void fill_lookuptables(void) {
    // first fill with function in case of not being implemented
    for (int i = 0; i < N_OPCODE; i++) {
        opcode_lookup[i] = &not_implemented;
    }

    opcode_lookup[0] = &parse_LOAD;
    opcode_lookup[1] = &parse_LOAD_FP;
    opcode_lookup[2] = &not_implemented;
    opcode_lookup[3] = &parse_MISC_MEM;
    opcode_lookup[4] = &parse_OP_IMM;
    opcode_lookup[5] = &parse_AUIPC;
    opcode_lookup[6] = &parse_OP_IMM_32;
    opcode_lookup[7] = &not_implemented;
    opcode_lookup[8] = &parse_STORE;
    opcode_lookup[9] = &parse_STORE_FP;
    opcode_lookup[10] = &not_implemented;
    opcode_lookup[11] = &parse_AMO;
    opcode_lookup[12] = &parse_OP;
    opcode_lookup[13] = &parse_LUI;
    opcode_lookup[14] = &parse_OP_32;
    opcode_lookup[15] = &not_implemented;
    opcode_lookup[16] = &parse_MADD;
    opcode_lookup[17] = &parse_MSUB;
    opcode_lookup[18] = &parse_NMSUB;
    opcode_lookup[19] = &parse_NMADD;
    opcode_lookup[20] = &parse_OP_FP;
    opcode_lookup[21] = &not_implemented;
    opcode_lookup[22] = &not_implemented;
    opcode_lookup[23] = &not_implemented;
    opcode_lookup[24] = &parse_BRANCH;
    opcode_lookup[25] = &parse_JALR;
    opcode_lookup[26] = &not_implemented;
    opcode_lookup[27] = &parse_JAL;
    opcode_lookup[28] = &parse_SYSTEM;
    opcode_lookup[29] = &not_implemented;
    opcode_lookup[30] = &not_implemented;
    opcode_lookup[31] = &not_implemented;
}

void parse_instruction(t_inst instruction) {
    // print out the line to parse in grouped binary as in the spec
    printf("Parsing: %#010x\n", *instruction);
    //extract opcode bits[6:2]
    int opcode = *instruction >> 2 & 0x1f;
    opcode_lookup[opcode](instruction);
}


void not_implemented(t_inst instruction) {
    printf("Error when trying to parse \"%#010x\", instruction not implemented!", *instruction);
}

t_parse_result parse_LUI(t_inst instruction) {
    //extract imm [31:12] 20 bits
    // fills rd with the upper 20 bits and fills lower 12 bits with zeros
    int imm = *instruction & (0xfffff << 12);
    printf("LUI rd: %d, imm32: %#010x\n", extract_rd(*instruction), imm);
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
                   extract_imm_11(*instruction));
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

