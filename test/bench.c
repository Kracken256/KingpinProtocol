#include <kingpin.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/random.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdarg.h>

// #define REMOTE_ADDR "3.101.24.113"
#define REMOTE_PORT 4444
#define REMOTE_ADDR "0.0.0.0"

void kp_bin2hex(const void *bin, kp_size bin_len, s8 *hex);

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

void move_cursor(int y)
{
    printf("\033[%dA", y);
}

void print_line_padded(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buffer[100];

    vsprintf(buffer, fmt, args);

    printf("| %s", buffer);

    for (size_t i = 0; i < 33 - strlen(buffer); i++)
    {
        printf(" ");
    }

    printf("|\n");

    va_end(args);
}

void print_metrics(size_t requests, size_t errors, double success_rate, double bitrate, size_t total_read, size_t time_elapsed)
{
    printf("+======| Kingpin Benchmarks |======+\n");
    print_line_padded("Total requests: %lu", requests);

    print_line_padded("Errors: %lu", errors);

    print_line_padded("Success rate: %.2f%%", success_rate);

    if (bitrate > 1024 * 1024)
    {
        print_line_padded("Bitrate: %.2f MiB/s", bitrate / 1024 / 1024);
    }
    else if (bitrate > 1024)
    {
        print_line_padded("Bitrate: %.2f KiB/s", bitrate / 1024);
    }
    else
    {
        print_line_padded("Bitrate: %.2f B/s", bitrate);
    }

    print_line_padded("Total bytes read: %lu", total_read);

    print_line_padded("Time elapsed: %lu", time_elapsed);

    printf("+==================================+\n");

    move_cursor(8);

    fflush(stdout);
}

int main(int argc, char **argv)
{
    kp_dependency dep;
    kp_dependency_init(&dep);
    dep.kp_free_fn = free;
    dep.kp_malloc_fn = malloc;
    dep.kp_realloc_fn = realloc;
    dep.kp_get_entropy_fn = rng_interface;

    size_t total_requests = 0;
    size_t total_success = 0;
    kp_size total_bytes_read = 0;

    kp_library_init(&dep);

    printf("+=======================================================+\n");
    printf("|                Kingpin Protocol Tests                 |\n");
    printf("+=======================================================+\n\n");

    time_t time_start;

    time_start = time(NULL);

    /// Clear console
    if (system("clear") != 0)
    {
        printf("Failed to clear console\n");
        return 1;
    }

    while (1)
    {

        kp_ec_keypair keypair_1;

        kp_x25519_generate_keypair(&keypair_1);

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);

        /// Connect to 0.0.0.0:4444
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(REMOTE_PORT);
        addr.sin_addr.s_addr = inet_addr(REMOTE_ADDR);

        if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
        {
            printf("connect failed\n");
            return 1;
        }

        kp_session session = kp_session_wrap(kp_socket_wrap(sockfd));

        kp_status err = kp_session_connect(&session, &keypair_1, NULL);

        char errstr[20];
        kp_errstr(err, errstr, 20);

        while (1)
        {
            u8 buffer[100];

            char encoded[sizeof(buffer) * 2 + 1];

            kp_size len = sizeof(buffer);

            err = kp_session_read(&session, buffer, &len);
            total_bytes_read += len;

            if (err != KP_SUCCESS)
            {
                total_requests++;
            }

            if (err != KP_SUCCESS && err != KP_SESSION_MSG_FINISH)
            {
                kp_errstr(err, errstr, 30);
                break;
            }

            kp_bin2hex(buffer, len, encoded);

            encoded[len * 2] = '\0';

            if (err == KP_SESSION_MSG_FINISH)
            {
                total_success++;
                break;
            }
        }

        kp_session_close(&session);

        kp_session_free(&session);

        double percent = (double)total_success / (double)total_requests * 100.0;

        /// Calculate bitrate
        double delta = (double)(time(NULL) - time_start);
        double bitrate = (double)total_bytes_read / delta;

        print_metrics(total_requests, total_requests - total_success, percent, bitrate, total_bytes_read, time(NULL) - time_start);
    }

    kp_library_deinit(0);

    return 0;
}