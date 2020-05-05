//
// Created by jhne on 5/5/20.
//

#include "blockloader.h"

int get_block(t_risc_instr* block_cache, t_risc_addr risc_addr) {

    int instructions_in_block = 0;

    while(1) {

        //call to instruction-block-creating function here, output -> block_cache

        instructions_in_block++;
        if (block_cache->optype == BRANCH) { //BEQ, BNE, BLT, BGE, BLTU, BGEU
            return instructions_in_block;
        }

        if (block_cache->optype == JUMP) {   //JAL, JALR
            if(block_cache->mnem == JAL) {
                //
                risc_addr += (signed long) (parse_jump_immediate(block_cache)); //left shift???

            } else if(block_cache->mnem == JALR) {
                //i need riscv-register access to go on here...
                return instructions_in_block;

            } else {
                //should not get here
            }
        }
        else {
            //next instruction address
            risc_addr += 4;
        }
    }
}

