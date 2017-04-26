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
