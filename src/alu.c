/* src/alu.c
 * Wraps all the ALU functions
 * ALU operations:
 *  - add
 *  - and
 *  - nor
 *  - or
 *  - sub
 */

#include "alu.h"

extern int flags; // from main.c

// Wrapper function
int execute(control_t *idex, control_t *exmem){
    word_t ALUArg1;
    word_t ALUArg2;
    word_t ALUresult;
    bool zero;

    ALUArg1 = idex->regRsValue;

    //ALUSrc multiplexor
    if(idex->ALUSrc){
        //Second argument comes from immediate 16 value
        ALUArg2 =  idex->immed;
    }
    else if(idex->opCode == OPC_JAL){
        //For JAL, put the pc+4 through and add with zero then write back to $ra
        ALUArg2 = idex->pcNext;
    }
    else {
        //Second argument comes from Rt
        ALUArg2 = idex->regRtValue;
    }

    alu(idex->ALUop, ALUArg1, ALUArg2, &ALUresult, &zero);

    //Copy the results into the next pipeline register
    exmem->regDst = idex->regDst;
    exmem->regWrite = idex->regWrite;
    exmem->ALUSrc = idex->ALUSrc;
    exmem->PCSrc = idex->PCSrc;
    exmem->memRead = idex->memRead;
    exmem->memWrite = idex->memWrite;
    exmem->memToReg = idex->memToReg;
    exmem->ALUop = idex->ALUop;
    exmem->jump = idex->jump;
    exmem->instr = idex->instr;
    exmem->opCode = idex->opCode;
    exmem->regRs = idex->regRs;
    exmem->regRt = idex->regRt;
    exmem->regRd = idex->regRd;
    exmem->immed = idex->immed;
    exmem->address = idex->address;
    exmem->funct = idex->funct;
    exmem->shamt = idex->shamt;
    exmem->regRsValue = idex->regRsValue;
    exmem->regRtValue = idex->regRtValue;
    exmem->ALUresult = ALUresult;
    exmem->pcNext = idex->pcNext;

    if(flags & MASK_DEBUG){
        printf("Decoded exmem register from instruction 0x%08x\n", exmem->instr);
        printf("Decoded Instrunction: \n");
        printf("\texmem->opCode: 0x%08x\n", exmem->opCode);
        printf("\texmem->regRs: 0x%08x\n", exmem->regRs);
        printf("\texmem->regRt: 0x%08x\n", exmem->regRt);
        printf("\texmem->regRd: 0x%08x\n", exmem->regRd);
        printf("\texmem->shamt: 0x%08x\n", exmem->shamt);
        printf("\texmem->funct: 0x%08x\n", exmem->funct);
        printf("\texmem->immed: 0x%08x\n", exmem->immed);
        printf("\texmem->address: 0x%08x\n", exmem->address);
        printf("\texmem->pcNext: 0x%08x\n", exmem->pcNext);
        printf("\texmem->regRtValue: 0x%08x\n", exmem->regRtValue);
        printf("\texmem->regRsValue: 0x%08x\n", exmem->regRsValue);
        printf("\texmem->ALUresult: 0x%08x\n", exmem->ALUresult);
        printf("Global Program Counter:\n");
        printf("exmem bits:\n");
        printf("\texmem->regDst: 0x%08x\n", exmem->regDst);
        printf("\texmem->ALUSrc: 0x%08x\n", exmem->ALUSrc);
        printf("\texmem->memToReg: 0x%08x\n", exmem->memToReg);
        printf("\texmem->regWrite: 0x%08x\n", exmem->regWrite);
        printf("\texmem->memRead: 0x%08x\n", exmem->memRead);
        printf("\texmem->memWrite: 0x%08x\n", exmem->memWrite);
        printf("\texmem->ALUop: 0x%08x\n", exmem->ALUop);
        printf("\texmem->PCSrc: 0x%08x\n", exmem->PCSrc);
        printf("\texmem->jump: 0x%08x\n", exmem->jump);

    }


    return 0;
}



