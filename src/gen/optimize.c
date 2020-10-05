//
// Created by Flo Schmidt on 12.09.20.
//

#include "optimize.h"
#include <common.h>
#include <gen/translate.h>
#include <gen/instr/patterns.h>

/**
 * Optimize the current instruction (potentially by applying macro operation fusing with the adjacent instructions).
 * @param block_cache the currently translated block's cache
 * @param index current instruction index
 * @param len block length boundary (for checking adjacent instructions)
 */
void optimize_instr(t_risc_instr *block_cache, size_t index, size_t len) {
    switch (block_cache[index].mnem) {
        case ADDI:
            if (block_cache[index].reg_dest == x0 && block_cache[index].reg_src_1 == x0 &&
                    block_cache[index].imm == 0) {
                //addi x0, x0, 0 == nop
                block_cache[index].mnem = NOP;
            } else if (block_cache[index].imm == 0) {
                //addi rd, rs, 0 == mv rd, rs
                block_cache[index].mnem = MV;
            }
            return;
        case ADDIW:
            if (block_cache[index].imm == 0) {
                block_cache[index].mnem = SEXTW;
            }
            return;
        case XORI:
            if (block_cache[index].imm == -1) {
                //xori rd, rs, -1 == not
                block_cache[index].mnem = NOT;
            }
            return;
        case SUB:
            if (block_cache[index].reg_src_1 == x0) {
                //sub rd, x0, rs == neg rd, rs
                block_cache[index].mnem = NEG;
                block_cache[index].reg_src_1 = block_cache[index].reg_src_2;
            }
            return;
        case SUBW:
            if (block_cache[index].reg_src_1 == x0) {
                //subw rd, x0, rs == negw rd, rs
                block_cache[index].mnem = NEGW;
                block_cache[index].reg_src_1 = block_cache[index].reg_src_2;
            }
            return;
        case SLTIU:
            if (block_cache[index].imm == 0) {
                //sltiu rd, rs, 1 == seqz rd, rs
                block_cache[index].mnem = SEQZ;
            }
            return;
        case SLTU:
            if (block_cache[index].reg_src_1 == x0) {
                //sltu rd, x0, rs == snez rd, rs
                block_cache[index].mnem = SNEZ;
                block_cache[index].reg_src_1 = block_cache[index].reg_src_2;
            }
            return;
        case SLT:
            if (block_cache[index].reg_src_1 != x0 && block_cache[index].reg_src_2 == x0) {
                //slt rd, rs, x0 == sltz rd, rs
                block_cache[index].mnem = SLTZ;
            } else if (block_cache[index].reg_src_1 == x0 && block_cache[index].reg_src_2 != x0) {
                //slt rd, x0, rs == sltz rd, rs
                block_cache[index].mnem = SGTZ;
                block_cache[index].reg_src_1 = block_cache[index].reg_src_2;
            }
            return;
        case AUIPC: {
            //auipc rd, imm1 followed by addi rd, rd, imm2 can be fused into a single operation
            if (
                    index + 1 < len &&
                            block_cache[index + 1].mnem == ADDI &&
                            block_cache[index].reg_dest == block_cache[index + 1].reg_dest &&
                            block_cache[index + 1].reg_dest == block_cache[index + 1].reg_src_1
                    ) {
                //fuse instructions, add immediate values, regDest is already set
                block_cache[index].mnem = LI;
                block_cache[index].imm = block_cache[index].addr + block_cache[index].imm + block_cache[index + 1].imm;

                //invalidate following instruction
                block_cache[index + 1].mnem = SILENT_NOP;
            }
            return;
        }
        case LUI: {
            //lui rd, imm1 followed by addi rd, rd, imm2 can be fused into a single operation
            if (
                    index + 1 < len &&
                            block_cache[index + 1].mnem == ADDI &&
                            block_cache[index].reg_dest == block_cache[index + 1].reg_dest &&
                            block_cache[index + 1].reg_dest == block_cache[index + 1].reg_src_1
                    ) {
                //fuse instructions, add immediate values, regDest is already set
                block_cache[index].mnem = LI;
                block_cache[index].imm = block_cache[index].imm + block_cache[index + 1].imm;

                //invalidate following instruction
                block_cache[index + 1].mnem = SILENT_NOP;
            }
            return;
        }
        default:
            return;
    }
}


