//
// Created by simon on 25.06.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_H

#include <util/log.h>
#include <util/util.h>
#include <util/typedefs.h>
#include <gen/translate.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int guest_exit_status;

void setupBrk(t_risc_addr brk);

void emulate_ecall(t_risc_addr addr, t_risc_reg_val *registerValues);

void setupMmapHint();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_EMULATEECALL_H
