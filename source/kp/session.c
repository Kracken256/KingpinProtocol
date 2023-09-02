

#define __KINGPIN_BACKEND
#include <kp/proto.h>
#include <kp/ec.h>
#include <types/proto-structs.h>
#include <kp/core.h>
#include <kp/checksum.h>

#include <kp/session.h>

static kp_session_fn gSessionFunctions;

static kp_session_keys kp_session_derive_keys(const kp_ec_keypair *keypair, const kp_ec_public_key *peer_public_key)
{
    kp_ecdh_secret shared_secret;
    kp_x25519_derive_shared_secret(&keypair->private_key, peer_public_key, &shared_secret);

    kp_session_keys keys;

    shared_secret.secret.fn->alloc(&shared_secret.secret, NULL, shared_secret.secret.size + 1);

    u8 *shared_secret_ptr = shared_secret.secret.data;

    /// Derive the keys

    /// Derive RX key
    shared_secret_ptr[KP_ECDH_SECRET_SIZE] = 'A';
    kp_sha256(shared_secret_ptr, KP_ECDH_SECRET_SIZE + 1, keys.rx);

    /// Derive TX key
    shared_secret_ptr[KP_ECDH_SECRET_SIZE] = 'B';
    kp_sha256(shared_secret_ptr, KP_ECDH_SECRET_SIZE + 1, keys.tx);

    /// Derive RX IV AND TX IV
    u8 shabuf[32];
    shared_secret_ptr[KP_ECDH_SECRET_SIZE] = 'C';
    kp_sha256(shared_secret_ptr, KP_ECDH_SECRET_SIZE + 1, shabuf);

    kp_memcpy(keys.rx_iv, shabuf, 12);
    kp_memcpy(keys.tx_iv, shabuf + 12, 12);

    /// Derive RX MAC AND TX MAC
    shared_secret_ptr[KP_ECDH_SECRET_SIZE] = 'D';
    kp_sha256(shared_secret_ptr, KP_ECDH_SECRET_SIZE + 1, shabuf);

    kp_memcpy(keys.rx_mac, shabuf, 16);
    kp_memcpy(keys.tx_mac, shabuf + 16, 16);

    kp_memscrub(shabuf, 32);

    shared_secret.secret.fn->free(&shared_secret.secret);

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
        kp_fingerprint peer_keyid;
        kp_ec_fingerprint(&keypair->public_key, &peer_keyid);

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

    self->status = KP_SESSION_ESTABLISHED;

    return KP_SUCCESS;
}

kp_status kp_fn_connect(kp_session *self, u8 flags, u32 id, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed)
{
    return KP_NOT_IMPLEMENTED;
}

kp_status kp_fn_read(kp_session *self, void *buffer, kp_size *length)
{
    return KP_NOT_IMPLEMENTED;
}

kp_status kp_fn_write(kp_session *self, const void *buffer, kp_size length)
{
    return KP_NOT_IMPLEMENTED;
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