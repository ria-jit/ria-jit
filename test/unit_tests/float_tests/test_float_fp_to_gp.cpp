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

typedef uint64_t (*t_f2iResFunc)(float);

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
class FpToGpFloatTest :
        public ::testing::TestWithParam<
                std::tuple<t_risc_mnem, float, t_f2iResFunc, bool,
                        bool>> {
protected:
    t_risc_mnem mnem{};
    float rs1StartValue{};
    t_f2iResFunc resFunc{};
    uint64_t expectedRd;
    bool rs1Mapped{};
    bool rdMapped{};

    t_risc_reg rs1 = x0;
    t_risc_reg rd = x0;

    t_risc_instr blockCache[1]{};
    static context_info *c_info;
    static register_info *r_info;

protected:
    FpToGpFloatTest() {
        std::tie(mnem, rs1StartValue, resFunc, rs1Mapped,
                 rdMapped) =
                GetParam();

        expectedRd = resFunc(rs1StartValue);
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
            bool curMap = r_info->gp_mapped[i];
            if (rs1 == x0 && curMap == rs1Mapped) {
                rs1 = static_cast<t_risc_reg>(i);
            } else if (rd == x0 && curMap == rdMapped) {
                rd = static_cast<t_risc_reg>(i);
            }
        }

    }
};

context_info *FpToGpFloatTest::c_info = nullptr;
register_info *FpToGpFloatTest::r_info = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//This should ignore the "unknown" ide pragmas without error, but the diagnostic pragmas in GCC don't work on stuff only
// the preprocessor sees.
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-non-const-global-variables"
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST_P(FpToGpFloatTest, AllDifferent) {
    blockCache[0] = t_risc_instr{0, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rd, 0};
    blockCache[0].rounding_mode = RTZ;

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info);

    set_fpvalue(rs1, get_fVal(rs1StartValue));

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_fpvalue(rs1).f);
    EXPECT_EQ(expectedRd, get_value(rd));
}

INSTANTIATE_TEST_SUITE_P(FMVXW,
                         FpToGpFloatTest,
                         testing::Combine(
                                 testing::Values(FMVXW),
                                 testing::Values(-1.f, 20.123f, 300.4f, -1232.123f),
                                 testing::Values([](float rs1) {
                                     int32_t float_bits = *(int32_t *) ((float *) &rs1);
                                     auto sign_extended_bits = (int64_t) float_bits;
                                     return (uint64_t) sign_extended_bits;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));


INSTANTIATE_TEST_SUITE_P(FCVTWS,
                         FpToGpFloatTest,
                         testing::Combine(
                                 testing::Values(FCVTWS),
                                 testing::Values(-1, 20.123, 300.4, 1232.123),
                                 testing::Values([](float rs1) {
                                     return (uint64_t) (int32_t) rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FCVTWUS,
                         FpToGpFloatTest,
                         testing::Combine(
                                 testing::Values(FCVTWUS),
                                 testing::Values(1, 20.123, 300.4, -1232.123, -10.111),
                                 testing::Values([](float rs1) {
                                     if(rs1 < 0) return (uint64_t) 0;
                                     return (uint64_t) (uint32_t) rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FCVTLS,
                         FpToGpFloatTest,
                         testing::Combine(
                                 testing::Values(FCVTLS),
                                 testing::Values(1, 20.123, 300.4, -1232.123, -0.111),
                                 testing::Values([](float rs1) {
                                     return (uint64_t) (int64_t) rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

INSTANTIATE_TEST_SUITE_P(FCVTLUS,
                         FpToGpFloatTest,
                         testing::Combine(
                                 testing::Values(FCVTLUS),
                                 testing::Values(1, 20.123, 300.4, -1232.123, -0.111, 10000000000000000000.0,
                                                 -10000000000000000.0),
                                 testing::Values([](float rs1) {
                                     if(rs1 < 0) return (uint64_t) 0;
                                     return (uint64_t) rs1;
                                 }),
                                 testing::Values(false),
                                 testing::Values(false)));

#pragma ide diagonstics pop
#pragma GCC diagnostic pop
