/* src/main.c
 * Simulator top level
 */

#include "main.h"

int flags = 0; // Global flags register, shared across all files

int main(int argc, char *argv[]) {
    int i;
    // Validate args, if they exist
    if (argc == 1) {
        printf("Nothing to execute.\nUsage:\n");
        printf("    sim [options] infile\n\n");
        printf("    Options:\n");
        printf("    -d: Enable debug mode\n");
        printf("    -v: Enable verbose output\n");
        printf("\n");
        return 0; // exit without errors
    } else {
        // Stone-age command-line argument parsing
        for (i=0; i<argc-2; ++i) {
            switch (argv[i+1][1]) { // add command line option flags here
                case 'd': // -d: debug
                    flags |= MASK_DEBUG;
                    printf("Debug mode enabled (flags = 0x%04x).\n",flags);
                    break;
                case 'v': // -v: verbose
                    flags |= MASK_VERBOSE;
                    printf("Verbose output enabled (flags = 0x%04x).\n",flags);
                    break;
                default:
                    printf("Option not recognized: %c\n",argv[i+1][1]);
                    break;
            }
        }
    }
    // Read in the assembly file to program space
    // The assembly file is always the last argument to the simulator
    FILE *asm_fp = fopen(argv[argc-1], "r");
    if (!asm_fp) {
        printf("Unable to open assembly file. Exiting..\n");
        return 1; // exit with errors
    }
    /**************************************************************************
     * Beginning the actual simulation                                        *
     * All initialization and state configuration happens below here          *
     **************************************************************************/

    // Create an array to hold all the debug information
    asm_line_t lines[MEMORY_SIZE];
    for (i = 0; i < MEMORY_SIZE; ++i) lines[i].type = 0; // initialize all invalid

    parse(asm_fp, lines);

    printf("Calculated offset: 0x%08x, printing 32 words from offset\n",mem_start());
    word_t temp;
    for (i = 0; i < 32; ++i) {
        mem_read_w(mem_start() + (i<<2), &temp);
        printf("    0x%08x: %08x\n", mem_start() + (i<<2), temp);
        printf("(%1x) 0x%08x: %08x\t%s\n", lines[i].type, lines[i].addr, lines[i].inst, lines[i].comment);
    }
    // Initialize the register file
    reg_init();
    // @TODO start simulation

    mem_close();
    return 0; // exit without errors
}

int parse(FILE *fp, asm_line_t *lines) {
    uint32_t addr, inst, start;
    int count = 0;
    char buf[120]; // for storing a line from the source file
    char str[80]; // for the comment part of a line from the source file
    // iterate through file line-by-line
    while (fgets(buf, sizeof(buf), fp) != NULL ) {
        // scanf magic to extract an address, colon, instruction, and the remaining line
        if (sscanf(buf,"%x: %x %[^\n]",&addr,&inst,str) == 3) {
            if (count == 0) { // first instruction, set offset and initialize memory
                if (flags & MASK_VERBOSE) printf("First instruction found. %s",buf);
                mem_init(MEMORY_SIZE,addr);
                start = addr;
            }
            // write extracted instruction into memory and also into lines array
            mem_write_w(addr,&inst);
            lines[(addr>>2)-(start>>2)].addr = addr;
            lines[(addr>>2)-(start>>2)].inst = inst;
            strcpy(lines[(addr>>2)-(start>>2)].comment, str);
            lines[(addr>>2)-(start>>2)].type = 2;
            ++count;
        }
    }
    fclose(fp); // close the file
    printf("Succesfully extracted %d instructions\n",count);
    return count;
}
