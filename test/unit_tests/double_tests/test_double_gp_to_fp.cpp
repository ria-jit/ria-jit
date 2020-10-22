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

typedef double (*t_i2dResFunc)(t_risc_reg_val);

inline t_risc_reg_val doubleToIntRep(double d) {
    return *(t_risc_reg_val *) ((double *) &d);
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
class GpToFpDoubleTest :
        public ::testing::TestWithParam<
                std::tuple<t_risc_mnem, uint64_t, t_i2dResFunc, bool,
                        bool>> {
protected:
    t_risc_mnem mnem{};
    t_risc_reg_val rs1StartValue{};
    t_i2dResFunc resFunc{};
    double expectedRd;
    bool rs1Mapped{};
    bool rdMapped{};

    t_risc_reg rs1 = x0;
    t_risc_reg rd = x0;

    t_risc_instr blockCache[1]{};
    static context_info *c_info;
    static register_info *r_info;

protected:
    GpToFpDoubleTest() {
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

context_info *GpToFpDoubleTest::c_info = nullptr;
register_info *GpToFpDoubleTest::r_info = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//This should ignore the "unknown" ide pragmas without error, but the diagnostic pragmas in GCC don't work on stuff only
// the preprocessor sees.
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-non-const-global-variables"
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST_P(GpToFpDoubleTest, AllDifferent) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rd, 0};
    blockCache[0].rounding_mode = DYN;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    set_value(rs1, rs1StartValue);

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_value(rs1));
    EXPECT_EQ(expectedRd, get_fpvalue(rd).d);
}

INSTANTIATE_TEST_SUITE_P(FMVDX,
                         GpToFpDoubleTest,
                         testing::Combine(
                                 testing::Values(FMVDX),
                                 testing::Values(doubleToIntRep(1), doubleToIntRep(-200), doubleToIntRep(12312.12)),
                                 testing::Values([](uint64_t rs1) {
                                     return *(double *) ((uint64_t *) &rs1);
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));


INSTANTIATE_TEST_SUITE_P(FCVTDW,
                         GpToFpDoubleTest,
                         testing::Combine(
                                 testing::Values(FCVTDW),
                                 testing::Values(-1, 20, 300, -1232, 0),
                                 testing::Values([](uint64_t rs1) {
                                     return (double) (int32_t) rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FCVTDWU,
                         GpToFpDoubleTest,
                         testing::Combine(
                                 testing::Values(FCVTDWU),
                                 testing::Values(-1, 20, 300, -1232, 0),
                                 testing::Values([](uint64_t rs1) {
                                     return (double) (uint32_t) rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FCVTDL,
                         GpToFpDoubleTest,
                         testing::Combine(
                                 testing::Values(FCVTDL),
                                 testing::Values(-1, 20, 300, -1232, 0),
                                 testing::Values([](uint64_t rs1) {
                                     return (double) (int64_t) rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FCVTDLU,
                         GpToFpDoubleTest,
                         testing::Combine(
                                 testing::Values(FCVTDLU),
                                 testing::Values(-1, 20, 300, -1232, 0),
                                 testing::Values([](uint64_t rs1) {
                                     return (double) rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

#pragma ide diagonstics pop
#pragma GCC diagnostic pop
