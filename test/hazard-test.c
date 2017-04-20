/* test/hazard-test.c
* Unit tests for hazard detection and forwarding logic
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "minunit.h"
#include "../src/hazard.h"
#include "../src/types.h"
#include "../src/util.h"
#include "../src/registers.h"

int tests_run = 0;

int flags = MASK_DEBUG | MASK_VERBOSE | MASK_SANITY;

pc_t pc;
control_t *ifid, *idex, *exmem, *memwb;


/*Test forwarding logic from exmem stage*/

static char * test_forwarding_exmem(){
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    exmem = (control_t *)malloc(sizeof(control_t));
    memwb = (control_t *)malloc(sizeof(control_t));
    pc = 4;
    /*Case where exmem has data that will be written to the reg file and another
    instruction in idex needs the result from the ALU*/
    exmem->regWrite = true;
    exmem->regRd = REG_T0;
    exmem->ALUresult = 0x64;

    idex->regRs = REG_T0;
    idex->regRsValue = 0;       //Make sure this isn't 100 before test is run

    hazard(ifid, idex, exmem, memwb, &pc);

    mu_assert(_FL "Incorrect exmem forwarding ALUresult Rd->Rs", idex->regRsValue == 0x64);


    // Also test for forwarding from Rt
    exmem->regWrite = true;
    exmem->ALUresult = 0x69;
    exmem->regRd = REG_S3;

    idex->regRt = REG_S3;
    idex->regRtValue = 0;

    hazard(ifid, idex, exmem, memwb, &pc);

    mu_assert(_FL "Incorrect exmem forwarding ALUresult Rd->Rt", idex->regRtValue == 0x69);

    //Test for both
    exmem->regWrite = true;
    exmem->ALUresult = 0x23;
    exmem->regRd = REG_S3;

    idex->regRt = REG_S3;
    idex->regRtValue = 0;
    idex->regRs = REG_S3;
    idex->regRsValue = 0;

    hazard(ifid, idex, exmem, memwb, &pc);

    mu_assert(_FL "Incorrect exmem forwarding ALUresult (both) Rd->Rt", idex->regRtValue == 0x23);
    mu_assert(_FL "Incorrect exmem forwarding ALUresult (both) Rd->Rs", idex->regRsValue == 0x23);


    free(ifid);
    free(idex);
    free(exmem);
    free(memwb);

    return 0;
}

/*Forwarding from memwb register*/
static char * test_forwarding_memwb(){
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    exmem = (control_t *)malloc(sizeof(control_t));
    memwb = (control_t *)malloc(sizeof(control_t));
    pc = 4;

    //Case where memwb has result an instruction in idex needs from memory data
    //Rs forwarding
    memwb->regWrite = true;
    memwb->regRd = REG_T5;
    memwb->memToReg = true;
    memwb->memData = 0x16;
    exmem->regWrite = false;
    idex->regRs = REG_T5;
    idex->regRsValue = 0;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "Incorrect memwb forwarding memData Rd->Rs", idex->regRsValue == 0x16);

    //Rt Forwarding
    memwb->regWrite = true;
    memwb->regRd = REG_A0;
    memwb->memToReg = true;
    memwb->memData = 0x92;
    exmem->regWrite = false;
    idex->regRt = REG_A0;
    idex->regRtValue = 0;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "Incorrect memwb forwarding memData Rd->Rt", idex->regRtValue == 0x92);

    //Forwarding to both rs and rt
    memwb->regWrite = true;
    memwb->regRd = REG_V1;
    memwb->memToReg = true;
    memwb->memData = 0xFF;
    exmem->regWrite = false;
    idex->regRt = REG_V1;
    idex->regRtValue = 0;
    idex->regRs = REG_V1;
    idex->regRsValue = 0;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "Incorrect memwb forwarding memData (both) Rd->Rt", idex->regRtValue == 0xff);
    mu_assert(_FL "Incorrect memwb forwarding memData (both) Rd->Rs", idex->regRsValue == 0xff);


    //Forwarding from memwb to idex with the alu result data and not data memory
    //Rs forwarding
    memwb->regWrite = true;
    memwb->regRd = REG_T5;
    memwb->memToReg = false;
    memwb->ALUresult = 0x56;
    exmem->regWrite = false;
    idex->regRs = REG_T5;
    idex->regRsValue = 0;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "Incorrect memwb forwarding ALUresult Rd->Rs", idex->regRsValue == 0x56);

    //Rt Forwarding
    memwb->regWrite = true;
    memwb->regRd = REG_A0;
    memwb->memToReg = false;
    memwb->ALUresult = 0x22;
    exmem->regWrite = false;
    idex->regRt = REG_A0;
    idex->regRtValue = 0;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "Incorrect memwb forwarding ALUresult Rd->Rt", idex->regRtValue == 0x22);

    //Forwarding to both rs and rt
    memwb->regWrite = true;
    memwb->regRd = REG_V1;
    memwb->memToReg = false;
    memwb->ALUresult = 0xa7;
    exmem->regWrite = false;
    idex->regRt = REG_V1;
    idex->regRtValue = 0;
    idex->regRs = REG_V1;
    idex->regRsValue = 0;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "Incorrect memwb forwarding ALUresult (both) Rd->Rt", idex->regRtValue == 0xa7);
    mu_assert(_FL "Incorrect memwb forwarding ALUresult (both) Rd->Rs", idex->regRsValue == 0xa7);


    free(ifid);
    free(idex);
    free(exmem);
    free(memwb);

    return 0;
}


/*Case where forwarding should come from the exmem stage even though the
instruction in idex also uses values from an instruction in memwb*/


