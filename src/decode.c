/*  src/decode.c
*   Decode instruction stage of the CPU
*/
#include <stdio.h>
#include "decode.h"


int decode( inst_t instr , pc_t  pc , reg_id_ex_t * id_ex ) {

    //Determine the if R type instruction
    opcode_t opCode = ( instr & OP_MASK ) >> OP_SHIFT;
    
    if ( opCode == OPC_RTYPE ) {
        //RType instruction
        id_ex->regRs = ( instr & RS_MASK ) >> RS_SHIFT;
        id_ex->regRt = ( instr & RT_MASK ) >> RT_SHIFT;
        id_ex->regRd = ( instr & RD_MASK ) >> RD_SHIFT;
        id_ex->shamt = ( instr & SH_MASK ) >> SH_SHIFT;
        id_ex->pcNext = pc;
        //Instruction function decoding
        funct_t funct = ( instr & FC_MASK );
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
        id_ex->immed = ( instr & AD_MASK ); 
        if( opCode == OPC_J ){
            id_ex->op = OPR_J;
        }
        else {
            id_ex->op = OPR_JAL;   
        }
    }
    else {
        //I type
        id_ex->regRs = ( instr & RS_MASK ) >> RS_SHIFT;
        id_ex->regRt = ( instr & RT_MASK ) >> RT_SHIFT;
        id_ex->immed = ( instr & IM_MASK );
        //Sign Extension of immediate field
        if( ( id_ex->immed & BIT15 ) != 0 ){
            id_ex->immed |= EXT_16_32;
        }
        switch(opCode){
            case OPC_ADDI:
                id_ex->op = OPR_ADDI;
                break;
            case OPC_ADDIU:
                id_ex->op = OPR_ADDIU;
                break;
            case OPC_ANDI:
                id_ex->op = OPR_ANDI;
                break;
            case OPC_BEQ:
                id_ex->op = OPR_BEQ;
                break;
            case OPC_BNE:
                id_ex->op = OPR_BNE;
                break;
            case OPC_LBU:
                id_ex->op = OPR_LBU;
                break;
            case OPC_LHU:
                id_ex->op = OPR_LHU;
                break;
            case OPC_LL:
                //id_ex->op = OPR_LL;
                printf("Unknown opcode OPC_LL");
                break;
            case OPC_LUI:
                printf("Unknown opcode OPC_LUI");
                //id_ex->op = OPR_LUI;
                break;
            case OPC_LW:
                id_ex->op = OPR_LW;
                break;
            case OPC_ORI:
                id_ex->op = OPR_ORI;
                break;
            case OPC_SLTI:
                id_ex->op = OPR_SLTI;
                break;
            case OPC_SLTIU:
                id_ex->op = OPR_SLTIU;
                break;
            case OPC_SB:
                id_ex->op = OPR_SB;
                break;
            case OPC_SC:
                printf("Unknown opcode OPC_SC");
                //id_ex->op = OPR_SC;
                break;
            case OPC_SH:
                id_ex->op = OPR_SH;
                break;
            case OPC_SW:
                id_ex->op = OPR_SW;
                break;
            default:
                printf("Illegal I type instruction 0x%02x\n", opCode);

        }
    }

    return opCode;



}
