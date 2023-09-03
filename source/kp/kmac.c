#define __KINGPIN_BACKEND

#include <kp/hmac.h>
#include <kp/core.h>

void kp_hmac_sha256_init(kp_hmac_sha256_ctx *ctx, const u8 key[], kp_size keylen)
{
    // The inner and outer pad constants
    const u8 ipad = 0x36;
    const u8 opad = 0x5c;

    // Ensure the key length is no more than the block size (64 bytes for SHA-256)
    if (keylen > 64)
    {
        return; // Handle this error appropriately
    }

    // Create inner and outer keys
    u8 ikey[64], okey[64];
    kp_memset(ikey, ipad, 64);
    kp_memset(okey, opad, 64);

    for (kp_size i = 0; i < keylen; ++i)
    {
        ikey[i] ^= key[i];
        okey[i] ^= key[i];
    }

    kp_sha256_init(&ctx->inner);
    kp_sha256_init(&ctx->outer);

    kp_sha256_update(&ctx->inner, ikey, 64);
    kp_sha256_update(&ctx->outer, okey, 64);
}

void kp_hmac_sha256_update(kp_hmac_sha256_ctx *ctx, const u8 *m, kp_size msgsize)
{
    kp_sha256_update(&ctx->inner, m, msgsize);
}

void kp_hmac_sha256_final(kp_hmac_sha256_ctx *ctx, u8 digest[16])
{
    u8 inner_digest[32];
    u8 partial_digest[32];
    kp_sha256_final(&ctx->inner, inner_digest);

    kp_sha256_update(&ctx->outer, inner_digest, 32);
    kp_sha256_final(&ctx->outer, partial_digest);

    kp_memcpy(digest, partial_digest, 16);
}

void kp_hmac_sha256(const u8 key[], kp_size keylen, const u8 *m, kp_size msgsize, u8 digest[16])
{
    kp_hmac_sha256_ctx ctx;
    kp_hmac_sha256_init(&ctx, key, keylen);
    kp_hmac_sha256_update(&ctx, m, msgsize);
    kp_hmac_sha256_final(&ctx, digest);
}
