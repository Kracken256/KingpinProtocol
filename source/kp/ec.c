#define __KINGPIN_BACKEND

#include <kp/ec.h>
#include <eddsa.h>
#include <kp/buffer.h>
#include <kp/rng.h>
#include <kp/sha256.h>

kp_status kp_ec_init(void)
{
    return KP_SUCCESS;
}

void kp_x25519_generate_keypair(kp_ec_keypair *keypair)
{
    kp_buffer_init(&keypair->private_key.key);
    kp_buffer_init(&keypair->public_key.key);

    keypair->curve = KP_EC_CURVE_X25519;
    keypair->private_key.curve = KP_EC_CURVE_X25519;
    keypair->public_key.curve = KP_EC_CURVE_X25519;

    keypair->private_key.key.fn->alloc(&keypair->private_key.key, NULL, 32);

    kp_rng_generate_buffer(&keypair->private_key.key, 32);

    keypair->private_key.key.fn->set_sensitive(&keypair->private_key.key, TRUE);

    kp_x25519_derive_public_key(&keypair->private_key, &keypair->public_key);
}

void kp_x25519_derive_public_key(const kp_ec_private_key *private_key, kp_ec_public_key *public_key)
{
    kp_buffer_init(&public_key->key);

    public_key->key.fn->alloc(&public_key->key, NULL, 32);
    public_key->curve = KP_EC_CURVE_X25519;

    x25519_base(public_key->key.data, private_key->key.data);
}

void kp_x25519_derive_shared_secret(const kp_ec_private_key *private_key, const kp_ec_public_key *public_key, kp_ecdh_secret *shared_secret)
{
    kp_buffer_init(&shared_secret->secret);

    shared_secret->secret.fn->alloc(&shared_secret->secret, NULL, 32);

    shared_secret->secret.fn->set_sensitive(&shared_secret->secret, TRUE);

    x25519(shared_secret->secret.data, private_key->key.data, public_key->key.data);
}

void kp_ec_fingerprint(const kp_ec_public_key *public_key, kp_buffer *fingerprint)
{
    kp_buffer_init(fingerprint);

    u8 pubkey_size;

    switch (public_key->curve)
    {
    case KP_EC_CURVE_X25519:
        pubkey_size = 32;
        break;
    case KP_EC_CURVE_ED25519:
        pubkey_size = 32;
        break;
    default:
        return;
    }

    fingerprint->fn->alloc(fingerprint, NULL, 32);

    kp_sha256_digest(public_key->key.data, pubkey_size, fingerprint->data);
}

void kp_ed25519_generate_keypair(kp_ec_keypair *keypair)
{
    kp_buffer_init(&keypair->private_key.key);
    kp_buffer_init(&keypair->public_key.key);

    keypair->curve = KP_EC_CURVE_ED25519;
    keypair->private_key.curve = KP_EC_CURVE_ED25519;
    keypair->public_key.curve = KP_EC_CURVE_ED25519;

    keypair->private_key.key.fn->alloc(&keypair->private_key.key, NULL, 32);

    kp_rng_generate_buffer(&keypair->private_key.key, 32);

    keypair->private_key.key.fn->set_sensitive(&keypair->private_key.key, TRUE);

    kp_ed25519_derive_public_key(&keypair->private_key, &keypair->public_key);
}

void kp_ed25519_derive_public_key(const kp_ec_private_key *private_key, kp_ec_public_key *public_key)
{
    kp_buffer_init(&public_key->key);

    public_key->key.fn->alloc(&public_key->key, NULL, 32);
    public_key->curve = KP_EC_CURVE_ED25519;

    ed25519_genpub(public_key->key.data, private_key->key.data);
}

void kp_ed25519_sign(const kp_ec_keypair *keypair, const kp_buffer *message, kp_ecdsa_signature *signature)
{
    // void ed25519_sign(uint8_t sig[64], const uint8_t sec[32], const uint8_t pub[32], const uint8_t *data, size_t len);
    kp_buffer_init(&signature->signature);

    signature->signature.fn->alloc(&signature->signature, NULL, 64);

    ed25519_sign(signature->signature.data, keypair->private_key.key.data, keypair->public_key.key.data, message->data, message->size);
}

boolean kp_ed25519_verify(const kp_ec_public_key *public_key, const kp_buffer *message, const kp_ecdsa_signature *signature)
{
    // int ed25519_verify(const uint8_t sig[64], const uint8_t pub[32], const uint8_t *data, size_t len);
    return ed25519_verify(signature->signature.data, public_key->key.data, message->data, message->size) == 1;
}