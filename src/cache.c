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

    #ifdef UNIFIED
    //The d cache will be the only cache that is used
    d_cache_init();
    #else
    //separate instruction and data caches
    d_cache_init();
    i_cache_init();
    #endif /* UNIFIED */

}

void d_cache_init(void){

    //Check if cache size is a power of two
    if((D_CACHE_SIZE & (D_CACHE_SIZE - 1)) != 0) {
        printf(ANSI_C_RED "cache_init: D_CACHE_SIZE %d not a power of two\n" ANSI_C_RESET, D_CACHE_SIZE);
        assert(0);
    }

    #ifdef DIRECT_MAPPED
    //Each block contains a word of data
    uint32_t num_blocks = D_CACHE_SIZE >> 2;
    d_cache = direct_cache_init(num_blocks);
    #endif /* DIRECT_MAPPED */
}

#ifndef UNIFIED
void i_cache_init(void){
    //chech to make sure instruction cache size is a power of two
    if((I_CACHE_SIZE & (I_CACHE_SIZE - 1)) != 0) {
        printf(ANSI_C_RED "cache_init: I_CACHE_SIZE %d not a power of two\n" ANSI_C_RESET, D_CACHE_SIZE);
        assert(0);
    }

    #ifdef DIRECT_MAPPED
    uint32_t num_blocks = I_CACHE_SIZE >> 2;
    i_cache = direct_cache_init(num_blocks);
    #endif /* DIRECT_MAPPED */
}
#endif /* UNIFIED */


void cache_destroy(void){
    #ifdef DIRECT_MAPPED
    //free the cache
    free(d_cache->blocks);
    free(d_cache);

    #ifndef UNIFIED
    free(i_cache->blocks);
    free(i_cache);
    #endif /* UNIFIED */

    #endif /* DIRECT_MAPPED */
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
            #ifndef UNIFIED
            else if(i_cache->fetching){
                set_mem_status(MEM_READING_I);
            }
            #endif /*UNIFIED*/
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
            #ifndef UNIFIED
            else if(i_cache->fetching){
                //Now instruction cache is reading
                set_mem_status(MEM_READING_I);
            }
            #endif /*UNIFIED*/
            else if(write_buffer->writing){
                //writing from data cache to memory
                set_mem_status(MEM_WRITING);
            }
            else {
                set_mem_status(MEM_IDLE);
            }
            break;
        #ifndef UNIFIED
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
        #endif /*UNIFIED*/
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
            #ifndef UNIFIED
            else if(i_cache->fetching){
                //Now reading into instruction cache
                set_mem_status(MEM_READING_I);
            }
            #endif /*UNIFIED*/
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

    #ifdef DIRECT_MAPPED
    direct_cache_digest(d_cache, MEM_READING_D);
    #ifndef UNIFIED
    direct_cache_digest(i_cache, MEM_READING_I);
    #endif /* UNIFIED */
    #endif /* DIRECT_MAPPED */
}




cache_status_t d_cache_read_w(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "D_CACHE GET WORD:\n" ANSI_C_RESET);
    }
    //Get data from the data cache
    #ifdef DIRECT_MAPPED
    cache_status_t status = direct_cache_get_word(d_cache, address, data);
    #endif /* DIRECT_MAPPED */
    return status;
}

cache_status_t d_cache_read_h(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "D_CACHE GET HALFWORD:\n" ANSI_C_RESET);
    }
    //Get data from the data cache
    #ifdef DIRECT_MAPPED
    cache_status_t status = direct_cache_get_word(d_cache, address, data);
    #endif /* DIRECT_MAPPED */

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

    #ifdef DIRECT_MAPPED
    cache_status_t status = direct_cache_get_word(d_cache, address, data);
    #endif /* DIRECT_MAPPED */

    uint32_t shift = ((3-(*address & 0x3))<<3); // shift amount based on byte position
    *data >>= shift;
    *data &= 0xff;
    return status;
}

cache_status_t d_cache_write_w(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "D_CACHE WRITE WORD:\n" ANSI_C_RESET);
    }

    #ifdef WRITETHROUGH
    cache_status_t status = write_through(address, data);
    #endif
    return status;
}

