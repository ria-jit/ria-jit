//
// Created by noah on 17.10.20.
//

#include <gtest/gtest.h>
#include <util/typedefs.h>
#include <main/context.h>
#include <gen/translate.h>
#include <runtime/register.h>

/**
 * Checks the load and store translation by copying data from an array into the registers and back into another array
 */
namespace {
    class FClassSTest : public ::testing::Test {
    protected:
        t_risc_reg src = x1;
        t_risc_reg rd = x1;
        const size_t test_size = 30;
        t_risc_instr blockCache[1]{};
        context_info *c_info;
        t_cache_loc loc;

        FClassSTest() {

        }

        virtual ~FClassSTest() {

        }

        virtual void SetUp() {
            blockCache[0] =
                    t_risc_instr{0, FCLASSS, static_cast<t_risc_optype>(0), src, src, rd, 0};

            c_info = init_map_context(true);
            loc = translate_block_instructions(blockCache, 1, c_info, true);
        }
    };

    TEST_F(FClassSTest, NegativeInfinity) {
        uint32_t testVal = 0xff800000;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 0), get_value(rd));
    }

    TEST_F(FClassSTest, NegativeNormal) {
        float testVal = -100.0;
        set_fpvalue(src, get_fVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).f);
        EXPECT_EQ((1 << 1), get_value(rd));
    }

    TEST_F(FClassSTest, NegativeSubNormal) {
        uint32_t testVal = 0x800000ff;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 2), get_value(rd));
    }


    TEST_F(FClassSTest, NegativeZero) {
        uint32_t testVal = 0x80000000;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 3), get_value(rd));
    }

    TEST_F(FClassSTest, PositiveZero) {
        uint32_t testVal = 0x0;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 4), get_value(rd));
    }


    TEST_F(FClassSTest, PositiveSubNormal) {
        uint32_t testVal = 0x000000ff;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 5), get_value(rd));
    }


    TEST_F(FClassSTest, PositiveNormal) {
        float testVal = 100.0;
        set_fpvalue(src, get_fVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).f);
        EXPECT_EQ((1 << 6), get_value(rd));
    }

    TEST_F(FClassSTest, PositiveInfinity) {
        uint32_t testVal = 0x7f800000;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 7), get_value(rd));
    }

    TEST_F(FClassSTest, SignalingNAN) {
        uint32_t testVal = 0x7f800001;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 8), get_value(rd));
    }

    TEST_F(FClassSTest, SilentNAN) {
        uint32_t testVal = 0x7fc00000;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 9), get_value(rd));
    }


}