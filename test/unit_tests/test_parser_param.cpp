//
// Created by Flo Schmidt on 22.09.20.
//

#include <googletest-master/googletest/include/gtest/gtest.h>
#include <util/typedefs.h>
#include <parser/parser.h>

class ParserTest : public ::testing::TestWithParam
        <std::tuple<int32_t, t_risc_mnem>> {
protected:
    int32_t raw_instr{};
    t_risc_mnem expect_mnem{};

protected:
    ParserTest() {
        //retrieve params
        std::tie(raw_instr, expect_mnem) = GetParam();
    }
};

/**
 * Verifies that all parameterized instructions contain the correct mnemonic.
 */
TEST_P(ParserTest, AllMnemonics) {
    t_risc_instr parsed_instr;
    parsed_instr.addr = reinterpret_cast<t_risc_addr>(&raw_instr);
    parse_instruction(&parsed_instr);

    EXPECT_EQ(expect_mnem, parsed_instr.mnem);
}

INSTANTIATE_TEST_SUITE_P(LUI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x0002a5b7),
                                 testing::Values(LUI)
                         ));

INSTANTIATE_TEST_SUITE_P(AUIPC, ParserTest,
                         testing::Combine(
                                 testing::Values(0x0002a597),
                                 testing::Values(AUIPC)
                         ));

INSTANTIATE_TEST_SUITE_P(JAL, ParserTest,
                         testing::Combine(
                                 testing::Values(0xff9ff5ef),
                                 testing::Values(JAL)
                         ));

INSTANTIATE_TEST_SUITE_P(JALR, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02a605e7),
                                 testing::Values(JALR)
                         ));

INSTANTIATE_TEST_SUITE_P(BEQ, ParserTest,
                         testing::Combine(
                                 testing::Values(0xff2588e3),
                                 testing::Values(BEQ)
                         ));

INSTANTIATE_TEST_SUITE_P(BNE, ParserTest,
                         testing::Combine(
                                 testing::Values(0xfec596e3),
                                 testing::Values(BNE)
                         ));

INSTANTIATE_TEST_SUITE_P(BLT, ParserTest,
                         testing::Combine(
                                 testing::Values(0xfec5c4e3),
                                 testing::Values(BLT)
                         ));

INSTANTIATE_TEST_SUITE_P(BGE, ParserTest,
                         testing::Combine(
                                 testing::Values(0xfec5d2e3),
                                 testing::Values(BGE)
                         ));

INSTANTIATE_TEST_SUITE_P(BLTU, ParserTest,
                         testing::Combine(
                                 testing::Values(0xfec5e0e3),
                                 testing::Values(BLTU)
                         ));

INSTANTIATE_TEST_SUITE_P(BGEU, ParserTest,
                         testing::Combine(
                                 testing::Values(0xfcc5fee3),
                                 testing::Values(BGEU)
                         ));

INSTANTIATE_TEST_SUITE_P(LB, ParserTest,
                         testing::Combine(
                                 testing::Values(0x60583),
                                 testing::Values(LB)
                         ));

INSTANTIATE_TEST_SUITE_P(LH, ParserTest,
                         testing::Combine(
                                 testing::Values(0x61583),
                                 testing::Values(LH)
                         ));

INSTANTIATE_TEST_SUITE_P(LW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x62583),
                                 testing::Values(LW)
                         ));

INSTANTIATE_TEST_SUITE_P(LBU, ParserTest,
                         testing::Combine(
                                 testing::Values(0x64583),
                                 testing::Values(LBU)
                         ));

INSTANTIATE_TEST_SUITE_P(LHU, ParserTest,
                         testing::Combine(
                                 testing::Values(0x65583),
                                 testing::Values(LHU)
                         ));

INSTANTIATE_TEST_SUITE_P(SB, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00b60023),
                                 testing::Values(SB)
                         ));

INSTANTIATE_TEST_SUITE_P(SH, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00b61023),
                                 testing::Values(SH)
                         ));

INSTANTIATE_TEST_SUITE_P(SW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00b62023),
                                 testing::Values(SW)
                         ));

INSTANTIATE_TEST_SUITE_P(ADDI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02a60593),
                                 testing::Values(ADDI)
                         ));

INSTANTIATE_TEST_SUITE_P(SLTI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02a62593),
                                 testing::Values(SLTI)
                         ));

INSTANTIATE_TEST_SUITE_P(SLTIU, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02a63593),
                                 testing::Values(SLTIU)
                         ));

INSTANTIATE_TEST_SUITE_P(XORI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02a64593),
                                 testing::Values(XORI)
                         ));

INSTANTIATE_TEST_SUITE_P(ORI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02a66593),
                                 testing::Values(ORI)
                         ));

INSTANTIATE_TEST_SUITE_P(ANDI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02a67593),
                                 testing::Values(ANDI)
                         ));

