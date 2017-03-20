/* test/decode-test.c
* Test a instructions and make sure the output is correct
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "minunit.h"
#include "../src/decode.h"
#include "../src/types.h"
#include "../src/util.h"

int tests_run = 0;

int flags = MASK_DEBUG | MASK_VERBOSE | MASK_SANITY;

word_t i, p;
control_t *idex, *ifid;

/* Some R-Type instructions*/

static char * test_decode_add() {
    i = 0x02518820; // add, $s1, $s2, $s1
    p = 0x4;
    idex = (control_t *)malloc(sizeof(control_t));
    ifid = (control_t *)malloc(sizeof(control_t));
    ifid->instr = OPC_RTYPE;
    ifid->regRs = REG_S2;
    ifid->regRt = REG_S1;
    ifid->regRd = REG_S1;
    ifid->funct = FNC_ADD;
    ifid->pcNext = p + 4;
    decode(ifid, idex);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad regDst", idex->regDst == 1);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 badregWrite", idex->regWrite == 1);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad ALUSrc", idex->ALUSrc == 0);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad PCSrc", idex->PCSrc == 0);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad memRead", idex->memRead == 0);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad memWrite", idex->memWrite == 0);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad memToReg", idex->memToReg == 0);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad ALUop", idex->ALUop == 0);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad jump", idex->jump == 0);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad pcNext", idex->pcNext == 0x8);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad regRsValue", idex->regRsValue == 18);
    mu_assert(_FL "instruction add, $s1, $s2, $s1 bad regRtValue", idex->regRtValue == 17);
    free(idex);
    free(ifid);
    return 0;
}


/* Some I-type instructions*/
static char * test_decode_addi() {
    i = 0x22a8ff9c;                  //addi $t0, $s5, -100
    p = 0x4;
    idex = (control_t *)malloc(sizeof(control_t));
    ifid = (control_t *)malloc(sizeof(control_t));
    ifid->opCode = OPC_ADDI;
    ifid->regRs = REG_S5;
    ifid->regRt = REG_T0;
    ifid->pcNext = p + 4;
    decode(ifid, idex);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad regDst", idex->regDst == 0);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad regWrite", idex->regWrite == 1);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad ALUSrc", idex->ALUSrc == 1);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad PCSrc", idex->PCSrc == 0);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad memRead", idex->memRead == 0);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad memWrite", idex->memWrite == 0);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad memToReg", idex->memToReg == 0);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad ALUop", idex->ALUop == OPR_ADD);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad jump", idex->jump == 0);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad pcNext", idex->pcNext == 0x8);
    mu_assert(_FL "instruction addi $t0, $s5, -100 bad regRsValue", idex->regRsValue == 21);
    free(idex);
    free(ifid);
    return 0;
}


static char * test_decode_beq() {
    i = 0x12110fff;                  //beq $s0, $s1, 0x4000
    p = 0x4;
    uint32_t temp = 14;
    idex = (control_t *)malloc(sizeof(control_t));
    ifid = (control_t *)malloc(sizeof(control_t));
    ifid->opCode = OPC_BEQ;
    ifid->regRs = REG_S0;
    ifid->regRt = REG_S1;
    ifid->immed = 0x0fff;
    ifid->pcNext = p + 4;
    // Branch Taken
    reg_write(REG_S0, &temp);
    reg_write(REG_S1, &temp);
    decode(ifid, idex);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (equal) regWrite", idex->regWrite == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (equal) ALUSrc", idex->ALUSrc == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (equal) PCSrc", idex->PCSrc == 1);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (equal) memRead", idex->memRead == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (equal) memWrite", idex->memWrite == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (equal) memToReg", idex->memToReg == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (equal) ALUop", idex->ALUop == OPR_SUB);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (equal) jump", idex->jump == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (equal) pcNext", idex->pcNext == 0x4004);

    // Branch not taken
    reg_write(REG_S0, &temp);
    temp = 9;
    reg_write(REG_S1, &temp);
    decode(ifid, idex);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (not equal) regWrite", idex->regWrite == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (not equal) ALUSrc", idex->ALUSrc == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (not equal) PCSrc", idex->PCSrc == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (not equal) memRead", idex->memRead == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (not equal) memWrite", idex->memWrite == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (not equal) memToReg", idex->memToReg == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (not equal) ALUop", idex->ALUop == OPR_SUB);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (not equal) jump", idex->jump == 0);
    mu_assert(_FL "instruction beq $s0, $s1, 0x4000 (not equal) pcNext", idex->pcNext == 0x8);
    free(ifid);
    free(idex);

    return 0;
}



