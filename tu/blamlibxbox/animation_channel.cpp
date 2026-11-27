// blamlibxbox/animation_channel.obj: from the records 0xdb468 0xdb4c8 0xdb528 0xdbad8 0xdcdf0 0xdcfc8 0xdf8c0 0xe00e8 0xe0278 0xe1da0 0xe2418 0xe9398 0xe95c8 0xe97a8 0xed798 0xedaf8 0xedc40 0xfe170 0xfe390 0x1022b8 0x2fa840 0x2fa908 0x2fa988 0x2fa9f0 0x2faa68 0x2fab28 0x2fae20 0x2faf30 0x2fb030 0x2fb110 0x2fb1a8 0x2fb350 0x2fb4b0 0x2fb608 0x2fb6b8 0x2fb790 0x2fb898 0x2fb998 0x2fba78 0x2fbb38 0x2fcf70 0x2fe450
// @flags /TP /O2 /Oc
#include "math/real_math_constants.h"
#include "math/real_math_types.h"
#include "math/real_math_globals.h"
#include "animations/animation_id.h"
#include "memory/static_arrays.h"
#include "cseries/cseries_wrapped.h"
#include "cseries/cseries.h"
#include "game/game_time_types.h"
// @address 0xdb528 ?reset@c_animation_channel@@QAAXXZ
// @address 0xdcfc8 ?paused@c_animation_channel@@QBA_NXZ
// @address 0xe00e8 ?get_dominant_animation_id@c_animation_channel@@QBA?AVc_animation_id@@XZ
// @address 0xe0278 ?note_all_future_and_past_events@c_animation_channel@@AAAXJ@Z
// @address 0xe2418 animation_channel_compute_base_weight
// @address 0xfe170 animation_channel_get_blended_animation_data
// @address 0x2fcf70 animation_channel_get_pill_offset
// @address 0xedaf8 ?set_frame_position@c_animation_channel@@QAAXM@Z
// @address 0xe09e8 ?get_playback_ratio_from_frame_position@c_model_animation@@QBAMM@Z
// @address 0xeda90 ?get_frame_position_from_playback_ratio@c_model_animation@@QBAMM@Z
// @address 0x10a9f8 ?animation_contains_movement_data@c_model_animation_graph@@QBA_NVc_animation_id@@@Z
// @address 0xe6368 ?export_real@?$t_short_fixed@$0N@@@QBAMXZ
// @address 0xe20f8 ?get_animation_data@c_model_animation_graph@@QBA_NVc_animation_id@@PAVc_animation_data@@@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0xdf848 ?playing@c_animation_channel@@QBA_NXZ
// @address 0xed968 ?setup_channel_base@c_channel_base@@IAA_NPBVc_base_channel_settings@@@Z
// @address 0xe2220 ?valid@c_animation_data@@QBA_NXZ
#include <math.h>

#include "objects/objects.h"
#include "animations/animation_definitions.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "animations/mixing_board/channels/anim_computation_flags_types.h"
#include "animations/animation_aiming_screen_bounds.h"
#include "animations/animation_channel_events.h"
#include "animations/animation_data.h"
#include "animations/animation_definition_types.h"
#include "animations/animation_event_data.h"
#include "animations/animation_node_flags.h"
#include "animations/animation_search_path_iterator.h"
#include "animations/mixing_board/channels/animation_channel_pill_offset.h"
#include "animations/mixing_board/channels/facial_animation_channel_types.h"
#include "math/real_math.h"
#include "tag_files/tag_groups.h"
#include "game/game_time_inline.h"

union real_vector3d;
const float k_real_max = 3.4028234663852886e+38f;
const float k_animation_channel_tick_scale = 30.0f;
const float k_animation_channel_tick_tolerance = 0.0001f;

__forceinline bool greater(short a, short b)
{
  return a > b;
}


struct render_model_definition;
struct real_orientation;
class c_channel_base;
static __forceinline void animation_channel_get_base_channel_settings(const c_channel_base *object, c_base_channel_settings *settings)
{
    settings->m_graph_index = object->m_graph_index;
    settings->m_definition_index = object->m_definition_index;
    settings->m_animation_id = object->m_animation_id;
    settings->m_location = object->m_location;
    settings->m_playback_flags = object->m_playback_flags;
}
// match: 0xdf8c0 0xedaf8 0x2fa840: the named local keeps `!looping_enabled()` a branch on the tested bit
// (March has no local here); without it c2 emits an extra `not` in all three
static __forceinline bool animation_channel_looping_enabled(const c_channel_base *object)
{
    bool looping = object->m_playback_flags.test(_animation_playback_looping_bit);
    return looping;
}
static __forceinline void animation_channel_set_animation_id(c_channel_base *object, c_animation_id animation_id) {
    object->m_animation_id = animation_id; }
class c_model_animation;
class c_animation_manager;
static __forceinline bool operator==(const c_animation_id &a, const c_animation_id &b)
{
    return a.m_graph_index == b.m_graph_index && a.m_animation_index == b.m_animation_index;
}
class c_animation_channel;
union real_vector3d;

union real_point3d;
union real_vector3d;
static __forceinline bool animation_channel_analog_blending_weights_locked(const c_animation_channel *object)
{
    return object->m_state_flags.test(_animation_state_blending_locked_bit);
}
static __forceinline void animation_channel_pause_playback(c_animation_channel *object, bool pause)
{
    if (object->autoplay_enabled())
        object->m_state_flags.set(_animation_state_paused_bit, pause);
}
static __forceinline void animation_channel_lock_analog_blending_weights(c_animation_channel *object, bool lock)
{
    object->m_state_flags.set(_animation_state_blending_locked_bit, lock);
}
static __forceinline float animation_channel_get_playback_rate(const c_animation_channel *object)
{
    float playback_rate = object->m_playback_speed * object->get_authored_rate();
    return playback_rate;
}
static __forceinline float animation_channel_get_frame_position_step(const c_animation_channel *object)
{
    return game_tick_length() * animation_channel_get_playback_rate(object);
}
// @address 0x2fb110 ?get_playback_ratio_remaining@c_animation_channel@@QBAMXZ
float c_animation_channel::get_playback_ratio_remaining() const
{
    float playback_ratio_remaining = 0.0f;
    if (valid())
        playback_ratio_remaining = 1.0f - get_playback_ratio();
    return playback_ratio_remaining;
}


// @address 0x3aa90 k_real_math_origin3d

// The frame count of the animation a channel plays (the graph's animation, found by the channel's id).

static __forceinline float animation_channel_frames_to_time(float frames, float rate)
{
    float clamped_rate = rate > 0.0001f ? rate : 0.0001f;
    return frames / clamped_rate;
}





// @address 0x1022b8 ?get_dominant_animation@c_animation_channel@@QBAPBVc_model_animation@@XZ
const c_model_animation *c_animation_channel::get_dominant_animation() const
{
    c_animation_id dominant_reference = get_dominant_animation_id();

    const c_model_animation *animation = 0;
    if (dominant_reference.valid())
        animation = TAG_GET('jmad', const c_model_animation_graph, m_graph_index)->get_animation(dominant_reference);
    return animation;
}

