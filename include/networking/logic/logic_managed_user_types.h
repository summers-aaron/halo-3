#ifndef NETWORKING_LOGIC_LOGIC_MANAGED_USER_TYPES_H
#define NETWORKING_LOGIC_LOGIC_MANAGED_USER_TYPES_H

#include <stddef.h>

struct s_online_data
{
    unsigned char type;
    union
    {
        long integer;
        float real;
        double real64; // XUSER_DATA double-precision statistics (leaderboard hopper mu and sigma)
        __int64 integer64;
    } value;
};
struct s_managed_user_context_data
{
    bool valid;
    long value;
};
struct s_managed_user_property_data
{
    bool valid;
    s_online_data data;
};
struct s_managed_user
{
    s_managed_user_context_data contexts[59];
    s_managed_user_property_data properties[59];
};
struct s_managed_user_globals
{
    s_managed_user users[4];
};
typedef char managed_user_data_value_offset[(offsetof(s_online_data, value) == 8) ? 1 : -1];
typedef char managed_user_context_value_offset[(offsetof(s_managed_user_context_data, value) == 4) ? 1 : -1];
typedef char managed_user_data_size[(sizeof(s_online_data) == 16) ? 1 : -1];

#endif
