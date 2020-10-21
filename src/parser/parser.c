//
// Created by noah on 29.04.20.
// Based on RISC-V-Spec.pdf ../documentation/RISC-V-Spec.pdf
// For register assembly conventions look at page 137
//

#include <util/log.h>
#include "parser.h"

// extract rd register number bit[11:7]
static inline int extract_rd(int32_t instr) {return instr >> 7 & 0b11111;}

// extract rs1 register number bit[19:15]
static inline int extract_rs1(int32_t instr) {return instr >> 15 & 0b11111;}

// extract rs2 register number bit[24:20]
static inline int extract_rs2(int32_t instr) {return instr >> 20 & 0b11111;}

// extract rs3 register number bit[31:27]
static inline int extract_rs3(int32_t instr) {return instr >> 27 & 0b11111;}

// extract func2 bit [26:25]
static inline int extract_funct2(int32_t instr) {return instr >> 25 & 0b11;}

// extract func3 bit [14:12]
static inline int extract_funct3(int32_t instr) {return instr >> 12 & 0b111;}

// extract func7 bit [31:25]
static inline int extract_funct7(int32_t instr) {return instr >> 25 & 0b1111111;}

// extract big_shamt bit[25:20]
static inline int extract_big_shamt(int32_t instr) {return instr >> 20 & 0b111111;}

// extract small_shamt bit[24:20] -> basically the same as rs2
static inline int extract_small_shamt(int32_t instr) {return instr >> 20 & 0b11111;}

// extract U-Type immediate bit[31:12] -> mask lower 12 bit [11:0] with zeros
static inline int extract_imm_U(int32_t instr) {return instr & ~(0xfff);}

// extract I-Type immediate bit[31:20]
static inline int extract_imm_I(int32_t instr) {return instr >> 20;} //sign extend!

// extract S-Type immediate bit[31:25] + [11:7] => 7bits + 5 bits
static inline int extract_imm_S(int32_t instr) {return (instr >> 20 & ~0b11111) | (instr >> 7 & 0b11111);}

// extract J-Type immediate bits[31:12] order: [20|10:1|11|19:12]
// sign extended because jump address is pc relative
// [20] => [31], [10:1] => [30:21], [11] => [20], [19:12] => [19:12]

static inline int extract_imm_J(int32_t instr) {
    return (instr & 0xff000) | (instr >> (20 - 11) & (1 << 11)) | (instr >> 11 & (1 << 20)) |
            ((signed) instr >> (30 - 10) & 0xffe007fe);
}

// extract B-Type immediate bits[31:25],[11:7] order: [12|10:5],[4:1|11]
static inline int32_t extract_imm_B(int32_t instr) {
    return (instr >> (31 - 12) & 0xfffff000) | (instr << (11 - 7) & (1 << 11)) |
            (instr >> (30 - 10) & 0b11111100000) | (instr >> (11 - 4) & 0b11110);
}

int32_t set_error_message(t_risc_instr *p_instr_struct, int32_t error_code) {
    p_instr_struct->optype = INVALID_INSTRUCTION;
    p_instr_struct->mnem = INVALID_MNEM;
    p_instr_struct->reg_dest = error_code;
    p_instr_struct->imm = *(int32_t *) p_instr_struct->addr; //cast and dereference
    return error_code;
}

/**
 *
 * @param p_instr_struct struct filled with the addr of the instruction to be translated
 */
