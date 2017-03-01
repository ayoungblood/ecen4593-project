#include <stdio.h>
#include <stdint.h>
#include <limits.h>
// #include <stdlib.h>

// Compile and run with
//    gcc -o parser parser.c && ./parser ../asm/disassembly.s

int main(int argc, char *argv[]){
    int rv;
    if (argc == 1) {
        printf("Nothing to parse.\n");
    } else {
        printf("Parsing %s\n",argv[argc-1]);
        FILE* fp = fopen(argv[argc-1], "r");
        char buf[80];
        char str[80];
        uint32_t addr, inst;
        int count = 0;
        while (fgets(buf, sizeof(buf), fp) != NULL ) {
            //printf("%s",buf);
            // scanf magic to extract an address, colon, instruction, and the remaining line
            if (sscanf(buf,"%x: %x %[^\n]",&addr,&inst,str) == 3) {
                printf("0x%08x: 0x%08x \t\t%s\n",addr,inst,str);
                ++count;
            }
        }
        fclose(fp);
        printf("Succesfully extracted %d instructions\n",count);
    }
    return 0;
}
