//
// Created by flo on 21.05.20.
//

#include "translate_csr.h"
#include <fadec/fadec-enc.h>
#include <util/util.h>
#include "runtime/manualCSRR.h"

/*
 * This implements/emulates the CSR behaviour without account for the atomicity of the instructions.
 */

/**
* Translate the CSRRW instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRW(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate CSRRW...\n");

    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    if (instr->reg_dest != x0) {
        //CSR value -> rd (CSR address is in immediate field)
        err |= fe_enc64(&current, FE_MOV64rm, regDest, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm));
    }

    //rs1 value -> CSR
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm), regSrc1);
}

/**
* Translate the CSRRS instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRS(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate CSRRS...\n");

    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    //CSR value -> rd (CSR address is in immediate field)
    err |= fe_enc64(&current, FE_MOV64rm, regDest, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm));

    if (instr->reg_src_1 != x0) {
        //use rs1 as bitmask to set the corresponding bits in the CSR
        err |= fe_enc64(&current, FE_OR64mr, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm), regSrc1);
    }
}

/**
* Translate the CSRRC instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRC(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate CSRRC...\n");

    FeReg regSrc1 = getRs1(instr, r_info);
    FeReg regDest = getRd(instr, r_info);

    //CSR value -> rd (CSR address is in immediate field)
    err |= fe_enc64(&current, FE_MOV64rm, regDest, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm));

    if (instr->reg_src_1 != x0) {
        //use rs1 as bitmask to clear the corresponding bits in the CSR
        err |= fe_enc64(&current, FE_NOT64r, regSrc1);
        err |= fe_enc64(&current, FE_AND64mr, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm), regSrc1);
        err |= fe_enc64(&current, FE_NOT64r, regSrc1); //does the rs1 value need to be preserved?
    }
}

/**
* Translate the CSRRWI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRWI(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate CSRRWI...\n");

    FeReg regDest = getRd(instr, r_info);

    if (instr->reg_dest != x0) {
        //CSR value -> rd (CSR address is in immediate field)
        err |= fe_enc64(&current, FE_MOV64rm, regDest, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm));
    }

    //uimm[4:0] value -> CSR
    uint8_t uimm_val = instr->reg_src_1;
    err |= fe_enc64(&current, FE_MOV64mi, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm), uimm_val);
}

/**
* Translate the CSRRSI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRSI(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate CSRRSI...\n");

    FeReg regDest = getRd(instr, r_info);

    //CSR value -> rd (CSR address is in immediate field)
    err |= fe_enc64(&current, FE_MOV64rm, regDest, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm));

    //see above for explanation
    uint8_t uimm_val = instr->reg_src_1;
    if (uimm_val != 0) {
        //use rs1 as bitmask to set the corresponding bits in the CSR
        err |= fe_enc64(&current, FE_OR64mi, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm), uimm_val);
    }
}

/**
* Translate the CSRRCI instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_CSRRCI(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate CSRRCI...\n");

    FeReg regDest = getRd(instr, r_info);

    //CSR value -> rd (CSR address is in immediate field)
    err |= fe_enc64(&current, FE_MOV64rm, regDest, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm));

    //see above for explanation
    uint8_t uimm_val = instr->reg_src_1;
    if (uimm_val != 0) {
        //use rs1 as bitmask to clear the corresponding bits in the CSR
        err |= fe_enc64(&current, FE_AND64mi, FE_MEM_ADDR(r_info->csr_base + 8 * instr->imm), ~uimm_val);
    }
}

/**
* Translate the MANUAL_CSRR instruction.
* Handle reads to existing x86 csr registers manually in c-code
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_MANUAL_CSRR(const t_risc_instr *instr, const register_info *r_info, const context_info *c_info) {
    log_asm_out("Translate MANUAL_CSRR...\n");

    invalidateAllReplacements(r_info);
    //emit c_info->save_context();
    err |= fe_enc64(&current, FE_CALL, (intptr_t) c_info->save_context);

    //emit emulate_ecall(instr->addr, r_info->base);
    err |= fe_enc64(&current, FE_MOV64ri, FE_DI, r_info->base);
    err |= fe_enc64(&current, FE_MOV64ri, FE_SI, instr->imm);
    err |= fe_enc64(&current, FE_MOV64ri, FE_DX, instr->reg_src_1);
    err |= fe_enc64(&current, FE_MOV64ri, FE_CX, instr->reg_src_2); //holds mnem
    err |= fe_enc64(&current, FE_MOV64ri, FE_R8, instr->reg_dest);
    typedef void manual(t_risc_reg_val *registerValues, t_risc_imm imm, t_risc_reg src_1, t_risc_mnem mnem, t_risc_reg dest);
    manual *mc = &manualCSRR;
    err |= fe_enc64(&current, FE_CALL, (uintptr_t) mc);
    //emit c_info->load_execute_save_context(*, false); //* means value does not matter, false means load without execute
    err |= fe_enc64(&current, FE_XOR32rr, FE_SI, FE_SI);
    err |= fe_enc64(&current, FE_CALL, (intptr_t) c_info->load_execute_save_context);
}
