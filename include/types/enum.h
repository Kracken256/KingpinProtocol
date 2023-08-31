#ifndef __KINGPIN_ENUM_H_
#define __KINGPIN_ENUM_H_

#ifndef __KINGPIN_KP_H_
#error "Do not include this file directly. Include types/kp.h instead."
#endif // __KINGPIN_KP_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    /// @brief The status of a function call
    /// @warning This status enum has success == 1 and fail == 0
    typedef enum _kp_status
    {
        KP_SUCCESS = 1,
        KP_FAIL = 0,
        KP_DEPENDENCY_MISSING = 100,
        KP_INVALID_ARGUMENT = 100,
        KP_INVALID_STATE = 200,
        KP_MEMORY_ERROR = 300,
        KP_INTERNAL_ERROR = 400,
        KP_NOT_IMPLEMENTED = 500,
        KP_NOT_FOUND = 600,
        KP_ALREADY_EXISTS = 700,
        KP_OUT_OF_BOUNDS = 800,
        KP_NOT_INITIALIZED = 900,
        KP_NOT_SUPPORTED = 1000,
    } kp_status;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_ENUM_H_