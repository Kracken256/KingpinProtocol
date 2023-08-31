#include <kingpin.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define __KINGPIN_BACKEND
#include <kp/sha256.h>

void print_buffer(kp_buffer *buffer)
{
    for (size_t i = 0; i < buffer->size; i++)
    {
        printf("%02x", buffer->data[i]);
    }
    printf("\n");
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

    kp_status status = kp_library_init(&dep);

    if (status != KP_SUCCESS)
    {
        printf("Failed to initialize library\n");
        return 1;
    }
    printf("Library initialized\n");

    kp_buffer buffer;

    char *message = "Hello, world!";

    char digest[32];

    kp_sha256_digest(message, strlen(message), digest);

    kp_buffer_init(&buffer);

    buffer.alloc(&buffer, digest, 32);

    printf("SHA256 digest: ");

    print_buffer(&buffer);    

    return 0;
}