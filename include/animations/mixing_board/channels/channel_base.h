#ifndef ANIMATIONS_MIXING_BOARD_CHANNELS_CHANNEL_BASE_H
#define ANIMATIONS_MIXING_BOARD_CHANNELS_CHANNEL_BASE_H

// The base of every mixing-board channel: the animation it plays, its playback flags and its parent slider.
// Methods read from the March 2007 debug build (`local/odin-bin/marchbody class c_channel_base`); its inline
// methods assert in animations\mixing_board\channels\channel_base.h (lines 183-195), its out-of-line ones in
// channel_base.cpp.

#include "animations/animation_id.h"
#include "animations/animation_search_path_iterator.h"
#include "tag_files/tag_groups.h"
#include "cseries/cseries_wrapped.h"

template<long k_maximum_count> class c_static_flags;
class c_animation_data;
class c_model_animation;
class c_model_animation_graph;
struct render_model_definition;
struct real_orientation;
struct s_animation_inheritence;
union real_point3d;

enum e_animation_playback_flag_bits {
    _animation_playback_autoplay_bit,
    _animation_playback_looping_bit,
    // When set, auxilary-channel setup zeroes the yaw setting instead of keeping it.
    _animation_playback_unknown2_bit = 2,
    // When set, channel setup resolves the animation and blend ids through the animation graph.
    _animation_playback_unknown4_bit = 4,
    // When clear with a zero loop frame, update_to_frame_position resolves a new animation id on loop.
    _animation_playback_unknown5_bit = 5,
    _animation_playback_unknown6_bit = 6,
};

enum e_animation_graph_location {
    _animation_location_none,
    _animation_location_action,
    _animation_location_overlay,
    _animation_location_damage,
    _animation_location_transition,
    _animation_location_suspension,
    _animation_location_object_overlay,
};

enum e_mixing_board_parent_slider_type {
    _mixing_board_parent_slider_none = 0,
    _mixing_board_parent_slider_regular = 1,
    _mixing_board_parent_slider_inverse = 2,
    k_mixing_board_parent_slider_type_count = 3
};

// Which fields two channel settings differ in (names from motor_animation.cpp, the owner of the comparisons).
enum e_channel_settings_comparison_bits {
    _comparison_channel_graph_bit,
    _comparison_channel_definition_bit,
    _comparison_channel_animation_bit,
    _comparison_channel_playback_flags_bit,
    _comparison_channel_location_bit,
    _comparison_channel_flags_bit,
    _comparison_playback_ratio_bit,
    _comparison_playback_rate_bit,
    _comparison_blend_ids_bit,
    _comparison_blend_ratios_bit,
};

typedef void (*animation_node_orientations_callback)(long, const c_model_animation_graph *, c_animation_id, const unsigned long *, float);

// The settings a channel is set up from (March: inline constructor and clear_base, both in motor_animation.obj).
class c_base_channel_settings {
public:
    __forceinline c_base_channel_settings() { clear_base(); }
    __forceinline void clear_base()
    {
        m_graph_index = -1;
        m_definition_index = -1;
        m_animation_id.clear();
        m_playback_flags.clear();
        m_location = _animation_location_none;
    }
    // Returns which base settings differ from another channel's settings.
    c_flags<e_channel_settings_comparison_bits, unsigned long, 10> compare_base_settings(const c_base_channel_settings &other) const;

    long m_graph_index;                                                          // graph tag index
    long m_definition_index;
    c_animation_id m_animation_id;
    c_flags<e_animation_playback_flag_bits, unsigned short, 16> m_playback_flags;
    c_enum<e_animation_graph_location, char, 0, 7> m_location;
};
typedef char channel_base_settings_size[(sizeof(c_base_channel_settings) == 16) ? 1 : -1];

class c_channel_base {
    // match: 0x28f240: parked L2 manager accesses its auxilary channels' protected method.
    friend class c_animation_manager;
public:
    // Starts unparented and enabled, with cleared channel settings.
    c_channel_base()
    {
        m_parent_slider_index = -1;
        m_parent_slider_type = _mixing_board_parent_slider_none;
        m_enabled = true;
        clear();
    }

    // Resets the graph, animation and playback settings.
    __forceinline void clear()
    {
        m_graph_index = -1;
        m_definition_index = -1;
        m_animation_id.clear();
        m_playback_flags.clear();
        m_location = _animation_location_none;
    }

