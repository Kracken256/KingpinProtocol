#define __KINGPIN_BACKEND

#include <kp/socket.h>

#if defined(__linux__) || defined(__APPLE__)

#include <unistd.h>

static s32 kp_socket_close(kp_socket *socket)
{
    return close(socket->socket_fd);
}

static kp_size kp_socket_read(kp_socket *socket, kp_buffer *buffer, kp_size size)
{
    kp_buffer_init(buffer);
    buffer->fn->alloc(buffer, NULL, size);

    kp_size bytes_read = read(socket->socket_fd, buffer->data, size);

    if (bytes_read <= 0)
    {
        buffer->fn->free(buffer);
        return bytes_read;
    }

    /// Reallocation / truncation
    if (bytes_read != size)
        buffer->fn->alloc(buffer, NULL, bytes_read);

    return bytes_read;
}

static kp_size kp_socket_read_raw(kp_socket *socket, void *buffer, kp_size size)
{
    return read(socket->socket_fd, buffer, size);
}

static kp_size kp_socket_write(kp_socket *socket, kp_buffer *buffer)
{
    return write(socket->socket_fd, buffer->data, buffer->size);
}

static kp_size kp_socket_write_raw(kp_socket *socket, const void *buffer, kp_size size)
{
    return write(socket->socket_fd, buffer, size);
}

kp_socket kp_socket_wrap(s32 socket_fd)
{
    kp_socket new_socket;
    new_socket.read = kp_socket_read;
    new_socket.read_raw = kp_socket_read_raw;
    new_socket.write = kp_socket_write;
    new_socket.write_raw = kp_socket_write_raw;
    new_socket.close = kp_socket_close;

    /// Set socket_fd
    new_socket.socket_fd = socket_fd;

    return new_socket;
}

#endif // __linux__

#if defined(_WIN32)

#include <winsock2.h>

static s32 kp_socket_close(kp_socket *socket)
{
    return close(socket->socket_fd);
}

static kp_size kp_socket_read(kp_socket *socket, kp_buffer *buffer, kp_size size)
{
    kp_buffer_init(buffer);
    buffer->fn->alloc(buffer, NULL, size);

    kp_size bytes_read = recv(socket->socket_fd, buffer->data, size, 0);

    if (bytes_read <= 0)
    {
        buffer->fn->free(buffer);
        return bytes_read;
    }

    /// Reallocation / truncation
    if (bytes_read != size)
        buffer->fn->alloc(buffer, NULL, bytes_read);

    return bytes_read;
}

static kp_size kp_socket_read_raw(kp_socket *socket, void *buffer, kp_size size)
{
    return recv(socket->socket_fd, buffer, size, 0);
}

static kp_size kp_socket_write(kp_socket *socket, kp_buffer *buffer)
{
    return send(socket->socket_fd, buffer->data, buffer->size, 0);
}

static kp_size kp_socket_write_raw(kp_socket *socket, const void *buffer, kp_size size)
{
    return send(socket->socket_fd, buffer, size, 0);
}

kp_socket kp_socket_wrap(SOCKET socket_fd)
{
    kp_socket new_socket;
    new_socket.read = kp_socket_read;
    new_socket.read_raw = kp_socket_read_raw;
    new_socket.write = kp_socket_write;
    new_socket.write_raw = kp_socket_write_raw;
    new_socket.close = kp_socket_close;

    /// Set socket_fd
    new_socket.socket_fd = socket_fd;

    return new_socket;
}

#endif // _WIN32