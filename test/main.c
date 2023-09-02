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

int main()
{
    kp_dependency dep;
    dep.kp_free_fn = free;
    dep.kp_malloc_fn = malloc;
    dep.kp_memset_fn = memset;
    dep.kp_memcpy_fn = memcpy;
    dep.kp_memcmp_fn = memcmp;
    dep.kp_memmove_fn = memmove;
    dep.kp_realloc_fn = realloc;
    dep.kp_log_fn = printf;
    dep.kp_get_entropy_fn = rng_interface;

    srand(time(NULL));

    kp_library_init(&dep);

    kp_ec_keypair keypair_1;

    kp_x25519_generate_keypair(&keypair_1);

    print_buffer(&keypair_1.private_key.key);
    print_buffer(&keypair_1.public_key.key);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

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

    kp_session session = kp_session_wrap(kp_socket_wrap(clientfd));

    kp_status err = kp_session_accept(&session, &keypair_1, NULL);

    char errstr[20];
    kp_errstr(err, errstr, 20);

    printf("err: %s\n", errstr);

    kp_session_close(&session);

    close(clientfd);

    close(sockfd);

    kp_library_deinit(0);

    return 0;
}