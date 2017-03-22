/* test/memory-test.c
 * Unit tests of memory access pipeline stage
 */

#include <stdio.h>
#include <stdbool.h>

#include "minunit.h"
#include "../src/types.h"
#include "../src/main_memory.h"
#include "../src/memory.h"

int tests_run = 0;

int flags = 0; // MASK_DEBUG | MASK_VERBOSE | MASK_SANITY;

word_t data;
uint64_t size, addr, offs;

static char * test_mem_small_store() {
    // Tests SB, SH, SW instructions
    // Create a small memory
    size = 0x140;
    offs = 0x80;
    mem_init(size,offs);

    // Write to the memory using the pipeline stage wrapper
    control_t *exmem = (control_t *)malloc(sizeof(control_t));
    control_t *memwb = (control_t *)malloc(sizeof(control_t));
    exmem->opCode = OPC_SW; // store word
    exmem->memWrite = true; // assert write
    exmem->memRead = false; // de-assert read
    exmem->ALUresult = 0x80; // the memory address
    exmem->regRtValue = 0xdeadbeef; // the value to write
    memory(exmem, memwb);
    exmem->opCode = OPC_SW; // store word
    exmem->memWrite = true; // assert write
    exmem->memRead = false; // de-assert read
    exmem->ALUresult = 0x84; // the memory address
    exmem->regRtValue = 0xfa5f4444; // the value to write
    memory(exmem, memwb);
    exmem->opCode = OPC_SH; // store halfword
    exmem->memWrite = true; // assert write
    exmem->memRead = false; // de-assert read
    exmem->ALUresult = 0x88; // the memory address
    exmem->regRtValue = 0xff88; // the value to write
    memory(exmem, memwb);
    exmem->opCode = OPC_SH; // store halfword
    exmem->memWrite = true; // assert write
    exmem->memRead = false; // de-assert read
    exmem->ALUresult = 0x8a; // the memory address
    exmem->regRtValue = 0xffaa; // the value to write
    memory(exmem, memwb);
    exmem->opCode = OPC_SB; // store byte
    exmem->memWrite = true; // assert write
    exmem->memRead = false; // de-assert read
    exmem->ALUresult = 0x8c; // the memory address
    exmem->regRtValue = 0xfc; // the value to write
    memory(exmem, memwb);
    exmem->opCode = OPC_SB; // store byte
    exmem->memWrite = true; // assert write
    exmem->memRead = false; // de-assert read
    exmem->ALUresult = 0x8d; // the memory address
    exmem->regRtValue = 0xfd; // the value to write
    memory(exmem, memwb);
    exmem->opCode = OPC_SH; // store halfword
    exmem->memWrite = true; // assert write
    exmem->memRead = false; // de-assert read
    exmem->ALUresult = 0x8e; // the memory address
    exmem->regRtValue = 0xfeef; // the value to write
    memory(exmem, memwb);

    mem_dump();
    mem_read_w(0x80, &data);
    mu_assert(_FL "bad assert", data == 0xdeadbeef);
    mem_read_w(0x84, &data);
    mu_assert(_FL "bad assert", data == 0xfa5f4444);
    mem_read_w(0x88, &data);
    mu_assert(_FL "bad assert", data == 0xff88ffaa);
    mem_read_w(0x8c, &data);
    mu_assert(_FL "bad assert", data == 0xfcfdfeef);

    // De-allocate memory and clean up pipeline register
    mem_close();
    free(exmem);
    free(memwb);
    return 0;
}

static char * test_mem_small_load() {
    // Tests LB, LBU, LH, LHU, LW
    // Create a small memory
    size = 0x140;
    offs = 0x80;
    mem_init(size,offs);

    // Write test data to the memory directly
    data = 0xfedcba98;
    mem_write_w(0x80, &data);
    data = 0x7444f444;
    mem_write_w(0x84, &data);
    data = 0x55558878;
    mem_write_w(0x88, &data);

    mem_dump();

    // Read from the memory using the pipeline stage wrapper
    control_t *exmem = (control_t *)malloc(sizeof(control_t));
    control_t *memwb = (control_t *)malloc(sizeof(control_t));
    exmem->opCode = OPC_LW; // load word
    exmem->memRead = true; // assert read
    exmem->memWrite = false; // de-assert write
    exmem->ALUresult = 0x80; // the memory address
    memory(exmem, memwb);
    mu_assert(_FL "bad assert", memwb->memData == 0xfedcba98);
    exmem->opCode = OPC_LH; // load halfword
    exmem->memRead = true; // assert read
    exmem->memWrite = false; // de-assert write
    exmem->ALUresult = 0x84; // the memory address
    memory(exmem, memwb);
    mu_assert(_FL "bad assert", memwb->memData == 0x7444);
    exmem->opCode = OPC_LH; // load halfword
    exmem->memRead = true; // assert read
    exmem->memWrite = false; // de-assert write
    exmem->ALUresult = 0x86; // the memory address
    memory(exmem, memwb);
    mu_assert(_FL "bad assert", memwb->memData == 0xfffff444); // sign-extended
    exmem->opCode = OPC_LHU; // load halfword unsigned
    exmem->memRead = true; // assert read
    exmem->memWrite = false; // de-assert write
    exmem->ALUresult = 0x86; // the memory address
    memory(exmem, memwb);
    mu_assert(_FL "bad assert", memwb->memData == 0xf444); // not sign-extended
    exmem->opCode = OPC_LB; // load byte
    exmem->memRead = true; // assert read
    exmem->memWrite = false; // de-assert write
    exmem->ALUresult = 0x8b; // the memory address
    memory(exmem, memwb);
    mu_assert(_FL "bad assert", memwb->memData == 0x78);
    exmem->opCode = OPC_LB; // load byte
    exmem->memRead = true; // assert read
    exmem->memWrite = false; // de-assert write
    exmem->ALUresult = 0x8a; // the memory address
    memory(exmem, memwb);
    mu_assert(_FL "bad assert", memwb->memData == 0xffffff88); // sign-extended
    exmem->opCode = OPC_LBU; // load byte unsigned
    exmem->memRead = true; // assert read
    exmem->memWrite = false; // de-assert write
    exmem->ALUresult = 0x8a; // the memory address
    memory(exmem, memwb);
    mu_assert(_FL "bad assert", memwb->memData == 0x88); // not sign-extended

    // De-allocate memory and clean up pipeline register
    mem_close();
    free(exmem);
    free(memwb);
    return 0;
}

static char * all_tests() {
    mu_run_test(test_mem_small_store);
    mu_run_test(test_mem_small_load);
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
