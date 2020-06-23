//
// Created by noah on 29.04.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_PARSER_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_PARSER_H

#include <util.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OP_LOAD = 0,
    OP_LOAD_FP = 1,
    OP_MISC_MEM = 3,
    OP_OP_IMM = 4,
    OP_AUIPC = 5,
    OP_OP_IMM_32 = 6,
    OP_STORE = 8,
    OP_STORE_FP = 9,
    OP_AMO = 11,
    OP_OP = 12,
    OP_LUI = 13,
    OP_OP_32 = 14,
    OP_MADD = 16,
    OP_MSUB = 17,
    OP_NMSUB = 18,
    OP_NMADD = 19,
    OP_OP_FP = 20,
    OP_BRANCH = 24,
    OP_JALR = 25,
    OP_JAL = 27,
    OP_SYSTEM = 28,
} t_opcodes;

/**
 * Parse given raw_instruction and save all data in the struct.
 * @param instr_struct a pointer to the struct with a raw address to be filled
 * @param reg_count uint32_t[N_REG] structure to count the register use
 */
void parse_instruction(t_risc_instr *instr_struct, uint32_t* reg_count);

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_PARSER_H
