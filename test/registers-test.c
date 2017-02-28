/* test/registers-test.c
 * Very simple unit tests for src/registers.c
 */

#include <stdio.h>
#include <stdbool.h>

#include "minunit.h"
#include "../src/registers.h"
#include "../src/types.h"

int tests_run = 0;

word_t a;
bool z;

static char * test_register_init() {
    reg_init();
    reg_read(0,&a);
    reg_dump();
    mu_assert(_FL "bad assert", a == 0);
    return 0;
}

static char * test_register_zero() {
    a = 0xffffffff;
    reg_write(REG_ZERO,&a);
    reg_read(REG_ZERO,&a);
    mu_assert(_FL "bad assert", a == 0);
    a = 0;
    reg_write(REG_ZERO,&a);
    reg_read(REG_ZERO,&a);
    mu_assert(_FL "bad assert", a == 0);
    return 0;
}

static char * test_registers() {
    int i;
    for (i = REG_AT; i <= REG_RA; ++i) {
        a = i + (i<<8);
        reg_write(i,&a);
    }
    for (i = REG_AT; i <= REG_RA; ++i) {
        reg_read(i,&a);
        mu_assert(_FL "bad assert", a == (i + (i<<8)));
    }
    for (i = REG_AT; i <= REG_RA; ++i) {
        a = 0x80000000 + i;
        reg_write(i,&a);
    }
    for (i = REG_AT; i <= REG_RA; ++i) {
        reg_read(i,&a);
        mu_assert(_FL "bad assert", a == 0x80000000 + i);
    }
    return 0;
}

static char * all_tests() {
    mu_run_test(test_register_init);
    mu_run_test(test_register_zero);
    mu_run_test(test_registers);
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
