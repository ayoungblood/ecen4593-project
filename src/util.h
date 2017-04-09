/* util.h
 * Utility/debug defines and other miscellany
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "types.h"

// Sign extension macros for byte and halfword
#define SIGN_EXTEND_B(x) ((x & (1<< 7))?(x | 0xffffff00):(x))
#define SIGN_EXTEND_H(x) ((x & (1<<15))?(x | 0xffff0000):(x))

// Debugging and internal status flags
#define MASK_DEBUG      0x1 // Show debugging messages
#define MASK_VERBOSE    0x2 // Show verbose messages
#define MASK_SANITY     0x4 // Do extra checking (bounds checking, etc)
#define MASK_INTERACTIVE 0x8 // Interactive stepping
#define MASK_ALTFORMAT  0x10 // Alternate assembly input format

// ANSI colour escapes
#define ANSI_C_BLACK   "\x1b[1;30m"
#define ANSI_C_RED     "\x1b[1;31m"
#define ANSI_C_YELLOW  "\x1b[1;33m"
#define ANSI_C_GREEN   "\x1b[1;32m"
#define ANSI_C_CYAN    "\x1b[1;36m"
#define ANSI_C_BLUE    "\x1b[1;34m"
#define ANSI_C_MAGENTA "\x1b[1;35m"
#define ANSI_C_WHITE   "\x1b[1;37m"
#define ANSI_C_RESET   "\x1b[0m"


void print_pipeline_register(control_t *reg);

void copy_pipeline_register(control_t *orig, control_t *copy);

void flush(control_t *reg);

void pipeline_init(control_t** ifid, control_t** idex, control_t** exmem, control_t** memwb, pc_t* pc, pc_t pc_start);

void pipeline_destroy(control_t** ifid, control_t** idex, control_t** exmem, control_t** memwb);

// Provides a reverse mapping (register number to string) for debugging
char* get_register_name_string(int reg);

#endif /* _TYPES_H */