#ifndef UNIFIED
cache_status_t i_cache_read_w(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "I_CACHE GET WORD:\n" ANSI_C_RESET);
    }
    //Get data from the data cache
    #ifdef DIRECT_MAPPED
    cache_status_t status = direct_cache_get_word(i_cache, address, data);
    #endif /* DIRECT_MAPPED */
    return status;
}
#endif /* UNIFIED */


/* END OF THE WRAPPER FUNCTIONS FOR THE CACHES.
* Below are the implementations of the two different cache architectures
*/


#ifdef DIRECT_MAPPED
/*
* direct_cache_t * direct_cache_init(uint32_t num_blocks)
* Creates an instance of a direct mapped cache with the number
* of blocks as a parameter. From the number of blocks we can determine
* dynamically the tag size. This function also initializes all of the
* blocks to have invalid data and sets up bitmasks to easily obtain
* index and tags from an address.
*/
direct_cache_t * direct_cache_init(uint32_t num_blocks){

    direct_cache_t *cache = (direct_cache_t *)malloc(sizeof(direct_cache_t));
    direct_cache_block_t *blocks = (direct_cache_block_t *)malloc(sizeof(direct_cache_block_t) * num_blocks);

    cache->blocks = blocks;
    //crash if unable to allocate memory
    if(cache->blocks == NULL){
        printf(ANSI_C_RED "cache_init: Unable to allocate direct mapped cache\n" ANSI_C_RESET);
        assert(0);
    }
    cache->num_blocks = num_blocks;
    //get the number of bits the index takes up
    uint32_t index_size = 1;
    while((num_blocks>>index_size) != 0) index_size+=1;
    cache->index_size = index_size;
    //32 bit address = Tag size + index size + 2(word aligned)
    cache->tag_size = 32 - index_size - 2;

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
    cache->index_mask = ((1 << (cache->index_size + 2)) - 1);
    cache->tag_mask = ~cache->index_mask;
    cache->index_mask &= ~3;

    //Set up the fetch variables
    cache->fetching = false;
    cache->penalty_count = 0;
    cache->subsequent_fetching = 0;

    //Invalidate all data in the cache
    uint32_t i = 0;
    for(i = 0; i < cache->num_blocks; i++){
        cache->blocks[i].valid = false;
    }
    return cache;
}

/*
* void direct_cache_digest(direct_cache_t *cache, memory_status_t proceed_condition)
* function to be called every cycle of the clock.
* No advancement on stall counters will occur if the memory state does not
* match the given proceed condition
* @params *cache is which cache the digest will operate on
* @params proceed_condition is a memory state to ensure a read doesn't proceed
*         if there is another memory operation occuring
*/
void direct_cache_digest(direct_cache_t *cache, memory_status_t proceed_condition){
    uint32_t index = 0;
    uint32_t tag = 0;
    direct_cache_get_tag_and_index(cache, &(cache->target_address), &index, &tag);
    if(get_mem_status() == proceed_condition){
        //Increment the wait count
        cache->penalty_count++;
        if(flags & MASK_DEBUG){
            printf("\tPenalty counter: %d\n", cache->penalty_count);
        }
        if((cache->penalty_count == CACHE_MISS_PENALTY) && (cache->subsequent_fetching == 0)){
            //Finished waiting, get data and return it
            uint32_t temp = 0;
            mem_read_w(cache->target_address, &temp);
            if(flags & MASK_DEBUG){
                printf("\tFilling block index %d with data 0x%08x and tag 0x%08x\n", index, temp, tag);
            }
            //fill block line
            cache->blocks[index].data = temp;
            cache->blocks[index].tag = tag;
            cache->blocks[index].valid = true;
            if(flags & MASK_DEBUG){
                direct_cache_block_t block = cache->blocks[index];
                printf("\tNEW Block data!\n");
                printf("\tVALID BIT: %d\n\tTAG: 0x%06x\n\tINDEX: %d\n\tDATA: 0x%08x\n",block.valid, block.tag, index, block.data);
            }
            cache->fetching = false;
            cache->penalty_count = 0;
            if(LINE_FILL != 1){
                cache->subsequent_fetching = 1;
                //Start retrieving data from the next address and restart the penalty counter
                cache->target_address += 4;
                cache->fetching = true;
            }
            return;
        }
        else if((cache->penalty_count == CACHE_MISS_SUBSEQUENT_PENALTY) && (cache->subsequent_fetching != 0) && (LINE_FILL != 1)){
            //We have data from a block line following a previous cache miss
            uint32_t temp = 0;
            mem_read_w(cache->target_address, &temp);
            if(flags & MASK_DEBUG){
                printf("\tFilling block index %d with data 0x%08x and tag 0x%08x\n", index, temp, tag);
            }
            cache->blocks[index].data = temp;
            cache->blocks[index].tag = tag;
            cache->blocks[index].valid = true;
            cache->subsequent_fetching++;
            if(cache->subsequent_fetching == (LINE_FILL - 1)){
                if(flags & MASK_DEBUG){
                    printf("\tDone retrieving block\n");
                }
                //We have retrieved the whole block. Relenquish the fetching flag
                cache->fetching = false;
                cache->subsequent_fetching = 0;
                cache->penalty_count = 0;
                return;
            }
            else {
                //Start fetching the next address
                cache->target_address+=4;
                cache->penalty_count = 0;
                if(flags & MASK_DEBUG){
                    printf("\tBeggining subsequent fetch of address 0x%08x\n", cache->target_address);
                }
                return;
            }
        }
    }

}

