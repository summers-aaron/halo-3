#ifndef ANIMATIONS_MIXING_BOARD_CHANNELS_ANIMATION_CHANNEL_H
#define ANIMATIONS_MIXING_BOARD_CHANNELS_ANIMATION_CHANNEL_H

class c_animation_channel;
class c_animation_data;
union real_vector3d;

extern "C" bool animation_channel_get_blended_animation_data(const c_animation_channel *channel, long index, c_animation_data *out_data);
extern "C" void animation_channel_compute_base_weight(short *output, const c_animation_channel *input);
extern "C" bool animation_channel_update_blend_from_velocity(
    c_animation_channel *channel,
    const real_vector3d *velocity,
    float blend_ratio,
    bool limited);
extern "C" void animation_channel_initialize_and_disable(c_animation_channel *channel);

#include "animations/mixing_board/channels/channel_base.h"
#include "math/short_fixed.h"

class c_animation_manager;
struct s_animation_event_data;
struct real_matrix4x3;
union real_point3d;

enum e_animation_frame_event_types {
    _animation_frame_event_type_primary_keyframe,
    _animation_frame_event_type_left_foot = 2,
    _animation_frame_event_type_right_foot = 3,
    _animation_frame_event_type_4 = 4,
    _animation_frame_event_type_both_feet_shuffle = 9,
    _animation_frame_event_type_body_impact = 10,
};

enum e_animation_state_flag_bits {
    _animation_state_paused_bit,
    _animation_state_finished_bit,
    _animation_state_stopped_bit,
    // Set by seeks and setups, consumed by update_to_frame_position to force an update with no frame advance.
    _animation_state_unknown3_bit,
    // Set when update_to_frame_position processes frames; returned by update.
    _animation_state_unknown4_bit,
    _animation_state_blending_locked_bit = 5,
};

enum e_animation_channel_settings_flags {
    _channel_settings_paused_bit,
    _channel_settings_blending_locked_bit,
};

typedef void (*animation_event_callback)(const s_animation_event_data *, long);

class c_animation_channel_settings : public c_base_channel_settings {
public:
    c_animation_channel_settings();
    void clear();
    c_flags<e_channel_settings_comparison_bits, unsigned long, 10> compare_settings(const c_animation_channel_settings &other) const;

    c_flags<e_animation_channel_settings_flags, long, 2> m_flags;
    float m_playback_ratio;
    float m_playback_rate;
    c_animation_id m_blend_ids[2];
    float m_blend_ratios[2];
};
typedef char animation_channel_settings_size[(sizeof(c_animation_channel_settings) == 44) ? 1 : -1];

// Plays one animation on the mixing board, with up to two blended animations.
class c_animation_channel : public c_channel_base {
public:
    bool update_analog_blend_from_throttle(const real_vector3d *throttle, float blend_ratio, bool limited);
    c_animation_channel();
    void initialize();
    void reset();

    bool paused() const;
    bool playing() const;
    __forceinline bool reached_last_frame() const { return m_state_flags.test(_animation_state_finished_bit); }
    __forceinline bool playback_complete() const { return m_state_flags.test(_animation_state_stopped_bit); }
    __forceinline void set_playback_complete()
    {
        access_playback_flags().set(_animation_playback_autoplay_bit, false);
        enable_looping(false);
        m_state_flags.set(_animation_state_stopped_bit, true);
        m_state_flags.set(_animation_state_paused_bit, true);
    }

    __forceinline const c_flags<e_animation_frame_event_types, unsigned short, 13> &get_event_flags() const { return m_event_flags; }
    __forceinline const c_flags<e_animation_frame_event_types, unsigned short, 13> &get_future_event_flags() const { return m_future_event_flags; }
    __forceinline const c_flags<e_animation_frame_event_types, unsigned short, 13> &get_past_event_flags() const { return m_past_event_flags; }

    void set_playback_speed(float playback_speed);
    __forceinline float get_playback_speed() const { return m_playback_speed; }
    __forceinline float get_frame_position() const { return m_frame_position; }
    __forceinline float get_authored_time() const { return m_frame_position / get_authored_rate(); }
    __forceinline long get_frame_index() const { return (long)m_frame_position; }
    __forceinline float get_playback_ratio() const
    {
        float playback_ratio = 0.0f;
        if (valid())
            playback_ratio = m_playback_ratio;
        return playback_ratio;
    }
    float get_playback_ratio_remaining() const;
    float get_playback_duration() const;
    float get_playback_time_remaining() const;
    float get_frames_remaining() const;
    float get_authored_primary_key_time() const;

