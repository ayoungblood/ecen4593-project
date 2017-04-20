/* src/main.c
 * Simulator top level
 */

#include "main.h"

int flags = 0; // Global flags register, shared across all files

/* Create and initialize CPU and cache settings with defaults */
cpu_config_t cpu_config = {
    .single_cycle   = false,
    .mem_size       = DEFAULT_MEM_SIZE,
};
cache_config_t cache_config = {
    .mode           = CACHE_DISABLE,
    .data_enabled   = true,
    .data_size      = 1024,
    .data_block     = 4,
    .data_type      = CACHE_DIRECT,
    .data_wpolicy   = CACHE_WRITETHROUGH,
    .inst_enabled   = true,
    .inst_size      = 1024,
    .inst_block     = 4,
    .inst_type      = CACHE_DIRECT,
    .inst_wpolicy   = CACHE_WRITETHROUGH,
    .size           = 1024,
    .block          = 4,
    .type           = CACHE_DIRECT,
    .wpolicy        = CACHE_WRITETHROUGH,
};

/* CPU state */
control_t* ifid  = NULL; // IF/ID pipeline register
control_t* idex  = NULL; // ID/EX pipeline register
control_t* exmem = NULL; // EX/MEM pipeline register
control_t* memwb = NULL; // MEM/WB pipeline register
pc_t pc = 0;             // Program counter

#define BREAKPOINT_MAX 8
uint32_t breakpoints_address[BREAKPOINT_MAX] = {0}; // the address of a breakpoint
uint8_t breakpoints_status[BREAKPOINT_MAX] = {0}; // breakpoint status, 0: disabled, 1:enabled

int main(int argc, char *argv[]) {
    int i;
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
    int rv = arguments(argc,argv,&source_fp,&cpu_config,&cache_config);
    if (rv !=  0) return rv;
    if (rv == -1) return 0;
    bprintf("","CPU settings:\n");
    bprintf("","\tArchitecture: %s\n",cpu_config.single_cycle?"single-cycle":"five-stage pipeline");
    bprintf("","\tMemory size: %lu words (%lu bytes, top = 0x%08lx)\n",cpu_config.mem_size>>2,cpu_config.mem_size,cpu_config.mem_size-1);
    bprintf("","Cache settings:\n");
    if (cache_config.mode == CACHE_SPLIT) {
        bprintf("","\tData cache:\n");
        bprintf("","\t    Data cache %s\n",cache_config.data_enabled?"enabled":"disabled");
        bprintf("","\t    Data cache size: %d\n",cache_config.data_size);
        bprintf("","\t    Data cache block size: %d\n",cache_config.data_block);
        bprintf("","\t    Data cache type: %s\n",CACHE_TYPE_STRINGS[cache_config.data_type]);
        bprintf("","\t    Data cache write policy: %s\n",CACHE_WPOLICY_STRINGS[cache_config.data_wpolicy]);
        bprintf("","\tInstruction cache:\n");
        bprintf("","\t    Instruction cache %s\n",cache_config.inst_enabled?"enabled":"disabled");
        bprintf("","\t    Instruction cache size: %d\n",cache_config.inst_size);
        bprintf("","\t    Instruction cache block size: %d\n",cache_config.inst_block);
        bprintf("","\t    Instruction cache type: %s\n",CACHE_TYPE_STRINGS[cache_config.inst_type]);
        bprintf("","\t    Instruction cache write policy: %s\n",CACHE_WPOLICY_STRINGS[cache_config.inst_wpolicy]);
    } else if (cache_config.mode == CACHE_UNIFIED) {
        bprintf("","\t    Unified cache size: %d\n",cache_config.size);
        bprintf("","\t    Unified cache block size: %d\n",cache_config.block);
        bprintf("","\t    Unified cache type: %s\n",CACHE_TYPE_STRINGS[cache_config.type]);
        bprintf("","\t    Unified cache write policy: %s\n",CACHE_WPOLICY_STRINGS[cache_config.wpolicy]);
    } else {
        bprintf("","\tAll caching disabled\n");
    }

    /**************************************************************************
     * Beginning the actual simulation                                        *
     * All initialization and state configuration happens below here          *
     **************************************************************************/
    printf("Starting simulation with flags: 0x%04x\n", flags);
    // Initialize the register file
    reg_init();
    // Create an array to hold all the debug information
    asm_line_t lines[cpu_config.mem_size];
    for (i = 0; i < (int)cpu_config.mem_size; ++i) lines[i].type = 0; // initialize all invalid
    // Parse the ASM file, parse() initializes the memory
    parse(source_fp, lines, cpu_config);
    mem_dump();
    // Initialize the pipeline registers
    pipeline_init(&ifid, &idex, &exmem, &memwb, &pc,  (pc_t)mem_start());
    hazard_init();
    cache_init();
    uint32_t word = 0;
    if (flags & MASK_ALTFORMAT) {
        // set the program counter based on the fifth word of memory
        mem_read_w(5<<2, &word);
        pc = word * 4;
    }
    // Run the simulation
    int cycles = 0;
    while (1) {
        // Run a pipeline cycle
        writeback(memwb);
        memory(exmem, memwb);
        execute(idex, exmem);
        decode(ifid, idex);
        fetch(ifid, &pc);
        hazard(ifid, idex, exmem, memwb, &pc);
        cache_digest();
        ++cycles;
        // Check for a magic halt number (beq zero zero -1 or jr zero)
        if (ifid->instr == 0x1000ffff || ifid->instr == 0x00000008 || pc == 0) break;
        // Breakpoint and interactive stuff
        breakpoint_check(pc);
        if (flags & MASK_INTERACTIVE) { // Run interactive step
            if (interactive(lines) !=0) return 1;
        }
    }
    printf("\nPipeline halted after %d cycles (address 0x%08x)\n",cycles,pc);
    // Dump registers and the first couple words of memory so we can see what's going on
    reg_dump();
    mem_dump_cute(0,16);
    // Close memory, and cleanup register files (we don't need to clean up registers)
    pipeline_destroy(&ifid, &idex, &exmem, &memwb);
    mem_close();
    return 0; // exit without errors
}

