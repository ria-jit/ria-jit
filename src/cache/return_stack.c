//
// Created by jhne on 9/7/20.
//

#include "return_stack.h"
#include <common.h>
#include <linux/mman.h>
#include <util/log.h>
#include <fadec/fadec-enc.h>
#include <gen/translate.h>
#include <util/util.h>

rs_entry *r_stack;
volatile uint64_t rs_front;
volatile uint64_t rs_back;

void init_return_stack(void) {
    r_stack = mmap(NULL, 64 * sizeof(rs_entry), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (BAD_ADDR(r_stack)) {
        dprintf(2, "Bad. Cache memory allocation failed.");
        _exit(FAIL_HEAP_ALLOC);
    }

    rs_front = 0;
    rs_back = 0;
}

void rs_emit_push(const t_risc_instr *instr) {

    ///push to return stack
    t_risc_addr ret_target = instr->addr + 4;

    t_cache_loc cache_loc;

    if ((cache_loc = lookup_cache_entry(ret_target)) == UNSEEN_CODE) {  // 1 ???
        log_asm_out("rs_emit_push: flag_translate_op is enabled, but return target is not in cache! riscv: %p\n",
               (void *) instr->addr);
        goto NOT_CACHED;
    }

    //asm rs_push   -----------
    err |= fe_enc64(&current, FE_PUSHr, FE_AX);

    err |= fe_enc64(&current, FE_MOV64rm, FE_AX, FE_MEM_ADDR((uint64_t) &rs_front));    //get front
    err |= fe_enc64(&current, FE_ADD64ri, FE_AX, 1);                                         //next field
    err |= fe_enc64(&current, FE_AND64ri, FE_AX, 0x3f);                                      //mod 64
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((uint64_t) &rs_front), FE_AX);    //save rs_front
    err |= fe_enc64(&current, FE_SHL64ri, FE_AX, 1);                                    //multiply by 2,struct array...
    err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_MEM_ADDR((intptr_t) &r_stack));     //get base
    err |= fe_enc64(&current, FE_MOV64ri, FE_CX, instr->addr + 4);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(FE_DX, 8, FE_AX, 0), FE_CX);         //save risc addr
    err |= fe_enc64(&current, FE_MOV64ri, FE_CX, (uintptr_t) cache_loc);
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM(FE_DX, 8, FE_AX, 8), FE_CX);         //save x86 addr
    err |= fe_enc64(&current, FE_SHR64ri, FE_AX, 1);                                         //revert multiplying by 2
    err |= fe_enc64(&current, FE_CMP64rm, FE_AX, FE_MEM_ADDR((uint64_t) &rs_back));     //front reached back?
    uint8_t *rs_full_jmp = current;
    err |= fe_enc64(&current, FE_JNZ, (intptr_t) current);                                   //dummy
    err |= fe_enc64(&current, FE_ADD64mi, FE_MEM_ADDR((uint64_t) &rs_back), 1);         //move back ahead
    err |= fe_enc64(&current, FE_AND64mi, FE_MEM_ADDR((uint64_t) &rs_back), 0x3f);      //mod 64
    err |= fe_enc64(&rs_full_jmp, FE_JNZ, (intptr_t) current);                               //replace dummy

    err |= fe_enc64(&current, FE_POPr, FE_AX);


    NOT_CACHED:;
}

void rs_emit_pop_RAX(bool jump_or_push) {   //true -> jump
    //RAX: target

    ///stack empty
    err |= fe_enc64(&current, FE_MOV64rm, FE_DX, FE_MEM_ADDR((intptr_t) &rs_front));    //get front
    err |= fe_enc64(&current, FE_CMP64rm, FE_DX, FE_MEM_ADDR((intptr_t) &rs_back));     //front == back?
    uint8_t *nullJMPempty = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current);  //dummy

    //RAX: target
    //RDX: front

    ///hit
    err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_MEM_ADDR((intptr_t) &r_stack));    //get base
    err |= fe_enc64(&current, FE_SHL64ri, FE_DX, 1);
    err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_MEM(FE_CX, 8, FE_DX, 0));        //get risc addr from stack
    err |= fe_enc64(&current, FE_CMP64rr, FE_AX, FE_CX);                                    //stack addr == target?
    uint8_t *nullJMPmiss = current;
    err |= fe_enc64(&current, FE_JNZ, (intptr_t) current);                                  //dummy
    err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_MEM_ADDR((intptr_t) &r_stack));    //get base  //DUPLICATE
    err |= fe_enc64(&current, FE_MOV64rm, FE_CX, FE_MEM(FE_CX, 8, FE_DX, 8));        // load x86 target
    err |= fe_enc64(&current, FE_SHR64ri, FE_DX, 1);
    err |= fe_enc64(&current, FE_ADD64ri, FE_DX, 63);                                       //-1 mod 64
    err |= fe_enc64(&current, FE_AND64ri, FE_DX, 0x3f);                                     //-1 mod 64
    err |= fe_enc64(&current, FE_MOV64mr, FE_MEM_ADDR((intptr_t) &rs_front), FE_DX);   //save rs_front
    uint8_t *noJumpHit = 0;
    if (jump_or_push) {
        err |= fe_enc64(&current, FE_JMPr, FE_CX);  //jmp to next block
    } else {
        err |= fe_enc64(&current, FE_PUSHr, FE_CX); //save jump target
        noJumpHit = current;
        err |= fe_enc64(&current, FE_JMP, (intptr_t) current); //dummy
    }

    //replace miss dummys
    err |= fe_enc64(&nullJMPempty, FE_JZ, (intptr_t) current);
    err |= fe_enc64(&nullJMPmiss, FE_JNZ, (intptr_t) current);

    if (!jump_or_push) {
        err |= fe_enc64(&current, FE_MOV64ri, FE_CX, 0);    //save target null: miss
        err |= fe_enc64(&current, FE_PUSHr, FE_CX);
        err |= fe_enc64(&noJumpHit, FE_JMP, (intptr_t) current);       //replace dummy
    }

}

void rs_jump_stack() {
    err |= fe_enc64(&current, FE_POPr, FE_CX);      //load jump target
    err |= fe_enc64(&current, FE_CMP64ri, FE_CX, 0);
    uint8_t *noJump = current;
    err |= fe_enc64(&current, FE_JZ, (intptr_t) current);      //dummy
    //err |= fe_enc64(&current, FE_MOV64ri, FE_CX, 0);
    err |= fe_enc64(&current, FE_JMPr, FE_CX);      //jmp to next block
    err |= fe_enc64(&noJump, FE_JZ, (intptr_t) current);
}
