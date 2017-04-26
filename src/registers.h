/* src/registers.h
 * Register file implementation
 */
#ifndef _REGISTERS_H
#define _REGISTERS_H

#include <stdio.h>

#include "types.h"
#include "util.h"

// Initialize the registers
void reg_init(void);
// Print all the register values
void reg_dump(void);

// Read from a register by register number
void reg_read(int reg, word_t *value);
// Write to a register by register number
void reg_write(int reg, word_t *value);

// Mapping register names to register numbers
enum RegNames {
    REG_ZERO    =  0,
    REG_AT      =  1,
    REG_V0      =  2,
    REG_V1      =  3,
    REG_A0      =  4,
    REG_A1      =  5,
    REG_A2      =  6,
    REG_A3      =  7,
    REG_T0      =  8,
    REG_T1      =  9,
    REG_T2      = 10,
    REG_T3      = 11,
    REG_T4      = 12,
    REG_T5      = 13,
    REG_T6      = 14,
    REG_T7      = 15,
    REG_S0      = 16,
    REG_S1      = 17,
    REG_S2      = 18,
    REG_S3      = 19,
    REG_S4      = 20,
    REG_S5      = 21,
    REG_S6      = 22,
    REG_S7      = 23,
    REG_T8      = 24,
    REG_T9      = 25,
    REG_K0      = 26,
    REG_K1      = 27,
    REG_GP      = 28,
    REG_SP      = 29,
    REG_FP      = 30,
    REG_RA      = 31
};

#endif /* _REGISTERS_H */
