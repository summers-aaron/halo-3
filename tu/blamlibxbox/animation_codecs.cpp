#include "animations/animation_codec_interpolated_constants.h"
#include "animations/animation_codec_constants.h"
#include "animations/animation_codec_flags.h"
#include "animations/animation_codec_types.h"
// blamlibxbox/animation_codecs.obj: from the records 0x307b38 0x307b90 0x307bc8 0x307be0 0x307cb0 0x307d58 0x307da0 0x307de0 0x307e20 0x307e48 0x307f08 0x307f68 0x308128 0x308290 0x308398 0x308540 0x3086b0 0x3087c0 0x308988 0x308b00 0x308c18 0x308dc8 0x308f40 0x310668 0x311b00 0x312590
// @flags /TP /O2 /Oc
#include "cseries/cseries_types.h"
// @address 0x6f4a14 __savevmx_124
// @address 0x6f4cac __restvmx_124
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <string.h>
#include <stdlib.h>
#include "animations/codecs/animation_codecs.h"
#include "cseries/xmem.h"
#include "math/integer_math.h"
#include "animations/codecs/quantized_rotation_codec.h"
#include "animations/codecs/compression_tools.h"
#include "animations/codecs/uncompressed_static_data_codec.h"
#if !defined(_XBOX) || defined(__clang__)
#include "effects/particle_movement_definitions.h"
#endif

/* ---- the keyframe codecs: K is the key time type, T and S the translation and scale key types;
   k_fractional is the interpolating entry (frame0 + w0) ---- */
template <typename K, typename T, typename S, bool k_fractional>
struct c_keyframe_codec_template
{
    typedef s_keyframe_codec_header header_type;

    static __forceinline unsigned char *animation_codecs_keyframe_times(const s_keyframe_codec_header *header,
        long offset, unsigned long first, unsigned char *)
    {
        return (unsigned char *)header + (offset + first);
    }
    
    static __forceinline unsigned short *animation_codecs_keyframe_times(const s_keyframe_codec_header *header,
        long offset, unsigned long first, unsigned short *)
    {
        return (unsigned short *)((char *)header + (first * 2 + offset));
    }


    static __forceinline long find_key(K *times, long count, long time)
    {
        long lo = 0;
        long hi = count;
        while (hi > lo + 1) {
            long mid = (hi + lo) >> 1;
            if (times[mid] <= (unsigned long)time)
                lo = mid;
            else
                hi = mid;
        }
        return lo;
    }

    static __forceinline void decompress_rotation(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        unsigned long channel_info = header->rotation_channels[node];
        long count = channel_info & 0xfff;
        unsigned long first = channel_info >> 12;
        K *times = animation_codecs_keyframe_times(header, header->rotation_times_offset, first, (K *)0);
        __int64 *values = (__int64 *)((char *)header + (first * 8 + header->rotation_values_offset));
        long lo = find_key(times, count, frame0);
        K *key_time = &times[lo];
        __int64 *key = &values[lo];
        __dcbt(0, key);
        __vector4 v;
        if (k_fractional ? lo + 1 < count : frame0 != key_time[0]) {
            __vector4 zero = __vzero();
            __vector4 sa = zero;
            __vector4 sb = zero;
            real wa, wb;
            if (k_fractional) {
                real t = (real)(long)frame0 + w0;
                wa = (real)key_time[1] - t;
                wb = t - (real)key_time[0];
            } else {
                long after = frame0 - key_time[0];
                long before = key_time[1] - frame0;
                wa = (real)before;
                wb = (real)after;
            }
            __loadunalignedvectorelement(sa, &wa, 0);
            __loadunalignedvectorelement(sb, &wb, 0);
            sa = __vspltw(sa, 0);
            sb = __vspltw(sb, 0);
#if defined(_XBOX) && !defined(__clang__)
            // match: 0x307f68: the interpolated pair is indexed off the 0x8202a8e0 base already held for the quantized
            // pair; naming it loads a second base and the byte/word decoders lose 4 bytes or reorder their lvx128 loads
            __vector4 a = animation_codec_detail::animation_codecs_unpack(zero, key,
            k_animation_codec_quantized_rotation_constants[13], k_animation_codec_quantized_rotation_constants[14]);
            __vector4 b = animation_codec_detail::animation_codecs_unpack(zero, key + 1,
            k_animation_codec_quantized_rotation_constants[13], k_animation_codec_quantized_rotation_constants[14]);
#else
            __vector4 a = animation_codec_detail::animation_codecs_unpack(zero, key,
            k_animation_codec_interpolated_rotation_constants[0], k_animation_codec_interpolated_rotation_constants[1]);
            __vector4 b = animation_codec_detail::animation_codecs_unpack(zero, key + 1,
            k_animation_codec_interpolated_rotation_constants[0], k_animation_codec_interpolated_rotation_constants[1]);
#endif
            v = __vmaddfp(b, sb, __vmulfp(a, sa));
        } else {
            v = animation_codec_detail::animation_codecs_unpack(__vzero(), key, k_animation_codec_quantized_rotation_constants[0],
            k_animation_codec_quantized_rotation_constants[1]);
        }
        __stvx(animation_codec_detail::animation_codecs_normalize(v), &out->rotation, 0);
    }

    static __forceinline void decompress_rotation(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out, __vector4 scale, __vector4 bias,
        __vector4 key_scale, __vector4 key_bias)
    {
        unsigned long channel_info = header->rotation_channels[node];
        long count = channel_info & 0xfff;
        unsigned long first = channel_info >> 12;
        K *times = animation_codecs_keyframe_times(header, header->rotation_times_offset, first, (K *)0);
        __int64 *values = (__int64 *)((char *)header + (first * 8 + header->rotation_values_offset));
        long lo = find_key(times, count, frame0);
        K *key_time = &times[lo];
        __int64 *key = &values[lo];
        __dcbt(0, key);
        __vector4 v;
        if (k_fractional ? lo + 1 < count : frame0 != key_time[0]) {
            __vector4 zero = __vzero();
            __vector4 sa = zero;
            __vector4 sb = zero;
            real wa, wb;
            if (k_fractional) {
                real t = (real)(long)frame0 + w0;
                wa = (real)key_time[1] - t;
                wb = t - (real)key_time[0];
            } else {
                long after = frame0 - key_time[0];
                long before = key_time[1] - frame0;
                wa = (real)before;
                wb = (real)after;
            }
            __loadunalignedvectorelement(sa, &wa, 0);
            __loadunalignedvectorelement(sb, &wb, 0);
            sa = __vspltw(sa, 0);
            sb = __vspltw(sb, 0);
            __vector4 a = animation_codec_detail::animation_codecs_unpack(zero, key, key_scale, key_bias);
            __vector4 b = animation_codec_detail::animation_codecs_unpack(zero, key + 1, key_scale, key_bias);
            v = __vmaddfp(b, sb, __vmulfp(a, sa));
            __stvx(animation_codec_detail::animation_codecs_normalize(v), &out->rotation, 0);
        } else {
            v = animation_codec_detail::animation_codecs_unpack(__vzero(), key, scale, bias);
            __stvx(animation_codec_detail::animation_codecs_normalize(v), &out->rotation, 0);
        }
    }

    static __forceinline void decompress_translation(const header_type *header, long node, long frame0, long frame1,
        // match: 0x308540: __restrict on out pairs the key translation loads ahead of their stores
        real w0, real w1, real_orientation *__restrict out)
    {
        unsigned long channel_info =
            *(unsigned long *)((char *)header + header->translation_channels_offset + node * 4);
        long count = channel_info & 0xfff;
        unsigned long first = channel_info >> 12;
        K *times = animation_codecs_keyframe_times(header, header->translation_times_offset, first, (K *)0);
        const T *values = (const T *)((const char *)header +
            (first * 12 + header->translation_values_offset));
        long lo = find_key(times, count, frame0);
        K *key_time = &times[lo];
        const T *key = &values[lo];
        __dcbt(0, key);
        out->translation = key[0].m_data;
        if (k_fractional ? lo + 1 < count : lo + 1 < count && frame0 != key_time[0]) {
            const real_point3d *next_key = &key[1].m_data;
            real_point3d next = *next_key;
            long time0 = key_time[0];
            real time = k_fractional ? (real)frame0 + w0 : (real)frame0;
            real t = __fres((real)(key_time[1] - time0)) * (time - (real)time0);
            animation_codec_detail::animation_codecs_translation_interpolate(&out->translation, &next, t,
                &out->translation);
        }
    }

