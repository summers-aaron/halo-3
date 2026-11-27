#ifndef ANIMATIONS_CODECS_ANIMATION_CODECS_H
#define ANIMATIONS_CODECS_ANIMATION_CODECS_H

#include "cseries/cseries_types.h"
#include "math/integer_math_templates.h"

// match: 0x310668: the rotation and translation skip paths need two out-of-line count_bits calls. Without the limit only one survives.
#pragma inline_depth(0)
__forceinline long animation_codec_next_channel(long channel, unsigned long animated_nodes)
{
    return channel + count_bits(animated_nodes);
}
// match: 0x312590: leaving the depth at zero emits a third count_bits call in the scale skip path.
#pragma inline_depth()

#if !defined(_XBOX) || defined(__clang__)
#include "animations/animation_codec_types.h"

animation_codec_decompress_all_function
    animation_codec_decompress_static,
    animation_codec_decompress_fullframe,
    animation_codec_decompress_quantized,
    animation_codec_decompress_quantized_interpolated,
    animation_codec_decompress_byte_keyframe,
    animation_codec_decompress_byte_keyframe_interpolated,
    animation_codec_decompress_word_keyframe;
#endif

// Functions defined in tu/blamlibxbox/animation_codecs.cpp.

struct real_orientation;
struct s_static_codec_header;
struct s_fullframe_codec_header;
struct s_keyframe_codec_header;

extern "C" void animation_codecs_get_constant_translation(const s_static_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_get_constant_scale(const s_static_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_compute_fullframe_rotation(const s_fullframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_compute_fullframe_translation(const s_fullframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_compute_fullframe_scale(const s_fullframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_get_fullframe_rotation(const s_fullframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_get_fullframe_translation(const s_fullframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_get_fullframe_scale(const s_fullframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_compute_quantized_rotation(const s_fullframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_get_quantized_rotation(const s_fullframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_byte_rotation_interpolated(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_byte_translation_interpolated(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_byte_scale_interpolated(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_byte_rotation(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_byte_translation(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_byte_scale(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_word_rotation_interpolated(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_word_translation_interpolated(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_word_scale_interpolated(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_word_rotation(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_word_translation(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_word_scale(const s_keyframe_codec_header *header,
    long node, long frame0, long frame1,
    real w0, real w1,
    real_orientation *out);
extern "C" void animation_codecs_decode_blend_screen_nodes(const s_fullframe_codec_header *header,
    long node_count,
    unsigned long frame0, unsigned long frame1,
    real w0, real w1, bool root_only,
    const unsigned long *animated, const unsigned long *requested, unsigned long *flags,
    real_orientation *out);

#include <vectorintrinsics.h>
extern "C" void animation_codecs_decode_quantized_rotation(const char *base, long index, long frame0, long frame1,
    real w0, real w1, __vector4 *out);
#endif // ANIMATIONS_CODECS_ANIMATION_CODECS_H
