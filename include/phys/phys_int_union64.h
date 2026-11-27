#ifndef INCLUDE_PHYS_INT_UNION64_H
#define INCLUDE_PHYS_INT_UNION64_H

#include <vectorintrinsics.h>

union phys_int_union64 {
    __vector4 quad;
    __int64 i64;
    unsigned __int64 as_u64;
    int i32[2];
    unsigned int as_u32[2];
    short i16_lanes[4];
    unsigned short as_u16[4];
    signed char i8[8];
    unsigned char u8[8];
};

#endif
