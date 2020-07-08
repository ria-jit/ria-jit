//
// Created by noah on 08.07.20.
//


#include "log.h"
#include "typedefs.h"
#include "parser/parser.h"

#include <common.h>
#include <stdbool.h>


void add_instruction(t_risc_addr addr, uint64_t *mnem_count);

void analyze(t_risc_addr startAddr, t_risc_addr endAddr){
    //create array for mnemomics
    uint64_t mnem[N_MNEM];
    //loop over full segment
    for(t_risc_addr addr = startAddr;addr<endAddr;addr+=4){
        add_instruction(addr,mnem);
    }

    ///rank mnem by usage

    int indicesRanked[N_MNEM];
    for (int i = 0; i < N_MNEM; i++) {
        indicesRanked[i] = i;
    }
    ///insertion sort:
    {
        int key, j;
        for (int i = 1; i < N_MNEM; i++) {
            key = indicesRanked[i];
            j = i - 1;

            ///move move elements with index < i && element > i one to the left
            while (j >= 0 && mnem[indicesRanked[j]] < mnem[key]) {
                indicesRanked[j + 1] = indicesRanked[j];
                j--;
            }

            ///insert former element i to correct position
            indicesRanked[j + 1] = key;
        }
    }

    /// TODO output
}

void add_instruction(t_risc_addr addr, uint64_t *mnem_count) {
    int32_t raw_instr = *(int32_t *) addr; //cast and dereference
    t_risc_instr risc_instr = {};
    risc_instr.addr = addr;

    uint32_t reg_count[N_REG]; //dont care
    parse_instruction(&risc_instr, reg_count);
    mnem_count[risc_instr.mnem]++;
}

char* to_mnem(int mnem){
    return "";
}


char* to_opcode(int opcode){
    return "";
}