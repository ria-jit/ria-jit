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

typedef uint32_t (*t_arithmResFunc)(float, float);

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
class ConditionalFloatTest :
        public ::testing::TestWithParam<
                std::tuple<t_risc_mnem, float, float, t_arithmResFunc, bool, bool,
                        bool>> {
protected:
    t_risc_mnem mnem{};
    float rs1StartValue{};
    float rs2StartValue{};
    t_arithmResFunc resFunc{};
    uint32_t expectedRd;
    bool rs1Mapped{};
    bool rs2Mapped{};
    bool rdMapped{};

    t_risc_reg rs1 = x0;
    t_risc_reg rs2 = x0;
    t_risc_reg rd = x0;

    t_risc_instr blockCache[1]{};
    static context_info *c_info;
    static register_info *r_info;

protected:
    ConditionalFloatTest() {
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
        for (int i = 1; i < N_REG; ++i) {
            bool curMap = r_info->fp_mapped[i];
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

context_info *ConditionalFloatTest::c_info = nullptr;
register_info *ConditionalFloatTest::r_info = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//This should ignore the "unknown" ide pragmas without error, but the diagnostic pragmas in GCC don't work on stuff only
// the preprocessor sees.
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-non-const-global-variables"
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST_P(ConditionalFloatTest, AllDifferent) {
    blockCache[0] =
            t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rd, 0};

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, true);

    set_fpvalue(rs1, {rs1StartValue});
    set_fpvalue(rs2, {rs2StartValue});

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).f);
    EXPECT_EQ(rs2StartValue, get_fpvalue(rs2).f);
    EXPECT_EQ(expectedRd, get_value(rd));
}

TEST_P(ConditionalFloatTest, Rs1Rs2Same) {
        blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rd, 0};

        t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, true);

        set_fpvalue(rs1, {rs1StartValue});

        execute_in_guest_context(c_info, loc);

        EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).f);
        EXPECT_EQ(resFunc(rs1StartValue, rs1StartValue), get_value(rd));
}

INSTANTIATE_TEST_SUITE_P(FEQS,
                         ConditionalFloatTest,
                         testing::Combine(
                                 testing::Values(FEQS),
                                 testing::Values(1, 18.5),
                                 testing::Values(2, 9.5, 18.5),
                                 testing::Values([](float rs1,float rs2) {
                                     return (uint32_t) (rs1 == rs2 ? 1 : 0);
                                 }),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false)));


INSTANTIATE_TEST_SUITE_P(FLTS,
                         ConditionalFloatTest,
                         testing::Combine(
                                 testing::Values(FLTS),
                                 testing::Values(1, 18.5),
                                 testing::Values(2, 9.5, 18.5),
                                 testing::Values([](float rs1,float rs2) {
                                     return (uint32_t) (rs1 < rs2 ? 1 : 0);
                                 }),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FLES,
                         ConditionalFloatTest,
                         testing::Combine(
                                 testing::Values(FLES),
                                 testing::Values(1, 18.5),
                                 testing::Values(2, 9.5, 18.5),
                                 testing::Values([](float rs1,float rs2) {
                                     return (uint32_t) (rs1 <= rs2 ? 1 : 0);
                                 }),
                                 testing::Values(false),
                                 testing::Values(false),
                                 testing::Values(false)));

#pragma ide diagonstics pop
#pragma GCC diagnostic pop
