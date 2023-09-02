#ifndef __KINGPIN_STR_H_
#define __KINGPIN_STR_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if !defined(__KINGPIN_BACKEND)
#error "Do not include this file directly."
#endif // __KINGPIN_H_

#include <kp/types/basic.h>


void kp_library_str_init(void);

void kp_library_str_deinit(void);

/// @brief Convert binary data to hex string. No null terminator.
/// @param bin 
/// @param bin_len 
/// @param hex 
void kp_bin2hex(const void *bin, kp_size bin_len, s8 *hex);

void kp_hex2bin(const s8 *hex, kp_size hex_len, void *bin);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KINGPIN_RNG_H_