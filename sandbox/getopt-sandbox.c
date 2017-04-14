// getopt-sandbox.c
// Test of getopt command line argument parsing in preparation for simulator
// argument parsing overhaul
// Based on getopt example from https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
// Compile and run with
//    gcc -Wall -Wextra -o argex getopt-sandbox.c && argex
// Use the following command to show fully expanded source
//    gcc -C -E getopt-sandbox.c
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

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

#define VERSION_STRING      "?.?.????"
// Debugging and internal status flags
#define MASK_DEBUG          (1<<0) // Show debugging messages
#define MASK_VERBOSE        (1<<1) // Show verbose messages
#define MASK_SANITY         (1<<2) // Do extra checking (bounds checking, etc)
#define MASK_INTERACTIVE    (1<<3) // Interactive stepping
#define MASK_ALTFORMAT      (1<<4) // Alternate assembly input format
#define MASK_COLOR          (1<<5) // Colorized text output
// Print macros (note that dprintf conflicts with POSIX, and vprintf conflicts with ISO C)
#define eprintf(...) fprintf(stderr,__VA_ARGS__)
#define cprintf(COLOR__,str,...) eprintf(COLOR__ str ANSI_C_RESET, ##__VA_ARGS__)
#define gprintf(COLOR__,str,...) if (flags & MASK_DEBUG) cprintf(COLOR__,str,##__VA_ARGS__)
#define bprintf(COLOR__,str,...) if (flags & MASK_VERBOSE) cprintf(COLOR__,str,##__VA_ARGS__)

typedef enum cache_config_t {
    CACHE_SPLIT,
    CACHE_UNIFIED
} cache_config_t;
typedef enum cache_type_t {
    CACHE_DIRECT,   // Direct-mapped
    CACHE_SA2       // Two-way set associative
} cache_type_t;
typedef enum cache_wpolicy_t {
    CACHE_WRITEBACK,
    CACHE_WRITETHROUGH
} cache_wpolicy_t;

typedef struct cache_settings_t {
    unsigned int    block_size;
    unsigned int    cache_size;
    unsigned int    data_size;
    unsigned int    inst_size;
    cache_config_t  config;
    cache_type_t    type;
    cache_wpolicy_t wpolicy;
} cache_settings_t;

uint32_t flags = 0;

cache_settings_t cache_settings = {4,1024,1024,1024,CACHE_SPLIT,CACHE_DIRECT,CACHE_WRITEBACK};

/* Flags we need
--alterate -a: alternate program format
--color -C (disable|auto|force): colorized output, auto uses CLICOLOR environment variable, defaults to auto
--debug -d: enable debug
--help -h -?: help
--interactive -i: interactive debugger enabled
--single-cycle -g: disable pipeline
--sanity -y: sanity checks enabled
--verbose -v: enable verbose
--version -V: print version
--cache-block -b size: cache block size, either 1/4, defaults to 4
--cache-config -c (unified|split): unified or split instruction/data cache, defaults to split
--cache-size -s size: cache size, should be power of 2, defaults to 1024
--cache-dsize -D size: for split config, specifies data cache size, defaults to --cache-size or 1024 if --cache-size not set
--cache-isize -I size: for split config, specifies instruction cache size, defaults to --cache-size or 1024 if --cache-size not set
--cache-type -t (direct|2): direct-mapped or two-way set associative, defaults to direct-mapped
--cache-write -w (through|back): write policy, defaults to writeback
*/