// Perform an ALU operation
int alu(operation_t operation, word_t op_rs, word_t op_rt, word_t *result, bool *zero) {
    int32_t temp;
    //uint64_t xu;
    switch(operation) {
        case OPR_ADD:
            // rd <= rs + rt
            // "The 32-bit word value in GPR rt is added to the 32-bit value in GPR rs
            // to produce a 32-bit result. If the addition results in 32-bit 2’s
            // complement arithmetic overflow then the destination register is not
            // modified and an Integer Overflow exception occurs. If it does not
            // overflow, the 32-bit result is placed into GPR rd."
            temp = (int32_t)op_rs + (int32_t)op_rt;
            if (!(ADD_OVERFLOW(op_rs,op_rt,temp))) {
                //printf("Add, no overflow. rs: %d, rt: %d, temp: %d; rs.31: %d, rt.31: %d, temp.31: %d\n",(int32_t)op_rs,(int32_t)op_rt,(int32_t)temp,BIT31(op_rs),BIT31(op_rt),BIT31(temp));
                *result = temp;
            } else {
                //printf("Add, OVERFLOW! rs: %d, rt: %d, temp: %d; rs.31: %d, rt.31: %d, temp.31: %d\n",(int32_t)op_rs,(int32_t)op_rt,(int32_t)temp,BIT31(op_rs),BIT31(op_rt),BIT31(temp));
                return ALU_INTEGER_OVERFLOW;
            }
            break;
        case OPR_ADDU:
            // rd <= rs + rt
            // No integer overflow occurs under any circumstances
            *result = (int32_t)op_rs + (int32_t)op_rt;
            break;
        case OPR_AND:
            // rd <= rs AND rt
            *result = op_rs & op_rt;
            break;
        case OPR_NOR:
            // rd <= rs NOR rt
            *result = ~(op_rs | op_rt);
            break;
        case OPR_OR:
            // rd <= rs OR rt
            *result = op_rs | op_rt;
            break;
        case OPR_SLT:
            // rd <= (rs < rt)
            *result = ((int32_t)op_rs)<((int32_t)op_rt)?1:0;
            break;
        case OPR_SLTU:
            // rd <= (rs < rt)
            *result = ((uint32_t)op_rs)<((uint32_t)op_rt)?1:0;
            break;
        case OPR_SUB:
            // rd <= rs - rt
            // "The 32-bit word value in GPR rt is subtracted from the 32-bit value in
            // GPR rs to produce a 32-bit result. If the subtraction results in 32-bit
            // 2’s complement arithmetic overflow then the destination register is not
            // modified and an Integer Overflow exception occurs. If it does not
            // overflow, the 32-bit result is placed into GPR rd."
            temp = (int32_t)op_rs - (int32_t)op_rt;
            if (!SUB_OVERFLOW(op_rs,op_rt,temp)) {
                //printf("Subtract, no overflow. rs: %d, rt: %d, temp: %d; rs.31: %d, rt.31: %d, temp.31: %d\n",(int32_t)op_rs,(int32_t)op_rt,(int32_t)temp,BIT31(op_rs),BIT31(op_rt),BIT31(temp));
                *result = temp;
            } else {
                //printf("Subtract, OVERFLOW! rs: %d, rt: %d, temp: %d; rs.31: %d, rt.31: %d, temp.31: %d\n",(int32_t)op_rs,(int32_t)op_rt,(int32_t)temp,BIT31(op_rs),BIT31(op_rt),BIT31(temp));
                return ALU_INTEGER_OVERFLOW;
            }
            break;
        case OPR_SUBU:
            // rd <= rs - rt
            // No integer overflow occurs under any circumstances
            *result = (int32_t)op_rs - (int32_t)op_rt;
            break;
        default:
            assert(0); // We should not get here
            break;
    }
    // Set the zero flag
    *zero = (*result == 0x0)?true:false;
    return 0;
}
