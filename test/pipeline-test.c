/* test/pipeline-test.c
* Some "unit" tests that insert multiple instructions into the pipeline
* and test to make sure we have an expected result.
* The goal of these tests is to make sure we stress out the edge cases of
* our pipeline and ensure expected results
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "minunit.h"
#include "../src/alu.h"
#include "../src/decode.h"
#include "../src/main_memory.h"
#include "../src/memory.h"
#include "../src/fetch.h"
#include "../src/write.h"
#include "../src/registers.h"
#include "../src/types.h"
#include "../src/util.h"
#include "../src/hazard.h"

int tests_run = 0;

extern int flags;
extern profile_t *prof;

control_t * ifid;
control_t * idex;
control_t * exmem;
control_t * memwb;

pc_t pc;
int clock;

cache_config_t cache_config = {
    .mode           = CACHE_DISABLE,
    .data_enabled   = false,
    .data_size      = 1024,
    .data_block     = 4,
    .data_type      = CACHE_DIRECT,
    .data_wpolicy   = CACHE_WRITETHROUGH,
    .inst_enabled   = false,
    .inst_size      = 1024,
    .inst_block     = 4,
    .inst_type      = CACHE_DIRECT,
    .inst_wpolicy   = CACHE_WRITETHROUGH,
    .size           = 1024,
    .block          = 4,
    .type           = CACHE_DIRECT,
    .wpolicy        = CACHE_WRITETHROUGH,
};

void execute_pipeline() {
    bcprintf(ANSI_C_CYAN,"PC:\n");
    bprintf("\t0x%08x\n", pc);
    writeback(memwb);
    memory(exmem, memwb, &cache_config);
    execute(idex, exmem);
    decode(ifid, idex);
    fetch(ifid, &pc, &cache_config);
    hazard(ifid, idex, exmem, memwb, &pc, &cache_config);
}

static char * test_basic_add() {
    reg_init();
    pipeline_init(&ifid, &idex, &exmem, &memwb, &pc, 0);
    // Load instructions
    pc = 0x00000000;
    word_t data = 0x20110064;       //addi $s1, $zero, 100
    mem_write_w(pc, &data);
    data = 0x20120031;              //addi $s2, $zero, 49
    mem_write_w(pc+4, &data);
    data = 0x02519820;              //add $s3, $s2, $s1
    mem_write_w(pc+8, &data);
    data = 0x00000000;              //nop
    mem_write_w(pc+12, &data);
    mem_write_w(pc+16, &data);
    mem_write_w(pc+20, &data);
    // Execute pipeline six times
    clock = 0;
    for (clock = 0; clock <= 6; clock++) {
        execute_pipeline();
    }
    // Check that the registers have expected values
    //reg_dump();
    reg_read(REG_S1, &data);
    mu_assert(_FL "$S1 does not equal 100!", data == 100);
    reg_read(REG_S2, &data);
    mu_assert(_FL "$S2 does not equal 49!", data == 49);
    reg_read(REG_S3, &data);
    mu_assert(_FL "$S3 does not equal 149!", data == 149);

    pipeline_destroy(&ifid, &idex, &exmem, &memwb);
    return 0;
}

static char * test_bne() {
    reg_init();
    pipeline_init(&ifid, &idex, &exmem, &memwb, &pc, 0);
    // Load instructions
    pc = 0x00000000;
    word_t data = 0x20110064;       //addi $s1, $zero, 100
    mem_write_w(pc, &data);
    data = 0x20120040;              //addi $s2, $zero, 64
    mem_write_w(pc+4, &data);
    data = 0x16320002;              //bne $s1, $s2, 0x14
    mem_write_w(pc+8, &data);
    data = 0xae510000;              //sw $s1, 0($s2)
    mem_write_w(pc+12, &data);
    data = 0x02324022;              //sub $t0, $s1, $s2
    mem_write_w(pc+16, &data);
    data = 0x001160c2;              //srl $t4, $s1, 3
    mem_write_w(pc+20, &data);
    data = 0x00000000;              //nop
    mem_write_w(pc+24, &data);
    mem_write_w(pc+28, &data);
    mem_write_w(pc+32, &data);
    mem_write_w(pc+36, &data);
    // Execute pipeline six times
    clock = 0;
    for (clock = 0; clock <= 9; clock++) {
        execute_pipeline();
    }
    // Check that the registers have expected values
    //reg_dump();
    reg_read(REG_S1, &data);
    mu_assert(_FL "$S1 does not equal 100!", data == 100);
    reg_read(REG_S2, &data);
    mu_assert(_FL "$S2 does not equal 64!", data == 64);
    reg_read(REG_T4, &data);
    mu_assert(_FL "$T4 does not equal 12!", data == 12);
    reg_read(REG_T0, &data);
    mu_assert(_FL "Incorrect instruction executed: $T0 = 36", data != 36);


    pipeline_destroy(&ifid, &idex, &exmem, &memwb);
    return 0;
}

static char * test_beq() {
    reg_init();
    pipeline_init(&ifid, &idex, &exmem, &memwb, &pc, 0);
    // Load instructions
    pc = 0x00000000;
    word_t data = 0x20110064;       //addi $s1, $zero, 100
    mem_write_w(pc, &data);
    data = 0x20120064;              //addi $s2, $zero, 100
    mem_write_w(pc+4, &data);
    data = 0x12320002;              //beq $s1, $s2, 0x14
    mem_write_w(pc+8, &data);
    data = 0xae510000;              //sw $s1, 0($s2)
    mem_write_w(pc+12, &data);
    data = 0x2008ffec;              //addi $t0, $zero, -20
    mem_write_w(pc+16, &data);
    data = 0x001160c2;              //srl $t4, $s1, 3
    mem_write_w(pc+20, &data);
    data = 0x00000000;              //nop
    mem_write_w(pc+24, &data);
    mem_write_w(pc+28, &data);
    mem_write_w(pc+32, &data);
    // Execute pipeline six times
    clock = 0;
    for (clock = 0; clock <= 9; clock++) {
        execute_pipeline();
    }
    //C heck that the registers have expected values
    // reg_dump();
    reg_read(REG_S1, &data);
    mu_assert(_FL "$S1 does not equal 100!", data == 100);
    reg_read(REG_S2, &data);
    mu_assert(_FL "$S2 does not equal 64!", data == 100);
    reg_read(REG_T4, &data);
    mu_assert(_FL "$T4 does not equal 12!", data == 12);
    reg_read(REG_T0, &data);
    mu_assert(_FL "Incorrect instruction executed: $T0 = -20", data != -20);

    pipeline_destroy(&ifid, &idex, &exmem, &memwb);
    return 0;
}

static char * test_load_dependency() {
    //0x20100800        addi $s0, $zero, 2048
    //0x2011000a        addi $s1, $zero, 10
    //0xae110000        sw $s1, 0($s0)
    //0x22310001        addi $s1, $s1, 1
    //0xae110004        sw $s1, 4($s0)
    //0x22310001        addi $s1, $s1, 1
    //0x8e120000        lw $s2, 0($s0)
    //0x8e130004        lw $s3, 4($s0)
    //0x0253a020        add $s4, $s2, $s3
    reg_init();
    pipeline_init(&ifid, &idex, &exmem, &memwb, &pc, 0);
    // Load instructions
    pc = 0x00000000;
    word_t data = 0x20100800;       //addi $s0, $zero, 2048
    mem_write_w(pc, &data);
    data = 0x2011000a;              //addi $s1, $zero, 10
    mem_write_w(pc+4, &data);
    data = 0xae110000;              //sw $s1, 0($s0)
    mem_write_w(pc+8, &data);
    data = 0x22310001;              //addi $s1, $s1, 1
    mem_write_w(pc+12, &data);
    data = 0xae110004;              //sw $s1, 4($s0)
    mem_write_w(pc+16, &data);
    data = 0x22310001;              //addi $s1, $s1, 1
    mem_write_w(pc+20, &data);
    data = 0x8e120000;              //lw $s2, 0($s0)
    mem_write_w(pc+24, &data);
    data = 0x8e130004;              //lw $s3, 4($s0)
    mem_write_w(pc+28, &data);
    data = 0x0253a020;              //add $s4, $s2, $s3
    mem_write_w(pc+32, &data);
    data = 0x00000000;              //nop
    mem_write_w(pc+36, &data);
    mem_write_w(pc+40, &data);
    mem_write_w(pc+44, &data);
    mem_write_w(pc+48, &data);
    // Execute pipeline six times
    clock = 0;
    for (clock = 0; clock <= 13; clock++) {
        execute_pipeline();
    }
    // Check that the registers have expected values
    // reg_dump();
    reg_read(REG_S0, &data);
    mu_assert(_FL "$S0 does not equal 2048!", data == 2048);
    reg_read(REG_S1, &data);
    mu_assert(_FL "$S1 does not equal 12!", data == 12);
    reg_read(REG_S2, &data);
    mu_assert(_FL "$S2 does not equal 10!", data == 10);
    reg_read(REG_S3, &data);
    mu_assert(_FL "$S3 does not equal 11!", data == 11);
    reg_read(REG_S4, &data);
    mu_assert(_FL "$S4 does not equal 21!", data == 21);

    pipeline_destroy(&ifid, &idex, &exmem, &memwb);
    return 0;
}

static char * all_tests() {
    // Pipeline initialization
    reg_init();
    mem_init(0x3000,0x0);
    // Tests
    mu_run_test(test_basic_add);
    mu_run_test(test_bne);
    mu_run_test(test_beq);
    mu_run_test(test_load_dependency);
    return 0;
}

int main(int argc, char **argv) {
    flags = MASK_SANITY;
    prof = (profile_t*)malloc(sizeof(profile_t));
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    } else {
        printf(__FILE__": ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    free(prof);
    return result != 0;
}
