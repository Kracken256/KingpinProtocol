#ifndef __KINGPIN_PROTO_H_
#define __KINGPIN_PROTO_H_

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <types/kp.h>
#include <types/x25519.h>

    typedef struct _kp_connect_msg
    {
        u32 proclamation;
        u8 sess_tp;
        kp_x25519_public_key pubkey;
        kp_ed25519_signature sig;
    } kp_connect_msg;

    /// @brief Initialize a Kingpin protocol connect message structure
    /// @param msg The message structure to initialize
    /// @param proclamation The proclamation to use
    /// @param sess_tp The session type to use
    /// @param pubkey The keypair to use
    /// @return KP_SUCCESS on success, an error code otherwise
    kp_status kp_init_connect_msg(kp_connect_msg *msg, u32 proclamation, u8 sess_tp, const kp_x25519_keypair *keypair);

    /// @brief Synthesize a Kingpin protocol connect message, for transmission by user
    /// @param buffer The buffer structure to use for the message
    /// @param msg The connect message to synthesize
    /// @return KP_SUCCESS on success, an error code otherwise
    kp_status kp_syn_connect(kp_buffer *buffer, const kp_connect_msg *msg);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_PROTO_H_