// @address 0x2fa840 ?enable_looping@c_animation_channel@@QAAX_N@Z
void c_animation_channel::enable_looping(bool enable)
{
    c_flags<e_animation_playback_flag_bits, unsigned short, 16> playback_flags = m_playback_flags;
    playback_flags.set(_animation_playback_looping_bit, false);
    m_playback_flags = playback_flags;
    const c_model_animation *animation = get_animation();
    if ((float)(animation->frame_count - 1) <= m_frame_position)
    {
        m_state_flags.set(_animation_state_finished_bit, true);
        m_state_flags.set(_animation_state_stopped_bit, !animation_channel_looping_enabled(this));
    }
}
void c_animation_channel::set_frame_position(float frame_position)
{
    const c_model_animation *animation = get_animation();
    float last_frame = (float)(animation->frame_count - 1);
    this->m_frame_position = MIN(MAX(frame_position, k_real_zero), last_frame);
    this->m_playback_ratio = animation->get_playback_ratio_from_frame_position(this->m_frame_position);
    this->note_all_future_and_past_events(0);
    this->note_all_future_and_past_events(1);
    if (last_frame <= this->m_frame_position)
    {
        this->m_playback_ratio = k_real_one;
        this->m_state_flags.set(_animation_state_finished_bit, true);
        this->m_state_flags.set(_animation_state_stopped_bit, !animation_channel_looping_enabled(this));
    }
    this->m_state_flags.set(_animation_state_unknown3_bit, true);
    this->m_state_flags.set(_animation_state_unknown4_bit, true);
}
// @address 0x2fa908 ?jump_to_last_frame_and_stop@c_animation_channel@@QAAXXZ
void c_animation_channel::jump_to_last_frame_and_stop()
{
    set_frame_position_from_playback_ratio(1.0f);
    m_playback_flags.set(_animation_playback_autoplay_bit, false);
    enable_looping(false);
    m_state_flags.set(_animation_state_stopped_bit, true);
    m_state_flags.set(_animation_state_paused_bit, true);
    m_state_flags.set(_animation_state_unknown3_bit, true);
    m_state_flags.set(_animation_state_unknown4_bit, true);
}
// @address 0x2fa988 ?set_frame_position_from_playback_ratio@c_animation_channel@@QAAXM@Z
void c_animation_channel::set_frame_position_from_playback_ratio(float playback_ratio)
{
    const c_model_animation *animation = get_animation();
    float frame_position = animation->get_frame_position_from_playback_ratio(playback_ratio);
    (this)->set_frame_position(frame_position);
}
// @address 0x2fa9f0 ?update_to_playback_ratio@c_animation_channel@@QAA_NMPBVc_animation_manager@@P6AXPBUs_animation_event_data@@J@ZJ@Z
bool c_animation_channel::update_to_playback_ratio(
    float playback_ratio, const c_animation_manager *manager, void (*callback)(const s_animation_event_data *, long), long user_data)
{
    // every caller passes object_animation_callback (giant_scarab_motor_program.cpp)
    callback = object_animation_callback;
    const c_model_animation *animation = get_animation();
    float frame_position = animation->get_frame_position_from_playback_ratio(playback_ratio);
    return update_to_frame_position(frame_position, manager, callback, user_data);
}

static __forceinline real_vector3d *animation_channel16_scale_vector3d(const real_vector3d *source, float scale, real_vector3d *output)
{
    output->i = scale * source->i;
    output->j = scale * source->j;
    output->k = scale * source->k;
    return output;
}
static __forceinline real_vector3d *animation_channel16_add_vectors3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *output)
{
    output->i = a->i + b->i;
    output->j = a->j + b->j;
    output->k = a->k + b->k;
    return output;
}
static __forceinline real_point3d *animation_channel16_scale_point3d(const real_point3d *source, float scale, real_point3d *output)
{
    output->x = scale * source->x;
    output->y = scale * source->y;
    output->z = scale * source->z;
    return output;
}
static __forceinline real_point3d *animation_channel16_add_points3d(const real_point3d *a, const real_point3d *b, real_point3d *output)
{
    output->x = a->x + b->x;
    output->y = a->y + b->y;
    output->z = a->z + b->z;
    return output;
}
// match: 0x2fab28, 0xe1da0: depth-0 keeps export_real a bl call; without it the fixed-point body inlines at all 4 sites and grows both frames
#pragma inline_depth(0)
static __forceinline float animation_channel16_export_weight(const t_short_fixed<13> *weight)
{
    float result = weight->export_real();
    return result;
}
// match: 0x2fab28: restore default depth so depth-0 does not leak into the callers compiled below
#pragma inline_depth()

// @address 0x2fab28 ?get_motion_data_for_frame@c_animation_channel@@QBA_NMPATreal_point3d@@PATreal_vector3d@@@Z
bool c_animation_channel::get_motion_data_for_frame(float frame, real_point3d *position, real_vector3d *delta) const
{
    const c_animation_channel *self = this;
    bool result = false;
    if (self->valid())
    do
    {
        {
            c_animation_data animation_data;
            animation_data.m_data = 0;
            animation_data.m_sizes = 0;
            animation_data.m_node_count = 0;
            animation_data.m_movement_data_type = 0;
            if (!(animation_definitions_try_and_get_data((const c_channel_base *)self, &animation_data) &&
                    animation_data.m_sizes->contains_pill_offset_data()))
                break;
            long frame_index = (long)frame;
            animation_channel_get_pill_offset(&animation_data, frame_index, position, delta);
        }
        result = true;
        {
            t_short_fixed<13> base_weight;
            short *base_weight_output = &base_weight.m_value;
            animation_channel_compute_base_weight(base_weight_output, self);
            if (base_weight.m_value == t_short_fixed<13>::k_fixed_one)
                break;
            float base_scale = animation_channel16_export_weight(&base_weight);
            animation_channel16_scale_vector3d(delta, base_scale, delta);
            animation_channel16_scale_point3d(position, base_scale, position);
        }
        const c_model_animation *animation = self->get_animation();
        for (long animation_index = 0; animation_index < 2; animation_index++)
        {
            if (self->m_animation_ids[animation_index].valid() && self->m_weights[animation_index] > 0)
            {
                float weight = animation_channel16_export_weight(&self->m_weights[animation_index]);
                const c_model_animation *blended_animation = 0;
                if (self->m_animation_ids[animation_index].valid())
                {
                    blended_animation = 
                        TAG_GET('jmad', const c_model_animation_graph, self->m_graph_index)->get_animation(self->m_animation_ids[animation_index]);
                }
                long blended_frame_index = (long)compute_relative_frame_index(animation, blended_animation, frame);
                c_animation_data blended_data; blended_data.m_data = 0; blended_data.m_sizes = 0; blended_data.m_node_count = 0;
                    blended_data.m_movement_data_type = 0;
                if (animation_channel_get_blended_animation_data(self, animation_index, &blended_data))
                {
                    real_point3d blended_position;
                    real_vector3d blended_delta;
                    animation_channel_get_pill_offset(&blended_data, blended_frame_index, &blended_position, &blended_delta);
                    animation_channel16_scale_vector3d(&blended_delta, weight, &blended_delta);
                    animation_channel16_add_vectors3d(delta, &blended_delta, delta);
                    animation_channel16_scale_point3d(&blended_position, weight, &blended_position);
                    animation_channel16_add_points3d(position, &blended_position, position);
                }
            }
        }
    } while (false);
    return result;
}
// @address 0x2faa68 ?get_pill_offset_data@c_animation_channel@@QBA_NPATreal_point3d@@PATreal_vector3d@@@Z
bool c_animation_channel::get_pill_offset_data(real_point3d *position, real_vector3d *delta) const
{
    bool valid_channel;
    bool result = false;
    real_vector3d frame_delta;
    valid_channel = valid();
    if (valid_channel && get_motion_data_for_frame(m_frame_position, position, &frame_delta))
    {
        result = true;
        delta->i = frame_delta.i * 30.0f;
        delta->j = frame_delta.j * 30.0f;
        delta->k = frame_delta.k * 30.0f;
    }
    return result;
}

