// blamlibxbox/animation_manager.obj: from the records 0xc0cc8 0xd63a0 0xd6d80 0xd6f50 0xdba18 0xdcd80 0xdf7d8 0xe1ce8 0xe84a8 0xe8710 0xe9298 0xed108 0xed738 0xedf58 0x101f40 0x102318 0x102a20 0x102ac0 0x10a968 0x110730 0x11a238 0x1205d0 0x1206a0 0x120b80 0x120f60 0x1213a0 0x1215c8 0x121648 0x121790 0x121910 0x122200 0x1d38f0 0x28e078 0x28e0e0 0x28e1a8 0x28e350 0x28e4c8 0x28e698 0x28e6f8 0x28e7a8 0x28e838 0x28e8c0 0x28e980 0x28eb20 0x28ebc8 0x28ec88 0x28ed80 0x28ee88 0x28ef18 0x28ef98 0x28eff0 0x28f050 0x28f1a8 0x28f240 0x28f788 0x28f938 0x28fa80 0x28fb38 0x28ff10 0x28ffa0 0x31e108 0x3482c0
#include "animations/animation_manager_queries.h"
#include "math/real_math_constants.h"
#include "animations/animation_manager_storage.h"
#include "animations/animation_node_flags.h"
#include "math/real_math_types.h"
#include "animations/animation_state_change_settings.h"
#include "tag_files/tag_types.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"
#include "animations/animation_id.h"
#include "game/game_types.h"
#include "memory/static_arrays.h"
#include <stdlib.h>
#include <math.h>
#include "objects/objects.h"
#include "animations/animation_definitions.h"
#include "animations/animation_search_path_iterator.h"
#include "scenario/scenario_map_variant.h"
#include "animations/animation_manager.h"
#include "game/map_variant.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "animations/mixing_board/mixing_board.h"
#include "animations/mixing_board/mixing_board_slider.h"
#include "objects/multiplayer_object_types.h"
#include "units/slider_movement_pattern_types.h"
#include "motor/mixing_board_component_types.h"
#include "units/slider_target_position_types.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "animations/mixing_board/mixing_board_slider_types.h"
#include "animations/animation_codec_flags.h"
#include "animations/suspension_animation_iterator.h"
#include "animations/mixing_board/channels/auxilary_channel.h"
#include "cseries/crt_xdk.h"
#include "animations/mixing_board/channels/facial_animation_channel_types.h"
#include "animations/mixing_board/channels/aiming_channel.h"
#include "tag_files/tag_groups.h"
#include "animations/animation_definition_types.h"
#include "memory/tls_globals.h"
#include "memory/datum_index.h"
#include "cseries/cseries.h"
#include "models/model_types.h"
#include "animations/animation_manager_mixing_board_inline.h"
#include "animations/ik_point_iterator.h"
#include "units/bipeds.h"
#include "units/giants.h"
#include "units/unit_animation.h"

// @address 0x2251c k_real_zero_storage
// @address 0x22dc0 k_real_half_pool
// @address 0x3aa90 k_real_math_zero_vector3d_storage
// @flags /TP /O2 /Oc /Ob1
// Checker symbol hint for this const member: update_animation_channel_on_graph_change() {
// @address 0x2fa908 ?jump_to_last_frame_and_stop@c_animation_channel@@QAAXXZ
// @address 0xe8850 ?find_named_block_entry_with_size@@YAPAXPBUs_tag_block@@JJ@Z
// @address 0xd63f8 ?update@c_interpolator_control@@QAA_NXZ
// @address 0xe88e0 ?find_action@c_model_animation_graph@@QBA?AVc_animation_id@@JJJJPAJ00@Z
// @address 0x2fc468 ?find_weapon_class@c_model_animation_graph@@QBAPBVc_weapon_class@@JJ@Z
// @address 0xdcfc8 ?paused@c_animation_channel@@QBA_NXZ
// @address 0x79d458 ?g_mixing_board_definition_list@@3PAVc_mixing_board_definition@@A
#if !defined(_XBOX) || defined(__clang__)
// The Xbox image fills this list at startup with the dynamic initializer 0x708c00 (stores, three fill loops and one
// memset); other compilers take the same contents as a constant initializer.
c_mixing_board_definition g_mixing_board_definition_list[3] = {
    {
        "biped_mixing_board_definition", 0x304,
        unit_animation_setup_mixing_board_channels, unit_animation_snap_mixing_sliders, biped_locate_auxilary_animations,
        { { 0x0, 8 }, { 0x60, 3 }, { 0xfc, 7 }, { 0x1a4, 5 }, { 0x2a8, 1 } },
        {
            { "aim_on", "look_on", "overlay_on", "weapon_down", "use_look_down", "weapon_ik", "mode_ik", "pendulum_on" },
            { "primary", "secondary", "overlay" },
            { "accel_x", "accel_y", "accel_z", "offset_1", "offset_2", "grip_1", "grip_2" },
            { "weapon_up", "weapon_down", "look", "look_wpn_dn", "pendulum" },
            { "facial" },
        },
    },
    {
        "vehicle_mixing_board_definition", 0x1f0,
        unit_animation_setup_mixing_board, unit_animation_reset_interpolators, unit_animation_locate_auxilary_animations,
        { { 0x0, 5 }, { 0x3c, 3 }, { 0xd8, 3 }, { 0x120, 4 }, { 0x0, 0 } },
        {
            { "aim_on", "look_on", "overlay_on", "weapon_down", "use_look_down" },
            { "primary", "secondary", "overlay" },
            { "accel_x", "accel_y", "accel_z" },
            { "weapon_up", "weapon_down", "look", "look_wpn_dn" },
            { "" },
        },
    },
    {
        "giant_mixing_board_definition", 0x254,
        giant_animation_setup_mixing_board, unit_animation_reset_interpolators, unit_animation_setup_weapon_aux_channels,
        { { 0x0, 9 }, { 0x6c, 4 }, { 0x13c, 3 }, { 0x184, 4 }, { 0x0, 0 } },
        {
            { "aim_on", "look_on", "overlay_on", "weapon_down", "use_look_down", "giant_speed", "buckle", "buckle_overlay" },
            { "primary", "secondary", "overlay", "buckle_overlay", "buckle_overlay_max" },
            { "accel_x", "accel_y", "accel_z" },
            { "weapon_up", "weapon_down", "look", "look_wpn_dn" },
            { "" },
        },
    },
};
#endif
// @address 0x11a238 ?state_channel_finished@c_animation_manager@@QBA_NXZ
// @address 0x2fae20 ?contains_motion_data@c_animation_channel@@QBA_NXZ
// @address 0x2fc4c8 ?find_damage_animation@c_model_animation_graph@@QBA?AVc_animation_id@@JJJJJJPAJ00@Z
// @address 0x2fd128 ?reset_mixing_board@@YAXPAVc_animation_manager@@JPBVc_mixing_board_definition@@@Z
// @address 0xdb468 ??0c_animation_channel@@QAA@XZ
// @address 0xdcd10 ??0c_animation_channel_settings@@QAA@XZ
// @address 0x2fbc10 ?animation_allows_interpolation_out@c_channel_base@@QBA_NXZ
// @address 0x2fc740 ?find_animation@c_model_animation_graph@@QBA?AVc_animation_id@@J@Z
// @address 0x1214b0 ?find_overlay@c_model_animation_graph@@QBA?AVc_animation_id@@JJJJPAJ00@Z
// @address 0x2faf30 ?get_motion_data@c_animation_channel@@QBAXPATreal_vector3d@@PAM@Z
// @address 0x120e88 ?object_start_interpolation@@YA_NJM@Z
// @address 0xdf848 ?playing@c_animation_channel@@QBA_NXZ
// @address 0x102318 ?recommend_state_change@c_animation_manager@@QBA_NJJJJJKPAUs_animation_state_change_settings@@@Z
// @address 0xed798 ?setup_channel@c_animation_channel@@AAA_NPBVc_animation_channel_settings@@@Z
// @address 0xdcdf0 ?get_channel_settings@c_animation_channel@@QBAXPAVc_animation_channel_settings@@@Z
// @address 0xed968 ?setup_channel_base@c_channel_base@@IAA_NPBVc_base_channel_settings@@@Z
// @address 0xdb528 ?reset@c_animation_channel@@QAAXXZ
// @address 0x120980 ?find_transition@c_model_animation_graph@@QBA?AVc_animation_id@@JJJJDDJJDDD@Z

// Return the chained mixing-board slider value.
class c_model_animation;
struct s_animation_goal_settings;

enum { k_mixing_board_facial_component_type = 4 };


enum {
    _label_default = 0xe2,
};


struct label_remap {
    int label;
    int replacement;
};


struct render_model_definition;
struct real_orientation;

class c_model_animation;
class c_model_animation_graph;
struct c_compact_auxilary_channel_settings : public c_base_channel_settings {
 float m_yaw;
};

struct s_matched_animation_state {
    long name;
    long state;
};

struct s_channel_limits {
 float yaw_left_scale; float yaw_right_scale; short yaw_left_count; short yaw_right_count;
 float pitch_down_scale; float pitch_up_scale; short pitch_down_count; short pitch_up_count;
};

struct object_overlay_iterator {
 __forceinline object_overlay_iterator() : function_name(0), mode(-1), index(-1) {}
 c_animation_id animation;
 long function_name;
 short mode;
 short index;
};

class c_animation_channel;
class c_animation_manager;
class c_animation_channel_settings;
static __forceinline bool animation_manager_setup_on_graph_change(
    const c_animation_manager *manager, c_animation_channel *channel, c_animation_channel_settings *settings);
class c_base_channel_settings;
class c_auxilary_channel;
struct s_animation_state_change_settings;
struct s_animation_event_data;
typedef void (*animation_manager_channel_callback)(long, const c_model_animation_graph *, c_animation_id, const unsigned long *, float);


// @address 0x2aaf4 k_facial_emotion_string_ids


class c_animation_manager;
static __forceinline bool animation_manager_setup_animation_channel_by_index(
    const c_animation_manager *manager, c_animation_channel *channel, c_animation_id id, unsigned long flags);
// match: 0xc0cc8: free-function form keeps the address arithmetic in integer registers; the method form reallocates them.
static __forceinline const c_mixing_board_slider *animation_manager_get_mixing_board_slider(const c_animation_manager *object, long slider_index)
{
 long slider_offset = mixing_board_definition_get_by_type(object->m_mixing_board_type)->component_types[0].offset;
 return reinterpret_cast<const c_mixing_board_slider *>(object->get_mixing_board_buffer() + slider_offset + g_mixing_board_component_sizes[0] * slider_index);
}


// The aiming channel has yaw/pitch limits; the auxilary channel plays one frame position.
class c_aiming_channel;

typedef char animation_manager_aiming_settings_size[(sizeof(c_aiming_channel_settings) == 28) ? 1 : -1];
typedef char c_aiming_channel_size[(sizeof(c_aiming_channel) == 52) ? 1 : -1];
/* Compact auxilary settings view (base + yaw only) used by setup_auxilary_channel (0x1213a0). */


// match: 0x28ee88: keep the setup path inlined here while 0xed738 stays an out-of-line body.
static __forceinline bool animation_manager_setup_animation_channel_by_index(
    const c_animation_manager *manager, c_animation_channel *channel, c_animation_id id, unsigned long flags)
{
 bool result = false;
 if (manager->m_graph_tag_index != -1)
 {
  c_animation_channel_settings settings;
  settings.m_graph_index = manager->m_graph_tag_index;
  settings.m_animation_id = id;
  settings.m_playback_flags.set_unsafe((unsigned short)flags);
  result = manager->internal_setup_animation_channel(channel, &settings);
 }
 return result;
}


static __forceinline void animation_manager_pause_playback(c_animation_channel *channel, bool paused)
{
 if (channel->m_playback_flags.get_unsafe() & 1)
  channel->m_state_flags.set_unsafe(paused ? channel->m_state_flags.get_unsafe() | 1 : channel->m_state_flags.get_unsafe() & ~1);
}
static __forceinline void animation_manager_reset_auxilary(c_auxilary_channel *channel)
{
 channel->m_graph_index = -1;
 channel->m_definition_index = -1;
 channel->m_animation_id.clear();
 channel->m_playback_flags.set_unsafe(0);
 channel->m_location = _animation_location_none;
 channel->m_frame_position = 0.0f;
}
static __forceinline bool animation_manager_setup_auxilary(c_auxilary_channel *channel, const c_auxilary_channel_settings *settings)
{
 animation_manager_reset_auxilary(channel);
 if (channel->setup_channel_base(settings))
  channel->m_frame_position = settings->m_yaw;
 return channel->valid();
}


