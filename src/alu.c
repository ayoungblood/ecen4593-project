/* src/alu.c
 * Wraps all the ALU functions
 */

#include "alu.h"

// Perform an ALU operation
int alu(operation_t operation, word_t op_rs, word_t op_rt, word_t *result, bool *zero) {
    int64_t  xs;
    //uint64_t xu;
    switch(operation) {
        case OPR_ADD:
            // rd <= rs + rt
            xs = (int32_t)op_rs + (int32_t)op_rt;
            if (!OVERFLOW(op_rs,op_rt,xs)) {
                *result = (int32_t)(xs & 0xffffffff);
            } else {
                return 1;
            }
            break;
        case OPR_SUB:
            //
            break;
        default:

            break;
    }
    // Set the zero flag
    *zero = (*result == 0x0)?true:false;
    return 0;
}
