#ifndef __KINGPIN_ERRSTR_H_
#define __KINGPIN_ERRSTR_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#include "kp.h"

    void kp_errstr(kp_status err, char *buf, kp_size maxlen);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_ERRSTR_H_