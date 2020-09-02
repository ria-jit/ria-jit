//
// Created by Flo Schmidt on 27.07.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_PERF_H
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_PERF_H

#include <common.h>

struct timespec begin_measure();

void end_display_measure(struct timespec *start);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_PERF_H
