

#define __KINGPIN_BACKEND
#include <kp/proto.h>
#include <kp/ec.h>
#include <kp/types/proto-structs.h>
#include <kp/core.h>
#include <kp/checksum.h>
#include <kp/str.h>
#include <kp/cipher.h>

#include <kp/session.h>

static kp_session_fn gSessionFunctions;

static kp_session_keys kp_session_derive_keys(const kp_ec_keypair *keypair, const kp_ec_public_key *peer_public_key)
{
    kp_ecdh_secret shared_secret;
    kp_x25519_derive_shared_secret(&keypair->private_key, peer_public_key, &shared_secret);

    kp_session_keys keys;

    kp_buffer secret_buffer;
    kp_buffer_init(&secret_buffer);

    secret_buffer.fn->set_sensitive(&secret_buffer, TRUE);

    secret_buffer.fn->alloc(&secret_buffer, NULL, shared_secret.secret.size + 1);
    kp_memcpy(secret_buffer.data, shared_secret.secret.data, shared_secret.secret.size);

    shared_secret.secret.fn->free(&shared_secret.secret);

    u8 *shared_secret_ptr = secret_buffer.data;

    /// Derive the keys

    /// Derive ENC key
    shared_secret_ptr[KP_ECDH_SECRET_SIZE] = 'A';
    kp_sha256(shared_secret_ptr, KP_ECDH_SECRET_SIZE + 1, keys.enc);

    /// Derive IV AND MAC keys
    u8 shabuf[32];
    shared_secret_ptr[KP_ECDH_SECRET_SIZE] = 'B';
    kp_sha256(shared_secret_ptr, KP_ECDH_SECRET_SIZE + 1, shabuf);

    kp_memcpy(keys.iv, shabuf, 16);
    kp_memcpy(keys.mac, shabuf + 16, 16);

    kp_memscrub(shabuf, 32);

    secret_buffer.fn->free(&secret_buffer);

    return keys;
}

kp_status kp_fn_accept(kp_session *self, u8 flags, u32 id, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed)
{
    kp_proto_handshake_init_sp init_sp;

    kp_size bytes_read = 0;

    while (bytes_read < sizeof(init_sp))
    {
        kp_size bytes_read_now = 0;
        bytes_read_now = self->socket.read(&self->socket, (u8 *)&init_sp + bytes_read, sizeof(init_sp) - bytes_read);

        if (bytes_read_now <= 0)
            return KP_FAIL;

        bytes_read += bytes_read_now;
    }

    if (init_sp.magic != KP_PROTO_INIT_MAGIC)
        return KP_SESSION_MAGIC_MISMATCH;

    if (init_sp.version != KP_PROTO_VERSION)
        return KP_SESSION_VERSION_MISMATCH;

    if (init_sp.id != id)
        return KP_SESSION_PEER_ID_MISMATCH;

    if (peers_allowed)
    {
        kp_ec_public_key peer_public_key;
        kp_buffer_init(&peer_public_key.key);

        peer_public_key.curve = KP_EC_CURVE_X25519;
        peer_public_key.key.fn->alloc(&peer_public_key.key, init_sp.pubkey, 32);

        kp_fingerprint peer_keyid;
        kp_ec_fingerprint(&peer_public_key, &peer_keyid);

        kp_size i;
        boolean found = FALSE;
        for (i = 0; i < peers_allowed->count; i++)
        {
            if (kp_ec_fingerprint_compare(&peer_keyid, &peers_allowed->keys[i]) == TRUE)
            {
                found = TRUE;
                break;
            }
        }

        if (found == FALSE)
            return KP_SESSION_PEER_NOT_ALLOWED;
    }

    kp_proto_handshake_resp_sp resp_sp;

    resp_sp.magic = KP_PROTO_HANDSHAKE_RESP_MAGIC;
    resp_sp.id = id;
    kp_memcpy(resp_sp.pubkey, keypair->public_key.key.data, 32);

    kp_size bytes_written = 0;

    while (bytes_written < sizeof(resp_sp))
    {
        kp_size bytes_written_now = 0;
        bytes_written_now = self->socket.write(&self->socket, (u8 *)&resp_sp + bytes_written, sizeof(resp_sp) - bytes_written);

        if (bytes_written_now <= 0)
            return KP_FAIL;

        bytes_written += bytes_written_now;
    }

    kp_ec_public_key peer_public_key;

    kp_buffer_init(&peer_public_key.key);
    peer_public_key.curve = KP_EC_CURVE_X25519;

    peer_public_key.key.fn->alloc(&peer_public_key.key, init_sp.pubkey, 32);

    self->remote_public_key = peer_public_key;

    /// Derive keys
    self->keys = kp_session_derive_keys(keypair, &peer_public_key);

    kp_chacha20_init_context(&self->cipher_ctx, self->keys.enc, self->keys.iv, 0);

    self->status = KP_SESSION_ESTABLISHED;

    return KP_SUCCESS;
}

