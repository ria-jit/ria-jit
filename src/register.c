//
// Created by flo on 10.05.20.
//

#include <glob.h>
#include "register.h"

size_t convert_to_index(t_risc_reg reg);

/**
 * The raw register contents stored in memory.
 * Access will be relatively expensive, so optimisation is needed.
 * contents[0] is undefined, as x0 is hardwired to 0 -
 * it is left unused in order to avoid off-by-one errors in indexing.
 */
t_risc_reg_val contents[33];

/**
 * Get the value currently in the passed register.
 * @param reg the register to lookup
 * @return value in register reg
 */
t_risc_reg_val get_value(t_risc_reg reg) {
    size_t index = convert_to_index(reg);

    switch(index) {
        case -1:
            //an illegal access occurred, we return 0 here as well
            //fallthrough
        case 0:
            //an access to x0 always yields 0
            return 0;
        default:
            return contents[index];
    }
}

void set_value(t_risc_reg reg, t_risc_reg_val val) {
    size_t index = convert_to_index(reg);

    switch(index) {
        case -1:
            //an illegal access has occured, so we ignore the write
            //fallthrough
        case 0:
            //a write to x0 (hardwired zero) is ignored
            return;
        default:
            contents[index] = val;
            return;
    }
}

size_t convert_to_index(t_risc_reg reg) {
    switch(reg) {
        case x0:
            return 0;
        case x1:
            return 1;
        case x2:
            return 2;
        case x3:
            return 3;
        case x4:
            return 4;
        case x5:
            return 5;
        case x6:
            return 6;
        case x7:
            return 7;
        case x8:
            return 8;
        case x9:
            return 9;
        case x10:
            return 10;
        case x11:
            return 11;
        case x12:
            return 12;
        case x13:
            return 13;
        case x14:
            return 14;
        case x15:
            return 15;
        case x16:
            return 16;
        case x17:
            return 17;
        case x18:
            return 18;
        case x19:
            return 19;
        case x20:
            return 20;
        case x21:
            return 21;
        case x22:
            return 22;
        case x23:
            return 23;
        case x24:
            return 24;
        case x25:
            return 25;
        case x26:
            return 26;
        case x27:
            return 27;
        case x28:
            return 28;
        case x29:
            return 29;
        case x30:
            return 30;
        case x31:
            return 31;
        case pc:
            return 32;
        default:
            return -1;
    }
}