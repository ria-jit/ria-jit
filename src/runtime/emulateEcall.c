//
// Created by simon on 25.06.20.
//

#include <asm/stat.h>
#include <linux/mman.h>
#include <common.h>
#include <runtime/register.h>
#include <elf/loadElf.h>
#include "emulateEcall.h"

//for potentially required syscalls see https://github.com/aengelke/instrew/blob/master/client/emulate.c

typedef struct {
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
} statRiscV;

extern bool finalize;

static t_risc_addr lastHint;

int guest_exit_status;

t_risc_addr curBrk;
t_risc_addr initialBrk;
t_risc_addr mappedBrk;

void setupBrk(t_risc_addr brk) {
    curBrk = initialBrk = mappedBrk = brk;
}

void setupMmapHint() {
    lastHint = TRANSLATOR_BASE - STACK_OFFSET - stackSize - guard;
}

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
    registerValues[pc] = addr + 4;
    switch (registerValues[a7]) {
        case 17: //getcwd
        {
            log_general("Emulate syscall getcwd (17)...\n");
            registerValues[a0] = syscall2(__NR_getcwd, registerValues[a0], registerValues[a1]);
        }
            break;
        case 25: //fcntl
        {
            log_general("Emulate syscall fcntl (25)...\n");
            registerValues[a0] = syscall3(__NR_fcntl, registerValues[a0], registerValues[a1], registerValues[a2]);
        }
            break;
        case 29: //ioctl
        {
            log_general("Emulate syscall ioctl (29)...\n");
            registerValues[a0] = syscall3(__NR_ioctl, registerValues[a0], registerValues[a1], registerValues[a2]);
        }
            break;
        case 35: //unlinkat
        {
            log_general("Emulate syscall unlinkat (35)...\n");
            registerValues[a0] = syscall3(__NR_unlinkat, registerValues[a0], registerValues[a1], registerValues[a2]);
        }
            break;
        case 49: //chdir
        {
            log_general("Emulate syscall chdir (49)...\n");
            registerValues[a0] = syscall1(__NR_chdir, registerValues[a0]);
        }
        case 52: //fchmod
        {
            log_general("Emulate syscall fchmod (52)...\n");
            registerValues[a0] = syscall2(__NR_fchmod, registerValues[a0], registerValues[a1]);
        }
            break;
        case 55: //fchown
        {
            log_general("Emulate syscall fchown (55)...\n");
            registerValues[a0] = syscall3(__NR_fchown, registerValues[a0], registerValues[a1], registerValues[a2]);
        }
            break;
        case 59: //pipe2
        {
            log_general("Emulate syscall pipe2 (59)...\n");
            registerValues[a0] = syscall2(__NR_pipe2, registerValues[a0], registerValues[a1]);
        }
            break;
        case 56: //openat
        {
            log_general("Emulate syscall openat (56)...\n");
            registerValues[a0] = syscall4(__NR_openat, registerValues[a0], registerValues[a1], registerValues[a2],
                                          registerValues[a3]);
        }
            break;
        case 57: //close
        {
            log_general("Emulate syscall close (57)...\n");
            registerValues[a0] = syscall1(__NR_close, registerValues[a0]);
        }
            break;
        case 61: //getdents64
        {
            log_general("Emulate syscall getdents64 (61)...\n");
            registerValues[a0] = syscall3(__NR_getdents64, registerValues[a0], registerValues[a1], registerValues[a2]);

        }
        case 62: //lseek
        {
            log_general("Emulate syscall lseek (62)...\n");
            registerValues[a0] = syscall3(__NR_lseek, registerValues[a0], registerValues[a1], registerValues[a2]);
        }
            break;
        case 63: //read
        {
            log_general("Emulate syscall read (63)...\n");
            registerValues[a0] = syscall3(__NR_read, registerValues[a0], registerValues[a1], registerValues[a2]);
        }
            break;
        case 64: //Write
        {
            log_general("Emulate syscall write (64)...\n");
            registerValues[a0] = syscall3(__NR_write, registerValues[a0], registerValues[a1], registerValues[a2]);
        }
            break;
        case 66: //writev
        {
            log_general("Emulate syscall writev (66)...\n");
            registerValues[a0] = syscall3(__NR_writev, registerValues[a0], registerValues[a1], registerValues[a2]);

        }
            break;
        case 78: //readlinkat
        {
            log_general("Emulate syscall readlinkat (78)...\n");
            registerValues[a0] = syscall4(__NR_readlinkat, registerValues[a0], registerValues[a1], registerValues[a2],
                                          registerValues[a3]);
        }
            break;
        case 79: //fstatat
        {
            log_general("Emulate syscall fstat (80)...\n");
            statRiscV *pStatRiscV = (statRiscV *) registerValues[a2];
            struct stat buf = {0};
            registerValues[a0] = syscall4(__NR_newfstatat, registerValues[a0], registerValues[a1], (size_t) &buf,
                                          registerValues[a3]);
            pStatRiscV->st_blksize = buf.st_blksize;
            pStatRiscV->st_size = buf.st_size;
            pStatRiscV->st_atim = buf.st_atime;
            pStatRiscV->st_atime_nsec = buf.st_atime_nsec;
            pStatRiscV->st_blocks = buf.st_blocks;
            pStatRiscV->st_ctim = buf.st_ctime;
            pStatRiscV->st_ctime_nsec = buf.st_ctime_nsec;
            pStatRiscV->st_dev = buf.st_dev;
            pStatRiscV->st_gid = buf.st_gid;
            pStatRiscV->st_ino = buf.st_ino;
            pStatRiscV->st_mode = buf.st_mode;
            pStatRiscV->st_mtim = buf.st_mtime;
            pStatRiscV->st_mtime_nsec = buf.st_mtime_nsec;
            pStatRiscV->st_nlink = buf.st_nlink;
            pStatRiscV->st_rdev = buf.st_rdev;
            pStatRiscV->st_uid = buf.st_uid;
        }
            break;
        case 80: //fstat
        {
            log_general("Emulate syscall fstat (80)...\n");
            statRiscV *pStatRiscV = (statRiscV *) registerValues[a1];
            struct stat buf = {0};
            registerValues[a0] = syscall2(__NR_fstat, registerValues[a0], (size_t) &buf);
            pStatRiscV->st_blksize = buf.st_blksize;
            pStatRiscV->st_size = buf.st_size;
            pStatRiscV->st_atim = buf.st_atime;
            pStatRiscV->st_atime_nsec = buf.st_atime_nsec;
            pStatRiscV->st_blocks = buf.st_blocks;
            pStatRiscV->st_ctim = buf.st_ctime;
            pStatRiscV->st_ctime_nsec = buf.st_ctime_nsec;
            pStatRiscV->st_dev = buf.st_dev;
            pStatRiscV->st_gid = buf.st_gid;
            pStatRiscV->st_ino = buf.st_ino;
            pStatRiscV->st_mode = buf.st_mode;
            pStatRiscV->st_mtim = buf.st_mtime;
            pStatRiscV->st_mtime_nsec = buf.st_mtime_nsec;
            pStatRiscV->st_nlink = buf.st_nlink;
            pStatRiscV->st_rdev = buf.st_rdev;
            pStatRiscV->st_uid = buf.st_uid;
        }
            break;
        case 88: //utimensat
        {
            log_general("Emulate syscall utimensat (88)...\n");
            registerValues[a0] = syscall4(__NR_utimensat, registerValues[a0], registerValues[a1], registerValues[a2],
                                          registerValues[a3]);
        }
            break;
        case 93: //Exit
        {
            log_general("Emulate syscall exit (93)...\n");

            //note the guest exit code and stop main loop
            guest_exit_status = (int) get_value((t_risc_reg) a0);
            finalize = true;
        }
            break;
        case 94: //exit_group
        {
            log_general("Emulate syscall exit_group (94)...\n");
            //note the guest exit code and stop main loop
            guest_exit_status = (int) get_value((t_risc_reg) a0);
            finalize = true;
        }
            break;
        case 96: //set_tid_address
        {
            log_general("Emulate syscall set_tid_address (96)...\n");
            registerValues[a0] = syscall1(__NR_set_tid_address, registerValues[a0]);

        }
            break;
        case 98: //futex
        {
            log_general("Emulate syscall futex (98)...\n");
            registerValues[a0] = syscall6(__NR_futex, registerValues[a0], registerValues[a1], registerValues[a2],
                                          registerValues[a3], registerValues[a4], registerValues[a5]);
        }
            break;
        case 99: //set_robust_list
        {
            log_general("Emulate syscall set_robust_list (99)...\n");
            registerValues[a0] = syscall2(__NR_set_robust_list, registerValues[a0], registerValues[a1]);
        }
            break;
        case 113: //clock_gettime
        {
            log_general("Emulate syscall clock_gettime (113)...\n");
            registerValues[a0] = syscall2(__NR_clock_gettime, registerValues[a0], registerValues[a1]);
        }
            break;
        case 131: //tgkill
        {
            log_general("Emulate syscall tgkill (131)...\n");
            registerValues[a0] =
                    syscall3(__NR_tgkill, registerValues[a0], registerValues[a1], registerValues[a2]);

        }
            break;
        case 134: //rt_sigaction
        {
            log_general("Ignore syscall rt_sigaction (134) return success...\n");
            //registerValues[a0] = syscall2(__NR_rt_sigaction, registerValues[a0], registerValues[a1]);
            registerValues[a0] = 0;
        }
            break;
        case 135: //rt_sigprocmask
        {
            log_general("Emulate syscall rt_sigprocmask (135)...\n");
            registerValues[a0] =
                    syscall4(__NR_rt_sigprocmask, registerValues[a0], registerValues[a1], registerValues[a2],
                             registerValues[a3]);
        }
            break;
        case 160: //uname
        {
            log_general("Emulate syscall uname (160)...\n");
            registerValues[a0] = syscall1(__NR_uname, registerValues[a0]);
        }
            break;
        case 169: //gettimeofday
        {
            log_general("Emulate syscall gettimeofday (169)...\n");
            registerValues[a0] = syscall2(__NR_gettimeofday, registerValues[a0], registerValues[a1]);
        }
            break;
        case 172: //getpid
        {
            log_general("Emulate syscall getpid (172)...\n");
            registerValues[a0] = syscall0(__NR_getpid);
        }
            break;
        case 174: //getuid
        {
            log_general("Emulate syscall getuid (174)...\n");
            registerValues[a0] = syscall0(__NR_getuid);
        }
            break;
        case 175: //geteuid
        {
            log_general("Emulate syscall geteuid (175)...\n");
            registerValues[a0] = syscall0(__NR_geteuid);
        }
            break;
        case 176: //getgid
        {
            log_general("Emulate syscall getgid (176)...\n");
            registerValues[a0] = syscall0(__NR_getgid);
        }
            break;
        case 177: //getegid
        {
            log_general("Emulate syscall getegid (177)...\n");
            registerValues[a0] = syscall0(__NR_getegid);
        }
            break;
        case 178: //gettid
        {
            log_general("Emulate syscall gettid (178)...\n");
            registerValues[a0] = syscall0(__NR_gettid);
        }
            break;
        case 179: //sysinfo
        {
            log_general("Emulate syscall sysinfo (179)...\n");
            registerValues[a0] = syscall1(__NR_sysinfo, registerValues[a0]);
        }
            break;
        case 214: //brk
        {
            log_general("Emulate syscall brk (214)...\n");
            t_risc_addr brkAddr = registerValues[a0];
            if (brkAddr < initialBrk) {
                ///Cannot go below initial brk or we would go into ELF territory
                registerValues[a0] = curBrk;
            } else {
                if (brkAddr <= mappedBrk) {
                    ///We already have the memory for the new brk allocated so just zero it if growing and "return" it
                    if (brkAddr > curBrk) {
                        memset((void *) curBrk, 0, brkAddr - curBrk);
                    }
                    curBrk = brkAddr;
                    registerValues[a0] = curBrk;
                } else {
                    ///Additional page(s) need to be allocated first
                    t_risc_addr newMappedBrk = ALIGN_UP(brkAddr, 4096lu);
                    void *map = mmap((void *) mappedBrk, newMappedBrk - mappedBrk, PROT_READ | PROT_WRITE,
                                     MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE, 0, 0);

                    if (!BAD_ADDR(map)) {
                        if ((t_risc_addr) map != mappedBrk) {
                            //Did not get right address and not a mapping fault (Possible if MAP_FIXED_NOREPLACE is not
                            // supported). Unmap the mappings at the wrong address.
                            munmap(map, newMappedBrk - mappedBrk);
                        } else {
                            ///Need to zero already allocated pages
                            memset((void *) curBrk, 0, mappedBrk - curBrk);
                            mappedBrk = newMappedBrk;
                            curBrk = brkAddr;
                        }
                    }
                    ///"Return" the new (= old on failed) brk
                    registerValues[a0] = curBrk;
                }
            }
        }
            break;
        case 215: //munmap
        {
            log_general("Emulate syscall munmap (215)...\n");
            t_risc_reg_val munmapAddr = registerValues[a0];
            t_risc_reg_val size = registerValues[a1];
            if (munmapAddr + size > (TRANSLATOR_BASE - STACK_OFFSET)) {
                log_general("Prevented munmap in translator region.");
                registerValues[a0] = -EINVAL; //Is this a fitting error code to return?
            } else {
                registerValues[a0] = syscall2(__NR_munmap, registerValues[a0], registerValues[a1]);
            }
        }
            break;
        case 222: //mmap
        {
            log_general("Emulate syscall mmap (222)...\n");
            t_risc_reg_val mmapAddr = registerValues[a0];
            t_risc_reg_val flags = registerValues[a3];
            if (!(flags & MAP_FIXED || flags & MAP_FIXED_NOREPLACE)) {
                if (mmapAddr == 0 || mmapAddr > (TRANSLATOR_BASE - STACK_OFFSET)) {
                    //non hinted mmap: hint to top of guest space or
                    //hinted mmap into translator region: re-hint to top of guest space
                    t_risc_addr hint = ALIGN_DOWN(lastHint - registerValues[a1], 4096lu);
                    registerValues[a0] =
                            syscall6(__NR_mmap, hint, registerValues[a1], registerValues[a2], flags,
                                     registerValues[a4], registerValues[a5]);
                    lastHint = hint;
                } else {
                    //Hinted Mapping that does not interfere with the translator's region.
                    registerValues[a0] =
                            syscall6(__NR_mmap, mmapAddr, registerValues[a1], registerValues[a2],
                                     flags, registerValues[a4], registerValues[a5]);
                }
            } else if (mmapAddr > (TRANSLATOR_BASE - STACK_OFFSET)) {
                log_general("Prevented mmap into translator region.");
                if (flags & MAP_FIXED) {
                    registerValues[a0] = -EINVAL; //Is this a fitting error code to return?
                } else {
                    registerValues[a0] = -EEXIST; //Simulate existing mapping for MAP_FIXED_NOREPLACE
                }
            } else {
                //Fixed mapping that does not interfere with translator region
                registerValues[a0] =
                        syscall6(__NR_mmap, mmapAddr, registerValues[a1], registerValues[a2],
                                 flags, registerValues[a4], registerValues[a5]);
            }
        }
            break;
        case 261: //prlimit64
        {
            log_general("Emulate syscall prlimit64 (261)...\n");
            registerValues[a0] = syscall4(__NR_prlimit64, registerValues[a0], registerValues[a1], registerValues[a2],
                                          registerValues[a3]);
        }
            break;
        default:
            dprintf(2, "Syscall %li is not yet implemented\n", registerValues[a7]);
            registerValues[a0] = -ENOSYS;
            break;
    }
}
