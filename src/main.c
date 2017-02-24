/* main.c
 * Top-level simulator source
 */

#include <stdio.h>

#include "types.h"
#include "decode.h"

int main(int argc, char *argv[]) {

    inst_t inst = 0x01000000;
    pc_t pc = 0x40000000;
    reg_id_ex_t * id_ex;

    int rv = decode(inst, pc, id_ex);
    print("%d", rv);

    printf("Hello World!\n");
    return 0;
}
