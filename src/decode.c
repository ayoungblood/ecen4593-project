/*  src/decode.c
*   Decode instruction stage of the CPU
*/
#include <stdio.h>
#include <stdbool.h>
#include "decode.h"

#define DEBUG 1


int decode( inst_t instr , pc_t pc , control_t * control ) {
    
    //Get all of the bitmasked values out of the instruction
    control->opCode = ( instr & OP_MASK ) >> OP_SHIFT;
    control->regRs = ( instr & RS_MASK ) >> RS_SHIFT;
    control->regRt = ( instr & RT_MASK ) >> RT_SHIFT;
    control->regRd = ( instr & RD_MASK ) >> RD_SHIFT;
    control->shamt = ( instr & SH_MASK ) >> SH_SHIFT;
    control->funct = ( instr & FC_MASK );
    control->immed = ( instr & IM_MASK );
    control->address = ( instr & AD_MASK );

    //Control registers. Truth table found on page 269
    control->regDst = (control->opCode == OPC_RTYPE) ? true : false;
    control->ALUSrc = (control->opCode == OPC_LW || control->opCode == OPC_SW) ? true : false;
    control->memToReg = (control->opCode == OPC_LW) ? true : false;
    control->regWrite = (control->opCode == OPC_RTYPE || control->opCode == OPC_LW) ? true : false;
    control->memRead = (control->opCode == OPC_LW) ? true : false;
    control->memWrite = (control->opCode == OPC_SW) ? true : false;

    switch(control->opCode){
        case OPC_RTYPE: 
            switch(control->funct){
                case FNC_ADD:
                    control->ALUop = OPR_ADD;
                    break;
                case FNC_SUB:
                    control->ALUop = OPR_SUB;
                    break;
                case FNC_AND:
                    control->ALUop = OPR_AND;
                    break;
                case FNC_OR:
                    control->ALUop = OPR_OR;
                    break;
                case FNC_SLT:
                    control->ALUop = OPR_SLT;
                    break;
                case FNC_NOR:
                    control->ALUop = OPR_NOR;
                    break;
                default:
                    printf("Unknown R-Type instruction 0x%08x\n", control->funct);
            }
            break;
        case OPC_LW:
            control->ALUop = OPR_ADD;
            break;
        case OPC_SW:
            control->ALUop = OPR_ADD;
            break;
        case OPC_BEQ:
            control->ALUop = OPR_SUB;
            break;
        default:
            printf("Unknown OpCode 0x%08x\n", control->opCode);
    }

    #ifdef DEBUG
    printf("Decoded control register from instruction 0x%08x\n", instr);
    printf("Decoded Instrunction: \n");
    printf("\tcontrol->opCode: 0x%08x\n", control->opCode);
    printf("\tcontrol->regRs: 0x%08x\n", control->regRs);
    printf("\tcontrol->regRt: 0x%08x\n", control->regRt);
    printf("\tcontrol->regRd: 0x%08x\n", control->regRd);
    printf("\tcontrol->shamt: 0x%08x\n", control->shamt);
    printf("\tcontrol->funct: 0x%08x\n", control->funct);
    printf("\tcontrol->immed: 0x%08x\n", control->immed);
    printf("\tcontrol->address: 0x%08x\n", control->address);
    printf("Control bits:\n");
    printf("\tcontrol->regDst: 0x%08x\n", control->regDst);
    printf("\tcontrol->ALUSrc: 0x%08x\n", control->ALUSrc);
    printf("\tcontrol->memToReg: 0x%08x\n", control->memToReg);
    printf("\tcontrol->regWrite: 0x%08x\n", control->regWrite);
    printf("\tcontrol->memRead: 0x%08x\n", control->memRead);
    printf("\tcontrol->memWrite: 0x%08x\n", control->memWrite);
    printf("\tcontrol->ALUop: 0x%08x\n", control->ALUop);
    #endif



    return 0;



}

