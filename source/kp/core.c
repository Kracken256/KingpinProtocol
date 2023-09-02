#define __KINGPIN_BACKEND

#include <kp/core.h>
#include <stdarg.h>

/// @brief Global configuration for Kingpin.
kp_dependency gKP_dependency_config;

inline void *kp_alloc(kp_size size)
{
    return gKP_dependency_config.kp_malloc_fn(size);
}

inline void *kp_realloc(void *ptr, kp_size size)
{
    return gKP_dependency_config.kp_realloc_fn(ptr, size);
}

inline void kp_free(void *ptr)
{
    gKP_dependency_config.kp_free_fn(ptr);
}

inline void kp_log(const s8 *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    gKP_dependency_config.kp_log_fn(fmt, args);
    va_end(args);
}

inline void *kp_memcpy(void *dest, const void *src, kp_size size)
{
    return gKP_dependency_config.kp_memcpy_fn(dest, src, size);
}

inline void *kp_memset(void *dest, u8 value, kp_size size)
{
    return gKP_dependency_config.kp_memset_fn(dest, value, size);
}

inline s32 kp_memcmp(const void *ptr1, const void *ptr2, kp_size size)
{
    return gKP_dependency_config.kp_memcmp_fn(ptr1, ptr2, size);
}

inline void *kp_memmove(void *dest, const void *src, kp_size size)
{
    return gKP_dependency_config.kp_memmove_fn(dest, src, size);
}

void kp_memscrub(void *ptr, kp_size size)
{
    u8 *ptr8 = (u8 *)ptr;

    kp_memset(ptr8, 0, size);
}

void kp_set_dependency_config(const kp_dependency *config)
{
    gKP_dependency_config = *config;
}

void kp_strncpy(s8 *dest, const s8 *src, kp_size size)
{
    kp_size i = 0;

    for (; i < size - 1 && src[i] != '\0'; i++)
    {
        dest[i] = src[i];
    }

    dest[i] = '\0';
}