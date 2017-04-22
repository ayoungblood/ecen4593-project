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
#include "types.h"
#include "main_memory.h"
#include "direct.h"


//Write to main memory penalty for first block written
#define CACHE_WRITE_PENALTY 4
//If multiple lines are written back, penalty for each subsequent write
#define CACHE_WRITE_SUBSEQUENT_PENALTY 1
//Write policy for the cache (EXACTLY ONE MUST BE DEFINED)



memory_status_t get_mem_status(void);
void set_mem_status(memory_status_t status);

void cache_init(cache_config_t *cache_cfg);
void cache_destroy(void);
void cache_digest(void);

cache_status_t d_cache_read_w(uint32_t *address, word_t *data);
cache_status_t d_cache_write_w(uint32_t *address, word_t *data);


void d_cache_init(cache_config_t *cache_cfg);
void i_cache_init(cache_config_t *cache_cfg);
cache_status_t i_cache_read_w(uint32_t *address, word_t *data);
cache_status_t i_cache_write_w(uint32_t *address, word_t *data);



typedef struct WRITE_BUFFER {
    uint32_t address;
    bool writing;
    uint32_t penalty_count;
    uint32_t subsequent_writing;
    word_t *data;
} write_buffer_t;


write_buffer_t *write_buffer_init(void);
void write_buffer_destroy(write_buffer_t *wb);
void write_buffer_digest(void);
cache_status_t write_buffer_enqueue(cache_access_t info);

/*Debugging stuff*/
void print_icache(int block);
void print_dcache(int block);
void printe_write_buffer(void);

#endif /*_CACHE_H*/