// @address 0xc0cc8 ?get_slider_value_from_chain@c_animation_manager@@QBA?AV?$t_short_fixed@$0N@@@JW4e_mixing_board_parent_slider_type@@@Z
t_short_fixed<13> c_animation_manager::get_slider_value_from_chain(long slider_index, e_mixing_board_parent_slider_type parent_slider_type) const
{
 const c_mixing_board_slider *slider = animation_manager_get_mixing_board_slider(this, slider_index);
 t_short_fixed<13> value = slider->get_value();
 if (parent_slider_type == _mixing_board_parent_slider_inverse)
  value = t_short_fixed<13>(t_short_fixed<13>::k_fixed_one) - value;
 while (slider->parent_slider_valid())
 {
  long parent_slider_index = slider->get_parent_slider_index();
  e_mixing_board_parent_slider_type parent_type = slider->get_parent_slider_type();
  slider = animation_manager_get_mixing_board_slider(this, parent_slider_index);
  if (parent_type == _mixing_board_parent_slider_inverse)
   value *= t_short_fixed<13>(t_short_fixed<13>::k_fixed_one) - slider->get_value();
  else
   value *= slider->get_value();
 }
 return value;
}

// @address 0xd6d80 ?update_state_animation@c_animation_manager@@QAA_NP6AXPBUs_animation_event_data@@J@ZJJPAUreal_orientation@@2@Z
bool c_animation_manager::update_state_animation(void (*event_callback)(const s_animation_event_data *, long), long event_data,
    long node_count, real_orientation *orientations, real_orientation *source)
{
 bool changed = false;
 if (m_transition_channel.valid())
  changed |= update_animation_channel(&m_transition_channel, event_callback, event_data);
 else if (m_state_channel.valid())
  changed |= update_animation_channel(&m_state_channel, event_callback, event_data);
 if (m_transition_channel.valid() &&
     ((bool)((m_transition_channel.m_state_flags.get_unsafe() >> 2) & 1) ||
         (bool)((m_transition_channel.m_state_flags.get_unsafe() >> 1) & 1) || !m_transition_channel.playing()))
 {
  if (m_transition_channel.animation_allows_interpolation_out())
  {
   start_interpolation(0.26666668f, node_count, orientations, source);
   changed = true;
  }
  m_transition_channel.reset();
  if (m_state_channel.valid())
  {
   animation_manager_pause_playback(&m_state_channel, false);
   changed = true;
  }
 }
 return changed;
}

// @address 0xdba18 ?find_next_vehicle_suspension@c_animation_manager@@QBA_NPAVsuspension_animation_iterator@@@Z
bool c_animation_manager::find_next_vehicle_suspension(suspension_animation_iterator *iterator) const {
    const c_model_animation_graph *graph = (const c_model_animation_graph *)tag_get(0, m_graph_tag_index);
    short suspension_index = (short)(iterator->cursor + 1);
    if (suspension_index < graph->m_vehicle_suspensions.count) {
        iterator->cursor = suspension_index;
        const s_vehicle_suspension *suspension = &((const s_vehicle_suspension *)graph->m_vehicle_suspensions.address)[iterator->cursor];
        iterator->animation = suspension->animation;
        iterator->rate = suspension->rate;
        iterator->compression = suspension->compression;
        iterator->marker = suspension->marker;
        iterator->extension = suspension->extension;
        iterator->suspension_index = suspension_index;
        iterator->region = suspension->region;
        iterator->limit = suspension->limit;
        iterator->offset = suspension->offset;
        iterator->scale = suspension->scale;
        return true;
    } else {
        return false;
    }
}

// Resolves a unit's four animation labels: "default" (0xe2) takes the unit's
// own label, NONE/1 fall back to fixed labels, and the weapon label may be
// remapped through the unit definition's table.

struct cache_file_tag_instance;


// @address 0xe8710 ?internal_determine_goal_state@c_animation_manager@@ABAXJJJJPAUs_animation_goal_settings@@@Z
void c_animation_manager::internal_determine_goal_state(long mode, long weapon_class, long weapon_type, long set, s_animation_goal_settings
    *settings) const
{
    settings->mode = mode;
    settings->weapon_class = weapon_class;
    settings->weapon_type = weapon_type;
    settings->state = set;

    if ((mode == _label_default && (settings->mode = m_current_goal.mode) == -1) || settings->mode == 1)
        settings->mode = 0x8a;

    if (weapon_class == _label_default)
        settings->weapon_class = m_current_goal.weapon_class;
    if (settings->weapon_class == 1 || settings->weapon_class == -1)
        settings->weapon_class = 0x87;

    if (weapon_type == _label_default)
        settings->weapon_type = m_current_goal.weapon_type;
    if (settings->weapon_type == 1 || settings->weapon_type == -1)
        settings->weapon_type = 200;

    int label = settings->weapon_class;
    label_remap *remap = (label_remap *)find_named_block_entry_with_size(&((const c_model_animation_graph *)tag_get(0,
        m_graph_tag_index))->m_weapon_type_remaps, settings->weapon_type, sizeof(label_remap));
    if (remap && remap->replacement != -1 && remap->replacement != 200 && label != 0x1e6)
        label = remap->replacement;
    settings->weapon_class = label;

    if (set == _label_default && (settings->state = m_current_goal.state) == -1)
        settings->state = 12;
    if (settings->state == 1)
        settings->state = 12;
}

// @address 0xdcd80 ?get_current_settings@c_animation_manager@@QBAXPAUs_animation_state_change_settings@@@Z
void c_animation_manager::get_current_settings(s_animation_state_change_settings *settings) const
{
 m_state_channel.get_channel_settings(&settings->primary);
 m_transition_channel.get_channel_settings(&settings->transition);
 settings->goals = m_current_goal;
}

// @address 0xed108 ?pause_state_channel@c_animation_manager@@QAAX_N@Z
void c_animation_manager::pause_state_channel(bool paused)
{
 if (m_transition_channel.valid())
  animation_manager_pause_playback(&m_transition_channel, paused);
 if ((paused || !m_transition_channel.valid()) && m_state_channel.valid())
  animation_manager_pause_playback(&m_state_channel, paused);
}

// @address 0xed738 ?setup_animation_channel_by_index@c_animation_manager@@QBA_NPAVc_animation_channel@@Vc_animation_id@@K@Z
bool c_animation_manager::setup_animation_channel_by_index(c_animation_channel *channel, c_animation_id id, unsigned long flags) const
{
 return animation_manager_setup_animation_channel_by_index(this, channel, id, flags);
}
// @address 0xedf58 ?state_channel_paused@c_animation_manager@@QBA_NXZ
bool c_animation_manager::state_channel_paused() const {
    bool result = (m_flags & 1) != 0;
    if (!result) {
        if (m_transition_channel.valid())
            result = m_transition_channel.paused();
        else if (m_state_channel.valid())
            result = m_state_channel.paused();
    }
    return result;
}


// a valid id is replaced by the graph's resolved id when the graph has a definition.
// @address 0x101f40 animation_manager_resolve_animation_id
extern "C" c_animation_id animation_manager_resolve_animation_id(const c_animation_manager *manager, c_animation_id id)
{
 c_animation_id result = id;
 if (id.valid())
 {
  const c_model_animation_graph *definition = (const c_model_animation_graph *)tag_get(0, manager->m_graph_tag_index);
  if (definition != 0)
  {
   char resolve_buffer[16];
   bool deterministic = ((manager->m_flags >> 1) & 1) != 0;
   const c_animation_id *resolved = animation_definitions_find_random_variation((c_animation_id *)resolve_buffer, definition, id, deterministic);
   unsigned short graph_index = (unsigned short)resolved->m_graph_index;
   unsigned short animation_index = (unsigned short)resolved->m_animation_index;
   result.m_graph_index = graph_index;
   result.m_animation_index = animation_index;
  }
 }
 return result;
}

// @address 0x102a20 ?apply_random_permutations_to_settings@c_animation_manager@@ABAXPAVc_animation_channel_settings@@@Z
void c_animation_manager::apply_random_permutations_to_settings(c_animation_channel_settings *settings) const
{
 if (settings->m_playback_flags.test(_animation_playback_unknown4_bit))
 {
  if (settings->m_animation_id.valid())
  {
   settings->m_animation_id = animation_manager_resolve_animation_id(this, settings->m_animation_id);
  }
  for (long blend_index = 0; blend_index < 2; ++blend_index)
  {
   if (settings->m_blend_ids[blend_index].valid())
   {
    settings->m_blend_ids[blend_index] = animation_manager_resolve_animation_id(this, settings->m_blend_ids[blend_index]);
   }
  }
 }
}

// @address 0x102ac0 ?get_authored_state_time@c_animation_manager@@QBAMXZ
float c_animation_manager::get_authored_state_time() const
{
    float result = 0.0f;
    if (m_state_channel.valid())
        result = m_state_channel.m_frame_position * (1.0f / 30.0f);
    return result;
}

// @address 0x10a968 ?current_state_contains_motion_data@c_animation_manager@@QBA_NXZ
bool c_animation_manager::current_state_contains_motion_data() const
{
    bool result = false;
    if (m_transition_channel.valid())
        result = m_transition_channel.contains_motion_data();
    else if (m_state_channel.valid())
        result = m_state_channel.contains_motion_data();
    return result;
}

bool c_animation_manager::state_channel_finished() const
{
    bool result = true;
    if (m_state_channel.valid())
        result = m_state_channel.playback_complete();
    return result;
}

static __forceinline const c_model_animation_graph *animation_manager_get_graph(const c_animation_manager *object) {
    return (const c_model_animation_graph *)tag_get(0, object->m_graph_tag_index);
}


// @address 0x744620 g_matched_animation_states
static s_matched_animation_state g_matched_animation_states[4] = { { 0x15, 0x1be }, { 0x16, 0x1bf }, { 0x17, 0x1c0 }, { 0x18, 0x1c1 } };
// @address 0x744640 g_matched_animation_state_names
static long g_matched_animation_state_names[28] = {
    0x0c, 0x15, 0x16, 0x17, 0x18, 0x1d, 0x1e, 0x3f, 0x40, 0x41, 0x42, 0x19, 0x1a, 0x1b,
    0x1c, 0x5c, 0x5d, 0x5e, 0x5f, 0x1f, 0x4f, 0x4b, 0x4d, 0x62, 0x4a, 0x6a, 0x29a, 0x1de };

// @address 0x7446b0 k_damage_region_fallback_table
extern "C" {
short k_damage_region_fallback_table[14] = { 2, 0, 1, 1, 3, 0, 5, 1, 7, 0, 9, 0, 1, 0 };
}

static __forceinline long animation_manager_find_matched_state(long name, const s_matched_animation_state *states, long count)
{
    for (long index = 0; index < count; ++index)
    {
        if (name == states[index].name)
            return states[index].state;
    }
    return -1;
}

// @address 0x1205d0 ?locate_blend_animation@c_animation_manager@@QBA?AVc_animation_id@@JJJJPAJ@Z
c_animation_id c_animation_manager::locate_blend_animation(long mode, long weapon_class, long weapon_type, long blend_state_name, long
    *blend_state) const
{
    c_animation_id result;
    long state = animation_manager_find_matched_state(blend_state_name, g_matched_animation_states, 4);
    *blend_state = -1;
    if (state != -1)
    {
        s_animation_goal_settings filtered_settings;
        s_animation_goal_settings actual_settings;
        if (internal_validate_goal_state(mode, weapon_class, weapon_type, state, 0, &filtered_settings,
                &actual_settings, &result))
            *blend_state = state;
        else
            result.clear();
    }
    return result;
}

