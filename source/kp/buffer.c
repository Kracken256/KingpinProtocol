
#define __KINGPIN_BACKEND

#include <kp/buffer.h>
#include <kp/core.h>

/// @brief List of sensitive buffers.
static kp_buffer **kp_buf_sensitive_list = NULL;

/// @brief Size of kp_buf_sensitive_list.
static kp_size kp_buf_sensitive_list_size = 0;

/// @brief List of allocated buffers.
static kp_buffer **kp_bufs_alloc = NULL;

/// @brief Size of kp_bufs_alloc.
static kp_size kp_bufs_alloc_size = 0;

/// @brief Reusable kp_buffer_methods structure.
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

    /// Add to kp_bufs_alloc
    kp_buffer **new_list = (kp_buffer **)kp_realloc(kp_bufs_alloc, sizeof(kp_buffer *) * (kp_bufs_alloc_size + 1));

    if (!new_list)
        return;

    kp_bufs_alloc = new_list;

    kp_bufs_alloc[kp_bufs_alloc_size++] = self;
}

static void kp_buffer_free(kp_buffer *self)
{
    if (!self)
        return;

    if (self->data && self->size > 0)
    {   
        /// If sensitive, zero out the buffer.
        if (self->flags & KP_BUFFER_FLAG_SENSITIVE)
            kp_memset(self->data, 0, self->size);

        kp_free(self->data);
    }

    self->data = NULL;
    self->size = 0;
    self->capacity = 0;
    self->fn = NULL;

    /// Remove from kp_bufs_alloc
    for (kp_size i = 0; i < kp_bufs_alloc_size; i++)
    {
        if (kp_bufs_alloc[i] == self)
        {
            kp_memmove(&kp_bufs_alloc[i], &kp_bufs_alloc[i + 1], sizeof(kp_buffer *) * (kp_bufs_alloc_size - i - 1));
            kp_bufs_alloc_size--;
            break;
        }
    }
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
        if (!kp_buf_sensitive_list[i])
            continue;
        if (kp_buf_sensitive_list[i]->data == buffer->data)
            return TRUE;
    }
    return FALSE;
}

static void kp_buffer_add_sensitive(kp_buffer *buffer)
{
    /// Add to kp_buf_sensitive_list
    kp_buffer **new_list = (kp_buffer **)kp_realloc(kp_buf_sensitive_list, sizeof(kp_buffer) * (kp_buf_sensitive_list_size + 1));

    if (!new_list)
        return;

    kp_buf_sensitive_list = new_list;
    kp_buf_sensitive_list[kp_buf_sensitive_list_size] = buffer;
    kp_buf_sensitive_list_size++;
}

static void kp_buffer_remove_sensitive(const kp_buffer *buffer)
{
    for (kp_size i = 0; i < kp_buf_sensitive_list_size; i++)
    {
        if (kp_buf_sensitive_list[i]->data == buffer->data)
        {
            kp_memmove(&kp_buf_sensitive_list[i], &kp_buf_sensitive_list[i + 1], sizeof(kp_buffer) * (kp_buf_sensitive_list_size - i - 1));

            /// Set the last element to NULL.
            kp_buf_sensitive_list[kp_buf_sensitive_list_size - 1] = NULL;
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
    /// @note If already initialized, free the buffer, then reinitialize it.
    if (buffer && buffer->fn && buffer->fn->alloc == kp_buffer_alloc && buffer->data)
    {
        buffer->fn->free(buffer);
    }

    buffer->fn = &reuse_kp_buffer_methods;
    buffer->data = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
    buffer->flags = 0;
}

void kp_buffer_clear_sensitive()
{
    for (kp_size i = 0; i < kp_buf_sensitive_list_size; i++)
    {
        if (!kp_buf_sensitive_list[i]->data || kp_buf_sensitive_list[i]->size == 0)
            continue;

        kp_memset(kp_buf_sensitive_list[i]->data, 0, kp_buf_sensitive_list[i]->size);
    }

    kp_buf_sensitive_list_size = 0;
}

boolean kp_library_buffer_init()
{
    reuse_kp_buffer_methods.alloc = kp_buffer_alloc;
    reuse_kp_buffer_methods.free = kp_buffer_free;
    reuse_kp_buffer_methods.concat = kp_buffer_concat;
    reuse_kp_buffer_methods.equal = kp_buffer_equal;
    reuse_kp_buffer_methods.set_sensitive = kp_buffer_set_sensitive;

    kp_buf_sensitive_list = (kp_buffer **)kp_alloc(sizeof(kp_buffer *) * 16);
    kp_bufs_alloc = (kp_buffer **)kp_alloc(sizeof(kp_buffer *) * 16);
    return TRUE;
}

void kp_library_buffer_deinit()
{
    kp_buffer_clear_sensitive();

    for (kp_size i = 0; i < kp_bufs_alloc_size; i++)
    {
        kp_bufs_alloc[i]->fn->free(kp_bufs_alloc[i]);
    }

    kp_bufs_alloc_size = 0;

    if (kp_buf_sensitive_list)
        kp_free(kp_buf_sensitive_list);
    
    if (kp_bufs_alloc)
        kp_free(kp_bufs_alloc);
    
    kp_buf_sensitive_list = NULL;
    kp_bufs_alloc = NULL;
}
