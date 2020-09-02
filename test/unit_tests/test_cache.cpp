//
// Created by flo on 07.06.20.
//

#include <gtest/gtest.h>
#include <cache/cache.h>
#include <util/log.h>

/**
 * Stores some values in the cache and reads them back to verify.
 */
TEST(CodeCache, StoresCachedCorrectly) {
    //initialize the cache
    init_hash_table();
    
    //store and lookup basic values
    set_cache_entry((t_risc_addr) 1, reinterpret_cast<t_cache_loc>(0x100));
    set_cache_entry((t_risc_addr) 2, reinterpret_cast<t_cache_loc>(0x200));
    set_cache_entry((t_risc_addr) 3, reinterpret_cast<t_cache_loc>(0x300));

    t_cache_loc entryA = lookup_cache_entry((t_risc_addr) 1);
    EXPECT_NE(UNSEEN_CODE, entryA);
    EXPECT_EQ((void*) 0x100, entryA);

    t_cache_loc entryB = lookup_cache_entry((t_risc_addr) 2);
    EXPECT_NE(UNSEEN_CODE, entryB);
    EXPECT_EQ((void*) 0x200, entryB);

    t_cache_loc entryC = lookup_cache_entry((t_risc_addr) 3);
    EXPECT_NE(UNSEEN_CODE, entryC);
    EXPECT_EQ((void*) 0x300, entryC);
}

/**
 * Maxes out the default cache size (4096) in order to trigger a resize, then checks value consistency.
 */
TEST(CodeCache, ResizesTableCorrectly) {
    //initialize the cache
    init_hash_table();

    //fill table
    for (size_t i = 1; i <= 5000; i++) {
        set_cache_entry((t_risc_addr) (i << 3u), (t_cache_loc) i);
    }

    printf("Filled…\n");

    //expect respective results
    for (size_t i = 1; i <= 5000; i++) {
        ASSERT_EQ((t_cache_loc) i, lookup_cache_entry((t_risc_addr) (i << 3u)));
    }

    printf("Checked…\n");
}
