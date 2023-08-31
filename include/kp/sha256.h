#ifndef __KINGPIN_SHA256_H_
#define __KINGPIN_SHA256_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly."
#endif // !defined(__KINGPIN_BACKEND)

#include <types/basic.h>

    typedef struct _kp_sha256_ctx
    {
        u32 state[8];
        u8 buffer[64];
        u64 n_bits;
        u8 buffer_counter;
    } kp_sha256_ctx;

    /* Functions to compute streaming SHA-256 checksums. */
    void kp_sha256_init(kp_sha256_ctx *ctx);
    void kp_sha256_update(kp_sha256_ctx *ctx, const void *data, kp_size n_bytes);
    void kp_sha256_final(kp_sha256_ctx *ctx, void *dst_bytes32);

    void kp_sha256_digest(const void *src, kp_size n_bytes, void *dst_bytes32);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_SHA256_H_