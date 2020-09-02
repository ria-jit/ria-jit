//
// Created by flo on 21.05.20.
//

#ifndef DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_OTHER_HPP
#define DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_OTHER_HPP

#include <util/log.h>
#include <gen/translate.hpp>
#include <util/typedefs.h>

void translate_FENCE(const t_risc_instr &instr, const register_info &r_info);

void translate_ECALL(const t_risc_instr &instr, const register_info &r_info);

void translate_EBREAK(const t_risc_instr &instr, const register_info &r_info);

void translate_FENCE_I(const t_risc_instr &instr, const register_info &r_info);

#endif //DYNAMICBINARYTRANSLATORRISCV64_X86_64_TRANSLATE_OTHER_HPP
