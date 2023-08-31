#ifndef __KINGPIN_INIT_H_
#define __KINGPIN_INIT_H_

#include <types/basic.h>
#include <types/struct.h>
#include <types/enum.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    kp_status kp_lib_init(kp_dependency *dependency);

    kp_status kp_lib_deinit(kp_uninit *uninit_mode);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_H_