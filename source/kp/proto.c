#define __KINGPIN_BACKEND

#include <kp/proto.h>

kp_status kp_init_connect_msg(kp_connect_msg *msg, u32 proclamation, u8 sess_tp, const kp_x25519_keypair *keypair)
{
}

kp_status kp_syn_connect(kp_buffer *buffer, const kp_connect_msg *msg)
{
    // buffer->alloc(buffer, NULL, 0);
}