// @address 0x1206a0 ?locate_transition_animation@c_animation_manager@@QBA?AVc_animation_id@@JJJJPAJ@Z
// match: 0x1206a0: pointer __restrict qualifier prevents alias reload
c_animation_id c_animation_manager::locate_transition_animation(long mode, long from_state, long next_mode, long next_state, long *__restrict
    frame_delay) const
{
    *frame_delay = 0;
    c_animation_id result;
    c_animation_id default_transition = animation_manager_get_graph(this)->find_transition(
        mode, m_current_goal.weapon_class, m_current_goal.weapon_type, from_state, -1, -1, next_mode, next_state, -1, -1, 0);
    const c_model_animation *animation = get_state_animation();
    c_animation_id best_transition = default_transition;
    long best_frame_delay = 0;
    bool found_key = false;
    if (animation) {
        long best_distance = animation->get_frame_count();
        long frame = (long)m_state_channel.get_frame_position();
        for (long type = 1; type <= 4; ++type) {
            c_animation_id transition = animation_manager_get_graph(this)->find_transition(mode, m_current_goal.weapon_class,
                m_current_goal.weapon_type, from_state, -1, -1, next_mode, next_state, -1, -1, (char)type);
            if (transition.valid()) {
                long key_type = 5;
                if (type == 1) key_type = 5;
                else if (type == 2) key_type = 6;
                else if (type == 3) key_type = 7;
                else if (type == 4) key_type = 8;
                long key_frame = animation->find_next_key_of_type(key_type, -1);
                while (key_frame != -1) {
                    found_key = true;
                    long delay = key_frame - frame;
                    long distance = delay;
                    if (distance < 0) {
                        distance = abs(delay * delay * delay);
                        delay = 0;
                    }
                    if (!best_transition.valid() || distance < best_distance) {
                        best_transition = transition;
                        best_distance = distance;
                        best_frame_delay = delay;
                    }
                    key_frame = animation->find_next_key_of_type(key_type, key_frame);
                }
            }
        }
    }
    if (default_transition.valid() && !best_transition.valid()) {
        result = default_transition;
        *frame_delay = 0;
    } else if (best_transition.valid()) {
        result = best_transition;
        *frame_delay = best_frame_delay;
    }
    return result;
}

// @address 0x120b80 ?execute_state_change@c_animation_manager@@QAA_NPBUs_animation_state_change_settings@@@Z
bool c_animation_manager::execute_state_change(const s_animation_state_change_settings *settings)
{
 m_transition_flags &= ~3;
 if ((bool)((settings->flags.get_unsafe() >> 4) & 1))
 {
  m_transition_channel.reset();
  if (m_state_channel.valid())
   animation_manager_pause_playback(&m_state_channel, true);
  else
   m_state_channel.reset();
 }
 bool result;
 if (!(bool)((settings->flags.get_unsafe() >> 3) & 1) && (!(bool)((settings->flags.get_unsafe() >> 2) & 1) ||
     settings->primary.m_animation_id.valid()))
 {
  if (settings->primary.m_animation_id.valid() && m_state_channel.setup_channel(&settings->primary))
  {
   result = true;
   if (settings->transition.m_animation_id.valid())
   {
    if (m_transition_channel.setup_channel(&settings->transition))
     animation_manager_pause_playback(&m_state_channel, true);
    else
    {
     m_transition_channel.reset();
     animation_manager_get_graph(this)->get_animation(settings->transition.m_animation_id);
    }
   }
   else
    m_transition_channel.reset();
  }
  else
  {
   m_state_channel.reset();
   m_transition_channel.reset();
   m_flags &= ~64;
   result = false;
   animation_manager_get_graph(this)->get_animation(settings->primary.m_animation_id);
  }
  if (result)
  {
   m_previous_goal = m_current_goal;
   m_current_goal = settings->goals;
   m_flags &= ~1;
   if (m_object_index != -1 && settings->interpolation_time > 0.0001f && !(bool)((settings->flags.get_unsafe() >> 11) & 1))
    object_start_interpolation(m_object_index, settings->interpolation_time);
  }
 }
 else
 {
  result = true;
  m_current_goal = settings->goals;
  m_transition_channel.reset();
  if (m_state_channel.valid())
   animation_manager_pause_playback(&m_state_channel, true);
  else
   m_state_channel.reset();
 }
 if (m_graph_tag_index != -1 && result)
  m_flags |= 64;
 else
  m_flags &= ~64;
 return result;
}

// @address 0x120f60 ?start_interpolation@c_animation_manager@@QAA_NMJPAUreal_orientation@@0@Z
bool c_animation_manager::start_interpolation(float duration, long node_count, real_orientation *orientations, real_orientation *source)
{
 bool result = false;
 if (node_count > 0 && duration > 0.0f)
 {
  c_interpolator_control *interpolator = &m_interpolator;
  if (!interpolator->enabled() || interpolator->finished() || interpolator->get_time_remaining() <= duration)
  {
   interpolator->disable();
   XMemCpy(orientations, source, node_count * sizeof(real_orientation));
   interpolator->set_status(true, duration);
   result = true;
  }
 }
 return result;
}


// @address 0x1215c8 ?find_state_animation_id@c_animation_manager@@QBA?AVc_animation_id@@JJJ@Z
c_animation_id c_animation_manager::find_state_animation_id(long weapon_class, long weapon_type, long state) const
{
    c_animation_id result;
    c_animation_id animation_id;
    s_animation_goal_settings filtered_settings;
    s_animation_goal_settings actual_settings;
    if (internal_validate_goal_state(m_current_goal.mode, weapon_class, weapon_type, state, 0,
            &filtered_settings, &actual_settings,
            &animation_id))
        result = animation_id;
    return result;
}

/* channel limits as mixing_board_get_blend_screen_bounds returns them: scales and counts for the yaw and pitch pins */


// @address 0x121910 ?setup_channel@c_aiming_channel@@AAA_NPBVc_aiming_channel_settings@@@Z
bool c_aiming_channel::setup_channel(const c_aiming_channel_settings *settings)
{
 float zero = k_real_zero_storage;
 this->m_graph_index = -1;
 this->m_definition_index = -1;
 this->m_animation_id.clear();
 { unsigned short *flags = &this->m_playback_flags.m_storage; *flags = 0; }
 { char *location = &this->m_location.m_storage; *location = 0; }
 this->m_yaw = zero;
 this->m_pitch = zero;
 this->m_target_yaw = zero;
 this->m_target_pitch = zero;
 this->m_yaw_min = zero;
 this->m_yaw_max = zero;
 this->m_pitch_min = zero;
 this->m_pitch_max = zero;
 if (this->setup_channel_base(settings))
 {
  const s_channel_limits *limits = mixing_board_get_blend_screen_bounds(this);
  if (limits)
  {
   this->m_yaw_min = -((float)limits->yaw_left_count * limits->yaw_left_scale);
   this->m_yaw_max = (float)limits->yaw_right_count * limits->yaw_right_scale;
   this->m_pitch_min = -((float)limits->pitch_down_count * limits->pitch_down_scale);
   this->m_pitch_max = (float)limits->pitch_up_count * limits->pitch_up_scale;
  }
  if (settings->m_use_angles)
  {
   float yaw = PIN(settings->m_yaw, this->m_yaw_min, this->m_yaw_max);
   this->m_yaw = yaw;
   this->m_target_yaw = yaw;
   float pitch = PIN(settings->m_pitch, this->m_pitch_min, this->m_pitch_max);
   this->m_pitch = pitch;
   this->m_target_pitch = pitch;
  }
 }
 return this->m_enabled && this->m_graph_index != -1 && (unsigned short)this->m_animation_id.m_animation_index < 0x8000;
}

// @address 0x122200 ?setup_animation_channel_by_parallel_impulse@c_animation_manager@@QBA_NPAVc_animation_channel@@JJJK@Z
bool c_animation_manager::setup_animation_channel_by_parallel_impulse(
    c_animation_channel *channel, long weapon_class, long weapon_type, long impulse, unsigned long flags) const
{
 bool result = false;
 if (weapon_class == 226 || weapon_class == -1)
  weapon_class = m_current_goal.weapon_class;
 if (weapon_type == 226 || weapon_type == -1)
  weapon_type = m_current_goal.weapon_type;
 if (m_graph_tag_index != -1)
 {
  c_animation_id animation = animation_manager_get_graph(this)->find_overlay(m_current_goal.mode, weapon_class, weapon_type, impulse, 0, 0, 0);
  long blend_state = -1;
  c_animation_id blend_animation = locate_blend_animation(m_current_goal.mode, weapon_class, weapon_type, impulse, &blend_state);
  if (animation.valid())
  {
   c_animation_channel_settings settings;
   settings.m_graph_index = m_graph_tag_index;
   settings.m_definition_index = impulse;
   settings.m_animation_id = animation;
   settings.m_location = _animation_location_overlay;
   settings.m_playback_flags.set_unsafe((unsigned short)flags);
   settings.m_blend_ids[0] = blend_animation;
   apply_random_permutations_to_settings(&settings);
   result = internal_setup_animation_channel(channel, &settings);
  }
 }
 return result;
}

// @address 0x1213a0 ?setup_auxilary_channel@c_animation_manager@@QBA_NPAVc_auxilary_channel@@JJJK@Z
bool c_animation_manager::setup_auxilary_channel(c_auxilary_channel *channel, long weapon_class, long weapon_type, long overlay, unsigned long
    flags) const
{
 bool result = false;
 if (weapon_class == 226 || weapon_class == -1)
  weapon_class = m_current_goal.weapon_class;
 if (weapon_type == 226 || weapon_type == -1)
  weapon_type = m_current_goal.weapon_type;
 if (m_graph_tag_index != -1)
 {
  c_animation_id animation = animation_manager_get_graph(this)->find_overlay(m_current_goal.mode, weapon_class, weapon_type, overlay, 0, 0, 0);
  char location = 2;
  if (!animation.valid())
  {
   animation = find_state_animation_id(weapon_class, weapon_type, overlay);
   location = 1;
  }
  c_compact_auxilary_channel_settings settings;
  settings.m_graph_index = m_graph_tag_index;
  settings.m_animation_id = animation;
  settings.m_definition_index = overlay;
  settings.m_location = (e_animation_graph_location)location;
  settings.m_playback_flags.set_unsafe((unsigned short)flags);
  settings.m_yaw = k_real_zero_storage;
  result = internal_setup_auxilary_channel(channel, (c_auxilary_channel_settings *)&settings);
 }
 return result;
}

// @address 0x28e078 ??0c_animation_manager@@QAA@XZ
c_animation_manager::c_animation_manager()
{
 initialize(-1, -1);
}

// @address 0x28e0e0 ?initialize@c_animation_manager@@QAAXJJ@Z
void c_animation_manager::initialize(long object_index, long mixing_board_index)
{
    m_object_index = object_index;
    m_state_channel.initialize();
    m_transition_channel.initialize();
    m_flags = 0;
    m_transition_flags = 0;
    m_graph_tag_index = -1;
    m_interpolator.disable();
    m_current_goal.state = -1;
    m_current_goal.weapon_class = -1;
    m_current_goal.weapon_type = -1;
    m_current_goal.mode = -1;
    m_previous_goal = m_current_goal;
    m_mixing_board_type = mixing_board_index;
    if (m_mixing_board_type != -1)
    {
        const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(m_mixing_board_type);
        if (definition)
#if defined(__clang__)
            ::reset_mixing_board(this, get_mixing_board_buffer(), definition);
#else
            ::reset_mixing_board(this, get_mixing_board_address(), definition);
#endif
    }
}

// @address 0x331f4 k_damage_direction_fallback_table
// @rdata 0x820331f4..0x820331fc
const short k_damage_direction_fallback_table[4] = { 1, 3, 0, 2 };
// @rdata-end

static __forceinline bool animation_manager_validate_graph(long graph_tag_index, long model_tag_index)
{
 bool result = false;
 if (graph_tag_index != -1)
 {
  const c_model_animation_graph *graph = (const c_model_animation_graph *)tag_get(0, graph_tag_index);
  const s_model_definition *model = (const s_model_definition *)tag_get(0, model_tag_index);
  result = model->nodes.count - (short)graph->m_nodes.count == 0;
 }
 return result;
}
// @address 0x28e1a8 ?reset_graph@c_animation_manager@@QAA_NJJ_N@Z
bool c_animation_manager::reset_graph(long graph_tag_index, long model_tag_index, bool keep)
{
 bool result = false;
 bool valid = true;
 if (model_tag_index != -1)
  valid = animation_manager_validate_graph(graph_tag_index, model_tag_index);
 if (valid)
 {
  if (keep)
   m_flags |= 2;
  else
   m_flags &= 0xfffd;
  m_graph_tag_index = graph_tag_index;
  m_previous_goal = m_current_goal;
  m_current_goal.weapon_class = -1;
  m_current_goal.weapon_type = -1;
  m_current_goal.mode = -1;
  m_current_goal.state = -1;
  reset_weapon();
  m_state_channel.reset();
  m_transition_channel.reset();
  if (m_mixing_board_type != -1)
   reset_mixing_board();
  set_goal(1, 1, 1, 1, 0xb17, 31);
  m_interpolator.disable();
  m_interpolator.m_interpolation_type = 1;
  m_transition_flags = 0;
  result = true;
 }
 if (result)
  m_flags |= 64;
 else
  m_flags &= 0xffbf;
 return result;
}

static __forceinline const s_animation_entry *animation_manager_find_suspension(const c_model_animation_graph *graph, long key)
{
 return (const s_animation_entry *)find_named_block_entry_with_size(&graph->m_vehicle_suspensions, key, sizeof(s_vehicle_suspension));
}
static __forceinline const s_animation_entry *animation_manager_find_object_overlay(const c_model_animation_graph *graph, long key)
{
 return (const s_animation_entry *)find_named_block_entry_with_size(&graph->m_object_overlays, key, sizeof(s_object_overlay));
}

