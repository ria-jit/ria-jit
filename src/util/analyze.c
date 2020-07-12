//
// Created by noah on 08.07.20.
//


#include "log.h"
#include "typedefs.h"
#include "parser/parser.h"

#include <common.h>
#include <stdbool.h>
#include <elf/loadElf.h>


void add_instruction(t_risc_addr addr, uint64_t *mnem_count);

char* to_mnem(int mnem);

void analyze(const char *file_path){

    if (file_path == NULL) {
        dprintf(2, "Bad. Invalid file path.\n");
        _exit(2);
    }

    t_risc_elf_map_result result = mapIntoMemory(file_path);
    if (!result.valid) {
        dprintf(2, "Bad. Failed to map into memory.\n");
        _exit(1);
    }

    t_risc_addr startAddr = result.execStart;
    t_risc_addr endAddr = result.execEnd;

    //create array for mnemomics
    uint64_t mnem[N_MNEM];
    for(int i = 0;i<N_MNEM;i++){
        mnem[i] = 0;
    }
    //loop over full segment
    for(t_risc_addr addr = startAddr; addr < endAddr; addr += 4) {
        add_instruction(addr, mnem);
    }

    ///rank mnem by usage

    int indicesRanked[N_MNEM];
    for(int i = 0; i < N_MNEM; i++) {
        indicesRanked[i] = i;
    }
    ///insertion sort:
    {
        int key, j;
        for(int i = 1; i < N_MNEM; i++) {
            key = indicesRanked[i];
            j = i - 1;

            ///move move elements with index < i && element > i one to the left
            while(j >= 0 && mnem[indicesRanked[j]] < mnem[key]) {
                indicesRanked[j + 1] = indicesRanked[j];
                j--;
            }

            ///insert former element i to correct position
            indicesRanked[j + 1] = key;
        }
    }

    for(int i = 0; i < N_MNEM; i++) {
        if (mnem[indicesRanked[i]] == 0) break;
        log_analyze("Mnem %s is used %li times.\n", mnem_to_string(indicesRanked[i]), mnem[indicesRanked[i]]);
    }

    //check for unsupported instructions

    //CSRR
    //CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI,
    bool usesCSRR = false;
    for(int i = CSRRW; i <= CSRRCI; i++) {
        if (mnem[i] != 0) {
            usesCSRR = true;
            break;
        }
    }



    //---RV32A---
    //LRW, SCW, AMOSWAPW, AMOADDW, AMOXORW, AMOANDW, AMOORW, AMOMINW, AMOMAXW, AMOMINUW, AMOMAXUW,
    bool usesRV32A = false;
    for(int i = LRW; i <= AMOMAXUW; i++) {
        if (mnem[i] != 0) {
            usesRV32A = true;
            break;
        }
    }

    //---RV64A---
    //LRD, SCD, AMOSWAPD, AMOADDD, AMOXORD, AMOANDD, AMOORD, AMOMIND, AMOMAXD, AMOMINUD, AMOMAXUD
    bool usesRV64A = false;
    for(int i = LRD; i <= AMOMAXUD; i++) {
        if (mnem[i] != 0) {
            usesRV64A = true;
            break;
        }
    }

    if(usesCSRR) log_analyze("Achtung: CSSR wird von diesem Programm genutzt!\n");
    if(usesRV32A) log_analyze("Achtung: RV32A wird von diesem Programm genutzt!\n");
    if(usesRV64A) log_analyze("Achtung: RV64A wird von diesem Programm genutzt!");
}

void add_instruction(t_risc_addr addr, uint64_t *mnem_count) {
    int32_t raw_instr = *(int32_t *) addr; //cast and dereference
    t_risc_instr risc_instr = {};
    risc_instr.addr = addr;

    uint32_t reg_count[N_REG]; //dont care
    parse_instruction(&risc_instr, reg_count);
    mnem_count[risc_instr.mnem]++;
}