INSTANTIATE_TEST_SUITE_P(SLLI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02a61593),
                                 testing::Values(SLLI)
                         ));

INSTANTIATE_TEST_SUITE_P(SRLI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02a65593),
                                 testing::Values(SRLI)
                         ));

INSTANTIATE_TEST_SUITE_P(SRAI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x42a65593),
                                 testing::Values(SRAI)
                         ));

INSTANTIATE_TEST_SUITE_P(ADD, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d605b3),
                                 testing::Values(ADD)
                         ));

INSTANTIATE_TEST_SUITE_P(SUB, ParserTest,
                         testing::Combine(
                                 testing::Values(0x40d605b3),
                                 testing::Values(SUB)
                         ));

INSTANTIATE_TEST_SUITE_P(SLL, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d615b3),
                                 testing::Values(SLL)
                         ));

INSTANTIATE_TEST_SUITE_P(SLT, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d625b3),
                                 testing::Values(SLT)
                         ));

INSTANTIATE_TEST_SUITE_P(SLTU, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d635b3),
                                 testing::Values(SLTU)
                         ));

INSTANTIATE_TEST_SUITE_P(XOR, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d645b3),
                                 testing::Values(XOR)
                         ));

INSTANTIATE_TEST_SUITE_P(SRL, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d655b3),
                                 testing::Values(SRL)
                         ));

INSTANTIATE_TEST_SUITE_P(SRA, ParserTest,
                         testing::Combine(
                                 testing::Values(0x40d655b3),
                                 testing::Values(SRA)
                         ));

INSTANTIATE_TEST_SUITE_P(OR, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d665b3),
                                 testing::Values(OR)
                         ));

INSTANTIATE_TEST_SUITE_P(AND, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d675b3),
                                 testing::Values(AND)
                         ));

INSTANTIATE_TEST_SUITE_P(FENCE, ParserTest,
                         testing::Combine(
                                 testing::Values(0x0ff0000f),
                                 testing::Values(FENCE)
                         ));

INSTANTIATE_TEST_SUITE_P(ECALL, ParserTest,
                         testing::Combine(
                                 testing::Values(0x73),
                                 testing::Values(ECALL)
                         ));

INSTANTIATE_TEST_SUITE_P(EBREAK, ParserTest,
                         testing::Combine(
                                 testing::Values(0x100073),
                                 testing::Values(EBREAK)
                         ));

INSTANTIATE_TEST_SUITE_P(FENCE_I, ParserTest,
                         testing::Combine(
                                 testing::Values(0x0000100f),
                                 testing::Values(FENCE_I)
                         ));

INSTANTIATE_TEST_SUITE_P(CSRRW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x003615f3),
                                 testing::Values(CSRRW)
                         ));

INSTANTIATE_TEST_SUITE_P(CSRRS, ParserTest,
                         testing::Combine(
                                 testing::Values(0x003625f3),
                                 testing::Values(CSRRS)
                         ));

INSTANTIATE_TEST_SUITE_P(CSRRC, ParserTest,
                         testing::Combine(
                                 testing::Values(0x003635f3),
                                 testing::Values(CSRRC)
                         ));

INSTANTIATE_TEST_SUITE_P(CSRRWI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x003455f3),
                                 testing::Values(CSRRWI)
                         ));

INSTANTIATE_TEST_SUITE_P(CSRRSI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x003465f3),
                                 testing::Values(CSRRSI)
                         ));

INSTANTIATE_TEST_SUITE_P(CSRRCI, ParserTest,
                         testing::Combine(
                                 testing::Values(0x003475f3),
                                 testing::Values(CSRRCI)
                         ));

INSTANTIATE_TEST_SUITE_P(LWU, ParserTest,
                         testing::Combine(
                                 testing::Values(0x66583),
                                 testing::Values(LWU)
                         ));

INSTANTIATE_TEST_SUITE_P(LD, ParserTest,
                         testing::Combine(
                                 testing::Values(0x63583),
                                 testing::Values(LD)
                         ));

INSTANTIATE_TEST_SUITE_P(SD, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00b63023),
                                 testing::Values(SD)
                         ));

INSTANTIATE_TEST_SUITE_P(ADDIW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02a6059b),
                                 testing::Values(ADDIW)
                         ));

INSTANTIATE_TEST_SUITE_P(SLLIW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x0086159b),
                                 testing::Values(SLLIW)
                         ));

INSTANTIATE_TEST_SUITE_P(SRLIW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x0086559b),
                                 testing::Values(SRLIW)
                         ));

INSTANTIATE_TEST_SUITE_P(SRAIW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x4086559b),
                                 testing::Values(SRAIW)
                         ));

INSTANTIATE_TEST_SUITE_P(ADDW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d605bb),
                                 testing::Values(ADDW)
                         ));

INSTANTIATE_TEST_SUITE_P(SUBW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x40d605bb),
                                 testing::Values(SUBW)
                         ));

