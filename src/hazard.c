/* src/hazard.c
* Hazard detection unit. Control and data hazard detection and forwarding
*/
#include "hazard.h"

extern int flags;

int hazard(control_t *ifid, control_t *idex, control_t *exmem, control_t *memwb, pc_t *pc, bool *stall){

    if(flags & MASK_DEBUG){
        printf(ANSI_C_CYAN "HAZARD:\n" ANSI_C_RESET);
    }

    //Reset stall
    *stall = false;

    //Hazard detection logic
    //If a load is immediately followed be an instruction that uses the result
    //Of the load, then detect it, stall the pipeline, and flush ifid to become nop
    if(idex->memRead && ((idex->regRt == ifid->regRs) || (idex->regRt == ifid->regRt)) && !(ifid->opCode == OPC_J || ifid->opCode == OPC_JAL)){
        //Stall the pipeline, data dependency after a load
        if(flags & MASK_VERBOSE){
            printf("\tFound dependency on load result: stalling pipeline\n");
        }
        *stall = true;
        flush(ifid);
    }
    //Determine if a branch was taken by looking at the result in
    //the idex pipeline register. If it was taken, udpate the program counter
    //to the new calculated value and flush IFID. Jumps are also treated the
    //as branches, so IFID will be flushed for all jump instructions.
    if(idex->jump || idex->PCSrc){
        //Jump or branch occured, flush ifid
        if(flags & MASK_VERBOSE){
            printf("\tBranching or Jumping: inserting nop and overriding pc\n");
        }
        flush(ifid);
        *pc = idex->pcNext;
    }
    //Fetch increments the PC by 4 always, so there is no else case

    //Our destination register could be Rd or Rt
    uint32_t exDest = 0;
    if(exmem->regDst){
        exDest = exmem->regRd;
    }
    else{
        exDest = exmem->regRt;
    }
    uint32_t memDest = 0;
    if(memwb->regDst){
        memDest = memwb->regRd;
    }
    else{
        memDest = memwb->regRt;
    }

    //Forwarding from the execute stage
    if(exmem->regWrite && (exDest != 0) && (exDest == idex->regRs)){
        //Forward alu result to rsvalue of idex
        if(flags & MASK_VERBOSE){
            printf("\tFound data hazard: Forwarding exmem->ALUresult to idex->regRsValue\n");
        }
        idex->regRsValue = exmem->ALUresult;
    }
    if(exmem->regWrite && (exDest != 0) && (exDest == idex->regRt)){
        //Forward alu result to rtvalue of idex
        if(flags & MASK_VERBOSE){
            printf("\tFound data hazard: Forwarding exmem->ALUresult to idex->regRtValue\n");
        }
        idex->regRtValue = exmem->ALUresult;
    }


    if(memwb->regWrite && (memDest != 0) && (memDest == idex->regRs) &&
        !(exmem->regWrite && (exDest != 0) && (exDest == idex->regRs))){
            if(memwb->memToReg){
                //Comes from data memory result
                if(flags & MASK_VERBOSE){
                    printf("\tFound data hazard: Forwarding memwb->memData to idex->regRsValue\n");
                }
                idex->regRsValue = memwb->memData;
            }
            else{
                //Forward aluresult to rsvalue of idex
                if(flags & MASK_VERBOSE){
                    printf("\tFound data hazard: Forwarding memwb->ALUresult to idex->regRsValue\n");
                }
                idex->regRsValue = memwb->ALUresult;
            }
    }

    if(memwb->regWrite && (memDest != 0) && (memDest == idex->regRt) &&
        !(exmem->regWrite && (exDest != 0) && (exDest == idex->regRt))){
            if(memwb->memToReg){
                //Forward comes from data memory
                if(flags & MASK_VERBOSE){
                    printf("\tFound data hazard: Forwarding memwb->memData to idex->regRtValue\n");
                }
                idex->regRtValue = memwb->memData;
            }
            else{
                //Forward comes from ALUresult
                if(flags & MASK_VERBOSE){
                    printf("\tFound data hazard: Forwarding memwb->ALUresult to idex->regRtValue\n");
                }
                idex->regRtValue = memwb->ALUresult;
            }
    }

    return 0;
}
