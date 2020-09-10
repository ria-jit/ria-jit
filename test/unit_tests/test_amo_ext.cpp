//
// Created by simon on 10.09.20.
//

#include <gtest/gtest.h>
#include <util/typedefs.h>
#include <main/context.h>
#include <gen/translate.h>
#include <runtime/register.h>

typedef t_risc_reg_val (*t_resFunc)(t_risc_reg_val, t_risc_reg_val);

/**
 * Parameterized using the following parameters:
 *
 * @param t_risc_mnem the mnemonic of the AMO Instruction.
 * @param bool whether this instruction operates only on wordSize memory (32bit) and not 64bit.
 * @param t_risc_reg_val the initial value for the memorySlot rs1 points to (if wordSize is true, the upper 32bit are
 *        masked away).
 * @param t_risc_reg_val the initial value for rs2.
 * @param t_res_func the functor/lambda that provides, given the old value in memory at rs1 and the value in rs2
 *        (masked to 32bits if wordSize is true), the expected new value for the memory at rs1.
 * @param bool whether rs1 should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 * @param bool whether rs2 should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 * @param bool whether rd should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 */
class AmoExtTest :
        public ::testing::TestWithParam<
                std::tuple<t_risc_mnem, bool, t_risc_reg_val, t_risc_reg_val, t_resFunc, bool, bool, bool>> {
protected:
    t_risc_mnem mnem{};
    bool wordSize{};
    t_risc_reg_val rs1StartValue{};
    t_risc_reg_val rs2StartValue{};
    t_resFunc resFunc{};
    t_risc_reg_val expectedRd;
    bool rs1Mapped{};
    bool rs2Mapped{};
    bool rdMapped{};

    t_risc_reg_val rs1EndValue;
    t_risc_reg rs1 = x0;
    t_risc_reg rs2 = x0;
    t_risc_reg rd = x0;

    t_risc_instr blockCache[1]{};
    static context_info *c_info;
    static register_info *r_info;

protected:
    AmoExtTest() {
        std::tie(mnem, wordSize, rs1StartValue, rs2StartValue, resFunc, rs1Mapped, rs2Mapped, rdMapped) =
                GetParam();

        if (wordSize) {
            rs1StartValue &= 0xffFFffFF;
            rs1EndValue = resFunc(rs1StartValue, rs2StartValue & 0xffFFffFF);
            rs1EndValue &= 0xffFFffFF;
            expectedRd = ((int64_t) (rs1StartValue << 32)) >> 32;
        } else {
            rs1EndValue = resFunc(rs1StartValue, rs2StartValue);
            expectedRd = rs1StartValue;
        }

    }

public:

    static void SetUpTestSuite() {
        if (c_info == nullptr) {
            c_info = init_map_context();
            r_info = c_info->r_info;
        }
    }

protected:

    void SetUp() override {
        for (int i = 1; i < N_REG; ++i) {
            bool curMap = r_info->mapped[i];
            if (rs1 == x0 && curMap == rs1Mapped) {
                rs1 = static_cast<t_risc_reg>(i);
            } else if (rs2 == x0 && curMap == rs2Mapped) {
                rs2 = static_cast<t_risc_reg>(i);
            } else if (rd == x0 && curMap == rdMapped) {
                rd = static_cast<t_risc_reg>(i);
            }
        }

    }
};

context_info *AmoExtTest::c_info = nullptr;
register_info *AmoExtTest::r_info = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//This should ignore the "unknown" ide pragmas without error, but the diagnostic pragmas in GCC don't work on stuff only
// the preprocessor sees.
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-non-const-global-variables"
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST_P(AmoExtTest, AllDifferent) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rd, 0};

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    volatile uint64_t memoryLocation = rs1StartValue;
    set_value(rs1, (t_risc_reg_val) &memoryLocation);
    set_value(rs2, rs2StartValue);

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1EndValue, memoryLocation);
    EXPECT_EQ((t_risc_reg_val) &memoryLocation, get_value(rs1));
    EXPECT_EQ(rs2StartValue, get_value(rs2));
    EXPECT_EQ(expectedRd, get_value(rd));

    ///rd = x0 means result (=old value) ignored, but memory should still update accordingly.
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, x0, 0};

    loc = translate_block_instructions(blockCache, 1, c_info);

    memoryLocation = rs1StartValue;
    set_value(rs1, (t_risc_reg_val) &memoryLocation);
    set_value(rs2, rs2StartValue);

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1EndValue, memoryLocation);
    EXPECT_EQ((t_risc_reg_val) &memoryLocation, get_value(rs1));
    EXPECT_EQ(rs2StartValue, get_value(rs2));

    ///rs2 = x0
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, x0, rd, 0};

    loc = translate_block_instructions(blockCache, 1, c_info);

    memoryLocation = rs1StartValue;
    set_value(rs1, (t_risc_reg_val) &memoryLocation);

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(resFunc(rs1StartValue, 0), memoryLocation);
    EXPECT_EQ((t_risc_reg_val) &memoryLocation, get_value(rs1));
    EXPECT_EQ(expectedRd, get_value(rd));
}

