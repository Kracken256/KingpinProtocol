#define __KINGPIN_BACKEND

#include <kp/rng.h>

typedef struct _kp_rng_state
{
    u64 state[8];
    kp_size index;
} kp_rng_state;

kp_status kp_library_rng_init(const void *entropy, kp_size entropy_size)
{
    return KP_SUCCESS;
}

void kp_rng_reseed(const void *entropy, kp_size entropy_size)
{
}

void kp_rng_generate(u8 *buffer, kp_size buffer_size)
{
    static u64 rng_state = 0xdeadbeef;

    /// @todo Implement a better RNG
    for (kp_size i = 0; i < buffer_size; i++)
    {
        rng_state = rng_state * 0x5DEECE66D + 0xB;
        buffer[i] = rng_state >> 24;
    }
}

void kp_rng_generate_buffer(kp_buffer *buffer, kp_size buffer_size)
{
    kp_buffer_init(buffer);

    buffer->alloc(buffer, NULL, buffer_size);

    kp_rng_generate(buffer->data, buffer_size);
}