//
// Created by flo on 06.07.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

//Apparently not included in the headers on my version.
#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x200000
#endif

// TODO Only here because we need it places where common.h can't be included
#ifndef ALIGN_DOWN
#define ALIGN_DOWN(v, a) ((v) & ~((a)-1))
#endif
#ifndef ALIGN_UP
#define ALIGN_UP(v, a) (((v) + (a - 1)) & ~((a)-1))
#endif

#define FAIL_HEAP_ALLOC 0x1000


#ifdef __cplusplus
}
#endif

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TYPEDEFS_H
