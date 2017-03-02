/* src/types.h
 * Internal types for the simulator
 */

#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <stdbool.h>

// Represents a 32-bit instruction
typedef uint32_t inst_t;
// Represents a 32-bit program counter
typedef uint32_t pc_t;
// Represents a single word (32b) of memory, with ambiguous signedness
typedef uint32_t word_t;

// Mapping opcode values to mnemonic
typedef enum OpCodes {
    OPC_RTYPE   = 0x00,
    OPC_ADDI    = 0x08,
    OPC_ADDIU   = 0x09,
    OPC_ANDI    = 0x0c,
    OPC_BEQ     = 0x04,
    OPC_BNE     = 0x05,
    OPC_J       = 0x02,
    OPC_JAL     = 0x03,
    OPC_LBU     = 0x24,
    OPC_LHU     = 0x25,
    OPC_LL      = 0x30,
    OPC_LUI     = 0x0f,
    OPC_LW      = 0x23,
    OPC_ORI     = 0x0d,
    OPC_SLTI    = 0x0a,
    OPC_SLTIU   = 0x0b,
    OPC_SB      = 0x28,
    OPC_SC      = 0x38,
    OPC_SH      = 0x29,
    OPC_SW      = 0x2b
} opcode_t;

// Mapping funct values to mnemonic (R-type, opcode=0x0)
typedef enum FunctCodes {
    FNC_ADD     = 0x20,
    FNC_ADDU    = 0x21,
    FNC_AND     = 0x24,
    FNC_JR      = 0x08,
    FNC_NOR     = 0x27,
    FNC_OR      = 0x25,
    FNC_SLT     = 0x2a,
    FNC_SLTU    = 0x2b,
    FNC_SLL     = 0x00,
    FNC_SRL     = 0x02,
    FNC_SUB     = 0x22
} funct_t;

// Enumerate all "operations" (R/J/I type instruction action)
// See http://alumni.cs.ucr.edu/~vladimir/cs161/mips.html
// These are used internally to represent operations between stages
typedef enum Operations {
    // Arithmetic and logical
    OPR_ADD,
    OPR_ADDU,
    OPR_ADDI,
    OPR_ADDIU,
    OPR_AND,
    OPR_ANDI,
    OPR_DIV,    // Probably not supported
    OPR_DIVU,   // Probably not supported
    OPR_MULT,
    OPR_MULTU,
    OPR_NOR,
    OPR_OR,
    OPR_ORI,
    OPR_SLL,
    OPR_SLLV,
    OPR_SRA,
    OPR_SRAV,
    OPR_SRL,
    OPR_SRLV,
    OPR_SUB,
    OPR_SUBU,
    OPR_XOR,
    OPR_XORI,
    // Constant-manipulating
    OPR_LHI,
    OPR_LLO,
    // Comparison
    OPR_SLT,
    OPR_SLTU,
    OPR_SLTI,
    OPR_SLTIU,
    // Branch
    OPR_BEQ,
    OPR_BTGZ,
    OPR_BLEZ,
    OPR_BNE,
    // Jump
    OPR_J,
    OPR_JAL,
    OPR_JALR,
    OPR_JR,
    // Load
    OPR_LB,
    OPR_LBU,
    OPR_LH,
    OPR_LHU,
    OPR_LW,
    // Store
    OPR_SB,
    OPR_SH,
    OPR_SW,
    // Data movement
    OPR_MFHI,
    OPR_MFLO,
    OPR_MTHI,
    OPR_MTLO,
    // Exception/interrupt
    OPR_TRAP
} operation_t;



typedef struct CONTROL_REGISTER {
    //These are control register definitions that come from Figure 4.16 on page 264 of the Hennessy textbook
    bool regDst;        //regDst ? destination register is Rd : destination register is Rt
    bool regWrite;      //TODO:regWrite ? Register on the write register input is written with the value of the Write data input : nothing
    bool ALUSrc;        //ALUSrc ? The second ALU operand comes from Immediate 16 : The second ALU operand comes from Rt
    bool PCSrc;         //PCSrc ? PC replaced branch targer calculation : PC is replace by output of adder (PC + 4)
    bool memRead;       //memRead ? Data memory contents given by address input are put on Read data output : Nothing
    bool memWrite;      //memWrite ? Data memory contents designated by the address input replace by data on Write data input : Nothing
    bool memToReg;      //memToReg ? Value from Write data input comes from the data memory : value fed to register Write data input comes from ALU
    operation_t ALUop;  //ALU operation
    bool jump;          //Override PC with shifted and concatenated address

    opcode_t opCode;
    uint32_t regRs;
    uint32_t regRt;
    uint32_t regRd;
    uint32_t immed;
    uint32_t address;
    funct_t funct;
    uint32_t shamt;

    uint32_t regRsValue;
    uint32_t regRtValue;

    uint32_t ALUresult;
    uint32_t pcNext;

} control_t;


#endif /* _TYPES_H */
