//
// Created by jhne on 9/7/20.
//

#include "return_stack.h"
#include <common.h>
#include <linux/mman.h>
#include <util/log.h>

rs_entry *r_stack;
int rs_front;
int rs_back;

void init_return_stack(void){
    r_stack = (rs_entry *) mmap(NULL, 64 * sizeof(rs_entry),
                                PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    rs_front = 0;
    rs_back = 0;
}

void rs_push(t_risc_addr r_add, uintptr_t x86_add) {
    rs_front = (rs_front + 1) & 0x3f; //mod 64
    r_stack[rs_front].risc_addr = r_add;
    r_stack[rs_front].x86_addr = x86_add;
    if(rs_front == rs_back) {
        rs_back = (rs_back + 1) & 0x3f;
    }

    //printf("rs_push called: riscV=%p, x86=%p s=%d\n", r_add, x86_add, rs_front);

}

uintptr_t rs_pop_check(t_risc_addr r_add) {
    if(rs_front == rs_back) {   ///stack empty
        //log_general("stack underflow prevented");
        //printf("rs_pop_check called: riscV=%p, x86=%p s=%d\n", r_add, 0, rs_front);
        return NULL;
    }

    if(r_stack[rs_front].risc_addr == r_add) {  ///hit
        uintptr_t ret = r_stack[rs_front].x86_addr;
        rs_front = (rs_front + 63) & 0x3f; //-1 mod 64
        //printf("rs_pop_check called: riscV=%p, x86=%p s=%d  ---------------hit---------------\n", r_add, ret, rs_front);
        return ret;
    }
    else {  ///miss
        //pop???    scrap cache???
        //printf("rs_pop_check called: riscV=%p, x86=%p s=%d\n", r_add, 1, rs_front);
        return NULL;
    }
}
