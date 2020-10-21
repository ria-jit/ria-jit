//
// Created by Flo Schmidt on 12.09.20.
//

#include "optimize.h"
#include <gen/instr/patterns.h>

/**
 * pattern matching
*/
void optimize_patterns(t_risc_instr block_cache[], int len) {
    for(int i = 0; patterns[i].len > 0; i++) {
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
                        if (block_cache[j + k].reg_src_1 !=
                                block_cache[j + patterns[i].elements[k].h1].reg_dest) {
                            goto MISMATCH;
                        }
                    }
                        break;

                    case not_rd_h1 : {
                        if (block_cache[j + k].reg_src_1 ==
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

                    case rd_h2 : {
                        if(block_cache[j + k].reg_src_2 !=
                                block_cache[j + patterns[i].elements[k].h2].reg_dest) {
                            goto MISMATCH;
                        }
                    }   break;

                    default: {
                        if(block_cache[j + k].reg_src_2 != patterns[i].elements[k].rs2) {
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

                    case rd_h2 : {
                        if(block_cache[j + k].reg_dest !=
                                block_cache[j + patterns[i].elements[k].h2].reg_dest) {
                            goto MISMATCH;
                        }
                    }
                        break;

                    case rs1_h1 : {
                        if (block_cache[j + k].reg_dest !=
                                block_cache[j + patterns[i].elements[k].h1].reg_src_1) {
                            goto MISMATCH;
                        }
                    }
                        break;

                    case not_rd_h1 : {
                        if (block_cache[j + k].reg_dest ==
                                block_cache[j + patterns[i].elements[k].h1].reg_dest) {
                            goto MISMATCH;
                        }
                    }
                        break;

                    default: {
                        if (block_cache[j + k].reg_dest != patterns[i].elements[k].rd) {
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
                    } break;

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
            j = j + patterns[i].len - 1;

            MISMATCH:;
        }
    }
}

void translate_pattern_emit(t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate Pattern...\n");

    patterns[instr->optype].emitter(instr, r_info);

    for(int i = 1; i < patterns[instr->optype].len; i++) {
        instr[i].mnem = SILENT_NOP;
    }
}
