#define __KINGPIN_BACKEND

#include "proto.h"
#include "core.h"
#include "ec.h"
#include "proto-structs.h"
#include "checksum.h"

kp_status kp_syn_init_msg(kp_buffer *buffer, u32 id, const kp_ec_keypair *keypair)
{
    /// Key must be X25519
    if (keypair->public_key.curve != KP_EC_CURVE_X25519)
        return KP_INVALID_ARGUMENT;

    kp_ec_keypair keypair_temp;

    /// Securely copy the keypair
    kp_ec_keypair_copy(&keypair_temp, keypair);

    /// Derive the public key from the private key ed25519
    kp_ed25519_derive_public_key(&keypair_temp.private_key, &keypair_temp.public_key);

    kp_buffer_init(buffer);

    buffer->fn->alloc(buffer, NULL, sizeof(kp_proto_handshake_init_sp));
    kp_memset(buffer->data, 0, sizeof(kp_proto_handshake_init_sp));

    kp_proto_handshake_init_sp *proto = (kp_proto_handshake_init_sp *)buffer->data;

    /// Set some parameters
    proto->magic = KP_PROTO_INIT_MAGIC;
    proto->id = id;
    proto->version = KP_PROTO_VERSION;

    /// Copy the X25519 public key
    kp_memcpy(proto->pubkey, keypair->public_key.key.data, 32);

    /// Free the temporary keypair
    kp_ec_keypair_free(&keypair_temp);

    return KP_SUCCESS;
}

kp_status kp_syn_resp_msg(kp_buffer *buffer, u32 id, const kp_ec_keypair *keypair)
{
    /// Key must be X25519
    if (keypair->public_key.curve != KP_EC_CURVE_X25519)
        return KP_INVALID_ARGUMENT;

    kp_ec_keypair keypair_temp;

    /// Securely copy the keypair
    kp_ec_keypair_copy(&keypair_temp, keypair);

    /// Derive the public key from the private key ed25519
    kp_ed25519_derive_public_key(&keypair_temp.private_key, &keypair_temp.public_key);

    kp_buffer_init(buffer);

    buffer->fn->alloc(buffer, NULL, sizeof(kp_proto_handshake_resp_sp));
    kp_memset(buffer->data, 0, sizeof(kp_proto_handshake_resp_sp));

    kp_proto_handshake_resp_sp *proto = (kp_proto_handshake_resp_sp *)buffer->data;

    proto->magic = KP_PROTO_HANDSHAKE_RESP_MAGIC;
    proto->id = id;

    kp_memcpy(proto->pubkey, keypair->public_key.key.data, 32);

    /// Free the temporary keypair
    kp_ec_keypair_free(&keypair_temp);

    return KP_SUCCESS;
}

kp_status kp_syn_ack_msg(kp_buffer *buffer, u16 flags, kp_buffer *data, u32 crc32)
{
    kp_buffer_init(buffer);

    buffer->fn->alloc(buffer, NULL, sizeof(kp_proto_ack_sp));

    kp_proto_ack_sp *proto = (kp_proto_ack_sp *)buffer->data;

    proto->magic = KP_PROTO_ACK_MAGIC;
    proto->flags = flags;

    if (!data)
    {
        proto->crc32 = crc32;
    }
    else
    {
        proto->crc32 = kp_crc32(data->data, data->size);
    }

    return KP_SUCCESS;
}

kp_status kp_syn_dat_msg(kp_buffer *buffer, u16 flags, u32 len)
{
    if (len > 0x1000000)
        return KP_INVALID_ARGUMENT;

    kp_buffer_init(buffer);

    buffer->fn->alloc(buffer, NULL, sizeof(kp_proto_dat_sp));

    kp_proto_dat_sp *proto = (kp_proto_dat_sp *)buffer->data;

    proto->magic = KP_PROTO_DAT_MAGIC;
    proto->flags = flags;
    proto->len[0] = (len >> 16) & 0xFF;
    proto->len[1] = (len >> 8) & 0xFF;
    proto->len[2] = len & 0xFF;

    return KP_SUCCESS;
}

kp_status kp_syn_fin_msg(kp_buffer *buffer, u16 flags)
{
    kp_buffer_init(buffer);

    buffer->fn->alloc(buffer, NULL, sizeof(kp_proto_fin_sp));

    kp_proto_fin_sp *proto = (kp_proto_fin_sp *)buffer->data;

    proto->magic = KP_PROTO_FIN_MAGIC;
    proto->flags = flags;

    return KP_SUCCESS;
}