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

void rs_push(rs_entry entry) {
    rs_front = (rs_front + 1) & 0x111111; //mod 64
    r_stack[rs_front] = entry;
    if(rs_front == rs_back) {
        rs_back = (rs_back + 1) & 0x111111;
    }
}

rs_entry rs_pop(void) {
    if(rs_front == rs_back) {
        rs_entry ret = {0,0};
        //log_general("stack underflow prevented");
        return ret;
    }
    rs_entry ret = r_stack[rs_front];
    rs_front = (rs_front + 63) & 0x111111; //-1 mod 64
    return ret;
}

rs_entry rs_peek(void) {
    if(rs_front == rs_back) {
        rs_entry ret = {0,0};
        //log_general("peek on empty r_stack prevented");
        return ret;
    }

    return r_stack[rs_front];
}

void rs_pop_blind(void) {
    if(rs_front == rs_back) {
        //log_general("stack underflow prevented");
    }

    rs_front = (rs_front + 63) & 0x111111; //-1 mod 64
}