    // Starts unparented and enabled, with cleared channel settings (`marchbody class c_channel_base`).
    __forceinline void initialize()
    {
        m_parent_slider_index = -1;
        m_parent_slider_type = _mixing_board_parent_slider_none;
        m_enabled = true;
        clear();
    }

    // True when the channel is enabled and names a valid animation.
    __forceinline bool valid() const { return m_enabled && m_graph_index != -1 && m_animation_id.valid(); }
    __forceinline bool autoplay_enabled() const { return m_playback_flags.test(_animation_playback_autoplay_bit); }
    __forceinline bool looping_enabled() const { return m_playback_flags.test(_animation_playback_looping_bit); }
    // channel_base.h:195
    __forceinline void set_playback_flags(c_flags<e_animation_playback_flag_bits, unsigned short, 16> playback_flags) { m_playback_flags = playback_flags; }
    __forceinline const c_flags<e_animation_playback_flag_bits, unsigned short, 16> &get_playback_flags() const { return m_playback_flags; }
    __forceinline c_flags<e_animation_playback_flag_bits, unsigned short, 16> &access_playback_flags() { return m_playback_flags; }
    __forceinline c_animation_id get_animation_id() const { return m_animation_id; }
    __forceinline long get_graph_index() const { return m_graph_index; }
    __forceinline long get_parent_slider_index() const { return m_parent_slider_index; }
    __forceinline c_enum<e_mixing_board_parent_slider_type, unsigned char, 0, 3> get_parent_slider_type() const { return m_parent_slider_type; }
    __forceinline bool parent_slider_valid() const { return m_parent_slider_index != -1 && m_parent_slider_type != _mixing_board_parent_slider_none; }
    // Authored animations play at 30 frames per second.
    __forceinline float get_authored_rate() const { return 30.0f; }

    // out of line (channel_base.cpp)
    __forceinline const c_model_animation *get_animation() const
    {
        const c_model_animation *animation = 0;
        if (m_animation_id.valid())
        {
            const c_model_animation_graph *graph = TAG_GET(0, const c_model_animation_graph, m_graph_index);
            animation = graph->get_animation(m_animation_id);
        }
        return animation;
    }
    const c_model_animation_graph *get_graph() const;
    long get_node_count() const;
    long get_frame_count() const;
    float get_authored_duration() const;
    bool animation_allows_interpolation_in() const;
    bool animation_allows_interpolation_out() const;
    bool animation_allows_transition_adjustments() const;
    void get_animation_data(c_animation_data *data) const;
    void get_keyframe_translation(float frame, unsigned short node_index, short keyframe, real_point3d *translation) const;
    const s_animation_inheritence *get_node_remap_data() const;
    const c_static_flags<255> *prepare_node_mask_flags(const render_model_definition *render_model, long node_count) const;
    void set_parent_slider(long parent_slider_index, e_mixing_board_parent_slider_type parent_slider_type);
    void get_base_channel_settings(c_base_channel_settings *settings) const;

protected:
    // Sets up the channel base from the given settings.
    bool setup_channel_base(const c_base_channel_settings *settings);
    void internal_apply_weighted_node_orientations(c_animation_id animation_id, float frame,
        const render_model_definition *render_model, float blend_ratio, const c_static_flags<255> *node_mask,
        long node_orientations_count, real_orientation *node_orientations,
        animation_node_orientations_callback animation_function, long user_data, bool use_frame_rate_scale);

public:
    long m_graph_index;                                                          // graph tag index
    long m_definition_index;
    c_animation_id m_animation_id;
    c_flags<e_animation_playback_flag_bits, unsigned short, 16> m_playback_flags;
    c_enum<e_animation_graph_location, char, 0, 7> m_location;
    char m_parent_slider_index;                                                  // NONE when unparented
    c_enum<e_mixing_board_parent_slider_type, unsigned char, 0, 3> m_parent_slider_type;
    char m_enabled;  // whether the channel is enabled
};
typedef char channel_base_size[(sizeof(c_channel_base) == 20) ? 1 : -1];

#endif // ANIMATIONS_MIXING_BOARD_CHANNELS_CHANNEL_BASE_H
