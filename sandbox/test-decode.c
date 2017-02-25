#include <stdio.h>
#include <stdlib.h>
#include "../src/decode.h"

// Compile with
//    gcc ../src/decode.o -o test-decode test-decode.c

int main(int argc, char *argv[]){
    inst_t inst = 0x2271ff9c;
    pc_t pc = 0;
    reg_id_ex_t * id_ex = malloc(sizeof(reg_id_ex_t));

    int rv = decode(inst, pc, id_ex);
    printf("rs: %d\n", id_ex->regRs);
    printf("rt: %d\n", id_ex->regRt);
    printf("rd: %d\n", id_ex->regRd);
    printf("shamt: %d\n", id_ex->shamt);
    printf("pc: %d\n", id_ex->pcNext);
    printf("immed: %d\n", id_ex->immed);
    printf("op: %d\n", id_ex->op);
    return 0;
}
