//
// Created by flo on 10.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_REGISTER_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_REGISTER_H

#include <util/log.h>
#include <util/typedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

t_risc_reg_val *get_gp_reg_file(void);

t_risc_reg_val *get_csr_reg_file(void);

t_risc_reg_val *get_fp_reg_file(void);

uint64_t *get_swap_file(void);

uint32_t *get_fctrl_file(void);

__attribute__((unused))
uint64_t  *get_usage_file(void);

t_risc_reg_val get_value(t_risc_reg reg);

t_risc_fp_reg_val get_fpvalue(t_risc_reg reg);

void set_value(t_risc_reg reg, t_risc_reg_val val);

void set_fpvalue(t_risc_reg reg, t_risc_fp_reg_val val);

void dump_gp_registers(void);

void dump_register_stats(void);

#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_REGISTER_H
