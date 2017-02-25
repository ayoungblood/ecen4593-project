/*	src/decode.h
*   Decode functions
*/

#ifndef _DECODE_H
#define _DECODE_H
#include "types.h"


int decode( inst_t , pc_t , reg_id_ex_t * );

//Instruction decoding bitmasks
#define OP_MASK 0xFC000000
#define RS_MASK 0x03E00000
#define RT_MASK 0x001F0000
#define RD_MASK 0x0000F800
#define SH_MASK 0x000007C0
#define FC_MASK 0x0000003F
#define AD_MASK 0x03FFFFFF
#define IM_MASK 0x0000FFFF

//Instruction decoding bitshifts
#define OP_SHIFT 26
#define RS_SHIFT 21
#define RT_SHIFT 16
#define RD_SHIFT 11
#define SH_SHIFT 6

//Sign Extenstion
#define BIT15 0x00008000
#define EXT_16_32 0xFFFF0000


#endif /* _DECODE_H */

