//
// Created by flo on 06.07.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TYPEDEFS_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TYPEDEFS_H

#include <stdint.h>
#include <fadec/fadec-enc.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

char *errorcode_to_string(int mnem);

//the RISC-V mnemonic of the instruction
char *mnem_to_string(int mnem);

char *reg_to_string(int reg);

char *reg_to_alias(int reg);

typedef enum {
    INVALID_MNEM,

    //---RV32I---
    LUI, //load upper Imm
    AUIPC, //register = Imm

    //control flow
    JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU,

    //load & store
    LB, LH, LW, LBU, LHU, SB, SH, SW,

    //Arithmetic
    ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,

    //??????
    FENCE, ECALL, EBREAK, FENCE_I,

    //CSRR
    CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI,

    //---RV64I---
    //load & store
    LWU, LD, SD,

    //Arithmetic
    //SLLI,
    //SRLI,
    //SRAI,
    ADDIW, SLLIW, SRLIW, SRAIW, ADDW, SUBW, SLLW, SRLW, SRAW,

    //---RV32M---
    MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU,

    //---RV64M---
    MULW, DIVW, DIVUW, REMW, REMUW,

    //---RV32A---
    LRW, SCW, AMOSWAPW, AMOADDW, AMOXORW, AMOANDW, AMOORW, AMOMINW, AMOMAXW, AMOMINUW, AMOMAXUW,

    //---RV64A---
    LRD, SCD, AMOSWAPD, AMOADDD, AMOXORD, AMOANDD, AMOORD, AMOMIND, AMOMAXD, AMOMINUD, AMOMAXUD,

    //---RV32F---
    FLW, FSW, FMADDS, FMSUBS, FNMSUBS, FNMADDS, FADDS, FSUBS, FMULS, FDIVS, FSQRTS, FSGNJS, FSGNJNS, FSGNJXS, FMINS, FMAXS, FCVTWS, FCVTWUS, FMVXW, FEQS, FLTS, FLES, FCLASSS, FCVTSW, FCVTSWU, FMVWX,

    //---RV64F---
    FCVTLS, FCVTLUS, FCVTSL, FCVTSLU,

    //---RV32D---
    FLD, FSD, FMADDD, FMSUBD, FNMSUBD, FNMADDD, FADDD, FSUBD, FMULD, FDIVD, FSQRTD, FSGNJD, FSGNJND, FSGNJXD, FMIND, FMAXD, FCVTSD, FCVTDS, FEQD, FLTD, FLED, FCLASSD, FCVTWD, FCVTWUD, FCVTDW, FCVTDWU,

    //---RV64D---
    FCVTLD, FCVTLUD, FMVXD, FCVTDL, FCVTDLU, FMVDX,

    //---PSEUDO---
    PC_NEXT_INST, NOP, SILENT_NOP, MV, NOT, NEG, NEGW, SEXTW, SEQZ, SNEZ, SLTZ, SGTZ, FUSE_AUIPC_ADDI

} t_risc_mnem;
#define N_MNEM (FUSE_AUIPC_ADDI + 1)

typedef enum {
    E_UNKNOWN,
    E_f3_MISC_MEM,
    E_f3_BRANCH,
    E_f3_LOAD,
    E_f3_STORE,
    E_f3_OP,
    E_f3_SYSTEM,
    E_f3_IMM_32,
    E_f3_RV64M,
    E_f3_32,
    E_f3_IMM,
    E_f7_AMO,
    E_f3_AMO
};

//general purpose registers (x1 is ret addr, x2 is sp by convention)
typedef enum {
    x0, //x0 is hardwired to constant 0
    x1, x2, x3, x4, x5, x6, x7, x8, x9,
    x10, x11, x12, x13, x14, x15, x16, x17, x18, x19,
    x20, x21, x22, x23, x24, x25, x26, x27, x28, x29,
    x30, x31,
    pc
} t_risc_reg;

typedef enum {
    zero,
    ra, sp, gp, tp, t0, t1, t2, s0, fp = s0, s1,
    a0, a1, a2, a3, a4, a5, a6, a7, s2, s3,
    s4, s5, s6, s7, s8, s9, s10, s11, t3, t4,
    t5, t6
} t_risc_reg_mnem;
#define N_REG 33

//CSR registers
#define N_CSR 4096
typedef enum {
    //read-write access (floating point)
    csr_fflags = 0x001, //floating point accrued exceptions
    csr_frm = 0x002,    //floating point dynamic rounding mode
    csr_fcsr = 0x003,   //floating point control and status register (frm + fflags)
    //read-only access (counters and timers)
    csr_cycle = 0xC00,  //cycle counter for RDCYCLE
    csr_time = 0xC01,   //timer for RDTIME
    csr_instret = 0xC02,//instructions retired counter for RDINSTRET
    csr_cycleh = 0xC80, //upper 32 bits of cycle (for RV32I)
    csr_timeh = 0xC81,  //upper 32 bits of time (for RV32I)
    csr_instreth = 0xC82//upper 32 bits of instret (for RV32I)
} t_risc_csr_reg;

//register value type
typedef uint64_t t_risc_reg_val;

//RISC-V operation types (for later optimization)
typedef enum {
    REG_REG, IMMEDIATE, UPPER_IMMEDIATE, STORE, BRANCH, JUMP, SYSTEM, INVALID_INSTRUCTION, INVALID_BLOCK, PSEUDO
} t_risc_optype;

//carry immediate values in the instruction struct
typedef int64_t t_risc_imm;

//carry a pointer to the raw instruction in the struct
typedef uintptr_t t_risc_addr;

// Idea
typedef struct {
    uint32_t reg_count[N_REG];
    uint32_t reg_map[N_REG];
} t_register_allocation;

typedef struct {
    t_risc_addr addr;
    t_risc_mnem mnem;
    t_risc_optype optype;
    t_risc_reg reg_src_1;
    t_risc_reg reg_src_2;
    t_risc_reg reg_dest;
    t_risc_imm imm;
} t_risc_instr;

/**
 * Register information for the translator functions.
 */
typedef struct {
    FeReg *map;
    bool *mapped;
    uint64_t base;
    uint64_t csr_base;
} register_info;

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TYPEDEFS_H
