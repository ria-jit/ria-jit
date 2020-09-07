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

rs_entry rs_pop(void);

rs_entry rs_peek(void);

void rs_pop_blind(void);

uintptr_t rs_pop_easy(t_risc_addr r_add);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_RETURN_STACK_H
