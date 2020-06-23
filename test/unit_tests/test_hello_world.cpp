//
// Created by flo on 07.06.20.
//

#include <gtest/gtest.h>
#include <main.h>

/**
 * Checks the register.c storage by storing and reading back values via get_reg_data() base access.
 */
TEST(ProgrammExecution, ShouldPrintHelloWorld) {
    //runs the a.out file with main loop
    const char path[] = "./compiled_hello_world";
    start_transcode(path);
}