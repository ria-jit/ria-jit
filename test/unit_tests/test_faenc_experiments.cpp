//
// Created by flo on 06.07.20.
//

#include <gtest/gtest.h>
#include <fadec/fadec-enc.h>
#include <fadec/fadec.h>

TEST(FaencExperiment, ShouldEncode) {
    int failed = 0;
    uint8_t buf[64];

    uint8_t *current = buf;

    //fixme this does not compile yet as the instruction mnemonics are missing?
    /*failed |= fe_enc64(&current, FE_XOR32rr, FE_AX, FE_AX);
    failed |= fe_enc64(&current, FE_ADD64ri, FE_AX, 3);
    failed |= fe_enc64(&current, FE_RET);*/

    typedef int (*void_asm)();
    int retVal = ((void_asm) buf)();
    ASSERT_EQ(retVal, 3);
}