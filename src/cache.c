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

void cache_init(cache_config_t *cpu_cfg){

    set_mem_status(MEM_IDLE);
    if(cpu_cfg->mode == CACHE_DISABLE){
        return;
    } else if(cpu_cfg->mode == CACHE_SPLIT){
        d_cache_init(cpu_cfg);
        i_cache_init(cpu_cfg);
        write_buffer = write_buffer_init();
    } else if(cpu_cfg->mode == CACHE_UNIFIED){
        d_cache_init(cpu_cfg);
        write_buffer = write_buffer_init();
    }

}

void d_cache_init(cache_config_t *cpu_cfg){

    //Check if cache size is a power of two
    if((cpu_cfg->data_size & (cpu_cfg->data_size - 1)) != 0) {
        cprintf(ANSI_C_RED, "cache_init: D_CACHE_SIZE %d not a power of two\n", cpu_cfg->data_size);
        assert(0);
    }

    if(flags & MASK_DEBUG){
        printf("Creating Data Cache (D Cache)\n");
    }
    //Each block contains a word of data
    uint32_t num_blocks = cpu_cfg->data_size >> 2;
    d_cache = direct_cache_init(num_blocks, cpu_cfg->data_block);
}

void i_cache_init(cache_config_t *cpu_cfg){
    //chech to make sure instruction cache size is a power of two
    if((cpu_cfg->inst_size & (cpu_cfg->inst_size - 1)) != 0) {
        cprintf(ANSI_C_RED, "cache_init: I_CACHE_SIZE %d not a power of two\n", cpu_cfg->inst_size);
        assert(0);
    }
    if(flags & MASK_DEBUG){
        printf("Creating Instruction Cache (I Cache)\n");
    }
    uint32_t num_blocks = cpu_cfg->inst_size >> 2;
    i_cache = direct_cache_init(num_blocks, cpu_cfg->inst_block);
}


void cache_destroy(void){

    direct_cache_free(d_cache);

    direct_cache_free(i_cache);
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
        cprintf(ANSI_C_CYAN, "CACHE DIGEST:\n");
    }

    if(d_cache == NULL){
        cprintf(ANSI_C_RED, "cache_digest: data cache is not initialized\n");
        assert(0);
    }
    if(i_cache == NULL){
        cprintf(ANSI_C_RED, "cache_digest: instruction cache is not initialized\n");
        assert(0);
    }
    if(write_buffer == NULL){
        cprintf(ANSI_C_RED, "cache_digest: write buffer is not initialized\n");
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
            cprintf(ANSI_C_RED, "cache_digest: Undefined Memory State %d\n", get_mem_status());
            assert(0);
            break;
    }
    if(flags & MASK_DEBUG){
        printf("\tcache_digest: Memory state is ");
        switch(get_mem_status()){
            case MEM_IDLE:
                printf("MEM_IDLE\n");
                break;
            case MEM_WRITING:
                printf("MEM_WRITING\n");
                break;
            case MEM_READING_D:
                printf("MEM_READING_D\n");
                break;
            case MEM_READING_I:
                printf("MEM_READING_I\n");
                break;
            default:
                printf("UNDEFINED\n");
                break;
        }
    }

    direct_cache_digest(d_cache, MEM_READING_D);
    direct_cache_digest(i_cache, MEM_READING_I);
    write_buffer_digest();

    //print_cache(i_cache);
}




cache_status_t d_cache_read_w(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        cprintf(ANSI_C_CYAN, "D_CACHE GET WORD:\n");
    }
    //Get data from the data cache
    cache_status_t status = direct_cache_read_w(d_cache, address, data);
    return status;
}


cache_status_t d_cache_write_w(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        cprintf(ANSI_C_CYAN, "D_CACHE WRITE WORD:\n");
    }

    cache_status_t status = direct_cache_write_w(d_cache, address, data);
    return status;
}

cache_status_t i_cache_read_w(uint32_t *address, word_t *data){
    if(flags & MASK_DEBUG){
        cprintf(ANSI_C_CYAN, "I_CACHE GET WORD:\n");
    }
    //Get data from the data cache

    cache_status_t status = direct_cache_read_w(i_cache, address, data);

    return status;
}


/* Write buffer implementation functions */
/* @brief Initializes a new write buffer
*  @returns an instance of a new write buffer depending on what write policy is defined
*/
write_buffer_t *write_buffer_init(void){

    write_buffer_t *wb = (write_buffer_t *)malloc(sizeof(write_buffer_t));
    wb->penalty_count = 0;
    wb->data = (word_t *)malloc(sizeof(word_t)*d_cache->block_size);
    return wb;
}

/* @brief frees the memory reserved for the write buffer
*  @params the write buffer to be destroyed.
*/
void write_buffer_destroy(write_buffer_t *wb){
    free(wb->data);
    free(wb);
}

void write_buffer_digest(void){
    if(write_buffer->writing){
        if(get_mem_status() != MEM_WRITING){
            //Its not my turn!!!
            return;
        }
        else {
            write_buffer->penalty_count++;
            if(write_buffer->penalty_count == CACHE_WRITE_PENALTY){
                mem_write_w(write_buffer->address, &write_buffer->data[write_buffer->subsequent_writing]);
                write_buffer->writing = false;
                write_buffer->penalty_count = 0;
                if(write_buffer->subsequent_writing != (d_cache->block_size - 1)){
                    //enqueue the next data address
                    write_buffer->address+=4;
                    write_buffer->writing = true;
                    write_buffer->penalty_count = 0;
                    write_buffer->subsequent_writing = 1;
                }
            }
            else if(write_buffer->subsequent_writing && write_buffer->penalty_count == CACHE_WRITE_SUBSEQUENT_PENALTY){
                mem_write_w(write_buffer->address, &write_buffer->data[write_buffer->subsequent_writing]);
                write_buffer->writing = false;
                write_buffer->penalty_count = 0;
                if(write_buffer->subsequent_writing != (d_cache->block_size - 1)){
                    write_buffer->address += 4;
                    write_buffer->writing = true;
                    write_buffer->subsequent_writing++;
                    write_buffer->penalty_count = 0;
                }
            }
        }
    }
}

cache_status_t write_buffer_enqueue(cache_access_t info){
    if(write_buffer == NULL){
        cprintf(ANSI_C_RED, "write_buffer_enqueue: buffer is not initialized\n");
        assert(0);
    }
    if(write_buffer->writing){
        //buffer is full!!
        if(flags & MASK_DEBUG){
            printf("\twrite_buffer_enqueue: Write buffer is full!\n");
        }
        return CACHE_MISS;
    }
    else {
        if(flags & MASK_DEBUG){
            printf("\twrite_buffer_enqueue: filling write buffer with block index %d and tag 0x%08x\n", info.index, info.tag);
        }
        write_buffer->address = info.tag | info.index;
        uint8_t i = 0;
        for(i = 0; i < d_cache->block_size; i++){
            write_buffer->data[i] = d_cache->blocks[info.index].data[i];
        }
        write_buffer->writing = true;
        write_buffer->penalty_count = 0;
        write_buffer->subsequent_writing = 0;
        return CACHE_HIT;
    }
}


void print_cache(void *cache){
    direct_cache_print((direct_cache_t*) cache);
}
