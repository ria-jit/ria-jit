//
// Created by simon on 05.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H


#define INVALID_ELF_MAP 0

/**
 * Maps all LOAD segments of the ELF file at the given path into the correct memory regions.
 * @param filePath the path to the ELF file
 * @return the entry address of the ELF or INVALID_ELF_MAP if the mapping failed.
 */
t_risc_addr mapIntoMemory(char *filePath);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H
