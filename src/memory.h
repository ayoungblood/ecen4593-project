/* src/memory.h
 * Memory header
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "util.h"
#include "types.h"

#define ENDIANNESS BIG
#define MEM_FILL 1
#define MEM_FILL_VALUE 0xffffffff

// Initialize the memory. Size and offset in bytes
void mem_init(uint32_t size, uint32_t offset);
// Display memory state (does _not_ dump the entire memory!)
void mem_dump(void);
// De-allocate memory
void mem_close(void);
// Get memory size in bytes or words
uint32_t mem_size_b(void);
uint32_t mem_size_w(void);
// Get memory start and end addresses
uint32_t mem_start(void);
uint32_t mem_end(void);

// Read from a memory address
void mem_read_w(uint32_t address, word_t *data); // read word
void mem_read_h(uint32_t address, word_t *data); // read half-word
void mem_read_b(uint32_t address, word_t *data); // read byte
// Write from a memory address
void mem_write_w(uint32_t address, word_t *data); // write word
void mem_write_h(uint32_t address, word_t *data); // write half-word
void mem_write_b(uint32_t address, word_t *data); // write byte
