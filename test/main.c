#include <kingpin.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define __KINGPIN_BACKEND
#include <kp/sha256.h>

void print_buffer(kp_buffer *buffer)
{
    for (size_t i = 0; i < buffer->size; i++)
    {
        printf("%02x", buffer->data[i]);
    }
    printf("\n");
}

kp_size rng_interface(u8 *bytes, kp_size size)
{
    kp_size chunks = size / 4;
    u8 remainder = size % 4;
    u32 *ptr = (u32 *)bytes;

    for (kp_size i = 0; i < chunks; i++)
    {
        ptr[i] = rand();
    }

    if (remainder > 0)
    {
        u32 last = rand();
        u8 *last_ptr = (u8 *)&last;

        for (u8 i = 0; i < remainder; i++)
        {
            bytes[(chunks * 4) + i] = last_ptr[i];
        }
    }

    return size;
}

int main()
{
    kp_dependency dep;
    dep.kp_free_fn = free;
    dep.kp_malloc_fn = malloc;
    dep.kp_memset_fn = memset;
    dep.kp_memcpy_fn = memcpy;
    dep.kp_memcmp_fn = memcmp;
    dep.kp_memmove_fn = memmove;
    dep.kp_realloc_fn = realloc;
    dep.kp_log_fn = printf;
    dep.kp_get_entropy_fn = rng_interface;

    srand(time(NULL));

    kp_status status = kp_library_init(&dep);

    if (status != KP_SUCCESS)
    {
        printf("Failed to initialize library\n");
        return 1;
    }
    printf("Library initialized\n");

    kp_ec_keypair keypair;

    kp_ed25519_generate_keypair(&keypair);

    kp_buffer init_msg;

    kp_syn_init_msg(&init_msg, 0, 0, &keypair);

    print_buffer(&init_msg);

    keypair.private_key.key.fn->free(&keypair.private_key.key);
    keypair.public_key.key.fn->free(&keypair.public_key.key);

    kp_library_deinit(0);

    return 0;
}