/* src/alu.c
 * ALU function wrapper and execute pipeline stage implementation
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
    else {
        //Second argument comes from Rt
        ALUArg2 = idex->regRtValue;
    }

    alu(idex->ALUop, ALUArg1, ALUArg2, idex->shamt, &ALUresult, &zero);

    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "EXECUTE: \n" ANSI_C_RESET);
        printf("\tInstruction: 0x%08x\n", idex->instr);
        printf("\tALUOp: 0x%08x\n", idex->ALUop);
        printf("\tArg1: 0x%08x, Arg2: 0x%08x\n", ALUArg1, ALUArg2);
        printf("\tshamt: %d (10)\n", idex->shamt);
        printf("\tALUresult: 0x%08x\n", ALUresult);
    }

    //Copy the results into the next pipeline register
    copy_pipeline_register(idex, exmem);
    exmem->ALUresult = ALUresult;


    return 0;
}

// Perform an ALU operation
int alu(operation_t operation, word_t op_rs, word_t op_rt, word_t shamt, word_t *result, bool *zero) {
    int32_t temp;
    switch (operation) {
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
        case OPR_SLL:
            // rd = rt << shamt
            *result = op_rt << shamt;
            break;
        case OPR_SRL:
            // rd = rt >> shamt
            *result = op_rt >> shamt;
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
        case OPR_XOR:
            // rd <= rs XOR rt
            // No integer overflow occurs under any circumstances
            *result = op_rs ^ op_rt;
            break;
        default: // We should not get here. Complain and crash.
            printf(ANSI_C_RED "Illegal ALU operation %d. Halting.\n" ANSI_C_RESET, operation);
            assert(0);
            break;
    }
    // Set the zero flag
    *zero = (*result == 0x0)?true:false;
    return 0;
}
