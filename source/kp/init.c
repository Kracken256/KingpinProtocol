#include "kp.h"

#define __KINGPIN_BACKEND

#include "kingpin.h"
#include "core.h"
#include "buffer.h"
#include "ec.h"
#include "rng.h"
#include "str.h"

static boolean kp_dep_check(const kp_dependency *d);

kp_status kp_library_init(const kp_dependency *dependency)
{
    if (dependency == NULL)
        return KP_INVALID_ARGUMENT;

    if (!kp_dep_check(dependency))
        return KP_DEPENDENCY_MISSING;

    kp_set_dependency_config(dependency);

    if (!kp_library_buffer_init())
        return KP_INTERNAL_ERROR;

    u8 entropy[32];
    kp_size entropy_size = dependency->kp_get_entropy_fn(entropy, 32);

    if (entropy_size != 32)
        return KP_INTERNAL_ERROR;

    if (!kp_library_rng_init(entropy, entropy_size))
        return KP_INTERNAL_ERROR;

    if (!kp_library_ec_init())
        return KP_INTERNAL_ERROR;

    kp_library_str_init();

    if (!kp_library_session_init())
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

    if (d->kp_get_entropy_fn == NULL)
    {
        status = FALSE;
        goto exit;
    }

exit:
    return status;
}

void kp_dependency_init(kp_dependency *dependency)
{
    dependency->kp_free_fn = NULL;
    dependency->kp_malloc_fn = NULL;
    dependency->kp_realloc_fn = NULL;
    dependency->kp_get_entropy_fn = NULL;
    dependency->kp_memcmp_fn = NULL;
    dependency->kp_memcpy_fn = NULL;
    dependency->kp_memmove_fn = NULL;
    dependency->kp_memset_fn = NULL;
}

kp_status kp_library_deinit()
{
    kp_library_buffer_deinit();

    return KP_SUCCESS;
}