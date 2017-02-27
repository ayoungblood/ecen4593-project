/* test/alu-test.c
 * Very simple unit tests for src/alu.c
 */

#include <stdio.h>
#include <stdbool.h>

#include "minunit.h"
#include "../src/alu.h"
#include "../src/types.h"

int tests_run = 0;

word_t s, t, d;
bool z;
int rv;

static char * test_ALU_ADD() {
    // These should not overflow
    s = t = 0x0;
    rv = alu(OPR_ADD,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0);
    mu_assert(_FL "bad assert", z == true);
    s = 0x2;
    t = 0xffffffff; // -1
    rv = alu(OPR_ADD,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 1);
    mu_assert(_FL "bad assert", z == false);
    s = 0x7fffffff; // 2147483647
    t = 0x80000001; // -2147483647
    rv = alu(OPR_ADD,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0);
    mu_assert(_FL "bad assert", z == true);
    // These should overflow, and should not modify d and z
    s = 0x7fffffff; // 2147483647
    t = 0x7fffffff; // 2147483647
    d = 0x55;
    z = false;
    rv = alu(OPR_ADD,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0x55);
    mu_assert(_FL "bad assert", z == false);
    s = 0x7fffffff; // 2147483647
    t = 0x1; // 1
    d = 0xffaa;
    z = true;
    rv = alu(OPR_ADD,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0xffaa);
    mu_assert(_FL "bad assert", z == true);
    s = 0xffffffff; // -1
    t = 0x80000000; // -2147483648
    d = 0x55aa55aa;
    z = false;
    rv = alu(OPR_ADD,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0x55aa55aa);
    mu_assert(_FL "bad assert", z == false);
    return 0;
}

static char * test_ALU_AND() {
    // AND cannot overflow
    s = t = 0x0;
    rv = alu(OPR_AND,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0x0);
    mu_assert(_FL "bad assert", z == true);
    s = 0x0f0f0f0f;
    t = 0xf0f0f0f0;
    rv = alu(OPR_AND,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0x0);
    mu_assert(_FL "bad assert", z == true);
    s = 0xff00a5a5;
    t = 0xf0f0a55a;
    rv = alu(OPR_AND,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0xf000a500);
    mu_assert(_FL "bad assert", z == false);
    s = 0xffffffff;
    t = 0xffffffff;
    rv = alu(OPR_AND,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0xffffffff);
    mu_assert(_FL "bad assert", z == false);
    return 0;
}

static char * test_ALU_NOR() {
    // NOR cannot overflow
    s = t = 0x0;
    rv = alu(OPR_NOR,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0xffffffff);
    mu_assert(_FL "bad assert", z == false);
    s = 0x0f0f0f0f;
    t = 0xf0f0f0f0;
    rv = alu(OPR_NOR,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0x0);
    mu_assert(_FL "bad assert", z == true);
    s = 0xff00a5a5;
    t = 0xf0f0a55a;
    rv = alu(OPR_NOR,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0x000f5a00);
    mu_assert(_FL "bad assert", z == false);
    s = 0xffffffff;
    t = 0xffffffff;
    rv = alu(OPR_NOR,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0x0);
    mu_assert(_FL "bad assert", z == true);
    return 0;
}

static char * test_ALU_OR() {
    // OR cannot overflow
    s = t = 0x0;
    rv = alu(OPR_OR,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0x0);
    mu_assert(_FL "bad assert", z == true);
    s = 0x0f0f0f0f;
    t = 0xf0f0f0f0;
    rv = alu(OPR_OR,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0xffffffff);
    mu_assert(_FL "bad assert", z == false);
    s = 0xff00a5a5;
    t = 0xf0f0a55a;
    rv = alu(OPR_OR,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0xfff0a5ff);
    mu_assert(_FL "bad assert", z == false);
    s = 0xffffffff;
    t = 0xffffffff;
    rv = alu(OPR_OR,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0xffffffff);
    mu_assert(_FL "bad assert", z == false);
    return 0;
}

static char * test_ALU_SUB() {
    // These should not overflow
    s = t = 0x0;
    rv = alu(OPR_SUB,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0);
    mu_assert(_FL "bad assert", z == true);
    s = 0x2;
    t = 0xffffffff; // -1
    rv = alu(OPR_SUB,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 3);
    mu_assert(_FL "bad assert", z == false);
    s = 0x7fffffff; // 2147483647
    t = 0x7ffffff0; // 2147483632
    rv = alu(OPR_SUB,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 15);
    mu_assert(_FL "bad assert", z == false);
    s = 0xfffffff0; // -16
    t = 0xfffffff8; // -8
    rv = alu(OPR_SUB,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == -8);
    mu_assert(_FL "bad assert", z == false);
    // These should overflow, and should not modify d and z
    s = 0x1; // -2
    t = 0x7fffffff; // 2147483647
    d = 0x55;
    z = false;
    rv = alu(OPR_SUB,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0x55);
    mu_assert(_FL "bad assert", z == false);
    s = 0x80000000; // -2147483648
    t = 0x2; // 1
    d = 0x55;
    z = true;
    rv = alu(OPR_SUB,s,t,&d,&z);
    printf("s: %d, t: %d, d: %d, z: %d\n",s,t,d,z);
    mu_assert(_FL "bad assert", d == 0xffaa);
    mu_assert(_FL "bad assert", z == true);
    s = 0xffffffff; // -1
    t = 0x80000000; // -2147483648
    d = 0x55aa55aa;
    z = false;
    rv = alu(OPR_SUB,s,t,&d,&z);
    mu_assert(_FL "bad assert", d == 0x55aa55aa);
    mu_assert(_FL "bad assert", z == false);
    return 0;
}

static char * all_tests() {
    mu_run_test(test_ALU_ADD);
    mu_run_test(test_ALU_AND);
    mu_run_test(test_ALU_NOR);
    mu_run_test(test_ALU_OR);
    //mu_run_test(test_ALU_SUB);
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