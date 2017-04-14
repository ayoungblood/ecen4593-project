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

#define VERSION_STRING      "?.?.????"
#define TARGET_STRING       "spam"
// Debugging and internal status flags
#define MASK_DEBUG          (1<<0) // Show debugging messages
#define MASK_VERBOSE        (1<<1) // Show verbose messages
#define MASK_SANITY         (1<<2) // Do extra checking (bounds checking, etc)
#define MASK_INTERACTIVE    (1<<3) // Interactive stepping
#define MASK_ALTFORMAT      (1<<4) // Alternate assembly input format
#define MASK_COLOR          (1<<5) // Colorized text output
// Print macros (note that dprintf conflicts with POSIX, and vprintf conflicts with ISO C)
#define eprintf(...) fprintf(stderr,__VA_ARGS__)
#define cprintf(COLOR__,str,...) \
do {if (flags & MASK_COLOR) \
        eprintf(COLOR__ str ANSI_RESET, ##__VA_ARGS__); \
    else \
        eprintf(str, ##__VA_ARGS__); \
    } while (0)
//#define cprintf(COLOR__,str,...) eprintf(COLOR__ str ANSI_RESET, ##__VA_ARGS__)
#define gprintf(COLOR__,str,...) if (flags & MASK_DEBUG) cprintf(COLOR__,str,##__VA_ARGS__)
#define bprintf(COLOR__,str,...) if (flags & MASK_VERBOSE) cprintf(COLOR__,str,##__VA_ARGS__)

typedef struct cpu_settings_t {
    bool single_cycle;
} cpu_settings_t;

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

/* Create and initialize CPU and cache settings */
cpu_settings_t cpu_settings = {
    false
};
cache_settings_t cache_settings = {
    4,
    1024,
    1024,
    1024,
    CACHE_SPLIT,
    CACHE_DIRECT,
    CACHE_WRITEBACK
};

int arguments(int argc, char **argv, FILE* source_fp,
        cpu_settings_t *cpu_settings, cache_settings_t *cache_settings);

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
    /* Parse command line arguments and options */
    FILE *source_fp = NULL;
    int rv = arguments(argc,argv,source_fp,&cpu_settings,&cache_settings);
    if (rv != 0) return rv;
    printf("Starting simulation with flags: 0x%08x\n", flags);
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
int arguments(int argc, char **argv, FILE* source_fp,
        cpu_settings_t *cpu_settings, cache_settings_t *cache_settings) {

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
                bprintf("","Alternate format enabled (flags = 0x%04x).\n",flags);
                break;
            case 'C':
                if (!strcmp(optarg,"disable")) {
                    flags &= ~MASK_COLOR;
                } else if (!strcmp(optarg,"force")) {
                    flags |= MASK_COLOR;
                } else if (!strcmp(optarg,"auto")) {
                    // do nothing, already set
                } else {
                    printf("Invalid color setting: %s\n", optarg);
                }
                if (flags & MASK_COLOR) {
                    bprintf("","Colorized output enabled (flags = 0x%04x).\n",flags);
                } else {
                    bprintf("","Colorized output disabled (flags = 0x%04x).\n",flags);
                }
                break;
            case 'd':
                flags |= MASK_DEBUG;
                bprintf("","Debug output enabled (flags = 0x%04x).\n",flags);
                break;
            case 'g':
                cpu_settings->single_cycle = true;
                bprintf("","CPU: single-cycle execution enabled (flags = 0x%04x).\n",flags);
                break;
            case 'h':
                printf( "Usage: %s [OPTION]... FILE[.s|.txt]\n" \
                        "   or: %s [--help|-h]\n" \
                        "   or: %s [--version|-V]\n" \
                        "  Run %s on an assembly source file, simulating a MIPS CPU execution of FILE,\n" \
                        "  or with [--help|h], display this usage information and exit,\n"
                        "  or with [--version|-V], display the version and exit.\n" \
                        "  One, and only one, assembly file must be provided for simulation.\n\n"
                        "   -a, --alternate\n" \
                        "   \tAlterate assembly format, expects lines like\n" \
                        "   \t\t0x24420004, // addiu v0,v0,4\n" \
                        "   \tinstead of the the default, which expects lines like\n" \
                        "   \t\t400048:	0x24420004    addiu v0,v0,4\n" \
                        "   -C mode, --color mode\n" \
                        "   --debug, -d\n" \
                        "   --single-cycle, -g\n" \
                        "   --help, -h\n" \
                        "   --interactive, -i\n" \
                        "   --sanity, -y\n" \
                        "   --version, -V\n" \
                        "   --verbose, -v\n" \
                        "   --cache-block size, -b size\n" \
                        "   --cache-config str, -c str\n" \
                        "   --cache-dsize size, -D size\n" \
                        "   --cache-isize size, -I size\n" \
                        "   --cache-size size, s size\n" \
                        "   --cache-type str, -t str\n" \
                        "   --cache-write str, -w str\n" \
                        "\nEmail bug reports to /dev/null\n", \
                        TARGET_STRING,TARGET_STRING,TARGET_STRING,TARGET_STRING);
                /*
                printf( "usage: sim [-adghiyVv] [-C mode] [-b num] [-c str] [-s num] [-t str] [-w str]\n"
                        "\t[--alternate] [--color str] [--debug] [--single-cycle] [--help]\n" \
                        "\t[--interactive] [--sanity] [--version] [--verbose] [--cache-block num]\n" \
                        "\t[--cache-config str] [--cache-size num] [--cache-type str]\n" \
                        "\t[--cache-write str] file\n");
                */
                return 0;
            case 'i':
                flags |= MASK_INTERACTIVE;
                bprintf("","Interactive mode enabled (flags = 0x%04x).\n",flags);
                break;
            case 'y':
                flags |= MASK_SANITY;
                bprintf("","Sanity checks enabled (flags = 0x%04x).\n",flags);
                break;
            case 'V':
                printf("%s - MIPS I CPU simulator %s\n",TARGET_STRING,VERSION_STRING);
                return 0;
            case 'v':
                flags |= MASK_VERBOSE;
                bprintf("","Verbose output enabled (flags = 0x%04x).\n",flags);
                break;
            /* Cache options */
            case 'b': // --block-size
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"Block size must be a number: %s\n",optarg);
                } else {
                    if (temp == 1 || temp == 4) {
                        cache_settings->block_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid block size: %d\n", temp);
                    }
                }
                bprintf("","CACHE: block size set to %d.\n",cache_settings->block_size);
                break;
            case 'c': // --cache-config
                if (!strcmp(optarg,"unified")) {
                    cache_settings->config = CACHE_UNIFIED;
                } else if (!strcmp(optarg,"split")) {
                    cache_settings->config = CACHE_SPLIT;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid cache configuration: %s\n", optarg);
                }
                bprintf("","CACHE: configuration set to ???.\n");
                break;
            case 'D': // --cache-dsize
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"D-cache size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1))) {
                        cache_settings->data_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid d-cache size: %d\n", temp);
                    }
                }
                bprintf("","CACHE: data cache size set to %d.\n",cache_settings->data_size);
                break;
            case 'I': // --cache-isize
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"I-cache size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1))) {
                        cache_settings->inst_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid i-cache size: %d\n", temp);
                    }
                }
                bprintf("","CACHE: instruction cache size set to %d.\n",cache_settings->inst_size);
                break;
            case 's': // --cache-size
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"Cache size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1))) {
                        cache_settings->cache_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid cache size: %d\n", temp);
                    }
                }
                bprintf("","CACHE: cache size set to %d.\n",cache_settings->cache_size);
                break;
            case 't': // --cache-type
                if (!strcmp(optarg,"direct")) {
                    cache_settings->type = CACHE_DIRECT;
                } else if (!strcmp(optarg,"sa2") || !strcmp(optarg,"2")) {
                    cache_settings->type = CACHE_SA2;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid cache type: %s\n", optarg);
                }
                bprintf("","CACHE: cache type set to ???.\n");
                break;
            case 'w': // --cache-write
                if (!strcmp(optarg,"writethrough") || !strcmp(optarg,"through") || !strcmp(optarg,"thru")) {
                    cache_settings->wpolicy = CACHE_WRITETHROUGH;
                } else if (!strcmp(optarg,"writeback") || !strcmp(optarg,"back")) {
                    cache_settings->wpolicy = CACHE_WRITEBACK;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid cache write policy: %s\n", optarg);
                }
                bprintf("","CACHE: cache write policy set to ???.\n");
                break;
            case '?': // error
                /* getopt_long already printed an error message. */
                break;
            default: // bad error
                printf("Failed to parse command line argument. Exiting.\n");
                return 1;
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        if (argc-optind > 1) {
            cprintf(ANSI_C_RED,"Too many files. (Cannot simulate many things!). Exiting.\n");
            return 1;
        } else {
            source_fp = fopen(argv[optind], "r");
            if (!source_fp) {
                cprintf(ANSI_C_RED,"You lied to me when you told me this was a file: %s\n",argv[optind]);
                return 1; // exit with errors
            }
        }
    } else {
        cprintf(ANSI_C_RED,"Expected at least one argument. (Cannot simulate nothing!). Exiting.\n");
        return 1;
    }
    return 0;
}
