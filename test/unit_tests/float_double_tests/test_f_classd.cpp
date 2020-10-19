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
    class FClassDTest : public ::testing::Test {
    protected:
        t_risc_reg src = x1;
        t_risc_reg rd = x1;
        const size_t test_size = 30;
        t_risc_instr blockCache[1]{};
        context_info *c_info;
        t_cache_loc loc;

        FClassDTest() {

        }

        virtual ~FClassDTest() {

        }

        virtual void SetUp() {
            blockCache[0] =
                    t_risc_instr{0, FCLASSD, static_cast<t_risc_optype>(0), src, src, rd, 0};

            c_info = init_map_context();
            loc = translate_block_instructions(blockCache, 1, c_info);
        }
    };

    TEST_F(FClassDTest, NegativeInfinity) {
        uint64_t testVal = 0xfff0000000000000;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 0), get_value(rd));
    }

    TEST_F(FClassDTest, NegativeNormal) {
        double testVal = -10000.0;
        set_fpvalue(src, get_dVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).d);
        EXPECT_EQ((1 << 1), get_value(rd));
    }

    TEST_F(FClassDTest, NegativeSubNormal) {
        uint64_t testVal = 0x80000000000000ff;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 2), get_value(rd));
    }


    TEST_F(FClassDTest, NegativeZero) {
        uint64_t testVal = 0x8000000000000000;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 3), get_value(rd));
    }

    TEST_F(FClassDTest, PositiveZero) {
        uint64_t testVal = 0x0;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 4), get_value(rd));
    }


    TEST_F(FClassDTest, PositiveSubNormal) {
        uint64_t testVal = 0x00000000000000ff;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 5), get_value(rd));
    }


    TEST_F(FClassDTest, PositiveNormal) {
        double testVal = 100.0;
        set_fpvalue(src, get_dVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).d);
        EXPECT_EQ((1 << 6), get_value(rd));
    }

    TEST_F(FClassDTest, PositiveInfinity) {
        uint64_t testVal = 0x7ff0000000000000;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 7), get_value(rd));
    }

    TEST_F(FClassDTest, SignalingNAN) {
        uint64_t testVal = 0x7ff0000000000001;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 8), get_value(rd));
    }

    TEST_F(FClassDTest, SilentNAN) {
        uint64_t testVal = 0x7ff8000000000000;
        set_fpvalue(src, get_iVal(testVal));
        execute_in_guest_context(c_info, loc);
        EXPECT_EQ(testVal, get_fpvalue(src).i);
        EXPECT_EQ((1 << 9), get_value(rd));
    }


}