kp_status kp_fn_connect(kp_session *self, u8 flags, u32 id, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed)
{
    kp_proto_handshake_init_sp init_sp;
    init_sp.magic = KP_PROTO_INIT_MAGIC;
    init_sp.version = KP_PROTO_VERSION;
    init_sp.id = id;
    kp_memcpy(init_sp.pubkey, keypair->public_key.key.data, 32);

    kp_size bytes_written = 0;

    while (bytes_written < sizeof(init_sp))
    {
        kp_size bytes_written_now = 0;
        bytes_written_now = self->socket.write(&self->socket, (u8 *)&init_sp + bytes_written, sizeof(init_sp) - bytes_written);

        if (bytes_written_now <= 0)
            return KP_FAIL;

        bytes_written += bytes_written_now;
    }

    kp_proto_handshake_resp_sp resp_sp;

    kp_size bytes_read = 0;

    while (bytes_read < sizeof(resp_sp))
    {
        kp_size bytes_read_now = 0;
        bytes_read_now = self->socket.read(&self->socket, (u8 *)&resp_sp + bytes_read, sizeof(resp_sp) - bytes_read);

        if (bytes_read_now <= 0)
            return KP_FAIL;

        bytes_read += bytes_read_now;
    }

    if (resp_sp.magic != KP_PROTO_HANDSHAKE_RESP_MAGIC)
        return KP_SESSION_MAGIC_MISMATCH;

    if (resp_sp.id != id)
        return KP_SESSION_PEER_ID_MISMATCH;

    kp_ec_public_key peer_public_key;
    kp_buffer_init(&peer_public_key.key);

    peer_public_key.curve = KP_EC_CURVE_X25519;
    peer_public_key.key.fn->alloc(&peer_public_key.key, resp_sp.pubkey, 32);

    if (peers_allowed)
    {
        kp_fingerprint peer_keyid;
        kp_ec_fingerprint(&peer_public_key, &peer_keyid);

        kp_size i;
        boolean found = FALSE;
        for (i = 0; i < peers_allowed->count; i++)
        {
            if (kp_ec_fingerprint_compare(&peer_keyid, &peers_allowed->keys[i]) == TRUE)
            {
                found = TRUE;
                break;
            }
        }

        if (found == FALSE)
            return KP_SESSION_PEER_NOT_ALLOWED;
    }

    self->remote_public_key = peer_public_key;

    /// Derive keys
    self->keys = kp_session_derive_keys(keypair, &peer_public_key);

    kp_chacha20_init_context(&self->cipher_ctx, self->keys.enc, self->keys.iv, 0);

    self->status = KP_SESSION_ESTABLISHED;

    return KP_SUCCESS;
}

kp_status kp_fn_read(kp_session *self, void *buffer, kp_size *length)
{
    kp_proto_dat_sp dat_sp;

    static boolean first = TRUE;
    static kp_size dat_len_total = 0;
    static kp_size dat_len_read = 0;
    static u8 dat_mac_sp[16];
    static kp_sha256_ctx dat_sha256_ctx;

    if (first)
    {
        kp_size bytes_read = 0;

        while (bytes_read < sizeof(dat_sp))
        {
            kp_size bytes_read_now = 0;
            bytes_read_now = self->socket.read(&self->socket, (u8 *)&dat_sp + bytes_read, sizeof(dat_sp) - bytes_read);

            if (bytes_read_now <= 0)
                return KP_FAIL;

            bytes_read += bytes_read_now;
        }

        kp_chacha20_xor(&self->cipher_ctx, (u8 *)&dat_sp, sizeof(dat_sp));

        if (dat_sp.magic != KP_PROTO_DAT_MAGIC)
            return KP_SESSION_MAGIC_MISMATCH;

        u32 dat_crc32_temp = dat_sp.crc32;
        dat_sp.crc32 = 0;

        if (kp_crc32((u8 *)&dat_sp, sizeof(dat_sp)) != dat_crc32_temp)
            return KP_SESSION_ERROR;

        dat_len_total = (dat_sp.len[0] << 16) | (dat_sp.len[1] << 8) | dat_sp.len[2];
        kp_memcpy(dat_mac_sp, dat_sp.mac, 16);
        dat_len_read = 0;
        first = FALSE;
        kp_sha256_init(&dat_sha256_ctx);
    }

    /// Read data until buffer is full
    kp_size bytes_read = 0;

    while (bytes_read < *length)
    {
        kp_size bytes_read_now = 0;
        bytes_read_now = self->socket.read(&self->socket, (u8 *)buffer + bytes_read, *length - bytes_read);

        if (bytes_read_now < 0)
            return KP_FAIL;

        if (bytes_read_now == 0)
            break;

        bytes_read += bytes_read_now;
    }

    dat_len_read += bytes_read;

    kp_chacha20_xor(&self->cipher_ctx, (u8 *)buffer, bytes_read);

    kp_sha256_update(&dat_sha256_ctx, (u8 *)buffer, bytes_read);

    *length = bytes_read;

    if (dat_len_read >= dat_len_total)
    {
        /// check MAC
        u8 sha256_full[32];
        kp_sha256_final(&dat_sha256_ctx, sha256_full);

        if (kp_memcmp(sha256_full, dat_mac_sp, 16) != 0)
            return KP_SESSION_ERROR;

        first = TRUE;
        *length = bytes_read;
        return KP_SUCCESS;
    }

    return KP_SUCCESS;
}

