//
// Created by flo on 14.07.20.
//

#include "translate_a_ext.h"
#include <fadec/fadec-enc.h>

/*
 * Experimental and not tested. TODO make actually atomic
 */

/**
 * Translate the LRW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LRW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate LRW…\n");

    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOVSXr64m32, FE_DX, FE_MEM(FE_AX, 0, 0, 0));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_DX);

    //not doing anything for fencing yet
}

/**
 * Translate the SCW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SCW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SCW…\n");

    // rs2 -> [rs1] 0 in rd if succeed

    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));
    err |= fe_enc64(&current, FE_MOV32mr, FE_MEM(FE_AX, 0, 0, 0), FE_DX);

    // we are not using any memory fences here, just simulate a successfull write (0 in rd)
    err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), 0);
}

/**
 * Translate the AMOSWAPW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOSWAPW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOSWAPW…\n");

    //load [rs1] --> rd
    //apply binary operator rd • rs2 --> [rs1]

    //load into rd
    err |= fe_enc64(&current, FE_XOR64rr, FE_DX, FE_DX); //erase dx
    err |= fe_enc64(&current, FE_MOV32rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));

    //load rs2 into FE_CX
    err |= fe_enc64(&current, FE_MOV32rm, FE_CX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));
    //swap data at address FE_DX with content of FE_CX, should be atomic
    err |= fe_enc64(&current, FE_XCHG32mr, FE_MEM(FE_DX, 0, 0, 0), FE_CX);

    //FE_CX is now holding information of [FE_DX] => store data back in rd and rs2 (sign extended)
    err |= fe_enc64(&current, FE_MOVSXr64r32, FE_AX, FE_CX);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2), FE_AX);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_AX);
}

/**
 * Translate the AMOADDW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOADDW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOADDW…\n");

    //load [rs1] --> rd
    //apply binary operator rd • rs2 --> [rs1]

    //load into rd
    err |= fe_enc64(&current, FE_MOV32rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM(FE_DX, 0, 0, 0));
    err |= fe_enc64(&current, FE_MOVSXr64r32, FE_CX, FE_AX);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_CX);

    //load rs2 into AX
    err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));

    //apply binary operator and store into location at DX
    err |= fe_enc64(&current, FE_ADD32rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest));
    err |= fe_enc64(&current, FE_MOV32mr, FE_MEM(FE_DX, 0, 0, 0), FE_AX);
}

/**
 * Translate the AMOXORW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOXORW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOXORW…\n");

    //load [rs1] --> rd
    //apply binary operator rd • rs2 --> [rs1]

    //load into rd
    err |= fe_enc64(&current, FE_MOV32rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM(FE_DX, 0, 0, 0));
    err |= fe_enc64(&current, FE_MOVSXr64r32, FE_CX, FE_AX);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_CX);

    //load rs2 into AX
    err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));

    //apply binary operator and store into location at DX
    err |= fe_enc64(&current, FE_XOR32rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest));
    err |= fe_enc64(&current, FE_MOV32mr, FE_MEM(FE_DX, 0, 0, 0), FE_AX);
}

/**
 * Translate the AMOANDW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOANDW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOANDW…\n");

    //load [rs1] --> rd
    //apply binary operator rd • rs2 --> [rs1]

    //load into rd
    err |= fe_enc64(&current, FE_MOV32rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM(FE_DX, 0, 0, 0));
    err |= fe_enc64(&current, FE_MOVSXr64r32, FE_CX, FE_AX);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_CX);

    //load rs2 into AX
    err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));

    //apply binary operator and store into location at DX
    err |= fe_enc64(&current, FE_AND32rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest));
    err |= fe_enc64(&current, FE_MOV32mr, FE_MEM(FE_DX, 0, 0, 0), FE_AX);
}

/**
 * Translate the AMOORW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOORW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOORW…\n");

    //load [rs1] --> rd
    //apply binary operator rd • rs2 --> [rs1]

    //load into rd
    err |= fe_enc64(&current, FE_MOV32rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM(FE_DX, 0, 0, 0));
    err |= fe_enc64(&current, FE_MOVSXr64r32, FE_CX, FE_AX);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_CX);

    //load rs2 into AX
    err |= fe_enc64(&current, FE_MOV32rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));

    //apply binary operator and store into location at DX
    err |= fe_enc64(&current, FE_OR32rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest));
    err |= fe_enc64(&current, FE_MOV32mr, FE_MEM(FE_DX, 0, 0, 0), FE_AX);
}

/**
 * Translate the AMOMINW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOMINW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOMINW…\n");
    critical_not_yet_implemented("AMOMINW not yet implemented.\n");
}

/**
 * Translate the AMOMAXW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOMAXW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOMAXW…\n");
    critical_not_yet_implemented("AMOMAXW not yet implemented.\n");
}

/**
 * Translate the AMOMINUW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOMINUW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOMINUW…\n");
    critical_not_yet_implemented("AMOMINUW not yet implemented.\n");
}

/**
 * Translate the AMOMAXUW instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOMAXUW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOMAXUW…\n");
    critical_not_yet_implemented("AMOMAXUW not yet implemented.\n");
}

/**
 * Translate the LRD instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_LRD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate LRD…\n");

    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_MEM(FE_AX, 0, 0, 0));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_DX);

    //not doing anything for fencing yet
}

/**
 * Translate the SCD instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SCD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SCD…\n");
    // rs2 -> [rs1] 0 in rd if succeed

    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(FE_AX, 0, 0, 0), FE_DX);

    // we are not using any memory fences here, just simulate a successfull write (0 in rd)
    err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), 0);
}

/**
 * Translate the AMOSWAPD instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOSWAPD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOSWAPD…\n");

    //load [rs1] --> rd
    //apply binary operator rd • rs2 --> [rs1]

    //load into rd
    err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));

    //load rs2 into FE_CX
    err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));
    //swap data at address FE_DX with content of FE_CX, should be atomic
    err |= fe_enc64(&current, FE_XCHG64mr, FE_MEM(FE_DX, 0, 0, 0), FE_CX);

    //FE_CX is now holding information of [FE_DX] => store data back in rd and rs2 (sign extended)
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2), FE_CX);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_CX);
}

/**
 * Translate the AMOADDD instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOADDD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOADDD…\n");

    //load [rs1] --> rd
    //apply binary operator rd • rs2 --> [rs1]

    //load into rd
    err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM(FE_DX, 0, 0, 0));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_AX);

    //load rs2 into AX
    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));

    //apply binary operator and store into location at DX
    err |= fe_enc64(&current, FE_ADD64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(FE_DX, 0, 0, 0), FE_AX);
}

/**
 * Translate the AMOXORD instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOXORD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOXORD…\n");

    //load [rs1] --> rd
    //apply binary operator rd • rs2 --> [rs1]

    //load into rd
    err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM(FE_DX, 0, 0, 0));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_AX);

    //load rs2 into AX
    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));

    //apply binary operator and store into location at DX
    err |= fe_enc64(&current, FE_XOR64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(FE_DX, 0, 0, 0), FE_AX);
}

/**
 * Translate the AMOANDD instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOANDD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOANDD…\n");

    //load [rs1] --> rd
    //apply binary operator rd • rs2 --> [rs1]

    //load into rd
    err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM(FE_DX, 0, 0, 0));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_AX);

    //load rs2 into AX
    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));

    //apply binary operator and store into location at DX
    err |= fe_enc64(&current, FE_AND64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(FE_DX, 0, 0, 0), FE_AX);
}

/**
 * Translate the AMOORD instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOORD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOORD…\n");

    //load [rs1] --> rd
    //apply binary operator rd • rs2 --> [rs1]

    //load into rd
    err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_1));
    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM(FE_DX, 0, 0, 0));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest), FE_AX);

    //load rs2 into AX
    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_src_2));

    //apply binary operator and store into location at DX
    err |= fe_enc64(&current, FE_OR64rm, FE_AX, FE_MEM_ADDR(r_info->base + 8 * instr->reg_dest));
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(FE_DX, 0, 0, 0), FE_AX);
}

/**
 * Translate the AMOMIND instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOMIND(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOMIND…\n");
    critical_not_yet_implemented("AMOMIND not yet implemented.\n");
}

/**
 * Translate the AMOMAXD instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOMAXD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOMAXD…\n");
    critical_not_yet_implemented("AMOMAXD not yet implemented.\n");
}

/**
 * Translate the AMOMINUD instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOMINUD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOMINUD…\n");
    critical_not_yet_implemented("AMOMINUD not yet implemented.\n");
}

/**
 * Translate the AMOMAXUD instruction.
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_AMOMAXUD(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate AMOMAXUD…\n");
    critical_not_yet_implemented("AMOMAXUD not yet implemented.\n");
}