// @address 0x2fae20 ?contains_motion_data@c_animation_channel@@QBA_NXZ
bool c_animation_channel::contains_motion_data() const
{
    bool contains_motion = false;
    if (valid())
    {
        const c_model_animation_graph *graph = TAG_GET('jmad', const c_model_animation_graph, m_graph_index);
        contains_motion = graph->animation_contains_movement_data(m_animation_id);
        short base_weight;
        animation_channel_compute_base_weight(&base_weight, this);
        if (base_weight != t_short_fixed<13>::k_fixed_one)
        {
            if (base_weight == 0)
                contains_motion = false;
            for (long animation_index = 0; animation_index < 2; animation_index++)
            {
                if (m_animation_ids[animation_index].valid() && m_weights[animation_index] > 0)
                    contains_motion |= graph->animation_contains_movement_data(m_animation_ids[animation_index]);
            }
        }
    }
    return contains_motion;
}

// @address 0x2fb608 ?internal_apply_weighted_node_orientations@c_animation_channel@@AAAXVc_animation_id@@MPBUrender_model_definition@@MPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@0PBKM@ZJ@Z
void c_animation_channel::internal_apply_weighted_node_orientations(c_animation_id animation_id, float ratio,
    const render_model_definition *render_model, float blend_ratio, const c_static_flags<255> *node_mask, long node_orientations_count,
    real_orientation *node_orientations,
    void (*animation_function)(long, const c_model_animation_graph *, c_animation_id, const unsigned long *, float), long user_data)
{
    bool use_frame_rate_scale = m_playback_flags.test(_animation_playback_unknown6_bit);
    if (m_playback_flags.test(_animation_playback_autoplay_bit) && m_playback_speed != 0.0f)
    {
        float tolerance = k_animation_channel_tick_tolerance;
        if (!(fabs((float)game_tick_rate() - m_playback_speed * k_animation_channel_tick_scale) < tolerance))
            use_frame_rate_scale = true;
    }
    c_channel_base::internal_apply_weighted_node_orientations(animation_id, ratio, render_model, blend_ratio, node_mask,
        node_orientations_count, node_orientations, 0, user_data, use_frame_rate_scale);
}

// @address 0xe01e0 ?compute_relative_frame_index@@YAMPBVc_model_animation@@0M@Z
// @address 0x2fb1a8 ?apply_weighted_node_orientations@c_animation_channel@@QAAXPBUrender_model_definition@@MPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@Vc_animation_id@@PBKM@ZJ@Z
void c_animation_channel::apply_weighted_node_orientations(const render_model_definition *render_model, float blend_ratio,
    const c_static_flags<255> *node_mask, long node_orientations_count, real_orientation *node_orientations,
    void (*animation_function)(long, const c_model_animation_graph *, c_animation_id, const unsigned long *, float), long user_data)
{
    if (!valid())
        return;
    internal_apply_weighted_node_orientations(
        m_animation_id, m_frame_position, render_model, blend_ratio, node_mask, node_orientations_count, node_orientations, 0, user_data);
    short base_weight;
    animation_channel_compute_base_weight(&base_weight, this);
    if (base_weight == t_short_fixed<13>::k_fixed_one)
        return;
    for (long animation_index = 0; animation_index < 2; animation_index++)
    {
        if (m_animation_ids[animation_index].valid() && m_weights[animation_index] > 0)
        {
            const c_model_animation *weight_animation =
                TAG_GET('jmad', const c_model_animation_graph, m_graph_index)->get_animation(m_animation_ids[animation_index]);
            const c_model_animation *animation = 0;
            if (m_animation_id.valid())
                animation = TAG_GET('jmad', const c_model_animation_graph, m_graph_index)->get_animation(m_animation_id);
            float relative_frame = compute_relative_frame_index(animation, weight_animation, m_frame_position);
            float weight = m_weights[animation_index].export_real();
            internal_apply_weighted_node_orientations(m_animation_ids[animation_index], relative_frame, render_model, weight *blend_ratio,
                node_mask, node_orientations_count, node_orientations, 0, user_data);
        }
    }
}

// @address 0x2fb898 ?get_authored_primary_key_time@c_animation_channel@@QBAMXZ
float c_animation_channel::get_authored_primary_key_time() const
{
    float primary_key_time = -1.0f;
    if (valid())
    {
        const c_model_animation *animation = get_animation();
        long primary_key_frame_index = animation->get_primary_key_frame_index();
        if (primary_key_frame_index != NONE)
            primary_key_time = animation_channel_frames_to_time((float)primary_key_frame_index, get_authored_rate());
    }
    return primary_key_time;
}
// @address 0x2fb998 ?get_playback_duration@c_animation_channel@@QBAMXZ
float c_animation_channel::get_playback_duration() const
{
    float playback_duration = 0.0f;
    if (valid()) {
        const c_model_animation *animation = get_animation();
        float scale = m_playback_speed;
        short frame_count = animation->frame_count;
        float playback_rate = scale * 30.0f;
        playback_duration = animation_channel_frames_to_time((float)frame_count, playback_rate);
    }
    return playback_duration;
}

// @address 0x2fba78 ?get_frames_remaining@c_animation_channel@@QBAMXZ
float c_animation_channel::get_frames_remaining() const
{
    float frames_remaining = 0.0f;
    if (valid()) {
        const c_model_animation *animation = get_animation();
        frames_remaining = (float)(animation->frame_count - 1) - m_frame_position;
    }
    return frames_remaining;
}

