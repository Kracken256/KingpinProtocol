#define __KINGPIN_BACKEND

#include <kp/rng.h>
#include <kp/core.h>
#include <kp/cipher/chacha20.h>
#include <kp/checksum.h>

#define KP_RNG_RESEED_INTERVAL 100
#define KP_RNG_BUFFER_SIZE 4096

typedef struct _kp_rng_state
{
    struct chacha20_context state_ctx;
    u8 buffer[KP_RNG_BUFFER_SIZE];
    kp_size index;
    u8 reseed_counter;
} kp_rng_state;

static kp_rng_state kp_rng;

static void kp_rng_reseed(const void *entropy, kp_size entropy_size)
{
    u8 chacha20_key[32];
    u8 entropy256[32];
    u64 chacha20_iv = 0;

    kp_get_entropy(entropy256, 32);

    /// Take advantage of ASLR
    *(u64 *)&entropy256[0] ^= *(u64 *)&kp_rng_reseed + *(u64 *)entropy;

    kp_sha256(entropy256, 32, entropy256);

    /// If no entropy is provided, then proceed with junk in buffer
    if (entropy_size > 0 && entropy != NULL)
        kp_sha256(entropy, entropy_size, chacha20_key);

    for (u8 i = 0; i < 32; i++)
        chacha20_key[i] ^= entropy256[i];

    // kp_chacha_keysetup(&kp_rng.state_ctx, chacha20_key, 256);
    // kp_chacha_ivsetup(&kp_rng.state_ctx, (u8 *)&chacha20_iv, NULL);

    kp_chacha20_init_context(&kp_rng.state_ctx, chacha20_key, (u8 *)&chacha20_iv, 0);

    kp_memset(kp_rng.buffer, 0, sizeof(kp_rng.buffer));

    // kp_chacha_encrypt_bytes(&kp_rng.state_ctx, kp_rng.buffer, kp_rng.buffer, sizeof(kp_rng.buffer));

    kp_chacha20_xor(&kp_rng.state_ctx, kp_rng.buffer, sizeof(kp_rng.buffer));

    kp_rng.index = 0;
    kp_rng.reseed_counter = 0;
}

static void kp_rng_update_buffer()
{
    if (kp_rng.reseed_counter > KP_RNG_RESEED_INTERVAL)
    {
        kp_rng_reseed(NULL, 0);
    }
    else if (kp_rng.index >= sizeof(kp_rng.buffer))
    {
        // Discard some entropy for security

        if (sizeof(kp_rng.buffer) > 256)
            kp_memset(kp_rng.buffer, 0, 128);

        // kp_chacha_encrypt_bytes(&kp_rng.state_ctx, kp_rng.buffer, kp_rng.buffer, sizeof(kp_rng.buffer));
        kp_chacha20_xor(&kp_rng.state_ctx, kp_rng.buffer, sizeof(kp_rng.buffer));
        kp_rng.index = 0;
        kp_rng.reseed_counter++;
    }
}

void kp_rng_generate(u8 *buffer, kp_size buffer_size)
{
    kp_size chunks = buffer_size / sizeof(kp_rng.buffer);
    u32 remainder = buffer_size % sizeof(kp_rng.buffer);

    for (kp_size i = 0; i < chunks; i++)
    {
        kp_memcpy(buffer + (i * sizeof(kp_rng.buffer)), kp_rng.buffer, sizeof(kp_rng.buffer));

        kp_rng.index += sizeof(kp_rng.buffer);
        kp_rng_update_buffer();
    }

    if (remainder > 0)
    {
        if (kp_rng.index + remainder > sizeof(kp_rng.buffer))
        {
            kp_rng_update_buffer();

            kp_memcpy(buffer + (chunks * sizeof(kp_rng.buffer)), kp_rng.buffer, remainder);
            kp_rng.index += remainder;
        }
        else
        {
            kp_memcpy(buffer + (chunks * sizeof(kp_rng.buffer)), kp_rng.buffer + kp_rng.index, remainder);
            kp_rng.index += remainder;

            kp_rng_update_buffer();
        }
    }
}

kp_status kp_library_rng_init(const void *entropy, kp_size entropy_size)
{
    kp_rng_reseed(entropy, entropy_size);

    return KP_SUCCESS;
}
