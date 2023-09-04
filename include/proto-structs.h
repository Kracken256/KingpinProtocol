#ifndef __KINGPIN_PROTO_STRUCTS_H_
#define __KINGPIN_PROTO_STRUCTS_H_

#ifndef __KINGPIN_BACKEND
#error "Do not include this file directly. Only backend .c files should do so."
#endif // __KINGPIN_BACKEND

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "kp.h"

/// @brief The magic number for the Kingpin protocol init
#define KP_PROTO_INIT_MAGIC 0xD350

/// @brief The magic number for the Kingpin protocol handshake response
#define KP_PROTO_HANDSHAKE_RESP_MAGIC 0xD352

/// @brief The magic number for the Kingpin protocol ack
#define KP_PROTO_ACK_MAGIC 0xD351

/// @brief The magic number for the Kingpin protocol data
#define KP_PROTO_DAT_MAGIC 0xD353

/// @brief The magic number for the Kingpin protocol fin
#define KP_PROTO_FIN_MAGIC 0xD354

/// @brief The Kingpin protocol version
#define KP_PROTO_VERSION ((u16)0x01)

#define KP_PROTO_SESSION_TYPE_GP 0x00

#define KP_PROTO_FIN_FLAG_TERMINATE ((u16)0x02)

#define KP_PROTO_ACK_FLAG_SUCCESS ((u16)(1 << 0))

#define KP_PROTO_ACK_FLAG_FIN ((u16)(1 << 2))

    typedef struct _kp_proto_handshake_init
    {
        u16 magic;
        u16 version;
        u32 id;
        u8 pubkey[32];
    } __attribute((packed)) kp_proto_handshake_init_sp;

    typedef struct _kp_proto_handshake_resp
    {
        u16 magic;
        u32 id;
        u8 pubkey[32];
    } __attribute((packed)) kp_proto_handshake_resp_sp;

    typedef struct _kp_proto_dat
    {
        u16 magic;
        u8 flags;
        u8 len[3]; // 24-bit length 16 MB max
        u8 mac[16];
        u32 crc32;
    } __attribute((packed)) kp_proto_dat_sp;

    typedef struct _kp_proto_fin
    {
        u16 magic;
        u16 flags;
    } __attribute((packed)) kp_proto_fin_sp;

    typedef struct _kp_proto_ack
    {
        u16 magic;
        u16 flags;
        u32 crc32;
    } __attribute((packed)) kp_proto_ack_sp;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_PROTO_STRUCTS_H_