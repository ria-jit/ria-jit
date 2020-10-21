//
// Created by simon on 17.09.20.
//

#include <gtest/gtest.h>
#include <util/typedefs.h>
#include <main/context.h>
#include <gen/translate.h>
#include <runtime/register.h>

#define attr_unused __attribute__((__unused__))

typedef t_risc_reg_val (*t_resFunc)(t_risc_addr, t_risc_reg_val, t_risc_reg_val);

/**
 * Parameterized using the following parameters:
 *
 * @param t_risc_mnem the mnemonic of the Arithm Instruction.
 * @param bool true if the instruction is register-immediate, false for register-register.
 * @param t_risc_reg_val the initial value for rs1 or the addr for AUIPC.
 * @param t_risc_reg_val the initial value for rs2 or the immediate (needs to be sign-extended 32bit for 32bit instructions),
 *        if it is a register-immediate instruction.
 * @param t_res_func the functor/lambda that provides, given the addr of the instruction, the value of rs1 and the value
 *        in rs2 or the immediate value if if it is a register-immediate instruction, the expected new value for rd, #
 *        masking needs to be done inside the functions.
 * @param bool whether rs1 should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 * @param bool whether rs2 should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 * @param bool whether rd should be mapped. (if multiple registers are the same for a test the priority is rs1>rs2>rd.)
 */
class ArithmTest :
        public ::testing::TestWithParam<
                std::tuple<t_risc_mnem, bool, t_risc_reg_val, t_risc_reg_val, t_resFunc, bool, bool, bool>> {
protected:
    t_risc_mnem mnem{};
    bool immediateInstr{};
    t_risc_reg_val rs1StartValue{};
    t_risc_reg_val rs2StartValue{};
    t_resFunc resFunc{};
    t_risc_reg_val expectedRd;
    const t_risc_reg_val test_reg_values[31] =
            {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                    30};
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
    ArithmTest() {
        std::tie(mnem, immediateInstr, rs1StartValue, rs2StartValue, resFunc, rs1Mapped, rs2Mapped, rdMapped) =
                GetParam();
        expectedRd = resFunc(rs1StartValue, rs1StartValue, rs2StartValue);
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
            } else if (rs2 == x0 && curMap == rs2Mapped) {
                rs2 = static_cast<t_risc_reg>(i);
            } else if (rd == x0 && curMap == rdMapped) {
                rd = static_cast<t_risc_reg>(i);
            }
        }

    }
};

context_info *ArithmTest::c_info = nullptr;
register_info *ArithmTest::r_info = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//This should ignore the "unknown" ide pragmas without error, but the diagnostic pragmas in GCC don't work on stuff only
// the preprocessor sees.
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-non-const-global-variables"
#pragma ide diagnostic ignored "cert-err58-cpp"