static char * test_decode_bne() {
    i = 0x1483fff7;                  //bne $a0, $v1, -32
    printf("Instruction: bne $a0, $v1, -32 (taken)\n");
    p = 0x40006020;
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    uint32_t temp = 0;
    reg_write(REG_A0, &temp);
    temp = 5;
    reg_write(REG_V1, &temp);
    ifid->opCode = OPC_BNE;
    ifid->regRs = REG_A0;
    ifid->regRt = REG_V1;
    ifid->immed = 0xfffffff7;
    ifid->pcNext = p + 4;
    decode(ifid, idex);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (not equal) regWrite", idex->regWrite == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (not equal) ALUSrc", idex->ALUSrc == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (not equal) PCSrc", idex->PCSrc == 1);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (not equal) memRead", idex->memRead == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (not equal) memWrite", idex->memWrite == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (not equal) memToReg", idex->memToReg == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (not equal) ALUop", idex->ALUop == OPR_SUB);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (not equal) jump", idex->jump == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (not equal) pcNext", idex->pcNext == 0x40006000);

    temp = 0;
    reg_write(REG_V1, &temp);
    decode(ifid, idex);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (equal) regWrite", idex->regWrite == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (equal) ALUSrc", idex->ALUSrc == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (equal) PCSrc", idex->PCSrc == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (equal) memRead", idex->memRead == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (equal) memWrite", idex->memWrite == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (equal) memToReg", idex->memToReg == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (equal) ALUop", idex->ALUop == OPR_SUB);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (equal) jump", idex->jump == 0);
    mu_assert(_FL "Instruction: bne $a0, $v1, -32 (equal) pcNext", idex->pcNext == 0x40006024);



    free(ifid);
    free(idex);

    return 0;
}




static char * test_decode_lw() {
    i = 0x8d120005;                  //lw $s2, 4($t0)
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    ifid->opCode = OPC_LW;
    ifid->regRs = REG_T0;
    ifid->regRt = REG_S2;
    ifid->immed = 0x0004;
    ifid->pcNext = p + 4;
    decode(ifid, idex);

    mu_assert(_FL "Instruction: lw $s2, 4($t0) regDst", idex->regDst == 0);
    mu_assert(_FL "Instruction: lw $s2, 4($t0) regWrite", idex->regWrite == 1);
    mu_assert(_FL "Instruction: lw $s2, 4($t0) ALUSrc", idex->ALUSrc == 1);
    mu_assert(_FL "Instruction: lw $s2, 4($t0) PCSrc", idex->PCSrc == 0);
    mu_assert(_FL "Instruction: lw $s2, 4($t0) memRead", idex->memRead == 1);
    mu_assert(_FL "Instruction: lw $s2, 4($t0) memWrite", idex->memWrite == 0);
    mu_assert(_FL "Instruction: lw $s2, 4($t0) memToReg", idex->memToReg == 1);
    mu_assert(_FL "Instruction: lw $s2, 4($t0) ALUop", idex->ALUop == OPR_ADDU);
    mu_assert(_FL "Instruction: lw $s2, 4($t0) jump", idex->jump == 0);
    mu_assert(_FL "Instruction: lw $s2, 4($t0) pcNext", idex->pcNext == 0x8);
    free(ifid);
    free(idex);

    return 0;
}



static char * test_decode_sw() {
    i = 0xad1f0000;                  //sw $ra, 0($t0)
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    ifid->opCode = OPC_SW;
    ifid->regRs = REG_T0;
    ifid->regRt = REG_RA;
    ifid->pcNext = p+4;
    decode(ifid, idex);

    mu_assert(_FL "Instruction: sw $ra, 0($t0) regWrite", idex->regWrite == 0);
    mu_assert(_FL "Instruction: sw $ra, 0($t0) ALUSrc", idex->ALUSrc == 1);
    mu_assert(_FL "Instruction: sw $ra, 0($t0) PCSrc", idex->PCSrc == 0);
    mu_assert(_FL "Instruction: sw $ra, 0($t0) memRead", idex->memRead == 0);
    mu_assert(_FL "Instruction: sw $ra, 0($t0) memWrite", idex->memWrite == 1);
    mu_assert(_FL "Instruction: sw $ra, 0($t0) ALUop", idex->ALUop == OPR_ADDU);
    mu_assert(_FL "Instruction: sw $ra, 0($t0) jump", idex->jump == 0);
    mu_assert(_FL "Instruction: sw $ra, 0($t0) pcNext", idex->pcNext == 0x8);

    free(ifid);
    free(idex);

    return 0;
}



