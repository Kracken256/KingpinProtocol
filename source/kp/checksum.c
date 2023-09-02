#define __KINGPIN_BACKEND

#include <kp/checksum.h>

/// Public domain SHA256 implementation https://github.com/983/SHA-256

static inline u32 kp_sha256_rotr(u32 x, int n)
{
    return (x >> n) | (x << (32 - n));
}

static inline u32 kp_sha256_step1(u32 e, u32 f, u32 g)
{
    return (kp_sha256_rotr(e, 6) ^ kp_sha256_rotr(e, 11) ^ kp_sha256_rotr(e, 25)) + ((e & f) ^ ((~e) & g));
}

static inline u32 kp_sha256_step2(u32 a, u32 b, u32 c)
{
    return (kp_sha256_rotr(a, 2) ^ kp_sha256_rotr(a, 13) ^ kp_sha256_rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
}

static inline void kp_sha256_update_w(u32 *w, s32 i, const u8 *buffer)
{
    s32 j;
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
            u32 s0 = (kp_sha256_rotr(a, 7) ^ kp_sha256_rotr(a, 18) ^ (a >> 3));
            u32 s1 = (kp_sha256_rotr(b, 17) ^ kp_sha256_rotr(b, 19) ^ (b >> 10));
            w[j] += w[(j + 9) & 15] + s0 + s1;
        }
    }
}