TEST_P(ArithmTest, AllDifferent) {
    blockCache[0] =
            t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rd,
                    {{static_cast<t_risc_imm>(rs2StartValue)}}};

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, false);

    //set every register
    set_value(rs1, rs1StartValue);
    if (!immediateInstr) {
        set_value(rs2, rs2StartValue);
    }
    for (int i = 1; i < 31; i++) {
        if(i != rs1 && (immediateInstr || i != rs2)){
            set_value(static_cast<t_risc_reg>(i),test_reg_values[i]);
        }
    }

    execute_in_guest_context(c_info, loc);

    //check for side effects
    for (int i = 1; i < 31; i++) {
        if(i != rd && i != rs1 && (immediateInstr || i != rs2)) {
            EXPECT_EQ(test_reg_values[i], get_value(static_cast<t_risc_reg>(i)));
        }
    }

    EXPECT_EQ(rs1StartValue, get_value(rs1));
    if (!immediateInstr) {
        EXPECT_EQ(rs2StartValue, get_value(rs2));
    }
    EXPECT_EQ(expectedRd, get_value(rd));

    ///rs1 = x0
    blockCache[0] =
            t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), x0, rs2, rd,
                    static_cast<t_risc_imm>(rs2StartValue)};

    loc = translate_block_instructions(blockCache, 1, c_info, false);

    if (!immediateInstr) {
        set_value(rs2, rs2StartValue);
    }

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(resFunc(rs1StartValue, 0, rs2StartValue), get_value(rd));
    if (!immediateInstr) {
        EXPECT_EQ(rs2StartValue, get_value(rs2));
    }
    if (!immediateInstr) {
        ///rs2 = x0
        blockCache[0] = t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), rs1, x0, rd,
                static_cast<t_risc_imm>(rs2StartValue)};

        loc = translate_block_instructions(blockCache, 1, c_info, false);

        set_value(rs1, rs1StartValue);

        execute_in_guest_context(c_info, loc);

        EXPECT_EQ(rs1StartValue, get_value(rs1));
        EXPECT_EQ(resFunc(rs1StartValue, rs1StartValue, 0), get_value(rd));
    }

    ///rd = x0 means result ignored, but other registers should still not change.
    blockCache[0] =
            t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), rs1, rs2, x0,
                    static_cast<t_risc_imm>(rs2StartValue)};

    loc = translate_block_instructions(blockCache, 1, c_info, false);

    set_value(rs1, rs1StartValue);
    if (!immediateInstr) {
        set_value(rs2, rs2StartValue);
    }

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(rs1StartValue, get_value(rs1));
    if (!immediateInstr) {
        EXPECT_EQ(rs2StartValue, get_value(rs2));
    }
}

TEST_P(ArithmTest, Rs1RdSame) {
    blockCache[0] =
            t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rs1,
                    static_cast<t_risc_imm>(rs2StartValue)};

    t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, false);

    set_value(rs1, rs1StartValue);
    if (!immediateInstr) {
        set_value(rs2, rs2StartValue);
    }

    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(expectedRd, get_value(rs1));
    if (!immediateInstr) {
        EXPECT_EQ(rs2StartValue, get_value(rs2));
    }

    if (!immediateInstr) {
        ///rs2 = x0
        blockCache[0] = t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), rs1, x0, rs1,
                static_cast<t_risc_imm>(rs2StartValue)};

        loc = translate_block_instructions(blockCache, 1, c_info, false);

        set_value(rs1, rs1StartValue);

        execute_in_guest_context(c_info, loc);

        EXPECT_EQ(resFunc(rs1StartValue, rs1StartValue, 0), get_value(rs1));

        ///rs1 = rd = x0 means result ignored, but other register should still not change.
        blockCache[0] =
                t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), x0, rs2, x0,
                        static_cast<t_risc_imm>(rs2StartValue)};

        loc = translate_block_instructions(blockCache, 1, c_info, false);

        if (!immediateInstr) {
            set_value(rs2, rs2StartValue);
        }

        execute_in_guest_context(c_info, loc);

        if (!immediateInstr) {
            EXPECT_EQ(rs2StartValue, get_value(rs2));
        }
    }
}

TEST_P(ArithmTest, Rs2RdSame) {
    if (!immediateInstr) {
        blockCache[0] =
                t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), rs1, rs2, rs2,
                        static_cast<t_risc_imm>(rs2StartValue)};

        t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, false);

        set_value(rs1, rs1StartValue);
        set_value(rs2, rs2StartValue);

        execute_in_guest_context(c_info, loc);

        EXPECT_EQ(rs1StartValue, get_value(rs1));
        EXPECT_EQ(expectedRd, get_value(rs2));

        ///rs1 = x0
        blockCache[0] =
                t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), x0, rs2, rs2,
                        static_cast<t_risc_imm>(rs2StartValue)};

        loc = translate_block_instructions(blockCache, 1, c_info, false);

        set_value(rs2, rs2StartValue);

        execute_in_guest_context(c_info, loc);

        EXPECT_EQ(resFunc(rs1StartValue, 0, rs2StartValue), get_value(rs2));

        ///rs2 = rd = x0 means result ignored, but other register should still not change.
        blockCache[0] =
                t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), rs1, x0, x0,
                        static_cast<t_risc_imm>(rs2StartValue)};

        loc = translate_block_instructions(blockCache, 1, c_info, false);

        set_value(rs1, rs1StartValue);

        execute_in_guest_context(c_info, loc);

        EXPECT_EQ(rs1StartValue, get_value(rs1));
    }
}

