#ifndef __KINGPIN_HMAC_H_
#define __KINGPIN_HMAC_H_

#ifndef __KINGPIN_BACKEND
#error "Do not include this file directly. Only backend .c files should do so."
#endif // __KINGPIN_BACKEND

#include <kp/types/kp.h>
#include <kp/checksum.h>

typedef struct _kp_hmac_sha256_ctx
{
    kp_sha256_ctx inner;
    kp_sha256_ctx outer;
} kp_hmac_sha256_ctx;

/// @brief Initialize an HMAC-SHA256 context
/// @param ctx A pointer to the context
/// @param key 16-byte key
void kp_hmac_sha256_init(kp_hmac_sha256_ctx *ctx, const u8 key[], kp_size keylen);

/// @brief Update an HMAC-SHA256 context with a message
/// @param ctx A pointer to the context
/// @param m Message
/// @param msgsize Message size
void kp_hmac_sha256_update(kp_hmac_sha256_ctx *ctx, const u8 *m, kp_size msgsize);

/// @brief Finalize an HMAC-SHA256 context
/// @param ctx A pointer to the context
/// @param digest 16-byte output buffer for the digest
void kp_hmac_sha256_final(kp_hmac_sha256_ctx *ctx, u8 digest[16]);

/// @brief HMAC-SHA256 non-standard implementation
/// @param key 16-byte key
/// @param m Message
/// @param msgsize Message size
/// @param output 16-byte output buffer for the digest
void kp_hmac_sha256(const u8 key[], kp_size keylen, const u8 *m, kp_size msgsize, u8 digest[16]);

#endif /* __KINGPIN_HMAC_H_ */