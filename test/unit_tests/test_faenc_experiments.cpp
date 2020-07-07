//
// Created by flo on 06.07.20.
//

#include <gtest/gtest.h>
#include <fadec/fadec-enc.h>
#include <sys/mman.h>

TEST(FaencExperiment, ShouldEncode) {
    int failed = 0;
    void *map = mmap(NULL, 4096, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (map == MAP_FAILED) {
        GTEST_FATAL_FAILURE_("Could not allocate memory for instructions.");
    }
    uint8_t *bufM = static_cast<uint8_t *>(map);

    uint8_t *current = bufM;

    *(current++) = 0x90; //Insert NOP at the beginning so the code step in works (The Clion debugger somehow starts
    // the dissasembly at the address directly after the previous PC. So we insert some one-byte instruction).
    failed |= fe_enc64(&current, FE_XOR32rr, FE_AX, FE_AX);
    failed |= fe_enc64(&current, FE_ADD64ri, FE_AX, 3);
    failed |= fe_enc64(&current, FE_RET);

    typedef int (*void_asm)();
    int retVal = ((void_asm) bufM)();
    munmap(map, 4096);
    ASSERT_EQ(retVal, 3);
}