static char * test_decode_slti() {
    i = 0x2a28f000;                  //slti $t0, $s1, 0xf000
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    ifid->opCode = OPC_SLTI;
    ifid->regRs = REG_S1;
    ifid->regRt = REG_T0;
    ifid->immed = 0xfffff000;
    ifid->pcNext = p + 4;
    decode(ifid, idex);
    mu_assert(_FL "slti $t0, $s1, 0xf000 regDst", idex->regDst == 0);
    mu_assert(_FL "slti $t0, $s1, 0xf000 regWrite", idex->regWrite == 1);
    mu_assert(_FL "slti $t0, $s1, 0xf000 ALUSrc", idex->ALUSrc == 1);
    mu_assert(_FL "slti $t0, $s1, 0xf000 PCSrc", idex->PCSrc == 0);
    mu_assert(_FL "slti $t0, $s1, 0xf000 memRead", idex->memRead == 0);
    mu_assert(_FL "slti $t0, $s1, 0xf000 memWrite", idex->memWrite == 0);
    mu_assert(_FL "slti $t0, $s1, 0xf000 memToReg", idex->memToReg == 0);
    mu_assert(_FL "slti $t0, $s1, 0xf000 ALUop", idex->ALUop == OPR_SLT);
    mu_assert(_FL "slti $t0, $s1, 0xf000 jump", idex->jump == 0);
    mu_assert(_FL "slti $t0, $s1, 0xf000 pcNext", idex->pcNext == 0x8);
    free(ifid);
    free(idex);

    return 0;
}


static char * test_decode_sltiu() {
    i = 0x2e28f000;                  //sltiu $t0, $s1, 0xf000
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    ifid->opCode = OPC_SLTIU;
    ifid->regRs = REG_S1;
    ifid->regRt = REG_T0;
    ifid->immed = 0x0000f000;
    ifid->pcNext = p + 4;
    decode(ifid, idex);
    mu_assert(_FL "sltiu $t0, $s1, 0xf000 regDst", idex->regDst == 0);
    mu_assert(_FL "sltiu $t0, $s1, 0xf000 regWrite", idex->regWrite == 1);
    mu_assert(_FL "sltiu $t0, $s1, 0xf000 ALUSrc", idex->ALUSrc == 1);
    mu_assert(_FL "sltiu $t0, $s1, 0xf000 PCSrc", idex->PCSrc == 0);
    mu_assert(_FL "sltiu $t0, $s1, 0xf000 memRead", idex->memRead == 0);
    mu_assert(_FL "sltiu $t0, $s1, 0xf000 memWrite", idex->memWrite == 0);
    mu_assert(_FL "sltiu $t0, $s1, 0xf000 memToReg", idex->memToReg == 0);
    mu_assert(_FL "sltiu $t0, $s1, 0xf000 ALUop", idex->ALUop == OPR_SLTU);
    mu_assert(_FL "sltiu $t0, $s1, 0xf000 jump", idex->jump == 0);
    mu_assert(_FL "sltiu $t0, $s1, 0xf000 pcNext", idex->pcNext == 0x8);
    free(ifid);
    free(idex);

    return 0;
}



static char * test_decode_j() {
    i = 0x08000804;                  //j 0x2011
    p = 0x600A1100;
    ifid = (control_t *)malloc(sizeof(control_t));
    idex = (control_t *)malloc(sizeof(control_t));
    ifid->opCode = OPC_J;
    ifid->address = 0x00002010;
    ifid->pcNext = p + 4;
    decode(ifid, idex);
    mu_assert(_FL "j 0x2011 regDst", idex->regDst == 1);
    mu_assert(_FL "j 0x2011 regWrite", idex->regWrite == 1);
    mu_assert(_FL "j 0x2011 ALUSrc", idex->ALUSrc == 0);
    mu_assert(_FL "j 0x2011 PCSrc", idex->PCSrc == 0);
    mu_assert(_FL "j 0x2011 memRead", idex->memRead == 0);
    mu_assert(_FL "j 0x2011 memWrite", idex->memWrite == 0);
    mu_assert(_FL "j 0x2011 memToReg", idex->memToReg == 0);
    mu_assert(_FL "j 0x2011 ALUop", idex->ALUop == OPR_ADDU);
    mu_assert(_FL "j 0x2011 jump", idex->jump == 1);
    mu_assert(_FL "j 0x2011 address", idex->address == 0x00008040);
    mu_assert(_FL "j 0x2011 pcNext", idex->pcNext == 0x60008040);
    free(ifid);
    free(idex);


    return 0;
}

static char * all_tests() {

    reg_init();

    //Initialize the register file with some values
    uint32_t i = 0;
    for(i = 0; i < 32; i++){
        reg_write(i, &i);
    }

    mu_run_test(test_decode_add);
    mu_run_test(test_decode_addi);
    mu_run_test(test_decode_beq);
    mu_run_test(test_decode_bne);
    mu_run_test(test_decode_lw);
    mu_run_test(test_decode_sw);
    mu_run_test(test_decode_slti);
    mu_run_test(test_decode_sltiu);
    mu_run_test(test_decode_j);


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
