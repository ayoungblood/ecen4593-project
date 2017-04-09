/* src/cache.c
 * Wrapper functions for all cache operations
 */


#include "cache.h"

extern int flags;

direct_cache_t *d_cache;


void cache_init(void){

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

    d_cache = (direct_cache_t *)malloc(sizeof(direct_cache_t));

    direct_cache_block_t *blocks = (direct_cache_block_t *)malloc(sizeof(direct_cache_block_t) * num_blocks);

    d_cache->blocks = blocks;
    //crash if unable to allocate memory
    if(d_cache->blocks == NULL){
        printf(ANSI_C_RED "cache_init: Unable to allocate data cache\n" ANSI_C_RESET);
        assert(0);
    }
    d_cache->num_blocks = num_blocks;
    //get the number of bits the index takes up
    uint32_t index_size = 1;
    while((num_blocks>>index_size) != 0) index_size+=1;
    d_cache->index_size = index_size;
    //32 bit address = Tag size + index size + 2(word aligned)
    d_cache->tag_size = 32 - index_size - 2;

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
    d_cache->index_mask = ((1 << (d_cache->index_size + 2)) - 1);
    d_cache->tag_mask = ~d_cache->index_mask;
    d_cache->index_mask &= ~3;

    //Set up the fetch variables
    d_cache->fetching = false;
    d_cache->penalty_count = 0;
    d_cache->subsequent_fetching = 0;

    //Invalidate all data in the cache
    uint32_t i = 0;
    for(i = 0; i < d_cache->num_blocks; i++){
        d_cache->blocks[i].valid = false;
    }
#endif /* DIRECT_MAPPED */
}


void i_cache_init(void){

}

void cache_destroy(void){
#ifdef DIRECT_MAPPED
    //free the cache
    free(d_cache->blocks);
    d_cache->num_blocks = 0;
    d_cache->tag_size = 0;
    d_cache->tag_mask = 0;
    d_cache->index_size = 0;
    d_cache->index_mask = 0;
    free(d_cache);
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

#ifdef DIRECT_MAPPED
    uint32_t index = 0;
    uint32_t tag = 0;
    direct_cache_get_tag_and_index(&(d_cache->target_address), &index, &tag);

    if(d_cache->fetching){
        //Increment the wait count
        d_cache->penalty_count++;
        if(flags & MASK_DEBUG){
            printf("\tPenalty counter: %d\n", d_cache->penalty_count);
        }
        if((d_cache->penalty_count == CACHE_MISS_PENALTY) && (d_cache->subsequent_fetching == 0)){
            //Finished waiting, get data and return it
            uint32_t temp = 0;
            mem_read_w(d_cache->target_address, &temp);
            if(flags & MASK_DEBUG){
                printf("\tFilling block index %d with data 0x%08x and tag 0x%08x\n", index, temp, tag);
            }
            //fill block line
            d_cache->blocks[index].data = temp;
            d_cache->blocks[index].tag = tag;
            d_cache->blocks[index].valid = true;
            if(flags & MASK_DEBUG){
                direct_cache_block_t block = d_cache->blocks[index];
                printf("\tNEW Block data!\n");
                printf("\tVALID BIT: %d\n\tTAG: 0x%06x\n\tINDEX: %d\n\tDATA: 0x%08x\n",block.valid, block.tag, index, block.data);
            }
            d_cache->fetching = false;
            d_cache->penalty_count = 0;
            if(LINE_FILL != 1){
                d_cache->subsequent_fetching = 1;
                //Start retrieving data from the next address and restart the penalty counter
                d_cache->target_address += 4;
                d_cache->fetching = true;
            }
            return;
        }
        else if((d_cache->penalty_count == CACHE_MISS_SUBSEQUENT_PENALTY) && (d_cache->subsequent_fetching != 0) && (LINE_FILL != 1)){
            //We have data from a block line following a previous cache miss
            uint32_t temp = 0;
            mem_read_w(d_cache->target_address, &temp);
            if(flags & MASK_DEBUG){
                printf("\tFilling block index %d with data 0x%08x and tag 0x%08x\n", index, temp, tag);
            }
            d_cache->blocks[index].data = temp;
            d_cache->blocks[index].tag = tag;
            d_cache->blocks[index].valid = true;
            d_cache->subsequent_fetching++;
            if(d_cache->subsequent_fetching == (LINE_FILL - 1)){
                if(flags & MASK_DEBUG){
                    printf("\tDone retrieving block\n");
                }
                //We have retrieved the whole block. Relenquish the fetching flag
                d_cache->fetching = false;
                d_cache->subsequent_fetching = 0;
                d_cache->penalty_count = 0;
                return;
            }
            else {
                //Start fetching the next address
                d_cache->target_address+=4;
                d_cache->penalty_count = 0;
                if(flags & MASK_DEBUG){
                    printf("\tBeggining subsequent fetch of address 0x%08x\n", d_cache->target_address);
                }
                return;
            }
        }
    }

#endif /* DIRECT_MAPPED */
}




cache_status_t d_cache_get_word(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "D_CACHE GET WORD:\n" ANSI_C_RESET);
    }
    //Get data from the data cache


#ifdef DIRECT_MAPPED
    uint32_t index = 0;
    uint32_t tag = 0;
    direct_cache_get_tag_and_index(address, &index, &tag);

    //Some index checking to make sure we don't seg fault
    if(index > d_cache->num_blocks){
        printf(ANSI_C_RED "d_cache_get_word: Index %d requested out of range\n" ANSI_C_RESET, index);
        assert(0);
    }
    //Make sure memory is initialized
    if(d_cache->blocks == NULL){
        printf(ANSI_C_RED "d_cache_get_word: Cache not initialized\n" ANSI_C_RESET);
        assert(0);
    }

    direct_cache_block_t block = d_cache->blocks[index];
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
        if(d_cache->fetching){
            if(flags & MASK_DEBUG){
                printf("\tCache is busy\n");
            }
            //Not the first request for this address, continue the penalty count
            //Make sure its the same address request from when the stall started
            //Not sure if this will ever happen or if we should crash on this
            //My thought is no, because in a unified memory, we could request an instruction and data from the same cache in the same cycle
            if(*address != d_cache->target_address){
                printf(ANSI_C_RED "d_cache_get_word: RESOURCE IN USE! Cache already retrieving memory at address 0x%08x and cannot retreive memory at address 0x%08x\n" ANSI_C_RESET, d_cache->target_address, *address);
            }
        }
        else {
            //Start the fetch stall
            if(flags & MASK_DEBUG){
                printf("\tCACHE MISS! Starting main memory data retreival for address 0x%08x\n",*address);
            }
            d_cache->fetching = true;
            d_cache->target_address = *address;
            d_cache->penalty_count = 0;
        }
        return CACHE_MISS;
    }

#endif /* DIRECT_MAPPED */



}


#ifdef DIRECT_MAPPED
/* Helper functions specific to the direct mapped cache */
void direct_cache_get_tag_and_index(uint32_t *address, uint32_t *index, uint32_t *tag){
    *index = (*address & d_cache->index_mask) >> 2;
    *tag = (*address & d_cache->tag_mask) >> (2 + d_cache->index_size);
}
#endif /* DIRECT_MAPPED */
