//
// Created by flo on 07.06.20.
//

#include <gtest/gtest.h>
#include <translate.hpp>

/**
 * Tests AsmJit-based code-generation by emitting a strlen example function.
 */
TEST(Translator, StrlenExample) {
    const char *testing_str = "Hello, World!"; //len = 13

    //initialize new block
    init_block();

    //calling convention: first argument in rdi, ret value in rax
    asmjit::x86::Gp str = asmjit::x86::rdi;
    asmjit::x86::Gp len = asmjit::x86::rax;

    asmjit::Label loop_cond = a->newLabel();
    asmjit::Label loop_end = a->newLabel();

    a->xor_(len, len);
    a->bind(loop_cond);
    a->cmp(asmjit::x86::byte_ptr(str), 0);
    a->jz(loop_end);
    a->inc(len);
    a->inc(str);
    a->jmp(loop_cond);
    a->bind(loop_end);

    //finalize block and get cached location
    t_cache_loc executable = finalize_block();

    ASSERT_NE(nullptr, executable);

    //call the assembled function
    typedef size_t (*str_len_asm)(const char *);
    size_t ret = ((str_len_asm) executable)(testing_str);

    ASSERT_EQ(strlen(testing_str), ret);
}
