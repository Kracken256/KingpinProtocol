#ifndef __KINGPIN_SOCKET_H_
#define __KINGPIN_SOCKET_H_

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <types/err.h>
#include <kp/buffer.h>

#ifdef _WIN32
#include <winsock2.h>
#endif // _WIN32

    typedef struct _kp_socket kp_socket;

    /// @brief Generic cross-platform socket interface.
    typedef struct _kp_socket
    {
        kp_size (*read)(struct _kp_socket *socket, kp_buffer *buffer, kp_size size);
        kp_size (*read_raw)(struct _kp_socket *socket, void *buffer, kp_size size);
        kp_size (*write)(struct _kp_socket *socket, kp_buffer *buffer);
        kp_size (*write_raw)(struct _kp_socket *socket, const void *buffer, kp_size size);
        s32 (*close)(struct _kp_socket *socket);

#if defined(__linux__) || defined(__APPLE__)
        s32 socket_fd;
#elif defined(_WIN32)
    SOCKET socket_fd;
#else
#warning "No KINGPIN socket wrapper implementation for this platform."
    u32 filler;
#endif
    } kp_socket;

    /// Socket wrapper implementation for Linux and macOS.
#if defined(__linux__) || defined(__APPLE__)
    kp_socket kp_socket_wrap(s32 socket_fd);
#endif // __linux__

    /// Socket wrapper implementation for Windows.
#if defined(_WIN32)
    kp_socket kp_socket_wrap(SOCKET socket_fd);
#endif // _WIN32

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_SOCKET_H_