/* $OpenBSD: chacha.h,v 1.4 2016/08/27 04:04:56 guenther Exp $ */

/*
chacha-merged.c version 20080118
D. J. Bernstein
Public domain.
*/

#ifndef CHACHA_H
#define CHACHA_H

#include <kp/types/kp.h>

struct kp_chacha_ctx
{
    u32 input[16];
};

#define KP_CHACHA_MINKEYLEN 16
#define KP_CHACHA_NONCELEN 8
#define KP_CHACHA_CTRLEN 8
#define KP_CHACHA_STATELEN (KP_CHACHA_NONCELEN + KP_CHACHA_CTRLEN)
#define KP_CHACHA_BLOCKLEN 64

void kp_chacha_keysetup(struct kp_chacha_ctx *x, const u8 *k, u32 kbits);
void kp_chacha_ivsetup(struct kp_chacha_ctx *x, const u8 *iv, const u8 *ctr);
void kp_chacha_encrypt_bytes(struct kp_chacha_ctx *x, const u8 *m, u8 *c,
                          u32 bytes);

#endif /* CHACHA_H */