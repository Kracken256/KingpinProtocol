#include <types/kp.h>

#define __KINGPIN_BACKEND

#include <kingpin.h>
#include <kp/core.h>
#include <kp/buffer.h>

static boolean kp_dep_check(const kp_dependency *d);

kp_status kp_library_init(const kp_dependency *dependency)
{
    if (dependency == NULL)
        return KP_INVALID_ARGUMENT;

    if (!kp_dep_check(dependency))
        return KP_DEPENDENCY_MISSING;

    kp_set_dependency_config(dependency);

    if (!kp_lib_buffer_init())
        return KP_INTERNAL_ERROR;

    return KP_SUCCESS;
}

/// @brief Check if the dependency structure is valid.
/// @param d The dependency structure to check.
/// @return TRUE if the dependency structure is valid, FALSE otherwise.
static boolean kp_dep_check(const kp_dependency *d)
{
    boolean status = TRUE;

    if (d->kp_free_fn == NULL)
    {
        status = FALSE;
        goto exit;
    }

    if (d->kp_malloc_fn == NULL)
    {
        status = FALSE;
        goto exit;
    }

    if (d->kp_realloc_fn == NULL)
    {
        status = FALSE;
        goto exit;
    }

    if (d->kp_memcmp_fn == NULL)
    {
        status = FALSE;
        goto exit;
    }

    if (d->kp_memcpy_fn == NULL)
    {
        status = FALSE;
        goto exit;
    }

    if (d->kp_memset_fn == NULL)
    {
        status = FALSE;
        goto exit;
    }

    if (d->kp_memmove_fn == NULL)
    {
        status = FALSE;
        goto exit;
    }

    if (d->kp_log_fn == NULL)
    {
        status = FALSE;
        goto exit;
    }

exit:
    return status;
}

kp_status kp_library_deinit(const kp_uninit *uninit_mode)
{
    if (uninit_mode == NULL)
        return KP_SUCCESS;

    return KP_SUCCESS;
}