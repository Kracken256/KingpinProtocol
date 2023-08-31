#ifndef __KINGPIN_STRUCT_H_
#define __KINGPIN_STRUCT_H_

#ifndef __KINGPIN_KP_H_
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
        void *(*kp_malloc_fn)(std_size_t size);

        /// @brief The function to use for memory reallocation
        /// @param ptr The pointer to the memory to reallocate
        /// @param size The new size of the memory
        /// @return A pointer to the reallocated memory
        /// @warning This function is REQUIRED
        void *(*kp_realloc_fn)(void *ptr, std_size_t size);

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
        void *(*kp_memcpy_fn)(void *dest, const void *src, std_size_t size);

        /// @brief The function to use for memory setting
        /// @param dest The destination memory
        /// @param value The value to set the memory to
        /// @param size The size of the memory to set
        /// @return A pointer to the destination memory
        /// @warning This function is REQUIRED
        void *(*kp_memset_fn)(void *dest, s32 value, std_size_t size);

        /// @brief The function to use for memory comparison
        /// @param dest The destination memory
        /// @param src The source memory
        /// @param size The size of the memory to compare
        /// @return A pointer to the destination memory
        /// @warning This function is REQUIRED
        s32 (*kp_memcmp_fn)(const void *dest, const void *src, std_size_t size);

        /// @brief The function to use for memory moving
        /// @param dest The destination memory
        /// @param src The source memory
        /// @param size The size of the memory to move
        /// @return A pointer to the destination memory
        /// @warning This function is REQUIRED
        void *(*kp_memmove_fn)(void *dest, const void *src, std_size_t size);
    } kp_dependency;

    typedef struct _kp_uninit
    {

    } kp_uninit;

    /// @brief A string structure for the Kingpin library
    /// @note This structure is used to provide the Kingpin library with
    /// strings to prevent me from going insane.
    /// Long live C++ and its std::string class.
    typedef struct _kp_string
    {
        s8 *data;
        std_size_t length;

        void (*alloc)(struct _kp_string *string, const s8 *data, std_size_t length);
        void (*free)(struct _kp_string *string);
        void (*concat)(struct _kp_string *string, const s8 *data, std_size_t length);
        void (*append)(struct _kp_string *string, s8 c);
        boolean *(*equals)(struct _kp_string *string, const struct _kp_string *other);
    } kp_string;

    /// @brief A vector structure for the Kingpin library
    /// @note This structure is used to provide the Kingpin library with
    /// vectors to prevent me from going insane.
    /// @note Who needs Rust when you have C?
    typedef struct _kp_vector
    {
        u8 *data;
        std_size_t length;

        void (*alloc)(struct _kp_vector *self, const u8 *data, std_size_t length);
        void (*free)(struct _kp_vector *self);
        void (*concat)(struct _kp_vector *self, const u8 *data, std_size_t length);
        void (*append)(struct _kp_vector *self, u8 c);
        boolean *(*equals)(struct _kp_vector *self, const struct _kp_vector *other);
    } kp_vector;

    typedef kp_vector kp_buffer;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_STRUCT_H_