TEST_P(ArithmTest, Rs1Rs2Same) {
    if (!immediateInstr) {
        blockCache[0] = t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rd,
                static_cast<t_risc_imm>(rs2StartValue)};

        t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, false);

        set_value(rs1, rs1StartValue);

        execute_in_guest_context(c_info, loc);

        EXPECT_EQ(rs1StartValue, get_value(rs1));
        EXPECT_EQ(resFunc(rs1StartValue, rs1StartValue, rs1StartValue), get_value(rd));

        ///rs1 = rs2 = x0
        blockCache[0] =
                t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), x0, x0, rd,
                        static_cast<t_risc_imm>(rs2StartValue)};

        loc = translate_block_instructions(blockCache, 1, c_info, false);

        execute_in_guest_context(c_info, loc);

        EXPECT_EQ(resFunc(rs1StartValue, 0, 0), get_value(rd));

        ///rd = x0 means result ignored, but other registers should still not change.
        blockCache[0] =
                t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), rs1, rs1, x0,
                        static_cast<t_risc_imm>(rs2StartValue)};

        loc = translate_block_instructions(blockCache, 1, c_info, false);

        set_value(rs1, rs1StartValue);

        execute_in_guest_context(c_info, loc);

        EXPECT_EQ(rs1StartValue, get_value(rs1));
    }
}

TEST_P(ArithmTest, AllSame) {
    if (!immediateInstr) {
        blockCache[0] = t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), rs1, rs1, rs1,
                static_cast<t_risc_imm>(rs2StartValue)};

        t_cache_loc loc = translate_block_instructions(blockCache, 1, c_info, false);

        set_value(rs1, rs1StartValue);

        execute_in_guest_context(c_info, loc);

        EXPECT_EQ(resFunc(rs1StartValue, rs1StartValue, rs1StartValue), get_value(rs1));

        ///rs1 = rs2 = rd = x0 (means effectively nop, not sure how to test)
//        blockCache[0] =
//                t_risc_instr{rs1StartValue, mnem, static_cast<t_risc_optype>(0), x0, x0, x0,
//                        static_cast<t_risc_imm>(rs2StartValue)};

    }
}

