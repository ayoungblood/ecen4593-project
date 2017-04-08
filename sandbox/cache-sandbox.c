/* sandbox/cache-sandbox.c
 * For development of the caches
 * compile from root with
 * gcc src/cache.o -o sandbox/cache-sandbox sandbox/cache-sandbox.c
 */

 #include <stdio.h>
 #include "../src/cache.h"


int main(int argc, char * argv[]){
    cache_init();
    cache_destroy();
    return 0;
}
