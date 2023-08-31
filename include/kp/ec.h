#ifndef __KINGPIN_EC_H_
#define __KINGPIN_EC_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#include <types/basic.h>
#include <types/struct.h>
#include <types/enum.h>
#include <types/ec-types.h>

    void kp_x25519_generate_keypair(kp_ec_keypair *keypair);

    void kp_x25519_derive_public_key(const kp_ec_private_key *private_key, kp_ec_public_key *public_key);

    void kp_x25519_derive_shared_secret(const kp_ec_private_key *private_key, const kp_ec_public_key *public_key, kp_ecdh_secret *shared_secret);

    void kp_ed25519_sign(const kp_ec_private_key *private_key, const kp_buffer *message, kp_ecdsa_signature *signature);

    boolean kp_ed25519_verify(const kp_ec_public_key *public_key, const kp_buffer *message, const kp_ecdsa_signature *signature);

    void kp_ed25519_generate_keypair(kp_ec_keypair *keypair);

    void kp_ed25519_derive_public_key(const kp_ec_private_key *private_key, kp_ec_public_key *public_key);

    void kp_ec_fingerprint(const kp_ec_public_key *public_key, kp_buffer *fingerprint);

#ifdef __KINGPIN_BACKEND
    kp_status kp_ec_init(void);

#endif // __KINGPIN_BACKEND

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_EC_H_