#pragma once

#include <kp/types/kp.h>

#ifdef __cplusplus 
extern "C" {
#endif

struct chacha20_context
{
	u32 keystream32[16];
	kp_size position;

	u8 key[32];
	u8 nonce[12];
	u64 counter;

	u32 state[16];
};

void kp_chacha20_init_context(struct chacha20_context *ctx, u8 key[], u8 nounc[], u64 counter);

void kp_chacha20_xor(struct chacha20_context *ctx, u8 *bytes, kp_size n_bytes);

#ifdef __cplusplus 
}
#endif 