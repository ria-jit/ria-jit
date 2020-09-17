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

void rs_push(t_risc_addr r_add, uintptr_t x86_add);

uintptr_t rs_pop_check(t_risc_addr r_add);

void rs_emit_push(const t_risc_instr *instr);

void rs_emit_pop_RAX(bool jump_or_push);

void rs_jump_stack();

extern rs_entry *r_stack;
extern volatile int rs_front;
extern volatile int rs_back;

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_RETURN_STACK_H