TEST_P(AmoExtTest, Rs2RdSame) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rs2, 0};

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    volatile uint64_t memoryLocation = rs1StartValue;
    set_value(rs1, (t_risc_reg_val) &memoryLocation);
    set_value(rs2, rs2StartValue);

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1EndValue, memoryLocation);
    EXPECT_EQ((t_risc_reg_val) &memoryLocation, get_value(rs1));
    EXPECT_EQ(expectedRd, get_value(rs2));

    ///rs2 = rd = x0 means result (=old value) ignored, but memory should still update accordingly.
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, x0, x0, 0};

    loc = translate_block_instructions(blockCache, 1, c_info);

    memoryLocation = rs1StartValue;
    set_value(rs1, (t_risc_reg_val) &memoryLocation);

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(resFunc(rs1StartValue, 0), memoryLocation);
    EXPECT_EQ((t_risc_reg_val) &memoryLocation, get_value(rs1));
}

TEST_P(AmoExtTest, Rs1RdSame) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rs1, 0};

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    volatile uint64_t memoryLocation = rs1StartValue;

    set_value(rs1, (t_risc_reg_val) &memoryLocation);
    set_value(rs2, rs2StartValue);
    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1EndValue, memoryLocation);
    EXPECT_EQ(rs2StartValue, get_value(rs2));
    EXPECT_EQ(expectedRd, get_value(rs1));

    ///rs2 = x0
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, x0, rs1, 0};

    loc = translate_block_instructions(blockCache, 1, c_info);

    memoryLocation = rs1StartValue;

    set_value(rs1, (t_risc_reg_val) &memoryLocation);
    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(resFunc(rs1StartValue, 0), memoryLocation);
    EXPECT_EQ(expectedRd, get_value(rs1));
}

TEST_P(AmoExtTest, Rs1Rs2Same) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rd, 0};

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    volatile uint64_t memoryLocation = rs1StartValue;
    set_value(rs1, (t_risc_reg_val) &memoryLocation);
    execute_in_guest_context(c_info, loc);

    t_risc_reg_val expectedMem;
    if (!wordSize) {
        expectedMem = (t_risc_reg_val) &memoryLocation;
    } else {
        expectedMem = (t_risc_reg_val) &memoryLocation & 0xffFFffFF;
    }
    EXPECT_EQ(resFunc(rs1StartValue, expectedMem), memoryLocation);
    EXPECT_EQ((t_risc_reg_val) &memoryLocation, get_value(rs1));
    EXPECT_EQ(expectedRd, get_value(rd));

    ///rd = x0 means result (=old value) ignored, but memory should still update accordingly.
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1, x0, 0};

    loc = translate_block_instructions(blockCache, 1, c_info);

    memoryLocation = rs1StartValue;
    set_value(rs1, (t_risc_reg_val) &memoryLocation);
    execute_in_guest_context(c_info, loc);

    if (!wordSize) {
        expectedMem = (t_risc_reg_val) &memoryLocation;
    } else {
        expectedMem = (t_risc_reg_val) &memoryLocation & 0xffFFffFF;
    }
    EXPECT_EQ(resFunc(rs1StartValue, expectedMem), memoryLocation);
    EXPECT_EQ((t_risc_reg_val) &memoryLocation, get_value(rs1));
}

TEST_P(AmoExtTest, AllSame) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rs1, 0};

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    volatile uint64_t memoryLocation = rs1StartValue;
    set_value(rs1, (t_risc_reg_val) &memoryLocation);

    execute_in_guest_context(c_info, loc);

    t_risc_reg_val expectedMem;
    if (!wordSize) {
        expectedMem = (t_risc_reg_val) &memoryLocation;
    } else {
        expectedMem = (t_risc_reg_val) &memoryLocation & 0xffFFffFF;
    }
    EXPECT_EQ(resFunc(rs1StartValue, expectedMem), memoryLocation);
    EXPECT_EQ(expectedRd, get_value(rs1));
}

