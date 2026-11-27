#ifndef ANIMATIONS_ANIMATION_MANAGER_H
#define ANIMATIONS_ANIMATION_MANAGER_H

class c_map_variant;
struct real_matrix4x3;
class c_animation_manager;

extern "C" short *animation_manager_find_acceleration_overlay_id(short *out, const c_animation_manager *request, int mode);
extern "C" void map_variant_ensure_required_objects(c_map_variant *map_variant);
extern "C" void animation_manager_apply_vehicle_ik(long object_index, long marker_name, long ik_index, real_matrix4x3 *matrix);

class c_animation_channel;
class c_animation_manager;
class c_model_animation;
union real_vector3d;

extern "C" const c_model_animation *animation_manager_get_auxilary_animation(
    const c_animation_manager *manager,
    long weapon_class,
    long weapon_type,
    long impulse);
extern "C" bool animation_manager_has_acceleration_overlay(const c_animation_manager *manager);
extern "C" float animation_channel_get_playback_ratio(const c_animation_channel *channel);
extern "C" bool animation_manager_compute_wrapped_range_fraction(float value, float start, float end, float length, float *fraction);
extern "C" bool animation_manager_update_interpolator(c_animation_manager *manager);
extern "C" bool animation_manager_update_analog_blending(
    c_animation_manager *manager,
    long object_index,
    const real_vector3d *velocity,
    float blend_ratio,
    bool limited);

#include "animations/animation_interpolation.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "math/real_math_types.h"

class c_aiming_channel;
class c_aiming_channel_settings;
class c_auxilary_channel;
class c_auxilary_channel_settings;
class c_facial_animation_channel;
class c_mixing_board_slider;
class suspension_animation_iterator;
class object_overlay_iterator;
struct s_animation_state_change_settings;

enum e_goal_settings_comparison_bits {
    _comparison_goal_field_0_bit,
    _comparison_goal_field_1_bit,
    _comparison_goal_field_2_bit,
    _comparison_goal_field_3_bit,
};

enum e_foot_tracking_state {
    _foot_tracking_none = 0,
    _foot_tracking_plant = 1,
    _foot_tracking_on = 2,
    _foot_tracking_unplant = 3
};

struct s_animation_goal_settings {
    c_flags<e_goal_settings_comparison_bits, unsigned long, 4> compare_goals(const s_animation_goal_settings &other) const;

    long mode;
    long weapon_class;
    long weapon_type;
    long state;
};
typedef char animation_goal_settings_size[(sizeof(s_animation_goal_settings) == 16) ? 1 : -1];

// Controls an object's animation state and the mixing board allocated after it.
class c_animation_manager {
public:
    c_animation_manager();
    ~c_animation_manager();

    void initialize(long object_index, long mixing_board_type);
    bool reset_graph(long graph_tag_index, long model_tag_index, bool keep);
    bool build_model_mapping_table(long model_tag_index, long *node_count, long *mapping, long maximum_node_count);
    bool valid() const;

    __forceinline bool valid_graph() const { return m_graph_tag_index != -1; }
    __forceinline bool valid_goals() const { return (m_flags & 0x40) != 0; }
    __forceinline bool paused() const { return (m_flags & 1) != 0; }
    __forceinline void pause_playback(bool paused)
    {
        if (paused)
            m_flags |= 1;
        else
            m_flags &= ~1;
    }
    __forceinline bool get_randomness_affects_game_state() const { return (m_flags & 2) != 0; }
    __forceinline bool interpolation_discouraged() const { return (m_flags & 0x10) != 0; }
    __forceinline bool awaiting_transition() const { return (m_transition_flags & 1) != 0; }
    __forceinline bool transition_underway() const { return m_transition_channel.valid(); }
    __forceinline long get_graph_tag_index() const { return m_graph_tag_index; }
    __forceinline long get_mode_name() const { return m_current_goal.mode; }
    __forceinline long get_weapon_class_name() const { return m_current_goal.weapon_class; }
    __forceinline long get_weapon_type_name() const { return m_current_goal.weapon_type; }
    __forceinline long get_state_name() const { return m_current_goal.state; }
    __forceinline void reset_weapon()
    {
        m_current_goal.weapon_class = -1;
        m_current_goal.weapon_type = -1;
    }
    __forceinline const s_animation_goal_settings *get_previous_goal() const { return &m_previous_goal; }
    __forceinline const c_animation_channel *get_state_channel() const { return &m_state_channel; }
    __forceinline c_interpolator_control *animation_interpolator() { return &m_interpolator; }
#if !defined(__clang__)
    __forceinline long get_mixing_board_address() const { return (long)this + 176; }
#endif
    __forceinline unsigned char *get_mixing_board_buffer() const
    {
        return reinterpret_cast<unsigned char *>(const_cast<c_animation_manager *>(this) + 1);
    }
    __forceinline c_animation_id find_goal_animation_id(long mode, long weapon_class, long weapon_type, long state, long flags) const
    {
        c_animation_id animation_id;
        s_animation_goal_settings current_goal;
        s_animation_goal_settings previous_goal;
        internal_validate_goal_state(mode, weapon_class, weapon_type, state, flags, &current_goal, &previous_goal, &animation_id);
        return animation_id;
    }
    __forceinline const c_model_animation *get_state_animation() const { return m_state_channel.get_animation(); }
    __forceinline const c_animation_channel *get_active_channel() const
    {
        const c_animation_channel *channel = &m_state_channel;
        if (m_transition_channel.valid())
            channel = &m_transition_channel;
        return channel;
    }
    const c_model_animation *get_active_animation() const;
    __forceinline c_animation_id get_active_animation_id() const { return get_active_channel()->get_animation_id(); }

