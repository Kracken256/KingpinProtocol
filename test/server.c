#include <kingpin.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

int main(int argc, char **argv)
{
    kp_dependency dep;
    kp_dependency_init(&dep);
    dep.kp_free_fn = free;
    dep.kp_malloc_fn = malloc;
    dep.kp_realloc_fn = realloc;
    dep.kp_get_entropy_fn = rng_interface;

    srand(time(NULL));

    kp_library_init(&dep);

    kp_ec_keypair keypair_1;

    kp_x25519_generate_keypair(&keypair_1);

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

    while (1)
    {
        int clientfd = accept(sockfd, NULL, NULL);

        kp_socket my_socket = kp_socket_wrap(clientfd);

        kp_session session = kp_session_wrap(my_socket);

        kp_status err = kp_session_accept(&session, &keypair_1, NULL);

        if (err != KP_SUCCESS)
        {
            close(clientfd);
            kp_session_free(&session);
            continue;
        }

        FILE *file = fopen("/etc/passwd", "r");

        if (file == NULL)
        {
            printf("fopen failed\n");
            return 1;
        }

        size_t file_size = 0;

        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        void *file_buffer = malloc(file_size);

        if (file_buffer == NULL)
        {
            printf("malloc failed\n");
            return 1;
        }

        if (fread(file_buffer, 1, file_size, file) != file_size)
        {
            fclose(file);
            free(file_buffer);
            kp_session_close(&session);

            kp_session_free(&session);

            printf("fread failed\n");
            return 1;
        }

        fclose(file);

        kp_session_write(&session, file_buffer, file_size);

        free(file_buffer);

        kp_session_close(&session);

        kp_session_free(&session);
    }

    /// Close server socket
    close(sockfd);

    kp_library_deinit(0);

    return 0;
}