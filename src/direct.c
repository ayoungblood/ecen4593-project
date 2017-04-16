/*
* src/direct.c
* implementation of a direct mapped cache
*/

#include "direct.h"


extern int flags;

direct_cache_t * direct_cache_init(uint32_t num_blocks, uint32_t block_size){
    //The linear memory that the cache blocks point to
    word_t *words = (word_t *)malloc(sizeof(word_t)*num_blocks*block_size);
    //The cache struct itself
    direct_cache_t *cache = (direct_cache_t *)malloc(sizeof(direct_cache_t));
    //Each block that contains tag info, dirty and valid bits, etc
    direct_cache_block_t *blocks = (direct_cache_block_t *)malloc(sizeof(direct_cache_block_t) * num_blocks);

    //set up each block to point to its corresponding word in memory
    uint32_t i = 0;
    for(i = 0; i < num_blocks; i++){
        blocks[i].data = words + i * block_size;
    }

    cache->blocks = blocks;
    cache->words = words;
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
    //This is magic. Look in the header file for some sort of explanation
    uint32_t inner_index_size = 1;
    while((block_size>>inner_index_size) != 0) inner_index_size+=1;
    cache->inner_index_size = inner_index_size;
    cache->inner_index_mask = ((1 << (inner_index_size + 1)) - 1) & ~3;
    cache->tag_size = 32 - cache->index_size - cache->inner_index_size - 2;
    cache->index_mask = ((1 << (cache->index_size + cache->inner_index_size + 2)) - 1);
    cache->tag_mask = ~cache->index_mask;
    cache->index_mask &= ~(cache->inner_index_mask | 0x3);

    if(flags & MASK_DEBUG){
        printf("creating cache masks...\n");
        printf("tag_mask: 0x%08x\n", cache->tag_mask);
        printf("index_mask: 0x%08x\n", cache->index_mask);
        printf("inner_index_mask: 0x%08x\n", cache->inner_index_mask);
    }

    //Set up the fetch variables
    cache->fetching = false;
    cache->penalty_count = 0;
    cache->subsequent_fetching = 0;

    //Invalidate all data in the cache
    for(i = 0; i < cache->num_blocks; i++){
        cache->blocks[i].valid = false;
    }
    return cache;
}

void direct_cache_free(direct_cache_t *cache){

    free(cache->words);
    free(cache->blocks);
    free(cache);
}


void direct_cache_digest(direct_cache_t *cache, memory_status_t proceed_condition){
    cache_access_t info;
    direct_cache_get_tag_and_index(&info, cache, &(cache->target_address));
    if(get_mem_status() == proceed_condition){
        //Increment the wait count
        cache->penalty_count++;
        if(flags & MASK_DEBUG){
            printf("\tdirect_cache_digest: Value of incremented penalty_count %d\n",cache->penalty_count);
        }
        if(cache->penalty_count == CACHE_MISS_PENALTY){
            //Finished waiting, get data and return it
            if(flags & MASK_DEBUG){
                printf("\tdirect_cache_digest: Reached stall count retreiveing data.\n");
            }
            direct_cache_fill_word(cache, info);
            cache->fetching = false;
            cache->penalty_count = 0;
            if(cache->subsequent_fetching != (cache->block_size - 1)){
                //get the second word in the block
                info.address |= (1 << 2);
                cache->subsequent_fetching = 1;
                direct_cache_queue_mem_access(cache, info);
            }
            return;
        }
        if(cache->subsequent_fetching && (cache->penalty_count == CACHE_MISS_SUBSEQUENT_PENALTY)){
            //Have the next word for the block
            direct_cache_fill_word(cache, info);
            cache->fetching = false;
            cache->penalty_count = 0;
            if(cache->subsequent_fetching != (cache->block_size - 1)){
                //get the next word for the block
                cache->subsequent_fetching++;
                info.address |= (cache->subsequent_fetching << 2);
                direct_cache_queue_mem_access(cache, info);
            }
            return;
        }
    }

}


cache_status_t direct_cache_read_word(direct_cache_t *cache, uint32_t *address, uint32_t *data){
    cache_access_t info;
    direct_cache_get_tag_and_index(&info, cache, address);
    info.request = CACHE_READ;

    //Some index checking to make sure we don't seg fault
    if(info.index >= cache->num_blocks){
        printf(ANSI_C_RED "direct_cache_read_word: Index %d requested out of range\n" ANSI_C_RESET, info.index);
        assert(0);
    }
    //Make sure memory is initialized
    if(cache->blocks == NULL){
        printf(ANSI_C_RED "direct_cache_read_word: Cache not initialized\n" ANSI_C_RESET);
        assert(0);
    }

    cache_status_t status = direct_cache_access_word(cache, &info);
    if(status == CACHE_HIT){
        *data = info.data;
        if(flags & MASK_DEBUG){
            printf("\tdirect_cache_read_word: CACHE_HIT, found data 0x%08x at address 0x%08x\n", info.data, info.address);
        }
        return CACHE_HIT;
    } else {
        if(cache->fetching){
            if(flags & MASK_DEBUG){
                printf("\tdirect_cache_read_word: CACHE_MISS, cache is fetching data.\n");
            }
            return CACHE_MISS;
        } else {
            //Data is not in the cache. Start retrieval
            if(flags & MASK_DEBUG){
                printf("\tdirect_cache_read_word: CACHE_MISS, data is not in the cache. Queueing read\n");
            }
            info.request = CACHE_READ;
            direct_cache_queue_mem_access(cache, info);
            return CACHE_MISS;
        }
    }
}

