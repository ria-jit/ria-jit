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

char *reg_x86_to_string(FeReg reg);

typedef enum {
    INVALID_MNEM,

    //---RV32I---
    LUI, //load upper Imm
    AUIPC, //register = Imm

    //control flow
    JAL,
    JALR,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,

    //load & store
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,

    //Arithmetic
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,

    //??????
    FENCE,
    ECALL,
    EBREAK,
    FENCE_I,

    //CSRR
    CSRRW,
    CSRRS,
    CSRRC,
    CSRRWI,
    CSRRSI,
    CSRRCI,

    //---RV64I---
    //load & store
    LWU,
    LD,
    SD,

    //Arithmetic
    //SLLI,
    //SRLI,
    //SRAI,
    ADDIW,
    SLLIW,
    SRLIW,
    SRAIW,
    ADDW,
    SUBW,
    SLLW,
    SRLW,
    SRAW,

    //---RV32M---
    MUL,
    MULH,
    MULHSU,
    MULHU,
    DIV,
    DIVU,
    REM,
    REMU,

    //---RV64M---
    MULW,
    DIVW,
    DIVUW,
    REMW,
    REMUW,

    //---RV32A---
    LRW,
    SCW,
    AMOSWAPW,
    AMOADDW,
    AMOXORW,
    AMOANDW,
    AMOORW,
    AMOMINW,
    AMOMAXW,
    AMOMINUW,
    AMOMAXUW,

    //---RV64A---
    LRD,
    SCD,
    AMOSWAPD,
    AMOADDD,
    AMOXORD,
    AMOANDD,
    AMOORD,
    AMOMIND,
    AMOMAXD,
    AMOMINUD,
    AMOMAXUD,

    //---RV32F---
    FLW,
    FSW,
    FMADDS,
    FMSUBS,
    FNMSUBS,
    FNMADDS,
    FADDS,
    FSUBS,
    FMULS,
    FDIVS,
    FSQRTS,
    FSGNJS,
    FSGNJNS,
    FSGNJXS,
    FMINS,
    FMAXS,
    FCVTWS,
    FCVTWUS,
    FMVXW,
    FEQS,
    FLTS,
    FLES,
    FCLASSS,
    FCVTSW,
    FCVTSWU,
    FMVWX,

    //---RV64F---
    FCVTLS,
    FCVTLUS,
    FCVTSL,
    FCVTSLU,

    //---RV32D--- + //---RV64D--- //reordering for easier parser design
    FLD,
    FSD,
    FMADDD,
    FMSUBD,
    FNMSUBD,
    FNMADDD,
    FADDD,
    FSUBD,
    FMULD,
    FDIVD,
    FSQRTD,
    FSGNJD,
    FSGNJND,
    FSGNJXD,
    FMIND,
    FMAXD,
    FCVTWD,
    FCVTWUD,
    FMVXD,
    FEQD,
    FLTD,
    FLED,
    FCLASSD,
    FCVTDW,
    FCVTDWU,
    FMVDX,
    FCVTLD,
    FCVTLUD,
    FCVTDL,
    FCVTDLU,
    FCVTSD,
    FCVTDS,

    //---PSEUDO---
    PC_NEXT_INST,
    SILENT_NOP,
    PATTERN_EMIT,

    //To always have a count, don't insert below here
    LAST_MNEM

} t_risc_mnem;
#define N_MNEM (LAST_MNEM)

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
} t_error_enum;

//general purpose registers (x1 is ret addr, x2 is sp by convention)
typedef enum {
    x0, //x0 is hardwired to constant 0
    x1, x2, x3, x4, x5, x6, x7, x8, x9,
    x10, x11, x12, x13, x14, x15, x16, x17, x18, x19,
    x20, x21, x22, x23, x24, x25, x26, x27, x28, x29,
    x30, x31,
    pc
} t_risc_reg;

#define INVALID_REG 33

typedef enum {
    zero,
    ra, sp, gp, tp, t0, t1, t2, s0, fp = s0, s1,
    a0, a1, a2, a3, a4, a5, a6, a7, s2, s3,
    s4, s5, s6, s7, s8, s9, s10, s11, t3, t4,
    t5, t6
} t_risc_reg_mnem;

#define N_REG 34
#define N_REPLACE 3

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

//floating point registers
#define N_FP 32
typedef enum {
    f0, f1, f2, f3, f4, f5, f6, f7, f8, f9,
    f10, f11, f12, f13, f14, f15, f16, f17, f18, f19,
    f20, f21, f22, f23, f24, f25, f26, f27, f28, f29,
    f30, f31
} t_risc_fp_reg;

typedef enum {
    RNE = 0, //Round to Nearest, ties to Even
    RTZ = 1, //Round towards Zero
    RDN = 2, //Round Down (towards negative infinity)
    RUP = 3, //Round Up (towards positive infinity)
    RMM = 4, //Round to Nearest, ties to Max Magnitude
    DYN = 7  //In instruction's rm field, selects dynamic rounding mode; In Rounding Mode register, Invalid.
} t_risc_rm;

typedef enum {
    NV = 4, //Invalid Operation
    DZ = 3, //Divide by Zero
    OF = 2, //Overflow
    UF = 1, //Underflow
    NX = 0 //Inexact
} t_risc_f_exp;

/**
 * Register value type.
 * XLEN = 64 for the integer registers.
 */
typedef uint64_t t_risc_reg_val;

/**
 * Float Register value type.
 * For floating point, F-extension requires FLEN = 32-bit, D-extension requires 64-bit.
 */
typedef union {
    float f;
    double d;
    uint64_t i;
} t_risc_fp_reg_val;

static inline t_risc_fp_reg_val get_fVal(float f) {
    t_risc_fp_reg_val ret;
    ret.f = f;
    return ret;
}

static inline t_risc_fp_reg_val get_dVal(double d) {
    t_risc_fp_reg_val ret;
    ret.d = d;
    return ret;
}

//RISC-V operation types (for later optimization)
typedef enum {
    REG_REG, IMMEDIATE, UPPER_IMMEDIATE, STORE, BRANCH, JUMP, SYSTEM, FLOAT, INVALID_INSTRUCTION, INVALID_BLOCK, PSEUDO
} t_risc_optype;

//carry immediate values in the instruction struct
typedef int64_t t_risc_imm;

//carry a pointer to the raw instruction in the struct
typedef uintptr_t t_risc_addr;

typedef struct {
    t_risc_addr addr;
    t_risc_mnem mnem;
    t_risc_optype optype;
    t_risc_reg reg_src_1;
    t_risc_reg reg_src_2;
    t_risc_reg reg_dest;
    union {
        struct {
            t_risc_imm imm;
        };
        struct {
            uint32_t reg_src_3;
            uint32_t rounding_mode;
        };
    };
} t_risc_instr;

/**
 * Register information for the translator functions.
 * @param map the RISC-V -> x86 register mapping
 * @param mapped which of the RISC-V registers are mapped
 * @param base the base address of the general-purpose register file in memory
 * @param csr_base the base address of the CSR register file in memory
 * @param replacement_content the current RISC-V registers that are loaded into the replacement registers (UNKNOWN is empty)
 * @param replacement_recency specifies the recently used RISC-V registers (lower is older, 0 is clean)
 */
typedef struct {
    FeReg *map;
    bool *mapped;
    uint64_t base;
    uint64_t csr_base;
    t_risc_reg *replacement_content;
    uint64_t *replacement_recency;
    uint64_t *current_recency;
    uint64_t fp_base;
} register_info;

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TYPEDEFS_H
