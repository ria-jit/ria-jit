//
// Created by flo on 02.05.20.
//

#include "util.h"
#include <stdio.h>

void not_yet_implemented() {
    printf("Not yet implemented\n");
}

//defunkify a J-type immediate; not tested -> dont use
int parse_jump_immediate(t_risc_instr* instr) {
    //why are the immediates so funky???
    unsigned int bullCrapFunkyImmediate = *instr->raw_bytes;
    unsigned int niceCleanImmediate = 0;
    niceCleanImmediate |= (bullCrapFunkyImmediate & 0x80000000);
    niceCleanImmediate |= (bullCrapFunkyImmediate & 0x00FF000) << 11;
    niceCleanImmediate |= (bullCrapFunkyImmediate & 0x0100000) << 2;
    niceCleanImmediate |= (bullCrapFunkyImmediate & 0x7FE00000) >> 9;
    niceCleanImmediate >>= 10;

    return niceCleanImmediate;
}