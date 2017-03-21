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

int flags = MASK_DEBUG | MASK_VERBOSE | MASK_SANITY;
//int flags = MASK_VERBOSE | MASK_SANITY;

control_t * ifid;
control_t * idex;
control_t * exmem;
control_t * memwb;

pc_t pc;
bool stall;
int clock;

void execute_pipeline(){
    printf(ANSI_C_CYAN "\nPC:\n\t" ANSI_C_RESET "0x%08x\n", pc);
    writeback(memwb);
    memory(exmem, memwb);
    execute(idex, exmem);
    decode(ifid, idex);
    fetch(ifid, &pc, &stall);
    hazard(ifid, idex, exmem, memwb, &pc, &stall);
}

static char * test_basic_add(){
    reg_init();
    pipeline_init(&ifid, &idex, &exmem, &memwb, &pc, & stall, 0);
    //Load instructions
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
    //Execute pipeline six times
    clock = 0;
    for(clock = 0; clock <= 6; clock++){
        execute_pipeline();
    }
    //Check that the registers have expected values
    reg_dump();
    reg_read(REG_S1, &data);
    mu_assert(_FL "$S1 does not equal 100!", data == 100);
    reg_read(REG_S2, &data);
    mu_assert(_FL "$S2 does not equal 49!", data == 49);
    reg_read(REG_S3, &data);
    mu_assert(_FL "$S3 does not equal 149!", data == 149);

    pipeline_destroy(&ifid, &idex, &exmem, &memwb);
    return 0;
}

static char * test_bne(){
    reg_init();
    pipeline_init(&ifid, &idex, &exmem, &memwb, &pc, & stall, 0);
    //Load instructions
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
    //Execute pipeline six times
    clock = 0;
    for(clock = 0; clock <= 9; clock++){
        execute_pipeline();
    }
    //Check that the registers have expected values
    reg_dump();
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

static char * test_beq(){
    reg_init();
    pipeline_init(&ifid, &idex, &exmem, &memwb, &pc, & stall, 0);
    //Load instructions
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
    //Execute pipeline six times
    clock = 0;
    for(clock = 0; clock <= 9; clock++){
        execute_pipeline();
    }
    //Check that the registers have expected values
    reg_dump();
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



static char * all_tests() {
    //Pipeline initialization
    reg_init();
    uint64_t size = 0x140;
    uint64_t offs = 0x00;
    mem_init(size,offs);
    //Tests
    mu_run_test(test_basic_add);
    mu_run_test(test_bne);
    mu_run_test(test_beq);
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
