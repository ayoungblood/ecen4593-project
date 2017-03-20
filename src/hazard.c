/* src/hazard.c
* Hazard detection unit. Control and data hazard detection and forwarding
*/
#include "hazard.h"

int hazard(control_t *ifid, control_t *idex, control_t *exmem, control_t *memwb, pc_t *pc, bool *stall){

    //Reset stall
    *stall = false;

    //Hazard detection logic
    //If a load is immediately followed be an instruction that uses the result
    //Of the load, then detect it, stall the pipeline, and flush ifid to become nop
    if(idex->memRead && ((idex->regRt == ifid->regRs) || (idex->regRt == ifid->regRt)) && !(ifid->opCode == OPC_J || ifid->opCode == OPC_JAL)){
        //Stall the pipeline, data dependency after a load
        *stall = true;
        flush(ifid);
    }
    //Determine if a branch was taken by looking at the result in
    //the idex pipeline register. If it was taken, udpate the program counter
    //to the new calculated value and flush IFID. Jumps are also treated the
    //as branches, so IFID will be flushed for all jump instructions.
    if(idex->jump || idex->PCSrc){
        //Jump or branch occured, flush ifid
        flush(ifid);
        *pc = idex->pcNext;
    }
    //Fetch increments the PC by 4 always, so there is no else case

    //Forwarding from the execute stage
    if(exmem->regWrite && exmem->regRd != 0 && exmem->regRd == idex->regRs){
        //Forward alu result to rsvalue of idex
        idex->regRsValue = exmem->ALUresult;
    }
    if(exmem->regWrite && exmem->regRd != 0 && exmem->regRd == idex->regRt){
        //Forward alu result to rtvalue of idex
        idex->regRtValue = exmem->ALUresult;
    }


    if(memwb->regWrite && memwb->regRd != 0 && memwb->regRd == idex->regRs &&
        !(exmem->regWrite && exmem->regRd != 0 && exmem->regRd == idex->regRs)){
            if(memwb->memToReg){
                //Comes from data memory result
                idex->regRsValue = memwb->memData;
            }
            else{
                //Forward aluresult to rsvalue of idex
                idex->regRsValue = memwb->ALUresult;
            }
    }

    if(memwb->regWrite && memwb->regRd != 0 && memwb->regRd == idex->regRt &&
        !(exmem->regWrite && exmem->regRd != 0 && exmem->regRd == idex->regRt)){
            if(memwb->memToReg){
                //Forward comes from data memory
                idex->regRtValue = memwb->memData;
            }
            else{
                //Forward comes from ALUresult
                idex->regRtValue = memwb->ALUresult;
            }
    }





    return 0;
}
