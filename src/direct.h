/*
* src/direct.h
* header for the direct mapped cache implementation functions
*/

#ifndef _DIRECT_H
#define _DIRECT_H


#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "util.h"
#include "main_memory.h"
#include "types.h"
#include "cache.h"

//Cache miss penalty
#define CACHE_MISS_PENALTY 8
//If multiple lines come in, subsequent lines miss penalty
#define CACHE_MISS_SUBSEQUENT_PENALTY 2

//Represents the tag field of the direct cache block
typedef uint32_t tag_t;



//Struct for a single block of a direct mapped cache
typedef struct DIRECT_CACHE_BLOCK {
    bool *valid;
    bool dirty;
    tag_t tag;
    word_t *data;
} direct_cache_block_t;

typedef struct DIRECT_CACHE {
    uint32_t num_blocks;
    uint32_t block_size;
    uint32_t tag_size;
    uint32_t tag_mask;
    uint32_t index_size;
    uint32_t index_mask;
    uint32_t inner_index_size;
    uint32_t inner_index_mask;
    //Flag to tell if active fetch from memory
    bool fetching;
    //Used for getting multiple block lines
    uint8_t subsequent_fetching;
    uint32_t penalty_count;
    uint32_t target_address;
    direct_cache_block_t *blocks;
    word_t *words;
} direct_cache_t;




/*
* direct_cache_t * direct_cache_init(uint32_t num_blocks)
* Creates an instance of a direct mapped cache with the number
* of blocks as a parameter. From the number of blocks we can determine
* dynamically the tag size. This function also initializes all of the
* blocks to have invalid data and sets up bitmasks to easily obtain
* index and tags from an address.
*/
/* Get the bit masks for the tag and index
hopefully this example will make this look less like magic
EXAMPLE:
tag_size = 12 (bits)
index_size = 18 (bits)
want       = tttt tttt tttt iiii iiii iiii iiii iixx
index_mask =           0001 0000 0000 0000 0000 0000 (1 << (index_size + 2))
index_mask =                1111 1111 1111 1111 1111 (index_mask - 1)
tag_mask   = 1111 1111 1111 0000 0000 0000 0000 0000 (tag_mask = ~index_mask)
index_mask =                1111 1111 1111 1111 1100 (index_mask & ~3)//helper functions do not call directly
*/
direct_cache_t * direct_cache_init(uint32_t num_blocks, uint32_t block_size);

void direct_cache_free(direct_cache_t *cache);

/*
* void direct_cache_digest(direct_cache_t *cache, memory_status_t proceed_condition)
* function to be called every cycle of the clock.
* No advancement on stall counters will occur if the memory state does not
* match the given proceed condition
* @params *cache is which cache the digest will operate on
* @params proceed_condition is a memory state to ensure a read doesn't proceed
*         if there is another memory operation occuring
*/
void direct_cache_digest(direct_cache_t *cache, memory_status_t proceed_condition);

/* cache_status_t direct_cache_get_word(direct_cache_t *cache, uint32_t *address, uint32_t *data)
* returns CACHE_HIT or CACHE_MISS depending on if the data is available in the cache
* if there is a CACHE_MISS, function will set up the direct mapped cache to
* start fetching the data from main memory.
*/
cache_status_t direct_cache_read_w(direct_cache_t *cache, uint32_t *address, uint32_t *data);

/*  @brief Sets up a word to be written back to main memory
*   If writeback, the dirty bit in the cache gets set and returns. Once the
*   block gets replaced, the data will be written back to main memory
*   If writethrough, the data gets set in the cache as well as the write buffer
*   If the write buffer is full, this will return CACHE_MISS to inform the processor
*   if needs to stall
*/
cache_status_t direct_cache_write_w(direct_cache_t *cache, uint32_t *address, uint32_t *data);

void direct_cache_fill_word(direct_cache_t *cache, cache_access_t info);

cache_status_t direct_cache_access_word(direct_cache_t *cache, cache_access_t *info);

void direct_cache_queue_mem_access(direct_cache_t *cache, cache_access_t info);

/* Helper functions specific to the direct mapped cache */
void direct_cache_get_tag_and_index(cache_access_t *info, direct_cache_t *cache, uint32_t *address);


/* Debugging functions */
void direct_cache_print(direct_cache_t *cache);
void direct_cache_print_block(direct_cache_t *cache, int index);




#endif /* _DIRECT_H */
