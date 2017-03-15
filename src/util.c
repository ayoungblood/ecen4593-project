/* util.c
 * Utility/debug defines and other miscellany
 */

 #include "util.h"

//Print all of the struct fields of the pipeline a pipeline register
 void print_pipeline_register(control_t * reg){
     printf("Pipeline Register: %s\n", reg->regName);
     printf("Instruction 0x%08x\n", reg->instr);
     printf("Decoded Instrunction: \n");
     printf("\treg->opCode: 0x%08x\n", reg->opCode);
     printf("\treg->regRs: 0x%08x\n", reg->regRs);
     printf("\treg->regRt: 0x%08x\n", reg->regRt);
     printf("\treg->regRd: 0x%08x\n", reg->regRd);
     printf("\treg->shamt: 0x%08x\n", reg->shamt);
     printf("\treg->funct: 0x%08x\n", reg->funct);
     printf("\treg->immed: 0x%08x\n", reg->immed);
     printf("\treg->address: 0x%08x\n", reg->address);
     printf("\treg->pcNext: 0x%08x\n", reg->pcNext);
     printf("\treg->regRtValue: 0x%08x\n", reg->regRtValue);
     printf("\treg->regRsValue: 0x%08x\n", reg->regRsValue);
     printf("\treg->ALUresult: 0x%08x\n", reg->ALUresult);
     printf("Global Program Counter:\n");
     printf("Register control bits:\n");
     printf("\treg->regDst: 0x%08x\n", reg->regDst);
     printf("\treg->ALUSrc: 0x%08x\n", reg->ALUSrc);
     printf("\treg->memToReg: 0x%08x\n", reg->memToReg);
     printf("\treg->regWrite: 0x%08x\n", reg->regWrite);
     printf("\treg->memRead: 0x%08x\n", reg->memRead);
     printf("\treg->memWrite: 0x%08x\n", reg->memWrite);
     printf("\treg->ALUop: 0x%08x\n", reg->ALUop);
     printf("\treg->PCSrc: 0x%08x\n", reg->PCSrc);
     printf("\treg->jump: 0x%08x\n\n", reg->jump);
 }

 void copy_pipeline_register(control_t *orig, control_t *copy){
     copy->regDst = orig->regDst;
     copy->regWrite = orig->regWrite;
     copy->ALUSrc = orig->ALUSrc;
     copy->PCSrc = orig->PCSrc;
     copy->memRead = orig->memRead;
     copy->memWrite = orig->memWrite;
     copy->memToReg = orig->memToReg;
     copy->ALUop = orig->ALUop;
     copy->jump = orig->jump;
     copy->instr = orig->instr;
     copy->opCode = orig->opCode;
     copy->regRs = orig->regRs;
     copy->regRt = orig->regRt;
     copy->regRd = orig->regRd;
     copy->immed = orig->immed;
     copy->address = orig->address;
     copy->funct = orig->funct;
     copy->shamt = orig->shamt;
     copy->regRsValue = orig->regRsValue;
     copy->regRtValue = orig->regRtValue;
     copy->ALUresult = orig->ALUresult;
     copy->pcNext = orig->pcNext;
 }

 void flush(control_t *reg){
     reg->regDst = 0;
     reg->regWrite = 0;
     reg->ALUSrc = 0;
     reg->PCSrc = 0;
     reg->memRead = 0;
     reg->memWrite = 0;
     reg->ALUop = 0;
     reg->jump = 0;
     reg->instr = 0;
     reg->opCode = 0;
     reg->regRs = 0;
     reg->regRt = 0;
     reg->regRd = 0;
     reg->immed = 0;
     reg->address = 0;
     reg->funct = 0;
     reg->shamt = 0;
     reg->regRsValue = 0;
     reg->regRtValue = 0;
     reg->ALUresult = 0;
     reg->pcNext = 0;
 }
