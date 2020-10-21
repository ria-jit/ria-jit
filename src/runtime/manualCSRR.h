//
// Created by noah on 20.10.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_MANUALCSRR_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_MANUALCSRR_H

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_MANUALCSRR_H

#include <util/log.h>
#include <util/util.h>
#include <util/typedefs.h>
#include <gen/translate.h>

#ifdef __cplusplus
extern "C" {
#endif

void manualCSRR(t_risc_reg_val *registerValues, t_risc_imm imm, t_risc_reg src_1, t_risc_mnem mnem, t_risc_reg dest);

#ifdef __cplusplus
}
#endif //__cplusplus
