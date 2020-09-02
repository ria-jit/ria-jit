//
// Created by flo on 07.06.20.
//

#include <gtest/gtest.h>
#include <runtime/register.h>

/**
 * Checks the register.c storage by storing and reading back values via get_reg_data() base access.
 */
TEST(RegisterCache, ShouldStoreValues) {
    t_risc_reg_val *base = get_reg_data();
    
    //store testing values
    for (size_t r = 1; r <= 31; r++) {
        base[r] = r;
    }

    //read back
    for (size_t r = 1; r <= 31; r++) {
        ASSERT_EQ(r, base[r]);
    }
}
