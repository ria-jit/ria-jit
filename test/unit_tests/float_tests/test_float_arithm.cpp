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

typedef float (*t_arithmResFunc)(float, float);

inline t_risc_fp_reg_val getFloat(float f) {
    return t_risc_fp_reg_val{f};
}

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
class ArithmFloatTest :
        public ::testing::TestWithParam<
                std::tuple<t_risc_mnem, float, float, t_arithmResFunc, bool, bool,
                        bool>> {
protected:
    t_risc_mnem mnem{};
    float rs1StartValue{};
    float rs2StartValue{};
    t_arithmResFunc resFunc{};
    float expectedRd;
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
    ArithmFloatTest() {
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

context_info *ArithmFloatTest::c_info = nullptr;
register_info *ArithmFloatTest::r_info = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//This should ignore the "unknown" ide pragmas without error, but the diagnostic pragmas in GCC don't work on stuff only
// the preprocessor sees.
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-non-const-global-variables"
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST_P(ArithmFloatTest, AllDifferent) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rd, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, true);

    set_fpvalue(rs1, {rs1StartValue});
    set_fpvalue(rs2, {rs2StartValue});

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).f);
    EXPECT_EQ(rs2StartValue, get_fpvalue(rs2).f);
    EXPECT_EQ(expectedRd, get_fpvalue(rd).f);
}

TEST_P(ArithmFloatTest, Rs1RdSame) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rs1, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, true);

    set_fpvalue(rs1, {rs1StartValue});
    set_fpvalue(rs2, {rs2StartValue});

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(expectedRd, get_fpvalue(rs1).f);
    EXPECT_EQ(rs2StartValue, get_fpvalue(rs2).f);
}

TEST_P(ArithmFloatTest, Rs2RdSame) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rs2, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, true);

    set_fpvalue(rs1, {rs1StartValue});
    set_fpvalue(rs2, {rs2StartValue});

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).f);
    EXPECT_EQ(expectedRd, get_fpvalue(rs2).f);
}

TEST_P(ArithmFloatTest, Rs1Rs2Same) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rd, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, true);

    set_fpvalue(rs1, {rs1StartValue});

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).f);
    EXPECT_EQ(resFunc(rs1StartValue, rs1StartValue), get_fpvalue(rd).f);
}

TEST_P(ArithmFloatTest, AllSame) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rs1, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, true);

    set_fpvalue(rs1, {rs1StartValue});

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(resFunc(rs1StartValue, rs1StartValue), get_fpvalue(rs1).f);
}

INSTANTIATE_TEST_SUITE_P(FADDS,
                         ArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FADDS),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](float rs1, float rs2) {
                                     return rs1 + rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FSUBS,
                         ArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FSUBS),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](float rs1, float rs2) {
                                     return rs1 - rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FMULS,
                         ArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FMULS),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](float rs1, float rs2) {
                                     return rs1 * rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FDIVS,
                         ArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FDIVS),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 1),
                                 testing::Values([](float rs1, float rs2) {
                                     return rs1 / rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FSQRTS,
                         ArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FSQRTS),
                                 testing::Values(1, 5, 64, 2601),
                                 testing::Values(0),
                                 testing::Values([](float rs1, float rs2 attr_unused) {
                                     return sqrt(rs1);
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));


INSTANTIATE_TEST_SUITE_P(FSGNJS,
                         ArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FSGNJS),
                                 testing::Values(1, -1),
                                 testing::Values(-5, 5),
                                 testing::Values([](float rs1, float rs2) {
                                     //get signs
                                     if ((rs2 >= 0 && rs1 < 0) || (rs2 < 0 && rs1 >= 0)) {
                                         return -rs1;
                                     }
                                     return rs1;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));


INSTANTIATE_TEST_SUITE_P(FSGNJNS,
                         ArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FSGNJNS),
                                 testing::Values(1, -1),
                                 testing::Values(-5, 5),
                                 testing::Values([](float rs1, float rs2) {
                                     //get signs
                                     if ((rs2 >= 0 && rs1 >= 0) || (rs2 < 0 && rs1 < 0)) {
                                         return -rs1;
                                     }
                                     return rs1;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FSGNJXS,
                         ArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FSGNJXS),
                                 testing::Values(1, -1),
                                 testing::Values(-5, 5),
                                 testing::Values([](float rs1, float rs2) {
                                     //get signs
                                     if ((rs2 < 0 && rs1 >= 0) || (rs2 < 0 && rs1 < 0)) {
                                         return -rs1;
                                     }
                                     return rs1;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FMINS,
                         ArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FMINS),
                                 testing::Values(1, -1),
                                 testing::Values(-5, 5),
                                 testing::Values([](float rs1, float rs2) {
                                     //return smaller
                                     return rs1 < rs2 ? rs1 : rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(FMAXS,
                         ArithmFloatTest,
                         testing::Combine(
                                 testing::Values(FMAXS),
                                 testing::Values(1, -1),
                                 testing::Values(-5, 5),
                                 testing::Values([](float rs1, float rs2) {
                                     //return bigger
                                     return rs1 > rs2 ? rs1 : rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));


#pragma ide diagonstics pop
#pragma GCC diagnostic pop
