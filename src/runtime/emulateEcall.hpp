//
// Created by simon on 25.06.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_HPP

#include <util/log.h>
#include <util/util.h>
#include <util/typedefs.h>
#include "gen/translate.hpp"

#ifdef __cplusplus
extern "C" {
#endif

extern int guest_exit_status;

void setupBrk(int brk);
#ifdef __cplusplus
}

void emulate_ecall(t_risc_addr addr, t_risc_reg_val *registerValues);

#endif //__cplusplus

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_HPP
