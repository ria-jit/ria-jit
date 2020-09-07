//
// Created by noah on 08.07.20.
//

#include <util/log.h>
#include <util/typedefs.h>
#include <parser/parser.h>
#include <elf/loadElf.h>
#include <common.h>
#include <stdbool.h>

void add_instruction(t_risc_addr addr, uint64_t *mnem_count, uint64_t *reg_count);

void analyze(const char *file_path) {

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
    for (int i = 0; i < N_MNEM; i++) {
        mnem[i] = 0;
    }

    //create array for registers
    uint64_t reg[N_REG];
    for (int i = 0; i < N_REG; i++) {
        reg[i] = 0;
    }

    //loop over full segment
    for (t_risc_addr addr = startAddr; addr < endAddr; addr += 4) {
        add_instruction(addr, mnem, reg);
    }

    ///rank mnem by usage
    int mnemRanked[N_MNEM];
    for (int i = 0; i < N_MNEM; i++) {
        mnemRanked[i] = i;
    }
    ///insertion sort:
    {
        int key, j;
        for (int i = 1; i < N_MNEM; i++) {
            key = mnemRanked[i];
            j = i - 1;

            ///move move elements with index < i && element > i one to the left
            while (j >= 0 && mnem[mnemRanked[j]] < mnem[key]) {
                mnemRanked[j + 1] = mnemRanked[j];
                j--;
            }

            ///insert former element i to correct position
            mnemRanked[j + 1] = key;
        }
    }

    //rank registers by usage
    int regRanked[N_REG];
    for (int i = 0; i < N_REG; i++) {
        regRanked[i] = i;
    }
    ///insertion sort:
    {
        int key, j;
        for (int i = 1; i < N_REG; i++) {
            key = regRanked[i];
            j = i - 1;

            ///move move elements with index < i && element > i one to the left
            while (j >= 0 && reg[regRanked[j]] < reg[key]) {
                regRanked[j + 1] = regRanked[j];
                j--;
            }

            ///insert former element i to correct position
            regRanked[j + 1] = key;
        }
    }

    log_analyze("Mnemonics…\n");
    log_analyze("==========\n");
    for (int i = 0; i < N_MNEM; i++) {
        if (mnem[mnemRanked[i]] == 0) break;
        log_analyze("Mnem %s is used %li times.\n", mnem_to_string(mnemRanked[i]), mnem[mnemRanked[i]]);
    }
    log_analyze("\n");

    log_analyze("Registers…\n");
    log_analyze("==========\n");
    for (int i = 0; i < N_REG; i++) {
        if (reg[regRanked[i]] == 0) break;
        log_analyze("Register %s (%s) is used %li times.\n", reg_to_string(regRanked[i]), reg_to_alias(regRanked[i]),
                    reg[regRanked[i]]);
    }

    //check for unsupported instructions

    //CSRR
    //CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI,
    bool usesCSRR = false;
    for (int i = CSRRW; i <= CSRRCI; i++) {
        if (mnem[i] != 0) {
            usesCSRR = true;
            break;
        }
    }



    //---RV32A---
    //LRW, SCW, AMOSWAPW, AMOADDW, AMOXORW, AMOANDW, AMOORW, AMOMINW, AMOMAXW, AMOMINUW, AMOMAXUW,
    bool usesRV32A = false;
    for (int i = LRW; i <= AMOMAXUW; i++) {
        if (mnem[i] != 0) {
            usesRV32A = true;
            break;
        }
    }

    //---RV64A---
    //LRD, SCD, AMOSWAPD, AMOADDD, AMOXORD, AMOANDD, AMOORD, AMOMIND, AMOMAXD, AMOMINUD, AMOMAXUD
    bool usesRV64A = false;
    for (int i = LRD; i <= AMOMAXUD; i++) {
        if (mnem[i] != 0) {
            usesRV64A = true;
            break;
        }
    }

    if (usesCSRR) log_analyze("Warning: Guest binary uses CSSR!\n");
    if (usesRV32A) log_analyze("Warning: Guest binary uses RV32A!\n");
    if (usesRV64A) log_analyze("Warning: Guest binary uses RV64A!\n");
}

void add_instruction(t_risc_addr addr, uint64_t *mnem_count, uint64_t *reg_count) {
    t_risc_instr risc_instr = {0};
    risc_instr.addr = addr;

    uint32_t disregard[N_REG]; //dont care
    parse_instruction(&risc_instr, disregard);

    //update statistics
    mnem_count[risc_instr.mnem]++;
    reg_count[risc_instr.reg_src_1]++;
    reg_count[risc_instr.reg_src_2]++;
    reg_count[risc_instr.reg_dest]++;
}
