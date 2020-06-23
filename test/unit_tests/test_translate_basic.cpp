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

/**
 * Tests init_block() and finalize_block() of translate.cpp by emitting and testing multiple code blocks.
 */
TEST(Translator, MultipleCodeBlocks) {
    //init block A
    init_block();

    //A: size_t x -> 2 * x
    a->shl(asmjit::x86::rdi, 1);
    a->mov(asmjit::x86::rax, asmjit::x86::rdi);

    t_cache_loc blockA = finalize_block();

    //init block B
    init_block();

    //B: size_t x -> 8 * x + 1
    a->shl(asmjit::x86::rdi, 3);
    a->add(asmjit::x86::rdi, 1);
    a->mov(asmjit::x86::rax, asmjit::x86::rdi);

    t_cache_loc blockB = finalize_block();

    ASSERT_NE(blockA, blockB);

    typedef size_t (*math_asm)(size_t);

    //testing values
    for (size_t i = 1; i <= 50; i++) {
        size_t expectA = 2 * i;
        size_t expectB = 8 * i + 1;

        EXPECT_EQ(expectA, ((math_asm) blockA)(i));
        EXPECT_EQ(expectB, ((math_asm) blockB)(i));
    }
}