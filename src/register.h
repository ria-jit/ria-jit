//
// Created by flo on 10.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_REGISTER_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_REGISTER_H

#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

t_risc_reg_val *get_reg_data(void);

t_risc_reg_val get_value(t_risc_reg reg);

void set_value(t_risc_reg reg, t_risc_reg_val val);

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_REGISTER_H
