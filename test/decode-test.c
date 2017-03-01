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
    mu_assert(_FL "bad assert opCode", c->opCode == 0);
    mu_assert(_FL "bad assert regRs", c->regRs == 18);
    mu_assert(_FL "bad assert regRt", c->regRt == 17);
    mu_assert(_FL "bad assert regRd", c->regRd == 17);
    mu_assert(_FL "bad assert shamt", c->shamt == 0);
    mu_assert(_FL "bad assert funct", c->funct == 0x20);
    mu_assert(_FL "bad assert regDst", c->regDst == 1);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == 0);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    free(c);
    return 0;
}

static char * test_decode_addi() {
	i = 0x22a8ff9c;									//addi $t0, $s5, -100
    printf("Instruction: addi $t0, $s5, -100\n");
	p = 0x4;
	c = (control_t *)malloc(sizeof(control_t));
	decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == 8);
    mu_assert(_FL "bad assert regRs", c->regRs == 21);
    mu_assert(_FL "bad assert regRt", c->regRt == 8);
    mu_assert(_FL "bad assert immed", c->immed == -100);
    mu_assert(_FL "bad assert regDst", c->regDst == 0);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 1);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == 0);
    mu_assert(_FL "bad assert jump", c->jump == 0);
    free(c);
    return 0;
}

static char * test_decode_and() {
	i = 0x02a44024;									//addi $t0, $s5, $a0
    printf("Instruction: addi $t0, $s5, $a0\n");
	p = 0x4;
	c = (control_t *)malloc(sizeof(control_t));
	decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == 0);
    mu_assert(_FL "bad assert regRs", c->regRs == 21);
    mu_assert(_FL "bad assert regRt", c->regRt == 4);
    mu_assert(_FL "bad assert regRd", c->regRd == 8);
    mu_assert(_FL "bad assert shamt", c->shamt == 0);
    mu_assert(_FL "bad assert funct", c->funct == 0x24);
    mu_assert(_FL "bad assert regDst", c->regDst == 1);
    mu_assert(_FL "bad assert regWrite", c->regWrite == 1);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 0);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == 4);
    mu_assert(_FL "bad assert jump", c->jump == 0);
	free(c);

    return 0;
}


static char * test_decode_beq() {
	i = 0x12110fff;									//beq $s0, $s1, 0x4000
    printf("Instruction: beq $s0, $s1, 0x4000\n");
	p = 0x4;
	c = (control_t *)malloc(sizeof(control_t));
	decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == 0x04);
    mu_assert(_FL "bad assert regRs", c->regRs == 16);
    mu_assert(_FL "bad assert regRt", c->regRt == 17);
    mu_assert(_FL "bad assert immed", c->immed == 0x0fff);  //(0x4000 - 0x4) / 4 (PC+4 and word alignment)
    mu_assert(_FL "bad assert regWrite", c->regWrite == 0);
    mu_assert(_FL "bad assert ALUSrc", c->ALUSrc == 0);
    mu_assert(_FL "bad assert PCSrc", c->PCSrc == 1);
    mu_assert(_FL "bad assert memRead", c->memRead == 0);
    mu_assert(_FL "bad assert memWrite", c->memWrite == 0);
    mu_assert(_FL "bad assert memToReg", c->memToReg == 0);
    mu_assert(_FL "bad assert ALUop", c->ALUop == 19);
    mu_assert(_FL "bad assert jump", c->jump == 0);
	free(c);

    return 0;
}

static char * test_decode_lw() {
	i = 0x8d120005;									//lw $s2, 5($t0)
    printf("Instruction: lw $s2, 5($t0)\n");
	p = 0x4;
	c = (control_t *)malloc(sizeof(control_t));
	decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == 0x23);
    mu_assert(_FL "bad assert regRs", c->regRs == 8);
    mu_assert(_FL "bad assert regRt", c->regRt == 18);
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
	free(c);

    return 0;
}

static char * test_decode_sw() {
	i = 0xad1f0000;									//sw $ra, 0($t0)
    printf("Instruction: sw $ra, 0($t0)\n");
	p = 0x4;
	c = (control_t *)malloc(sizeof(control_t));
	decode(i, p, c);
    mu_assert(_FL "bad assert opCode", c->opCode == 0x2B);
    mu_assert(_FL "bad assert regRs", c->regRs == 8);
    mu_assert(_FL "bad assert regRt", c->regRt == 31);
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
	free(c);

    return 0;
}
static char * all_tests() {
    mu_run_test(test_decode_add);
    mu_run_test(test_decode_addi);
    mu_run_test(test_decode_and);
    mu_run_test(test_decode_beq);
    mu_run_test(test_decode_lw);
    mu_run_test(test_decode_sw);
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
