/* $OpenBSD: poly1305.h,v 1.4 2014/05/02 03:27:54 djm Exp $ */

/*
 * Public Domain poly1305 from Andrew Moon
 * poly1305-donna-unrolled.c from https://github.com/floodyberry/poly1305-donna
 */

#ifndef POLY1305_H
#define POLY1305_H

#include <kp/types/kp.h>

#define KP_POLY1305_KEYLEN 32
#define KP_POLY1305_TAGLEN 16

void kp_poly1305_auth(u8 out[KP_POLY1305_TAGLEN], const u8 *m, kp_size inlen,
                   const u8 key[KP_POLY1305_KEYLEN]);

#endif /* POLY1305_H */