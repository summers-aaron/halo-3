#ifndef ANIMATIONS_ANIMATION_CODEC_TYPES_H
#define ANIMATIONS_ANIMATION_CODEC_TYPES_H

#include <stddef.h>
#include "cseries/cseries.h"
#include "math/real_math_types.h"

struct s_animation_codec_header
{
    unsigned char codec_type;
    signed char rotation_count;
    signed char translation_count;
    signed char scale_count;
    real error_percentage;
    real playback_rate;
};

// The codecs store signed byte offsets relative to the start of the header.
template<typename t_sample>
struct s_animation_codec_data_offset
{
    int32 byte_offset;

    __forceinline const t_sample *get(const void *header) const
    {
        const void *data = static_cast<const char *>(header) + byte_offset;
        return static_cast<const t_sample *>(data);
    }

    __forceinline const t_sample *get(const void *header, long channel_byte_offset) const
    {
        const void *data = static_cast<const char *>(header) + (channel_byte_offset + byte_offset);
        return static_cast<const t_sample *>(data);
    }
};

struct s_fullframe_codec_header : s_animation_codec_header
{
    s_animation_codec_data_offset<real_point3d> translation_offset;
    s_animation_codec_data_offset<real> scale_offset;
    long rotation_stride;
    long translation_stride;
    long scale_stride;
    __int64 rotations[1];               /* rotation_stride bytes per node */
};

struct s_keyframe_codec_header : s_animation_codec_header
{
    long translation_channels_offset;
    long scale_channels_offset;
    long rotation_times_offset;
    long translation_times_offset;
    long scale_times_offset;
    long rotation_values_offset;
    long translation_values_offset;
    long scale_values_offset;
    unsigned char rotation_channels_padding[4]; /* filler so the inline channel array starts at 0x30; no codec reader uses these bytes */
    unsigned long rotation_channels[1];
};

struct s_static_codec_header : s_animation_codec_header
{
    s_animation_codec_data_offset<real_point3d> translation_offset;
    s_animation_codec_data_offset<real> scale_offset;
};

struct flags256;
struct s_node_flag_sets;
typedef void animation_codec_decompress_all_function(const s_animation_codec_header *header, long node_count,
    unsigned long frame0, unsigned long frame1, real w0, real w1, bool root_only,
    const s_node_flag_sets *animated, const flags256 *requested,
    s_node_flag_sets *flags, real_orientation *orientations);
typedef animation_codec_decompress_all_function *animation_codec_decompress_all_proc;
typedef void (*animation_codec_decompress_translation_proc)(const s_animation_codec_header *header, long node,
    unsigned long frame0, unsigned long frame1, real w0, real w1, real_orientation *orientation);

class c_model_animation_graph;
class c_model_animation;

typedef bool (*animation_codec_compress_proc)(unsigned char codec_type, const c_model_animation_graph *graph,
    c_model_animation *animation, void *data);
typedef void (*animation_codec_byteswap_proc)(s_animation_codec_header *header, long node_count, bool byteswap);

class c_codec_description
{
public:
    const char *name;
    long codec_type;
    long time_value;
    animation_codec_decompress_translation_proc decompress_rotation;
    animation_codec_decompress_translation_proc decompress_translation;
    animation_codec_decompress_translation_proc decompress_scale;
    animation_codec_decompress_translation_proc fast_decompress_rotation;
    animation_codec_decompress_translation_proc fast_decompress_translation;
    animation_codec_decompress_translation_proc fast_decompress_scale;
    animation_codec_decompress_all_proc decompress_all;
    animation_codec_decompress_all_proc decompress_all_interpolated;
    animation_codec_compress_proc compress;
    animation_codec_byteswap_proc byteswap;
};

typedef char animation_codec_header_size[(sizeof(s_animation_codec_header) == 12) ? 1 : -1];
typedef char animation_codec_playback_rate_offset[(offsetof(s_animation_codec_header, playback_rate) == 8) ? 1 : -1];
typedef char static_codec_translation_offset[(offsetof(s_static_codec_header, translation_offset) == 0xc) ? 1 : -1];
typedef char static_codec_scale_offset[(offsetof(s_static_codec_header, scale_offset) == 0x10) ? 1 : -1];
typedef char static_codec_header_size[(sizeof(s_static_codec_header) == 0x14) ? 1 : -1];
typedef char fullframe_codec_translation_offset[(offsetof(s_fullframe_codec_header, translation_offset) == 0xc) ? 1 : -1];
typedef char fullframe_codec_scale_offset[(offsetof(s_fullframe_codec_header, scale_offset) == 0x10) ? 1 : -1];
typedef char fullframe_codec_rotation_offset[(offsetof(s_fullframe_codec_header, rotations) == 0x20) ? 1 : -1];
typedef char keyframe_codec_channel_offset[(offsetof(s_keyframe_codec_header, rotation_channels) == 0x30) ? 1 : -1];
typedef char animation_codec_definition_size[(sizeof(void *) != 4 || sizeof(c_codec_description) == 52) ? 1 : -1];

extern c_codec_description g_codec_descriptions[];

#endif // ANIMATIONS_ANIMATION_CODEC_TYPES_H
