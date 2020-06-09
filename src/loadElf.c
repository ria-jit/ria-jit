//
// Created by Simon on 03.05.2020.
// Based on work of flo.
// ASM file assembled with riscv64-linux-gnu-as, linked with riscv64-linux-gnu-ld
//

#include "../lib/common.h"
#include <linux/mman.h>
#include "util.h"
#include "loadElf.h"

//Apparently not included in the headers on my version.
#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x200000
#endif
#ifndef EF_RISCV_RVE
#define EF_RISCV_RVE 0x8
#endif
#ifndef EF_RISCV_TSO
#define EF_RISCV_TSO 0x10
#endif

t_risc_elf_map_result mapIntoMemory(const char *filePath) {
    printf("Reading %s...\n", filePath);

    //get the file descriptor
    int fd = open(filePath, O_RDONLY, 0);

    //get the size via fstatx to map it into memory
    struct statx statxbuf;
    fstatx(fd, &statxbuf);
    __off_t size = statxbuf.stx_size;

    //map the executable file into memory
    //Try putting it closely above TRANSLATOR_BASE so it goes in our address space and the other regions are free for
    // the mapping.
    char *exec = mmap((void *) (TRANSLATOR_BASE + 0x10000000), size, PROT_READ, MAP_SHARED, fd, 0);

    //read as elf header
    Elf64_Ehdr *header = (Elf64_Ehdr *) exec;

    //get program headers by offset in file
    Elf64_Half ph_count = header->e_phnum;
    Elf64_Off ph_offset = header->e_phoff;
    Elf64_Phdr *segments = (Elf64_Phdr *) (exec + ph_offset);
    Elf64_Word flags = header->e_flags;
    Elf64_Half phentsize = header->e_phentsize;
    Elf64_Addr entry = header->e_entry;
    bool incompatible = false;

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
        return INVALID_ELF_MAP;
    }
    t_risc_addr load_addr = 0;
    for(int i = 0; i < ph_count; i++) {
        Elf64_Phdr *segment = segments + i;
        switch(segment->p_type) {
            case PT_LOAD: {
                Elf64_Off load_offset = segment->p_offset;
                Elf64_Xword memory_size = segment->p_memsz;
                Elf64_Xword physical_size = segment->p_filesz;
                Elf64_Addr vaddr = segment->p_vaddr;
                printf("Found segment at file offset 0x%lx with virtual address 0x%lx (virtual size 0x%lx, "
                       "physical size 0x%lx).\n", load_offset, vaddr, memory_size, physical_size);
                //Refuse to map to a location in the address space of the translator.
                if ((vaddr + memory_size) > TRANSLATOR_BASE) {
                    printf("Bad. This segment wants to be in the translators memory region");
                    return INVALID_ELF_MAP;
                }
                if (!load_addr) {
                    load_addr = vaddr - load_offset;
                }
                //Copy flags over
                int prot = 0;
                if (segment->p_flags & PF_R) {
                    prot |= PROT_READ;
                }
                if (segment->p_flags & PF_W) {
                    prot |= PROT_WRITE;
                }
                if (segment->p_flags & PF_X) {
                    prot |= PROT_EXEC; //Probably not even needed
                }
                //Allocate memory for the segment at the correct address and map the file segment into that memory
                void *segment_in_memory =
                        mmap((void *) vaddr, memory_size, prot, MAP_FIXED_NOREPLACE + MAP_PRIVATE, fd,
                             load_offset);
                //Failed means that we couldn't get enough memory at the correct address
                if (BAD_ADDR(segment_in_memory)) {
                    printf("Could not map segment %i because error %li", i, -(intptr_t) segment_in_memory);
                    return INVALID_ELF_MAP;
                }
                //Check in case MAP_FIXED_NOREPLACE is not supported on that kernel version.
                if (segment_in_memory != (void *) vaddr) {
                    printf("Did not get the correct memory address for segment %i.", i);
                    return INVALID_ELF_MAP;
                }
                //Initialize additional memory to 0.
                if (memory_size > physical_size) {
                    memset((segment_in_memory + physical_size), 0, memory_size - physical_size);
                }
                break;
            }
            case PT_DYNAMIC: //Fallthrough
            case PT_INTERP: {
                printf("Bad. Got file that needs dynamic linking.");
                return INVALID_ELF_MAP;
            }

        }
    }
    t_risc_addr phdr = load_addr + ph_offset;
    //Close and unmap the elf file
    munmap(exec, size);
    close(fd);

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

    uintptr_t stackStart = TRANSLATOR_BASE - (stackSize + guard + 4096);
    void *bottomOfStack = mmap((void *) stackStart, stackSize + guard, PROT_WRITE | PROT_READ,
                               MAP_ANONYMOUS | MAP_STACK | MAP_PRIVATE | MAP_FIXED_NOREPLACE, -1, 0);

    //Failed means that we couldn't get enough memory at the correct address
    if (BAD_ADDR(bottomOfStack)) {
        printf("Could not map stack because error %li", -(uintptr_t) (bottomOfStack));
        return INVALID_STACK;
    }
    //Check in case MAP_FIXED_NOREPLACE is not supported on that kernel version.
    if ((uintptr_t) bottomOfStack != stackStart) {
        printf("Did not get the correct memory address for stack");
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
        stackPos = (t_risc_addr) stack;
    }
    ///Copy envp to guest stack
    {
        int envc = 0;
        for(; environ[envc]; ++envc);
        char **stack = (char **) stackPos;
        //Zero terminated so environ[envc] will be zero and also needs to be copied
        for(int i = envc; i >= 0; i--) {
            *(--stack) = environ[i];
        }
        for(int i = 0; stack[i] || environ[i]; ++i) {
            if (stack[i] != environ[i]) {
                printf("Difference in envp %p and environ %p\n", stack[i], environ[i]);
            }
        }
        stackPos = (t_risc_addr) stack;
    }
    ///Copy guestArgv to guest stack
    {
        const char **stack = (const char **) stackPos;
        //Zero terminated so guestArgv[guestArgc] will be zero and also needs to be copied
        for(int i = guestArgc; i >= 0; i--) {
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
    return ALIGN_DOWN(argsToStack, 16u);

}
