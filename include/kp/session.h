#ifndef __KINGPIN_SESSION_H_
#define __KINGPIN_SESSION_H_

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <kp/types/err.h>
#include <kp/buffer.h>
#include <kp/types/ec-types.h>
#include <kp/socket.h>
#include <kp/cipher/chacha20.h>

    typedef struct _kp_session_keys
    {
        u8 enc[32];
        u8 iv[16];
        u8 mac[16];
    } kp_session_keys;

    typedef enum _kp_session_status
    {
        KP_SESSION_NOT_STARTED = 0,
        KP_SESSION_STARTED = 100,
        KP_SESSION_ESTABLISHED = 200,
        KP_SESSION_ENDED = 300
    } kp_session_status;

    typedef struct _kp_peers_allowed
    {
        kp_size count;
        kp_fingerprint *keys;
    } kp_peers_allowed;

    typedef struct _kp_session kp_session;

    typedef struct _kp_session_fn
    {
        /// @brief Accept connection on socket.
        kp_status (*accept)(kp_session *self, u8 flags, u32 id, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed);

        /// @brief Establish connection on socket (already TCP connected)
        kp_status (*connect)(kp_session *self, u8 flags, u32 id, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed);

        /// @brief Read data from the session into buffer.
        kp_status (*read)(kp_session *self, void *buffer, kp_size *length);

        /// @brief Write data from buffer into the session.
        kp_status (*write)(kp_session *self, const void *buffer, kp_size length);

        /// @brief Clears the sensitive data from the session and frees the memory.
        kp_status (*close)(kp_session *self);
    } kp_session_fn;

    typedef struct _kp_session
    {
        u8 flags;
        u32 id;
        u16 version;
        kp_ec_public_key remote_public_key;
        kp_session_keys keys;
        kp_session_status status;
        kp_socket socket;

        struct chacha20_context cipher_ctx;

        const kp_session_fn *fn;
    } kp_session;

    kp_session kp_session_wrap(kp_socket socket);

    kp_status kp_session_accept(kp_session *session, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed);

    kp_status kp_session_accept_ex(kp_session *session, u8 flags, u32 id, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed);

    kp_status kp_session_connect(kp_session *session, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed);

    kp_status kp_session_connect_ex(kp_session *session, u8 flags, u32 id, const kp_ec_keypair *keypair, const kp_peers_allowed *peers_allowed);

    kp_status kp_session_write(kp_session *session, const u8 *buffer, kp_size length);

    kp_status kp_session_read(kp_session *session, u8 *buffer, kp_size *length);

    kp_status kp_session_close(kp_session *session);

    void kp_session_free(kp_session *session);

    void kp_session_key_serialize(const kp_session_keys *keys, kp_buffer *buffer);

    void kp_session_key_deserialize(kp_session_keys *keys, const kp_buffer *buffer);

#ifdef __KINGPIN_BACKEND
    kp_status kp_library_session_init();
#endif // __KINGPIN_BACKEND

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_SESSION_H_