cache_status_t direct_cache_write_w(direct_cache_t *cache, uint32_t *address, uint32_t *data){
    cache_access_t info;
    direct_cache_get_tag_and_index(&info, cache, address);
    info.request = CACHE_WRITE;
    info.fromMem = false;
    info.data = *data;
    cache_status_t status = direct_cache_access_word(cache, &info);
    if(flags & MASK_DEBUG){
        if(status == CACHE_HIT){
            printf("\tdirect_cache_write_w: successfully wrote to cache\n");
        } else if(status == CACHE_MISS){
            printf("\tdirect_cache_write_w: unsuccessful write. \n");
        }
    }
    return status;
}


void direct_cache_fill_word(direct_cache_t *cache, cache_access_t info){
    //Some preventative measures to make sure a block contains data from the same tag
    if(info.inner_index != 0){
        //make sure the current tag matches the tag thats already in the block
        if(info.tag != cache->blocks[info.index].tag){
            printf(ANSI_C_RED "direct_cache_fill_word: Inconsistent Tag Data. Aborting\n" ANSI_C_RESET);
            assert(0);
        }
        //make sure the first word in the block is valid
        if(cache->blocks[info.index].valid == false){
            printf(ANSI_C_RED "direct_cache_fill_word: First word in block is not valid. Aborting\n"ANSI_C_RESET);
            assert(0);
        }
    }
    mem_read_w(info.address, &(info.data));
    info.request = CACHE_WRITE;
    info.fromMem = true;
    direct_cache_access_word(cache, &info);

}

cache_status_t direct_cache_access_word(direct_cache_t *cache, cache_access_t *info){
    //Helper function for getting data in and out of a cache block
    if(info->request == CACHE_READ){
        if(flags & MASK_DEBUG){
            printf("\tdirect_cache_access_word: Reading from cache block %d\n", info->index);
        }
        //Check to make sure the data is valid
        if(cache->blocks[info->index].valid == true && cache->blocks[info->index].tag == info->tag){
            info->data = cache->blocks[info->index].data[info->inner_index];
            info->dirty = cache->blocks[info->index].dirty;
            if(flags & MASK_DEBUG){
                printf("\tdirect_cache_access_word: Found valid data 0x%08x for address 0x%08x\n", info->data, info->address);
                if(info->dirty){
                    printf("\tdirect_cache_access_word: Data is dirty\n");
                }
            }
            return CACHE_HIT;
        }
        else {
            if(flags & MASK_DEBUG){
                printf("\tdirect_cache_access_word: Data at requested address is not in the cache\n");
            }
            return CACHE_MISS;
        }
    } else if(info->request == CACHE_WRITE){
        //read the data currently in that block
        if(info->fromMem == false){
            cache_access_t *read_info = info;
            read_info->request = CACHE_READ;
            if(flags & MASK_DEBUG){
                printf("\tdirect_cache_access_word: Attempting read before write\n");
            }
            cache_status_t status = direct_cache_access_word(cache, read_info);
            if(status == CACHE_MISS){
                //The processor is writing to a place in memory that isnt in the cache
                //The transaction becomes a READ MODIFY WRITE
                if(flags & MASK_DEBUG){
                    printf("\tdirect_cache_access_word: no valid data in the cache for the specified address.\n");
                }
                return CACHE_MISS;
            }
            else if(status == CACHE_HIT){
                if(read_info->dirty && write_policy == WRITEBACK){
                    //There is valid dirty data in the cache, and we must put it in the write buffer
                    status = write_buffer_enqueue(*info);
                    if(status == CACHE_MISS){
                        if(flags & MASK_DEBUG){
                            printf("\tdirect_cache_access_word: Write buffer is full. Cannot fill cache without losing data.\n");
                        }
                        //The write buffer is full! Don't fill the block
                        return CACHE_MISS;
                    }
                }
            }
        }
        if(flags & MASK_DEBUG){
            printf("\tdirect_cache_access_word: writing 0x%08x to block index %d\n", info->data, info->index);
        }
        //update the data

        cache->blocks[info->index].data[info->inner_index] = info->data;
        cache->blocks[info->index].tag = info->tag;
        cache->blocks[info->index].valid = true;
        cache->blocks[info->index].dirty = info->fromMem ? false : true;
        return CACHE_HIT;
    }
    else {
        printf(ANSI_C_RED "direct_cache_access_word: unimplemented cache access request\n"ANSI_C_RESET);
        return CACHE_MISS;
    }
}



void direct_cache_queue_mem_access(direct_cache_t *cache, cache_access_t info){
    if(flags & MASK_DEBUG){
        printf("\tdirect_cache_queue_mem_access: Queueing memory access for address 0x%08x\n", info.address);
    }
    cache->fetching = true;
    if(cache->subsequent_fetching == 0){
        //We must get the first word in a block first
        cache->target_address = info.address & (cache->tag_mask | cache->index_mask);
    }
    else {
        cache->target_address = info.address;
    }
    cache->penalty_count = 0;
    if(flags & MASK_DEBUG && cache->block_size > 1){
        printf("\tdirect_cache_queue_mem_access: Actual requested address will be 0x%08x\n", cache->target_address);
    }
}

void direct_cache_get_tag_and_index(cache_access_t *info, direct_cache_t *cache, uint32_t *address){
    info->index = (*address & cache->index_mask) >> (2 + cache->inner_index_size);
    info->tag = (*address & cache->tag_mask) >> (2 + cache->index_size + cache->inner_index_size);
    info->inner_index = (*address & cache->inner_index_mask) >> 2;
    info->address = *address;
}
