//
// Created by flo on 12.06.20.
//

#include<gtest/gtest.h>
#include <translate.hpp>

uint64_t numbers[10];

TEST(AsmJitFunctionality, ShouldAssemblePointersCorrectly) {
    //compile testing block with memory array access
    uint64_t base = reinterpret_cast<uint64_t>(numbers);
    size_t offset = 2;
    numbers[offset] = 10;

    init_block();
    //experimenting here: seems we will have to calculate the addressing offset at translate-time!
    a->mov(asmjit::x86::rax, asmjit::x86::ptr(base + 8 * offset));
    a->add(asmjit::x86::rax, 3);
    a->mov(asmjit::x86::ptr(base + 8 * offset), asmjit::x86::rax);
    t_cache_loc block = finalize_block();

    typedef void (*executable)(void);
    ((executable) block)();

    //array at offset should now contain 13
    ASSERT_EQ(13, numbers[offset]);
}