INSTANTIATE_TEST_SUITE_P(ADDIW,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(ADDIW),
                                 testing::Values(true),
                                 testing::Values(1, -1),
                                 testing::Values(2, -5, 0),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     //(Mask and ) Cast to int32_t to do 32bit addition of the lower 32bits, then cast
                                     // to int64_t to do sign extension, finally cast to t_risc_reg_val so the lambda
                                     // has the right type
                                     return (t_risc_reg_val) (int64_t) ((int32_t) (rs1 & 0xffFFffFF) +
                                             (int32_t) immediate);
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SLLI,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SLLI),
                                 testing::Values(true),
                                 testing::Values(1, -1),
                                 testing::Values(2, 0b1001111),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     return rs1 << (immediate & 0x3F);
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(LUI,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(LUI),
                                 testing::Values(true),
                                 testing::Values(0), //Rs1 does not matter
                                 //In reality only bits 31-12 can be occupied, but our implementation demands also
                                 // non-upper immediates to work.
                                 testing::Values(0x15 << 12, 5, -1lu << 12, -1),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1 attr_unused,
                                                    t_risc_reg_val immediate) {
                                     //Cast to int32_t to get the value as 32bit, then cast to int64_t to do sign
                                     // extension, finally cast to t_risc_reg_val so the lambda has the right type
                                     return (t_risc_reg_val) (int64_t) (int32_t) immediate;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(ADDI,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(ADDI),
                                 testing::Values(true),
                                 testing::Values(1lu << 33, -1),
                                 testing::Values(2, -5, 0),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {return rs1 + immediate;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AUIPC,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(AUIPC),
                                 testing::Values(true),
                                 testing::Values(1lu << 33, -1),
                                 //In reality only bits 31-12 can be occupied, but our implementation demands also
                                 // non-upper immediates to work.
                                 testing::Values(0x15 << 12, 5, -1lu << 12, -1),
                                 testing::Values([](t_risc_addr addr, t_risc_reg_val rs1 attr_unused,
                                                    t_risc_reg_val immediate) {
                                     //Cast to int32_t to get the value as 32bit, then cast to int64_t to do sign
                                     // extension, finally cast to t_risc_reg_val so the addition has the right type
                                     return addr + ((t_risc_reg_val) (int64_t) (int32_t) immediate);
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SLTI,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SLTI),
                                 testing::Values(true),
                                 testing::Values(1lu << 33, -1),
                                 testing::Values(2, -5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     return (int64_t) rs1 < (int64_t) immediate ? 1lu : 0lu;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SLTIU,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SLTIU),
                                 testing::Values(true),
                                 testing::Values(1lu << 33, -1),
                                 testing::Values(2, -5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     return rs1 < immediate ? 1lu : 0lu;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(XORI,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(XORI),
                                 testing::Values(true),
                                 testing::Values(1lu << 33, -1),
                                 testing::Values(2, -5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     return rs1 ^ immediate;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(ORI,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(ORI),
                                 testing::Values(true),
                                 testing::Values(1lu << 33, -1lu << 15),
                                 testing::Values(2, -5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     return rs1 | immediate;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(ANDI,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(ANDI),
                                 testing::Values(true),
                                 testing::Values(1lu << 33, -1lu << 5),
                                 testing::Values(2, -5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     return rs1 & immediate;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SRLI,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SRLI),
                                 testing::Values(true),
                                 testing::Values(1lu << 32, -1),
                                 testing::Values(2, 0b1001111),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     return rs1 >> (immediate & 0x3F);
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SRAI,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SRAI),
                                 testing::Values(true),
                                 testing::Values(1lu << 32, -10),
                                 testing::Values(2, 0b1001111),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     //First cast to int64_t to do arithmetic shift, then cast back to get right lambda
                                     // type
                                     return (t_risc_reg_val) ((int64_t) rs1 >> (immediate & 0x3F));
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(ADD,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(ADD),
                                 testing::Values(false),
                                 testing::Values(1lu << 32, -1),
                                 testing::Values(2lu << 33, 5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {return rs1 + rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SUB,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SUB),
                                 testing::Values(false),
                                 testing::Values(1lu << 32, -1),
                                 testing::Values(2lu << 33, 5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {return rs1 - rs2;}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SLL,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SLL),
                                 testing::Values(false),
                                 testing::Values(1lu << 32, -1),
                                 testing::Values(0b1000001, 5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {return rs1 << (rs2 & 0b111111);}),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SLT,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SLT),
                                 testing::Values(false),
                                 testing::Values(1lu << 33, -1),
                                 testing::Values(2, -5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     return
                                             (int64_t) rs1 < (int64_t) rs2 ? 1lu : 0lu;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SLTU,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SLTU),
                                 testing::Values(false),
                                 testing::Values(1lu << 33, -1),
                                 testing::Values(2, -5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     return
                                             rs1 < rs2 ? 1lu : 0lu;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(XOR,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(XOR),
                                 testing::Values(false),
                                 testing::Values(1lu << 33, -1),
                                 testing::Values(2, -5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     return rs1 ^ rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SRL,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SRL),
                                 testing::Values(false),
                                 testing::Values(1lu << 32, -1),
                                 testing::Values(2, 0b1001111),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     return rs1 >> (rs2 & 0x3F);
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SRA,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SRA),
                                 testing::Values(false),
                                 testing::Values(1lu << 32, -10),
                                 testing::Values(2, 0b1001111),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     //First cast to int64_t to do arithmetic shift, then cast back to get right lambda
                                     // type
                                     return (t_risc_reg_val) ((int64_t) rs1 >> (rs2 & 0x3F));
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(OR,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(OR),
                                 testing::Values(false),
                                 testing::Values(1lu << 33, -1lu << 15),
                                 testing::Values(2, -5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     return rs1 | rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(AND,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(AND),
                                 testing::Values(false),
                                 testing::Values(1lu << 33, -1lu << 5),
                                 testing::Values(2, -5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     return rs1 & rs2;
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SLLIW,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SLLIW),
                                 testing::Values(true),
                                 testing::Values(1, -1),
                                 testing::Values(2, 0b101111),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     //First cast to int32_t and mask to do shift of 32bit value, then cast to
                                     // int64_t to do sign extension and then back to get right lambda type
                                     return (t_risc_reg_val) (int64_t) ((int32_t) (rs1 & 0xffFFffFF)
                                             << (immediate & 0x1F));
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SRLIW,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SRLIW),
                                 testing::Values(true),
                                 testing::Values(1lu << 20, -1),
                                 testing::Values(2, 0b101111),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     //First cast to uint32_t and mask to do logical shift of 32bit value, then cast to
                                     // int32_t and int64_t to do sign extension and then back to get right lambda type
                                     return (t_risc_reg_val) (int64_t) (int32_t) ((uint32_t) (rs1 & 0xffFFffFF)
                                             >> (immediate & 0x1F));
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SRAIW,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SRAIW),
                                 testing::Values(true),
                                 testing::Values(1lu << 20, -10),
                                 testing::Values(2, 0b101111),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val immediate) {
                                     //First cast to int32_t and mask to do arithmetic shift of 32bit value, then cast
                                     // to int64_t to do sign extension and then back to get right lambda type
                                     return (t_risc_reg_val) (int64_t) ((int32_t) (rs1 & 0xffFFffFF)
                                             >> (immediate & 0x1F));
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(ADDW,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(ADDW),
                                 testing::Values(false),
                                 testing::Values(1, -1),
                                 testing::Values(2, 5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     return (t_risc_reg_val) (int64_t) ((int32_t) (rs1 & 0xffFFffFF) +
                                             (int32_t) (rs2 & 0xffFFffFF));
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SUBW,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SUBW),
                                 testing::Values(false),
                                 testing::Values(1, -1),
                                 testing::Values(2, 5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     return (t_risc_reg_val) (int64_t) ((int32_t) (rs1 & 0xffFFffFF) -
                                             (int32_t) (rs2 & 0xffFFffFF));
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));


INSTANTIATE_TEST_SUITE_P(SLLW,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SLLW),
                                 testing::Values(false),
                                 testing::Values(1lu << 32, -1),
                                 testing::Values(0b1000001, 5),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     //First cast to int32_t and mask to do shift of 32bit value, then cast to
                                     // int64_t to do sign extension and then back to get right lambda type
                                     return (t_risc_reg_val) (int64_t) ((int32_t) (rs1 & 0xffFFffFF)
                                             << (rs2 & 0x1F));
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SRLW,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SRLW),
                                 testing::Values(false),
                                 testing::Values(1lu << 32, -1),
                                 testing::Values(2, 0b1001111),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     //First cast to uint32_t and mask to do logical shift of 32bit value, then cast to
                                     // int32_t and int64_t to do sign extension and then back to get right lambda type
                                     return (t_risc_reg_val) (int64_t) (int32_t) ((uint32_t) (rs1 & 0xffFFffFF)
                                             >> (rs2 & 0x1F));

                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

INSTANTIATE_TEST_SUITE_P(SRAW,
                         ArithmTest,
                         testing::Combine(
                                 testing::Values(SRAW),
                                 testing::Values(false),
                                 testing::Values(1lu << 32, -10),
                                 testing::Values(2, 0b1001111),
                                 testing::Values([](t_risc_addr addr attr_unused, t_risc_reg_val rs1,
                                                    t_risc_reg_val rs2) {
                                     //First cast to int32_t and mask to do arithmetic shift of 32bit value, then cast
                                     // to int64_t to do sign extension and then back to get right lambda type
                                     return (t_risc_reg_val) (int64_t) ((int32_t) (rs1 & 0xffFFffFF)
                                             >> (rs2 & 0x1F));
                                 }),
                                 testing::Bool(),
                                 testing::Bool(),
                                 testing::Bool()));

#pragma ide diagonstics pop
#pragma GCC diagnostic pop
