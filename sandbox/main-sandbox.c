/* sandbox/main-sandbox.c
* testing out the whole execution pipeline
*/


#include <stdio.h>
#include <stdlib.h>
#include "../src/alu.h"
#include "../src/decode.h"
#include "../src/main_memory.h"
#include "../src/fetch.h"
#include "../src/write.h"
#include "../src/registers.h"
#include "../src/types.h"
#include "../src/util.h"

int flags = 1; // Global flags register, shared across all files

int main(int argc, char * argv[]){


    printf("Beginning sandbox simulation\n");

    //Initialize the register file
    reg_init();

    //Pipeline registers
    control_t * ifid = (control_t*)malloc(sizeof(control_t));
    control_t * idex = (control_t*)malloc(sizeof(control_t));
    control_t * exmem = (control_t*)malloc(sizeof(control_t));
    control_t * memwb = (control_t*)malloc(sizeof(control_t));

    //Override fetch stage, dont want to deal with memory
    pc_t pc = 0x00000000;
    ifid->instr = 0x20100064;       //addi $s0, $zero, 100
    ifid->pcNext = 0x00000004;

    decode(ifid, &pc, idex);

    execute(idex, exmem);



}
