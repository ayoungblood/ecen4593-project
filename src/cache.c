/* src/cache.c
* Wrapper functions for all cache operations
*/

#include "cache.h"

extern int flags; // from util.c
extern profile_t *prof;

direct_cache_t *d_cache;
direct_cache_t *i_cache;
write_buffer_t *write_buffer;
memory_status_t memory_status = MEM_IDLE;

cache_config_t *config;

memory_status_t get_mem_status(void){
    return memory_status;
}
void set_mem_status(memory_status_t status){
    memory_status = status;
}

void cache_init(cache_config_t *cpu_cfg){
    config = (cache_config_t*)malloc(sizeof(cache_config_t));
    memcpy(config, cpu_cfg, sizeof(cache_config_t));
    set_mem_status(MEM_IDLE);
    if(config->mode == CACHE_DISABLE){
        return;
    } else if(config->mode == CACHE_SPLIT){
        d_cache_init(config);
        i_cache_init(config);
        write_buffer = write_buffer_init();
    } else if(config->mode == CACHE_UNIFIED){
        d_cache_init(config);
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
    uint32_t num_blocks = (cpu_cfg->data_size >> 2) / cpu_cfg->data_block;
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
    uint32_t num_blocks = (cpu_cfg->inst_size >> 2) / cpu_cfg->inst_block;
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
    gcprintf(ANSI_C_CYAN, "CACHE DIGEST:\n");
    if (d_cache == NULL) {
        cprintf(ANSI_C_RED, "cache_digest: data cache is not initialized\n", NULL);
        assert(0);
    }
    if (i_cache == NULL) {
        cprintf(ANSI_C_RED, "cache_digest: instruction cache is not initialized\n", NULL);
        assert(0);
    }
    if (write_buffer == NULL) {
        cprintf(ANSI_C_RED, "cache_digest: write buffer is not initialized\n", NULL);
        assert(0);
    }

    // State machine to ensure we do not have more than one memory access at a time
    // This is a state that is consistent between all
    switch (get_mem_status()) {
        case MEM_IDLE:
            // Ready to accept new memory accesses
            // Check for data cache read requests
            if (d_cache->fetching) {
                set_mem_status(MEM_READING_D);
            } else if(i_cache->fetching) {
                set_mem_status(MEM_READING_I);
            } else if(write_buffer->writing) {
                set_mem_status(MEM_WRITING);
            }
            break;
        case MEM_READING_D:
            // Last digest cycle, we were reading into data cache. See if still reading
            if (d_cache->fetching) {
                // Still reading, no state change
                break;
            } else if (i_cache->fetching) {
                // Now instruction cache is reading
                set_mem_status(MEM_READING_I);
            } else if(write_buffer->writing) {
                // Writing from data cache to memory
                set_mem_status(MEM_WRITING);
            } else {
                set_mem_status(MEM_IDLE);
            }
            break;
        case MEM_READING_I:
            // Last cycle we were reading into instruction cache
            if (i_cache->fetching) {
                // Still reading into I cache
                break;
            } else if (d_cache->fetching) {
                // Now we are reading into D cache
                set_mem_status(MEM_READING_D);
            } else if (write_buffer->writing) {
                // Now we are writing into memory from D cache
                set_mem_status(MEM_WRITING);
            } else {
                // Nothing to do
                set_mem_status(MEM_IDLE);
            }
            break;
        case MEM_WRITING:
            // Last cycle we were writing to memory
            if (write_buffer->writing) {
                // Still writing
                break;
            } else if (d_cache->fetching) {
                // Now reading into D cache
                set_mem_status(MEM_READING_D);
            } else if (i_cache->fetching) {
                //Now reading into I cache
                set_mem_status(MEM_READING_I);
            } else {
                // Nothing to do
                set_mem_status(MEM_IDLE);
            }
            break;
        default:
            cprintf(ANSI_C_RED, "cache_digest: Undefined Memory State %d\n", get_mem_status());
            assert(0);
            break;
    }
    if (flags & MASK_DEBUG) {
        printf("\tcache_digest: Memory state is ");
        switch (get_mem_status()) {
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
    // Get data from the D cache
    cache_status_t status = direct_cache_read_w(d_cache, address, data);
    return status;
}

cache_status_t d_cache_write_w(uint32_t *address, word_t *data){
    // Write data to the D cache
    cache_status_t status = direct_cache_write_w(d_cache, address, data);
    return status;
}

cache_status_t i_cache_read_w(uint32_t *address, word_t *data){
    // Get data from the I cache
    cache_status_t status = direct_cache_read_w(i_cache, address, data);
    return status;
}

cache_wpolicy_t get_write_policy(void){
    if (config->mode ==CACHE_UNIFIED) {
        return config->wpolicy;
    } else {
        return config->data_wpolicy;
    }
}

/* Write buffer implementation functions */
/* @brief Initializes a new write buffer
*  @returns an instance of a new write buffer depending on what write policy is defined
*/
write_buffer_t *write_buffer_init(void) {
    write_buffer_t *wb = (write_buffer_t *)malloc(sizeof(write_buffer_t));
    wb->penalty_count = 0;
    wb->writing = false;
    wb->subsequent_writing = 0;
    wb->data = (word_t *)malloc(sizeof(word_t)*d_cache->block_size);
    return wb;
}

/* @brief frees the memory reserved for the write buffer
*  @params the write buffer to be destroyed.
*/
void write_buffer_destroy(write_buffer_t *wb) {
    free(wb->data);
    free(wb);
}

uint32_t write_buffer_get_address(void){
    if(write_buffer->writing == false){
        //We should never be writing to this memory address
        return 0xffffffff;
    } else {
        return write_buffer->address;
    }
}

void write_buffer_digest(void) {
    //word_t temp;
    if (write_buffer->writing) {
        if (get_mem_status() != MEM_WRITING) {
            //Its not my turn!!!
            return;
        } else {
            write_buffer->penalty_count++;
            if (write_buffer->penalty_count == CACHE_WRITE_PENALTY) {
                mem_write_w(write_buffer->address, &write_buffer->data[write_buffer->subsequent_writing]);
                write_buffer->writing = false;
                write_buffer->penalty_count = 0;
                if(write_buffer->subsequent_writing != (d_cache->block_size - 1) && (get_write_policy() == CACHE_WRITEBACK)){
                    //enqueue the next data address
                    write_buffer->address+=4;
                    write_buffer->writing = true;
                    write_buffer->penalty_count = 0;
                    write_buffer->subsequent_writing = 1;
                } else {
                    set_mem_status(MEM_IDLE);
                }
            } else if (write_buffer->subsequent_writing && write_buffer->penalty_count == CACHE_WRITE_SUBSEQUENT_PENALTY) {
                mem_write_w(write_buffer->address, &write_buffer->data[write_buffer->subsequent_writing]);
                write_buffer->writing = false;
                write_buffer->penalty_count = 0;
                if (write_buffer->subsequent_writing != (d_cache->block_size - 1)) {
                    write_buffer->address += 4;
                    write_buffer->writing = true;
                    write_buffer->subsequent_writing++;
                    write_buffer->penalty_count = 0;
                } else {
                    set_mem_status(MEM_IDLE);
                }
            }
        }
    }
}

cache_status_t write_buffer_get_status(void){
    if(write_buffer->writing){
        return CACHE_MISS;
    } else {
        return CACHE_HIT;
    }
}

cache_status_t write_buffer_enqueue(cache_access_t info){
    if (write_buffer == NULL) {
        cprintf(ANSI_C_RED, "write_buffer_enqueue: buffer is not initialized\n", NULL);
        assert(0);
    }
    if (write_buffer->writing) {
        // Buffer is full!!
        if (flags & MASK_DEBUG) {
            printf("\twrite_buffer_enqueue: Write buffer is full!\n");
        }
        return CACHE_MISS;
    } else {
        uint8_t i = 0;
        for (i = 0; i < d_cache->block_size; i++) {
            if (d_cache->blocks[info.index].valid[i] == false) {
                if (flags & MASK_DEBUG) {
                    printf("\tEntire block is not valid. Waiting until block is valid before proceeding.\n");
                }
                return CACHE_MISS;
            }
        }
        if (flags & MASK_DEBUG) {
            printf("\twrite_buffer_enqueue: filling write buffer with block index %d and tag 0x%08x\n", info.index, info.tag);
        }
        if(get_write_policy() == CACHE_WRITEBACK){
            write_buffer->address = (info.address & (d_cache->tag_mask | d_cache->index_mask));
            for (i = 0; i < d_cache->block_size; i++) {
                write_buffer->data[i] = d_cache->blocks[info.index].data[i];
            }
        } else {
            write_buffer->address = info.address;
            write_buffer->data[0] = d_cache->blocks[info.index].data[info.inner_index];
        }
        write_buffer->writing = true;
        write_buffer->penalty_count = 0;
        write_buffer->subsequent_writing = 0;
        return CACHE_HIT;
    }
}

void flush_dcache(void){
    eprintf("Flushing cache...\n");
    uint32_t address;
    for (uint32_t i = 0; i < d_cache->num_blocks; i++) {
        if (d_cache->blocks[i].dirty) {
            for (uint32_t j = 0; j < d_cache->block_size; j++) {
                address = (d_cache->blocks[i].tag << (2 + d_cache->index_size + d_cache->inner_index_size)) | (i << (2 + d_cache->inner_index_size)) | (j << 2);
                eprintf("\tWriting 0x%08x (0d%d) to 0x%08x\n", d_cache->blocks[i].data[j], d_cache->blocks[i].data[j], address);
                mem_write_w(address, &(d_cache->blocks[i].data[j]));
            }
        }
    }
    eprintf("Flushing write buffer...\n");
    while (write_buffer->writing) {
        set_mem_status(MEM_WRITING);
        write_buffer_digest();
    }
}

void print_icache(int block) {
    direct_cache_print_block(i_cache, block);
}
void dump_dcache(void) {
    for (uint32_t i = 0; i < d_cache->num_blocks; i++) {
        print_dcache(i);
    }
}
void print_dcache(int block) {
    direct_cache_print_block(d_cache, block);
}

void print_write_buffer(void) {
    if (write_buffer == NULL) {
        cprintf(ANSI_C_RED, "write_buffer_enqueue: buffer is not initialized\n", NULL);
        assert(0);
    }
    eprintf("Writing: %d, Penalty Count: %d, Subsequent Writing: %d\n", write_buffer->writing, write_buffer->penalty_count, write_buffer->subsequent_writing);
    eprintf("Address: 0x%08x\n", write_buffer->address);
    eprintf("Data: \t0x%08x\n", write_buffer->data[0]);
    for (uint32_t i = 1; i < d_cache->block_size; i++) {
        printf("\t0x%08x\n", write_buffer->data[i]);
    }
}
