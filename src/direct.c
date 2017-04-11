/*
* src/direct.c
* implementation of a direct mapped cache
*/

#include "direct.h"


extern int flags;

direct_cache_t * direct_cache_init(uint32_t num_blocks, uint32_t block_size){

    direct_cache_t *cache = (direct_cache_t *)malloc(sizeof(direct_cache_t));
    direct_cache_block_t *blocks = (direct_cache_block_t *)malloc(sizeof(direct_cache_block_t) * num_blocks);

    cache->blocks = blocks;
    //crash if unable to allocate memory
    if(cache == NULL || cache->blocks == NULL){
        printf(ANSI_C_RED "cache_init: Unable to allocate direct mapped cache\n" ANSI_C_RESET);
        assert(0);
    }
    cache->num_blocks = num_blocks;
    cache->block_size = block_size;

    //get the number of bits the index takes up
    uint32_t index_size = 1;
    while((num_blocks>>index_size) != 0) index_size+=1;
    cache->index_size = index_size;
    //cache size is 2^n blocks, so n bits are needed for the index
    //block size is 2^m words, m bits needed for word within block
    //tag bits = 32 - (n + m + 2)
    if(cache->block_size == 4){
        //two bits are needed to access the word within the block
        cache->inner_index_size = 2;
        cache->inner_index_mask = 0xc;
    } else if(cache->block_size == 1){
        cache->inner_index_size = 0;
        cache->inner_index_mask = 0;
    } else {
        printf(ANSI_C_RED "direct_cache_init: unsupported block size\n" ANSI_C_RESET);
        assert(0);
    }
    cache->tag_size = 32 - cache->index_size - cache->inner_index_size - 2;
    cache->index_mask = ((1 << (cache->index_size + cache->inner_index_size + 2)) - 1);
    cache->tag_mask = ~cache->index_mask;
    cache->index_mask &= ~(cache->inner_index_mask | 0x3);

    if(flags & MASK_DEBUG){
        printf("tag_mask: 0x%08x\n", cache->tag_mask);
        printf("index_mask: 0x%08x\n", cache->index_mask);
        printf("inner_index_mask: 0x%08x\n", cache->inner_index_mask);
    }

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
            if(cache->inner_index_size != 1){
                cache->subsequent_fetching = 1;
                //Start retrieving data from the next address and restart the penalty counter
                cache->target_address += 4;
                cache->fetching = true;
            }
            return;
        }
        else if((cache->penalty_count == CACHE_MISS_SUBSEQUENT_PENALTY) && (cache->subsequent_fetching != 0) && (cache->inner_index_size != 1)){
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
            if(cache->subsequent_fetching == (cache->inner_index_size - 1)){
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


void direct_cache_get_tag_and_index(direct_cache_t *cache, uint32_t *address, uint32_t *index, uint32_t *tag){
    *index = (*address & cache->index_mask) >> 2;
    *tag = (*address & cache->tag_mask) >> (2 + cache->index_size);
}
