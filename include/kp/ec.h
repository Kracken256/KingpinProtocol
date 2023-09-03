#ifndef __KINGPIN_EC_H_
#define __KINGPIN_EC_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#include <kp/types/basic.h>
#include <kp/types/struct.h>
#include <kp/types/err.h>
#include <kp/types/ec-types.h>

#define KP_SHA256_DIGEST_SIZE 32
#define KP_ECDH_SECRET_SIZE 32

    void kp_x25519_generate_keypair(kp_ec_keypair *keypair);

    void kp_x25519_derive_public_key(const kp_ec_private_key *private_key, kp_ec_public_key *public_key);

    void kp_x25519_derive_shared_secret(const kp_ec_private_key *private_key, const kp_ec_public_key *public_key, kp_ecdh_secret *shared_secret);

    void kp_ec_fingerprint(const kp_ec_public_key *public_key, kp_fingerprint *fingerprint);

    boolean kp_ec_fingerprint_compare(const kp_fingerprint *fingerprint1, const kp_fingerprint *fingerprint2);

    void kp_ed25519_generate_keypair(kp_ec_keypair *keypair);

    void kp_ed25519_derive_public_key(const kp_ec_private_key *private_key, kp_ec_public_key *public_key);

    void kp_ed25519_sign(const kp_ec_keypair *keypair, const kp_buffer *message, kp_ecdsa_signature *signature);

    boolean kp_ed25519_verify(const kp_ec_public_key *public_key, const kp_buffer *message, const kp_ecdsa_signature *signature);

    void kp_ec_keypair_free(kp_ec_keypair *keypair);

#ifdef __KINGPIN_BACKEND
    kp_status kp_library_ec_init(void);

    void kp_ec_keypair_copy(kp_ec_keypair *dst, const kp_ec_keypair *src);

#endif // __KINGPIN_BACKEND

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_EC_H_