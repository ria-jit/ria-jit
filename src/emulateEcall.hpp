//
// Created by simon on 25.06.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_HPP

#include "util.h"
#include "translate.hpp"

void emulate_ecall(const t_risc_instr &instr, const register_info &r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_HPP
