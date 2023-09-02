#define __KINGPIN_BACKEND

#include <kp/core.h>
#include <stdarg.h>

/// @brief Global configuration for Kingpin.
kp_dependency gKP_dependency_config;

void *kp_memcpy_basic(void *dest, const void *src, kp_size size)
{
    u8 *dest8 = (u8 *)dest;
    const u8 *src8 = (const u8 *)src;

    for (kp_size i = 0; i < size; i++)
    {
        dest8[i] = src8[i];
    }

    return dest;
}

void *kp_memset_basic(void *dest, s32 value, kp_size size)
{
    u8 *dest8 = (u8 *)dest;

    for (kp_size i = 0; i < size; i++)
    {
        dest8[i] = value;
    }

    return dest;
}

s32 kp_memcmp_basic(const void *ptr1, const void *ptr2, kp_size size)
{
    const u8 *ptr1_8 = (const u8 *)ptr1;
    const u8 *ptr2_8 = (const u8 *)ptr2;

    for (kp_size i = 0; i < size; i++)
    {
        if (ptr1_8[i] != ptr2_8[i])
            return ptr1_8[i] - ptr2_8[i];
    }

    return 0;
}

void *kp_memmove_basic(void *dest, const void *src, kp_size size)
{
    u8 *dest8 = (u8 *)dest;
    const u8 *src8 = (const u8 *)src;

    if (dest8 < src8)
    {
        for (kp_size i = 0; i < size; i++)
        {
            dest8[i] = src8[i];
        }
    }
    else
    {
        for (kp_size i = size; i > 0; i--)
        {
            dest8[i - 1] = src8[i - 1];
        }
    }

    return dest;
}

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

inline kp_size kp_get_entropy(u8 *bytes, kp_size size)
{
    return gKP_dependency_config.kp_get_entropy_fn(bytes, size);
}

void kp_memscrub(void *ptr, kp_size size)
{
    u8 *ptr8 = (u8 *)ptr;

    kp_memset(ptr8, 0, size);
}

void kp_set_dependency_config(const kp_dependency *config)
{
    gKP_dependency_config = *config;

    if (gKP_dependency_config.kp_memcpy_fn == NULL)
        gKP_dependency_config.kp_memcpy_fn = &kp_memcpy_basic;

    if (gKP_dependency_config.kp_memset_fn == NULL)
        gKP_dependency_config.kp_memset_fn = &kp_memset_basic;

    if (gKP_dependency_config.kp_memcmp_fn == NULL)
        gKP_dependency_config.kp_memcmp_fn = &kp_memcmp_basic;

    if (gKP_dependency_config.kp_memmove_fn == NULL)
        gKP_dependency_config.kp_memmove_fn = &kp_memmove_basic;
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