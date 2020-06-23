//
// Created by Simon on 03.05.2020.
// Based on work of flo.
// ASM file assembled with riscv64-linux-gnu-as, linked with riscv64-linux-gnu-ld
//

#include <common.h>
#include <linux/mman.h>
#include <stdio.h>
#include <string.h>
#include "loadElf.h"

//Apparently not included in the headers on my version.
#ifndef EF_RISCV_RVE
#define EF_RISCV_RVE 0x8
#endif
#ifndef EF_RISCV_TSO
#define EF_RISCV_TSO 0x10
#endif

//TODO Figure out proper offset
#define STACK_OFFSET 0x10000000

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


    Elf64_Half ph_count = header.e_phnum;
    Elf64_Off ph_offset = header.e_phoff;
    Elf64_Word flags = header.e_flags;
    Elf64_Half phentsize = header.e_phentsize;
    Elf64_Addr entry = header.e_entry;
    bool incompatible = false;

    //Check for not supported ABI Flags
    if (flags & EF_RISCV_RVC) {
        not_yet_implemented("C ABI is not yet supported");
        incompatible = true;
    }
    if (flags & EF_RISCV_FLOAT_ABI_SINGLE) {
        not_yet_implemented("F ABI is not yet supported");
        incompatible = true;
    }
    if (flags & EF_RISCV_FLOAT_ABI_DOUBLE) {
        not_yet_implemented("D ABI is not yet supported");
        incompatible = true;
    }
    if (flags & EF_RISCV_FLOAT_ABI_QUAD) {
        not_yet_implemented("Q ABI is not yet supported");
        incompatible = true;
    }
    if (flags & EF_RISCV_RVE) {
        not_yet_implemented("E ABI is not yet supported");
        incompatible = true;
    }
    if (flags & EF_RISCV_TSO) {
        not_yet_implemented("TSO ABI is not yet supported");
        incompatible = true;
    }
    if (incompatible) {
        //return INVALID_ELF_MAP;
    }

    Elf64_Addr minAddr = 0, maxAddr = 0;
    t_risc_addr load_addr = 0;
    off_t fileOffset = lseek(fd, ph_offset, SEEK_SET);
    if (fileOffset < 0) {
        dprintf(2, "Could not seek file, error %li", -fileOffset);
        return INVALID_ELF_MAP;
    }
    for(int i = 0; i < ph_count; i++) {
        Elf64_Phdr segment;
        ssize_t segmentBytes = read_full(fd, (void *) &segment, sizeof(Elf64_Phdr));
        if (segmentBytes <= 0) {
            dprintf(2, "Could not read header for segment %i, error %li", i, -segmentBytes);
            return INVALID_ELF_MAP;
        }
        switch(segment.p_type) {
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
            case PT_DYNAMIC: //Fallthrough
            case PT_INTERP: {
                dprintf(2, "Bad. Got file that needs dynamic linking.");
                return INVALID_ELF_MAP;
            }

        }
    }
    Elf64_Addr startAddr = ALIGN_DOWN(minAddr, 4096);
    Elf64_Addr endAddr = ALIGN_UP(maxAddr, 4096);
    //Allocate the whole address space that is needed (Should not be READ/WRITE everywhere but I am too lazy right now).
    void *elf = mmap_mini((void *) startAddr, endAddr - startAddr, PROT_READ | PROT_WRITE,
                          MAP_FIXED_NOREPLACE | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    //Failed means that we couldn't get enough memory at the correct address
    if (BAD_ADDR(elf)) {
        dprintf(2, "Could not map elf because error %s", strerror(-(intptr_t) elf));//-(intptr_t) elf
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
    for(int i = 0; i < ph_count; i++) {
        Elf64_Phdr segment;
        ssize_t segmentBytes = read_full(fd, (void *) &segment, sizeof(Elf64_Phdr));
        if (segmentBytes <= 0) {
            dprintf(2, "Could not read header for segment %i, error %li", i, -segmentBytes);
            return INVALID_ELF_MAP;
        }
        switch(segment.p_type) {
            case PT_LOAD: {
                Elf64_Off load_offset = segment.p_offset;
                Elf64_Xword memory_size = segment.p_memsz;
                Elf64_Xword physical_size = segment.p_filesz;
                Elf64_Addr vaddr = segment.p_vaddr;
                //Copy flags over (not used right now to be implemented later)
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
                fileOffset = lseek(fd2, load_offset, SEEK_SET);
                if (fileOffset < 0) {
                    dprintf(2, "Could not seek file, error %li", -fileOffset);
                    return INVALID_ELF_MAP;
                }
                ssize_t segmentMemoryBytes = read_full(fd2, (void *) ((void *) vaddr), physical_size);
                if (segmentBytes <= 0) {
                    dprintf(2, "Could not load segment %i, error %li", i, -segmentMemoryBytes);
                    return INVALID_ELF_MAP;
                }
                break;
            }
        }
    }
    t_risc_addr phdr = load_addr + ph_offset;
    close(fd);
    close(fd2);

    return (t_risc_elf_map_result) {true, entry, phdr, ph_count, phentsize};
}

t_risc_addr allocateStack() {
    size_t stackSize = 8 * 1024 * 1024; //Default stack size

    struct rlimit rlimit;
    //Try to get the stacksize from kernel if unsuccessful or infinite use default
    //0 return means successful.
    if (getrlimit(RLIMIT_STACK, &rlimit) == 0 && rlimit.rlim_cur != RLIM_INFINITY) {
        stackSize = rlimit.rlim_cur; //Stack size from kernel
    }

    //Add guard page at bottom just in case.
    size_t guard = 4096;

    //Allocate the stack with offset under the translator region where the cached blocks can go.
    uintptr_t stackStart = TRANSLATOR_BASE - STACK_OFFSET - (stackSize + guard + 4096);
    void *bottomOfStack = mmap_mini((void *) stackStart, stackSize + guard, PROT_WRITE | PROT_READ,
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
    ///Add auxv to guest stack
    {
        Elf64_auxv_t *stack = (Elf64_auxv_t *) stackPos;
        *(--stack) = (Elf64_auxv_t) {AT_NULL};
        *(--stack) = (Elf64_auxv_t) {AT_ENTRY, {mapInfo.entry}};
        *(--stack) = (Elf64_auxv_t) {AT_PHDR, {mapInfo.entry}};
        *(--stack) = (Elf64_auxv_t) {AT_PHNUM, {mapInfo.ph_count}};
        *(--stack) = (Elf64_auxv_t) {AT_PHENT, {mapInfo.ph_entsize}};
//        *(--stack) = (Elf64_auxv_t) {AT_UID, {getauxval(AT_UID)}}; //TODO getauxval does not work since
//        *(--stack) = (Elf64_auxv_t) {AT_GID, {getauxval(AT_GID)}}; // auxvptr is not initialized
//        *(--stack) = (Elf64_auxv_t) {AT_EGID, {getauxval(AT_EGID)}};
//        *(--stack) = (Elf64_auxv_t) {AT_EUID, {getauxval(AT_EUID)}};
//        *(--stack) = (Elf64_auxv_t) {AT_CLKTCK, {getauxval(AT_CLKTCK)}};
//        *(--stack) = (Elf64_auxv_t) {AT_RANDOM, {getauxval(AT_RANDOM)}}; //TODO Copy/Generate new one?
        *(--stack) = (Elf64_auxv_t) {AT_SECURE, {0}};
        *(--stack) = (Elf64_auxv_t) {AT_PAGESZ, {4096}};
        *(--stack) = (Elf64_auxv_t) {AT_HWCAP, {0}}; //Seems to not be defined for RISCV
        *(--stack) = (Elf64_auxv_t) {AT_HWCAP2, {0}}; //Seems to not be defined for RISCV
//        *(--stack) = (Elf64_auxv_t) {AT_PLATFORM, {0}}; //Seems to not be defined for RISCV
        *(--stack) = (Elf64_auxv_t) {AT_EXECFN, {(uintptr_t) guestArgv[0]}};
        stackPos = (t_risc_addr) stack;
    }
    ///Copy envp to guest stack
    {
        int envc = 0;
        for(; __environ[envc]; ++envc);
        char **stack = (char **) stackPos;
        //Zero terminated so environ[envc] will be zero and also needs to be copied
        for(int i = envc - 1; i >= 0; i--) {
            *(--stack) = __environ[i];
        }
        for(int i = 0; stack[i] || __environ[i]; ++i) {
            if (stack[i] != __environ[i]) {
                log_general("Difference in envp %p and environ %p\n", stack[i], __environ[i]);
            }
        }
        stackPos = (t_risc_addr) stack;
    }
    ///Copy guestArgv to guest stack
    {
        const char **stack = (const char **) stackPos;
        //Zero terminated so guestArgv[guestArgc] will be zero and also needs to be copied
        for(int i = guestArgc - 1; i >= 0; i--) {
            *(--stack) = guestArgv[i];
        }
        stackPos = (t_risc_addr) stack;
    }
    ///Copy guestArgc to guest stack
    {
        int *stack = (int *) stackPos;
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
    t_risc_addr argsToStack = copyArgsToStack(stack, guestArgc, guestArgv, mapInfo);
    ///Stack pointer always needs to be 16-Byte aligned per ABI convention
    return ALIGN_DOWN(argsToStack, 16lu);

}
