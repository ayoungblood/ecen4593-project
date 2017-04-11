/* src/main_memory.c
 * Provides access to a byte-addressable memory
 */

#include "main_memory.h"

extern int flags; // from main.c or memory-test.c
static word_t *mem; // pointer to memory block
static uint32_t start; // internal offset, in bytes, should be word-aligned
static uint32_t length; // length, in words

// Initialize the memory with a given size. Size and offset in bytes
void mem_init(uint32_t size, uint32_t offset) {
    mem = (word_t *)malloc(size);
    // If memory didn't get allocated, crash the program. (Time to download more RAM)
    if (NULL == mem) assert(0);
    length = size>>2; // length in words is size in bytes divided by four
    start = offset & 0xfffffffc; // start address is the offset in bytes, mask bottom two bits
    if (flags & MASK_VERBOSE) {
        printf("Initializing memory. Size: %d B (%d words), offset: 0x%08x\n",(length<<2),length, offset);
    }
#if (MEM_FILL)
    for (uint32_t i = 0; i < (size>>2); ++i) {
        //mem[i] = i + (i<<16);
        mem[i] = MEM_FILL_VALUE;
    }
#endif // MEM_FILL
}
// Display memory state (does _not_ dump the entire memory!)
void mem_dump(void) {
    printf("Memory statistics:\n");
    printf("  Bytes - start: 0x%08x; end: 0x%08x\n",start,start + (length<<2) - 1);
    printf("  Words - start: 0x%08x; end: 0x%08x\n",start,(start + ((length<<2)>>2) - 1));
    printf("  Size: %d B (%d words)\n",(length<<2),length);
    if (flags & MASK_DEBUG) {
        printf("Printing first 80 words of memory:\n");
        for (int i = 0; i < 16; ++i) {
            printf("  0x%02x: %08x | 0x%02x: %08x | 0x%02x: %08x | 0x%02x: %08x | 0x%02x: %08x\n",
                i<<2,mem[i],
                (i+16)<<2,mem[i+16],
                (i+32)<<2,mem[i+32],
                (i+48)<<2,mem[i+48],
                (i+64)<<2,mem[i+64]);
        }
    }
}
// Display a small section of memory starting at an address
void mem_dump_cute(uint32_t offset, uint32_t words) {
    printf("Printing %d words of memory starting at 0x%08x:\n",words,offset);
    offset = offset >> 2;
    for (uint32_t i = 0; i < words; ++i) {
        printf("\t0x%08x: 0x%08x (0d%d)\n",
            (offset+i)<<2,
            mem[(offset+i)],
            mem[(offset+i)]);
    }
}

// De-allocate memory space
void mem_close(void) {
    if (flags & MASK_VERBOSE) {
        printf("De-initializing memory. Size: %d B (%d words)\n",(length<<2),length);
    }
    free(mem);
    length = 0;
}

// Get memory size in bytes or words
uint32_t mem_size_b(void) {
    return length<<2;
}
uint32_t mem_size_w(void) {
    return length;
}
// Get memory start address (offset)
uint32_t mem_start(void) {
    return start;
}
// Get memory end address
uint32_t mem_end(void) {
    return (start + (length<<2) - 1);
}

// Read a word from a (word-aligned) memory address
void mem_read_w(uint32_t address, word_t *data) {
    uint32_t index = (address>>2) - (start>>2);
    if (flags & MASK_SANITY && index >= length) {
        printf(ANSI_C_RED "mem_read_w: out of range address 0x%08x (index %d >= length %d)\n" ANSI_C_RESET,address,index,length);
        assert(!(index >= length)); // fail fast
    }
    *data = mem[index];
    if (flags & MASK_DEBUG) {
        printf("mem_read_w: address 0x%08x, data 0x%08x, array index %d\n",address,*data,index);
    }
}
// Read a halfword from a (halfword-aligned) memory address
void mem_read_h(uint32_t address, word_t *data) {
    uint32_t index = (address>>2) - (start>>2);
    uint32_t shift = ((2-(address & 0x2))<<3); // shift amount based on byte position
    if (flags & MASK_SANITY && index >= length) {
        printf(ANSI_C_RED "mem_read_h: out of range address 0x%08x (index %d >= length %d)\n" ANSI_C_RESET,address,index,length);
        assert(!(index >= length)); // fail fast
    }
    *data = mem[index];
    *data >>= shift;
    *data &= 0xffff;
    if (flags & MASK_DEBUG) {
        printf("mem_read_h: address 0x%08x, data 0x%08x, array index %d\n",address,*data,index);
    }
}
// Read a byte from a memory address
void mem_read_b(uint32_t address, word_t *data) {
    uint32_t index = (address>>2) - (start>>2);
    uint32_t shift = ((3-(address & 0x3))<<3); // shift amount based on byte position
    if (flags & MASK_SANITY && index >= length) {
        printf(ANSI_C_RED "mem_read_b: out of range address 0x%08x (index %d >= length %d)\n" ANSI_C_RESET,address,index,length);
        assert(!(index >= length)); // fail fast
    }
    *data = mem[index];
    *data >>= shift;
    *data &= 0xff;
    if (flags & MASK_DEBUG) {
        printf("mem_read_b: address 0x%08x, data 0x%08x, array index %d\n",address,*data,index);
    }
}
// Write a word to a (word-aligned) memory address
void mem_write_w(uint32_t address, word_t *data) {
    uint32_t index = (address>>2) - (start>>2);
    if (flags & MASK_SANITY && index >= length) {
        printf(ANSI_C_RED "mem_write_w: out of range address 0x%08x (index %d >= length %d)\n" ANSI_C_RESET,address,index,length);
        assert(!(index >= length)); // fail fast
    }
    mem[index] = *data;
    if (flags & MASK_DEBUG) {
        printf("mem_write_w: address 0x%08x, data 0x%08x, array index %d\n",address,*data,index);
    }
}
// Write a halfword to a (halfword-aligned) memory address
void mem_write_h(uint32_t address, word_t *data) {
    uint32_t index = (address>>2) - (start>>2);
    uint32_t shift = ((2-(address & 0x2))<<3); // shift amount based on byte position
    if (flags & MASK_SANITY && index >= length) {
        printf(ANSI_C_RED "mem_write_h: out of range address 0x%08x (index %d >= length %d)\n" ANSI_C_RESET,address,index,length);
        assert(!(index >= length)); // fail fast
    }
    mem[index] &= ~(0xffff << shift); // clear the byte we are writing to
    mem[index] |= (*data & 0xffff)<<shift; // set the byte we are writing to
    if (flags & MASK_DEBUG) {
        printf("mem_write_h: address 0x%08x, data 0x%08x, array index %d\n",address,*data,index);
    }
}
// Write a byte to a memory address
void mem_write_b(uint32_t address, word_t *data) {
    uint32_t index = (address>>2) - (start>>2);
    uint32_t shift = ((3-(address & 0x3))<<3); // shift amount based on byte position
    if (flags & MASK_SANITY && index >= length) {
        printf(ANSI_C_RED "mem_write_h: out of range address 0x%08x (index %d >= length %d)\n" ANSI_C_RESET,address,index,length);
        assert(!(index >= length)); // fail fast
    }
    mem[index] &= ~(0xff << shift); // clear the byte we are writing to
    mem[index] |= (*data & 0xff)<<shift; // set the byte we are writing to
    if (flags & MASK_DEBUG) {
        printf("mem_write_b: address 0x%08x, data 0x%08x, array index %d\n",address,*data,index);
    }
}
