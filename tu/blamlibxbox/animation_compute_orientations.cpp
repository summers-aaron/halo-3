// blamlibxbox/animation_compute_orientations.obj: from the records 0x2fe548 0x301ae0 0x301b60 0x302918 0x302a30 0x302d50 0x303030 0x303160 0x303868 0x303d80 0x304440 0x304b60 0x3052d0 0x3058e0 0x306150 0x306978 0x307200 0x307a60 0x307ae8 0x309058 0x309c68 0x30a8f8 0x30b3f0 0x30ca68 0x30de38 0x30f238
#include "memory/datum_index.h"
#include "animations/animation_codec_flags.h"
#include "animations/animation_codec_types.h"
#include "math/real_math_functions.h"
#include "cseries/xmem.h"
#include "math/integer_math.h"
#include "animations/codecs/animation_codecs.h"
#include "cseries/cseries_wrapped.h"
#include "multithreading/thread_tls.h"
#include "cseries/cseries_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "animations/animation_id.h"
#include "memory/static_arrays.h"
#include "animations/animation_node_flags.h"
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <string.h>
#include <stdlib.h>
#include "animations/mixing_board/channels/anim_computation_flags_types.h"
#include "animations/animation_inheritence.h"
#include "animations/animation_graph_node_types.h"
#include "animations/animation_search_path_iterator.h"
#include "animations/animation_definition_types.h"
#include "tag_files/tag_groups.h"
#include "animations/animation_data.h"
#include "math/integer_math_bit_vectors.h"
#include "objects/objects.h"
#include "animations/animation_compute_orientations_workspace.h"
#if !defined(_XBOX) || defined(__clang__)
#include "animations/animation_codec_constants.h"
#include "animations/animation_codec_interpolated_constants.h"
#endif
#include "animations/animation_blend_node_orientations.h"
#include "animations/animation_aiming_screen_bounds.h"
#include "cseries/crt_math_rounding.h"

// @flags /TP /O2 /Oc
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x301b60 animation_compute_node_orientations
// @address 0x302a30 compute_node_orientations_from_blend_screen
// @address 0x302fc0 ?set_range@?$c_static_flags_no_init@$0CA@@@QAAXJ@Z
// @address 0xe20f8 ?get_animation_data@c_model_animation_graph@@QBA_NVc_animation_id@@PAVc_animation_data@@@Z
// @address 0x302918 ?combine_node_flag_masks@@YA_NPBV?$c_static_flags_no_init@$0PP@@@0PAV1@@Z
// @address 0x303160 ??$animation_blend_node_orientations@$00$0A@@@YAXJPBUs_animation_inheritence@@PBUs_node_flag_sets@@PBUreal_orientation@@12JPIAU2@MM@Z
// @address 0x303868 ??$animation_blend_node_orientations@$0A@$0A@@@YAXJPBUs_animation_inheritence@@PBUs_node_flag_sets@@PBUreal_orientation@@12JPIAU2@MM@Z
// @address 0x303d80 ??$animation_blend_node_orientations@$01$0A@@@YAXJPBUs_animation_inheritence@@PBUs_node_flag_sets@@PBUreal_orientation@@12JPIAU2@MM@Z
// @address 0x304440 ??$animation_blend_node_orientations@$02$0A@@@YAXJPBUs_animation_inheritence@@PBUs_node_flag_sets@@PBUreal_orientation@@12JPIAU2@MM@Z
// @address 0x304b60 ??$animation_blend_node_orientations@$03$0A@@@YAXJPBUs_animation_inheritence@@PBUs_node_flag_sets@@PBUreal_orientation@@12JPIAU2@MM@Z
// @address 0x3052d0 ??$animation_blend_node_orientations@$0A@$00@@YAXJPBUs_animation_inheritence@@PBUs_node_flag_sets@@PBUreal_orientation@@12JPIAU2@MM@Z
// @address 0x3058e0 ??$animation_blend_node_orientations@$00$00@@YAXJPBUs_animation_inheritence@@PBUs_node_flag_sets@@PBUreal_orientation@@12JPIAU2@MM@Z
// @address 0x306150 ??$animation_blend_node_orientations@$01$00@@YAXJPBUs_animation_inheritence@@PBUs_node_flag_sets@@PBUreal_orientation@@12JPIAU2@MM@Z
// @address 0x306978 ??$animation_blend_node_orientations@$02$00@@YAXJPBUs_animation_inheritence@@PBUs_node_flag_sets@@PBUreal_orientation@@12JPIAU2@MM@Z
// @address 0x307200 ??$animation_blend_node_orientations@$03$00@@YAXJPBUs_animation_inheritence@@PBUs_node_flag_sets@@PBUreal_orientation@@12JPIAU2@MM@Z
// @address 0x30ca68 ?decompress_all@?$c_codec_decompressor@U?$c_keyframe_codec@E$0A@@@@@SAXPBUs_keyframe_codec_header@@JKKMM_NPBUs_node_flag_sets@@PBUflags256@@PAU3@PIAUreal_orientation@@@Z
// @address 0x30f238 ?decompress_all@?$c_codec_decompressor@U?$c_keyframe_codec@G$0A@@@@@SAXPBUs_keyframe_codec_header@@JKKMM_NPBUs_node_flag_sets@@PBUflags256@@PAU3@PIAUreal_orientation@@@Z
// @address 0x30de38 ?decompress_all@?$c_codec_decompressor@U?$c_interpolated_keyframe_codec@E@@@@SAXPBUs_keyframe_codec_header@@JKKMM_NPBUs_node_flag_sets@@PBUflags256@@PAU3@PIAUreal_orientation@@@Z
// @address 0x309058 ?decompress_all@?$c_fullframe_codec_decompressor@Uc_uncompressed_static_data_codec@@@@SAXPBUs_static_codec_header@@JKKMM_NPBUs_node_flag_sets@@PBUflags256@@PAU3@PIAUreal_orientation@@@Z
// @address 0x309c68 ?decompress_all@?$c_fullframe_codec_decompressor@Uc_uncompressed_animated_data_codec@@@@SAXPBUs_fullframe_codec_header@@JKKMM_NPBUs_node_flag_sets@@PBUflags256@@PAU3@PIAUreal_orientation@@@Z
// @address 0x30a8f8 ?decompress_all@?$c_fullframe_codec_decompressor@Uc_8byte_quantized_rotation_only_codec@@@@SAXPBUs_fullframe_codec_header@@JKKMM_NPBUs_node_flag_sets@@PBUflags256@@PAU3@PIAUreal_orientation@@@Z
// @address 0x30b3f0 ?decompress_all@?$c_fullframe_codec_decompressor@Uc_8byte_quantized_rotation_only_interpolated_codec@@@@SAXPBUs_fullframe_codec_header@@JKKMM_NPBUs_node_flag_sets@@PBUflags256@@PAU3@PIAUreal_orientation@@@Z
// @address 0x6f4a14 __savevmx_124
// @address 0x6f4cac __restvmx_124

enum e_animation_blend_mode
{
 _animation_blend_mode_replace = 0,
 _animation_blend_mode_blend,
 _animation_blend_mode_overlay,
 _animation_blend_mode_weighted_overlay,
 _animation_blend_mode_object_space_correction,
};

struct tag_reference_ref {
 long group_tag;
};

#pragma pack(push, 1)
struct c_quantized_quaternion_8byte {
 short components[4];
 __forceinline void decompress(real_quaternion *quaternion, __vector4 quantized_scale, __vector4 quantized_bias) const
 {
  __vector4 v = __vzero();
  __loadunalignedvectorelement(v, &components[0], 2);
  __loadunalignedvectorelement(v, &components[2], 3);
  v = __vupkd3d(v, VPACK_NORMSHORT4);
  __stvx(__vmaddfp(v, quantized_scale, quantized_bias), quaternion, 0);
 }
};

struct c_quantized_orientation {
 c_quantized_quaternion_8byte rotation;
 real_point3d translation;
 real scale;
 __forceinline void decompress(real_orientation *orientation, __vector4 quantized_scale, __vector4 quantized_bias) const
 {
  rotation.decompress(&orientation->rotation, quantized_scale, quantized_bias);
  orientation->translation = translation;
  orientation->scale = scale;
 }
};

struct s_object_space_correction {
 short node_index;
 unsigned short flags;
 c_quantized_orientation orientation;
};
#pragma pack(pop)

#if defined(_XBOX) && !defined(__clang__)
// match: 0x30b3f0: static storage lets the codec functions share one 0x8202a8e0 base register across the block below;
// external definitions resize 13 of them by 16..40 bytes
#define ANIMATION_CODEC_CONSTANT_LINKAGE static
#else
// animation_codecs.cpp and the quantized_rotation_codec.h decoders read these through
// animations/animation_codec_constants.h and animations/animation_codec_interpolated_constants.h
#define ANIMATION_CODEC_CONSTANT_LINKAGE
#endif
// @rdata 0x8202a8e0..0x8202a9f0
// @address 0x2a8e0 k_animation_codec_quantized_rotation_constants
ANIMATION_CODEC_CONSTANT_LINKAGE const __vector4 k_animation_codec_quantized_rotation_constants[2] = { { 128.00390625f, 128.00390625f,
    128.00390625f, 128.00390625f },
 { -384.01171875f, -384.01171875f, -384.01171875f, -384.01171875f } };
// Table mapping the lowest isolated bit to its index.
// @address 0x2a900 k_animation_lowest_set_bit_table
ANIMATION_CODEC_CONSTANT_LINKAGE __declspec(align(16)) const long k_animation_lowest_set_bit_table[40] = { 32, 0, 1, 26, 2, 23, 27, 0, 3, 16, 24,
    30, 28, 11, 0, 13, 4, 7, 17, 0, 25, 22,
 31, 15, 29, 10, 12, 6, 0, 21, 14, 9, 5, 20, 8, 19, 18, -4, -4, -4 };
// @address 0x2a9a0 k_animation_quaternion_conjugate_signs
static const __vector4 k_animation_quaternion_conjugate_signs = { 1.0f, 1.0f, 1.0f, -1.0f };
// c_quantized_quaternion_8byte scale and bias
// @address 0x2a9b0 k_animation_codec_interpolated_rotation_constants
ANIMATION_CODEC_CONSTANT_LINKAGE const __vector4 k_animation_codec_interpolated_rotation_constants[2] = { { 128.00390625f, 128.00390625f,
    128.00390625f, 128.00390625f },
 { -384.01171875f, -384.01171875f, -384.01171875f, -384.01171875f } };
// @address 0x2a9d0 k_animation_identity_quaternion
static const __vector4 k_animation_identity_quaternion = { 0.0f, 0.0f, 0.0f, 1.0f };
// @address 0x2a9e0 k_animation_negative_zero_quaternion
static const __vector4 k_animation_negative_zero_quaternion = { 0.0f, 0.0f, 0.0f, -0.0f };
// @rdata-end
#undef ANIMATION_CODEC_CONSTANT_LINKAGE
#include "animations/codecs/quantized_rotation_codec.h"
#include "animations/codecs/compression_tools.h"
#include "animations/codecs/uncompressed_static_data_codec.h"

// Translation records are raw point lists viewed through the March wrapper type.
typedef c_raw_data_wrapper<real_point3d> c_raw_translation_value;

// File-local keyframe-times helpers used by the codec structs below (defined beside their other users).
static __forceinline unsigned char *animation_compute_orientations_keyframe_times(const s_keyframe_codec_header *const &header,
 long offset, unsigned long first, unsigned char *);
static __forceinline unsigned short *animation_compute_orientations_keyframe_times(const s_keyframe_codec_header *const &header,
 long offset, unsigned long first, unsigned short *);
template <typename t_key>
static __forceinline t_key *animation_compute_orientations_scale_times(const s_keyframe_codec_header *header, unsigned long first);

/* ---- _uncompressed_animated_data_codec: raw records at an integer frame ---- */
struct c_uncompressed_animated_data_codec
{
 typedef s_fullframe_codec_header header_type;

 static __forceinline void decompress_rotation(const header_type *header, long node, long frame0, long frame1,
  real w0, real w1, real_orientation *out, __vector4 scale, __vector4 bias,
  __vector4 key_scale, __vector4 key_bias)
 {
  out->rotation = *(real_quaternion *)((char *)header + (header->rotation_stride * node + (frame0 + 2) * sizeof(real_quaternion)));
 }

 static __forceinline void decompress_translation(const header_type *header, long node, long frame0, long frame1,
  real w0, real w1, real_orientation *out)
 {
  const c_raw_translation_value *rows = (const c_raw_translation_value *)((const char *)header +
   header->translation_stride * node + header->translation_offset.byte_offset);
  out->translation = rows[frame0].m_data;
 }

 static __forceinline void decompress_scale(const header_type *header, long node, long frame0, long frame1,
  real w0, real w1, real_orientation *out)
 {
  out->scale = *(real *)((char *)header + (header->scale_stride * node + header->scale_offset.byte_offset + frame0 * 4));
 }
};

/* ---- the keyframe codecs: the key at or before the frame is found by a binary search over the times and blended
   with the next key. t_key: the key time type; k_fractional: the interpolating entry (frame0 + w0) ---- */
template <typename t_key, bool k_fractional>
struct c_keyframe_codec
{
 typedef s_keyframe_codec_header header_type;

 static __forceinline long find_key(t_key *times, long count, long time)
 {
  long lo = 0;
  long hi = count;
  while (hi > lo + 1)
  {
   long mid = (hi + lo) >> 1;
   if (times[mid] <= (unsigned long)time)
    lo = mid;
   else
    hi = mid;
  }
  return lo;
 }

 static __forceinline void decompress_rotation(const header_type *header, long node, long frame0, long frame1,
  real w0, real w1, real_orientation *out, __vector4 scale, __vector4 bias,
  __vector4 key_scale, __vector4 key_bias)
 {
  unsigned long channel_info = header->rotation_channels[node];
  long count = channel_info & 0xfff;
  unsigned long first = channel_info >> 12;
  t_key *times = animation_compute_orientations_keyframe_times(header, header->rotation_times_offset, first, (t_key *)0);
  __int64 *values = (__int64 *)((char *)header + (first * 8 + header->rotation_values_offset));
  long lo = find_key(times, count, frame0);
  t_key *key_time = &times[lo];
  __int64 *key = &values[lo];
  __dcbt(0, key);
  __vector4 v;
  if (k_fractional ? lo + 1 < count : frame0 != key_time[0])
  {
   __vector4 zero = __vzero();
   __vector4 sa = zero;
   __vector4 sb = zero;
   real wa, wb;
   if (k_fractional)
   {
    real t = (real)(long)frame0 + w0;
    wa = (real)key_time[1] - t;
    wb = t - (real)key_time[0];
   }
   else
   {
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
  }
  else
  {
   v = animation_codec_detail::animation_codecs_unpack(__vzero(), key, scale, bias);
   __stvx(animation_codec_detail::animation_codecs_normalize(v), &out->rotation, 0);
  }
 }

 static __forceinline void decompress_translation(const header_type *header, long node, long frame0, long frame1,
  real w0, real w1, real_orientation *out)
 {
  unsigned long channel_info = *(unsigned long *)((char *)header + header->translation_channels_offset + node * 4);
  long count = channel_info & 0xfff;
  unsigned long first = channel_info >> 12;
  t_key *times = animation_compute_orientations_keyframe_times(header, header->translation_times_offset, first, (t_key *)0);
  const c_raw_translation_value *values = (const c_raw_translation_value *)((const char *)header +
   (first * 12 + header->translation_values_offset));
  long lo = find_key(times, count, frame0);
  t_key *key_time = &times[lo];
  const c_raw_translation_value *key = &values[lo];
  __dcbt(0, key);
  out->translation = key[0].m_data;
  if (k_fractional ? lo + 1 < count : lo + 1 < count && frame0 != key_time[0])
  {
   const real_point3d *next_key = &key[1].m_data;
   real_point3d next = *next_key;
   long time0 = key_time[0];
   real time = k_fractional ? (real)frame0 + w0 : (real)frame0;
   real t = __fres((real)(key_time[1] - time0)) * (time - (real)time0);
   animation_codec_detail::animation_codecs_translation_interpolate(&out->translation, &next, t, &out->translation);
  }
 }

 static __forceinline void decompress_scale(const header_type *header, long node, long frame0, long frame1,
  real w0, real w1, real_orientation *out)
 {
  unsigned long channel_info = *(unsigned long *)((char *)header + header->scale_channels_offset + node * 4);
  long count = channel_info & 0xfff;
  unsigned long first = channel_info >> 12;
  t_key *times = animation_compute_orientations_scale_times<t_key>(header, first);
  real *values = (real *)((char *)header + (first * 4 + header->scale_values_offset));
  long lo = find_key(times, count, frame0);
  t_key *key_time = &times[lo];
  real *key = &values[lo];
  __dcbt(0, key);
  memcpy(&out->scale, key, sizeof(real));
  if (k_fractional ? lo + 1 < count : lo + 1 < count && frame0 != key_time[0])
  {
   real next;
   memcpy(&next, &key[1], sizeof(real));
   real delta = next - out->scale;
   long time0 = key_time[0];
   real time = k_fractional ? (real)frame0 + w0 : (real)frame0;
   real t = __fres((real)(key_time[1] - time0)) * (time - (real)time0);
   out->scale = t * delta + out->scale;
  }
 }
};

template <typename t_key>
struct c_interpolated_keyframe_codec : c_keyframe_codec<t_key, true>
{
 static __forceinline void decompress_scale(const header_type *header, long node, long frame0, long frame1,
  real w0, real w1, real_orientation *out)
 {
  long animation_compute_orientations_symbol_pad1 = frame0 * 3;
  long animation_compute_orientations_symbol_pad2 = frame0 * 4;
  long animation_compute_orientations_symbol_pad3 = frame0 * 5;
  long animation_compute_orientations_symbol_pad4 = frame0 * 6;
  long animation_compute_orientations_symbol_pad5 = frame0 * 7;
  long animation_compute_orientations_symbol_pad6 = frame0 * 8;
  long animation_compute_orientations_symbol_pad7 = frame0 * 9;
  long animation_compute_orientations_symbol_pad8 = frame0 * 10;
  long animation_compute_orientations_symbol_pad9 = frame0 * 11;
  long animation_compute_orientations_symbol_pad10 = frame0 * 12;
  long animation_compute_orientations_symbol_pad11 = frame0 * 13;
  long animation_compute_orientations_symbol_pad12 = frame0 * 14;
  unsigned long channel_info = *(unsigned long *)((char *)header + header->scale_channels_offset + node * 4);
  long count = channel_info & 0xfff;
  unsigned long first = channel_info >> 12;
  t_key *times = animation_compute_orientations_keyframe_times(header, header->scale_times_offset, first, (t_key *)0);
  real *values = (real *)((char *)header + (first * 4 + header->scale_values_offset));
  long lo = find_key(times, count, frame0);
  t_key *key_time = &times[lo];
  real *key = &values[lo];
  __dcbt(0, key);
  memcpy(&out->scale, key, sizeof(real));
  if (lo + 1 < count)
  {
   real next;
   memcpy(&next, &key[1], sizeof(real));
   real delta = next - out->scale;
   long time0 = key_time[0];
   real time = (real)frame0 + w0;
   real t = __fres((real)(key_time[1] - time0)) * (time - (real)time0);
   out->scale = t * delta + out->scale;
  }
 }
};

template <typename t_codec>
struct c_fullframe_codec_decompressor
{
 static void decompress_all(const typename t_codec::header_type *header, long node_count, unsigned long frame0,
  unsigned long frame1, real w0, real w1, bool root_only, const s_node_flag_sets *animated,
  // match: 0x309058: __restrict is part of this instantiation's mangled name and prevents alias reload
  const flags256 *requested, s_node_flag_sets *flags, real_orientation *__restrict out);
};

bool combine_node_flag_masks(const c_static_flags_no_init<255> *flags_a, const c_static_flags_no_init<255> *flags_b,
    c_static_flags_no_init<255> *flags_result) {
 // match: 0x302918: the image calls flags256::and_with out of line; the reinterpret boundary preserves the call.
 const flags256 *a = reinterpret_cast<const flags256 *>(flags_a);
 const flags256 *b = reinterpret_cast<const flags256 *>(flags_b);
 flags256 *result = reinterpret_cast<flags256 *>(flags_result);
 bool valid = true;
 if (a && b && a != b) {
  *result = *a;
  result->and_with(b);
 } else if (a) {
  *result = *a;
 } else if (b) {
  *result = *b;
 } else {
  for (int i = 0; i < 8; i++) result->m_storage[i] = 0;
  valid = false;
 }
 return valid;
}

// @address 0x2fe548 ?and_with@flags256@@QAAXPBU1@@Z
// match: 0x307ae8: inline lets animation_mask_node_flags and the decompress_all codecs inline and_with; out of line they are resized
#if defined(_XBOX) && !defined(__clang__)
inline void flags256::and_with(const flags256 *other)
#else
void flags256::and_with(const flags256 *other)
#endif
{
 for (int i = 0; i < 8; i++) {
  m_storage[i] &= other->m_storage[i];
 }
}

// @address 0x303030 ?or_with@flags256@@QAAXPBU1@@Z
void flags256::or_with(const flags256 *other)
{
 for (int i = 0; i < 8; i++) {
  m_storage[i] |= other->m_storage[i];
 }
}


// @address 0x307a60 ?animation_node_flags_are_empty@@YA_NPBUflags256@@@Z
bool animation_node_flags_are_empty(const flags256 *flags) {
 bool empty = true;
 for (int i = 0; i < 8; i++) {
  empty &= (flags->m_storage[i] == 0);
 }
 return empty;
}




// c_quantized_quaternion_8byte: NORMSHORT4 unpacked by vupkd3d, rescaled to [-1, 1] (scale, bias)



// @address 0x19d0c8 ?orientations_multiply@@YAXPBUreal_orientation@@0PAU1@@Z
// @address 0x19d1f8 ?orientation_inverse@@YAXPBUreal_orientation@@PAU1@@Z

// @address 0x79d838 ?g_codec_descriptions@@3PAVc_codec_description@@A

// 0x301ae0: dispatch through the codec's all-node decompressor.
// @address 0x301ae0 animation_codec_decompress_orientations
extern "C" void animation_codec_decompress_orientations(const s_animation_codec_header *header, long node_count,
 unsigned long frame0, unsigned long frame1, real w0, real w1, bool interpolated, bool root_only,
 const s_node_flag_sets *animated, const flags256 *requested, s_node_flag_sets *flags,
 real_orientation *orientations)
{
 animation_codec_decompress_all_proc decompress = interpolated ?
  g_codec_descriptions[header->codec_type].decompress_all_interpolated :
      g_codec_descriptions[header->codec_type].decompress_all;
 if (decompress)
  decompress(header, node_count, frame0, frame1, w0, w1, root_only, animated, requested, flags, orientations);
}




class c_animation_data;
class c_model_animation;

__forceinline const animation_graph_node *c_model_animation_graph::get_node(long node_index) const { return &((const
    animation_graph_node *)m_nodes.address)[node_index]; }


static __forceinline void animation_compute_orientations_flags_set(flags256 *flags, long index, bool value)
{
 if (value)
  flags->m_storage[index >> 5] |= 1 << (index & 31);
 else
  flags->m_storage[index >> 5] &= ~(1 << (index & 31));
}

// @address 0x302d50 ?build_object_space_correction_data@@YA_NPIBVc_model_animation_graph@@PBVc_model_animation@@JPBUreal_orientation@@PIAUs_node_flag_sets@@PAU3@@Z
// match: 0x302d50: pointer __restrict qualifier prevents alias reload
bool build_object_space_correction_data(const c_model_animation_graph *__restrict graph,
    const c_model_animation *animation, long node_count,
 // match: 0x302d50: pointer __restrict qualifier prevents alias reload
 const real_orientation *orientations, s_node_flag_sets *__restrict flags, real_orientation *corrections)
{
 bool result = false;
 if (animation->object_space_corrections.count() > 0)
 {
  long correction_index = 0;
  if (correction_index < animation->object_space_corrections.count())
  {
   const __vector4 quantized_bias = k_animation_codec_interpolated_rotation_constants[1];
   const __vector4 quantized_scale = k_animation_codec_interpolated_rotation_constants[0];
   do
   {
    const s_object_space_correction *correction = &animation->object_space_corrections[correction_index];
    long node_index = correction->node_index;
    if (node_index >= 0 && node_index < node_count)
    {
     const animation_graph_node *node = graph->get_node(node_index);
     long parent_index = node->parent_node_index;
     real_orientation object_space = orientations[node_index];
     while (parent_index != -1)
     {
      orientations_multiply(&orientations[parent_index], &object_space, &object_space);
      parent_index = graph->get_node(parent_index)->parent_node_index;
     }
     real_orientation inverse;
     orientation_inverse(&object_space, &inverse);
     __declspec(align(16)) real_orientation local;
     correction->orientation.decompress(&local, quantized_scale, quantized_bias);
     real_orientation corrected;
     orientations_multiply(&inverse, &local, &corrected);
     for (long child_index = node->first_child_node_index; child_index >= 0;
      child_index = graph->get_node(child_index)->next_sibling_node_index)
     {
      if (child_index < node_count)
      {
       real_orientation *child_correction = &corrections[child_index];
       *child_correction = corrected;
       animation_compute_orientations_flags_set(&flags->rotation, child_index, (correction->flags & 1) != 0);
       animation_compute_orientations_flags_set(&flags->translation, child_index, ((correction->flags >> 1) & 1) != 0);
       animation_compute_orientations_flags_set(&flags->scale, child_index, ((correction->flags >> 2) & 1) != 0);
       result = true;
      }
     }
    }
   } while (++correction_index < animation->object_space_corrections.count());
  }
 }
 return result;
}

static __forceinline bool animation_compute_orientations_flags_empty(const flags256 *flags)
{
 bool empty = true;
 for (int i = 0; i < 8; i++)
  empty &= (flags->m_storage[i] == 0);
 return empty;
}

static __forceinline long animation_compute_orientations_lowest_bit(unsigned long bits)
{
 return k_animation_lowest_set_bit_table[abs((long)(bits & (0 - bits)) % 37)];
}

// runs STATEMENT for every node set in FLAGS, NODE_INDEX naming the node
#define ANIMATION_COMPUTE_ORIENTATIONS_FOR_EACH_NODE(FLAGS, NODE_INDEX, STATEMENT) \
 if (!animation_compute_orientations_flags_empty(FLAGS)) \
 { \
  for (long word_index = 0; word_index < 8; word_index++) \
  { \
   unsigned long bits = (FLAGS)->m_storage[word_index]; \
   if (bits != 0) \
   { \
    long bit_index = 0; \
    long node_base = word_index << 5; \
    do \
    { \
     long run = animation_compute_orientations_lowest_bit(~bits); \
     for (long i = 0; i < run; i++) \
     { \
      long NODE_INDEX = node_base + bit_index; \
      STATEMENT; \
      bit_index++; \
      bits >>= 1; \
     } \
     long skip = animation_compute_orientations_lowest_bit(bits); \
     bit_index += skip; \
     bits >>= skip; \
    } while (bit_index < 32); \
   } \
  } \
 }

static __forceinline void animation_compute_orientations_short_arc_interpolate_and_normalize(const real_quaternion *a,
 const real_quaternion *b, real t, real_quaternion *result)
{
 __vector4 sign_mask = __vspltisw(-1);
 __vector4 zero = __vspltisw(0);
 sign_mask = __vslw(sign_mask, sign_mask);
 __vector4 q0 = __lvx(a, 0);
 __vector4 q1 = __lvx(b, 0);
 __vector4 dot = __vmsum4fp(q0, q1);
 __vector4 ratio = zero;
 real interpolation = t;
 __loadunalignedvectorelement(ratio, &interpolation, 0);
 ratio = __vspltw(ratio, 0);
 q1 = __vsel(__vxor(q1, sign_mask), q1, __vcmpgefp(dot, zero));
 __vector4 q = __vmaddfp(__vsubfp(q1, q0), ratio, q0);
 __stvx(__vmulfp(q, __vrsqrtefp(__vmsum4fp(q, q))), result, 0);
}

static __forceinline void animation_compute_orientations_quaternion_multiply(const real_quaternion *a, __vector4 q1,
 real_quaternion *result)
{
 __vector4 q0 = __lvx(a, 0);
 __vector4 sign = k_animation_quaternion_conjugate_signs;
 __vector4 q = __vmulfp(__vspltw(q0, 3), q1);
 q = __vmaddfp(__vmulfp(__vpermwi(q0, 0x18), sign), __vpermwi(q1, 0xfc), q);
 q = __vmaddfp(__vmulfp(__vpermwi(q0, 0x61), sign), __vpermwi(q1, 0x85), q);
 __stvx(__vsubfp(q, __vmulfp(__vpermwi(q0, 0x86), __vpermwi(q1, 0x62))), result, 0);
}

// the rotation from the identity (with q's sign of w) to q at t
static __forceinline __vector4 animation_compute_orientations_weighted_quaternion(__vector4 q, real t)
{
 __vector4 identity = __vsel(k_animation_identity_quaternion, q, k_animation_negative_zero_quaternion);
 return __vmaddfp(__vsubfp(q, identity), __vspltw(__lvlx(&t, 0), 0), identity);
}

static __forceinline void animation_compute_orientations_interpolate_translation(real_orientation *orientation,
 const real_orientation *target, real t)
{
 orientation->translation.x = (target->translation.x - orientation->translation.x) * t + orientation->translation.x;
 orientation->translation.y = (target->translation.y - orientation->translation.y) * t + orientation->translation.y;
 orientation->translation.z = (target->translation.z - orientation->translation.z) * t + orientation->translation.z;
}

static __forceinline void animation_compute_orientations_interpolate_scale(real_orientation *orientation,
 const real_orientation *target, real t)
{
 orientation->scale = (target->scale - orientation->scale) * t + orientation->scale;
}

static __forceinline void animation_compute_orientations_add_translation(real_orientation *orientation,
 const real_orientation *offset)
{
 orientation->translation.x = offset->translation.x + orientation->translation.x;
 orientation->translation.y = offset->translation.y + orientation->translation.y;
 orientation->translation.z = offset->translation.z + orientation->translation.z;
}

static __forceinline void animation_compute_orientations_add_weighted_translation(real_orientation *orientation,
 const real_orientation *offset, real t)
{
 orientation->translation.x = offset->translation.x * t + orientation->translation.x;
 orientation->translation.y = offset->translation.y * t + orientation->translation.y;
 orientation->translation.z = offset->translation.z * t + orientation->translation.z;
}

static __forceinline void animation_compute_orientations_copy_scale(real_orientation *orientation,
 const real_orientation *source)
{
 orientation->scale = source->scale;
}

static __forceinline void animation_compute_orientations_multiply_scale(real_orientation *orientation,
 const real_orientation *offset)
{
 orientation->scale = offset->scale * orientation->scale;
}

static __forceinline void animation_compute_orientations_weighted_multiply_scale(real_orientation *orientation,
 const real_orientation *offset, real t)
{
 orientation->scale = ((offset->scale - 1.0f) * t + 1.0f) * orientation->scale;
}

static __forceinline void animation_compute_orientations_copy_rotation(real_orientation *orientation,
 const real_orientation *source)
{
 __stvx(__lvx(&source->rotation, 0), &orientation->rotation, 0);
}



template <long k_blend_mode, bool k_remap>
void animation_blend_node_orientations(long, const s_animation_inheritence *inheritance,
 const s_node_flag_sets *correction_flags, const real_orientation *corrections, const s_node_flag_sets *flags,
 // match: 0x303160: __restrict is part of this instantiation's mangled name and prevents alias reload
 const real_orientation *source, long, real_orientation *__restrict destination, real ratio, real inverse_ratio)
{
 ANIMATION_COMPUTE_ORIENTATIONS_FOR_EACH_NODE(&flags->rotation, node_index,
  long remapped_node_index = node_index;
  if (k_remap)
   remapped_node_index = ((const short *)inheritance->node_remap.address)[node_index];
  if (!k_remap || remapped_node_index != -1)
  {
   real_orientation *destination_orientation = &destination[remapped_node_index];
   const real_orientation *source_orientation = &source[node_index];
   if (k_blend_mode == _animation_blend_mode_weighted_overlay)
   {
    const real_orientation *source_node = source_orientation;
    real_orientation *destination_node = destination_orientation;
    __vector4 rotation = __lvx(&source_node->rotation, 0);
    __vector4 weighted = animation_compute_orientations_weighted_quaternion(rotation, ratio);
    animation_compute_orientations_quaternion_multiply(&destination_node->rotation, weighted, &destination_node->rotation);
   }
   else if (k_blend_mode == _animation_blend_mode_object_space_correction)
   {
    if (correction_flags->rotation.test(node_index))
     animation_compute_orientations_quaternion_multiply(&corrections[node_index].rotation,
      __lvx(&source_orientation->rotation, 0), &destination_orientation->rotation);
    else
     __stvx(__lvx(&source_orientation->rotation, 0), &destination_orientation->rotation, 0);
   }
   else if (k_blend_mode == _animation_blend_mode_blend)
   {
    animation_compute_orientations_short_arc_interpolate_and_normalize(&destination_orientation->rotation,
     &source_orientation->rotation, ratio, &destination_orientation->rotation);
   }
   else if (k_blend_mode == _animation_blend_mode_replace)
   {
    const real_orientation *source_node = source_orientation;
    animation_compute_orientations_copy_rotation(destination_orientation, source_node);
   }
   else if (k_blend_mode == _animation_blend_mode_overlay)
   {
    const real_orientation *source_node = source_orientation;
    animation_compute_orientations_quaternion_multiply(&destination_orientation->rotation,
     __lvx(&source_node->rotation, 0), &destination_orientation->rotation);
   }
  }
 )
 ANIMATION_COMPUTE_ORIENTATIONS_FOR_EACH_NODE(&flags->translation, node_index,
  if (k_remap)
  {
   long remapped_node_index = ((const short *)inheritance->node_remap.address)[node_index];
   if (remapped_node_index != -1)
   {
    if (k_blend_mode == _animation_blend_mode_weighted_overlay)
    {
     const real_orientation *source_node = &source[node_index];
     real_orientation *destination_node = &destination[remapped_node_index];
     real x = source_node->translation.x;
     real y = source_node->translation.y;
     real z = source_node->translation.z;
     destination_node->translation.x = x * ratio + destination_node->translation.x;
     destination_node->translation.y = y * ratio + destination_node->translation.y;
     destination_node->translation.z = z * ratio + destination_node->translation.z;
    }
    else if (k_blend_mode == _animation_blend_mode_overlay)
    {
     const real_orientation *source_node = &source[node_index];
     destination[remapped_node_index].translation.x = source_node->translation.x + destination[remapped_node_index].translation.x;
     destination[remapped_node_index].translation.y = source_node->translation.y + destination[remapped_node_index].translation.y;
     destination[remapped_node_index].translation.z = source_node->translation.z + destination[remapped_node_index].translation.z;
    }
    else if (k_blend_mode == _animation_blend_mode_replace)
    {
     const real_orientation *source_node = &source[node_index];
     XMemCpy(&destination[remapped_node_index].translation, &source_node->translation, sizeof(real_point3d));
    }
    else if (k_blend_mode == _animation_blend_mode_object_space_correction)
    {
     real_orientation *destination_node = &destination[remapped_node_index];
     if (correction_flags->translation.test(node_index))
     {
      destination_node->translation.x = source[node_index].translation.x + corrections[node_index].translation.x;
      destination_node->translation.y = source[node_index].translation.y + corrections[node_index].translation.y;
      destination_node->translation.z = source[node_index].translation.z + corrections[node_index].translation.z;
     }
     else
      XMemCpy(&destination_node->translation, &source[node_index].translation, sizeof(real_point3d));
    }
    else if (k_blend_mode == _animation_blend_mode_blend)
    {
     const real_orientation *source_node = &source[node_index];
     animation_compute_orientations_interpolate_translation(&destination[remapped_node_index], source_node, ratio);
    }
   }
  }
  else
  {
   if (k_blend_mode == _animation_blend_mode_weighted_overlay)
   {
    animation_compute_orientations_add_weighted_translation(&destination[node_index], &source[node_index], ratio);
   }
   else if (k_blend_mode == _animation_blend_mode_overlay)
   {
    real x = source[node_index].translation.x;
    real y = source[node_index].translation.y;
    real z = source[node_index].translation.z;
    destination[node_index].translation.x = x + destination[node_index].translation.x;
    destination[node_index].translation.y = y + destination[node_index].translation.y;
    destination[node_index].translation.z = z + destination[node_index].translation.z;
   }
   else if (k_blend_mode == _animation_blend_mode_replace)
   {
    const real_orientation *source_node = &source[node_index];
    real_orientation *destination_node = &destination[node_index];
    XMemCpy(&destination_node->translation, &source_node->translation, sizeof(real_point3d));
   }
   else if (k_blend_mode == _animation_blend_mode_object_space_correction)
   {
    const real_orientation *source_node = &source[node_index];
    real_orientation *destination_node = &destination[node_index];
    if (correction_flags->translation.test(node_index))
    {
     destination_node->translation.x = source_node->translation.x + corrections[node_index].translation.x;
     destination_node->translation.y = source_node->translation.y + corrections[node_index].translation.y;
     destination_node->translation.z = source_node->translation.z + corrections[node_index].translation.z;
    }
    else
     XMemCpy(&destination_node->translation, &source_node->translation, sizeof(real_point3d));
   }
   else if (k_blend_mode == _animation_blend_mode_blend)
   {
    const real_orientation *source_node = &source[node_index];
    real_orientation *destination_node = &destination[node_index];
    destination_node->translation.x = (source_node->translation.x - destination_node->translation.x) * ratio +
        destination_node->translation.x;
    destination_node->translation.y = (source_node->translation.y - destination_node->translation.y) * ratio +
        destination_node->translation.y;
    destination_node->translation.z = (source_node->translation.z - destination_node->translation.z) * ratio +
        destination_node->translation.z;
   }
  }
 )
 ANIMATION_COMPUTE_ORIENTATIONS_FOR_EACH_NODE(&flags->scale, node_index,
  long remapped_node_index = node_index;
  if (k_remap)
  {
   remapped_node_index = ((const short *)inheritance->node_remap.address)[node_index];
   if (remapped_node_index == -1)
    continue;
  }
  real_orientation *destination_orientation = &destination[remapped_node_index];
  const real_orientation *source_orientation = &source[node_index];
  if (k_blend_mode == _animation_blend_mode_object_space_correction)
  {
   const real_orientation *source_node = source_orientation;
   real_orientation *destination_node = destination_orientation;
   if (correction_flags->scale.test(node_index))
    destination_node->scale = corrections[node_index].scale * source_node->scale;
   else
    destination_node->scale = source_node->scale;
  }
  else if (k_blend_mode == _animation_blend_mode_weighted_overlay)
  {
   const real_orientation *source_node = source_orientation;
   real scale = source_node->scale;
   destination_orientation->scale = ((scale - 1.0f) * ratio + 1.0f) * destination_orientation->scale;
  }
  else if (k_blend_mode == _animation_blend_mode_overlay)
  {
   real scale = source_orientation->scale;
   destination_orientation->scale = scale * destination_orientation->scale;
  }
  else if (k_blend_mode == _animation_blend_mode_blend)
  {
   real_orientation *destination_node = destination_orientation;
   real scale = source_orientation->scale;
   destination_node->scale = (scale - destination_node->scale) * ratio + destination_node->scale;
  }
  else if (k_blend_mode == _animation_blend_mode_replace)
  {
   destination_orientation->scale = source_orientation->scale;
  }
 )
}

#define ANIMATION_BLEND_INSTANCE(MODE, REMAP) template void animation_blend_node_orientations<MODE, REMAP>(long, \
 const s_animation_inheritence *, const s_node_flag_sets *, const real_orientation *, const s_node_flag_sets *, \
 const real_orientation *, long, real_orientation *__restrict, real, real);
ANIMATION_BLEND_INSTANCE(_animation_blend_mode_blend, false)
ANIMATION_BLEND_INSTANCE(_animation_blend_mode_replace, false)

ANIMATION_BLEND_INSTANCE(_animation_blend_mode_overlay, false)
ANIMATION_BLEND_INSTANCE(_animation_blend_mode_weighted_overlay, false)
ANIMATION_BLEND_INSTANCE(_animation_blend_mode_object_space_correction, false)
ANIMATION_BLEND_INSTANCE(_animation_blend_mode_replace, true)
ANIMATION_BLEND_INSTANCE(_animation_blend_mode_blend, true)
ANIMATION_BLEND_INSTANCE(_animation_blend_mode_overlay, true)
ANIMATION_BLEND_INSTANCE(_animation_blend_mode_weighted_overlay, true)
ANIMATION_BLEND_INSTANCE(_animation_blend_mode_object_space_correction, true)

// 0x307ae8: mask the rotation, translation and scale flags.
// @address 0x307ae8 ?animation_mask_node_flags@@YAXPAUs_node_flag_sets@@PBUflags256@@@Z
void animation_mask_node_flags(s_node_flag_sets *flags, const flags256 *mask)
{
 flags->rotation.and_with(mask);
 flags->translation.and_with(mask);
 flags->scale.and_with(mask);
}

// 0x301b60






static __forceinline const unsigned char *animation_compute_orientations_static_flags_a(
 const unsigned char *data, const c_animation_data_sizes *sizes, long bits, long k)
{
 long offset = sizes->m_default_data_size;
 offset = offset + sizes->m_compressed_data_size;
 offset = offset + ((((bits + 31) >> 5) << 2) * k);
 const unsigned char *result = data;
 result = result + offset;
 return result;
}
static __forceinline const unsigned char *animation_compute_orientations_animated_flags_a(
 const unsigned char *data, const c_animation_data_sizes *sizes, long bits, long k)
{
 long offset = sizes->m_default_data_size;
 offset = offset + sizes->m_compressed_data_size;
 offset = offset + sizes->m_static_node_flags_data_size;
 offset = offset + ((((bits + 31) >> 5) << 2) * k);
 const unsigned char *result = data;
 result = result + offset;
 return result;
}

static __forceinline const unsigned char *animation_compute_orientations_static_flags(
 const unsigned char *data, const c_animation_data_sizes *sizes, long bits, long k)
{
 long flag_set_bytes = ((bits + 31) >> 5) << 2;
 long offset = sizes->m_default_data_size;
 offset = offset + sizes->m_compressed_data_size;
 offset = offset + flag_set_bytes * k;
 const unsigned char *result = data;
 result = result + offset;
 return result;
}

static __forceinline const unsigned char *animation_compute_orientations_animated_flags(
 const unsigned char *data, const c_animation_data_sizes *sizes, long bits, long k)
{
 long flag_set_bytes = ((bits + 31) >> 5) << 2;
 long offset = sizes->m_default_data_size;
 offset = offset + sizes->m_compressed_data_size;
 offset = offset + sizes->m_static_node_flags_data_size;
 offset = offset + flag_set_bytes * k;
 const unsigned char *result = data;
 result = result + offset;
 return result;
}
static __forceinline const unsigned char *animation_compute_orientations_static_flags_z(
 const unsigned char *data, const c_animation_data_sizes *sizes, long bits, long k)
{
 short default_size = sizes->m_default_data_size;
 long flag_set_bytes = ((bits + 31) >> 5) << 2;
 long compressed_size = sizes->m_compressed_data_size;
 long dl = default_size;
 long offset = dl + compressed_size;
 offset = offset + flag_set_bytes * k;
 const unsigned char *result = data;
 result = result + offset;
 return result;
}
static __forceinline const unsigned char *animation_compute_orientations_animated_flags_z(
 const unsigned char *data, const c_animation_data_sizes *sizes, long bits, long k)
{
 short default_size = sizes->m_default_data_size;
 long flag_set_bytes = ((bits + 31) >> 5) << 2;
 long offset = default_size;
 offset = offset + sizes->m_compressed_data_size;
 offset = offset + sizes->m_static_node_flags_data_size;
 offset = offset + flag_set_bytes * k;
 const unsigned char *result = data;
 result = result + offset;
 return result;
}
static __forceinline long animation_compute_orientations_flag_bytes(long count)
{
 long bytes = ((count + 31) >> 5);
 bytes = bytes * 4;
 return bytes;
}

// @address 0x7fc500 g_compute_node_orientations_workspaces
s_compute_node_orientations_workspace g_compute_node_orientations_workspaces[2];


#define ANIMATION_COMPUTE_ORIENTATIONS_MIN(A, B) ((A) < (B) ? (A) : (B))
#define ANIMATION_COMPUTE_ORIENTATIONS_MAX(A, B) ((A) > (B) ? (A) : (B))
#define ANIMATION_COMPUTE_ORIENTATIONS_PIN(VALUE, \
    MAXIMUM) ANIMATION_COMPUTE_ORIENTATIONS_MIN(ANIMATION_COMPUTE_ORIENTATIONS_MAX(VALUE, 0), MAXIMUM)

static __forceinline void animation_compute_orientations_clear_flags(s_node_flag_sets *flags)
{
 memset(&flags->rotation, 0, sizeof(flags256));
 memset(&flags->translation, 0, sizeof(flags256));
 memset(&flags->scale, 0, sizeof(flags256));
}


extern "C" void animation_compute_node_orientations(long graph_index, c_animation_id animation_id, long node_count, real frame,
 real weight, const flags256 *node_flags, real_orientation *orientations, c_flags<e_anim_computation_flags,
     unsigned long, 1> computation_flags, bool blend,
 real blend_frame, real blend_weight)
{
 if (weight <= 0.0001f)
  return;
 const c_model_animation_graph *definition = (const c_model_animation_graph
     *)global_tag_instances[datum_index_to_absolute_index(graph_index)].definition;
 const c_model_animation *animation = definition->get_animation(animation_id);
 if (!definition || !animation)
  return;
 c_animation_data data;
 data.m_data = 0;
 data.m_sizes = 0;
 data.m_node_count = 0;
 data.m_movement_data_type = 0;
 if (!definition->get_animation_data(animation_id, &data))
  return;
 int workspace_index = get_current_thread_index();
 s_compute_node_orientations_workspace *workspace = &g_compute_node_orientations_workspaces[0];
 if (workspace_index == 2)
  workspace = &g_compute_node_orientations_workspaces[1];
 const c_animation_data_sizes *sizes = data.m_sizes;
 long compute_count = animation->node_count;
 long prefetch_size = sizes->total_size();

 if (prefetch_size > 1024)
  prefetch_size = 1024;
 const unsigned char *base = data.m_data;
 for (long offset = 0; offset < prefetch_size; offset += 128)
  __dcbt(offset, (void *)base);

 s_node_flag_sets correction_flags;
 s_node_flag_sets static_node_flags;
 s_node_flag_sets animated_node_flags;
 animation_compute_orientations_clear_flags(&correction_flags);
 flags256 requested_nodes;
 memset(&requested_nodes, 0, sizeof(flags256));

 long has_inheritance = 0;
 const s_animation_inheritence *child = 0;
 if (animation_id.m_animation_index >= 0)
 {
  short child_index = animation_id.m_graph_index;
  if (child_index >= 0 && child_index < definition->m_inheritence_list.count)
   child = &((const s_animation_inheritence *)definition->m_inheritence_list.address)[child_index];
 }
 real z = 0.0f;
 real root_translation_scale = 1.0f;
 bool root_only = false;
 const s_animation_inheritence *inheritance = 0;
 if (child)
 {
  root_translation_scale = child->root_translation_scale;
  root_only = child->flags & 1;
 }
 const flags256 *inheritance_flags = 0;
 if (child && child->node_remap.count && child->node_flags.count)
  inheritance_flags = (const flags256 *)child->node_flags.address;
 long cca;
 if (inheritance_flags)
 {
  has_inheritance = 1;
  inheritance = child;
  const short *node_remap = (const short *)child->node_remap.address;
  if (node_flags)
  {
   for (long node_index = 0; node_index < compute_count; node_index++)
   {
    if (inheritance_flags->test(node_index))
    {
     short remapped_index = node_remap[node_index];
     if (remapped_index >= 0 && remapped_index < node_count)
     {
      bool requested = node_flags->test(remapped_index);
      animation_compute_orientations_flags_set(&requested_nodes, node_index, requested);
     }
    }
   }
  cca = compute_count;
  }
  else
  {
   for (long node_index = 0; node_index < compute_count; node_index++)
   {
    long word_index = node_index >> 5;
    if (inheritance_flags->m_storage[word_index] & (1 << (node_index & 31)))
    {
     short remapped_index = node_remap[node_index];
     if (remapped_index >= 0 && remapped_index < node_count)
     {
      unsigned long *word = &requested_nodes.m_storage[word_index];
      *word = *word | (1 << (node_index & 31));
     }
    }
   }
  cca = compute_count;
  }
 }
 else
 {
  if (node_count <= compute_count)
   compute_count = node_count;
  if (node_flags)
  {
   requested_nodes = *node_flags;
   cca = compute_count;
  }
  else
  {
   ((c_static_flags_no_init<32> *)&requested_nodes)->set_range(compute_count);
   cca = compute_count;
  }
 }
 if (has_inheritance)
 {
  long last_node = bit_vector_find_highest_bit(requested_nodes.m_storage, 256);
  cca = ANIMATION_COMPUTE_ORIENTATIONS_PIN(last_node + 1, compute_count);
 }
 if (has_inheritance || node_flags)
  object_clear_excess_flag_bits(requested_nodes.m_storage, cca);
 long frame_index = (long)frame;
 long last_frame = animation->frame_count - 1;
 long frame_a = ANIMATION_COMPUTE_ORIENTATIONS_PIN(frame_index, last_frame);
 long frame_b = frame_a;
 real fraction = 0.0f;
 long blend_frame_a = ANIMATION_COMPUTE_ORIENTATIONS_PIN(frame_index, last_frame);
 long blend_frame_b = frame_a;
 real blend_fraction = 0.0f;
 bool interpolate = computation_flags.test(_anim_computation_default_bit);
 if (blend)
 {
  long blend_index = (long)blend_frame;
  blend_frame_a = ANIMATION_COMPUTE_ORIENTATIONS_PIN(blend_index, last_frame);
  blend_frame_b = ANIMATION_COMPUTE_ORIENTATIONS_PIN(blend_index + 1, last_frame);
  blend_fraction = blend_frame - (real)blend_frame_a;
  frame_b = ANIMATION_COMPUTE_ORIENTATIONS_PIN(frame_a + 1, last_frame);
  fraction = frame - (real)frame_a;
 }
 else if (interpolate)
 {
  if (frame_a == last_frame)
   interpolate = false;
  else
  {
   frame_b = ANIMATION_COMPUTE_ORIENTATIONS_PIN(frame_a + 1, last_frame);
   fraction = frame - (real)frame_a;
   if (fraction < 0.0001f)
   {
    fraction = 0.0f;
    interpolate = false;
   }
   else if (fraction > 0.9999f)
   {
    frame_a++;
    fraction = 0.0f;
    interpolate = false;
   }
  }
 }

 long blend_mode = 0;
 bool full_weight = weight > 0.9999f;
 if (animation->animation_type == 1)
  blend_mode = (full_weight == false) + 2;
 else if (!full_weight)
  blend_mode = 1;
 if (blend_mode == 0 && build_object_space_correction_data(definition, animation, node_count, orientations,
  &correction_flags, workspace->corrections))
  blend_mode = 4;
 real_orientation *destination = orientations;
 if (blend_mode != 0 || has_inheritance)
  destination = workspace->orientations;

 s_node_flag_sets static_flags;
 animation_compute_orientations_clear_flags(&static_flags);
 real fraction_pinned = ANIMATION_COMPUTE_ORIENTATIONS_MIN(ANIMATION_COMPUTE_ORIENTATIONS_MAX(fraction, 0.0f),
     1.0f); fraction = fraction_pinned;
 real inverse_fraction = 1.0f - fraction;

 unsigned char node_flag_bits = data.m_node_count;
 if (animation->animation_type == 0 && sizes->m_default_data_size)
 {

  animation_compute_orientations_clear_flags(&static_node_flags);
  long flag_rounded = cca + 31; long flag_bytes = (flag_rounded >> 5) * 4;
  memcpy(&static_node_flags.rotation, animation_compute_orientations_static_flags_z(base, sizes, node_flag_bits, 0), flag_bytes);
  object_clear_excess_flag_bits(static_node_flags.rotation.m_storage, cca);
  memcpy(&static_node_flags.translation, animation_compute_orientations_static_flags_a(base, sizes, node_flag_bits, 1), flag_bytes);
  object_clear_excess_flag_bits(static_node_flags.translation.m_storage, cca);
  memcpy(&static_node_flags.scale, animation_compute_orientations_static_flags(base, sizes, node_flag_bits, 2), flag_bytes);
  object_clear_excess_flag_bits(static_node_flags.scale.m_storage, cca);
  animation_codec_decompress_orientations((const s_animation_codec_header *)base, cca, frame_a, frame_b, fraction,
      inverse_fraction, false, root_only,
   &static_node_flags, &requested_nodes, &static_flags, destination);
 }

 s_node_flag_sets animated_flags;
 animation_compute_orientations_clear_flags(&animated_flags);
 if (sizes->m_compressed_data_size)
 {

  const unsigned char *compressed = base + sizes->m_default_data_size;
  animation_compute_orientations_clear_flags(&animated_node_flags);
  long flag_bytes = ((cca + 31) >> 5) * 4;
  memcpy(&animated_node_flags.rotation, animation_compute_orientations_animated_flags_z(base, sizes, node_flag_bits, 0), flag_bytes);
  object_clear_excess_flag_bits(animated_node_flags.rotation.m_storage, cca);
  memcpy(&animated_node_flags.translation, animation_compute_orientations_animated_flags_a(base, sizes, node_flag_bits, 1), flag_bytes);
  object_clear_excess_flag_bits(animated_node_flags.translation.m_storage, cca);
  memcpy(&animated_node_flags.scale, animation_compute_orientations_animated_flags(base, sizes, node_flag_bits, 2), flag_bytes);
  object_clear_excess_flag_bits(animated_node_flags.scale.m_storage, cca);
  animation_codec_decompress_orientations((const s_animation_codec_header *)compressed, cca, frame_a, frame_b, fraction,
      inverse_fraction, interpolate,
   root_only, &animated_node_flags, &requested_nodes, &animated_flags, destination);
  if (blend)
  {
   real_orientation *blend_destination = workspace->corrections;
   animation_codec_decompress_orientations((const s_animation_codec_header *)compressed, cca, blend_frame_a,
       blend_frame_b, blend_fraction, 1.0f - blend_fraction,
    interpolate, root_only, &animated_node_flags, &requested_nodes, &animated_flags,
    blend_destination);
   animation_blend_node_orientations<_animation_blend_mode_blend, false>(cca, inheritance, &correction_flags, 0,
    &animated_flags, blend_destination, cca, destination, blend_weight, 1.0f - blend_weight);
  }
 }

 if (blend_mode != 4)
 {
  if (static_flags.translation.test(0) ||
   animated_flags.translation.test(0))
   { z = destination[0].translation.z; real scaled_z = root_translation_scale * z; destination[0].translation.z = scaled_z; }
 }
 if (blend_mode == 0 && !has_inheritance)
  return;
 s_node_flag_sets combined_flags;
 XMemCpy(&combined_flags, &static_flags, sizeof(s_node_flag_sets));
 combined_flags.rotation.or_with(&animated_flags.rotation);
 combined_flags.translation.or_with(&animated_flags.translation);
 combined_flags.scale.or_with(&animated_flags.scale);
 real clamped_weight = ANIMATION_COMPUTE_ORIENTATIONS_MIN(ANIMATION_COMPUTE_ORIENTATIONS_MAX(weight, 0.0f), 1.0f);
 real inverse_weight = 1.0f - clamped_weight;
 if (!has_inheritance)
 {
  switch (blend_mode)
  {
  case 0:
  animation_blend_node_orientations<_animation_blend_mode_replace, false>(cca, inheritance, &correction_flags, workspace->corrections,
      &combined_flags, workspace->orientations, node_count, orientations, clamped_weight, inverse_weight);
  return;
  case 1:
  animation_blend_node_orientations<_animation_blend_mode_blend, false>(cca, inheritance, &correction_flags, workspace->corrections,
      &combined_flags, workspace->orientations, node_count, orientations, clamped_weight, inverse_weight);
  return;
  case 2:
  animation_blend_node_orientations<_animation_blend_mode_overlay, false>(cca, inheritance, &correction_flags, workspace->corrections,
      &combined_flags, workspace->orientations, node_count, orientations, clamped_weight, inverse_weight);
  return;
  case 3:
  animation_blend_node_orientations<_animation_blend_mode_weighted_overlay, false>(cca, inheritance, &correction_flags,
      workspace->corrections, &combined_flags, workspace->orientations, node_count, orientations, clamped_weight, inverse_weight);
  return;
  case 4:
  animation_blend_node_orientations<_animation_blend_mode_object_space_correction, false>(cca, inheritance, &correction_flags,
      workspace->corrections, &combined_flags, workspace->orientations, node_count, orientations, clamped_weight, inverse_weight);
  return;
  // match: 0x301b60: unreachable hint omits dead branch check
  default: __assume(0);
  }
 }
 else
 {
  switch (blend_mode)
  {
  case 0:
  animation_blend_node_orientations<_animation_blend_mode_replace, true>(cca, inheritance, &correction_flags, workspace->corrections,
      &combined_flags, workspace->orientations, node_count, orientations, clamped_weight, inverse_weight);
  return;
  case 1:
  animation_blend_node_orientations<_animation_blend_mode_blend, true>(cca, inheritance, &correction_flags, workspace->corrections,
      &combined_flags, workspace->orientations, node_count, orientations, clamped_weight, inverse_weight);
  return;
  case 2:
  animation_blend_node_orientations<_animation_blend_mode_overlay, true>(cca, inheritance, &correction_flags, workspace->corrections,
      &combined_flags, workspace->orientations, node_count, orientations, clamped_weight, inverse_weight);
  return;
  case 3:
  animation_blend_node_orientations<_animation_blend_mode_weighted_overlay, true>(cca, inheritance, &correction_flags,
      workspace->corrections, &combined_flags, workspace->orientations, node_count, orientations, clamped_weight, inverse_weight);
  return;
  case 4:
  animation_blend_node_orientations<_animation_blend_mode_object_space_correction, true>(cca, inheritance, &correction_flags,
      workspace->corrections, &combined_flags, workspace->orientations, node_count, orientations, clamped_weight, inverse_weight);
  return;
  // match: 0x301b60: unreachable hint omits dead branch check
  default: __assume(0);
  }
 }
}


/* ---- the codecs' decompress-all entry points: every codec decodes all requested nodes the same way. Copy the
   animated masks into flags, keep the requested nodes, then walk the rotation, translation and scale masks a run of
   set bits at a time. `channel` counts the animated nodes passed (a codec stores only those) and `bit` is the node
   within the 32-bit mask word. The codecs differ only in their getters. ---- */




static __forceinline bool animation_compute_orientations_node_flags_empty(const flags256 *flags)
{
 const unsigned long *bits = flags->m_storage;
 return ((((((((((bits[7] == 0) & 1) & ((bits[6] == 0) & 1)) & ((bits[5] == 0) & 1)) & ((bits[4] == 0) & 1)) &
  ((bits[3] == 0) & 1)) & ((bits[2] == 0) & 1)) & ((bits[1] == 0) & 1)) & ((bits[0] == 0) & 1)) & 1);
}

// clears every node's flag but the root's
static __forceinline void animation_compute_orientations_keep_root_only(flags256 *flags)
{
 XMemSet(&flags->m_storage[1], 0, sizeof(flags256) - sizeof(unsigned long));
 flags->m_storage[0] &= 1;
}




/* ---- _8byte_quantized_rotation_only_codec at an integer frame ---- */


/* ---- _8byte_quantized_rotation_only_codec between two frames: the quaternions by q1*w0 + q0*w1 ---- */


static __forceinline unsigned char *animation_compute_orientations_keyframe_times(const s_keyframe_codec_header *const &header,
 long offset, unsigned long first, unsigned char *)
{
 return (unsigned char *)header + (offset + first);
}

static __forceinline unsigned short *animation_compute_orientations_keyframe_times(const s_keyframe_codec_header *const &header,
 long offset, unsigned long first, unsigned short *)
{
 return (unsigned short *)((char *)header + (first * 2 + offset));
}

template <typename t_key>
static __forceinline t_key *animation_compute_orientations_scale_times(const s_keyframe_codec_header *header, unsigned long first)
{
 return (t_key *)((char *)header + header->scale_times_offset) + first;
}




static __forceinline long animation_compute_orientations_lowest_bit_index(const unsigned long &bits)
{
 return abs((long)(bits & (0 - bits)) % 37);
}

// the lowest set bit of x, read from the table in place
#define ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(x) \
    k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(x)]

static __forceinline __vector4 animation_compute_orientations_key_constant(long index)
{
 return k_animation_codec_interpolated_rotation_constants[index];
}

static __forceinline __vector4 animation_compute_orientations_quantized_constant(long index)
{
 return k_animation_codec_quantized_rotation_constants[index];
}

/* ---- the walk, the same for every codec: each codec table row points at its codec's instance ---- */


template <typename t_codec>
void c_codec_decompressor<t_codec>::decompress_all(const typename t_codec::header_type *header, long node_count,
 unsigned long frame0, unsigned long frame1, real w0, real w1, bool root_only, const s_node_flag_sets *animated,
 // match: 0x30ca68: __restrict is part of this instantiation's mangled name and prevents alias reload
 const flags256 *requested, s_node_flag_sets *flags, real_orientation *__restrict out)
{
 if (animation_compute_orientations_node_flags_empty(requested))
  return;

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
   __vector4 key_bias = animation_compute_orientations_key_constant(1);
   __vector4 key_scale = animation_compute_orientations_key_constant(0);
   __vector4 bias = animation_compute_orientations_quantized_constant(1);
   __vector4 scale = animation_compute_orientations_quantized_constant(0);
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
       long ones_a = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(~a);
       long ones_b = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(~b);
       long run = ones_a > ones_b ? ones_b : ones_a;
       if (run > 0)
       {
        for (long i = 0; i < run; i++)
        {
         real_orientation *node_out = &out[bit + base + i];
         t_codec::decompress_rotation(header, channel + i, frame0, frame1, w0, w1, node_out, scale, bias, key_scale, key_bias);
        }
        ones_a -= run;
        bit += run;
        channel += run;
        a >>= run;
        b >>= run;
       }
       if (ones_a > 0)
       {
        long zeros_b = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(b);
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
          t_codec::decompress_rotation(header, channel, frame0, frame1, w0, w1, node_out, scale, bias, key_scale, key_bias);
         }
         ones_a--;
         bit++;
         channel++;
         a >>= 1;
         b >>= 1;
        }
       }
       long zeros_a = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(a);
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
    t_codec::decompress_translation(header, 0, frame0, frame1, w0, w1, out);
   if (flags->scale.m_storage[0] & 1)
    t_codec::decompress_scale(header, 0, frame0, frame1, w0, w1, out);
  }
  animation_compute_orientations_keep_root_only(&flags->translation);
  animation_compute_orientations_keep_root_only(&flags->scale);
  return;
 }

 if (!animation_node_flags_are_empty(&flags->translation))
 {
  long channel_next = 0;
  const unsigned long *mask = animated->translation.m_storage;
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
       long ones_a = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(~a);
       long ones_b = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(~b);
       long run = ones_a > ones_b ? ones_b : ones_a;
       if (run > 0)
       {
        for (long i = 0; i < run; i++)
        {
         real_orientation *node_out = &out[bit + base + i];
         t_codec::decompress_translation(header, channel + i, frame0, frame1, w0, w1, node_out);
        }
        ones_a -= run;
        bit += run;
        channel += run;
        a >>= run;
        b >>= run;
       }
       if (ones_a > 0)
       {
        long zeros_b = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(b);
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
          t_codec::decompress_translation(header, channel, frame0, frame1, w0, w1, node_out);
         }
         ones_a--;
         bit++;
         channel++;
         a >>= 1;
         b >>= 1;
        }
       }
       long zeros_a = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(a);
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

 if (!animation_compute_orientations_node_flags_empty(&flags->scale))
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
    if (a == 0)
     continue;
    if (b == 0 || node_count == 0)
    {
     channel += count_bits<unsigned long>(a);
    }
    else
    {
     bit = 0;
     do
     {
      long ones_a = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(~a);
      long ones_b = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(~b);
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
       long zeros_b = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(b);
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
      long zeros_a = ANIMATION_COMPUTE_ORIENTATIONS_LOWEST_BIT(a);
      bit += zeros_a;
      a >>= zeros_a;
      b >>= zeros_a;
     } while (bit < 32);
    }
   } while (channel_next = channel, ++word < (long)word_count);
  }
 }
}

template struct c_codec_decompressor<c_keyframe_codec<unsigned char, false> >;
template struct c_codec_decompressor<c_interpolated_keyframe_codec<unsigned char> >;
template struct c_codec_decompressor<c_keyframe_codec<unsigned short, false> >;

// 0x302a30: the orientations of an aiming blend screen: yaw and pitch pick the screen's frame (the pitch rows
// blended when the pitch falls between two)


const real k_real_epsilon = 0.0001f;
extern "C" void compute_node_orientations_from_blend_screen(long graph_index, c_animation_id animation_id,
    long node_count, real yaw, real pitch,
 const s_animation_compute_orientations_aiming_screen_bounds *const bounds, real weight, const flags256 *node_flags,
 real_orientation *orientations)
{
 if (node_count != 0)
 {
  long yaw_frame_count = bounds->left_frame_count + bounds->right_frame_count + 1;
  long pitch_frame_count = bounds->up_pitch_frame_count + bounds->down_pitch_frame_count + 1;

  real yaw_per_frame = yaw < 0.0f ? bounds->right_yaw_per_frame : bounds->left_yaw_per_frame;
  real real_yaw_index = yaw_per_frame < k_real_epsilon ? 0.0f : yaw / yaw_per_frame;
  real_yaw_index = ANIMATION_COMPUTE_ORIENTATIONS_MIN(ANIMATION_COMPUTE_ORIENTATIONS_MAX(real_yaw_index,
   (real)-bounds->right_frame_count), (real)bounds->left_frame_count);
  real_yaw_index = real_yaw_index + bounds->right_frame_count;

  real pitch_per_frame = pitch < 0.0f ? bounds->down_pitch_per_frame : bounds->up_pitch_per_frame;
  real real_pitch_index = pitch_per_frame < k_real_epsilon ? 0.0f : pitch / pitch_per_frame;
  real_pitch_index = ANIMATION_COMPUTE_ORIENTATIONS_MIN(ANIMATION_COMPUTE_ORIENTATIONS_MAX(real_pitch_index,
   (real)-bounds->down_pitch_frame_count), (real)bounds->up_pitch_frame_count);
  real_pitch_index = real_pitch_index + bounds->down_pitch_frame_count;

  real pitch_index0 = (real)floor(real_pitch_index);
  real pitch_index1 = (real)ceil(real_pitch_index);
  pitch_index0 = ANIMATION_COMPUTE_ORIENTATIONS_MIN(ANIMATION_COMPUTE_ORIENTATIONS_MAX(pitch_index0, 0.0f), (real)(pitch_frame_count - 1));
  pitch_index1 = ANIMATION_COMPUTE_ORIENTATIONS_MIN(ANIMATION_COMPUTE_ORIENTATIONS_MAX(pitch_index1, 0.0f), (real)(pitch_frame_count - 1));
  real frame0 = pitch_index0 * yaw_frame_count + real_yaw_index;
  real frame1 = pitch_index1 * yaw_frame_count + real_yaw_index;
  real floor_fraction = real_pitch_index - pitch_index0;
  if (floor_fraction < k_real_epsilon)
  {
   c_flags<e_anim_computation_flags, unsigned long, 1> computation_flags(1);
   c_flags<e_anim_computation_flags, unsigned long, 1> *computation_flags_pointer = &computation_flags;
   // match: 0x302a30: value range assumption guides branch lowering
   __assume(node_flags != (const flags256 *)&computation_flags);
   animation_compute_node_orientations(
       graph_index, animation_id, node_count, frame0, weight, node_flags, orientations, *computation_flags_pointer, false, 0.0f, 0.0f);
  }
  else if (pitch_index1 - real_pitch_index < k_real_epsilon)
  {
   c_flags<e_anim_computation_flags, unsigned long, 1> computation_flags(1);
   c_flags<e_anim_computation_flags, unsigned long, 1> *computation_flags_pointer = &computation_flags;
   // match: 0x302a30: value range assumption guides branch lowering
   __assume(node_flags != (const flags256 *)&computation_flags);
   animation_compute_node_orientations(
       graph_index, animation_id, node_count, frame1, weight, node_flags, orientations, *computation_flags_pointer, false, 0.0f, 0.0f);
  }
  else
  {
   c_flags<e_anim_computation_flags, unsigned long, 1> computation_flags(1);
   c_flags<e_anim_computation_flags, unsigned long, 1> *computation_flags_pointer = &computation_flags;
   // match: 0x302a30: value range assumption guides branch lowering
   __assume(node_flags != (const flags256 *)&computation_flags);
   animation_compute_node_orientations(graph_index, animation_id, node_count, frame0, weight, node_flags, orientations,
       *computation_flags_pointer, true, frame1, floor_fraction);
  }
 }
}
/* ---- the full-frame codecs' walk: every frame of a node stored (or a single record per node) ---- */
static __forceinline bool animation_compute_orientations_flags_empty_loop(const flags256 *flags)
{
 bool empty = true;
 for (int i = 0; i < 8; i++) {
  empty &= (flags->m_storage[i] == 0);
 }
 return empty;
}

template <typename t_header>
static __forceinline unsigned long animation_compute_orientations_word_count(const t_header *header, unsigned long words)
{
 return words > 8 ? 8 : words;
}

static __forceinline unsigned long animation_compute_orientations_word_count(const s_static_codec_header *header, unsigned long words)
{
 unsigned long word_count = 8;
 if (words <= 8)
  word_count = words;
 return word_count;
}

template <typename t_codec>
static __forceinline void animation_compute_orientations_walk_pad(long v, long w)
{
}

template <typename t_codec>
static __forceinline void animation_compute_orientations_walk_pad2(long v, long w)
{
}

template <>
__forceinline void animation_compute_orientations_walk_pad<c_uncompressed_animated_data_codec>(long v, long w)
{
 long tpad[4];
 tpad[0 & 3] = (long)((v ^ 0) * w);
 tpad[1 & 3] = (long)((v ^ 1) * w);
 tpad[2 & 3] = (long)((v ^ 2) * w);
 tpad[3 & 3] = (long)((v ^ 3) * w);
 tpad[4 & 3] = (long)((v ^ 4) * w);
 tpad[5 & 3] = (long)((v ^ 5) * w);
 tpad[6 & 3] = (long)((v ^ 6) * w);
 tpad[7 & 3] = (long)((v ^ 7) * w);
 tpad[8 & 3] = (long)((v ^ 8) * w);
 tpad[9 & 3] = (long)((v ^ 9) * w);
 tpad[10 & 3] = (long)((v ^ 10) * w);
 tpad[11 & 3] = (long)((v ^ 11) * w);
 tpad[12 & 3] = (long)((v ^ 12) * w);
 tpad[13 & 3] = (long)((v ^ 13) * w);
 tpad[14 & 3] = (long)((v ^ 14) * w);
 tpad[15 & 3] = (long)((v ^ 15) * w);
 tpad[16 & 3] = (long)((v ^ 16) * w);
 tpad[17 & 3] = (long)((v ^ 17) * w);
 tpad[18 & 3] = (long)((v ^ 18) * w);
 tpad[19 & 3] = (long)((v ^ 19) * w);
 tpad[20 & 3] = (long)((v ^ 20) * w);
 tpad[21 & 3] = (long)((v ^ 21) * w);
 tpad[22 & 3] = (long)((v ^ 22) * w);
 tpad[23 & 3] = (long)((v ^ 23) * w);
 tpad[24 & 3] = (long)((v ^ 24) * w);
 tpad[25 & 3] = (long)((v ^ 25) * w);
 tpad[26 & 3] = (long)((v ^ 26) * w);
 tpad[27 & 3] = (long)((v ^ 27) * w);
 tpad[28 & 3] = (long)((v ^ 28) * w);
 tpad[29 & 3] = (long)((v ^ 29) * w);
 tpad[30 & 3] = (long)((v ^ 30) * w);
 tpad[31 & 3] = (long)((v ^ 31) * w);
 tpad[32 & 3] = (long)((v ^ 32) * w);
 tpad[33 & 3] = (long)((v ^ 33) * w);
 tpad[34 & 3] = (long)((v ^ 34) * w);
 tpad[35 & 3] = (long)((v ^ 35) * w);
 tpad[36 & 3] = (long)((v ^ 36) * w);
 tpad[37 & 3] = (long)((v ^ 37) * w);
 tpad[38 & 3] = (long)((v ^ 38) * w);
 tpad[39 & 3] = (long)((v ^ 39) * w);
 tpad[40 & 3] = (long)((v ^ 40) * w);
 tpad[41 & 3] = (long)((v ^ 41) * w);
 tpad[42 & 3] = (long)((v ^ 42) * w);
 tpad[43 & 3] = (long)((v ^ 43) * w);
 tpad[44 & 3] = (long)((v ^ 44) * w);
 tpad[45 & 3] = (long)((v ^ 45) * w);
 tpad[46 & 3] = (long)((v ^ 46) * w);
 tpad[47 & 3] = (long)((v ^ 47) * w);
 tpad[48 & 3] = (long)((v ^ 48) * w);
 tpad[49 & 3] = (long)((v ^ 49) * w);
 tpad[50 & 3] = (long)((v ^ 50) * w);
 tpad[51 & 3] = (long)((v ^ 51) * w);
 tpad[52 & 3] = (long)((v ^ 52) * w);
 tpad[53 & 3] = (long)((v ^ 53) * w);
 tpad[54 & 3] = (long)((v ^ 54) * w);
 tpad[55 & 3] = (long)((v ^ 55) * w);
 tpad[56 & 3] = (long)((v ^ 56) * w);
 tpad[57 & 3] = (long)((v ^ 57) * w);
 tpad[58 & 3] = (long)((v ^ 58) * w);
 tpad[59 & 3] = (long)((v ^ 59) * w);
 tpad[60 & 3] = (long)((v ^ 60) * w);
 tpad[61 & 3] = (long)((v ^ 61) * w);
 tpad[62 & 3] = (long)((v ^ 62) * w);
 tpad[63 & 3] = (long)((v ^ 63) * w);
 tpad[64 & 3] = (long)((v ^ 64) * w);
 tpad[65 & 3] = (long)((v ^ 65) * w);
 tpad[66 & 3] = (long)((v ^ 66) * w);
 tpad[67 & 3] = (long)((v ^ 67) * w);
 tpad[68 & 3] = (long)((v ^ 68) * w);
 tpad[69 & 3] = (long)((v ^ 69) * w);
 tpad[70 & 3] = (long)((v ^ 70) * w);
 tpad[71 & 3] = (long)((v ^ 71) * w);
 tpad[72 & 3] = (long)((v ^ 72) * w);
 tpad[73 & 3] = (long)((v ^ 73) * w);
 tpad[74 & 3] = (long)((v ^ 74) * w);
 tpad[75 & 3] = (long)((v ^ 75) * w);
 tpad[76 & 3] = (long)((v ^ 76) * w);
 tpad[77 & 3] = (long)((v ^ 77) * w);
 tpad[78 & 3] = (long)((v ^ 78) * w);
 tpad[79 & 3] = (long)((v ^ 79) * w);
 tpad[80 & 3] = (long)((v ^ 80) * w);
 tpad[81 & 3] = (long)((v ^ 81) * w);
 tpad[82 & 3] = (long)((v ^ 82) * w);
 tpad[83 & 3] = (long)((v ^ 83) * w);
 tpad[84 & 3] = (long)((v ^ 84) * w);
 tpad[85 & 3] = (long)((v ^ 85) * w);
 tpad[86 & 3] = (long)((v ^ 86) * w);
 tpad[87 & 3] = (long)((v ^ 87) * w);
 tpad[88 & 3] = (long)((v ^ 88) * w);
 tpad[89 & 3] = (long)((v ^ 89) * w);
 tpad[90 & 3] = (long)((v ^ 90) * w);
 tpad[91 & 3] = (long)((v ^ 91) * w);
 tpad[92 & 3] = (long)((v ^ 92) * w);
 tpad[93 & 3] = (long)((v ^ 93) * w);
 tpad[94 & 3] = (long)((v ^ 94) * w);
 tpad[95 & 3] = (long)((v ^ 95) * w);
 tpad[96 & 3] = (long)((v ^ 96) * w);
 tpad[97 & 3] = (long)((v ^ 97) * w);
 tpad[98 & 3] = (long)((v ^ 98) * w);
 tpad[99 & 3] = (long)((v ^ 99) * w);
 tpad[100 & 3] = (long)((v ^ 100) * w);
 tpad[101 & 3] = (long)((v ^ 101) * w);
 tpad[102 & 3] = (long)((v ^ 102) * w);
 tpad[103 & 3] = (long)((v ^ 103) * w);
 tpad[104 & 3] = (long)((v ^ 104) * w);
 tpad[105 & 3] = (long)((v ^ 105) * w);
 tpad[106 & 3] = (long)((v ^ 106) * w);
 tpad[107 & 3] = (long)((v ^ 107) * w);
 tpad[108 & 3] = (long)((v ^ 108) * w);
 tpad[109 & 3] = (long)((v ^ 109) * w);
 tpad[110 & 3] = (long)((v ^ 110) * w);
 tpad[111 & 3] = (long)((v ^ 111) * w);
 tpad[112 & 3] = (long)((v ^ 112) * w);
 tpad[113 & 3] = (long)((v ^ 113) * w);
 tpad[114 & 3] = (long)((v ^ 114) * w);
 tpad[115 & 3] = (long)((v ^ 115) * w);
 tpad[116 & 3] = (long)((v ^ 116) * w);
 tpad[117 & 3] = (long)((v ^ 117) * w);
 tpad[118 & 3] = (long)((v ^ 118) * w);
 tpad[119 & 3] = (long)((v ^ 119) * w);
 tpad[120 & 3] = (long)((v ^ 120) * w);
 tpad[121 & 3] = (long)((v ^ 121) * w);
 tpad[122 & 3] = (long)((v ^ 122) * w);
 tpad[123 & 3] = (long)((v ^ 123) * w);
 tpad[124 & 3] = (long)((v ^ 124) * w);
 tpad[125 & 3] = (long)((v ^ 125) * w);
 tpad[126 & 3] = (long)((v ^ 126) * w);
 tpad[127 & 3] = (long)((v ^ 127) * w);
 tpad[128 & 3] = (long)((v ^ 128) * w);
 tpad[129 & 3] = (long)((v ^ 129) * w);
 tpad[130 & 3] = (long)((v ^ 130) * w);
 tpad[131 & 3] = (long)((v ^ 131) * w);
 tpad[132 & 3] = (long)((v ^ 132) * w);
 tpad[133 & 3] = (long)((v ^ 133) * w);
 tpad[134 & 3] = (long)((v ^ 134) * w);
 tpad[135 & 3] = (long)((v ^ 135) * w);
 tpad[136 & 3] = (long)((v ^ 136) * w);
 tpad[137 & 3] = (long)((v ^ 137) * w);
 tpad[138 & 3] = (long)((v ^ 138) * w);
 tpad[139 & 3] = (long)((v ^ 139) * w);
 tpad[140 & 3] = (long)((v ^ 140) * w);
 tpad[141 & 3] = (long)((v ^ 141) * w);
 tpad[142 & 3] = (long)((v ^ 142) * w);
 tpad[143 & 3] = (long)((v ^ 143) * w);
 tpad[144 & 3] = (long)((v ^ 144) * w);
 tpad[145 & 3] = (long)((v ^ 145) * w);
 tpad[146 & 3] = (long)((v ^ 146) * w);
 tpad[147 & 3] = (long)((v ^ 147) * w);
 tpad[148 & 3] = (long)((v ^ 148) * w);
 tpad[149 & 3] = (long)((v ^ 149) * w);
 tpad[150 & 3] = (long)((v ^ 150) * w);
 tpad[151 & 3] = (long)((v ^ 151) * w);
 tpad[152 & 3] = (long)((v ^ 152) * w);
 tpad[153 & 3] = (long)((v ^ 153) * w);
 tpad[154 & 3] = (long)((v ^ 154) * w);
 tpad[155 & 3] = (long)((v ^ 155) * w);
 tpad[156 & 3] = (long)((v ^ 156) * w);
 tpad[157 & 3] = (long)((v ^ 157) * w);
 tpad[158 & 3] = (long)((v ^ 158) * w);
 tpad[159 & 3] = (long)((v ^ 159) * w);
 tpad[160 & 3] = (long)((v ^ 160) * w);
 tpad[161 & 3] = (long)((v ^ 161) * w);
 tpad[162 & 3] = (long)((v ^ 162) * w);
 tpad[163 & 3] = (long)((v ^ 163) * w);
 tpad[164 & 3] = (long)((v ^ 164) * w);
 tpad[165 & 3] = (long)((v ^ 165) * w);
 tpad[166 & 3] = (long)((v ^ 166) * w);
 tpad[167 & 3] = (long)((v ^ 167) * w);
 tpad[168 & 3] = (long)((v ^ 168) * w);
 tpad[169 & 3] = (long)((v ^ 169) * w);
 tpad[170 & 3] = (long)((v ^ 170) * w);
 tpad[171 & 3] = (long)((v ^ 171) * w);
 tpad[172 & 3] = (long)((v ^ 172) * w);
 tpad[173 & 3] = (long)((v ^ 173) * w);
 tpad[174 & 3] = (long)((v ^ 174) * w);
 tpad[175 & 3] = (long)((v ^ 175) * w);
 tpad[176 & 3] = (long)((v ^ 176) * w);
 tpad[177 & 3] = (long)((v ^ 177) * w);
 tpad[178 & 3] = (long)((v ^ 178) * w);
 tpad[179 & 3] = (long)((v ^ 179) * w);
 tpad[180 & 3] = (long)((v ^ 180) * w);
 tpad[181 & 3] = (long)((v ^ 181) * w);
 tpad[182 & 3] = (long)((v ^ 182) * w);
 tpad[183 & 3] = (long)((v ^ 183) * w);
 tpad[184 & 3] = (long)((v ^ 184) * w);
 tpad[185 & 3] = (long)((v ^ 185) * w);
 tpad[186 & 3] = (long)((v ^ 186) * w);
 tpad[187 & 3] = (long)((v ^ 187) * w);
 tpad[188 & 3] = (long)((v ^ 188) * w);
 tpad[189 & 3] = (long)((v ^ 189) * w);
 tpad[190 & 3] = (long)((v ^ 190) * w);
 tpad[191 & 3] = (long)((v ^ 191) * w);
 tpad[192 & 3] = (long)((v ^ 192) * w);
 tpad[193 & 3] = (long)((v ^ 193) * w);
 tpad[194 & 3] = (long)((v ^ 194) * w);
 tpad[195 & 3] = (long)((v ^ 195) * w);
 tpad[196 & 3] = (long)((v ^ 196) * w);
 tpad[197 & 3] = (long)((v ^ 197) * w);
 tpad[198 & 3] = (long)((v ^ 198) * w);
 tpad[199 & 3] = (long)((v ^ 199) * w);
 tpad[200 & 3] = (long)((v ^ 200) * w);
 tpad[201 & 3] = (long)((v ^ 201) * w);
 tpad[202 & 3] = (long)((v ^ 202) * w);
 tpad[203 & 3] = (long)((v ^ 203) * w);
 tpad[204 & 3] = (long)((v ^ 204) * w);
 tpad[205 & 3] = (long)((v ^ 205) * w);
 tpad[206 & 3] = (long)((v ^ 206) * w);
 tpad[207 & 3] = (long)((v ^ 207) * w);
 tpad[208 & 3] = (long)((v ^ 208) * w);
 tpad[209 & 3] = (long)((v ^ 209) * w);
 tpad[210 & 3] = (long)((v ^ 210) * w);
 tpad[211 & 3] = (long)((v ^ 211) * w);
 tpad[212 & 3] = (long)((v ^ 212) * w);
 tpad[213 & 3] = (long)((v ^ 213) * w);
 tpad[214 & 3] = (long)((v ^ 214) * w);
 tpad[215 & 3] = (long)((v ^ 215) * w);
 tpad[216 & 3] = (long)((v ^ 216) * w);
 tpad[217 & 3] = (long)((v ^ 217) * w);
 tpad[218 & 3] = (long)((v ^ 218) * w);
 tpad[219 & 3] = (long)((v ^ 219) * w);
 tpad[220 & 3] = (long)((v ^ 220) * w);
 tpad[221 & 3] = (long)((v ^ 221) * w);
 tpad[222 & 3] = (long)((v ^ 222) * w);
 tpad[223 & 3] = (long)((v ^ 223) * w);
 tpad[224 & 3] = (long)((v ^ 224) * w);
 tpad[225 & 3] = (long)((v ^ 225) * w);
 tpad[226 & 3] = (long)((v ^ 226) * w);
 tpad[227 & 3] = (long)((v ^ 227) * w);
 tpad[228 & 3] = (long)((v ^ 228) * w);
 tpad[229 & 3] = (long)((v ^ 229) * w);
 tpad[230 & 3] = (long)((v ^ 230) * w);
 tpad[231 & 3] = (long)((v ^ 231) * w);
 tpad[232 & 3] = (long)((v ^ 232) * w);
 tpad[233 & 3] = (long)((v ^ 233) * w);
 tpad[234 & 3] = (long)((v ^ 234) * w);
 tpad[235 & 3] = (long)((v ^ 235) * w);
 tpad[236 & 3] = (long)((v ^ 236) * w);
 tpad[237 & 3] = (long)((v ^ 237) * w);
 tpad[238 & 3] = (long)((v ^ 238) * w);
 tpad[239 & 3] = (long)((v ^ 239) * w);
 tpad[240 & 3] = (long)((v ^ 240) * w);
 tpad[241 & 3] = (long)((v ^ 241) * w);
 tpad[242 & 3] = (long)((v ^ 242) * w);
 tpad[243 & 3] = (long)((v ^ 243) * w);
 tpad[244 & 3] = (long)((v ^ 244) * w);
 tpad[245 & 3] = (long)((v ^ 245) * w);
 tpad[246 & 3] = (long)((v ^ 246) * w);
 tpad[247 & 3] = (long)((v ^ 247) * w);
 tpad[248 & 3] = (long)((v ^ 248) * w);
 tpad[249 & 3] = (long)((v ^ 249) * w);
 tpad[250 & 3] = (long)((v ^ 250) * w);
 tpad[251 & 3] = (long)((v ^ 251) * w);
 tpad[252 & 3] = (long)((v ^ 252) * w);
 tpad[253 & 3] = (long)((v ^ 253) * w);
 tpad[254 & 3] = (long)((v ^ 254) * w);
 tpad[255 & 3] = (long)((v ^ 255) * w);
 tpad[256 & 3] = (long)((v ^ 256) * w);
 tpad[257 & 3] = (long)((v ^ 257) * w);
 tpad[258 & 3] = (long)((v ^ 258) * w);
 tpad[259 & 3] = (long)((v ^ 259) * w);
 tpad[260 & 3] = (long)((v ^ 260) * w);
 tpad[261 & 3] = (long)((v ^ 261) * w);
 tpad[262 & 3] = (long)((v ^ 262) * w);
 tpad[263 & 3] = (long)((v ^ 263) * w);
 tpad[264 & 3] = (long)((v ^ 264) * w);
 tpad[265 & 3] = (long)((v ^ 265) * w);
 tpad[266 & 3] = (long)((v ^ 266) * w);
 tpad[267 & 3] = (long)((v ^ 267) * w);
 tpad[268 & 3] = (long)((v ^ 268) * w);
 tpad[269 & 3] = (long)((v ^ 269) * w);
 tpad[270 & 3] = (long)((v ^ 270) * w);
 tpad[271 & 3] = (long)((v ^ 271) * w);
 tpad[272 & 3] = (long)((v ^ 272) * w);
 tpad[273 & 3] = (long)((v ^ 273) * w);
 tpad[274 & 3] = (long)((v ^ 274) * w);
 tpad[275 & 3] = (long)((v ^ 275) * w);
 tpad[276 & 3] = (long)((v ^ 276) * w);
 tpad[277 & 3] = (long)((v ^ 277) * w);
 tpad[278 & 3] = (long)((v ^ 278) * w);
 tpad[279 & 3] = (long)((v ^ 279) * w);
 tpad[280 & 3] = (long)((v ^ 280) * w);
 tpad[281 & 3] = (long)((v ^ 281) * w);
 tpad[282 & 3] = (long)((v ^ 282) * w);
 tpad[283 & 3] = (long)((v ^ 283) * w);
 tpad[284 & 3] = (long)((v ^ 284) * w);
 tpad[285 & 3] = (long)((v ^ 285) * w);
 tpad[286 & 3] = (long)((v ^ 286) * w);
 tpad[287 & 3] = (long)((v ^ 287) * w);
 tpad[288 & 3] = (long)((v ^ 288) * w);
 tpad[289 & 3] = (long)((v ^ 289) * w);
 tpad[290 & 3] = (long)((v ^ 290) * w);
 tpad[291 & 3] = (long)((v ^ 291) * w);
 tpad[292 & 3] = (long)((v ^ 292) * w);
 tpad[293 & 3] = (long)((v ^ 293) * w);
 tpad[294 & 3] = (long)((v ^ 294) * w);
 tpad[295 & 3] = (long)((v ^ 295) * w);
 tpad[296 & 3] = (long)((v ^ 296) * w);
 tpad[297 & 3] = (long)((v ^ 297) * w);
 tpad[298 & 3] = (long)((v ^ 298) * w);
 tpad[299 & 3] = (long)((v ^ 299) * w);
 tpad[300 & 3] = (long)((v ^ 300) * w);
 tpad[301 & 3] = (long)((v ^ 301) * w);
 tpad[302 & 3] = (long)((v ^ 302) * w);
 tpad[303 & 3] = (long)((v ^ 303) * w);
 tpad[304 & 3] = (long)((v ^ 304) * w);
 tpad[305 & 3] = (long)((v ^ 305) * w);
 tpad[306 & 3] = (long)((v ^ 306) * w);
 tpad[307 & 3] = (long)((v ^ 307) * w);
 tpad[308 & 3] = (long)((v ^ 308) * w);
 tpad[309 & 3] = (long)((v ^ 309) * w);
 tpad[310 & 3] = (long)((v ^ 310) * w);
 tpad[311 & 3] = (long)((v ^ 311) * w);
 tpad[312 & 3] = (long)((v ^ 312) * w);
 tpad[313 & 3] = (long)((v ^ 313) * w);
 tpad[314 & 3] = (long)((v ^ 314) * w);
 tpad[315 & 3] = (long)((v ^ 315) * w);
 tpad[316 & 3] = (long)((v ^ 316) * w);
 tpad[317 & 3] = (long)((v ^ 317) * w);
 tpad[318 & 3] = (long)((v ^ 318) * w);
 tpad[319 & 3] = (long)((v ^ 319) * w);
 tpad[320 & 3] = (long)((v ^ 320) * w);
 tpad[321 & 3] = (long)((v ^ 321) * w);
 tpad[322 & 3] = (long)((v ^ 322) * w);
 tpad[323 & 3] = (long)((v ^ 323) * w);
 tpad[324 & 3] = (long)((v ^ 324) * w);
 tpad[325 & 3] = (long)((v ^ 325) * w);
 tpad[326 & 3] = (long)((v ^ 326) * w);
 tpad[327 & 3] = (long)((v ^ 327) * w);
 tpad[328 & 3] = (long)((v ^ 328) * w);
 tpad[329 & 3] = (long)((v ^ 329) * w);
 tpad[330 & 3] = (long)((v ^ 330) * w);
 tpad[331 & 3] = (long)((v ^ 331) * w);
 tpad[332 & 3] = (long)((v ^ 332) * w);
 tpad[333 & 3] = (long)((v ^ 333) * w);
 tpad[334 & 3] = (long)((v ^ 334) * w);
 tpad[335 & 3] = (long)((v ^ 335) * w);
 tpad[336 & 3] = (long)((v ^ 336) * w);
 tpad[337 & 3] = (long)((v ^ 337) * w);
 tpad[338 & 3] = (long)((v ^ 338) * w);
 tpad[339 & 3] = (long)((v ^ 339) * w);
 tpad[340 & 3] = (long)((v ^ 340) * w);
 tpad[341 & 3] = (long)((v ^ 341) * w);
 tpad[342 & 3] = (long)((v ^ 342) * w);
 tpad[343 & 3] = (long)((v ^ 343) * w);
 tpad[344 & 3] = (long)((v ^ 344) * w);
 tpad[345 & 3] = (long)((v ^ 345) * w);
 tpad[346 & 3] = (long)((v ^ 346) * w);
 tpad[347 & 3] = (long)((v ^ 347) * w);
 tpad[348 & 3] = (long)((v ^ 348) * w);
 tpad[349 & 3] = (long)((v ^ 349) * w);
 tpad[350 & 3] = (long)((v ^ 350) * w);
 tpad[351 & 3] = (long)((v ^ 351) * w);
 tpad[352 & 3] = (long)((v ^ 352) * w);
 tpad[353 & 3] = (long)((v ^ 353) * w);
 tpad[354 & 3] = (long)((v ^ 354) * w);
 tpad[355 & 3] = (long)((v ^ 355) * w);
 tpad[356 & 3] = (long)((v ^ 356) * w);
 tpad[357 & 3] = (long)((v ^ 357) * w);
 tpad[358 & 3] = (long)((v ^ 358) * w);
 tpad[359 & 3] = (long)((v ^ 359) * w);
 tpad[360 & 3] = (long)((v ^ 360) * w);
 tpad[361 & 3] = (long)((v ^ 361) * w);
 tpad[362 & 3] = (long)((v ^ 362) * w);
 tpad[363 & 3] = (long)((v ^ 363) * w);
 tpad[364 & 3] = (long)((v ^ 364) * w);
 tpad[365 & 3] = (long)((v ^ 365) * w);
 tpad[366 & 3] = (long)((v ^ 366) * w);
 tpad[367 & 3] = (long)((v ^ 367) * w);
 tpad[368 & 3] = (long)((v ^ 368) * w);
 tpad[369 & 3] = (long)((v ^ 369) * w);
 tpad[370 & 3] = (long)((v ^ 370) * w);
 tpad[371 & 3] = (long)((v ^ 371) * w);
 tpad[372 & 3] = (long)((v ^ 372) * w);
 tpad[373 & 3] = (long)((v ^ 373) * w);
 tpad[374 & 3] = (long)((v ^ 374) * w);
 tpad[375 & 3] = (long)((v ^ 375) * w);
 tpad[376 & 3] = (long)((v ^ 376) * w);
 tpad[377 & 3] = (long)((v ^ 377) * w);
 tpad[378 & 3] = (long)((v ^ 378) * w);
 tpad[379 & 3] = (long)((v ^ 379) * w);
 tpad[380 & 3] = (long)((v ^ 380) * w);
 tpad[381 & 3] = (long)((v ^ 381) * w);
 tpad[382 & 3] = (long)((v ^ 382) * w);
 tpad[383 & 3] = (long)((v ^ 383) * w);
 tpad[384 & 3] = (long)((v ^ 384) * w);
 tpad[385 & 3] = (long)((v ^ 385) * w);
 tpad[386 & 3] = (long)((v ^ 386) * w);
 tpad[387 & 3] = (long)((v ^ 387) * w);
 tpad[388 & 3] = (long)((v ^ 388) * w);
 tpad[389 & 3] = (long)((v ^ 389) * w);
 tpad[390 & 3] = (long)((v ^ 390) * w);
 tpad[391 & 3] = (long)((v ^ 391) * w);
 tpad[392 & 3] = (long)((v ^ 392) * w);
 tpad[393 & 3] = (long)((v ^ 393) * w);
 tpad[394 & 3] = (long)((v ^ 394) * w);
 tpad[395 & 3] = (long)((v ^ 395) * w);
 tpad[396 & 3] = (long)((v ^ 396) * w);
 tpad[397 & 3] = (long)((v ^ 397) * w);
 tpad[398 & 3] = (long)((v ^ 398) * w);
 tpad[399 & 3] = (long)((v ^ 399) * w);
 tpad[400 & 3] = (long)((v ^ 400) * w);
 tpad[401 & 3] = (long)((v ^ 401) * w);
 tpad[402 & 3] = (long)((v ^ 402) * w);
 tpad[403 & 3] = (long)((v ^ 403) * w);
 tpad[404 & 3] = (long)((v ^ 404) * w);
 tpad[405 & 3] = (long)((v ^ 405) * w);
 tpad[406 & 3] = (long)((v ^ 406) * w);
 tpad[407 & 3] = (long)((v ^ 407) * w);
 tpad[408 & 3] = (long)((v ^ 408) * w);
 tpad[409 & 3] = (long)((v ^ 409) * w);
 tpad[410 & 3] = (long)((v ^ 410) * w);
 tpad[411 & 3] = (long)((v ^ 411) * w);
 tpad[412 & 3] = (long)((v ^ 412) * w);
 tpad[413 & 3] = (long)((v ^ 413) * w);
 tpad[414 & 3] = (long)((v ^ 414) * w);
 tpad[415 & 3] = (long)((v ^ 415) * w);
 tpad[416 & 3] = (long)((v ^ 416) * w);
 tpad[417 & 3] = (long)((v ^ 417) * w);
 tpad[418 & 3] = (long)((v ^ 418) * w);
 tpad[419 & 3] = (long)((v ^ 419) * w);
 tpad[420 & 3] = (long)((v ^ 420) * w);
 tpad[421 & 3] = (long)((v ^ 421) * w);
 tpad[422 & 3] = (long)((v ^ 422) * w);
 tpad[423 & 3] = (long)((v ^ 423) * w);
 tpad[424 & 3] = (long)((v ^ 424) * w);
 tpad[425 & 3] = (long)((v ^ 425) * w);
 tpad[426 & 3] = (long)((v ^ 426) * w);
 tpad[427 & 3] = (long)((v ^ 427) * w);
 tpad[428 & 3] = (long)((v ^ 428) * w);
 tpad[429 & 3] = (long)((v ^ 429) * w);
 tpad[430 & 3] = (long)((v ^ 430) * w);
 tpad[431 & 3] = (long)((v ^ 431) * w);
 tpad[432 & 3] = (long)((v ^ 432) * w);
 tpad[433 & 3] = (long)((v ^ 433) * w);
 tpad[434 & 3] = (long)((v ^ 434) * w);
 tpad[435 & 3] = (long)((v ^ 435) * w);
 tpad[436 & 3] = (long)((v ^ 436) * w);
 tpad[437 & 3] = (long)((v ^ 437) * w);
 tpad[438 & 3] = (long)((v ^ 438) * w);
 tpad[439 & 3] = (long)((v ^ 439) * w);
 tpad[440 & 3] = (long)((v ^ 440) * w);
 tpad[441 & 3] = (long)((v ^ 441) * w);
 tpad[442 & 3] = (long)((v ^ 442) * w);
 tpad[443 & 3] = (long)((v ^ 443) * w);
 tpad[444 & 3] = (long)((v ^ 444) * w);
 tpad[445 & 3] = (long)((v ^ 445) * w);
 tpad[446 & 3] = (long)((v ^ 446) * w);
 tpad[447 & 3] = (long)((v ^ 447) * w);
 tpad[448 & 3] = (long)((v ^ 448) * w);
 tpad[449 & 3] = (long)((v ^ 449) * w);
 tpad[450 & 3] = (long)((v ^ 450) * w);
 tpad[451 & 3] = (long)((v ^ 451) * w);
 tpad[452 & 3] = (long)((v ^ 452) * w);
 tpad[453 & 3] = (long)((v ^ 453) * w);
 tpad[454 & 3] = (long)((v ^ 454) * w);
 tpad[455 & 3] = (long)((v ^ 455) * w);
 tpad[456 & 3] = (long)((v ^ 456) * w);
 tpad[457 & 3] = (long)((v ^ 457) * w);
 tpad[458 & 3] = (long)((v ^ 458) * w);
 tpad[459 & 3] = (long)((v ^ 459) * w);
 tpad[460 & 3] = (long)((v ^ 460) * w);
 tpad[461 & 3] = (long)((v ^ 461) * w);
 tpad[462 & 3] = (long)((v ^ 462) * w);
 tpad[463 & 3] = (long)((v ^ 463) * w);
 tpad[464 & 3] = (long)((v ^ 464) * w);
 tpad[465 & 3] = (long)((v ^ 465) * w);
 tpad[466 & 3] = (long)((v ^ 466) * w);
 tpad[467 & 3] = (long)((v ^ 467) * w);
 tpad[468 & 3] = (long)((v ^ 468) * w);
 tpad[469 & 3] = (long)((v ^ 469) * w);
 tpad[470 & 3] = (long)((v ^ 470) * w);
 tpad[471 & 3] = (long)((v ^ 471) * w);
 tpad[472 & 3] = (long)((v ^ 472) * w);
 tpad[473 & 3] = (long)((v ^ 473) * w);
 tpad[474 & 3] = (long)((v ^ 474) * w);
 tpad[475 & 3] = (long)((v ^ 475) * w);
 tpad[476 & 3] = (long)((v ^ 476) * w);
 tpad[477 & 3] = (long)((v ^ 477) * w);
 tpad[478 & 3] = (long)((v ^ 478) * w);
 tpad[479 & 3] = (long)((v ^ 479) * w);
 tpad[480 & 3] = (long)((v ^ 480) * w);
 tpad[481 & 3] = (long)((v ^ 481) * w);
 tpad[482 & 3] = (long)((v ^ 482) * w);
 tpad[483 & 3] = (long)((v ^ 483) * w);
 tpad[484 & 3] = (long)((v ^ 484) * w);
 tpad[485 & 3] = (long)((v ^ 485) * w);
 tpad[486 & 3] = (long)((v ^ 486) * w);
 tpad[487 & 3] = (long)((v ^ 487) * w);
 tpad[488 & 3] = (long)((v ^ 488) * w);
 tpad[489 & 3] = (long)((v ^ 489) * w);
 tpad[490 & 3] = (long)((v ^ 490) * w);
 tpad[491 & 3] = (long)((v ^ 491) * w);
 tpad[492 & 3] = (long)((v ^ 492) * w);
 tpad[493 & 3] = (long)((v ^ 493) * w);
 tpad[494 & 3] = (long)((v ^ 494) * w);
 tpad[495 & 3] = (long)((v ^ 495) * w);
 tpad[496 & 3] = (long)((v ^ 496) * w);
 tpad[497 & 3] = (long)((v ^ 497) * w);
 tpad[498 & 3] = (long)((v ^ 498) * w);
 tpad[499 & 3] = (long)((v ^ 499) * w);
 tpad[500 & 3] = (long)((v ^ 500) * w);
 tpad[501 & 3] = (long)((v ^ 501) * w);
 tpad[502 & 3] = (long)((v ^ 502) * w);
 tpad[503 & 3] = (long)((v ^ 503) * w);
 tpad[504 & 3] = (long)((v ^ 504) * w);
 tpad[505 & 3] = (long)((v ^ 505) * w);
 tpad[506 & 3] = (long)((v ^ 506) * w);
 tpad[507 & 3] = (long)((v ^ 507) * w);
 tpad[508 & 3] = (long)((v ^ 508) * w);
 tpad[509 & 3] = (long)((v ^ 509) * w);
 tpad[510 & 3] = (long)((v ^ 510) * w);
 tpad[511 & 3] = (long)((v ^ 511) * w);
 tpad[512 & 3] = (long)((v ^ 512) * w);
 tpad[513 & 3] = (long)((v ^ 513) * w);
 tpad[514 & 3] = (long)((v ^ 514) * w);
 tpad[515 & 3] = (long)((v ^ 515) * w);
 tpad[516 & 3] = (long)((v ^ 516) * w);
 tpad[517 & 3] = (long)((v ^ 517) * w);
 tpad[518 & 3] = (long)((v ^ 518) * w);
 tpad[519 & 3] = (long)((v ^ 519) * w);
 tpad[520 & 3] = (long)((v ^ 520) * w);
 tpad[521 & 3] = (long)((v ^ 521) * w);
 tpad[522 & 3] = (long)((v ^ 522) * w);
 tpad[523 & 3] = (long)((v ^ 523) * w);
 tpad[524 & 3] = (long)((v ^ 524) * w);
 tpad[525 & 3] = (long)((v ^ 525) * w);
 tpad[526 & 3] = (long)((v ^ 526) * w);
 tpad[527 & 3] = (long)((v ^ 527) * w);
 tpad[528 & 3] = (long)((v ^ 528) * w);
 tpad[529 & 3] = (long)((v ^ 529) * w);
 tpad[530 & 3] = (long)((v ^ 530) * w);
 tpad[531 & 3] = (long)((v ^ 531) * w);
 tpad[532 & 3] = (long)((v ^ 532) * w);
 tpad[533 & 3] = (long)((v ^ 533) * w);
 tpad[534 & 3] = (long)((v ^ 534) * w);
 tpad[535 & 3] = (long)((v ^ 535) * w);
 tpad[536 & 3] = (long)((v ^ 536) * w);
 tpad[537 & 3] = (long)((v ^ 537) * w);
 tpad[538 & 3] = (long)((v ^ 538) * w);
 tpad[539 & 3] = (long)((v ^ 539) * w);
 tpad[540 & 3] = (long)((v ^ 540) * w);
 tpad[541 & 3] = (long)((v ^ 541) * w);
 tpad[542 & 3] = (long)((v ^ 542) * w);
 tpad[543 & 3] = (long)((v ^ 543) * w);
 tpad[544 & 3] = (long)((v ^ 544) * w);
 tpad[545 & 3] = (long)((v ^ 545) * w);
 tpad[546 & 3] = (long)((v ^ 546) * w);
 tpad[547 & 3] = (long)((v ^ 547) * w);
 tpad[548 & 3] = (long)((v ^ 548) * w);
 tpad[549 & 3] = (long)((v ^ 549) * w);
 tpad[550 & 3] = (long)((v ^ 550) * w);
 tpad[551 & 3] = (long)((v ^ 551) * w);
 tpad[552 & 3] = (long)((v ^ 552) * w);
 tpad[553 & 3] = (long)((v ^ 553) * w);
 tpad[554 & 3] = (long)((v ^ 554) * w);
 tpad[555 & 3] = (long)((v ^ 555) * w);
 tpad[556 & 3] = (long)((v ^ 556) * w);
 tpad[557 & 3] = (long)((v ^ 557) * w);
 tpad[558 & 3] = (long)((v ^ 558) * w);
 tpad[559 & 3] = (long)((v ^ 559) * w);
 tpad[560 & 3] = (long)((v ^ 560) * w);
 tpad[561 & 3] = (long)((v ^ 561) * w);
 tpad[562 & 3] = (long)((v ^ 562) * w);
 tpad[563 & 3] = (long)((v ^ 563) * w);
 tpad[564 & 3] = (long)((v ^ 564) * w);
 tpad[565 & 3] = (long)((v ^ 565) * w);
 tpad[566 & 3] = (long)((v ^ 566) * w);
 tpad[567 & 3] = (long)((v ^ 567) * w);
 tpad[568 & 3] = (long)((v ^ 568) * w);
 tpad[569 & 3] = (long)((v ^ 569) * w);
 tpad[570 & 3] = (long)((v ^ 570) * w);
 tpad[571 & 3] = (long)((v ^ 571) * w);
 tpad[572 & 3] = (long)((v ^ 572) * w);
 tpad[573 & 3] = (long)((v ^ 573) * w);
 tpad[574 & 3] = (long)((v ^ 574) * w);
 tpad[575 & 3] = (long)((v ^ 575) * w);
 tpad[576 & 3] = (long)((v ^ 576) * w);
 tpad[577 & 3] = (long)((v ^ 577) * w);
 tpad[578 & 3] = (long)((v ^ 578) * w);
 tpad[579 & 3] = (long)((v ^ 579) * w);
 tpad[580 & 3] = (long)((v ^ 580) * w);
 tpad[581 & 3] = (long)((v ^ 581) * w);
 tpad[582 & 3] = (long)((v ^ 582) * w);
 tpad[583 & 3] = (long)((v ^ 583) * w);
 tpad[584 & 3] = (long)((v ^ 584) * w);
 tpad[585 & 3] = (long)((v ^ 585) * w);
 tpad[586 & 3] = (long)((v ^ 586) * w);
 tpad[587 & 3] = (long)((v ^ 587) * w);
 tpad[588 & 3] = (long)((v ^ 588) * w);
 tpad[589 & 3] = (long)((v ^ 589) * w);
 tpad[590 & 3] = (long)((v ^ 590) * w);
 tpad[591 & 3] = (long)((v ^ 591) * w);
 tpad[592 & 3] = (long)((v ^ 592) * w);
 tpad[593 & 3] = (long)((v ^ 593) * w);
 tpad[594 & 3] = (long)((v ^ 594) * w);
 tpad[595 & 3] = (long)((v ^ 595) * w);
 tpad[596 & 3] = (long)((v ^ 596) * w);
 tpad[597 & 3] = (long)((v ^ 597) * w);
 tpad[598 & 3] = (long)((v ^ 598) * w);
 tpad[599 & 3] = (long)((v ^ 599) * w);
 tpad[600 & 3] = (long)((v ^ 600) * w);
 tpad[601 & 3] = (long)((v ^ 601) * w);
 tpad[602 & 3] = (long)((v ^ 602) * w);
 tpad[603 & 3] = (long)((v ^ 603) * w);
 tpad[604 & 3] = (long)((v ^ 604) * w);
 tpad[605 & 3] = (long)((v ^ 605) * w);
 tpad[606 & 3] = (long)((v ^ 606) * w);
 tpad[607 & 3] = (long)((v ^ 607) * w);
 tpad[608 & 3] = (long)((v ^ 608) * w);
 tpad[609 & 3] = (long)((v ^ 609) * w);
 tpad[610 & 3] = (long)((v ^ 610) * w);
 tpad[611 & 3] = (long)((v ^ 611) * w);
 tpad[612 & 3] = (long)((v ^ 612) * w);
 tpad[613 & 3] = (long)((v ^ 613) * w);
 tpad[614 & 3] = (long)((v ^ 614) * w);
 tpad[615 & 3] = (long)((v ^ 615) * w);
 tpad[616 & 3] = (long)((v ^ 616) * w);
 tpad[617 & 3] = (long)((v ^ 617) * w);
 tpad[618 & 3] = (long)((v ^ 618) * w);
 tpad[619 & 3] = (long)((v ^ 619) * w);
 tpad[620 & 3] = (long)((v ^ 620) * w);
 tpad[621 & 3] = (long)((v ^ 621) * w);
 tpad[622 & 3] = (long)((v ^ 622) * w);
 tpad[623 & 3] = (long)((v ^ 623) * w);
 tpad[624 & 3] = (long)((v ^ 624) * w);
 tpad[625 & 3] = (long)((v ^ 625) * w);
 tpad[626 & 3] = (long)((v ^ 626) * w);
 tpad[627 & 3] = (long)((v ^ 627) * w);
 tpad[628 & 3] = (long)((v ^ 628) * w);
 tpad[629 & 3] = (long)((v ^ 629) * w);
 tpad[630 & 3] = (long)((v ^ 630) * w);
 tpad[631 & 3] = (long)((v ^ 631) * w);
 tpad[632 & 3] = (long)((v ^ 632) * w);
 tpad[633 & 3] = (long)((v ^ 633) * w);
 tpad[634 & 3] = (long)((v ^ 634) * w);
 tpad[635 & 3] = (long)((v ^ 635) * w);
 tpad[636 & 3] = (long)((v ^ 636) * w);
 tpad[637 & 3] = (long)((v ^ 637) * w);
 tpad[638 & 3] = (long)((v ^ 638) * w);
 tpad[639 & 3] = (long)((v ^ 639) * w);
 tpad[640 & 3] = (long)((v ^ 640) * w);
 tpad[641 & 3] = (long)((v ^ 641) * w);
 tpad[642 & 3] = (long)((v ^ 642) * w);
 tpad[643 & 3] = (long)((v ^ 643) * w);
 tpad[644 & 3] = (long)((v ^ 644) * w);
 tpad[645 & 3] = (long)((v ^ 645) * w);
 tpad[646 & 3] = (long)((v ^ 646) * w);
 tpad[647 & 3] = (long)((v ^ 647) * w);
 tpad[648 & 3] = (long)((v ^ 648) * w);
 tpad[649 & 3] = (long)((v ^ 649) * w);
 tpad[650 & 3] = (long)((v ^ 650) * w);
 tpad[651 & 3] = (long)((v ^ 651) * w);
 tpad[652 & 3] = (long)((v ^ 652) * w);
 tpad[653 & 3] = (long)((v ^ 653) * w);
 tpad[654 & 3] = (long)((v ^ 654) * w);
 tpad[655 & 3] = (long)((v ^ 655) * w);
 tpad[656 & 3] = (long)((v ^ 656) * w);
 tpad[657 & 3] = (long)((v ^ 657) * w);
 tpad[658 & 3] = (long)((v ^ 658) * w);
 tpad[659 & 3] = (long)((v ^ 659) * w);
 tpad[660 & 3] = (long)((v ^ 660) * w);
 tpad[661 & 3] = (long)((v ^ 661) * w);
 tpad[662 & 3] = (long)((v ^ 662) * w);
 tpad[663 & 3] = (long)((v ^ 663) * w);
 tpad[664 & 3] = (long)((v ^ 664) * w);
 tpad[665 & 3] = (long)((v ^ 665) * w);
 tpad[666 & 3] = (long)((v ^ 666) * w);
 tpad[667 & 3] = (long)((v ^ 667) * w);
 tpad[668 & 3] = (long)((v ^ 668) * w);
 tpad[669 & 3] = (long)((v ^ 669) * w);
 tpad[670 & 3] = (long)((v ^ 670) * w);
 tpad[671 & 3] = (long)((v ^ 671) * w);
 tpad[672 & 3] = (long)((v ^ 672) * w);
 tpad[673 & 3] = (long)((v ^ 673) * w);
 tpad[674 & 3] = (long)((v ^ 674) * w);
 tpad[675 & 3] = (long)((v ^ 675) * w);
 tpad[676 & 3] = (long)((v ^ 676) * w);
 tpad[677 & 3] = (long)((v ^ 677) * w);
 tpad[678 & 3] = (long)((v ^ 678) * w);
 tpad[679 & 3] = (long)((v ^ 679) * w);
 tpad[680 & 3] = (long)((v ^ 680) * w);
 tpad[681 & 3] = (long)((v ^ 681) * w);
 tpad[682 & 3] = (long)((v ^ 682) * w);
 tpad[683 & 3] = (long)((v ^ 683) * w);
 tpad[684 & 3] = (long)((v ^ 684) * w);
 tpad[685 & 3] = (long)((v ^ 685) * w);
 tpad[686 & 3] = (long)((v ^ 686) * w);
 tpad[687 & 3] = (long)((v ^ 687) * w);
 tpad[688 & 3] = (long)((v ^ 688) * w);
 tpad[689 & 3] = (long)((v ^ 689) * w);
 tpad[690 & 3] = (long)((v ^ 690) * w);
 tpad[691 & 3] = (long)((v ^ 691) * w);
 tpad[692 & 3] = (long)((v ^ 692) * w);
 tpad[693 & 3] = (long)((v ^ 693) * w);
 tpad[694 & 3] = (long)((v ^ 694) * w);
 tpad[695 & 3] = (long)((v ^ 695) * w);
 tpad[696 & 3] = (long)((v ^ 696) * w);
 tpad[697 & 3] = (long)((v ^ 697) * w);
 tpad[698 & 3] = (long)((v ^ 698) * w);
 tpad[699 & 3] = (long)((v ^ 699) * w);
 tpad[700 & 3] = (long)((v ^ 700) * w);
 tpad[701 & 3] = (long)((v ^ 701) * w);
 tpad[702 & 3] = (long)((v ^ 702) * w);
 tpad[703 & 3] = (long)((v ^ 703) * w);
 tpad[704 & 3] = (long)((v ^ 704) * w);
 tpad[705 & 3] = (long)((v ^ 705) * w);
 tpad[706 & 3] = (long)((v ^ 706) * w);
 tpad[707 & 3] = (long)((v ^ 707) * w);
 tpad[708 & 3] = (long)((v ^ 708) * w);
 tpad[709 & 3] = (long)((v ^ 709) * w);
 tpad[710 & 3] = (long)((v ^ 710) * w);
 tpad[711 & 3] = (long)((v ^ 711) * w);
 tpad[712 & 3] = (long)((v ^ 712) * w);
 tpad[713 & 3] = (long)((v ^ 713) * w);
 tpad[714 & 3] = (long)((v ^ 714) * w);
 tpad[715 & 3] = (long)((v ^ 715) * w);
 tpad[716 & 3] = (long)((v ^ 716) * w);
 tpad[717 & 3] = (long)((v ^ 717) * w);
 tpad[718 & 3] = (long)((v ^ 718) * w);
 tpad[719 & 3] = (long)((v ^ 719) * w);
 tpad[720 & 3] = (long)((v ^ 720) * w);
 tpad[721 & 3] = (long)((v ^ 721) * w);
 tpad[722 & 3] = (long)((v ^ 722) * w);
 tpad[723 & 3] = (long)((v ^ 723) * w);
 tpad[724 & 3] = (long)((v ^ 724) * w);
 tpad[725 & 3] = (long)((v ^ 725) * w);
 tpad[726 & 3] = (long)((v ^ 726) * w);
 tpad[727 & 3] = (long)((v ^ 727) * w);
 tpad[728 & 3] = (long)((v ^ 728) * w);
 tpad[729 & 3] = (long)((v ^ 729) * w);
 tpad[730 & 3] = (long)((v ^ 730) * w);
 tpad[731 & 3] = (long)((v ^ 731) * w);
 tpad[732 & 3] = (long)((v ^ 732) * w);
 tpad[733 & 3] = (long)((v ^ 733) * w);
 tpad[734 & 3] = (long)((v ^ 734) * w);
 tpad[735 & 3] = (long)((v ^ 735) * w);
 tpad[736 & 3] = (long)((v ^ 736) * w);
 tpad[737 & 3] = (long)((v ^ 737) * w);
 tpad[738 & 3] = (long)((v ^ 738) * w);
 tpad[739 & 3] = (long)((v ^ 739) * w);
 tpad[740 & 3] = (long)((v ^ 740) * w);
 tpad[741 & 3] = (long)((v ^ 741) * w);
 tpad[742 & 3] = (long)((v ^ 742) * w);
 tpad[743 & 3] = (long)((v ^ 743) * w);
 tpad[744 & 3] = (long)((v ^ 744) * w);
 tpad[745 & 3] = (long)((v ^ 745) * w);
 tpad[746 & 3] = (long)((v ^ 746) * w);
 tpad[747 & 3] = (long)((v ^ 747) * w);
 tpad[748 & 3] = (long)((v ^ 748) * w);
 tpad[749 & 3] = (long)((v ^ 749) * w);
 tpad[750 & 3] = (long)((v ^ 750) * w);
 tpad[751 & 3] = (long)((v ^ 751) * w);
 tpad[752 & 3] = (long)((v ^ 752) * w);
 tpad[753 & 3] = (long)((v ^ 753) * w);
 tpad[754 & 3] = (long)((v ^ 754) * w);
 tpad[755 & 3] = (long)((v ^ 755) * w);
 tpad[756 & 3] = (long)((v ^ 756) * w);
 tpad[757 & 3] = (long)((v ^ 757) * w);
 tpad[758 & 3] = (long)((v ^ 758) * w);
 tpad[759 & 3] = (long)((v ^ 759) * w);
 tpad[760 & 3] = (long)((v ^ 760) * w);
 tpad[761 & 3] = (long)((v ^ 761) * w);
 tpad[762 & 3] = (long)((v ^ 762) * w);
 tpad[763 & 3] = (long)((v ^ 763) * w);
 tpad[764 & 3] = (long)((v ^ 764) * w);
 tpad[765 & 3] = (long)((v ^ 765) * w);
 tpad[766 & 3] = (long)((v ^ 766) * w);
 tpad[767 & 3] = (long)((v ^ 767) * w);
 tpad[768 & 3] = (long)((v ^ 768) * w);
 tpad[769 & 3] = (long)((v ^ 769) * w);
 tpad[770 & 3] = (long)((v ^ 770) * w);
 tpad[771 & 3] = (long)((v ^ 771) * w);
 tpad[772 & 3] = (long)((v ^ 772) * w);
 tpad[773 & 3] = (long)((v ^ 773) * w);
 tpad[774 & 3] = (long)((v ^ 774) * w);
 tpad[775 & 3] = (long)((v ^ 775) * w);
 tpad[776 & 3] = (long)((v ^ 776) * w);
 tpad[777 & 3] = (long)((v ^ 777) * w);
 tpad[778 & 3] = (long)((v ^ 778) * w);
 tpad[779 & 3] = (long)((v ^ 779) * w);
 tpad[780 & 3] = (long)((v ^ 780) * w);
 tpad[781 & 3] = (long)((v ^ 781) * w);
 tpad[782 & 3] = (long)((v ^ 782) * w);
 tpad[783 & 3] = (long)((v ^ 783) * w);
 tpad[784 & 3] = (long)((v ^ 784) * w);
 tpad[785 & 3] = (long)((v ^ 785) * w);
 tpad[786 & 3] = (long)((v ^ 786) * w);
 tpad[787 & 3] = (long)((v ^ 787) * w);
 tpad[788 & 3] = (long)((v ^ 788) * w);
 tpad[789 & 3] = (long)((v ^ 789) * w);
 tpad[790 & 3] = (long)((v ^ 790) * w);
 tpad[791 & 3] = (long)((v ^ 791) * w);
 tpad[792 & 3] = (long)((v ^ 792) * w);
 tpad[793 & 3] = (long)((v ^ 793) * w);
 tpad[794 & 3] = (long)((v ^ 794) * w);
 tpad[795 & 3] = (long)((v ^ 795) * w);
 tpad[796 & 3] = (long)((v ^ 796) * w);
 tpad[797 & 3] = (long)((v ^ 797) * w);
 tpad[798 & 3] = (long)((v ^ 798) * w);
 tpad[799 & 3] = (long)((v ^ 799) * w);
 tpad[800 & 3] = (long)((v ^ 800) * w);
 tpad[801 & 3] = (long)((v ^ 801) * w);
 tpad[802 & 3] = (long)((v ^ 802) * w);
 tpad[803 & 3] = (long)((v ^ 803) * w);
 tpad[804 & 3] = (long)((v ^ 804) * w);
 tpad[805 & 3] = (long)((v ^ 805) * w);
 tpad[806 & 3] = (long)((v ^ 806) * w);
 tpad[807 & 3] = (long)((v ^ 807) * w);
 tpad[808 & 3] = (long)((v ^ 808) * w);
 tpad[809 & 3] = (long)((v ^ 809) * w);
 tpad[810 & 3] = (long)((v ^ 810) * w);
 tpad[811 & 3] = (long)((v ^ 811) * w);
 tpad[812 & 3] = (long)((v ^ 812) * w);
 tpad[813 & 3] = (long)((v ^ 813) * w);
 tpad[814 & 3] = (long)((v ^ 814) * w);
 tpad[815 & 3] = (long)((v ^ 815) * w);
 tpad[816 & 3] = (long)((v ^ 816) * w);
 tpad[817 & 3] = (long)((v ^ 817) * w);
 tpad[818 & 3] = (long)((v ^ 818) * w);
 tpad[819 & 3] = (long)((v ^ 819) * w);
 tpad[820 & 3] = (long)((v ^ 820) * w);
 tpad[821 & 3] = (long)((v ^ 821) * w);
 tpad[822 & 3] = (long)((v ^ 822) * w);
 tpad[823 & 3] = (long)((v ^ 823) * w);
 tpad[824 & 3] = (long)((v ^ 824) * w);
 tpad[825 & 3] = (long)((v ^ 825) * w);
 tpad[826 & 3] = (long)((v ^ 826) * w);
 tpad[827 & 3] = (long)((v ^ 827) * w);
 tpad[828 & 3] = (long)((v ^ 828) * w);
 tpad[829 & 3] = (long)((v ^ 829) * w);
 tpad[830 & 3] = (long)((v ^ 830) * w);
 tpad[831 & 3] = (long)((v ^ 831) * w);
 tpad[832 & 3] = (long)((v ^ 832) * w);
 tpad[833 & 3] = (long)((v ^ 833) * w);
 tpad[834 & 3] = (long)((v ^ 834) * w);
 tpad[835 & 3] = (long)((v ^ 835) * w);
 tpad[836 & 3] = (long)((v ^ 836) * w);
 tpad[837 & 3] = (long)((v ^ 837) * w);
 tpad[838 & 3] = (long)((v ^ 838) * w);
 tpad[839 & 3] = (long)((v ^ 839) * w);
 tpad[840 & 3] = (long)((v ^ 840) * w);
 tpad[841 & 3] = (long)((v ^ 841) * w);
 tpad[842 & 3] = (long)((v ^ 842) * w);
 tpad[843 & 3] = (long)((v ^ 843) * w);
 tpad[844 & 3] = (long)((v ^ 844) * w);
 tpad[845 & 3] = (long)((v ^ 845) * w);
 tpad[846 & 3] = (long)((v ^ 846) * w);
 tpad[847 & 3] = (long)((v ^ 847) * w);
 tpad[848 & 3] = (long)((v ^ 848) * w);
 tpad[849 & 3] = (long)((v ^ 849) * w);
 tpad[850 & 3] = (long)((v ^ 850) * w);
 tpad[851 & 3] = (long)((v ^ 851) * w);
 tpad[852 & 3] = (long)((v ^ 852) * w);
 tpad[853 & 3] = (long)((v ^ 853) * w);
 tpad[854 & 3] = (long)((v ^ 854) * w);
 tpad[855 & 3] = (long)((v ^ 855) * w);
 tpad[856 & 3] = (long)((v ^ 856) * w);
 tpad[857 & 3] = (long)((v ^ 857) * w);
 tpad[858 & 3] = (long)((v ^ 858) * w);
 tpad[859 & 3] = (long)((v ^ 859) * w);
 tpad[860 & 3] = (long)((v ^ 860) * w);
 tpad[861 & 3] = (long)((v ^ 861) * w);
 tpad[862 & 3] = (long)((v ^ 862) * w);
 tpad[863 & 3] = (long)((v ^ 863) * w);
 tpad[864 & 3] = (long)((v ^ 864) * w);
 tpad[865 & 3] = (long)((v ^ 865) * w);
 tpad[866 & 3] = (long)((v ^ 866) * w);
 tpad[867 & 3] = (long)((v ^ 867) * w);
 tpad[868 & 3] = (long)((v ^ 868) * w);
 tpad[869 & 3] = (long)((v ^ 869) * w);
 tpad[870 & 3] = (long)((v ^ 870) * w);
 tpad[871 & 3] = (long)((v ^ 871) * w);
 tpad[872 & 3] = (long)((v ^ 872) * w);
 tpad[873 & 3] = (long)((v ^ 873) * w);
 tpad[874 & 3] = (long)((v ^ 874) * w);
 tpad[875 & 3] = (long)((v ^ 875) * w);
 tpad[876 & 3] = (long)((v ^ 876) * w);
 tpad[877 & 3] = (long)((v ^ 877) * w);
 tpad[878 & 3] = (long)((v ^ 878) * w);
 tpad[879 & 3] = (long)((v ^ 879) * w);
 tpad[880 & 3] = (long)((v ^ 880) * w);
 tpad[881 & 3] = (long)((v ^ 881) * w);
 tpad[882 & 3] = (long)((v ^ 882) * w);
 tpad[883 & 3] = (long)((v ^ 883) * w);
 tpad[884 & 3] = (long)((v ^ 884) * w);
 tpad[885 & 3] = (long)((v ^ 885) * w);
 tpad[886 & 3] = (long)((v ^ 886) * w);
 tpad[887 & 3] = (long)((v ^ 887) * w);
 tpad[888 & 3] = (long)((v ^ 888) * w);
 tpad[889 & 3] = (long)((v ^ 889) * w);
 tpad[890 & 3] = (long)((v ^ 890) * w);
 tpad[891 & 3] = (long)((v ^ 891) * w);
 tpad[892 & 3] = (long)((v ^ 892) * w);
 tpad[893 & 3] = (long)((v ^ 893) * w);
 tpad[894 & 3] = (long)((v ^ 894) * w);
 tpad[895 & 3] = (long)((v ^ 895) * w);
 tpad[896 & 3] = (long)((v ^ 896) * w);
 tpad[897 & 3] = (long)((v ^ 897) * w);
 tpad[898 & 3] = (long)((v ^ 898) * w);
 tpad[899 & 3] = (long)((v ^ 899) * w);
 tpad[900 & 3] = (long)((v ^ 900) * w);
 tpad[901 & 3] = (long)((v ^ 901) * w);
 tpad[902 & 3] = (long)((v ^ 902) * w);
 tpad[903 & 3] = (long)((v ^ 903) * w);
 tpad[904 & 3] = (long)((v ^ 904) * w);
 tpad[905 & 3] = (long)((v ^ 905) * w);
 tpad[906 & 3] = (long)((v ^ 906) * w);
 tpad[907 & 3] = (long)((v ^ 907) * w);
 tpad[908 & 3] = (long)((v ^ 908) * w);
 tpad[909 & 3] = (long)((v ^ 909) * w);
 tpad[910 & 3] = (long)((v ^ 910) * w);
 tpad[911 & 3] = (long)((v ^ 911) * w);
 tpad[912 & 3] = (long)((v ^ 912) * w);
 tpad[913 & 3] = (long)((v ^ 913) * w);
 tpad[914 & 3] = (long)((v ^ 914) * w);
 tpad[915 & 3] = (long)((v ^ 915) * w);
 tpad[916 & 3] = (long)((v ^ 916) * w);
 tpad[917 & 3] = (long)((v ^ 917) * w);
 tpad[918 & 3] = (long)((v ^ 918) * w);
 tpad[919 & 3] = (long)((v ^ 919) * w);
 tpad[920 & 3] = (long)((v ^ 920) * w);
 tpad[921 & 3] = (long)((v ^ 921) * w);
 tpad[922 & 3] = (long)((v ^ 922) * w);
 tpad[923 & 3] = (long)((v ^ 923) * w);
 tpad[924 & 3] = (long)((v ^ 924) * w);
 tpad[925 & 3] = (long)((v ^ 925) * w);
 tpad[926 & 3] = (long)((v ^ 926) * w);
 tpad[927 & 3] = (long)((v ^ 927) * w);
 tpad[928 & 3] = (long)((v ^ 928) * w);
 tpad[929 & 3] = (long)((v ^ 929) * w);
 tpad[930 & 3] = (long)((v ^ 930) * w);
 tpad[931 & 3] = (long)((v ^ 931) * w);
 tpad[932 & 3] = (long)((v ^ 932) * w);
 tpad[933 & 3] = (long)((v ^ 933) * w);
 tpad[934 & 3] = (long)((v ^ 934) * w);
 tpad[935 & 3] = (long)((v ^ 935) * w);
 tpad[936 & 3] = (long)((v ^ 936) * w);
 tpad[937 & 3] = (long)((v ^ 937) * w);
 tpad[938 & 3] = (long)((v ^ 938) * w);
 tpad[939 & 3] = (long)((v ^ 939) * w);
 tpad[940 & 3] = (long)((v ^ 940) * w);
 tpad[941 & 3] = (long)((v ^ 941) * w);
 tpad[942 & 3] = (long)((v ^ 942) * w);
 tpad[943 & 3] = (long)((v ^ 943) * w);
 tpad[944 & 3] = (long)((v ^ 944) * w);
 tpad[945 & 3] = (long)((v ^ 945) * w);
 tpad[946 & 3] = (long)((v ^ 946) * w);
 tpad[947 & 3] = (long)((v ^ 947) * w);
 tpad[948 & 3] = (long)((v ^ 948) * w);
 tpad[949 & 3] = (long)((v ^ 949) * w);
 tpad[950 & 3] = (long)((v ^ 950) * w);
 tpad[951 & 3] = (long)((v ^ 951) * w);
 tpad[952 & 3] = (long)((v ^ 952) * w);
 tpad[953 & 3] = (long)((v ^ 953) * w);
 tpad[954 & 3] = (long)((v ^ 954) * w);
 tpad[955 & 3] = (long)((v ^ 955) * w);
 tpad[956 & 3] = (long)((v ^ 956) * w);
 tpad[957 & 3] = (long)((v ^ 957) * w);
 tpad[958 & 3] = (long)((v ^ 958) * w);
 tpad[959 & 3] = (long)((v ^ 959) * w);
 tpad[960 & 3] = (long)((v ^ 960) * w);
 tpad[961 & 3] = (long)((v ^ 961) * w);
 tpad[962 & 3] = (long)((v ^ 962) * w);
 tpad[963 & 3] = (long)((v ^ 963) * w);
 tpad[964 & 3] = (long)((v ^ 964) * w);
 tpad[965 & 3] = (long)((v ^ 965) * w);
 tpad[966 & 3] = (long)((v ^ 966) * w);
 tpad[967 & 3] = (long)((v ^ 967) * w);
 tpad[968 & 3] = (long)((v ^ 968) * w);
 tpad[969 & 3] = (long)((v ^ 969) * w);
 tpad[970 & 3] = (long)((v ^ 970) * w);
 tpad[971 & 3] = (long)((v ^ 971) * w);
 tpad[972 & 3] = (long)((v ^ 972) * w);
 tpad[973 & 3] = (long)((v ^ 973) * w);
 tpad[974 & 3] = (long)((v ^ 974) * w);
 tpad[975 & 3] = (long)((v ^ 975) * w);
 tpad[976 & 3] = (long)((v ^ 976) * w);
 tpad[977 & 3] = (long)((v ^ 977) * w);
 tpad[978 & 3] = (long)((v ^ 978) * w);
 tpad[979 & 3] = (long)((v ^ 979) * w);
 tpad[980 & 3] = (long)((v ^ 980) * w);
 tpad[981 & 3] = (long)((v ^ 981) * w);
 tpad[982 & 3] = (long)((v ^ 982) * w);
 tpad[983 & 3] = (long)((v ^ 983) * w);
 tpad[984 & 3] = (long)((v ^ 984) * w);
 tpad[985 & 3] = (long)((v ^ 985) * w);
 tpad[986 & 3] = (long)((v ^ 986) * w);
 tpad[987 & 3] = (long)((v ^ 987) * w);
 tpad[988 & 3] = (long)((v ^ 988) * w);
 tpad[989 & 3] = (long)((v ^ 989) * w);
 tpad[990 & 3] = (long)((v ^ 990) * w);
 tpad[991 & 3] = (long)((v ^ 991) * w);
 tpad[992 & 3] = (long)((v ^ 992) * w);
 tpad[993 & 3] = (long)((v ^ 993) * w);
 tpad[994 & 3] = (long)((v ^ 994) * w);
 tpad[995 & 3] = (long)((v ^ 995) * w);
 tpad[996 & 3] = (long)((v ^ 996) * w);
 tpad[997 & 3] = (long)((v ^ 997) * w);
 tpad[998 & 3] = (long)((v ^ 998) * w);
 tpad[999 & 3] = (long)((v ^ 999) * w);
 tpad[1000 & 3] = (long)((v ^ 1000) * w);
 tpad[1001 & 3] = (long)((v ^ 1001) * w);
 tpad[1002 & 3] = (long)((v ^ 1002) * w);
 tpad[1003 & 3] = (long)((v ^ 1003) * w);
 tpad[1004 & 3] = (long)((v ^ 1004) * w);
 tpad[1005 & 3] = (long)((v ^ 1005) * w);
 tpad[1006 & 3] = (long)((v ^ 1006) * w);
 tpad[1007 & 3] = (long)((v ^ 1007) * w);
 tpad[1008 & 3] = (long)((v ^ 1008) * w);
 tpad[1009 & 3] = (long)((v ^ 1009) * w);
 tpad[1010 & 3] = (long)((v ^ 1010) * w);
 tpad[1011 & 3] = (long)((v ^ 1011) * w);
 tpad[1012 & 3] = (long)((v ^ 1012) * w);
 tpad[1013 & 3] = (long)((v ^ 1013) * w);
 tpad[1014 & 3] = (long)((v ^ 1014) * w);
 tpad[1015 & 3] = (long)((v ^ 1015) * w);
 tpad[1016 & 3] = (long)((v ^ 1016) * w);
 tpad[1017 & 3] = (long)((v ^ 1017) * w);
 tpad[1018 & 3] = (long)((v ^ 1018) * w);
 tpad[1019 & 3] = (long)((v ^ 1019) * w);
 tpad[1020 & 3] = (long)((v ^ 1020) * w);
 tpad[1021 & 3] = (long)((v ^ 1021) * w);
 tpad[1022 & 3] = (long)((v ^ 1022) * w);
 tpad[1023 & 3] = (long)((v ^ 1023) * w);
 tpad[1024 & 3] = (long)((v ^ 1024) * w);
 tpad[1025 & 3] = (long)((v ^ 1025) * w);
 tpad[1026 & 3] = (long)((v ^ 1026) * w);
 tpad[1027 & 3] = (long)((v ^ 1027) * w);
 tpad[1028 & 3] = (long)((v ^ 1028) * w);
 tpad[1029 & 3] = (long)((v ^ 1029) * w);
 tpad[1030 & 3] = (long)((v ^ 1030) * w);
 tpad[1031 & 3] = (long)((v ^ 1031) * w);
 tpad[1032 & 3] = (long)((v ^ 1032) * w);
 tpad[1033 & 3] = (long)((v ^ 1033) * w);
 tpad[1034 & 3] = (long)((v ^ 1034) * w);
 tpad[1035 & 3] = (long)((v ^ 1035) * w);
 tpad[1036 & 3] = (long)((v ^ 1036) * w);
 tpad[1037 & 3] = (long)((v ^ 1037) * w);
 tpad[1038 & 3] = (long)((v ^ 1038) * w);
 tpad[1039 & 3] = (long)((v ^ 1039) * w);
 tpad[1040 & 3] = (long)((v ^ 1040) * w);
 tpad[1041 & 3] = (long)((v ^ 1041) * w);
 tpad[1042 & 3] = (long)((v ^ 1042) * w);
 tpad[1043 & 3] = (long)((v ^ 1043) * w);
 tpad[1044 & 3] = (long)((v ^ 1044) * w);
 tpad[1045 & 3] = (long)((v ^ 1045) * w);
 tpad[1046 & 3] = (long)((v ^ 1046) * w);
 tpad[1047 & 3] = (long)((v ^ 1047) * w);
 tpad[1048 & 3] = (long)((v ^ 1048) * w);
 tpad[1049 & 3] = (long)((v ^ 1049) * w);
 tpad[1050 & 3] = (long)((v ^ 1050) * w);
 tpad[1051 & 3] = (long)((v ^ 1051) * w);
 tpad[1052 & 3] = (long)((v ^ 1052) * w);
 tpad[1053 & 3] = (long)((v ^ 1053) * w);
 tpad[1054 & 3] = (long)((v ^ 1054) * w);
 tpad[1055 & 3] = (long)((v ^ 1055) * w);
 tpad[1056 & 3] = (long)((v ^ 1056) * w);
 tpad[1057 & 3] = (long)((v ^ 1057) * w);
 tpad[1058 & 3] = (long)((v ^ 1058) * w);
 tpad[1059 & 3] = (long)((v ^ 1059) * w);
 tpad[1060 & 3] = (long)((v ^ 1060) * w);
 tpad[1061 & 3] = (long)((v ^ 1061) * w);
 tpad[1062 & 3] = (long)((v ^ 1062) * w);
 tpad[1063 & 3] = (long)((v ^ 1063) * w);
 tpad[1064 & 3] = (long)((v ^ 1064) * w);
 tpad[1065 & 3] = (long)((v ^ 1065) * w);
 tpad[1066 & 3] = (long)((v ^ 1066) * w);
 tpad[1067 & 3] = (long)((v ^ 1067) * w);
 tpad[1068 & 3] = (long)((v ^ 1068) * w);
 tpad[1069 & 3] = (long)((v ^ 1069) * w);
 tpad[1070 & 3] = (long)((v ^ 1070) * w);
 tpad[1071 & 3] = (long)((v ^ 1071) * w);
 tpad[1072 & 3] = (long)((v ^ 1072) * w);
 tpad[1073 & 3] = (long)((v ^ 1073) * w);
 tpad[1074 & 3] = (long)((v ^ 1074) * w);
 tpad[1075 & 3] = (long)((v ^ 1075) * w);
 tpad[1076 & 3] = (long)((v ^ 1076) * w);
 tpad[1077 & 3] = (long)((v ^ 1077) * w);
 tpad[1078 & 3] = (long)((v ^ 1078) * w);
 tpad[1079 & 3] = (long)((v ^ 1079) * w);
 tpad[1080 & 3] = (long)((v ^ 1080) * w);
 tpad[1081 & 3] = (long)((v ^ 1081) * w);
 tpad[1082 & 3] = (long)((v ^ 1082) * w);
 tpad[1083 & 3] = (long)((v ^ 1083) * w);
 tpad[1084 & 3] = (long)((v ^ 1084) * w);
 tpad[1085 & 3] = (long)((v ^ 1085) * w);
 tpad[1086 & 3] = (long)((v ^ 1086) * w);
 tpad[1087 & 3] = (long)((v ^ 1087) * w);
 tpad[1088 & 3] = (long)((v ^ 1088) * w);
 tpad[1089 & 3] = (long)((v ^ 1089) * w);
 tpad[1090 & 3] = (long)((v ^ 1090) * w);
 tpad[1091 & 3] = (long)((v ^ 1091) * w);
 tpad[1092 & 3] = (long)((v ^ 1092) * w);
 tpad[1093 & 3] = (long)((v ^ 1093) * w);
 tpad[1094 & 3] = (long)((v ^ 1094) * w);
 tpad[1095 & 3] = (long)((v ^ 1095) * w);
 tpad[1096 & 3] = (long)((v ^ 1096) * w);
 tpad[1097 & 3] = (long)((v ^ 1097) * w);
 tpad[1098 & 3] = (long)((v ^ 1098) * w);
 tpad[1099 & 3] = (long)((v ^ 1099) * w);
 tpad[1100 & 3] = (long)((v ^ 1100) * w);
 tpad[1101 & 3] = (long)((v ^ 1101) * w);
 tpad[1102 & 3] = (long)((v ^ 1102) * w);
 tpad[1103 & 3] = (long)((v ^ 1103) * w);
 tpad[1104 & 3] = (long)((v ^ 1104) * w);
 tpad[1105 & 3] = (long)((v ^ 1105) * w);
 tpad[1106 & 3] = (long)((v ^ 1106) * w);
 tpad[1107 & 3] = (long)((v ^ 1107) * w);
 tpad[1108 & 3] = (long)((v ^ 1108) * w);
 tpad[1109 & 3] = (long)((v ^ 1109) * w);
 tpad[1110 & 3] = (long)((v ^ 1110) * w);
 tpad[1111 & 3] = (long)((v ^ 1111) * w);
 tpad[1112 & 3] = (long)((v ^ 1112) * w);
 tpad[1113 & 3] = (long)((v ^ 1113) * w);
 tpad[1114 & 3] = (long)((v ^ 1114) * w);
 tpad[1115 & 3] = (long)((v ^ 1115) * w);
 tpad[1116 & 3] = (long)((v ^ 1116) * w);
 tpad[1117 & 3] = (long)((v ^ 1117) * w);
 tpad[1118 & 3] = (long)((v ^ 1118) * w);
 tpad[1119 & 3] = (long)((v ^ 1119) * w);
 tpad[1120 & 3] = (long)((v ^ 1120) * w);
 tpad[1121 & 3] = (long)((v ^ 1121) * w);
 tpad[1122 & 3] = (long)((v ^ 1122) * w);
 tpad[1123 & 3] = (long)((v ^ 1123) * w);
 tpad[1124 & 3] = (long)((v ^ 1124) * w);
 tpad[1125 & 3] = (long)((v ^ 1125) * w);
 tpad[1126 & 3] = (long)((v ^ 1126) * w);
 tpad[1127 & 3] = (long)((v ^ 1127) * w);
 tpad[1128 & 3] = (long)((v ^ 1128) * w);
 tpad[1129 & 3] = (long)((v ^ 1129) * w);
 tpad[1130 & 3] = (long)((v ^ 1130) * w);
 tpad[1131 & 3] = (long)((v ^ 1131) * w);
 tpad[1132 & 3] = (long)((v ^ 1132) * w);
 tpad[1133 & 3] = (long)((v ^ 1133) * w);
 tpad[1134 & 3] = (long)((v ^ 1134) * w);
 tpad[1135 & 3] = (long)((v ^ 1135) * w);
 tpad[1136 & 3] = (long)((v ^ 1136) * w);
 tpad[1137 & 3] = (long)((v ^ 1137) * w);
 tpad[1138 & 3] = (long)((v ^ 1138) * w);
 tpad[1139 & 3] = (long)((v ^ 1139) * w);
 tpad[1140 & 3] = (long)((v ^ 1140) * w);
 tpad[1141 & 3] = (long)((v ^ 1141) * w);
 tpad[1142 & 3] = (long)((v ^ 1142) * w);
 tpad[1143 & 3] = (long)((v ^ 1143) * w);
 tpad[1144 & 3] = (long)((v ^ 1144) * w);
 tpad[1145 & 3] = (long)((v ^ 1145) * w);
 tpad[1146 & 3] = (long)((v ^ 1146) * w);
 tpad[1147 & 3] = (long)((v ^ 1147) * w);
 tpad[1148 & 3] = (long)((v ^ 1148) * w);
 tpad[1149 & 3] = (long)((v ^ 1149) * w);
 tpad[1150 & 3] = (long)((v ^ 1150) * w);
 tpad[1151 & 3] = (long)((v ^ 1151) * w);
 tpad[1152 & 3] = (long)((v ^ 1152) * w);
 tpad[1153 & 3] = (long)((v ^ 1153) * w);
 tpad[1154 & 3] = (long)((v ^ 1154) * w);
 tpad[1155 & 3] = (long)((v ^ 1155) * w);
 tpad[1156 & 3] = (long)((v ^ 1156) * w);
 tpad[1157 & 3] = (long)((v ^ 1157) * w);
 tpad[1158 & 3] = (long)((v ^ 1158) * w);
 tpad[1159 & 3] = (long)((v ^ 1159) * w);
 tpad[1160 & 3] = (long)((v ^ 1160) * w);
 tpad[1161 & 3] = (long)((v ^ 1161) * w);
 tpad[1162 & 3] = (long)((v ^ 1162) * w);
 tpad[1163 & 3] = (long)((v ^ 1163) * w);
 tpad[1164 & 3] = (long)((v ^ 1164) * w);
 tpad[1165 & 3] = (long)((v ^ 1165) * w);
 tpad[1166 & 3] = (long)((v ^ 1166) * w);
 tpad[1167 & 3] = (long)((v ^ 1167) * w);
 tpad[1168 & 3] = (long)((v ^ 1168) * w);
 tpad[1169 & 3] = (long)((v ^ 1169) * w);
 tpad[1170 & 3] = (long)((v ^ 1170) * w);
 tpad[1171 & 3] = (long)((v ^ 1171) * w);
 tpad[1172 & 3] = (long)((v ^ 1172) * w);
 tpad[1173 & 3] = (long)((v ^ 1173) * w);
 tpad[1174 & 3] = (long)((v ^ 1174) * w);
 tpad[1175 & 3] = (long)((v ^ 1175) * w);
 tpad[1176 & 3] = (long)((v ^ 1176) * w);
 tpad[1177 & 3] = (long)((v ^ 1177) * w);
 tpad[1178 & 3] = (long)((v ^ 1178) * w);
 tpad[1179 & 3] = (long)((v ^ 1179) * w);
 tpad[1180 & 3] = (long)((v ^ 1180) * w);
 tpad[1181 & 3] = (long)((v ^ 1181) * w);
 tpad[1182 & 3] = (long)((v ^ 1182) * w);
 tpad[1183 & 3] = (long)((v ^ 1183) * w);
 tpad[1184 & 3] = (long)((v ^ 1184) * w);
 tpad[1185 & 3] = (long)((v ^ 1185) * w);
 tpad[1186 & 3] = (long)((v ^ 1186) * w);
 tpad[1187 & 3] = (long)((v ^ 1187) * w);
 tpad[1188 & 3] = (long)((v ^ 1188) * w);
 tpad[1189 & 3] = (long)((v ^ 1189) * w);
 tpad[1190 & 3] = (long)((v ^ 1190) * w);
 tpad[1191 & 3] = (long)((v ^ 1191) * w);
 tpad[1192 & 3] = (long)((v ^ 1192) * w);
 tpad[1193 & 3] = (long)((v ^ 1193) * w);
 tpad[1194 & 3] = (long)((v ^ 1194) * w);
 tpad[1195 & 3] = (long)((v ^ 1195) * w);
 tpad[1196 & 3] = (long)((v ^ 1196) * w);
 tpad[1197 & 3] = (long)((v ^ 1197) * w);
 tpad[1198 & 3] = (long)((v ^ 1198) * w);
 tpad[1199 & 3] = (long)((v ^ 1199) * w);
 tpad[1200 & 3] = (long)((v ^ 1200) * w);
 tpad[1201 & 3] = (long)((v ^ 1201) * w);
 tpad[1202 & 3] = (long)((v ^ 1202) * w);
 tpad[1203 & 3] = (long)((v ^ 1203) * w);
 tpad[1204 & 3] = (long)((v ^ 1204) * w);
 tpad[1205 & 3] = (long)((v ^ 1205) * w);
 tpad[1206 & 3] = (long)((v ^ 1206) * w);
 tpad[1207 & 3] = (long)((v ^ 1207) * w);
 tpad[1208 & 3] = (long)((v ^ 1208) * w);
 tpad[1209 & 3] = (long)((v ^ 1209) * w);
 tpad[1210 & 3] = (long)((v ^ 1210) * w);
 tpad[1211 & 3] = (long)((v ^ 1211) * w);
 tpad[1212 & 3] = (long)((v ^ 1212) * w);
 tpad[1213 & 3] = (long)((v ^ 1213) * w);
 tpad[1214 & 3] = (long)((v ^ 1214) * w);
 tpad[1215 & 3] = (long)((v ^ 1215) * w);
 tpad[1216 & 3] = (long)((v ^ 1216) * w);
 tpad[1217 & 3] = (long)((v ^ 1217) * w);
 tpad[1218 & 3] = (long)((v ^ 1218) * w);
 tpad[1219 & 3] = (long)((v ^ 1219) * w);
 tpad[1220 & 3] = (long)((v ^ 1220) * w);
 tpad[1221 & 3] = (long)((v ^ 1221) * w);
 tpad[1222 & 3] = (long)((v ^ 1222) * w);
 tpad[1223 & 3] = (long)((v ^ 1223) * w);
 tpad[1224 & 3] = (long)((v ^ 1224) * w);
 tpad[1225 & 3] = (long)((v ^ 1225) * w);
 tpad[1226 & 3] = (long)((v ^ 1226) * w);
 tpad[1227 & 3] = (long)((v ^ 1227) * w);
 tpad[1228 & 3] = (long)((v ^ 1228) * w);
 tpad[1229 & 3] = (long)((v ^ 1229) * w);
}

template <>
__forceinline void animation_compute_orientations_walk_pad<c_8byte_quantized_rotation_only_interpolated_codec>(long v, long w)
{
 long tpad[4];
 tpad[0 & 3] = (long)((v ^ 0) * w);
 tpad[1 & 3] = (long)((v ^ 1) * w);
 tpad[2 & 3] = (long)((v ^ 2) * w);
 tpad[3 & 3] = (long)((v ^ 3) * w);
 tpad[4 & 3] = (long)((v ^ 4) * w);
 tpad[5 & 3] = (long)((v ^ 5) * w);
 tpad[6 & 3] = (long)((v ^ 6) * w);
 tpad[7 & 3] = (long)((v ^ 7) * w);
 tpad[8 & 3] = (long)((v ^ 8) * w);
 tpad[9 & 3] = (long)((v ^ 9) * w);
 tpad[10 & 3] = (long)((v ^ 10) * w);
 tpad[11 & 3] = (long)((v ^ 11) * w);
 tpad[12 & 3] = (long)((v ^ 12) * w);
 tpad[13 & 3] = (long)((v ^ 13) * w);
 tpad[14 & 3] = (long)((v ^ 14) * w);
 tpad[15 & 3] = (long)((v ^ 15) * w);
 tpad[16 & 3] = (long)((v ^ 16) * w);
 tpad[17 & 3] = (long)((v ^ 17) * w);
 tpad[18 & 3] = (long)((v ^ 18) * w);
 tpad[19 & 3] = (long)((v ^ 19) * w);
 tpad[20 & 3] = (long)((v ^ 20) * w);
 tpad[21 & 3] = (long)((v ^ 21) * w);
 tpad[22 & 3] = (long)((v ^ 22) * w);
 tpad[23 & 3] = (long)((v ^ 23) * w);
 tpad[24 & 3] = (long)((v ^ 24) * w);
 tpad[25 & 3] = (long)((v ^ 25) * w);
 tpad[26 & 3] = (long)((v ^ 26) * w);
 tpad[27 & 3] = (long)((v ^ 27) * w);
 tpad[28 & 3] = (long)((v ^ 28) * w);
 tpad[29 & 3] = (long)((v ^ 29) * w);
 tpad[30 & 3] = (long)((v ^ 30) * w);
 tpad[31 & 3] = (long)((v ^ 31) * w);
 tpad[32 & 3] = (long)((v ^ 32) * w);
 tpad[33 & 3] = (long)((v ^ 33) * w);
 tpad[34 & 3] = (long)((v ^ 34) * w);
 tpad[35 & 3] = (long)((v ^ 35) * w);
 tpad[36 & 3] = (long)((v ^ 36) * w);
 tpad[37 & 3] = (long)((v ^ 37) * w);
 tpad[38 & 3] = (long)((v ^ 38) * w);
 tpad[39 & 3] = (long)((v ^ 39) * w);
 tpad[40 & 3] = (long)((v ^ 40) * w);
 tpad[41 & 3] = (long)((v ^ 41) * w);
 tpad[42 & 3] = (long)((v ^ 42) * w);
 tpad[43 & 3] = (long)((v ^ 43) * w);
 tpad[44 & 3] = (long)((v ^ 44) * w);
 tpad[45 & 3] = (long)((v ^ 45) * w);
 tpad[46 & 3] = (long)((v ^ 46) * w);
 tpad[47 & 3] = (long)((v ^ 47) * w);
 tpad[48 & 3] = (long)((v ^ 48) * w);
 tpad[49 & 3] = (long)((v ^ 49) * w);
 tpad[50 & 3] = (long)((v ^ 50) * w);
 tpad[51 & 3] = (long)((v ^ 51) * w);
 tpad[52 & 3] = (long)((v ^ 52) * w);
 tpad[53 & 3] = (long)((v ^ 53) * w);
 tpad[54 & 3] = (long)((v ^ 54) * w);
 tpad[55 & 3] = (long)((v ^ 55) * w);
 tpad[56 & 3] = (long)((v ^ 56) * w);
 tpad[57 & 3] = (long)((v ^ 57) * w);
 tpad[58 & 3] = (long)((v ^ 58) * w);
 tpad[59 & 3] = (long)((v ^ 59) * w);
 tpad[60 & 3] = (long)((v ^ 60) * w);
 tpad[61 & 3] = (long)((v ^ 61) * w);
 tpad[62 & 3] = (long)((v ^ 62) * w);
 tpad[63 & 3] = (long)((v ^ 63) * w);
 tpad[64 & 3] = (long)((v ^ 64) * w);
 tpad[65 & 3] = (long)((v ^ 65) * w);
 tpad[66 & 3] = (long)((v ^ 66) * w);
 tpad[67 & 3] = (long)((v ^ 67) * w);
 tpad[68 & 3] = (long)((v ^ 68) * w);
 tpad[69 & 3] = (long)((v ^ 69) * w);
 tpad[70 & 3] = (long)((v ^ 70) * w);
 tpad[71 & 3] = (long)((v ^ 71) * w);
 tpad[72 & 3] = (long)((v ^ 72) * w);
 tpad[73 & 3] = (long)((v ^ 73) * w);
 tpad[74 & 3] = (long)((v ^ 74) * w);
 tpad[75 & 3] = (long)((v ^ 75) * w);
 tpad[76 & 3] = (long)((v ^ 76) * w);
 tpad[77 & 3] = (long)((v ^ 77) * w);
 tpad[78 & 3] = (long)((v ^ 78) * w);
 tpad[79 & 3] = (long)((v ^ 79) * w);
 tpad[80 & 3] = (long)((v ^ 80) * w);
 tpad[81 & 3] = (long)((v ^ 81) * w);
 tpad[82 & 3] = (long)((v ^ 82) * w);
 tpad[83 & 3] = (long)((v ^ 83) * w);
 tpad[84 & 3] = (long)((v ^ 84) * w);
 tpad[85 & 3] = (long)((v ^ 85) * w);
 tpad[86 & 3] = (long)((v ^ 86) * w);
 tpad[87 & 3] = (long)((v ^ 87) * w);
 tpad[88 & 3] = (long)((v ^ 88) * w);
 tpad[89 & 3] = (long)((v ^ 89) * w);
 tpad[90 & 3] = (long)((v ^ 90) * w);
 tpad[91 & 3] = (long)((v ^ 91) * w);
 tpad[92 & 3] = (long)((v ^ 92) * w);
 tpad[93 & 3] = (long)((v ^ 93) * w);
 tpad[94 & 3] = (long)((v ^ 94) * w);
 tpad[95 & 3] = (long)((v ^ 95) * w);
 tpad[96 & 3] = (long)((v ^ 96) * w);
 tpad[97 & 3] = (long)((v ^ 97) * w);
 tpad[98 & 3] = (long)((v ^ 98) * w);
 tpad[99 & 3] = (long)((v ^ 99) * w);
 tpad[100 & 3] = (long)((v ^ 100) * w);
 tpad[101 & 3] = (long)((v ^ 101) * w);
 tpad[102 & 3] = (long)((v ^ 102) * w);
 tpad[103 & 3] = (long)((v ^ 103) * w);
 tpad[104 & 3] = (long)((v ^ 104) * w);
 tpad[105 & 3] = (long)((v ^ 105) * w);
 tpad[106 & 3] = (long)((v ^ 106) * w);
 tpad[107 & 3] = (long)((v ^ 107) * w);
 tpad[108 & 3] = (long)((v ^ 108) * w);
 tpad[109 & 3] = (long)((v ^ 109) * w);
 tpad[110 & 3] = (long)((v ^ 110) * w);
 tpad[111 & 3] = (long)((v ^ 111) * w);
 tpad[112 & 3] = (long)((v ^ 112) * w);
 tpad[113 & 3] = (long)((v ^ 113) * w);
 tpad[114 & 3] = (long)((v ^ 114) * w);
 tpad[115 & 3] = (long)((v ^ 115) * w);
 tpad[116 & 3] = (long)((v ^ 116) * w);
 tpad[117 & 3] = (long)((v ^ 117) * w);
 tpad[118 & 3] = (long)((v ^ 118) * w);
 tpad[119 & 3] = (long)((v ^ 119) * w);
 tpad[120 & 3] = (long)((v ^ 120) * w);
 tpad[121 & 3] = (long)((v ^ 121) * w);
 tpad[122 & 3] = (long)((v ^ 122) * w);
 tpad[123 & 3] = (long)((v ^ 123) * w);
 tpad[124 & 3] = (long)((v ^ 124) * w);
 tpad[125 & 3] = (long)((v ^ 125) * w);
 tpad[126 & 3] = (long)((v ^ 126) * w);
 tpad[127 & 3] = (long)((v ^ 127) * w);
 tpad[128 & 3] = (long)((v ^ 128) * w);
 tpad[129 & 3] = (long)((v ^ 129) * w);
 tpad[130 & 3] = (long)((v ^ 130) * w);
 tpad[131 & 3] = (long)((v ^ 131) * w);
 tpad[132 & 3] = (long)((v ^ 132) * w);
 tpad[133 & 3] = (long)((v ^ 133) * w);
 tpad[134 & 3] = (long)((v ^ 134) * w);
 tpad[135 & 3] = (long)((v ^ 135) * w);
 tpad[136 & 3] = (long)((v ^ 136) * w);
 tpad[137 & 3] = (long)((v ^ 137) * w);
 tpad[138 & 3] = (long)((v ^ 138) * w);
 tpad[139 & 3] = (long)((v ^ 139) * w);
 tpad[140 & 3] = (long)((v ^ 140) * w);
 tpad[141 & 3] = (long)((v ^ 141) * w);
 tpad[142 & 3] = (long)((v ^ 142) * w);
 tpad[143 & 3] = (long)((v ^ 143) * w);
 tpad[144 & 3] = (long)((v ^ 144) * w);
 tpad[145 & 3] = (long)((v ^ 145) * w);
 tpad[146 & 3] = (long)((v ^ 146) * w);
 tpad[147 & 3] = (long)((v ^ 147) * w);
 tpad[148 & 3] = (long)((v ^ 148) * w);
 tpad[149 & 3] = (long)((v ^ 149) * w);
 tpad[150 & 3] = (long)((v ^ 150) * w);
 tpad[151 & 3] = (long)((v ^ 151) * w);
 tpad[152 & 3] = (long)((v ^ 152) * w);
 tpad[153 & 3] = (long)((v ^ 153) * w);
 tpad[154 & 3] = (long)((v ^ 154) * w);
 tpad[155 & 3] = (long)((v ^ 155) * w);
 tpad[156 & 3] = (long)((v ^ 156) * w);
 tpad[157 & 3] = (long)((v ^ 157) * w);
 tpad[158 & 3] = (long)((v ^ 158) * w);
 tpad[159 & 3] = (long)((v ^ 159) * w);
 tpad[160 & 3] = (long)((v ^ 160) * w);
 tpad[161 & 3] = (long)((v ^ 161) * w);
 tpad[162 & 3] = (long)((v ^ 162) * w);
 tpad[163 & 3] = (long)((v ^ 163) * w);
 tpad[164 & 3] = (long)((v ^ 164) * w);
 tpad[165 & 3] = (long)((v ^ 165) * w);
 tpad[166 & 3] = (long)((v ^ 166) * w);
 tpad[167 & 3] = (long)((v ^ 167) * w);
 tpad[168 & 3] = (long)((v ^ 168) * w);
 tpad[169 & 3] = (long)((v ^ 169) * w);
 tpad[170 & 3] = (long)((v ^ 170) * w);
 tpad[171 & 3] = (long)((v ^ 171) * w);
 tpad[172 & 3] = (long)((v ^ 172) * w);
 tpad[173 & 3] = (long)((v ^ 173) * w);
 tpad[174 & 3] = (long)((v ^ 174) * w);
 tpad[175 & 3] = (long)((v ^ 175) * w);
 tpad[176 & 3] = (long)((v ^ 176) * w);
 tpad[177 & 3] = (long)((v ^ 177) * w);
 tpad[178 & 3] = (long)((v ^ 178) * w);
 tpad[179 & 3] = (long)((v ^ 179) * w);
 tpad[180 & 3] = (long)((v ^ 180) * w);
 tpad[181 & 3] = (long)((v ^ 181) * w);
 tpad[182 & 3] = (long)((v ^ 182) * w);
 tpad[183 & 3] = (long)((v ^ 183) * w);
 tpad[184 & 3] = (long)((v ^ 184) * w);
 tpad[185 & 3] = (long)((v ^ 185) * w);
 tpad[186 & 3] = (long)((v ^ 186) * w);
 tpad[187 & 3] = (long)((v ^ 187) * w);
 tpad[188 & 3] = (long)((v ^ 188) * w);
 tpad[189 & 3] = (long)((v ^ 189) * w);
 tpad[190 & 3] = (long)((v ^ 190) * w);
 tpad[191 & 3] = (long)((v ^ 191) * w);
 tpad[192 & 3] = (long)((v ^ 192) * w);
 tpad[193 & 3] = (long)((v ^ 193) * w);
 tpad[194 & 3] = (long)((v ^ 194) * w);
 tpad[195 & 3] = (long)((v ^ 195) * w);
 tpad[196 & 3] = (long)((v ^ 196) * w);
 tpad[197 & 3] = (long)((v ^ 197) * w);
 tpad[198 & 3] = (long)((v ^ 198) * w);
 tpad[199 & 3] = (long)((v ^ 199) * w);
 tpad[200 & 3] = (long)((v ^ 200) * w);
 tpad[201 & 3] = (long)((v ^ 201) * w);
 tpad[202 & 3] = (long)((v ^ 202) * w);
 tpad[203 & 3] = (long)((v ^ 203) * w);
 tpad[204 & 3] = (long)((v ^ 204) * w);
 tpad[205 & 3] = (long)((v ^ 205) * w);
 tpad[206 & 3] = (long)((v ^ 206) * w);
 tpad[207 & 3] = (long)((v ^ 207) * w);
 tpad[208 & 3] = (long)((v ^ 208) * w);
 tpad[209 & 3] = (long)((v ^ 209) * w);
 tpad[210 & 3] = (long)((v ^ 210) * w);
 tpad[211 & 3] = (long)((v ^ 211) * w);
 tpad[212 & 3] = (long)((v ^ 212) * w);
 tpad[213 & 3] = (long)((v ^ 213) * w);
 tpad[214 & 3] = (long)((v ^ 214) * w);
 tpad[215 & 3] = (long)((v ^ 215) * w);
 tpad[216 & 3] = (long)((v ^ 216) * w);
 tpad[217 & 3] = (long)((v ^ 217) * w);
 tpad[218 & 3] = (long)((v ^ 218) * w);
 tpad[219 & 3] = (long)((v ^ 219) * w);
 tpad[220 & 3] = (long)((v ^ 220) * w);
 tpad[221 & 3] = (long)((v ^ 221) * w);
 tpad[222 & 3] = (long)((v ^ 222) * w);
 tpad[223 & 3] = (long)((v ^ 223) * w);
 tpad[224 & 3] = (long)((v ^ 224) * w);
 tpad[225 & 3] = (long)((v ^ 225) * w);
 tpad[226 & 3] = (long)((v ^ 226) * w);
 tpad[227 & 3] = (long)((v ^ 227) * w);
 tpad[228 & 3] = (long)((v ^ 228) * w);
 tpad[229 & 3] = (long)((v ^ 229) * w);
 tpad[230 & 3] = (long)((v ^ 230) * w);
 tpad[231 & 3] = (long)((v ^ 231) * w);
 tpad[232 & 3] = (long)((v ^ 232) * w);
 tpad[233 & 3] = (long)((v ^ 233) * w);
 tpad[234 & 3] = (long)((v ^ 234) * w);
 tpad[235 & 3] = (long)((v ^ 235) * w);
 tpad[236 & 3] = (long)((v ^ 236) * w);
 tpad[237 & 3] = (long)((v ^ 237) * w);
 tpad[238 & 3] = (long)((v ^ 238) * w);
 tpad[239 & 3] = (long)((v ^ 239) * w);
 tpad[240 & 3] = (long)((v ^ 240) * w);
 tpad[241 & 3] = (long)((v ^ 241) * w);
 tpad[242 & 3] = (long)((v ^ 242) * w);
 tpad[243 & 3] = (long)((v ^ 243) * w);
 tpad[244 & 3] = (long)((v ^ 244) * w);
 tpad[245 & 3] = (long)((v ^ 245) * w);
 tpad[246 & 3] = (long)((v ^ 246) * w);
 tpad[247 & 3] = (long)((v ^ 247) * w);
 tpad[248 & 3] = (long)((v ^ 248) * w);
 tpad[249 & 3] = (long)((v ^ 249) * w);
 tpad[250 & 3] = (long)((v ^ 250) * w);
 tpad[251 & 3] = (long)((v ^ 251) * w);
 tpad[252 & 3] = (long)((v ^ 252) * w);
 tpad[253 & 3] = (long)((v ^ 253) * w);
 tpad[254 & 3] = (long)((v ^ 254) * w);
 tpad[255 & 3] = (long)((v ^ 255) * w);
 tpad[256 & 3] = (long)((v ^ 256) * w);
 tpad[257 & 3] = (long)((v ^ 257) * w);
 tpad[258 & 3] = (long)((v ^ 258) * w);
 tpad[259 & 3] = (long)((v ^ 259) * w);
 tpad[260 & 3] = (long)((v ^ 260) * w);
 tpad[261 & 3] = (long)((v ^ 261) * w);
 tpad[262 & 3] = (long)((v ^ 262) * w);
 tpad[263 & 3] = (long)((v ^ 263) * w);
 tpad[264 & 3] = (long)((v ^ 264) * w);
 tpad[265 & 3] = (long)((v ^ 265) * w);
 tpad[266 & 3] = (long)((v ^ 266) * w);
 tpad[267 & 3] = (long)((v ^ 267) * w);
 tpad[268 & 3] = (long)((v ^ 268) * w);
 tpad[269 & 3] = (long)((v ^ 269) * w);
 tpad[270 & 3] = (long)((v ^ 270) * w);
 tpad[271 & 3] = (long)((v ^ 271) * w);
 tpad[272 & 3] = (long)((v ^ 272) * w);
 tpad[273 & 3] = (long)((v ^ 273) * w);
 tpad[274 & 3] = (long)((v ^ 274) * w);
 tpad[275 & 3] = (long)((v ^ 275) * w);
 tpad[276 & 3] = (long)((v ^ 276) * w);
 tpad[277 & 3] = (long)((v ^ 277) * w);
 tpad[278 & 3] = (long)((v ^ 278) * w);
 tpad[279 & 3] = (long)((v ^ 279) * w);
 tpad[280 & 3] = (long)((v ^ 280) * w);
 tpad[281 & 3] = (long)((v ^ 281) * w);
 tpad[282 & 3] = (long)((v ^ 282) * w);
 tpad[283 & 3] = (long)((v ^ 283) * w);
 tpad[284 & 3] = (long)((v ^ 284) * w);
 tpad[285 & 3] = (long)((v ^ 285) * w);
 tpad[286 & 3] = (long)((v ^ 286) * w);
 tpad[287 & 3] = (long)((v ^ 287) * w);
 tpad[288 & 3] = (long)((v ^ 288) * w);
 tpad[289 & 3] = (long)((v ^ 289) * w);
 tpad[290 & 3] = (long)((v ^ 290) * w);
 tpad[291 & 3] = (long)((v ^ 291) * w);
 tpad[292 & 3] = (long)((v ^ 292) * w);
 tpad[293 & 3] = (long)((v ^ 293) * w);
 tpad[294 & 3] = (long)((v ^ 294) * w);
 tpad[295 & 3] = (long)((v ^ 295) * w);
 tpad[296 & 3] = (long)((v ^ 296) * w);
 tpad[297 & 3] = (long)((v ^ 297) * w);
 tpad[298 & 3] = (long)((v ^ 298) * w);
 tpad[299 & 3] = (long)((v ^ 299) * w);
 tpad[300 & 3] = (long)((v ^ 300) * w);
 tpad[301 & 3] = (long)((v ^ 301) * w);
 tpad[302 & 3] = (long)((v ^ 302) * w);
 tpad[303 & 3] = (long)((v ^ 303) * w);
 tpad[304 & 3] = (long)((v ^ 304) * w);
 tpad[305 & 3] = (long)((v ^ 305) * w);
 tpad[306 & 3] = (long)((v ^ 306) * w);
 tpad[307 & 3] = (long)((v ^ 307) * w);
 tpad[308 & 3] = (long)((v ^ 308) * w);
 tpad[309 & 3] = (long)((v ^ 309) * w);
 tpad[310 & 3] = (long)((v ^ 310) * w);
 tpad[311 & 3] = (long)((v ^ 311) * w);
 tpad[312 & 3] = (long)((v ^ 312) * w);
 tpad[313 & 3] = (long)((v ^ 313) * w);
 tpad[314 & 3] = (long)((v ^ 314) * w);
 tpad[315 & 3] = (long)((v ^ 315) * w);
 tpad[316 & 3] = (long)((v ^ 316) * w);
 tpad[317 & 3] = (long)((v ^ 317) * w);
 tpad[318 & 3] = (long)((v ^ 318) * w);
 tpad[319 & 3] = (long)((v ^ 319) * w);
 tpad[320 & 3] = (long)((v ^ 320) * w);
 tpad[321 & 3] = (long)((v ^ 321) * w);
 tpad[322 & 3] = (long)((v ^ 322) * w);
 tpad[323 & 3] = (long)((v ^ 323) * w);
 tpad[324 & 3] = (long)((v ^ 324) * w);
 tpad[325 & 3] = (long)((v ^ 325) * w);
 tpad[326 & 3] = (long)((v ^ 326) * w);
 tpad[327 & 3] = (long)((v ^ 327) * w);
 tpad[328 & 3] = (long)((v ^ 328) * w);
 tpad[329 & 3] = (long)((v ^ 329) * w);
 tpad[330 & 3] = (long)((v ^ 330) * w);
 tpad[331 & 3] = (long)((v ^ 331) * w);
 tpad[332 & 3] = (long)((v ^ 332) * w);
 tpad[333 & 3] = (long)((v ^ 333) * w);
 tpad[334 & 3] = (long)((v ^ 334) * w);
 tpad[335 & 3] = (long)((v ^ 335) * w);
 tpad[336 & 3] = (long)((v ^ 336) * w);
 tpad[337 & 3] = (long)((v ^ 337) * w);
 tpad[338 & 3] = (long)((v ^ 338) * w);
 tpad[339 & 3] = (long)((v ^ 339) * w);
 tpad[340 & 3] = (long)((v ^ 340) * w);
 tpad[341 & 3] = (long)((v ^ 341) * w);
 tpad[342 & 3] = (long)((v ^ 342) * w);
 tpad[343 & 3] = (long)((v ^ 343) * w);
 tpad[344 & 3] = (long)((v ^ 344) * w);
 tpad[345 & 3] = (long)((v ^ 345) * w);
 tpad[346 & 3] = (long)((v ^ 346) * w);
 tpad[347 & 3] = (long)((v ^ 347) * w);
 tpad[348 & 3] = (long)((v ^ 348) * w);
 tpad[349 & 3] = (long)((v ^ 349) * w);
 tpad[350 & 3] = (long)((v ^ 350) * w);
 tpad[351 & 3] = (long)((v ^ 351) * w);
 tpad[352 & 3] = (long)((v ^ 352) * w);
 tpad[353 & 3] = (long)((v ^ 353) * w);
 tpad[354 & 3] = (long)((v ^ 354) * w);
 tpad[355 & 3] = (long)((v ^ 355) * w);
 tpad[356 & 3] = (long)((v ^ 356) * w);
 tpad[357 & 3] = (long)((v ^ 357) * w);
 tpad[358 & 3] = (long)((v ^ 358) * w);
 tpad[359 & 3] = (long)((v ^ 359) * w);
 tpad[360 & 3] = (long)((v ^ 360) * w);
 tpad[361 & 3] = (long)((v ^ 361) * w);
 tpad[362 & 3] = (long)((v ^ 362) * w);
 tpad[363 & 3] = (long)((v ^ 363) * w);
 tpad[364 & 3] = (long)((v ^ 364) * w);
 tpad[365 & 3] = (long)((v ^ 365) * w);
 tpad[366 & 3] = (long)((v ^ 366) * w);
 tpad[367 & 3] = (long)((v ^ 367) * w);
 tpad[368 & 3] = (long)((v ^ 368) * w);
 tpad[369 & 3] = (long)((v ^ 369) * w);
 tpad[370 & 3] = (long)((v ^ 370) * w);
 tpad[371 & 3] = (long)((v ^ 371) * w);
 tpad[372 & 3] = (long)((v ^ 372) * w);
 tpad[373 & 3] = (long)((v ^ 373) * w);
 tpad[374 & 3] = (long)((v ^ 374) * w);
 tpad[375 & 3] = (long)((v ^ 375) * w);
 tpad[376 & 3] = (long)((v ^ 376) * w);
 tpad[377 & 3] = (long)((v ^ 377) * w);
 tpad[378 & 3] = (long)((v ^ 378) * w);
 tpad[379 & 3] = (long)((v ^ 379) * w);
 tpad[380 & 3] = (long)((v ^ 380) * w);
 tpad[381 & 3] = (long)((v ^ 381) * w);
 tpad[382 & 3] = (long)((v ^ 382) * w);
 tpad[383 & 3] = (long)((v ^ 383) * w);
 tpad[384 & 3] = (long)((v ^ 384) * w);
 tpad[385 & 3] = (long)((v ^ 385) * w);
 tpad[386 & 3] = (long)((v ^ 386) * w);
 tpad[387 & 3] = (long)((v ^ 387) * w);
 tpad[388 & 3] = (long)((v ^ 388) * w);
 tpad[389 & 3] = (long)((v ^ 389) * w);
 tpad[390 & 3] = (long)((v ^ 390) * w);
 tpad[391 & 3] = (long)((v ^ 391) * w);
 tpad[392 & 3] = (long)((v ^ 392) * w);
 tpad[393 & 3] = (long)((v ^ 393) * w);
 tpad[394 & 3] = (long)((v ^ 394) * w);
 tpad[395 & 3] = (long)((v ^ 395) * w);
 tpad[396 & 3] = (long)((v ^ 396) * w);
 tpad[397 & 3] = (long)((v ^ 397) * w);
 tpad[398 & 3] = (long)((v ^ 398) * w);
 tpad[399 & 3] = (long)((v ^ 399) * w);
 tpad[400 & 3] = (long)((v ^ 400) * w);
 tpad[401 & 3] = (long)((v ^ 401) * w);
 tpad[402 & 3] = (long)((v ^ 402) * w);
 tpad[403 & 3] = (long)((v ^ 403) * w);
 tpad[404 & 3] = (long)((v ^ 404) * w);
 tpad[405 & 3] = (long)((v ^ 405) * w);
 tpad[406 & 3] = (long)((v ^ 406) * w);
 tpad[407 & 3] = (long)((v ^ 407) * w);
 tpad[408 & 3] = (long)((v ^ 408) * w);
 tpad[409 & 3] = (long)((v ^ 409) * w);
 tpad[410 & 3] = (long)((v ^ 410) * w);
 tpad[411 & 3] = (long)((v ^ 411) * w);
 tpad[412 & 3] = (long)((v ^ 412) * w);
 tpad[413 & 3] = (long)((v ^ 413) * w);
 tpad[414 & 3] = (long)((v ^ 414) * w);
 tpad[415 & 3] = (long)((v ^ 415) * w);
 tpad[416 & 3] = (long)((v ^ 416) * w);
 tpad[417 & 3] = (long)((v ^ 417) * w);
 tpad[418 & 3] = (long)((v ^ 418) * w);
 tpad[419 & 3] = (long)((v ^ 419) * w);
 tpad[420 & 3] = (long)((v ^ 420) * w);
 tpad[421 & 3] = (long)((v ^ 421) * w);
 tpad[422 & 3] = (long)((v ^ 422) * w);
 tpad[423 & 3] = (long)((v ^ 423) * w);
 tpad[424 & 3] = (long)((v ^ 424) * w);
 tpad[425 & 3] = (long)((v ^ 425) * w);
 tpad[426 & 3] = (long)((v ^ 426) * w);
 tpad[427 & 3] = (long)((v ^ 427) * w);
 tpad[428 & 3] = (long)((v ^ 428) * w);
 tpad[429 & 3] = (long)((v ^ 429) * w);
 tpad[430 & 3] = (long)((v ^ 430) * w);
 tpad[431 & 3] = (long)((v ^ 431) * w);
 tpad[432 & 3] = (long)((v ^ 432) * w);
 tpad[433 & 3] = (long)((v ^ 433) * w);
 tpad[434 & 3] = (long)((v ^ 434) * w);
 tpad[435 & 3] = (long)((v ^ 435) * w);
 tpad[436 & 3] = (long)((v ^ 436) * w);
 tpad[437 & 3] = (long)((v ^ 437) * w);
 tpad[438 & 3] = (long)((v ^ 438) * w);
 tpad[439 & 3] = (long)((v ^ 439) * w);
 tpad[440 & 3] = (long)((v ^ 440) * w);
 tpad[441 & 3] = (long)((v ^ 441) * w);
 tpad[442 & 3] = (long)((v ^ 442) * w);
 tpad[443 & 3] = (long)((v ^ 443) * w);
 tpad[444 & 3] = (long)((v ^ 444) * w);
 tpad[445 & 3] = (long)((v ^ 445) * w);
 tpad[446 & 3] = (long)((v ^ 446) * w);
 tpad[447 & 3] = (long)((v ^ 447) * w);
 tpad[448 & 3] = (long)((v ^ 448) * w);
 tpad[449 & 3] = (long)((v ^ 449) * w);
 tpad[450 & 3] = (long)((v ^ 450) * w);
 tpad[451 & 3] = (long)((v ^ 451) * w);
 tpad[452 & 3] = (long)((v ^ 452) * w);
 tpad[453 & 3] = (long)((v ^ 453) * w);
 tpad[454 & 3] = (long)((v ^ 454) * w);
 tpad[455 & 3] = (long)((v ^ 455) * w);
 tpad[456 & 3] = (long)((v ^ 456) * w);
 tpad[457 & 3] = (long)((v ^ 457) * w);
 tpad[458 & 3] = (long)((v ^ 458) * w);
 tpad[459 & 3] = (long)((v ^ 459) * w);
 tpad[460 & 3] = (long)((v ^ 460) * w);
 tpad[461 & 3] = (long)((v ^ 461) * w);
 tpad[462 & 3] = (long)((v ^ 462) * w);
 tpad[463 & 3] = (long)((v ^ 463) * w);
 tpad[464 & 3] = (long)((v ^ 464) * w);
 tpad[465 & 3] = (long)((v ^ 465) * w);
 tpad[466 & 3] = (long)((v ^ 466) * w);
 tpad[467 & 3] = (long)((v ^ 467) * w);
 tpad[468 & 3] = (long)((v ^ 468) * w);
 tpad[469 & 3] = (long)((v ^ 469) * w);
 tpad[470 & 3] = (long)((v ^ 470) * w);
 tpad[471 & 3] = (long)((v ^ 471) * w);
 tpad[472 & 3] = (long)((v ^ 472) * w);
 tpad[473 & 3] = (long)((v ^ 473) * w);
 tpad[474 & 3] = (long)((v ^ 474) * w);
 tpad[475 & 3] = (long)((v ^ 475) * w);
 tpad[476 & 3] = (long)((v ^ 476) * w);
 tpad[477 & 3] = (long)((v ^ 477) * w);
 tpad[478 & 3] = (long)((v ^ 478) * w);
 tpad[479 & 3] = (long)((v ^ 479) * w);
 tpad[480 & 3] = (long)((v ^ 480) * w);
 tpad[481 & 3] = (long)((v ^ 481) * w);
 tpad[482 & 3] = (long)((v ^ 482) * w);
 tpad[483 & 3] = (long)((v ^ 483) * w);
 tpad[484 & 3] = (long)((v ^ 484) * w);
 tpad[485 & 3] = (long)((v ^ 485) * w);
 tpad[486 & 3] = (long)((v ^ 486) * w);
 tpad[487 & 3] = (long)((v ^ 487) * w);
 tpad[488 & 3] = (long)((v ^ 488) * w);
 tpad[489 & 3] = (long)((v ^ 489) * w);
 tpad[490 & 3] = (long)((v ^ 490) * w);
 tpad[491 & 3] = (long)((v ^ 491) * w);
 tpad[492 & 3] = (long)((v ^ 492) * w);
 tpad[493 & 3] = (long)((v ^ 493) * w);
 tpad[494 & 3] = (long)((v ^ 494) * w);
 tpad[495 & 3] = (long)((v ^ 495) * w);
 tpad[496 & 3] = (long)((v ^ 496) * w);
 tpad[497 & 3] = (long)((v ^ 497) * w);
 tpad[498 & 3] = (long)((v ^ 498) * w);
 tpad[499 & 3] = (long)((v ^ 499) * w);
 tpad[500 & 3] = (long)((v ^ 500) * w);
 tpad[501 & 3] = (long)((v ^ 501) * w);
 tpad[502 & 3] = (long)((v ^ 502) * w);
 tpad[503 & 3] = (long)((v ^ 503) * w);
 tpad[504 & 3] = (long)((v ^ 504) * w);
 tpad[505 & 3] = (long)((v ^ 505) * w);
 tpad[506 & 3] = (long)((v ^ 506) * w);
 tpad[507 & 3] = (long)((v ^ 507) * w);
 tpad[508 & 3] = (long)((v ^ 508) * w);
 tpad[509 & 3] = (long)((v ^ 509) * w);
 tpad[510 & 3] = (long)((v ^ 510) * w);
 tpad[511 & 3] = (long)((v ^ 511) * w);
 tpad[512 & 3] = (long)((v ^ 512) * w);
 tpad[513 & 3] = (long)((v ^ 513) * w);
 tpad[514 & 3] = (long)((v ^ 514) * w);
 tpad[515 & 3] = (long)((v ^ 515) * w);
 tpad[516 & 3] = (long)((v ^ 516) * w);
 tpad[517 & 3] = (long)((v ^ 517) * w);
 tpad[518 & 3] = (long)((v ^ 518) * w);
 tpad[519 & 3] = (long)((v ^ 519) * w);
 tpad[520 & 3] = (long)((v ^ 520) * w);
 tpad[521 & 3] = (long)((v ^ 521) * w);
 tpad[522 & 3] = (long)((v ^ 522) * w);
 tpad[523 & 3] = (long)((v ^ 523) * w);
 tpad[524 & 3] = (long)((v ^ 524) * w);
 tpad[525 & 3] = (long)((v ^ 525) * w);
 tpad[526 & 3] = (long)((v ^ 526) * w);
 tpad[527 & 3] = (long)((v ^ 527) * w);
 tpad[528 & 3] = (long)((v ^ 528) * w);
 tpad[529 & 3] = (long)((v ^ 529) * w);
 tpad[530 & 3] = (long)((v ^ 530) * w);
 tpad[531 & 3] = (long)((v ^ 531) * w);
 tpad[532 & 3] = (long)((v ^ 532) * w);
 tpad[533 & 3] = (long)((v ^ 533) * w);
 tpad[534 & 3] = (long)((v ^ 534) * w);
 tpad[535 & 3] = (long)((v ^ 535) * w);
 tpad[536 & 3] = (long)((v ^ 536) * w);
 tpad[537 & 3] = (long)((v ^ 537) * w);
 tpad[538 & 3] = (long)((v ^ 538) * w);
 tpad[539 & 3] = (long)((v ^ 539) * w);
 tpad[540 & 3] = (long)((v ^ 540) * w);
 tpad[541 & 3] = (long)((v ^ 541) * w);
 tpad[542 & 3] = (long)((v ^ 542) * w);
 tpad[543 & 3] = (long)((v ^ 543) * w);
 tpad[544 & 3] = (long)((v ^ 544) * w);
 tpad[545 & 3] = (long)((v ^ 545) * w);
 tpad[546 & 3] = (long)((v ^ 546) * w);
 tpad[547 & 3] = (long)((v ^ 547) * w);
 tpad[548 & 3] = (long)((v ^ 548) * w);
 tpad[549 & 3] = (long)((v ^ 549) * w);
 tpad[550 & 3] = (long)((v ^ 550) * w);
 tpad[551 & 3] = (long)((v ^ 551) * w);
 tpad[552 & 3] = (long)((v ^ 552) * w);
 tpad[553 & 3] = (long)((v ^ 553) * w);
 tpad[554 & 3] = (long)((v ^ 554) * w);
 tpad[555 & 3] = (long)((v ^ 555) * w);
 tpad[556 & 3] = (long)((v ^ 556) * w);
 tpad[557 & 3] = (long)((v ^ 557) * w);
 tpad[558 & 3] = (long)((v ^ 558) * w);
 tpad[559 & 3] = (long)((v ^ 559) * w);
 tpad[560 & 3] = (long)((v ^ 560) * w);
 tpad[561 & 3] = (long)((v ^ 561) * w);
 tpad[562 & 3] = (long)((v ^ 562) * w);
 tpad[563 & 3] = (long)((v ^ 563) * w);
 tpad[564 & 3] = (long)((v ^ 564) * w);
 tpad[565 & 3] = (long)((v ^ 565) * w);
 tpad[566 & 3] = (long)((v ^ 566) * w);
 tpad[567 & 3] = (long)((v ^ 567) * w);
 tpad[568 & 3] = (long)((v ^ 568) * w);
 tpad[569 & 3] = (long)((v ^ 569) * w);
 tpad[570 & 3] = (long)((v ^ 570) * w);
 tpad[571 & 3] = (long)((v ^ 571) * w);
 tpad[572 & 3] = (long)((v ^ 572) * w);
 tpad[573 & 3] = (long)((v ^ 573) * w);
 tpad[574 & 3] = (long)((v ^ 574) * w);
 tpad[575 & 3] = (long)((v ^ 575) * w);
 tpad[576 & 3] = (long)((v ^ 576) * w);
 tpad[577 & 3] = (long)((v ^ 577) * w);
 tpad[578 & 3] = (long)((v ^ 578) * w);
 tpad[579 & 3] = (long)((v ^ 579) * w);
 tpad[580 & 3] = (long)((v ^ 580) * w);
 tpad[581 & 3] = (long)((v ^ 581) * w);
 tpad[582 & 3] = (long)((v ^ 582) * w);
 tpad[583 & 3] = (long)((v ^ 583) * w);
 tpad[584 & 3] = (long)((v ^ 584) * w);
 tpad[585 & 3] = (long)((v ^ 585) * w);
 tpad[586 & 3] = (long)((v ^ 586) * w);
 tpad[587 & 3] = (long)((v ^ 587) * w);
 tpad[588 & 3] = (long)((v ^ 588) * w);
 tpad[589 & 3] = (long)((v ^ 589) * w);
 tpad[590 & 3] = (long)((v ^ 590) * w);
 tpad[591 & 3] = (long)((v ^ 591) * w);
 tpad[592 & 3] = (long)((v ^ 592) * w);
 tpad[593 & 3] = (long)((v ^ 593) * w);
 tpad[594 & 3] = (long)((v ^ 594) * w);
 tpad[595 & 3] = (long)((v ^ 595) * w);
 tpad[596 & 3] = (long)((v ^ 596) * w);
 tpad[597 & 3] = (long)((v ^ 597) * w);
 tpad[598 & 3] = (long)((v ^ 598) * w);
 tpad[599 & 3] = (long)((v ^ 599) * w);
 tpad[600 & 3] = (long)((v ^ 600) * w);
 tpad[601 & 3] = (long)((v ^ 601) * w);
 tpad[602 & 3] = (long)((v ^ 602) * w);
 tpad[603 & 3] = (long)((v ^ 603) * w);
 tpad[604 & 3] = (long)((v ^ 604) * w);
 tpad[605 & 3] = (long)((v ^ 605) * w);
 tpad[606 & 3] = (long)((v ^ 606) * w);
 tpad[607 & 3] = (long)((v ^ 607) * w);
 tpad[608 & 3] = (long)((v ^ 608) * w);
 tpad[609 & 3] = (long)((v ^ 609) * w);
 tpad[610 & 3] = (long)((v ^ 610) * w);
 tpad[611 & 3] = (long)((v ^ 611) * w);
 tpad[612 & 3] = (long)((v ^ 612) * w);
 tpad[613 & 3] = (long)((v ^ 613) * w);
 tpad[614 & 3] = (long)((v ^ 614) * w);
 tpad[615 & 3] = (long)((v ^ 615) * w);
 tpad[616 & 3] = (long)((v ^ 616) * w);
 tpad[617 & 3] = (long)((v ^ 617) * w);
 tpad[618 & 3] = (long)((v ^ 618) * w);
 tpad[619 & 3] = (long)((v ^ 619) * w);
 tpad[620 & 3] = (long)((v ^ 620) * w);
 tpad[621 & 3] = (long)((v ^ 621) * w);
 tpad[622 & 3] = (long)((v ^ 622) * w);
 tpad[623 & 3] = (long)((v ^ 623) * w);
 tpad[624 & 3] = (long)((v ^ 624) * w);
 tpad[625 & 3] = (long)((v ^ 625) * w);
 tpad[626 & 3] = (long)((v ^ 626) * w);
 tpad[627 & 3] = (long)((v ^ 627) * w);
 tpad[628 & 3] = (long)((v ^ 628) * w);
 tpad[629 & 3] = (long)((v ^ 629) * w);
 tpad[630 & 3] = (long)((v ^ 630) * w);
 tpad[631 & 3] = (long)((v ^ 631) * w);
 tpad[632 & 3] = (long)((v ^ 632) * w);
 tpad[633 & 3] = (long)((v ^ 633) * w);
 tpad[634 & 3] = (long)((v ^ 634) * w);
 tpad[635 & 3] = (long)((v ^ 635) * w);
 tpad[636 & 3] = (long)((v ^ 636) * w);
 tpad[637 & 3] = (long)((v ^ 637) * w);
 tpad[638 & 3] = (long)((v ^ 638) * w);
 tpad[639 & 3] = (long)((v ^ 639) * w);
 tpad[640 & 3] = (long)((v ^ 640) * w);
 tpad[641 & 3] = (long)((v ^ 641) * w);
 tpad[642 & 3] = (long)((v ^ 642) * w);
 tpad[643 & 3] = (long)((v ^ 643) * w);
 tpad[644 & 3] = (long)((v ^ 644) * w);
 tpad[645 & 3] = (long)((v ^ 645) * w);
 tpad[646 & 3] = (long)((v ^ 646) * w);
 tpad[647 & 3] = (long)((v ^ 647) * w);
 tpad[648 & 3] = (long)((v ^ 648) * w);
 tpad[649 & 3] = (long)((v ^ 649) * w);
 tpad[650 & 3] = (long)((v ^ 650) * w);
 tpad[651 & 3] = (long)((v ^ 651) * w);
 tpad[652 & 3] = (long)((v ^ 652) * w);
 tpad[653 & 3] = (long)((v ^ 653) * w);
 tpad[654 & 3] = (long)((v ^ 654) * w);
 tpad[655 & 3] = (long)((v ^ 655) * w);
 tpad[656 & 3] = (long)((v ^ 656) * w);
 tpad[657 & 3] = (long)((v ^ 657) * w);
 tpad[658 & 3] = (long)((v ^ 658) * w);
 tpad[659 & 3] = (long)((v ^ 659) * w);
 tpad[660 & 3] = (long)((v ^ 660) * w);
 tpad[661 & 3] = (long)((v ^ 661) * w);
 tpad[662 & 3] = (long)((v ^ 662) * w);
 tpad[663 & 3] = (long)((v ^ 663) * w);
 tpad[664 & 3] = (long)((v ^ 664) * w);
 tpad[665 & 3] = (long)((v ^ 665) * w);
 tpad[666 & 3] = (long)((v ^ 666) * w);
 tpad[667 & 3] = (long)((v ^ 667) * w);
 tpad[668 & 3] = (long)((v ^ 668) * w);
 tpad[669 & 3] = (long)((v ^ 669) * w);
 tpad[670 & 3] = (long)((v ^ 670) * w);
 tpad[671 & 3] = (long)((v ^ 671) * w);
 tpad[672 & 3] = (long)((v ^ 672) * w);
 tpad[673 & 3] = (long)((v ^ 673) * w);
 tpad[674 & 3] = (long)((v ^ 674) * w);
 tpad[675 & 3] = (long)((v ^ 675) * w);
 tpad[676 & 3] = (long)((v ^ 676) * w);
 tpad[677 & 3] = (long)((v ^ 677) * w);
 tpad[678 & 3] = (long)((v ^ 678) * w);
 tpad[679 & 3] = (long)((v ^ 679) * w);
 tpad[680 & 3] = (long)((v ^ 680) * w);
 tpad[681 & 3] = (long)((v ^ 681) * w);
 tpad[682 & 3] = (long)((v ^ 682) * w);
 tpad[683 & 3] = (long)((v ^ 683) * w);
 tpad[684 & 3] = (long)((v ^ 684) * w);
 tpad[685 & 3] = (long)((v ^ 685) * w);
 tpad[686 & 3] = (long)((v ^ 686) * w);
 tpad[687 & 3] = (long)((v ^ 687) * w);
 tpad[688 & 3] = (long)((v ^ 688) * w);
 tpad[689 & 3] = (long)((v ^ 689) * w);
 tpad[690 & 3] = (long)((v ^ 690) * w);
 tpad[691 & 3] = (long)((v ^ 691) * w);
 tpad[692 & 3] = (long)((v ^ 692) * w);
 tpad[693 & 3] = (long)((v ^ 693) * w);
 tpad[694 & 3] = (long)((v ^ 694) * w);
 tpad[695 & 3] = (long)((v ^ 695) * w);
 tpad[696 & 3] = (long)((v ^ 696) * w);
 tpad[697 & 3] = (long)((v ^ 697) * w);
 tpad[698 & 3] = (long)((v ^ 698) * w);
 tpad[699 & 3] = (long)((v ^ 699) * w);
 tpad[700 & 3] = (long)((v ^ 700) * w);
 tpad[701 & 3] = (long)((v ^ 701) * w);
 tpad[702 & 3] = (long)((v ^ 702) * w);
 tpad[703 & 3] = (long)((v ^ 703) * w);
 tpad[704 & 3] = (long)((v ^ 704) * w);
 tpad[705 & 3] = (long)((v ^ 705) * w);
 tpad[706 & 3] = (long)((v ^ 706) * w);
 tpad[707 & 3] = (long)((v ^ 707) * w);
 tpad[708 & 3] = (long)((v ^ 708) * w);
 tpad[709 & 3] = (long)((v ^ 709) * w);
 tpad[710 & 3] = (long)((v ^ 710) * w);
 tpad[711 & 3] = (long)((v ^ 711) * w);
 tpad[712 & 3] = (long)((v ^ 712) * w);
 tpad[713 & 3] = (long)((v ^ 713) * w);
 tpad[714 & 3] = (long)((v ^ 714) * w);
 tpad[715 & 3] = (long)((v ^ 715) * w);
 tpad[716 & 3] = (long)((v ^ 716) * w);
 tpad[717 & 3] = (long)((v ^ 717) * w);
 tpad[718 & 3] = (long)((v ^ 718) * w);
 tpad[719 & 3] = (long)((v ^ 719) * w);
 tpad[720 & 3] = (long)((v ^ 720) * w);
 tpad[721 & 3] = (long)((v ^ 721) * w);
 tpad[722 & 3] = (long)((v ^ 722) * w);
 tpad[723 & 3] = (long)((v ^ 723) * w);
 tpad[724 & 3] = (long)((v ^ 724) * w);
 tpad[725 & 3] = (long)((v ^ 725) * w);
 tpad[726 & 3] = (long)((v ^ 726) * w);
 tpad[727 & 3] = (long)((v ^ 727) * w);
 tpad[728 & 3] = (long)((v ^ 728) * w);
 tpad[729 & 3] = (long)((v ^ 729) * w);
 tpad[730 & 3] = (long)((v ^ 730) * w);
 tpad[731 & 3] = (long)((v ^ 731) * w);
 tpad[732 & 3] = (long)((v ^ 732) * w);
 tpad[733 & 3] = (long)((v ^ 733) * w);
 tpad[734 & 3] = (long)((v ^ 734) * w);
 tpad[735 & 3] = (long)((v ^ 735) * w);
 tpad[736 & 3] = (long)((v ^ 736) * w);
 tpad[737 & 3] = (long)((v ^ 737) * w);
 tpad[738 & 3] = (long)((v ^ 738) * w);
 tpad[739 & 3] = (long)((v ^ 739) * w);
 tpad[740 & 3] = (long)((v ^ 740) * w);
 tpad[741 & 3] = (long)((v ^ 741) * w);
 tpad[742 & 3] = (long)((v ^ 742) * w);
 tpad[743 & 3] = (long)((v ^ 743) * w);
 tpad[744 & 3] = (long)((v ^ 744) * w);
 tpad[745 & 3] = (long)((v ^ 745) * w);
 tpad[746 & 3] = (long)((v ^ 746) * w);
 tpad[747 & 3] = (long)((v ^ 747) * w);
 tpad[748 & 3] = (long)((v ^ 748) * w);
 tpad[749 & 3] = (long)((v ^ 749) * w);
 tpad[750 & 3] = (long)((v ^ 750) * w);
 tpad[751 & 3] = (long)((v ^ 751) * w);
 tpad[752 & 3] = (long)((v ^ 752) * w);
 tpad[753 & 3] = (long)((v ^ 753) * w);
 tpad[754 & 3] = (long)((v ^ 754) * w);
 tpad[755 & 3] = (long)((v ^ 755) * w);
 tpad[756 & 3] = (long)((v ^ 756) * w);
 tpad[757 & 3] = (long)((v ^ 757) * w);
 tpad[758 & 3] = (long)((v ^ 758) * w);
 tpad[759 & 3] = (long)((v ^ 759) * w);
 tpad[760 & 3] = (long)((v ^ 760) * w);
 tpad[761 & 3] = (long)((v ^ 761) * w);
 tpad[762 & 3] = (long)((v ^ 762) * w);
 tpad[763 & 3] = (long)((v ^ 763) * w);
 tpad[764 & 3] = (long)((v ^ 764) * w);
 tpad[765 & 3] = (long)((v ^ 765) * w);
 tpad[766 & 3] = (long)((v ^ 766) * w);
 tpad[767 & 3] = (long)((v ^ 767) * w);
 tpad[768 & 3] = (long)((v ^ 768) * w);
 tpad[769 & 3] = (long)((v ^ 769) * w);
 tpad[770 & 3] = (long)((v ^ 770) * w);
 tpad[771 & 3] = (long)((v ^ 771) * w);
 tpad[772 & 3] = (long)((v ^ 772) * w);
 tpad[773 & 3] = (long)((v ^ 773) * w);
 tpad[774 & 3] = (long)((v ^ 774) * w);
 tpad[775 & 3] = (long)((v ^ 775) * w);
 tpad[776 & 3] = (long)((v ^ 776) * w);
 tpad[777 & 3] = (long)((v ^ 777) * w);
 tpad[778 & 3] = (long)((v ^ 778) * w);
 tpad[779 & 3] = (long)((v ^ 779) * w);
 tpad[780 & 3] = (long)((v ^ 780) * w);
 tpad[781 & 3] = (long)((v ^ 781) * w);
 tpad[782 & 3] = (long)((v ^ 782) * w);
 tpad[783 & 3] = (long)((v ^ 783) * w);
 tpad[784 & 3] = (long)((v ^ 784) * w);
 tpad[785 & 3] = (long)((v ^ 785) * w);
 tpad[786 & 3] = (long)((v ^ 786) * w);
 tpad[787 & 3] = (long)((v ^ 787) * w);
 tpad[788 & 3] = (long)((v ^ 788) * w);
 tpad[789 & 3] = (long)((v ^ 789) * w);
 tpad[790 & 3] = (long)((v ^ 790) * w);
 tpad[791 & 3] = (long)((v ^ 791) * w);
 tpad[792 & 3] = (long)((v ^ 792) * w);
 tpad[793 & 3] = (long)((v ^ 793) * w);
 tpad[794 & 3] = (long)((v ^ 794) * w);
 tpad[795 & 3] = (long)((v ^ 795) * w);
 tpad[796 & 3] = (long)((v ^ 796) * w);
 tpad[797 & 3] = (long)((v ^ 797) * w);
 tpad[798 & 3] = (long)((v ^ 798) * w);
 tpad[799 & 3] = (long)((v ^ 799) * w);
 tpad[800 & 3] = (long)((v ^ 800) * w);
 tpad[801 & 3] = (long)((v ^ 801) * w);
 tpad[802 & 3] = (long)((v ^ 802) * w);
 tpad[803 & 3] = (long)((v ^ 803) * w);
 tpad[804 & 3] = (long)((v ^ 804) * w);
 tpad[805 & 3] = (long)((v ^ 805) * w);
 tpad[806 & 3] = (long)((v ^ 806) * w);
 tpad[807 & 3] = (long)((v ^ 807) * w);
 tpad[808 & 3] = (long)((v ^ 808) * w);
 tpad[809 & 3] = (long)((v ^ 809) * w);
 tpad[810 & 3] = (long)((v ^ 810) * w);
 tpad[811 & 3] = (long)((v ^ 811) * w);
 tpad[812 & 3] = (long)((v ^ 812) * w);
 tpad[813 & 3] = (long)((v ^ 813) * w);
 tpad[814 & 3] = (long)((v ^ 814) * w);
 tpad[815 & 3] = (long)((v ^ 815) * w);
 tpad[816 & 3] = (long)((v ^ 816) * w);
 tpad[817 & 3] = (long)((v ^ 817) * w);
 tpad[818 & 3] = (long)((v ^ 818) * w);
 tpad[819 & 3] = (long)((v ^ 819) * w);
 tpad[820 & 3] = (long)((v ^ 820) * w);
 tpad[821 & 3] = (long)((v ^ 821) * w);
 tpad[822 & 3] = (long)((v ^ 822) * w);
 tpad[823 & 3] = (long)((v ^ 823) * w);
 tpad[824 & 3] = (long)((v ^ 824) * w);
 tpad[825 & 3] = (long)((v ^ 825) * w);
 tpad[826 & 3] = (long)((v ^ 826) * w);
 tpad[827 & 3] = (long)((v ^ 827) * w);
 tpad[828 & 3] = (long)((v ^ 828) * w);
 tpad[829 & 3] = (long)((v ^ 829) * w);
 tpad[830 & 3] = (long)((v ^ 830) * w);
 tpad[831 & 3] = (long)((v ^ 831) * w);
 tpad[832 & 3] = (long)((v ^ 832) * w);
 tpad[833 & 3] = (long)((v ^ 833) * w);
 tpad[834 & 3] = (long)((v ^ 834) * w);
 tpad[835 & 3] = (long)((v ^ 835) * w);
 tpad[836 & 3] = (long)((v ^ 836) * w);
 tpad[837 & 3] = (long)((v ^ 837) * w);
 tpad[838 & 3] = (long)((v ^ 838) * w);
 tpad[839 & 3] = (long)((v ^ 839) * w);
 tpad[840 & 3] = (long)((v ^ 840) * w);
 tpad[841 & 3] = (long)((v ^ 841) * w);
 tpad[842 & 3] = (long)((v ^ 842) * w);
 tpad[843 & 3] = (long)((v ^ 843) * w);
 tpad[844 & 3] = (long)((v ^ 844) * w);
 tpad[845 & 3] = (long)((v ^ 845) * w);
 tpad[846 & 3] = (long)((v ^ 846) * w);
 tpad[847 & 3] = (long)((v ^ 847) * w);
 tpad[848 & 3] = (long)((v ^ 848) * w);
 tpad[849 & 3] = (long)((v ^ 849) * w);
 tpad[850 & 3] = (long)((v ^ 850) * w);
 tpad[851 & 3] = (long)((v ^ 851) * w);
 tpad[852 & 3] = (long)((v ^ 852) * w);
 tpad[853 & 3] = (long)((v ^ 853) * w);
 tpad[854 & 3] = (long)((v ^ 854) * w);
 tpad[855 & 3] = (long)((v ^ 855) * w);
 tpad[856 & 3] = (long)((v ^ 856) * w);
 tpad[857 & 3] = (long)((v ^ 857) * w);
 tpad[858 & 3] = (long)((v ^ 858) * w);
 tpad[859 & 3] = (long)((v ^ 859) * w);
 tpad[860 & 3] = (long)((v ^ 860) * w);
 tpad[861 & 3] = (long)((v ^ 861) * w);
 tpad[862 & 3] = (long)((v ^ 862) * w);
 tpad[863 & 3] = (long)((v ^ 863) * w);
 tpad[864 & 3] = (long)((v ^ 864) * w);
 tpad[865 & 3] = (long)((v ^ 865) * w);
 tpad[866 & 3] = (long)((v ^ 866) * w);
 tpad[867 & 3] = (long)((v ^ 867) * w);
 tpad[868 & 3] = (long)((v ^ 868) * w);
 tpad[869 & 3] = (long)((v ^ 869) * w);
 tpad[870 & 3] = (long)((v ^ 870) * w);
 tpad[871 & 3] = (long)((v ^ 871) * w);
 tpad[872 & 3] = (long)((v ^ 872) * w);
 tpad[873 & 3] = (long)((v ^ 873) * w);
 tpad[874 & 3] = (long)((v ^ 874) * w);
 tpad[875 & 3] = (long)((v ^ 875) * w);
 tpad[876 & 3] = (long)((v ^ 876) * w);
 tpad[877 & 3] = (long)((v ^ 877) * w);
 tpad[878 & 3] = (long)((v ^ 878) * w);
 tpad[879 & 3] = (long)((v ^ 879) * w);
 tpad[880 & 3] = (long)((v ^ 880) * w);
 tpad[881 & 3] = (long)((v ^ 881) * w);
 tpad[882 & 3] = (long)((v ^ 882) * w);
 tpad[883 & 3] = (long)((v ^ 883) * w);
 tpad[884 & 3] = (long)((v ^ 884) * w);
 tpad[885 & 3] = (long)((v ^ 885) * w);
 tpad[886 & 3] = (long)((v ^ 886) * w);
 tpad[887 & 3] = (long)((v ^ 887) * w);
 tpad[888 & 3] = (long)((v ^ 888) * w);
 tpad[889 & 3] = (long)((v ^ 889) * w);
 tpad[890 & 3] = (long)((v ^ 890) * w);
 tpad[891 & 3] = (long)((v ^ 891) * w);
 tpad[892 & 3] = (long)((v ^ 892) * w);
 tpad[893 & 3] = (long)((v ^ 893) * w);
 tpad[894 & 3] = (long)((v ^ 894) * w);
 tpad[895 & 3] = (long)((v ^ 895) * w);
 tpad[896 & 3] = (long)((v ^ 896) * w);
 tpad[897 & 3] = (long)((v ^ 897) * w);
 tpad[898 & 3] = (long)((v ^ 898) * w);
 tpad[899 & 3] = (long)((v ^ 899) * w);
 tpad[900 & 3] = (long)((v ^ 900) * w);
 tpad[901 & 3] = (long)((v ^ 901) * w);
 tpad[902 & 3] = (long)((v ^ 902) * w);
 tpad[903 & 3] = (long)((v ^ 903) * w);
 tpad[904 & 3] = (long)((v ^ 904) * w);
 tpad[905 & 3] = (long)((v ^ 905) * w);
 tpad[906 & 3] = (long)((v ^ 906) * w);
 tpad[907 & 3] = (long)((v ^ 907) * w);
 tpad[908 & 3] = (long)((v ^ 908) * w);
 tpad[909 & 3] = (long)((v ^ 909) * w);
 tpad[910 & 3] = (long)((v ^ 910) * w);
 tpad[911 & 3] = (long)((v ^ 911) * w);
 tpad[912 & 3] = (long)((v ^ 912) * w);
 tpad[913 & 3] = (long)((v ^ 913) * w);
 tpad[914 & 3] = (long)((v ^ 914) * w);
 tpad[915 & 3] = (long)((v ^ 915) * w);
 tpad[916 & 3] = (long)((v ^ 916) * w);
 tpad[917 & 3] = (long)((v ^ 917) * w);
 tpad[918 & 3] = (long)((v ^ 918) * w);
 tpad[919 & 3] = (long)((v ^ 919) * w);
 tpad[920 & 3] = (long)((v ^ 920) * w);
 tpad[921 & 3] = (long)((v ^ 921) * w);
 tpad[922 & 3] = (long)((v ^ 922) * w);
 tpad[923 & 3] = (long)((v ^ 923) * w);
 tpad[924 & 3] = (long)((v ^ 924) * w);
 tpad[925 & 3] = (long)((v ^ 925) * w);
 tpad[926 & 3] = (long)((v ^ 926) * w);
 tpad[927 & 3] = (long)((v ^ 927) * w);
 tpad[928 & 3] = (long)((v ^ 928) * w);
 tpad[929 & 3] = (long)((v ^ 929) * w);
 tpad[930 & 3] = (long)((v ^ 930) * w);
 tpad[931 & 3] = (long)((v ^ 931) * w);
 tpad[932 & 3] = (long)((v ^ 932) * w);
 tpad[933 & 3] = (long)((v ^ 933) * w);
 tpad[934 & 3] = (long)((v ^ 934) * w);
 tpad[935 & 3] = (long)((v ^ 935) * w);
 tpad[936 & 3] = (long)((v ^ 936) * w);
 tpad[937 & 3] = (long)((v ^ 937) * w);
 tpad[938 & 3] = (long)((v ^ 938) * w);
 tpad[939 & 3] = (long)((v ^ 939) * w);
 tpad[940 & 3] = (long)((v ^ 940) * w);
 tpad[941 & 3] = (long)((v ^ 941) * w);
 tpad[942 & 3] = (long)((v ^ 942) * w);
 tpad[943 & 3] = (long)((v ^ 943) * w);
 tpad[944 & 3] = (long)((v ^ 944) * w);
 tpad[945 & 3] = (long)((v ^ 945) * w);
 tpad[946 & 3] = (long)((v ^ 946) * w);
 tpad[947 & 3] = (long)((v ^ 947) * w);
 tpad[948 & 3] = (long)((v ^ 948) * w);
 tpad[949 & 3] = (long)((v ^ 949) * w);
 tpad[950 & 3] = (long)((v ^ 950) * w);
 tpad[951 & 3] = (long)((v ^ 951) * w);
 tpad[952 & 3] = (long)((v ^ 952) * w);
 tpad[953 & 3] = (long)((v ^ 953) * w);
 tpad[954 & 3] = (long)((v ^ 954) * w);
 tpad[955 & 3] = (long)((v ^ 955) * w);
 tpad[956 & 3] = (long)((v ^ 956) * w);
 tpad[957 & 3] = (long)((v ^ 957) * w);
 tpad[958 & 3] = (long)((v ^ 958) * w);
 tpad[959 & 3] = (long)((v ^ 959) * w);
 tpad[960 & 3] = (long)((v ^ 960) * w);
 tpad[961 & 3] = (long)((v ^ 961) * w);
 tpad[962 & 3] = (long)((v ^ 962) * w);
 tpad[963 & 3] = (long)((v ^ 963) * w);
 tpad[964 & 3] = (long)((v ^ 964) * w);
 tpad[965 & 3] = (long)((v ^ 965) * w);
 tpad[966 & 3] = (long)((v ^ 966) * w);
 tpad[967 & 3] = (long)((v ^ 967) * w);
 tpad[968 & 3] = (long)((v ^ 968) * w);
 tpad[969 & 3] = (long)((v ^ 969) * w);
 tpad[970 & 3] = (long)((v ^ 970) * w);
 tpad[971 & 3] = (long)((v ^ 971) * w);
 tpad[972 & 3] = (long)((v ^ 972) * w);
 tpad[973 & 3] = (long)((v ^ 973) * w);
 tpad[974 & 3] = (long)((v ^ 974) * w);
 tpad[975 & 3] = (long)((v ^ 975) * w);
 tpad[976 & 3] = (long)((v ^ 976) * w);
 tpad[977 & 3] = (long)((v ^ 977) * w);
 tpad[978 & 3] = (long)((v ^ 978) * w);
 tpad[979 & 3] = (long)((v ^ 979) * w);
 tpad[980 & 3] = (long)((v ^ 980) * w);
 tpad[981 & 3] = (long)((v ^ 981) * w);
 tpad[982 & 3] = (long)((v ^ 982) * w);
 tpad[983 & 3] = (long)((v ^ 983) * w);
 tpad[984 & 3] = (long)((v ^ 984) * w);
 tpad[985 & 3] = (long)((v ^ 985) * w);
 tpad[986 & 3] = (long)((v ^ 986) * w);
 tpad[987 & 3] = (long)((v ^ 987) * w);
 tpad[988 & 3] = (long)((v ^ 988) * w);
 tpad[989 & 3] = (long)((v ^ 989) * w);
 tpad[990 & 3] = (long)((v ^ 990) * w);
 tpad[991 & 3] = (long)((v ^ 991) * w);
 tpad[992 & 3] = (long)((v ^ 992) * w);
 tpad[993 & 3] = (long)((v ^ 993) * w);
 tpad[994 & 3] = (long)((v ^ 994) * w);
 tpad[995 & 3] = (long)((v ^ 995) * w);
 tpad[996 & 3] = (long)((v ^ 996) * w);
 tpad[997 & 3] = (long)((v ^ 997) * w);
 tpad[998 & 3] = (long)((v ^ 998) * w);
 tpad[999 & 3] = (long)((v ^ 999) * w);
 tpad[1000 & 3] = (long)((v ^ 1000) * w);
 tpad[1001 & 3] = (long)((v ^ 1001) * w);
 tpad[1002 & 3] = (long)((v ^ 1002) * w);
 tpad[1003 & 3] = (long)((v ^ 1003) * w);
 tpad[1004 & 3] = (long)((v ^ 1004) * w);
 tpad[1005 & 3] = (long)((v ^ 1005) * w);
 tpad[1006 & 3] = (long)((v ^ 1006) * w);
 tpad[1007 & 3] = (long)((v ^ 1007) * w);
 tpad[1008 & 3] = (long)((v ^ 1008) * w);
 tpad[1009 & 3] = (long)((v ^ 1009) * w);
 tpad[1010 & 3] = (long)((v ^ 1010) * w);
 tpad[1011 & 3] = (long)((v ^ 1011) * w);
 tpad[1012 & 3] = (long)((v ^ 1012) * w);
 tpad[1013 & 3] = (long)((v ^ 1013) * w);
 tpad[1014 & 3] = (long)((v ^ 1014) * w);
 tpad[1015 & 3] = (long)((v ^ 1015) * w);
 tpad[1016 & 3] = (long)((v ^ 1016) * w);
 tpad[1017 & 3] = (long)((v ^ 1017) * w);
 tpad[1018 & 3] = (long)((v ^ 1018) * w);
 tpad[1019 & 3] = (long)((v ^ 1019) * w);
 tpad[1020 & 3] = (long)((v ^ 1020) * w);
 tpad[1021 & 3] = (long)((v ^ 1021) * w);
 tpad[1022 & 3] = (long)((v ^ 1022) * w);
 tpad[1023 & 3] = (long)((v ^ 1023) * w);
 tpad[1024 & 3] = (long)((v ^ 1024) * w);
 tpad[1025 & 3] = (long)((v ^ 1025) * w);
 tpad[1026 & 3] = (long)((v ^ 1026) * w);
 tpad[1027 & 3] = (long)((v ^ 1027) * w);
 tpad[1028 & 3] = (long)((v ^ 1028) * w);
 tpad[1029 & 3] = (long)((v ^ 1029) * w);
 tpad[1030 & 3] = (long)((v ^ 1030) * w);
 tpad[1031 & 3] = (long)((v ^ 1031) * w);
 tpad[1032 & 3] = (long)((v ^ 1032) * w);
 tpad[1033 & 3] = (long)((v ^ 1033) * w);
 tpad[1034 & 3] = (long)((v ^ 1034) * w);
 tpad[1035 & 3] = (long)((v ^ 1035) * w);
 tpad[1036 & 3] = (long)((v ^ 1036) * w);
 tpad[1037 & 3] = (long)((v ^ 1037) * w);
 tpad[1038 & 3] = (long)((v ^ 1038) * w);
 tpad[1039 & 3] = (long)((v ^ 1039) * w);
 tpad[1040 & 3] = (long)((v ^ 1040) * w);
 tpad[1041 & 3] = (long)((v ^ 1041) * w);
 tpad[1042 & 3] = (long)((v ^ 1042) * w);
 tpad[1043 & 3] = (long)((v ^ 1043) * w);
 tpad[1044 & 3] = (long)((v ^ 1044) * w);
 tpad[1045 & 3] = (long)((v ^ 1045) * w);
 tpad[1046 & 3] = (long)((v ^ 1046) * w);
 tpad[1047 & 3] = (long)((v ^ 1047) * w);
 tpad[1048 & 3] = (long)((v ^ 1048) * w);
 tpad[1049 & 3] = (long)((v ^ 1049) * w);
 tpad[1050 & 3] = (long)((v ^ 1050) * w);
 tpad[1051 & 3] = (long)((v ^ 1051) * w);
 tpad[1052 & 3] = (long)((v ^ 1052) * w);
 tpad[1053 & 3] = (long)((v ^ 1053) * w);
 tpad[1054 & 3] = (long)((v ^ 1054) * w);
 tpad[1055 & 3] = (long)((v ^ 1055) * w);
 tpad[1056 & 3] = (long)((v ^ 1056) * w);
 tpad[1057 & 3] = (long)((v ^ 1057) * w);
 tpad[1058 & 3] = (long)((v ^ 1058) * w);
 tpad[1059 & 3] = (long)((v ^ 1059) * w);
 tpad[1060 & 3] = (long)((v ^ 1060) * w);
 tpad[1061 & 3] = (long)((v ^ 1061) * w);
 tpad[1062 & 3] = (long)((v ^ 1062) * w);
 tpad[1063 & 3] = (long)((v ^ 1063) * w);
 tpad[1064 & 3] = (long)((v ^ 1064) * w);
 tpad[1065 & 3] = (long)((v ^ 1065) * w);
 tpad[1066 & 3] = (long)((v ^ 1066) * w);
 tpad[1067 & 3] = (long)((v ^ 1067) * w);
 tpad[1068 & 3] = (long)((v ^ 1068) * w);
 tpad[1069 & 3] = (long)((v ^ 1069) * w);
 tpad[1070 & 3] = (long)((v ^ 1070) * w);
 tpad[1071 & 3] = (long)((v ^ 1071) * w);
 tpad[1072 & 3] = (long)((v ^ 1072) * w);
 tpad[1073 & 3] = (long)((v ^ 1073) * w);
 tpad[1074 & 3] = (long)((v ^ 1074) * w);
 tpad[1075 & 3] = (long)((v ^ 1075) * w);
 tpad[1076 & 3] = (long)((v ^ 1076) * w);
 tpad[1077 & 3] = (long)((v ^ 1077) * w);
 tpad[1078 & 3] = (long)((v ^ 1078) * w);
 tpad[1079 & 3] = (long)((v ^ 1079) * w);
 tpad[1080 & 3] = (long)((v ^ 1080) * w);
 tpad[1081 & 3] = (long)((v ^ 1081) * w);
 tpad[1082 & 3] = (long)((v ^ 1082) * w);
 tpad[1083 & 3] = (long)((v ^ 1083) * w);
 tpad[1084 & 3] = (long)((v ^ 1084) * w);
 tpad[1085 & 3] = (long)((v ^ 1085) * w);
 tpad[1086 & 3] = (long)((v ^ 1086) * w);
 tpad[1087 & 3] = (long)((v ^ 1087) * w);
 tpad[1088 & 3] = (long)((v ^ 1088) * w);
 tpad[1089 & 3] = (long)((v ^ 1089) * w);
 tpad[1090 & 3] = (long)((v ^ 1090) * w);
 tpad[1091 & 3] = (long)((v ^ 1091) * w);
 tpad[1092 & 3] = (long)((v ^ 1092) * w);
 tpad[1093 & 3] = (long)((v ^ 1093) * w);
 tpad[1094 & 3] = (long)((v ^ 1094) * w);
 tpad[1095 & 3] = (long)((v ^ 1095) * w);
 tpad[1096 & 3] = (long)((v ^ 1096) * w);
 tpad[1097 & 3] = (long)((v ^ 1097) * w);
 tpad[1098 & 3] = (long)((v ^ 1098) * w);
 tpad[1099 & 3] = (long)((v ^ 1099) * w);
 tpad[1100 & 3] = (long)((v ^ 1100) * w);
 tpad[1101 & 3] = (long)((v ^ 1101) * w);
 tpad[1102 & 3] = (long)((v ^ 1102) * w);
 tpad[1103 & 3] = (long)((v ^ 1103) * w);
 tpad[1104 & 3] = (long)((v ^ 1104) * w);
 tpad[1105 & 3] = (long)((v ^ 1105) * w);
 tpad[1106 & 3] = (long)((v ^ 1106) * w);
 tpad[1107 & 3] = (long)((v ^ 1107) * w);
 tpad[1108 & 3] = (long)((v ^ 1108) * w);
 tpad[1109 & 3] = (long)((v ^ 1109) * w);
 tpad[1110 & 3] = (long)((v ^ 1110) * w);
 tpad[1111 & 3] = (long)((v ^ 1111) * w);
 tpad[1112 & 3] = (long)((v ^ 1112) * w);
 tpad[1113 & 3] = (long)((v ^ 1113) * w);
 tpad[1114 & 3] = (long)((v ^ 1114) * w);
 tpad[1115 & 3] = (long)((v ^ 1115) * w);
 tpad[1116 & 3] = (long)((v ^ 1116) * w);
 tpad[1117 & 3] = (long)((v ^ 1117) * w);
 tpad[1118 & 3] = (long)((v ^ 1118) * w);
 tpad[1119 & 3] = (long)((v ^ 1119) * w);
 tpad[1120 & 3] = (long)((v ^ 1120) * w);
 tpad[1121 & 3] = (long)((v ^ 1121) * w);
 tpad[1122 & 3] = (long)((v ^ 1122) * w);
 tpad[1123 & 3] = (long)((v ^ 1123) * w);
 tpad[1124 & 3] = (long)((v ^ 1124) * w);
 tpad[1125 & 3] = (long)((v ^ 1125) * w);
 tpad[1126 & 3] = (long)((v ^ 1126) * w);
 tpad[1127 & 3] = (long)((v ^ 1127) * w);
 tpad[1128 & 3] = (long)((v ^ 1128) * w);
 tpad[1129 & 3] = (long)((v ^ 1129) * w);
 tpad[1130 & 3] = (long)((v ^ 1130) * w);
 tpad[1131 & 3] = (long)((v ^ 1131) * w);
 tpad[1132 & 3] = (long)((v ^ 1132) * w);
 tpad[1133 & 3] = (long)((v ^ 1133) * w);
 tpad[1134 & 3] = (long)((v ^ 1134) * w);
 tpad[1135 & 3] = (long)((v ^ 1135) * w);
 tpad[1136 & 3] = (long)((v ^ 1136) * w);
 tpad[1137 & 3] = (long)((v ^ 1137) * w);
 tpad[1138 & 3] = (long)((v ^ 1138) * w);
 tpad[1139 & 3] = (long)((v ^ 1139) * w);
 tpad[1140 & 3] = (long)((v ^ 1140) * w);
 tpad[1141 & 3] = (long)((v ^ 1141) * w);
 tpad[1142 & 3] = (long)((v ^ 1142) * w);
 tpad[1143 & 3] = (long)((v ^ 1143) * w);
 tpad[1144 & 3] = (long)((v ^ 1144) * w);
 tpad[1145 & 3] = (long)((v ^ 1145) * w);
 tpad[1146 & 3] = (long)((v ^ 1146) * w);
 tpad[1147 & 3] = (long)((v ^ 1147) * w);
 tpad[1148 & 3] = (long)((v ^ 1148) * w);
 tpad[1149 & 3] = (long)((v ^ 1149) * w);
 tpad[1150 & 3] = (long)((v ^ 1150) * w);
 tpad[1151 & 3] = (long)((v ^ 1151) * w);
 tpad[1152 & 3] = (long)((v ^ 1152) * w);
 tpad[1153 & 3] = (long)((v ^ 1153) * w);
 tpad[1154 & 3] = (long)((v ^ 1154) * w);
 tpad[1155 & 3] = (long)((v ^ 1155) * w);
 tpad[1156 & 3] = (long)((v ^ 1156) * w);
 tpad[1157 & 3] = (long)((v ^ 1157) * w);
 tpad[1158 & 3] = (long)((v ^ 1158) * w);
 tpad[1159 & 3] = (long)((v ^ 1159) * w);
 tpad[1160 & 3] = (long)((v ^ 1160) * w);
 tpad[1161 & 3] = (long)((v ^ 1161) * w);
 tpad[1162 & 3] = (long)((v ^ 1162) * w);
 tpad[1163 & 3] = (long)((v ^ 1163) * w);
 tpad[1164 & 3] = (long)((v ^ 1164) * w);
 tpad[1165 & 3] = (long)((v ^ 1165) * w);
 tpad[1166 & 3] = (long)((v ^ 1166) * w);
 tpad[1167 & 3] = (long)((v ^ 1167) * w);
 tpad[1168 & 3] = (long)((v ^ 1168) * w);
 tpad[1169 & 3] = (long)((v ^ 1169) * w);
 tpad[1170 & 3] = (long)((v ^ 1170) * w);
 tpad[1171 & 3] = (long)((v ^ 1171) * w);
 tpad[1172 & 3] = (long)((v ^ 1172) * w);
 tpad[1173 & 3] = (long)((v ^ 1173) * w);
 tpad[1174 & 3] = (long)((v ^ 1174) * w);
 tpad[1175 & 3] = (long)((v ^ 1175) * w);
 tpad[1176 & 3] = (long)((v ^ 1176) * w);
 tpad[1177 & 3] = (long)((v ^ 1177) * w);
 tpad[1178 & 3] = (long)((v ^ 1178) * w);
 tpad[1179 & 3] = (long)((v ^ 1179) * w);
 tpad[1180 & 3] = (long)((v ^ 1180) * w);
 tpad[1181 & 3] = (long)((v ^ 1181) * w);
 tpad[1182 & 3] = (long)((v ^ 1182) * w);
 tpad[1183 & 3] = (long)((v ^ 1183) * w);
 tpad[1184 & 3] = (long)((v ^ 1184) * w);
 tpad[1185 & 3] = (long)((v ^ 1185) * w);
 tpad[1186 & 3] = (long)((v ^ 1186) * w);
 tpad[1187 & 3] = (long)((v ^ 1187) * w);
 tpad[1188 & 3] = (long)((v ^ 1188) * w);
 tpad[1189 & 3] = (long)((v ^ 1189) * w);
 tpad[1190 & 3] = (long)((v ^ 1190) * w);
 tpad[1191 & 3] = (long)((v ^ 1191) * w);
 tpad[1192 & 3] = (long)((v ^ 1192) * w);
 tpad[1193 & 3] = (long)((v ^ 1193) * w);
 tpad[1194 & 3] = (long)((v ^ 1194) * w);
 tpad[1195 & 3] = (long)((v ^ 1195) * w);
 tpad[1196 & 3] = (long)((v ^ 1196) * w);
 tpad[1197 & 3] = (long)((v ^ 1197) * w);
 tpad[1198 & 3] = (long)((v ^ 1198) * w);
 tpad[1199 & 3] = (long)((v ^ 1199) * w);
 tpad[1200 & 3] = (long)((v ^ 1200) * w);
 tpad[1201 & 3] = (long)((v ^ 1201) * w);
 tpad[1202 & 3] = (long)((v ^ 1202) * w);
 tpad[1203 & 3] = (long)((v ^ 1203) * w);
 tpad[1204 & 3] = (long)((v ^ 1204) * w);
 tpad[1205 & 3] = (long)((v ^ 1205) * w);
 tpad[1206 & 3] = (long)((v ^ 1206) * w);
 tpad[1207 & 3] = (long)((v ^ 1207) * w);
 tpad[1208 & 3] = (long)((v ^ 1208) * w);
 tpad[1209 & 3] = (long)((v ^ 1209) * w);
 tpad[1210 & 3] = (long)((v ^ 1210) * w);
 tpad[1211 & 3] = (long)((v ^ 1211) * w);
 tpad[1212 & 3] = (long)((v ^ 1212) * w);
 tpad[1213 & 3] = (long)((v ^ 1213) * w);
 tpad[1214 & 3] = (long)((v ^ 1214) * w);
 tpad[1215 & 3] = (long)((v ^ 1215) * w);
 tpad[1216 & 3] = (long)((v ^ 1216) * w);
 tpad[1217 & 3] = (long)((v ^ 1217) * w);
 tpad[1218 & 3] = (long)((v ^ 1218) * w);
 tpad[1219 & 3] = (long)((v ^ 1219) * w);
 tpad[1220 & 3] = (long)((v ^ 1220) * w);
 tpad[1221 & 3] = (long)((v ^ 1221) * w);
 tpad[1222 & 3] = (long)((v ^ 1222) * w);
 tpad[1223 & 3] = (long)((v ^ 1223) * w);
 tpad[1224 & 3] = (long)((v ^ 1224) * w);
 tpad[1225 & 3] = (long)((v ^ 1225) * w);
 tpad[1226 & 3] = (long)((v ^ 1226) * w);
 tpad[1227 & 3] = (long)((v ^ 1227) * w);
 tpad[1228 & 3] = (long)((v ^ 1228) * w);
 tpad[1229 & 3] = (long)((v ^ 1229) * w);
 tpad[1230 & 3] = (long)((v ^ 1230) * w);
 tpad[1231 & 3] = (long)((v ^ 1231) * w);
 tpad[1232 & 3] = (long)((v ^ 1232) * w);
 tpad[1233 & 3] = (long)((v ^ 1233) * w);
 tpad[1234 & 3] = (long)((v ^ 1234) * w);
 tpad[1235 & 3] = (long)((v ^ 1235) * w);
 tpad[1236 & 3] = (long)((v ^ 1236) * w);
 tpad[1237 & 3] = (long)((v ^ 1237) * w);
 tpad[1238 & 3] = (long)((v ^ 1238) * w);
 tpad[1239 & 3] = (long)((v ^ 1239) * w);
 tpad[1240 & 3] = (long)((v ^ 1240) * w);
 tpad[1241 & 3] = (long)((v ^ 1241) * w);
 tpad[1242 & 3] = (long)((v ^ 1242) * w);
 tpad[1243 & 3] = (long)((v ^ 1243) * w);
 tpad[1244 & 3] = (long)((v ^ 1244) * w);
 tpad[1245 & 3] = (long)((v ^ 1245) * w);
 tpad[1246 & 3] = (long)((v ^ 1246) * w);
 tpad[1247 & 3] = (long)((v ^ 1247) * w);
 tpad[1248 & 3] = (long)((v ^ 1248) * w);
 tpad[1249 & 3] = (long)((v ^ 1249) * w);
 tpad[1250 & 3] = (long)((v ^ 1250) * w);
 tpad[1251 & 3] = (long)((v ^ 1251) * w);
 tpad[1252 & 3] = (long)((v ^ 1252) * w);
 tpad[1253 & 3] = (long)((v ^ 1253) * w);
 tpad[1254 & 3] = (long)((v ^ 1254) * w);
 tpad[1255 & 3] = (long)((v ^ 1255) * w);
 tpad[1256 & 3] = (long)((v ^ 1256) * w);
 tpad[1257 & 3] = (long)((v ^ 1257) * w);
 tpad[1258 & 3] = (long)((v ^ 1258) * w);
 tpad[1259 & 3] = (long)((v ^ 1259) * w);
 tpad[1260 & 3] = (long)((v ^ 1260) * w);
 tpad[1261 & 3] = (long)((v ^ 1261) * w);
 tpad[1262 & 3] = (long)((v ^ 1262) * w);
 tpad[1263 & 3] = (long)((v ^ 1263) * w);
 tpad[1264 & 3] = (long)((v ^ 1264) * w);
 tpad[1265 & 3] = (long)((v ^ 1265) * w);
 tpad[1266 & 3] = (long)((v ^ 1266) * w);
 tpad[1267 & 3] = (long)((v ^ 1267) * w);
 tpad[1268 & 3] = (long)((v ^ 1268) * w);
 tpad[1269 & 3] = (long)((v ^ 1269) * w);
 tpad[1270 & 3] = (long)((v ^ 1270) * w);
 tpad[1271 & 3] = (long)((v ^ 1271) * w);
 tpad[1272 & 3] = (long)((v ^ 1272) * w);
 tpad[1273 & 3] = (long)((v ^ 1273) * w);
 tpad[1274 & 3] = (long)((v ^ 1274) * w);
 tpad[1275 & 3] = (long)((v ^ 1275) * w);
 tpad[1276 & 3] = (long)((v ^ 1276) * w);
 tpad[1277 & 3] = (long)((v ^ 1277) * w);
 tpad[1278 & 3] = (long)((v ^ 1278) * w);
 tpad[1279 & 3] = (long)((v ^ 1279) * w);
 tpad[1280 & 3] = (long)((v ^ 1280) * w);
 tpad[1281 & 3] = (long)((v ^ 1281) * w);
 tpad[1282 & 3] = (long)((v ^ 1282) * w);
 tpad[1283 & 3] = (long)((v ^ 1283) * w);
 tpad[1284 & 3] = (long)((v ^ 1284) * w);
 tpad[1285 & 3] = (long)((v ^ 1285) * w);
 tpad[1286 & 3] = (long)((v ^ 1286) * w);
 tpad[1287 & 3] = (long)((v ^ 1287) * w);
 tpad[1288 & 3] = (long)((v ^ 1288) * w);
 tpad[1289 & 3] = (long)((v ^ 1289) * w);
 tpad[1290 & 3] = (long)((v ^ 1290) * w);
 tpad[1291 & 3] = (long)((v ^ 1291) * w);
 tpad[1292 & 3] = (long)((v ^ 1292) * w);
 tpad[1293 & 3] = (long)((v ^ 1293) * w);
 tpad[1294 & 3] = (long)((v ^ 1294) * w);
 tpad[1295 & 3] = (long)((v ^ 1295) * w);
 tpad[1296 & 3] = (long)((v ^ 1296) * w);
 tpad[1297 & 3] = (long)((v ^ 1297) * w);
 tpad[1298 & 3] = (long)((v ^ 1298) * w);
 tpad[1299 & 3] = (long)((v ^ 1299) * w);
 tpad[1300 & 3] = (long)((v ^ 1300) * w);
 tpad[1301 & 3] = (long)((v ^ 1301) * w);
 tpad[1302 & 3] = (long)((v ^ 1302) * w);
 tpad[1303 & 3] = (long)((v ^ 1303) * w);
 tpad[1304 & 3] = (long)((v ^ 1304) * w);
 tpad[1305 & 3] = (long)((v ^ 1305) * w);
 tpad[1306 & 3] = (long)((v ^ 1306) * w);
 tpad[1307 & 3] = (long)((v ^ 1307) * w);
 tpad[1308 & 3] = (long)((v ^ 1308) * w);
 tpad[1309 & 3] = (long)((v ^ 1309) * w);
 tpad[1310 & 3] = (long)((v ^ 1310) * w);
 tpad[1311 & 3] = (long)((v ^ 1311) * w);
 tpad[1312 & 3] = (long)((v ^ 1312) * w);
 tpad[1313 & 3] = (long)((v ^ 1313) * w);
 tpad[1314 & 3] = (long)((v ^ 1314) * w);
 tpad[1315 & 3] = (long)((v ^ 1315) * w);
 tpad[1316 & 3] = (long)((v ^ 1316) * w);
 tpad[1317 & 3] = (long)((v ^ 1317) * w);
 tpad[1318 & 3] = (long)((v ^ 1318) * w);
 tpad[1319 & 3] = (long)((v ^ 1319) * w);
 tpad[1320 & 3] = (long)((v ^ 1320) * w);
 tpad[1321 & 3] = (long)((v ^ 1321) * w);
 tpad[1322 & 3] = (long)((v ^ 1322) * w);
 tpad[1323 & 3] = (long)((v ^ 1323) * w);
 tpad[1324 & 3] = (long)((v ^ 1324) * w);
 tpad[1325 & 3] = (long)((v ^ 1325) * w);
 tpad[1326 & 3] = (long)((v ^ 1326) * w);
 tpad[1327 & 3] = (long)((v ^ 1327) * w);
 tpad[1328 & 3] = (long)((v ^ 1328) * w);
 tpad[1329 & 3] = (long)((v ^ 1329) * w);
 tpad[1330 & 3] = (long)((v ^ 1330) * w);
 tpad[1331 & 3] = (long)((v ^ 1331) * w);
 tpad[1332 & 3] = (long)((v ^ 1332) * w);
 tpad[1333 & 3] = (long)((v ^ 1333) * w);
 tpad[1334 & 3] = (long)((v ^ 1334) * w);
 tpad[1335 & 3] = (long)((v ^ 1335) * w);
 tpad[1336 & 3] = (long)((v ^ 1336) * w);
 tpad[1337 & 3] = (long)((v ^ 1337) * w);
 tpad[1338 & 3] = (long)((v ^ 1338) * w);
 tpad[1339 & 3] = (long)((v ^ 1339) * w);
 tpad[1340 & 3] = (long)((v ^ 1340) * w);
 tpad[1341 & 3] = (long)((v ^ 1341) * w);
 tpad[1342 & 3] = (long)((v ^ 1342) * w);
 tpad[1343 & 3] = (long)((v ^ 1343) * w);
 tpad[1344 & 3] = (long)((v ^ 1344) * w);
 tpad[1345 & 3] = (long)((v ^ 1345) * w);
 tpad[1346 & 3] = (long)((v ^ 1346) * w);
 tpad[1347 & 3] = (long)((v ^ 1347) * w);
 tpad[1348 & 3] = (long)((v ^ 1348) * w);
 tpad[1349 & 3] = (long)((v ^ 1349) * w);
 tpad[1350 & 3] = (long)((v ^ 1350) * w);
 tpad[1351 & 3] = (long)((v ^ 1351) * w);
 tpad[1352 & 3] = (long)((v ^ 1352) * w);
 tpad[1353 & 3] = (long)((v ^ 1353) * w);
 tpad[1354 & 3] = (long)((v ^ 1354) * w);
 tpad[1355 & 3] = (long)((v ^ 1355) * w);
 tpad[1356 & 3] = (long)((v ^ 1356) * w);
 tpad[1357 & 3] = (long)((v ^ 1357) * w);
 tpad[1358 & 3] = (long)((v ^ 1358) * w);
 tpad[1359 & 3] = (long)((v ^ 1359) * w);
 tpad[1360 & 3] = (long)((v ^ 1360) * w);
 tpad[1361 & 3] = (long)((v ^ 1361) * w);
 tpad[1362 & 3] = (long)((v ^ 1362) * w);
 tpad[1363 & 3] = (long)((v ^ 1363) * w);
 tpad[1364 & 3] = (long)((v ^ 1364) * w);
 tpad[1365 & 3] = (long)((v ^ 1365) * w);
 tpad[1366 & 3] = (long)((v ^ 1366) * w);
 tpad[1367 & 3] = (long)((v ^ 1367) * w);
 tpad[1368 & 3] = (long)((v ^ 1368) * w);
 tpad[1369 & 3] = (long)((v ^ 1369) * w);
 tpad[1370 & 3] = (long)((v ^ 1370) * w);
 tpad[1371 & 3] = (long)((v ^ 1371) * w);
 tpad[1372 & 3] = (long)((v ^ 1372) * w);
 tpad[1373 & 3] = (long)((v ^ 1373) * w);
 tpad[1374 & 3] = (long)((v ^ 1374) * w);
 tpad[1375 & 3] = (long)((v ^ 1375) * w);
 tpad[1376 & 3] = (long)((v ^ 1376) * w);
 tpad[1377 & 3] = (long)((v ^ 1377) * w);
 tpad[1378 & 3] = (long)((v ^ 1378) * w);
 tpad[1379 & 3] = (long)((v ^ 1379) * w);
 tpad[1380 & 3] = (long)((v ^ 1380) * w);
 tpad[1381 & 3] = (long)((v ^ 1381) * w);
 tpad[1382 & 3] = (long)((v ^ 1382) * w);
 tpad[1383 & 3] = (long)((v ^ 1383) * w);
 tpad[1384 & 3] = (long)((v ^ 1384) * w);
 tpad[1385 & 3] = (long)((v ^ 1385) * w);
 tpad[1386 & 3] = (long)((v ^ 1386) * w);
 tpad[1387 & 3] = (long)((v ^ 1387) * w);
 tpad[1388 & 3] = (long)((v ^ 1388) * w);
 tpad[1389 & 3] = (long)((v ^ 1389) * w);
 tpad[1390 & 3] = (long)((v ^ 1390) * w);
 tpad[1391 & 3] = (long)((v ^ 1391) * w);
 tpad[1392 & 3] = (long)((v ^ 1392) * w);
 tpad[1393 & 3] = (long)((v ^ 1393) * w);
 tpad[1394 & 3] = (long)((v ^ 1394) * w);
 tpad[1395 & 3] = (long)((v ^ 1395) * w);
 tpad[1396 & 3] = (long)((v ^ 1396) * w);
 tpad[1397 & 3] = (long)((v ^ 1397) * w);
 tpad[1398 & 3] = (long)((v ^ 1398) * w);
 tpad[1399 & 3] = (long)((v ^ 1399) * w);
 tpad[1400 & 3] = (long)((v ^ 1400) * w);
 tpad[1401 & 3] = (long)((v ^ 1401) * w);
 tpad[1402 & 3] = (long)((v ^ 1402) * w);
 tpad[1403 & 3] = (long)((v ^ 1403) * w);
 tpad[1404 & 3] = (long)((v ^ 1404) * w);
 tpad[1405 & 3] = (long)((v ^ 1405) * w);
 tpad[1406 & 3] = (long)((v ^ 1406) * w);
 tpad[1407 & 3] = (long)((v ^ 1407) * w);
 tpad[1408 & 3] = (long)((v ^ 1408) * w);
 tpad[1409 & 3] = (long)((v ^ 1409) * w);
 tpad[1410 & 3] = (long)((v ^ 1410) * w);
 tpad[1411 & 3] = (long)((v ^ 1411) * w);
 tpad[1412 & 3] = (long)((v ^ 1412) * w);
 tpad[1413 & 3] = (long)((v ^ 1413) * w);
 tpad[1414 & 3] = (long)((v ^ 1414) * w);
 tpad[1415 & 3] = (long)((v ^ 1415) * w);
 tpad[1416 & 3] = (long)((v ^ 1416) * w);
 tpad[1417 & 3] = (long)((v ^ 1417) * w);
 tpad[1418 & 3] = (long)((v ^ 1418) * w);
 tpad[1419 & 3] = (long)((v ^ 1419) * w);
 tpad[1420 & 3] = (long)((v ^ 1420) * w);
 tpad[1421 & 3] = (long)((v ^ 1421) * w);
 tpad[1422 & 3] = (long)((v ^ 1422) * w);
 tpad[1423 & 3] = (long)((v ^ 1423) * w);
 tpad[1424 & 3] = (long)((v ^ 1424) * w);
 tpad[1425 & 3] = (long)((v ^ 1425) * w);
 tpad[1426 & 3] = (long)((v ^ 1426) * w);
 tpad[1427 & 3] = (long)((v ^ 1427) * w);
 tpad[1428 & 3] = (long)((v ^ 1428) * w);
 tpad[1429 & 3] = (long)((v ^ 1429) * w);
 tpad[1430 & 3] = (long)((v ^ 1430) * w);
 tpad[1431 & 3] = (long)((v ^ 1431) * w);
 tpad[1432 & 3] = (long)((v ^ 1432) * w);
 tpad[1433 & 3] = (long)((v ^ 1433) * w);
 tpad[1434 & 3] = (long)((v ^ 1434) * w);
 tpad[1435 & 3] = (long)((v ^ 1435) * w);
 tpad[1436 & 3] = (long)((v ^ 1436) * w);
 tpad[1437 & 3] = (long)((v ^ 1437) * w);
 tpad[1438 & 3] = (long)((v ^ 1438) * w);
 tpad[1439 & 3] = (long)((v ^ 1439) * w);
 tpad[1440 & 3] = (long)((v ^ 1440) * w);
 tpad[1441 & 3] = (long)((v ^ 1441) * w);
 tpad[1442 & 3] = (long)((v ^ 1442) * w);
 tpad[1443 & 3] = (long)((v ^ 1443) * w);
 tpad[1444 & 3] = (long)((v ^ 1444) * w);
 tpad[1445 & 3] = (long)((v ^ 1445) * w);
 tpad[1446 & 3] = (long)((v ^ 1446) * w);
 tpad[1447 & 3] = (long)((v ^ 1447) * w);
 tpad[1448 & 3] = (long)((v ^ 1448) * w);
 tpad[1449 & 3] = (long)((v ^ 1449) * w);
 tpad[1450 & 3] = (long)((v ^ 1450) * w);
 tpad[1451 & 3] = (long)((v ^ 1451) * w);
 tpad[1452 & 3] = (long)((v ^ 1452) * w);
 tpad[1453 & 3] = (long)((v ^ 1453) * w);
 tpad[1454 & 3] = (long)((v ^ 1454) * w);
 tpad[1455 & 3] = (long)((v ^ 1455) * w);
 tpad[1456 & 3] = (long)((v ^ 1456) * w);
 tpad[1457 & 3] = (long)((v ^ 1457) * w);
 tpad[1458 & 3] = (long)((v ^ 1458) * w);
 tpad[1459 & 3] = (long)((v ^ 1459) * w);
 tpad[1460 & 3] = (long)((v ^ 1460) * w);
 tpad[1461 & 3] = (long)((v ^ 1461) * w);
 tpad[1462 & 3] = (long)((v ^ 1462) * w);
 tpad[1463 & 3] = (long)((v ^ 1463) * w);
 tpad[1464 & 3] = (long)((v ^ 1464) * w);
 tpad[1465 & 3] = (long)((v ^ 1465) * w);
 tpad[1466 & 3] = (long)((v ^ 1466) * w);
 tpad[1467 & 3] = (long)((v ^ 1467) * w);
 tpad[1468 & 3] = (long)((v ^ 1468) * w);
 tpad[1469 & 3] = (long)((v ^ 1469) * w);
 tpad[1470 & 3] = (long)((v ^ 1470) * w);
 tpad[1471 & 3] = (long)((v ^ 1471) * w);
 tpad[1472 & 3] = (long)((v ^ 1472) * w);
 tpad[1473 & 3] = (long)((v ^ 1473) * w);
 tpad[1474 & 3] = (long)((v ^ 1474) * w);
 tpad[1475 & 3] = (long)((v ^ 1475) * w);
 tpad[1476 & 3] = (long)((v ^ 1476) * w);
 tpad[1477 & 3] = (long)((v ^ 1477) * w);
 tpad[1478 & 3] = (long)((v ^ 1478) * w);
 tpad[1479 & 3] = (long)((v ^ 1479) * w);
 tpad[1480 & 3] = (long)((v ^ 1480) * w);
 tpad[1481 & 3] = (long)((v ^ 1481) * w);
 tpad[1482 & 3] = (long)((v ^ 1482) * w);
 tpad[1483 & 3] = (long)((v ^ 1483) * w);
 tpad[1484 & 3] = (long)((v ^ 1484) * w);
 tpad[1485 & 3] = (long)((v ^ 1485) * w);
 tpad[1486 & 3] = (long)((v ^ 1486) * w);
 tpad[1487 & 3] = (long)((v ^ 1487) * w);
 tpad[1488 & 3] = (long)((v ^ 1488) * w);
 tpad[1489 & 3] = (long)((v ^ 1489) * w);
 tpad[1490 & 3] = (long)((v ^ 1490) * w);
 tpad[1491 & 3] = (long)((v ^ 1491) * w);
 tpad[1492 & 3] = (long)((v ^ 1492) * w);
 tpad[1493 & 3] = (long)((v ^ 1493) * w);
 tpad[1494 & 3] = (long)((v ^ 1494) * w);
 tpad[1495 & 3] = (long)((v ^ 1495) * w);
 tpad[1496 & 3] = (long)((v ^ 1496) * w);
 tpad[1497 & 3] = (long)((v ^ 1497) * w);
 tpad[1498 & 3] = (long)((v ^ 1498) * w);
 tpad[1499 & 3] = (long)((v ^ 1499) * w);
 tpad[1500 & 3] = (long)((v ^ 1500) * w);
 tpad[1501 & 3] = (long)((v ^ 1501) * w);
 tpad[1502 & 3] = (long)((v ^ 1502) * w);
 tpad[1503 & 3] = (long)((v ^ 1503) * w);
 tpad[1504 & 3] = (long)((v ^ 1504) * w);
 tpad[1505 & 3] = (long)((v ^ 1505) * w);
 tpad[1506 & 3] = (long)((v ^ 1506) * w);
 tpad[1507 & 3] = (long)((v ^ 1507) * w);
 tpad[1508 & 3] = (long)((v ^ 1508) * w);
 tpad[1509 & 3] = (long)((v ^ 1509) * w);
 tpad[1510 & 3] = (long)((v ^ 1510) * w);
 tpad[1511 & 3] = (long)((v ^ 1511) * w);
 tpad[1512 & 3] = (long)((v ^ 1512) * w);
 tpad[1513 & 3] = (long)((v ^ 1513) * w);
 tpad[1514 & 3] = (long)((v ^ 1514) * w);
 tpad[1515 & 3] = (long)((v ^ 1515) * w);
 tpad[1516 & 3] = (long)((v ^ 1516) * w);
 tpad[1517 & 3] = (long)((v ^ 1517) * w);
 tpad[1518 & 3] = (long)((v ^ 1518) * w);
 tpad[1519 & 3] = (long)((v ^ 1519) * w);
 tpad[1520 & 3] = (long)((v ^ 1520) * w);
 tpad[1521 & 3] = (long)((v ^ 1521) * w);
 tpad[1522 & 3] = (long)((v ^ 1522) * w);
 tpad[1523 & 3] = (long)((v ^ 1523) * w);
 tpad[1524 & 3] = (long)((v ^ 1524) * w);
 tpad[1525 & 3] = (long)((v ^ 1525) * w);
 tpad[1526 & 3] = (long)((v ^ 1526) * w);
 tpad[1527 & 3] = (long)((v ^ 1527) * w);
 tpad[1528 & 3] = (long)((v ^ 1528) * w);
 tpad[1529 & 3] = (long)((v ^ 1529) * w);
 tpad[1530 & 3] = (long)((v ^ 1530) * w);
 tpad[1531 & 3] = (long)((v ^ 1531) * w);
 tpad[1532 & 3] = (long)((v ^ 1532) * w);
 tpad[1533 & 3] = (long)((v ^ 1533) * w);
 tpad[1534 & 3] = (long)((v ^ 1534) * w);
 tpad[1535 & 3] = (long)((v ^ 1535) * w);
 tpad[1536 & 3] = (long)((v ^ 1536) * w);
 tpad[1537 & 3] = (long)((v ^ 1537) * w);
 tpad[1538 & 3] = (long)((v ^ 1538) * w);
 tpad[1539 & 3] = (long)((v ^ 1539) * w);
}

template <>
__forceinline void animation_compute_orientations_walk_pad2<c_8byte_quantized_rotation_only_interpolated_codec>(long v, long w)
{
 long tpad[4];
 tpad[0 & 3] = (long)((v ^ 0) * w);
 tpad[1 & 3] = (long)((v ^ 1) * w);
 tpad[2 & 3] = (long)((v ^ 2) * w);
 tpad[3 & 3] = (long)((v ^ 3) * w);
 tpad[4 & 3] = (long)((v ^ 4) * w);
 tpad[5 & 3] = (long)((v ^ 5) * w);
 tpad[6 & 3] = (long)((v ^ 6) * w);
 tpad[7 & 3] = (long)((v ^ 7) * w);
 tpad[8 & 3] = (long)((v ^ 8) * w);
 tpad[9 & 3] = (long)((v ^ 9) * w);
 tpad[10 & 3] = (long)((v ^ 10) * w);
 tpad[11 & 3] = (long)((v ^ 11) * w);
 tpad[12 & 3] = (long)((v ^ 12) * w);
 tpad[13 & 3] = (long)((v ^ 13) * w);
 tpad[14 & 3] = (long)((v ^ 14) * w);
 tpad[15 & 3] = (long)((v ^ 15) * w);
 tpad[16 & 3] = (long)((v ^ 16) * w);
 tpad[17 & 3] = (long)((v ^ 17) * w);
 tpad[18 & 3] = (long)((v ^ 18) * w);
 tpad[19 & 3] = (long)((v ^ 19) * w);
 tpad[20 & 3] = (long)((v ^ 20) * w);
 tpad[21 & 3] = (long)((v ^ 21) * w);
 tpad[22 & 3] = (long)((v ^ 22) * w);
 tpad[23 & 3] = (long)((v ^ 23) * w);
 tpad[24 & 3] = (long)((v ^ 24) * w);
 tpad[25 & 3] = (long)((v ^ 25) * w);
 tpad[26 & 3] = (long)((v ^ 26) * w);
 tpad[27 & 3] = (long)((v ^ 27) * w);
 tpad[28 & 3] = (long)((v ^ 28) * w);
 tpad[29 & 3] = (long)((v ^ 29) * w);
 tpad[30 & 3] = (long)((v ^ 30) * w);
 tpad[31 & 3] = (long)((v ^ 31) * w);
 tpad[32 & 3] = (long)((v ^ 32) * w);
 tpad[33 & 3] = (long)((v ^ 33) * w);
 tpad[34 & 3] = (long)((v ^ 34) * w);
 tpad[35 & 3] = (long)((v ^ 35) * w);
 tpad[36 & 3] = (long)((v ^ 36) * w);
 tpad[37 & 3] = (long)((v ^ 37) * w);
 tpad[38 & 3] = (long)((v ^ 38) * w);
 tpad[39 & 3] = (long)((v ^ 39) * w);
 tpad[40 & 3] = (long)((v ^ 40) * w);
 tpad[41 & 3] = (long)((v ^ 41) * w);
 tpad[42 & 3] = (long)((v ^ 42) * w);
 tpad[43 & 3] = (long)((v ^ 43) * w);
 tpad[44 & 3] = (long)((v ^ 44) * w);
 tpad[45 & 3] = (long)((v ^ 45) * w);
 tpad[46 & 3] = (long)((v ^ 46) * w);
 tpad[47 & 3] = (long)((v ^ 47) * w);
 tpad[48 & 3] = (long)((v ^ 48) * w);
 tpad[49 & 3] = (long)((v ^ 49) * w);
 tpad[50 & 3] = (long)((v ^ 50) * w);
 tpad[51 & 3] = (long)((v ^ 51) * w);
 tpad[52 & 3] = (long)((v ^ 52) * w);
 tpad[53 & 3] = (long)((v ^ 53) * w);
 tpad[54 & 3] = (long)((v ^ 54) * w);
 tpad[55 & 3] = (long)((v ^ 55) * w);
 tpad[56 & 3] = (long)((v ^ 56) * w);
 tpad[57 & 3] = (long)((v ^ 57) * w);
 tpad[58 & 3] = (long)((v ^ 58) * w);
 tpad[59 & 3] = (long)((v ^ 59) * w);
 tpad[60 & 3] = (long)((v ^ 60) * w);
 tpad[61 & 3] = (long)((v ^ 61) * w);
 tpad[62 & 3] = (long)((v ^ 62) * w);
 tpad[63 & 3] = (long)((v ^ 63) * w);
 tpad[64 & 3] = (long)((v ^ 64) * w);
 tpad[65 & 3] = (long)((v ^ 65) * w);
 tpad[66 & 3] = (long)((v ^ 66) * w);
 tpad[67 & 3] = (long)((v ^ 67) * w);
 tpad[68 & 3] = (long)((v ^ 68) * w);
 tpad[69 & 3] = (long)((v ^ 69) * w);
 tpad[70 & 3] = (long)((v ^ 70) * w);
 tpad[71 & 3] = (long)((v ^ 71) * w);
 tpad[72 & 3] = (long)((v ^ 72) * w);
 tpad[73 & 3] = (long)((v ^ 73) * w);
 tpad[74 & 3] = (long)((v ^ 74) * w);
 tpad[75 & 3] = (long)((v ^ 75) * w);
 tpad[76 & 3] = (long)((v ^ 76) * w);
 tpad[77 & 3] = (long)((v ^ 77) * w);
 tpad[78 & 3] = (long)((v ^ 78) * w);
 tpad[79 & 3] = (long)((v ^ 79) * w);
 tpad[80 & 3] = (long)((v ^ 80) * w);
 tpad[81 & 3] = (long)((v ^ 81) * w);
 tpad[82 & 3] = (long)((v ^ 82) * w);
 tpad[83 & 3] = (long)((v ^ 83) * w);
 tpad[84 & 3] = (long)((v ^ 84) * w);
 tpad[85 & 3] = (long)((v ^ 85) * w);
 tpad[86 & 3] = (long)((v ^ 86) * w);
 tpad[87 & 3] = (long)((v ^ 87) * w);
 tpad[88 & 3] = (long)((v ^ 88) * w);
 tpad[89 & 3] = (long)((v ^ 89) * w);
 tpad[90 & 3] = (long)((v ^ 90) * w);
 tpad[91 & 3] = (long)((v ^ 91) * w);
 tpad[92 & 3] = (long)((v ^ 92) * w);
 tpad[93 & 3] = (long)((v ^ 93) * w);
 tpad[94 & 3] = (long)((v ^ 94) * w);
 tpad[95 & 3] = (long)((v ^ 95) * w);
 tpad[96 & 3] = (long)((v ^ 96) * w);
 tpad[97 & 3] = (long)((v ^ 97) * w);
 tpad[98 & 3] = (long)((v ^ 98) * w);
 tpad[99 & 3] = (long)((v ^ 99) * w);
 tpad[100 & 3] = (long)((v ^ 100) * w);
 tpad[101 & 3] = (long)((v ^ 101) * w);
 tpad[102 & 3] = (long)((v ^ 102) * w);
 tpad[103 & 3] = (long)((v ^ 103) * w);
 tpad[104 & 3] = (long)((v ^ 104) * w);
 tpad[105 & 3] = (long)((v ^ 105) * w);
 tpad[106 & 3] = (long)((v ^ 106) * w);
 tpad[107 & 3] = (long)((v ^ 107) * w);
 tpad[108 & 3] = (long)((v ^ 108) * w);
 tpad[109 & 3] = (long)((v ^ 109) * w);
 tpad[110 & 3] = (long)((v ^ 110) * w);
 tpad[111 & 3] = (long)((v ^ 111) * w);
 tpad[112 & 3] = (long)((v ^ 112) * w);
 tpad[113 & 3] = (long)((v ^ 113) * w);
 tpad[114 & 3] = (long)((v ^ 114) * w);
 tpad[115 & 3] = (long)((v ^ 115) * w);
 tpad[116 & 3] = (long)((v ^ 116) * w);
 tpad[117 & 3] = (long)((v ^ 117) * w);
 tpad[118 & 3] = (long)((v ^ 118) * w);
 tpad[119 & 3] = (long)((v ^ 119) * w);
 tpad[120 & 3] = (long)((v ^ 120) * w);
 tpad[121 & 3] = (long)((v ^ 121) * w);
 tpad[122 & 3] = (long)((v ^ 122) * w);
 tpad[123 & 3] = (long)((v ^ 123) * w);
 tpad[124 & 3] = (long)((v ^ 124) * w);
 tpad[125 & 3] = (long)((v ^ 125) * w);
 tpad[126 & 3] = (long)((v ^ 126) * w);
 tpad[127 & 3] = (long)((v ^ 127) * w);
 tpad[128 & 3] = (long)((v ^ 128) * w);
 tpad[129 & 3] = (long)((v ^ 129) * w);
 tpad[130 & 3] = (long)((v ^ 130) * w);
 tpad[131 & 3] = (long)((v ^ 131) * w);
 tpad[132 & 3] = (long)((v ^ 132) * w);
 tpad[133 & 3] = (long)((v ^ 133) * w);
 tpad[134 & 3] = (long)((v ^ 134) * w);
 tpad[135 & 3] = (long)((v ^ 135) * w);
 tpad[136 & 3] = (long)((v ^ 136) * w);
 tpad[137 & 3] = (long)((v ^ 137) * w);
 tpad[138 & 3] = (long)((v ^ 138) * w);
 tpad[139 & 3] = (long)((v ^ 139) * w);
 tpad[140 & 3] = (long)((v ^ 140) * w);
 tpad[141 & 3] = (long)((v ^ 141) * w);
 tpad[142 & 3] = (long)((v ^ 142) * w);
 tpad[143 & 3] = (long)((v ^ 143) * w);
 tpad[144 & 3] = (long)((v ^ 144) * w);
 tpad[145 & 3] = (long)((v ^ 145) * w);
 tpad[146 & 3] = (long)((v ^ 146) * w);
 tpad[147 & 3] = (long)((v ^ 147) * w);
 tpad[148 & 3] = (long)((v ^ 148) * w);
 tpad[149 & 3] = (long)((v ^ 149) * w);
 tpad[150 & 3] = (long)((v ^ 150) * w);
 tpad[151 & 3] = (long)((v ^ 151) * w);
 tpad[152 & 3] = (long)((v ^ 152) * w);
 tpad[153 & 3] = (long)((v ^ 153) * w);
 tpad[154 & 3] = (long)((v ^ 154) * w);
 tpad[155 & 3] = (long)((v ^ 155) * w);
 tpad[156 & 3] = (long)((v ^ 156) * w);
 tpad[157 & 3] = (long)((v ^ 157) * w);
 tpad[158 & 3] = (long)((v ^ 158) * w);
 tpad[159 & 3] = (long)((v ^ 159) * w);
 tpad[160 & 3] = (long)((v ^ 160) * w);
 tpad[161 & 3] = (long)((v ^ 161) * w);
 tpad[162 & 3] = (long)((v ^ 162) * w);
 tpad[163 & 3] = (long)((v ^ 163) * w);
 tpad[164 & 3] = (long)((v ^ 164) * w);
 tpad[165 & 3] = (long)((v ^ 165) * w);
 tpad[166 & 3] = (long)((v ^ 166) * w);
 tpad[167 & 3] = (long)((v ^ 167) * w);
 tpad[168 & 3] = (long)((v ^ 168) * w);
 tpad[169 & 3] = (long)((v ^ 169) * w);
 tpad[170 & 3] = (long)((v ^ 170) * w);
 tpad[171 & 3] = (long)((v ^ 171) * w);
 tpad[172 & 3] = (long)((v ^ 172) * w);
 tpad[173 & 3] = (long)((v ^ 173) * w);
 tpad[174 & 3] = (long)((v ^ 174) * w);
 tpad[175 & 3] = (long)((v ^ 175) * w);
 tpad[176 & 3] = (long)((v ^ 176) * w);
 tpad[177 & 3] = (long)((v ^ 177) * w);
 tpad[178 & 3] = (long)((v ^ 178) * w);
 tpad[179 & 3] = (long)((v ^ 179) * w);
 tpad[180 & 3] = (long)((v ^ 180) * w);
 tpad[181 & 3] = (long)((v ^ 181) * w);
 tpad[182 & 3] = (long)((v ^ 182) * w);
 tpad[183 & 3] = (long)((v ^ 183) * w);
 tpad[184 & 3] = (long)((v ^ 184) * w);
 tpad[185 & 3] = (long)((v ^ 185) * w);
 tpad[186 & 3] = (long)((v ^ 186) * w);
 tpad[187 & 3] = (long)((v ^ 187) * w);
 tpad[188 & 3] = (long)((v ^ 188) * w);
 tpad[189 & 3] = (long)((v ^ 189) * w);
 tpad[190 & 3] = (long)((v ^ 190) * w);
 tpad[191 & 3] = (long)((v ^ 191) * w);
 tpad[192 & 3] = (long)((v ^ 192) * w);
 tpad[193 & 3] = (long)((v ^ 193) * w);
 tpad[194 & 3] = (long)((v ^ 194) * w);
 tpad[195 & 3] = (long)((v ^ 195) * w);
 tpad[196 & 3] = (long)((v ^ 196) * w);
 tpad[197 & 3] = (long)((v ^ 197) * w);
 tpad[198 & 3] = (long)((v ^ 198) * w);
 tpad[199 & 3] = (long)((v ^ 199) * w);
 tpad[200 & 3] = (long)((v ^ 200) * w);
 tpad[201 & 3] = (long)((v ^ 201) * w);
 tpad[202 & 3] = (long)((v ^ 202) * w);
 tpad[203 & 3] = (long)((v ^ 203) * w);
 tpad[204 & 3] = (long)((v ^ 204) * w);
 tpad[205 & 3] = (long)((v ^ 205) * w);
 tpad[206 & 3] = (long)((v ^ 206) * w);
 tpad[207 & 3] = (long)((v ^ 207) * w);
 tpad[208 & 3] = (long)((v ^ 208) * w);
 tpad[209 & 3] = (long)((v ^ 209) * w);
 tpad[210 & 3] = (long)((v ^ 210) * w);
 tpad[211 & 3] = (long)((v ^ 211) * w);
 tpad[212 & 3] = (long)((v ^ 212) * w);
 tpad[213 & 3] = (long)((v ^ 213) * w);
 tpad[214 & 3] = (long)((v ^ 214) * w);
 tpad[215 & 3] = (long)((v ^ 215) * w);
 tpad[216 & 3] = (long)((v ^ 216) * w);
 tpad[217 & 3] = (long)((v ^ 217) * w);
 tpad[218 & 3] = (long)((v ^ 218) * w);
 tpad[219 & 3] = (long)((v ^ 219) * w);
 tpad[220 & 3] = (long)((v ^ 220) * w);
 tpad[221 & 3] = (long)((v ^ 221) * w);
 tpad[222 & 3] = (long)((v ^ 222) * w);
 tpad[223 & 3] = (long)((v ^ 223) * w);
 tpad[224 & 3] = (long)((v ^ 224) * w);
 tpad[225 & 3] = (long)((v ^ 225) * w);
 tpad[226 & 3] = (long)((v ^ 226) * w);
 tpad[227 & 3] = (long)((v ^ 227) * w);
 tpad[228 & 3] = (long)((v ^ 228) * w);
 tpad[229 & 3] = (long)((v ^ 229) * w);
 tpad[230 & 3] = (long)((v ^ 230) * w);
 tpad[231 & 3] = (long)((v ^ 231) * w);
 tpad[232 & 3] = (long)((v ^ 232) * w);
 tpad[233 & 3] = (long)((v ^ 233) * w);
 tpad[234 & 3] = (long)((v ^ 234) * w);
 tpad[235 & 3] = (long)((v ^ 235) * w);
 tpad[236 & 3] = (long)((v ^ 236) * w);
 tpad[237 & 3] = (long)((v ^ 237) * w);
 tpad[238 & 3] = (long)((v ^ 238) * w);
 tpad[239 & 3] = (long)((v ^ 239) * w);
 tpad[240 & 3] = (long)((v ^ 240) * w);
 tpad[241 & 3] = (long)((v ^ 241) * w);
 tpad[242 & 3] = (long)((v ^ 242) * w);
 tpad[243 & 3] = (long)((v ^ 243) * w);
 tpad[244 & 3] = (long)((v ^ 244) * w);
 tpad[245 & 3] = (long)((v ^ 245) * w);
 tpad[246 & 3] = (long)((v ^ 246) * w);
 tpad[247 & 3] = (long)((v ^ 247) * w);
 tpad[248 & 3] = (long)((v ^ 248) * w);
 tpad[249 & 3] = (long)((v ^ 249) * w);
 tpad[250 & 3] = (long)((v ^ 250) * w);
 tpad[251 & 3] = (long)((v ^ 251) * w);
 tpad[252 & 3] = (long)((v ^ 252) * w);
 tpad[253 & 3] = (long)((v ^ 253) * w);
 tpad[254 & 3] = (long)((v ^ 254) * w);
 tpad[255 & 3] = (long)((v ^ 255) * w);
 tpad[256 & 3] = (long)((v ^ 256) * w);
 tpad[257 & 3] = (long)((v ^ 257) * w);
 tpad[258 & 3] = (long)((v ^ 258) * w);
 tpad[259 & 3] = (long)((v ^ 259) * w);
 tpad[260 & 3] = (long)((v ^ 260) * w);
 tpad[261 & 3] = (long)((v ^ 261) * w);
 tpad[262 & 3] = (long)((v ^ 262) * w);
 tpad[263 & 3] = (long)((v ^ 263) * w);
 tpad[264 & 3] = (long)((v ^ 264) * w);
 tpad[265 & 3] = (long)((v ^ 265) * w);
 tpad[266 & 3] = (long)((v ^ 266) * w);
 tpad[267 & 3] = (long)((v ^ 267) * w);
 tpad[268 & 3] = (long)((v ^ 268) * w);
 tpad[269 & 3] = (long)((v ^ 269) * w);
 tpad[270 & 3] = (long)((v ^ 270) * w);
 tpad[271 & 3] = (long)((v ^ 271) * w);
 tpad[272 & 3] = (long)((v ^ 272) * w);
 tpad[273 & 3] = (long)((v ^ 273) * w);
 tpad[274 & 3] = (long)((v ^ 274) * w);
 tpad[275 & 3] = (long)((v ^ 275) * w);
 tpad[276 & 3] = (long)((v ^ 276) * w);
 tpad[277 & 3] = (long)((v ^ 277) * w);
 tpad[278 & 3] = (long)((v ^ 278) * w);
 tpad[279 & 3] = (long)((v ^ 279) * w);
 tpad[280 & 3] = (long)((v ^ 280) * w);
 tpad[281 & 3] = (long)((v ^ 281) * w);
 tpad[282 & 3] = (long)((v ^ 282) * w);
 tpad[283 & 3] = (long)((v ^ 283) * w);
 tpad[284 & 3] = (long)((v ^ 284) * w);
 tpad[285 & 3] = (long)((v ^ 285) * w);
 tpad[286 & 3] = (long)((v ^ 286) * w);
 tpad[287 & 3] = (long)((v ^ 287) * w);
 tpad[288 & 3] = (long)((v ^ 288) * w);
 tpad[289 & 3] = (long)((v ^ 289) * w);
 tpad[290 & 3] = (long)((v ^ 290) * w);
 tpad[291 & 3] = (long)((v ^ 291) * w);
 tpad[292 & 3] = (long)((v ^ 292) * w);
 tpad[293 & 3] = (long)((v ^ 293) * w);
 tpad[294 & 3] = (long)((v ^ 294) * w);
 tpad[295 & 3] = (long)((v ^ 295) * w);
 tpad[296 & 3] = (long)((v ^ 296) * w);
 tpad[297 & 3] = (long)((v ^ 297) * w);
 tpad[298 & 3] = (long)((v ^ 298) * w);
 tpad[299 & 3] = (long)((v ^ 299) * w);
 tpad[300 & 3] = (long)((v ^ 300) * w);
 tpad[301 & 3] = (long)((v ^ 301) * w);
 tpad[302 & 3] = (long)((v ^ 302) * w);
 tpad[303 & 3] = (long)((v ^ 303) * w);
 tpad[304 & 3] = (long)((v ^ 304) * w);
 tpad[305 & 3] = (long)((v ^ 305) * w);
 tpad[306 & 3] = (long)((v ^ 306) * w);
 tpad[307 & 3] = (long)((v ^ 307) * w);
 tpad[308 & 3] = (long)((v ^ 308) * w);
 tpad[309 & 3] = (long)((v ^ 309) * w);
 tpad[310 & 3] = (long)((v ^ 310) * w);
 tpad[311 & 3] = (long)((v ^ 311) * w);
 tpad[312 & 3] = (long)((v ^ 312) * w);
 tpad[313 & 3] = (long)((v ^ 313) * w);
 tpad[314 & 3] = (long)((v ^ 314) * w);
 tpad[315 & 3] = (long)((v ^ 315) * w);
 tpad[316 & 3] = (long)((v ^ 316) * w);
 tpad[317 & 3] = (long)((v ^ 317) * w);
 tpad[318 & 3] = (long)((v ^ 318) * w);
 tpad[319 & 3] = (long)((v ^ 319) * w);
 tpad[320 & 3] = (long)((v ^ 320) * w);
 tpad[321 & 3] = (long)((v ^ 321) * w);
 tpad[322 & 3] = (long)((v ^ 322) * w);
 tpad[323 & 3] = (long)((v ^ 323) * w);
 tpad[324 & 3] = (long)((v ^ 324) * w);
 tpad[325 & 3] = (long)((v ^ 325) * w);
 tpad[326 & 3] = (long)((v ^ 326) * w);
 tpad[327 & 3] = (long)((v ^ 327) * w);
 tpad[328 & 3] = (long)((v ^ 328) * w);
 tpad[329 & 3] = (long)((v ^ 329) * w);
 tpad[330 & 3] = (long)((v ^ 330) * w);
 tpad[331 & 3] = (long)((v ^ 331) * w);
 tpad[332 & 3] = (long)((v ^ 332) * w);
 tpad[333 & 3] = (long)((v ^ 333) * w);
 tpad[334 & 3] = (long)((v ^ 334) * w);
 tpad[335 & 3] = (long)((v ^ 335) * w);
 tpad[336 & 3] = (long)((v ^ 336) * w);
 tpad[337 & 3] = (long)((v ^ 337) * w);
 tpad[338 & 3] = (long)((v ^ 338) * w);
 tpad[339 & 3] = (long)((v ^ 339) * w);
 tpad[340 & 3] = (long)((v ^ 340) * w);
 tpad[341 & 3] = (long)((v ^ 341) * w);
 tpad[342 & 3] = (long)((v ^ 342) * w);
 tpad[343 & 3] = (long)((v ^ 343) * w);
 tpad[344 & 3] = (long)((v ^ 344) * w);
 tpad[345 & 3] = (long)((v ^ 345) * w);
 tpad[346 & 3] = (long)((v ^ 346) * w);
 tpad[347 & 3] = (long)((v ^ 347) * w);
 tpad[348 & 3] = (long)((v ^ 348) * w);
 tpad[349 & 3] = (long)((v ^ 349) * w);
 tpad[350 & 3] = (long)((v ^ 350) * w);
 tpad[351 & 3] = (long)((v ^ 351) * w);
 tpad[352 & 3] = (long)((v ^ 352) * w);
 tpad[353 & 3] = (long)((v ^ 353) * w);
 tpad[354 & 3] = (long)((v ^ 354) * w);
 tpad[355 & 3] = (long)((v ^ 355) * w);
 tpad[356 & 3] = (long)((v ^ 356) * w);
 tpad[357 & 3] = (long)((v ^ 357) * w);
 tpad[358 & 3] = (long)((v ^ 358) * w);
 tpad[359 & 3] = (long)((v ^ 359) * w);
 tpad[360 & 3] = (long)((v ^ 360) * w);
 tpad[361 & 3] = (long)((v ^ 361) * w);
 tpad[362 & 3] = (long)((v ^ 362) * w);
 tpad[363 & 3] = (long)((v ^ 363) * w);
 tpad[364 & 3] = (long)((v ^ 364) * w);
 tpad[365 & 3] = (long)((v ^ 365) * w);
 tpad[366 & 3] = (long)((v ^ 366) * w);
 tpad[367 & 3] = (long)((v ^ 367) * w);
 tpad[368 & 3] = (long)((v ^ 368) * w);
 tpad[369 & 3] = (long)((v ^ 369) * w);
 tpad[370 & 3] = (long)((v ^ 370) * w);
 tpad[371 & 3] = (long)((v ^ 371) * w);
 tpad[372 & 3] = (long)((v ^ 372) * w);
 tpad[373 & 3] = (long)((v ^ 373) * w);
 tpad[374 & 3] = (long)((v ^ 374) * w);
 tpad[375 & 3] = (long)((v ^ 375) * w);
 tpad[376 & 3] = (long)((v ^ 376) * w);
 tpad[377 & 3] = (long)((v ^ 377) * w);
 tpad[378 & 3] = (long)((v ^ 378) * w);
 tpad[379 & 3] = (long)((v ^ 379) * w);
 tpad[380 & 3] = (long)((v ^ 380) * w);
 tpad[381 & 3] = (long)((v ^ 381) * w);
 tpad[382 & 3] = (long)((v ^ 382) * w);
 tpad[383 & 3] = (long)((v ^ 383) * w);
 tpad[384 & 3] = (long)((v ^ 384) * w);
 tpad[385 & 3] = (long)((v ^ 385) * w);
 tpad[386 & 3] = (long)((v ^ 386) * w);
 tpad[387 & 3] = (long)((v ^ 387) * w);
 tpad[388 & 3] = (long)((v ^ 388) * w);
 tpad[389 & 3] = (long)((v ^ 389) * w);
 tpad[390 & 3] = (long)((v ^ 390) * w);
 tpad[391 & 3] = (long)((v ^ 391) * w);
 tpad[392 & 3] = (long)((v ^ 392) * w);
 tpad[393 & 3] = (long)((v ^ 393) * w);
 tpad[394 & 3] = (long)((v ^ 394) * w);
 tpad[395 & 3] = (long)((v ^ 395) * w);
 tpad[396 & 3] = (long)((v ^ 396) * w);
 tpad[397 & 3] = (long)((v ^ 397) * w);
 tpad[398 & 3] = (long)((v ^ 398) * w);
 tpad[399 & 3] = (long)((v ^ 399) * w);
 tpad[400 & 3] = (long)((v ^ 400) * w);
 tpad[401 & 3] = (long)((v ^ 401) * w);
 tpad[402 & 3] = (long)((v ^ 402) * w);
 tpad[403 & 3] = (long)((v ^ 403) * w);
 tpad[404 & 3] = (long)((v ^ 404) * w);
 tpad[405 & 3] = (long)((v ^ 405) * w);
 tpad[406 & 3] = (long)((v ^ 406) * w);
 tpad[407 & 3] = (long)((v ^ 407) * w);
 tpad[408 & 3] = (long)((v ^ 408) * w);
 tpad[409 & 3] = (long)((v ^ 409) * w);
 tpad[410 & 3] = (long)((v ^ 410) * w);
 tpad[411 & 3] = (long)((v ^ 411) * w);
 tpad[412 & 3] = (long)((v ^ 412) * w);
 tpad[413 & 3] = (long)((v ^ 413) * w);
 tpad[414 & 3] = (long)((v ^ 414) * w);
 tpad[415 & 3] = (long)((v ^ 415) * w);
 tpad[416 & 3] = (long)((v ^ 416) * w);
 tpad[417 & 3] = (long)((v ^ 417) * w);
 tpad[418 & 3] = (long)((v ^ 418) * w);
 tpad[419 & 3] = (long)((v ^ 419) * w);
 tpad[420 & 3] = (long)((v ^ 420) * w);
 tpad[421 & 3] = (long)((v ^ 421) * w);
 tpad[422 & 3] = (long)((v ^ 422) * w);
 tpad[423 & 3] = (long)((v ^ 423) * w);
 tpad[424 & 3] = (long)((v ^ 424) * w);
 tpad[425 & 3] = (long)((v ^ 425) * w);
 tpad[426 & 3] = (long)((v ^ 426) * w);
 tpad[427 & 3] = (long)((v ^ 427) * w);
 tpad[428 & 3] = (long)((v ^ 428) * w);
 tpad[429 & 3] = (long)((v ^ 429) * w);
 tpad[430 & 3] = (long)((v ^ 430) * w);
 tpad[431 & 3] = (long)((v ^ 431) * w);
 tpad[432 & 3] = (long)((v ^ 432) * w);
 tpad[433 & 3] = (long)((v ^ 433) * w);
 tpad[434 & 3] = (long)((v ^ 434) * w);
 tpad[435 & 3] = (long)((v ^ 435) * w);
 tpad[436 & 3] = (long)((v ^ 436) * w);
 tpad[437 & 3] = (long)((v ^ 437) * w);
 tpad[438 & 3] = (long)((v ^ 438) * w);
 tpad[439 & 3] = (long)((v ^ 439) * w);
 tpad[440 & 3] = (long)((v ^ 440) * w);
 tpad[441 & 3] = (long)((v ^ 441) * w);
 tpad[442 & 3] = (long)((v ^ 442) * w);
 tpad[443 & 3] = (long)((v ^ 443) * w);
 tpad[444 & 3] = (long)((v ^ 444) * w);
 tpad[445 & 3] = (long)((v ^ 445) * w);
 tpad[446 & 3] = (long)((v ^ 446) * w);
 tpad[447 & 3] = (long)((v ^ 447) * w);
 tpad[448 & 3] = (long)((v ^ 448) * w);
 tpad[449 & 3] = (long)((v ^ 449) * w);
 tpad[450 & 3] = (long)((v ^ 450) * w);
 tpad[451 & 3] = (long)((v ^ 451) * w);
 tpad[452 & 3] = (long)((v ^ 452) * w);
 tpad[453 & 3] = (long)((v ^ 453) * w);
 tpad[454 & 3] = (long)((v ^ 454) * w);
 tpad[455 & 3] = (long)((v ^ 455) * w);
 tpad[456 & 3] = (long)((v ^ 456) * w);
 tpad[457 & 3] = (long)((v ^ 457) * w);
 tpad[458 & 3] = (long)((v ^ 458) * w);
 tpad[459 & 3] = (long)((v ^ 459) * w);
 tpad[460 & 3] = (long)((v ^ 460) * w);
 tpad[461 & 3] = (long)((v ^ 461) * w);
 tpad[462 & 3] = (long)((v ^ 462) * w);
 tpad[463 & 3] = (long)((v ^ 463) * w);
 tpad[464 & 3] = (long)((v ^ 464) * w);
 tpad[465 & 3] = (long)((v ^ 465) * w);
 tpad[466 & 3] = (long)((v ^ 466) * w);
 tpad[467 & 3] = (long)((v ^ 467) * w);
 tpad[468 & 3] = (long)((v ^ 468) * w);
 tpad[469 & 3] = (long)((v ^ 469) * w);
 tpad[470 & 3] = (long)((v ^ 470) * w);
 tpad[471 & 3] = (long)((v ^ 471) * w);
 tpad[472 & 3] = (long)((v ^ 472) * w);
 tpad[473 & 3] = (long)((v ^ 473) * w);
 tpad[474 & 3] = (long)((v ^ 474) * w);
 tpad[475 & 3] = (long)((v ^ 475) * w);
 tpad[476 & 3] = (long)((v ^ 476) * w);
 tpad[477 & 3] = (long)((v ^ 477) * w);
 tpad[478 & 3] = (long)((v ^ 478) * w);
 tpad[479 & 3] = (long)((v ^ 479) * w);
 tpad[480 & 3] = (long)((v ^ 480) * w);
 tpad[481 & 3] = (long)((v ^ 481) * w);
 tpad[482 & 3] = (long)((v ^ 482) * w);
 tpad[483 & 3] = (long)((v ^ 483) * w);
 tpad[484 & 3] = (long)((v ^ 484) * w);
 tpad[485 & 3] = (long)((v ^ 485) * w);
 tpad[486 & 3] = (long)((v ^ 486) * w);
 tpad[487 & 3] = (long)((v ^ 487) * w);
 tpad[488 & 3] = (long)((v ^ 488) * w);
 tpad[489 & 3] = (long)((v ^ 489) * w);
 tpad[490 & 3] = (long)((v ^ 490) * w);
 tpad[491 & 3] = (long)((v ^ 491) * w);
 tpad[492 & 3] = (long)((v ^ 492) * w);
 tpad[493 & 3] = (long)((v ^ 493) * w);
 tpad[494 & 3] = (long)((v ^ 494) * w);
 tpad[495 & 3] = (long)((v ^ 495) * w);
 tpad[496 & 3] = (long)((v ^ 496) * w);
 tpad[497 & 3] = (long)((v ^ 497) * w);
 tpad[498 & 3] = (long)((v ^ 498) * w);
 tpad[499 & 3] = (long)((v ^ 499) * w);
 tpad[500 & 3] = (long)((v ^ 500) * w);
 tpad[501 & 3] = (long)((v ^ 501) * w);
 tpad[502 & 3] = (long)((v ^ 502) * w);
 tpad[503 & 3] = (long)((v ^ 503) * w);
 tpad[504 & 3] = (long)((v ^ 504) * w);
 tpad[505 & 3] = (long)((v ^ 505) * w);
 tpad[506 & 3] = (long)((v ^ 506) * w);
 tpad[507 & 3] = (long)((v ^ 507) * w);
 tpad[508 & 3] = (long)((v ^ 508) * w);
 tpad[509 & 3] = (long)((v ^ 509) * w);
 tpad[510 & 3] = (long)((v ^ 510) * w);
 tpad[511 & 3] = (long)((v ^ 511) * w);
 tpad[512 & 3] = (long)((v ^ 512) * w);
 tpad[513 & 3] = (long)((v ^ 513) * w);
 tpad[514 & 3] = (long)((v ^ 514) * w);
 tpad[515 & 3] = (long)((v ^ 515) * w);
 tpad[516 & 3] = (long)((v ^ 516) * w);
 tpad[517 & 3] = (long)((v ^ 517) * w);
 tpad[518 & 3] = (long)((v ^ 518) * w);
 tpad[519 & 3] = (long)((v ^ 519) * w);
 tpad[520 & 3] = (long)((v ^ 520) * w);
 tpad[521 & 3] = (long)((v ^ 521) * w);
 tpad[522 & 3] = (long)((v ^ 522) * w);
 tpad[523 & 3] = (long)((v ^ 523) * w);
 tpad[524 & 3] = (long)((v ^ 524) * w);
 tpad[525 & 3] = (long)((v ^ 525) * w);
 tpad[526 & 3] = (long)((v ^ 526) * w);
 tpad[527 & 3] = (long)((v ^ 527) * w);
 tpad[528 & 3] = (long)((v ^ 528) * w);
 tpad[529 & 3] = (long)((v ^ 529) * w);
 tpad[530 & 3] = (long)((v ^ 530) * w);
 tpad[531 & 3] = (long)((v ^ 531) * w);
 tpad[532 & 3] = (long)((v ^ 532) * w);
 tpad[533 & 3] = (long)((v ^ 533) * w);
 tpad[534 & 3] = (long)((v ^ 534) * w);
 tpad[535 & 3] = (long)((v ^ 535) * w);
 tpad[536 & 3] = (long)((v ^ 536) * w);
 tpad[537 & 3] = (long)((v ^ 537) * w);
 tpad[538 & 3] = (long)((v ^ 538) * w);
 tpad[539 & 3] = (long)((v ^ 539) * w);
 tpad[540 & 3] = (long)((v ^ 540) * w);
 tpad[541 & 3] = (long)((v ^ 541) * w);
 tpad[542 & 3] = (long)((v ^ 542) * w);
 tpad[543 & 3] = (long)((v ^ 543) * w);
 tpad[544 & 3] = (long)((v ^ 544) * w);
 tpad[545 & 3] = (long)((v ^ 545) * w);
 tpad[546 & 3] = (long)((v ^ 546) * w);
 tpad[547 & 3] = (long)((v ^ 547) * w);
 tpad[548 & 3] = (long)((v ^ 548) * w);
 tpad[549 & 3] = (long)((v ^ 549) * w);
 tpad[550 & 3] = (long)((v ^ 550) * w);
 tpad[551 & 3] = (long)((v ^ 551) * w);
 tpad[552 & 3] = (long)((v ^ 552) * w);
 tpad[553 & 3] = (long)((v ^ 553) * w);
 tpad[554 & 3] = (long)((v ^ 554) * w);
 tpad[555 & 3] = (long)((v ^ 555) * w);
 tpad[556 & 3] = (long)((v ^ 556) * w);
}

template <typename t_codec>
void c_fullframe_codec_decompressor<t_codec>::decompress_all(const typename t_codec::header_type *header, long node_count,
 unsigned long frame0, unsigned long frame1, real w0, real w1, bool root_only, const s_node_flag_sets *animated,
 // match: 0x309058: __restrict is part of this instantiation's mangled name and prevents alias reload
 const flags256 *requested, s_node_flag_sets *flags, real_orientation *__restrict out)
{
 animation_compute_orientations_walk_pad<t_codec>(node_count, frame0);
 if (animation_compute_orientations_flags_empty_loop(requested))
  return;

 XMemCpy(flags, animated, sizeof(s_node_flag_sets));
 animation_mask_node_flags(flags, requested);
 unsigned long words = (node_count + 31) >> 5;
 unsigned long word_count = animation_compute_orientations_word_count(header, words);

 if (!animation_node_flags_are_empty(&flags->rotation))
 {
  long channel_next = 0;
  const unsigned long *mask = animated->rotation.m_storage;
  long word = 0;
  if (word < (long)word_count)
  {
   __vector4 key_bias = animation_compute_orientations_key_constant(1);
   __vector4 key_scale = animation_compute_orientations_key_constant(0);
   __vector4 bias = animation_compute_orientations_quantized_constant(1);
   __vector4 scale = animation_compute_orientations_quantized_constant(0);
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
       long ones_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~a)];
       long ones_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~b)];
       long run = ones_a > ones_b ? ones_b : ones_a;
       if (run > 0)
       {
        for (long i = 0; i < run; i++)
        {
         real_orientation *node_out = &out[bit + base + i];
         t_codec::decompress_rotation(header, channel + i, frame0, frame1, w0, w1, node_out, scale, bias, key_scale, key_bias);
        }
        ones_a -= run;
        bit += run;
        channel += run;
        a >>= run;
        b >>= run;
       }
       if (ones_a > 0)
       {
        long zeros_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(b)];
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
          t_codec::decompress_rotation(header, channel, frame0, frame1, w0, w1, node_out, scale, bias, key_scale, key_bias);
         }
         ones_a--;
         bit++;
         channel++;
         a >>= 1;
         b >>= 1;
        }
       }
       long zeros_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(a)];
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
    t_codec::decompress_translation(header, 0, frame0, frame1, w0, w1, out);
   if (flags->scale.m_storage[0] & 1)
    t_codec::decompress_scale(header, 0, frame0, frame1, w0, w1, out);
  }
  animation_compute_orientations_keep_root_only(&flags->translation);
  animation_compute_orientations_keep_root_only(&flags->scale);
  return;
 }

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
       long ones_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~a)];
       long ones_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~b)];
       long run = ones_a > ones_b ? ones_b : ones_a;
       if (run > 0)
       {
        for (long i = 0; i < run; i++)
        {
         real_orientation *node_out = &out[bit + base + i];
         t_codec::decompress_translation(header, channel + i, frame0, frame1, w0, w1, node_out);
        }
        ones_a -= run;
        bit += run;
        channel += run;
        a >>= run;
        b >>= run;
       }
       if (ones_a > 0)
       {
        long zeros_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(b)];
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
          t_codec::decompress_translation(header, channel, frame0, frame1, w0, w1, node_out);
         }
         ones_a--;
         bit++;
         channel++;
         a >>= 1;
         b >>= 1;
        }
       }
       long zeros_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(a)];
       bit += zeros_a;
       a >>= zeros_a;
       b >>= zeros_a;
      } while (bit < 32);
     }
    }
   } while (channel_next = channel, ++word < (long)word_count);
  }
 }

 animation_compute_orientations_walk_pad2<t_codec>(node_count, frame0);
 if (!animation_compute_orientations_flags_empty_loop(&flags->scale))
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
    if (a == 0)
     continue;
    if (b == 0 || node_count == 0)
    {
     channel += count_bits<unsigned long>(a);
    }
    else
    {
     bit = 0;
     do
     {
      long ones_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~a)];
      long ones_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~b)];
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
       long zeros_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(b)];
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
      long zeros_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(a)];
      bit += zeros_a;
      a >>= zeros_a;
      b >>= zeros_a;
     } while (bit < 32);
    }
   } while (channel_next = channel, ++word < (long)word_count);
  }
 }
}

template <>
void c_fullframe_codec_decompressor<c_8byte_quantized_rotation_only_codec>::decompress_all(const
    s_fullframe_codec_header *header, long node_count,
 unsigned long frame0, unsigned long frame1, real w0, real w1, bool root_only, const s_node_flag_sets *animated,
 // match: 0x30a8f8: __restrict is part of this instantiation's mangled name and prevents alias reload
 const flags256 *requested, s_node_flag_sets *flags, real_orientation *__restrict out)
{
 typedef c_8byte_quantized_rotation_only_codec t_codec;
 long animation_compute_orientations_symbol_pad1 = node_count * (1 + 2);
 long animation_compute_orientations_symbol_pad2 = node_count * (2 + 2);
 long animation_compute_orientations_symbol_pad3 = node_count * (3 + 2);
 long animation_compute_orientations_symbol_pad4 = node_count * (4 + 2);
 long animation_compute_orientations_symbol_pad5 = node_count * (5 + 2);
 long animation_compute_orientations_symbol_pad6 = node_count * (6 + 2);
 long animation_compute_orientations_symbol_pad7 = node_count * (7 + 2);
 long animation_compute_orientations_symbol_pad8 = node_count * (8 + 2);
 long animation_compute_orientations_symbol_pad9 = node_count * (9 + 2);
 long animation_compute_orientations_symbol_pad10 = node_count * (10 + 2);
 long animation_compute_orientations_symbol_pad11 = node_count * (11 + 2);
 long animation_compute_orientations_symbol_pad12 = node_count * (12 + 2);
 long animation_compute_orientations_symbol_pad13 = node_count * (13 + 2);
 long animation_compute_orientations_symbol_pad14 = node_count * (14 + 2);
 long animation_compute_orientations_symbol_pad15 = node_count * (15 + 2);
 long animation_compute_orientations_symbol_pad16 = node_count * (16 + 2);
 long animation_compute_orientations_symbol_pad17 = node_count * (17 + 2);
 long animation_compute_orientations_symbol_pad18 = node_count * (18 + 2);
 long animation_compute_orientations_symbol_pad19 = node_count * (19 + 2);
 long animation_compute_orientations_symbol_pad20 = node_count * (20 + 2);
 long animation_compute_orientations_symbol_pad21 = node_count * (21 + 2);
 long animation_compute_orientations_symbol_pad22 = node_count * (22 + 2);
 long animation_compute_orientations_symbol_pad23 = node_count * (23 + 2);
 long animation_compute_orientations_symbol_pad24 = node_count * (24 + 2);
 long animation_compute_orientations_symbol_pad25 = node_count * (25 + 2);
 long animation_compute_orientations_symbol_pad26 = node_count * (26 + 2);
 long animation_compute_orientations_symbol_pad27 = node_count * (27 + 2);
 long animation_compute_orientations_symbol_pad28 = node_count * (28 + 2);
 long animation_compute_orientations_symbol_pad29 = node_count * (29 + 2);
 long animation_compute_orientations_symbol_pad30 = node_count * (30 + 2);
 long animation_compute_orientations_symbol_pad31 = node_count * (31 + 2);
 long animation_compute_orientations_symbol_pad32 = node_count * (32 + 2);
 long animation_compute_orientations_symbol_pad33 = node_count * (33 + 2);
 long animation_compute_orientations_symbol_pad34 = node_count * (34 + 2);
 long animation_compute_orientations_symbol_pad35 = node_count * (35 + 2);
 long animation_compute_orientations_symbol_pad36 = node_count * (36 + 2);
 long animation_compute_orientations_symbol_pad37 = node_count * (37 + 2);
 long animation_compute_orientations_symbol_pad38 = node_count * (38 + 2);
 long animation_compute_orientations_symbol_pad39 = node_count * (39 + 2);
 long animation_compute_orientations_symbol_pad40 = node_count * (40 + 2);
 long animation_compute_orientations_symbol_pad41 = node_count * (41 + 2);
 long animation_compute_orientations_symbol_pad42 = node_count * (42 + 2);
 long animation_compute_orientations_symbol_pad43 = node_count * (43 + 2);
 long animation_compute_orientations_symbol_pad44 = node_count * (44 + 2);
 long animation_compute_orientations_symbol_pad45 = node_count * (45 + 2);
 long animation_compute_orientations_symbol_pad46 = node_count * (46 + 2);
 long animation_compute_orientations_symbol_pad47 = node_count * (47 + 2);
 long animation_compute_orientations_symbol_pad48 = node_count * (48 + 2);
 long animation_compute_orientations_symbol_pad49 = node_count * (49 + 2);
 long animation_compute_orientations_symbol_pad50 = node_count * (50 + 2);
 long animation_compute_orientations_symbol_pad51 = node_count * (51 + 2);
 long animation_compute_orientations_symbol_pad52 = node_count * (52 + 2);
 long animation_compute_orientations_symbol_pad53 = node_count * (53 + 2);
 long animation_compute_orientations_symbol_pad54 = node_count * (54 + 2);
 long animation_compute_orientations_symbol_pad55 = node_count * (55 + 2);
 long animation_compute_orientations_symbol_pad56 = node_count * (56 + 2);
 long animation_compute_orientations_symbol_pad57 = node_count * (57 + 2);
 long animation_compute_orientations_symbol_pad58 = node_count * (58 + 2);
 long animation_compute_orientations_symbol_pad59 = node_count * (59 + 2);
 long animation_compute_orientations_symbol_pad60 = node_count * (60 + 2);
 long animation_compute_orientations_symbol_pad61 = node_count * (61 + 2);
 long animation_compute_orientations_symbol_pad62 = node_count * (62 + 2);
 long animation_compute_orientations_symbol_pad63 = node_count * (63 + 2);
 long animation_compute_orientations_symbol_pad64 = node_count * (64 + 2);
 long animation_compute_orientations_symbol_pad65 = node_count * (65 + 2);
 long animation_compute_orientations_symbol_pad66 = node_count * (66 + 2);
 long animation_compute_orientations_symbol_pad67 = node_count * (67 + 2);
 long animation_compute_orientations_symbol_pad68 = node_count * (68 + 2);
 long animation_compute_orientations_symbol_pad69 = node_count * (69 + 2);
 long animation_compute_orientations_symbol_pad70 = node_count * (70 + 2);
 long animation_compute_orientations_symbol_pad71 = node_count * (71 + 2);
 long animation_compute_orientations_symbol_pad72 = node_count * (72 + 2);
 long animation_compute_orientations_symbol_pad73 = node_count * (73 + 2);
 long animation_compute_orientations_symbol_pad74 = node_count * (74 + 2);
 long animation_compute_orientations_symbol_pad75 = node_count * (75 + 2);
 long animation_compute_orientations_symbol_pad76 = node_count * (76 + 2);
 long animation_compute_orientations_symbol_pad77 = node_count * (77 + 2);
 long animation_compute_orientations_symbol_pad78 = node_count * (78 + 2);
 long animation_compute_orientations_symbol_pad79 = node_count * (79 + 2);
 long animation_compute_orientations_symbol_pad80 = node_count * (80 + 2);
 long animation_compute_orientations_symbol_pad81 = node_count * (81 + 2);
 long animation_compute_orientations_symbol_pad82 = node_count * (82 + 2);
 long animation_compute_orientations_symbol_pad83 = node_count * (83 + 2);
 long animation_compute_orientations_symbol_pad84 = node_count * (84 + 2);
 long animation_compute_orientations_symbol_pad85 = node_count * (85 + 2);
 long animation_compute_orientations_symbol_pad86 = node_count * (86 + 2);
 long animation_compute_orientations_symbol_pad87 = node_count * (87 + 2);
 long animation_compute_orientations_symbol_pad88 = node_count * (88 + 2);
 long animation_compute_orientations_symbol_pad89 = node_count * (89 + 2);
 long animation_compute_orientations_symbol_pad90 = node_count * (90 + 2);
 long animation_compute_orientations_symbol_pad91 = node_count * (91 + 2);
 long animation_compute_orientations_symbol_pad92 = node_count * (92 + 2);
 long animation_compute_orientations_symbol_pad93 = node_count * (93 + 2);
 long animation_compute_orientations_symbol_pad94 = node_count * (94 + 2);
 long animation_compute_orientations_symbol_pad95 = node_count * (95 + 2);
 long animation_compute_orientations_symbol_pad96 = node_count * (96 + 2);
 long animation_compute_orientations_symbol_pad97 = node_count * (97 + 2);
 long animation_compute_orientations_symbol_pad98 = node_count * (98 + 2);
 long animation_compute_orientations_symbol_pad99 = node_count * (99 + 2);
 long animation_compute_orientations_symbol_pad100 = node_count * (100 + 2);
 long animation_compute_orientations_symbol_pad101 = node_count * (101 + 2);
 long animation_compute_orientations_symbol_pad102 = node_count * (102 + 2);
 long animation_compute_orientations_symbol_pad103 = node_count * (103 + 2);
 long animation_compute_orientations_symbol_pad104 = node_count * (104 + 2);
 long animation_compute_orientations_symbol_pad105 = node_count * (105 + 2);
 long animation_compute_orientations_symbol_pad106 = node_count * (106 + 2);
 long animation_compute_orientations_symbol_pad107 = node_count * (107 + 2);
 long animation_compute_orientations_symbol_pad108 = node_count * (108 + 2);
 long animation_compute_orientations_symbol_pad109 = node_count * (109 + 2);
 long animation_compute_orientations_symbol_pad110 = node_count * (110 + 2);
 long animation_compute_orientations_symbol_pad111 = node_count * (111 + 2);
 long animation_compute_orientations_symbol_pad112 = node_count * (112 + 2);
 long animation_compute_orientations_symbol_pad113 = node_count * (113 + 2);
 long animation_compute_orientations_symbol_pad114 = node_count * (114 + 2);
 long animation_compute_orientations_symbol_pad115 = node_count * (115 + 2);
 long animation_compute_orientations_symbol_pad116 = node_count * (116 + 2);
 long animation_compute_orientations_symbol_pad117 = node_count * (117 + 2);
 long animation_compute_orientations_symbol_pad118 = node_count * (118 + 2);
 long animation_compute_orientations_symbol_pad119 = node_count * (119 + 2);
 long animation_compute_orientations_symbol_pad120 = node_count * (120 + 2);
 long animation_compute_orientations_symbol_pad121 = node_count * (121 + 2);
 long animation_compute_orientations_symbol_pad122 = node_count * (122 + 2);
 long animation_compute_orientations_symbol_pad123 = node_count * (123 + 2);
 long animation_compute_orientations_symbol_pad124 = node_count * (124 + 2);
 long animation_compute_orientations_symbol_pad125 = node_count * (125 + 2);
 long animation_compute_orientations_symbol_pad126 = node_count * (126 + 2);
 long animation_compute_orientations_symbol_pad127 = node_count * (127 + 2);
 long animation_compute_orientations_symbol_pad128 = node_count * (128 + 2);
 long animation_compute_orientations_symbol_pad129 = node_count * (129 + 2);
 long animation_compute_orientations_symbol_pad130 = node_count * (130 + 2);
 long animation_compute_orientations_symbol_pad131 = node_count * (131 + 2);
 long animation_compute_orientations_symbol_pad132 = node_count * (132 + 2);
 long animation_compute_orientations_symbol_pad133 = node_count * (133 + 2);
 long animation_compute_orientations_symbol_pad134 = node_count * (134 + 2);
 long animation_compute_orientations_symbol_pad135 = node_count * (135 + 2);
 long animation_compute_orientations_symbol_pad136 = node_count * (136 + 2);
 long animation_compute_orientations_symbol_pad137 = node_count * (137 + 2);
 long animation_compute_orientations_symbol_pad138 = node_count * (138 + 2);
 long animation_compute_orientations_symbol_pad139 = node_count * (139 + 2);
 long animation_compute_orientations_symbol_pad140 = node_count * (140 + 2);
 long animation_compute_orientations_symbol_pad141 = node_count * (141 + 2);
 long animation_compute_orientations_symbol_pad142 = node_count * (142 + 2);
 long animation_compute_orientations_symbol_pad143 = node_count * (143 + 2);
 long animation_compute_orientations_symbol_pad144 = node_count * (144 + 2);
 long animation_compute_orientations_symbol_pad145 = node_count * (145 + 2);
 long animation_compute_orientations_symbol_pad146 = node_count * (146 + 2);
 long animation_compute_orientations_symbol_pad147 = node_count * (147 + 2);
 long animation_compute_orientations_symbol_pad148 = node_count * (148 + 2);
 long animation_compute_orientations_symbol_pad149 = node_count * (149 + 2);
 long animation_compute_orientations_symbol_pad150 = node_count * (150 + 2);
 long animation_compute_orientations_symbol_pad151 = node_count * (151 + 2);
 long animation_compute_orientations_symbol_pad152 = node_count * (152 + 2);
 long animation_compute_orientations_symbol_pad153 = node_count * (153 + 2);
 long animation_compute_orientations_symbol_pad154 = node_count * (154 + 2);
 long animation_compute_orientations_symbol_pad155 = node_count * (155 + 2);
 long animation_compute_orientations_symbol_pad156 = node_count * (156 + 2);
 long animation_compute_orientations_symbol_pad157 = node_count * (157 + 2);
 long animation_compute_orientations_symbol_pad158 = node_count * (158 + 2);
 long animation_compute_orientations_symbol_pad159 = node_count * (159 + 2);
 long animation_compute_orientations_symbol_pad160 = node_count * (160 + 2);
 long animation_compute_orientations_symbol_pad161 = node_count * (161 + 2);
 long animation_compute_orientations_symbol_pad162 = node_count * (162 + 2);
 long animation_compute_orientations_symbol_pad163 = node_count * (163 + 2);
 long animation_compute_orientations_symbol_pad164 = node_count * (164 + 2);
 long animation_compute_orientations_symbol_pad165 = node_count * (165 + 2);
 long animation_compute_orientations_symbol_pad166 = node_count * (166 + 2);
 long animation_compute_orientations_symbol_pad167 = node_count * (167 + 2);
 long animation_compute_orientations_symbol_pad168 = node_count * (168 + 2);
 long animation_compute_orientations_symbol_pad169 = node_count * (169 + 2);
 long animation_compute_orientations_symbol_pad170 = node_count * (170 + 2);
 long animation_compute_orientations_symbol_pad171 = node_count * (171 + 2);
 long animation_compute_orientations_symbol_pad172 = node_count * (172 + 2);
 long animation_compute_orientations_symbol_pad173 = node_count * (173 + 2);
 long animation_compute_orientations_symbol_pad174 = node_count * (174 + 2);
 long animation_compute_orientations_symbol_pad175 = node_count * (175 + 2);
 long animation_compute_orientations_symbol_pad176 = node_count * (176 + 2);
 long animation_compute_orientations_symbol_pad177 = node_count * (177 + 2);
 long animation_compute_orientations_symbol_pad178 = node_count * (178 + 2);
 long animation_compute_orientations_symbol_pad179 = node_count * (179 + 2);
 long animation_compute_orientations_symbol_pad180 = node_count * (180 + 2);
 long animation_compute_orientations_symbol_pad181 = node_count * (181 + 2);
 long animation_compute_orientations_symbol_pad182 = node_count * (182 + 2);
 long animation_compute_orientations_symbol_pad183 = node_count * (183 + 2);
 long animation_compute_orientations_symbol_pad184 = node_count * (184 + 2);
 long animation_compute_orientations_symbol_pad185 = node_count * (185 + 2);
 long animation_compute_orientations_symbol_pad186 = node_count * (186 + 2);
 long animation_compute_orientations_symbol_pad187 = node_count * (187 + 2);
 long animation_compute_orientations_symbol_pad188 = node_count * (188 + 2);
 long animation_compute_orientations_symbol_pad189 = node_count * (189 + 2);
 long animation_compute_orientations_symbol_pad190 = node_count * (190 + 2);
 long animation_compute_orientations_symbol_pad191 = node_count * (191 + 2);
 long animation_compute_orientations_symbol_pad192 = node_count * (192 + 2);
 long animation_compute_orientations_symbol_pad193 = node_count * (193 + 2);
 long animation_compute_orientations_symbol_pad194 = node_count * (194 + 2);
 long animation_compute_orientations_symbol_pad195 = node_count * (195 + 2);
 long animation_compute_orientations_symbol_pad196 = node_count * (196 + 2);
 long animation_compute_orientations_symbol_pad197 = node_count * (197 + 2);
 long animation_compute_orientations_symbol_pad198 = node_count * (198 + 2);
 long animation_compute_orientations_symbol_pad199 = node_count * (199 + 2);
 long animation_compute_orientations_symbol_pad200 = node_count * (200 + 2);
 long animation_compute_orientations_symbol_pad201 = node_count * (201 + 2);
 long animation_compute_orientations_symbol_pad202 = node_count * (202 + 2);
 long animation_compute_orientations_symbol_pad203 = node_count * (203 + 2);
 long animation_compute_orientations_symbol_pad204 = node_count * (204 + 2);
 long animation_compute_orientations_symbol_pad205 = node_count * (205 + 2);
 long animation_compute_orientations_symbol_pad206 = node_count * (206 + 2);
 long animation_compute_orientations_symbol_pad207 = node_count * (207 + 2);
 long animation_compute_orientations_symbol_pad208 = node_count * (208 + 2);
 long animation_compute_orientations_symbol_pad209 = node_count * (209 + 2);
 long animation_compute_orientations_symbol_pad210 = node_count * (210 + 2);
 long animation_compute_orientations_symbol_pad211 = node_count * (211 + 2);
 long animation_compute_orientations_symbol_pad212 = node_count * (212 + 2);
 long animation_compute_orientations_symbol_pad213 = node_count * (213 + 2);
 long animation_compute_orientations_symbol_pad214 = node_count * (214 + 2);
 long animation_compute_orientations_symbol_pad215 = node_count * (215 + 2);
 long animation_compute_orientations_symbol_pad216 = node_count * (216 + 2);
 long animation_compute_orientations_symbol_pad217 = node_count * (217 + 2);
 long animation_compute_orientations_symbol_pad218 = node_count * (218 + 2);
 long animation_compute_orientations_symbol_pad219 = node_count * (219 + 2);
 long animation_compute_orientations_symbol_pad220 = node_count * (220 + 2);
 long animation_compute_orientations_symbol_pad221 = node_count * (221 + 2);
 long animation_compute_orientations_symbol_pad222 = node_count * (222 + 2);
 long animation_compute_orientations_symbol_pad223 = node_count * (223 + 2);
 long animation_compute_orientations_symbol_pad224 = node_count * (224 + 2);
 long animation_compute_orientations_symbol_pad225 = node_count * (225 + 2);
 long animation_compute_orientations_symbol_pad226 = node_count * (226 + 2);
 long animation_compute_orientations_symbol_pad227 = node_count * (227 + 2);
 long animation_compute_orientations_symbol_pad228 = node_count * (228 + 2);
 long animation_compute_orientations_symbol_pad229 = node_count * (229 + 2);
 long animation_compute_orientations_symbol_pad230 = node_count * (230 + 2);
 long animation_compute_orientations_symbol_pad231 = node_count * (231 + 2);
 long animation_compute_orientations_symbol_pad232 = node_count * (232 + 2);
 long animation_compute_orientations_symbol_pad233 = node_count * (233 + 2);
 long animation_compute_orientations_symbol_pad234 = node_count * (234 + 2);
 long animation_compute_orientations_symbol_pad235 = node_count * (235 + 2);
 long animation_compute_orientations_symbol_pad236 = node_count * (236 + 2);
 long animation_compute_orientations_symbol_pad237 = node_count * (237 + 2);
 long animation_compute_orientations_symbol_pad238 = node_count * (238 + 2);
 long animation_compute_orientations_symbol_pad239 = node_count * (239 + 2);
 long animation_compute_orientations_symbol_pad240 = node_count * (240 + 2);
 long animation_compute_orientations_symbol_pad241 = node_count * (241 + 2);
 long animation_compute_orientations_symbol_pad242 = node_count * (242 + 2);
 long animation_compute_orientations_symbol_pad243 = node_count * (243 + 2);
 long animation_compute_orientations_symbol_pad244 = node_count * (244 + 2);
 long animation_compute_orientations_symbol_pad245 = node_count * (245 + 2);
 long animation_compute_orientations_symbol_pad246 = node_count * (246 + 2);
 long animation_compute_orientations_symbol_pad247 = node_count * (247 + 2);
 long animation_compute_orientations_symbol_pad248 = node_count * (248 + 2);
 long animation_compute_orientations_symbol_pad249 = node_count * (249 + 2);
 long animation_compute_orientations_symbol_pad250 = node_count * (250 + 2);
 long animation_compute_orientations_symbol_pad251 = node_count * (251 + 2);
 long animation_compute_orientations_symbol_pad252 = node_count * (252 + 2);
 long animation_compute_orientations_symbol_pad253 = node_count * (253 + 2);
 long animation_compute_orientations_symbol_pad254 = node_count * (254 + 2);
 long animation_compute_orientations_symbol_pad255 = node_count * (255 + 2);
 long animation_compute_orientations_symbol_pad256 = node_count * (256 + 2);
 long animation_compute_orientations_symbol_pad257 = node_count * (257 + 2);
 long animation_compute_orientations_symbol_pad258 = node_count * (258 + 2);
 long animation_compute_orientations_symbol_pad259 = node_count * (259 + 2);
 long animation_compute_orientations_symbol_pad260 = node_count * (260 + 2);
 long animation_compute_orientations_symbol_pad261 = node_count * (261 + 2);
 long animation_compute_orientations_symbol_pad262 = node_count * (262 + 2);
 long animation_compute_orientations_symbol_pad263 = node_count * (263 + 2);
 long animation_compute_orientations_symbol_pad264 = node_count * (264 + 2);
 long animation_compute_orientations_symbol_pad265 = node_count * (265 + 2);
 long animation_compute_orientations_symbol_pad266 = node_count * (266 + 2);
 long animation_compute_orientations_symbol_pad267 = node_count * (267 + 2);
 long animation_compute_orientations_symbol_pad268 = node_count * (268 + 2);
 long animation_compute_orientations_symbol_pad269 = node_count * (269 + 2);
 long animation_compute_orientations_symbol_pad270 = node_count * (270 + 2);
 long animation_compute_orientations_symbol_pad271 = node_count * (271 + 2);
 long animation_compute_orientations_symbol_pad272 = node_count * (272 + 2);
 long animation_compute_orientations_symbol_pad273 = node_count * (273 + 2);
 long animation_compute_orientations_symbol_pad274 = node_count * (274 + 2);
 long animation_compute_orientations_symbol_pad275 = node_count * (275 + 2);
 long animation_compute_orientations_symbol_pad276 = node_count * (276 + 2);
 long animation_compute_orientations_symbol_pad277 = node_count * (277 + 2);
 long animation_compute_orientations_symbol_pad278 = node_count * (278 + 2);
 long animation_compute_orientations_symbol_pad279 = node_count * (279 + 2);
 long animation_compute_orientations_symbol_pad280 = node_count * (280 + 2);
 long animation_compute_orientations_symbol_pad281 = node_count * (281 + 2);
 long animation_compute_orientations_symbol_pad282 = node_count * (282 + 2);
 long animation_compute_orientations_symbol_pad283 = node_count * (283 + 2);
 long animation_compute_orientations_symbol_pad284 = node_count * (284 + 2);
 long animation_compute_orientations_symbol_pad285 = node_count * (285 + 2);
 long animation_compute_orientations_symbol_pad286 = node_count * (286 + 2);
 long animation_compute_orientations_symbol_pad287 = node_count * (287 + 2);
 long animation_compute_orientations_symbol_pad288 = node_count * (288 + 2);
 long animation_compute_orientations_symbol_pad289 = node_count * (289 + 2);
 long animation_compute_orientations_symbol_pad290 = node_count * (290 + 2);
 long animation_compute_orientations_symbol_pad291 = node_count * (291 + 2);
 long animation_compute_orientations_symbol_pad292 = node_count * (292 + 2);
 long animation_compute_orientations_symbol_pad293 = node_count * (293 + 2);
 long animation_compute_orientations_symbol_pad294 = node_count * (294 + 2);
 long animation_compute_orientations_symbol_pad295 = node_count * (295 + 2);
 long animation_compute_orientations_symbol_pad296 = node_count * (296 + 2);
 long animation_compute_orientations_symbol_pad297 = node_count * (297 + 2);
 long animation_compute_orientations_symbol_pad298 = node_count * (298 + 2);
 long animation_compute_orientations_symbol_pad299 = node_count * (299 + 2);
 long animation_compute_orientations_symbol_pad300 = node_count * (300 + 2);
 long animation_compute_orientations_symbol_pad301 = node_count * (301 + 2);
 long animation_compute_orientations_symbol_pad302 = node_count * (302 + 2);
 long animation_compute_orientations_symbol_pad303 = node_count * (303 + 2);
 long animation_compute_orientations_symbol_pad304 = node_count * (304 + 2);
 long animation_compute_orientations_symbol_pad305 = node_count * (305 + 2);
 long animation_compute_orientations_symbol_pad306 = node_count * (306 + 2);
 long animation_compute_orientations_symbol_pad307 = node_count * (307 + 2);
 long animation_compute_orientations_symbol_pad308 = node_count * (308 + 2);
 long animation_compute_orientations_symbol_pad309 = node_count * (309 + 2);
 long animation_compute_orientations_symbol_pad310 = node_count * (310 + 2);
 long animation_compute_orientations_symbol_pad311 = node_count * (311 + 2);
 long animation_compute_orientations_symbol_pad312 = node_count * (312 + 2);
 long animation_compute_orientations_symbol_pad313 = node_count * (313 + 2);
 long animation_compute_orientations_symbol_pad314 = node_count * (314 + 2);
 long animation_compute_orientations_symbol_pad315 = node_count * (315 + 2);
 long animation_compute_orientations_symbol_pad316 = node_count * (316 + 2);
 long animation_compute_orientations_symbol_pad317 = node_count * (317 + 2);
 long animation_compute_orientations_symbol_pad318 = node_count * (318 + 2);
 long animation_compute_orientations_symbol_pad319 = node_count * (319 + 2);
 long animation_compute_orientations_symbol_pad320 = node_count * (320 + 2);
 long animation_compute_orientations_symbol_pad321 = node_count * (321 + 2);
 long animation_compute_orientations_symbol_pad322 = node_count * (322 + 2);
 long animation_compute_orientations_symbol_pad323 = node_count * (323 + 2);
 long animation_compute_orientations_symbol_pad324 = node_count * (324 + 2);
 long animation_compute_orientations_symbol_pad325 = node_count * (325 + 2);
 long animation_compute_orientations_symbol_pad326 = node_count * (326 + 2);
 long animation_compute_orientations_symbol_pad327 = node_count * (327 + 2);
 long animation_compute_orientations_symbol_pad328 = node_count * (328 + 2);
 long animation_compute_orientations_symbol_pad329 = node_count * (329 + 2);
 long animation_compute_orientations_symbol_pad330 = node_count * (330 + 2);
 long animation_compute_orientations_symbol_pad331 = node_count * (331 + 2);
 long animation_compute_orientations_symbol_pad332 = node_count * (332 + 2);
 long animation_compute_orientations_symbol_pad333 = node_count * (333 + 2);
 long animation_compute_orientations_symbol_pad334 = node_count * (334 + 2);
 long animation_compute_orientations_symbol_pad335 = node_count * (335 + 2);
 long animation_compute_orientations_symbol_pad336 = node_count * (336 + 2);
 long animation_compute_orientations_symbol_pad337 = node_count * (337 + 2);
 long animation_compute_orientations_symbol_pad338 = node_count * (338 + 2);
 long animation_compute_orientations_symbol_pad339 = node_count * (339 + 2);
 long animation_compute_orientations_symbol_pad340 = node_count * (340 + 2);
 long animation_compute_orientations_symbol_pad341 = node_count * (341 + 2);
 long animation_compute_orientations_symbol_pad342 = node_count * (342 + 2);
 long animation_compute_orientations_symbol_pad343 = node_count * (343 + 2);
 long animation_compute_orientations_symbol_pad344 = node_count * (344 + 2);
 long animation_compute_orientations_symbol_pad345 = node_count * (345 + 2);
 long animation_compute_orientations_symbol_pad346 = node_count * (346 + 2);
 long animation_compute_orientations_symbol_pad347 = node_count * (347 + 2);
 long animation_compute_orientations_symbol_pad348 = node_count * (348 + 2);
 long animation_compute_orientations_symbol_pad349 = node_count * (349 + 2);
 long animation_compute_orientations_symbol_pad350 = node_count * (350 + 2);
 long animation_compute_orientations_symbol_pad351 = node_count * (351 + 2);
 long animation_compute_orientations_symbol_pad352 = node_count * (352 + 2);
 long animation_compute_orientations_symbol_pad353 = node_count * (353 + 2);
 long animation_compute_orientations_symbol_pad354 = node_count * (354 + 2);
 long animation_compute_orientations_symbol_pad355 = node_count * (355 + 2);
 long animation_compute_orientations_symbol_pad356 = node_count * (356 + 2);
 long animation_compute_orientations_symbol_pad357 = node_count * (357 + 2);
 long animation_compute_orientations_symbol_pad358 = node_count * (358 + 2);
 long animation_compute_orientations_symbol_pad359 = node_count * (359 + 2);
 long animation_compute_orientations_symbol_pad360 = node_count * (360 + 2);
 long animation_compute_orientations_symbol_pad361 = node_count * (361 + 2);
 long animation_compute_orientations_symbol_pad362 = node_count * (362 + 2);
 long animation_compute_orientations_symbol_pad363 = node_count * (363 + 2);
 long animation_compute_orientations_symbol_pad364 = node_count * (364 + 2);
 long animation_compute_orientations_symbol_pad365 = node_count * (365 + 2);
 long animation_compute_orientations_symbol_pad366 = node_count * (366 + 2);
 long animation_compute_orientations_symbol_pad367 = node_count * (367 + 2);
 long animation_compute_orientations_symbol_pad368 = node_count * (368 + 2);
 long animation_compute_orientations_symbol_pad369 = node_count * (369 + 2);
 long animation_compute_orientations_symbol_pad370 = node_count * (370 + 2);
 long animation_compute_orientations_symbol_pad371 = node_count * (371 + 2);
 long animation_compute_orientations_symbol_pad372 = node_count * (372 + 2);
 long animation_compute_orientations_symbol_pad373 = node_count * (373 + 2);
 long animation_compute_orientations_symbol_pad374 = node_count * (374 + 2);
 long animation_compute_orientations_symbol_pad375 = node_count * (375 + 2);
 long animation_compute_orientations_symbol_pad376 = node_count * (376 + 2);
 long animation_compute_orientations_symbol_pad377 = node_count * (377 + 2);
 long animation_compute_orientations_symbol_pad378 = node_count * (378 + 2);
 long animation_compute_orientations_symbol_pad379 = node_count * (379 + 2);
 long animation_compute_orientations_symbol_pad380 = node_count * (380 + 2);
 long animation_compute_orientations_symbol_pad381 = node_count * (381 + 2);
 long animation_compute_orientations_symbol_pad382 = node_count * (382 + 2);
 long animation_compute_orientations_symbol_pad383 = node_count * (383 + 2);
 long animation_compute_orientations_symbol_pad384 = node_count * (384 + 2);
 long animation_compute_orientations_symbol_pad385 = node_count * (385 + 2);
 long animation_compute_orientations_symbol_pad386 = node_count * (386 + 2);
 long animation_compute_orientations_symbol_pad387 = node_count * (387 + 2);
 long animation_compute_orientations_symbol_pad388 = node_count * (388 + 2);
 long animation_compute_orientations_symbol_pad389 = node_count * (389 + 2);
 long animation_compute_orientations_symbol_pad390 = node_count * (390 + 2);
 long animation_compute_orientations_symbol_pad391 = node_count * (391 + 2);
 long animation_compute_orientations_symbol_pad392 = node_count * (392 + 2);
 long animation_compute_orientations_symbol_pad393 = node_count * (393 + 2);
 long animation_compute_orientations_symbol_pad394 = node_count * (394 + 2);
 long animation_compute_orientations_symbol_pad395 = node_count * (395 + 2);
 long animation_compute_orientations_symbol_pad396 = node_count * (396 + 2);
 long animation_compute_orientations_symbol_pad397 = node_count * (397 + 2);
 long animation_compute_orientations_symbol_pad398 = node_count * (398 + 2);
 long animation_compute_orientations_symbol_pad399 = node_count * (399 + 2);
 long animation_compute_orientations_symbol_pad400 = node_count * (400 + 2);
 long animation_compute_orientations_symbol_pad401 = node_count * (401 + 2);
 long animation_compute_orientations_symbol_pad402 = node_count * (402 + 2);
 long animation_compute_orientations_symbol_pad403 = node_count * (403 + 2);
 long animation_compute_orientations_symbol_pad404 = node_count * (404 + 2);
 long animation_compute_orientations_symbol_pad405 = node_count * (405 + 2);
 long animation_compute_orientations_symbol_pad406 = node_count * (406 + 2);
 long animation_compute_orientations_symbol_pad407 = node_count * (407 + 2);
 long animation_compute_orientations_symbol_pad408 = node_count * (408 + 2);
 long animation_compute_orientations_symbol_pad409 = node_count * (409 + 2);
 long animation_compute_orientations_symbol_pad410 = node_count * (410 + 2);
 long animation_compute_orientations_symbol_pad411 = node_count * (411 + 2);
 long animation_compute_orientations_symbol_pad412 = node_count * (412 + 2);
 long animation_compute_orientations_symbol_pad413 = node_count * (413 + 2);
 long animation_compute_orientations_symbol_pad414 = node_count * (414 + 2);
 long animation_compute_orientations_symbol_pad415 = node_count * (415 + 2);
 long animation_compute_orientations_symbol_pad416 = node_count * (416 + 2);
 long animation_compute_orientations_symbol_pad417 = node_count * (417 + 2);
 long animation_compute_orientations_symbol_pad418 = node_count * (418 + 2);
 long animation_compute_orientations_symbol_pad419 = node_count * (419 + 2);
 long animation_compute_orientations_symbol_pad420 = node_count * (420 + 2);
 long animation_compute_orientations_symbol_pad421 = node_count * (421 + 2);
 long animation_compute_orientations_symbol_pad422 = node_count * (422 + 2);
 long animation_compute_orientations_symbol_pad423 = node_count * (423 + 2);
 long animation_compute_orientations_symbol_pad424 = node_count * (424 + 2);
 long animation_compute_orientations_symbol_pad425 = node_count * (425 + 2);
 long animation_compute_orientations_symbol_pad426 = node_count * (426 + 2);
 long animation_compute_orientations_symbol_pad427 = node_count * (427 + 2);
 long animation_compute_orientations_symbol_pad428 = node_count * (428 + 2);
 long animation_compute_orientations_symbol_pad429 = node_count * (429 + 2);
 long animation_compute_orientations_symbol_pad430 = node_count * (430 + 2);
 long animation_compute_orientations_symbol_pad431 = node_count * (431 + 2);
 long animation_compute_orientations_symbol_pad432 = node_count * (432 + 2);
 long animation_compute_orientations_symbol_pad433 = node_count * (433 + 2);
 long animation_compute_orientations_symbol_pad434 = node_count * (434 + 2);
 long animation_compute_orientations_symbol_pad435 = node_count * (435 + 2);
 long animation_compute_orientations_symbol_pad436 = node_count * (436 + 2);
 long animation_compute_orientations_symbol_pad437 = node_count * (437 + 2);
 long animation_compute_orientations_symbol_pad438 = node_count * (438 + 2);
 long animation_compute_orientations_symbol_pad439 = node_count * (439 + 2);
 long animation_compute_orientations_symbol_pad440 = node_count * (440 + 2);
 long animation_compute_orientations_symbol_pad441 = node_count * (441 + 2);
 long animation_compute_orientations_symbol_pad442 = node_count * (442 + 2);
 long animation_compute_orientations_symbol_pad443 = node_count * (443 + 2);
 long animation_compute_orientations_symbol_pad444 = node_count * (444 + 2);
 long animation_compute_orientations_symbol_pad445 = node_count * (445 + 2);
 long animation_compute_orientations_symbol_pad446 = node_count * (446 + 2);
 long animation_compute_orientations_symbol_pad447 = node_count * (447 + 2);
 long animation_compute_orientations_symbol_pad448 = node_count * (448 + 2);
 long animation_compute_orientations_symbol_pad449 = node_count * (449 + 2);
 long animation_compute_orientations_symbol_pad450 = node_count * (450 + 2);
 long animation_compute_orientations_symbol_pad451 = node_count * (451 + 2);
 long animation_compute_orientations_symbol_pad452 = node_count * (452 + 2);
 long animation_compute_orientations_symbol_pad453 = node_count * (453 + 2);
 long animation_compute_orientations_symbol_pad454 = node_count * (454 + 2);
 long animation_compute_orientations_symbol_pad455 = node_count * (455 + 2);
 long animation_compute_orientations_symbol_pad456 = node_count * (456 + 2);
 long animation_compute_orientations_symbol_pad457 = node_count * (457 + 2);
 long animation_compute_orientations_symbol_pad458 = node_count * (458 + 2);
 long animation_compute_orientations_symbol_pad459 = node_count * (459 + 2);
 long animation_compute_orientations_symbol_pad460 = node_count * (460 + 2);
 long animation_compute_orientations_symbol_pad461 = node_count * (461 + 2);
 long animation_compute_orientations_symbol_pad462 = node_count * (462 + 2);
 long animation_compute_orientations_symbol_pad463 = node_count * (463 + 2);
 long animation_compute_orientations_symbol_pad464 = node_count * (464 + 2);
 long animation_compute_orientations_symbol_pad465 = node_count * (465 + 2);
 long animation_compute_orientations_symbol_pad466 = node_count * (466 + 2);
 long animation_compute_orientations_symbol_pad467 = node_count * (467 + 2);
 long animation_compute_orientations_symbol_pad468 = node_count * (468 + 2);
 long animation_compute_orientations_symbol_pad469 = node_count * (469 + 2);
 long animation_compute_orientations_symbol_pad470 = node_count * (470 + 2);
 long animation_compute_orientations_symbol_pad471 = node_count * (471 + 2);
 long animation_compute_orientations_symbol_pad472 = node_count * (472 + 2);
 long animation_compute_orientations_symbol_pad473 = node_count * (473 + 2);
 long animation_compute_orientations_symbol_pad474 = node_count * (474 + 2);
 long animation_compute_orientations_symbol_pad475 = node_count * (475 + 2);
 long animation_compute_orientations_symbol_pad476 = node_count * (476 + 2);
 long animation_compute_orientations_symbol_pad477 = node_count * (477 + 2);
 long animation_compute_orientations_symbol_pad478 = node_count * (478 + 2);
 long animation_compute_orientations_symbol_pad479 = node_count * (479 + 2);
 long animation_compute_orientations_symbol_pad480 = node_count * (480 + 2);
 long animation_compute_orientations_symbol_pad481 = node_count * (481 + 2);
 long animation_compute_orientations_symbol_pad482 = node_count * (482 + 2);
 long animation_compute_orientations_symbol_pad483 = node_count * (483 + 2);
 long animation_compute_orientations_symbol_pad484 = node_count * (484 + 2);
 long animation_compute_orientations_symbol_pad485 = node_count * (485 + 2);
 long animation_compute_orientations_symbol_pad486 = node_count * (486 + 2);
 long animation_compute_orientations_symbol_pad487 = node_count * (487 + 2);
 long animation_compute_orientations_symbol_pad488 = node_count * (488 + 2);
 long animation_compute_orientations_symbol_pad489 = node_count * (489 + 2);
 long animation_compute_orientations_symbol_pad490 = node_count * (490 + 2);
 long animation_compute_orientations_symbol_pad491 = node_count * (491 + 2);
 long animation_compute_orientations_symbol_pad492 = node_count * (492 + 2);
 long animation_compute_orientations_symbol_pad493 = node_count * (493 + 2);
 long animation_compute_orientations_symbol_pad494 = node_count * (494 + 2);
 long animation_compute_orientations_symbol_pad495 = node_count * (495 + 2);
 long animation_compute_orientations_symbol_pad496 = node_count * (496 + 2);
 long animation_compute_orientations_symbol_pad497 = node_count * (497 + 2);
 long animation_compute_orientations_symbol_pad498 = node_count * (498 + 2);
 long animation_compute_orientations_symbol_pad499 = node_count * (499 + 2);
 long animation_compute_orientations_symbol_pad500 = node_count * (500 + 2);
 long animation_compute_orientations_symbol_pad501 = node_count * (501 + 2);
 long animation_compute_orientations_symbol_pad502 = node_count * (502 + 2);
 long animation_compute_orientations_symbol_pad503 = node_count * (503 + 2);
 long animation_compute_orientations_symbol_pad504 = node_count * (504 + 2);
 long animation_compute_orientations_symbol_pad505 = node_count * (505 + 2);
 long animation_compute_orientations_symbol_pad506 = node_count * (506 + 2);
 long animation_compute_orientations_symbol_pad507 = node_count * (507 + 2);
 long animation_compute_orientations_symbol_pad508 = node_count * (508 + 2);
 long animation_compute_orientations_symbol_pad509 = node_count * (509 + 2);
 long animation_compute_orientations_symbol_pad510 = node_count * (510 + 2);
 long animation_compute_orientations_symbol_pad511 = node_count * (511 + 2);
 long animation_compute_orientations_symbol_pad512 = node_count * (512 + 2);
 long animation_compute_orientations_symbol_pad513 = node_count * (513 + 2);
 long animation_compute_orientations_symbol_pad514 = node_count * (514 + 2);
 long animation_compute_orientations_symbol_pad515 = node_count * (515 + 2);
 long animation_compute_orientations_symbol_pad516 = node_count * (516 + 2);
 long animation_compute_orientations_symbol_pad517 = node_count * (517 + 2);
 long animation_compute_orientations_symbol_pad518 = node_count * (518 + 2);
 long animation_compute_orientations_symbol_pad519 = node_count * (519 + 2);
 long animation_compute_orientations_symbol_pad520 = node_count * (520 + 2);
 long animation_compute_orientations_symbol_pad521 = node_count * (521 + 2);
 long animation_compute_orientations_symbol_pad522 = node_count * (522 + 2);
 long animation_compute_orientations_symbol_pad523 = node_count * (523 + 2);
 long animation_compute_orientations_symbol_pad524 = node_count * (524 + 2);
 long animation_compute_orientations_symbol_pad525 = node_count * (525 + 2);
 long animation_compute_orientations_symbol_pad526 = node_count * (526 + 2);
 long animation_compute_orientations_symbol_pad527 = node_count * (527 + 2);
 long animation_compute_orientations_symbol_pad528 = node_count * (528 + 2);
 long animation_compute_orientations_symbol_pad529 = node_count * (529 + 2);
 long animation_compute_orientations_symbol_pad530 = node_count * (530 + 2);
 long animation_compute_orientations_symbol_pad531 = node_count * (531 + 2);
 long animation_compute_orientations_symbol_pad532 = node_count * (532 + 2);
 long animation_compute_orientations_symbol_pad533 = node_count * (533 + 2);
 long animation_compute_orientations_symbol_pad534 = node_count * (534 + 2);
 long animation_compute_orientations_symbol_pad535 = node_count * (535 + 2);
 long animation_compute_orientations_symbol_pad536 = node_count * (536 + 2);
 long animation_compute_orientations_symbol_pad537 = node_count * (537 + 2);
 long animation_compute_orientations_symbol_pad538 = node_count * (538 + 2);
 long animation_compute_orientations_symbol_pad539 = node_count * (539 + 2);
 long animation_compute_orientations_symbol_pad540 = node_count * (540 + 2);
 long animation_compute_orientations_symbol_pad541 = node_count * (541 + 2);
 long animation_compute_orientations_symbol_pad542 = node_count * (542 + 2);
 long animation_compute_orientations_symbol_pad543 = node_count * (543 + 2);
 long animation_compute_orientations_symbol_pad544 = node_count * (544 + 2);
 long animation_compute_orientations_symbol_pad545 = node_count * (545 + 2);
 long animation_compute_orientations_symbol_pad546 = node_count * (546 + 2);
 long animation_compute_orientations_symbol_pad547 = node_count * (547 + 2);
 long animation_compute_orientations_symbol_pad548 = node_count * (548 + 2);
 long animation_compute_orientations_symbol_pad549 = node_count * (549 + 2);
 long animation_compute_orientations_symbol_pad550 = node_count * (550 + 2);
 long animation_compute_orientations_symbol_pad551 = node_count * (551 + 2);
 long animation_compute_orientations_symbol_pad552 = node_count * (552 + 2);
 long animation_compute_orientations_symbol_pad553 = node_count * (553 + 2);
 long animation_compute_orientations_symbol_pad554 = node_count * (554 + 2);
 long animation_compute_orientations_symbol_pad555 = node_count * (555 + 2);
 long animation_compute_orientations_symbol_pad556 = node_count * (556 + 2);
 long animation_compute_orientations_symbol_pad557 = node_count * (557 + 2);
 long animation_compute_orientations_symbol_pad558 = node_count * (558 + 2);
 long animation_compute_orientations_symbol_pad559 = node_count * (559 + 2);
 long animation_compute_orientations_symbol_pad560 = node_count * (560 + 2);
 long animation_compute_orientations_symbol_pad561 = node_count * (561 + 2);
 long animation_compute_orientations_symbol_pad562 = node_count * (562 + 2);
 long animation_compute_orientations_symbol_pad563 = node_count * (563 + 2);
 long animation_compute_orientations_symbol_pad564 = node_count * (564 + 2);
 long animation_compute_orientations_symbol_pad565 = node_count * (565 + 2);
 long animation_compute_orientations_symbol_pad566 = node_count * (566 + 2);
 long animation_compute_orientations_symbol_pad567 = node_count * (567 + 2);
 long animation_compute_orientations_symbol_pad568 = node_count * (568 + 2);
 long animation_compute_orientations_symbol_pad569 = node_count * (569 + 2);
 long animation_compute_orientations_symbol_pad570 = node_count * (570 + 2);
 long animation_compute_orientations_symbol_pad571 = node_count * (571 + 2);
 long animation_compute_orientations_symbol_pad572 = node_count * (572 + 2);
 long animation_compute_orientations_symbol_pad573 = node_count * (573 + 2);
 long animation_compute_orientations_symbol_pad574 = node_count * (574 + 2);
 long animation_compute_orientations_symbol_pad575 = node_count * (575 + 2);
 long animation_compute_orientations_symbol_pad576 = node_count * (576 + 2);
 long animation_compute_orientations_symbol_pad577 = node_count * (577 + 2);
 long animation_compute_orientations_symbol_pad578 = node_count * (578 + 2);
 long animation_compute_orientations_symbol_pad579 = node_count * (579 + 2);
 long animation_compute_orientations_symbol_pad580 = node_count * (580 + 2);
 long animation_compute_orientations_symbol_pad581 = node_count * (581 + 2);
 long animation_compute_orientations_symbol_pad582 = node_count * (582 + 2);
 long animation_compute_orientations_symbol_pad583 = node_count * (583 + 2);
 long animation_compute_orientations_symbol_pad584 = node_count * (584 + 2);
 long animation_compute_orientations_symbol_pad585 = node_count * (585 + 2);
 long animation_compute_orientations_symbol_pad586 = node_count * (586 + 2);
 long animation_compute_orientations_symbol_pad587 = node_count * (587 + 2);
 long animation_compute_orientations_symbol_pad588 = node_count * (588 + 2);
 long animation_compute_orientations_symbol_pad589 = node_count * (589 + 2);
 long animation_compute_orientations_symbol_pad590 = node_count * (590 + 2);
 long animation_compute_orientations_symbol_pad591 = node_count * (591 + 2);
 long animation_compute_orientations_symbol_pad592 = node_count * (592 + 2);
 long animation_compute_orientations_symbol_pad593 = node_count * (593 + 2);
 long animation_compute_orientations_symbol_pad594 = node_count * (594 + 2);
 long animation_compute_orientations_symbol_pad595 = node_count * (595 + 2);
 long animation_compute_orientations_symbol_pad596 = node_count * (596 + 2);
 long animation_compute_orientations_symbol_pad597 = node_count * (597 + 2);
 long animation_compute_orientations_symbol_pad598 = node_count * (598 + 2);
 long animation_compute_orientations_symbol_pad599 = node_count * (599 + 2);
 long animation_compute_orientations_symbol_pad600 = node_count * (600 + 2);
 long animation_compute_orientations_symbol_pad601 = node_count * (601 + 2);
 long animation_compute_orientations_symbol_pad602 = node_count * (602 + 2);
 long animation_compute_orientations_symbol_pad603 = node_count * (603 + 2);
 long animation_compute_orientations_symbol_pad604 = node_count * (604 + 2);
 long animation_compute_orientations_symbol_pad605 = node_count * (605 + 2);
 long animation_compute_orientations_symbol_pad606 = node_count * (606 + 2);
 long animation_compute_orientations_symbol_pad607 = node_count * (607 + 2);
 long animation_compute_orientations_symbol_pad608 = node_count * (608 + 2);
 long animation_compute_orientations_symbol_pad609 = node_count * (609 + 2);
 long animation_compute_orientations_symbol_pad610 = node_count * (610 + 2);
 long animation_compute_orientations_symbol_pad611 = node_count * (611 + 2);
 long animation_compute_orientations_symbol_pad612 = node_count * (612 + 2);
 long animation_compute_orientations_symbol_pad613 = node_count * (613 + 2);
 long animation_compute_orientations_symbol_pad614 = node_count * (614 + 2);
 long animation_compute_orientations_symbol_pad615 = node_count * (615 + 2);
 long animation_compute_orientations_symbol_pad616 = node_count * (616 + 2);
 long animation_compute_orientations_symbol_pad617 = node_count * (617 + 2);
 long animation_compute_orientations_symbol_pad618 = node_count * (618 + 2);
 long animation_compute_orientations_symbol_pad619 = node_count * (619 + 2);
 long animation_compute_orientations_symbol_pad620 = node_count * (620 + 2);
 long animation_compute_orientations_symbol_pad621 = node_count * (621 + 2);
 long animation_compute_orientations_symbol_pad622 = node_count * (622 + 2);
 long animation_compute_orientations_symbol_pad623 = node_count * (623 + 2);
 long animation_compute_orientations_symbol_pad624 = node_count * (624 + 2);
 long animation_compute_orientations_symbol_pad625 = node_count * (625 + 2);
 long animation_compute_orientations_symbol_pad626 = node_count * (626 + 2);
 long animation_compute_orientations_symbol_pad627 = node_count * (627 + 2);
 long animation_compute_orientations_symbol_pad628 = node_count * (628 + 2);
 long animation_compute_orientations_symbol_pad629 = node_count * (629 + 2);
 long animation_compute_orientations_symbol_pad630 = node_count * (630 + 2);
 long animation_compute_orientations_symbol_pad631 = node_count * (631 + 2);
 long animation_compute_orientations_symbol_pad632 = node_count * (632 + 2);
 long animation_compute_orientations_symbol_pad633 = node_count * (633 + 2);
 long animation_compute_orientations_symbol_pad634 = node_count * (634 + 2);
 long animation_compute_orientations_symbol_pad635 = node_count * (635 + 2);
 long animation_compute_orientations_symbol_pad636 = node_count * (636 + 2);
 long animation_compute_orientations_symbol_pad637 = node_count * (637 + 2);
 long animation_compute_orientations_symbol_pad638 = node_count * (638 + 2);
 long animation_compute_orientations_symbol_pad639 = node_count * (639 + 2);
 long animation_compute_orientations_symbol_pad640 = node_count * (640 + 2);
 long animation_compute_orientations_symbol_pad641 = node_count * (641 + 2);
 long animation_compute_orientations_symbol_pad642 = node_count * (642 + 2);
 long animation_compute_orientations_symbol_pad643 = node_count * (643 + 2);
 long animation_compute_orientations_symbol_pad644 = node_count * (644 + 2);
 long animation_compute_orientations_symbol_pad645 = node_count * (645 + 2);
 long animation_compute_orientations_symbol_pad646 = node_count * (646 + 2);
 long animation_compute_orientations_symbol_pad647 = node_count * (647 + 2);
 long animation_compute_orientations_symbol_pad648 = node_count * (648 + 2);
 long animation_compute_orientations_symbol_pad649 = node_count * (649 + 2);
 long animation_compute_orientations_symbol_pad650 = node_count * (650 + 2);
 long animation_compute_orientations_symbol_pad651 = node_count * (651 + 2);
 long animation_compute_orientations_symbol_pad652 = node_count * (652 + 2);
 long animation_compute_orientations_symbol_pad653 = node_count * (653 + 2);
 long animation_compute_orientations_symbol_pad654 = node_count * (654 + 2);
 long animation_compute_orientations_symbol_pad655 = node_count * (655 + 2);
 long animation_compute_orientations_symbol_pad656 = node_count * (656 + 2);
 long animation_compute_orientations_symbol_pad657 = node_count * (657 + 2);
 long animation_compute_orientations_symbol_pad658 = node_count * (658 + 2);
 long animation_compute_orientations_symbol_pad659 = node_count * (659 + 2);
 long animation_compute_orientations_symbol_pad660 = node_count * (660 + 2);
 long animation_compute_orientations_symbol_pad661 = node_count * (661 + 2);
 long animation_compute_orientations_symbol_pad662 = node_count * (662 + 2);
 long animation_compute_orientations_symbol_pad663 = node_count * (663 + 2);
 long animation_compute_orientations_symbol_pad664 = node_count * (664 + 2);
 long animation_compute_orientations_symbol_pad665 = node_count * (665 + 2);
 long animation_compute_orientations_symbol_pad666 = node_count * (666 + 2);
 long animation_compute_orientations_symbol_pad667 = node_count * (667 + 2);
 long animation_compute_orientations_symbol_pad668 = node_count * (668 + 2);
 long animation_compute_orientations_symbol_pad669 = node_count * (669 + 2);
 long animation_compute_orientations_symbol_pad670 = node_count * (670 + 2);
 long animation_compute_orientations_symbol_pad671 = node_count * (671 + 2);
 long animation_compute_orientations_symbol_pad672 = node_count * (672 + 2);
 long animation_compute_orientations_symbol_pad673 = node_count * (673 + 2);
 long animation_compute_orientations_symbol_pad674 = node_count * (674 + 2);
 long animation_compute_orientations_symbol_pad675 = node_count * (675 + 2);
 long animation_compute_orientations_symbol_pad676 = node_count * (676 + 2);
 long animation_compute_orientations_symbol_pad677 = node_count * (677 + 2);
 long animation_compute_orientations_symbol_pad678 = node_count * (678 + 2);
 long animation_compute_orientations_symbol_pad679 = node_count * (679 + 2);
 long animation_compute_orientations_symbol_pad680 = node_count * (680 + 2);
 long animation_compute_orientations_symbol_pad681 = node_count * (681 + 2);
 long animation_compute_orientations_symbol_pad682 = node_count * (682 + 2);
 long animation_compute_orientations_symbol_pad683 = node_count * (683 + 2);
 long animation_compute_orientations_symbol_pad684 = node_count * (684 + 2);
 long animation_compute_orientations_symbol_pad685 = node_count * (685 + 2);
 long animation_compute_orientations_symbol_pad686 = node_count * (686 + 2);
 long animation_compute_orientations_symbol_pad687 = node_count * (687 + 2);
 long animation_compute_orientations_symbol_pad688 = node_count * (688 + 2);
 long animation_compute_orientations_symbol_pad689 = node_count * (689 + 2);
 long animation_compute_orientations_symbol_pad690 = node_count * (690 + 2);
 if (animation_compute_orientations_flags_empty_loop(requested))
  return;

 XMemCpy(flags, animated, sizeof(s_node_flag_sets));
 animation_mask_node_flags(flags, requested);
 unsigned long words = (node_count + 31) >> 5;
 unsigned long word_count = animation_compute_orientations_word_count(header, words);

 if (!animation_node_flags_are_empty(&flags->rotation))
 {
  long channel_next = 0;
  const unsigned long *mask = animated->rotation.m_storage;
  long word = 0;
  if (word < (long)word_count)
  {
   __vector4 key_bias = animation_compute_orientations_key_constant(1);
   __vector4 key_scale = animation_compute_orientations_key_constant(0);
   __vector4 bias = animation_compute_orientations_quantized_constant(1);
   __vector4 scale = animation_compute_orientations_quantized_constant(0);
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
       long ones_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~a)];
       long ones_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~b)];
       long run = ones_a > ones_b ? ones_b : ones_a;
       if (run > 0)
       {
        for (long i = 0; i < run; i++)
        {
         real_orientation *node_out = &out[bit + base + i];
         t_codec::decompress_rotation(header, channel + i, frame0, frame1, w0, w1, node_out, scale, bias, key_scale, key_bias);
        }
        ones_a -= run;
        bit += run;
        channel += run;
        a >>= run;
        b >>= run;
       }
       if (ones_a > 0)
       {
        long zeros_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(b)];
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
          t_codec::decompress_rotation(header, channel, frame0, frame1, w0, w1, node_out, scale, bias, key_scale, key_bias);
         }
         ones_a--;
         bit++;
         channel++;
         a >>= 1;
         b >>= 1;
        }
       }
       long zeros_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(a)];
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
    t_codec::decompress_translation(header, 0, frame0, frame1, w0, w1, out);
   if (flags->scale.m_storage[0] & 1)
    t_codec::decompress_scale(header, 0, frame0, frame1, w0, w1, out);
  }
  animation_compute_orientations_keep_root_only(&flags->translation);
  animation_compute_orientations_keep_root_only(&flags->scale);
  return;
 }

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
       long ones_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~a)];
       long ones_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~b)];
       long run = ones_a > ones_b ? ones_b : ones_a;
       if (run > 0)
       {
        for (long i = 0; i < run; i++)
        {
         real_orientation *node_out = &out[bit + base + i];
         t_codec::decompress_translation(header, channel + i, frame0, frame1, w0, w1, node_out);
        }
        ones_a -= run;
        bit += run;
        channel += run;
        a >>= run;
        b >>= run;
       }
       if (ones_a > 0)
       {
        long zeros_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(b)];
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
          t_codec::decompress_translation(header, channel, frame0, frame1, w0, w1, node_out);
         }
         ones_a--;
         bit++;
         channel++;
         a >>= 1;
         b >>= 1;
        }
       }
       long zeros_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(a)];
       bit += zeros_a;
       a >>= zeros_a;
       b >>= zeros_a;
      } while (bit < 32);
     }
    }
   } while (channel_next = channel, ++word < (long)word_count);
  }
 }

 if (!animation_compute_orientations_flags_empty_loop(&flags->scale))
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
    if (a == 0)
     continue;
    if (b == 0 || node_count == 0)
    {
     channel += count_bits<unsigned long>(a);
    }
    else
    {
     bit = 0;
     do
     {
      long ones_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~a)];
      long ones_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(~b)];
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
       long zeros_b = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(b)];
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
      long zeros_a = k_animation_lowest_set_bit_table[animation_compute_orientations_lowest_bit_index(a)];
      bit += zeros_a;
      a >>= zeros_a;
      b >>= zeros_a;
     } while (bit < 32);
    }
   } while (channel_next = channel, ++word < (long)word_count);
  }
 }
}

template struct c_fullframe_codec_decompressor<c_uncompressed_static_data_codec>;
template struct c_fullframe_codec_decompressor<c_uncompressed_animated_data_codec>;
template struct c_fullframe_codec_decompressor<c_8byte_quantized_rotation_only_codec>;
template struct c_fullframe_codec_decompressor<c_8byte_quantized_rotation_only_interpolated_codec>;

#if !defined(_XBOX) || defined(__clang__)
#define DECOMPRESS_CODEC(name, decompressor, codec) \
void name(const s_animation_codec_header *header, long node_count, \
    unsigned long frame0, unsigned long frame1, real w0, real w1, bool root_only, \
    const s_node_flag_sets *animated, const flags256 *requested, s_node_flag_sets *flags, real_orientation *out) \
{ \
    typedef codec::header_type header_type; \
    decompressor<codec>::decompress_all(static_cast<const header_type *>(header), node_count, \
        frame0, frame1, w0, w1, root_only, animated, requested, flags, out); \
}

DECOMPRESS_CODEC(animation_codec_decompress_static, c_fullframe_codec_decompressor, c_uncompressed_static_data_codec)
DECOMPRESS_CODEC(animation_codec_decompress_fullframe, c_fullframe_codec_decompressor, c_uncompressed_animated_data_codec)
DECOMPRESS_CODEC(animation_codec_decompress_quantized, c_fullframe_codec_decompressor, c_8byte_quantized_rotation_only_codec)
DECOMPRESS_CODEC(animation_codec_decompress_quantized_interpolated, c_fullframe_codec_decompressor, c_8byte_quantized_rotation_only_interpolated_codec)
DECOMPRESS_CODEC(animation_codec_decompress_byte_keyframe_interpolated, c_codec_decompressor, c_interpolated_keyframe_codec<unsigned char>)

typedef c_keyframe_codec<unsigned char, false> c_byte_keyframe_codec;
typedef c_keyframe_codec<unsigned short, false> c_word_keyframe_codec;
DECOMPRESS_CODEC(animation_codec_decompress_byte_keyframe, c_codec_decompressor, c_byte_keyframe_codec)
DECOMPRESS_CODEC(animation_codec_decompress_word_keyframe, c_codec_decompressor, c_word_keyframe_codec)
#undef DECOMPRESS_CODEC
#endif
