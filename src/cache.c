/* src/cache.c
* Wrapper functions for all cache operations
*/


#include "cache.h"

extern int flags;

direct_cache_t *d_cache;
direct_cache_t *i_cache;
write_buffer_t *write_buffer;
memory_status_t memory_status = MEM_IDLE;

memory_status_t get_mem_status(void){
    return memory_status;
}
void set_mem_status(memory_status_t status){
    memory_status = status;
}

void cache_init(void){

    set_mem_status(MEM_IDLE);
    d_cache_init();
    i_cache_init();


}

void d_cache_init(void){

    //Check if cache size is a power of two
    if((D_CACHE_SIZE & (D_CACHE_SIZE - 1)) != 0) {
        printf(ANSI_C_RED "cache_init: D_CACHE_SIZE %d not a power of two\n" ANSI_C_RESET, D_CACHE_SIZE);
        assert(0);
    }

    //Each block contains a word of data
    uint32_t num_blocks = D_CACHE_SIZE >> 2;
    d_cache = direct_cache_init(num_blocks, 4);
}

void i_cache_init(void){
    //chech to make sure instruction cache size is a power of two
    if((I_CACHE_SIZE & (I_CACHE_SIZE - 1)) != 0) {
        printf(ANSI_C_RED "cache_init: I_CACHE_SIZE %d not a power of two\n" ANSI_C_RESET, D_CACHE_SIZE);
        assert(0);
    }

    uint32_t num_blocks = I_CACHE_SIZE >> 2;
    i_cache = direct_cache_init(num_blocks, 1);
}


void cache_destroy(void){
    //free the cache
    free(d_cache->blocks);
    free(d_cache);

    free(i_cache->blocks);
    free(i_cache);
    return;
}

/* void cache_digest(void)
* processes the cache on each cycle
* handles the business logic of fetching data from main memory,
* waiting the specified cache miss penalty, retreiving subsequent lines from
* memory.
*/

void cache_digest(void){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "CACHE DIGEST:\n" ANSI_C_RESET);
    }

    if(d_cache == NULL){
        printf(ANSI_C_RED "cache_digest: data cache is not initialized\n" ANSI_C_RESET);
        assert(0);
    }
    #ifndef UNIFIED
    if(i_cache == NULL){
        printf(ANSI_C_RED "cache_digest: instruction cache is not initialized\n" ANSI_C_RESET);
        assert(0);
    }
    #endif
    if(write_buffer == NULL){
        printf(ANSI_C_RED "cache_digest: write buffer is not initialized\n" ANSI_C_RESET);
        assert(0);
    }

    //State machine to ensure we do not have more than one memory access at a time
    //This is a state that is consistent between all
    switch (get_mem_status()) {
        case MEM_IDLE:
            //Ready to accept new memory accesses
            //check for data cache read requests
            if(d_cache->fetching){
                set_mem_status(MEM_READING_D);
            }
            else if(i_cache->fetching){
                set_mem_status(MEM_READING_I);
            }
            else if(write_buffer->writing){
                set_mem_status(MEM_WRITING);
            }
            break;
        case MEM_READING_D:
            //Last digest cycle, we were reading into data cache. See if still reading
            if(d_cache->fetching){
                //Still reading, no state change
                break;
            }
            else if(i_cache->fetching){
                //Now instruction cache is reading
                set_mem_status(MEM_READING_I);
            }
            else if(write_buffer->writing){
                //writing from data cache to memory
                set_mem_status(MEM_WRITING);
            }
            else {
                set_mem_status(MEM_IDLE);
            }
            break;
        case MEM_READING_I:
            //Last cycle we were reading into instruction cache
            if(i_cache->fetching){
                //still reading into i cache
                break;
            }
            else if(d_cache->fetching){
                //now we are reading into d cache
                set_mem_status(MEM_READING_D);
            }
            else if(write_buffer->writing){
                //now we are writing into memory from d cache
                set_mem_status(MEM_WRITING);
            }
            else {
                //nothing to do
                set_mem_status(MEM_IDLE);
            }
            break;
        case MEM_WRITING:
            //Last cycle we were writing to memory
            if(write_buffer->writing){
                //still writing
                break;
            }
            else if(d_cache->fetching){
                //Now reading into data cache
                set_mem_status(MEM_READING_D);
            }
            else if(i_cache->fetching){
                //Now reading into instruction cache
                set_mem_status(MEM_READING_I);
            }
            else{
                //nothing to do
                set_mem_status(MEM_IDLE);
            }
            break;
        default:
            printf(ANSI_C_RED "cache_digest: Undefined Memory State %d" ANSI_C_RESET, get_mem_status());
            assert(0);
            break;
    }

    direct_cache_digest(d_cache, MEM_READING_D);
    direct_cache_digest(i_cache, MEM_READING_I);
}




