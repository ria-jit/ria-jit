//
// Created by Flo Schmidt on 12.09.20.
//

#include "optimize.h"

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
        default:
            return;
    }
}
