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
    FNC_SRL     = 0x02
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


//Pipeline register struct typedefs

//Currently no IF/ID struct, as the only two fields are the instruction and pcnext
//May implement later if needed

//ID/EX pipeline register
typedef struct ID_EX_REG{
    int regRs;          //Register file number for Rs
    int regRt;          //Register file number for Rt
    int regRd;          //Register file number for Rd
    int immed;          //Sign extended immediate 16 value
    int pcNext;         //The address where the NEXT instruction is located
    operation_t op;     //ALU operation, Memory Operation (decoded)
    int shamt;          //Shift amount from instruction
    int regRsVal;       //TODO:Value of the Rs register from the register file
    int regRtVal;       //TODO:Value of the Rt register from the register file
    bool RegDst;        //RegDst ? destination register is Rt : destination register is Rd
    bool RegWrite;      //TODO:RegWrite ? N/A : Register on the write register input is written with the value of the Write data input
    bool ALUSrc;        //ALUSrc ? The second ALU operand comes from Rt : The second ALU operand comes from Immediate 16

} reg_id_ex_t;

//EX/MEM pipeline register
typedef struct EX_MEM_REG {
    int pcNext;         //
    int aluResult;
    int regRtVal;
    int wbReg;
    bool memToReg;      //Should the memory result be written to the register file
} reg_ex_mem_t;


#endif /* _TYPES_H */
