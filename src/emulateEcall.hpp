//
// Created by simon on 25.06.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_HPP

#include "util.h"
#include "translate.hpp"

void emulate_ecall(t_risc_addr addr, t_risc_reg_val *registerValues);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_HPP
