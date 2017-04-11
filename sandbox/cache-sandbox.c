/* sandbox/cache-sandbox.c
 * For development of the caches
 * compile from root with
 * gcc src/cache.o -o sandbox/cache-sandbox sandbox/cache-sandbox.c
 */

 #include <stdio.h>
 #include "../src/cache.h"
 #include "../src/main_memory.h"

int flags = MASK_DEBUG;

int main(int argc, char * argv[]){


    mem_init(2<<14, 0);
    cache_init();

    //load sample data into main memory
    uint32_t i = 0;
    for(i = 0; i < 200; i+=4){
        mem_write_w(i, &i);
    }

    cache_status_t status;
    uint32_t data;
    for(i = 4; i < 200; i+=4){
        //cache_digest();
        while(d_cache_read_w(&i, &data) != CACHE_HIT){
            cache_digest();
            system ("/bin/stty raw"); // set terminal to raw/unbuffered
            char c = getchar();
            system ("/bin/stty sane"); // set back to sane
            if(c == 'x'){
                return 0;
            }
        }
    }

    cache_destroy();
    return 0;
}
