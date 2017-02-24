/* src/types.h
 * Internal types for the simulator
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// Represents a 32-bit instruction
typedef uint32_t inst_t;
// Represents a 32-bit program counter
typedef uint32_t pc_t;

// Mapping opcode values to mnemonic
typedef enum OpCodes {
    RTYPE = 0x0,
    ADDI = 0x8,
    ADDIU = 0x9,
    ANDI = 0xc,
    BEQ = 0x4,
    BNE = 0x5,
    J = 0x2,
    JAL = 0x3,
    LBU = 0x24,
    LHU = 0x25,
    LL = 0x30,
    LUI = 0xf,
    LW = 0x23,
    ORI = 0xd,
    SLTI = 0xa,
    SLTIU = 0xb,
    SB = 0x28,
    SC = 0x38,
    SH = 0x29,
    SW = 0x2b
} opcode_t;

// Mapping funct values to mnemonic (R-type, opcode=0x0)
typedef enum FunctCodes {
    ADD = 0x20,
    ADDU = 0x21,
    AND = 0x24,
    JR = 0x8,
    NOR = 0x27,
    OR = 0x25,
    SLT = 0x2a,
    SLTU = 0x2b,
    SLL = 0x00,
    SRL = 0x02
} funct_t;

#endif /* TYPES_H */
