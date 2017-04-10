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
#include "main_memory.h"


/* CACHE INFO */
//Data cache size in bytes
#define D_CACHE_SIZE 1024
//Instruction cache size in bytes
#define I_CACHE_SIZE 1024
//Direct mapped data cache
#define DIRECT_MAPPED 1
//line fill block policy (1 or 4)
#define LINE_FILL 1
//Cache miss penalty
#define CACHE_MISS_PENALTY 8
//If multiple lines come in, subsequent lines miss penalty
#define CACHE_MISS_SUBSEQUENT_PENALTY 2
//Write to main memory penalty for first block written
#define CACHE_WRITE_PENALTY 4
//If multiple lines are written back, penalty for each subsequent write
#define CACHE_WRITE_SUBSEQUENT_PENALTY 1
//Write policy for the cache (EXACTLY ONE MUST BE DEFINED)
//#define WRITEBACK
#define WRITETHROUGH


//type for cache retrieval. Will be CACHE_HIT, CACHE_MISS
typedef enum CACHE_STATUS {
    CACHE_MISS,         //Data isn't in cache, stall
    CACHE_HIT           //Data returned is valid
} cache_status_t;

typedef enum MEMORY_STATUS {
    MEM_IDLE,         //Initial state of memory
    MEM_WRITING,      //Writing data cache to main memory
    MEM_READING_D,    //Reading from memory into data cache
    MEM_READING_I     //Reading from memory into instruction cache
} memory_status_t;




//Represents the tag field of the direct cache block
typedef uint32_t tag_t;
//Represents the data in a direct cache block
typedef uint32_t data_t;

#ifdef DIRECT_MAPPED
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
    //Flag to tell if active fetch from memory
    bool fetching;
    //Used for getting multiple block lines
    uint8_t subsequent_fetching;
    uint32_t penalty_count;
    uint32_t target_address;
    direct_cache_block_t *blocks;
} direct_cache_t;

void direct_cache_get_tag_and_index(direct_cache_t *cache, uint32_t *address, uint32_t *index, uint32_t *tag);
void direct_cache_digest(direct_cache_t *cache, memory_status_t proceed_condition);
direct_cache_t * direct_cache_init(uint32_t num_blocks);
cache_status_t direct_cache_get_word(direct_cache_t *cache, uint32_t *address, uint32_t *data);
#endif /* DIRECT_MAPPED */


memory_status_t get_mem_status(void);
void set_mem_status(memory_status_t status);

void cache_init(void);
void cache_destroy(void);
void cache_digest(void);

cache_status_t d_cache_read_w(uint32_t *address, word_t *data);
cache_status_t d_cache_read_h(uint32_t *address, word_t *data);
cache_status_t d_cache_read_b(uint32_t *address, word_t *data);

cache_status_t d_cache_write_w(uint32_t *address, word_t *data);
cache_status_t d_cache_write_h(uint32_t *address, word_t *data);
cache_status_t d_cache_write_b(uint32_t *address, word_t *data);


void d_cache_init(void);

#ifndef UNIFIED
void i_cache_init(void);
cache_status_t i_cache_read_w(uint32_t *address, word_t *data);
cache_status_t i_cache_write_w(uint32_t *address, word_t *data);
#endif /* UNIFIED */

#ifdef WRITETHROUGH
typedef struct WRITETHROUGH_BUFFER {
    uint32_t data;
    uint32_t address;
    uint32_t penalty_count;
    bool writing;
    bool overflow;
} write_buffer_t;
cache_status_t write_through(uint32_t *address, word_t *data);
#endif


write_buffer_t *write_buffer_init(void);
void write_buffer_destroy(write_buffer_t *wb);
void write_buffer_digest(void);





#endif /*_CACHE_H*/
