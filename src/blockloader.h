//
// Created by jhne on 5/5/20.
//

//------------------------------------OBSOLETE----------------------------------------
//this functionality is now in translate, will remove file later

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_BLOCKLOADER_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_BLOCKLOADER_H

#include "util.h"
#include "cache.h"
#include "parser.h"

//returns number of structs written into block_cache
int get_block(t_risc_instr* block_cache, t_risc_addr risc_addr);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_BLOCKLOADER_H
