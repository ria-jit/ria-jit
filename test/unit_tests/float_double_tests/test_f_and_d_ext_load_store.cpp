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

TEST(FloatExtension, LoadStore) {
    t_risc_reg src_base = x1;
    t_risc_reg dest_base = x2;
    t_risc_reg rd = x1;
    const size_t test_size = 30;
    t_risc_instr blockCache[test_size*2]{};
    for(int i = 0;i<test_size;i++) {
        blockCache[i] =
                t_risc_instr{0, FLW, static_cast<t_risc_optype>(0), src_base, src_base, static_cast<t_risc_reg>(rd + i), static_cast<t_risc_imm>(i*sizeof(float))};
    }
    for(int i = 0;i<test_size;i++) {
        blockCache[30+i] =
                t_risc_instr{0, FSW, static_cast<t_risc_optype>(0), dest_base,static_cast<t_risc_reg>(i + 1), rd, static_cast<t_risc_imm>(i*sizeof(float))};
    }

    static const context_info *c_info = init_map_context(true);
    t_cache_loc loc = translate_block_instructions(blockCache, test_size * 2, c_info, false);

    float test_source[] = {1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9,10.1,11.11,12.12,13.13,14.14,15.15,16.16,17.17,18.18,19.19,20.2,21.21,22.22,23.23,24.24,25.25,26.26,27.27,28.28,29.29,30.3};
    float test_destination[30];



    t_risc_reg_val src_base_address = (t_risc_reg_val)&test_source;
    t_risc_reg_val dest_base_address = (t_risc_reg_val)&test_destination;
    set_value(src_base, src_base_address);
    set_value(dest_base, dest_base_address);


    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(src_base_address, get_value(src_base));
    EXPECT_EQ(dest_base_address, get_value(dest_base));
    for(int i = 0;i<30;i++){
        EXPECT_EQ(test_source[i], test_destination[i]);
    }
}

TEST(DoubleExtension, LoadStore) {
    t_risc_reg src_base = x1;
    t_risc_reg dest_base = x2;
    t_risc_reg rd = x1;
    const size_t test_size = 30;
    t_risc_instr blockCache[test_size*2]{};
    for(int i = 0;i<test_size;i++) {
        blockCache[i] =
                t_risc_instr{0, FLD, static_cast<t_risc_optype>(0), src_base, src_base, static_cast<t_risc_reg>(rd + i), {{static_cast<t_risc_imm>(i*sizeof(double))}}};
    }
    for(int i = 0;i<test_size;i++) {
        blockCache[30+i] =
                t_risc_instr{0, FSD, static_cast<t_risc_optype>(0), dest_base,static_cast<t_risc_reg>(i + 1), rd, {{static_cast<t_risc_imm>(i*sizeof(double))}}};
    }

    static const context_info *c_info = init_map_context(true);
    t_cache_loc loc = translate_block_instructions(blockCache, test_size * 2, c_info, false);

    double test_source[] = {1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9,10.1,11.11,12.12,13.13,14.14,15.15,16.16,17.17,18.18,19.19,20.2,21.21,22.22,23.23,24.24,25.25,26.26,27.27,28.28,29.29,30.3};
    double test_destination[30];



    t_risc_reg_val src_base_address = (t_risc_reg_val)&test_source;
    t_risc_reg_val dest_base_address = (t_risc_reg_val)&test_destination;
    set_value(src_base, src_base_address);
    set_value(dest_base, dest_base_address);


    execute_in_guest_context(c_info, loc);

    EXPECT_EQ(src_base_address, get_value(src_base));
    EXPECT_EQ(dest_base_address, get_value(dest_base));
    for(int i = 0;i<30;i++) {
        EXPECT_EQ(test_source[i], test_destination[i]);
    }
}