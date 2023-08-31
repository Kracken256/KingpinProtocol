#ifndef __KINGPIN_X25519_H_
#define __KINGPIN_X25519_H_

#if !defined(__KINGPIN_KP_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include types/kp.h instead."
#endif // __KINGPIN_KP_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <types/basic.h>
#include <kp/buffer.h>

    typedef enum _kp_ec_curve
    {
        KP_EC_CURVE_X25519,
        KP_EC_CURVE_ED25519,
    } kp_ec_curve;

    typedef struct _kp_ec_public_key
    {
        kp_ec_curve curve;
        kp_buffer key;
    } kp_ec_public_key;

    typedef struct _kp_ec_private_key
    {
        kp_ec_curve curve;
        kp_buffer key;
    } kp_ec_private_key;

    typedef struct _kp_ec_keypair
    {
        kp_ec_curve curve;
        kp_ec_public_key public_key;
        kp_ec_private_key private_key;
    } kp_ec_keypair;

    typedef struct _kp_ecdh_secret
    {
        kp_buffer secret;
    } kp_ecdh_secret;

    typedef struct _kp_ecdsa_signature
    {
        kp_buffer signature;
    } kp_ecdsa_signature;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_X25519_H_