#ifndef __KINGPIN_CORE_H_
#define __KINGPIN_CORE_H_

#ifndef __KINGPIN_BACKEND
#error "Do not include this file directly. Only backend .c files should do so."
#endif // __KINGPIN_BACKEND

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "kp.h"

    void *kp_alloc(kp_size size);

    void *kp_realloc(void *ptr, kp_size size);

    void kp_free(void *ptr);

    void kp_log(const s8 *fmt, ...);

    void *kp_memcpy(void *dest, const void *src, kp_size size);

    void *kp_memset(void *dest, u8 value, kp_size size);

    s32 kp_memcmp(const void *ptr1, const void *ptr2, kp_size size);

    void *kp_memmove(void *dest, const void *src, kp_size size);

    void kp_memscrub(void *ptr, kp_size size);

    void kp_set_dependency_config(const kp_dependency *config);

    void kp_strncpy(s8 *dest, const s8 *src, kp_size size);

    kp_size kp_get_entropy(u8 *bytes, kp_size size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_CORE_H_