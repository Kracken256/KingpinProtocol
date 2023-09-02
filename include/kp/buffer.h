#ifndef __KINGPIN_BUFFER_H_
#define __KINGPIN_BUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#include <kp/types/basic.h>

#define KP_BUFFER_FLAG_SENSITIVE 0x01

    typedef struct _kp_buffer_methods kp_buffer_methods;

    /// @brief A buffer structure for the Kingpin library
    /// @note This structure is used to provide the Kingpin library with
    /// dynamic memory to prevent me from going insane.
    /// @note Who needs C++/Rust when you have C?
    typedef struct _kp_buffer
    {
        u8 *data;
        kp_size size;
        kp_size capacity;
        u8 flags;
        const kp_buffer_methods *fn;
    } kp_buffer;

    typedef struct _kp_buffer_methods
    {
        void (*alloc)(struct _kp_buffer *self, const u8 *data, kp_size length);
        void (*free)(struct _kp_buffer *self);
        void (*concat)(struct _kp_buffer *self, const u8 *data, kp_size length);
        boolean (*equal)(const struct _kp_buffer *self, const struct _kp_buffer *other);
        void (*set_sensitive)(struct _kp_buffer *self, boolean sensitive);
    } kp_buffer_methods;

    void kp_buffer_init(kp_buffer *buffer);

    void kp_buffer_clear_sensitive();

#ifdef __KINGPIN_BACKEND

    boolean kp_library_buffer_init();
    void kp_library_buffer_deinit();

#endif // __KINGPIN_BACKEND

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_BUFFER_H_