/* src/alu.h
 * ALU function wrappers
 */

#ifndef _ALU_H
#define _ALU_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "types.h"
#include "util.h"
/* execute() is a wrapper function that will take the state input of the
 * pipeline register and output the result to the EX/MEM pipeline register
  */
int execute(control_t *, control_t *);

/* alu() performs a specified ALU operation, setting result to the output
 * and setting the zero flag if the output was zero.
 * Returns 0 on success, or signals an exception according to the return
 * values specified below. Crashes the program if an unknown operation
 * is specified
 */
int alu(operation_t operation, word_t op_a, word_t op_b, word_t shamt, word_t *result, bool *zero);

// Detecting overflow based on AVR instruction set
// http://www.atmel.com/images/Atmel-0856-AVR-Instruction-Set-Manual.pdf
#define BIT31(x) (((uint32_t)x)>>31)
// #define OVERFLOW(rs,rt,x) (((rs & BIT31) & (rt & BIT31) & ~(x & BIT31)) | (~(rs & BIT31) & ~(rt & BIT31) & (x & BIT31)))
#define ADD_OVERFLOW(rs,rt,x) ((BIT31(rs)==BIT31(rt) && BIT31(x)^BIT31(rs)))
#define SUB_OVERFLOW(rs,rt,x) ((BIT31(rs)==BIT31(rt) && BIT31(x)^BIT31(rs)))

// alu() signalling return values
#define ALU_INTEGER_OVERFLOW 1

#endif /* _ALU_H */
