# ecen4593-project

[![Build Status](https://travis-ci.org/ayoungblood/ecen4593-project.svg?branch=master)](https://travis-ci.org/ayoungblood/ecen4593-project)

MIPS simulator built for ECEN4593 Computer Organization.

## Installation
Make sure you have gcc installed on your computer, then simply clone the repository and type `make`

## Running the Simulator
We have a make target, `make run`, which will run the simulation with all of the different cache combinations required for project.

If you wish to run one program and see the first 16 memory locations after the program finishes, type `./sim -a asm/program1file.txt`, where the last argument is the location of the program file.

A list of all possible commands can be seen by typing `./sim --help`. Here are some possible run configurations:
- `./sim -aidvy -C s -D 0 -K 4 -H b asm/program2file.txt` runs program 2 with a split cache, but the data cache is disabled. The block size for the instruction cache is 4, and the write policy is set to *write back*. The `-i` indicates that the program will go into *interactive* mode, which allows the user to step through each clock cycle seeing the debug output of each pipeline stage. See the Interactive Mode section for more information
- `./sim -a -C s -E 128 -K 16 asm/program1file.txt` runs program 1 with a split cache, data cache size of 128 bytes, and an instruction cache block size of 16.  

Here is a list of all of the options and a breif explanation of what they do

    Usage: sim [OPTION]... FILE[.s,.txt]
        or: sim [--help|-h]
        or: sim [--version|-V]
      Run sim on an assembly source file, simulating a MIPS CPU execution of FILE,
    or with [--help|h], display this usage information and exit,
    or with [--version|-V], display the version and exit.
    One, and only one, assembly file must be provided for simulation.

    General simulator options:
    -a, --alternate
        Alterate assembly format, expects lines like
                0x24420004, // addiu v0,v0,4
        instead of the the default, which expects lines like
                400048: 0x24420004    addiu v0,v0,4
    -c mode, --color mode
        Colorized output behaviour. mode may be disable, which disables
        colorized output; force, which colorizes the output; or auto,
        which attempts to automatically detect whether to colorize.
    --debug, -d
        Enables debugging output.
    --help, -h
        Prints this usage information and exits.
    --interactive, -i
        Enables an interactive debugger for step-by-step and breakpoint-
        based debugging.
    --sanity, -y
        Enables internal sanity checking with a slight speed penalty.
    --version, -V
        Prints simulator version information.
    --verbose, -v
        Enable verbose output.
    CPU configuration options:
        --single-cycle, -g
            Models a single-cycle CPU, where each instruction takes one cycle.
            If not set, the default is a five-stage pipeline architecture.
        --mem-size size, -m size
            Sets the size of main program memory. Defaults to 8192 bytes.
    Cache configuration options:
        --cache-mode mode, -C mode
            Sets the cache mode, where mode must be (disabled,split,unified).
            disabled - turns off all caching.
            split - uses split caches; data and instruction caches are separate.
            unified - uses a single cache for instruction and data.
        --cache-data en, -D en
        --cache-inst en, -I en
            Enable or disable data or instruction cache respectively.
            en must be (0,1,enabled,disabled). Only applies with split cache.
            Both default to enabled.
        --cache-size size, -S size
        --cache-dsize size, -E size
        --cache-isize size, -J size
            Sets the size of the unified, data, or instruction cache,
            respectively. size must be 2^n, 0 < n < 15, defaults to 1024.
        --cache-block size, -B size
        --cache-dblock size, -F size
        --cache-iblock size, -K size
            Sets the block size of the unified, data, or instruction cache,
            respectively. size must be 2^n, 0 < n < 7, defaults to 4.
        --cache-type type, -T type
        --cache-dtype type, -G type
        --cache-itype type, -L type
            Sets the type of the unified, data, or instruction cache,
            respectively. type must be (direct,sa2).
            direct - uses a direct-mapped cache.
            sa2 - uses a 2-way set associative cache.
        --cache-write policy, -W policy
        --cache-dwrite policy, -H policy
        --cache-iwrite policy, -M policy
            Sets the write policy of the unified, data, or instruction cache,
            respectively. policy must be (back,thru).
            back - uses a writeback policy.
            thru - uses a writethrough policy.

## Interactive Mode
The simulator can be run with interactive mode, which allows the user to step through one clock cycle at a time through the program. This mode is most helpful when -dvy are also included in the arguments to the program so the debug output can be seen. Once in interactive mode, a help prompt can be displayed by typing '?'. From here, you could add a breakpoint at a memory address, which allows you to continue the program until the instruction at that memory address is fetched. Here is a list of the commands available in interactive mode
- a: add breakpoint at a memory address
- b: list breakpoints
- c: clear a breakpoint by breakpoint index
- d: disable interactive mode
- l: print the original disassembly for a given memory address
- m: print a word in memory for a given memory address
- o: print 11 words of memory surrounding a given memory address
- s: singe step the pipeline
- r: print out the pipeline registers
- x: exit simulation
- D: print a block in the data cache
- I: print a block in the instruction cache
- W: print out the write buffer
