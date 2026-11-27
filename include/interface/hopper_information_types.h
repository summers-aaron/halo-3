#ifndef INTERFACE_HOPPER_INFORMATION_TYPES_H
#define INTERFACE_HOPPER_INFORMATION_TYPES_H

#include <stddef.h>

// user_interface_matchmaking_get_hopper_information writes this 0x54-byte result.
// The hopper list reads visibility at +0x50 and enabled at +0x51; achievements
// reads ranked at +0x52. The compiler supplies the alignment before the longs.
struct s_hopper_information
{
    unsigned short name[32];
    unsigned short id;
    long value;
    unsigned char kind;
    long number;
    bool visible;
    bool enabled;
    bool ranked;
    unsigned char b3;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char hopper_information_size[(sizeof(s_hopper_information) == 0x54) ? 1 : -1];
typedef char hopper_information_number_offset[(offsetof(s_hopper_information, number) == 0x4c) ? 1 : -1];
typedef char hopper_information_ranked_offset[(offsetof(s_hopper_information, ranked) == 0x52) ? 1 : -1];
#endif

#endif // INTERFACE_HOPPER_INFORMATION_TYPES_H
