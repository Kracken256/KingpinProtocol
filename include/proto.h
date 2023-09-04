#ifndef __KINGPIN_PROTO_H_
#define __KINGPIN_PROTO_H_

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "err.h"
#include "buffer.h"
#include "ec-types.h"

    /// @brief Initialize a Kingpin protocol connect message buffer
    /// @param msg The buffer to store the message in
    /// @param proclamation The proclamation to use
    /// @param sess_tp The session type to use
    /// @param keypair The X25519 keypair to use
    /// @return KP_SUCCESS on success, an error code otherwise
    kp_status kp_syn_init_msg(kp_buffer *buffer, u32 id, const kp_ec_keypair *keypair);

    /// @brief Initialize a Kingpin protocol response message buffer
    /// @param msg The buffer to store the message in
    /// @param keypair The X25519 keypair to use
    /// @return KP_SUCCESS on success, an error code otherwise
    kp_status kp_syn_resp_msg(kp_buffer *buffer, u32 id, const kp_ec_keypair *keypair);

    /// @brief Initialize a Kingpin protocol ack message buffer
    /// @param msg The buffer to store the message in
    /// @param flags The flags to use
    /// @param data Optional data buffer to compute the CRC32 of
    /// @param crc32 Required if data is NULL, otherwise ignored
    /// @return KP_SUCCESS on success, an error code otherwise
    kp_status kp_syn_ack_msg(kp_buffer *buffer, u16 flags, kp_buffer *data, u32 crc32);

    /// @brief Initialize a Kingpin protocol data message buffer
    /// @param msg The buffer to store the message in
    /// @param flags The flags to use
    /// @param len The data to send 24 bits field
    kp_status kp_syn_dat_msg(kp_buffer *buffer, u16 flags, u32 len);

    /// @brief Initialize a Kingpin protocol fin message buffer
    /// @param msg The buffer to store the message in
    /// @param flags The flags to use
    /// @return KP_SUCCESS on success, an error code otherwise
    kp_status kp_syn_fin_msg(kp_buffer *buffer, u16 flags);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_PROTO_H_