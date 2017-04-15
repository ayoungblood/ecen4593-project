/* test/cache-test.c
* Test a bunch of cache instructions and watch magic right before your eyes
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "minunit.h"
#include "../src/cache.h"
#include "../src/direct.h"
#include "../src/types.h"
#include "../src/util.h"


int tests_run = 0;

int flags = MASK_DEBUG | MASK_VERBOSE | MASK_SANITY;


static char * test_cache(){
    mem_init(2<<14, 0);
    cache_init();
}

static char * all_tests() {

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
