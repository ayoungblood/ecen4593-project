/* src/main.h
 * Simulator top level
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "util.h"
#include "types.h"
#include "decode.h"
#include "main_memory.h"
#include "write.h"
#include "memory.h"
#include "alu.h"
#include "fetch.h"
#include "hazard.h"

#define MEMORY_SIZE (2<<14)

// For storing debugging information per line
typedef struct ASMLine {
    uint32_t addr;        // address
    uint32_t inst;        // instruction
    char     comment[80]; // remaining string data
    char     type;        // 0: invalid, 2/3: valid
} asm_line_t;

int parse(FILE *fp, asm_line_t *lines);

int interactive(asm_line_t *lines);

// Breakpoint wrappers
int breakpoint_get_active(void);
void breakpoint_add(uint32_t address);
void breakpoint_dump(void);
void breakpoint_delete(int n);
void breakpoint_check(pc_t current_pc);