INSTANTIATE_TEST_SUITE_P(AMOSWAPW,
                         AmoExtTest,
                         testing::Combine(
                                 testing::Values(AMOSWAPW),
                                 testing::Values(true),
                                 testing::Values(1, -1),
                                 testing::Values(2, UINT32_MAX + 3ul),
                                 testing::Values([](t_risc_reg_val memAtRs1 __attribute__((__unused__)),
                                                    t_risc_reg_val rs2) {return rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AMOSWAPD,
                         AmoExtTest,
                         testing::Combine(
                                 testing::Values(AMOSWAPD),
                                 testing::Values(false),
                                 testing::Values(1lu << 32),
                                 testing::Values(1lu << 33),
                                 testing::Values([](t_risc_reg_val memAtRs1 __attribute__((__unused__)),
                                                    t_risc_reg_val rs2) {return rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AMOADDW,
                         AmoExtTest,
                         testing::Combine(
                                 testing::Values(AMOADDW),
                                 testing::Values(true),
                                 testing::Values(1, -1),
                                 testing::Values(2, UINT32_MAX + 3ul),
                                 testing::Values(
                                         [](t_risc_reg_val memAtRs1, t_risc_reg_val rs2) {
                                             return (memAtRs1 + rs2) & 0xffFFffFF;
                                         }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AMOADDD,
                         AmoExtTest,
                         testing::Combine(
                                 testing::Values(AMOADDD),
                                 testing::Values(false),
                                 testing::Values(1lu << 32, -1),
                                 testing::Values(1lu << 33),
                                 testing::Values(
                                         [](t_risc_reg_val memAtRs1, t_risc_reg_val rs2) {return memAtRs1 + rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AMOORW,
                         AmoExtTest,
                         testing::Combine(
                                 testing::Values(AMOORW),
                                 testing::Values(true),
                                 testing::Values(1, 1 << 31),
                                 testing::Values(2, UINT32_MAX + 3ul),
                                 testing::Values(
                                         [](t_risc_reg_val memAtRs1, t_risc_reg_val rs2) {return memAtRs1 | rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AMOORD,
                         AmoExtTest,
                         testing::Combine(
                                 testing::Values(AMOORD),
                                 testing::Values(false),
                                 testing::Values(1lu << 32),
                                 testing::Values(1lu << 33),
                                 testing::Values(
                                         [](t_risc_reg_val memAtRs1, t_risc_reg_val rs2) {return memAtRs1 | rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AMOXORW,
                         AmoExtTest,
                         testing::Combine(
                                 testing::Values(AMOXORW),
                                 testing::Values(true),
                                 testing::Values(3, -1),
                                 testing::Values(2, UINT32_MAX + 3lu),
                                 testing::Values(
                                         [](t_risc_reg_val memAtRs1, t_risc_reg_val rs2) {return memAtRs1 ^ rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AMOXORD,
                         AmoExtTest,
                         testing::Combine(
                                 testing::Values(AMOXORD),
                                 testing::Values(false),
                                 testing::Values(3lu << 32),
                                 testing::Values(1lu << 33),
                                 testing::Values(
                                         [](t_risc_reg_val memAtRs1, t_risc_reg_val rs2) {return memAtRs1 ^ rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AMOANDW,
                         AmoExtTest,
                         testing::Combine(
                                 testing::Values(AMOANDW),
                                 testing::Values(true),
                                 testing::Values(3, -1),
                                 testing::Values(2, UINT32_MAX + 3lu),
                                 testing::Values(
                                         [](t_risc_reg_val memAtRs1, t_risc_reg_val rs2) {return memAtRs1 & rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AMOANDD,
                         AmoExtTest,
                         testing::Combine(
                                 testing::Values(AMOANDD),
                                 testing::Values(false),
                                 testing::Values(3lu << 32),
                                 testing::Values(1lu << 33),
                                 testing::Values(
                                         [](t_risc_reg_val memAtRs1, t_risc_reg_val rs2) {return memAtRs1 & rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));
#pragma ide diagonstics pop
#pragma GCC diagnostic pop
