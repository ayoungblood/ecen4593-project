/* src/alu.h
 * ALU functions
 */

#ifndef _ALU_H
#define _ALU_H

#include <stdint.h>
#include <stdbool.h>

#include "types.h"

int alu(operation_t operation, word_t op_a, word_t op_b, word_t *result, bool *zero);

#define OVERFLOW(x) (x & 0xffffffff00000000)
#define PANIC() printf("ALU Overflow. Send help\n")

#endif /* _ALU_H */
