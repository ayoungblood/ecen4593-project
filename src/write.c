/* src/write.c
* Write back stage of the pipeline
*/

#include "write.h"

extern int flags;  // from util.c

void writeback(control_t *memwb){
    word_t writeRegister = 0;
    word_t writeRegisterValue = 0;
    // Determine the WB register based on regDst
    if (memwb->regDst) {
        writeRegister = memwb->regRd;
    } else {
        writeRegister = memwb->regRt;
    }

    // Get correct writeback register value
    if (memwb->memToReg) {
        writeRegisterValue = memwb->memData;
    } else {
        writeRegisterValue = memwb->ALUresult;
    }

    gcprintf(ANSI_C_CYAN, "WRITE: \n");
    gprintf("\tInstruction: 0x%08x\n", memwb->instr);

    // Write back to the register file
    if (memwb->regWrite) {
        gprintf("\tWriting 0x%08x to register 0x%02x (0d%d, $%s)\n",
            writeRegisterValue,
            writeRegister,
            writeRegister,
            get_register_name_string(writeRegister));
        reg_write(writeRegister, &writeRegisterValue);
    }
}
