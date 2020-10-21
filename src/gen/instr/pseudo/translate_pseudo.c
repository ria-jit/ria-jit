//
// Created by Flo Schmidt on 12.09.20.
//

#include <gen/translate.h>
#include <util/util.h>
#include "translate_pseudo.h"
#include <env/flags.h>

///set the pc to next addr after inst
/**
 * Translate the PC_NEXT_INST pseudo instruction that is inserted when a block would overflow our buffer.
 * Emits instructions that sets the PC RISC_V register in our gp_reg_file to the given address.
 * @param addr the address the risc PC reg should have at the end of the current block (the start address of the next
 *        block)
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_PC_NEXT_INST(const t_risc_addr addr, const register_info *r_info) {
    log_asm_out("Translate pseudo PC_NEXT_INST\n");
    ///write chainEnd to be chained by chainer
    invalidateAllReplacements(r_info);
    if (flag_translate_opt_chain) {
        err |= fe_enc64(&current, FE_LEA64rm, FE_AX, FE_MEM(FE_IP, 0, 0, 0));
        err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((uint64_t) &chain_end), FE_AX);
        err |= fe_enc64(&current, FE_MOV32mi, FE_MEM_ADDR((uint64_t) &chain_type), FE_JMP);
    }

    ///set pc
    err |= fe_enc64(&current, FE_MOV64ri, FE_AX, addr);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR(r_info->base + 8 * pc), FE_AX);
}

/**
 * Translate the SILENT_NOP instruction.
 * Description
 * @param instr the RISC-V instruction to translate
 * @param r_info the runtime register mapping (RISC-V -> x86)
 */
void translate_SILENT_NOP(const t_risc_instr *instr, const register_info *r_info) {
    log_asm_out("Translate SILENT_NOP...\n");
    //Intentionally left blank. Can be used in order to ignore parts of the block cache prior to translation.
}
