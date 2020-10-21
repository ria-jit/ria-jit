//
// Created by simon on 17.09.20.
//

#include <gtest/gtest.h>
#include <util/typedefs.h>
#include <main/context.h>
#include <gen/translate.h>
#include <runtime/register.h>
#include "math.h"

#define attr_unused __attribute__((__unused__))

typedef double (*t_d2iResFunc)(double, double);

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
class ArithmDoubleTest :
        public ::testing::TestWithParam<
                std::tuple<t_risc_mnem, double, double, t_d2iResFunc, bool, bool,
                        bool>> {
protected:
    t_risc_mnem mnem{};
    double rs1StartValue{};
    double rs2StartValue{};
    t_d2iResFunc resFunc{};
    double expectedRd;
    bool rs1Mapped{};
    bool rs2Mapped{};
    bool rdMapped{};

    t_risc_reg rs1 = static_cast<t_risc_reg>(fInvalid);
    t_risc_reg rs2 = static_cast<t_risc_reg>(fInvalid);
    t_risc_reg rd = static_cast<t_risc_reg>(fInvalid);

    t_risc_instr blockCache[1]{};
    static context_info *c_info;
    static register_info *r_info;

protected:
    ArithmDoubleTest() {
        std::tie(mnem, rs1StartValue, rs2StartValue, resFunc, rs1Mapped, rs2Mapped,
                 rdMapped) =
                GetParam();

        expectedRd = resFunc(rs1StartValue, rs2StartValue);
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
        for (int i = 0; i < N_FP_REG; ++i) {
            bool curMap = r_info->fp_mapped[i];
            if (rs1 == fInvalid && curMap == rs1Mapped) {
                rs1 = static_cast<t_risc_reg>(i);
            } else if (rs2 == fInvalid && curMap == rs2Mapped) {
                rs2 = static_cast<t_risc_reg>(i);
            } else if (rd == fInvalid && curMap == rdMapped) {
                rd = static_cast<t_risc_reg>(i);
            }
        }
    }
};

context_info *ArithmDoubleTest::c_info = nullptr;
register_info *ArithmDoubleTest::r_info = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//This should ignore the "unknown" ide pragmas without error, but the diagnostic pragmas in GCC don't work on stuff only
// the preprocessor sees.
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-non-const-global-variables"
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST_P(ArithmDoubleTest, AllDifferent) {
    blockCache[0] =
            t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rd, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, false);

    set_fpvalue(rs1, get_dVal(rs1StartValue));
    set_fpvalue(rs2, get_dVal(rs2StartValue));

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).d);
    EXPECT_EQ(rs2StartValue, get_fpvalue(rs2).d);
    EXPECT_EQ(expectedRd, get_fpvalue(rd).d);
}

TEST_P(ArithmDoubleTest, Rs1RdSame) {
    blockCache[0] =
            t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rs1, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, false);

    set_fpvalue(rs1, get_dVal(rs1StartValue));
    set_fpvalue(rs2, get_dVal(rs2StartValue));

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(expectedRd, get_fpvalue(rs1).d);
    EXPECT_EQ(rs2StartValue, get_fpvalue(rs2).d);
}

TEST_P(ArithmDoubleTest, Rs2RdSame) {
    blockCache[0] =
            t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rs2, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, false);

    set_fpvalue(rs1, get_dVal(rs1StartValue));
    set_fpvalue(rs2, get_dVal(rs2StartValue));

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).d);
    EXPECT_EQ(expectedRd, get_fpvalue(rs2).d);
}

TEST_P(ArithmDoubleTest, Rs1Rs2Same) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rd, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, false);

    set_fpvalue(rs1, get_dVal(rs1StartValue));

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).d);
    EXPECT_EQ(resFunc(rs1StartValue, rs1StartValue), get_fpvalue(rd).d);
}

TEST_P(ArithmDoubleTest, AllSame) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rs1, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, false);

    set_fpvalue(rs1, get_dVal(rs1StartValue));

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(resFunc(rs1StartValue, rs1StartValue), get_fpvalue(rs1).d);
}

INSTANTIATE_TEST_SUITE_P(FADDD,
                         ArithmDoubleTest,
                         testing::Combine(
                                 testing::Values(FADDD),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](double rs1, double rs2) {
                                     return rs1 + rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FSUBD,
                         ArithmDoubleTest,
                         testing::Combine(
                                 testing::Values(FSUBD),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](double rs1, double rs2) {
                                     return rs1 - rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FMULD,
                         ArithmDoubleTest,
                         testing::Combine(
                                 testing::Values(FMULD),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](double rs1, double rs2) {
                                     return rs1 * rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FDIVD,
                         ArithmDoubleTest,
                         testing::Combine(
                                 testing::Values(FDIVD),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](double rs1, double rs2) {
                                     return rs1 / rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FSQRTD,
                         ArithmDoubleTest,
                         testing::Combine(
                                 testing::Values(FSQRTD),
                                 testing::Values(1, 5, 64, 2601),
                                 testing::Values(0),
                                 testing::Values([](double rs1, double rs2 attr_unused) {
                                     return sqrt(rs1);
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));


INSTANTIATE_TEST_SUITE_P(FSGNJD,
                         ArithmDoubleTest,
                         testing::Combine(
                                 testing::Values(FSGNJD),
                                 testing::Values(1, -1),
                                 testing::Values(-5, 5),
                                 testing::Values([](double rs1, double rs2) {
                                     //get signs
                                     if ((rs2 >= 0 && rs1 < 0) || (rs2 < 0 && rs1 >= 0)) {
                                         return -rs1;
                                     }
                                     return rs1;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));


INSTANTIATE_TEST_SUITE_P(FSGNJND,
                         ArithmDoubleTest,
                         testing::Combine(
                                 testing::Values(FSGNJND),
                                 testing::Values(1, -1),
                                 testing::Values(-5, 5),
                                 testing::Values([](double rs1, double rs2) {
                                     //get signs
                                     if ((rs2 >= 0 && rs1 >= 0) || (rs2 < 0 && rs1 < 0)) {
                                         return -rs1;
                                     }
                                     return rs1;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FSGNJXD,
                         ArithmDoubleTest,
                         testing::Combine(
                                 testing::Values(FSGNJXD),
                                 testing::Values(1, -1),
                                 testing::Values(-5, 5),
                                 testing::Values([](double rs1, double rs2) {
                                     //get signs
                                     if ((rs2 < 0 && rs1 >= 0) || (rs2 < 0 && rs1 < 0)) {
                                         return -rs1;
                                     }
                                     return rs1;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FMIND,
                         ArithmDoubleTest,
                         testing::Combine(
                                 testing::Values(FMIND),
                                 testing::Values(1, -1),
                                 testing::Values(-5, 5),
                                 testing::Values([](double rs1, double rs2) {
                                     //return smaller
                                     return rs1 < rs2 ? rs1 : rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FMAXD,
                         ArithmDoubleTest,
                         testing::Combine(
                                 testing::Values(FMAXD),
                                 testing::Values(1, -1),
                                 testing::Values(-5, 5),
                                 testing::Values([](double rs1, double rs2) {
                                     //return bigger
                                     return rs1 > rs2 ? rs1 : rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));


#pragma ide diagonstics pop
#pragma GCC diagnostic pop
