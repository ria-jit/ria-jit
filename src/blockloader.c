//
// Created by jhne on 5/5/20.
//

//------------------------------------OBSOLETE----------------------------------------
//this functionality is now in translate, will remove file later

#include "blockloader.h"

int get_block(t_risc_instr* block_cache, t_risc_addr risc_addr) {

    int instructions_in_block = 0;

    while(1) {

        //get the raw contents of the instruction at address risc_addr somehow
        t_risc_raw_instr rawInstr = 0x38537; //value for testing

        block_cache->addr = risc_addr;
        block_cache->raw_bytes = rawInstr;

        parse_instruction(block_cache);

        instructions_in_block++;

        //branch?
        if (block_cache->optype == BRANCH) { //BEQ, BNE, BLT, BGE, BLTU, BGEU
            return instructions_in_block;
        }

        //unconditional jump? -> follow
        if (block_cache->optype == JUMP) {   //JAL, JALR
            if(block_cache->mnem == JAL) {
                //
                risc_addr += block_cache->imm;//(signed long) (parse_jump_immediate(block_cache)); //left shift???

            } else if(block_cache->mnem == JALR) {
                //i need riscv-register access to go on here...
                return instructions_in_block;

            } else {
                //should not get here
            }
        }

        //no jump or branch -> continue fetching
        else {
            //next instruction address
            risc_addr += 4;
        }

        block_cache++;
    }
}

