#include <kingpin.h>

/*
Tested against:
`
https://gchq.github.io/CyberChef/#recipe=HMAC(%7B'option':'Hex','string':'00000000000000000000000000000000'%7D,'SHA256')Take_bytes(0,32,false)&input=SGVsbG8sIFdvcmxkIQ
`

*/

#define __KINGPIN_BACKEND
#include <kp/hmac.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>

kp_size rng_interface(u8 *bytes, kp_size size)
{
    return getrandom(bytes, size, 0);
}

int main()
{
    kp_dependency dep;
    kp_dependency_init(&dep);
    dep.kp_free_fn = free;
    dep.kp_malloc_fn = malloc;
    dep.kp_realloc_fn = realloc;
    dep.kp_get_entropy_fn = rng_interface;

    kp_library_init(&dep);

    kp_hmac_sha256_ctx ctx;

    u8 key[16];

    memset(key, 0, 16);

    kp_hmac_sha256_init(&ctx, key, 16);

    char *m = "Hello, World!";

    kp_hmac_sha256_update(&ctx, (u8 *)m, strlen(m));

    u8 digest[16];

    kp_hmac_sha256_final(&ctx, digest);

    for (int i = 0; i < 16; i++)
    {
        printf("%02x", digest[i]);
    }

    return 0;
}