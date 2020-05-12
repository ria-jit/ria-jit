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

typedef void t_parse_result; //maybe some more return information later

// extract rd register number bit[11:7]
static inline int extract_rd(int32_t instr) { return instr >> 7 & 0b11111; }

// extract rs1 register number bit[19:15]
static inline int extract_rs1(int32_t instr) { return instr >> 15 & 0b11111; }

// extract rs2 register number bit[24:20]
static inline int extract_rs2(int32_t instr) { return instr >> 20 & 0b11111; }

// extract func3 bit [14:12]
static inline int extract_func3(int32_t instr) { return instr >> 12 & 0b111; }

// extract big_shamt bit[25:20]
static inline int extract_big_shamt(int32_t instr) { return instr >> 20 & 0b111111; }

// extract small_shamt bit[24:20] -> basically the same as rs2
static inline int extract_small_shamt(int32_t instr) { return instr >> 20 & 0b11111; }

// extract U-Type immediate bit[31:12] -> mask lower 12 bit [11:0] with zeros
static inline int extract_imm_U(int32_t instr) { return instr & ~(0xfff); }

// extract I-Type immediate bit[31:20]
static inline int extract_imm_I(int32_t instr) { return instr >> 20; } //sign extend!

// extract S-Type immediate bit[31:25] + [11:7] => 7bits + 5 bits
// TODO need sign extend?
static inline int extract_imm_S(int32_t instr) { return (instr >> 20 & 0b1111111) | (instr >> 7 & 0b11111); }

// extract J-Type immediate bits[31:12] order: [20|10:1|11|19:12]
// only for jumps so no sign extend?
// [20] => [31], [10:1] => [30:21], [11] => [20], [19:12] => [19:12]
static inline int extract_imm_J(int32_t instr) {
    return (instr & 0xff000) | (instr >> (20 - 11) & (1 << 11)) | (instr >> 11 & (1 << 20)) |
           (instr >> (30 - 10) & 0b11111111110);
}

// extract B-Type immediate bits[31:25],[11:7] order: [12|10:5],[4:1|11]
static inline int32_t extract_imm_B(int32_t instr) {
    return (instr >> (31 - 12) & 0xfffff000) | (instr << (11 - 7) & (1 << 11)) |
           (instr >> (30 - 10) & 0b11111100000) | (instr >> (11 - 4) & 0b11110);
}

//function prototypes DEPRECATED
t_parse_result parse_OP_IMM(int32_t* instruction);

t_parse_result parse_OP_IMM_32(int32_t* instruction);

t_parse_result parse_LUI(int32_t* instruction);

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
        t_risc_instr instr;
        instr.addr = &memory[i];
        parse_instruction(&instr);
    }
}

