#ifndef GEOMETRY_COMPRESSION_INFO_TYPES_H
#define GEOMETRY_COMPRESSION_INFO_TYPES_H

#include "math/real_math_types.h"

struct s_compression_info
{
    unsigned long flags;
    real_bounds x;
    real_bounds y;
    real_bounds z;
    real_bounds u;
    real_bounds v;
};

typedef char s_compression_info_size_assert[(sizeof(long) != 4 || sizeof(s_compression_info) == 0x2c) ? 1 : -1];

#endif // GEOMETRY_COMPRESSION_INFO_TYPES_H
