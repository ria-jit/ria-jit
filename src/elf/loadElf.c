//
// Created by Simon on 03.05.2020.
// Based on work of flo.
// ASM file assembled with riscv64-linux-gnu-as, linked with riscv64-linux-gnu-ld
//

#include <common.h>
#include <linux/mman.h>
#include <linux/fs.h>
#include <env/exit.h>
#include "loadElf.h"

// Older glibc versions don't include these flags
#ifndef EF_RISCV_RVC
#define EF_RISCV_RVC                    0x0001
#endif // !EF_RISCV_RVC
#ifndef EF_RISCV_FLOAT_ABI
#define EF_RISCV_FLOAT_ABI              0x0006
#endif // !EF_RISCV_FLOAT_ABI
#ifndef EF_RISCV_FLOAT_ABI_SOFT
#define EF_RISCV_FLOAT_ABI_SOFT         0x0000
#endif // !EF_RISCV_FLOAT_ABI_SOFT
#ifndef EF_RISCV_FLOAT_ABI_SINGLE
#define EF_RISCV_FLOAT_ABI_SINGLE       0x0002
#endif // !EF_RISCV_FLOAT_ABI_SINGLE
#ifndef EF_RISCV_FLOAT_ABI_DOUBLE
#define EF_RISCV_FLOAT_ABI_DOUBLE       0x0004
#endif // !EF_RISCV_FLOAT_ABI_DOUBLE
#ifndef EF_RISCV_FLOAT_ABI_QUAD
#define EF_RISCV_FLOAT_ABI_QUAD         0x0006
#endif // !EF_RISCV_FLOAT_ABI_QUAD
#ifndef EF_RISCV_RVE
#define EF_RISCV_RVE 0x8
#endif
#ifndef EF_RISCV_TSO
#define EF_RISCV_TSO 0x10
#endif

#define AUXC 16

#ifdef NO_STDLIB
#define envp environ
#else
#define envp __environ
#endif


size_t stackSize = 8 * 1024 * 1024; //Default stack size
//Add guard page at bottom just in case.
const size_t guard = 4096;

