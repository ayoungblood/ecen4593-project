/* test/main-memory-test.c
 * Very simple unit tests for src/main_memory.c
 */

#include <stdio.h>
#include <stdbool.h>

#include "minunit.h"
#include "../src/main_memory.h"
#include "../src/types.h"

int tests_run = 0;

int flags = MASK_DEBUG | MASK_VERBOSE | MASK_SANITY;

word_t data;
uint64_t size, addr, offs;

static char * test_mem_small_word() {
    // Test a small memory with words
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

static char * test_mem_small_halfword() {
    // Test a small memory with halfwords
    size = 0x140;
    offs = 0x80;
    mem_init(size,offs);
    // check the size
    size = mem_size_b();
    mu_assert(_FL "bad assert", size == 320);
    size = mem_size_w();
    mu_assert(_FL "bad assert", size == 80);
    // write to first halfword
    data = 0xff11;
    mem_write_h(offs + 0, &data);
    // write to second halfword
    data = 0xff22;
    mem_write_h(offs + 2, &data);
    // write to third halfword
    data = 0xff33;
    mem_write_h(offs + 4, &data);
    // write to fourth halfword
    data = 0xff44;
    mem_write_h(0x86, &data);
    // write to second last halfword
    data = 0xffcc;
    mem_write_h(0x1bc, &data);
    // write to last halfword
    data = 0xffee;
    mem_write_h(0x1be, &data);
    mem_dump();

    // read from first halfword
    mem_read_h(offs, &data);
    mu_assert(_FL "bad assert", data == 0xff11);
    // read from second halfword
    mem_read_h(0x82, &data);
    mu_assert(_FL "bad assert", data == 0xff22);
    // read from third halfword
    mem_read_h(0x84, &data);
    mu_assert(_FL "bad assert", data == 0xff33);
    // read from fourth halfword
    mem_read_h(0x86, &data);
    mu_assert(_FL "bad assert", data == 0xff44);
    // read from second last halfword
    mem_read_h(0x1bc, &data);
    mu_assert(_FL "bad assert", data == 0xffcc);
    // read from last halfword
    mem_read_h(0x1be, &data);
    mu_assert(_FL "bad assert", data == 0xffee);

    // De-allocate memory and make sure it worked
    mem_close();
    size = mem_size_b();
    mu_assert(_FL "bad assert", size == 0);
    size = mem_size_w();
    mu_assert(_FL "bad assert", size == 0);
    return 0;
}

static char * test_mem_small_byte() {
    // Test a small memory with bytes
    size = 0x140;
    offs = 0x80;
    mem_init(size,offs);
    // check the size
    size = mem_size_b();
    mu_assert(_FL "bad assert", size == 320);
    size = mem_size_w();
    mu_assert(_FL "bad assert", size == 80);
    // write to first byte
    data = 0x11;
    mem_write_b(offs + 0, &data);
    // write to second byte
    data = 0x22;
    mem_write_b(offs + 1, &data);
    // write to third byte
    data = 0x33;
    mem_write_b(offs + 2, &data);
    // write to fourth byte
    data = 0x44;
    mem_write_b(0x83, &data);
    // write to fifth byte
    data = 0x55;
    mem_write_b(offs + 4, &data);
    // write to eighth byte
    data = 0x88;
    mem_write_b(offs + 7, &data);
    // write to fourth last byte
    data = 0xcc;
    mem_write_b(0x1bc, &data);
    // write to last byte
    data = 0xff;
    mem_write_b(0x1bf, &data);
    mem_dump();

    // read from first byte
    mem_read_b(offs, &data);
    mu_assert(_FL "bad assert", data == 0x11);
    // read from second byte
    mem_read_b(0x81, &data);
    mu_assert(_FL "bad assert", data == 0x22);
    // read from third byte
    mem_read_b(0x82, &data);
    mu_assert(_FL "bad assert", data == 0x33);
    // read from fourth byte
    mem_read_b(0x83, &data);
    mu_assert(_FL "bad assert", data == 0x44);
    // read from fifth byte
    mem_read_b(0x84, &data);
    mu_assert(_FL "bad assert", data == 0x55);
    // read from eigth byte
    mem_read_b(0x87, &data);
    mu_assert(_FL "bad assert", data == 0x88);
    // read from fourth last byte
    mem_read_b(0x1bc, &data);
    mu_assert(_FL "bad assert", data == 0xcc);
    // read from last byte
    mem_read_b(0x1bf, &data);
    mu_assert(_FL "bad assert", data == 0xff);

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

static char * test_mem_halfword() {
    uint32_t i;
    size = 0x00842000; // 8 MB memory
    offs = 0x00010000;
    // preload array
    uint32_t test[0x00842000]; // should be same as size
    for (i = 0; i < (size>>1); ++i) {
        test[i] = rand() & 0xffff; // data
    }
    mem_init(size,offs);
    // disable debug statements and write data
    flags &= ~(MASK_DEBUG);
    for (i = 0; i < (size>>1); ++i) {
        mem_write_h((i<<1) + offs,&test[i]);
    }
    // read data and check
    for (i = 0; i < (size>>1); ++i) {
        mem_read_h((i<<1) + offs,&data);
        mu_assert(_FL "bad assert", test[i] == data);
    }
    // restore debug flag
    flags |= MASK_DEBUG;
    mem_close();
    return 0;
}

static char * test_mem_byte() {
    uint32_t i;
    size = 0x00842000; // 8 MB memory
    offs = 0x00010000;
    // preload array
    uint32_t test[0x00842000]; // should be same as size
    for (i = 0; i < size; ++i) {
        test[i] = rand() & 0xff; // data
    }
    mem_init(size,offs);
    // disable debug statements and write data
    flags &= ~(MASK_DEBUG);
    for (i = 0; i < size; ++i) {
        mem_write_b(i + offs,&test[i]);
    }
    // read data and check
    for (i = 0; i < size; ++i) {
        mem_read_b(i + offs,&data);
        mu_assert(_FL "bad assert", test[i] == data);
    }
    // restore debug flag
    flags |= MASK_DEBUG;
    mem_close();
    return 0;
}

static char * all_tests() {
    mu_run_test(test_mem_small_word);
    mu_run_test(test_mem_small_halfword);
    mu_run_test(test_mem_small_byte);
    mu_run_test(test_mem_word);
    mu_run_test(test_mem_halfword);
    mu_run_test(test_mem_byte);
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
