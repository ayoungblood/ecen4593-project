/* src/cache.h
 * Wrapper functions for cache operations
 */

#ifndef _CACHE_H
#define _CACHE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "util.h"


/* CACHE INFO */
//Data cache size in bytes
#define D_CACHE_SIZE 1024
//Instruction cache size in bytes
#define I_CACHE_SIZE 1024
//Direct mapped data cache
#define DIRECT_MAPPED 1

//type for cache retrieval. Will be CACHE_HIT or CACHE_MISS
typedef bool cache_status_t;
#define CACHE_HIT 1
#define CACHE_MISS 0



//Represents the tag field of the direct cache block
typedef uint32_t tag_t;
//Represents the data in a direct cache block
typedef uint32_t data_t;

//Struct for a single block of a direct mapped cache
typedef struct DIRECT_CACHE_BLOCK {
    bool valid;
    tag_t tag;
    data_t data;
} direct_cache_block_t;


typedef struct DIRECT_CACHE {
    uint32_t num_blocks;
    uint32_t tag_size;
    uint32_t tag_mask;
    uint32_t index_size;
    uint32_t index_mask;
    direct_cache_block_t *blocks;
} direct_cache_t;

#ifdef DIRECT_MAPPED
direct_cache_t *d_cache;
#endif /* DIRECT_MAPPED */




void cache_init(void);
void cache_destroy(void);

cache_status_t d_cache_get_word(uint32_t *address, word_t *data);



void d_cache_init(void);
void i_cache_init(void);



#endif /*_CACHE_H*/
