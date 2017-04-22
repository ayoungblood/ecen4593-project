/* src/fetch.c
* Instruction fetch stage of the pipeline
*/

#include "fetch.h"

extern int flags;

void fetch(control_t * ifid, pc_t * pc, cache_config_t *cache_cfg){

    //Read the instruction at the current program counter
    if(cache_cfg->inst_enabled && !(cache_cfg->mode == CACHE_DISABLE)){
        ifid->status = i_cache_read_w(pc, &(ifid->instr));
        if(ifid->status == CACHE_HIT){
            //check to make sure its the same one from memory
            //We will want to remove this check once we are sure the cache works
            uint32_t temp;
            mem_read_w(*pc, &temp);
            if(temp != ifid->instr){
                cprintf(ANSI_C_RED, "Inconsistent data from cache! Data 0x%08x from cache does not match data 0x%08x from memory at address 0x%08x", ifid->instr, temp, *pc);
                assert(0);
            }
        }
    } else {
        mem_read_w(*pc, &(ifid->instr));
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

    if(flags & MASK_DEBUG){
        cprintf(ANSI_C_CYAN, "FETCH:\n");
        if (cache_cfg->inst_enabled && !(cache_cfg->mode == CACHE_DISABLE)) {
            if(ifid->status == CACHE_HIT){
                printf("\tretrieved instruction 0x%08x at 0x%08x\n", ifid->instr, *pc);
            } else {
                printf("\tCACHE MISS at 0x%08x\n", *pc);
            }
        } else {
            printf("\tretrieved instruction 0x%08x at 0x%08x\n", ifid->instr, *pc);
        }

    }

}