// Reset animation references, playback state and blend weights.
void c_animation_channel::reset()
{
  c_animation_channel *state = this;
  state->m_graph_index = -1;
  state->m_definition_index = -1;
  state->m_animation_id.m_graph_index = -1;
  state->m_animation_id.m_animation_index = -1;
  state->m_playback_flags.clear();
  state->m_location = _animation_location_none;
  for (unsigned int i = 0; i < 2; i++) {
    c_animation_id *pair = &state->m_animation_ids[i];
    pair->m_graph_index = -1;
    pair->m_animation_index = -1;
    state->m_weights[i].m_value = 0;
  }
  state->m_event_flags.clear();
  state->m_future_event_flags.clear();
  state->m_past_event_flags.clear();
  state->m_state_flags.clear();
  state->m_playback_speed = k_real_one;
  state->m_frame_position = k_real_zero;
  state->m_playback_ratio = k_real_zero;
}

// @address 0xdcdf0 ?get_channel_settings@c_animation_channel@@QBAXPAVc_animation_channel_settings@@@Z
void c_animation_channel::get_channel_settings(c_animation_channel_settings *settings) const
{
    animation_channel_get_base_channel_settings(this, settings);
    settings->m_playback_ratio = get_playback_ratio();
    settings->m_playback_rate = m_playback_speed;
    settings->m_flags.set(_channel_settings_paused_bit, paused());
    settings->m_flags.set(_channel_settings_blending_locked_bit, animation_channel_analog_blending_weights_locked(this));
    for (long i = 0; i < 2; i++)
    {
        settings->m_blend_ids[i] = m_animation_ids[i];
        settings->m_blend_ratios[i] = m_weights[i].export_real();
    }
}

// match: 0x2fb6b8: inline lets internal_apply_weighted_node_orientations_at_playback_ratio inline paused; out of line the function shrinks 216B -> 184B
#if defined(_XBOX) && !defined(__clang__)
inline bool c_animation_channel::paused() const
#else
bool c_animation_channel::paused() const
#endif
{
    return valid() && !playing();
}

c_animation_id c_animation_channel::get_dominant_animation_id() const
{
  // match: 0xe00e8: retain the original view's volatile weight reads.
  const volatile c_animation_channel *source = this;
  short remaining = 0x2000;
  short best = 0;
  c_animation_id out = m_animation_id;
  if ((unsigned short)m_animation_ids[0].m_animation_index < 0x8000 && greater(source->m_weights[0].m_value, best)) {
    best = source->m_weights[0].m_value;
    remaining -= best;
    out = m_animation_ids[0];
  }
  if ((unsigned short)m_animation_ids[1].m_animation_index < 0x8000 && greater(source->m_weights[1].m_value, best)) {
    best = source->m_weights[1].m_value;
    remaining -= best;
    out = m_animation_ids[1];
  }
  if (greater(remaining, best))
    out = m_animation_id;
  return out;
}

// @address 0xe97a8 ?get_blended_animation_id@c_animation_channel@@QBA?AVc_animation_id@@JPAM@Z
c_animation_id c_animation_channel::get_blended_animation_id(long index, float *weight) const
{
    if (weight)
    {
        if (m_animation_ids[index].valid())
            *weight = m_weights[index].export_real();
        else
            *weight = 0.0f;
    }
    return m_animation_ids[index];
}

// @address 0xed798 ?setup_channel@c_animation_channel@@AAA_NPBVc_animation_channel_settings@@@Z
bool c_animation_channel::setup_channel(const c_animation_channel_settings *settings)
{
    float old_frame_position = m_frame_position;
    reset();
    if (setup_channel_base(settings))
    {
        m_playback_speed = settings->m_playback_rate;
        set_frame_position_from_playback_ratio(settings->m_playback_ratio);
        if ((float)fabs(m_frame_position - old_frame_position) < 0.0001f)
            (this)->set_frame_position(old_frame_position);
        for (long i = 0; i < 2; i++)
        {
            m_animation_ids[i] = settings->m_blend_ids[i];
            if (m_animation_ids[i].valid())
                m_weights[i].import_real(settings->m_blend_ratios[i]);
            else
                m_weights[i] = 0;
        }
        prepare_animation_weights();
        m_state_flags.set(_animation_state_unknown3_bit, true);
        m_state_flags.set(_animation_state_unknown4_bit, true);
        animation_channel_pause_playback(this, settings->m_flags.test(_channel_settings_paused_bit));
        animation_channel_lock_analog_blending_weights(this, settings->m_flags.test(_channel_settings_blending_locked_bit));
    }
    return valid();
}

// @address 0xedc40 ?prepare_animation_weights@c_animation_channel@@AAAXXZ
void c_animation_channel::prepare_animation_weights()
{
    t_short_fixed<13> remaining(t_short_fixed<13>::k_fixed_one);
    for (long i = 0; i < 2; i++) {
        if (m_animation_ids[i].valid()) {
            m_weights[i] = m_weights[i] > remaining ? remaining : m_weights[i];
            remaining -= m_weights[i];
        }
    }
}

extern "C" bool animation_channel_get_blended_animation_data(const c_animation_channel *channel, long index, c_animation_data *out_data)
{
    bool result = false;
    float weight;
    c_animation_id blended_id = channel->get_blended_animation_id(index, &weight);
    if (blended_id.valid())
    {
        const c_model_animation_graph *graph = TAG_GET('jmad', const c_model_animation_graph, channel->m_graph_index);
        if (graph)
            result = graph->get_animation_data(blended_id, out_data);
    }
    return result;
}

extern "C" {
extern "C" void animation_channel_get_pill_offset(const c_animation_data *animation,long frame,real_point3d *position,real_vector3d *delta)
{
 *position=k_real_math_origin3d;
 memcpy(delta,&k_real_math_origin3d,sizeof(*delta));
 if(animation->contains_pill_offset_data()) {
  const real_point3d *pill=(const real_point3d*)animation->get_pill_offset_data();
  *position=pill[frame];
  if(frame+1<animation->m_frame_count) vector_from_points3d(&pill[frame],&pill[frame+1],delta);
 }
}
}

// @address 0x2fe450 animation_data_channel_update_flags
extern "C" void animation_data_channel_update_flags(const c_facial_animation_channel *channel, unsigned char *destination)
{
    if (channel->valid())
    {
        c_animation_id reference = channel->m_animation_ids[0];
        if ((channel->m_state_flags.m_storage >> 3) & 1)
            reference = channel->m_animation_ids[1];
        if (reference.valid())
        {
            const c_model_animation_graph *graph = TAG_GET('jmad', const c_model_animation_graph, channel->m_graph_index);
            if (graph)
            {
                c_animation_data animation_data;
                if (graph->get_animation_data(reference, &animation_data) && animation_data.valid())
                    animation_data_merge_flags(&animation_data, destination);
            }
        }
    }
}


// @address 0xdb468 ??0c_animation_channel@@QAA@XZ
c_animation_channel::c_animation_channel()
{
    initialize();
}

