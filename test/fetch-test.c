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

extern int flags;

word_t i, p;
control_t  *ifid;

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

static char * test_fetch_add() {
    i = 0x02518820; // add, $s1, $s2, $s1
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
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
    fetch(ifid, &p, &cache_config);
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
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad opCode", ifid->opCode == OPC_RTYPE);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad funct", ifid->funct == FNC_SLL);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad Rd", ifid->regRd == REG_S0);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad shamt", ifid->shamt == 10);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad Rt", ifid->regRt == REG_T1);
    mu_assert(_FL "instruction sll $s0, $t1, 10 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_jr() {
    i = 0x03e00008; // jr $ra
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction jr $ra bad opCode", ifid->opCode == OPC_RTYPE);
    mu_assert(_FL "instruction jr $ra bad funct", ifid->funct == FNC_JR);
    mu_assert(_FL "instruction jr $ra bad Rs", ifid->regRs == REG_RA);
    mu_assert(_FL "instruction jr $ra bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_nor() {
    i = 0x02e26027; // nor $t4, $s7, $v0
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction nor $t4, $s7, $v0 bad opCode", ifid->opCode == OPC_RTYPE);
    mu_assert(_FL "instruction nor $t4, $s7, $v0 bad funct", ifid->funct == FNC_NOR);
    mu_assert(_FL "instruction nor $t4, $s7, $v0 bad Rd", ifid->regRd == REG_T4);
    mu_assert(_FL "instruction nor $t4, $s7, $v0 bad Rs", ifid->regRs == REG_S7);
    mu_assert(_FL "instruction nor $t4, $s7, $v0 bad Rt", ifid->regRt == REG_V0);
    mu_assert(_FL "instruction nor $t4, $s7, $v0 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_sltu() {
    i = 0x0085482b; // sltu $t1, $a0, $a1
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction sltu $t1, $a0, $a1 bad opCode", ifid->opCode == OPC_RTYPE);
    mu_assert(_FL "instruction sltu $t1, $a0, $a1 bad funct", ifid->funct == FNC_SLTU);
    mu_assert(_FL "instruction sltu $t1, $a0, $a1 bad Rd", ifid->regRd == REG_T1);
    mu_assert(_FL "instruction sltu $t1, $a0, $a1 bad Rs", ifid->regRs == REG_A0);
    mu_assert(_FL "instruction sltu $t1, $a0, $a1 bad Rt", ifid->regRt == REG_A1);
    mu_assert(_FL "instruction sltu $t1, $a0, $a1 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_xor() {
    i = 0x039d5826; // xor $t3, $gp, $sp
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction xor $t3, $gp, $sp bad opCode", ifid->opCode == OPC_RTYPE);
    mu_assert(_FL "instruction xor $t3, $gp, $sp bad funct", ifid->funct == FNC_XOR);
    mu_assert(_FL "instruction xor $t3, $gp, $sp bad Rd", ifid->regRd == REG_T3);
    mu_assert(_FL "instruction xor $t3, $gp, $sp bad Rs", ifid->regRs == REG_GP);
    mu_assert(_FL "instruction xor $t3, $gp, $sp bad Rt", ifid->regRt == REG_SP);
    mu_assert(_FL "instruction xor $t3, $gp, $sp bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

/*Some I-Type instructions*/

static char * test_fetch_addi() {
    i = 0x2252ff9c; // addi $s2, $s2, -100
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction addi $s2, $s2, -100 bad opCode", ifid->opCode == OPC_ADDI);
    mu_assert(_FL "instruction addi $s2, $s2, -100 bad Rs", ifid->regRs == REG_S2);
    mu_assert(_FL "instruction addi $s2, $s2, -100 bad Rt", ifid->regRt == REG_S2);
    mu_assert(_FL "instruction addi $s2, $s2, -100 bad immed", ifid->immed == 0xffffff9c);
    mu_assert(_FL "instruction addi $s2, $s2, -100 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_andi() {
    i = 0x32888000; // andi $t0, $s4, 0x8000
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction andi $t0, $s4, 0x8000 bad opCode", ifid->opCode == OPC_ANDI);
    mu_assert(_FL "instruction andi $t0, $s4, 0x8000 bad Rs", ifid->regRs == REG_S4);
    mu_assert(_FL "instruction andi $t0, $s4, 0x8000 bad Rt", ifid->regRt == REG_T0);
    mu_assert(_FL "instruction andi $t0, $s4, 0x8000 bad immed", ifid->immed == 0x00008000);
    mu_assert(_FL "instruction andi $t0, $s4, 0x8000 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_beq() {
    i = 0x1257fff9; // beq $s2, $s7, -24
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction beq $s2, $s7, -6 bad opCode", ifid->opCode == OPC_BEQ);
    mu_assert(_FL "instruction beq $s2, $s7, -6 bad Rs", ifid->regRs == REG_S2);
    mu_assert(_FL "instruction beq $s2, $s7, -6 bad Rt", ifid->regRt == REG_S7);
    mu_assert(_FL "instruction beq $s2, $s7, -6 bad immed", ifid->immed == 0xfffffff9);
    mu_assert(_FL "instruction beq $s2, $s7, -6 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_lb() {
    i = 0x8208000c; // lb $t0, 12($s0)
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction lb $t0, 12($s0) bad opCode", ifid->opCode == OPC_LB);
    mu_assert(_FL "instruction lb $t0, 12($s0) bad Rs", ifid->regRs == REG_S0);
    mu_assert(_FL "instruction lb $t0, 12($s0) bad Rt", ifid->regRt == REG_T0);
    mu_assert(_FL "instruction lb $t0, 12($s0) bad immed", ifid->immed == 0xC);
    mu_assert(_FL "instruction lb $t0, 12($s0) bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_lhu() {
    i = 0x950e0000; // lhu $t6, 0($t0)
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction lb $t0, 0($s0) bad opCode", ifid->opCode == OPC_LHU);
    mu_assert(_FL "instruction lb $t0, 0($s0) bad Rs", ifid->regRs == REG_T0);
    mu_assert(_FL "instruction lb $t0, 0($s0) bad Rt", ifid->regRt == REG_T6);
    mu_assert(_FL "instruction lb $t0, 0($s0) bad immed", ifid->immed == 0x0);
    mu_assert(_FL "instruction lb $t0, 0($s0) bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_ori() {
    i = 0x348e8000; // ori $t6, $a0, 0x8000
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction ori $t6, $a0, 0x8000 bad opCode", ifid->opCode == OPC_ORI);
    mu_assert(_FL "instruction ori $t6, $a0, 0x8000 bad Rs", ifid->regRs == REG_A0);
    mu_assert(_FL "instruction ori $t6, $a0, 0x8000 bad Rt", ifid->regRt == REG_T6);
    mu_assert(_FL "instruction ori $t6, $a0, 0x8000 bad immed", ifid->immed == 0x00008000);
    mu_assert(_FL "instruction ori $t6, $a0, 0x8000 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_sb() {
    i = 0xa2510003; // sb $s1, 3($s2)
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction sb $s1, 3($s2) bad opCode", ifid->opCode == OPC_SB);
    mu_assert(_FL "instruction sb $s1, 3($s2) bad Rs", ifid->regRs == REG_S2);
    mu_assert(_FL "instruction sb $s1, 3($s2) bad Rt", ifid->regRt == REG_S1);
    mu_assert(_FL "instruction sb $s1, 3($s2) bad immed", ifid->immed == 0x3);
    mu_assert(_FL "instruction sb $s1, 3($s2) bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_sw() {
    i = 0xae510003; // sw $s1, 3($s2)
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction sw $s1, 3($s2) bad opCode", ifid->opCode == OPC_SW);
    mu_assert(_FL "instruction sw $s1, 3($s2) bad Rs", ifid->regRs == REG_S2);
    mu_assert(_FL "instruction sw $s1, 3($s2) bad Rt", ifid->regRt == REG_S1);
    mu_assert(_FL "instruction sw $s1, 3($s2) bad immed", ifid->immed == 0x3);
    mu_assert(_FL "instruction sw $s1, 3($s2) bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_sltiu() {
    i = 0x2e2aff9d; // sltiu $t2, $s1, 68719476637
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction sltiu $t2, $s1, 68719476637 bad opCode", ifid->opCode == OPC_SLTIU);
    mu_assert(_FL "instruction sltiu $t2, $s1, 68719476637 bad Rs", ifid->regRs == REG_S1);
    mu_assert(_FL "instruction sltiu $t2, $s1, 68719476637 bad Rt", ifid->regRt == REG_T2);
    mu_assert(_FL "instruction sltiu $t2, $s1, 68719476637 bad immed", ifid->immed == 0xff9d);
    mu_assert(_FL "instruction sltiu $t2, $s1, 68719476637 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_xori() {
    i = 0x39d25555; // xori $s2, $t6, 0x5555
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction xori $s2, $t6, 0x5555 bad opCode", ifid->opCode == OPC_XORI);
    mu_assert(_FL "instruction xori $s2, $t6, 0x5555 bad Rs", ifid->regRs == REG_T6);
    mu_assert(_FL "instruction xori $s2, $t6, 0x5555 bad Rt", ifid->regRt == REG_S2);
    mu_assert(_FL "instruction xori $s2, $t6, 0x5555 bad immed", ifid->immed == 0x5555);
    mu_assert(_FL "instruction xori $s2, $t6, 0x5555 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

/* J-Type Instructions*/

static char * test_fetch_j() {
    i = 0x0800048d; // j 0x1234
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction j 0x1234 bad opCode", ifid->opCode == OPC_J);
    mu_assert(_FL "instruction j 0x1234 bad address", ifid->address == 0x48d);
    mu_assert(_FL "instruction j 0x1234 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}

static char * test_fetch_jal() {
    i = 0x0c1a5a58; // jal 0x696960
    p = 0x4;
    ifid = (control_t *)malloc(sizeof(control_t));
    mem_write_w(p, &i);
    fetch(ifid, &p, &cache_config);
    mu_assert(_FL "instruction j 0x1234 bad opCode", ifid->opCode == OPC_JAL);
    mu_assert(_FL "instruction j 0x1234 bad address", ifid->address == 0x1a5a58);
    mu_assert(_FL "instruction j 0x1234 bad pcNext", ifid->pcNext == 0x8);
    free(ifid);
    return 0;
}


static char * all_tests() {
    uint64_t size = 0x140;
    uint64_t offs = 0x00;
    mem_init(size,offs);

    reg_init();

    mu_run_test(test_fetch_add);
    mu_run_test(test_fetch_sub);
    mu_run_test(test_fetch_sll);
    mu_run_test(test_fetch_jr);
    mu_run_test(test_fetch_nor);
    mu_run_test(test_fetch_sltu);
    mu_run_test(test_fetch_xor);
    mu_run_test(test_fetch_addi);
    mu_run_test(test_fetch_andi);
    mu_run_test(test_fetch_beq);
    mu_run_test(test_fetch_lb);
    mu_run_test(test_fetch_lhu);
    mu_run_test(test_fetch_ori);
    mu_run_test(test_fetch_sb);
    mu_run_test(test_fetch_sw);
    mu_run_test(test_fetch_sltiu);
    mu_run_test(test_fetch_xori);
    mu_run_test(test_fetch_j);
    mu_run_test(test_fetch_jal);


    return 0;
}

int main(int argc, char **argv) {
    flags = MASK_SANITY;
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    } else {
        printf(__FILE__": ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != 0;
}
