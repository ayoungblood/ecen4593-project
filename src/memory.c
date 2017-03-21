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
    if (exmem->memRead) {
        switch (exmem->opCode) {
            case OPC_LBU:
            case OPC_LB:
                mem_read_b(exmem->ALUresult,&temp);
                if (exmem->opCode == OPC_LB) temp = SIGN_EXTEND_B(temp);
                break;
            case OPC_LHU:
            case OPC_LH:
                mem_read_h(exmem->ALUresult,&temp);
                if (exmem->opCode == OPC_LH) temp = SIGN_EXTEND_H(temp);
                break;
            case OPC_LW:
                mem_read_w(exmem->ALUresult,&temp);
                break;
            default: // We should not get here. Complain and crash.
                printf(ANSI_C_RED "Illegal memory operation, opcode 0x%02x, (memRead asserted). Halting.\n" ANSI_C_RESET, exmem->opCode);
                assert(0);
        }
        if(flags & MASK_DEBUG){
            printf("\tLoaded 0x%08x from address 0x%08x\n", temp, exmem->ALUresult);
        }
        memwb->memData = temp;
    }
    if (exmem->memWrite) {
        switch (exmem->opCode) {
            case OPC_SB:
                temp = exmem->regRt;
                mem_write_b(exmem->ALUresult,&temp);
                break;
            case OPC_SH:
                temp = exmem->regRt;
                mem_write_h(exmem->ALUresult,&temp);
                break;
            case OPC_SW:
                temp = exmem->regRt;
                mem_write_w(exmem->ALUresult,&temp);
                break;
            default: // We should not get here. Complain and crash.
                printf(ANSI_C_RED "Illegal memory operation, opcode 0x%02x, (memWrite asserted). Halting.\n" ANSI_C_RESET, exmem->opCode);
                assert(0);
        }
        if(flags & MASK_DEBUG){
            printf("\tStored 0x%08x to address 0x%08x\n", temp, exmem->ALUresult);
        }
    }
}