// @address 0x28e350 ?locate_new_animation_id_on_graph_change@c_animation_manager@@QBA?AVc_animation_id@@V2@JW4e_animation_graph_location@@JJ@Z
c_animation_id c_animation_manager::locate_new_animation_id_on_graph_change(
    c_animation_id previous, long key, e_animation_graph_location location, long weapon_class, long weapon_type) const
{
 c_animation_id result;
 if (previous.valid())
 {
  const c_model_animation_graph *graph = animation_manager_get_graph(this);
  weapon_class = weapon_class != 226 ? weapon_class : m_current_goal.weapon_class;
  weapon_type = weapon_type != 226 ? weapon_type : m_current_goal.weapon_type;
  switch (location)
  {
  case _animation_location_action:
   result = graph->find_action(m_current_goal.mode, weapon_class, weapon_type, key, 0, 0, 0);
   break;
  case _animation_location_overlay:
   result = graph->find_overlay(m_current_goal.mode, weapon_class, weapon_type, key, 0, 0, 0);
   break;
  case _animation_location_suspension:
   {
    const s_animation_entry *entry = animation_manager_find_suspension(graph, key);
    if (entry)
     result = entry->animation;
   }
   break;
  case _animation_location_object_overlay:
   {
    const s_animation_entry *entry = animation_manager_find_object_overlay(graph, key);
    if (entry)
     result = entry->animation;
   }
   break;
  default:
   break;
  }
 }
 return result;
}

// @address 0x28e698 ?internal_setup_animation_channel@c_animation_manager@@ABA_NPAVc_animation_channel@@PAVc_animation_channel_settings@@@Z
bool c_animation_manager::internal_setup_animation_channel(c_animation_channel *channel, c_animation_channel_settings *settings) const
{
 bool result = false;
 if (m_graph_tag_index != -1)
 {
  settings->m_graph_index = m_graph_tag_index;
  if (channel->setup_channel(settings))
   result = true;
 }
 return result;
}

// @address 0x121648 ?internal_setup_auxilary_channel@c_animation_manager@@ABA_NPAVc_auxilary_channel@@PAVc_auxilary_channel_settings@@@Z
static __forceinline void animation_manager_auxilary_clear(c_auxilary_channel *channel, float zero)
{
 channel->m_graph_index = -1;
 channel->m_definition_index = -1;
 channel->m_animation_id.clear();
 { unsigned short *flags = &channel->m_playback_flags.m_storage; *flags = 0; }
 { char *location = &channel->m_location.m_storage; *location = 0; }
 channel->m_frame_position = zero;
}

bool c_animation_manager::internal_setup_auxilary_channel(c_auxilary_channel *channel, c_auxilary_channel_settings *settings) const
{
 bool result = false;
 if (m_graph_tag_index != -1 && (unsigned short)settings->m_animation_id.m_animation_index < 0x8000)
 {
  if (settings->m_playback_flags.test(_animation_playback_unknown4_bit))
   settings->m_animation_id = animation_manager_resolve_animation_id(this, settings->m_animation_id);
  if (settings->m_playback_flags.test(_animation_playback_unknown2_bit))
   settings->m_yaw = k_real_zero_storage;
  settings->m_graph_index = m_graph_tag_index;
  animation_manager_auxilary_clear(channel, k_real_zero_storage);
  if (channel->setup_channel_base(settings))
   channel->m_frame_position = settings->m_yaw;
  if (channel->valid())
   result = true;
 }
 if (!result)
  animation_manager_auxilary_clear(channel, k_real_zero_storage);
 return result;
}

// @address 0x28e6f8 ?setup_animation_channel_by_name@c_animation_manager@@QBA_NPAVc_animation_channel@@JK@Z
bool c_animation_manager::setup_animation_channel_by_name(c_animation_channel *channel, long name, unsigned long flags) const
{
 bool result = false;
 if (m_graph_tag_index != -1)
 {
  const c_model_animation_graph *graph = animation_manager_get_graph(this);
  c_animation_id animation = graph->find_animation(name);
  if (animation.valid())
  {
   c_animation_channel_settings settings;
   settings.m_graph_index = m_graph_tag_index;
   settings.m_definition_index = name;
   settings.m_animation_id = animation;
   settings.m_location = _animation_location_action;
   settings.m_playback_flags.set_unsafe(64);
   apply_random_permutations_to_settings(&settings);
   result = internal_setup_animation_channel(channel, &settings);
  }
 }
 return result;
}

// @address 0x28e7a8 ?find_parallel_impulse_animation_id@c_animation_manager@@QBA?AVc_animation_id@@JJJ@Z
c_animation_id c_animation_manager::find_parallel_impulse_animation_id(long weapon_class, long weapon_type, long impulse) const
{
 c_animation_id result;
 if (m_graph_tag_index != -1)
 {
  c_animation_channel channel;
  if (setup_animation_channel_by_parallel_impulse(&channel, weapon_class, weapon_type, impulse, 0))
   result = channel.m_animation_id;
  animation_channel_initialize_and_disable(&channel);
 }
 return result;
}

// @address 0x28e838 animation_manager_get_auxilary_animation
extern "C" const c_model_animation *animation_manager_get_auxilary_animation(
    const c_animation_manager *manager, long weapon_class, long weapon_type, long impulse)
{
 const c_model_animation *animation = 0;
 c_animation_id id = manager->find_impulse_or_state_animation_id(weapon_class, weapon_type, impulse);
 if (id.valid())
  animation = animation_manager_get_graph(manager)->get_animation(id);
 return animation;
}

// @address 0x28e8c0 ?find_impulse_or_state_animation_id@c_animation_manager@@QBA?AVc_animation_id@@JJJ@Z
c_animation_id c_animation_manager::find_impulse_or_state_animation_id(long weapon_class, long weapon_type, long impulse) const
{
 c_animation_id animation;
 if (weapon_class == 226 || weapon_class == -1)
  weapon_class = m_current_goal.weapon_class;
 if (weapon_type == 226 || weapon_type == -1)
  weapon_type = m_current_goal.weapon_type;
 animation = find_parallel_impulse_animation_id(weapon_class, weapon_type, impulse);
 if (!animation.valid())
  animation = find_state_animation_id(weapon_class, weapon_type, impulse);
 return animation;
}

// @address 0x28e980 ?find_death_or_damage_animation_id@c_animation_manager@@QBA?AVc_animation_id@@JJFFPAF0@Z
c_animation_id c_animation_manager::find_death_or_damage_animation_id(
    long mode, long damage, short desired_direction, short desired_region, short *actual_direction, short *actual_region) const
{
    c_animation_id result;
    const c_model_animation_graph *graph = animation_manager_get_graph(this);
    short original_direction = desired_direction;
    short original_region = desired_region;
    bool search_direction = true;
    bool search_region = true;
    long direction_count = 0;
    const short *direction_table = k_damage_direction_fallback_table;
    const short *region_table = k_damage_region_fallback_table;
    while (search_direction && direction_count < 4)
    {
        ++direction_count;
        search_region = true;
        desired_region = original_region;
        long region_count = 0;
        while (search_region && region_count < 11)
        {
            ++region_count;
            long found_mode, found_class, found_type;
            result = graph->find_damage_animation(mode, m_current_goal.weapon_class, m_current_goal.weapon_type, damage, desired_direction,
                desired_region, &found_mode, &found_class, &found_type);
            if (result.valid())
            {
                search_region = false;
                search_direction = false;
            }
            else
            {
                desired_region = region_table[desired_region];
                if (desired_region == original_region)
                    search_region = false;
            }
        }
        if (!result.valid())
        {
            desired_direction = direction_table[desired_direction];
            if (desired_direction == original_direction)
                search_direction = false;
        }
    }
    if (result.valid())
    {
        if (actual_direction)
            *actual_direction = desired_direction;
        if (actual_region)
            *actual_region = desired_region;
    }
    return result;
}

// (name unknown) true when the graph has a damage animation for one of the three damage kinds (modes 81, 82, 83).

// @address 0x28eb20 ?setup_animation_channel_by_death_or_damage@c_animation_manager@@QBA_NPAVc_animation_channel@@JJFFPAF1@Z
bool c_animation_manager::setup_animation_channel_by_death_or_damage(c_animation_channel *channel, long mode, long damage,
    short desired_direction, short desired_region, short *actual_direction, short *actual_region) const
{
 bool result = false;
 if (mode == 226)
  mode = m_current_goal.mode;
 c_animation_id animation = find_death_or_damage_animation_id(mode, damage, desired_direction, desired_region, actual_direction, actual_region);
 if (animation.valid())
 {
  c_animation_channel_settings settings;
  settings.m_graph_index = m_graph_tag_index;
  settings.m_definition_index = damage;
  settings.m_animation_id = animation;
  settings.m_location = _animation_location_damage;
  settings.m_playback_flags.set_unsafe(29);
  apply_random_permutations_to_settings(&settings);
  result = internal_setup_animation_channel(channel, &settings);
 }
 return result;
}

// @address 0x28ebc8 ?build_model_mapping_table@c_animation_manager@@QAA_NJPAJ0J@Z
bool c_animation_manager::build_model_mapping_table(long model_tag_index, long *node_count, long *mapping, long maximum_node_count)
{
 struct c_model_animation_graph_node { long name; char pad[28]; };
 struct s_model_definition_node { long name; char pad[92]; };
 struct c_model_animation_graph_nodes { char pad[20]; long count; TAG_POINTER(c_model_animation_graph_node) nodes; };
 struct s_model_definition_nodes { char pad[48]; long count; TAG_POINTER(s_model_definition_node) nodes; };
 const c_model_animation_graph *definition = animation_manager_get_graph(this);
 const c_model_animation_graph_nodes *graph = (const c_model_animation_graph_nodes *)definition;
 const s_model_definition_nodes *model = (const s_model_definition_nodes *)tag_get(0, model_tag_index);
 bool result = true;
 *node_count = model->count;
 for (long model_node_index = 0; model_node_index < model->count; ++model_node_index)
 {
  const s_model_definition_node *model_node = &model->nodes[model_node_index];
  long graph_node_index = -1;
  for (long node_index = 0; node_index < graph->count; ++node_index)
  {
   if (model_node->name == graph->nodes[node_index].name)
   {
    graph_node_index = node_index;
    break;
   }
  }
  mapping[model_node_index] = graph_node_index;
  if (graph_node_index == -1)
   result = false;
 }
 return result;
}
// @address 0x28ec88 animation_manager_has_acceleration_overlay
bool animation_manager_has_acceleration_overlay(const c_animation_manager *manager)
{
 const c_model_animation_graph *definition = (const c_model_animation_graph *)tag_get(0, manager->m_graph_tag_index);
 c_animation_id animation = definition->find_overlay(
     manager->m_current_goal.mode, manager->m_current_goal.weapon_class, manager->m_current_goal.weapon_type, 81, 0, 0, 0);
 if (!animation.valid())
 {
  animation = definition->find_overlay(manager->m_current_goal.mode, manager->m_current_goal.weapon_class, manager->m_current_goal.weapon_type,
      82, 0, 0, 0);
  if (!animation.valid())
   animation = definition->find_overlay(manager->m_current_goal.mode, manager->m_current_goal.weapon_class, manager->m_current_goal.weapon_type,
       83, 0, 0, 0);
 }
 return animation.valid();
}


// the impulse animation id of a mode (0, 1 or 2) for an object's graph; both
// output shorts are -1 unless the lookup finds one
#if defined(_XBOX) && !defined(__clang__)
static __forceinline const c_animation_id *animation_manager_overlay_address(const c_animation_id &id)
{
 return &id;
}
#endif

