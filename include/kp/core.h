#ifndef __KINGPIN_CORE_H_
#define __KINGPIN_CORE_H_

#ifndef __KINGPIN_BACKEND
#error "Do not include this file directly. Only backend .c files should do so."
#endif // __KINGPIN_BACKEND

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <types/kp.h>

    void *kp_alloc(kp_size size);

    void *kp_realloc(void *ptr, kp_size size);

    void kp_free(void *ptr);

    void kp_log(const char *fmt, ...);

    void *kp_memcpy(void *dest, const void *src, kp_size size);

    void *kp_memset(void *dest, u8 value, kp_size size);

    s32 kp_memcmp(const void *ptr1, const void *ptr2, kp_size size);

    void *kp_memmove(void *dest, const void *src, kp_size size);

    void kp_set_dependency_config(const kp_dependency *config);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_CORE_H_