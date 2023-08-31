#ifndef __KINGPIN_BUFFER_H_
#define __KINGPIN_BUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#include <types/struct.h>

    void kp_buffer_init(kp_buffer *buffer);

#ifdef __KINGPIN_BACKEND

    boolean kp_lib_buffer_init();

#endif // __KINGPIN_BACKEND

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_BUFFER_H_