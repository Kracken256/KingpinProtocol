#ifndef __KINGPIN_CORE_H_
#define __KINGPIN_CORE_H_

#ifndef __KINGPIN_BACKEND_H_
#error "Do not include this file directly. Only backend .c files should do so."
#endif // __KINGPIN_BACKEND_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <types/kp.h>

    /// @brief Global configuration for Kingpin.
    extern kp_dependency gKP_dependency_config;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_CORE_H_