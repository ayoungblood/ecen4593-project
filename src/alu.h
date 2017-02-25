/* src/alu.h
 * ALU functions
 */

#ifndef _ALU_H
#define _ALU_H

#include <stdint.h>
#include <stdbool.h>

#include "types.h"

int alu(operation_t operation, word_t op_a, word_t op_b, word_t *result, bool *zero);

// Detecting overflow based on AVR instruction set
// http://www.atmel.com/images/Atmel-0856-AVR-Instruction-Set-Manual.pdf
#define BIT31 (1<<31)
#define OVERFLOW(rs,rt,x) (((rs & BIT31) && (rt & BIT31) && ~(x & BIT31)) || (~(rs & BIT31) && ~(rt & BIT31) && (x & BIT31)))
#define PANIC() printf("ALU Overflow. Send help\n")

#endif /* _ALU_H */
