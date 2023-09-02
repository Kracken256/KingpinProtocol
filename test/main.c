#include <kingpin.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void print_buffer(kp_buffer *buffer)
{
    for (size_t i = 0; i < buffer->size; i++)
    {
        printf("%02x", buffer->data[i]);
    }
    printf("\n");
}

void print_buffer_raw(u8 *buffer, kp_size size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("%02x", buffer[i]);
    }
    printf("\n");
}

kp_size rng_interface(u8 *bytes, kp_size size)
{
    kp_size chunks = size / 4;
    u8 remainder = size % 4;
    u32 *ptr = (u32 *)bytes;

    for (kp_size i = 0; i < chunks; i++)
    {
        ptr[i] = rand();
    }

    if (remainder > 0)
    {
        u32 last = rand();
        u8 *last_ptr = (u8 *)&last;

        for (u8 i = 0; i < remainder; i++)
        {
            bytes[(chunks * 4) + i] = last_ptr[i];
        }
    }

    return size;
}

kp_size kp_socket_i2c_read(kp_socket *socket, void *buffer, kp_size size)
{
    /// Read data I2C into buffer
    return 0;
}

kp_size kp_socket_i2c_write(kp_socket *socket, const void *buffer, kp_size size)
{
    /// Write data I2C from buffer
    return 0;
}

s32 kp_socket_i2c_close(kp_socket *socket)
{
    /// Close I2C socket
    return 0;
}

kp_socket kp_socket_i2c_create()
{
    kp_socket socket;
    socket.read = kp_socket_i2c_read;
    socket.write = kp_socket_i2c_write;
    socket.close = kp_socket_i2c_close;
    return socket;
}

int main(int argc, char **argv)
{
    kp_dependency dep;
    dep.kp_free_fn = free;
    dep.kp_malloc_fn = malloc;
    dep.kp_realloc_fn = realloc;
    dep.kp_get_entropy_fn = rng_interface;
    dep.kp_memcmp_fn = NULL;
    dep.kp_memcpy_fn = NULL;
    dep.kp_memmove_fn = NULL;
    dep.kp_memset_fn = NULL;

    srand(time(NULL));

    kp_library_init(&dep);

    kp_ec_keypair keypair_1;

    kp_x25519_generate_keypair(&keypair_1);

    print_buffer(&keypair_1.private_key.key);
    print_buffer(&keypair_1.public_key.key);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (argc == 1)
    {

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(4444);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        int true = 1;

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) == -1)
        {
            perror("Setsockopt");
            exit(1);
        }

        if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
        {
            printf("bind failed\n");
            return 1;
        }

        if (listen(sockfd, 3) != 0)
        {
            printf("listen failed\n");
            return 1;
        }

        int clientfd = accept(sockfd, NULL, NULL);

        kp_socket my_socket = kp_socket_wrap(clientfd);

        kp_session session = kp_session_wrap(my_socket);

        kp_status err = kp_session_accept(&session, &keypair_1, NULL);

        char errstr[20];
        kp_errstr(err, errstr, 20);

        printf("err: %s\n", errstr);

        // kp_session_write(&session, "Hello, world!", 13);

        printf("enc: ");
        print_buffer_raw(session.keys.enc, 32);
        printf("iv: ");
        print_buffer_raw(session.keys.iv, 16);
        printf("mac: ");
        print_buffer_raw(session.keys.mac, 16);

        kp_session_close(&session);

        kp_session_free(&session);

        close(sockfd);
    }
    else
    {
        /// Connect to 0.0.0.0:4444
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(4444);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
        {
            printf("connect failed\n");
            return 1;
        }

        kp_socket my_socket = kp_socket_wrap(sockfd);

        kp_session session = kp_session_wrap(my_socket);

        kp_status err = kp_session_connect(&session, &keypair_1, NULL);

        char errstr[20];
        kp_errstr(err, errstr, 20);

        printf("err: %s\n", errstr);

        printf("enc: ");
        print_buffer_raw(session.keys.enc, 32);
        printf("iv: ");
        print_buffer_raw(session.keys.iv, 16);
        printf("mac: ");
        print_buffer_raw(session.keys.mac, 16);

        kp_session_close(&session);

        kp_session_free(&session);
    }

    kp_library_deinit(0);

    return 0;
}