    const c_model_animation_graph *get_graph() const;
    const c_model_animation_graph *get_subgraph(c_animation_id animation_id) const;
    c_animation_id find_animation(long name) const;
    long find_node(long name) const;

    bool find_goal(long mode, long weapon_class, long weapon_type, long state, long flags) const;
    bool set_goal(long mode, long weapon_class, long weapon_type, long state, long flags, unsigned long playback_flags);
    bool set_custom_goal(c_animation_id animation_id, long flags);
    bool recommend_state_change(
        long mode,
        long weapon_class,
        long weapon_type,
        long state,
        long flags,
        unsigned long playback_flags,
        s_animation_state_change_settings *settings) const;
    bool execute_state_change(const s_animation_state_change_settings *settings);
    void get_current_settings(s_animation_state_change_settings *settings) const;

    c_animation_id find_state_animation_id(long weapon_class, long weapon_type, long state) const;
    c_animation_id find_parallel_impulse_animation_id(long weapon_class, long weapon_type, long impulse) const;
    c_animation_id find_impulse_or_state_animation_id(long weapon_class, long weapon_type, long name) const;
    c_animation_id find_death_or_damage_animation_id(
        long mode,
        long damage,
        short desired_direction,
        short desired_region,
        short *actual_direction,
        short *actual_region) const;
    c_animation_id locate_blend_animation(long mode, long weapon_class, long weapon_type, long blend_state_name, long *blend_state) const;
    c_animation_id locate_transition_animation(long mode, long from_state, long next_mode, long next_state, long *frame_delay) const;
    c_animation_id locate_new_animation_id_on_graph_change(
        c_animation_id previous,
        long key,
        e_animation_graph_location location,
        long weapon_class,
        long weapon_type) const;

    bool setup_animation_channel_by_index(c_animation_channel *channel, c_animation_id animation_id, unsigned long playback_flags) const;
    bool setup_animation_channel_by_name(c_animation_channel *channel, long name, unsigned long playback_flags) const;
    bool setup_animation_channel_by_parallel_impulse(
        c_animation_channel *channel,
        long weapon_class,
        long weapon_type,
        long impulse,
        unsigned long playback_flags) const;
    bool setup_animation_channel_by_death_or_damage(
        c_animation_channel *channel,
        long mode,
        long damage,
        short desired_direction,
        short desired_region,
        short *actual_direction,
        short *actual_region) const;
    bool setup_auxilary_channel(c_auxilary_channel *channel, long weapon_class, long weapon_type, long impulse, unsigned long playback_flags) const;
    bool setup_aiming_channel(c_aiming_channel *channel, long weapon_class, long weapon_type, long name, unsigned long playback_flags) const;
    bool update_animation_channel(c_animation_channel *channel, animation_event_callback event_callback, long event_data) const;
    bool update_animation_channel_on_graph_change(c_animation_channel *channel, long weapon_class, long weapon_type) const;
    bool update_state_animation(
        animation_event_callback event_callback,
        long event_data,
        long node_count,
        real_orientation *orientations,
        real_orientation *source_orientations);
    bool update_mixing_board_components(animation_event_callback event_callback, long event_data) const;
    bool find_next_vehicle_suspension(suspension_animation_iterator *iterator) const;
    bool find_next_object_overlay(object_overlay_iterator *iterator) const;

