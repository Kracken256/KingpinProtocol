#include <kp/cipher/chacha20.h>

#define __KINGPIN_BACKEND
#include <kp/core.h>

static u32 rotl32(u32 x, int n)
{
    return (x << n) | (x >> (32 - n));
}

static u32 pack4(const u8 *a)
{
    u32 res = 0;
    res |= (u32)a[0] << 0 * 8;
    res |= (u32)a[1] << 1 * 8;
    res |= (u32)a[2] << 2 * 8;
    res |= (u32)a[3] << 3 * 8;
    return res;
}

static void unpack4(u32 src, u8 *dst)
{
    dst[0] = (src >> 0 * 8) & 0xff;
    dst[1] = (src >> 1 * 8) & 0xff;
    dst[2] = (src >> 2 * 8) & 0xff;
    dst[3] = (src >> 3 * 8) & 0xff;
}

static void chacha20_init_block(struct chacha20_context *ctx, u8 key[], u8 nonce[])
{
    kp_memcpy(ctx->key, key, sizeof(ctx->key));
    kp_memcpy(ctx->nonce, nonce, sizeof(ctx->nonce));

    const u8 *magic_constant = (u8 *)"expand 32-byte k";
    ctx->state[0] = pack4(magic_constant + 0 * 4);
    ctx->state[1] = pack4(magic_constant + 1 * 4);
    ctx->state[2] = pack4(magic_constant + 2 * 4);
    ctx->state[3] = pack4(magic_constant + 3 * 4);
    ctx->state[4] = pack4(key + 0 * 4);
    ctx->state[5] = pack4(key + 1 * 4);
    ctx->state[6] = pack4(key + 2 * 4);
    ctx->state[7] = pack4(key + 3 * 4);
    ctx->state[8] = pack4(key + 4 * 4);
    ctx->state[9] = pack4(key + 5 * 4);
    ctx->state[10] = pack4(key + 6 * 4);
    ctx->state[11] = pack4(key + 7 * 4);
    // 64 bit counter initialized to zero by default.
    ctx->state[12] = 0;
    ctx->state[13] = pack4(nonce + 0 * 4);
    ctx->state[14] = pack4(nonce + 1 * 4);
    ctx->state[15] = pack4(nonce + 2 * 4);

    kp_memcpy(ctx->nonce, nonce, sizeof(ctx->nonce));
}

static void chacha20_block_set_counter(struct chacha20_context *ctx, u64 counter)
{
    ctx->state[12] = (u32)counter;
    ctx->state[13] = pack4(ctx->nonce + 0 * 4) + (u32)(counter >> 32);
}

static void chacha20_block_next(struct chacha20_context *ctx)
{
    // This is where the crazy voodoo magic happens.
    // Mix the bytes a lot and hope that nobody finds out how to undo it.
    for (int i = 0; i < 16; i++)
        ctx->keystream32[i] = ctx->state[i];

#define CHACHA20_QUARTERROUND(x, a, b, c, d) \
    x[a] += x[b];                            \
    x[d] = rotl32(x[d] ^ x[a], 16);          \
    x[c] += x[d];                            \
    x[b] = rotl32(x[b] ^ x[c], 12);          \
    x[a] += x[b];                            \
    x[d] = rotl32(x[d] ^ x[a], 8);           \
    x[c] += x[d];                            \
    x[b] = rotl32(x[b] ^ x[c], 7);

    for (int i = 0; i < 10; i++)
    {
        CHACHA20_QUARTERROUND(ctx->keystream32, 0, 4, 8, 12)
        CHACHA20_QUARTERROUND(ctx->keystream32, 1, 5, 9, 13)
        CHACHA20_QUARTERROUND(ctx->keystream32, 2, 6, 10, 14)
        CHACHA20_QUARTERROUND(ctx->keystream32, 3, 7, 11, 15)
        CHACHA20_QUARTERROUND(ctx->keystream32, 0, 5, 10, 15)
        CHACHA20_QUARTERROUND(ctx->keystream32, 1, 6, 11, 12)
        CHACHA20_QUARTERROUND(ctx->keystream32, 2, 7, 8, 13)
        CHACHA20_QUARTERROUND(ctx->keystream32, 3, 4, 9, 14)
    }

    for (int i = 0; i < 16; i++)
        ctx->keystream32[i] += ctx->state[i];

    u32 *counter = ctx->state + 12;
    // increment counter
    counter[0]++;
    if (0 == counter[0])
    {
        // wrap around occured, increment higher 32 bits of counter
        counter[1]++;
        // Limited to 2^64 blocks of 64 bytes each.
        // If you want to process more than 1180591620717411303424 bytes
        // you have other problems.
        // We could keep counting with counter[2] and counter[3] (nonce),
        // but then we risk reusing the nonce which is very bad.
    }
}

void kp_chacha20_init_context(struct chacha20_context *ctx, u8 key[], u8 nonce[], u64 counter)
{
    kp_memset(ctx, 0, sizeof(struct chacha20_context));

    chacha20_init_block(ctx, key, nonce);
    chacha20_block_set_counter(ctx, counter);

    ctx->counter = counter;
    ctx->position = 64;
}

void kp_chacha20_xor(struct chacha20_context *ctx, u8 *bytes, kp_size n_bytes)
{
    u8 *keystream8 = (u8 *)ctx->keystream32;
    for (kp_size i = 0; i < n_bytes; i++)
    {
        if (ctx->position >= 64)
        {
            chacha20_block_next(ctx);
            ctx->position = 0;
        }
        bytes[i] ^= keystream8[ctx->position];
        ctx->position++;
    }
}