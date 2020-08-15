//
// Created by simon on 25.06.20.
//

#include <linux/unistd.h>
#include <cerrno>
#include <sys/mman.h>
#include <cstring>
#include <cstdio>
#include <sys/stat.h>
#include <runtime/register.h>
#include "emulateEcall.hpp"

struct statRiscV {
    unsigned long st_dev;        /* Device.  */
    unsigned long st_ino;        /* File serial number.  */
    unsigned int st_mode;    /* File mode.  */
    unsigned int st_nlink;    /* Link count.  */
    unsigned int st_uid;        /* User ID of the file's owner.  */
    unsigned int st_gid;        /* Group ID of the file's group. */
    unsigned long st_rdev;    /* Device number, if device.  */
    unsigned long __pad1;
    long st_size;    /* Size of file, in bytes.  */
    int st_blksize;    /* Optimal block size for I/O.  */
    int __pad2;
    long st_blocks;    /* Number 512-byte blocks allocated. */
    long st_atim;    /* Time of last access.  */
    unsigned long st_atime_nsec;
    long st_mtim;    /* Time of last modification.  */
    unsigned long st_mtime_nsec;
    long st_ctim;    /* Time of last status change.  */
    unsigned long st_ctime_nsec;
    unsigned int __unused4;
    unsigned int __unused5;
};

extern bool finalize;

int guest_exit_status;

t_risc_addr curBrk;
t_risc_addr initialBrk;
t_risc_addr mappedBrk;

void setupBrk(int brk) {
    curBrk = initialBrk = mappedBrk = brk;
}


//@formatter:off
static size_t syscall0(int syscall_number) {
    size_t retval = syscall_number;
    __asm__ volatile("syscall" : "+a"(retval) : : "memory", "rcx", "r11");
    return retval;
}

static size_t syscall1(int syscall_number, size_t a1) {
    size_t retval = syscall_number;
    __asm__ volatile("syscall" : "+a"(retval) :
    "D"(a1) :
    "memory", "rcx", "r11");
    return retval;
}

static size_t syscall2(int syscall_number, size_t a1, size_t a2) {
    size_t retval = syscall_number;
    __asm__ volatile("syscall" : "+a"(retval) :
    "D"(a1), "S"(a2) :
    "memory", "rcx", "r11");
    return retval;
}

static size_t syscall3(int syscall_number, size_t a1, size_t a2, size_t a3) {
    size_t retval = syscall_number;
    __asm__ volatile("syscall" : "+a"(retval) :
    "D"(a1), "S"(a2), "d"(a3) :
    "memory", "rcx", "r11");
    return retval;
}

static size_t syscall4(int syscall_number, size_t a1, size_t a2, size_t a3,
                       size_t a4) {
    size_t retval = syscall_number;
    register size_t r10 __asm__("r10") = a4;
    __asm__ volatile("syscall" : "+a"(retval) :
    "D"(a1), "S"(a2), "d"(a3), "r"(r10) :
    "memory", "rcx", "r11");
    return retval;
}

