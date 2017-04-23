/* src/memory.c
 * Memory access stage of the pipeline
 */

#include "memory.h"

extern int flags;

void memory(control_t * exmem, control_t * memwb, cache_config_t *cache_cfg) {
    if(flags & MASK_DEBUG){
        cprintf(ANSI_C_CYAN, "MEMORY:\n");
        printf("\tInstruction: 0x%08x\n", exmem->instr);
    }
    copy_pipeline_register(exmem, memwb);
    word_t temp;
    cache_status_t status = CACHE_NO_ACCESS; //if no memory operation, don't want to send an accidental miss
    if (exmem->memRead) {
        switch (exmem->opCode) {
            case OPC_LBU:
            case OPC_LB:
                if(cache_cfg->data_enabled){
                    status = d_cache_read_w(&exmem->ALUresult, &temp);
                    temp = temp >> ((3-(exmem->ALUresult & 0x3))<<3);
                    temp &= 0xff;
                } else {
                    mem_read_b(exmem->ALUresult,&temp);
                }
                if (exmem->opCode == OPC_LB) temp = SIGN_EXTEND_B(temp);
                break;
            case OPC_LHU:
            case OPC_LH:
                if(cache_cfg->data_enabled){
                    status = d_cache_read_w(&exmem->ALUresult, &temp);
                    temp = temp >> ((2-(exmem->ALUresult & 0x2))<<3);
                    temp &= 0xffff;
                } else {
                    mem_read_h(exmem->ALUresult,&temp);
                }
                if (exmem->opCode == OPC_LH) temp = SIGN_EXTEND_H(temp);
                break;
            case OPC_LW:
                if(cache_cfg->data_enabled){
                    status = d_cache_read_w(&exmem->ALUresult, &temp);
                } else {
                    mem_read_w(exmem->ALUresult, &temp);
                }
                break;
            default: // We should not get here. Complain and crash.
                cprintf(ANSI_C_RED, "Illegal memory operation, opcode 0x%02x, (memRead asserted). Halting.\n", exmem->opCode);
                assert(0);
        }
        if(flags & MASK_DEBUG){
            printf("\tLoaded 0x%08x from address 0x%08x\n", temp, exmem->ALUresult);
        }
        memwb->memData = temp;
        memwb->status = status;
    }
    if (exmem->memWrite) {
        word_t data_in_cache;
        switch (exmem->opCode) {
            case OPC_SB:
                temp = exmem->regRtValue;
                if(cache_cfg->data_enabled){
                    status = d_cache_read_w(&exmem->ALUresult, &data_in_cache);
                    if(status == CACHE_HIT){
                        uint32_t shift = ((3-(exmem->ALUresult & 0x3))<<3);
                        temp = temp << shift;
                        data_in_cache &= ~(0xff << shift);
                        temp = temp & data_in_cache;
                        status = d_cache_write_w(&exmem->ALUresult, &temp);
                    }
                } else {
                    mem_write_b(exmem->ALUresult, &temp);
                }
                break;
            case OPC_SH:
                temp = exmem->regRtValue;
                if(cache_cfg->data_enabled){
                    status = d_cache_read_w(&exmem->ALUresult, &data_in_cache);
                    if(status == CACHE_HIT){
                        uint32_t shift = ((2-(exmem->ALUresult & 0x2))<<3);
                        temp = temp << shift; // shift amount based on byte position
                        data_in_cache &= ~(0xffff << shift);
                        temp = temp & data_in_cache;
                        status = d_cache_write_w(&exmem->ALUresult, &temp);
                    }
                } else {
                    mem_write_h(exmem->ALUresult, &temp);
                }
                break;
            case OPC_SW:
                temp = exmem->regRtValue;
                if(cache_cfg->data_enabled){
                    status = d_cache_read_w(&exmem->ALUresult, &data_in_cache);
                    if(status == CACHE_HIT){
                        status = d_cache_write_w(&exmem->ALUresult, &temp);
                    }
                } else {
                    mem_write_w(exmem->ALUresult, &temp);
                }
                break;
            default: // We should not get here. Complain and crash.
                cprintf(ANSI_C_RED, "Illegal memory operation, opcode 0x%02x, (memWrite asserted). Halting.\n", exmem->opCode);
                assert(0);
        }
        memwb->status = status;
        if(flags & MASK_DEBUG){
            if(cache_cfg->mode != CACHE_DISABLE && cache_cfg->data_enabled){
                if(memwb->status == CACHE_HIT){
                    printf("\tStored 0x%08x to address 0x%08x\n", temp, exmem->ALUresult);
                }
                else {
                    printf("\tTried to store 0x%08x to address 0x%08x\n", temp, exmem->ALUresult);
                }
            } else {
                printf("\tStored 0x%08x to address 0x%08x\n", temp, exmem->ALUresult);
            }
        }
    }
}
