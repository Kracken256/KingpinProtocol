#define __KINGPIN_BACKEND

#include <kp/str.h>

void kp_library_str_init(void)
{
}

void kp_library_str_deinit(void)
{
}

void kp_bin2hex(const void *bin, kp_size bin_len, s8 *hex)
{
    static const s8 *hex_table = "0123456789abcdef";
    const u8 *bin_ptr = (const u8 *)bin;
    kp_size i;

    for (i = 0; i < bin_len; i++)
    {
        hex[i * 2] = hex_table[bin_ptr[i] >> 4];
        hex[i * 2 + 1] = hex_table[bin_ptr[i] & 0x0F];
    }
}

void kp_hex2bin(const s8 *hex, kp_size hex_len, void *bin)
{
    u8 *bin_ptr = (u8 *)bin;
    kp_size i;

    for (i = 0; i < hex_len; i += 2)
    {
        bin_ptr[i / 2] = (hex[i] % 32 + 9) % 25 * 16 + (hex[i + 1] % 32 + 9) % 25;
    }
}