//
// Created by simon on 05.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H

#include <elf.h>
#include <util/log.h>
#include <util/util.h>

#define INVALID_ELF_MAP (t_risc_elf_map_result){.valid = false}
#define INVALID_STACK 0

//TODO Figure out proper offset
#define STACK_OFFSET 0x10000000
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

extern size_t stackSize;
extern const size_t guard;

/**
 * Struct containing map metadata. If valid is false the other values are undefined.
 */
typedef struct {
    bool valid;
    t_risc_addr entry;
    t_risc_addr phdr;
    Elf64_Half ph_count;
    Elf64_Half ph_entsize;
    t_risc_addr dataEnd;
    t_risc_addr execStart;
    t_risc_addr execEnd;
} t_risc_elf_map_result;

/**
 * Maps all LOAD segments of the ELF file at the given path into the correct memory regions.
 * @param filePath the path to the ELF file.
 * @return t_risc_elf_map_result the map result containing or INVALID_ELF_MAP if the mapping failed.
 */
t_risc_elf_map_result mapIntoMemory(const char *filePath);

/**
 * Maps a stack for the program into memory and copies argc, argv, envp, and auxv onto it.
 * @param guestArgc the number of arguments passed on to the guest program.
 * @param guestArgv the array containing pointers to the argument strings passed on to the guest program.
 * @param mapInfo the map result returned by the mapIntoMemory call.
 * @return the 16-Byte aligned address of the start stack pointer.
 */
t_risc_addr createStack(int guestArgc, char *guestArgv[], t_risc_elf_map_result mapInfo);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H
