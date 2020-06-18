//
// Created by flo on 21.05.20.
//

#include "translate_other.hpp"
#include "register.h"

using namespace asmjit;

/**
* Translate the FENCE instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_FENCE(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate FENCE…\n");
}

/**
* Translate the ECALL instruction.
* Makes a system call to the execution environment.
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_ECALL(const t_risc_instr &instr, const register_info &r_info) {
    //see https://stackoverflow.com/questions/59800430/risc-v-ecall-syscall-calling-convention-on-pk-linux
    log_verbose("Translate ECALL…\n");

    /*
     * As a quick implementation of these system calls, we want to support write(64) and exit(93).
     * For the numbers: see unistd.h in the RISC-V toolchain linux headers.
     * Syscall number from a7 needs to be in rax.
     * Arguments from a0…a6 need to be in rdi, rsi, rdx, r10, r8, r9
     * Then, syscall is called.
     * This assumes all registers are stored in memory, change for future references.
     */

    //load system call number…
    a->mov(x86::rax, x86::ptr(r_info.base + 8 * a7));

    //to handle exit: x86 number 60, so sub 33. This is of course not generalizable and just a temporary test
    const Label &notExit = a->newLabel();
    const Label &notWrite = a->newLabel();
    const Label &continueSyscall = a->newLabel();
    a->cmp(x86::rax, 93);
    a->jne(notExit);
    a->mov(x86::rax, 60); //exit
    a->jmp(continueSyscall);

    a->bind(notExit);
    a->cmp(x86::rax, 64);
    a->jne(notWrite);
    a->mov(x86::rax, 1); //write
    a->jmp(continueSyscall);

    a->bind(notWrite);

    //…and arguments…
    a->bind(continueSyscall);
    a->mov(x86::rdi, x86::ptr(r_info.base + 8 * a0));
    a->mov(x86::rsi, x86::ptr(r_info.base + 8 * a1));
    a->mov(x86::rdx, x86::ptr(r_info.base + 8 * a2));
    a->mov(x86::r10, x86::ptr(r_info.base + 8 * a3));
    a->mov(x86::r8, x86::ptr(r_info.base + 8 * a4));
    a->mov(x86::r9, x86::ptr(r_info.base + 8 * a5));

    //…and syscall
    a->syscall();

    //set pc; temporary
    a->push(x86::rax); //Preserve syscall return value
    a->mov(x86::rax, instr.addr + 4);
    a->mov(x86::ptr(r_info.base + 8 * pc), x86::rax); //add 4 for next instr
    a->pop(x86::rax);
}

/**
* Translate the EBREAK instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_EBREAK(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate EBREAK…\n");
}

/**
* Translate the FENCE_I instruction.
* Description
* @param instr the RISC-V instruction to translate
* @param r_info the runtime register mapping (RISC-V -> x86)
*/
void translate_FENCE_I(const t_risc_instr &instr, const register_info &r_info) {
    log_verbose("Translate FENCE_I…\n");
}
