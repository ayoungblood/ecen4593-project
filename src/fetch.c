/* src/fetch.c
* Instruction fetch stage of the pipeline
*/


#include "fetch.h"


extern int flags;


void fetch(control_t * ifid, pc_t * pc){
    //Read the instruction at the current program counter
    mem_read_w(*pc, &(ifid->instr));

    //Update the program counter by 4
    *pc = *pc + 4;
    ifid->pcNext = *pc;

    //The only two data fields that are in the ifid register are 
    //the next program counter and the instruction
}
