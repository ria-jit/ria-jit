//
// Created by jhne on 9/7/20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_RETURN_STACK_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_RETURN_STACK_H

#include <util/typedefs.h>

typedef struct {
    t_risc_addr risc_addr;
    uintptr_t x86_addr;
} rs_entry;

void init_return_stack(void);

void rs_emit_push(const t_risc_instr *instr, const register_info *r_info);

void rs_emit_pop_RAX(bool jump_or_push, const register_info *r_info);

void rs_jump_stack(const register_info *r_info);

extern rs_entry *r_stack;
extern volatile uint64_t rs_front;

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_RETURN_STACK_H