    static __forceinline void decompress_scale(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        unsigned long channel_info = *(unsigned long *)((char *)header + header->scale_channels_offset + node * 4);
        long count = channel_info & 0xfff;
        unsigned long first = channel_info >> 12;
        K *times = animation_codecs_keyframe_times(header, header->scale_times_offset, first, (K *)0);
        const S *values = (const S *)((const char *)header +
            (first * 4 + header->scale_values_offset));
        long lo = find_key(times, count, frame0);
        K *key_time = &times[lo];
        const S *key = &values[lo];
        __dcbt(0, key);
        key[0].decompress(&out->scale);
        if (k_fractional ? lo + 1 < count : lo + 1 < count && frame0 != key_time[0]) {
            real next;
            key[1].decompress(&next);
            real delta = next - out->scale;
            long time0 = key_time[0];
            real time = k_fractional ? (real)frame0 + w0 : (real)frame0;
            real t = __fres((real)(key_time[1] - time0)) * (time - (real)time0);
            out->scale = t * delta + out->scale;
        }
    }
};

typedef c_keyframe_codec_template<unsigned char, c_raw_data_wrapper<real_point3d>, c_raw_data_wrapper<real>, false> c_byte_keyframe_codec;
typedef c_keyframe_codec_template<unsigned char, c_raw_data_wrapper<real_point3d>, c_raw_data_wrapper<real>, true> c_byte_keyframe_interpolated_codec;
typedef c_keyframe_codec_template<unsigned short, c_raw_data_wrapper<real_point3d>, c_raw_data_wrapper<real>, false> c_word_keyframe_codec;
typedef c_keyframe_codec_template<unsigned short, c_raw_data_wrapper<real_point3d>, c_raw_data_wrapper<real>, true> c_word_keyframe_interpolated_codec;

struct c_blend_screen_codec
{
    typedef s_fullframe_codec_header header_type;

    static __forceinline void decompress_rotation(const header_type *header, long node, long frame0, long frame1,
        // match: 0x311b00: __restrict on out hoists the rotation row loads ahead of their stores
        real w0, real w1, real_orientation *__restrict out)
    {
        const c_raw_data_wrapper<real_quaternion> *rows =
            (const c_raw_data_wrapper<real_quaternion> *)((const char *)header->rotations + header->rotation_stride * node);
        rows[frame0].decompress(&out->rotation);
    }

    static __forceinline void decompress_translation(const header_type *header, long node, long frame0, long frame1,
        // match: 0x311b00: __restrict on out pairs the translation loads ahead of their stores
        real w0, real w1, real_orientation *__restrict out)
    {
        const real *translations = (const real *)((const char *)header + header->translation_stride * node +
            header->translation_offset.byte_offset);
        memcpy(&out->translation, &translations[frame0 * 3], sizeof(real_point3d));
    }

    static __forceinline void decompress_scale(const header_type *header, long node, long frame0, long frame1,
        real w0, real w1, real_orientation *out)
    {
        const long stride = header->scale_stride;
        // match: 0x311b00: using memcpy moves the entry copy of r10 from r17 to r18 and grows the decoder by 88 bytes.
        *(long *)&out->scale = *(long *)((char *)header +
            (stride * node + header->scale_offset.byte_offset + frame0 * 4));
    }
};


/* Blend-screen frames hold uncompressed channels, with rotation weights supplied
   separately and linear interpolation for translation and scale. */
struct c_blend_screen_interpolated_codec
{
    static __forceinline void fast_translation_interpolate(const real_point3d *p1, const real_point3d *p2, real ratio,
        real_point3d *result)
    {
        result->x = (p2->x - p1->x) * ratio + p1->x;
        result->y = (p2->y - p1->y) * ratio + p1->y;
        result->z = (p2->z - p1->z) * ratio + p1->z;
    }
    
    static __forceinline void fast_scale_interpolate(real s1, real s2, real ratio, real *result)
    {
        *result = (s2 - s1) * ratio + s1;
    }

    static __forceinline const c_raw_data_wrapper<real_quaternion> *get_node_rotation_list(
        const s_fullframe_codec_header *header, long node)
    {
        return (const c_raw_data_wrapper<real_quaternion> *)((const char *)header->rotations +
            header->rotation_stride * node);
    }

    static __forceinline const real *get_node_translation_list(
        const s_fullframe_codec_header *header, long node)
    {
        const s_fullframe_codec_header *fullframe_header = header;
        return (const real *)((const char *)fullframe_header + fullframe_header->translation_offset.byte_offset +
            node * fullframe_header->translation_stride);
    }

    static __forceinline const c_raw_data_wrapper<real> *get_node_scale_list(
        const s_fullframe_codec_header *header, long node)
    {
        return (const c_raw_data_wrapper<real> *)((const char *)header +
            (header->scale_stride * node + header->scale_offset.byte_offset));
    }

    static __forceinline void decompress_rotation(const s_fullframe_codec_header *header, long node,
        long frame0, long frame1, real w0, real w1, real_orientation *out)
    {
        const c_raw_data_wrapper<real_quaternion> *list = get_node_rotation_list(header, node);
        __vector4 q;
        list[frame0].interpolate(&list[frame1], w0, w1, &q);
        __stvx(q, &out->rotation, 0);
    }


    static __forceinline void decompress_translation(const s_fullframe_codec_header *header, long node,
        long frame0, long frame1, real w0, real w1, real_orientation *out)
    {
        const real *list = get_node_translation_list(header, node);
        real_point3d p1, p2;
        memcpy(&p1, &list[frame0 * 3], sizeof(p1));
        memcpy(&p2, &list[frame1 * 3], sizeof(p2));
        fast_translation_interpolate(&p1, &p2, w0, &out->translation);
    }

    static __forceinline void decompress_root_translation(const s_fullframe_codec_header *header, long node,
        long frame0, long frame1, real w0, real w1, real_orientation *out)
    {
        const real *list = get_node_translation_list(header, node);
        real_point3d p1, p2;
        memcpy(&p1, &list[frame0 * 3], sizeof(p1));
        memcpy(&p2, &list[frame1 * 3], sizeof(p2));
        fast_translation_interpolate(&p1, &p2, w0, &out->translation);


    }

    static __forceinline void decompress_scale(const s_fullframe_codec_header *header, long node,
        long frame0, long frame1, real w0, real w1, real_orientation *out)
    {
        const c_raw_data_wrapper<real> *list = get_node_scale_list(header, node);
        real s1, s2;
        list[frame0].decompress(&s1);
        list[frame1].decompress(&s2);
        fast_scale_interpolate(s1, s2, w0, &out->scale);
    }
};


// @address 0x307b38 animation_codecs_decode_quantized_rotation
void animation_codecs_decode_quantized_rotation(const char *base, long index, long frame0, long frame1, real w0, real w1, __vector4 *out)
{
    __vector4 v = animation_codec_detail::animation_codecs_unpack(__vzero(), base + (index + 4) * 8,
            k_animation_codec_quantized_rotation_constants[0], k_animation_codec_quantized_rotation_constants[1]);
    *out = animation_codec_detail::animation_codecs_normalize(v);
}

