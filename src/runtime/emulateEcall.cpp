//
// Created by simon on 25.06.20.
//

#include <linux/unistd.h>
#include <cerrno>
#include <sys/mman.h>
#include "register.h"
#include "emulateEcall.hpp"

extern bool finalize;

int guest_exit_status;

t_risc_addr curBrk;
t_risc_addr initialBrk;
t_risc_addr mappedBrk;

void setupBrk(int brk) {
    curBrk = initialBrk = mappedBrk = brk;
}

void emulate_ecall(t_risc_addr addr, t_risc_reg_val *registerValues) {
    log_general("Emulate syscall...\n");
    ///Increment PC, if the syscall needs to modify it just overwrite it in the specific branch.
    registerValues[t_risc_reg::pc] = addr + 4;
    switch(registerValues[t_risc_reg_mnem::a7]) {
        case 64: //Write
        {
            log_general("Emulate syscall write (64)...\n");
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
            log_general("Emulate syscall exit (93)...\n");

            //note the guest exit code and stop main loop
            guest_exit_status = (int) get_value(static_cast<t_risc_reg>(a0));
            finalize = true;
        }
            break;
        case 214: //brk
        {
            log_general("Emulate syscall brk (214)...\n");
            t_risc_addr brkAddr = registerValues[t_risc_reg_mnem::a0];
            if (brkAddr < initialBrk) {
                ///Cannot go below initial brk or we would go into ELF territory
                registerValues[t_risc_reg_mnem::a0] = curBrk;
            } else {
                if (brkAddr <= mappedBrk) {
                    ///We already have the memory for the new brk allocated so just zero it if growing and "return" it
                    if (brkAddr > curBrk) {
                        memset(reinterpret_cast<void *>(curBrk), 0, brkAddr - curBrk);
                    }
                    curBrk = brkAddr;
                    registerValues[t_risc_reg_mnem::a0] = curBrk;
                } else {
                    ///Additional page(s) need to be allocated first
                    t_risc_addr newMappedBrk = ALIGN_UP(brkAddr, 4096lu);
                    //TODO This not the minilib mmap, switch over when switching from ASMJit and not needing C++ 
                    // anymore.
                    void *map =
                            mmap(reinterpret_cast<void *>(mappedBrk), newMappedBrk - mappedBrk, PROT_READ | PROT_WRITE,
                                 MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE, 0, 0);

                    if (map != MAP_FAILED) {
                        if (reinterpret_cast<t_risc_addr>(map) != mappedBrk) {
                            //Did not get right address and not a mapping fault (Possible if MAP_FIXED_NOREPLACE is not
                            // supported). Unmap the mappings at the wrong address.
                            munmap(map, newMappedBrk - mappedBrk);
                        } else {
                            ///Need to zero already allocated pages
                            memset(reinterpret_cast<void *>(curBrk), 0, mappedBrk - curBrk);
                            mappedBrk = newMappedBrk;
                            curBrk = brkAddr;
                        }
                    }
                    ///"Return" the new (= old on failed) brk
                    registerValues[t_risc_reg_mnem::a0] = curBrk;
                }
            }
        }
            break;
        default:
            dprintf(2, "Syscall 0x%lx is not yet implemented\n", registerValues[t_risc_reg_mnem::a7]);
            registerValues[t_risc_reg_mnem::a0] = -ENOSYS;
            break;
    }
}