t_risc_elf_map_result mapIntoMemory(const char *filePath) {
    log_general("Reading %s...\n", filePath);

    //get the file descriptor
    int fd = open(filePath, O_RDONLY, 0);
    if (fd <= 0) {
        dprintf(2, "Could not open file, error %i\n", -fd);
        return INVALID_ELF_MAP;
    }

    //read as elf header
    Elf64_Ehdr header;
    ssize_t bytes = read_full(fd, (void *) &header, sizeof(Elf64_Ehdr));
    if (bytes <= 0) {
        dprintf(2, "Could not read header, error %li\n", -bytes);
        return INVALID_ELF_MAP;
    }

    if (header.e_machine != EM_RISCV) {
        dprintf(2, "Tried to translate a non-RISCV binary.");
        panic(FAIL_INCOMPATIBLE);
    }
    if (header.e_ident[EI_CLASS] != ELFCLASS64) {
        dprintf(2, "Tried executing a non-64bit binary.");
        panic(FAIL_INCOMPATIBLE);
    }
    if (header.e_type != ET_EXEC) {
        dprintf(2, "Tried executing a non-static binary.");
        panic(FAIL_INCOMPATIBLE);
    }

    Elf64_Half ph_count = header.e_phnum;
    Elf64_Off ph_offset = header.e_phoff;
    Elf64_Word flags = header.e_flags;
    Elf64_Half phentsize = header.e_phentsize;
    Elf64_Addr entry = header.e_entry;

    Elf64_Off sh_offset = header.e_shoff;
    Elf64_Half sh_count = header.e_shnum;

    bool incompatible = false;
    bool floatBinary = false;

    //Check for not supported ABI Flags
    if (flags & EF_RISCV_FLOAT_ABI_QUAD || flags & EF_RISCV_FLOAT_ABI_DOUBLE || flags & EF_RISCV_FLOAT_ABI_SINGLE) {
        floatBinary = true;
    }
    if (flags & EF_RISCV_RVE) {
        critical_not_yet_implemented("E ABI is not yet supported");
        incompatible = true;
    }
    if (flags & EF_RISCV_TSO) {
        critical_not_yet_implemented("TSO ABI is not yet supported");
        incompatible = true;
    }
    if (incompatible) {
        return INVALID_ELF_MAP;
    }

    off_t offsetSh = lseek(fd, sh_offset, SEEK_SET);
    if (offsetSh < 0) {
        dprintf(2, "Could not seek file, error %li", -offsetSh);
        return INVALID_ELF_MAP;
    }

    Elf64_Addr minAddrExec = 0, maxAddrExec = 0;
    for (int i = 0; i < sh_count; i++) {
        Elf64_Shdr section;
        ssize_t sectionBytes = read_full(fd, (void *) &section, sizeof(Elf64_Shdr));
        if (sectionBytes <= 0) {
            dprintf(2, "Could not read header for segment %i, error %li", i, -sectionBytes);
            return INVALID_ELF_MAP;
        }
        if (section.sh_flags & SHF_EXECINSTR) {
            Elf64_Addr shAddr = section.sh_addr;
            Elf64_Xword shSize = section.sh_size;
            //Update min and max addresses.
            if (!minAddrExec || minAddrExec > shAddr) {
                minAddrExec = shAddr;
            }
            if (!maxAddrExec || maxAddrExec < (shAddr + shSize)) {
                maxAddrExec = shAddr + shSize;
            }
        }
    }

    Elf64_Addr minAddr = 0, maxAddr = 0;
    t_risc_addr load_addr = 0;
    off_t fileOffset = lseek(fd, ph_offset, SEEK_SET);
    if (fileOffset < 0) {
        dprintf(2, "Could not seek file, error %li", -fileOffset);
        return INVALID_ELF_MAP;
    }
    for (int i = 0; i < ph_count; i++) {
        Elf64_Phdr segment;
        ssize_t segmentBytes = read_full(fd, (void *) &segment, sizeof(Elf64_Phdr));
        if (segmentBytes <= 0) {
            dprintf(2, "Could not read header for segment %i, error %li", i, -segmentBytes);
            return INVALID_ELF_MAP;
        }
        switch (segment.p_type) {
            case PT_LOAD: {
                Elf64_Off load_offset = segment.p_offset;
                Elf64_Xword memory_size = segment.p_memsz;
                Elf64_Xword physical_size = segment.p_filesz;
                Elf64_Addr vaddr = segment.p_vaddr;
                log_general("Found segment at file offset 0x%lx with virtual address 0x%lx (virtual size 0x%lx, "
                            "physical size 0x%lx).\n", load_offset, vaddr, memory_size, physical_size);
                //Refuse to map to a location in the address space of the translator.
                if ((vaddr + memory_size) > TRANSLATOR_BASE) {
                    dprintf(2, "Bad. This segment wants to be in the translators memory region");
                    return INVALID_ELF_MAP;
                }
                if (!load_addr) {
                    load_addr = vaddr - load_offset;
                }
                //Update min and max addresses.
                if (!minAddr || minAddr > vaddr) {
                    minAddr = vaddr;
                }
                if (!maxAddr || maxAddr < (vaddr + memory_size)) {
                    maxAddr = vaddr + memory_size;
                }
                break;
            }
            case PT_INTERP: {
                dprintf(2, "Bad. Got file that needs dynamic linking.");
                return INVALID_ELF_MAP;
            }
            default:
                break;
        }
    }
    Elf64_Addr startAddr = ALIGN_DOWN(minAddr, 4096lu);
    Elf64_Addr endAddr = ALIGN_UP(maxAddr, 4096lu);
    //Allocate the whole address space that is needed (TODO Should not be READ/WRITE everywhere but I am too lazy right
    // now).
    void *elf = mmap((void *) startAddr, endAddr - startAddr, PROT_READ | PROT_WRITE,
                     MAP_FIXED_NOREPLACE | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    //Failed means that we couldn't get enough memory at the correct address
    if (BAD_ADDR(elf)) {
        dprintf(2, "Could not map elf because error %li", -(intptr_t) elf);//-(intptr_t) elf
        return INVALID_ELF_MAP;
    }
    //Check in case MAP_FIXED_NOREPLACE is not supported on that kernel version.
    if (elf != (void *) startAddr) {
        dprintf(2, "Did not get the correct memory address for elf");
        return INVALID_ELF_MAP;
    }

    fileOffset = lseek(fd, ph_offset, SEEK_SET);
    if (fileOffset < 0) {
        dprintf(2, "Could not seek file, error %li", -fileOffset);
        return INVALID_ELF_MAP;
    }
    int fd2 = open(filePath, O_RDONLY, 0);
    if (fd2 <= 0) {
        dprintf(2, "Could not open file, error %i", -fd2);
        return INVALID_ELF_MAP;
    }
    for (int i = 0; i < ph_count; i++) {
        Elf64_Phdr segment;
        ssize_t segmentBytes = read_full(fd, (void *) &segment, sizeof(Elf64_Phdr));
        if (segmentBytes <= 0) {
            dprintf(2, "Could not read header for segment %i, error %li", i, -segmentBytes);
            return INVALID_ELF_MAP;
        }
        switch (segment.p_type) {
            case PT_LOAD: {
                Elf64_Off load_offset = segment.p_offset;
                Elf64_Xword physical_size = segment.p_filesz;
                Elf64_Addr vaddr = segment.p_vaddr;
                //Copy flags over (not used right now to be implemented later)
#if FALSE
                int prot = 0;
                if (segment.p_flags & PF_R) {
                    prot |= PROT_READ;
                }
                if (segment.p_flags & PF_W) {
                    prot |= PROT_WRITE;
                }
                if (segment.p_flags & PF_X) {
                    prot |= PROT_EXEC; //Probably not even needed
                }
#endif
                fileOffset = lseek(fd2, load_offset, SEEK_SET);
                if (fileOffset < 0) {
                    dprintf(2, "Could not seek file, error %li", -fileOffset);
                    return INVALID_ELF_MAP;
                }
                ssize_t segmentMemoryBytes = read_full(fd2, (void *) vaddr, physical_size);
                if (segmentBytes <= 0) {
                    dprintf(2, "Could not load segment %i, error %li", i, -segmentMemoryBytes);
                    return INVALID_ELF_MAP;
                }
                break;
            }
            default: {
                break;
            }
        }
    }
    t_risc_addr phdr = load_addr + ph_offset;
    close(fd);
    close(fd2);

    return (t_risc_elf_map_result) {true, entry, phdr, ph_count, phentsize, endAddr, minAddrExec, maxAddrExec,
            floatBinary};
}

t_risc_addr allocateStack() {

    struct rlimit rlimit;
    //Try to get the stacksize from kernel if unsuccessful or infinite use default
    //0 return means successful.
    if (getrlimit(RLIMIT_STACK, &rlimit) == 0 && rlimit.rlim_cur != RLIM_INFINITY) {
        stackSize = rlimit.rlim_cur; //Stack size from kernel
    }

    //Allocate the stack with offset under the translator region where the cached blocks can go.
    uintptr_t stackStart = TRANSLATOR_BASE - STACK_OFFSET - (stackSize + guard + 4096);
    void *bottomOfStack = mmap((void *) stackStart, stackSize + guard, PROT_WRITE | PROT_READ,
                               MAP_ANONYMOUS | MAP_STACK | MAP_PRIVATE | MAP_FIXED_NOREPLACE, -1, 0);

    //Failed means that we couldn't get enough memory at the correct address
    if (BAD_ADDR(bottomOfStack)) {
        dprintf(2, "Could not map stack because error %li", -(intptr_t) (bottomOfStack));
        return INVALID_STACK;
    }
    //Check in case MAP_FIXED_NOREPLACE is not supported on that kernel version.
    if ((uintptr_t) bottomOfStack != stackStart) {
        dprintf(2, "Did not get the correct memory address for stack");
        return INVALID_STACK;
    }

    //Make guard protected
    mprotect(bottomOfStack, guard, PROT_NONE);

    //Return top of stack
    return (t_risc_addr) bottomOfStack + guard + stackSize;
}

t_risc_addr copyArgsToStack(t_risc_addr stackPos, int guestArgc, char *guestArgv[], t_risc_elf_map_result
mapInfo) {
    ///Stack pointer always needs to be 16-Byte aligned per ABI convention
    int envc = 0;
    for (; envp[envc]; ++envc);
#ifndef NO_STDLIB
    //TODO This is a very temporary solution since the actual startup routine of minilib is not yet used.
    auxvptr_temp = (const size_t *) &__environ[envc + 1];
#endif
    size_t totalStackSize = sizeof(Elf64_auxv_t) * AUXC + sizeof(char **) * (envc + 1) + sizeof(char **) *
            (guestArgc + 1) + sizeof(long);
    uintptr_t stackOffset = ALIGN_UP(totalStackSize, 16lu) - totalStackSize;
    stackPos -= stackOffset;
    ///Add auxv to guest stack
    {
        Elf64_auxv_t *stack = (Elf64_auxv_t *) stackPos;
        *(--stack) = (Elf64_auxv_t) {AT_NULL};
        *(--stack) = (Elf64_auxv_t) {AT_ENTRY, {mapInfo.entry}};
        *(--stack) = (Elf64_auxv_t) {AT_PHDR, {mapInfo.phdr}};
        *(--stack) = (Elf64_auxv_t) {AT_PHNUM, {mapInfo.ph_count}};
        *(--stack) = (Elf64_auxv_t) {AT_PHENT, {mapInfo.ph_entsize}};
        *(--stack) = (Elf64_auxv_t) {AT_UID, {getauxval(AT_UID)}};
        *(--stack) = (Elf64_auxv_t) {AT_GID, {getauxval(AT_GID)}};
        *(--stack) = (Elf64_auxv_t) {AT_EGID, {getauxval(AT_EGID)}};
        *(--stack) = (Elf64_auxv_t) {AT_EUID, {getauxval(AT_EUID)}};
        *(--stack) = (Elf64_auxv_t) {AT_CLKTCK, {getauxval(AT_CLKTCK)}};
        *(--stack) = (Elf64_auxv_t) {AT_RANDOM, {getauxval(AT_RANDOM)}}; //TODO Copy/Generate new one?
        *(--stack) = (Elf64_auxv_t) {AT_SECURE, {0}};
        *(--stack) = (Elf64_auxv_t) {AT_PAGESZ, {4096}};
        *(--stack) = (Elf64_auxv_t) {AT_HWCAP, {0}}; //Seems to not be defined for RISCV
        *(--stack) = (Elf64_auxv_t) {AT_HWCAP2, {0}}; //Seems to not be defined for RISCV
//        *(--stack) = (Elf64_auxv_t) {AT_PLATFORM, {0}}; //Seems to not be defined for RISCV
        *(--stack) = (Elf64_auxv_t) {AT_EXECFN, {(uintptr_t) guestArgv[0]}};
        if (stackPos - (sizeof(Elf64_auxv_t) * AUXC) != (t_risc_addr) stack) {
            dprintf(2, "Not the expected number of auxv entries.");
            return 0;
        }
        stackPos = (t_risc_addr) stack;
    }
    ///Copy envp to guest stack
    {
        char **stack = (char **) stackPos;
        *(--stack) = NULL;
        //Zero terminated so environ[envc] will be zero and also needs to be copied
        for (int i = envc - 1; i >= 0; i--) {
            *(--stack) = envp[i];
        }
        for (int i = 0; stack[i] || envp[i]; ++i) {
            if (stack[i] != envp[i]) {
                log_general("Difference in envp %s and environ %s\n", stack[i], envp[i]);
            }
        }
        stackPos = (t_risc_addr) stack;
    }
    ///Copy guestArgv to guest stack
    {
        const char **stack = (const char **) stackPos;

        //Zero terminated so guestArgv[guestArgc] will be zero and also needs to be copied
        *(--stack) = NULL;
        for (int i = guestArgc - 1; i >= 0; i--) {
            *(--stack) = guestArgv[i];
        }
        stackPos = (t_risc_addr) stack;
    }
    ///Copy guestArgc to guest stack
    {
        long *stack = (long *) stackPos;
        *(--stack) = guestArgc;
        stackPos = (t_risc_addr) stack;
    }

    return (t_risc_addr) stackPos;
}


t_risc_addr createStack(int guestArgc, char **guestArgv, t_risc_elf_map_result mapInfo) {
    t_risc_addr stack = allocateStack();
    if (!stack) {
        return INVALID_STACK;
    }
    return copyArgsToStack(stack, guestArgc, guestArgv, mapInfo);

}
