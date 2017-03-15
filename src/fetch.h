/* src/fetch.h
* Instruction fetch stage of the pipeline
*/

#ifndef _FETCH_H
#define _FETCH_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "util.h"
#include "main_memory.h"

void fetch(control_t *, pc_t * pc, bool *stall);

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


#endif /* _FETCH_H */
