//
// Created by flo on 21.05.20.
//

#include "translate_other.hpp"
#include "runtime/register.h"
#include "runtime/emulateEcall.hpp"

/**
* Translate the FENCE instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_FENCE(const t_risc_instr &instr, const register_info &r_info) {
    log_general("Ignored FENCE…\n"); //just ignore for now
}

/**
* Translate the ECALL instruction.
* Makes a system call to the execution environment.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ECALL(const t_risc_instr &instr, const register_info &r_info) {
    //see https://stackoverflow.com/questions/59800430/risc-v-ecall-syscall-calling-convention-on-pk-linux
    log_asm_out("Translate ECALL…\n");

    save_risc_registers(r_info);
    err |= fe_enc64(&current, FE_MOV64ri, FE_DI, instr.addr);
    err |= fe_enc64(&current, FE_MOV64ri, FE_SI, r_info.base);
    typedef void emulate(t_risc_addr addr, t_risc_reg_val *registerValues);
    emulate *em = &emulate_ecall;
    err |= fe_enc64(&current, FE_CALL, reinterpret_cast<uintptr_t>(em));
}

/**
* Translate the EBREAK instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_EBREAK(const t_risc_instr &instr, const register_info &r_info) {
    critical_not_yet_implemented("Translate EBREAK…\n");
}

/**
* Translate the FENCE_I instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_FENCE_I(const t_risc_instr &instr, const register_info &r_info) {
    critical_not_yet_implemented("Translate FENCE_I…\n");
}
