//
// Created by flo on 02.05.20.
//

#include <stddef.h>
#include "cache.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_SIZE 4096
#define HASH_MASK 0x0000FFF0u

//init table for 2^12 elements (key size is 12 bit)
t_cache_entry *cache_table = NULL;
size_t table_size = INITIAL_SIZE;
int count_entries = 0;

/**
 * Initializes the hash table array.
 */
void init_hash_table() {
    //allocate memory for our table
    cache_table = malloc(table_size * sizeof(t_cache_entry));

    //check for heap memory allocation fail
    if (cache_table == NULL) {
        printf("Bad. Cache memory allocation failed.");
        exit(FAIL_HEAP_ALLOC);
    }

    //initialize memory to 0
    memset(cache_table, 0, table_size * sizeof(t_cache_entry));
}

inline unsigned int hash(t_risc_addr risc_addr) {
    return ((uintptr_t)risc_addr & HASH_MASK) >> 4;
}

/**
 * Find the hash table index for the passed address.
 * @param risc_addr RISC-V code address to lookup_cache_entry in hash table
 * @return table index for either the address in question or the next available field
 */
unsigned int find_lin_slot(t_risc_addr risc_addr) {
    unsigned int index = hash(risc_addr);

    //linearly probe for key or empty field
    while(cache_table[index].cache_loc != 0 && cache_table[index].risc_addr != risc_addr) {
        //cyclically increment to find available slots
        index = (index + 1) % table_size;
    }

    return index;
}

/**
 * Lookup the cache location of a given RISC-V instruction address.
 * @param risc_addr the RISC-V instruction address.
 * @return code cache address of that instruction, or NULL if nonexistent
 */
t_cache_loc lookup_cache_entry(t_risc_addr risc_addr) {
    unsigned int index = find_lin_slot(risc_addr);

    if (cache_table[index].cache_loc != 0) {
        //value is cached and exists
        return cache_table[index].cache_loc;
    } else {
        //value does not exist
        return UNSEEN_CODE;
    }
}

void set_cache_entry(t_risc_addr risc_addr, t_cache_loc cache_loc) {
    unsigned int index = find_lin_slot(risc_addr);

    //check for table full before inserting
    if (count_entries >= table_size) {
        //double the table size
        size_t offset = table_size * sizeof(t_cache_entry);
        table_size <<= 1u;

        //allocate new heap space for the cache table and copy over the values we have saved
        t_cache_entry *copy_buffer = malloc(table_size * sizeof(t_cache_entry));
        memset(copy_buffer, 0, table_size * sizeof(t_cache_entry));
        for(int i = 0; i < count_entries; ++i) {
            copy_buffer[i] = cache_table[i];
        }

        //free and reset original allocated space
        free(cache_table);
        cache_table = copy_buffer;

        //find index again
        index = find_lin_slot(risc_addr);
    }

    //if we already have a value there, update it
    if (cache_table[index].cache_loc != 0) {
        //update value in table
        cache_table[index].cache_loc = cache_loc;
        return;
    }

    //insert value into the table if we have not seen it before
    cache_table[index].cache_loc = cache_loc;
    cache_table[index].risc_addr = risc_addr;
    count_entries++;
}

/**
 * Print out the hash table contents.
 */
void print_values() {
    for (int i = 0; i < count_entries; ++i) {
        if (cache_table[i].cache_loc != 0) {
            printf("cache_table[%d]: RISC-V addr %lu at cache loc %p\n",
                    i,
                   (uintptr_t)cache_table[i].risc_addr,
                    cache_table[i].cache_loc);
        }
    }
}