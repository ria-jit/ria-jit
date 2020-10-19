//
// Created by simon on 17.09.20.
//

#include <gtest/gtest.h>
#include <util/typedefs.h>
#include <main/context.h>
#include <gen/translate.h>
#include <runtime/register.h>
#include "math.h"

typedef float (*t_farithmResFunc)(float, float, float);

/**
 * Parameterized using the following parameters:
 *
 * @param t_risc_mnem the mnemonic of the Arithm Instruction.
 * @param t_risc_reg_val the initial value for rs1
 * @param t_risc_reg_val the initial value for rs2
 * @param t_res_func the functor/lambda that provides, given the addr of the instruction, the value of rs1 and the value
 *        in rs2
 * @param bool whether rs1 should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 * @param bool whether rs2 should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 * @param bool whether rd should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 */
class FusedArithmFloatTest :
        public ::testing::TestWithParam<
                std::tuple<t_risc_mnem, float, float, float, t_farithmResFunc, bool, bool, bool,
                        bool>> {
protected:
    t_risc_mnem mnem{};
    float rs1StartValue{};
    float rs2StartValue{};
    float rs3StartValue{};
    t_farithmResFunc resFunc{};
    float expectedRd;
    bool rs1Mapped{};
    bool rs2Mapped{};
    bool rs3Mapped{};
    bool rdMapped{};

    t_risc_reg rs1 = x0;
    t_risc_reg rs2 = x0;
    t_risc_reg rs3 = x0;
    t_risc_reg rd = x0;

    t_risc_instr blockCache[1]{};
    static context_info *c_info;
    static register_info *r_info;

protected:
    FusedArithmFloatTest() {
        std::tie(mnem, rs1StartValue, rs2StartValue, rs3StartValue, resFunc, rs1Mapped, rs2Mapped, rs3Mapped,
                 rdMapped) =
                GetParam();

        expectedRd = resFunc(rs1StartValue, rs2StartValue, rs3StartValue);
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
            } else if (rs3 == x0 && curMap == rs3Mapped) {
                rs3 = static_cast<t_risc_reg>(i);
            } else if (rd == x0 && curMap == rdMapped) {
                rd = static_cast<t_risc_reg>(i);
            }
        }

    }
};

context_info *FusedArithmFloatTest::c_info = nullptr;
register_info *FusedArithmFloatTest::r_info = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//This should ignore the "unknown" ide pragmas without error, but the diagnostic pragmas in GCC don't work on stuff only
// the preprocessor sees.
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-non-const-global-variables"
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST_P(FusedArithmFloatTest, AllDifferent) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rd, 0};
    blockCache[0].reg_src_3 = rs3;
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    set_fpvalue(rs1, get_fVal(rs1StartValue));
    set_fpvalue(rs2, get_fVal(rs2StartValue));
    set_fpvalue(rs3, get_fVal(rs3StartValue));

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).f);
    EXPECT_EQ(rs2StartValue, get_fpvalue(rs2).f);
    EXPECT_EQ(rs3StartValue, get_fpvalue(rs3).f);
    EXPECT_EQ(expectedRd, get_fpvalue(rd).f);
}

TEST_P(FusedArithmFloatTest, Rs1RdSame) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rs1, 0};
    blockCache[0].reg_src_3 = rs3;
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    set_fpvalue(rs1, get_fVal(rs1StartValue));
    set_fpvalue(rs2, get_fVal(rs2StartValue));
    set_fpvalue(rs3, get_fVal(rs3StartValue));

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(expectedRd, get_fpvalue(rs1).f);
    EXPECT_EQ(rs3StartValue, get_fpvalue(rs3).f);
    EXPECT_EQ(rs2StartValue, get_fpvalue(rs2).f);
}

TEST_P(FusedArithmFloatTest, Rs2RdSame) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rs2, 0};
    blockCache[0].reg_src_3 = rs3;
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    set_fpvalue(rs1, get_fVal(rs1StartValue));
    set_fpvalue(rs2, get_fVal(rs2StartValue));
    set_fpvalue(rs3, get_fVal(rs3StartValue));

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).f);
    EXPECT_EQ(rs3StartValue, get_fpvalue(rs3).f);
    EXPECT_EQ(expectedRd, get_fpvalue(rs2).f);
}

INSTANTIATE_TEST_SUITE_P(FMADDS,
                         FusedArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FMADDS),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](float rs1, float rs2, float rs3) {
                                     return rs1 * rs2 + rs3;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FNMADDS,
                         FusedArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FNMADDS),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](float rs1, float rs2, float rs3) {
                                     return -rs1 * rs2 - rs3;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FMSUBS,
                         FusedArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FMSUBS),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](float rs1, float rs2, float rs3) {
                                     return rs1 * rs2 - rs3;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FNMSUBS,
                         FusedArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FNMSUBS),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values(4, 8, -10),
                                 testing::Values([](float rs1, float rs2, float rs3) {
                                     return -rs1 * rs2 + rs3;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false)));