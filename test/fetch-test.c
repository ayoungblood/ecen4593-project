/* test/fetch-test.c
* some unit tests for fetching an Instruction
*/


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "minunit.h"
#include "../src/fetch.h"
#include "../src/types.h"
#include "../src/util.h"
#include "../src/registers.h"
#include "../src/main_memory.h"

int tests_run = 0;

int flags = MASK_DEBUG | MASK_VERBOSE | MASK_SANITY;

word_t i, p;
control_t  *ifid;
bool stall;

static char * test_fetch_add() {
    i = 0x02518820; // add, $s1, $s2, $s1
    p = 0x4;
    stall = false;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &stall);
    mu_assert(_FL "instruction add $s1, $s2, $s1 bad opCode", ifid->opCode == OPC_RTYPE);
    mu_assert(_FL "instruction add $s1, $s2, $s1 bad funct", ifid->funct == FNC_ADD);
    mu_assert(_FL "instruction add $s1, $s2, $s1 bad Rd", ifid->regRd == REG_S1);
    mu_assert(_FL "instruction add $s1, $s2, $s1 bad Rs", ifid->regRs == REG_S2);
    mu_assert(_FL "instruction add $s1, $s2, $s1 bad Rt", ifid->regRt == REG_S1);
    mu_assert(_FL "instruction add $s1, $s2, $s1 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_sub() {
    i = 0x03e2e822; // sub $sp, $ra, $v0
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &stall);
    mu_assert(_FL "instruction sub $sp, $ra, $v0 bad opCode", ifid->opCode == OPC_RTYPE);
    mu_assert(_FL "instruction sub $sp, $ra, $v0 bad funct", ifid->funct == FNC_SUB);
    mu_assert(_FL "instruction sub $sp, $ra, $v0 bad Rd", ifid->regRd == REG_SP);
    mu_assert(_FL "instruction sub $sp, $ra, $v0 bad Rs", ifid->regRs == REG_RA);
    mu_assert(_FL "instruction sub $sp, $ra, $v0 bad Rt", ifid->regRt == REG_V0);
    mu_assert(_FL "instruction sub $sp, $ra, $v0 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_sll() {
    i = 0x00098280; // sll $s0, $t1, 10
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &stall);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad opCode", ifid->opCode == OPC_RTYPE);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad funct", ifid->funct == FNC_SLL);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad Rd", ifid->regRd == REG_S0);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad shamt", ifid->shamt == 10);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad Rt", ifid->regRt == REG_T1);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * all_tests() {
    uint64_t size = 0x140;
    uint64_t offs = 0x00;
    mem_init(size,offs);

    mu_run_test(test_fetch_add);
    mu_run_test(test_fetch_sub);
    mu_run_test(test_fetch_sll);

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