void parse_instruction(t_risc_instr *p_instr_struct) {
    //TODO verify all commands, clean up textual output, add float and multiprocessor memory opcodes?

    // print out the line to parse in grouped binary as in the spec
    t_risc_instr instr_struct = *p_instr_struct;
    int32_t raw_instr = instr_struct.addr;
    printf("Parsing: %#010x\n", raw_instr);

    //fill basic struct
    instr_struct.reg_dest = extract_rd(raw_instr);
    instr_struct.reg_src_1 = extract_rs1(raw_instr);
    //instr_struct.reg_src_2 = extract_rs2(raw_instr); NOT REALLY NEEDED MOST OF TIME

    //extract opcode bits[6:2]
    t_opcodes opcode = raw_instr >> 2 & 0x1f;
    switch (opcode) {
        case OP_LUI:
            parse_LUI(&raw_instr);
            instr_struct.optype = UPPER_IMMEDIATE;
            instr_struct.mnem = LUI;
            instr_struct.imm = extract_imm_U(raw_instr);
            break;
        case OP_AUIPC:
            instr_struct.optype = IMMEDIATE;
            instr_struct.mnem = AUIPC;
            instr_struct.imm = extract_imm_U(raw_instr);
            break;
        case OP_JAL:
            instr_struct.optype = JUMP;
            instr_struct.mnem = JAL;
            instr_struct.imm = extract_imm_J(raw_instr);
            break;
        case OP_JALR:
            instr_struct.optype = JUMP;
            instr_struct.mnem = JALR;
            instr_struct.imm = extract_imm_I(raw_instr);
            break;
        case OP_MISC_MEM:
            instr_struct.optype = SYSTEM;
            instr_struct.imm = extract_imm_I(raw_instr);
            switch(extract_func3(raw_instr)){
                case 0:
                    instr_struct.mnem = FENCE;
                    break;
                case 1:
                    instr_struct.mnem = FENCE_I;
                    break;
                default:
                    not_yet_implemented("Invalid func3 for OP_MISC_MEM Opcode");
            }
            break;
        case OP_BRANCH:
            // BEQ, BNE...
            instr_struct.optype = BRANCH;
            instr_struct.reg_src_2 = extract_rs2(raw_instr);
            instr_struct.imm = extract_imm_B(raw_instr);
            switch (extract_func3(raw_instr)) {
                case 0:
                    instr_struct.mnem = BEQ;
                    break;
                case 1:
                    instr_struct.mnem = BNE;
                    break;
                case 4:
                    instr_struct.mnem = BLT;
                    break;
                case 5:
                    instr_struct.mnem = BGE;
                    break;
                case 6:
                    instr_struct.mnem = BLTU;
                    break;
                case 7:
                    instr_struct.mnem = BGEU;
                    break;
                default:
                    not_yet_implemented("Invalid func3 for BRANCH Opcode");
            }
            break;
        case OP_LOAD:
            instr_struct.optype = IMMEDIATE;
            instr_struct.imm = extract_imm_I(raw_instr);
            switch (extract_func3(raw_instr)) {
                case 0: {
                    instr_struct.mnem = LB;
                    break;
                }
                case 1: {
                    instr_struct.mnem = LH;
                    break;
                }
                case 2: {
                    instr_struct.mnem = LW;
                    break;
                }
                case 3: {
                    instr_struct.mnem = LD;
                    break;
                }
                case 4: {
                    instr_struct.mnem = LBU;
                    break;
                }
                case 5: {
                    instr_struct.mnem = LHU;
                    break;
                }
                case 6: {
                    instr_struct.mnem = LWU;
                    break;
                }
                default: {
                    not_yet_implemented("Invalid LOAD Instruction");
                }
            }
            break;
        case OP_STORE:
            instr_struct.optype = STORE;
            instr_struct.imm = extract_imm_S(raw_instr);
            instr_struct.reg_src_2 = extract_rs2(raw_instr);
            switch (extract_func3(raw_instr)) {
                case 0:
                    instr_struct.mnem = SB;
                    break;
                case 1:
                    instr_struct.mnem = SH;
                    break;
                case 2:
                    instr_struct.mnem = SW;
                    break;
                case 3:
                    instr_struct.mnem = SD;
                    break;
                default:
                    not_yet_implemented("Invalid OP_STORE Instruction");
            }
            break;
        case OP_OP:
            instr_struct.optype = REG_REG;
            instr_struct.reg_src_2 = extract_rs2(raw_instr);
            if(raw_instr & (1<<25)) {
                switch (extract_func3(raw_instr)) {
                    case 0:
                        instr_struct.mnem = MUL;
                        break;
                    case 1:
                        instr_struct.mnem = MULH;
                        break;
                    case 2:
                        instr_struct.mnem = MULHSU;
                        break;
                    case 3:
                        instr_struct.mnem = MULHU;
                        break;
                    case 4:
                        instr_struct.mnem = DIV;
                        break;
                    case 5:
                        instr_struct.mnem = DIVU;
                        break;
                    case 6:
                        instr_struct.mnem = REM;
                        break;
                    case 7:
                        instr_struct.mnem = REMU;
                        break;
                }
            } else {
                switch (extract_func3(raw_instr)) {
                    case 0:
                        if (raw_instr & (1 << 30)) {
                            //SRAI
                            instr_struct.mnem = SUB;
                        } else {
                            //SRLI
                            instr_struct.mnem = ADD;
                        }
                        break;
                    case 1:
                        instr_struct.mnem = SLL;
                        break;
                    case 2:
                        instr_struct.mnem = SLT;
                        break;
                    case 3:
                        instr_struct.mnem = SLTU;
                        break;
                    case 4:
                        instr_struct.mnem = XOR;
                        break;
                    case 5:
                        if (raw_instr & (1 << 30)) {
                            //SRAI
                            instr_struct.mnem = SRA;
                        } else {
                            //SRLI
                            instr_struct.mnem = SRL;
                        }
                        break;
                    case 6:
                        instr_struct.mnem = OR;
                        break;
                    case 7:
                        instr_struct.mnem = AND;
                        break;
                }
            }
            break;
        case OP_SYSTEM:
            instr_struct.optype = SYSTEM;
            instr_struct.imm = extract_imm_I(raw_instr);
            switch(extract_func3(raw_instr)){
                case 0:
                    if(raw_instr & (1<<20)){
                        instr_struct.mnem = ECALL;
                    } else {
                        instr_struct.mnem = EBREAK;
                    }
                    break;
                case 1:
                    instr_struct.mnem = CSRRW;
                    break;
                case 2:
                    instr_struct.mnem = CSRRS;
                    break;
                case 3:
                    instr_struct.mnem = CSRRC;
                    break;
                case 5:
                    instr_struct.mnem = CSRRWI;
                    break;
                case 6:
                    instr_struct.mnem = CSRRSI;
                    break;
                case 7:
                    instr_struct.mnem = CSRRCI;
                    break;
                default:
                    not_yet_implemented("Invalid CSSR Instruction");
            }
            break;
        case OP_OP_IMM_32:
            parse_OP_IMM_32(&raw_instr);
            instr_struct.optype = IMMEDIATE;
            switch (extract_func3(raw_instr)) {
                case 0:
                    instr_struct.mnem = ADDIW;
                    instr_struct.imm = extract_imm_I(raw_instr);
                    break;
                case 1: //SLLIW opcode and func3 are unique
                    instr_struct.mnem = SLLIW;
                    instr_struct.imm = extract_small_shamt(raw_instr);
                    break;
                case 5: //SRAIW / SRLIW
                    instr_struct.imm = extract_small_shamt(raw_instr);
                    if (raw_instr & (1 << 30)) {
                        //SRAI
                        instr_struct.mnem = SRAIW;
                    } else {
                        //SRLI
                        instr_struct.mnem = SRLIW;
                    }
                default:
                    not_yet_implemented("Invalid OP_IMM_32 Instruction");
            }
            break;
        case OP_OP_32:
            instr_struct.optype = REG_REG;
            instr_struct.reg_src_2 = extract_rs2(raw_instr);
            if(raw_instr & (1<<25)){
                switch (extract_func3(raw_instr)) {
                    case 0:
                        instr_struct.mnem = MULW;
                        break;
                    case 4:
                        instr_struct.mnem = DIVW;
                        break;
                    case 5:
                        instr_struct.mnem = DIVUW;
                        break;
                    case 6:
                        instr_struct.mnem = REMW;
                        break;
                    case 7:
                        instr_struct.mnem = REMUW;
                        break;
                    default:
                        not_yet_implemented("Invalid OP_32 RV64M Instruction");
                }
            } else {
                switch (extract_func3(raw_instr)) {
                    case 0:
                        if (raw_instr & (1 << 30)) {
                            //SRAI
                            instr_struct.mnem = SUBW;
                        } else {
                            //SRLI
                            instr_struct.mnem = ADDW;
                        }
                        break;
                    case 1:
                        instr_struct.mnem = SLLW;
                        break;
                    case 5:
                        if (raw_instr & (1 << 30)) {
                            //SRAI
                            instr_struct.mnem = SRAW;
                        } else {
                            //SRLI
                            instr_struct.mnem = SRLW;
                        }
                        break;
                    default:
                        not_yet_implemented("Invalid OP_32 Instruction");
                }
            }
            break;
        case OP_OP_IMM:
            parse_OP_IMM(&raw_instr);
            instr_struct.optype = IMMEDIATE;
            switch (extract_func3(raw_instr)) {
                case 0: //ADDI
                    instr_struct.mnem = ADDI;
                    instr_struct.imm = extract_imm_I(raw_instr);
                    break;
                case 1: //SLLI opcode and func3 are unique
                    instr_struct.mnem = SLLI;
                    instr_struct.imm = extract_big_shamt(raw_instr);
                    break;
                case 2: //SLTI
                    instr_struct.mnem = SLTI;
                    instr_struct.imm = extract_imm_I(raw_instr);
                    break;
                case 3: //SLTIU
                    instr_struct.mnem = SLTIU;
                    instr_struct.imm = extract_imm_I(raw_instr);
                    break;
                case 4:
                    instr_struct.mnem = XORI;
                    instr_struct.imm = extract_imm_I(raw_instr);
                    break;
                case 5: //SRAI / SRLI
                    instr_struct.imm = extract_big_shamt(raw_instr);
                    if (raw_instr & (1 << 30)) {
                        //SRAI
                        instr_struct.mnem = SRAI;
                    } else {
                        //SRLI
                        instr_struct.mnem = SRLI;
                    }
                    break;
                case 6:
                    instr_struct.mnem = XORI;
                    instr_struct.imm = extract_imm_I(raw_instr);
                    break;
                case 7:
                    instr_struct.mnem = ANDI;
                    instr_struct.imm = extract_imm_I(raw_instr);
                    break;
                default: {
                    not_yet_implemented("Invalid OP_IMM Instruction");
                }
            }
            break;
        default:
            not_yet_implemented("Opcode Not Implemented");
    }
}

t_parse_result parse_LUI(int32_t* instruction) {
    //extract imm [31:12] 20 bits
    // fills rd with the upper 20 bits and fills lower 12 bits with zeros
    printf("LUI rd: %d, imm32: %#010x\n", extract_rd(*instruction), extract_imm_U(*instruction));
}

t_parse_result parse_OP_IMM(int32_t* instruction) {
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
            not_yet_implemented("Opcode not Implemented");
        }
    }
}

t_parse_result parse_OP_IMM_32(int32_t* instruction) {
    // extract func 3 bits[14:12]
    switch (extract_func3(*instruction)) {
        case 0: {
            //extract imm bits[31:20] 12 bits we need sign extension!
            printf("ADDIW rd: %d, rs1 %d, imm %d\n", extract_rd(*instruction), extract_rs1(*instruction),
                   extract_imm_I(*instruction));
            break;
        }
        default: {
            not_yet_implemented("Opcode not Implemented");
            break;
        }
    }
}
