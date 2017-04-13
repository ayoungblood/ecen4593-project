// getopt-sandbox.c
// Test of getopt command line argument parsing in preparation for simulator
// argument parsing overhaul
// Based on getopt example from https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
// Compile and run with
//    gcc -Wall -Wextra -o argex getopt-sandbox.c && argex
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

/* Flags we need
--alterate -a: alternate program format
--color -C (disable|auto|force): colorized output, auto uses CLICOLOR environment variable
--debug -d: enable debug
--help -h -?: help
--interactive -i: interactive debugger enabled
--single-cycle -g: disable pipeline
--sanity -y: sanity checks enabled
--verbose -v: enable verbose
--version -V: print version
--cache-block -b size: cache block size, either 1/4, defaults to 4
--cache-config -c (unified|separate): unified or separate instruction/data cache, defaults to separate
--cache-size -s size: cache size, should be power of 2, defaults to 1024
--cache-type -t (direct|2): direct-mapped or two-way set associative, defaults to direct-mapped
--cache-write -w (through|back): write policy, defaults to writeback
*/

int main (int argc, char **argv) {
    /* Parse command line options with getopt */
    int c;
    int option_index = 0;
    while (1) {
        static struct option long_options[] = {
            {"alternate",       no_argument,        0, 'a'},
            {"color",           required_argument,  0, 'C'},
            {"debug",           no_argument,        0, 'd'},
            {"single-cycle",    no_argument,        0, 'g'},
            {"help",            no_argument,        0, 'h'},
            {"interactive",     no_argument,        0, 'i'},
            {"sanity",          no_argument,        0, 'y'},
            {"verbose",         no_argument,        0, 'v'},
            {"version",         no_argument,        0, 'V'},
            /* Cache options */
            {"cache-block",     required_argument,  0, 'b'},
            {"cache-config",    required_argument,  0, 'c'},
            {"cache-size",      required_argument,  0, 's'},
            {"cache-type",      required_argument,  0, 't'},
            {"cache-write",     required_argument,  0, 'w'},
            {0, 0, 0, 0}
        };
        c = getopt_long (argc, argv, "aC:dghiyvVb:c:s:t:w:",long_options, &option_index);
        if (c == -1) break; // Detect the end of the options.

        switch (c) {
            case 'a':
                printf("option -a (alternate)\n");
                break;
            case 'C':
                printf("option -C (color) with value '%s'\n", optarg);
                break;
            case 'd':
                printf("option -d (debug)\n");
                break;
            case 'g':
                printf("option -g (single-cycle)\n");
                break;
            case 'h':
                printf("option -h (help)\n");
                break;
            case 'i':
                printf("option -i (interactive)\n");
                break;
            case 'y':
                printf("option -y (sanity)\n");
                break;
            case 'v':
                printf("option -v (verbose)\n");
                break;
            case 'V':
                printf("option -V (version)\n");
                break;
            /* Cache options */
            case 'b':
                printf("option -b (cache-block) with value '%s'\n", optarg);
                break;
            case 'c':
                printf("option -c (cache-config) with value '%s'\n", optarg);
                break;
            case 's':
                printf("option -s (cache-size) with value '%s'\n", optarg);
                break;
            case 't':
                printf("option -t (cache-type) with value '%s'\n", optarg);
                break;
            case 'w':
                printf("option -w (cache-write) with value '%s'\n", optarg);
                break;
            case '?':
                /* getopt_long already printed an error message. */
                break;
            default:
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
        printf("Expected at least one argument. Exiting. (Cannot simulate nothing!)\n");
        return 0;
    }
    return 0;
}
