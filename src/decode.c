/* src/decode.c
 * Instruction decode stage of the pipeline
 */

#include "decode.h"

extern int flags; // from main.c or memory-test.c

int decode( control_t * ifid , control_t * idex) {

    copy_pipeline_register(ifid, idex);

    switch(idex->opCode){
        case OPC_RTYPE:
            idex->regDst = true;
            idex->ALUSrc = false;
            idex->memToReg = false;
            idex->regWrite = true;
            idex->memRead = false;
            idex->memWrite = false;
            idex->jump = false;
            idex->PCSrc = false;
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
                case FNC_JR:
                    idex->ALUop = OPR_ADD;
                    idex->jump = true;
                    //Make sure we don't accidentally write back, although rd = $zero
                    idex->regWrite = false;
                    break;
                case FNC_MOVZ:
                    idex->ALUop = OPR_MOVZ;
                    break;
                case FNC_MOVN:
                    idex->ALUop = OPR_MOVN;
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

            break;
        case OPC_LW:
        case OPC_LH:
        case OPC_LB:
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
        case OPC_BLTZ:
        case OPC_BGTZ:
        case OPC_BLEZ:
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
            break;
        case OPC_LUI:
            idex->ALUop = OPR_SLL;
            idex->regDst = false;
            idex->ALUSrc = true;
            idex->regWrite = true;
            idex->memRead = false;
            idex->memWrite = false;
            idex->memToReg = false;
            idex->PCSrc = false;
            idex->jump = false;
            idex->shamt = 16; // fixed shift amount
            break;
        case OPC_SPECIAL3:
            // R-type:   | SPECIAL  | RS | RT | RD | 0   | ADDU (funct) |
            // Special3: | SPECIAL3 | 0  | RT | RD | SEB | BSHFL        |
            // For SPECIAL3, the mapping is like R-type, but the names are different:
            // OpCode is still the OpCode,
            // Rs is zero
            // Rt is Rt
            // Rd is Rd
            // shamt in R-type is the BSHFL code in Special3
            // funct in R-type is the special3 code in Special3
            // Despite being in a whole weird class of instructions,
            // SEB/SEH are implemented as R-type and handled by the ALU
            idex->regDst = true;
            idex->ALUSrc = false;
            idex->memToReg = false;
            idex->regWrite = true;
            idex->memRead = false;
            idex->memWrite = false;
            idex->jump = false;
            idex->PCSrc = false;
            switch (idex->funct) {
                case SP3_BSHFL:
                    switch (idex->shamt) {
                        case BSHFL_SEB:
                            idex->ALUop = OPR_SEB;
                            break;
                        case BSHFL_SEH:
                            idex->ALUop = OPR_SEH;
                            break;
                        default:
                            printf(ANSI_C_RED "Illegal SPECIAL3-type instruction, funct (special3) 0x%02x, shamt (BSHFL) 0x%02x (instruction 0x%08x). Halting.\n" ANSI_C_RESET, idex->funct, idex->shamt, idex->instr);
                            assert(0);
                            break; // never reached
                    }
                    break;
                default:
                    printf(ANSI_C_RED "Illegal SPECIAL3-type instruction, funct (special3) 0x%02x, shamt (BSHFL) 0x%02x (instruction 0x%08x). Halting.\n" ANSI_C_RESET, idex->funct, idex->shamt, idex->instr);
                    assert(0);
                    break; // never reached
            }
            break;
        default:
            printf(ANSI_C_RED "Illegal instruction, opcode 0x%02x (instruction 0x%08x). Halting.\n" ANSI_C_RESET, idex->opCode, idex->instr);
            assert(0);
            break; // never reached
    }


    // Set register values for input to the ALU
    reg_read((int)(idex->regRs), &(idex->regRsValue));
    reg_read((int)(idex->regRt), &(idex->regRtValue));
    // Load ALUresult so that ALUresult can remain "unmodified" for MOVZ/MOVN
    // This is needed if a MOVZ/MOVN result needs to be forwarded
    reg_read((int)(idex->regRd), &(idex->ALUresult));

    //Jump address calculation
    idex->address = ( idex->address << 2 );         //Word aligned
    //Don't think i need to bitmask the address since in theory it shouldn't be
    //"signed"
    if(idex->jump && (idex->opCode != OPC_RTYPE)){
        idex->regRtValue = idex->pcNext; // RA value goes into ALU, gets added to zero to set RA
        idex->pcNext = ( idex->pcNext & 0xF0000000 ) | idex->address;
    }
    else if(idex->jump && (idex->opCode == OPC_RTYPE)){
        //This is a jr instruction, pc comes from rs
        idex->pcNext = idex->regRsValue;
    }
    else {
        //branch determination in ID phase, dont want to overwrite jump
        idex->pcNext = idex->pcNext + ( idex->immed << 2 );
    }
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
    else if (idex->opCode == OPC_BLTZ){
        if((int)idex->regRsValue < 0){
            idex->PCSrc = true;
        }
        else{
            idex->PCSrc = false;
        }
    }
    else if (idex->opCode == OPC_BGTZ){
        if((int)idex->regRsValue > 0){
            idex->PCSrc = true;
        }
        else{
            idex->PCSrc = false;
        }
    }
    else if (idex->opCode == OPC_BLEZ){
        if((int)idex->regRsValue <= 0){
            idex->PCSrc = true;
        }
        else{
            idex->PCSrc = false;
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
