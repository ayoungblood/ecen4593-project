#include <stdio.h>
#include <stdint.h>
#include <limits.h>
// #include <stdlib.h>

// Compile and run with
//    gcc -o sign-confusion sign-confusion.c && ./sign-confusion

#define HIBIT(x) (((uint32_t)x)>>31)

int main(int argc, char *argv[]){
    int i,j;
    int8_t a8, b8;
    int16_t a16, b16;
    for (i = CHAR_MIN; i <= CHAR_MAX; ++i) {
        for (j = CHAR_MIN; j <= CHAR_MAX; ++j) {
            a16 = (int16_t) i;
            a8 = (int8_t) i;
            b16 = (int16_t) j;
            b8 = (int8_t) j;
            if (((a8 - b8) & 0xff) != ((a16 - b16) & 0xff)) {
                printf("OVERFLOW: i=%d,j=%d\n",i,j);
            }
        }
    }
    return 0;
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
