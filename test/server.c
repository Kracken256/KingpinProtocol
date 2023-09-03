#include <kingpin.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/random.h>

kp_size rng_interface(u8 *bytes, kp_size size)
{
    return getrandom(bytes, size, 0);
}

int main(int argc, char **argv)
{
    char *LOCAL_ADDR;
    u16 LOCAL_PORT;
    char *FILE_EXFIL;
    if (argc != 4)
    {
        printf("Usage: %s <listen_addr> <listen_port> <file-exfil>\n", argv[0]);
        return 1;
    }

    LOCAL_ADDR = argv[1];
    LOCAL_PORT = atoi(argv[2]);
    FILE_EXFIL = argv[3];

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
    addr.sin_port = htons(LOCAL_PORT);
    addr.sin_addr.s_addr = inet_addr(LOCAL_ADDR);

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

        FILE *file = fopen(FILE_EXFIL, "r");

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