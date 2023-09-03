#include <kingpin.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/random.h>
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
    return getrandom(bytes, size, 0);
}

int main(int argc, char **argv)
{
    kp_dependency dep;
    kp_dependency_init(&dep);
    dep.kp_free_fn = free;
    dep.kp_malloc_fn = malloc;
    dep.kp_realloc_fn = realloc;
    dep.kp_get_entropy_fn = rng_interface;

    kp_library_init(&dep);

    kp_ec_keypair keypair_1;

    kp_x25519_generate_keypair(&keypair_1);

    printf("+==================================================+\n");
    printf("|                Kingpin Protocol                  |\n");
    printf("+==================================================+\n\n");

    printf("X25519 priv: 0x");
    print_buffer(&keypair_1.private_key.key);
    printf("X25519 pub:  0x");
    print_buffer(&keypair_1.public_key.key);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

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

    kp_session session = kp_session_wrap(kp_socket_wrap(sockfd));

    kp_status err = kp_session_connect(&session, &keypair_1, NULL);

    char errstr[20];
    kp_errstr(err, errstr, 20);

    printf("connect status: %s\n", errstr);

    printf("\nSession secrets:\n");
    printf("ChaCha20 Symmetric Key: 0x");
    print_buffer_raw(session.keys.enc, 32);
    printf("ChaCha20 Nonce: 0x");
    print_buffer_raw(session.keys.iv, 16);
    printf("Message Auth Code (MAC): 0x");
    print_buffer_raw(session.keys.mac, 16);

    printf("\n====================================================\n\n");

    kp_size total_bytes_read = 0;
    kp_size total_size = 4096;
    while (1)
    {
        char buffer[127];

        char encoded[sizeof(buffer) * 2 + 1];

        kp_size len = (total_size - total_bytes_read) > sizeof(buffer) ? sizeof(buffer) : (total_size - total_bytes_read);

        if (kp_session_read(&session, buffer, &len) != KP_SUCCESS || len == 0)
        {
            printf("read EOF\n");
            break;
        }
        total_bytes_read += len;

        printf("recv len: %lu\n", len);

        kp_bin2hex(buffer, len, encoded);

        encoded[len * 2] = '\0';

        printf("recv (hex): %s\n", encoded);
    }

    kp_session_close(&session);

    kp_session_free(&session);

    kp_library_deinit(0);

    printf("\n====================================================\n");
    printf("Total bytes read: %lu\n", total_bytes_read);
    printf("======================================================\n");

    return 0;
}