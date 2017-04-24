/* src/hazard.c
* Hazard detection unit. Control and data hazard detection and forwarding
*/
#include "hazard.h"

extern int flags;

control_t *ifid_backup;
control_t *idex_backup;
control_t *exmem_backup;
control_t *memwb_backup;
pc_t pc_backup;

int hazard(control_t *ifid, control_t *idex, control_t *exmem, control_t *memwb, pc_t *pc, cache_config_t *cache_cfg) {
    bool forward = false;
    pc_backup = *pc;

    gprintf(ANSI_C_CYAN, "HAZARD:\n", NULL);

    // Reset stall
    bool stall = false;
    // Our destination register could be Rd or Rt
    uint32_t exDest = 0;
    if (exmem->regDst) {
        exDest = exmem->regRd;
    } else {
        exDest = exmem->regRt;
    }
    uint32_t memDest = 0;
    if (memwb->regDst) {
        memDest = memwb->regRd;
    } else {
        memDest = memwb->regRt;
    }

    // Forwarding from the execute stage
    if (exmem->regWrite && (exDest != 0) && (exDest == idex->regRs)) {
        // Forward alu result to rsvalue of idex
        forward = true;
        if (flags & MASK_VERBOSE) {
            printf("\tFound data hazard: Forwarding exmem->ALUresult to idex->regRsValue\n");
            printf("\tNEW idex->regRsValue is 0x%08x\n", exmem->ALUresult);
        }
        idex->regRsValue = exmem->ALUresult;
    }
    if (exmem->regWrite && (exDest != 0) && (exDest == idex->regRt)) {
        // Forward alu result to rtvalue of idex
        forward = true;
        if (flags & MASK_VERBOSE) {
            printf("\tFound data hazard: Forwarding exmem->ALUresult to idex->regRtValue\n");
            printf("\tNEW idex->regRtValue is 0x%08x\n", exmem->ALUresult);
        }
        idex->regRtValue = exmem->ALUresult;
    }

    if (memwb->regWrite && (memDest != 0) && (memDest == idex->regRs) &&
        !(exmem->regWrite && (exDest != 0) && (exDest == idex->regRs))) {
        forward = true;
        if (memwb->memToReg) {
            // Comes from data memory result
            if (flags & MASK_VERBOSE) {
                printf("\tFound data hazard: Forwarding memwb->memData to idex->regRsValue\n");
                printf("\tNEW idex->regRsValue is 0x%08x\n", memwb->memData);
            }
            idex->regRsValue = memwb->memData;
        } else {
            // Forward aluresult to rsvalue of idex
            if (flags & MASK_VERBOSE) {
                printf("\tFound data hazard: Forwarding memwb->ALUresult to idex->regRsValue\n");
                printf("\tNEW idex->regRsValue is 0x%08x\n", memwb->ALUresult);
            }
            idex->regRsValue = memwb->ALUresult;
        }
    }

    if (memwb->regWrite && (memDest != 0) && (memDest == idex->regRt) &&
        !(exmem->regWrite && (exDest != 0) && (exDest == idex->regRt))) {
            forward = true;
        if (memwb->memToReg) {
            // Forward comes from data memory
            if (flags & MASK_VERBOSE) {
                printf("\tFound data hazard: Forwarding memwb->memData to idex->regRtValue\n");
                printf("\tNEW idex->regRtValue is 0x%08x\n", memwb->memData);
            }
            idex->regRtValue = memwb->memData;
        } else {
            // Forward comes from ALUresult
            if (flags & MASK_VERBOSE) {
                printf("\tFound data hazard: Forwarding memwb->ALUresult to idex->regRtValue\n");
                printf("\tNEW idex->regRtValue is 0x%08x\n", memwb->ALUresult);
            }
            idex->regRtValue = memwb->ALUresult;
        }
    }

    // Recheck the outcome of the branch if there was a forward that occured.
    if (forward) {
        if (idex->opCode == OPC_BNE) {
            if (flags & MASK_DEBUG) {
                printf("\tRecalculating BNE\n");
            }
            if(idex->regRsValue != idex->regRtValue) { // Branch taken
                idex->PCSrc = true;
            } else {
                idex->PCSrc = false;
            }
        } else if(idex->opCode == OPC_BEQ) {
            if (flags & MASK_DEBUG) {
                printf("\tRecalculating BEQ\n");
            }
            if (idex->regRsValue == idex->regRtValue) { // Branch taken
                idex->PCSrc = true;
            } else {
                idex->PCSrc = false;
            }
        } else if (idex->opCode == OPC_BLTZ) {
            if (flags & MASK_DEBUG) {
                printf("\tRecalculating BLTZ\n");
            }
            if ((int)idex->regRsValue < 0) { // Branch taken
                idex->PCSrc = true;
            } else{
                idex->PCSrc = false;
            }
        } else if (idex->opCode == OPC_BGTZ) {
            if (flags & MASK_DEBUG) {
                printf("\tRecalculating BGTZ\n");
            }
            if ((int)idex->regRsValue > 0) { // Branch taken
                idex->PCSrc = true;
            } else {
                idex->PCSrc = false;
            }
        } else if (idex->opCode == OPC_BLEZ){
            if (flags & MASK_DEBUG) {
                printf("\tRecalculating BLEZ\n");
            }
            if ((int)idex->regRsValue <= 0) {
                idex->PCSrc = true;
            } else {
                idex->PCSrc = false;
            }
        } else if ((idex->opCode == OPC_RTYPE) && (idex->funct == FNC_JR)) {
            if (flags & MASK_DEBUG) {
                printf("\tRecalculating JR\n");
            }
            idex->pcNext = idex->regRsValue;
        }
        if (flags & MASK_DEBUG) {
            if (idex->PCSrc) {
                printf("\tBranch will be taken\n");
            }
        }
    }

    /* Hazard detection logic
     * If a load is immediately followed be an instruction that uses the result
     * of the load, then detect it, stall the pipeline, and flush ifid to become nop */
    if (idex->memRead && ((idex->regRt == ifid->regRs) || (idex->regRt == ifid->regRt)) &&
        !(ifid->opCode == OPC_J || ifid->opCode == OPC_JAL)) {
        // Stall the pipeline, data dependency after a load
        if (flags & MASK_VERBOSE) {
            printf("\tFound dependency on load result: stalling pipeline\n");
        }
        stall = true;
        flush(ifid);
    }

    /* Updating the Prgram Counter
     * Determine if a branch was taken by looking at the result in
     * the idex pipeline register. If it was taken, udpate the program counter
     * to the new calculated value and flush IFID. Jumps are also treated the
     * as branches, so IFID will be flushed for all jump instructions. */
    if (idex->jump || idex->PCSrc) {
        // Jump or branch occured, flush ifid
        if (flags & MASK_VERBOSE) {
            printf("\tBranching or Jumping: inserting nop and overriding pc\n");
        }
        //flush(ifid);
        *pc = idex->pcNext;
    } else if (stall) {
        // Stall the pipeline by not updating pc and flushing ifid
        if (flags & MASK_VERBOSE) {
            printf("\tStalling the pipeline\n");
        }
        flush(ifid);
    } else {
        // Normal operation update the program counter by 4
        *pc = *pc + 4;
    }

    if (cache_cfg->mode != CACHE_DISABLE && (cache_cfg->inst_enabled || cache_cfg->data_enabled)) {
        if (memwb->status == CACHE_MISS || ifid->status == CACHE_MISS) {
            if (flags & MASK_DEBUG) {
                printf("\tcache miss! Restoring the pipeline\n");
            }
            restore(ifid, idex, exmem, memwb, pc);
        }
    }
    return 0;
}

void hazard_init(void) {
    pipeline_init(&ifid_backup, &idex_backup, &exmem_backup, &memwb_backup, &pc_backup, 0);
}

void backup(control_t *ifid, control_t *idex, control_t *exmem, control_t *memwb, pc_t *pc) {
    copy_pipeline_register(ifid, ifid_backup);
    copy_pipeline_register(idex, idex_backup);
    copy_pipeline_register(exmem, exmem_backup);
    copy_pipeline_register(memwb, memwb_backup);
    pc_backup = *pc;
}

void restore(control_t *ifid, control_t *idex, control_t *exmem, control_t *memwb, pc_t *pc) {
    // Some checking to ensure the register file hasn't changed
    copy_pipeline_register(ifid_backup, ifid);
    copy_pipeline_register(idex_backup, idex);
    copy_pipeline_register(exmem_backup, exmem);
    copy_pipeline_register(memwb_backup, memwb);
    *pc = pc_backup;
}
