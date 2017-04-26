/* src/main.h
 * Simulator top level
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "util.h"
#include "types.h"
#include "decode.h"
#include "main_memory.h"
#include "write.h"
#include "memory.h"
#include "alu.h"
#include "fetch.h"
#include "hazard.h"

// Set at compile time from the Makefile
//#define VERSION_STRING      "?.?.????"
//#define TARGET_STRING       "spam"

#define DEFAULT_MEM_SIZE    (1<<13)

// For storing debugging information per line
typedef struct ASMLine {
    uint32_t addr;        // address
    uint32_t inst;        // instruction
    char     comment[80]; // remaining string data
    char     type;        // 0: invalid, 2/3: valid
} asm_line_t;

const char * const CACHE_MODE_STRINGS[] = {
    [CACHE_DISABLE]         = "disabled",
    [CACHE_SPLIT]           = "split",
    [CACHE_UNIFIED]         = "unified"
};
const char * const CACHE_TYPE_STRINGS[] = {
    [CACHE_DIRECT]          = "direct-mapped",
    [CACHE_SA2]             = "2-way set associative"
};
const char * const CACHE_WPOLICY_STRINGS[] = {
    [CACHE_WRITEBACK]       = "writeback",
    [CACHE_WRITETHROUGH]    = "writethrough"
};

int arguments(int argc, char **argv, FILE** source_fp,
        cpu_config_t *cpu_cfg, cache_config_t *cache_cfg);

int parse(FILE *fp, asm_line_t *lines, cpu_config_t cpu_cfg);

int interactive(asm_line_t *lines);

// Breakpoint wrappers
int breakpoint_get_active(void);
void breakpoint_add(uint32_t address);
void breakpoint_dump(void);
void breakpoint_delete(int n);
void breakpoint_check(pc_t current_pc);
