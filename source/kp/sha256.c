#define __KINGPIN_BACKEND

#include <kp/sha256.h>

static inline u32 rotr(u32 x, int n)
{
    return (x >> n) | (x << (32 - n));
}

static inline u32 step1(u32 e, u32 f, u32 g)
{
    return (rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25)) + ((e & f) ^ ((~e) & g));
}

static inline u32 step2(u32 a, u32 b, u32 c)
{
    return (rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
}

static inline void update_w(u32 *w, int i, const u8 *buffer)
{
    int j;
    for (j = 0; j < 16; j++)
    {
        if (i < 16)
        {
            w[j] =
                ((u32)buffer[0] << 24) |
                ((u32)buffer[1] << 16) |
                ((u32)buffer[2] << 8) |
                ((u32)buffer[3]);
            buffer += 4;
        }
        else
        {
            u32 a = w[(j + 1) & 15];
            u32 b = w[(j + 14) & 15];
            u32 s0 = (rotr(a, 7) ^ rotr(a, 18) ^ (a >> 3));
            u32 s1 = (rotr(b, 17) ^ rotr(b, 19) ^ (b >> 10));
            w[j] += w[(j + 9) & 15] + s0 + s1;
        }
    }
}

static void sha256_block(kp_sha256_ctx *sha)
{
    u32 *state = sha->state;

    static const u32 k[8 * 8] = {
        0x428a2f98,
        0x71374491,
        0xb5c0fbcf,
        0xe9b5dba5,
        0x3956c25b,
        0x59f111f1,
        0x923f82a4,
        0xab1c5ed5,
        0xd807aa98,
        0x12835b01,
        0x243185be,
        0x550c7dc3,
        0x72be5d74,
        0x80deb1fe,
        0x9bdc06a7,
        0xc19bf174,
        0xe49b69c1,
        0xefbe4786,
        0x0fc19dc6,
        0x240ca1cc,
        0x2de92c6f,
        0x4a7484aa,
        0x5cb0a9dc,
        0x76f988da,
        0x983e5152,
        0xa831c66d,
        0xb00327c8,
        0xbf597fc7,
        0xc6e00bf3,
        0xd5a79147,
        0x06ca6351,
        0x14292967,
        0x27b70a85,
        0x2e1b2138,
        0x4d2c6dfc,
        0x53380d13,
        0x650a7354,
        0x766a0abb,
        0x81c2c92e,
        0x92722c85,
        0xa2bfe8a1,
        0xa81a664b,
        0xc24b8b70,
        0xc76c51a3,
        0xd192e819,
        0xd6990624,
        0xf40e3585,
        0x106aa070,
        0x19a4c116,
        0x1e376c08,
        0x2748774c,
        0x34b0bcb5,
        0x391c0cb3,
        0x4ed8aa4a,
        0x5b9cca4f,
        0x682e6ff3,
        0x748f82ee,
        0x78a5636f,
        0x84c87814,
        0x8cc70208,
        0x90befffa,
        0xa4506ceb,
        0xbef9a3f7,
        0xc67178f2,
    };

    u32 a = state[0];
    u32 b = state[1];
    u32 c = state[2];
    u32 d = state[3];
    u32 e = state[4];
    u32 f = state[5];
    u32 g = state[6];
    u32 h = state[7];

    u32 w[16];

    int i, j;
    for (i = 0; i < 64; i += 16)
    {
        update_w(w, i, sha->buffer);

        for (j = 0; j < 16; j += 4)
        {
            u32 temp;
            temp = h + step1(e, f, g) + k[i + j + 0] + w[j + 0];
            h = temp + d;
            d = temp + step2(a, b, c);
            temp = g + step1(h, e, f) + k[i + j + 1] + w[j + 1];
            g = temp + c;
            c = temp + step2(d, a, b);
            temp = f + step1(g, h, e) + k[i + j + 2] + w[j + 2];
            f = temp + b;
            b = temp + step2(c, d, a);
            temp = e + step1(f, g, h) + k[i + j + 3] + w[j + 3];
            e = temp + a;
            a = temp + step2(b, c, d);
        }
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

void kp_sha256_init(kp_sha256_ctx *ctx)
{
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
    ctx->n_bits = 0;
    ctx->buffer_counter = 0;
}

void sha256_append_byte(kp_sha256_ctx *sha, u8 byte)
{
    sha->buffer[sha->buffer_counter++] = byte;
    sha->n_bits += 8;

    if (sha->buffer_counter == 64)
    {
        sha->buffer_counter = 0;
        sha256_block(sha);
    }
}

void kp_sha256_update(kp_sha256_ctx *ctx, const void *data, kp_size n_bytes)
{
    const u8 *bytes = (const u8 *)data;
    kp_size i;

    for (i = 0; i < n_bytes; i++)
    {
        sha256_append_byte(ctx, bytes[i]);
    }
}

void kp_sha256_final(kp_sha256_ctx *ctx, void *dst_bytes32)
{
    u8 *ptr = (u8 *)dst_bytes32;
    int i, j;
    u64 n_bits = ctx->n_bits;

    sha256_append_byte(ctx, 0x80);

    while (ctx->buffer_counter != 56)
    {
        sha256_append_byte(ctx, 0);
    }

    for (i = 7; i >= 0; i--)
    {
        u8 byte = (n_bits >> 8 * i) & 0xff;
        sha256_append_byte(ctx, byte);
    }

    for (i = 0; i < 8; i++)
    {
        for (j = 3; j >= 0; j--)
        {
            *ptr++ = (ctx->state[i] >> j * 8) & 0xff;
        }
    }
}

void kp_sha256_digest(const void *src, kp_size n_bytes, void *dst_bytes32)
{
    kp_sha256_ctx ctx;
    kp_sha256_init(&ctx);
    kp_sha256_update(&ctx, src, n_bytes);
    kp_sha256_final(&ctx, dst_bytes32);
}