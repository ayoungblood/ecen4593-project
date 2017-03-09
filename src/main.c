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
    // Initialize the register file
    reg_init();
    //
    char buf[120]; // for storing a line from the source file
    char str[80]; // for the comment part of a line from the source file

    typedef struct ASMLine { // for storing debugging information per line
        uint32_t addr;
        uint32_t inst;
        char     comment[80];
        char     type; // 0: invalid, 2: valid
    } asm_line_t;
    asm_line_t lines[MEMORY_SIZE];
    for (i = 0; i < MEMORY_SIZE; ++i) lines[i].type = 0; // all invalid

    uint32_t addr, inst, start;
    int count = 0;
    // iterate through file line-by-line
    while (fgets(buf, sizeof(buf), asm_fp) != NULL ) {
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
    fclose(asm_fp); // close the file
    printf("Succesfully extracted %d instructions\n",count);

    printf("Calculated offset: 0x%08x, printing 32 words from offset\n",start);
    word_t temp;
    for (i = 0; i < 32; ++i) {
        mem_read_w(start + (i<<2), &temp);
        printf("    0x%08x: %08x\n",(i<<2) + start,temp);
        printf("(%1x) 0x%08x: %08x\t%s\n", lines[i].type, lines[i].addr, lines[i].inst, lines[i].comment);
    }

    // @TODO start simulation

    return 0; // exit without errors
}