static void kp_sha256_block(kp_sha256_ctx *sha)
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

    s32 i, j;
    for (i = 0; i < 64; i += 16)
    {
        kp_sha256_update_w(w, i, sha->buffer);

        for (j = 0; j < 16; j += 4)
        {
            u32 temp;
            temp = h + kp_sha256_step1(e, f, g) + k[i + j + 0] + w[j + 0];
            h = temp + d;
            d = temp + kp_sha256_step2(a, b, c);
            temp = g + kp_sha256_step1(h, e, f) + k[i + j + 1] + w[j + 1];
            g = temp + c;
            c = temp + kp_sha256_step2(d, a, b);
            temp = f + kp_sha256_step1(g, h, e) + k[i + j + 2] + w[j + 2];
            f = temp + b;
            b = temp + kp_sha256_step2(c, d, a);
            temp = e + kp_sha256_step1(f, g, h) + k[i + j + 3] + w[j + 3];
            e = temp + a;
            a = temp + kp_sha256_step2(b, c, d);
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
        kp_sha256_block(sha);
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
    s32 i, j;
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

void kp_sha256(const void *src, kp_size n_bytes, void *dst_bytes32)
{
    kp_sha256_ctx ctx;
    kp_sha256_init(&ctx);
    kp_sha256_update(&ctx, src, n_bytes);
    kp_sha256_final(&ctx, dst_bytes32);
}

static const u32 crc32_table[256] = {
    0x00000000,
    0x77073096,
    0xee0e612c,
    0x990951ba,
    0x076dc419,
    0x706af48f,
    0xe963a535,
    0x9e6495a3,
    0x0edb8832,
    0x79dcb8a4,
    0xe0d5e91e,
    0x97d2d988,
    0x09b64c2b,
    0x7eb17cbd,
    0xe7b82d07,
    0x90bf1d91,
    0x1db71064,
    0x6ab020f2,
    0xf3b97148,
    0x84be41de,
    0x1adad47d,
    0x6ddde4eb,
    0xf4d4b551,
    0x83d385c7,
    0x136c9856,
    0x646ba8c0,
    0xfd62f97a,
    0x8a65c9ec,
    0x14015c4f,
    0x63066cd9,
    0xfa0f3d63,
    0x8d080df5,
    0x3b6e20c8,
    0x4c69105e,
    0xd56041e4,
    0xa2677172,
    0x3c03e4d1,
    0x4b04d447,
    0xd20d85fd,
    0xa50ab56b,
    0x35b5a8fa,
    0x42b2986c,
    0xdbbbc9d6,
    0xacbcf940,
    0x32d86ce3,
    0x45df5c75,
    0xdcd60dcf,
    0xabd13d59,
    0x26d930ac,
    0x51de003a,
    0xc8d75180,
    0xbfd06116,
    0x21b4f4b5,
    0x56b3c423,
    0xcfba9599,
    0xb8bda50f,
    0x2802b89e,
    0x5f058808,
    0xc60cd9b2,
    0xb10be924,
    0x2f6f7c87,
    0x58684c11,
    0xc1611dab,
    0xb6662d3d,
    0x76dc4190,
    0x01db7106,
    0x98d220bc,
    0xefd5102a,
    0x71b18589,
    0x06b6b51f,
    0x9fbfe4a5,
    0xe8b8d433,
    0x7807c9a2,
    0x0f00f934,
    0x9609a88e,
    0xe10e9818,
    0x7f6a0dbb,
    0x086d3d2d,
    0x91646c97,
    0xe6635c01,
    0x6b6b51f4,
    0x1c6c6162,
    0x856530d8,
    0xf262004e,
    0x6c0695ed,
    0x1b01a57b,
    0x8208f4c1,
    0xf50fc457,
    0x65b0d9c6,
    0x12b7e950,
    0x8bbeb8ea,
    0xfcb9887c,
    0x62dd1ddf,
    0x15da2d49,
    0x8cd37cf3,
    0xfbd44c65,
    0x4db26158,
    0x3ab551ce,
    0xa3bc0074,
    0xd4bb30e2,
    0x4adfa541,
    0x3dd895d7,
    0xa4d1c46d,
    0xd3d6f4fb,
    0x4369e96a,
    0x346ed9fc,
    0xad678846,
    0xda60b8d0,
    0x44042d73,
    0x33031de5,
    0xaa0a4c5f,
    0xdd0d7cc9,
    0x5005713c,
    0x270241aa,
    0xbe0b1010,
    0xc90c2086,
    0x5768b525,
    0x206f85b3,
    0xb966d409,
    0xce61e49f,
    0x5edef90e,
    0x29d9c998,
    0xb0d09822,
    0xc7d7a8b4,
    0x59b33d17,
    0x2eb40d81,
    0xb7bd5c3b,
    0xc0ba6cad,
    0xedb88320,
    0x9abfb3b6,
    0x03b6e20c,
    0x74b1d29a,
    0xead54739,
    0x9dd277af,
    0x04db2615,
    0x73dc1683,
    0xe3630b12,
    0x94643b84,
    0x0d6d6a3e,
    0x7a6a5aa8,
    0xe40ecf0b,
    0x9309ff9d,
    0x0a00ae27,
    0x7d079eb1,
    0xf00f9344,
    0x8708a3d2,
    0x1e01f268,
    0x6906c2fe,
    0xf762575d,
    0x806567cb,
    0x196c3671,
    0x6e6b06e7,
    0xfed41b76,
    0x89d32be0,
    0x10da7a5a,
    0x67dd4acc,
    0xf9b9df6f,
    0x8ebeeff9,
    0x17b7be43,
    0x60b08ed5,
    0xd6d6a3e8,
    0xa1d1937e,
    0x38d8c2c4,
    0x4fdff252,
    0xd1bb67f1,
    0xa6bc5767,
    0x3fb506dd,
    0x48b2364b,
    0xd80d2bda,
    0xaf0a1b4c,
    0x36034af6,
    0x41047a60,
    0xdf60efc3,
    0xa867df55,
    0x316e8eef,
    0x4669be79,
    0xcb61b38c,
    0xbc66831a,
    0x256fd2a0,
    0x5268e236,
    0xcc0c7795,
    0xbb0b4703,
    0x220216b9,
    0x5505262f,
    0xc5ba3bbe,
    0xb2bd0b28,
    0x2bb45a92,
    0x5cb36a04,
    0xc2d7ffa7,
    0xb5d0cf31,
    0x2cd99e8b,
    0x5bdeae1d,
    0x9b64c2b0,
    0xec63f226,
    0x756aa39c,
    0x026d930a,
    0x9c0906a9,
    0xeb0e363f,
    0x72076785,
    0x05005713,
    0x95bf4a82,
    0xe2b87a14,
    0x7bb12bae,
    0x0cb61b38,
    0x92d28e9b,
    0xe5d5be0d,
    0x7cdcefb7,
    0x0bdbdf21,
    0x86d3d2d4,
    0xf1d4e242,
    0x68ddb3f8,
    0x1fda836e,
    0x81be16cd,
    0xf6b9265b,
    0x6fb077e1,
    0x18b74777,
    0x88085ae6,
    0xff0f6a70,
    0x66063bca,
    0x11010b5c,
    0x8f659eff,
    0xf862ae69,
    0x616bffd3,
    0x166ccf45,
    0xa00ae278,
    0xd70dd2ee,
    0x4e048354,
    0x3903b3c2,
    0xa7672661,
    0xd06016f7,
    0x4969474d,
    0x3e6e77db,
    0xaed16a4a,
    0xd9d65adc,
    0x40df0b66,
    0x37d83bf0,
    0xa9bcae53,
    0xdebb9ec5,
    0x47b2cf7f,
    0x30b5ffe9,
    0xbdbdf21c,
    0xcabac28a,
    0x53b39330,
    0x24b4a3a6,
    0xbad03605,
    0xcdd70693,
    0x54de5729,
    0x23d967bf,
    0xb3667a2e,
    0xc4614ab8,
    0x5d681b02,
    0x2a6f2b94,
    0xb40bbe37,
    0xc30c8ea1,
    0x5a05df1b,
    0x2d02ef8d,
};

void kp_crc32_init(kp_crc32_ctx *ctx)
{
    ctx->crc = 0xffffffff;
}

void kp_crc32_update(kp_crc32_ctx *ctx, const void *data, kp_size n_bytes)
{
    const u8 *bytes = (const u8 *)data;
    kp_size i;

    for (i = 0; i < n_bytes; i++)
    {
        ctx->crc = crc32_table[(ctx->crc ^ bytes[i]) & 0xff] ^ (ctx->crc >> 8);
    }
}

void kp_crc32_final(kp_crc32_ctx *ctx, u32 *dst_crc32)
{
    *dst_crc32 = ctx->crc ^ 0xffffffff;
}

u32 kp_crc32(const void *src, kp_size n_bytes)
{
    kp_crc32_ctx ctx;
    kp_crc32_init(&ctx);
    kp_crc32_update(&ctx, src, n_bytes);
    u32 crc32;
    kp_crc32_final(&ctx, &crc32);
    return crc32;
}