static __forceinline void animation_channel_clear(c_animation_channel *state)
{
    state->m_graph_index = -1;
    state->m_definition_index = -1;
    state->m_animation_id.m_graph_index = -1;
    state->m_animation_id.m_animation_index = -1;
    state->m_playback_flags.clear();
    state->m_location = _animation_location_none;
    for (int i = 0; i < 2; i++) {
        c_animation_id *pair = &state->m_animation_ids[i];
        pair->m_graph_index = -1;
        pair->m_animation_index = -1;
        state->m_weights[i].m_value = 0;
    }
}

// @address 0xdb4c8 ?initialize@c_animation_channel@@QAAXXZ
void c_animation_channel::initialize(void)
{
    m_parent_slider_index = -1;
    m_parent_slider_type = _mixing_board_parent_slider_none;
    m_enabled = true;
    animation_channel_clear(this);
    reset();
}

// 0xdbad8: accepted initialization followed by disabling the channel
// @address 0xdbad8 animation_channel_initialize_and_disable
extern "C" void animation_channel_initialize_and_disable(c_animation_channel *channel)
{
    channel->initialize();
    channel->c_channel_base::initialize();
    channel->m_enabled = false;
}


// @address 0x2fbb38 ?get_playback_time_remaining@c_animation_channel@@QBAMXZ
float c_animation_channel::get_playback_time_remaining() const
{
    // the pooled 30.0f authored animation rate, addressed off the epsilon anchor
    float divisor = m_playback_speed * (&k_real_math_epsilon_storage)[0x199c];
    float value = get_frames_remaining();
    if (divisor <= k_real_math_epsilon_storage)
        return value / k_real_math_epsilon_storage;
    return value / divisor;
}

// Direction selects pending events (1) or handled events (0).
void c_animation_channel::note_all_future_and_past_events(long direction)
{
    m_future_event_flags.clear();
    if (valid()) {
        bool paused = (m_playback_flags.m_storage >> 7) & 1;
        if (!paused) {
        const c_model_animation *animation = get_dominant_animation();
        float frame = m_frame_position;
        if (direction == 1) {
            if (m_playback_speed >= 0.0f) {
                long count = animation->frame_events.begin() ? animation->frame_events.count() : 0;
                for (long i = 0; count > i; i++) {
                    if ((float)animation->frame_events[i].frame > frame)
                        m_future_event_flags.m_storage |= 1 << (short)animation->frame_events[i].type;
                }
            } else {
                long count = animation->frame_events.begin() ? animation->frame_events.count() : 0;
                for (long i = 0; count > i; i++) {
                    if ((float)animation->frame_events[i].frame < frame)
                        m_future_event_flags.m_storage |= 1 << (short)animation->frame_events[i].type;
                }
            }
        } else if (direction == 0) {
            if (m_playback_speed >= 0.0f) {
                long count = animation->frame_events.begin() ? animation->frame_events.count() : 0;
                for (long i = 0; count > i; i++) {
                    if ((float)animation->frame_events[i].frame < frame)
                        m_past_event_flags.m_storage |= 1 << (short)animation->frame_events[i].type;
                }
            } else {
                long count = animation->frame_events.begin() ? animation->frame_events.count() : 0;
                for (long i = 0; count > i; i++) {
                    if ((float)animation->frame_events[i].frame > frame)
                        m_past_event_flags.m_storage |= 1 << (short)animation->frame_events[i].type;
                }
            }
        }
        }
    }
}

static __forceinline short animation_channel_weight_value(const short *weight) { return *weight; }
// match: 0xe2418: __restrict on output colors the second animation-index load r10 (without it c2 uses r11)
extern "C" void animation_channel_compute_base_weight(short *__restrict output, const c_animation_channel *input)
{
 *output = 8192;
 if ((unsigned short)input->m_animation_ids[0].m_animation_index < 0x8000) {
   unsigned int weight = (unsigned short)input->m_weights[0].m_value;
   bool positive = (short)weight > 0;
   if ((unsigned int)positive != 0)
     *output = 8192 - animation_channel_weight_value(&input->m_weights[0].m_value);
 }
 if ((unsigned short)input->m_animation_ids[1].m_animation_index < 0x8000) {
   unsigned int weight = (unsigned short)input->m_weights[1].m_value;
   bool positive = (short)weight > 0;
   if ((unsigned int)positive != 0)
     *output = *output - weight;
 }
}


typedef void (*animation_channel_callback)(long, const c_model_animation_graph *, c_animation_id, const unsigned long *, float);
// @address 0x2fbba0 ?prepare_node_mask_flags@c_channel_base@@QBAPBV?$c_static_flags@$0PP@@@PBUrender_model_definition@@J@Z
// @address 0x301b60 animation_compute_node_orientations

static __forceinline void animation_channel_compute_node_orientations_at_playback_ratio(long graph_index, c_animation_id animation_id,
    long node_count, float playback_ratio, float blend_ratio, const c_static_flags_no_init<255> *node_mask, real_orientation
        *node_orientations)
{
    if (node_count != 0)
    {
        const c_model_animation *animation = TAG_GET('jmad', const c_model_animation_graph, graph_index)->get_animation(animation_id);
        c_animation_data animation_data;
        c_flags<e_anim_computation_flags, unsigned long, 1> computation_flags(1);
        c_flags<e_anim_computation_flags, unsigned long, 1> *computation_flags_pointer = &computation_flags;
        // The incoming mask cannot alias this local flags object.
        // match: 0x301b60: value range assumption guides branch lowering
        __assume(node_mask != (const c_static_flags_no_init<255> *)&computation_flags);
        float frame_position = animation->get_frame_position_from_playback_ratio(playback_ratio);
        animation_compute_node_orientations(graph_index, animation_id, node_count, frame_position, blend_ratio, node_mask,
            node_orientations, *computation_flags_pointer, false, 0.0f, 0.0f);
    }
}

// @address 0x2fb350 ?apply_weighted_blend_screen_node_orientations@c_animation_channel@@QAAXMMMPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@Vc_animation_id@@PBKM@ZJ@Z
void c_animation_channel::apply_weighted_blend_screen_node_orientations(float yaw, float pitch, float weight,
    const c_static_flags<255> *node_mask, long node_orientations_count, real_orientation *node_orientations,
    void (*animation_function)(long, const c_model_animation_graph *, c_animation_id, const unsigned long *, float), long user_data)
{
    if (valid())
    {
        t_short_fixed<13> base_weight;
        animation_channel_compute_base_weight((short *)&base_weight, this);
        float base_weight_ratio = base_weight.export_real();
        internal_apply_weighted_blend_screen_node_orientations(
            m_animation_id, yaw, pitch, base_weight_ratio *weight, node_mask, node_orientations_count, node_orientations, 0, user_data);
        if (base_weight.m_value != t_short_fixed<13>::k_fixed_one)
        {
            for (long animation_index = 0; animation_index < 2; animation_index++)
            {
                if (m_animation_ids[animation_index].valid() && m_weights[animation_index] > 0)
                {
                    float blend_weight = m_weights[animation_index].export_real();
                    internal_apply_weighted_blend_screen_node_orientations(m_animation_ids[animation_index], yaw, pitch,
                        blend_weight *weight, node_mask, node_orientations_count, node_orientations, 0, user_data);
                }
            }
        }
    }
}

