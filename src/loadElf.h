//
// Created by simon on 05.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H

#include <elf.h>

#define INVALID_ELF_MAP (t_risc_elf_map_result){.valid = false}
#define INVALID_STACK 0
/**
 * Struct containing map metadata. If valid is false the other values are undefined.
 */
typedef struct {
    bool valid;
    t_risc_addr entry;
    t_risc_addr phdr;
    Elf64_Half ph_count;
    Elf64_Half ph_entsize;
} t_risc_elf_map_result;

/**
 * Maps all LOAD segments of the ELF file at the given path into the correct memory regions.
 * @param filePath the path to the ELF file.
 * @return t_risc_elf_map_result the map result containing or INVALID_ELF_MAP if the mapping failed.
 */
t_risc_elf_map_result mapIntoMemory(const char *filePath);

/**
 * Maps a stack for the program into memory and copies argc, argv, envp, and auxv onto it.
 * @param guestArgc the number of arguments passed on to the guest program. (Should be equal to argc - optind + 1
 * after parsing in main This bracketed bit is just an impl note and can be deleted whenever.)
 * @param guestArgv the array containing pointers to the argument strings passed on to the guest program. (The
 * easiest way to get this would probably to rearrange the the argv in the main function so all guest program
 * arguments are after a certain point and then put the pointer to the filename string in front of there. The cmd
 * line could then look like dynamic-translate -f <filename> [-v][…] -- <guest programm arguments>. And then
 * after parsing the arguments in main we could exchange the pointer to '--' with the filename pointer. optind should
 * then be the index of the first guest program argument so this should be easy. Although getopt() will probably not
 * work with -nostdlib, so we will have to write our own implementation... This bracketed bit is just an impl note
 * and can be deleted whenever.)
 * @param mapInfo the map result returned by the mapIntoMemory call.
 * @return the 16-Byte aligned address of the start stack pointer.
 */
t_risc_addr createStack(int guestArgc, char *guestArgv[], t_risc_elf_map_result mapInfo);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H
