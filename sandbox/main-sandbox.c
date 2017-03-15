/* sandbox/main-sandbox.c
* testing out the whole execution pipeline
*/


// compile from root with
// gcc src/*.o -o sandbox/main-sandbox sandbox/main-sandbox.c


#include <stdio.h>
#include <stdlib.h>
#include "../src/alu.h"
#include "../src/decode.h"
#include "../src/main_memory.h"
#include "../src/memory.h"
#include "../src/fetch.h"
#include "../src/write.h"
#include "../src/registers.h"
#include "../src/types.h"
#include "../src/util.h"

int flags = 0; // Global flags register, shared across all files


control_t * ifid;
control_t * idex;
control_t * exmem;
control_t * memwb;

pc_t pc;

void execute_pipeline(){
    writeback(memwb);
    memory(exmem, memwb);
    execute(idex, exmem);
    decode(ifid, idex);
    fetch(ifid, &pc);
}

int main(int argc, char * argv[]){


    printf("Beginning sandbox simulation\n");

    //Initialize the register file
    reg_init();

    //Pipeline registers
    ifid = (control_t*)malloc(sizeof(control_t));
    idex = (control_t*)malloc(sizeof(control_t));
    exmem = (control_t*)malloc(sizeof(control_t));
    memwb = (control_t*)malloc(sizeof(control_t));
    ifid->regName = "ifid";
    idex->regName = "idex";
    exmem->regName = "exme";
    memwb->regName = "mewb";

    uint64_t size = 0x140;
    uint64_t offs = 0x00;
    mem_init(size,offs);
    //Initialize the PC to the zero address
    pc = 0x00000000;

    //Put some sample instructions into instruction memory
    word_t data = 0x20100064;            //addi $s0, $zero, 100
    mem_write_w(pc, &data);

    data = 0;
    mem_write_w(pc + 4, &data);         //nop
    mem_write_w(pc + 8, &data);         //nop


    data = 0x01308020;                   //add $s0, $t1, $s0
    mem_write_w(pc + 12, &data);

    data = 0;
    mem_write_w(pc + 16, &data);         //nop
    mem_write_w(pc + 20, &data);         //nop

    data = 0x2a0a01f4;                   //slti $t2, $s0, 500
    mem_write_w(pc + 24, &data);

    //Place some sample data in the register file
    word_t temp = 67;
    reg_write(REG_T1, &temp);

    int cycles = 0;
    for(cycles = 0; cycles < 11; cycles++){
        execute_pipeline();
    }

    printf("PC: 0x%08x\n", pc);

    print_pipeline_register(ifid);
    print_pipeline_register(idex);
    print_pipeline_register(exmem);
    print_pipeline_register(memwb);

    reg_read(REG_T2, &temp);
    printf("REG_T2: 0x%08x\n", temp);
    reg_read(REG_S0, &temp);
    printf("REG_S0: 0x%08x\n", temp);

    free(idex);
    free(ifid);
    free(memwb);
    free(exmem);

}