// @address 0x2fb4b0 ?apply_weighted_node_orientations_at_playback_ratio@c_animation_channel@@QBAXPBUrender_model_definition@@MMPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@Vc_animation_id@@PBKM@ZJ@Z
void c_animation_channel::apply_weighted_node_orientations_at_playback_ratio(const render_model_definition *render_model,
    float playback_ratio, float blend_ratio, const c_static_flags<255> *node_mask, long node_orientations_count,
    real_orientation *node_orientations,
    void (*animation_function)(long, const c_model_animation_graph *, c_animation_id, const unsigned long *, float), long user_data) const
{
    if (valid())
    {
        t_short_fixed<13> base_weight;
        animation_channel_compute_base_weight((short *)&base_weight, this);
        float base_weight_ratio = base_weight.export_real();
        internal_apply_weighted_node_orientations_at_playback_ratio(m_animation_id, render_model, playback_ratio,
            base_weight_ratio *blend_ratio, node_mask, node_orientations_count, node_orientations, 0, user_data);
        if (base_weight.m_value != t_short_fixed<13>::k_fixed_one)
        {
            for (long animation_index = 0; animation_index < 2; animation_index++)
            {
                if (m_animation_ids[animation_index].valid() && m_weights[animation_index] > 0)
                {
                    float weight = m_weights[animation_index].export_real();
                    internal_apply_weighted_node_orientations_at_playback_ratio(m_animation_ids[animation_index], render_model,
                        playback_ratio, weight *blend_ratio, node_mask, node_orientations_count, node_orientations, 0, user_data);
                }
            }
        }
    }
}

// @address 0x2fb6b8 ?internal_apply_weighted_node_orientations_at_playback_ratio@c_animation_channel@@ABAXVc_animation_id@@PBUrender_model_definition@@MMPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@0PBKM@ZJ@Z
void c_animation_channel::internal_apply_weighted_node_orientations_at_playback_ratio(c_animation_id animation_id,
    const render_model_definition *render_model, float playback_ratio, float blend_ratio, const c_static_flags<255> *node_mask,
    long node_orientations_count, real_orientation *node_orientations, animation_channel_callback animation_function, long user_data) const
{
    // match: 0x2fb6b8: pointer __restrict qualifier prevents alias reload
    const c_static_flags<255> *__restrict channel_node_mask = prepare_node_mask_flags(render_model, node_orientations_count);
    c_static_flags_no_init<255> combined_node_mask;
    const c_static_flags_no_init<255> *effective_node_mask;
    if (combine_node_flag_masks((const c_static_flags_no_init<255> *)channel_node_mask, (const c_static_flags_no_init<255> *)node_mask,
        &combined_node_mask))
        effective_node_mask = &combined_node_mask;
    else
        effective_node_mask = 0;
    animation_channel_compute_node_orientations_at_playback_ratio(
        m_graph_index, animation_id, node_orientations_count, playback_ratio, blend_ratio, effective_node_mask, node_orientations);
}

// @address 0x2fb790 ?internal_apply_weighted_blend_screen_node_orientations@c_animation_channel@@AAAXVc_animation_id@@MMMPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@0PBKM@ZJ@Z
void c_animation_channel::internal_apply_weighted_blend_screen_node_orientations(c_animation_id animation_id, float yaw, float pitch,
    float weight, const c_static_flags<255> *static_flags, long node_count, real_orientation *orientations,
    animation_channel_callback animation_function, long user_data)
{
    if (!valid())
        return;
    const c_model_animation_graph *graph = TAG_GET('jmad', const c_model_animation_graph, m_graph_index);
    const animation_aiming_screen_bounds *bounds = graph->get_blend_screen_bounds(m_animation_id);
    if (!bounds)
        return;
    const c_static_flags<255> *channel_node_mask = prepare_node_mask_flags(0, node_count);
    c_static_flags_no_init<255> combined_node_mask;
    const c_static_flags_no_init<255> *effective_node_mask;
    if (combine_node_flag_masks((const c_static_flags_no_init<255> *)channel_node_mask, (const c_static_flags_no_init<255>
        *)static_flags, &combined_node_mask))
        effective_node_mask = &combined_node_mask;
    else
        effective_node_mask = 0;
    compute_node_orientations_from_blend_screen(m_graph_index, animation_id, node_count, yaw, pitch,
        (const s_animation_compute_orientations_aiming_screen_bounds *)bounds, weight, effective_node_mask, orientations);
}

static __forceinline const c_model_animation *animation_channel17_get_blended_animation(
    const c_animation_channel *channel, long index)
{
    const c_model_animation *animation = 0;
    if (channel->m_animation_ids[index].valid())
        animation = TAG_GET('jmad', const c_model_animation_graph, channel->m_graph_index)->get_animation(channel->m_animation_ids[index]);
    return animation;
}
// @address 0xe1da0 ?get_pill_offset_data_for_frame@c_animation_channel@@ABA_NJPATreal_point3d@@PAM@Z
// match: 0xe1da0: pointer __restrict qualifier prevents alias reload
bool c_animation_channel::get_pill_offset_data_for_frame(long frame_index, real_point3d *position, float *__restrict position_delta) const
{
    const c_animation_channel *self = this;
    position->x = 0.0f;
    position->y = 0.0f;
    position->z = 0.0f;
    bool result = false;
    *position_delta = 0.0f;
    if (self->valid())
    do
    {
        {
            c_animation_data data;
            data.m_data = 0;
            data.m_sizes = 0;
            data.m_node_count = 0;
            data.m_movement_data_type = 0;
            if (!animation_definitions_try_and_get_data((const c_channel_base *)self, &data))
                break;
            result = true;
            data.get_pill_offset_for_frame(frame_index, position, (real_vector3d *)position_delta);
        }
        {
            t_short_fixed<13> base_weight;
            animation_channel_compute_base_weight(&base_weight.m_value, self);
            if (base_weight.m_value == t_short_fixed<13>::k_fixed_one)
                break;
            float scale = animation_channel16_export_weight(&base_weight);
            animation_channel16_scale_point3d(position, scale, position);
            *position_delta = *position_delta * scale;
        }
            const c_model_animation *animation = self->get_animation();
            for (long i = 0; i < 2; i++)
            {
                if (self->m_animation_ids[i].valid() && self->m_weights[i] > 0)
                {
                    const c_model_animation *blended_animation = animation_channel17_get_blended_animation(self, i);
                    float frame_ratio = (float)blended_animation->frame_count / (float)animation->frame_count;
                    float blend_weight = animation_channel16_export_weight(&self->m_weights[i]);
                    long blended_frame_index = (long)compute_relative_frame_index(animation, blended_animation, (float)frame_index);
                    c_animation_data blended_data;
                    blended_data.m_data = 0;
                    blended_data.m_sizes = 0;
                    blended_data.m_node_count = 0;
                    blended_data.m_movement_data_type = 0;
                    if (animation_channel_get_blended_animation_data(self, i, &blended_data))
                    {
                        real_point3d blended_position;
                        float blended_delta;
                        blended_data.get_pill_offset_for_frame(blended_frame_index, &blended_position, (real_vector3d *)&blended_delta);
                        float blend_scale = frame_ratio * blend_weight;
                        *position_delta = blended_delta * blend_weight * frame_ratio + *position_delta;
                        animation_channel16_scale_point3d(&blended_position, blend_scale, &blended_position);
                        animation_channel16_add_points3d(position, &blended_position, position);
                    }
                }
            }
    } while (false);
    return result;
}


