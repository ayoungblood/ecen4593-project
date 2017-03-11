/* util.h
 * Utility/debug defines and other miscellany
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <string.h>
#include "types.h"


// Debugging and internal status flags
#define MASK_DEBUG      0x1 // Show debugging messages
#define MASK_VERBOSE    0x2 // Show verbose messages
#define MASK_SANITY     0x4 // Do extra checking (bounds checking, etc)

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


void print_pipeline_register(control_t *);

#endif /* _TYPES_H */
