/*	src/decode.h
*   Decode functions
*/

#ifndef _DECODE_H
#define _DECODE_H
#include "types.h"

typedef struct {
    int regRs;
    int regRt;
    int regRd;
    int immed;
    int pcNext;
    operation_t op;
    int shamt;

} reg_id_ex_t;

int decode( inst_t , pc_t , reg_id_ex_t * );

#endif /* _DECODE_H */

