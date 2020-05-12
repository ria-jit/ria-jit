//
// Created by Simon on 03.05.2020.
// Based on work of flo.
// ASM file assembled with riscv64-linux-gnu-as, linked with riscv64-linux-gnu-ld
//

#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "util.h"
#include "loadElf.h"
//Apparently not included in the headers on my version.
#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x200000
#endif

t_risc_addr mapIntoMemory(char *filePath) {
    printf("Reading %s...\n", filePath);

    //get the file descriptor
    int fd = open(filePath, O_RDONLY);

    //get the size via fstat to map it into memory
    struct stat statbuf;
    fstat(fd, &statbuf);
    __off_t size = statbuf.st_size;

    //map the executable file into memory
    char *exec = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);

    //read as elf header
    Elf64_Ehdr *header = (Elf64_Ehdr *) exec;

    //get program headers by offset in file
    Elf64_Half ph_count = header->e_phnum;
    Elf64_Off ph_offset = header->e_phoff;
    Elf64_Phdr *segments = (Elf64_Phdr *) (exec + ph_offset);

    for(int i = 0; i < ph_count; i++) {
        Elf64_Phdr segment = segments[i];
        switch(segment.p_type) {
            case PT_LOAD: {
                Elf64_Off load_offset = segment.p_offset;
                Elf64_Xword memory_size = segment.p_memsz;
                Elf64_Xword physical_size = segment.p_filesz;
                Elf64_Addr vaddr = segment.p_vaddr;
                printf("Found segment at file offset 0x%lx with virtual address 0x%lx (virtual size 0x%lx, "
                       "physical size 0x%lx).\n", load_offset, vaddr, memory_size, physical_size);
                //Refuse to map to a location in the address space of the translator.
                if ((vaddr + memory_size) > TRANSLATOR_BASE) {
                    printf("Bad. This segment wants to be in the translators memory region");
                    return INVALID_ELF_MAP;
                }
                //Allocate memory for the segment at the correct address and map the file segment into that memory
                void *segment_in_memory =
                        mmap((void *) vaddr, memory_size, PROT_READ, MAP_FIXED_NOREPLACE + MAP_PRIVATE, fd,
                             load_offset);
                //Failed means that we couldn't get enough memory at the correct address
                if (segment_in_memory == MAP_FAILED) {
                    printf("Could not map segment %i because error %i", i, errno);
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

    close(fd);


    //Entry address for start of translation.
    return header->e_entry;
}
