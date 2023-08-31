#include <kingpin.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int main() {
    kp_x25519_keypair keypair;

    kp_dependency dep;
    dep.kp_free_fn = free;
    dep.kp_malloc_fn = malloc;
    dep.kp_memset_fn = memset;
    dep.kp_memcpy_fn = memcpy;
    dep.kp_memcmp_fn = memcmp;
    dep.kp_memmove_fn = memmove;
    dep.kp_realloc_fn = realloc;
    dep.kp_log_fn = printf;

    kp_status status = kp_lib_init(&dep);

    if (status != KP_SUCCESS) {
        printf("Failed to initialize library\n");
        return 1;
    }

    printf("Library initialized\n");




    return 0;
}