/* src/cache.c
 * Wrapper functions for all cache operations
 */


#include "cache.h"

extern int flags;


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
    if((D_CACHE_SIZE & (D_CACHE_SIZE - 1)) == 0) {
        printf(ANSI_C_RED "cache_init: D_CACHE_SIZE %d not a power of two\n" ANSI_C_RESET, D_CACHE_SIZE);
        assert(0);
    }

#ifdef DIRECT_MAPPED
    //Each block contains a word of data
    uint32_t num_blocks = D_CACHE_SIZE >> 2;

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
    free(d_cache->blocks);//helper functions do not call directly
    d_cache->num_blocks = 0;
#endif /* DIRECT_MAPPED */
    return;
}

cache_status_t d_cache_get_word(uint32_t *address, word_t *data){
    //Get data from the data cache
#ifdef DIRECT_MAPPED
    uint32_t index = (*address & d_cache->index_mask) >> 2;
    uint32_t tag = (*address & d_cache->tag_mask) >> (2 + d_cache->index_size);

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
    if(block.valid && (tag == block.tag)){
        if(flags & MASK_DEBUG){
            printf("\tFound valid data 0x%08x for address 0x%08x in d cache\n", block.data, *address);
        }
        *data = block.data;
        return CACHE_HIT;
    } else {

        return CACHE_MISS;
    }

#endif /* DIRECT_MAPPED */
}
