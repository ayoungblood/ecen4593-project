/* util.h
 * Utility/debug defines and other miscellany
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "types.h"

// Sign extension macros for byte and halfword
#define SIGN_EXTEND_B(x) ((x & (1<< 7))?(x | 0xffffff00):(x))
#define SIGN_EXTEND_H(x) ((x & (1<<15))?(x | 0xffff0000):(x))

// Debugging and internal status flags
#define MASK_DEBUG          (1<<0) // Show debugging messages
#define MASK_VERBOSE        (1<<1) // Show verbose messages
#define MASK_SANITY         (1<<2) // Do extra checking (bounds checking, etc)
#define MASK_INTERACTIVE    (1<<3) // Interactive stepping
#define MASK_ALTFORMAT      (1<<4) // Alternate assembly input format
#define MASK_COLOR          (1<<5) // Colorized text output

// ANSI colour escapes
#define ANSI_C_BLACK        "\x1b[1;30m"
#define ANSI_C_RED          "\x1b[1;31m"
#define ANSI_C_YELLOW       "\x1b[1;33m"
#define ANSI_C_GREEN        "\x1b[1;32m"
#define ANSI_C_CYAN         "\x1b[1;36m"
#define ANSI_C_BLUE         "\x1b[1;34m"
#define ANSI_C_MAGENTA      "\x1b[1;35m"
#define ANSI_C_WHITE        "\x1b[1;37m"
#define ANSI_RESET          "\x1b[0m"
#define ANSI_BOLD           "\x1b[1m"
#define ANSI_UNDER          "\x1b[4m"
#define ANSI_RBOLD          "\x1b[0m\x1b[1m"
#define ANSI_RUNDER         "\x1b[0m\x1b[4m"

// Print macros (note that dprintf conflicts with POSIX, and vprintf conflicts with ISO C)
#define eprintf(...) fprintf(stderr,__VA_ARGS__)
#define gprintf(...) if (flags & MASK_DEBUG) eprintf(__VA_ARGS__)
#define bprintf(...) if (flags & MASK_VERBOSE) eprintf(__VA_ARGS__)
#define gcprintf(COLOR__,...) if (flags & MASK_DEBUG) cprintf(COLOR__,__VA_ARGS__)
#define bcprintf(COLOR__,...) if (flags & MASK_VERBOSE) cprintf(COLOR__,__VA_ARGS__)

int flags;

typedef struct cpu_config_t {
    bool single_cycle;
    unsigned long mem_size;
} cpu_config_t;

typedef enum cache_mode_t {
    CACHE_DISABLE,      // All caching disabled
    CACHE_SPLIT,        // Split caches, both enabled
    CACHE_UNIFIED       // Unified cache
} cache_mode_t;
typedef enum cache_type_t {
    CACHE_DIRECT,       // Direct-mapped
    CACHE_SA2           // Two-way set associative
} cache_type_t;
typedef enum cache_wpolicy_t {
    CACHE_WRITEBACK,
    CACHE_WRITETHROUGH
} cache_wpolicy_t;

typedef struct cache_config_t {
    cache_mode_t    mode;
    /* Split cache options */
    bool            data_enabled;
    unsigned int    data_size;
    unsigned int    data_block;
    cache_type_t    data_type;
    cache_wpolicy_t data_wpolicy;
    bool            inst_enabled;
    unsigned int    inst_size;
    unsigned int    inst_block;
    cache_type_t    inst_type;
    cache_wpolicy_t inst_wpolicy;
    /* Unified cache options */
    unsigned int    size;
    unsigned int    block;
    cache_type_t    type;
    cache_wpolicy_t wpolicy;
} cache_config_t;

void print_pipeline_register(control_t *reg);

void copy_pipeline_register(control_t *orig, control_t *copy);

void flush(control_t *reg);

void pipeline_init(control_t** ifid, control_t** idex, control_t** exmem, control_t** memwb, pc_t* pc, pc_t pc_start);

void pipeline_destroy(control_t** ifid, control_t** idex, control_t** exmem, control_t** memwb);

// Provides a reverse mapping (register number to string) for debugging
char* get_register_name_string(int reg);

// Print wrappers, because macros can't do everything
void cprintf(const char *color, const char *format, ...);

#endif /* _TYPES_H */