static size_t syscall6(int syscall_number, size_t a1, size_t a2, size_t a3,
                       size_t a4, size_t a5, size_t a6) {
    size_t retval = syscall_number;
    register size_t r8 __asm__("r8") = a5;
    register size_t r9 __asm__("r9") = a6;
    register size_t r10 __asm__("r10") = a4;
    __asm__ volatile("syscall" : "+a"(retval) :
    "D"(a1), "S"(a2), "d"(a3), "r"(r8), "r"(r9), "r"(r10) :
    "memory", "rcx", "r11");
    return retval;
}
__attribute__((force_align_arg_pointer))
void emulate_ecall(t_risc_addr addr, t_risc_reg_val *registerValues) {
    ///Increment PC, if the syscall needs to modify it just overwrite it in the specific branch.
    registerValues[t_risc_reg::pc] = addr + 4;
    switch (registerValues[t_risc_reg_mnem::a7]) {
        case 29: //ioctl
        {
            log_general("Emulate syscall ioctl (29)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall3(__NR_ioctl, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1],
                                                           registerValues[t_risc_reg_mnem::a2]);
        }
            break;
        case 35: //unlinkat
        {
            log_general("Emulate syscall unlinkat (35)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall3(__NR_unlinkat, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1],
                                                           registerValues[t_risc_reg_mnem::a2]);
        }
            break;
        case 52: //fchmod
        {
            log_general("Emulate syscall fchmod (52)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall2(__NR_fchmod, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1]);
        }
            break;
        case 55: //fchown
        {
            log_general("Emulate syscall fchown (55)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall3(__NR_fchown, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1],
                                                           registerValues[t_risc_reg_mnem::a2]);
        }
            break;
        case 56: //openat
        {
            log_general("Emulate syscall openat (56)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall4(__NR_openat, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1],
                                                           registerValues[t_risc_reg_mnem::a2],
                                                           registerValues[t_risc_reg_mnem::a3]);
        }
            break;
        case 57: //close
        {
            log_general("Emulate syscall close (57)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall1(__NR_close, registerValues[t_risc_reg_mnem::a0]);
        }
            break;
        case 62: //lseek
        {
            log_general("Emulate syscall lseek (62)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall3(__NR_lseek, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1],
                                                           registerValues[t_risc_reg_mnem::a2]);
        }
            break;
        case 63: //read
        {
            log_general("Emulate syscall read (63)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall3(__NR_read, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1],
                                                           registerValues[t_risc_reg_mnem::a2]);
        }
            break;
        case 64: //Write
        {
            log_general("Emulate syscall write (64)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall3(__NR_write, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1],
                                                           registerValues[t_risc_reg_mnem::a2]);
        }
            break;
        case 78: //readlinkat
        {
            log_general("Emulate syscall readlinkat (78)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall3(__NR_readlinkat, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1],
                                                           registerValues[t_risc_reg_mnem::a2]);
        }
            break;
        case 80: //fstat
        {
            log_general("Emulate syscall fstat (80)...\n");
            auto *pStatRiscV = reinterpret_cast<statRiscV *>(registerValues[t_risc_reg_mnem::a1]);
            struct stat buf {};
            registerValues[t_risc_reg_mnem::a0] = syscall2(__NR_fstat, registerValues[t_risc_reg_mnem::a0],
                                                           reinterpret_cast<size_t>(&buf));
            pStatRiscV->st_blksize=buf.st_blksize;
            pStatRiscV->st_size=buf.st_size;
            pStatRiscV->st_atim=buf.st_atime;
            pStatRiscV->st_atime_nsec=buf.st_atim.tv_nsec;
            pStatRiscV->st_blocks=buf.st_blocks;
            pStatRiscV->st_ctim=buf.st_ctime;
            pStatRiscV->st_ctime_nsec=buf.st_ctim.tv_nsec;
            pStatRiscV->st_dev=buf.st_dev;
            pStatRiscV->st_gid=buf.st_gid;
            pStatRiscV->st_ino=buf.st_ino;
            pStatRiscV->st_mode=buf.st_mode;
            pStatRiscV->st_mtim=buf.st_mtime;
            pStatRiscV->st_mtime_nsec=buf.st_mtim.tv_nsec;
            pStatRiscV->st_nlink=buf.st_nlink;
            pStatRiscV->st_rdev=buf.st_rdev;
            pStatRiscV->st_uid=buf.st_uid;
        }
            break;
        case 88: //utimensat
        {
            log_general("Emulate syscall utimensat (88)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall4(__NR_utimensat, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1],
                                                           registerValues[t_risc_reg_mnem::a2],
                                                           registerValues[t_risc_reg_mnem::a3]);
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
        case 94: //exit_group
        {
            log_general("Emulate syscall exit_group (94)...\n");
            //note the guest exit code and stop main loop
            guest_exit_status = (int) get_value(static_cast<t_risc_reg>(a0));
            finalize = true;
        }
            break;
        case 98: //futex
        {
            log_general("Emulate syscall futex (98)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall2(__NR_futex, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1]);
        }
            break;
        case 113: //clock_gettime
        {
            log_general("Emulate syscall clock_gettime (98)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall2(__NR_clock_gettime, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1]);
        }
            break;
        case 134: //rt_sigaction
        {
            log_general("Ignore syscall rt_sigaction (134) return success...\n");
            //registerValues[t_risc_reg_mnem::a0] = syscall2(__NR_rt_sigaction, registerValues[t_risc_reg_mnem::a0],
            //                                               registerValues[t_risc_reg_mnem::a1]);
            registerValues[t_risc_reg_mnem::a0] = 0;
        }
            break;
        case 135: //rt_sigprocmask
        {
            log_general("Emulate syscall rt_sigprocmask (135)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall3(__NR_rt_sigprocmask, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1],
                                                           registerValues[t_risc_reg_mnem::a2]);
        }
            break;
        case 160: //uname
        {
            log_general("Emulate syscall uname (160)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall1(__NR_uname, registerValues[t_risc_reg_mnem::a0]);
        }
            break;
        case 169: //gettimeofday
        {
            log_general("Emulate syscall gettimeofday (169)...\n");
            registerValues[t_risc_reg_mnem::a0] = syscall2(__NR_gettimeofday, registerValues[t_risc_reg_mnem::a0],
                                                           registerValues[t_risc_reg_mnem::a1]);
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
                    //TODO This not the minilib mmap, switch over when not needing C++ anymore.
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
            dprintf(2, "Syscall %li is not yet implemented\n", registerValues[t_risc_reg_mnem::a7]);
            registerValues[t_risc_reg_mnem::a0] = -ENOSYS;
            break;
    }
}