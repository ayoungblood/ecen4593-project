/*  src/decode.c
*   Decode instruction stage of the CPU
*/

#include "decode.h"

extern int flags; // from main.c or memory-test.c


int decode( control_t * ifid , pc_t * pc , control_t * idex ) {

    //Get all of the bitmasked values out of the instruction
    idex->opCode = ( ifid->instr & OP_MASK ) >> OP_SHIFT;
    idex->regRs = ( ifid->instr & RS_MASK ) >> RS_SHIFT;
    idex->regRt = ( ifid->instr & RT_MASK ) >> RT_SHIFT;
    idex->regRd = ( ifid->instr & RD_MASK ) >> RD_SHIFT;
    idex->shamt = ( ifid->instr & SH_MASK ) >> SH_SHIFT;
    idex->funct = ( ifid->instr & FC_MASK );
    idex->address = ( ifid->instr & AD_MASK );
    uint32_t immed = ( ifid->instr & IM_MASK );

    idex->pcNext = ifid->pcNext;

    //Sign extension of the immediate field
    idex->immed = (( ifid->instr & BIT15 ) && (idex->opCode != OPC_SLTIU)) ? immed | EXT_16_32 : immed;

    switch(idex->opCode){
        case OPC_RTYPE:
            switch(idex->funct){
                case FNC_ADD:
                    idex->ALUop = OPR_ADD;
                    break;
                case FNC_SUB:
                    idex->ALUop = OPR_SUB;
                    break;
                case FNC_AND:
                    idex->ALUop = OPR_AND;
                    break;
                case FNC_OR:
                    idex->ALUop = OPR_OR;
                    break;
                case FNC_SLT:
                    idex->ALUop = OPR_SLT;
                    break;
                case FNC_NOR:
                    idex->ALUop = OPR_NOR;
                    break;
                default:
                    printf("Unknown R-Type instruction 0x%08x\n", idex->funct);
            }
            idex->regDst = true;
            idex->ALUSrc = false;
            idex->memToReg = false;
            idex->regWrite = true;
            idex->memRead = false;
            idex->memWrite = false;
            idex->jump = false;
            idex->PCSrc = false;
            break;
        case OPC_LW:
        case OPC_LHU:
        case OPC_LBU:
            idex->ALUop = OPR_ADDU;
            setidexLoad(idex);
            break;
        case OPC_SW:
        case OPC_SH:
        case OPC_SB:
            idex->ALUop = OPR_ADDU;
            setidexStore(idex);
            break;
        case OPC_BEQ:
        case OPC_BNE:
            idex->ALUop = OPR_SUB;
            idex->ALUSrc = false;
            idex->regWrite = false;
            idex->memRead = false;
            idex->memWrite = false;
            idex->jump = false;
            idex->PCSrc = true;
            break;
        case OPC_ADDI:
            idex->ALUop = OPR_ADD;
            setidexImmedArithmetic(idex);
            break;
        case OPC_ADDIU:
            idex->ALUop = OPR_ADDU;
            setidexImmedArithmetic(idex);
            break;
        case OPC_ANDI:
            idex->ALUop = OPR_AND;
            setidexImmedArithmetic(idex);
            break;
        case OPC_ORI:
            idex->ALUop = OPR_OR;
            setidexImmedArithmetic(idex);
            break;
        case OPC_SLTI:
            idex->ALUop = OPR_SLT;
            setidexImmedArithmetic(idex);
            break;
        case OPC_SLTIU:
            idex->ALUop = OPR_SLTU;
            setidexImmedArithmetic(idex);
            break;
        case OPC_J:
            idex->ALUop = OPR_ADDU;
            idex->regRs = REG_ZERO;
            idex->regRt = REG_ZERO;
            idex->regRd = REG_ZERO;
            idex->regDst = true;
            idex->ALUSrc = false;
            idex->regWrite = true;
            idex->memRead = false;
            idex->memWrite = false;
            idex->memToReg = false;
            idex->PCSrc = false;
            idex->jump = true;
            break;
        case OPC_JAL:
            idex->ALUop = OPR_ADDU;
            idex->regRs = REG_ZERO;
            idex->regRt = REG_ZERO;
            idex->regRd = REG_RA;    //Override so we can put pc in $ra
            idex->regDst = true;
            idex->ALUSrc = false;
            idex->regWrite = true;
            idex->memRead = false;
            idex->memWrite = false;
            idex->memToReg = false;
            idex->PCSrc = false;
            idex->jump = true;

        default:
            printf("Unknown OpCode 0x%08x\n", idex->opCode);
    }

    //Set register values for input to the ALU
    reg_read((int)(idex->regRs), &(idex->regRsValue));
    if(idex->ALUSrc){
        //Second argument comes from immediate 16 value
        idex->regRtValue =  idex->immed;
    }
    else if(idex->opCode == OPC_JAL){
        //For JAL, put the pc+4 through and add with zero then write back to $ra
        idex->regRtValue = idex->pcNext;
    }
    else {
        //Second argument comes from Rt
        reg_read((int)(idex->regRt), &(idex->regRtValue));
    }


    //Jump address calculation
    idex->address = ( idex->address << 2 );         //Word aligned
    //Don't think i need to bitmask the address since in theory it shouldn't be
    //"signed"
    if(idex->jump){
        idex->pcNext = ( idex->pcNext & 0xF0000000 ) | idex->address;
        //Update the actual program counter
        *pc = idex->pcNext;
    }
    //branch determination in ID phase
    if(idex->opCode == OPC_BEQ){
        if(idex->regRsValue == idex->regRtValue){
            idex->pcNext = idex->pcNext + idex->immed;
            idex->PCSrc = true; //Branch is taken, use pcNext for address
            //Update the actual program counter
            *pc = ifid->pcNext;
        }
        else{
            idex->PCSrc = false; //Branch not taken
        }
    }
    else if (idex->opCode == OPC_BNE){
        if(idex->regRsValue != idex->regRtValue){
            idex->pcNext = (int32_t)idex->pcNext + (int32_t)(idex->immed * 4);
            idex->PCSrc = true;  //Branch taken
            //Update the actual program counter
            *pc = ifid->pcNext;
        }
        else{
            idex->PCSrc = false; //Branch not taken
        }
    }


    if(flags & MASK_DEBUG){
        printf("Decoded idex register from instruction 0x%08x\n", ifid->instr);
        printf("Decoded Instrunction: \n");
        printf("\tidex->opCode: 0x%08x\n", idex->opCode);
        printf("\tidex->regRs: 0x%08x\n", idex->regRs);
        printf("\tidex->regRt: 0x%08x\n", idex->regRt);
        printf("\tidex->regRd: 0x%08x\n", idex->regRd);
        printf("\tidex->shamt: 0x%08x\n", idex->shamt);
        printf("\tidex->funct: 0x%08x\n", idex->funct);
        printf("\tidex->immed: 0x%08x\n", idex->immed);
        printf("\tidex->address: 0x%08x\n", idex->address);
        printf("\tidex->pcNext: 0x%08x\n", idex->pcNext);
        printf("Global Program Counter:\n");
        printf("\t0x%08x\n", *pc);
        printf("idex bits:\n");
        printf("\tidex->regDst: 0x%08x\n", idex->regDst);
        printf("\tidex->ALUSrc: 0x%08x\n", idex->ALUSrc);
        printf("\tidex->memToReg: 0x%08x\n", idex->memToReg);
        printf("\tidex->regWrite: 0x%08x\n", idex->regWrite);
        printf("\tidex->memRead: 0x%08x\n", idex->memRead);
        printf("\tidex->memWrite: 0x%08x\n", idex->memWrite);
        printf("\tidex->ALUop: 0x%08x\n", idex->ALUop);
        printf("\tidex->PCSrc: 0x%08x\n", idex->PCSrc);
        printf("\tidex->jump: 0x%08x\n", idex->jump);
        printf("First argument:\n");
        printf("\tRs = %d, Rs Value = 0x%08x\n",idex->regRs, idex->regRsValue);
        printf("Second argument:\n");
        if(idex->ALUSrc){
            printf("\tImmed16 = 0x%08x\n", idex->regRtValue);
        }
        else if(idex->opCode == OPC_JAL){
            printf("\tJAL instruction PC+4 -> Rt Value, Rt Value = 0x%08x\n", idex->regRtValue);
        }
        else{
            printf("\tRt = %d, Rt Value = 0x%08x\n", idex->regRt, idex->regRtValue);
        }
    } /*DEBUG*/

    return 0;



}

void setidexImmedArithmetic(control_t * idex){
    idex->regDst = false;
    idex->ALUSrc = true;
    idex->memToReg = false;
    idex->regWrite = true;
    idex->memRead = false;
    idex->memWrite = false;
    idex->jump = false;
    idex->PCSrc = false;
}


void setidexLoad(control_t * idex){
    idex->regDst = false;
    idex->ALUSrc = true;
    idex->memToReg = true;
    idex->regWrite = true;
    idex->memRead = true;
    idex->memWrite = false;
    idex->jump = false;
    idex->PCSrc = false;
}

void setidexStore(control_t * idex){
    idex->ALUSrc = true;
    idex->regWrite = false;
    idex->memRead = false;
    idex->memWrite = true;
    idex->jump = false;
    idex->PCSrc = false;
}
