/* parser2.c
 * A quick test program for parsing "array"-based assembly instructions
 * Reads in lines of the format 0xXXXXXXXX, with optional comment
 * Ignores all other lines that do not fit this format
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

// Compile and run with
//    gcc -o parser2 parser2.c && ./parser2 ../asm/program1file.txt

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
        char buf[180], comment[120];
        int count = 0;
        // Iterate through file line-by-line
        while (fgets(buf, sizeof(buf), fp) != NULL ) {
            // Read the instruction into memory
            if (sscanf(buf,"0x%x",&(mem[count])) == 1) {
                printf("0x%08x: 0x%08x\n",(count<<2),mem[count]);
                ++count;
            }
            // Read the comment if it exists
            if (sscanf(buf,"0x%*x, // %[^\n]", comment) == 1) {
                printf("Comment: %s\n",comment);
            }
        }
        fclose(fp);
        printf("Succesfully extracted %d instructions\n",count);
    }
    return 0;
}
