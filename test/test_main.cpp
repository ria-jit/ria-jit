//
// Created by flo on 07.06.20.
//

#include <gtest/gtest.h>

/**
 * Initializes the Googletest framework and runs all corresponding tests.
 * @param argc command line arg count
 * @param argv command line arg vector
 * @return 0 if all tests are successful, 1 otherwise
 */
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}