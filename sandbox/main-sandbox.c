/* sandbox/main-sandbox.c
* testing out the whole execution pipeline
*/


#include <stdio.h>
#include <stdlib.h>
#include "alu.h"
#include "decode.h"
#include "memory.h"
#include "registers.h"
#include "types.h"
#include "util.h"

int main(int argc, char * argv[]){


    printf("Beginning sandbox simulation");

    //Initialize the register file
    reg_init();
    
}
