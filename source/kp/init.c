#include <types/kp.h>

#define __KINGPIN_BACKEND_H_

#include <kp/core.h>

static boolean kp_dep_check(kp_dependency *d);

kp_status kp_lib_init(kp_dependency *dependency)
{
    if (dependency == NULL)
        return KP_INVALID_ARGUMENT;

    if (!kp_dep_check(dependency))
        return KP_DEPENDENCY_MISSING;

    gKP_dependency_config = *dependency;

    return KP_SUCCESS;
}

/// @brief Check if the dependency structure is valid.
/// @param d The dependency structure to check.
/// @return TRUE if the dependency structure is valid, FALSE otherwise.
static boolean kp_dep_check(kp_dependency *d)
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

kp_status kp_lib_deinit(kp_uninit *uninit_mode)
{
    if (uninit_mode == NULL)
        return KP_SUCCESS;

    return KP_SUCCESS;
}