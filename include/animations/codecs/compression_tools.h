#ifndef ANIMATIONS_CODECS_COMPRESSION_TOOLS_H
#define ANIMATIONS_CODECS_COMPRESSION_TOOLS_H

#include <string.h>
#include "animations/codecs/quantized_rotation_codec.h"

// March animations\codecs\compression_tools.h: typed views over uncompressed frame records.
// Include after the owning unit declares its codec constant anchors (quantized_rotation_codec.h).

// the uncompressed frame data: each frame's record copied out as raw bytes
template <typename t_type>
struct c_raw_data_wrapper
{
    t_type m_data;

    __forceinline void decompress(t_type *out) const
    {
        memcpy(out, this, sizeof(t_type));
    }
};

// the raw quaternion record: blends with a second record by the two frame weights
template <>
struct c_raw_data_wrapper<real_quaternion>
{
    real_quaternion m_data;

    __forceinline void decompress(real_quaternion *out) const
    {
        memcpy(out, this, sizeof(real_quaternion));
    }

    __forceinline void interpolate(const c_raw_data_wrapper *other, real w0, real w1, __vector4 *result) const
    {
        __vector4 a, b;
        decompress((real_quaternion *)&a);
        other->decompress((real_quaternion *)&b);
        __vector4 weight0 = __vzero();
        __vector4 weight1 = __vzero();
        __loadunalignedvectorelement(weight0, &w0, 0);
        __loadunalignedvectorelement(weight1, &w1, 0);
        weight0 = __vspltw(weight0, 0);
        weight1 = __vspltw(weight1, 0);
        *result = __vmaddfp(b, weight0, __vmulfp(a, weight1));
        *result = animation_codec_detail::animation_codecs_normalize(*result);
    }
};

#endif
