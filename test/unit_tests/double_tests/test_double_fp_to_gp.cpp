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

typedef uint64_t (*t_d2iResFunc)(double);

/**
 * Parameterized using the following parameters:
 *
 * @param t_risc_mnem the mnemonic of the Arithm Instruction.
 * @param t_risc_reg_val the initial value for rs1
 * @param t_arithmResFunc the functor/lambda that provides, given the addr of the instruction, the value of rs1 and the value
 *        in rs2
 * @param bool whether rs1 should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 * @param bool whether rd should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 */
class FpToGpDoubleTest :
        public ::testing::TestWithParam<
                std::tuple<t_risc_mnem, double, t_d2iResFunc, bool,
                        bool>> {
protected:
    t_risc_mnem mnem{};
    double rs1StartValue{};
    t_d2iResFunc resFunc{};
    uint64_t expectedRd;
    bool rs1Mapped{};
    bool rdMapped{};

    t_risc_reg rs1 = x0;
    t_risc_reg rd = x0;

    t_risc_instr blockCache[1]{};
    static context_info *c_info;
    static register_info *r_info;

protected:
    FpToGpDoubleTest() {
        std::tie(mnem, rs1StartValue, resFunc, rs1Mapped,
                 rdMapped) =
                GetParam();

        expectedRd = resFunc(rs1StartValue);
    }

public:

    static void SetUpTestSuite() {
        if (c_info == nullptr) {
            c_info = init_map_context(true);
            r_info = c_info->r_info;
        }
    }

protected:

    void SetUp() override {
        for (int i = 1; i < N_REG; ++i) {
            bool curMap = r_info->fp_mapped[i];
            if (rs1 == x0 && curMap == rs1Mapped) {
                rs1 = static_cast<t_risc_reg>(i);
            } else if (rd == x0 && curMap == rdMapped) {
                rd = static_cast<t_risc_reg>(i);
            }
        }

    }
};

context_info *FpToGpDoubleTest::c_info = nullptr;
register_info *FpToGpDoubleTest::r_info = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//This should ignore the "unknown" ide pragmas without error, but the diagnostic pragmas in GCC don't work on stuff only
// the preprocessor sees.
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-non-const-global-variables"
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST_P(FpToGpDoubleTest, AllDifferent) {
    blockCache[0] =            t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1,rd, 0};
    blockCache[0].rounding_mode = RTZ;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    set_fpvalue(rs1, get_dVal(rs1StartValue));

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).d);
    EXPECT_EQ(expectedRd, get_value(rd));
}

INSTANTIATE_TEST_SUITE_P(FMVXD,
                         FpToGpDoubleTest,
                         testing::Combine(
                                 testing::Values(FMVXD),
                                 testing::Values(1, 20.123, 300.4,-1232.123),
                                 testing::Values([](double rs1) {
                                     return *(uint64_t *)((double*)&rs1);
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));



INSTANTIATE_TEST_SUITE_P(FCVTWD,
                         FpToGpDoubleTest,
                         testing::Combine(
                                 testing::Values(FCVTWD),
                                 testing::Values(1, 20.123, 300.4,-1232.123),
                                 testing::Values([](double rs1) {
                                     return (uint64_t )(int32_t)rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FCVTWUD,
                         FpToGpDoubleTest,
                         testing::Combine(
                                 testing::Values(FCVTWUD),
                                 testing::Values(1, 20.123, 300.4,-1232.123,-0.111),
                                 testing::Values([](double rs1) {
                                     if(rs1 < 0) return (uint64_t) 0;
                                     return (uint64_t )(uint32_t)rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FCVTLD,
                         FpToGpDoubleTest,
                         testing::Combine(
                                 testing::Values(FCVTLD),
                                 testing::Values(1, 20.123, 300.4,-1232.123,-0.111),
                                 testing::Values([](double rs1) {
                                     return (uint64_t )(int64_t)rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));
INSTANTIATE_TEST_SUITE_P(FCVTLUD,
                         FpToGpDoubleTest,
                         testing::Combine(
                                 testing::Values(FCVTLUD),
                                 testing::Values(1, 20.123, 300.4,-1232.123,-0.111, 10000000000000000000.0,-10000000000000000.0),
                                 testing::Values([](double rs1) {
                                     if(rs1 < 0) return (uint64_t) 0;
                                     return (uint64_t)rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

#pragma ide diagonstics pop
#pragma GCC diagnostic pop
