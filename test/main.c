#include <kingpin.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

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

    kp_library_init(&dep);

    kp_ec_keypair keypair_1;
    kp_ec_keypair keypair_2;

    kp_x25519_generate_keypair(&keypair_1);
    kp_x25519_generate_keypair(&keypair_2);

    kp_buffer init_msg;
    kp_buffer resp_msg;

    kp_syn_init_msg(&init_msg, 0xdeadbeef, 0x01, &keypair_1);

    kp_syn_resp_msg(&resp_msg, &keypair_2);

    print_buffer(&init_msg);
    print_buffer(&resp_msg);
    

    kp_ecdh_secret secret_1;
    kp_ecdh_secret secret_2;

    kp_x25519_derive_shared_secret(&keypair_1.private_key, &keypair_2.public_key, &secret_1);

    kp_x25519_derive_shared_secret(&keypair_2.private_key, &keypair_1.public_key, &secret_2);

    print_buffer(&secret_1.secret);
    print_buffer(&secret_2.secret);

    kp_library_deinit(0);

    return 0;
}