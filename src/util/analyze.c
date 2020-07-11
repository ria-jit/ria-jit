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

    for(int i = 0;i<N_MNEM;i++){
        if(mnem[indicesRanked[i]] == 0) break;
        log_analyze("Mnem %s is used %li times.\n", to_mnem(indicesRanked[i]),mnem[indicesRanked[i]]);
    }
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
    switch(mnem){
        case LUI:
            return "LUI";
        case AUIPC:
            return "AUIPC";
        case JAL:
            return "JAL";
        case JALR:
            return "JALR";
        case BEQ:
            return "BEQ";
        case BNE:
            return "BNE";
        case BLT:
            return "BLT";
        case BGE:
            return "BGE";
        case BLTU:
            return "BLTU";
        case BGEU:
            return "BGEU";
        case LB:
            return "LB";
        case LH:
            return "LH";
        case LW:
            return "LW";
        case LBU:
            return "LBU";
        case LHU:
            return "LHU";
        case SB:
            return "SB";
        case SH:
            return "SH";
        case SW:
            return "SW";
        case ADDI:
            return "ADDI";
        case SLTI:
            return "SLTI";
        case SLTIU:
            return "SLTIU";
        case XORI:
            return "XORI";
        case ORI:
            return "ORI";
        case ANDI:
            return "ANDI";
        case SLLI:
            return "SLLI";
        case SRLI:
            return "SRLI";
        case SRAI:
            return "SRAI";
        case ADD:
            return "ADD";
        case SUB:
            return "SUB";
        case SLL:
            return "SLL";
        case SLT:
            return "SLT";
        case SLTU:
            return "SLTU";
        case XOR:
            return "XOR";
        case SRL:
            return "SRL";
        case SRA:
            return "SRA";
        case OR:
            return "OR";
        case AND:
            return "AND";
        case FENCE:
            return "FENCE";
        case ECALL:
            return "ECALL";
        case EBREAK:
            return "EBREAK";
        case FENCE_I:
            return "FENCE_I";
        case CSRRW:
            return "CSRRW";
        case CSRRS:
            return "CSRRS";
        case CSRRC:
            return "CSRRC";
        case CSRRWI:
            return "CSRRWI";
        case CSRRSI:
            return "CSRRSI";
        case CSRRCI:
            return "CSRRCI";
        case LWU:
            return "LWU";
        case LD:
            return "LD";
        case SD:
            return "SD";
        case ADDIW:
            return "ADDIW";
        case SLLIW:
            return "SLLIW";
        case SRLIW:
            return "SRLIW";
        case SRAIW:
            return "SRAIW";
        case ADDW:
            return "ADDW";
        case SUBW:
            return "SUBW";
        case SLLW:
            return "SLLW";
        case SRLW:
            return "SRLW";
        case SRAW:
            return "SRAW";
        case MUL:
            return "MUL";
        case MULH:
            return "MULH";
        case MULHSU:
            return "MULHSU";
        case MULHU:
            return "MULHU";
        case DIV:
            return "DIV";
        case DIVU:
            return "DIVU";
        case REM:
            return "REM";
        case REMU:
            return "REMU";
        case MULW:
            return "MULW";
        case DIVW:
            return "DIVW";
        case DIVUW:
            return "DIVUW";
        case REMW:
            return "REMW";
        case REMUW:
            return "REMUW";
        case LRW:
            return "LRW";
        case SCW:
            return "SCW";
        case AMOSWAPW:
            return "AMOSWAPW";
        case AMOADDW:
            return "AMOADDW";
        case AMOXORW:
            return "AMOXORW";
        case AMOANDW:
            return "AMOANDW";
        case AMOORW:
            return "AMOORW";
        case AMOMINW:
            return "AMOMINW";
        case AMOMAXW:
            return "AMOMAXW";
        case AMOMINUW:
            return "AMOMAXW";
        case AMOMAXUW:
            return "AMOMAXUW";
        case LRD:
            return "LRD";
        case SCD:
            return "SCD";
        case AMOSWAPD:
            return "AMOSWAPD";
        case AMOADDD:
            return "AMOADDD";
        case AMOXORD:
            return "AMOXORD";
        case AMOANDD:
            return "AMOANDD";
        case AMOORD:
            return "AMOORD";
        case AMOMIND:
            return "AMOMIND";
        case AMOMAXD:
            return "AMOMAXD";
        case AMOMINUD:
            return "AMOMINUD";
        case AMOMAXUD:
            return "AMOMAXUD";
        default:
            return "UNKNOWN";
    }
}


char* to_opcode(int opcode){
    return "";
}