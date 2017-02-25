/*
*  src/decode.c
*/

#include "decode.h"


int decode( inst_t instr , pc_t  pc , reg_id_ex_t * id_ex ) {

    //Determine the if R type instruction
    int opCode = ( instr & 0xFC000000 ) >> 26;
    
    if ( opCode == 0 ) {
        //RType instruction
        id_ex->regRs = ( instr & 0x3E00000 ) >> 21;
        id_ex->regRt = ( instr & 0x01F0000 ) >> 16;
        id_ex->regRd = ( instr & 0x000F800 ) >> 11;
        id_ex->shamt = ( instr & 0x00007C0 ) >> 6;
        id_ex->pcNext = pc;
        //Instruction function decoding
    }
    else {
        //I or J type
        

    }

    return opCode;



}
