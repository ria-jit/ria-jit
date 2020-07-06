//
// Created by flo on 07.06.20.
//

#include <gtest/gtest.h>
#include <elf/loadElf.h>

/**
 * Tests parsing of basic load-immediate as per test_parsing() in parser.c (--> @author Noah)
 */
TEST(LoadElf, LoadFileInMemory) {
    const char path[] = "./a.out";
    mapIntoMemory(path);
}