    void get_channel_settings(c_animation_channel_settings *settings) const;
    c_animation_id get_blended_animation_id(long index, float *weight) const;
    c_animation_id get_dominant_animation_id() const;
    const c_model_animation *get_dominant_animation() const;
    bool contains_motion_data() const;
    void get_motion_data(real_vector3d *velocity, float *yaw_rate) const;
    void get_motion_offsets(real_vector3d *velocity, float *yaw_rate) const;
    bool get_motion_data_for_frame(float frame, real_point3d *position, real_vector3d *delta) const;
    bool get_pill_offset_data(real_point3d *position, real_vector3d *delta) const;

    void set_frame_position(float frame_position);
    void set_frame_position_from_playback_ratio(float playback_ratio);
    void enable_looping(bool enable);
    void jump_to_last_frame_and_stop();

    bool update(const c_animation_manager *manager, animation_event_callback event_callback, long event_data);
    bool update_to_frame_position(float frame_position, const c_animation_manager *manager, animation_event_callback event_callback, long event_data);
    bool update_to_playback_ratio(float playback_ratio, const c_animation_manager *manager, animation_event_callback event_callback, long event_data);

    void apply_weighted_node_orientations(
        const render_model_definition *render_model,
        float blend_ratio,
        const c_static_flags<255> *node_mask,
        long node_orientations_count,
        real_orientation *node_orientations,
        animation_node_orientations_callback animation_function,
        long user_data);
    void apply_weighted_node_orientations_at_playback_ratio(
        const render_model_definition *render_model,
        float playback_ratio,
        float blend_ratio,
        const c_static_flags<255> *node_mask,
        long node_orientations_count,
        real_orientation *node_orientations,
        animation_node_orientations_callback animation_function,
        long user_data) const;
    void apply_weighted_blend_screen_node_orientations(
        float yaw,
        float pitch,
        float weight,
        const c_static_flags<255> *node_mask,
        long node_orientations_count,
        real_orientation *node_orientations,
        animation_node_orientations_callback animation_function,
        long user_data);

private:
    bool setup_channel(const c_animation_channel_settings *settings);
    void prepare_animation_weights();
    void note_all_future_and_past_events(long direction);
    bool get_pill_offset_data_for_frame(long frame_index, real_point3d *position, float *position_delta) const;
    t_short_fixed<13> compute_base_animation_weight() const;
    __forceinline float get_playback_rate() const { float playback_rate = m_playback_speed * get_authored_rate(); return playback_rate; }
    void internal_apply_weighted_node_orientations(
        c_animation_id animation_id,
        float frame,
        const render_model_definition *render_model,
        float blend_ratio,
        const c_static_flags<255> *node_mask,
        long node_orientations_count,
        real_orientation *node_orientations,
        animation_node_orientations_callback animation_function,
        long user_data);
    void internal_apply_weighted_node_orientations_at_playback_ratio(
        c_animation_id animation_id,
        const render_model_definition *render_model,
        float playback_ratio,
        float blend_ratio,
        const c_static_flags<255> *node_mask,
        long node_orientations_count,
        real_orientation *node_orientations,
        animation_node_orientations_callback animation_function,
        long user_data) const;
    void internal_apply_weighted_blend_screen_node_orientations(
        c_animation_id animation_id,
        float yaw,
        float pitch,
        float weight,
        const c_static_flags<255> *node_mask,
        long node_orientations_count,
        real_orientation *node_orientations,
        animation_node_orientations_callback animation_function,
        long user_data);

    friend class c_animation_manager;
    // Prediction samples the private pill-offset path.
    friend bool giant_solve_future_marker_position(long giant_index, long marker_index, long frame, real_matrix4x3 *matrix);
    friend bool animation_manager_setup_on_graph_change(
        const c_animation_manager *manager,
        c_animation_channel *channel,
        c_animation_channel_settings *settings);
    friend bool vehicles_setup_suspension_settings(
        c_animation_channel *channel, c_animation_channel_settings *settings, long graph_index);

public:
    float m_playback_speed;
    float m_frame_position;
    float m_playback_ratio;
    c_animation_id m_animation_ids[2];
    t_short_fixed<13> m_weights[2];
    c_flags<e_animation_frame_event_types, unsigned short, 13> m_event_flags;
    c_flags<e_animation_frame_event_types, unsigned short, 13> m_future_event_flags;
    c_flags<e_animation_frame_event_types, unsigned short, 13> m_past_event_flags;
    c_flags<e_animation_state_flag_bits, unsigned char, 6> m_state_flags;
};
typedef char animation_channel_size[(sizeof(c_animation_channel) == 52) ? 1 : -1];

#endif
