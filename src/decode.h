/*
*   src/decode.h
*/

#include <stdint.h>

typedef struct {
    int regRs;
    int regRt;
    int regRd;
    int immed;
    int pcNext;
    int inst;
    
} reg_id_ex_t;

int decode( inst_t , pc_t , reg_id_ex_t * );

