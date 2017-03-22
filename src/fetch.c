/* src/fetch.c
* Instruction fetch stage of the pipeline
*/

#include "fetch.h"

extern int flags;

void fetch(control_t * ifid, pc_t * pc){

    //Read the instruction at the current program counter
    mem_read_w(*pc, &(ifid->instr));
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "FETCH:\n" ANSI_C_RESET);
        printf("\tretrieved instruction 0x%08x at 0x%08x\n", ifid->instr, *pc);
    }

    //Break the instruction into the specific fields
    ifid->opCode = ( ifid->instr & OP_MASK ) >> OP_SHIFT;
    ifid->regRs = ( ifid->instr & RS_MASK ) >> RS_SHIFT;
    ifid->regRt = ( ifid->instr & RT_MASK ) >> RT_SHIFT;
    ifid->regRd = ( ifid->instr & RD_MASK ) >> RD_SHIFT;
    ifid->shamt = ( ifid->instr & SH_MASK ) >> SH_SHIFT;
    ifid->funct = ( ifid->instr & FC_MASK );
    ifid->address = ( ifid->instr & AD_MASK );
    uint32_t immed = ( ifid->instr & IM_MASK );

    //Sign extension of the immediate field
    ifid->immed = (( ifid->instr & BIT15 ) && (ifid->opCode != OPC_SLTIU) && (ifid->opCode != OPC_ANDI) && (ifid->opCode != OPC_ORI) && (ifid->opCode != OPC_XORI)) ? immed | EXT_16_32 : immed;

    //Update the program counter by 4
    ifid->pcNext = *pc + 4;

}
