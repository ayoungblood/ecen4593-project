/*
*  src/decode.c
*/

#include "decode.h"
#include <stdio.h>


int decode( inst_t instr , pc_t  pc , reg_id_ex_t * id_ex ) {

    //Determine the if R type instruction
    int opCode = ( instr & 0xF800000 ) >> 27;
    return opCode;



}