int32_t parse_instruction(t_risc_instr *p_instr_struct) {
    // print out the line to parse in grouped binary as in the spec
    int32_t raw_instr = *(int32_t *) p_instr_struct->addr; //cast and dereference
    log_asm_in("Parsing 0x%x at %p\n", raw_instr, (void *) p_instr_struct->addr);

    //fill basic struct
    p_instr_struct->reg_dest = (t_risc_reg) extract_rd(raw_instr);
    p_instr_struct->reg_src_1 = (t_risc_reg) extract_rs1(raw_instr);
    p_instr_struct->reg_src_2 = INVALID_REG; //Set to not used value for analyzer to work correctly

    //extract opcode bits[6:2]
    t_opcodes opcode = (t_opcodes) (raw_instr >> 2 & 0x1f);
    switch (opcode) {
        case OP_LUI:
            p_instr_struct->optype = UPPER_IMMEDIATE;
            p_instr_struct->mnem = LUI;
            p_instr_struct->reg_src_1 = INVALID_REG; //Set to not used value for analyzer to work correctly
            p_instr_struct->imm = extract_imm_U(raw_instr);
            break;
        case OP_AUIPC:
            p_instr_struct->optype = IMMEDIATE;
            p_instr_struct->mnem = AUIPC;
            p_instr_struct->reg_src_1 = INVALID_REG; //Set to not used value for analyzer to work correctly
            p_instr_struct->imm = extract_imm_U(raw_instr);
            break;
        case OP_JAL:
            p_instr_struct->optype = JUMP;
            p_instr_struct->mnem = JAL;
            p_instr_struct->reg_src_1 = INVALID_REG; //Set to not used value for analyzer to work correctly
            p_instr_struct->imm = extract_imm_J(raw_instr);
            break;
        case OP_JALR:
            p_instr_struct->optype = JUMP;
            p_instr_struct->mnem = JALR;
            p_instr_struct->imm = extract_imm_I(raw_instr);
            break;
        case OP_LOAD_FP:
            p_instr_struct->optype = FLOAT;
            p_instr_struct->imm = extract_imm_I(raw_instr);
            switch (extract_funct3(raw_instr)) {
                case 2:
                    p_instr_struct->mnem = FLW;
                    break;
                case 3:
                    p_instr_struct->mnem = FLD;
                    break;
                default:
                    critical_not_yet_implemented("Invalid func3 for OP_LOAD_FP Opcode");
            }
            break;
        case OP_STORE_FP:
            p_instr_struct->optype = FLOAT;
            p_instr_struct->imm = extract_imm_S(raw_instr);
            p_instr_struct->reg_src_2 = extract_rs2(raw_instr);
            switch (extract_funct3(raw_instr)) {
                case 2:
                    p_instr_struct->mnem = FSW;
                    break;
                case 3:
                    p_instr_struct->mnem = FSD;
                    break;
                default:
                    critical_not_yet_implemented("Invalid func3 for OP_LOAD_FP Opcode");
            }
            break;
        case OP_MISC_MEM:
            p_instr_struct->optype = SYSTEM;
            p_instr_struct->imm = extract_imm_I(raw_instr);
            switch (extract_funct3(raw_instr)) {
                case 0:
                    p_instr_struct->mnem = FENCE;
                    break;
                case 1:
                    p_instr_struct->mnem = FENCE_I;
                    break;
                default:
                    return set_error_message(p_instr_struct, E_f3_MISC_MEM);
            }
            break;
        case OP_BRANCH:
            // BEQ, BNE...
            p_instr_struct->optype = BRANCH;
            p_instr_struct->reg_src_2 = (t_risc_reg) extract_rs2(raw_instr);
            p_instr_struct->reg_dest = INVALID_REG; //Set to not used value for analyzer to work correctly
            p_instr_struct->imm = extract_imm_B(raw_instr);
            switch (extract_funct3(raw_instr)) {
                case 0:
                    p_instr_struct->mnem = BEQ;
                    break;
                case 1:
                    p_instr_struct->mnem = BNE;
                    break;
                case 4:
                    p_instr_struct->mnem = BLT;
                    break;
                case 5:
                    p_instr_struct->mnem = BGE;
                    break;
                case 6:
                    p_instr_struct->mnem = BLTU;
                    break;
                case 7:
                    p_instr_struct->mnem = BGEU;
                    break;
                default:
                    return set_error_message(p_instr_struct, E_f3_BRANCH);
            }
            break;
        case OP_LOAD:
            p_instr_struct->optype = IMMEDIATE;
            p_instr_struct->imm = extract_imm_I(raw_instr);
            switch (extract_funct3(raw_instr)) {
                case 0: {
                    p_instr_struct->mnem = LB;
                    break;
                }
                case 1: {
                    p_instr_struct->mnem = LH;
                    break;
                }
                case 2: {
                    p_instr_struct->mnem = LW;
                    break;
                }
                case 3: {
                    p_instr_struct->mnem = LD;
                    break;
                }
                case 4: {
                    p_instr_struct->mnem = LBU;
                    break;
                }
                case 5: {
                    p_instr_struct->mnem = LHU;
                    break;
                }
                case 6: {
                    p_instr_struct->mnem = LWU;
                    break;
                }
                default: {
                    //int error = extract_funct3(raw_instr); (could potentially output this?)
                    return set_error_message(p_instr_struct, E_f3_LOAD);
                }
            }
            break;
        case OP_STORE:
            p_instr_struct->optype = STORE;
            p_instr_struct->imm = extract_imm_S(raw_instr);
            p_instr_struct->reg_dest = INVALID_REG; //Set to not used value for analyzer to work correctly
            p_instr_struct->reg_src_2 = (t_risc_reg) extract_rs2(raw_instr);
            switch (extract_funct3(raw_instr)) {
                case 0:
                    p_instr_struct->mnem = SB;
                    break;
                case 1:
                    p_instr_struct->mnem = SH;
                    break;
                case 2:
                    p_instr_struct->mnem = SW;
                    break;
                case 3:
                    p_instr_struct->mnem = SD;
                    break;
                default:
                    return set_error_message(p_instr_struct, E_f3_STORE);
            }
            break;
        case OP_OP:
            p_instr_struct->optype = REG_REG;
            p_instr_struct->reg_src_2 = (t_risc_reg) extract_rs2(raw_instr);
            if (raw_instr & (1 << 25)) {
                switch (extract_funct3(raw_instr)) {
                    case 0:
                        p_instr_struct->mnem = MUL;
                        break;
                    case 1:
                        p_instr_struct->mnem = MULH;
                        break;
                    case 2:
                        p_instr_struct->mnem = MULHSU;
                        break;
                    case 3:
                        p_instr_struct->mnem = MULHU;
                        break;
                    case 4:
                        p_instr_struct->mnem = DIV;
                        break;
                    case 5:
                        p_instr_struct->mnem = DIVU;
                        break;
                    case 6:
                        p_instr_struct->mnem = REM;
                        break;
                    case 7:
                        p_instr_struct->mnem = REMU;
                        break;
                    default:
                        return set_error_message(p_instr_struct, E_f3_OP);
                }
            } else {
                switch (extract_funct3(raw_instr)) {
                    case 0:
                        if (raw_instr & (1 << 30)) {
                            //SRAI
                            p_instr_struct->mnem = SUB;
                        } else {
                            //SRLI
                            p_instr_struct->mnem = ADD;
                        }
                        break;
                    case 1:
                        p_instr_struct->mnem = SLL;
                        break;
                    case 2:
                        p_instr_struct->mnem = SLT;
                        break;
                    case 3:
                        p_instr_struct->mnem = SLTU;
                        break;
                    case 4:
                        p_instr_struct->mnem = XOR;
                        break;
                    case 5:
                        if (raw_instr & (1 << 30)) {
                            //SRAI
                            p_instr_struct->mnem = SRA;
                        } else {
                            //SRLI
                            p_instr_struct->mnem = SRL;
                        }
                        break;
                    case 6:
                        p_instr_struct->mnem = OR;
                        break;
                    case 7:
                        p_instr_struct->mnem = AND;
                        break;
                    default:
                        return set_error_message(p_instr_struct, E_f3_OP);
                }
            }
            break;
        case OP_SYSTEM:
            p_instr_struct->optype = SYSTEM;
            p_instr_struct->imm = extract_imm_I(raw_instr);
            switch (extract_funct3(raw_instr)) {
                case 0:
                    if (raw_instr & (1 << 20)) {
                        p_instr_struct->mnem = EBREAK;
                    } else {
                        p_instr_struct->mnem = ECALL;
                    }
                    break;
                case 1:
                    p_instr_struct->mnem = CSRRW;
                    break;
                case 2:
                    p_instr_struct->mnem = CSRRS;
                    break;
                case 3:
                    p_instr_struct->mnem = CSRRC;
                    break;
                case 5:
                    p_instr_struct->mnem = CSRRWI;
                    break;
                case 6:
                    p_instr_struct->mnem = CSRRSI;
                    break;
                case 7:
                    p_instr_struct->mnem = CSRRCI;
                    break;
                default:
                    return set_error_message(p_instr_struct, E_f3_SYSTEM);
            }
            break;
        case OP_OP_IMM_32:
            p_instr_struct->optype = IMMEDIATE;
            switch (extract_funct3(raw_instr)) {
                case 0:
                    p_instr_struct->mnem = ADDIW;
                    p_instr_struct->imm = extract_imm_I(raw_instr);
                    break;
                case 1: //SLLIW opcode and func3 are unique
                    p_instr_struct->mnem = SLLIW;
                    p_instr_struct->imm = extract_small_shamt(raw_instr);
                    break;
                case 5: //SRAIW / SRLIW
                    p_instr_struct->imm = extract_small_shamt(raw_instr);
                    if (raw_instr & (1 << 30)) {
                        //SRAI
                        p_instr_struct->mnem = SRAIW;
                    } else {
                        //SRLI
                        p_instr_struct->mnem = SRLIW;
                    }
                    break;
                default: {
                    return set_error_message(p_instr_struct, E_f3_IMM_32);
                }
            }
            break;
        case OP_OP_32:
            p_instr_struct->optype = REG_REG;
            p_instr_struct->reg_src_2 = (t_risc_reg) extract_rs2(raw_instr);
            if (raw_instr & (1 << 25)) {
                switch (extract_funct3(raw_instr)) {
                    case 0:
                        p_instr_struct->mnem = MULW;
                        break;
                    case 4:
                        p_instr_struct->mnem = DIVW;
                        break;
                    case 5:
                        p_instr_struct->mnem = DIVUW;
                        break;
                    case 6:
                        p_instr_struct->mnem = REMW;
                        break;
                    case 7:
                        p_instr_struct->mnem = REMUW;
                        break;
                    default:
                        return set_error_message(p_instr_struct, E_f3_RV64M);
                }
            } else {
                switch (extract_funct3(raw_instr)) {
                    case 0:
                        if (raw_instr & (1 << 30)) {
                            //SRAI
                            p_instr_struct->mnem = SUBW;
                        } else {
                            //SRLI
                            p_instr_struct->mnem = ADDW;
                        }
                        break;
                    case 1:
                        p_instr_struct->mnem = SLLW;
                        break;
                    case 5:
                        if (raw_instr & (1 << 30)) {
                            //SRAI
                            p_instr_struct->mnem = SRAW;
                        } else {
                            //SRLI
                            p_instr_struct->mnem = SRLW;
                        }
                        break;
                    default:
                        return set_error_message(p_instr_struct, E_f3_32);
                }
            }
            break;
        case OP_OP_IMM:
            p_instr_struct->optype = IMMEDIATE;
            switch (extract_funct3(raw_instr)) {
                case 0: //ADDI
                    p_instr_struct->mnem = ADDI;
                    p_instr_struct->imm = extract_imm_I(raw_instr);
                    break;
                case 1: //SLLI opcode and func3 are unique
                    p_instr_struct->mnem = SLLI;
                    p_instr_struct->imm = extract_big_shamt(raw_instr);
                    break;
                case 2: //SLTI
                    p_instr_struct->mnem = SLTI;
                    p_instr_struct->imm = extract_imm_I(raw_instr);
                    break;
                case 3: //SLTIU
                    p_instr_struct->mnem = SLTIU;
                    p_instr_struct->imm = extract_imm_I(raw_instr);
                    break;
                case 4:
                    p_instr_struct->mnem = XORI;
                    p_instr_struct->imm = extract_imm_I(raw_instr);
                    break;
                case 5: //SRAI / SRLI
                    p_instr_struct->imm = extract_big_shamt(raw_instr);
                    if (raw_instr & (1 << 30)) {
                        //SRAI
                        p_instr_struct->mnem = SRAI;
                    } else {
                        //SRLI
                        p_instr_struct->mnem = SRLI;
                    }
                    break;
                case 6:
                    p_instr_struct->mnem = ORI;
                    p_instr_struct->imm = extract_imm_I(raw_instr);
                    break;
                case 7:
                    p_instr_struct->mnem = ANDI;
                    p_instr_struct->imm = extract_imm_I(raw_instr);
                    break;
                default: {
                    return set_error_message(p_instr_struct, E_f3_IMM);
                }
            }
            break;
        case OP_MADD:
            p_instr_struct->optype = FLOAT;
            p_instr_struct->mnem = FMADDS;
            p_instr_struct->reg_src_2 = extract_rs2(raw_instr);
            p_instr_struct->reg_src_3 = extract_rs3(raw_instr);
            p_instr_struct->rounding_mode = extract_funct3(raw_instr);
            if(p_instr_struct->rounding_mode == RMM) {
                critical_not_yet_implemented("unsupported rounding mode RMM for %s at 0x%lx",
                                             mnem_to_string(p_instr_struct->mnem),
                                             p_instr_struct->addr);
            }
            switch (extract_funct2(raw_instr)) {
                case 0:
                    p_instr_struct->mnem = FMADDS;
                    break;
                case 1:
                    p_instr_struct->mnem = FMADDD;
                    break;
                default:
                    critical_not_yet_implemented("unsupported operand size FMADD");
            }
            break;
        case OP_MSUB:
            p_instr_struct->optype = FLOAT;
            p_instr_struct->reg_src_2 = extract_rs2(raw_instr);
            p_instr_struct->reg_src_3 = extract_rs3(raw_instr);
            p_instr_struct->rounding_mode = extract_funct3(raw_instr);
            if(p_instr_struct->rounding_mode == RMM) {
                critical_not_yet_implemented("unsupported rounding mode RMM for %s at 0x%lx",
                                             mnem_to_string(p_instr_struct->mnem),
                                             p_instr_struct->addr);
            }
            switch (extract_funct2(raw_instr)) {
                case 0:
                    p_instr_struct->mnem = FMSUBS;
                    break;
                case 1:
                    p_instr_struct->mnem = FMSUBD;
                    break;
                default:
                    critical_not_yet_implemented("unsupported operand size FMSUB");
            }
            break;
        case OP_NMADD:
            p_instr_struct->optype = FLOAT;
            p_instr_struct->reg_src_2 = extract_rs2(raw_instr);
            p_instr_struct->reg_src_3 = extract_rs3(raw_instr);
            p_instr_struct->rounding_mode = extract_funct3(raw_instr);
            if(p_instr_struct->rounding_mode == RMM) {
                critical_not_yet_implemented("unsupported rounding mode RMM for %s at 0x%lx",
                                             mnem_to_string(p_instr_struct->mnem),
                                             p_instr_struct->addr);
            }
            switch (extract_funct2(raw_instr)) {
                case 0:
                    p_instr_struct->mnem = FNMADDS;
                    break;
                case 1:
                    p_instr_struct->mnem = FNMADDD;
                    break;
                default:
                    critical_not_yet_implemented("unsupported operand size FNMADD");
            }
            break;
        case OP_NMSUB:
            p_instr_struct->optype = FLOAT;
            p_instr_struct->reg_src_2 = extract_rs2(raw_instr);
            p_instr_struct->reg_src_3 = extract_rs3(raw_instr);
            p_instr_struct->rounding_mode = extract_funct3(raw_instr);
            if(p_instr_struct->rounding_mode == RMM) {
                critical_not_yet_implemented("unsupported rounding mode RMM for %s at 0x%lx",
                                             mnem_to_string(p_instr_struct->mnem),
                                             p_instr_struct->addr);
            }
            switch (extract_funct2(raw_instr)) {
                case 0:
                    p_instr_struct->mnem = FNMSUBS;
                    break;
                case 1:
                    p_instr_struct->mnem = FNMSUBD;
                    break;
                default:
                    critical_not_yet_implemented("unsupported operand size FNMSUB");
            }
            break;
        case OP_OP_FP: {
            p_instr_struct->optype = FLOAT;
            int funct7 = extract_funct7(raw_instr);
            int funct3 = extract_funct3(raw_instr);
            int rs2 = extract_rs2(raw_instr);
            int operandSize = funct7 & 0b11; // lower two bits determine operand size
            // it looks like this bit determines if the rs2 field is used as a rounding mode, or as a funct code
            if ((funct7 & 0x0100000) == 0) {
                p_instr_struct->reg_src_2 = rs2;
            }

            // furthermore the 5 bit determines if the funct3 field is used as a rounding mode, or as a funct code
            if ((funct7 & 0x0010000) == 0) {
                p_instr_struct->rounding_mode = funct3;
                if(p_instr_struct->rounding_mode == RMM) {
                    critical_not_yet_implemented("unsupported rounding mode RMM for %s at 0x%lx",
                                                 mnem_to_string(p_instr_struct->mnem),
                                                 p_instr_struct->addr);
                }
            }

            //ignore lower two bits which only set operand size
            funct7 = funct7 >> 2;
            switch (funct7) {
                case 0:
                    p_instr_struct->mnem = FADDS;
                    break;
                case 1:
                    p_instr_struct->mnem = FSUBS;
                    break;
                case 2:
                    p_instr_struct->mnem = FMULS;
                    break;
                case 3:
                    p_instr_struct->mnem = FDIVS;
                    break;
                case 4:
                    switch (funct3) {
                        case 0:
                            p_instr_struct->mnem = FSGNJS;
                            break;
                        case 1:
                            p_instr_struct->mnem = FSGNJNS;
                            break;
                        case 2:
                            p_instr_struct->mnem = FSGNJXS;
                            break;
                        default:
                            critical_not_yet_implemented("unknown funct3 for FSGNJ");
                    }
                    break;
                case 5:
                    switch (funct3) {
                        case 0:
                            p_instr_struct->mnem = FMINS;
                            break;
                        case 1:
                            p_instr_struct->mnem = FMAXS;
                            break;
                        default:
                            critical_not_yet_implemented("unknown funct3 for funct7=5");
                    }
                    break;
                case 8:
                    switch (rs2) {
                        case 0:
                            p_instr_struct->mnem = FCVTDS;
                            break;
                        case 1:
                            p_instr_struct->mnem = FCVTSD;
                            break;
                        default:
                            critical_not_yet_implemented("unknown funct3 for funct7=8");
                    }
                    break;
                case 11:
                    p_instr_struct->mnem = FSQRTS;
                    break;
                case 20:
                    switch (funct3) {
                        case 0:
                            p_instr_struct->mnem = FLES;
                            break;
                        case 1:
                            p_instr_struct->mnem = FLTS;
                            break;
                        case 2:
                            p_instr_struct->mnem = FEQS;
                            break;
                        default:
                            critical_not_yet_implemented("unknown funct3 for FSGNJ");
                    }
                    break;
                case 24:
                    switch (rs2) {
                        case 0:
                            p_instr_struct->mnem = FCVTWS;
                            break;
                        case 1:
                            p_instr_struct->mnem = FCVTWUS;
                            break;
                        case 2:
                            p_instr_struct->mnem = FCVTLS;
                            break;
                        case 3:
                            p_instr_struct->mnem = FCVTLUS;
                            break;
                        default:
                            critical_not_yet_implemented("unknown funct3 for FCVTW");
                    }
                    break;
                case 26:
                    switch (rs2) {
                        case 0:
                            p_instr_struct->mnem = FCVTSW;
                            break;
                        case 1:
                            p_instr_struct->mnem = FCVTSWU;
                            break;
                        case 2:
                            p_instr_struct->mnem = FCVTSL;
                            break;
                        case 3:
                            p_instr_struct->mnem = FCVTSLU;
                            break;
                        default:
                            critical_not_yet_implemented("unknown funct3 for FCVTS");
                    }
                    break;
                case 28:
                    switch (funct3) {
                        case 0:
                            p_instr_struct->mnem = FMVXW;
                            break;
                        case 1:
                            p_instr_struct->mnem = FCLASSS;
                            break;
                        default:
                            critical_not_yet_implemented("unknown funct3 for funct7=28");
                    }
                    break;
                case 30:
                    p_instr_struct->mnem = FMVWX;
                    break;
                default:
                    critical_not_yet_implemented("unknown funct7 for OP_OP_FP");
                    break;
            }
            if (funct7 != 8) {
                //fit mnem to the operand size
                switch (operandSize) {
                    case 0:
                        //default 16 bit nothing to do
                        break;
                    case 1:
                        //because of our ordering of the mnems in typedef.h we can just add a constant factor
                        p_instr_struct->mnem += FLD - FLW;
                        break;
                    default:
                        critical_not_yet_implemented("unsupported operand size for OP_OP_FP;\n"
                                                     " you are probably using the RV32Q/RV64Q extension");
                }
            }
        }
            break;
        case OP_AMO:
            p_instr_struct->reg_src_2 = (t_risc_reg) extract_rs2(raw_instr);
            p_instr_struct->imm = extract_funct7(raw_instr);
            // switch between different OP_AMO types, which are decoded in upper bytes of funct7
            switch (p_instr_struct->imm >> 2) {
                case 0:
                    p_instr_struct->mnem = AMOADDW;
                    break;
                case 1:
                    p_instr_struct->mnem = AMOSWAPW;
                    break;
                case 2:
                    p_instr_struct->mnem = LRW;
                    break;
                case 3:
                    p_instr_struct->mnem = SCW;
                    break;
                case 4:
                    p_instr_struct->mnem = AMOXORW;
                    break;
                case 8:
                    p_instr_struct->mnem = AMOORW;
                    break;
                case 12:
                    p_instr_struct->mnem = AMOANDW;
                    break;
                case 16:
                    p_instr_struct->mnem = AMOMINW;
                    break;
                case 20:
                    p_instr_struct->mnem = AMOMAXW;
                    break;
                case 24:
                    p_instr_struct->mnem = AMOMINUW;
                    break;
                case 28:
                    p_instr_struct->mnem = AMOMAXUW;
                    break;
                default:
                    return set_error_message(p_instr_struct, E_f7_AMO);
            }
            // funct3 differentiates between RV32A and RV64A
            switch (extract_funct3(raw_instr)) {
                case 2:
                    //RV32A
                    break;
                case 3:
                    //RV64A
                    p_instr_struct->mnem += LRD - LRW;
                    break;
                default:
                    return set_error_message(p_instr_struct, E_f3_AMO);
            }

            break;
        default:
            return set_error_message(p_instr_struct, E_UNKNOWN);
    }
    return 0;
}
