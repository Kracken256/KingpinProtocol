#ifndef __KINGPIN_INIT_H_
#define __KINGPIN_INIT_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#include <types/basic.h>
#include <types/struct.h>
#include <types/err.h>

    kp_status kp_library_init(const kp_dependency *dependency);

    kp_status kp_library_deinit(const kp_uninit *uninit_mode);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_H_