kp_status kp_fn_write(kp_session *self, const void *buffer, kp_size length)
{
    kp_proto_dat_sp dat_sp;
    dat_sp.flags = 0;
    dat_sp.magic = KP_PROTO_DAT_MAGIC;
    dat_sp.crc32 = 0;
    dat_sp.len[0] = (length >> 16) & 0xFF;
    dat_sp.len[1] = (length >> 8) & 0xFF;
    dat_sp.len[2] = length & 0xFF;

    u8 sha256_full[32];
    kp_sha256(buffer, length, sha256_full);

    kp_memcpy(dat_sp.mac, sha256_full, 16);

    dat_sp.crc32 = kp_crc32((u8 *)&dat_sp, sizeof(dat_sp));

    kp_chacha20_xor(&self->cipher_ctx, (u8 *)&dat_sp, sizeof(dat_sp));

    kp_size bytes_written = 0;

    while (bytes_written < sizeof(dat_sp))
    {
        kp_size bytes_written_now = 0;
        bytes_written_now = self->socket.write(&self->socket, (u8 *)&dat_sp + bytes_written, sizeof(dat_sp) - bytes_written);

        if (bytes_written_now <= 0)
            return KP_FAIL;

        bytes_written += bytes_written_now;
    }

    void *buffer_copy = kp_alloc(length);

    kp_memcpy(buffer_copy, buffer, length);

    kp_chacha20_xor(&self->cipher_ctx, (u8 *)buffer_copy, length);

    bytes_written = 0;

    while (bytes_written < length)
    {
        kp_size bytes_written_now = 0;
        bytes_written_now = self->socket.write(&self->socket, (u8 *)buffer_copy + bytes_written, length - bytes_written);

        if (bytes_written_now <= 0)
            return KP_FAIL;

        bytes_written += bytes_written_now;
    }

    kp_free(buffer_copy);

    return KP_SUCCESS;
}

kp_status kp_fn_session_close(kp_session *self)
{
    return self->socket.close(&self->socket);
}

kp_session kp_session_wrap(kp_socket socket)
{
    kp_session session;
    session.flags = 0;
    session.status = KP_SESSION_NOT_STARTED;
    session.version = KP_PROTO_VERSION;
    session.id = 0;

    session.socket = socket;

    /// Attach the session functions
    session.fn = &gSessionFunctions;

    return session;
}

kp_status kp_library_session_init()
{
    gSessionFunctions.accept = kp_fn_accept;
    gSessionFunctions.connect = kp_fn_connect;
    gSessionFunctions.read = kp_fn_read;
    gSessionFunctions.write = kp_fn_write;
    gSessionFunctions.close = kp_fn_session_close;

    return KP_SUCCESS;
}

kp_status kp_session_accept(kp_session *session, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed)
{
    return session->fn->accept(session, 0, 0, keypair, peers_allowed);
}

kp_status kp_session_accept_ex(kp_session *session, u8 flags, u32 id, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed)
{
    return session->fn->accept(session, flags, id, keypair, peers_allowed);
}

kp_status kp_session_connect(kp_session *session, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed)
{
    return session->fn->connect(session, 0, 0, keypair, peers_allowed);
}

kp_status kp_session_connect_ex(kp_session *session, u8 flags, u32 id, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed)
{
    return session->fn->connect(session, flags, id, keypair, peers_allowed);
}

kp_status kp_session_write(kp_session *session, const u8 *buffer, kp_size length)
{
    return session->fn->write(session, buffer, length);
}

kp_status kp_session_read(kp_session *session, u8 *buffer, kp_size *length)
{
    return session->fn->read(session, buffer, length);
}

kp_status kp_session_close(kp_session *session)
{
    return session->fn->close(session);
}

void kp_session_free(kp_session *session)
{
    session->remote_public_key.key.fn->free(&session->remote_public_key.key);
}

void kp_session_key_serialize(const kp_session_keys *keys, kp_buffer *buffer)
{
    kp_buffer_init(buffer);

    const u8 key_serial_size = (32 + 16 + 16) * 2 + 1;

    buffer->fn->alloc(buffer, NULL, key_serial_size);
    buffer->fn->set_sensitive(buffer, TRUE);

    /// Hex encode the keys
    kp_bin2hex(keys->enc, 32, buffer->data);
    kp_bin2hex(keys->iv, 16, buffer->data + 64);
    kp_bin2hex(keys->mac, 16, buffer->data + 96);

    buffer->data[key_serial_size - 1] = '\0';
}

void kp_session_key_deserialize(kp_session_keys *keys, const kp_buffer *buffer)
{
    kp_hex2bin(buffer->data, 64, keys->enc);
    kp_hex2bin(buffer->data + 64, 32, keys->iv);
    kp_hex2bin(buffer->data + 96, 32, keys->mac);
}