// getopt-sandbox.c
// Variadic arguments and macros are difficult
// Compile and run with
//    gcc -Wall -Wextra -o cprintf cprintf-sandbox.c && ./cprintf
// Use the following command to show fully expanded source
//    gcc -C -E cprintf-sandbox.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

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

// Debugging and internal status flags
#define MASK_DEBUG          (1<<0) // Show debugging messages
#define MASK_VERBOSE        (1<<1) // Show verbose messages
#define MASK_SANITY         (1<<2) // Do extra checking (bounds checking, etc)
#define MASK_INTERACTIVE    (1<<3) // Interactive stepping
#define MASK_ALTFORMAT      (1<<4) // Alternate assembly input format
#define MASK_COLOR          (1<<5) // Colorized text output

int flags = MASK_COLOR | MASK_DEBUG;

// Print macros (note that dprintf conflicts with POSIX, and vprintf conflicts with ISO C)
#define eprintf(...) fprintf(stderr,__VA_ARGS__)
#define gprintf(...) if (flags & MASK_DEBUG) eprintf(__VA_ARGS__)
#define bprintf(...) if (flags & MASK_VERBOSE) eprintf(__VA_ARGS__)
#define gcprintf(COLOR__,...) if (flags & MASK_DEBUG) cprintf(COLOR__,__VA_ARGS__)
#define bcprintf(COLOR__,...) if (flags & MASK_VERBOSE) cprintf(COLOR__,__VA_ARGS__)

// Print wrappers, because macros can't do everything
void cprintf(const char *color, const char *format, ...);

int main(int argc, char * argv[]) {
    (void) argc;
    (void) argv;
    eprintf("eprintf test with zero arguments\n");
    eprintf("eprintf test with %d arguments\n", 1);
    eprintf("eprintf test with %d %s\n", 2, "arguments");
    cprintf(ANSI_C_RED,"cprintf test with zero arguments\n");
    cprintf(ANSI_C_RED,"cprintf test with %d arguments\n", 1);
    cprintf(ANSI_C_RED,"cprintf test with %d %s\n", 2, "arguments");
    gprintf("gprintf test with zero arguments\n");
    gprintf("gprintf test with %d arguments\n", 1);
    gprintf("gprintf test with %d %s\n", 2, "arguments");
    bprintf("bprintf test with zero arguments\n");
    bprintf("bprintf test with %d arguments\n", 1);
    bprintf("bprintf test with %d %s\n", 2, "arguments");
    gcprintf(ANSI_C_MAGENTA,"gcprintf test with zero arguments\n");
    gcprintf(ANSI_C_MAGENTA,"gcprintf test with %d arguments\n", 1);
    gcprintf(ANSI_C_MAGENTA,"gcprintf test with %d %s\n", 2, "arguments");
    bcprintf(ANSI_C_YELLOW,"bcprintf test with zero arguments\n");
    bcprintf(ANSI_C_YELLOW,"bcprintf test with %d arguments\n", 1);
    bcprintf(ANSI_C_YELLOW,"bcprintf test with %d %s\n", 2, "arguments");
    return 0;
}

void cprintf(const char *color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (flags & MASK_COLOR) {
        eprintf("%s",color);
        vfprintf(stderr,format, args);
        eprintf(ANSI_RESET);
    } else {
        vprintf(format, args);
    }
    va_end(args);
}