static char * test_forwarding_exmem_not_memwb(){
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    exmem = (control_t *)malloc(sizeof(control_t));
    memwb = (control_t *)malloc(sizeof(control_t));
    pc = 4;
    //Forwarding from rd in exmem to rs in idex despite memwb rd
    memwb->regWrite = true;
    memwb->regRd = REG_S6;
    memwb->memToReg = false;
    memwb->ALUresult = 0x49;
    exmem->regWrite = true;
    exmem->regRd = REG_S6;
    exmem->ALUresult = 0x56;
    idex->regRs = REG_S6;
    idex->regRsValue = 0;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "Incorrect exmem (not memwb) forwarding ALUresult Rd->Rs", idex->regRsValue == 0x56);

    //Forwarding from rd in exmem to rt in idex despite memwb rd
    memwb->regWrite = true;
    memwb->regRd = REG_S6;
    memwb->memToReg = false;
    memwb->ALUresult = 0x4f9;
    exmem->regWrite = true;
    exmem->regRd = REG_S6;
    exmem->ALUresult = 0x6f;
    idex->regRt = REG_S6;
    idex->regRtValue = 0;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "Incorrect exmem (not memwb) forwarding ALUresult Rd->Rt", idex->regRtValue == 0x6f);

    //Forwarding from rd in exmem to rt and rs in idex despite memwb rd
    memwb->regWrite = true;
    memwb->regRd = REG_S6;
    memwb->memToReg = false;
    memwb->ALUresult = 0x99;
    exmem->regWrite = true;
    exmem->regRd = REG_S6;
    exmem->ALUresult = 0x1883;
    idex->regRs = REG_S6;
    idex->regRsValue = 0;
    idex->regRt = REG_S6;
    idex->regRtValue = 0;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "Incorrect exmem (not memwb) forwarding ALUresult Rd->Rs", idex->regRsValue == 0x1883);
    mu_assert(_FL "Incorrect exmem (not memwb) forwarding ALUresult Rd->Rt", idex->regRtValue == 0x1883);

    //Forwarding from rd in exmem to rt and rs in idex despite memwb rd
    memwb->regWrite = true;
    memwb->regRd = REG_S6;
    memwb->memToReg = false;
    memwb->ALUresult = 0x9;
    exmem->regWrite = true;
    exmem->regRd = REG_T3;
    exmem->ALUresult = 0x13;
    idex->regRs = REG_S6;
    idex->regRsValue = 0;
    idex->regRt = REG_T3;
    idex->regRtValue = 0;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "Incorrect memwb forwarding ALUresult Rd->Rs", idex->regRsValue == 0x9);
    mu_assert(_FL "Incorrect exmem (not memwb) forwarding ALUresult Rd->Rt", idex->regRtValue == 0x13);


    free(ifid);
    free(idex);
    free(exmem);
    free(memwb);

    return 0;


}

/*Test branch and jump flushing of the ifid register*/

static char * test_branch_flush(){
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    exmem = (control_t *)malloc(sizeof(control_t));
    memwb = (control_t *)malloc(sizeof(control_t));
    pc = 4;
    //Data that will override the pc and insert a nop
    idex->PCSrc = true;
    idex->pcNext = 0x02345678;
    //Dumb data in ifid that will get dumped.
    ifid->instr = 0x8d120005;
    ifid->opCode = OPC_LW;
    ifid->regRs = REG_T0;
    ifid->regRt = REG_S2;
    ifid->immed = 0x0004;
    ifid->pcNext = pc + 4;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "no nop in ifid! instr", ifid->instr == 0);
    mu_assert(_FL "no nop in ifid! opCode", ifid->opCode == 0);
    mu_assert(_FL "no nop in ifid! regRs", ifid->regRs == 0);
    mu_assert(_FL "no nop in ifid! regRt", ifid->regRt == 0);
    mu_assert(_FL "no nop in ifid! immed", ifid->immed == 0);
    mu_assert(_FL "no nop in ifid! pcNext", ifid->pcNext == 0);
    mu_assert(_FL "incorrect program counter", pc == 0x02345678);

    free(ifid);
    free(idex);
    free(exmem);
    free(memwb);
    return 0;
}

/*Load data dependency and nop insertion while maintaing the program counter*/
static char * test_load_data_dependency(){
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    exmem = (control_t *)malloc(sizeof(control_t));
    memwb = (control_t *)malloc(sizeof(control_t));
    pc = 4;
    //Load instruction detected in idex
    idex->memRead = true;
    idex->regRt = REG_S2;
    //Next instruction in ifid that depends on value from Load
    ifid->opCode = OPC_ADDI;
    ifid->regRs = REG_S2;
    hazard(ifid, idex, exmem, memwb, &pc);
    mu_assert(_FL "no nop in ifid! instr", ifid->instr == 0);
    mu_assert(_FL "no nop in ifid! opCode", ifid->opCode == 0);
    mu_assert(_FL "no nop in ifid! regRs", ifid->regRs == 0);
    mu_assert(_FL "no nop in ifid! regRt", ifid->regRt == 0);
    mu_assert(_FL "no nop in ifid! immed", ifid->immed == 0);
    mu_assert(_FL "no nop in ifid! pcNext", ifid->pcNext == 0);


    free(ifid);
    free(idex);
    free(exmem);
    free(memwb);
    return 0;
}





static char * all_tests() {
    mu_run_test(test_forwarding_exmem);
    mu_run_test(test_forwarding_memwb);
    mu_run_test(test_forwarding_exmem_not_memwb);
    mu_run_test(test_branch_flush);
    mu_run_test(test_load_data_dependency);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    } else {
        printf(__FILE__": ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != 0;
}