// @address 0x2faf30 ?get_motion_data@c_animation_channel@@QBAXPATreal_vector3d@@PAM@Z
// match: 0x2faf30: pointer __restrict qualifier prevents alias reload
void c_animation_channel::get_motion_data(real_vector3d *vector, float *__restrict scalar) const
{
    *vector = *global_zero_vector3d;
    *scalar = 0.0f;
    bool valid_channel = m_enabled != 0 && m_graph_index != -1 && (unsigned short)m_animation_id.m_animation_index < 0x8000;
    if (valid_channel) {
        float scale = m_playback_speed * 30.0f;
        real_point3d direction;
        float spin;
        get_pill_offset_data_for_frame((int)m_frame_position, &direction, &spin);
        vector->i = direction.x * scale;
        vector->j = direction.y * scale;
        vector->k = direction.z * scale;
        *scalar = spin * scale;
    }
}

// @address 0x2251c k_real_zero_storage
// @address 0x2fb030 ?get_motion_offsets@c_animation_channel@@QBAXPATreal_vector3d@@PAM@Z
// match: 0x2fb030: pointer __restrict qualifier prevents alias reload
void c_animation_channel::get_motion_offsets(union real_vector3d * __restrict translation, float * __restrict yaw) const
{
    memcpy(translation, &k_real_math_origin3d, sizeof(*translation));
    *yaw = k_real_zero_storage;
    if (this->valid())
    {
        real_point3d offset;
        float yaw_offset;
        this->get_pill_offset_data_for_frame((long)this->m_frame_position, &offset, &yaw_offset);
        translation->i = offset.x * this->m_playback_speed;
        translation->j = offset.y * this->m_playback_speed;
        translation->k = offset.z * this->m_playback_speed;
        *yaw = yaw_offset * this->m_playback_speed;
    }
}


// @rdata 0x82033bc0 animation_channel_default_blend
static const float animation_channel_default_blend = 0.25f;
// @rdata 0x820221fc animation_channel_blend_one
static const float animation_channel_blend_one = 1.0f;
// @rdata 0x8202251c animation_channel_blend_zero
static const float animation_channel_blend_zero = 0.0f;
// @address 0x7c8894 ?s_real_one@?1??import_real@?$t_short_fixed@$0N@@@QAAXM@Z@4MB
// @address 0x7c8898 ??_B?1??import_real@?$t_short_fixed@$0N@@@QAAXM@Z@51
// @address 0xe9398 ?update_analog_blend_from_throttle@c_animation_channel@@QAA_NPBTreal_vector3d@@M_N@Z
bool c_animation_channel::update_analog_blend_from_throttle(const real_vector3d *throttle, float blend_ratio, bool limited)
{
    bool result = false;
    bool ready = this->valid();
    if (ready) {
        const c_model_animation *animation = get_animation();
        if (animation && animation->movement_type) {
            float zero = 0.0f;
            float weight = MIN(MAX(dot_product3d(throttle, &animation->motion_direction), 0.0f), 1.0f);
            if (this->m_animation_ids[0].valid()) {
                real_vector3d velocity;
                __int64 ticks = (__int64)game_tick_rate();
                scale_vector3d(&animation->motion_direction, (float)(double)ticks * animation->motion_distance *
                    weight, &velocity);
                result = animation_channel_update_blend_from_velocity(this, &velocity, blend_ratio, limited);
            } else if (!animation_channel_analog_blending_weights_locked(this)) {
                float rate = 0.25f;
                float playback = weight * 0.2f + 0.8f;
                float previous_playback = this->m_playback_speed;
                if (blend_ratio > zero)
                    rate = blend_ratio;
                playback = (playback - previous_playback) * rate + previous_playback;
                if (!(playback > 0.0001f))
                    playback = 0.0001f;
                this->m_playback_speed = playback;
                result = true;
            }
        }
    }
    if (!result)
        this->m_weights[0] = 0;
    return result;
}
// @address 0xe95c8 animation_channel_update_blend_from_velocity
extern "C" bool animation_channel_update_blend_from_velocity(c_animation_channel *channel, const real_vector3d *velocity, float
    blend_ratio, bool limited)
{
    bool result = false;
    bool ready = channel->valid();
    if (ready) {
        const c_model_animation *animation = channel->get_animation();
        if (animation && animation->movement_type && channel->m_animation_ids[0].valid()) {
            result = true;
            if (!animation_channel_analog_blending_weights_locked(channel)) {
                float ratio = animation_channel_default_blend;
                if (blend_ratio > animation_channel_blend_zero) {
                    ratio = animation_channel_blend_one;
                    if (blend_ratio < animation_channel_blend_one)
                        ratio = blend_ratio;
                }
                float blend;
                float playback;
                float weight;
                const c_model_animation_graph *graph = TAG_GET('jmad', const c_model_animation_graph, channel->m_graph_index);
                c_animation_id primary = channel->m_animation_id;
                animation_definitions_compute_blend_and_playback(
                    graph, primary, channel->get_blended_animation_id(0, &weight), velocity, limited, &blend, &playback);
                float previous_weight = channel->m_weights[0].export_real();
                channel->m_weights[0].import_real((blend - previous_weight) * ratio + previous_weight);
                channel->m_weights[1] = 0;
                float playback_rate = (playback - channel->m_playback_speed) * ratio + channel->m_playback_speed;
                if (!(playback_rate > 0.0001f))
                    playback_rate = 0.0001f;
                channel->m_playback_speed = playback_rate;
            }
        }
    }
    if (!result)
        channel->m_weights[0] = 0;
    return result;
}