int main (int argc, char **argv) {
    /* Default to pipelined */
    bool single_cycle_flag = false;
    /* Automatically configure colorized output based on CLICOLOR and TERM
       environment variables (CLICOLOR=1 or TERM=xterm-256color) */
    char* crv;
    if ((crv = getenv("CLICOLOR"))) { // CLICOLOR is set
        if (!strcmp(crv,"1")) {
            flags |= MASK_COLOR;
        } else {
            flags &= ~MASK_COLOR;
        }
    } else { // CLICOLOR not defined, check TERM
        if ((crv = getenv("TERM"))) { // TERM is set
            if (!strcmp(crv,"xterm-256color")) {
                flags |= MASK_COLOR;
            } else {
                flags &= ~MASK_COLOR;
            }
        } else { // CLICOLOR and TERM not defined, give up
            flags &= ~MASK_COLOR;
        }
    }
    /* Parse command line options with getopt */
    int c;
    int option_index = 0;
    int temp, srv;
    //opterr = 0; // disable getopt_long default errors
    while (1) {
        static struct option long_options[] = {
            {"alternate",       no_argument,        0, 'a'},
            {"color",           required_argument,  0, 'C'},
            {"debug",           no_argument,        0, 'd'},
            {"single-cycle",    no_argument,        0, 'g'},
            {"help",            no_argument,        0, 'h'},
            {"interactive",     no_argument,        0, 'i'},
            {"sanity",          no_argument,        0, 'y'},
            {"version",         no_argument,        0, 'V'},
            {"verbose",         no_argument,        0, 'v'},
            /* Cache options */
            {"cache-block",     required_argument,  0, 'b'},
            {"cache-config",    required_argument,  0, 'c'},
            {"cache-dsize",     required_argument,  0, 'D'},
            {"cache-isize",     required_argument,  0, 'I'},
            {"cache-size",      required_argument,  0, 's'},
            {"cache-type",      required_argument,  0, 't'},
            {"cache-write",     required_argument,  0, 'w'},
            {0, 0, 0, 0}
        };
        c = getopt_long (argc, argv, "aC:dghiyVvb:c:D:I:s:t:w:",long_options, &option_index);
        if (c == -1) break; // Detect the end of the options.

        switch (c) {
            case 'a':
                flags |= MASK_ALTFORMAT;
                break;
            case 'C':
                if (!strcmp(optarg,"disable")) {
                    flags &= ~MASK_COLOR;
                } else if (!strcmp(optarg,"force")) {
                    flags |= MASK_COLOR;
                } else if (!strcmp(optarg,"auto")) {
                    // do nothing, already set
                } else {
                    printf("Unrecognized option argument: %s\n", optarg);
                }
                break;
            case 'd':
                flags |= MASK_DEBUG;
                break;
            case 'g':
                single_cycle_flag = true;
                break;
            case 'h':
                printf( "usage: sim [-adghiyVv] [-C mode] [-b num] [-c str] [-s num] [-t str] [-w str]\n"
                        "\t[--alternate] [--color str] [--debug] [--single-cycle] [--help]\n" \
                        "\t[--interactive] [--sanity] [--version] [--verbose] [--cache-block num]\n" \
                        "\t[--cache-config str] [--cache-size num] [--cache-type str]\n" \
                        "\t[--cache-write str] file\n");
                return 0;
            case 'i':
                flags |= MASK_INTERACTIVE;
                break;
            case 'y':
                flags |= MASK_SANITY;
                break;
            case 'V':
                printf("maps - MIPS simulator %s\n",VERSION_STRING);
                return 0;
            case 'v':
                flags |= MASK_VERBOSE;
                break;
            /* Cache options */
            case 'b': // --block-size
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"Block size must be a number (%s)\n",optarg);
                } else {
                    if (temp == 1 || temp == 4) {
                        cache_settings.block_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid block size %d\n", temp);
                    }
                }
                break;
            case 'c': // --cache-config
                if (!strcmp(optarg,"unified")) {
                    cache_settings.config = CACHE_UNIFIED;
                } else if (!strcmp(optarg,"split")) {
                    cache_settings.config = CACHE_SPLIT;
                } else {
                    cprintf(ANSI_C_YELLOW,"Unrecognized cache configuration: %s\n", optarg);
                }
                break;
            case 'D': // --cache-dsize
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"D-cache size must be a number (%s)\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1))) {
                        cache_settings.data_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid d-cache size %d\n", temp);
                    }
                }
                break;
            case 'I': // --cache-isize
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"I-cache size must be a number (%s)\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1))) {
                        cache_settings.inst_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid i-cache size %d\n", temp);
                    }
                }
                break;
            case 's': // --cache-size
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"Cache size must be a number (%s)\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1))) {
                        cache_settings.cache_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid cache size %d\n", temp);
                    }
                }
                break;
            case 't': // --cache-type
                if (!strcmp(optarg,"direct")) {
                    cache_settings.type = CACHE_DIRECT;
                } else if (!strcmp(optarg,"sa2") || !strcmp(optarg,"2")) {
                    cache_settings.type = CACHE_SA2;
                } else {
                    cprintf(ANSI_C_YELLOW,"Unrecognized cache type: %s\n", optarg);
                }
                break;
            case 'w': // --cache-write
                if (!strcmp(optarg,"writethrough") || !strcmp(optarg,"through") || !strcmp(optarg,"thru")) {
                    cache_settings.wpolicy = CACHE_WRITETHROUGH;
                } else if (!strcmp(optarg,"writeback") || !strcmp(optarg,"back")) {
                    cache_settings.wpolicy = CACHE_WRITEBACK;
                } else {
                    cprintf(ANSI_C_YELLOW,"Unrecognized cache write policy: %s\n", optarg);
                }
                break;
            case '?': // error
                /* getopt_long already printed an error message. */
                break;
            default: // bad error
                printf("Failed to parse command line argument. Exiting.\n");
                return 0;
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        printf("args:\n");
        while (optind < argc)
            printf ("\t%s\n", argv[optind++]);
    } else {
        cprintf(ANSI_C_RED,"Expected at least one argument. (Cannot simulate nothing!). Exiting.\n");
        return 0;
    }
    printf("Starting simulation with flags: 0x%08x\n", flags);
    cprintf(ANSI_C_YELLOW,"cprintf yellow\n");
    gprintf(ANSI_C_CYAN,"gprintf cyan\n");
    bprintf(ANSI_C_MAGENTA,"bprintf magenta\n");
    bprintf("","Cache settings:\n");
    bprintf("","\tblock size: %d\n",cache_settings.block_size);
    bprintf("","\tcache size: %d\n",cache_settings.cache_size);
    bprintf("","\tdata cache size: %d\n",cache_settings.data_size);
    bprintf("","\tinstruction cache size: %d\n",cache_settings.inst_size);
    bprintf("","\tconfiguration: %s\n",(cache_settings.config==CACHE_SPLIT?"SPLIT":"UNIFIED"));
    bprintf("","\ttype: %s\n",(cache_settings.type==CACHE_DIRECT?"DIRECT-MAPPED":"TWO-WAY SET ASSOCIATIVE"));
    bprintf("","\twrite policy: %s\n",(cache_settings.wpolicy==CACHE_WRITEBACK?"WRITEBACK":"WRITETHROUGH"));
    return 0;
}
