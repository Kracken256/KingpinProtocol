#ifndef __KINGPIN_TYPES_H_
#define __KINGPIN_TYPES_H_

#if !defined(__KINGPIN_KP_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include types/kp.h instead."
#endif // __KINGPIN_KP_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    /// @brief Unsigned 8-bit integer
    typedef unsigned char u8;

    /// @brief Unsigned 16-bit integer
    typedef unsigned short u16;

    /// @brief Unsigned 32-bit integer
    typedef unsigned int u32;

    /// @brief Unsigned 64-bit integer
    typedef unsigned long u64;

    /// @brief Signed 8-bit integer
    typedef signed char s8;

    /// @brief Signed 16-bit integer
    typedef signed short s16;

    /// @brief Signed 32-bit integer
    typedef signed int s32;

    /// @brief Signed 64-bit integer
    typedef signed long s64;

    /// @brief 32-bit floating point number
    typedef float f32;

    /// @brief 64-bit floating point number
    typedef double f64;

    /// @brief 8-bit boolean
    typedef u8 boolean;

    /// @brief Standard size type
    typedef u64 kp_size;

    #define KP_UINT64_MAX 0xFFFFFFFFFFFFFFFF

/// @brief Boolean true
#define TRUE ((u8)1)

/// @brief Boolean false
#define FALSE ((u8)0)

#define NULL ((void *)0)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_TYPES_H_