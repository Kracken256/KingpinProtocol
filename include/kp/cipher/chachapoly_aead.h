#ifndef CHACHA20_POLY_AEAD_H
#define CHACHA20_POLY_AEAD_H

#include <types/basic.h>
#include <kp/cipher/chacha20.h>

#define CHACHA_KEYLEN 32 /* 2 x 256 bit keys */
#define CHACHA20_POLY1305_AEAD_KEY_LEN 32
#define CHACHA20_POLY1305_AEAD_AAD_LEN 3 /* 3 bytes length */
#define CHACHA20_ROUND_OUTPUT 64         /* 64 bytes per round */
#define AAD_PACKAGES_PER_ROUND 21        /* 64 / 3 round down*/

struct chachapolyaead_ctx {
    struct chacha_ctx main_ctx, header_ctx;
    u8 aad_keystream_buffer[CHACHA20_ROUND_OUTPUT];
    u64 cached_aad_seqnr;
};

int chacha20poly1305_init(struct chachapolyaead_ctx* cpctx, const u8* k_1, int k_1_len, const u8* k_2, int k_2_len);
int chacha20poly1305_crypt(struct chachapolyaead_ctx* ctx, u64 seqnr, u64 seqnr_aad, int pos_aad, u8* dest, kp_size dest_len, const u8* src, kp_size srv_len, int is_encrypt);
int chacha20poly1305_get_length(struct chachapolyaead_ctx* ctx,
    u32* len_out,
    u64 seqnr,
    const u8* ciphertext);
#endif /* CHACHA20_POLY_AEAD_H */