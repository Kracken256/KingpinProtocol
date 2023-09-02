/*
 * Public Domain poly1305 from Andrew Moon
 * poly1305-donna-unrolled.c from https://github.com/floodyberry/poly1305-donna
 */

/* $OpenBSD: poly1305.c,v 1.3 2013/12/19 22:57:13 djm Exp $ */
/// Modified from: https://github.com/jonasschnelli/chacha20poly1305


#include <kp/cipher/poly1305.h>

#define mul32x32_64(a, b) ((u64)(a) * (b))

#define U8TO32_LE(p)                                    \
    (((u32)((p)[0])) | ((u32)((p)[1]) << 8) | \
     ((u32)((p)[2]) << 16) | ((u32)((p)[3]) << 24))

#define U32TO8_LE(p, v)                \
    do                                 \
    {                                  \
        (p)[0] = (u8)((v));       \
        (p)[1] = (u8)((v) >> 8);  \
        (p)[2] = (u8)((v) >> 16); \
        (p)[3] = (u8)((v) >> 24); \
    } while (0)

void kp_poly1305_auth(u8 out[KP_POLY1305_TAGLEN], const u8 *m,
                   kp_size inlen, const u8 key[KP_POLY1305_KEYLEN])
{
    u32 t0, t1, t2, t3;
    u32 h0, h1, h2, h3, h4;
    u32 r0, r1, r2, r3, r4;
    u32 s1, s2, s3, s4;
    u32 b, nb;
    kp_size j;
    u64 t[5];
    u64 f0, f1, f2, f3;
    u32 g0, g1, g2, g3, g4;
    u64 c;
    u8 mp[16];

    /* clamp key */
    t0 = U8TO32_LE(key + 0);
    t1 = U8TO32_LE(key + 4);
    t2 = U8TO32_LE(key + 8);
    t3 = U8TO32_LE(key + 12);

    /* precompute multipliers */
    r0 = t0 & 0x3ffffff;
    t0 >>= 26;
    t0 |= t1 << 6;
    r1 = t0 & 0x3ffff03;
    t1 >>= 20;
    t1 |= t2 << 12;
    r2 = t1 & 0x3ffc0ff;
    t2 >>= 14;
    t2 |= t3 << 18;
    r3 = t2 & 0x3f03fff;
    t3 >>= 8;
    r4 = t3 & 0x00fffff;

    s1 = r1 * 5;
    s2 = r2 * 5;
    s3 = r3 * 5;
    s4 = r4 * 5;

    /* init state */
    h0 = 0;
    h1 = 0;
    h2 = 0;
    h3 = 0;
    h4 = 0;

    /* full blocks */
    if (inlen < 16)
        goto poly1305_donna_atmost15bytes;
poly1305_donna_16bytes:
    m += 16;
    inlen -= 16;

    t0 = U8TO32_LE(m - 16);
    t1 = U8TO32_LE(m - 12);
    t2 = U8TO32_LE(m - 8);
    t3 = U8TO32_LE(m - 4);

    h0 += t0 & 0x3ffffff;
    h1 += ((((u64)t1 << 32) | t0) >> 26) & 0x3ffffff;
    h2 += ((((u64)t2 << 32) | t1) >> 20) & 0x3ffffff;
    h3 += ((((u64)t3 << 32) | t2) >> 14) & 0x3ffffff;
    h4 += (t3 >> 8) | (1 << 24);

poly1305_donna_mul:
    t[0] = mul32x32_64(h0, r0) + mul32x32_64(h1, s4) + mul32x32_64(h2, s3) +
           mul32x32_64(h3, s2) + mul32x32_64(h4, s1);
    t[1] = mul32x32_64(h0, r1) + mul32x32_64(h1, r0) + mul32x32_64(h2, s4) +
           mul32x32_64(h3, s3) + mul32x32_64(h4, s2);
    t[2] = mul32x32_64(h0, r2) + mul32x32_64(h1, r1) + mul32x32_64(h2, r0) +
           mul32x32_64(h3, s4) + mul32x32_64(h4, s3);
    t[3] = mul32x32_64(h0, r3) + mul32x32_64(h1, r2) + mul32x32_64(h2, r1) +
           mul32x32_64(h3, r0) + mul32x32_64(h4, s4);
    t[4] = mul32x32_64(h0, r4) + mul32x32_64(h1, r3) + mul32x32_64(h2, r2) +
           mul32x32_64(h3, r1) + mul32x32_64(h4, r0);

    h0 = (u32)t[0] & 0x3ffffff;
    c = (t[0] >> 26);
    t[1] += c;
    h1 = (u32)t[1] & 0x3ffffff;
    b = (u32)(t[1] >> 26);
    t[2] += b;
    h2 = (u32)t[2] & 0x3ffffff;
    b = (u32)(t[2] >> 26);
    t[3] += b;
    h3 = (u32)t[3] & 0x3ffffff;
    b = (u32)(t[3] >> 26);
    t[4] += b;
    h4 = (u32)t[4] & 0x3ffffff;
    b = (u32)(t[4] >> 26);
    h0 += b * 5;

    if (inlen >= 16)
        goto poly1305_donna_16bytes;

/* final bytes */
poly1305_donna_atmost15bytes:
    if (!inlen)
        goto poly1305_donna_finish;

    for (j = 0; j < inlen; j++)
        mp[j] = m[j];
    mp[j++] = 1;
    for (; j < 16; j++)
        mp[j] = 0;
    inlen = 0;

    t0 = U8TO32_LE(mp + 0);
    t1 = U8TO32_LE(mp + 4);
    t2 = U8TO32_LE(mp + 8);
    t3 = U8TO32_LE(mp + 12);

    h0 += t0 & 0x3ffffff;
    h1 += ((((u64)t1 << 32) | t0) >> 26) & 0x3ffffff;
    h2 += ((((u64)t2 << 32) | t1) >> 20) & 0x3ffffff;
    h3 += ((((u64)t3 << 32) | t2) >> 14) & 0x3ffffff;
    h4 += (t3 >> 8);

    goto poly1305_donna_mul;

poly1305_donna_finish:
    b = h0 >> 26;
    h0 = h0 & 0x3ffffff;
    h1 += b;
    b = h1 >> 26;
    h1 = h1 & 0x3ffffff;
    h2 += b;
    b = h2 >> 26;
    h2 = h2 & 0x3ffffff;
    h3 += b;
    b = h3 >> 26;
    h3 = h3 & 0x3ffffff;
    h4 += b;
    b = h4 >> 26;
    h4 = h4 & 0x3ffffff;
    h0 += b * 5;
    b = h0 >> 26;
    h0 = h0 & 0x3ffffff;
    h1 += b;

    g0 = h0 + 5;
    b = g0 >> 26;
    g0 &= 0x3ffffff;
    g1 = h1 + b;
    b = g1 >> 26;
    g1 &= 0x3ffffff;
    g2 = h2 + b;
    b = g2 >> 26;
    g2 &= 0x3ffffff;
    g3 = h3 + b;
    b = g3 >> 26;
    g3 &= 0x3ffffff;
    g4 = h4 + b - (1 << 26);

    b = (g4 >> 31) - 1;
    nb = ~b;
    h0 = (h0 & nb) | (g0 & b);
    h1 = (h1 & nb) | (g1 & b);
    h2 = (h2 & nb) | (g2 & b);
    h3 = (h3 & nb) | (g3 & b);
    h4 = (h4 & nb) | (g4 & b);

    f0 = ((h0) | (h1 << 26)) + (u64)U8TO32_LE(&key[16]);
    f1 = ((h1 >> 6) | (h2 << 20)) + (u64)U8TO32_LE(&key[20]);
    f2 = ((h2 >> 12) | (h3 << 14)) + (u64)U8TO32_LE(&key[24]);
    f3 = ((h3 >> 18) | (h4 << 8)) + (u64)U8TO32_LE(&key[28]);

    U32TO8_LE(&out[0], f0);
    f1 += (f0 >> 32);
    U32TO8_LE(&out[4], f1);
    f2 += (f1 >> 32);
    U32TO8_LE(&out[8], f2);
    f3 += (f2 >> 32);
    U32TO8_LE(&out[12], f3);
}