cache_status_t d_cache_read_w(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "D_CACHE GET WORD:\n" ANSI_C_RESET);
    }
    //Get data from the data cache
    cache_status_t status = direct_cache_get_word(d_cache, address, data);
    return status;
}

cache_status_t d_cache_read_h(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "D_CACHE GET HALFWORD:\n" ANSI_C_RESET);
    }
    //Get data from the data cache
    cache_status_t status = direct_cache_get_word(d_cache, address, data);

    //Make it a halfword
    uint32_t shift = ((2-(*address & 0x2))<<3); // shift amount based on byte position
    *data >>= shift;
    *data &= 0xffff;
    return status;
}

cache_status_t d_cache_read_b(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "D_CACHE GET BYTE:\n" ANSI_C_RESET);
    }
    //Get data from the data cache

    cache_status_t status = direct_cache_get_word(d_cache, address, data);

    uint32_t shift = ((3-(*address & 0x3))<<3); // shift amount based on byte position
    *data >>= shift;
    *data &= 0xff;
    return status;
}

cache_status_t d_cache_write_w(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "D_CACHE WRITE WORD:\n" ANSI_C_RESET);
    }

    cache_status_t status = write_through(address, data);
    return status;
}

cache_status_t i_cache_read_w(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "I_CACHE GET WORD:\n" ANSI_C_RESET);
    }
    //Get data from the data cache

    cache_status_t status = direct_cache_get_word(i_cache, address, data);

    return status;
}


/* WRITETHROUGH BUFFER FUNCTIONS */
#ifdef WRITETHROUGH
/*
* @brief Puts data into the write buffer and attempts to initialze a write
* to main memory.
* @params the address of the data and the data to be written
* @returns CACHE_HIT if the write is successfully placed in the buffer
*          and a CACHE_MISS if the write buffer is full
*/
cache_status_t write_through(uint32_t *address, word_t *data){
    if(write_buffer == NULL){
        printf(ANSI_C_RED "write_through: Write Buffer not initialized\n" ANSI_C_RESET);
        assert(0);
    }
    if(write_buffer->writing){
        printf("\twrite_through: Write Buffer is full\n");
        write_buffer->overflow = true;
        return CACHE_MISS;
    }
    else {
        write_buffer->address = *address;
        write_buffer->data = *data;
        write_buffer->writing = true;
        write_buffer->penalty_count = 0;
        return CACHE_HIT;
    }
}

#endif

/* Write buffer implementation functions */
/* @brief Initializes a new write buffer
*  @returns an instance of a new write buffer depending on what write policy is defined
*/
write_buffer_t *write_buffer_init(void){

    write_buffer_t *wb = (write_buffer_t *)malloc(sizeof(write_buffer_t));
    wb->penalty_count = 0;
    return wb;
}

/* @brief frees the memory reserved for the write buffer
*  @params the write buffer to be destroyed.
*/
void write_buffer_destroy(write_buffer_t *wb){
    free(wb);
}

void write_buffer_digest(void){
    if(get_mem_status() != MEM_WRITING){
        //Its not my turn!!!
        return;
    }

}
