/// Modified from: https://github.com/jonasschnelli/chacha20poly1305


#include <kp/cipher/chachapoly_aead.h>
#include <kp/cipher/poly1305.h>

#define __KINGPIN_BACKEND

#include <kp/core.h>

#if BYTE_ORDER == LITTLE_ENDIAN
#define htole32(x) (x)
#define le32toh(x) (x)

#define htole64(x) (x)
#define le64toh(x) (x)

#elif BYTE_ORDER == BIG_ENDIAN
#define htole32(x) __builtin_bswap32(x)
#define le32toh(x) __builtin_bswap32(x)

#define htole64(x) __builtin_bswap64(x)
#define le64toh(x) __builtin_bswap64(x)

#else
#error byte order not supported
#endif /* endif byteorder */

static s32 timingsafe_bcmp(const void *b1, const void *b2, kp_size n)
{
    const u8 *p1 = b1, *p2 = b2;
    s32 ret = 0;

    for (; n > 0; n--)
        ret |= *p1++ ^ *p2++;
    return (ret != 0);
}

void memory_cleanse(void *p, kp_size n)
{
    kp_memset(p, 0, n);
}

#define XOR(v, w) ((v) ^ (w))

s32 kp_chacha20poly1305_init(struct kp_chachapolyaead_ctx *ctx, const u8 *k_1, s32 k_1_len, const u8 *k_2, s32 k_2_len)
{
    if (k_1_len != KP_CHACHA20_POLY1305_AEAD_KEY_LEN || k_2_len != KP_CHACHA20_POLY1305_AEAD_KEY_LEN)
        return -1;
    kp_chacha_keysetup(&ctx->main_ctx, k_1, 256);
    kp_chacha_keysetup(&ctx->header_ctx, k_2, 256);
    ctx->cached_aad_seqnr = KP_UINT64_MAX;
    return 0;
}

s32 kp_chacha20poly1305_crypt(struct kp_chachapolyaead_ctx *ctx, u64 seqnr, u64 seqnr_aad, s32 pos_aad, u8 *dest, kp_size dest_len, const u8 *src, kp_size src_len, s32 is_encrypt)
{
    const u8 one[8] = {1, 0, 0, 0, 0, 0, 0, 0}; /* NB little-endian */
    u64 aad_chacha_nonce_hdr = 0;
    u8 expected_tag[KP_POLY1305_TAGLEN], poly_key[KP_POLY1305_KEYLEN];
    s32 r = -1;
    s32 aad_pos = 0;

    if (
        // if we encrypt, make sure the source contains at least the expected AAD and the destination has at least space for the source + MAC
        (is_encrypt && (src_len < KP_CHACHA20_POLY1305_AEAD_AAD_LEN || dest_len < src_len + KP_POLY1305_TAGLEN)) ||
        // if we decrypt, make sure the source contains at least the expected AAD+MAC and the destination has at least space for the source - MAc
        (!is_encrypt && (src_len < KP_CHACHA20_POLY1305_AEAD_AAD_LEN + KP_POLY1305_TAGLEN || dest_len < src_len - KP_POLY1305_TAGLEN)))
    {
        return r;
    }

    u64 chacha_iv = htole64(seqnr);
    kp_memset(poly_key, 0, sizeof(poly_key));
    kp_chacha_ivsetup(&ctx->main_ctx, (u8 *)&chacha_iv, NULL);
    kp_chacha_encrypt_bytes(&ctx->main_ctx, poly_key, poly_key, sizeof(poly_key));

    if (!is_encrypt)
    {
        const u8 *tag = src + src_len - KP_POLY1305_TAGLEN;

        kp_poly1305_auth(expected_tag, src, src_len - KP_POLY1305_TAGLEN, poly_key);
        if (timingsafe_bcmp(expected_tag, tag, KP_POLY1305_TAGLEN) != 0)
        {
            goto out;
        }
        /* MAC has been successfully verified, make sure we don't covert it in decryption */
        src_len -= KP_POLY1305_TAGLEN;
    }

    /* add AAD (encrypted length) */
    if (ctx->cached_aad_seqnr != seqnr_aad)
    {
        ctx->cached_aad_seqnr = seqnr_aad;
        aad_chacha_nonce_hdr = htole64(seqnr_aad);
        kp_chacha_ivsetup(&ctx->header_ctx, (u8 *)&aad_chacha_nonce_hdr, NULL); // block counter 0
        kp_chacha_encrypt_bytes(&ctx->header_ctx, NULL, ctx->aad_keystream_buffer, KP_CHACHA20_ROUND_OUTPUT);
    }
    /* crypt the AAD (3 byte length) */
    dest[0] = XOR(src[0], ctx->aad_keystream_buffer[aad_pos + 0]);
    dest[1] = XOR(src[1], ctx->aad_keystream_buffer[aad_pos + 1]);
    dest[2] = XOR(src[2], ctx->aad_keystream_buffer[aad_pos + 2]);

    /* Set Chacha's block counter to 1 and encipher */
    kp_chacha_ivsetup(&ctx->main_ctx, (u8 *)&chacha_iv, one);
    kp_chacha_encrypt_bytes(&ctx->main_ctx, src + KP_CHACHA20_POLY1305_AEAD_AAD_LEN, dest + KP_CHACHA20_POLY1305_AEAD_AAD_LEN, src_len - KP_CHACHA20_POLY1305_AEAD_AAD_LEN);

    /* If encrypting, calculate and append tag */
    if (is_encrypt)
    {
        kp_poly1305_auth(dest + src_len, dest, src_len, poly_key);
    }
    r = 0;
out:
    memory_cleanse(expected_tag, sizeof(expected_tag));
    memory_cleanse(&chacha_iv, sizeof(chacha_iv));
    memory_cleanse(poly_key, sizeof(poly_key));
    return r;
}

s32 kp_chacha20poly1305_get_length(struct kp_chachapolyaead_ctx *ctx,
                                   u32 *len_out,
                                   u64 seqnr,
                                   const u8 *ciphertext)
{
    u8 buf[3], seqbuf[8];

    s32 pos = seqnr % KP_AAD_PACKAGES_PER_ROUND * KP_CHACHA20_POLY1305_AEAD_AAD_LEN;
    seqnr = seqnr / (float)KP_AAD_PACKAGES_PER_ROUND; /* 21 x 3byte length packages fits in a ChaCha20 round */
    if (ctx->cached_aad_seqnr != seqnr)
    {
        /* we need to calculate the 64 keystream bytes since we reached a new sequence number */
        ctx->cached_aad_seqnr = seqnr;
        seqnr = htole64(seqnr);                                  // use LE for the nonce
        kp_chacha_ivsetup(&ctx->header_ctx, (u8 *)&seqnr, NULL); // block counter 0
        kp_chacha_encrypt_bytes(&ctx->header_ctx, NULL, ctx->aad_keystream_buffer, KP_CHACHA20_ROUND_OUTPUT);
    }

    /* decrypt the ciphertext length by XORing the right position of the 64byte keystream cache with the ciphertext */
    *len_out = 0;
    *len_out = XOR(ciphertext[0], ctx->aad_keystream_buffer[pos + 0]) |
               XOR(ciphertext[1], ctx->aad_keystream_buffer[pos + 1]) << 8 |
               XOR(ciphertext[2], ctx->aad_keystream_buffer[pos + 2]) << 16;

    /* convert to host endianness 32bit integer (only 24bit though) */
    *len_out = le32toh(*len_out);
    return 0;
}