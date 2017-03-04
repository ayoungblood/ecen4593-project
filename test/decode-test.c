/* test/decode-test.c
* Test a instructions and make sure the output is correct
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "minunit.h"
#include "../src/decode.h"
#include "../src/types.h"

int tests_run = 0;

word_t i, p;
control_t *c;

static char * test_decode_add() {
    i = 0x02518820; // add, $s1, $s2, $s1
    printf("Instruction: add $s1, $s2, $s1\n");
    p = 0x4;
    c = (control_t *)malloc(sizeof(control_t));
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_RTYPE);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_S2);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_S1);
    mu_assert(_FL "bad assert regRd", c->regRd == REG_S1);
    mu_assert(_FL "bad assert shamt", c->shamt == 0);
    mu_assert(_FL "bad assert funct", c->funct == FNC_ADD);
    mu_assert(_FL "bad assert regDst", c->regDst == 1);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == 0);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x8);
    free(c);
    return 0;
}

static char * test_decode_addi() {
    i = 0x22a8ff9c;                  //addi $t0, $s5, -100
    printf("Instruction: addi $t0, $s5, -100\n");
    p = 0x4;
    c = (control_t *)malloc(sizeof(control_t));
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_ADDI);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_S5);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_T0);
    mu_assert(_FL "bad assert immed", c->immed == -100);
    mu_assert(_FL "bad assert regDst", c->regDst == 0);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 1);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == OPR_ADD);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x8);
    free(c);
    return 0;
}

static char * test_decode_and() {
    i = 0x02a44024;                  //addi $t0, $s5, $a0
    printf("Instruction: addi $t0, $s5, $a0\n");
    p = 0x4;
    c = (control_t *)malloc(sizeof(control_t));
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_RTYPE);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_S5);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_A0);
    mu_assert(_FL "bad assert regRd", c->regRd == REG_T0);
    mu_assert(_FL "bad assert shamt", c->shamt == 0);
    mu_assert(_FL "bad assert funct", c->funct == FNC_AND);
    mu_assert(_FL "bad assert regDst", c->regDst == 1);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == OPR_AND);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x8);
    free(c);

    return 0;
}


static char * test_decode_beq() {
    i = 0x12110fff;                  //beq $s0, $s1, 0x4000
    printf("Instruction: beq $s0, $s1, 0x4000 (taken)\n");
    p = 0x4;
    c = (control_t *)malloc(sizeof(control_t));
    reg_init();
    reg_write(REG_S0, &p);
    reg_write(REG_S1, &p);
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_BEQ);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_S0);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_S1);
    mu_assert(_FL "bad assert immed", c->immed == 0x0fff);  //(0x4000 - 0x4) / 4 (PC+4 and word alignment)
    mu_assert(_FL "bad assert regWrite", c->regWrite == 0);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 1);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == OPR_SUB);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x1007);
    free(c);

    return 0;
}

static char * test_decode_beq_not_taken() {
    i = 0x12110fff;                  //beq $s0, $s1, 0x4000
    printf("Instruction: beq $s0, $s1, 0x4000 (not taken)\n");
    p = 0x4;
    c = (control_t *)malloc(sizeof(control_t));
    reg_init();
    reg_write(REG_S0, &p);
    reg_write(REG_S1, &i);
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_BEQ);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_S0);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_S1);
    mu_assert(_FL "bad assert immed", c->immed == 0x0fff);  //(0x4000 - 0x4) / 4 (PC+4 and word alignment)
    mu_assert(_FL "bad assert regWrite", c->regWrite == 0);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == OPR_SUB);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x8);
    free(c);

    return 0;
}

static char * test_decode_bne() {
    i = 0x1483fff7;                  //bne $a0, $v1, -32
    printf("Instruction: bne $a0, $v1, -32 (taken)\n");
    p = 0x40006022;
    c = (control_t *)malloc(sizeof(control_t));
    reg_init();
    reg_write(REG_A0, &p);
    reg_write(REG_V1, &i);
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_BNE);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_A0);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_V1);
    mu_assert(_FL "bad assert immed", c->immed == 0xfffffff7);  //(0x4000 - 0x4) / 4 (PC+4 and word alignment)
    mu_assert(_FL "bad assert regWrite", c->regWrite == 0);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 1);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == OPR_SUB);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x40006002);
    free(c);

    return 0;
}
static char * test_decode_bne_not_taken() {
    i = 0x1483fff7;                  //bne $a0, $v1, -32
    printf("Instruction: bne $a0, $v1, -32 (taken)\n");
    p = 0x40006022;
    c = (control_t *)malloc(sizeof(control_t));
    reg_init();
    reg_write(REG_A0, &p);
    reg_write(REG_V1, &p);
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_BNE);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_A0);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_V1);
    mu_assert(_FL "bad assert immed", c->immed == 0xfffffff7);  //(0x4000 - 0x4) / 4 (PC+4 and word alignment)
    mu_assert(_FL "bad assert regWrite", c->regWrite == 0);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == OPR_SUB);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x40006026);
    free(c);

    return 0;
}

static char * test_decode_lw() {
    i = 0x8d120005;                  //lw $s2, 5($t0)
    printf("Instruction: lw $s2, 5($t0)\n");
    p = 0x4;
    c = (control_t *)malloc(sizeof(control_t));
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_LW);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_T0);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_S2);
    mu_assert(_FL "bad assert immed", c->immed == 0x0005);
    mu_assert(_FL "bad assert regDst", c->regDst == 0);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 1);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 1);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 1);
    mu_assert(_FL "bad assert ALUop", c->ALUop == 0);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x8);
    free(c);

    return 0;
}

static char * test_decode_sw() {
    i = 0xad1f0000;                  //sw $ra, 0($t0)
    printf("Instruction: sw $ra, 0($t0)\n");
    p = 0x4;
    c = (control_t *)malloc(sizeof(control_t));
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_SW);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_T0);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_RA);
    mu_assert(_FL "bad assert immed", c->immed == 0x0000);
    //mu_assert(_FL "bad assert regDst", c->regDst == 0);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 0);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 1);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 1);
    // mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == 0);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x8);
    free(c);

    return 0;
}


static char * test_decode_slti() {
    i = 0x2a28f000;                  //slti $t0, $s1, 0xf000
    printf("Instruction: slti $t0, $s1, 0xf000\n");
    p = 0x4;
    c = (control_t *)malloc(sizeof(control_t));
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_SLTI);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_S1);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_T0);
    mu_assert(_FL "bad assert immed", c->immed == 0xfffff000);
    mu_assert(_FL "bad assert regDst", c->regDst == 0);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 1);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == OPR_SLT);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x8);
    free(c);

    return 0;
}
static char * test_decode_sltiu() {
    i = 0x2e28f000;                  //sltiu $t0, $s1, 0xf000
    printf("Instruction: sltiu $t0, $s1, 0xf000\n");
    p = 0x4;
    c = (control_t *)malloc(sizeof(control_t));
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_SLTIU);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_S1);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_T0);
    mu_assert(_FL "bad assert immed", c->immed == 0xf000);
    mu_assert(_FL "bad assert regDst", c->regDst == 0);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 1);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == OPR_SLTU);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x8);
    free(c);

    return 0;
}

static char * test_decode_j() {
    i = 0x08000804;                  //j 0x2011
    printf("Instruction: j 0x2011\n");
    p = 0x600A1100;
    c = (control_t *)malloc(sizeof(control_t));
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_J);
    mu_assert(_FL "bad assert regRs", c->regRs == 0);
    mu_assert(_FL "bad assert regRt", c->regRt == 0);
    mu_assert(_FL "bad assert regRd", c->regRd == 0);
    //mu_assert(_FL "bad assert immed", c->immed == 0x0000);
    mu_assert(_FL "bad assert regDst", c->regDst == 1);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == OPR_ADDU);
    mu_assert(_FL "bad assert jump", c->jump == 1);
    mu_assert(_FL "bad assert address", c->address == 0x00002010);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x60002010);
    free(c);


    return 0;
}

static char * test_decode_jal() {
    i = 0x0c000804;                  //jal 0x2011
    printf("Instruction: jal 0x2011\n");
    p = 0x600A1100;
    c = (control_t *)malloc(sizeof(control_t));
    decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == OPC_JAL);
    mu_assert(_FL "bad assert regRs", c->regRs == REG_ZERO);
    mu_assert(_FL "bad assert regRt", c->regRt == REG_ZERO);
    mu_assert(_FL "bad assert regRd", c->regRd == REG_RA);
    //mu_assert(_FL "bad assert immed", c->immed == 0x0000);
    mu_assert(_FL "bad assert regDst", c->regDst == 1);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == OPR_ADDU);
    mu_assert(_FL "bad assert jump", c->jump == 1);
    mu_assert(_FL "bad assert address", c->address == 0x00002010);
    mu_assert(_FL "bad assert pcNext", c->pcNext == 0x60002010);
    free(c);


    return 0;
}
static char * all_tests() {
    mu_run_test(test_decode_add);
    mu_run_test(test_decode_addi);
    mu_run_test(test_decode_and);
    mu_run_test(test_decode_beq);
    mu_run_test(test_decode_beq_not_taken);
    mu_run_test(test_decode_bne);
    mu_run_test(test_decode_bne_not_taken);
    mu_run_test(test_decode_lw);
    mu_run_test(test_decode_sw);
    mu_run_test(test_decode_j);
    mu_run_test(test_decode_jal);
    mu_run_test(test_decode_sltiu);
    mu_run_test(test_decode_slti);
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
