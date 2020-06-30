//
// Created by simon on 25.06.20.
//

#include <linux/unistd.h>
#include <cerrno>
#include "register.h"
#include "emulateEcall.hpp"

extern bool finalize;

void emulate_ecall(t_risc_addr addr, t_risc_reg_val *registerValues) {
    log_general("Emulate syscall...\n");
    ///Increment PC, if the syscall needs to modify it just overwrite it in the specific branch.
    registerValues[t_risc_reg::pc] = addr + 4;
    switch(registerValues[t_risc_reg_mnem::a7]) {
        case 64: //Write
        {
            size_t retval = __NR_write;
            __asm__ volatile("syscall" : "+a"(retval) :
            "D"(registerValues[t_risc_reg_mnem::a0]), "S"(registerValues[t_risc_reg_mnem::a1]), "d"
            (registerValues[t_risc_reg_mnem::a2]) :
            "memory", "rcx", "r11");

            registerValues[t_risc_reg_mnem::a0] = retval;
        }
            break;
        case 93: //Exit
        {
            finalize = true;
        }
            break;
        default:
            dprintf(2, "Syscall 0x%lx is not yet implemented\n", registerValues[t_risc_reg_mnem::a7]);
            registerValues[t_risc_reg_mnem::a0] = -ENOSYS;
            break;
    }
}