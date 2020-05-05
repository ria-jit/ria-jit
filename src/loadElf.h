//
// Created by simon on 05.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H

typedef struct map_result {
    bool valid;
    t_risc_addr startAddress;
} t_map_result;

t_map_result mapMemory(char *filePath);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_LOADELF_H
