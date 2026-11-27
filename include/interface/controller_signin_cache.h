#ifndef INTERFACE_CONTROLLER_SIGNIN_CACHE_H
#define INTERFACE_CONTROLLER_SIGNIN_CACHE_H

#include <stddef.h>

struct s_controller_signin_cache_entry {
    long key;
    long info_key;
    long global_grade;
    unsigned char valid;
};
extern "C" s_controller_signin_cache_entry g_controller_signin_cache[4];

#if defined(_XBOX)
typedef char s_controller_signin_cache_entry_grade_check[(offsetof(s_controller_signin_cache_entry, global_grade) == 8) ? 1 : -1];
typedef char s_controller_signin_cache_entry_valid_check[(offsetof(s_controller_signin_cache_entry, valid) == 0xc) ? 1 : -1];
typedef char s_controller_signin_cache_entry_size_check[(sizeof(s_controller_signin_cache_entry) == 0x10) ? 1 : -1];
#endif

#endif // INTERFACE_CONTROLLER_SIGNIN_CACHE_H
