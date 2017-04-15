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

#define DEFAULT_MEM_SIZE    (1<<14)
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

typedef struct cpu_config_t {
    bool single_cycle;
    unsigned long mem_size;
} cpu_config_t;

typedef enum cache_mode_t {
    CACHE_DISABLE,      // All caching disabled
    CACHE_SPLIT,        // Split caches, both enabled
    CACHE_UNIFIED       // Unified cache
} cache_mode_t;
const char * const CACHE_MODE_STRINGS[] = {"disabled","split","unified"};
typedef enum cache_type_t {
    CACHE_DIRECT,       // Direct-mapped
    CACHE_SA2           // Two-way set associative
} cache_type_t;
const char * const CACHE_TYPE_STRINGS[] = {"direct-mapped","2-way set associative"};
typedef enum cache_wpolicy_t {
    CACHE_WRITEBACK,
    CACHE_WRITETHROUGH
} cache_wpolicy_t;
const char * const CACHE_WPOLICY_STRINGS[] = {"writeback","writethrough"};

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

uint32_t flags = 0;

/* Create and initialize CPU and cache settings */
cpu_config_t cpu_config = {
    false,
    (1<<14)
};
cache_config_t cache_config = {
    CACHE_DISABLE,
    true,
    1024,
    4,
    CACHE_DIRECT,
    CACHE_WRITETHROUGH,
    true,
    1024,
    4,
    CACHE_DIRECT,
    CACHE_WRITETHROUGH,
    1024,
    4,
    CACHE_DIRECT,
    CACHE_WRITETHROUGH
};

