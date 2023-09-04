#ifndef __KINGPIN_RNG_H_
#define __KINGPIN_RNG_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly."
#endif // __KINGPIN_H_

#include "basic.h"
#include "buffer.h"
#include "err.h"

    kp_status kp_library_rng_init(const void *entropy, kp_size entropy_size);

    void kp_rng_generate(u8 *buffer, kp_size buffer_size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_RNG_H_