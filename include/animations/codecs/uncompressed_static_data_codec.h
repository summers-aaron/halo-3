#ifndef ANIMATIONS_CODECS_UNCOMPRESSED_STATIC_DATA_CODEC_H
#define ANIMATIONS_CODECS_UNCOMPRESSED_STATIC_DATA_CODEC_H

#include <vectorintrinsics.h>
#include "animations/animation_codec_types.h"
#include "animations/codecs/quantized_rotation_codec.h"

// March animations\codecs\uncompressed_static_data_codec.h: one record per node, no frames.
// Include after the owning unit declares its codec constant anchors (quantized_rotation_codec.h).
struct c_uncompressed_static_data_codec
{
    typedef s_static_codec_header header_type;

    static __forceinline void decompress_rotation(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out, __vector4 scale, __vector4 bias,
        __vector4 key_scale, __vector4 key_bias)
    {
        __stvx(animation_codec_detail::animation_codecs_normalize(
            animation_codec_detail::animation_codecs_unpack(__vzero(), (char *)header + (node + 4) * 8, scale, bias)),
            &out->rotation, 0);
    }

    // March get_node_translation_list: the node's constant translation in the list at translation_offset
    static __forceinline const real_point3d *get_node_translation_list(const header_type *header, long node)
    {
        return header->translation_offset.get(header) + node;
    }

    // March get_node_scale_list: the node's constant scale in the list at scale_offset
    static __forceinline const real *get_node_scale_list(const header_type *header, long node)
    {
        return header->scale_offset.get(header) + node;
    }

    static __forceinline void decompress_translation(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        out->translation = *get_node_translation_list(header, node);
    }

    static __forceinline void decompress_scale(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        out->scale = *get_node_scale_list(header, node);
    }
};

#endif // ANIMATIONS_CODECS_UNCOMPRESSED_STATIC_DATA_CODEC_H