// @address 0x28ed80 animation_manager_find_acceleration_overlay_id
// match: 0x28ed80: pointer __restrict qualifier prevents alias reload
extern "C" short *animation_manager_find_acceleration_overlay_id(short *__restrict out, const c_animation_manager *request, int mode)
{
#if defined(_XBOX) && !defined(__clang__)
 const c_animation_id *found;
 int graph_index = request->m_graph_tag_index;
 out[0] = -1;
 const c_model_animation_graph *definition = (const c_model_animation_graph *)((char
     *)global_tag_instances[datum_index_to_absolute_index(graph_index)].definition);
 out[1] = -1;
 if (mode == 1)
  found = animation_manager_overlay_address(definition->find_overlay(request->m_current_goal.mode, request->m_current_goal.weapon_class,
      request->m_current_goal.weapon_type, 82, 0, 0, 0));
 else if (mode == 0)
  found = animation_manager_overlay_address(definition->find_overlay(request->m_current_goal.mode, request->m_current_goal.weapon_class,
      request->m_current_goal.weapon_type, 81, 0, 0, 0));
 else if (mode == 2)
  found = animation_manager_overlay_address(definition->find_overlay(request->m_current_goal.mode, request->m_current_goal.weapon_class,
      request->m_current_goal.weapon_type, 83, 0, 0, 0));
 else
  return out;
 out[0] = found->m_graph_index;
 out[1] = found->m_animation_index;
#else
 int graph_index = request->m_graph_tag_index;
 out[0] = -1;
 const c_model_animation_graph *definition = (const c_model_animation_graph *)((char
     *)global_tag_instances[datum_index_to_absolute_index(graph_index)].definition);
 out[1] = -1;
 int action;
 if (mode == 1)
  action = 82;
 else if (mode == 0)
  action = 81;
 else if (mode == 2)
  action = 83;
 else
  return out;
 c_animation_id found = definition->find_overlay(request->m_current_goal.mode, request->m_current_goal.weapon_class,
     request->m_current_goal.weapon_type, action, 0, 0, 0);
 out[0] = found.m_graph_index;
 out[1] = found.m_animation_index;
#endif
 return out;
}

// (name unknown) the playback ratio of a valid channel, 0 otherwise.
// @address 0x28ee88 ?set_custom_goal@c_animation_manager@@QAA_NVc_animation_id@@J@Z
bool c_animation_manager::set_custom_goal(c_animation_id id, long flags)
{
 set_goal(226, 226, 226, 182, 4, 29);
 bool result = animation_manager_setup_animation_channel_by_index(this, &m_state_channel, id, flags);
 if (result)
  m_transition_channel.reset();
 return result;
}

// @address 0x28ef18 animation_channel_get_playback_ratio
float animation_channel_get_playback_ratio(const c_animation_channel *channel)
{
 float result = 0.0f;
 if (channel->valid())
 {
  if (channel->valid())
   result = channel->m_playback_ratio;
 }
 return result;
}

// @address 0x28ef98 ?set_playback_speed@c_animation_channel@@QAAXM@Z
void c_animation_channel::set_playback_speed(float speed)
{
 if (valid())
  m_playback_speed = speed > 0.0001f ? speed : 0.0001f;
}

// @address 0x28eff0 ?find_goal@c_animation_manager@@QBA_NJJJJJ@Z
bool c_animation_manager::find_goal(long mode, long weapon_class, long weapon_type, long state, long flags) const
{
    c_animation_id animation_id;
    s_animation_goal_settings filtered_settings;
    s_animation_goal_settings actual_settings;
    internal_validate_goal_state(mode, 226, 226, state, 0, &filtered_settings,
        &actual_settings, &animation_id);
    return animation_id.valid();
}


// (name unknown) the fraction of the way value lies from start to end, where the range may wrap around length.
// @address 0x28fa80 animation_manager_compute_wrapped_range_fraction
bool animation_manager_compute_wrapped_range_fraction(float value, float start, float end, float length, float *fraction)
{
    bool result = false;
    if (start <= end)
    {
        if (value >= start && value <= end)
        {
            float range = end - start;
            result = true;
            if (range > 0.0f)
                *fraction = (value - start) / range;
            else
                *fraction = 1.0f;
        }
    }
    else
    {
        float total = length - start + end;
        if (value >= start)
        {
            *fraction = (value - start) / total;
            result = true;
        }
        else if (value <= end)
        {
            *fraction = (length - start + value) / total;
            result = true;
        }
    }
    *fraction = MIN(MAX(*fraction, 0.0f), 1.0f);
    return result;
}

// @address 0x28f1a8 ?set_state_position_to_last_frame@c_animation_manager@@QAAXXZ
void c_animation_manager::set_state_position_to_last_frame()
{
 m_transition_channel.reset();
 if (m_state_channel.valid())
  m_state_channel.jump_to_last_frame_and_stop();
 m_interpolator.disable();
 pause_state_channel(true);
}

// @address 0x28f938 animation_manager_update_facial_channels


static __forceinline bool animation_manager_facial_has_frames(const c_facial_animation_channel *channel)
{
    return channel->m_lipsync_frames_remaining.m_value > 0;
}
static __forceinline bool animation_manager_facial_needs_update(const c_facial_animation_channel *channel)
{
    // match: 0x28f938: the guarded flag tests extract the gesture bit before the branch on the emotion bit.
    bool emotion_in_progress = channel->m_state_flags.valid_bit(_facial_animation_state_emotion_bit) &&
        channel->m_state_flags.test(_facial_animation_state_emotion_bit);
    bool gesture_in_progress = channel->m_state_flags.valid_bit(_facial_animation_state_gesture_bit) &&
        channel->m_state_flags.test(_facial_animation_state_gesture_bit);
    return emotion_in_progress || gesture_in_progress || animation_manager_facial_has_frames(channel) || channel->m_sound_index != -1;
}

#ifdef HALO3_64BIT
extern "C" int animation_manager_update_facial_channels(c_animation_manager *manager, void *argument)
#else
extern "C" int animation_manager_update_facial_channels(c_animation_manager *manager, int argument)
#endif
{
    int result = 0;
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(manager->m_mixing_board_type);
    if (definition)
    {
        c_mixing_board_component_iterator<c_facial_animation_channel> iterator;
        c_facial_animation_channel *channel;
        iterator.setup(manager->get_mixing_board_buffer(), definition, k_mixing_board_facial_component_type);
        while (iterator.next(&channel))
        {
            if (channel->valid() && animation_manager_facial_needs_update(channel))
            {
                result = 1;
                animation_data_channel_update_flags(channel, argument);
            }
        }
    }
    return result;
}


// @address 0xd6f50 ?update_mixing_board_components@c_animation_manager@@QBA_NP6AXPBUs_animation_event_data@@J@ZJ@Z
bool c_animation_manager::update_mixing_board_components(void (*event_callback)(const s_animation_event_data *, long), long event_data) const
{
 event_callback = object_animation_callback;
 bool result = false;
 const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(m_mixing_board_type);
 if (definition)
 {
  c_mixing_board_component_iterator<c_mixing_board_slider> slider_iterator;
  c_mixing_board_slider *slider;
  slider_iterator.setup(get_mixing_board_buffer(), definition, 0);
  while (slider_iterator.next(&slider))
   result |= mixing_board_slider_update(slider);
  c_mixing_board_component_iterator<c_animation_channel> channel_iterator;
  c_animation_channel *channel;
  channel_iterator.setup(get_mixing_board_buffer(), definition, 1);
  while (channel_iterator.next(&channel))
  {
   if (!channel->valid())
    continue;
   result |= channel->update(this, event_callback, event_data);
  }
  c_mixing_board_component_iterator<c_aiming_channel> aiming_iterator;
  c_aiming_channel *aiming;
  aiming_iterator.setup(get_mixing_board_buffer(), definition, 3);
  while (aiming_iterator.next(&aiming))
  {
   if (!aiming->valid())
    continue;
   bool changed = false;
   float target_yaw = aiming->m_target_yaw;
   float yaw = aiming->m_yaw;
   float yaw_difference = target_yaw - yaw;
   if (fabsf(yaw_difference) >= 0.0001f)
   {
    aiming->m_yaw = (target_yaw - yaw) * k_real_third + yaw;
    changed = true;
   }
   else
    aiming->m_yaw = target_yaw;
   float target_pitch = aiming->m_target_pitch;
   float pitch = aiming->m_pitch;
   float pitch_difference = target_pitch - pitch;
   if (fabsf(pitch_difference) >= 0.0001f)
   {
    aiming->m_pitch = (target_pitch - pitch) * k_real_third + pitch;
    changed = true;
   }
   else
    aiming->m_pitch = target_pitch;
   result |= changed;
  }
  c_mixing_board_component_iterator<c_facial_animation_channel> facial_iterator;
  c_facial_animation_channel *facial;
  facial_iterator.setup(get_mixing_board_buffer(), definition, k_mixing_board_facial_component_type);
  while (facial_iterator.next(&facial))
  {
   if (!((const c_channel_base *)facial)->valid())
    continue;
   facial->update();
  }
 }
 return result;
}

// @address 0x28f240 ?get_mixing_board_node_orientations@c_animation_manager@@QAAXPBUrender_model_definition@@MPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@Vc_animation_id@@PBKM@ZJ@Z
void c_animation_manager::get_mixing_board_node_orientations(const render_model_definition *render_model, float blend_ratio,
    const c_static_flags<255> *node_mask, long node_orientations_count, real_orientation *node_orientations,
    animation_manager_channel_callback callback, long user_data)
{
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(this->m_mixing_board_type);
    if (!definition)
        return;
    {
        c_mixing_board_component_iterator<c_animation_channel> iterator;
        c_animation_channel *channel;
        iterator.setup(get_mixing_board_buffer(), definition, 1);
        while (iterator.next(&channel))
        {
            if (!channel->valid())
                continue;
            float weight = blend_ratio;
            if (channel->parent_slider_valid())
            {
                t_short_fixed<13> slider_value = this->get_slider_value_from_chain(channel->get_parent_slider_index(),
                    channel->get_parent_slider_type());
                weight = slider_value.export_real() * weight;
            }
            ((c_animation_channel *)channel)
                ->apply_weighted_node_orientations(
                    render_model, weight, node_mask, node_orientations_count, node_orientations, 0, user_data);
        }
    }
    {
        c_mixing_board_component_iterator<c_auxilary_channel> aux_iterator;
        c_auxilary_channel *aux_channel;
        aux_iterator.setup(get_mixing_board_buffer(), definition, 2);
#if defined(_XBOX) && !defined(__clang__)
        const float half = k_real_half_pool[0];
#else
        const float half = 0.5f;
#endif
        while (aux_iterator.next(&aux_channel))
        {
            if (!aux_channel->valid())
                continue;
            float aux_weight = blend_ratio;
            if (aux_channel->parent_slider_valid())
            {
                t_short_fixed<13> slider_value = this->get_slider_value_from_chain(
                    aux_channel->get_parent_slider_index(), aux_channel->get_parent_slider_type());
                aux_weight = slider_value.export_real() * aux_weight;
            }
            if (!aux_channel->valid())
                continue;
            if ((bool)((aux_channel->m_playback_flags.get_unsafe() >> 15) & 1))
            {
                float playback_ratio = ((const c_auxilary_channel *)aux_channel)->get_playback_ratio();
#if defined(_XBOX) && !defined(__clang__)
                // match: 0x28f240: 0.5f and 0.0001f load off one 0x82022dc0 GPR base (literal: a second lis in the loop)
                if (fabsf(playback_ratio - half) < k_real_half_pool[1747])
#else
                if (fabsf(playback_ratio - half) < 0.0001f)
#endif
                    continue;
            }
            ((c_channel_base *)aux_channel)
                ->internal_apply_weighted_node_orientations(aux_channel->m_animation_id, aux_channel->m_frame_position, render_model,
                    aux_weight, node_mask, node_orientations_count, node_orientations, 0, user_data, false);
        }
    }
    {
        c_mixing_board_component_iterator<c_aiming_channel> aim_iterator;
        c_aiming_channel *aim_channel;
        aim_iterator.setup(get_mixing_board_buffer(), definition, 3);
        while (aim_iterator.next(&aim_channel))
        {
            if (!aim_channel->valid())
                continue;
            float aim_weight = blend_ratio;
            if (aim_channel->parent_slider_valid())
            {
                t_short_fixed<13> slider_value = this->get_slider_value_from_chain(
                    aim_channel->get_parent_slider_index(), aim_channel->get_parent_slider_type());
                aim_weight = slider_value.export_real() * aim_weight;
            }
            if (!aim_channel->valid())
                continue;
            const c_model_animation_graph *graph = (const c_model_animation_graph *)tag_get(0, aim_channel->m_graph_index);
            const animation_aiming_screen_bounds *bounds = graph->get_blend_screen_bounds(aim_channel->m_animation_id);
            if (!bounds)
                continue;
            const c_static_flags<255> *channel_node_mask =
                ((const c_channel_base *)aim_channel)->prepare_node_mask_flags(0, node_orientations_count);
            c_static_flags_no_init<255> combined_node_mask;
            const c_static_flags_no_init<255> *effective_node_mask;
            if (combine_node_flag_masks(channel_node_mask, node_mask, &combined_node_mask))
                effective_node_mask = &combined_node_mask;
            else
                effective_node_mask = 0;
            compute_node_orientations_from_blend_screen(aim_channel->m_graph_index, aim_channel->m_animation_id, node_orientations_count,
                aim_channel->m_yaw, aim_channel->m_pitch, bounds, aim_weight, (const flags256 *)effective_node_mask, node_orientations);
        }
    }
}