// @address 0xdf8c0 ?update_to_frame_position@c_animation_channel@@QAA_NMPBVc_animation_manager@@P6AXPBUs_animation_event_data@@J@ZJ@Z
bool c_animation_channel::update_to_frame_position(
    float frame_position, const c_animation_manager *manager, void (*callback)(const s_animation_event_data *, long), long user_data)
{
    const c_model_animation_graph *graph = TAG_GET('jmad', const c_model_animation_graph, m_graph_index);
    const c_model_animation_graph *subgraph = 0;
    const c_model_animation *primary_animation = 0;
    c_flags<e_animation_playback_flag_bits, unsigned short, 16> playback_flags = m_playback_flags;
    c_animation_id animation_id = get_animation_id();
    graph->get_graph_and_animation(animation_id, &subgraph, &primary_animation);

    float start_frame = MAX(0.0f, m_frame_position);
    float end_frame = MAX(0.0f, frame_position);
    float frame_count = (float)primary_animation->get_frame_count();
    float last_frame = frame_count - 1.0f;
    if (m_definition_index == 182 && playing() && playback_flags.test(_animation_playback_looping_bit) && m_playback_speed < 0.95f)
    {
        frame_count = last_frame;
        last_frame -= 1.0f;
    }

    c_animation_id dominant_animation_id = get_dominant_animation_id();
    const c_model_animation *dominant_animation = 0;
    const c_model_animation_graph *dominant_subgraph = 0;
    graph->get_graph_and_animation(dominant_animation_id, &dominant_subgraph, &dominant_animation);
    float relative_start_frame = compute_relative_frame_index(primary_animation, dominant_animation, start_frame);
    float relative_end_frame = compute_relative_frame_index(primary_animation, dominant_animation, end_frame);

    note_all_future_and_past_events(0);
    m_past_event_flags |= m_event_flags;
    m_event_flags = c_flags<e_animation_frame_event_types, unsigned short, 13>(0);
    m_state_flags.set(_animation_state_unknown4_bit, false);

    if (end_frame - start_frame != 0.0f || m_state_flags.test(_animation_state_unknown3_bit))
    {
        m_state_flags.set(_animation_state_unknown4_bit, true);
        m_state_flags.set(_animation_state_unknown3_bit, false);
        m_state_flags.set(_animation_state_finished_bit, false);
        m_state_flags.set(_animation_state_stopped_bit, false);
        if (start_frame >= last_frame)
        {
            m_playback_ratio = 1.0f;
            m_state_flags.set(_animation_state_finished_bit, true);
            m_state_flags.set(_animation_state_stopped_bit, !animation_channel_looping_enabled(this));
        }

        unsigned long events = check_animation_frame_range_for_events(
            dominant_subgraph, dominant_animation, &playback_flags, relative_start_frame, relative_end_frame, callback, user_data);
        if (animation_id == get_animation_id())
        {
            m_event_flags |= c_flags<e_animation_frame_event_types, unsigned short, 13>((unsigned short)events);
            m_frame_position = end_frame;
            m_playback_ratio = primary_animation->get_playback_ratio_from_frame_position(m_frame_position);
            if (m_frame_position >= frame_count)
            {
                m_state_flags.set(_animation_state_finished_bit, true);
                m_frame_position = last_frame;
                m_playback_ratio = 1.0f;
                events = check_animation_frame_range_for_events(
                    dominant_subgraph, dominant_animation, &playback_flags, relative_end_frame, k_real_max, callback, user_data);
                bool unchanged = animation_id == get_animation_id() && dominant_animation_id == get_dominant_animation_id();
                if (unchanged)
                {
                    m_event_flags |= c_flags<e_animation_frame_event_types, unsigned short, 13>((unsigned short)events);
                    bool looping = animation_channel_looping_enabled(this);
                    if (!looping)
                    {
                        set_frame_position(last_frame);
                        if (callback)
                        {
                            s_animation_event_data event_data;
                            event_data.event_type = 4;
                            event_data.frame = (long)last_frame;
                            callback(&event_data, user_data);
                        }
                    }
                    else
                    {
                        bool loop = true;
                        if (callback)
                        {
                            s_animation_event_data event_data;
                            event_data.event_type = 5;
                            event_data.frame = (long)last_frame;
                            callback(&event_data, user_data);
                            bool changed = animation_id != get_animation_id() || dominant_animation_id != get_dominant_animation_id() ||
                                looping != animation_channel_looping_enabled(this);
                            if (changed)
                                loop = false;
                        }
                        if (loop)
                        {
                            float overshoot = end_frame - frame_count;
                            m_past_event_flags.clear();
                            m_future_event_flags.clear();
                            float loop_frame = (float)primary_animation->get_loop_frame_index();
                            if (!playback_flags.test(_animation_playback_unknown5_bit) && loop_frame == 0.0f)
                            {
                                animation_channel_set_animation_id(this, animation_manager_resolve_animation_id(manager,
                                    get_animation_id()));
                                m_frame_position = 0.0f;
                                m_playback_ratio = 0.0f;
                                animation_id = get_animation_id();
                                graph->get_graph_and_animation(animation_id, &subgraph, &primary_animation);
                                last_frame = (float)primary_animation->get_frame_count() - 1.0f;
                                dominant_animation_id = get_dominant_animation_id();
                                dominant_animation = 0;
                                dominant_subgraph = 0;
                                graph->get_graph_and_animation(dominant_animation_id, &dominant_subgraph, &dominant_animation);
                            }
                            m_frame_position = MIN(MAX(loop_frame + overshoot, 0.0f), last_frame);
                            m_playback_ratio = primary_animation->get_playback_ratio_from_frame_position(m_frame_position);
                            events = check_animation_frame_range_for_events(
                                dominant_subgraph, dominant_animation, &playback_flags, loop_frame, m_frame_position, callback, user_data);
                            unchanged = animation_id == get_animation_id() && dominant_animation_id == get_dominant_animation_id();
                            if (unchanged)
                                m_event_flags |= c_flags<e_animation_frame_event_types, unsigned short, 13>((unsigned short)events);
                        }
                    }
                }
            }
        }
    }
    else if (start_frame >= last_frame)
    {
        m_frame_position = m_frame_position > frame_count - 0.0001f ? frame_count - 0.0001f : m_frame_position;
        m_playback_ratio = 1.0f;
        m_state_flags.set(_animation_state_finished_bit, true);
        m_state_flags.set(_animation_state_stopped_bit, !animation_channel_looping_enabled(this));
    }

    note_all_future_and_past_events(1);
    return m_state_flags.test(_animation_state_unknown4_bit);
}

// @address 0xfe390 ?update@c_animation_channel@@QAA_NPBVc_animation_manager@@P6AXPBUs_animation_event_data@@J@ZJ@Z
bool c_animation_channel::update(const c_animation_manager *manager, void (*callback)(const s_animation_event_data *, long), long user_data)
{
    bool result = false;
    m_past_event_flags |= m_event_flags;
    m_event_flags = c_flags<e_animation_frame_event_types, unsigned short, 13>(0);
    if (playing())
        result = update_to_frame_position(m_frame_position + animation_channel_get_frame_position_step(this), manager, callback, user_data);
    return result;
}
