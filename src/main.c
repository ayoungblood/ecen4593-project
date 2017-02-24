/* main.c
 * Top-level simulator source
 */

#include "main.h"

bool debug = false;
bool verbose = false;

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
                    printf("Debug mode enabled.\n");
                    debug = true;
                    break;
                case 'v': // -v: verbose
                    printf("Verbose output enabled.\n");
                    verbose = true;
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
    // @TODO parse file and start simulation

    fclose(asm_fp); // close the file
    return 0; // exit without errors
}
