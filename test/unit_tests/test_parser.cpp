//
// Created by flo on 07.06.20.
//

#include <gtest/gtest.h>
#include <parser/parser.h>
#include <util/log.h>

/**
 * Tests parsing of basic load-immediate as per test_parsing() in parser.c (--> @author Noah)
 */
TEST(Parser, BasicOpcodeParsing) {
    unsigned int *memory = (unsigned int *) malloc(0x10);
    memory[0] = 0x38537;
    memory[1] = 0xab75051b;
    memory[2] = 0xe51513;
    memory[3] = 0xeef50513;

    //parsing result array
    t_risc_instr parsed[4];

    //ignored register count
    uint32_t reg_count;

    for (int i = 0; i < 4; i++) {
        parsed[i].addr = (t_risc_addr) &memory[i];
        parse_instruction(&parsed[i], &reg_count);
    }

    /*
     * dissassembly of li a0, 0xDEADBEEF
     *  00038537                lui     a0,0x38
     *  ab75051b                addiw   a0,a0,-1353
     *  00e51513                slli    a0,a0,0xe
     *  eef50513                addi    a0,a0,-273 # 37eef <__global_pointer$+0x26667>
     */

    EXPECT_EQ(LUI, parsed[0].mnem);
    EXPECT_EQ(0x38 << 12, parsed[0].imm);
    EXPECT_EQ(a0, parsed[0].reg_dest);

    EXPECT_EQ(ADDIW, parsed[1].mnem);
    EXPECT_EQ(-1353, parsed[1].imm);
    EXPECT_EQ(a0, parsed[1].reg_src_1);
    EXPECT_EQ(a0, parsed[1].reg_dest);

    EXPECT_EQ(SLLI, parsed[2].mnem);
    EXPECT_EQ(0xe, parsed[2].imm & 0b111111);
    EXPECT_EQ(a0, parsed[2].reg_src_1);
    EXPECT_EQ(a0, parsed[2].reg_dest);

    EXPECT_EQ(ADDI, parsed[3].mnem);
    EXPECT_EQ(a0, parsed[3].reg_src_1);
    EXPECT_EQ(a0, parsed[3].reg_dest);
    EXPECT_EQ(-273, parsed[3].imm);
}