int arguments(int argc, char **argv, FILE* source_fp,
        cpu_config_t *cpu_config, cache_config_t *cache_config);

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
/*
Enable flags for icache and dcache
Separate block size of icache and dcache
Global cache enable flag
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
    int rv = arguments(argc,argv,source_fp,&cpu_config,&cache_config);
    if (rv != 0) return rv;
    printf("Starting simulation with flags: 0x%08x\n", flags);
    bprintf("","Cache settings:\n");
    bprintf("","\tblock size: %d\n",cache_config.block);
    bprintf("","\tcache size: %d\n",cache_config.size);
    bprintf("","\tdata cache size: %d\n",cache_config.data_size);
    bprintf("","\tinstruction cache size: %d\n",cache_config.inst_size);
    //bprintf("","\tconfiguration: %s\n",(cache_config.config==CACHE_SPLIT?"SPLIT":"UNIFIED"));
    //bprintf("","\ttype: %s\n",(cache_config.type==CACHE_DIRECT?"DIRECT-MAPPED":"TWO-WAY SET ASSOCIATIVE"));
    //bprintf("","\twrite policy: %s\n",(cache_config.wpolicy==CACHE_WRITEBACK?"WRITEBACK":"WRITETHROUGH"));
    return 0;
}
int arguments(int argc, char **argv, FILE* source_fp,
        cpu_config_t *cpu_config, cache_config_t *cache_config) {

    /* Parse command line options with getopt */
    int c;
    int option_index = 0;
    int32_t temp, srv;
    //opterr = 0; // disable getopt_long default errors
    while (1) {
        static struct option long_options[] = {
            /* Simulator options */
            {"alternate",       no_argument,        0, 'a'},
            {"color",           required_argument,  0, 'C'}, // (disabled,auto,force)
            {"debug",           no_argument,        0, 'd'},
            {"help",            no_argument,        0, 'h'},
            {"interactive",     no_argument,        0, 'i'},
            {"sanity",          no_argument,        0, 'y'},
            {"version",         no_argument,        0, 'V'},
            {"verbose",         no_argument,        0, 'v'},
            /* CPU options */
            {"single-cycle",    no_argument,        0, 'g'},
            {"mem-size",        required_argument,  0, 'm'}, // 2^n, 0 <= n < 15
            /* Cache options */
            {"cache-mode",      required_argument,  0, 'c'}, // (disabled,split,unified)
            /* Split cache options */
            {"cache-data",      required_argument,  0, 'D'}, // (enabled,disabled)
            {"cache-dsize",     required_argument,  0, 'E'}, // 2^n, 0 < n <= 15
            {"cache-dblock",    required_argument,  0, 'F'}, // 2^n, 0 < n <= 7
            {"cache-dtype",     required_argument,  0, 'G'}, // (direct,sa2)
            {"cache-dwrite",    required_argument,  0, 'H'}, // (back,thru)
            {"cache-inst",      required_argument,  0, 'I'}, // (enabled,disabled)
            {"cache-isize",     required_argument,  0, 'J'}, // 2^n, 0 < n <= 15
            {"cache-iblock",    required_argument,  0, 'K'}, // 2^n, 0 < n <= 7
            {"cache-itype",     required_argument,  0, 'L'}, // (direct,sa2)
            {"cache-iwrite",    required_argument,  0, 'M'}, // (back,thru)
            /* Unified cache options */
            {"cache-block",     required_argument,  0, 'B'}, // 2^n, 0 < n <= 15
            {"cache-size",      required_argument,  0, 'S'}, // 2^n, 0 < n <= 7
            {"cache-type",      required_argument,  0, 'T'}, // (direct,sa2)
            {"cache-write",     required_argument,  0, 'W'}, // (back,thru)
            {0, 0, 0, 0}
        };
        c = getopt_long (argc, argv, "aC:dhiyVvc:gm:D:E:F:G:H:I:J:K:L:M:B:S:T:W:",long_options, &option_index);
        if (c == -1) break; // Detect the end of the options.

        switch (c) {
            /* Simulator options */
            case 'a': // --alternate
                flags |= MASK_ALTFORMAT;
                bprintf("","Alternate format enabled (flags = 0x%04x).\n",flags);
                break;
            case 'C': // --color
                if (!strcmp(optarg,"disabled") || !strcmp(optarg,"d")) {
                    flags &= ~MASK_COLOR;
                } else if (!strcmp(optarg,"force") || !strcmp(optarg,"f")) {
                    flags |= MASK_COLOR;
                } else if (!strcmp(optarg,"auto") || !strcmp(optarg,"a")) {
                    // do nothing, already set
                } else {
                    printf("Invalid color setting: %s\n", optarg);
                }
                bprintf("","Colorized output %s (flags = 0x%04x).\n",(flags & MASK_COLOR)?"enabled":"disabled",flags);
                break;
            case 'd': // --debug
                flags |= MASK_DEBUG;
                bprintf("","Debug output enabled (flags = 0x%04x).\n",flags);
                break;
            case 'h': // --help
                printf( "Usage: %s [OPTION]... FILE[.s,.txt]\n" \
                        "   or: %s [--help|-h]\n" \
                        "   or: %s [--version|-V]\n" \
                        "  Run %s on an assembly source file, simulating a MIPS CPU execution of FILE,\n" \
                        "  or with [--help|h], display this usage information and exit,\n"
                        "  or with [--version|-V], display the version and exit.\n" \
                        "  One, and only one, assembly file must be provided for simulation.\n\n" \
                        "General simulator options:\n" \
                        "   -a, --alternate\n" \
                        "   \tAlterate assembly format, expects lines like\n" \
                        "   \t\t0x24420004, // addiu v0,v0,4\n" \
                        "   \tinstead of the the default, which expects lines like\n" \
                        "   \t\t400048:	0x24420004    addiu v0,v0,4\n" \
                        "   -C mode, --color mode\n" \
                        "   \tColorized output behaviour. mode may be disable, which disables\n" \
                        "   \tcolorized output; force, which colorizes the output; or auto,\n" \
                        "   \twhich attempts to automatically detect whether to colorize.\n" \
                        "   --debug, -d\n" \
                        "   \tEnables debugging output.\n" \
                        "   --help, -h\n" \
                        "   \tPrints this usage information and exits.\n" \
                        "   --interactive, -i\n" \
                        "   \tEnables an interactive debugger for step-by-step and breakpoint-\n" \
                        "   \tbased debugging.\n" \
                        "   --sanity, -y\n" \
                        "   \tEnables internal sanity checking with a slight speed penalty.\n" \
                        "   --version, -V\n" \
                        "   \tPrints simulator version information.\n" \
                        "   --verbose, -v\n" \
                        "   \tEnable verbose output.\n" \
                        "CPU configuration options:\n" \
                        "   --cpu-single, -g\n" \
                        "   \tModels a single-cycle CPU, where each instruction takes one cycle.\n" \
                        "   \tIf not set, the default is a five-stage pipeline architecture.\n" \
                        "   --mem-size size, -m size\n" \
                        "   \tSets the size of main program memory. Defaults to %d bytes.\n" \
                        "Cache configuration options:\n" \
                        "   --cache-mode mode, -c mode\n" \
                        "   \tSets the cache mode, where mode must be (disabled,split,unified).\n" \
                        "   \tdisabled - turns off all caching.\n" \
                        "   \tsplit - uses split caches; data and instruction caches are separate.\n" \
                        "   \tunified - uses a single cache for instruction and data.\n" \
                        "   --cache-data en, -D en\n" \
                        "   --cache-inst en, -I en\n" \
                        "   \tEnable or disable data or instruction cache respectively.\n" \
                        "   \ten must be (0,1,enabled,disabled). Only applies with split cache.\n" \
                        "   \tBoth default to enabled.\n" \
                        "   --cache-size size, -S size\n" \
                        "   --cache-dsize size, -E size\n" \
                        "   --cache-isize size, -J size\n" \
                        "   \tSets the size of the unified, data, or instruction cache,\n" \
                        "   \trespectively. size must be 2^n, 0 < n < 15, defaults to 1024.\n" \
                        "   --cache-block size, -B size\n" \
                        "   --cache-dblock size, -F size\n" \
                        "   --cache-iblock size, -K size\n" \
                        "   \tSets the block size of the unified, data, or instruction cache,\n" \
                        "   \trespectively. size must be 2^n, 0 < n < 7, defaults to 4.\n" \
                        "   --cache-type type, -T type\n" \
                        "   --cache-dtype type, -G type\n" \
                        "   --cache-itype type, -L type\n" \
                        "   \tSets the type of the unified, data, or instruction cache,\n" \
                        "   \trespectively. type must be (direct,sa2).\n" \
                        "   \tdirect - uses a direct-mapped cache.\n" \
                        "   \tsa2 - uses a 2-way set associative cache.\n" \
                        "   --cache-write policy, -W policy\n" \
                        "   --cache-dwrite policy, -H policy\n" \
                        "   --cache-iwrite policy, -M policy\n" \
                        "   \tSets the write policy of the unified, data, or instruction cache,\n" \
                        "   \trespectively. policy must be (back,thru).\n" \
                        "   \tback - uses a writeback policy.\n" \
                        "   \tthru - uses a writethrough policy.\n" \
                        "\nEmail bug reports to /dev/null\n", \
                        TARGET_STRING,TARGET_STRING,TARGET_STRING,TARGET_STRING,DEFAULT_MEM_SIZE);
                /*
                printf( "usage: sim [-adghiyVv] [-C mode] [-b num] [-c str] [-s num] [-t str] [-w str]\n"
                        "\t[--alternate] [--color str] [--debug] [--single-cycle] [--help]\n" \
                        "\t[--interactive] [--sanity] [--version] [--verbose] [--cache-block num]\n" \
                        "\t[--cache-config str] [--cache-size num] [--cache-type str]\n" \
                        "\t[--cache-write str] file\n");
                */
                return 0;
            case 'i': // --interactive
                flags |= MASK_INTERACTIVE;
                bprintf("","Interactive mode enabled (flags = 0x%04x).\n",flags);
                break;
            case 'y': // --sanity
                flags |= MASK_SANITY;
                bprintf("","Sanity checks enabled (flags = 0x%04x).\n",flags);
                break;
            case 'V': // --version
                printf("%s - MIPS I CPU simulator %s\n",TARGET_STRING,VERSION_STRING);
                return 0;
            case 'v': // --verbose
                flags |= MASK_VERBOSE;
                bprintf("","Verbose output enabled (flags = 0x%04x).\n",flags);
                break;
            /* CPU options */
            case 'g': // --single-cycle
                cpu_config->single_cycle = true;
                bprintf("","CPU$ single-cycle execution enabled.\n");
                break;
            case 'm': // --mem-size
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"Memory size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<15)) {
                        cpu_config->mem_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid memory size: %d\n", temp);
                    }
                }
                bprintf("","CPU$ memory size set to %ld.\n",cpu_config->mem_size);
                break;
            /* Cache options */
            case 'c': // --cache-mode
                if (!strcmp(optarg,"disabled") || !strcmp(optarg,"d")) {
                    cache_config->mode = CACHE_DISABLE;
                } else if (!strcmp(optarg,"split") || !strcmp(optarg,"s")) {
                    cache_config->mode = CACHE_SPLIT;
                } else if (!strcmp(optarg,"unified") || !strcmp(optarg,"u")) {
                    cache_config->mode = CACHE_UNIFIED;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid cache mode: %s\n",optarg);
                }
                bprintf("","CACHE$ cache mode: %s.\n",CACHE_MODE_STRINGS[cache_config->mode]);
                break;
            /* Split cache options */
            case 'D': // --cache-data
                if (!strcmp(optarg,"disabled") || !strcmp(optarg,"d")) {
                    cache_config->data_enabled = false;
                } else if (!strcmp(optarg,"enabled") || !strcmp(optarg,"e")) {
                    cache_config->data_enabled = true;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid data cache setting: %s\n",optarg);
                }
                bprintf("","CACHE$ data cache setting: %s.\n",cache_config->data_enabled?"enabled":"disabled");
                break;
            case 'E': // --cache-dsize
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"D-cache size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<15)) {
                        cache_config->data_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid d-cache size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ data cache size set to %d.\n",cache_config->data_size);
                break;
            case 'F': // --cache-dblock
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"D-block size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<7)) {
                        cache_config->data_block = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid d-block size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ data cache block size set to %d.\n",cache_config->data_block);
                break;
            case 'G': // --cache-dtype
                if (!strcmp(optarg,"direct") || !strcmp(optarg,"d")) {
                    cache_config->data_type = CACHE_DIRECT;
                } else if (!strcmp(optarg,"sa2") || !strcmp(optarg,"2")) {
                    cache_config->data_type = CACHE_SA2;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid d-cache type: %s\n", optarg);
                }
                bprintf("","CACHE$ data cache type set to %s.\n",CACHE_TYPE_STRINGS[cache_config->data_type]);
                break;
            case 'H': // --cache-dwrite
                if (!strcmp(optarg,"through") || !strcmp(optarg,"thru") || !strcmp(optarg,"t")) {
                    cache_config->data_wpolicy = CACHE_WRITETHROUGH;
                } else if (!strcmp(optarg,"back") || !strcmp(optarg,"b")) {
                    cache_config->data_wpolicy = CACHE_WRITEBACK;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid data cache write policy: %s\n", optarg);
                }
                bprintf("","CACHE$ data cache write policy set to %s.\n",CACHE_WPOLICY_STRINGS[cache_config->data_wpolicy]);
                break;
            case 'I': // --cache-inst
                if (!strcmp(optarg,"disabled") || !strcmp(optarg,"d")) {
                    cache_config->inst_enabled = false;
                } else if (!strcmp(optarg,"enabled") || !strcmp(optarg,"e")) {
                    cache_config->inst_enabled = true;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid instruction cache setting: %s\n",optarg);
                }
                bprintf("","CACHE$ instruction cache setting: %s.\n",cache_config->inst_enabled?"enabled":"disabled");
                break;
            case 'J': // --cache-isize
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"I-cache size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<15)) {
                        cache_config->inst_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid i-cache size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ instruction cache size set to %d.\n",cache_config->inst_size);
                break;
            case 'K': // --cache-iblock
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"I-block size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<7)) {
                        cache_config->inst_block = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid i-block size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ instruction cache block size set to %d.\n",cache_config->inst_block);
                break;
            case 'L': // --cache-itype
                if (!strcmp(optarg,"direct") || !strcmp(optarg,"d")) {
                    cache_config->inst_type = CACHE_DIRECT;
                } else if (!strcmp(optarg,"sa2") || !strcmp(optarg,"2")) {
                    cache_config->inst_type = CACHE_SA2;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid i-cache type: %s\n", optarg);
                }
                bprintf("","CACHE$ instruction cache type set to %s.\n",CACHE_TYPE_STRINGS[cache_config->inst_type]);
                break;
            case 'M': // --cache-iwrite
                if (!strcmp(optarg,"through") || !strcmp(optarg,"thru") || !strcmp(optarg,"t")) {
                    cache_config->inst_wpolicy = CACHE_WRITETHROUGH;
                } else if (!strcmp(optarg,"back") || !strcmp(optarg,"b")) {
                    cache_config->inst_wpolicy = CACHE_WRITEBACK;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid instruction cache write policy: %s\n", optarg);
                }
                bprintf("","CACHE$ instruction cache write policy set to %s.\n",CACHE_WPOLICY_STRINGS[cache_config->inst_wpolicy]);
                break;
            /* Unified cache options */
            case 'B': // --cache-block
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"Block size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<7)) {
                        cache_config->block = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid block size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ cache block size set to %d.\n",cache_config->block);
                break;
            case 'S': // --cache-size
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"Cache size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<15)) {
                        cache_config->size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid cache size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ cache size set to %d.\n",cache_config->size);
                break;
            case 'T': // --cache-type
                if (!strcmp(optarg,"direct") || !strcmp(optarg,"d")) {
                    cache_config->type = CACHE_DIRECT;
                } else if (!strcmp(optarg,"sa2") || !strcmp(optarg,"2")) {
                    cache_config->type = CACHE_SA2;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid cache type: %s\n", optarg);
                }
                bprintf("","CACHE$ cache type set to %s.\n",CACHE_TYPE_STRINGS[cache_config->type]);
                break;
            case 'W': // --cache-write
                if (!strcmp(optarg,"through") || !strcmp(optarg,"thru") || !strcmp(optarg,"t")) {
                    cache_config->wpolicy = CACHE_WRITETHROUGH;
                } else if (!strcmp(optarg,"back") || !strcmp(optarg,"b")) {
                    cache_config->wpolicy = CACHE_WRITEBACK;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid cache write policy: %s\n", optarg);
                }
                bprintf("","CACHE$ cache write policy set to %s.\n",CACHE_WPOLICY_STRINGS[cache_config->wpolicy]);
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