static __forceinline t_short_fixed<13> animation_manager_slider_value_from_chain(
    const c_animation_manager *manager, long slider_index, e_mixing_board_parent_slider_type type)
{
    t_short_fixed<13> value = manager->get_slider_value_from_chain(slider_index, type);
    return value;
}
// @address 0x28f788 ?apply_facial_node_orientations@c_animation_manager@@QAAXPBUrender_model_definition@@MPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@Vc_animation_id@@PBKM@ZJ@Z
void c_animation_manager::apply_facial_node_orientations(const render_model_definition *render_model, float blend_ratio,
    const c_static_flags<255> *node_mask, long node_orientations_count, real_orientation *node_orientations,
    animation_node_orientations_callback animation_function, long user_data)
{
    const c_mixing_board_definition *definition = mixing_board_definition_get_by_type(m_mixing_board_type);
    if (definition)
    {
        c_mixing_board_component_iterator<c_facial_animation_channel> iterator;
        c_facial_animation_channel *channel;
        iterator.setup(get_mixing_board_buffer(), definition, k_mixing_board_facial_component_type);
        while (iterator.next(&channel))
        {
            if (channel->valid())
            {
                float weight = blend_ratio;
                if (channel->parent_slider_valid())
                    weight *= animation_manager_slider_value_from_chain(
                        this, channel->get_parent_slider_index(), channel->get_parent_slider_type())
                                  .export_real();
                ((c_facial_animation_channel *)channel)
                    ->apply_node_orientations(render_model, weight, node_mask, node_orientations_count, node_orientations, 0, user_data);
            }
        }
    }
}

