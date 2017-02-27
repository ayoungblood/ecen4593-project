#include <stdio.h>
#include <stdlib.h>
#include "../src/decode.h"


#define DEBUG 1

// Compile with
//    gcc ../src/decode.o -o test-decode test-decode.c

int main(int argc, char *argv[]){
    inst_t inst = 0x02518824;
    pc_t pc = 0;
    control_t * control = malloc(sizeof(control_t));

    int rv = decode(inst, pc, control);
    return 0;
}
