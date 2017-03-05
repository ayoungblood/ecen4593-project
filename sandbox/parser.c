/* parser.c
 * A quick test program for parsing disassembler output
 * Reads in lines of the format <hex digits>: <hex digits>
 * Ignores all other lines that do not fit this format
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

// Compile and run with
//    gcc -o parser parser.c && ./parser ../asm/disassembly.s

#define MEM_SIZE 8192

uint32_t start;
uint32_t mem[MEM_SIZE] = {0};

int main(int argc, char *argv[]){
    int rv, i;
    if (argc == 1) {
        printf("Nothing to parse.\n");
    } else {
        printf("Parsing %s\n",argv[argc-1]);
        FILE* fp = fopen(argv[argc-1], "r");
        char buf[120];
        char str[80];
        uint32_t addr, inst;
        int count = 0;
        // iterate through file line-by-line
        while (fgets(buf, sizeof(buf), fp) != NULL ) {
            // scanf magic to extract an address, colon, instruction, and the remaining line
            if (sscanf(buf,"%x: %x %[^\n]",&addr,&inst,str) == 3) {
                printf("0x%08x: 0x%08x \t\t%s\n",addr,inst,str);
                if (count == 0) start = addr; // set offset
                mem[(addr>>2) - (start>>2)] = inst;
                ++count;
            }
        }
        fclose(fp);
        printf("Succesfully extracted %d instructions\n",count);
        printf("Calculated offset: 0x%08x, printing 32 words from offset\n",start);
        for (i = 0; i < 32; ++i) printf("0x%08x: %08x\n",(i<<2) + start,mem[i]);
    }
    return 0;
}