// @address 0x28fb38 ?get_current_foot_tracking_state@c_animation_manager@@QBA_NJPAV?$c_enum@W4e_foot_tracking_state@@E$0A@$03@@PAMPAF@Z
bool c_animation_manager::get_current_foot_tracking_state(
    long type, c_enum<e_foot_tracking_state, unsigned char, 0, 4> *state, float *fraction, short *frame) const
{
    bool result = false;
    if (m_state_channel.valid())
    {
        const c_model_animation *a = m_state_channel.get_animation();
        float position = m_state_channel.m_frame_position;
        float count = (float)a->get_frame_count();
        for (long i = 0; i < a->feet.count(); ++i)
        {
            const s_foot_tracking_foot *f = &a->feet[i];
            if (f->type == type)
            {
                for (long j = 0; j < f->cycle_count; ++j)
                {
                    const s_foot_tracking_cycle *v = &f->cycles[j];
                    if (animation_manager_compute_wrapped_range_fraction(position, (float)v->start, (float)v->plant, count, fraction))
                    {
                        *state = _foot_tracking_plant;
                        *frame = v->plant;
                        if (v->plant < v->start && position <= v->plant)
                            *frame = (short)m_state_channel.get_frame_index();
                        result = true;
                    }
                    else if (animation_manager_compute_wrapped_range_fraction(position, (float)v->plant, (float)v->unplant, count, fraction))
                    {
                        *state = _foot_tracking_on;
                        *frame = v->plant;
                        *fraction = 1.0f;
                        if (v->unplant < v->plant && position <= v->unplant)
                            *frame = (short)m_state_channel.get_frame_index();
                        result = true;
                    }
                    else if (animation_manager_compute_wrapped_range_fraction(position, (float)v->unplant, (float)v->end, count, fraction))
                    {
                        *state = _foot_tracking_unplant;
                        *frame = v->plant;
                        *fraction = 1.0f - *fraction;
                        if (v->end < v->unplant && position <= v->end)
                            *frame = (short)m_state_channel.get_frame_index();
                        result = true;
                    }
                    else if (animation_manager_compute_wrapped_range_fraction(position, (float)v->end, (float)v->start, count, fraction))
                    {
                        *state = _foot_tracking_none;
                        *frame = v->plant;
                        *fraction = 0.0f;
                        if (v->start < v->end && position <= v->start)
                            *frame = (short)m_state_channel.get_frame_index();
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x28ff10 ?set_goal@c_animation_manager@@QAA_NJJJJJK@Z
bool c_animation_manager::set_goal(long mode, long weapon_class, long weapon_type, long state, long flags, unsigned long playback_flags)
{
 bool result = false;
 s_animation_state_change_settings settings;
 settings.flags.clear();
 if (recommend_state_change(mode, weapon_class, weapon_type, state, flags, playback_flags, &settings))
  result = execute_state_change(&settings);
 return result;
}

// @address 0x28ffa0 ?set_target_emotion_by_name@c_animation_manager@@QAAXJMM@Z
void c_animation_manager::set_target_emotion_by_name(long name, float value, float time)
{
    c_facial_animation_channel *channel = get_mixing_board_facial_animation_channel();
    if (channel)
        channel->set_target_emotion_by_name(name, value, time);
}

// @address 0xd63a0 animation_manager_update_interpolator

bool animation_manager_update_interpolator(c_animation_manager *manager)
{
    bool result = manager->m_interpolator.update();
    if (manager->m_interpolator.finished())
        manager->m_interpolator.disable();
    return result;
}

// @address 0xe84a8 ?internal_validate_goal_state@c_animation_manager@@ABA_NJJJJJPAUs_animation_goal_settings@@0PAVc_animation_id@@@Z
static __forceinline void *animation_manager_find_weapon_type(const c_model_animation_graph *graph, long mode, long weapon_class, long weapon_type)
{
    const c_weapon_class *weapon_class_definition = graph->find_weapon_class(mode, weapon_class);
    if (weapon_class_definition)
        return find_named_block_entry_with_size(&weapon_class_definition->m_weapon_types, weapon_type, sizeof(c_weapon_type));
    return 0;
}

bool c_animation_manager::internal_validate_goal_state(long mode, long weapon_class, long weapon_type, long state, long flags,
    // match: 0xe84a8: __restrict keeps filtered_settings live in registers across find_action instead of spilling
    struct s_animation_goal_settings *__restrict filtered_settings, struct s_animation_goal_settings *__restrict actual_settings,
    class c_animation_id *animation_id) const
{
    bool valid = false;
    const c_model_animation_graph *graph = (const c_model_animation_graph *)((char
        *)global_tag_instances[datum_index_to_absolute_index(this->m_graph_tag_index)].definition);
    internal_determine_goal_state(mode, weapon_class, weapon_type, state, filtered_settings);
    *actual_settings = *filtered_settings;
    *animation_id = graph->find_action(filtered_settings->mode, filtered_settings->weapon_class, filtered_settings->weapon_type,
        filtered_settings->state,
        &actual_settings->mode, &actual_settings->weapon_class, &actual_settings->weapon_type);
    if (animation_id->valid()) {
        valid = true;
    } else {
        if (flags & 2) {
            *animation_id = graph->find_action(filtered_settings->mode, filtered_settings->weapon_class, filtered_settings->weapon_type, 12,
                &actual_settings->mode, &actual_settings->weapon_class, &actual_settings->weapon_type);
            if (animation_id->valid()) {
                actual_settings->state = 12;
                valid = true;
            }
        }
        if (!animation_id->valid() && (flags & 4)) {
            if (animation_manager_find_weapon_type(graph, filtered_settings->mode, filtered_settings->weapon_class, filtered_settings->weapon_type)) {
                valid = true;
            } else if (animation_manager_find_weapon_type(graph, filtered_settings->mode, filtered_settings->weapon_class, 200)) {
                valid = true;
                actual_settings->weapon_type = 200;
            } else if (animation_manager_find_weapon_type(graph, filtered_settings->mode, 200, filtered_settings->weapon_type)) {
                valid = true;
                actual_settings->weapon_class = 200;
            } else if (animation_manager_find_weapon_type(graph, filtered_settings->mode, 200, 200)) {
                valid = true;
                actual_settings->weapon_class = 200;
                actual_settings->weapon_type = 200;
            } else if (animation_manager_find_weapon_type(graph, 200, 200, 200)) {
                valid = true;
                actual_settings->mode = 200;
                actual_settings->weapon_class = 200;
                actual_settings->weapon_type = 200;
            }
        }
    }
    if (!valid && state == 182 && (flags & 4)) {
        actual_settings->mode = 138;
        actual_settings->weapon_class = 200;
        actual_settings->weapon_type = 200;
        actual_settings->state = 182;
        valid = true;
    }
    return valid;
}

// @address 0x110730 ?apply_transition_delta@c_animation_manager@@AAAXMPAUreal_orientation@@@Z
static __forceinline bool transition_channel_valid(const c_channel_base *channel)
{
    return channel->m_enabled && channel->m_graph_index != -1 && (unsigned short)channel->m_animation_id.m_animation_index < 0x8000;
}

static __forceinline void animation_manager_apply_root_delta(const real_vector3d *translation, float ratio, float scale, real_orientation
    *orientation)
{
    real_vector3d delta;
    delta.i = translation->i * (ratio * scale);
    delta.j = translation->j * (ratio * scale);
    delta.k = translation->k * (ratio * scale);
    orientation->translation.n[0] += delta.i;
    orientation->translation.n[1] += delta.j;
    orientation->translation.n[2] += delta.k;
}

void c_animation_manager::apply_transition_delta(float scale, real_orientation *orientation)
{
    if ((m_transition_flags & 2) == 0)
        return;

    if (!transition_channel_valid(&m_transition_channel))
        return;

    float ratio = 0.0f;
    if (transition_channel_valid(&m_transition_channel))
        ratio = m_transition_channel.m_playback_ratio;

    animation_manager_apply_root_delta(&m_transition_translation, ratio, scale, orientation);
}

// @address 0x28f050 ?get_node_orientations@c_animation_manager@@QAAXPBUrender_model_definition@@MPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@P6AXJPBVc_model_animation_graph@@Vc_animation_id@@PBKM@ZJ@Z
void c_animation_manager::get_node_orientations(const render_model_definition *render_model, float blend_ratio,
    const c_static_flags<255> *node_mask, long node_orientations_count, real_orientation *node_orientations,
    animation_manager_channel_callback callback, long user_data)
{
    // match: 0x28f050: the channels get no callback (every caller passes 0; March forwards callback): the image loads li 0
    if (m_graph_tag_index == -1)
        return;
    if (transition_channel_valid(&m_transition_channel))
    {
        m_transition_channel.apply_weighted_node_orientations(render_model, blend_ratio, node_mask, node_orientations_count, node_orientations,
            0, user_data);
        if (node_orientations_count > 0 && (!node_mask || node_mask->test(0)))
            apply_transition_delta(blend_ratio, node_orientations);
    }
    else if (transition_channel_valid(&m_state_channel))
    {
        m_state_channel.apply_weighted_node_orientations(render_model, blend_ratio, node_mask, node_orientations_count, node_orientations, 0,
            user_data);
    }
    get_mixing_board_node_orientations(render_model, blend_ratio, node_mask, node_orientations_count, node_orientations, 0, user_data);
}


static __forceinline void animation_manager_set_id_components(c_animation_id *destination, short graph_index, short animation_index)
{
 destination->m_animation_index = animation_index;
 destination->m_graph_index = graph_index;
}

static __forceinline void animation_manager_assign_blend_id(c_animation_channel_settings *settings, long blend_index, const c_animation_id &source)
{
 animation_manager_set_id_components(&settings->m_blend_ids[blend_index], source.m_graph_index, source.m_animation_index);
}

static __forceinline bool animation_manager_channel_valid_on_graph_change(const c_animation_channel *channel)
{
 return channel->m_enabled && channel->m_graph_index != -1 && (unsigned short)channel->m_animation_id.m_animation_index < 0x8000;
}

// @address 0x2fa988 ?set_frame_position_from_playback_ratio@c_animation_channel@@QAAXM@Z
static __forceinline float animation_manager_channel_playback_ratio(const c_animation_channel *channel)
{
 float result = 0.0f;
 if (animation_manager_channel_valid_on_graph_change(channel))
  result = channel->m_playback_ratio;
 return result;
}

// match: 0x28e4c8: keep the setup path inlined here while 0x28e698 and 0xed798 stay out of line.
static __forceinline bool animation_manager_setup_on_graph_change(
    const c_animation_manager *manager, c_animation_channel *channel, c_animation_channel_settings *settings)
{
 bool result = false;
 if (manager->m_graph_tag_index != -1)
 {
  settings->m_graph_index = manager->m_graph_tag_index;
  if (channel->setup_channel(settings))
   result = true;
 }
 return result;
}

// @address 0x28e4c8 ?update_animation_channel_on_graph_change@c_animation_manager@@QBA_NPAVc_animation_channel@@JJ@Z
bool c_animation_manager::update_animation_channel_on_graph_change(c_animation_channel *channel, long weapon_class, long weapon_type) const
{
 bool result = false;
 if (animation_manager_channel_valid_on_graph_change(channel))
 {
  c_animation_channel_settings settings;
  channel->get_channel_settings(&settings);
  settings.m_graph_index = m_graph_tag_index;
  settings.m_animation_id = locate_new_animation_id_on_graph_change(settings.m_animation_id, settings.m_definition_index,
      (e_animation_graph_location)(signed char)settings.m_location.m_value, weapon_class, weapon_type);
  for (long blend_index = 0; blend_index < 2; ++blend_index)
  {
   short &animation_index = settings.m_blend_ids[blend_index].m_animation_index;
   if ((unsigned short)animation_index < 0x8000)
   {
    animation_manager_assign_blend_id(&settings, blend_index,
        locate_new_animation_id_on_graph_change(settings.m_blend_ids[blend_index], settings.m_definition_index,
            (e_animation_graph_location)(signed char)settings.m_location.m_value, weapon_class, weapon_type));
    if ((unsigned short)animation_index >= 0x8000)
     settings.m_blend_ratios[blend_index] = 0.0f;
   }
  }
  if (settings.m_animation_id.valid())
  {
   float playback_ratio = animation_manager_channel_playback_ratio(channel);
   result = animation_manager_setup_on_graph_change(this, channel, &settings);
   if (result)
    channel->set_frame_position_from_playback_ratio(playback_ratio);
  }
  if (!result)
   channel->reset();
 }
 return result;
}


// an object's identity in a scenario (unique id, origin bsp, type and source)


// @address 0x1d38f0 map_variant_ensure_required_objects
extern "C" void map_variant_ensure_required_objects(c_map_variant *map_variant)
{
  map_variant_update_flag_bomb_placements(map_variant, 0);
  map_variant_update_flag_bomb_placements(map_variant, 6);
  long asymmetric = 0;
  long symmetric = 0;
  long scenario_count = map_variant->m_number_of_scenario_objects;
  long first = map_variant->object_type_start[10];
  for (long index = first; index < scenario_count; index++) {
    s_variant_object_datum *object = &map_variant->objects[index];
    if (object->properties.teleporter_channel == 20 && !object->flags.test(_variant_object_placement_scenario_object_removed_bit)) {
      if (object->properties.placement_flags.test(_variant_placement_symmetric_bit)) symmetric++;
      if (object->properties.placement_flags.test(_variant_placement_asymmetric_bit)) asymmetric++;
    }
  }
  for (long index = map_variant->m_number_of_scenario_objects; index < map_variant->m_number_of_variant_objects; index++) {
    s_variant_object_datum *object = &map_variant->objects[index];
    if (!object->flags.is_empty() && object->properties.teleporter_channel == 20) {
      if (object->properties.placement_flags.test(_variant_placement_symmetric_bit)) symmetric++;
      if (object->properties.placement_flags.test(_variant_placement_asymmetric_bit)) asymmetric++;
    }
  }
  if (!symmetric || !asymmetric) {
    for (long index = first; index < scenario_count; index++) {
      s_variant_object_datum *object = &map_variant->objects[index];
      if (object->properties.teleporter_channel == 20) {
        const s_scenario_multiplayer_object_properties *properties = map_variant_try_and_get_multiplayer_properties(map_variant, index);
        if (properties && (properties->symmetry == 0 || (properties->symmetry == 2 && !asymmetric) || (properties->symmetry == 1 && !symmetric))) {
          object->flags.set_unsafe((object->flags.get_unsafe() & 0xffd6) | 9);
          object->properties.team_designator = static_cast<e_variant_team_designator>(_multiplayer_team_designator_none);
          object->properties.placement_flags.set(_variant_placement_symmetric_bit, true);
          object->properties.placement_flags.set(_variant_placement_asymmetric_bit, true);
          if (properties->symmetry != 0) {
            if (properties->symmetry == 2) {
              object->properties.placement_flags.set(_variant_placement_symmetric_bit, false);
              asymmetric++;
            } else {
              symmetric++;
              object->properties.placement_flags.set(_variant_placement_asymmetric_bit, false);
            }
          } else {
            symmetric++;
            asymmetric++;
          }
          if (symmetric && asymmetric) break;
        }
      }
    }
  }
  {
    long count = 0;
    for (long index = first; index < scenario_count; index++) {
      s_variant_object_datum *object = &map_variant->objects[index];
      if (object->properties.teleporter_channel == 18 && !object->flags.test(_variant_object_placement_scenario_object_removed_bit)) count++;
    }
    if (!count) {
      for (long index = map_variant->m_number_of_scenario_objects; index < map_variant->m_number_of_variant_objects; index++) {
        s_variant_object_datum *object = &map_variant->objects[index];
        if (!object->flags.is_empty() && object->properties.teleporter_channel == 18) count++;
      }
      if (!count) {
        for (long index = first; index < scenario_count; index++) {
          s_variant_object_datum *object = &map_variant->objects[index];
          if (object->properties.teleporter_channel == 18 && object->flags.test(_variant_object_placement_scenario_object_removed_bit)) {
            object->flags.set_unsafe((object->flags.get_unsafe() & 0xffd6) | 9);
            break;
          }
        }
      }
    }
  }
  {
    long count = 0;
    for (long index = first; index < scenario_count; index++) {
      s_variant_object_datum *object = &map_variant->objects[index];
      if (object->properties.teleporter_channel == 14 && !object->flags.test(_variant_object_placement_scenario_object_removed_bit)) count++;
    }
    if (!count) {
      for (long index = map_variant->m_number_of_scenario_objects; index < map_variant->m_number_of_variant_objects; index++) {
        s_variant_object_datum *object = &map_variant->objects[index];
        if (!object->flags.is_empty() && object->properties.teleporter_channel == 14) count++;
      }
      if (!count) {
        for (long index = first; index < scenario_count; index++) {
          s_variant_object_datum *object = &map_variant->objects[index];
          if (object->properties.teleporter_channel == 14 && object->flags.test(_variant_object_placement_scenario_object_removed_bit)) {
            object->flags.set_unsafe((object->flags.get_unsafe() & 0xffd6) | 9);
            break;
          }
        }
      }
    }
  }
  {
    long count = 0;
    for (long index = first; index < scenario_count; index++) {
      s_variant_object_datum *object = &map_variant->objects[index];
      if (object->properties.teleporter_channel == 23 && !object->flags.test(_variant_object_placement_scenario_object_removed_bit)) count++;
    }
    if (!count) {
      for (long index = map_variant->m_number_of_scenario_objects; index < map_variant->m_number_of_variant_objects; index++) {
        s_variant_object_datum *object = &map_variant->objects[index];
        if (!object->flags.is_empty() && object->properties.teleporter_channel == 23) count++;
      }
      if (!count) {
        for (long index = first; index < scenario_count; index++) {
          s_variant_object_datum *object = &map_variant->objects[index];
          if (object->properties.teleporter_channel == 23 && object->flags.test(_variant_object_placement_scenario_object_removed_bit)) {
            object->flags.set_unsafe((object->flags.get_unsafe() & 0xffd6) | 9);
            break;
          }
        }
      }
    }
  }
  asymmetric = 0;
  symmetric = 0;
for (long index = first; index < scenario_count; index++) {
    s_variant_object_datum *object = &map_variant->objects[index];
    if (object->properties.teleporter_channel == 22 && !object->flags.test(_variant_object_placement_scenario_object_removed_bit)) {
      if (object->properties.placement_flags.test(_variant_placement_symmetric_bit)) symmetric++;
      if (object->properties.placement_flags.test(_variant_placement_asymmetric_bit)) asymmetric++;
    }
  }
  for (long index = map_variant->m_number_of_scenario_objects; index < map_variant->m_number_of_variant_objects; index++) {
    s_variant_object_datum *object = &map_variant->objects[index];
    if (!object->flags.is_empty() && object->properties.teleporter_channel == 22) {
      if (object->properties.placement_flags.test(_variant_placement_symmetric_bit)) symmetric++;
      if (object->properties.placement_flags.test(_variant_placement_asymmetric_bit)) asymmetric++;
    }
  }
  if (!symmetric || !asymmetric) {
    for (long index = first; index < scenario_count; index++) {
      s_variant_object_datum *object = &map_variant->objects[index];
      if (object->properties.teleporter_channel == 22) {
        const s_scenario_multiplayer_object_properties *properties = map_variant_try_and_get_multiplayer_properties(map_variant, index);
        if (properties && (properties->symmetry == 0 || (properties->symmetry == 2 && !asymmetric) || (properties->symmetry == 1 && !symmetric))) {
          object->flags.set_unsafe((object->flags.get_unsafe() & 0xffd6) | 9);
          object->properties.team_designator = static_cast<e_variant_team_designator>(_multiplayer_team_designator_none);
          object->properties.placement_flags.set(_variant_placement_symmetric_bit, true);
          object->properties.placement_flags.set(_variant_placement_asymmetric_bit, true);
          if (properties->symmetry != 0) {
            if (properties->symmetry == 2) {
              object->properties.placement_flags.set(_variant_placement_symmetric_bit, false);
              asymmetric++;
            } else {
              symmetric++;
              object->properties.placement_flags.set(_variant_placement_asymmetric_bit, false);
            }
          } else {
            symmetric++;
            asymmetric++;
          }
          if (symmetric && asymmetric) break;
        }
      }
    }
  }
}

// @address 0xe1ce8 ?read_state_motion_data@c_animation_manager@@QBAXJPATreal_vector3d@@PAM@Z
void c_animation_manager::read_state_motion_data(long node_count, real_vector3d *velocity, float *yaw_rate) const
{
 if (m_transition_channel.valid()) {
  m_transition_channel.get_motion_data(velocity, yaw_rate);
 } else if (m_state_channel.valid()) {
  m_state_channel.get_motion_data(velocity, yaw_rate);
 } else {
  float zero = k_real_zero_storage;
  const volatile real_vector3d &zero_velocity = k_real_math_zero_vector3d_storage;
  // match: 0xe1ce8: word stores emit stw instead of stfs, and the volatile load keeps the pool base in r10 instead of r9.
  long x = zero_velocity.words[0];
  *yaw_rate = zero;
  velocity->words[0] = x;
  velocity->words[1] = k_real_math_zero_vector3d_storage.words[1];
  velocity->words[2] = k_real_math_zero_vector3d_storage.words[2];
 }
}

static __forceinline void animation_manager_capture_angles(const c_aiming_channel *channel, c_aiming_channel_settings *settings)
{
 bool valid = channel->valid();
 if (valid)
 {
  settings->m_use_angles = true;
  settings->m_angles = channel->m_angles;
 }
 else
 {
  settings->m_use_angles = true;
  settings->m_pitch = 0.0f;
  settings->m_yaw = 0.0f;
 }
}

// @address 0x121790 ?setup_aiming_channel@c_animation_manager@@QBA_NPAVc_aiming_channel@@JJJK@Z
bool c_animation_manager::setup_aiming_channel(c_aiming_channel *channel, long weapon_class, long weapon_type, long overlay, unsigned long
    playback_flags) const
{
 bool result = false;
 if (weapon_class == 226 || weapon_class == -1)
  weapon_class = m_current_goal.weapon_class;
 if (weapon_type == 226 || weapon_type == -1)
  weapon_type = m_current_goal.weapon_type;
 if (m_graph_tag_index != -1)
 {
  c_animation_id animation = animation_manager_get_graph(this)->find_overlay(m_current_goal.mode, weapon_class, weapon_type, overlay, 0, 0, 0);
  const bool use_angles = true;
  char location = 2;
  if (!animation.valid())
  {
   animation = find_state_animation_id(weapon_class, weapon_type, overlay);
   location = use_angles;
  }
  c_aiming_channel_settings settings;
  animation_manager_capture_angles(channel, &settings);
  settings.m_graph_index = m_graph_tag_index;
  settings.m_animation_id = animation;
  settings.m_definition_index = overlay;
  settings.m_location = (e_animation_graph_location)location;
  // match: 0x121790: the image ignores playback_flags (every caller passes 0; March sets the flags from it): +4 bytes otherwise
  settings.m_playback_flags.set_unsafe(0);
  bool setup = false;
  if (m_graph_tag_index != -1)
  {
   settings.m_graph_index = m_graph_tag_index;
   if (channel->setup_channel(&settings))
    setup = use_angles;
  }
  result = setup;
 }
 return result;
}


static __forceinline char *animation_manager_overlay_tag_get(const int &index)
{
 return ((char *)global_tag_instances[datum_index_to_absolute_index(index)].definition);
}


static __forceinline bool animation_manager_next_object_overlay(const c_animation_manager *manager, object_overlay_iterator *iterator)
{
 const c_model_animation_graph *graph = (const c_model_animation_graph *)animation_manager_overlay_tag_get(manager->m_graph_tag_index);
 short index = (short)(iterator->index + 1);
 bool found;
 if (index < graph->m_object_overlays.count) {
  iterator->index = index;
  const s_object_overlay *overlay = &((const s_object_overlay *)graph->m_object_overlays.address)[index];
  iterator->function_name = overlay->function_name;
  iterator->mode = overlay->mode;
  iterator->animation = overlay->animation;
  found = true;
 } else {
  found = false;
 }
 return found;
}
static __forceinline bool animation_manager_id_valid(const c_animation_id *id)
{
 return id->m_animation_index >= 0;
}
static __forceinline const c_model_animation *animation_manager_overlay_animation(const c_animation_channel *channel)
{
 const c_model_animation *animation = 0;
 if (animation_manager_id_valid(&channel->m_animation_id)) {
  const c_model_animation *part = 0;
  ((const c_model_animation_graph *)animation_manager_overlay_tag_get(channel->m_graph_index))
      ->get_graph_and_animation(channel->m_animation_id, 0, &part);
  animation = part;
 }
 return animation;
}
static __forceinline bool animation_manager_setup_object_overlay(const c_animation_manager *manager, c_animation_channel *channel, c_animation_id id)
{
 bool result = false;
 if (manager->m_graph_tag_index != -1) {
  c_animation_channel_settings settings;
  settings.m_graph_index = manager->m_graph_tag_index;
  settings.m_animation_id = id;
  settings.m_playback_flags.m_storage = 93;
  result = animation_manager_setup_on_graph_change(manager, channel, &settings);
 }
 return result;
}
// @address 0x31e108 ?object_apply_function_overlay_node_orientations@@YAXJPBUrender_model_definition@@PAVc_animation_manager@@PBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@@Z
void object_apply_function_overlay_node_orientations(long object_index, const render_model_definition *render_model,
    c_animation_manager *manager, const c_static_flags<255> *node_mask, long node_count, real_orientation *orientations)
{
 object_overlay_iterator iterator;
 c_animation_channel channel;
 while (animation_manager_next_object_overlay(manager, &iterator)) {
  if (iterator.function_name && animation_manager_setup_object_overlay(manager, &channel, iterator.animation)) {
   const c_model_animation *animation = animation_manager_overlay_animation(&channel);
   float magnitude;
   bool deterministic;
   object_get_function_value(object_index, iterator.function_name, manager->m_graph_tag_index, &magnitude, &deterministic);
   if (iterator.mode == 0) {
    (&channel)->set_frame_position((float)(animation->get_frame_count() - 1) * magnitude);
    channel.apply_weighted_node_orientations(render_model, 1.0f, node_mask, node_count, orientations, 0, object_index);
   } else if (iterator.mode == 1) {
    unsigned long ticks = game_time_globals->time + object_index;
    float rfT0 = game_time_globals->tick_length;
    float frame = (float)fmod((double)((float)ticks * rfT0 * 0.033333335f), (double)animation->get_frame_count());
    (&channel)->set_frame_position(frame);
    channel.apply_weighted_node_orientations(render_model, magnitude, node_mask, node_count, orientations, 0, object_index);
   }
  }
 }
 (&channel)->initialize();
}


// @address 0xdf7d8 ?update_animation_channel@c_animation_manager@@QBA_NPAVc_animation_channel@@P6AXPBUs_animation_event_data@@J@ZJ@Z
bool c_animation_manager::update_animation_channel(
    c_animation_channel *channel, void (*event_callback)(const s_animation_event_data *, long), long event_data) const
{
    bool result = false;
    if (m_graph_tag_index != -1 && channel->valid() && !(m_flags & 1))
        result = channel->update(this, event_callback, event_data);
    return result;
}

struct real_matrix4x3;


// @address 0x3482c0 animation_manager_apply_vehicle_ik
extern "C" void animation_manager_apply_vehicle_ik(long object_index, long marker_name, long ik_index, real_matrix4x3 *matrix)
{
 object_datum *object = ((object_header_datum *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
 const c_animation_manager *manager = (const c_animation_manager *)((char *)object + object->animation.offset);
 if (manager->valid() && marker_name != -1) {
  t_short_fixed<13> slider_value = manager->get_slider_value_from_chain(6, _mixing_board_parent_slider_regular);
  bool active = slider_value.m_value > 0;
  float weight = slider_value.export_real();
  if (active) {
   ik_point_iterator target;
   target.marker = -1;
   target.attach_to_marker = -1;
   target.index = -1;
   target.mode = 0;
   target.mode_name = -1;
   target.weapon_class_name = -1;
   biped_process_ik(object_index, marker_name, -1, false, weight, manager, &target, ik_index, matrix);
  }
 }
}


static __forceinline bool animation_manager_locomotion_state(long state)
{
 return state == 21 || state == 22 || state == 23 || state == 24;
}

// @address 0xe9298 animation_manager_update_analog_blending
extern "C" bool animation_manager_update_analog_blending(
    c_animation_manager *manager, long object_index, const real_vector3d *velocity, float blend_ratio, bool limited)
{
 bool result = false;
 bool current = animation_manager_locomotion_state(manager->m_current_goal.state);
 bool previous = animation_manager_locomotion_state(manager->m_previous_goal.state);
 if (manager->m_transition_channel.valid()) {
  if (current || previous)
   result = manager->m_transition_channel.update_analog_blend_from_throttle(velocity, blend_ratio, limited);
 } else if (current) {
  result = manager->m_state_channel.update_analog_blend_from_throttle(velocity, blend_ratio, limited);
 }
 return result;
}
__forceinline void s_animation_state_change_settings::clear()
{
    goals.mode = 1;
    goals.weapon_class = 200;
    goals.weapon_type = 200;
    goals.state = 12;
    transition.clear();
    primary.clear();
    flags.clear();
    interpolation_time = 0.0f;
}
static __forceinline bool animation_manager_animation_loops(long state)
{
    bool result = false;
    for (long index = 0; index < 28; index++)
    {
        if (state == g_matched_animation_state_names[index])
        {
            result = true;
            break;
        }
    }
    return result;
}


static __forceinline bool animation_manager_recommend_playback_flag(unsigned short flags, long bit) { return ((flags >> bit) & 1) != 0; }
bool c_animation_manager::recommend_state_change(long mode, long weapon_class, long weapon_type, long state, long flags,
    unsigned long playback_flags, s_animation_state_change_settings *settings) const
{
    float yaw_rate;
    bool result = false;
    s_animation_goal_settings requested = { mode, weapon_class, weapon_type, state };
    settings->clear();
    settings->primary.m_graph_index = m_graph_tag_index;
    settings->transition.m_graph_index = m_graph_tag_index;
    settings->flags.set_unsafe((unsigned short)flags);
    settings->interpolation_time = 0.26666668f;
    c_animation_id animation;
    s_animation_goal_settings filtered;
    s_animation_goal_settings actual;
    if (internal_validate_goal_state(requested.mode, requested.weapon_class, requested.weapon_type, requested.state, flags, &filtered, &actual,
        &animation))
    {
        settings->goals = filtered;
        c_flags<e_goal_settings_comparison_bits, unsigned long, 4> difference = ((const s_animation_goal_settings
            *)&m_current_goal)->compare_goals(filtered);
        if (difference.is_empty() && m_state_channel.valid() && m_state_channel.m_definition_index == actual.state && !(flags & 0x10))
        {
            get_current_settings(settings);
            const c_model_animation *current = m_state_channel.get_dominant_animation();
            if (current && current->loop_frame_index == 0 &&
                !animation_manager_recommend_playback_flag(m_state_channel.m_playback_flags.get_unsafe(), 5))
                apply_random_permutations_to_settings(&settings->primary);
            result = true;
        }
        else if (!animation.valid())
            result = (flags & 4) != 0;
        else
        {
            result = true;
            settings->primary.m_graph_index = m_graph_tag_index;
            settings->primary.m_animation_id = animation;
            settings->primary.m_definition_index = actual.state;
            settings->primary.m_location = _animation_location_action;
            settings->primary.m_playback_flags.set_unsafe((unsigned short)playback_flags);
            if (m_state_channel.valid())
            {
                if (flags & 0x40)
                    settings->primary.m_playback_ratio = m_state_channel.get_playback_ratio_remaining();
                else if (flags & 0x20)
                    settings->primary.m_playback_ratio = m_state_channel.get_playback_ratio();
            }
            if (!settings->primary.m_playback_flags.test(_animation_playback_looping_bit) &&
                animation_manager_animation_loops(settings->primary.m_definition_index))
                settings->primary.m_playback_flags.set(_animation_playback_looping_bit, true);
            long blend_state = -1;
            if (!(flags & 0x100))
            {
                c_animation_id blend;
                blend = locate_blend_animation(filtered.mode, filtered.weapon_class, filtered.weapon_type, filtered.state, &blend_state);
                if (blend.valid())
                {
                    settings->primary.m_blend_ids[0] = blend;
                    float weight = 0.0f;
                    c_animation_id blended = m_state_channel.get_blended_animation_id(0, &weight);
                    if (blended.valid())
                        settings->primary.m_blend_ratios[0] = weight;
                    else if (animation_manager_get_graph(this)->animation_contains_movement_data(animation) &&
                        animation_manager_get_graph(this)->animation_contains_movement_data(blend))
                    {
                        real_vector3d velocity;
                        read_state_motion_data(-1, &velocity, &yaw_rate);
                        float blend_ratio;
                        animation_definitions_compute_blend_and_playback(
                            animation_manager_get_graph(this), animation, blend, &velocity, false, &blend_ratio, &weight);
                        settings->primary.m_blend_ratios[0] = blend_ratio;
                        settings->primary.m_playback_rate = weight;
                    }
                }
            }
            if (!(flags & 0x200) && m_state_channel.valid())
            {
                long delay = 0;
                c_animation_id transition;
                transition = locate_transition_animation(m_current_goal.mode, m_current_goal.state, filtered.mode, filtered.state, &delay);
                if (transition.valid())
                {
                    settings->transition.m_graph_index = m_graph_tag_index;
                    settings->transition.m_animation_id = transition;
                    settings->transition.m_definition_index = filtered.state;
                    settings->transition.m_location = _animation_location_transition;
                    settings->transition.m_playback_flags.set_unsafe(21);
                    if (settings->primary.m_blend_ids[0].valid() && blend_state != -1)
                    {
                        long blend_delay;
                        c_animation_id blend_transition;
                        blend_transition = locate_transition_animation(m_current_goal.mode, m_current_goal.state, filtered.mode, blend_state,
                            &blend_delay);
                        if (blend_transition.valid() && blend_delay == delay)
                        {
                            settings->transition.m_blend_ids[0] = blend_transition;
                            settings->transition.m_blend_ratios[0] = settings->primary.m_blend_ratios[0];
                        }
                    }
                    else
                    {
                        float weight = 0.0f;
                        c_animation_id blended = m_state_channel.get_blended_animation_id(0, &weight);
                        if (blended.valid())
                        {
                            long matched_state = animation_manager_find_matched_state(m_current_goal.state, g_matched_animation_states, 4);
                            if (matched_state != -1)
                            {
                                long blend_delay;
                                c_animation_id blend_transition;
                                blend_transition = locate_transition_animation(m_current_goal.mode, matched_state, filtered.mode,
                                    filtered.state, &blend_delay);
                                if (blend_transition.valid() && blend_delay == delay)
                                {
                                    settings->transition.m_blend_ids[0] = blend_transition;
                                    settings->transition.m_blend_ratios[0] = weight;
                                }
                            }
                        }
                    }
                }
            }
            if (settings->transition.m_animation_id.valid() && settings->primary.m_animation_id.valid())
            {
                bool contains_movement = m_state_channel.get_animation() &&
                    animation_manager_get_graph(this)->animation_contains_movement_data(m_state_channel.get_animation_id());
                if (contains_movement)
                {
                    const c_model_animation *next = animation_manager_get_graph(this)->get_animation(settings->primary.m_animation_id);
                    if (next && !animation_manager_get_graph(this)->animation_contains_movement_data(settings->primary.m_animation_id))
                        settings->transition.m_flags.m_storage |= 2;
                }
            }
            apply_random_permutations_to_settings(&settings->primary);
            apply_random_permutations_to_settings(&settings->transition);
        }
    }
    return result;
}
