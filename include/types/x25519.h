#ifndef __KINGPIN_X25519_H_
#define __KINGPIN_X25519_H_

#ifndef __KINGPIN_KP_H_
#error "Do not include this file directly. Include types/kp.h instead."
#endif // __KINGPIN_KP_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <types/basic.h>

    typedef struct _kp_x25519_public_key
    {
        u8 key[32];
    } kp_x25519_public_key;

    typedef struct _kp_x25519_private_key
    {
        u8 key[32];
    } kp_x25519_private_key;

    typedef struct _kp_x25519_keypair
    {
        kp_x25519_public_key public_key;
        kp_x25519_private_key private_key;
    } kp_x25519_keypair;

    typedef struct _kp_x25519_shared_secret
    {
        u8 secret[32];
    } kp_x25519_shared_secret;

    typedef struct _kp_ed25519_public_key
    {
        u8 key[64];
    } kp_ed25519_public_key;

    typedef struct _kp_ed25519_private_key
    {
        u8 key[32];
    } kp_ed25519_private_key;

    typedef struct _kp_ed25519_keypair
    {
        kp_ed25519_public_key public_key;
        kp_ed25519_private_key private_key;
    } kp_ed25519_keypair;

    typedef struct _kp_ed25519_signature
    {
        u8 sig[64];
    } kp_ed25519_signature;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_X25519_H_