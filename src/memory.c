/* src/memory.c
 * Memory access stage of the pipeline
 */

#include "memory.h"

extern int flags;

void memory(control_t * exmem, control_t * memwb) {
    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "MEMORY:\n" ANSI_C_RESET);
        printf("\tInstruction: 0x%08x\n", exmem->instr);
    }
    copy_pipeline_register(exmem, memwb);
    word_t temp;
    cache_status_t status = CACHE_NO_ACCESS; //if no memory operation, don't want to send an accidental miss
    if (exmem->memRead) {
        switch (exmem->opCode) {
            case OPC_LBU:
            case OPC_LB:
                mem_read_b(exmem->ALUresult,&temp);
                /*
                if (exmem->opCode == OPC_LB) temp = SIGN_EXTEND_B(temp);
                printf("\tMemory data: 0x%08x\n", temp);
                status = d_cache_read_w(&exmem->ALUresult, &temp);
                temp = temp >> ((3-(exmem->ALUresult & 0x3))<<3);
                temp &= 0xff;
                if (exmem->opCode == OPC_LB) temp = SIGN_EXTEND_B(temp);
                printf("\tCache data: 0x%08x\n", temp);*/
                break;
            case OPC_LHU:
            case OPC_LH:
                mem_read_h(exmem->ALUresult,&temp);/*
                if (exmem->opCode == OPC_LH) temp = SIGN_EXTEND_H(temp);
                printf("\tMemory data: 0x%08x\n", temp);
                status = d_cache_read_w(&exmem->ALUresult, &temp);
                temp = temp >> ((2-(exmem->ALUresult & 0x2))<<3);
                temp &= 0xffff;
                if (exmem->opCode == OPC_LH) temp = SIGN_EXTEND_H(temp);
                printf("\tCache data: 0x%08x\n", temp);*/
                break;
            case OPC_LW:
                mem_read_w(exmem->ALUresult, &temp);/*
                printf("\tMemory data: 0x%08x\n", temp);
                status = d_cache_read_w(&exmem->ALUresult, &temp);
                printf("\tCache data: 0x%08x\n", temp);*/
                break;
            default: // We should not get here. Complain and crash.
                printf(ANSI_C_RED "Illegal memory operation, opcode 0x%02x, (memRead asserted). Halting.\n" ANSI_C_RESET, exmem->opCode);
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
                mem_write_b(exmem->ALUresult, &temp);
                /*
                status = d_cache_read_w(&exmem->ALUresult, &data_in_cache);
                if(status == CACHE_HIT){
                    uint32_t shift = ((3-(exmem->ALUresult & 0x3))<<3);
                    temp = temp << shift;
                    data_in_cache &= ~(0xff << shift);
                    temp = temp & data_in_cache;
                    status = d_cache_write_w(&exmem->ALUresult, &temp);
                }
                */
                break;
            case OPC_SH:
                temp = exmem->regRtValue;
                mem_write_h(exmem->ALUresult, &temp);
                /*
                status = d_cache_read_w(&exmem->ALUresult, &data_in_cache);
                if(status == CACHE_HIT){
                    uint32_t shift = ((2-(exmem->ALUresult & 0x2))<<3);
                    temp = temp << shift; // shift amount based on byte position
                    data_in_cache &= ~(0xffff << shift);
                    temp = temp & data_in_cache;
                    status = d_cache_write_w(&exmem->ALUresult, &temp);
                }*/
                break;
            case OPC_SW:
                temp = exmem->regRtValue;
                mem_write_w(exmem->ALUresult, &temp);
                /*
                status = d_cache_read_w(&exmem->ALUresult, &temp);
                if(status == CACHE_HIT){
                    status = d_cache_write_w(&exmem->ALUresult, &temp);
                }*/
                break;
            default: // We should not get here. Complain and crash.
                printf(ANSI_C_RED "Illegal memory operation, opcode 0x%02x, (memWrite asserted). Halting.\n" ANSI_C_RESET, exmem->opCode);
                assert(0);
        }
        memwb->status = status;
        if(flags & MASK_DEBUG){
            printf("\tStored 0x%08x to address 0x%08x\n", temp, exmem->ALUresult);
        }
    }
}
