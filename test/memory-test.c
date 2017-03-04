/* test/mem-test.c
 * Very simple unit tests for src/memory.c
 */

#include <stdio.h>
#include <stdbool.h>

#include "minunit.h"
#include "../src/memory.h"
#include "../src/types.h"

int tests_run = 0;

int flags = MASK_DEBUG | MASK_VERBOSE | MASK_SANITY;

word_t data;
uint64_t size, addr, offs;

static char * test_mem_small() {
    // Test a small memory
    size = 0x140;
    offs = 0x80;
    mem_init(size,offs);
    // check the size
    size = mem_size_b();
    mu_assert(_FL "bad assert", size == 320);
    size = mem_size_w();
    mu_assert(_FL "bad assert", size == 80);
    // write to the first word
    data = 0xf0a50000;
    mem_write_w(offs + 0, &data);
    // write to the second word
    data = 0xdead1111;
    mem_write_w(0x84, &data);
    // write to the third word
    data = 0xdead2222;
    mem_write_w(offs + 8, &data);
    // write to third last word
    data = 0xdead4444;
    mem_write_w(offs + 308, &data);
    // write to second last word
    data = 0xdead8888;
    mem_write_w(offs + 312, &data);
    // write to the last word
    data = 0xf0a5cccc;
    mem_write_w(offs + 316, &data);

    mem_dump();
    // read from first word
    mem_read_w(offs, &data);
    mu_assert(_FL "bad assert", data == 0xf0a50000);
    // read from second word
    mem_read_w(0x84, &data);
    mu_assert(_FL "bad assert", data == 0xdead1111);
    // read from third word
    mem_read_w(0x88, &data);
    mu_assert(_FL "bad assert", data == 0xdead2222);
    // read from third last word
    mem_read_w(0x1b4, &data);
    mu_assert(_FL "bad assert", data == 0xdead4444);
    // read from second last word
    mem_read_w(0x1b8, &data);
    mu_assert(_FL "bad assert", data == 0xdead8888);
    // read from last word
    mem_read_w(0x1bc, &data);
    mu_assert(_FL "bad assert", data == 0xf0a5cccc);

    // De-allocate memory and make sure it worked
    mem_close();
    size = mem_size_b();
    mu_assert(_FL "bad assert", size == 0);
    size = mem_size_w();
    mu_assert(_FL "bad assert", size == 0);
    return 0;
}

static char * test_mem_word() {
    uint32_t i;
    size = 0x00842000; // 8 MB memory
    offs = 0x00010000;
    // preload array
    uint32_t test[0x00842000]; // should be same as size
    for (i = 0; i < (size>>2); ++i) {
        test[i] = rand()<<1; // data
    }
    mem_init(size,offs);
    // disable debug statements and write data
    flags &= ~(MASK_DEBUG);
    for (i = 0; i < (size>>2); ++i) {
        mem_write_w((i<<2) + offs,&test[i]);
    }
    // read data and check
    for (i = 0; i < (size>>2); ++i) {
        mem_read_w((i<<2) + offs,&data);
        mu_assert(_FL "bad assert", test[i] == data);
    }
    // restore debug flag
    flags |= MASK_DEBUG;
    mem_close();
    return 0;
}

static char * all_tests() {
    mu_run_test(test_mem_small);
    mu_run_test(test_mem_word);
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
