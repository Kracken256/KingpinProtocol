#ifndef __KINGPIN_STRUCT_H_
#define __KINGPIN_STRUCT_H_

#if !defined(__KINGPIN_KP_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include types/kp.h instead."
#endif // __KINGPIN_KP_H_

#include <types/basic.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    /// @brief A dependency structure for the Kingpin library
    /// @note This structure is used to provide the Kingpin library with
    /// functions it needs to perform critical operations, while still
    /// allowing the user to override them with their own functions and/or
    /// take advantage of performance optimizations provided elsewhere.
    /// @warning All functions marked as REQUIRED must be set, or the library
    /// will not be initialized. If the function is not required, it can be
    /// set to NULL.
    typedef struct _kp_dependency
    {
        /// @brief The function to use for memory allocation
        /// @param size The size of the memory to allocate
        /// @return A pointer to the allocated memory
        /// @warning This function is REQUIRED
        void *(*kp_malloc_fn)(kp_size size);

        /// @brief The function to use for memory reallocation
        /// @param ptr The pointer to the memory to reallocate
        /// @param size The new size of the memory
        /// @return A pointer to the reallocated memory
        /// @warning This function is REQUIRED
        void *(*kp_realloc_fn)(void *ptr, kp_size size);

        /// @brief The function to use for memory deallocation
        /// @param ptr The pointer to the memory to deallocate
        /// @warning This function is REQUIRED
        void (*kp_free_fn)(void *ptr);

        /// @brief The function to use for logging
        /// @param message The message to log
        /// @warning This function is REQUIRED
        s32 (*kp_log_fn)(const char *fmt, ...);

        /// @brief The function to use for memory copying
        /// @param dest The destination memory
        /// @param src The source memory
        /// @param size The size of the memory to copy
        /// @return A pointer to the destination memory
        /// @warning This function is REQUIRED
        void *(*kp_memcpy_fn)(void *dest, const void *src, kp_size size);

        /// @brief The function to use for memory setting
        /// @param dest The destination memory
        /// @param value The value to set the memory to
        /// @param size The size of the memory to set
        /// @return A pointer to the destination memory
        /// @warning This function is REQUIRED
        void *(*kp_memset_fn)(void *dest, s32 value, kp_size size);

        /// @brief The function to use for memory comparison
        /// @param dest The destination memory
        /// @param src The source memory
        /// @param size The size of the memory to compare
        /// @return A pointer to the destination memory
        /// @warning This function is REQUIRED
        s32 (*kp_memcmp_fn)(const void *dest, const void *src, kp_size size);

        /// @brief The function to use for memory moving
        /// @param dest The destination memory
        /// @param src The source memory
        /// @param size The size of the memory to move
        /// @return A pointer to the destination memory
        /// @warning This function is REQUIRED
        void *(*kp_memmove_fn)(void *dest, const void *src, kp_size size);
    } kp_dependency;

    typedef struct _kp_uninit
    {

    } kp_uninit;

    /// @brief A buffer structure for the Kingpin library
    /// @note This structure is used to provide the Kingpin library with
    /// dynamic memory to prevent me from going insane.
    /// @note Who needs C++/Rust when you have C?
    typedef struct _kp_buffer
    {
        u8 *data;
        kp_size size;
        kp_size capacity;
        const void (*alloc)(struct _kp_buffer *self, const u8 *data, kp_size length);
        void (*free)(struct _kp_buffer *self);
        void (*concat)(struct _kp_buffer *self, const u8 *data, kp_size length);
        boolean (*equal)(const struct _kp_buffer *self, const struct _kp_buffer *other);
    } kp_buffer;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_STRUCT_H_