/**
 * pattern matching
*/
void optimize_patterns(t_risc_instr *block_cache, int len) {
#define PATTERNS_NUM 2

    for(int i = 0; i < PATTERNS_NUM; i++) {
        for(int j = 0; j < len - patterns[i].len; j++) {
            for(int k = 0; k < patterns[i].len; k++) {
                ///mnem match
                if(patterns[i].elements[k].mnem != block_cache[j + k].mnem) {
                    goto MISMATCH;
                }

                ///rs1 match
                switch(patterns[i].elements[k].rs1) {
                    case DONT_CARE : {} break;

                    //uncomment as needed

                    /*
                    case rs1_h1 : {
                        if(block_cache[j + k].reg_src_1 !=
                                block_cache[j + patterns[i].elements[k].h1].reg_src_1) {
                            goto MISMATCH;
                        }
                    } break;
                     */
                    /*
                    case rs2_h1: {
                        if(block_cache[j + k].reg_src_1 !=
                                block_cache[j + patterns[i].elements[k].h1].reg_src_2) {
                            goto MISMATCH;
                        }
                    }
                        break;
                    */

                    case rd_h1: {
                        if(block_cache[j + k].reg_src_1 !=
                                block_cache[j + patterns[i].elements[k].h1].reg_dest) {
                            goto MISMATCH;
                        }
                    }
                        break;

                    /*
                    case rs1_h2: {
                        if(block_cache[j + k].reg_src_1 !=
                                block_cache[j + patterns[i].elements[k].h2].reg_src_1) {
                            goto MISMATCH;
                        }
                    }
                        break;
                    */
                    /*
                    case rs2_h2: {
                        if(block_cache[j + k].reg_src_1 !=
                                block_cache[j + patterns[i].elements[k].h2].reg_src_2) {
                            goto MISMATCH;
                        }
                    }
                        break;
                    */

                    case rd_h2: {
                        if(block_cache[j + k].reg_src_1 !=
                                block_cache[j + patterns[i].elements[k].h2].reg_dest) {
                            goto MISMATCH;
                        }
                    }
                        break;

                    default: {
                        if(block_cache[j + k].reg_src_1 != patterns[i].elements[k].rs1) {
                            goto MISMATCH;
                        }
                    }
                }

                ///rs2 match
                switch (patterns[i].elements[k].rs2) {
                    case DONT_CARE : {} break;

                        //insert as needed

                        /*
                        case rs2_h2 : {
                            if(block_cache[j + k].reg_src_2 !=
                                    block_cache[j + patterns[i].elements[k].h2].reg_src_2) {
                                goto MISMATCH;
                            }
                        } break;
                         */

                    case rd_h1 : {
                        if(block_cache[j + k].reg_src_2 !=
                                block_cache[j + patterns[i].elements[k].h1].reg_dest) {
                            goto MISMATCH;
                        }
                    }   break;

                    default: {
                        if(block_cache[j + k].reg_src_2 != patterns[i].elements[k].rs1) {
                            goto MISMATCH;
                        }
                    }
                }

                ///rd match
                switch (patterns[i].elements[k].rd) {
                    case DONT_CARE : {} break;

                        //insert as needed


                        case rd_h1 : {
                            if(block_cache[j + k].reg_dest !=
                                    block_cache[j + patterns[i].elements[k].h1].reg_dest) {
                                goto MISMATCH;
                            }
                        } break;

                    default: {
                        if(block_cache[j + k].reg_src_2 != patterns[i].elements[k].rs1) {
                            goto MISMATCH;
                        }
                    }
                }

                ///imm match
                switch (patterns[i].elements[k].imm) {
                    case 0 : {} break;

                    case 1 : {
                        if (block_cache[j + k].imm != patterns[i].elements[k].imm_value) {
                            goto MISMATCH;
                        }
                    } break;

                    case 2 : {
                        if (block_cache[j + k].imm != block_cache[j + patterns[i].elements[k].imm_value].imm) {
                            goto MISMATCH;
                        }
                    }

                    default : {
                        goto MISMATCH;
                    }
                }
            }

            ///match:

            ///insert pseudo instruction
            block_cache[j].mnem = PATTERN_EMIT;
            block_cache[j].optype = i;

            //invalidate matched sequence ??

            ///skip to end of pattern
            j = j + patterns[i].len;

            MISMATCH:;
        }
    }

    for(size_t i = 0; i < len; i++) {
        for(int j = 0; j < PATTERNS_NUM; j++) {
            /*
            if(patterns_length[i] > len - i) break;
            for(int k = 0; k < patterns_length[i]; k++) {
                if(block_cache[i + k] != patterns[j][k]) break;
            }

            block_cache[i] = pattern_replacement[j];
            for(int k = 1; k < pattern_length[j]; k++) {
                block_cache[i + k] = SILENT_NOP;
            }
            */
        }
    }
}

void translate_pattern_emit(t_risc_instr *instr, const register_info *r_info) {
    patterns[instr->optype].emitter(instr);

    for(int i = 1; i < patterns[instr->optype].len; i++) {
        instr[i].mnem = SILENT_NOP;
    }
}
