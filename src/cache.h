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


/* CACHE INFO */
//Data cache size in bytes
#define D_CACHE_SIZE 1024
//Instruction cache size in bytes
#define I_CACHE_SIZE 1024

//Write to main memory penalty for first block written
#define CACHE_WRITE_PENALTY 4
//If multiple lines are written back, penalty for each subsequent write
#define CACHE_WRITE_SUBSEQUENT_PENALTY 1
//Write policy for the cache (EXACTLY ONE MUST BE DEFINED)
//#define WRITEBACK
#define WRITETHROUGH



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
void i_cache_init(void);
cache_status_t i_cache_read_w(uint32_t *address, word_t *data);
cache_status_t i_cache_write_w(uint32_t *address, word_t *data);



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
