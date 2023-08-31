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

    kp_sha256_digest(entropy, entropy_size, kp_rng.state);

    return KP_SUCCESS;
}

void kp_rng_reseed(const void *entropy, kp_size entropy_size)
{
    u8 digest[32];

    kp_sha256_digest(entropy, entropy_size, digest);

    for (u8 i = 0; i < 32; i++)
        kp_rng.state[i] ^= digest[i];

    kp_sha256_digest(kp_rng.state, 32, kp_rng.state);

    kp_rng.index = 0;
}

void kp_rng_update_state()
{
    kp_sha256_digest(kp_rng.state, 32, kp_rng.state);
    kp_rng.index += 32;

    if (kp_rng.index >= 256)
    {
        kp_rng.index = 0;
        /// TODO: reseed new entropy
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

void kp_rng_generate_buffer(kp_buffer *buffer, kp_size buffer_size)
{
    kp_buffer_init(buffer);

    buffer->fn->alloc(buffer, NULL, buffer_size);

    kp_rng_generate(buffer->data, buffer_size);
}