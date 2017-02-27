/* test/decode-test.c
*	Test a instructions and make sure the output is correct
*/


#include <stdio.h>
#include <stdbool.h>

#include "minunit.h"
#include "../src/decode.h"
#include "../src/types.h"


int tests_run = 0;

word_t i, p;
control_t *c;

static char * test_decode_add() {
	i = 0x02518820;							//add, $s1, $s2, $s1
	p = 0x4;
	c = malloc(sizeof(control_t));
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
    return 0;
}

static char * test_decode_and() {

    return 0;
}

static char * all_tests() {
    mu_run_test(test_decode_add);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != 0;
}

  