    void pause_state_channel(bool pause);
    bool state_channel_paused() const;
    bool state_channel_finished() const;
    float get_authored_state_time() const;
    bool current_state_contains_motion_data() const;
    void read_state_motion_data(long node_count, real_vector3d *velocity, float *yaw_rate) const;
    __forceinline bool get_pill_offset_data(real_point3d *position, real_vector3d *velocity) const
    {
        bool result = false;
        if (m_transition_channel.valid())
            result = m_transition_channel.get_pill_offset_data(position, velocity);
        else if (m_state_channel.valid())
            result = m_state_channel.get_pill_offset_data(position, velocity);
        return result;
    }
    void set_state_position_to_last_frame();
    bool start_interpolation(float duration, long node_count, real_orientation *orientations, real_orientation *source_orientations);
    bool get_current_foot_tracking_state(
        long foot_index,
        c_enum<e_foot_tracking_state, unsigned char, 0, 4> *state,
        float *fraction,
        short *frame) const;

    void get_node_orientations(
        const render_model_definition *render_model,
        float blend_ratio,
        const c_static_flags<255> *node_mask,
        long node_orientations_count,
        real_orientation *node_orientations,
        animation_node_orientations_callback animation_function,
        long user_data);
    void get_mixing_board_node_orientations(
        const render_model_definition *render_model,
        float blend_ratio,
        const c_static_flags<255> *node_mask,
        long node_orientations_count,
        real_orientation *node_orientations,
        animation_node_orientations_callback animation_function,
        long user_data);
    void apply_facial_node_orientations(
        const render_model_definition *render_model,
        float blend_ratio,
        const c_static_flags<255> *node_mask,
        long node_orientations_count,
        real_orientation *node_orientations,
        animation_node_orientations_callback animation_function,
        long user_data);
    t_short_fixed<13> get_slider_value_from_chain(long slider_index, e_mixing_board_parent_slider_type parent_slider_type) const;
    c_mixing_board_slider *get_mixing_board_slider(long slider_index) const;
    c_animation_channel *get_mixing_board_animation_channel(long channel_index) const;
    c_auxilary_channel *get_mixing_board_auxilary_channel(long channel_index) const;
    c_aiming_channel *get_mixing_board_aiming_channel(long channel_index) const;
    c_facial_animation_channel *get_mixing_board_facial_animation_channel() const;
    void reset_mixing_board();
    void reset_facial_animation_channels();
    void set_target_emotion(long emotion, float value, float time);
    void set_target_emotion_by_name(long name, float value, float time);

private:
    bool internal_setup_animation_channel(c_animation_channel *channel, c_animation_channel_settings *settings) const;
    bool internal_setup_auxilary_channel(c_auxilary_channel *channel, c_auxilary_channel_settings *settings) const;
    bool internal_setup_aiming_channel(c_aiming_channel *channel, c_aiming_channel_settings *settings) const;
    void apply_transition_delta(float scale, real_orientation *orientation);
    void apply_random_permutations_to_settings(c_animation_channel_settings *settings) const;
    void internal_determine_goal_state(long mode, long weapon_class, long weapon_type, long state, s_animation_goal_settings *settings) const;
    bool internal_validate_goal_state(
        long mode,
        long weapon_class,
        long weapon_type,
        long state,
        long flags,
        s_animation_goal_settings *current_goal,
        s_animation_goal_settings *previous_goal,
        c_animation_id *animation_id) const;

    // Allow channel setup helpers to access the private setup path.
    friend bool animation_manager_setup_animation_channel_by_index(
        const c_animation_manager *manager,
        c_animation_channel *channel,
        c_animation_id id,
        unsigned long flags);
    friend bool first_person_weapons_internal_setup_animation_channel_by_index(
        const c_animation_manager *manager,
        c_animation_channel *channel,
        c_animation_id id,
        unsigned long flags);
    friend bool vehicles_setup_acceleration(
        c_animation_manager *manager,
        c_animation_channel *channel,
        c_animation_id id,
        c_animation_channel_settings *storage);

public:
    c_animation_channel m_state_channel;
    c_animation_channel m_transition_channel;
    c_interpolator_control m_interpolator;
    unsigned long m_mixing_board_type;
    long m_object_index;
    long m_graph_tag_index;
    // Bits 0, 1, 4 and 6 control pause, randomness, interpolation and valid goals.
    unsigned short m_flags;
    // Bit 0 indicates an awaited transition.
    unsigned short m_transition_flags;
    s_animation_goal_settings m_current_goal;
    s_animation_goal_settings m_previous_goal;
    real_vector3d m_transition_translation;
};
typedef char animation_manager_size[(sizeof(c_animation_manager) == 176) ? 1 : -1];
extern "C" c_animation_id animation_manager_resolve_animation_id(const c_animation_manager *manager, c_animation_id id);

struct render_model_definition;
void object_apply_function_overlay_node_orientations(long object_index, const render_model_definition *render_model,
    c_animation_manager *manager, const c_static_flags<255> *node_mask, long node_count, real_orientation *orientations);

extern "C" const long k_facial_emotion_string_ids[9];
extern "C" const short k_damage_direction_fallback_table[4];
extern "C" short k_damage_region_fallback_table[];
#endif