/* Parse command line arguments and options
 * Returns > 1 on error, or -1 if no error occurred but the caller should still exit */
int arguments(int argc, char **argv, FILE** source_fp,
        cpu_config_t *cpu_cfg, cache_config_t *cache_cfg) {

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
                        "   "ANSI_BOLD"-a, --alternate"ANSI_RESET"\n" \
                        "   \tAlterate assembly format, expects lines like\n" \
                        "   \t\t0x24420004, // addiu v0,v0,4\n" \
                        "   \tinstead of the the default, which expects lines like\n" \
                        "   \t\t400048:	0x24420004    addiu v0,v0,4\n" \
                        "   "ANSI_BOLD"-C "ANSI_RUNDER"mode"ANSI_RBOLD", --color "ANSI_RUNDER"mode"ANSI_RESET"\n" \
                        "   \tColorized output behaviour. "ANSI_UNDER"mode"ANSI_RESET" may be "ANSI_BOLD"disable"ANSI_RESET", which disables\n" \
                        "   \tcolorized output; "ANSI_BOLD"force"ANSI_RESET", which colorizes the output; or "ANSI_BOLD"auto"ANSI_RESET",\n" \
                        "   \twhich attempts to automatically detect whether to colorize.\n" \
                        "   "ANSI_BOLD"--debug, -d"ANSI_RESET"\n" \
                        "   \tEnables debugging output.\n" \
                        "   "ANSI_BOLD"--help, -h"ANSI_RESET"\n" \
                        "   \tPrints this usage information and exits.\n" \
                        "   "ANSI_BOLD"--interactive, -i"ANSI_RESET"\n" \
                        "   \tEnables an interactive debugger for step-by-step and breakpoint-\n" \
                        "   \tbased debugging.\n" \
                        "   "ANSI_BOLD"--sanity, -y"ANSI_RESET"\n" \
                        "   \tEnables internal sanity checking with a slight speed penalty.\n" \
                        "   "ANSI_BOLD"--version, -V"ANSI_RESET"\n" \
                        "   \tPrints simulator version information.\n" \
                        "   "ANSI_BOLD"--verbose, -v"ANSI_RESET"\n" \
                        "   \tEnable verbose output.\n" \
                        "CPU configuration options:\n" \
                        "   "ANSI_BOLD"--single-cycle, -g"ANSI_RESET"\n" \
                        "   \tModels a single-cycle CPU, where each instruction takes one cycle.\n" \
                        "   \tIf not set, the default is a five-stage pipeline architecture.\n" \
                        "   "ANSI_BOLD"--mem-size "ANSI_RUNDER"size"ANSI_RBOLD", -m "ANSI_RUNDER"size"ANSI_RESET"\n" \
                        "   \tSets the size of main program memory. Defaults to %d bytes.\n" \
                        "Cache configuration options:\n" \
                        "   "ANSI_BOLD"--cache-mode "ANSI_RUNDER"mode"ANSI_RBOLD", -c "ANSI_RUNDER"mode"ANSI_RESET"\n" \
                        "   \tSets the cache mode, where "ANSI_UNDER"mode"ANSI_RESET" must be ("ANSI_BOLD"disabled,split,unified"ANSI_RESET").\n" \
                        "   \t"ANSI_BOLD"disabled"ANSI_RESET" - turns off all caching.\n" \
                        "   \t"ANSI_BOLD"split"ANSI_RESET" - uses split caches; data and instruction caches are separate.\n" \
                        "   \t"ANSI_BOLD"unified"ANSI_RESET" - uses a single cache for instruction and data.\n" \
                        "   "ANSI_BOLD"--cache-data "ANSI_RUNDER"en"ANSI_RBOLD", -D "ANSI_RUNDER"en"ANSI_RESET"\n" \
                        "   "ANSI_BOLD"--cache-inst "ANSI_RUNDER"en"ANSI_RBOLD", -I "ANSI_RUNDER"en"ANSI_RESET"\n" \
                        "   \tEnable or disable data or instruction cache respectively.\n" \
                        "   \t"ANSI_UNDER"en"ANSI_RESET" must be ("ANSI_BOLD"0,1,enabled,disabled"ANSI_RESET"). Only applies with split cache.\n" \
                        "   \tBoth default to enabled.\n" \
                        "   "ANSI_BOLD"--cache-size "ANSI_RUNDER"size"ANSI_RBOLD", -S "ANSI_RUNDER"size"ANSI_RESET"\n" \
                        "   "ANSI_BOLD"--cache-dsize "ANSI_RUNDER"size"ANSI_RBOLD", -E "ANSI_RUNDER"size"ANSI_RESET"\n" \
                        "   "ANSI_BOLD"--cache-isize "ANSI_RUNDER"size"ANSI_RBOLD", -J "ANSI_RUNDER"size"ANSI_RESET"\n" \
                        "   \tSets the size of the unified, data, or instruction cache,\n" \
                        "   \trespectively. "ANSI_UNDER"size"ANSI_RESET" must be 2^n, 0 < n < 15, defaults to 1024.\n" \
                        "   "ANSI_BOLD"--cache-block "ANSI_RUNDER"size"ANSI_RBOLD", -B "ANSI_RUNDER"size"ANSI_RESET"\n" \
                        "   "ANSI_BOLD"--cache-dblock "ANSI_RUNDER"size"ANSI_RBOLD", -F "ANSI_RUNDER"size"ANSI_RESET"\n" \
                        "   "ANSI_BOLD"--cache-iblock "ANSI_RUNDER"size"ANSI_RBOLD", -K "ANSI_RUNDER"size"ANSI_RESET"\n" \
                        "   \tSets the block size of the unified, data, or instruction cache,\n" \
                        "   \trespectively. "ANSI_UNDER"size"ANSI_RESET" must be 2^n, 0 < n < 7, defaults to 4.\n" \
                        "   "ANSI_BOLD"--cache-type "ANSI_RUNDER"type"ANSI_RBOLD", -T "ANSI_RUNDER"type"ANSI_RESET"\n" \
                        "   "ANSI_BOLD"--cache-dtype "ANSI_RUNDER"type"ANSI_RBOLD", -G "ANSI_RUNDER"type"ANSI_RESET"\n" \
                        "   "ANSI_BOLD"--cache-itype "ANSI_RUNDER"type"ANSI_RBOLD", -L "ANSI_RUNDER"type"ANSI_RESET"\n" \
                        "   \tSets the type of the unified, data, or instruction cache,\n" \
                        "   \trespectively. "ANSI_UNDER"type"ANSI_RESET" must be ("ANSI_BOLD"direct,sa2"ANSI_RESET").\n" \
                        "   \t"ANSI_BOLD"direct"ANSI_RESET" - uses a direct-mapped cache.\n" \
                        "   \t"ANSI_BOLD"sa2"ANSI_RESET" - uses a 2-way set associative cache.\n" \
                        "   "ANSI_BOLD"--cache-write "ANSI_RUNDER"policy"ANSI_RBOLD", -W "ANSI_RUNDER"policy"ANSI_RESET"\n" \
                        "   "ANSI_BOLD"--cache-dwrite "ANSI_RUNDER"policy"ANSI_RBOLD", -H "ANSI_RUNDER"policy"ANSI_RESET"\n" \
                        "   "ANSI_BOLD"--cache-iwrite "ANSI_RUNDER"policy"ANSI_RBOLD", -M "ANSI_RUNDER"policy"ANSI_RESET"\n" \
                        "   \tSets the write policy of the unified, data, or instruction cache,\n" \
                        "   \trespectively. "ANSI_UNDER"policy"ANSI_RESET" must be ("ANSI_BOLD"back,thru"ANSI_RESET").\n" \
                        "   \t"ANSI_BOLD"back"ANSI_RESET" - uses a writeback policy.\n" \
                        "   \t"ANSI_BOLD"thru"ANSI_RESET" - uses a writethrough policy.\n" \
                        "\nEmail bug reports to /dev/null\n", \
                        TARGET_STRING,TARGET_STRING,TARGET_STRING,TARGET_STRING,DEFAULT_MEM_SIZE);
                return -1; // caller should exit
            case 'i': // --interactive
                flags |= MASK_INTERACTIVE;
                bprintf("","Interactive mode enabled (flags = 0x%04x).\n",flags);
                break;
            case 'y': // --sanity
                flags |= MASK_SANITY;
                bprintf("","Sanity checks enabled (flags = 0x%04x).\n",flags);
                break;
            case 'V': // --version
                printf("%s - MIPS I CPU simulator (v. %s)\n",TARGET_STRING,VERSION_STRING);
                return -1; // caller should exit
            case 'v': // --verbose
                flags |= MASK_VERBOSE;
                bprintf("","Verbose output enabled (flags = 0x%04x).\n",flags);
                break;
            /* CPU options */
            case 'g': // --single-cycle
                cpu_cfg->single_cycle = true;
                bprintf("","CPU$ single-cycle execution enabled.\n");
                break;
            case 'm': // --mem-size
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"Memory size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<15)) {
                        cpu_cfg->mem_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid memory size: %d\n", temp);
                    }
                }
                bprintf("","CPU$ memory size set to %ld.\n",cpu_cfg->mem_size);
                break;
            /* Cache options */
            case 'c': // --cache-mode
                if (!strcmp(optarg,"disabled") || !strcmp(optarg,"d")) {
                    cache_cfg->mode = CACHE_DISABLE;
                } else if (!strcmp(optarg,"split") || !strcmp(optarg,"s")) {
                    cache_cfg->mode = CACHE_SPLIT;
                } else if (!strcmp(optarg,"unified") || !strcmp(optarg,"u")) {
                    cache_cfg->mode = CACHE_UNIFIED;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid cache mode: %s\n",optarg);
                }
                bprintf("","CACHE$ cache mode: %s.\n",CACHE_MODE_STRINGS[cache_cfg->mode]);
                break;
            /* Split cache options */
            case 'D': // --cache-data
                if (!strcmp(optarg,"disabled") || !strcmp(optarg,"d")) {
                    cache_cfg->data_enabled = false;
                } else if (!strcmp(optarg,"enabled") || !strcmp(optarg,"e")) {
                    cache_cfg->data_enabled = true;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid data cache setting: %s\n",optarg);
                }
                bprintf("","CACHE$ data cache setting: %s.\n",cache_cfg->data_enabled?"enabled":"disabled");
                break;
            case 'E': // --cache-dsize
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"D-cache size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<15)) {
                        cache_cfg->data_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid d-cache size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ data cache size set to %d.\n",cache_cfg->data_size);
                break;
            case 'F': // --cache-dblock
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"D-block size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<7)) {
                        cache_cfg->data_block = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid d-block size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ data cache block size set to %d.\n",cache_cfg->data_block);
                break;
            case 'G': // --cache-dtype
                if (!strcmp(optarg,"direct") || !strcmp(optarg,"d")) {
                    cache_cfg->data_type = CACHE_DIRECT;
                } else if (!strcmp(optarg,"sa2") || !strcmp(optarg,"2")) {
                    cache_cfg->data_type = CACHE_SA2;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid d-cache type: %s\n", optarg);
                }
                bprintf("","CACHE$ data cache type set to %s.\n",CACHE_TYPE_STRINGS[cache_cfg->data_type]);
                break;
            case 'H': // --cache-dwrite
                if (!strcmp(optarg,"through") || !strcmp(optarg,"thru") || !strcmp(optarg,"t")) {
                    cache_cfg->data_wpolicy = CACHE_WRITETHROUGH;
                } else if (!strcmp(optarg,"back") || !strcmp(optarg,"b")) {
                    cache_cfg->data_wpolicy = CACHE_WRITEBACK;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid data cache write policy: %s\n", optarg);
                }
                bprintf("","CACHE$ data cache write policy set to %s.\n",CACHE_WPOLICY_STRINGS[cache_cfg->data_wpolicy]);
                break;
            case 'I': // --cache-inst
                if (!strcmp(optarg,"disabled") || !strcmp(optarg,"d")) {
                    cache_cfg->inst_enabled = false;
                } else if (!strcmp(optarg,"enabled") || !strcmp(optarg,"e")) {
                    cache_cfg->inst_enabled = true;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid instruction cache setting: %s\n",optarg);
                }
                bprintf("","CACHE$ instruction cache setting: %s.\n",cache_cfg->inst_enabled?"enabled":"disabled");
                break;
            case 'J': // --cache-isize
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"I-cache size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<15)) {
                        cache_cfg->inst_size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid i-cache size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ instruction cache size set to %d.\n",cache_cfg->inst_size);
                break;
            case 'K': // --cache-iblock
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"I-block size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<7)) {
                        cache_cfg->inst_block = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid i-block size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ instruction cache block size set to %d.\n",cache_cfg->inst_block);
                break;
            case 'L': // --cache-itype
                if (!strcmp(optarg,"direct") || !strcmp(optarg,"d")) {
                    cache_cfg->inst_type = CACHE_DIRECT;
                } else if (!strcmp(optarg,"sa2") || !strcmp(optarg,"2")) {
                    cache_cfg->inst_type = CACHE_SA2;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid i-cache type: %s\n", optarg);
                }
                bprintf("","CACHE$ instruction cache type set to %s.\n",CACHE_TYPE_STRINGS[cache_cfg->inst_type]);
                break;
            case 'M': // --cache-iwrite
                if (!strcmp(optarg,"through") || !strcmp(optarg,"thru") || !strcmp(optarg,"t")) {
                    cache_cfg->inst_wpolicy = CACHE_WRITETHROUGH;
                } else if (!strcmp(optarg,"back") || !strcmp(optarg,"b")) {
                    cache_cfg->inst_wpolicy = CACHE_WRITEBACK;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid instruction cache write policy: %s\n", optarg);
                }
                bprintf("","CACHE$ instruction cache write policy set to %s.\n",CACHE_WPOLICY_STRINGS[cache_cfg->inst_wpolicy]);
                break;
            /* Unified cache options */
            case 'B': // --cache-block
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"Block size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<7)) {
                        cache_cfg->block = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid block size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ cache block size set to %d.\n",cache_cfg->block);
                break;
            case 'S': // --cache-size
                srv = sscanf(optarg,"%d",&temp);
                if (!srv) {
                    cprintf(ANSI_C_YELLOW,"Cache size must be a number: %s\n",optarg);
                } else {
                    if ((temp!=0) && !(temp&(temp-1)) && temp <= (2<<15)) {
                        cache_cfg->size = temp;
                    } else {
                        cprintf(ANSI_C_YELLOW,"Invalid cache size: %d\n", temp);
                    }
                }
                bprintf("","CACHE$ cache size set to %d.\n",cache_cfg->size);
                break;
            case 'T': // --cache-type
                if (!strcmp(optarg,"direct") || !strcmp(optarg,"d")) {
                    cache_cfg->type = CACHE_DIRECT;
                } else if (!strcmp(optarg,"sa2") || !strcmp(optarg,"2")) {
                    cache_cfg->type = CACHE_SA2;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid cache type: %s\n", optarg);
                }
                bprintf("","CACHE$ cache type set to %s.\n",CACHE_TYPE_STRINGS[cache_cfg->type]);
                break;
            case 'W': // --cache-write
                if (!strcmp(optarg,"through") || !strcmp(optarg,"thru") || !strcmp(optarg,"t")) {
                    cache_cfg->wpolicy = CACHE_WRITETHROUGH;
                } else if (!strcmp(optarg,"back") || !strcmp(optarg,"b")) {
                    cache_cfg->wpolicy = CACHE_WRITEBACK;
                } else {
                    cprintf(ANSI_C_YELLOW,"Invalid cache write policy: %s\n", optarg);
                }
                bprintf("","CACHE$ cache write policy set to %s.\n",CACHE_WPOLICY_STRINGS[cache_cfg->wpolicy]);
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
            *source_fp = fopen(argv[optind], "r");
            if (!(*source_fp)) {
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

int parse(FILE *fp, asm_line_t *lines, cpu_config_t cpu_cfg) {
    uint32_t addr, inst, data, start;
    int count = 0;
    char buf[180]; // for storing a line from the source file
    char str[120]; // for the comment part of a line from the source file
    if (flags & MASK_ALTFORMAT) { // .txt "array" format
        addr = 0;
        mem_init(cpu_cfg.mem_size,0); // memory is assumed to start at 0x0
        // Iterate through file line-by-line
        while (fgets(buf, sizeof(buf), fp) != NULL ) {
            // Read the instruction into memory
            if (sscanf(buf,"0x%x",&inst) == 1) {
                mem_write_w(addr,&inst);
                lines[count].addr = addr;
                lines[count].inst = inst;
                lines[count].type = 2;
                addr += 4;
                // Read the comment if it exists
                if (sscanf(buf,"0x%*x, // %[^\n]", str) == 1) {
                    strcpy(lines[count].comment, str);
                    lines[count].type = 3;
                }
                ++count;
            }
        }
        // Set registers
        mem_read_w(0x0,&data);
        reg_write(REG_SP, &data);
        mem_read_w(0x1,&data);
        reg_write(REG_FP, &data);
        // (program counter is set after initializing pipeline)
    } else { // .s format
        // iterate through file line-by-line
        while (fgets(buf, sizeof(buf), fp) != NULL ) {
            // scanf magic to extract an address, colon, instruction, and the remaining line
            if (sscanf(buf,"%x: %x %[^\n]",&addr,&inst,str) == 3) {
                if (count == 0) { // first instruction, set offset and initialize memory
                    if (flags & MASK_VERBOSE) printf("First instruction found. %s",buf);
                    mem_init(cpu_cfg.mem_size,addr);
                    start = addr;
                }
                // write extracted instruction into memory and also into lines array
                mem_write_w(addr,&inst);
                lines[(addr>>2)-(start>>2)].addr = addr;
                lines[(addr>>2)-(start>>2)].inst = inst;
                strcpy(lines[(addr>>2)-(start>>2)].comment, str);
                lines[(addr>>2)-(start>>2)].type = 3;
                ++count;
            } else if (sscanf(buf,"%x: %x\n",&addr,&data) == 2) {
                // write extracted data into memory and also into lines array
                mem_write_w(addr,&data);
                lines[(addr>>2)-(start>>2)].addr = addr;
                lines[(addr>>2)-(start>>2)].inst = data;
                lines[(addr>>2)-(start>>2)].type = 2;
                ++count;
            }
        }
    }
    fclose(fp); // close the file
    printf("Successfully extracted %d lines\n",count);
    return count;
}
// Breakpoint wrappers
int breakpoint_get_active(void) {
    int i, sum = 0;
    for (i = 0; i < BREAKPOINT_MAX; ++i) {
        sum += breakpoints_status[i] & 0x1;
    }
    return sum;
}
void breakpoint_add(uint32_t address) {
    int i = 0;
    do {
        if (!(breakpoints_status[i] & 0x1)) {
            breakpoints_address[i] = address;
            breakpoints_status[i] |= 0x1;
            break;
        }
    } while (i++ < BREAKPOINT_MAX);
    cprintf(ANSI_C_GREEN, "Added breakpoint at 0x%08x, %d active breakpoints\n",
        address,
        breakpoint_get_active());
}
void breakpoint_dump(void) {
    int i = 0;
    printf("\tIndex Status  Address\n");
    for (i = 0; i < BREAKPOINT_MAX; ++i) {
        printf("%s\t(%2d)  %s     0x%08x\n" ANSI_RESET,
            (breakpoints_status[i] & 0x1)?(ANSI_C_CYAN):(ANSI_RESET),
            i,
            (breakpoints_status[i] & 0x1)?"SET":"---",
            breakpoints_address[i]);
    }
}
void breakpoint_delete(int n) {
    if (breakpoints_status[n] & 0x1) {
        breakpoints_status[n] &= ~(0x1);
        cprintf(ANSI_C_GREEN, "Cleared breakpoint at 0x%08x, %d active breakpoints\n",
            breakpoints_address[n],
            breakpoint_get_active());
        breakpoints_address[n] = 0x0;
    } else {
        cprintf(ANSI_C_GREEN, "Breakpoint not set, so not cleared. Pay attention!\n");
    }
}
void breakpoint_check(pc_t current_pc) {
    int i = 0;
    for (i = 0; i < BREAKPOINT_MAX; ++i) {
        if ((breakpoints_status[i] & 0x1) && (current_pc == breakpoints_address[i])) {
            flags |= MASK_INTERACTIVE | MASK_DEBUG | MASK_VERBOSE;
            cprintf(ANSI_C_GREEN, "Halted at breakpoint %d (pc = 0x%08x)\n",i,current_pc);
            break;
        }
    }
}
// Provides a crude interactive debugger for the simulator
int interactive(asm_line_t* lines) {
    uint32_t i_addr = 0, i_data;
    asm_line_t line;
PROMPT: // LOL gotos
    cprintf(ANSI_C_GREEN, "(interactive) > ");
    system ("/bin/stty raw"); // set terminal to raw/unbuffered
    char c = getchar();
    system ("/bin/stty sane"); // set back to sane
    printf("%c\n",c);
    switch(c) {
        case 'a': // add a breakpoint
            if (breakpoint_get_active() >= BREAKPOINT_MAX) {
                cprintf(ANSI_C_GREEN, "Cannot add breakpoint, active breakpoint limit reached.\n");
            } else {
                cprintf(ANSI_C_GREEN, "breakpoint address: ");
                scanf("%x",&i_addr); getchar();
                if (i_addr < mem_start() || i_addr > mem_end()) {
                    printf("Address out of range\n");
                    goto PROMPT;
                }
                breakpoint_add(i_addr);

            }
            goto PROMPT;
        case 'b': // list breakpoints
            if (breakpoint_get_active() == 0) {
                cprintf(ANSI_C_GREEN, "No breakpoints active.\n");
            } else {
                breakpoint_dump();
            }
            goto PROMPT;
        case 'c': // clear a breakpoint
            if (breakpoint_get_active() == 0) {
                cprintf(ANSI_C_GREEN, "No breakpoints active.\n");
            } else {
                cprintf(ANSI_C_GREEN, "breakpoint number to clear: ");
                scanf("%d",&i_addr); getchar();
                if (i_addr < BREAKPOINT_MAX) breakpoint_delete(i_addr);
            }
            goto PROMPT;
        case 'd': // disable interactive (disable verbose and debug as well to avoid flood)
            flags &= ~(MASK_INTERACTIVE | MASK_VERBOSE | MASK_DEBUG);
            cprintf(ANSI_C_GREEN, "Interactive stepping disabled. Running until breakpoint (if set).\n");
            break;
        case 'l': // print the original disassembly for a given address
            cprintf(ANSI_C_GREEN, "input address: ");
            scanf("%x",&i_addr); getchar();
            line = lines[(i_addr>>2)-(mem_start()>>2)];
            if (line.type == 3) {
                printf("\t0x%08x: 0x%08x %s\n",line.addr,line.inst,line.comment);
            } else if (line.type == 2) {
                printf("\t0x%08x: 0x%08x\n",line.addr,line.inst);
            } else {
                printf("\tNot a valid input line\n");
            }
            goto PROMPT;
        case 'm': // view a word of memory
            cprintf(ANSI_C_GREEN, "memory address: ");
            scanf("%x",&i_addr); getchar();
            if (i_addr < mem_start() || i_addr > mem_end()) {
                printf("Address out of range\n");
                goto PROMPT;
            }
            mem_read_w(i_addr, &i_data);
            printf("mem[0x%08x]: 0x%08x (0d%d)\n",i_addr,i_data,i_data);
            goto PROMPT;
        case 'o': // view a region of memory
            cprintf(ANSI_C_GREEN,"memory address: ");
            scanf("%x",&i_addr); getchar();
            if (i_addr < mem_start() || i_addr > mem_end()) {
                printf("Address out of range\n");
                goto PROMPT;
            }
            if (i_addr < mem_start()+(5<<2)) i_addr = mem_start()+(5<<2);
            if (i_addr > mem_end()-(5<<2)) i_addr = mem_end()-(5<<2);
            mem_dump_cute(i_addr-(5<<2),11);
            goto PROMPT;
        case 's': // step
            break;
        case 'r': // dump registers
            reg_dump();
            goto PROMPT;
        case 'x': // exit
            cprintf(ANSI_C_GREEN, "Simulation halted in interactive mode.\n");
            return 1;
        case '?': // help
            printf("Available interactive commands: \n" \
                "\ta: add breakpoint at a memory address\n" \
                "\tb: list breakpoints\n" \
                "\tc: clear a breakpoint by breakpoint index\n" \
                "\td: disable interactive mode\n" \
                "\tl: print the original disassembly for a given memory address\n" \
                "\tm: print a memory word for a given memory address\n" \
                "\to: print 11 words of memory surrounding a given memory address\n" \
                "\ts: single-step the pipeline\n" \
                "\tr: dump registers\n" \
                "\tx: exit simulation run\n");
            goto PROMPT;
        default:
            printf("Unrecognized interactive command \"%c\", press \"?\" for help.\n", c);
            goto PROMPT;
    }
    return 0;
}
