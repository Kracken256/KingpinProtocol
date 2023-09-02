#define __KINGPIN_BACKEND

#include <kp/rng.h>
#include <kp/core.h>
#include <kp/checksum.h>

typedef struct _kp_rng_state
{
    u8 state[32];
    kp_size index;
} kp_rng_state;

static kp_rng_state kp_rng;

kp_status kp_library_rng_init(const void *entropy, kp_size entropy_size)
{
    kp_rng.index = 0;

    kp_sha256(entropy, entropy_size, kp_rng.state);

    return KP_SUCCESS;
}

static void kp_rng_reseed(const void *entropy, kp_size entropy_size)
{
    u8 digest[32];

    kp_get_entropy(digest, 32);

    kp_sha256(digest, 32, digest);

    if (entropy_size != 0 && entropy != NULL)
        for (kp_size i = 0; i < entropy_size; i++)
            digest[i % 32] ^= ((u8 *)entropy)[i];

    kp_sha256(digest, 32, kp_rng.state);
}

static void kp_rng_update_state()
{
    kp_sha256(kp_rng.state, 32, kp_rng.state);
    kp_rng.index += 32;

    if (kp_rng.index >= 256)
    {
        kp_rng_reseed(NULL, 0);
        kp_rng.index = 0;
    }
}

void kp_rng_generate(u8 *buffer, kp_size buffer_size)
{
    kp_size chunks = buffer_size / 32;
    u8 remainder = buffer_size % 32;

    for (kp_size i = 0; i < chunks; i++)
    {
        kp_rng_update_state();
        kp_memcpy(buffer + (i * 32), kp_rng.state, 32);
    }

    if (remainder > 0)
    {
        kp_rng_update_state();
        kp_memcpy(buffer + (chunks * 32), kp_rng.state, remainder);
    }
}