
#define __KINGPIN_BACKEND

#include <kp/buffer.h>
#include <kp/core.h>

static void kp_buffer_alloc(kp_buffer *self, const u8 *data, kp_size length)
{
    if (!self)
        return;

    if (self->data)
        self->free(self);

    self->data = (u8 *)kp_alloc(length);
    self->size = length;
    self->capacity = length;

    kp_memcpy(self->data, data, length);
}

static void kp_buffer_free(kp_buffer *self)
{
    if (!self)
        return;

    if (self->data)
        kp_free(self->data);

    self->data = NULL;
    self->size = 0;
    self->capacity = 0;
}

static void kp_buffer_concat(kp_buffer *self, const u8 *data, kp_size length)
{
    if (!self)
        return;

    if (!self->data)
    {
        self->alloc(self, data, length);
        return;
    }

    if (self->size + length > self->capacity)
    {
        self->data = kp_realloc(self->data, self->size + length);
        self->capacity = self->size + length;
    }

    kp_memcpy(self->data + self->size, data, length);
    self->size += length;
}

static boolean kp_buffer_equal(const kp_buffer *self, const kp_buffer *other)
{
    if (!self && !other)
        return TRUE;

    if (!self || !other)
        return FALSE;

    if (self->size != other->size)
        return FALSE;

    return kp_memcmp(self->data, other->data, self->size) == 0;
}

void kp_buffer_init(kp_buffer *buffer)
{
    buffer->data = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
    buffer->alloc = kp_buffer_alloc;
    buffer->free = kp_buffer_free;
    buffer->concat = kp_buffer_concat;
    buffer->equal = kp_buffer_equal;
}

boolean kp_lib_buffer_init()
{
    return TRUE;
}