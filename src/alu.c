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
        case OPR_SUB:
            // rd <= rs - rt
            // "The 32-bit word value in GPR rt is subtracted from the 32-bit value in
            // GPR rs to produce a 32-bit result. If the subtraction results in 32-bit
            // 2’s complement arithmetic overflow then the destination register is not
            // modified and an Integer Overflow exception occurs. If it does not
            // overflow, the 32-bit result is placed into GPR rd."
            temp = (int32_t)op_rs - (int32_t)op_rt;
            if (!SUB_OVERFLOW(op_rs,op_rt,temp)) {
                printf("Subtract, no overflow. rs: %d, rt: %d, temp: %d; rs.31: %d, rt.31: %d, temp.31: %d\n",(int32_t)op_rs,(int32_t)op_rt,(int32_t)temp,BIT31(op_rs),BIT31(op_rt),BIT31(temp));
                *result = temp;
            } else {
                printf("Subtract, OVERFLOW! rs: %d, rt: %d, temp: %d; rs.31: %d, rt.31: %d, temp.31: %d\n",(int32_t)op_rs,(int32_t)op_rt,(int32_t)temp,BIT31(op_rs),BIT31(op_rt),BIT31(temp));
                return ALU_INTEGER_OVERFLOW;
            }
            break;
        default:
            assert(0); // We should not get here
            break;
    }
    // Set the zero flag
    *zero = (*result == 0x0)?true:false;
    return 0;
}
