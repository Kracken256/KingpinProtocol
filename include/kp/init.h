#ifndef __KINGPIN_INIT_H_
#define __KINGPIN_INIT_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if !defined(__KINGPIN_H_) && !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly. Include kingpin.h instead."
#endif // __KINGPIN_H_

#include <kp/types/struct.h>
#include <kp/types/err.h>

    /// @brief Initializes the Kingpin library
    /// @param dependency The dependency structure to use
    /// @return KP_SUCCESS on success, an error code otherwise
    /// @warning The library uses dynamic memory allocation internally, so
    /// make sure to call the `kp_library_deinit` function when you are done.
    kp_status kp_library_init(const kp_dependency *dependency);

    /// @brief Initializes a dependency structure with default values
    /// @param dependency The dependency structure to initialize
    void kp_dependency_init(kp_dependency *dependency);

    /// @brief Deinitializes the Kingpin library
    /// @return KP_SUCCESS on success, an error code otherwise
    kp_status kp_library_deinit();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_INIT_H_