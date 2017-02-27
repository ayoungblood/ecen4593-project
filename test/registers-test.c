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

static char * test_register() {

    return 0;
}

static char * all_tests() {
    mu_run_test(test_register_init);
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