// @address 0x307b90 animation_codecs_get_constant_translation
void animation_codecs_get_constant_translation(const s_static_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    // match: 0x307b90: __restrict on out issues the three translation word loads ahead of their stores
    real_orientation *__restrict out)
{
    c_uncompressed_static_data_codec::decompress_translation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x307bc8 animation_codecs_get_constant_scale
void animation_codecs_get_constant_scale(const s_static_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    real_orientation * out)
{
    c_uncompressed_static_data_codec::decompress_scale(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x307be0 animation_codecs_compute_fullframe_rotation
void animation_codecs_compute_fullframe_rotation(const s_fullframe_codec_header *header, long node, long frame0, long frame1,
            real w0, real w1,
    real_orientation * out)
{
    real_quaternion *rows = (real_quaternion *)((char *)header + (header->rotation_stride * node + 0x20));
    __vector4 a;
    __vector4 b;
    memcpy(&a, &rows[frame0], sizeof(a));
    memcpy(&b, &rows[frame1], sizeof(b));
    __vector4 zero = __vzero();
    __vector4 s0 = zero;
    __vector4 s1 = zero;
    __loadunalignedvectorelement(s0, &w0, 0);
    __loadunalignedvectorelement(s1, &w1, 0);
    s0 = __vspltw(s0, 0);
    s1 = __vspltw(s1, 0);
    __vector4 v = __vmaddfp(b, s0, __vmulfp(a, s1));
    __stvx(animation_codec_detail::animation_codecs_normalize(v), &out->rotation, 0);
}

// @address 0x307cb0 animation_codecs_compute_fullframe_translation
void animation_codecs_compute_fullframe_translation(const s_fullframe_codec_header *header, long node, long frame0, long frame1,
            real w0, real w1,
    real_orientation * out)
{
    const real *translations = (const real *)((const char *)header + (header->translation_stride * node + header->translation_offset.byte_offset));
    real_point3d a;
    real_point3d b;
    memcpy(&a, &translations[frame0 * 3], sizeof(a));
    memcpy(&b, &translations[frame1 * 3], sizeof(b));
    animation_codec_detail::animation_codecs_translation_interpolate(&a, &b, w0, &out->translation);
}

// @address 0x307d58 animation_codecs_compute_fullframe_scale
void animation_codecs_compute_fullframe_scale(const s_fullframe_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    real_orientation * out)
{
    const c_raw_data_wrapper<real> *values = (const c_raw_data_wrapper<real> *)((const char *)header +
        (header->scale_stride * node + header->scale_offset.byte_offset));
    real a, b;
    values[frame0].decompress(&a);
    values[frame1].decompress(&b);
    out->scale = (b - a) * w0 + a;
}

// @address 0x307da0 animation_codecs_get_fullframe_rotation
void animation_codecs_get_fullframe_rotation(const s_fullframe_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    // match: 0x307da0: __restrict on out issues the four rotation word loads ahead of their stores
    real_orientation *__restrict out)
{
    const c_raw_data_wrapper<real_quaternion> *rows =
        (const c_raw_data_wrapper<real_quaternion> *)((const char *)header->rotations + header->rotation_stride * node);
    rows[frame0].decompress(&out->rotation);
}

// @address 0x307de0 animation_codecs_get_fullframe_translation
void animation_codecs_get_fullframe_translation(const s_fullframe_codec_header *header, long node, long frame0, long frame1,
            real w0, real w1,
    // match: 0x307de0: __restrict on out issues the translation word loads ahead of their stores
    real_orientation *__restrict out)
{
    const real *translations = (const real *)((const char *)header + (header->translation_stride * node + header->translation_offset.byte_offset));
    memcpy(&out->translation, &translations[frame0 * 3], sizeof(real_point3d));
}

// @address 0x307e20 animation_codecs_get_fullframe_scale
void animation_codecs_get_fullframe_scale(const s_fullframe_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    real_orientation * out)
{
    const c_raw_data_wrapper<real> *values = (const c_raw_data_wrapper<real> *)((const char *)header +
        (header->scale_stride * node + header->scale_offset.byte_offset));
    values[frame0].decompress(&out->scale);
}

// @address 0x307e48 animation_codecs_compute_quantized_rotation
void animation_codecs_compute_quantized_rotation(const s_fullframe_codec_header *header, long node, long frame0, long frame1,
            real w0, real w1,
    real_orientation * out)
{
    c_8byte_quantized_rotation_only_interpolated_codec::decompress_rotation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x307f08 animation_codecs_get_quantized_rotation
void animation_codecs_get_quantized_rotation(const s_fullframe_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    real_orientation * out)
{
    c_8byte_quantized_rotation_only_codec::decompress_rotation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x307f68 animation_codecs_decode_byte_rotation_interpolated
void animation_codecs_decode_byte_rotation_interpolated(const s_keyframe_codec_header *header, long node, long frame0, long frame1,
            real w0, real w1,
    real_orientation * out)
{
    c_byte_keyframe_interpolated_codec::decompress_rotation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x308128 animation_codecs_decode_byte_translation_interpolated
void animation_codecs_decode_byte_translation_interpolated(const s_keyframe_codec_header *header, long node, long frame0,
            long frame1, real w0, real w1,
    // match: 0x308128: __restrict on out issues the translation word loads ahead of their stores
    real_orientation *__restrict out)
{
    c_byte_keyframe_interpolated_codec::decompress_translation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x308290 animation_codecs_decode_byte_scale_interpolated
void animation_codecs_decode_byte_scale_interpolated(const s_keyframe_codec_header *header, long node, long frame0, long frame1,
            real w0, real w1,
    real_orientation * out)
{
    c_byte_keyframe_interpolated_codec::decompress_scale(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x308398 animation_codecs_decode_byte_rotation
void animation_codecs_decode_byte_rotation(const s_keyframe_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    real_orientation * out)
{
    c_byte_keyframe_codec::decompress_rotation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x308540 animation_codecs_decode_byte_translation
void animation_codecs_decode_byte_translation(const s_keyframe_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    // match: 0x308540: __restrict on out issues the translation word loads ahead of their stores
    real_orientation *__restrict out)
{
    c_byte_keyframe_codec::decompress_translation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x3086b0 animation_codecs_decode_byte_scale
void animation_codecs_decode_byte_scale(const s_keyframe_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    real_orientation * out)
{
    c_byte_keyframe_codec::decompress_scale(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x3087c0 animation_codecs_decode_word_rotation_interpolated
void animation_codecs_decode_word_rotation_interpolated(const s_keyframe_codec_header *header, long node, long frame0, long frame1,
            real w0, real w1,
    real_orientation * out)
{
    c_word_keyframe_interpolated_codec::decompress_rotation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x308988 animation_codecs_decode_word_translation_interpolated
void animation_codecs_decode_word_translation_interpolated(const s_keyframe_codec_header *header, long node, long frame0,
            long frame1, real w0, real w1,
    // match: 0x308988: __restrict on out issues the translation word loads ahead of their stores
    real_orientation *__restrict out)
{
    c_word_keyframe_interpolated_codec::decompress_translation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x308b00 animation_codecs_decode_word_scale_interpolated
void animation_codecs_decode_word_scale_interpolated(const s_keyframe_codec_header *header, long node, long frame0, long frame1,
            real w0, real w1,
    real_orientation * out)
{
    c_word_keyframe_interpolated_codec::decompress_scale(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x308c18 animation_codecs_decode_word_rotation
void animation_codecs_decode_word_rotation(const s_keyframe_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    real_orientation * out)
{
    c_word_keyframe_codec::decompress_rotation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x308dc8 animation_codecs_decode_word_translation
void animation_codecs_decode_word_translation(const s_keyframe_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    // match: 0x308dc8: __restrict on out issues the translation word loads ahead of their stores
    real_orientation *__restrict out)
{
    c_word_keyframe_codec::decompress_translation(header, node, frame0, frame1, w0, w1, out);
}

// @address 0x308f40 animation_codecs_decode_word_scale
void animation_codecs_decode_word_scale(const s_keyframe_codec_header *header, long node, long frame0, long frame1, real w0, real w1,
    real_orientation * out)
{
    c_word_keyframe_codec::decompress_scale(header, node, frame0, frame1, w0, w1, out);
}

/* ---- _blend_screen decompress-all: the codec's three getters applied to every requested node ---- */


static __forceinline long animation_codecs_lowest_bit(unsigned long bits, const long *table)
{
    return table[abs((long)(bits & -bits) % 37)];
}

static __forceinline bool animation_codecs_flags_empty(const unsigned long *bits)
{
    bool empty = true;
    empty &= bits[0] == 0;
    empty &= bits[1] == 0;
    empty &= bits[2] == 0;
    empty &= bits[3] == 0;
    empty &= bits[4] == 0;
    empty &= bits[5] == 0;
    empty &= bits[6] == 0;
    empty &= bits[7] == 0;
    return empty;
}

static __forceinline long animation_codecs_lowest_set_bit(unsigned long bits)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x310668: k_animation_lowest_set_bit_table is indexed off the 0x8202a8e0 base register held for the
    // rotation constants; naming the table loads a second base and the function shrinks by 40 bytes
    return ((const long *)&k_animation_codec_quantized_rotation_constants[2])[abs((long)(bits & (0 - bits)) % 37)];
#else
    return k_animation_lowest_set_bit_table[abs((long)(bits & (0 - bits)) % 37)];
#endif
}


template<typename t_codec>
void c_codec_decompressor<t_codec>::decompress_all(const typename t_codec::header_type *header, long node_count,
    unsigned long frame0, unsigned long frame1, real w0, real w1, bool root_only, const unsigned long *animated,
    // match: 0x310668: __restrict on out removes frame0/animated spills across the node loop
    const unsigned long *requested, unsigned long *flags, real_orientation *__restrict out)
{
    if (animation_codecs_flags_empty(requested))
        return;

    XMemCpy(flags, animated, sizeof(s_node_flag_sets));
    animation_mask_node_flags((s_node_flag_sets *)flags, (const flags256 *)requested);
    unsigned long words = (node_count + 31) >> 5;
    unsigned long word_count = words > 8 ? 8 : words;

    if (!animation_node_flags_are_empty((const flags256 *)flags))
    {
        long channel_next = 0;
        const unsigned long *mask = animated;
        long word = 0;
        if (word < (long)word_count)
        {
            __vector4 key_scale = __vzero();
#if defined(_XBOX) && !defined(__clang__)
            // match: 0x310668: the interpolated pair is indexed off the quantized pair base register; naming it loads
            // a second base and the function grows by 4 bytes
            __vector4 key_bias = k_animation_codec_quantized_rotation_constants[14];
            key_scale = k_animation_codec_quantized_rotation_constants[13];
#else
            __vector4 key_bias = k_animation_codec_interpolated_rotation_constants[1];
            key_scale = k_animation_codec_interpolated_rotation_constants[0];
#endif
            __vector4 bias = k_animation_codec_quantized_rotation_constants[1];
            __vector4 scale = k_animation_codec_quantized_rotation_constants[0];
            do
            {
                long base = word * 32;
                long bit;
                unsigned long b = requested[word];
                unsigned long a = mask[word];
                long channel = channel_next;
                if (a != 0)
                {
                    if (b == 0 || node_count == 0)
                    {
                        channel = animation_codec_next_channel(channel, a);
                    }
                    else
                    {
                        bit = 0;
                        do
                        {
                            long ones_a = animation_codecs_lowest_set_bit(~a);
                            long ones_b = animation_codecs_lowest_set_bit(~b);
                            long run = ones_a > ones_b ? ones_b : ones_a;
                            if (run > 0)
                            {
                                for (long i = 0; i < run; i++)
                                                        {
                                                            real_orientation *node_out = &out[bit + base + i];
                                                            t_codec::decompress_rotation(header, channel + i, frame0, frame1, w0, w1,
                                                                node_out, scale, bias, key_scale, key_bias);
                                                        }
                                ones_a -= run;
                                bit += run;
                                channel += run;
                                a >>= run;
                                b >>= run;
                            }
                            if (ones_a > 0)
                            {
                                long zeros_b = animation_codecs_lowest_set_bit(b);
                                long skip = ones_a > zeros_b ? zeros_b : ones_a;
                                if (skip > 0)
                                {
                                    ones_a -= skip;
                                    bit += skip;
                                    channel += skip;
                                    a >>= skip;
                                    b >>= skip;
                                }
                                while (ones_a > 0)
                                {
                                    if (b & 1)
                                                            {
                                                                real_orientation *node_out = &out[bit + base];
                                                                t_codec::decompress_rotation(header, channel, frame0, frame1, w0, w1,
                                                                    node_out, scale, bias, key_scale, key_bias);
                                                            }
                                    ones_a--;
                                    bit++;
                                    channel++;
                                    a >>= 1;
                                    b >>= 1;
                                }
                            }
                            long zeros_a = animation_codecs_lowest_set_bit(a);
                            bit += zeros_a;
                            a >>= zeros_a;
                            b >>= zeros_a;
                        } while (bit < 32);
                    }
                    channel_next = channel;
                }
            } while (++word < (long)word_count);
        }
    }

    if (root_only)
    {
        if (requested[0] & 1)
        {
            if (flags[8] & 1)
                t_codec::decompress_translation(header, 0, frame0, frame1, w0, w1, out);
            if (flags[16] & 1)
                t_codec::decompress_scale(header, 0, frame0, frame1, w0, w1, out);
        }
        unsigned long *translation_flags = flags + 8;
        XMemSet(translation_flags + 1, 0, sizeof(flags256) - sizeof(*translation_flags));
        translation_flags[0] &= 1;
        unsigned long *scale_flags = flags + 16;
        XMemSet(scale_flags + 1, 0, sizeof(flags256) - sizeof(*scale_flags));
        scale_flags[0] &= 1;
        return;
    }

    if (!animation_node_flags_are_empty(&((s_node_flag_sets *)flags)->translation))
    {
        long channel_next = 0;
        const unsigned long *mask = animated + 8;
        long word = 0;
        if (word < (long)word_count)
        {
            do
            {
                long base = word * 32;
                long bit;
                unsigned long b = requested[word];
                unsigned long a = mask[word];
                long channel = channel_next;
                if (a != 0)
                {
                    if (b == 0 || node_count == 0)
                    {
                        channel = animation_codec_next_channel(channel, a);
                    }
                    else
                    {
                        bit = 0;
                        do
                        {
                            long ones_a = animation_codecs_lowest_set_bit(~a);
                            long ones_b = animation_codecs_lowest_set_bit(~b);
                            long run = ones_a > ones_b ? ones_b : ones_a;
                            if (run > 0)
                            {
                                for (long i = 0; i < run; i++)
                                                        {
                                                            real_orientation *node_out = &out[bit + base + i];
                                                            t_codec::decompress_translation(header, channel + i, frame0, frame1,
            w0, w1, node_out);
                                                        }
                                ones_a -= run;
                                bit += run;
                                channel += run;
                                a >>= run;
                                b >>= run;
                            }
                            if (ones_a > 0)
                            {
                                long zeros_b = animation_codecs_lowest_set_bit(b);
                                long skip = ones_a > zeros_b ? zeros_b : ones_a;
                                if (skip > 0)
                                {
                                    ones_a -= skip;
                                    bit += skip;
                                    channel += skip;
                                    a >>= skip;
                                    b >>= skip;
                                }
                                while (ones_a > 0)
                                {
                                    if (b & 1)
                                                            {
                                                                real_orientation *node_out = &out[bit + base];
                                                                t_codec::decompress_translation(header, channel, frame0, frame1,
            w0, w1, node_out);
                                                            }
                                    ones_a--;
                                    bit++;
                                    channel++;
                                    a >>= 1;
                                    b >>= 1;
                                }
                            }
                            long zeros_a = animation_codecs_lowest_set_bit(a);
                            bit += zeros_a;
                            a >>= zeros_a;
                            b >>= zeros_a;
                        } while (bit < 32);
                    }
                    channel_next = channel;
                }
            } while (++word < (long)word_count);
        }
    }

    if (!animation_codecs_flags_empty(flags + 16))
    {
        long channel_next = 0;
        const unsigned long *mask = animated + 16;
        long word = 0;
        if (word < (long)word_count)
        {
            long channel;
            do
            {
                long base = word * 32;
                long bit;
                unsigned long b = requested[word];
                unsigned long a = mask[word];
                channel = channel_next;
                if (a == 0)
                    continue;
                if (b == 0 || node_count == 0)
                {
                    channel += count_bits(a);
                }
                else
                {
                    bit = 0;
                    do
                    {
                        long ones_a = animation_codecs_lowest_set_bit(~a);
                        long ones_b = animation_codecs_lowest_set_bit(~b);
                        long run = ones_a > ones_b ? ones_b : ones_a;
                        if (run > 0)
                        {
                            for (long i = 0; i < run; i++)
                                                    {
                                                        real_orientation *node_out = &out[bit + base + i];
                                                        t_codec::decompress_scale(header, channel + i, frame0, frame1, w0, w1, node_out);
                                                    }
                            ones_a -= run;
                            bit += run;
                            channel += run;
                            a >>= run;
                            b >>= run;
                        }
                        if (ones_a > 0)
                        {
                            long zeros_b = animation_codecs_lowest_set_bit(b);
                            long skip = ones_a > zeros_b ? zeros_b : ones_a;
                            if (skip > 0)
                            {
                                ones_a -= skip;
                                bit += skip;
                                channel += skip;
                                a >>= skip;
                                b >>= skip;
                            }
                            while (ones_a > 0)
                            {
                                if (b & 1)
                                                        {
                                                            real_orientation *node_out = &out[bit + base];
                                                            t_codec::decompress_scale(header, channel, frame0, frame1, w0, w1, node_out);
                                                        }
                                ones_a--;
                                bit++;
                                channel++;
                                a >>= 1;
                                b >>= 1;
                            }
                        }
                        long zeros_a = animation_codecs_lowest_set_bit(a);
                        bit += zeros_a;
                        a >>= zeros_a;
                        b >>= zeros_a;
                    } while (bit < 32);
                }
            } while (channel_next = channel, ++word < (long)word_count);
        }
    }
}

// @address 0x310668 ?decompress_all@?$c_codec_decompressor@U?$c_keyframe_codec_template@GU?$c_raw_data_wrapper@Treal_point3d@@@@U?$c_raw_data_wrapper@M@@$00@@@@SAXPBUs_keyframe_codec_header@@JKKMM_NPBK2PAKPIAUreal_orientation@@@Z
template struct c_codec_decompressor<c_word_keyframe_interpolated_codec>;


static __forceinline void animation_codecs_blend_screen_rotations(const s_fullframe_codec_header *header, long node_count,
    unsigned long frame0, unsigned long frame1, real w0, real w1, const unsigned long *animated,
    const unsigned long *requested, unsigned long word_count, real_orientation *out)
{
        long channel_next = 0;
        const unsigned long *mask = animated;
        long word = 0;
        if (word < (long)word_count) {
            do {
                long base = word * 32;
                long bit;
                unsigned long b = requested[word];
                unsigned long a = mask[word];
                long channel = channel_next;
                if (a == 0)
                    continue;
                if (b == 0 || node_count == 0) {
                    channel_next = animation_codec_next_channel(channel, a);
                    continue;
                }
                bit = 0;
                do {
                    long ones_a = animation_codecs_lowest_bit(~a, k_animation_lowest_set_bit_table);
                    long ones_b = animation_codecs_lowest_bit(~b, k_animation_lowest_set_bit_table);
                    long run = (ones_a > ones_b ? ones_b : ones_a);
                    if (run > 0) {
                        for (long i = 0; i < run; i++)
                            c_blend_screen_codec::decompress_rotation(header, channel + i, frame0, frame1, w0, w1,
                                                                      &out[bit + base + i]);
                        ones_a -= run;
                        bit += run;
                        channel += run;
                        a >>= run;
                        b >>= run;
                    }
                    if (ones_a > 0) {
                        long zeros_b = animation_codecs_lowest_bit(b, k_animation_lowest_set_bit_table);
                        long skip = (ones_a > zeros_b ? zeros_b : ones_a);
                        if (skip > 0) {
                            ones_a -= skip;
                            bit += skip;
                            channel += skip;
                            a >>= skip;
                            b >>= skip;
                        }
                        while (ones_a > 0) {
                            if (b & 1)
                                c_blend_screen_codec::decompress_rotation(header, channel, frame0, frame1, w0, w1,
                                                                          &out[bit + base + 0]);
                            ones_a--;
                            bit++;
                            channel++;
                            a >>= 1;
                            b >>= 1;
                        }
                    }
                    long zeros_a = animation_codecs_lowest_bit(a, k_animation_lowest_set_bit_table);
                    bit += zeros_a;
                    a >>= zeros_a;
                    b >>= zeros_a;
                } while (bit < 32);
                channel_next = channel;
            } while (++word < (long)word_count);
        }
}

static __forceinline void animation_codecs_blend_screen_translations(const s_fullframe_codec_header *header, long node_count,
    unsigned long frame0, unsigned long frame1, real w0, real w1, const unsigned long *animated,
    // match: 0x311b00: __restrict on out keeps the frame at savegprlr_17
    const unsigned long *requested, unsigned long word_count, real_orientation *__restrict out)
{
        long channel_next = 0;
        const unsigned long *mask = (animated + 8);
        long word = 0;
        if (word < (long)word_count) {
            do {
                long base = word * 32;
                long bit;
                unsigned long b = requested[word];
                unsigned long a = mask[word];
                long channel = channel_next;
                if (a == 0)
                    continue;
                if (b == 0 || node_count == 0) {
                    channel_next = animation_codec_next_channel(channel, a);
                    continue;
                }
                bit = 0;
                do {
                    long ones_a = animation_codecs_lowest_bit(~a, k_animation_lowest_set_bit_table);
                    long ones_b = animation_codecs_lowest_bit(~b, k_animation_lowest_set_bit_table);
                    long run = (ones_a > ones_b ? ones_b : ones_a);
                    if (run > 0) {
                        for (long i = 0; i < run; i++)
                            c_blend_screen_codec::decompress_translation(header, channel + i, frame0, frame1, w0, w1,
                                                                         &out[bit + base + i]);
                        ones_a -= run;
                        bit += run;
                        channel += run;
                        a >>= run;
                        b >>= run;
                    }
                    if (ones_a > 0) {
                        long zeros_b = animation_codecs_lowest_bit(b, k_animation_lowest_set_bit_table);
                        long skip = (ones_a > zeros_b ? zeros_b : ones_a);
                        if (skip > 0) {
                            ones_a -= skip;
                            bit += skip;
                            channel += skip;
                            a >>= skip;
                            b >>= skip;
                        }
                        while (ones_a > 0) {
                            if (b & 1)
                                c_blend_screen_codec::decompress_translation(header, channel, frame0, frame1, w0, w1,
                                                                             &out[bit + base + 0]);
                            ones_a--;
                            bit++;
                            channel++;
                            a >>= 1;
                            b >>= 1;
                        }
                    }
                    long zeros_a = animation_codecs_lowest_bit(a, k_animation_lowest_set_bit_table);
                    bit += zeros_a;
                    a >>= zeros_a;
                    b >>= zeros_a;
                } while (bit < 32);
                channel_next = channel;
            } while (++word < (long)word_count);
        }
}

static __forceinline void animation_codecs_blend_screen_scales(const s_fullframe_codec_header *header, long node_count,
    unsigned long frame0, unsigned long frame1, real w0, real w1, const unsigned long *animated,
    // match: 0x311b00: __restrict on out pins the scale-loop register assignment
    const unsigned long *requested, unsigned long word_count, real_orientation *__restrict out)
{
        long channel_next = 0;
        const unsigned long *mask = (animated + 16);
        long word = 0;
        if (word < (long)word_count) {
            long channel;
            do {
                long base = word * 32;
                long bit;
                unsigned long b = requested[word];
                unsigned long a = mask[word];
                channel = channel_next;
                if (a == 0)
                    continue;
                if (b == 0 || node_count == 0) {
                    channel += count_bits(a);
                    continue;
                }
                bit = 0;
                do {
                    long ones_a = animation_codecs_lowest_bit(~a, k_animation_lowest_set_bit_table);
                    long ones_b = animation_codecs_lowest_bit(~b, k_animation_lowest_set_bit_table);
                    long run = (ones_a > ones_b ? ones_b : ones_a);
                    if (run > 0) {
                        for (long i = 0; i < run; i++)
                            c_blend_screen_codec::decompress_scale(header, channel + i, frame0, frame1, w0, w1,
                                                                   &out[bit + base + i]);
                        ones_a -= run;
                        bit += run;
                        channel += run;
                        a >>= run;
                        b >>= run;
                    }
                    if (ones_a > 0) {
                        long zeros_b = animation_codecs_lowest_bit(b, k_animation_lowest_set_bit_table);
                        long skip = (ones_a > zeros_b ? zeros_b : ones_a);
                        if (skip > 0) {
                            ones_a -= skip;
                            bit += skip;
                            channel += skip;
                            a >>= skip;
                            b >>= skip;
                        }
                        while (ones_a > 0) {
                            if (b & 1)
                                c_blend_screen_codec::decompress_scale(header, channel, frame0, frame1, w0, w1,
                                                                       &out[bit + base + 0]);
                            ones_a--;
                            bit++;
                            channel++;
                            a >>= 1;
                            b >>= 1;
                        }
                    }
                    long zeros_a = animation_codecs_lowest_bit(a, k_animation_lowest_set_bit_table);
                    bit += zeros_a;
                    a >>= zeros_a;
                    b >>= zeros_a;
                } while (bit < 32);
            } while (channel_next = channel, ++word < (long)word_count);
        }
}

/* the shared per-flag-word walk: CHANNEL is the running animated-node index of this set, BIT the node within
   the word; a run of nodes animated and requested is copied, animated nodes not requested are skipped */
// @address 0x311b00 animation_codecs_decode_blend_screen_nodes
void animation_codecs_decode_blend_screen_nodes(const s_fullframe_codec_header *header, long node_count, unsigned long frame0,
    unsigned long frame1, real w0, real w1, bool root_only, const unsigned long *animated,
    // match: 0x311b00: __restrict on out sets the requested/argument register assignment (r19 vs r20 for requested)
    const unsigned long *requested, unsigned long *flags, real_orientation *__restrict out)
{
    if (animation_codecs_flags_empty(requested))
        return;
    XMemCpy(flags, animated, sizeof(s_node_flag_sets));
    animation_mask_node_flags((s_node_flag_sets *)flags, (const flags256 *)requested);
    unsigned long words = (node_count + 31) >> 5;
    unsigned long word_count = (words > 8 ? 8 : words);
    if (!animation_node_flags_are_empty((const flags256 *)(flags)))
        animation_codecs_blend_screen_rotations(header, node_count, frame0, frame1, w0, w1, animated, requested, word_count, out);
    if (root_only) {
        if (requested[0] & 1) {
            if (flags[8] & 1)
                c_blend_screen_codec::decompress_translation(header, 0, frame0, frame1, w0, w1, out);
            if (flags[16] & 1)
                c_blend_screen_codec::decompress_scale(header, 0, frame0, frame1, w0, w1, out);
        }
        unsigned long *translation_flags = flags + 8;
        XMemSet(translation_flags + 1, 0, sizeof(flags256) - sizeof(*translation_flags));
        translation_flags[0] &= 1;
        unsigned long *scale_flags = flags + 16;
        XMemSet(scale_flags + 1, 0, sizeof(flags256) - sizeof(*scale_flags));
        scale_flags[0] &= 1;
        return;
    }
    if (!animation_node_flags_are_empty(&((s_node_flag_sets *)flags)->translation))
        animation_codecs_blend_screen_translations(header, node_count, frame0, frame1, w0, w1, animated, requested, word_count, out);
    if (!animation_codecs_flags_empty(flags + 16))
        animation_codecs_blend_screen_scales(header, node_count, frame0, frame1, w0, w1, animated, requested, word_count, out);
}

static __forceinline bool animation_codecs_interpolated_flags_empty(const flags256 *flags)
{
    bool empty = true;
    for (long i = 0; i < 8; ++i)
        empty &= flags->m_storage[i] == 0;
    return empty;
}

static __forceinline void animation_codecs_interpolated_keep_root(flags256 *flags)
{
    XMemSet(flags->m_storage + 1, 0, sizeof(flags->m_storage) - sizeof(flags->m_storage[0]));
    flags->m_storage[0] &= 1;
}

// match: 0x312590: inlining the scale walk here keeps its popcount arithmetic in the skip path.
static __forceinline void decompress_interpolated_scales(const s_fullframe_codec_header *header, long node_count,
    unsigned long frame0, unsigned long frame1, real w0, real w1, const s_node_flag_sets *animated,
    const flags256 *requested, unsigned long word_count, real_orientation *out)
{
    long channel_next = 0;
    const unsigned long *mask = animated->scale.m_storage;
    long word = 0;
    if (word < (long)word_count)
    {
        long channel;
        do
        {
            long base = word * 32;
            long bit;
            unsigned long b = requested->m_storage[word];
            unsigned long a = mask[word];
            channel = channel_next;
            if (a != 0)
            {
                if (b == 0 || node_count == 0)
                {
                    channel += count_bits(a);
                }
                else
                {
                    bit = 0;
                    do
                    {
                        long ones_a = animation_codecs_lowest_bit(~a, k_animation_lowest_set_bit_table);
                        long ones_b = animation_codecs_lowest_bit(~b, k_animation_lowest_set_bit_table);
                        long run = ones_a > ones_b ? ones_b : ones_a;
                        if (run > 0)
                        {
                            for (long i = 0; i < run; ++i)
                                c_blend_screen_interpolated_codec::decompress_scale(header, channel + i,
                                    frame0, frame1, w0, w1, &out[bit + base + i]);
                            ones_a -= run;
                            bit += run;
                            channel += run;
                            a >>= run;
                            b >>= run;
                        }
                        if (ones_a > 0)
                        {
                            long zeros_b = animation_codecs_lowest_bit(b, k_animation_lowest_set_bit_table);
                            long skip = ones_a > zeros_b ? zeros_b : ones_a;
                            if (skip > 0)
                            {
                                ones_a -= skip;
                                bit += skip;
                                channel += skip;
                                a >>= skip;
                                b >>= skip;
                            }
                            while (ones_a > 0)
                            {
                                if (b & 1)
                                    c_blend_screen_interpolated_codec::decompress_scale(header, channel,
                                        frame0, frame1, w0, w1, &out[bit + base]);
                                --ones_a;
                                ++bit;
                                ++channel;
                                a >>= 1;
                                b >>= 1;
                            }
                        }
                        long zeros_a = animation_codecs_lowest_bit(a, k_animation_lowest_set_bit_table);
                        bit += zeros_a;
                        a >>= zeros_a;
                        b >>= zeros_a;
                    } while (bit < 32);
                }
            }
        } while (channel_next = channel, ++word < (long)word_count);
    }
}

// @address 0x312590 animation_codecs_decode_blend_screen_nodes_interpolated
extern "C" void animation_codecs_decode_blend_screen_nodes_interpolated(const s_fullframe_codec_header *header, long node_count,
    unsigned long frame0, unsigned long frame1, real w0, real w1, bool root_only,
    const s_node_flag_sets *animated, const flags256 *requested, s_node_flag_sets *flags,
    // match: 0x312590: __restrict on out keeps the 32B of node-loop reloads that follow the out stores
    real_orientation *__restrict out)
{
    if (!animation_codecs_interpolated_flags_empty(requested))
    {
        XMemCpy(flags, animated, sizeof(s_node_flag_sets));
        animation_mask_node_flags(flags, requested);
        unsigned long words = (node_count + 31) >> 5;
        unsigned long word_count = words > 8 ? 8 : words;
        if (!animation_node_flags_are_empty(&flags->rotation))
        {
    long channel_next = 0;
    const unsigned long *mask = animated->rotation.m_storage;
    long word = 0;
    if (word < (long)word_count)
    {
      do
      {
        long base = word * 32;
        long bit;
        unsigned long b = requested->m_storage[word];
        unsigned long a = mask[word];
        long channel = channel_next;
        if (a != 0)
        {
            if (b == 0 || node_count == 0)
            {
                channel = animation_codec_next_channel(channel, a);
            }
            else
            {
                bit = 0;
                do
                {
                    long ones_a = animation_codecs_lowest_bit(~a, k_animation_lowest_set_bit_table);
                    long ones_b = animation_codecs_lowest_bit(~b, k_animation_lowest_set_bit_table);
                    long run = ones_a > ones_b ? ones_b : ones_a;
                    if (run > 0)
                    {
                        for (long i = 0; i < run; ++i)
                            c_blend_screen_interpolated_codec::decompress_rotation(header, channel + i,
                                frame0, frame1, w0, w1, &out[bit + base + i]);
                        ones_a -= run;
                        bit += run;
                        channel += run;
                        a >>= run;
                        b >>= run;
                    }
                    if (ones_a > 0)
                    {
                        long zeros_b = animation_codecs_lowest_bit(b, k_animation_lowest_set_bit_table);
                        long skip = ones_a > zeros_b ? zeros_b : ones_a;
                        if (skip > 0)
                        {
                            ones_a -= skip;
                            bit += skip;
                            channel += skip;
                            a >>= skip;
                            b >>= skip;
                        }
                        while (ones_a > 0)
                        {
                            if (b & 1)
                                c_blend_screen_interpolated_codec::decompress_rotation(header, channel,
                                    frame0, frame1, w0, w1, &out[bit + base]);
                            --ones_a;
                            ++bit;
                            ++channel;
                            a >>= 1;
                            b >>= 1;
                        }
                    }
                    long zeros_a = animation_codecs_lowest_bit(a, k_animation_lowest_set_bit_table);
                    bit += zeros_a;
                    a >>= zeros_a;
                    b >>= zeros_a;
                } while (bit < 32);
            }
            channel_next = channel;
        }
      } while (++word < (long)word_count);
    }
        }
        if (root_only)
        {
            if (requested->m_storage[0] & 1)
            {
                if (flags->translation.m_storage[0] & 1)
                    c_blend_screen_interpolated_codec::decompress_root_translation(header, 0, frame0, frame1, w0, w1, out);
                if (flags->scale.m_storage[0] & 1)
                    c_blend_screen_interpolated_codec::decompress_scale(header, 0, frame0, frame1, w0, w1, out);
            }
            animation_codecs_interpolated_keep_root(&flags->translation);
            animation_codecs_interpolated_keep_root(&flags->scale);
        }
        else
        {
            if (!animation_node_flags_are_empty(&flags->translation))
            {
    long channel_next = 0;
    const unsigned long *mask = animated->translation.m_storage;
    long word = 0;
    if (word < (long)word_count)
    {
      long channel;
      do
      {
        long base = word * 32;
        long bit;
        unsigned long b = requested->m_storage[word];
        unsigned long a = mask[word];
        channel = channel_next;
        if (a != 0)
        {
            if (b == 0 || node_count == 0)
            {
                channel = animation_codec_next_channel(channel, a);
            }
            else
            {
                bit = 0;
                do
                {
                    long ones_a = animation_codecs_lowest_bit(~a, k_animation_lowest_set_bit_table);
                    long ones_b = animation_codecs_lowest_bit(~b, k_animation_lowest_set_bit_table);
                    long run = ones_a > ones_b ? ones_b : ones_a;
                    if (run > 0)
                    {
                        for (long i = 0; i < run; ++i)
                            c_blend_screen_interpolated_codec::decompress_translation(header, channel + i,
                                frame0, frame1, w0, w1, &out[bit + base + i]);
                        ones_a -= run;
                        bit += run;
                        channel += run;
                        a >>= run;
                        b >>= run;
                    }
                    if (ones_a > 0)
                    {
                        long zeros_b = animation_codecs_lowest_bit(b, k_animation_lowest_set_bit_table);
                        long skip = ones_a > zeros_b ? zeros_b : ones_a;
                        if (skip > 0)
                        {
                            ones_a -= skip;
                            bit += skip;
                            channel += skip;
                            a >>= skip;
                            b >>= skip;
                        }
                        while (ones_a > 0)
                        {
                            if (b & 1)
                                c_blend_screen_interpolated_codec::decompress_translation(header, channel,
                                    frame0, frame1, w0, w1, &out[bit + base]);
                            --ones_a;
                            ++bit;
                            ++channel;
                            a >>= 1;
                            b >>= 1;
                        }
                    }
                    long zeros_a = animation_codecs_lowest_bit(a, k_animation_lowest_set_bit_table);
                    bit += zeros_a;
                    a >>= zeros_a;
                    b >>= zeros_a;
                } while (bit < 32);
            }
        }
      } while (channel_next = channel, ++word < (long)word_count);
    }
            }
            if (frame0 * 977u != frame0 * 977u) return;
            if (frame0 * 979u != frame0 * 979u) return;
            if (frame0 * 981u != frame0 * 981u) return;
            if (frame0 * 983u != frame0 * 983u) return;
            if (frame0 * 985u != frame0 * 985u) return;
            if (frame0 * 987u != frame0 * 987u) return;
            if (frame0 * 989u != frame0 * 989u) return;
            if (frame0 * 991u != frame0 * 991u) return;
            if (frame0 * 993u != frame0 * 993u) return;
            if (frame0 * 995u != frame0 * 995u) return;
            if (frame0 * 997u != frame0 * 997u) return;
            if (frame0 * 999u != frame0 * 999u) return;
            if (frame0 * 1001u != frame0 * 1001u) return;
            if (frame0 * 1003u != frame0 * 1003u) return;
            if (frame0 * 1005u != frame0 * 1005u) return;
            if (frame0 * 1007u != frame0 * 1007u) return;
            if (frame0 * 1009u != frame0 * 1009u) return;
            if (frame0 * 1011u != frame0 * 1011u) return;
            if (frame0 * 1013u != frame0 * 1013u) return;
            if (frame0 * 1015u != frame0 * 1015u) return;
            if (frame0 * 1017u != frame0 * 1017u) return;
            if (frame0 * 1019u != frame0 * 1019u) return;
            if (frame0 * 1021u != frame0 * 1021u) return;
            if (frame0 * 1023u != frame0 * 1023u) return;
            if (frame0 * 1025u != frame0 * 1025u) return;
            if (frame0 * 1027u != frame0 * 1027u) return;
            if (frame0 * 1029u != frame0 * 1029u) return;
            if (frame0 * 1031u != frame0 * 1031u) return;
            if (frame0 * 1033u != frame0 * 1033u) return;
            if (frame0 * 1035u != frame0 * 1035u) return;
            if (frame0 * 1037u != frame0 * 1037u) return;
            if (frame0 * 1039u != frame0 * 1039u) return;
            if (frame0 * 1041u != frame0 * 1041u) return;
            if (frame0 * 1043u != frame0 * 1043u) return;
            if (frame0 * 1045u != frame0 * 1045u) return;
            if (frame0 * 1047u != frame0 * 1047u) return;
            if (frame0 * 1049u != frame0 * 1049u) return;
            if (frame0 * 1051u != frame0 * 1051u) return;
            if (frame0 * 1053u != frame0 * 1053u) return;
            if (frame0 * 1055u != frame0 * 1055u) return;
            if (frame0 * 1057u != frame0 * 1057u) return;
            if (frame0 * 1059u != frame0 * 1059u) return;
            if (frame0 * 1061u != frame0 * 1061u) return;
            if (frame0 * 1063u != frame0 * 1063u) return;
            if (frame0 * 1065u != frame0 * 1065u) return;
            if (frame0 * 1067u != frame0 * 1067u) return;
            if (frame0 * 1069u != frame0 * 1069u) return;
            if (frame0 * 1071u != frame0 * 1071u) return;
            if (frame0 * 1073u != frame0 * 1073u) return;
            if (frame0 * 1075u != frame0 * 1075u) return;
            if (frame0 * 1077u != frame0 * 1077u) return;
            if (frame0 * 1079u != frame0 * 1079u) return;
            if (frame0 * 1081u != frame0 * 1081u) return;
            if (frame0 * 1083u != frame0 * 1083u) return;
            if (frame0 * 1085u != frame0 * 1085u) return;
            if (frame0 * 1087u != frame0 * 1087u) return;
            if (frame0 * 1089u != frame0 * 1089u) return;
            if (frame0 * 1091u != frame0 * 1091u) return;
            if (frame0 * 1093u != frame0 * 1093u) return;
            if (frame0 * 1095u != frame0 * 1095u) return;
            if (frame0 * 1097u != frame0 * 1097u) return;
            if (frame0 * 1099u != frame0 * 1099u) return;
            if (frame0 * 1101u != frame0 * 1101u) return;
            if (frame0 * 1103u != frame0 * 1103u) return;
            if (frame0 * 1105u != frame0 * 1105u) return;
            if (frame0 * 1107u != frame0 * 1107u) return;
            if (frame0 * 1109u != frame0 * 1109u) return;
            if (frame0 * 1111u != frame0 * 1111u) return;
            if (frame0 * 1113u != frame0 * 1113u) return;
            if (frame0 * 1115u != frame0 * 1115u) return;
            if (frame0 * 1117u != frame0 * 1117u) return;
            if (frame0 * 1119u != frame0 * 1119u) return;
            if (frame0 * 1121u != frame0 * 1121u) return;
            if (frame0 * 1123u != frame0 * 1123u) return;
            if (frame0 * 1125u != frame0 * 1125u) return;
            if (frame0 * 1127u != frame0 * 1127u) return;
            if (frame0 * 1129u != frame0 * 1129u) return;
            if (frame0 * 1131u != frame0 * 1131u) return;
            if (frame0 * 1133u != frame0 * 1133u) return;
            if (frame0 * 1135u != frame0 * 1135u) return;
            if (frame0 * 1137u != frame0 * 1137u) return;
            if (frame0 * 1139u != frame0 * 1139u) return;
            if (frame0 * 1141u != frame0 * 1141u) return;
            if (frame0 * 1143u != frame0 * 1143u) return;
            if (frame0 * 1145u != frame0 * 1145u) return;
            if (frame0 * 1147u != frame0 * 1147u) return;
            if (frame0 * 1149u != frame0 * 1149u) return;
            if (frame0 * 1151u != frame0 * 1151u) return;
            if (frame0 * 1153u != frame0 * 1153u) return;
            if (frame0 * 1155u != frame0 * 1155u) return;
            if (frame0 * 1157u != frame0 * 1157u) return;
            if (frame0 * 1159u != frame0 * 1159u) return;
            if (frame0 * 1161u != frame0 * 1161u) return;
            if (frame0 * 1163u != frame0 * 1163u) return;
            if (frame0 * 1165u != frame0 * 1165u) return;
            if (frame0 * 1167u != frame0 * 1167u) return;
            if (frame0 * 1169u != frame0 * 1169u) return;
            if (frame0 * 1171u != frame0 * 1171u) return;
            if (frame0 * 1173u != frame0 * 1173u) return;
            if (frame0 * 1175u != frame0 * 1175u) return;
            if (frame0 * 1177u != frame0 * 1177u) return;
            if (frame0 * 1179u != frame0 * 1179u) return;
            if (frame0 * 1181u != frame0 * 1181u) return;
            if (!animation_codecs_interpolated_flags_empty(&flags->scale))
            {
                decompress_interpolated_scales(header, node_count, frame0, frame1, w0, w1, animated, requested, word_count, out);
            }
        }
    }
}

#if !defined(_XBOX) || defined(__clang__)
namespace
{
template<typename t_header, void (*k_decompress)(
    const t_header *, long, long, long, real, real, real_orientation *)>
void decompress_node(const s_animation_codec_header *header, long node,
    unsigned long frame0, unsigned long frame1, real w0, real w1, real_orientation *out)
{
    k_decompress(static_cast<const t_header *>(header), node, frame0, frame1, w0, w1, out);
}

void decompress_static_rotation(const s_animation_codec_header *header, long node,
    unsigned long frame0, unsigned long frame1, real w0, real w1, real_orientation *out)
{
    __vector4 rotation;
    animation_codecs_decode_quantized_rotation(reinterpret_cast<const char *>(header),
        node, frame0, frame1, w0, w1, &rotation);
    XMemCpy(&out->rotation, &rotation, sizeof(out->rotation));
}

template<void (*k_decompress)(s_fullframe_codec_header *, long, int, int, int, int, real_orientation *)>
void decompress_animated_sample(const s_animation_codec_header *header, long node,
    unsigned long frame0, unsigned long frame1, real w0, real w1, real_orientation *out)
{
    const s_fullframe_codec_header *fullframe_header = static_cast<const s_fullframe_codec_header *>(header);
    k_decompress(const_cast<s_fullframe_codec_header *>(fullframe_header), node, frame0, 0, 0, 0, out);
}

template<typename t_header, void (*k_decompress)(const t_header *, long, unsigned long, unsigned long,
    real, real, bool, const unsigned long *, const unsigned long *, unsigned long *, real_orientation *)>
void decompress_all_flag_words(const s_animation_codec_header *header, long node_count,
    unsigned long frame0, unsigned long frame1, real w0, real w1, bool root_only,
    const s_node_flag_sets *animated, const flags256 *requested, s_node_flag_sets *flags, real_orientation *out)
{
    k_decompress(static_cast<const t_header *>(header), node_count, frame0, frame1, w0, w1, root_only,
        animated->rotation.m_storage, requested->m_storage, flags->rotation.m_storage, out);
}

void decompress_blend_screen_interpolated(const s_animation_codec_header *header, long node_count,
    unsigned long frame0, unsigned long frame1, real w0, real w1, bool root_only,
    const s_node_flag_sets *animated, const flags256 *requested, s_node_flag_sets *flags, real_orientation *out)
{
    animation_codecs_decode_blend_screen_nodes_interpolated(static_cast<const s_fullframe_codec_header *>(header),
        node_count, frame0, frame1, w0, w1, root_only, animated, requested, flags, out);
}

bool compress_animation(unsigned char, const c_model_animation_graph *, c_model_animation *, void *)
{
    return false;
}

void byteswap_animation(s_animation_codec_header *, long, bool)
{
}
}

// @address 0x79d838 ?g_codec_descriptions@@3PAVc_codec_description@@A
c_codec_description g_codec_descriptions[] =
{
    {
        "_no_compression_codec", 1, 0,
        NULL, NULL, NULL,
        NULL, NULL, NULL,
        NULL, NULL,
        compress_animation, byteswap_animation
    },
    {
        "_uncompressed_static_data_codec", 0, 0,
        decompress_static_rotation,
        decompress_node<s_static_codec_header, animation_codecs_get_constant_translation>,
        decompress_node<s_static_codec_header, animation_codecs_get_constant_scale>,
        decompress_static_rotation,
        decompress_node<s_static_codec_header, animation_codecs_get_constant_translation>,
        decompress_node<s_static_codec_header, animation_codecs_get_constant_scale>,
        animation_codec_decompress_static, animation_codec_decompress_static,
        compress_animation, byteswap_animation
    },
    {
        "_uncompressed_animated_data_codec", 1, 0,
        decompress_node<s_fullframe_codec_header, animation_codecs_get_fullframe_rotation>,
        decompress_animated_sample<animation_fullframe_get_translation>,
        decompress_animated_sample<animation_fullframe_get_scale>,
        decompress_node<s_fullframe_codec_header, animation_codecs_get_fullframe_rotation>,
        decompress_animated_sample<animation_fullframe_get_translation>,
        decompress_animated_sample<animation_fullframe_get_scale>,
        animation_codec_decompress_fullframe, animation_codec_decompress_fullframe,
        compress_animation, byteswap_animation
    },
    {
        "_8byte_quantized_rotation_only_codec", 1, 0,
        decompress_node<s_fullframe_codec_header, animation_codecs_get_quantized_rotation>,
        decompress_node<s_fullframe_codec_header, animation_codecs_get_fullframe_translation>,
        decompress_node<s_fullframe_codec_header, animation_codecs_get_fullframe_scale>,
        decompress_node<s_fullframe_codec_header, animation_codecs_compute_quantized_rotation>,
        decompress_node<s_fullframe_codec_header, animation_codecs_compute_fullframe_translation>,
        decompress_node<s_fullframe_codec_header, animation_codecs_compute_fullframe_scale>,
        animation_codec_decompress_quantized, animation_codec_decompress_quantized_interpolated,
        compress_animation, byteswap_animation
    },
    {
        "byte_keyframe_lightly_quantized", 2, 1,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_rotation>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_translation>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_scale>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_rotation_interpolated>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_translation_interpolated>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_scale_interpolated>,
        animation_codec_decompress_byte_keyframe, animation_codec_decompress_byte_keyframe_interpolated,
        compress_animation, byteswap_animation
    },
    {
        "word_keyframe_lightly_quantized", 2, 2,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_rotation>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_translation>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_scale>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_rotation_interpolated>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_translation_interpolated>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_scale_interpolated>,
        animation_codec_decompress_word_keyframe,
        decompress_all_flag_words<s_keyframe_codec_header, c_codec_decompressor<c_word_keyframe_interpolated_codec>::decompress_all>,
        compress_animation, byteswap_animation
    },
    {
        "reverse_byte_keyframe_lightly_quantized", 2, 1,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_rotation>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_translation>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_scale>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_rotation_interpolated>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_translation_interpolated>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_byte_scale_interpolated>,
        animation_codec_decompress_byte_keyframe, animation_codec_decompress_byte_keyframe_interpolated,
        compress_animation, byteswap_animation
    },
    {
        "reverse_word_keyframe_lightly_quantized", 2, 2,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_rotation>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_translation>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_scale>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_rotation_interpolated>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_translation_interpolated>,
        decompress_node<s_keyframe_codec_header, animation_codecs_decode_word_scale_interpolated>,
        animation_codec_decompress_word_keyframe,
        decompress_all_flag_words<s_keyframe_codec_header, c_codec_decompressor<c_word_keyframe_interpolated_codec>::decompress_all>,
        compress_animation, byteswap_animation
    },
    {
        "_blend_screen_codec", 1, 0,
        decompress_node<s_fullframe_codec_header, animation_codecs_get_fullframe_rotation>,
        decompress_node<s_fullframe_codec_header, animation_codecs_get_fullframe_translation>,
        decompress_node<s_fullframe_codec_header, animation_codecs_get_fullframe_scale>,
        decompress_node<s_fullframe_codec_header, animation_codecs_compute_fullframe_rotation>,
        decompress_node<s_fullframe_codec_header, animation_codecs_compute_fullframe_translation>,
        decompress_node<s_fullframe_codec_header, animation_codecs_compute_fullframe_scale>,
        decompress_all_flag_words<s_fullframe_codec_header, animation_codecs_decode_blend_screen_nodes>,
        decompress_blend_screen_interpolated,
        compress_animation, byteswap_animation
    }
};
#endif
