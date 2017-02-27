#include <stdio.h>
#include <stdint.h>
#include <limits.h>
// #include <stdlib.h>

// Compile and run with
//    gcc -o sign-confusion sign-confusion.c && ./sign-confusion

#define HIBIT(x) (((uint32_t)x)>>31)

int main(int argc, char *argv[]){
    int32_t i32 = INT_MIN;
    do {
        if (i32 < 0) {
            if (HIBIT(i32) != 1) printf("ERROR\n");
        } else if (i32 >= 0) {
            if (HIBIT(i32) != 0) printf("ERROR\n");
        }
    } while (i32++ < INT_MAX);
    // int32_t i32 = 0x7fffffff;
    // uint32_t u32 = -1;
    // printf("int32_t - s32: 0x%08x/0d%d; u32: 0x%08x/0d%u; s32.31: %d\n",i32,i32,i32,i32,(((uint32_t)i32)>>31));
    // printf("uint32_t - s32: 0x%08x/0d%d; u32: 0x%08x/0d%u\n",u32,u32,u32,u32);
}
