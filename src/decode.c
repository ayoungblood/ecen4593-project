/* src/decode.c
 * Instruction decode stage of the pipeline
 */

#include "decode.h"

extern int flags; // from main.c or memory-test.c

int decode( control_t * ifid , control_t * idex) {

    copy_pipeline_register(ifid, idex);

    switch(idex->opCode){
        case OPC_RTYPE:
            switch(idex->funct){
                case FNC_ADD:
                    idex->ALUop = OPR_ADD;
                    break;
                case FNC_ADDU:
                    idex->ALUop = OPR_ADDU;
                    break;
                case FNC_AND:
                    idex->ALUop = OPR_AND;
                    break;
                case FNC_NOR:
                    idex->ALUop = OPR_NOR;
                    break;
                case FNC_OR:
                    idex->ALUop = OPR_OR;
                    break;
                case FNC_SLL:
                    idex->ALUop = OPR_SLL;
                    break;
                case FNC_SRL:
                    idex->ALUop = OPR_SRL;
                    break;
                case FNC_SLT:
                    idex->ALUop = OPR_SLT;
                    break;
                case FNC_SLTU:
                    idex->ALUop = OPR_SLTU;
                    break;
                case FNC_SUB:
                    idex->ALUop = OPR_SUB;
                    break;
                case FNC_SUBU:
                    idex->ALUop = OPR_SUBU;
                    break;
                case FNC_XOR:
                    idex->ALUop = OPR_XOR;
                    break;
                default:
                    printf(ANSI_C_RED "Illegal R-type instruction, funct 0x%02x (instruction 0x%08x). Halting.\n" ANSI_C_RESET, idex->funct, idex->instr);
                    assert(0);
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
        case OPC_XORI:
            idex->ALUop = OPR_XOR;
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
            printf(ANSI_C_RED "Illegal instruction, opcode 0x%02x (instruction 0x%08x). Halting.\n" ANSI_C_RESET, idex->opCode,idex->instr);
            assert(0);
    }


    //Set register values for input to the ALU
    reg_read((int)(idex->regRs), &(idex->regRsValue));
    reg_read((int)(idex->regRt), &(idex->regRtValue));


    //Jump address calculation
    idex->address = ( idex->address << 2 );         //Word aligned
    //Don't think i need to bitmask the address since in theory it shouldn't be
    //"signed"
    if(idex->jump){
        idex->pcNext = ( idex->pcNext & 0xF0000000 ) | idex->address;
    }
    //branch determination in ID phase
    idex->pcNext = idex->pcNext + ( idex->immed << 2 );
    if(idex->opCode == OPC_BEQ){
        if(idex->regRsValue == idex->regRtValue){
            idex->PCSrc = true; //Branch is taken, use pcNext for address
        }
        else{
            idex->PCSrc = false; //Branch not taken
        }
    }
    else if (idex->opCode == OPC_BNE){
        if(idex->regRsValue != idex->regRtValue){
            idex->PCSrc = true;  //Branch taken
        }
        else{
            idex->PCSrc = false; //Branch not taken
        }
    }


    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "DECODE: \n" ANSI_C_RESET);
        print_pipeline_register(idex);
    }

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
