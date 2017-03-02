/*  src/decode.c
*   Decode instruction stage of the CPU
*/

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
    control->address = ( instr & AD_MASK );
    uint32_t immed = ( instr & IM_MASK );

    control->pcNext = pc + 4;

    //Sign extension of the immediate field
    control->immed = ( instr & BIT15 ) ? immed | EXT_16_32 : immed;

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
            control->regDst = true;
            control->ALUSrc = false;
            control->memToReg = false;
            control->regWrite = true;
            control->memRead = false;
            control->memWrite = false;
            control->jump = false;
            control->PCSrc = false;
            break;
        case OPC_LW:
            control->ALUop = OPR_ADD;
            control->regDst = false;
            control->ALUSrc = true;
            control->memToReg = true;
            control->regWrite = true;
            control->memRead = true;
            control->memWrite = false;
            control->jump = false;
            control->PCSrc = false;
            break;
        case OPC_SW:
            control->ALUop = OPR_ADD;
            // control->regDst = false; Dont Care
            control->ALUSrc = true;
            // control->memToReg = true; Dont Care
            control->regWrite = false;
            control->memRead = false;
            control->memWrite = true;
            control->jump = false;
            control->PCSrc = false;
            break;
        case OPC_BEQ:
        case OPC_BNE:
            control->ALUop = OPR_SUB;
            // control->regDst = false; Dont Care
            control->ALUSrc = false;
            // control->memToReg = true; Dont Care
            control->regWrite = false;
            control->memRead = false;
            control->memWrite = false;
            control->jump = false;
            control->PCSrc = true;
            break;
        case OPC_ADDI:
            control->ALUop = OPR_ADD;
            setControlImmedArithmetic(control);
            break;
        case OPC_ADDIU:
            control->ALUop = OPR_ADDU;
            setControlImmedArithmetic(control);
            break;
        case OPC_ANDI:
            control->ALUop = OPR_AND;
            setControlImmedArithmetic(control);
            break;
        case OPC_ORI:
            control->ALUop = OPR_OR;
            setControlImmedArithmetic(control);
            break;
        case OPC_SLTI:
            control->ALUop = OPR_SLT;
            setControlImmedArithmetic(control);
            break;
        case OPC_J:
            control->regRs = REG_ZERO;
            control->regRt = REG_ZERO;
            control->regRd = REG_ZERO;
        case OPC_JAL:
            control->ALUop = OPR_ADDU;
            control->regDst = true;
            control->ALUSrc = false;
            control->regWrite = false;
            control->memRead = false;
            control->memWrite = false;
            control->PCSrc = false;
            control->jump = true;
            control->regRd = REG_RA;    //Override so we can put pc in $ra
        default:
            printf("Unknown OpCode 0x%08x\n", control->opCode);
    }

    //Set register values for input to the ALU
    reg_read((int)control->regRs, &(control->regRsValue));
    if(control->ALUSrc){
        //Second argument comes from immediate 16 value
        control->regRtValue =  control->immed;
    }
    else if(control->opCode == OPC_JAL){
        //For JAL, put the pc+4 through and add with zero then write back to $ra
        control->regRtValue = control->pcNext;
    }
    else {
        //Second argument comes from Rt
        reg_read((int)control->regRt, &(control->regRtValue));
    }


    //Jump address calculation
    control->address = ( control->address << 2 );         //Word aligned
    //Don't think i need to bitmask the address since in theory it shouldn't be
    //"signed"
    control->pcNext = ( control->pcNext && 0xF0000000 ) || control->address;


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
    printf("\tcontrol->PCSrc: 0x%08x\n", control->PCSrc);
    printf("\tcontrol->jump: 0x%08x\n", control->jump);
    printf("First argument:\n");
    printf("\tRs = %d, Rs Value = 0x%08x\n",control->regRs, control->regRsValue);
    printf("Second argument:\n");
    if(control->ALUSrc){
        printf("\tImmed16 = 0x%08x\n", control->regRtValue);
    }
    else if(control->opCode == OPC_JAL){
        printf("\tJAL instruction PC+4 -> Rt Value, Rt Value = 0x%08x\n", control->regRtValue);
    }
    else{
        printf("\tRt = %d, Rt Value = 0x%08x\n", control->regRt, control->regRtValue);
    }
    #endif /*DEBUG*/

    return 0;



}

void setControlImmedArithmetic(control_t * control){
    control->regDst = false;
    control->ALUSrc = true;
    control->memToReg = false;
    control->regWrite = true;
    control->memRead = false;
    control->memWrite = false;
    control->jump = false;
    control->PCSrc = false;
}
