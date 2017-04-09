/* util.c
 * Utility/debug defines and other miscellany
 */

#include "util.h"

// Print all of the struct fields of a pipeline register
void print_pipeline_register(control_t * reg){
    printf("\tInstruction: 0x%08x\n", reg->instr);
    printf("\tPipeline Register: %s\n", reg->regName);
    printf("\tDecoded Instruction: \n");
    printf("\t    reg->opCode:     0x%02x\n", reg->opCode);
    printf("\t    reg->regRs:      0x%02x\t(0d%d, $%s)\n", reg->regRs, reg->regRs, get_register_name_string(reg->regRs));
    printf("\t    reg->regRt:      0x%02x\t(0d%d, $%s)\n", reg->regRt, reg->regRt, get_register_name_string(reg->regRt));
    printf("\t    reg->regRd:      0x%02x\t(0d%d, $%s)\n", reg->regRd, reg->regRd, get_register_name_string(reg->regRd));
    printf("\t    reg->shamt:      0x%02x\t(0d%d)\n", reg->shamt, reg->shamt);
    printf("\t    reg->funct:      0x%02x\n", reg->funct);
    printf("\t    reg->immed:      0x%04x\t(0d%d)\n", reg->immed, reg->immed);
    printf("\t    reg->address:    0x%08x\n", reg->address);
    printf("\t    reg->pcNext:     0x%08x\n", reg->pcNext);
    printf("\t    reg->regRtValue: 0x%08x\t(0d%d)\n", reg->regRtValue, reg->regRtValue);
    printf("\t    reg->regRsValue: 0x%08x\t(0d%d)\n", reg->regRsValue, reg->regRsValue);
    printf("\t    reg->ALUresult:  0x%08x\t(0d%d)\n", reg->ALUresult , reg->ALUresult);
    printf("\tRegister control bits:\n");
    printf("\t    reg->regDst:     %01d\n", reg->regDst);
    printf("\t    reg->ALUSrc:     %01d\n", reg->ALUSrc);
    printf("\t    reg->memToReg:   %01d\n", reg->memToReg);
    printf("\t    reg->regWrite:   %01d\n", reg->regWrite);
    printf("\t    reg->memRead:    %01d\n", reg->memRead);
    printf("\t    reg->memWrite:   %01d\n", reg->memWrite);
    //printf("\t    reg->ALUop:      %02d\n", reg->ALUop);
    printf("\t    reg->PCSrc:      %01d\n", reg->PCSrc);
    printf("\t    reg->jump:       %01d\n\n", reg->jump);
}

void copy_pipeline_register(control_t* orig, control_t* copy){
    copy->regDst        = orig->regDst;
    copy->regWrite      = orig->regWrite;
    copy->ALUSrc        = orig->ALUSrc;
    copy->PCSrc         = orig->PCSrc;
    copy->memRead       = orig->memRead;
    copy->memWrite      = orig->memWrite;
    copy->memToReg      = orig->memToReg;
    copy->ALUop         = orig->ALUop;
    copy->jump          = orig->jump;
    copy->instr         = orig->instr;
    copy->opCode        = orig->opCode;
    copy->regRs         = orig->regRs;
    copy->regRt         = orig->regRt;
    copy->regRd         = orig->regRd;
    copy->immed         = orig->immed;
    copy->address       = orig->address;
    copy->funct         = orig->funct;
    copy->shamt         = orig->shamt;
    copy->regRsValue    = orig->regRsValue;
    copy->regRtValue    = orig->regRtValue;
    copy->ALUresult     = orig->ALUresult;
    copy->pcNext        = orig->pcNext;
}

void flush(control_t* reg){
    reg->regDst         = false;
    reg->regWrite       = false;
    reg->ALUSrc         = false;
    reg->PCSrc          = false;
    reg->memRead        = false;
    reg->memWrite       = false;
    reg->ALUop          = (operation_t)0x0;
    reg->jump           = false;
    reg->instr          = (inst_t)0x0;
    reg->opCode         = (opcode_t)0x0;
    reg->regRs          = 0;
    reg->regRt          = 0;
    reg->regRd          = 0;
    reg->immed          = 0;
    reg->address        = 0;
    reg->funct          = (funct_t)0;
    reg->shamt          = 0;
    reg->regRsValue     = 0;
    reg->regRtValue     = 0;
    reg->ALUresult      = 0;
    reg->pcNext         = 0;
    reg->memData        = 0;
}

void pipeline_init(control_t** ifid, control_t** idex, control_t** exmem, control_t** memwb, pc_t* pc, pc_t pc_start) {
    // Instantiate pipeline registers
    *ifid  = (control_t*)malloc(sizeof(control_t));
    *idex  = (control_t*)malloc(sizeof(control_t));
    *exmem = (control_t*)malloc(sizeof(control_t));
    *memwb = (control_t*)malloc(sizeof(control_t));
    if (*ifid == NULL || *idex == NULL || *exmem == NULL || *memwb == NULL) {
        printf("Failed to allocate pipeline registers\n");
    }
    // Allocate and assign pipeline register names
    (*ifid)->regName  = (char*)malloc(7*sizeof(char));
    (*idex)->regName  = (char*)malloc(7*sizeof(char));
    (*exmem)->regName = (char*)malloc(7*sizeof(char));
    (*memwb)->regName = (char*)malloc(7*sizeof(char));
    if ((*ifid)->regName == NULL || (*idex)->regName == NULL || (*exmem)->regName == NULL || (*memwb)->regName == NULL) {
        printf("Failed to allocate register name strings\n");
    }
    (*ifid)->regName  = "IF/ID";
    (*idex)->regName  = "ID/EX";
    (*exmem)->regName = "EX/MEM";
    (*memwb)->regName = "MEM/WB";
    // Flush registers to initialize
    flush(*ifid);
    flush(*idex);
    flush(*exmem);
    flush(*memwb);
    // Initialize program counter from first memory address
    *pc = pc_start;
}

void pipeline_destroy(control_t** ifid, control_t** idex, control_t** exmem, control_t** memwb) {
    // Clean up pipeline register name space (malloced in pipeline_init())
    // free((*ifid)->regName);
    //free((*idex)->regName);
    //free((*exmem)->regName);
    //free((*memwb)->regName);
    // Clean up pipeline registers
    free(*ifid);
    free(*idex);
    free(*exmem);
    free(*memwb);
    // Clear pointers to fail fast if re-used
    *ifid  = NULL;
    *idex  = NULL;
    *exmem = NULL;
    *memwb = NULL;
}

char* get_register_name_string(int reg) {
    char* names[] = {
        "zero",
        "at",
        "v0",
        "v1",
        "a0",
        "a1",
        "a2",
        "a3",
        "t0",
        "t1",
        "t2",
        "t3",
        "t4",
        "t5",
        "t6",
        "t7",
        "s0",
        "s1",
        "s2",
        "s3",
        "s4",
        "s5",
        "s6",
        "s7",
        "t8",
        "t9",
        "k0",
        "k1",
        "gp",
        "sp",
        "fp",
        "ra"
    };
    return names[reg];
}
