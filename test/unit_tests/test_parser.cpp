//
// Created by flo on 07.06.20.
//

#include <gtest/gtest.h>
#include <parser/parser.h>
#include <util/log.h>
#include <elf/loadElf.h>

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

    for (int i = 0; i < 4; i++) {
        parsed[i].addr = (t_risc_addr) &memory[i];
        parse_instruction(&parsed[i]);
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

/**
 * Tests parsing of basic load-immediate as per test_parsing() in parser.c (--> @author Noah)
 */
TEST(Parser, CheckAllOpcodes) {
    const int len = 92;
    int32_t instr[len];

    //fill memory with assembled instructions
    instr[0] = 0x0002a5b7;
    instr[1] = 0x0002a597;
    instr[2] = 0xff9ff5ef;
    instr[3] = 0x02a605e7;
    instr[4] = 0xff2588e3;
    instr[5] = 0xfec596e3;
    instr[6] = 0xfec5c4e3;
    instr[7] = 0xfec5d2e3;
    instr[8] = 0xfec5e0e3;
    instr[9] = 0xfcc5fee3;
    instr[10] = 0x60583;
    instr[11] = 0x61583;
    instr[12] = 0x62583;
    instr[13] = 0x64583;
    instr[14] = 0x65583;
    instr[15] = 0x00b60023;
    instr[16] = 0x00b61023;
    instr[17] = 0x00b62023;
    instr[18] = 0x02a60593;
    instr[19] = 0x02a62593;
    instr[20] = 0x02a63593;
    instr[21] = 0x02a64593;
    instr[22] = 0x02a66593;
    instr[23] = 0x02a67593;
    instr[24] = 0x02a61593;
    instr[25] = 0x02a65593;
    instr[26] = 0x42a65593;
    instr[27] = 0x00d605b3;
    instr[28] = 0x40d605b3;
    instr[29] = 0x00d615b3;
    instr[30] = 0x00d625b3;
    instr[31] = 0x00d635b3;
    instr[32] = 0x00d645b3;
    instr[33] = 0x00d655b3;
    instr[34] = 0x40d655b3;
    instr[35] = 0x00d665b3;
    instr[36] = 0x00d675b3;
    instr[37] = 0x0ff0000f;
    instr[38] = 0x73;
    instr[39] = 0x100073;
    instr[40] = 0x0000100f;
    instr[41] = 0x003615f3;
    instr[42] = 0x003625f3;
    instr[43] = 0x003635f3;
    instr[44] = 0x003455f3;
    instr[45] = 0x003465f3;
    instr[46] = 0x003475f3;
    instr[47] = 0x66583;
    instr[48] = 0x63583;
    instr[49] = 0x00b63023;
    instr[50] = 0x02a6059b;
    instr[51] = 0x0086159b;
    instr[52] = 0x0086559b;
    instr[53] = 0x4086559b;
    instr[54] = 0x00d605bb;
    instr[55] = 0x40d605bb;
    instr[56] = 0x00d615bb;
    instr[57] = 0x00d655bb;
    instr[58] = 0x40d655bb;
    instr[59] = 0x02d605b3;
    instr[60] = 0x02d615b3;
    instr[61] = 0x02d625b3;
    instr[62] = 0x02d635b3;
    instr[63] = 0x02d645b3;
    instr[64] = 0x02d655b3;
    instr[65] = 0x02d665b3;
    instr[66] = 0x02d675b3;
    instr[67] = 0x02d605bb;
    instr[68] = 0x02d645bb;
    instr[69] = 0x02d655bb;
    instr[70] = 0x02d665bb;
    instr[71] = 0x02d675bb;
    instr[72] = 0x100625af;
    instr[73] = 0x18c6a5af;
    instr[74] = 0x08c6a5af;
    instr[75] = 0x00c6a5af;
    instr[76] = 0x20c6a5af;
    instr[77] = 0x40c6a5af;
    instr[78] = 0x80c6a5af;
    instr[79] = 0xa0c6a5af;
    instr[80] = 0xc0c6a5af;
    instr[81] = 0xe0c6a5af;
    instr[82] = 0x100635af;
    instr[83] = 0x18c6b5af;
    instr[84] = 0x08c6b5af;
    instr[85] = 0x00c6b5af;
    instr[86] = 0x20c6b5af;
    instr[87] = 0x40c6b5af;
    instr[88] = 0x80c6b5af;
    instr[89] = 0xa0c6b5af;
    instr[90] = 0xc0c6b5af;
    instr[91] = 0xe0c6b5af;

    //parse the raw instructions
    t_risc_instr parsed[len];

    for (int i = 0; i < len; i++) {
        parsed[i].addr = (t_risc_addr) &instr[i];
        parse_instruction(&parsed[i]);
    }

    //check all mnemonics
    EXPECT_EQ(LUI, parsed[0].mnem);
    EXPECT_EQ(AUIPC, parsed[1].mnem);
    EXPECT_EQ(JAL, parsed[2].mnem);
    EXPECT_EQ(JALR, parsed[3].mnem);
    EXPECT_EQ(BEQ, parsed[4].mnem);
    EXPECT_EQ(BNE, parsed[5].mnem);
    EXPECT_EQ(BLT, parsed[6].mnem);
    EXPECT_EQ(BGE, parsed[7].mnem);
    EXPECT_EQ(BLTU, parsed[8].mnem);
    EXPECT_EQ(BGEU, parsed[9].mnem);
    EXPECT_EQ(LB, parsed[10].mnem);
    EXPECT_EQ(LH, parsed[11].mnem);
    EXPECT_EQ(LW, parsed[12].mnem);
    EXPECT_EQ(LBU, parsed[13].mnem);
    EXPECT_EQ(LHU, parsed[14].mnem);
    EXPECT_EQ(SB, parsed[15].mnem);
    EXPECT_EQ(SH, parsed[16].mnem);
    EXPECT_EQ(SW, parsed[17].mnem);
    EXPECT_EQ(ADDI, parsed[18].mnem);
    EXPECT_EQ(SLTI, parsed[19].mnem);
    EXPECT_EQ(SLTIU, parsed[20].mnem);
    EXPECT_EQ(XORI, parsed[21].mnem);
    EXPECT_EQ(ORI, parsed[22].mnem);
    EXPECT_EQ(ANDI, parsed[23].mnem);
    EXPECT_EQ(SLLI, parsed[24].mnem);
    EXPECT_EQ(SRLI, parsed[25].mnem);
    EXPECT_EQ(SRAI, parsed[26].mnem);
    EXPECT_EQ(ADD, parsed[27].mnem);
    EXPECT_EQ(SUB, parsed[28].mnem);
    EXPECT_EQ(SLL, parsed[29].mnem);
    EXPECT_EQ(SLT, parsed[30].mnem);
    EXPECT_EQ(SLTU, parsed[31].mnem);
    EXPECT_EQ(XOR, parsed[32].mnem);
    EXPECT_EQ(SRL, parsed[33].mnem);
    EXPECT_EQ(SRA, parsed[34].mnem);
    EXPECT_EQ(OR, parsed[35].mnem);
    EXPECT_EQ(AND, parsed[36].mnem);
    EXPECT_EQ(FENCE, parsed[37].mnem);
    EXPECT_EQ(ECALL, parsed[38].mnem);
    EXPECT_EQ(EBREAK, parsed[39].mnem);
    EXPECT_EQ(FENCE_I, parsed[40].mnem);
    EXPECT_EQ(CSRRW, parsed[41].mnem);
    EXPECT_EQ(CSRRS, parsed[42].mnem);
    EXPECT_EQ(CSRRC, parsed[43].mnem);
    EXPECT_EQ(CSRRWI, parsed[44].mnem);
    EXPECT_EQ(CSRRSI, parsed[45].mnem);
    EXPECT_EQ(CSRRCI, parsed[46].mnem);
    EXPECT_EQ(LWU, parsed[47].mnem);
    EXPECT_EQ(LD, parsed[48].mnem);
    EXPECT_EQ(SD, parsed[49].mnem);
    EXPECT_EQ(ADDIW, parsed[50].mnem);
    EXPECT_EQ(SLLIW, parsed[51].mnem);
    EXPECT_EQ(SRLIW, parsed[52].mnem);
    EXPECT_EQ(SRAIW, parsed[53].mnem);
    EXPECT_EQ(ADDW, parsed[54].mnem);
    EXPECT_EQ(SUBW, parsed[55].mnem);
    EXPECT_EQ(SLLW, parsed[56].mnem);
    EXPECT_EQ(SRLW, parsed[57].mnem);
    EXPECT_EQ(SRAW, parsed[58].mnem);
    EXPECT_EQ(MUL, parsed[59].mnem);
    EXPECT_EQ(MULH, parsed[60].mnem);
    EXPECT_EQ(MULHSU, parsed[61].mnem);
    EXPECT_EQ(MULHU, parsed[62].mnem);
    EXPECT_EQ(DIV, parsed[63].mnem);
    EXPECT_EQ(DIVU, parsed[64].mnem);
    EXPECT_EQ(REM, parsed[65].mnem);
    EXPECT_EQ(REMU, parsed[66].mnem);
    EXPECT_EQ(MULW, parsed[67].mnem);
    EXPECT_EQ(DIVW, parsed[68].mnem);
    EXPECT_EQ(DIVUW, parsed[69].mnem);
    EXPECT_EQ(REMW, parsed[70].mnem);
    EXPECT_EQ(REMUW, parsed[71].mnem);
    EXPECT_EQ(LRW, parsed[72].mnem);
    EXPECT_EQ(SCW, parsed[73].mnem);
    EXPECT_EQ(AMOSWAPW, parsed[74].mnem);
    EXPECT_EQ(AMOADDW, parsed[75].mnem);
    EXPECT_EQ(AMOXORW, parsed[76].mnem);
    EXPECT_EQ(AMOORW, parsed[77].mnem);
    EXPECT_EQ(AMOMINW, parsed[78].mnem);
    EXPECT_EQ(AMOMAXW, parsed[79].mnem);
    EXPECT_EQ(AMOMINUW, parsed[80].mnem);
    EXPECT_EQ(AMOMAXUW, parsed[81].mnem);
    EXPECT_EQ(LRD, parsed[82].mnem);
    EXPECT_EQ(SCD, parsed[83].mnem);
    EXPECT_EQ(AMOSWAPD, parsed[84].mnem);
    EXPECT_EQ(AMOADDD, parsed[85].mnem);
    EXPECT_EQ(AMOXORD, parsed[86].mnem);
    EXPECT_EQ(AMOORD, parsed[87].mnem);
    EXPECT_EQ(AMOMIND, parsed[88].mnem);
    EXPECT_EQ(AMOMAXD, parsed[89].mnem);
    EXPECT_EQ(AMOMINUD, parsed[90].mnem);
    EXPECT_EQ(AMOMAXUD, parsed[91].mnem);
}