/* cache_status_t direct_cache_get_word(direct_cache_t *cache, uint32_t *address, uint32_t *data)
* returns CACHE_HIT or CACHE_MISS depending on if the data is available in the cache
* if there is a CACHE_MISS, function will set up the direct mapped cache to
* start fetching the data from main memory.
*/
cache_status_t direct_cache_get_word(direct_cache_t *cache, uint32_t *address, uint32_t *data){
    uint32_t index = 0;
    uint32_t tag = 0;
    direct_cache_get_tag_and_index(cache, address, &index, &tag);

    //Some index checking to make sure we don't seg fault
    if(index >= cache->num_blocks){
        printf(ANSI_C_RED "direct_cache_get_word: Index %d requested out of range\n" ANSI_C_RESET, index);
        assert(0);
    }
    //Make sure memory is initialized
    if(cache->blocks == NULL){
        printf(ANSI_C_RED "direct_cache_get_word: Cache not initialized\n" ANSI_C_RESET);
        assert(0);
    }

    direct_cache_block_t block = cache->blocks[index];
    if(flags & MASK_DEBUG){
        printf("\tCurrent data in block at requested index\n");
        printf("\tVALID BIT: %d \n\tTAG: 0x%06x\n\tINDEX: %d\n\tDATA: 0x%08x\n",block.valid, block.tag, index, block.data);
    }
    if(block.valid && (tag == block.tag)){
        if(flags & MASK_DEBUG){
            printf("\tCACHE HIT!\n");
            printf("\tFound valid data 0x%08x for address 0x%08x in d cache\n", block.data, *address);
        }
        *data = block.data;
        return CACHE_HIT;
    } else {
        //Data is not in the cache. Either start the retrieval, wait, or fill depending on the stall count
        if(cache->fetching){
            if(flags & MASK_DEBUG){
                printf("\tCache is busy\n");
            }
            //Not the first request for this address, continue the penalty count
            //Make sure its the same address request from when the stall started
            //Not sure if this will ever happen or if we should crash on this
            //My thought is no, because in a unified memory, we could request an instruction and data from the same cache in the same cycle
            if(*address != cache->target_address){
                printf(ANSI_C_RED "d_cache_get_word: RESOURCE IN USE! Cache already retrieving memory at address 0x%08x and cannot retreive memory at address 0x%08x\n" ANSI_C_RESET, cache->target_address, *address);
            }
        }
        else {
            //Start the fetch stall
            if(flags & MASK_DEBUG){
                printf("\tCACHE MISS! Starting main memory data retreival for address 0x%08x\n",*address);
            }
            cache->fetching = true;
            cache->target_address = *address;
            cache->penalty_count = 0;
        }
        return CACHE_MISS;
    }
}

/* Helper functions specific to the direct mapped cache */
void direct_cache_get_tag_and_index(direct_cache_t *cache, uint32_t *address, uint32_t *index, uint32_t *tag){
    *index = (*address & cache->index_mask) >> 2;
    *tag = (*address & cache->tag_mask) >> (2 + cache->index_size);
}

#endif /* DIRECT_MAPPED */

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
