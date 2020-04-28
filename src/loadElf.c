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
#include <unistd.h>


typedef int t_map_result;

t_map_result mapMemory(char *filePath) {
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
                break;
            }
            case PT_DYNAMIC: //Fallthrough
            case PT_INTERP: {
                printf("Bad. Got file that needs dynamic linking.");
                return 1;
            }

        }
    }

    close(fd);

    return 0;
}