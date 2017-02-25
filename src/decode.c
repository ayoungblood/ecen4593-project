/*
*  src/decode.c
*/
#include <stdio.h>
#include "decode.h"


int decode( inst_t instr , pc_t  pc , reg_id_ex_t * id_ex ) {

    //Determine the if R type instruction
    opcode_t opCode = ( instr & 0xFC000000 ) >> 26;
    
    if ( opCode == OPC_RTYPE ) {
        //RType instruction
        id_ex->regRs = ( instr & 0x03E00000 ) >> 21;
        id_ex->regRt = ( instr & 0x001F0000 ) >> 16;
        id_ex->regRd = ( instr & 0x0000F800 ) >> 11;
        id_ex->shamt = ( instr & 0x000007C0 ) >> 6;
        id_ex->pcNext = pc;
        //Instruction function decoding
        funct_t funct = ( instr & 0x00000003F );
        switch(funct){
            case FNC_ADD:
                id_ex->op = OPR_ADD;
                break;
            case FNC_ADDU:
                id_ex->op = OPR_ADDU;
                break;
            case FNC_AND:
                id_ex->op = OPR_AND;
                break;
            case FNC_JR:
                id_ex->op = OPR_JR;
                break;
            case FNC_NOR:
                id_ex->op = OPR_NOR;
                break;
            case FNC_OR:
                id_ex->op = OPR_OR;
                break;
            case FNC_SLT:
                id_ex->op = OPR_SLT;
                break;
            case FNC_SLTU:
                id_ex->op = OPR_SLTU;
                break;
            case FNC_SLL:
                id_ex->op = OPR_SLL;
                break;
            case FNC_SRL:
                id_ex->op = OPR_SRL;
                break;
            default:
                printf("Illegal instruction funct code: 0x%02x\n", funct);
        }
    }
    else if( opCode == OPC_J || opCode == OPC_JAL ){
        //J type 
        id_ex->immed = ( instr & 0x03FFFFFF ); 
        
    }
    else {
        //I type
        id_ex->regRs = ( instr & 0x03E00000 ) >> 21;
        id_ex->regRt = ( instr & 0x001F0000 ) >> 16;
        id_ex->immed = ( instr & 0x0000FFFF );
    }

    return opCode;



}