INSTANTIATE_TEST_SUITE_P(SLLW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d615bb),
                                 testing::Values(SLLW)
                         ));

INSTANTIATE_TEST_SUITE_P(SRLW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00d655bb),
                                 testing::Values(SRLW)
                         ));

INSTANTIATE_TEST_SUITE_P(SRAW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x40d655bb),
                                 testing::Values(SRAW)
                         ));

INSTANTIATE_TEST_SUITE_P(MUL, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d605b3),
                                 testing::Values(MUL)
                         ));

INSTANTIATE_TEST_SUITE_P(MULH, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d615b3),
                                 testing::Values(MULH)
                         ));

INSTANTIATE_TEST_SUITE_P(MULHSU, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d625b3),
                                 testing::Values(MULHSU)
                         ));

INSTANTIATE_TEST_SUITE_P(MULHU, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d635b3),
                                 testing::Values(MULHU)
                         ));

INSTANTIATE_TEST_SUITE_P(DIV, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d645b3),
                                 testing::Values(DIV)
                         ));

INSTANTIATE_TEST_SUITE_P(DIVU, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d655b3),
                                 testing::Values(DIVU)
                         ));

INSTANTIATE_TEST_SUITE_P(REM, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d665b3),
                                 testing::Values(REM)
                         ));

INSTANTIATE_TEST_SUITE_P(REMU, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d675b3),
                                 testing::Values(REMU)
                         ));

INSTANTIATE_TEST_SUITE_P(MULW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d605bb),
                                 testing::Values(MULW)
                         ));

INSTANTIATE_TEST_SUITE_P(DIVW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d645bb),
                                 testing::Values(DIVW)
                         ));

INSTANTIATE_TEST_SUITE_P(DIVUW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d655bb),
                                 testing::Values(DIVUW)
                         ));

INSTANTIATE_TEST_SUITE_P(REMW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d665bb),
                                 testing::Values(REMW)
                         ));

INSTANTIATE_TEST_SUITE_P(REMUW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x02d675bb),
                                 testing::Values(REMUW)
                         ));

INSTANTIATE_TEST_SUITE_P(LRW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x100625af),
                                 testing::Values(LRW)
                         ));

INSTANTIATE_TEST_SUITE_P(SCW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x18c6a5af),
                                 testing::Values(SCW)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOSWAPW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x08c6a5af),
                                 testing::Values(AMOSWAPW)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOADDW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00c6a5af),
                                 testing::Values(AMOADDW)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOXORW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x20c6a5af),
                                 testing::Values(AMOXORW)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOORW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x40c6a5af),
                                 testing::Values(AMOORW)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOMINW, ParserTest,
                         testing::Combine(
                                 testing::Values(0x80c6a5af),
                                 testing::Values(AMOMINW)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOMAXW, ParserTest,
                         testing::Combine(
                                 testing::Values(0xa0c6a5af),
                                 testing::Values(AMOMAXW)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOMINUW, ParserTest,
                         testing::Combine(
                                 testing::Values(0xc0c6a5af),
                                 testing::Values(AMOMINUW)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOMAXUW, ParserTest,
                         testing::Combine(
                                 testing::Values(0xe0c6a5af),
                                 testing::Values(AMOMAXUW)
                         ));

INSTANTIATE_TEST_SUITE_P(LRD, ParserTest,
                         testing::Combine(
                                 testing::Values(0x100635af),
                                 testing::Values(LRD)
                         ));

INSTANTIATE_TEST_SUITE_P(SCD, ParserTest,
                         testing::Combine(
                                 testing::Values(0x18c6b5af),
                                 testing::Values(SCD)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOSWAPD, ParserTest,
                         testing::Combine(
                                 testing::Values(0x08c6b5af),
                                 testing::Values(AMOSWAPD)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOADDD, ParserTest,
                         testing::Combine(
                                 testing::Values(0x00c6b5af),
                                 testing::Values(AMOADDD)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOXORD, ParserTest,
                         testing::Combine(
                                 testing::Values(0x20c6b5af),
                                 testing::Values(AMOXORD)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOORD, ParserTest,
                         testing::Combine(
                                 testing::Values(0x40c6b5af),
                                 testing::Values(AMOORD)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOMIND, ParserTest,
                         testing::Combine(
                                 testing::Values(0x80c6b5af),
                                 testing::Values(AMOMIND)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOMAXD, ParserTest,
                         testing::Combine(
                                 testing::Values(0xa0c6b5af),
                                 testing::Values(AMOMAXD)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOMINUD, ParserTest,
                         testing::Combine(
                                 testing::Values(0xc0c6b5af),
                                 testing::Values(AMOMINUD)
                         ));

INSTANTIATE_TEST_SUITE_P(AMOMAXUD, ParserTest,
                         testing::Combine(
                                 testing::Values(0xe0c6b5af),
                                 testing::Values(AMOMAXUD)
                         ));
