
#define __KINGPIN_BACKEND

#include <kp/buffer.h>
#include <kp/core.h>

static kp_buffer *kp_buf_sensitive_list = NULL;
static kp_size kp_buf_sensitive_list_size = 0;

static kp_buffer_methods reuse_kp_buffer_methods;

static void kp_buffer_alloc(kp_buffer *self, const u8 *data, kp_size length)
{
    if (!self)
        return;

    if (self->data)
        self->fn->free(self);

    self->data = (u8 *)kp_alloc(length);
    self->size = length;
    self->capacity = length;

    if (data)
        kp_memcpy(self->data, data, length);
}

static void kp_buffer_free(kp_buffer *self)
{
    if (!self)
        return;

    if (self->data)
    {
        if (self->flags & KP_BUFFER_FLAG_SENSITIVE)
            kp_memset(self->data, 0, self->size);

        kp_free(self->data);
    }

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
        self->fn->alloc(self, data, length);
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

static boolean kp_buffer_is_sensitive(const kp_buffer *buffer)
{
    for (kp_size i = 0; i < kp_buf_sensitive_list_size; i++)
    {
        if (kp_buf_sensitive_list[i].data == buffer->data)
            return TRUE;
    }
    return FALSE;
}

static void kp_buffer_add_sensitive(const kp_buffer *buffer)
{
    kp_buffer *new_list = (kp_buffer *)kp_realloc(kp_buf_sensitive_list, sizeof(kp_buffer) * (kp_buf_sensitive_list_size + 1));

    if (!new_list)
        return;

    kp_buf_sensitive_list = new_list;
    kp_buf_sensitive_list[kp_buf_sensitive_list_size] = *buffer;
    kp_buf_sensitive_list_size++;
}

static void kp_buffer_remove_sensitive(const kp_buffer *buffer)
{
    for (kp_size i = 0; i < kp_buf_sensitive_list_size; i++)
    {
        if (kp_buf_sensitive_list[i].data == buffer->data)
        {
            kp_memmove(&kp_buf_sensitive_list[i], &kp_buf_sensitive_list[i + 1], sizeof(kp_buffer) * (kp_buf_sensitive_list_size - i - 1));
            kp_buf_sensitive_list_size--;
            break;
        }
    }
}

static void kp_buffer_set_sensitive(kp_buffer *self, boolean sensitive)
{
    if (!self)
        return;

    if (sensitive)
    {
        self->flags |= KP_BUFFER_FLAG_SENSITIVE;
        if (!kp_buffer_is_sensitive(self))
            kp_buffer_add_sensitive(self);
    }
    else
    {
        self->flags &= ~KP_BUFFER_FLAG_SENSITIVE;
        if (kp_buffer_is_sensitive(self))
            kp_buffer_remove_sensitive(self);
    }
}

void kp_buffer_init(kp_buffer *buffer)
{
    buffer->fn = &reuse_kp_buffer_methods;

    /// @note If already initialized, free the buffer, then reinitialize it.
    if (buffer->fn->alloc == kp_buffer_alloc)
        buffer->fn->free(buffer);

    buffer->data = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
    buffer->flags = 0;
}

void kp_buffer_clear_sensitive()
{
    for (kp_size i = 0; i < kp_buf_sensitive_list_size; i++)
    {
        if (!kp_buf_sensitive_list[i].data || kp_buf_sensitive_list[i].size == 0)
            continue;

        kp_memset(kp_buf_sensitive_list[i].data, 0, kp_buf_sensitive_list[i].size);
    }
}

boolean kp_library_buffer_init()
{
    reuse_kp_buffer_methods.alloc = kp_buffer_alloc;
    reuse_kp_buffer_methods.free = kp_buffer_free;
    reuse_kp_buffer_methods.concat = kp_buffer_concat;
    reuse_kp_buffer_methods.equal = kp_buffer_equal;
    reuse_kp_buffer_methods.set_sensitive = kp_buffer_set_sensitive;

    kp_buf_sensitive_list = (kp_buffer *)kp_alloc(sizeof(kp_buffer) * 16);
    return TRUE;
}

void kp_library_buffer_deinit()
{
    kp_free(kp_buf_sensitive_list);
}
