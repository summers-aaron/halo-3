// interfacelibxbox/chud.obj: from the records 0x118dd8 0x12bc50 0x12cdf0 0x12db38 0x12f300 0x12f388 0x12f6a8 0x12f920 0x130438 0x130610 0x130bf0 0x130e28 0x130f10 0x131118 0x131d00 0x1321a8 0x132478 0x134e68 0x135490 0x135740 0x135920 0x135d90 0x135f00 0x135fe8 0x136338 0x1364b0 0x136d30 0x136f08 0x166220 0x166490 0x166568 0x1665f8 0x167d18 0x186930 0x18e438 0x18eab8 0x1a8378 0x1aaea0 0x4645e8 0x4712d0 0x4712e8 0x471300 0x471318 0x471330 0x471340 0x471350 0x4713a8 0x471440 0x471690 0x471800 0x471900 0x471cb0 0x471f20 0x471f70 0x472090 0x4721f0 0x4722f8 0x472440 0x472580 0x472658 0x472720 0x472788 0x4728e0 0x472968 0x472a10 0x472b88 0x472cf8 0x472db8 0x472e60 0x472e88 0x472f30 0x472f58 0x473000 0x473028 0x4730c8 0x4730e0 0x473100 0x4731a0 0x4731b8 0x487820 0x4878c8 0x487930 0x4879e8 0x487b68 0x487e10 0x487fd0 0x488100 0x488618 0x4886f0 0x4889d8 0x488a78 0x488a90 0x49b610 0x4e42a0 0x4e43e8 0x4e44b8 0x4e4618
// @address 0x8c4694 g_rasterizer_device
// @address 0x81dab8 g_rasterizer_alpha_blend_mode
// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes
// @address 0x11df8a8 g_chud_widget_data
// @address 0x11df6dc x_persistent_user_data
// @flags /TP /O2 /Oc
// @address 0x11df70c g_director_globals
// @address 0xb34 k_random_word_normalization
// @address 0x94e3ac g_saved_film_playback_tick
// @address 0x81dab0 g_rasterizer_current_z_buffer_mode
// @address 0x11df838 g_light_volume_gpu_row_data
// @address 0x11df8cc g_observer_globals
// @address 0x135920 ?widget_definition_should_be_active@c_chud_update_user_data@@QAA_NPAUs_chud_widget_base@@PAUs_chud_widget_collection@@J@Z
// @address 0x135f00 ?widget_definition_should_be_hidden@c_chud_update_user_data@@QAA_NPAUs_chud_widget_base@@PAUs_chud_widget_collection@@J@Z
// @address 0x1a8378 ?__tls_set_g_main_gamestate_timing_data_allocator@@YAXPAX@Z
// @address 0x4645e8 ?__tls_set_g_user_widget_allocator_0@@YAXPAX@Z
// @address 0x472090 ?chud_dispose_from_old_map@@YAXXZ
// @address 0x471900 ?set_fade@c_chud_cinematic_fade@@QAAXMM@Z
struct s_widget_geometry;

// @address 0x17a188 D3DDevice_SetPixelShaderConstantFN
struct s_chud_widget_collection;

#define XVIDEO_MODE XDK_VIDEO_MODE
#include <xtl.h>
#undef XVIDEO_MODE
#include "rasterizer/rasterizer_device_state.h"
#include "rasterizer/rasterizer_sampler_filter_state.h"
#include "effects/player_effects.h"
#include "saved_games/saved_film_work_types.h"
#include "game/campaign_metagame_player_types.h"
#include "game/players_globals_types.h"
#include "rasterizer/rasterizer_depth_bias_globals.h"
#include "interface/chud/chud_manager.h"
#include "game/player_control_globals.h"
#include "game/player_control_types.h"
#include "camera/director_storage_types.h"
#include "camera/saved_film_director_storage.h"
#include "game/campaign_metagame_globals.h"
#include "render/render_texture_camera_globals.h"
#include "interface/chud/chud_render_dependencies.h"
#include "saved_games/saved_film_tick.h"
#include "items/equipment_globals.h"
#include "effects/effect_runtime_storage.h"
#include "cseries/cseries_unicode.h"
#include "interface/chud/chud_widget_data.h"
#include "interface/chud/chud_navpoint_manager.h"
#include "interface/chud/chud_user_data_types.h"
#include "game/game_engine_list.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_default.h"
#include "game/player_types.h"
#include "items/item_types.h"
#include "units/unit_definitions.h"
#include "networking/online/voice_observer_types.h"
#include "networking/online/voice_lifecycle.h"
#include "rasterizer/rasterizer_vertex_types.h"
#include "interface/chud/chud_medal_message_types.h"
#include "memory/tls_data_arrays.h"
#include "text/font_cache_thread_state_types.h"
#include "simulation/simulation.h"
#include "bitmaps/bitmap_group.h"
#include "main/main.h"
#include "game/game_engine_team.h"
#include "text/font_id_types.h"
#include "scenario/scenario.h"
#include "math/real_constant_pool.h"
#include "game/player_control_constants.h"
#include "math/real_math_types.h"
#include <string.h>
#include "math/real_math.h"
#include "cseries/bit_operations.h"
#include "math/random_math_range.h"
#include "math/real_constant_pool_access.h"
#include "game/game_time_access.h"
#include "cseries/maximum.h"
#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include "tag_files/tag_groups.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include "objects/object_placement.h"
#include "units/unit_types.h"
#include "units/unit_control.h"
#include "units/unit_inventory_inline.h"
#include "memory/static_arrays.h"
#include <math.h>
#include <stddef.h>
#include "scenario/scenario_tag.h"
#include "objects/object_types.h"
#include "cache/restricted_memory.h"
#include "bitmaps/bitmap_data.h"
#include "game/game_time_types.h"
#include "game/game_globals.h"
#include "game/game_globals_inline.h"
#include "cache/restricted_memory_regions.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_allocation_manager_inline.h"
#include "game/game_variant_inline.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "objects/objects.h"
#include "sound/game_sound.h"
#include "units/units.h"
#include "interface/chud/chud.h"
#include "interface/chud/chud_draw.h"
#include "text/font_cache_lock.h"
#include "rasterizer/rasterizer_states.h"
#include "interface/chud/chud_messaging.h"
#include "items/equipment.h"
#include "items/weapons.h"
#include "math/periodic_functions.h"
#include "networking/online/voice_xenon.h"
#include "rasterizer/rasterizer_main.h"
#include "sound/sound_manager.h"
#include "cutscene/cinematics.h"
#include "game/game_time.h"
#include "gpu_particle/particle_block.h"
#include "interface/user_interface_controller.h"
#include "memory/data.h"
#include "memory/datum_index_inlines.h"
#include "game/player_access.h"
#include "objects/object_datum.h"
#include "game/game_allegiance.h"
#include "interface/controller_types.h"
#include "interface/c_controller.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "cache/restricted_memory_layout.h"
#include "items/equipment_definitions.h"
#include "render/render_camera_types.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "units/unit_interface_state_types.h"
#include "interface/chud/chud_cortana_definition_types.h"
#include "interface/chud/chud_dispose_state_types.h"
#include "cache/restricted_memory_registration.h"
#include "game/player_training.h"
#include "interface/chud/chud_messaging_handle_types.h"
#include "interface/chud/utf32_types.h"
#include "rasterizer/rasterizer_hue_saturation.h"
#include "networking/online/voice_interface_types.h"
#include "game/player_mapping_types.h"
#include "game/player_mapping_inlines.h"
#include "text/character_data_index_types.h"
#include "game/game_engine_interface_state_types.h"
#include "game/game_engine_scoring_globals.h"
#include "render_methods/render_method_definitions.h"
#include "interface/chud/chud_definitions.h"
#include "interface/chud/chud_update_user_data_types.h"
#include "objects/object_header.h"
#include "items/weapon_definition_access.h"
#include "interface/chud/chud_globals_definitions.h"
#include "creatures/creature_datum.h"
#include "objects/object_material_types.h"
#include "models/model_target_types.h"
#include "models/model_types.h"
#include "interface/chud/chud_scripting_globals_types.h"
#include "saved_games/saved_film_recording_types.h"
#include "game/game_time_inline.h"
#include "interface/chud/chud_directional_damage.h"
#include "interface/chud/chud_motion_sensor.h"
#include "cutscene/cinematic_globals_types.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard_types.h"
#include "rasterizer/rasterizer_shader_definitions.h"
#include "rasterizer/rasterizer_sampler_texture_calls.h"
#include "sound/game_sound_types.h"
#include "sound/game_looping_sound_data.h"
#include "camera/camera_script_globals_types.h"
#include "interface/chud/cortana_effect_definition.h"
#include "scenario/scenario_cutscene_flag.h"
#include "interface/chud/chud_draw_types.h"
#include "text/font_cache_types.h"
#include "text/draw_string_types.h"
#include "text/rasterizer_draw_string_types.h"
#include "camera/observer_globals_types.h"
#include "interface/controller_inline.h"
#include "game/game_globals_interface_types.h"
#include "game/campaign_metagame_definitions.h"
#include "interface/chud_draw_globals.h"
#include "items/weapon_interface_state_types.h"
#include "simulation/simulation_world.h"
#include "interface/chud/chud_navpoints.h"
#include "render/views/player_view_types.h"
#include "interface/chud/chud_sampler_filter_mode.h"
#include "multithreading/thread_tls.h"
#include "interface/chud/chud_external_input_types.h"

// March 2007 TLS: ?x_persistent_user_data@c_chud_manager@@2PAVc_chud_persistent_user_data@@A.
__declspec(thread) c_chud_persistent_user_data *c_chud_manager::x_persistent_user_data;
__declspec(thread) s_data_array *g_chud_widget_data[4];

class c_chud_state_allocator {
public:
    c_chud_state_allocator() : m_member_index(-1), m_owner_thread_index(-1) {}
    ~c_chud_state_allocator() {}
    void *reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits);
    virtual void *allocate(unsigned int size, const char *name);
    virtual void deallocate(void *allocation);
private:
    long m_member_index;
    long m_owner_thread_index;
};

struct s_chud_curve_follower {
    long flags;
    real_point3d offset;
    real_point3d position;
    s_tag_block curve;
    real_point3d secondary;
};

struct s_chud_curve_key_3d
{
    unsigned int frame;
    real_point3d value;
};

struct s_chud_curve_3d
{
    int count;
#ifdef HALO3_64BIT
    TAG_POINTER(s_chud_curve_key_3d) keys;
#else
    s_chud_curve_key_3d *keys;
#endif
};

struct s_chud_curve_key_2d
{
    unsigned int frame;
    real_point2d value;
};

struct s_chud_curve_2d
{
    int count;
#ifdef HALO3_64BIT
    TAG_POINTER(s_chud_curve_key_2d) keys;
#else
    s_chud_curve_key_2d *keys;
#endif
};

struct s_chud_curve_follower_2d {
    long flags;
    real_point3d offset;
    real_point2d position;
    s_tag_block curve;
    real_point3d secondary;
};

struct s_chud_strip_point
{
    real_point2d position;
    real_vector2d normal;
    float width;
};

struct s_chud_cinematic_strip_tuning
{
    float center_start;
    float center_end;
    float width_start;
    float width_end;
    float wave_length;
    float wave_speed;
    float horizontal_amplitude_start;
    float horizontal_amplitude_end;
    float vertical_amplitude_start;
    float vertical_amplitude_end;
};

struct s_chud_cinematic_curve_set
{
    s_tag_block channels[5];
};

struct s_chud_cinematic_strip
{
    s_chud_strip_point points[32];
    long point_count;
};

struct s_damage_flash_owner_argument
{
    const s_damage_owner *value;
};

struct s_damage_flash_direction_argument
{
    real_vector3d *value;
};

struct s_damage_flash_elapsed_ticks
{
    long value;
};

template<typename t_datum_type>
class c_chud_data_iterator
{
public:
    __forceinline void begin(s_data_array *data)
    {
        m_iterator.absolute_index = -1;
        m_iterator.index = -1;
        m_iterator.data = data;
    }
    __forceinline bool next()
    {
        m_datum = (t_datum_type *)::data_iterator_next(&m_iterator);
        return m_datum != 0;
    }
    __forceinline t_datum_type *get_datum() const { return m_datum; }
    __forceinline long get_index() const { return m_iterator.index; }
private:
    t_datum_type *m_datum;
    s_data_iterator m_iterator;
};


namespace nl_3193 {
__forceinline wchar_t *ustrnzcpy(wchar_t *dest, const wchar_t *source, long size)
{
    const wchar_t *s = source;
    for (long i = 0; i <= size - 1; ++i)
    {
        dest[i] = *s;
        if (dest[i] == 0)
            break;
        ++s;
    }
    dest[size - 1] = 0;
    return dest;
}
}

// @address 0x12f300 chud_get_user_skin_index
extern "C" long chud_get_user_skin_index(long id)
{
    long result = 0;
    long key = chud_get_user_skin_key(id);
    if (key != 0) {
        s_chud_globals_definition *table = chud_globals;
        for (long i = 0; i < table->row_count; ++i) {
            if (table->rows[i].key == key) {
                result = i;
                break;
            }
        }
    }
    return result;
}

// @address 0x12f388 chud_get_user_skin_key
extern "C" long chud_get_user_skin_key(long id)
{
    long player_index = player_index_from_user_index_render(id);
    long result = 0;
    if (player_index != -1)
    {
        switch (game_get_player_character_type(player_index))
        {
        case 0:
        case 2:
            result = 0;
            break;
        case 1:
        case 3:
        case 4:
            result = 1;
            break;
        case 5:
            result = 2;
            break;
        }
    }
    return result;
}


static __forceinline bool chud_player_is_immune_to_headshot(long player_index)
{
    bool immune = false;
    if (game_globals->options.game_mode == 1)
        immune = true;
    else if (game_globals->options.game_mode == 2)
        immune = player_get(player_index)->traits.get_shield_vitality_traits()->get_headshot_immunity_setting() == 1;
    return immune;
}

// 0x12f6a8
// @address 0x12f6a8 ?study_autoaim@c_chud_update_user_data@@IAAXJ@Z
void c_chud_update_user_data::study_autoaim(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 4099);
    autoaim_flags = 0;
    autoaim_target_index = -1;
    long target_object_index = unit->unit.targeting.target_object_index;
    if (target_object_index != -1 && (unit->unit.targeting.flags & 1) == 0)
    {
        unit_datum *target = (unit_datum *)object_try_and_get_and_verify_type(target_object_index, 0xffffffff);
        if (target)
        {
            s_object_material_definition *definition = (s_object_material_definition *)tag_get(1868720741, target->object.definition_index);
            bool immune_to_headshot = false;
            autoaim_target_index = target_object_index;
            long type_bit = 1 << target->object.object_type;
            if ((type_bit & 4099) != 0)
            {
                if (game_team_is_friendly((e_game_team)unit->unit.team, (e_game_team)target->unit.team))
                    autoaim_flags |= 1;
                if (target->unit.player_index != -1)
                    immune_to_headshot = chud_player_is_immune_to_headshot(target->unit.player_index);
                if (target->object.parent_object_index != -1 && target->unit.parent_seat_index != -1)
                {
                    unit_datum *parent = (unit_datum *)object_get_and_verify_type(target->object.parent_object_index, 4099);
                    unit_definition *parent_definition = (unit_definition *)tag_get(1970170228, parent->object.definition_index);
                    unit_seat *seat = &parent_definition->seats.begin()[target->unit.parent_seat_index];
                    immune_to_headshot = (bool)((seat->flags >> 23) & 1) | immune_to_headshot;
                }
            }
            else if ((type_bit & (1 << 11)) != 0)
            {
                if (game_team_is_friendly((e_game_team)unit->unit.team, (e_game_team)((creature_datum *)target)->creature.team))
                    autoaim_flags |= 1;
            }
            else if ((type_bit & (1 << 3)) != 0)
            {
                if (game_team_is_friendly((e_game_team)unit->unit.team, (e_game_team)((equipment_datum *)target)->creator_damage_owner.team))
                    autoaim_flags |= 1;
            }
            if (definition->model.index != -1)
            {
                if (unit->unit.targeting.model_target_index != -1 && unit->unit.targeting.primary_autoaim_level >= 1.0f)
                {
                    s_model_definition *model = (s_model_definition *)tag_get(1751936372, definition->model.index);
                    if (unit->unit.targeting.model_target_index < model->targets.count)
                    {
                        s_model_target *element = &((s_model_target *)model->targets.address)[unit->unit.targeting.model_target_index];
                        if ((element->flags & (1 << 2)) != 0 && !immune_to_headshot)
                            autoaim_flags |= 16;
                        if ((element->flags & (1 << 3)) != 0)
                            autoaim_flags |= 4;
                    }
                }
            }
        }
    }
}

// 0x130438
long chud_compute_unit_backpack_weapon_index(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    long readied_weapon = ::unit_inventory_get_must_be_readied_weapon(unit_index);
    long backpack_weapon = ::unit_get_current_backpack_weapon(unit_index);
    long current_weapon = unit_inventory_get_weapon_or_none(unit_index,
        ((unit_datum *)object_get_and_verify_type(unit_index, 8195))->unit.current_weapon_index);
    if (current_weapon != -1 && weapon_is_support_weapon(current_weapon)) {
        readied_weapon = unit->unit.current_weapon_index;
    }
    if (readied_weapon != -1) {
        long greatest_age = -1;
        backpack_weapon = -1;
        for (long i = 0; i < 4; ++i) {
            if (i != readied_weapon && unit->unit.weapon_object_indices[i] != -1) {
                long age = unit->unit.weapon_ages[i];
                if (age > greatest_age) {
                    greatest_age = age;
                    backpack_weapon = unit->unit.weapon_object_indices[i];
                }
            }
        }
    }
    return backpack_weapon;
}

typedef char chud_saved_film_rate_offset[(offsetof(chud_saved_film_progress, rate) == 0x14) ? 1 : -1];
typedef char chud_saved_film_active_offset[(offsetof(chud_saved_film_progress, active) == 0x18) ? 1 : -1];

// @address 0x130e28 chud_update_progress_timer
extern "C" void chud_update_progress_timer(chud_saved_film_progress *timer)
{
    long state = g_saved_film_recording_globals.state;
    if (state == 3 || state == 4 || state == 5 || state == 6 || state == 7 || state == 8 || state == 9
        || state == 10 || state == 11 || state == 12 || state == 13 || state == 14 || state == 15) {
        float duration = timer->duration;
        float elapsed = game_time_globals->tick_length * (float)g_saved_film_recording_globals.first_tick;
        timer->active = true;
        if (duration < 0.0001f)
            timer->rate = 0.0f;
        else
            timer->rate = elapsed / duration;
    }
}

// 0x130f10

// 0x132478
static __forceinline real_vector3d *chud_slot_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *v)
{
    v->i = b->x - a->x;
    v->j = b->y - a->y;
    v->k = b->z - a->z;
    return v;
}
static __forceinline float chud_slot_magnitude_squared(const real_vector3d *a)
{
    return a->i * a->i + a->j * a->j + a->k * a->k;
}
static __forceinline float chud_slot_distance_squared(const real_point3d *a, const real_point3d *b)
{
    real_vector3d vector;
    return chud_slot_magnitude_squared(chud_slot_vector_from_points(a, b, &vector));
}
// @address 0x132478 chud_find_slot_containing_point
extern "C" long chud_find_slot_containing_point(s_equipment_slot *slots, const real_point3d *point, bool use_alternate_radius)
{
    long index;
    for (index = 0; index < 8; index++)
    {
        const s_equipment_slot *slot = &slots[index];
        if (slot->in_use)
        {
            float radius = use_alternate_radius ? slot->speed : slot->radius;
            if (chud_slot_distance_squared(point, &slot->center) < radius * radius)
                break;
        }
    }
    if (index >= 8)
        return -1;
    return index;
}

// @address 0x134e68 chud_get_layout_entry
extern "C" chud_table_entry *chud_get_layout_entry(long id, long flag_index)
{
    chud_table_row *row = &chud_globals->rows[chud_get_user_skin_index(id)];
    long index = 0;
    for (; index < row->entry_count; ++index) {
        if ((row->entries[index].resolution_flags & (1 << flag_index)) != 0)
            break;
    }
    if (index >= row->entry_count)
        index = 0;
    return &row->entries[index];
}

// 0x135920
bool c_chud_update_user_data::widget_definition_should_be_active(
    s_chud_widget_base *widget, s_chud_widget_collection *collection, long index)
{
    bool active = true;
    state *widget_state = widget->states.count() ? widget->states.begin() : 0;
    state *collection_state = collection->states.count() ? collection->states.begin() : 0;
    if (widget_state == 0 && collection_state == 0) {
        weapon_state *weapon = find_weapon_state(index);
        if (weapon == 0 && index != -1)
            active = false;
    } else {
        weapon_state *weapon = find_weapon_state(index);
        if (weapon == 0 && index != -1) {
            active = false;
        } else {
            for (long i = 0; i < 2; ++i) {
                state *candidate = i ? widget_state : collection_state;
                if (candidate) {
                    if (candidate->active_flags[0])
                        active = active && (candidate->active_flags[0] & active_flags[0]) != 0;
                    if (candidate->active_flags[1])
                        active = active && (candidate->active_flags[1] & active_flags[1]) != 0;
                    if (candidate->active_flags[2])
                        active = active && (candidate->active_flags[2] & active_flags[2]) != 0;
                    if (candidate->active_flags[3])
                        active = active && (candidate->active_flags[3] & active_flags[3]) != 0;
                    if (candidate->active_flags[4])
                        active = active && (candidate->active_flags[4] & active_flags[4]) != 0;
                    if (candidate->active_flags[5])
                        active = active && (candidate->active_flags[5] & active_flags[5]) != 0;
                    if (candidate->active_flags[6])
                        active = active && (candidate->active_flags[6] & active_flags[6]) != 0;
                    if (candidate->active_flags[7])
                        active = active && (candidate->active_flags[7] & active_flags[7]) != 0;
                    if (candidate->active_flags_12)
                        active = active && (candidate->active_flags_12 & active_flags[9]) != 0;
                    if (candidate->active_flags_14)
                        active = active && (candidate->active_flags_14 & active_flags[10]) != 0;
                    if (candidate->active_flags_16)
                        active = active && (candidate->active_flags_16 & active_flags[11]) != 0;
                    if (weapon) {
                        if (candidate->weapon_active_flags[0])
                            active = active && (candidate->weapon_active_flags[0] & weapon->active_flags_2) != 0;
                        if (candidate->weapon_active_flags[1])
                            active = active && (candidate->weapon_active_flags[1] & weapon->active_flags_4) != 0;
                        if (candidate->weapon_active_flags[2])
                            active = active && (candidate->weapon_active_flags[2] & weapon->active_flags_6) != 0;
                    }
                }
            }
        }
    }
    if (!chud_scripting_is_element_enabled(&g_chud_scripting_globals->scripting, widget->scripting_class, user_index)
        || !chud_scripting_is_element_enabled(&g_chud_scripting_globals->scripting, collection->scripting_class, user_index))
        active = false;
    return active;
}

// 0x135d90
bool c_chud_update_user_data::widget_definition_should_be_flashing(
    s_chud_widget_base *widget, s_chud_widget_collection *collection, long index)
{
    bool flashing = false;
    state *widget_state = widget->states.count() ? widget->states.begin() : 0;
    state *collection_state = collection->states.count() ? collection->states.begin() : 0;
    if (widget_state || collection_state) {
        weapon_state *weapon = find_weapon_state(index);
        for (long i = 0; i < 2; ++i) {
            state *candidate = i ? widget_state : collection_state;
            if (candidate) {
                if (candidate->flashing) {
                    flashing = flashing || (candidate->flashing & flashingFlags) != 0;
                }
                if (weapon && candidate->weaponFlashing) {
                    flashing = flashing || (candidate->weaponFlashing & weapon->flashingFlags) != 0;
                }
            }
        }
    }
    return flashing;
}

bool c_chud_update_user_data::widget_definition_should_be_hidden(s_chud_widget_base *widget, s_chud_widget_collection *collection,
    long index)
{
 bool hidden=false;
 state *widget_state=widget->states.count() ? widget->states.begin() : 0;
 state *collection_state=collection->states.count() ? collection->states.begin() : 0;
 if(widget_state || collection_state) {
  weapon_state *weapon=find_weapon_state(index);
  if(weapon || index == -1) {
  for(long i=0;i<2;i++) {
   state *candidate=i ? widget_state : collection_state;
   if(candidate && candidate->hide) hidden |= (flags & candidate->hide)!=0;
  }
 }
 }
 return hidden;
}


__forceinline void c_chud_impulse_manager::picked_up_grenade(long grenade_index)
{
    s_game_globals *globals = global_game_globals;
    for (long index = 0; index < globals->grenades.count(); ++index) {
        s_grenade_definition *element = globals->grenades.count() ? &globals->grenades[index] : 0;
        if (element && element->equipment.index == grenade_index) {
            ++grenade_counts[index];
            break;
        }
    }
}

__forceinline long c_chud_impulse_manager::get_grenade_pickup_count(long grenade_index)
{
    return grenade_counts[grenade_index];
}

__forceinline long c_chud_impulse_manager::get_ammo_pickup_count(long object_index)
{
    long count = 0;
    if (object_index != -1 && ammo.count > 0) {
        unit_datum *object = (unit_datum *)object_try_and_get_and_verify_type(object_index, 4);
        if (object) {
            for (long index = 0; index < ammo.count; ++index) {
                if (ammo.impulses[index].definition_index == object->object.definition_index) {
                    count = ammo.impulses[index].count;
                    break;
                }
            }
        }
    }
    return count;
}

// 0x136d30
// @address 0x136d30 ?widget_definition_should_be_impulse@c_chud_update_user_data@@SA_NJPAUs_chud_widget_base@@PAUs_chud_widget_collection@@JPAJ@Z
bool c_chud_update_user_data::widget_definition_should_be_impulse(
    long user_index, s_chud_widget_base *widget, s_chud_widget_collection *collection, long object_index, long *count)
{
    state *widget_state = widget->states.count() ? widget->states.begin() : 0;
    state *collection_state = collection->states.count() ? collection->states.begin() : 0;
    bool impulse = false;
    if (widget_state || collection_state) {
        for (long i = 0; i < 2; ++i) {
            state *candidate = i ? widget_state : collection_state;
            if (candidate) {
                if (candidate->impulse_grenade_flags) {
                    c_chud_impulse_manager *manager = c_chud_impulse_manager::get(user_index);
                    if (candidate->impulse_grenade_flags & 1) {
                        *count = manager->get_grenade_pickup_count(0);
                        impulse = *count > 0;
                        break;
                    } else if (candidate->impulse_grenade_flags & 2) {
                        *count = manager->get_grenade_pickup_count(1);
                        impulse = *count > 0;
                        break;
                    } else if (candidate->impulse_grenade_flags & 4) {
                        *count = manager->get_grenade_pickup_count(2);
                        impulse = *count > 0;
                        break;
                    } else if (candidate->impulse_grenade_flags & 8) {
                        *count = manager->get_grenade_pickup_count(3);
                        impulse = *count > 0;
                        break;
                    }
                }
                if (candidate->impulse_ammo_flags) {
                    c_chud_impulse_manager *manager = c_chud_impulse_manager::get(user_index);
                    if (candidate->impulse_ammo_flags & 2) {
                        *count = manager->get_ammo_pickup_count(object_index);
                        impulse = *count > 0;
                        break;
                    }
                }
            }
        }
    }
    return impulse;
}

// @address 0x166490 chud_get_scoreboard_fade
extern "C" float chud_get_scoreboard_fade(long user_index)
{
    float one = real_constant_pool_one();
    game_globals_storage *globals = game_globals;
    bool game_active = false;
    if (globals != 0 && globals->in_progress && !globals->initializing && globals->map_active)
        game_active = true;
    if (game_active && globals->options.game_mode != 3)
    {
        long rumble_index = user_interface_controller_get_rumble_enabled(user_index);
        if (rumble_index != -1)
        {
            return one - c_gui_screen_scoreboard::get_scoreboard_alpha((e_controller_index)rumble_index);
        }
    }
    return one;
}

void __tls_set_g_main_gamestate_timing_data_allocator(void *value)
{
    g_main_gamestate_timing_data = (s_game_tick_time_samples *)value;
}

void __tls_set_g_user_widget_allocator_0(void *object)
{
    data_set_new_base_address((s_data_array **)&g_light_volume_gpu_row_data, (s_data_array *)object);
}

// 0x4712d0
// @address 0x4712d0 ?__tls_set_g_chud_user_data_allocator_0@@YAXPAX@Z
void __tls_set_g_chud_user_data_allocator_0(void *object)
{
    data_set_new_base_address((s_data_array **)&g_chud_widget_data[0], (s_data_array *)object);
}

// 0x4712e8
// @address 0x4712e8 ?__tls_set_g_chud_user_data_allocator_1@@YAXPAX@Z
void __tls_set_g_chud_user_data_allocator_1(void *object)
{
    data_set_new_base_address((s_data_array **)&g_chud_widget_data[1], (s_data_array *)object);
}

// 0x471300
// @address 0x471300 ?__tls_set_g_chud_user_data_allocator_2@@YAXPAX@Z
void __tls_set_g_chud_user_data_allocator_2(void *object)
{
    data_set_new_base_address((s_data_array **)&g_chud_widget_data[2], (s_data_array *)object);
}

// 0x471318
// @address 0x471318 ?__tls_set_g_chud_user_data_allocator_3@@YAXPAX@Z
void __tls_set_g_chud_user_data_allocator_3(void *object)
{
    data_set_new_base_address((s_data_array **)&g_chud_widget_data[3], (s_data_array *)object);
}

// 0x471340
// @address 0x471340 ?__tls_set_g_chud_scripting_globals_allocator@@YAXPAX@Z
void __tls_set_g_chud_scripting_globals_allocator(void *value)
{
    g_chud_scripting_globals = (chud_scripting_globals_tls *)value;
}

// @address 0x471350 chud_get_external_type_value
extern "C" int chud_get_external_type_value(int type)
{
    int result = -1;
    switch (type)
    {
    case 0:
    case 1:
        result = -1;
        break;
    case 2:
        result = 1;
        break;
    case 3:
        result = 0;
        break;
    }
    return result;
}

void c_chud_cinematic_fade::set_fade(float target, float duration)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x471900: shared constant-pool base register serves the 0.0f compare and the 0.5f rounding load
    float zero = k_real_constant_pool_half[-553];
#else
    float zero = 0.0f;
#endif
    if (duration == zero) {
        m_fade = target;
        m_fade_rate = zero;
        m_target_fade = target;
        return;
    }

    int ticks_per_second = game_time_globals->ticks_per_second;
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x471900: shared constant-pool base register (loaded once at the 0.0f compare) supplies 0.5f
    int ticks = (int)((float)ticks_per_second * duration + k_real_constant_pool_half[0]);
#else
    int ticks = (int)((float)ticks_per_second * duration + 0.5f);
#endif
    if (ticks <= 1)
        ticks = 1;
    float difference = target - m_fade;
    m_target_fade = target;
    m_fade_rate = difference / (float)(long long)ticks;
}

// @address 0x471800 ?start@c_chud_directional_damage@@QAAXPBTreal_vector3d@@MFMMMPATreal_argb_color@@@Z
void c_chud_directional_damage::start(const real_vector3d *direction, float duration, short type, float intensity,
    float first, float second, real_argb_color *color)
{
    s_directional_damage *damage=&entries[get_oldest_index()];
    damage->direction=*direction;
    damage->type=type;
    damage->start_time=game_time_get();
    damage->end_time=damage->start_time+game_seconds_to_ticks_round(duration);
    damage->intensity=intensity;
    damage->first=first;
    damage->second=second;
    damage->color=*color;
}

class c_chud_persistent_user_data;
class c_chud_navpoint_manager;


// @address 0x471330 ?__tls_set_g_chud_persistent_user_data_allocator@@YAXPAX@Z
void __tls_set_g_chud_persistent_user_data_allocator(void *globals)
{
    c_chud_manager::x_persistent_user_data = (c_chud_persistent_user_data *)globals;
}

// @address 0x473028 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_persistent_user_data_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_persistent_user_data_allocator>::reserve_memory(const char *name, const char *type,
    unsigned int size, long alignment_bits)
{
    // match: 0x473028: folding alignment_bits to 0 keeps li r7,0 in the add_member argument setup
    __assume(alignment_bits == 0);
    long member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, alignment_bits,
        __tls_set_g_chud_persistent_user_data_allocator, 0, 0);
    void *address = restricted_region_get_member_address(3, member_index);
    __tls_set_g_chud_persistent_user_data_allocator(address);
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, member_index);
}

// @address 0x4730c8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_persistent_user_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_persistent_user_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x4730e0 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_persistent_user_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_persistent_user_data_allocator>::deallocate(void *allocation)
{
    __tls_set_g_chud_persistent_user_data_allocator(0);
    restricted_region_free_member(3, m_member_index);
}

// @address 0x4dfc8 ??_7?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_persistent_user_data_allocator@@YAXPAX@Z@@6B@
// @address 0x70a488 ??__Eg_chud_persistent_user_data_allocator@@YAXXZ
// @address 0x70cb80 ??__Fg_chud_persistent_user_data_allocator@@YAXXZ
// @address 0x7c1b14 ?g_chud_persistent_user_data_allocator@@3V?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_persistent_user_data_allocator@@YAXPAX@Z@@A
t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_persistent_user_data_allocator> g_chud_persistent_user_data_allocator;

// 0x471690

// @address 0x471690 ?initialize@c_chud_manager@@SAXXZ
void c_chud_manager::initialize()
{
    chud_scripting_globals_tls *scripting_globals;
    g_chud_widget_data[0] = (s_data_array *)data_new("chud widgets", 128, 24, 0, (c_allocation_base *)&chud_user_data_allocator_0);
    g_chud_widget_data[1] = (s_data_array *)data_new("chud widgets", 128, 24, 0, (c_allocation_base *)&chud_user_data_allocator_1);
    g_chud_widget_data[2] = (s_data_array *)data_new("chud widgets", 128, 24, 0, (c_allocation_base *)&chud_user_data_allocator_2);
    g_chud_widget_data[3] = (s_data_array *)data_new("chud widgets", 128, 24, 0, (c_allocation_base *)&chud_user_data_allocator_3);
    scripting_globals = (chud_scripting_globals_tls *)chud_allocate_scripting_globals(&chud_scripting_globals_allocator,
        "chud", "persistent user data", 0x1b0, 0);
    g_chud_scripting_globals = scripting_globals;
    memset(scripting_globals, 0, offsetof(chud_scripting_globals_tls, scripting));
    scripting_globals->user_index = 0;
    for (long slot_index = 0; slot_index < NUMBEROF(scripting_globals->equipment_slots); slot_index++)
        scripting_globals->equipment_slots[slot_index].active = false;
    c_chud_persistent_user_data *persistent_user_data = (c_chud_persistent_user_data *)
        g_chud_persistent_user_data_allocator.reserve_memory("chud", "persistent user data", 0xa200, 0);
    c_chud_manager::x_persistent_user_data = persistent_user_data;
    for (long user_index = 0; user_index < 4; user_index++)
        chud_persistent_user_data_initialize(&persistent_user_data[user_index], user_index);
}

// 0x471f20
// @address 0x471f20 chud_initialize
extern "C" void chud_initialize(void)
{
    c_chud_manager::initialize();
    g_chud_cinematic_state = (chud_dispose_state *)g_chud_state_allocator.reserve_memory("chud",
        "cortana effect",
        16, 0);
}

// 0x472090 natural source

// 0x4721f0
static __forceinline bool chud_cinematic_letterbox_active()
{
    bool active = false;
    if (cinematic_globals)
        active = cinematic_globals->in_progress && cinematic_globals->letterbox_closing && cinematic_globals->letterbox_fraction > 0.0f;
    return active;
}
// @address 0x4721f0 chud_can_draw_for_user
extern "C" unsigned char chud_can_draw_for_user(int user_index)
{
    bool result = true;
    if (user_index == -1)
    {
        result = 0;
    }
    else
    {
        s_director_storage *entry = &g_director_globals->directors[user_index];
        if (!entry->get_director()->should_draw_hud_saved_film()
            || c_gui_screen_scoreboard::is_scoreboard_displayed((e_controller_index)user_interface_controller_get_rumble_enabled(user_index))
            || c_gui_screen_scoreboard::is_scoreboard_displayed(k_any_controller))
        {
            result = 0;
        }
        else
        {
            if (inl_cinematic_in_progress() && chud_cinematic_letterbox_active())
                result = 0;
        }
    }
    return result;
}

// 0x472440

struct D3DDevice;
// @address 0x130f10 chud_get_layout_index
extern "C" long chud_get_layout_index(long user_index)
{
    bool widescreen = rasterizer_main_get_video_mode()->fIsWideScreen != 0;
    long player_count = players_get_active_user_count();
    long player_index = player_get_user_from_controller(user_interface_controller_get_rumble_enabled(user_index));
    if (!widescreen) {
        if (player_count > 1) {
            if (player_count >= 3 && (player_count != 3 || player_index != 0))
                return 7;
            else
                return 5;
        } else {
            return 3;
        }
    } else {
        rectangle2d bounds;
        ::c_rasterizer::get_display_pixel_bounds(&bounds);
        float width = bounds.x1;
        float height = bounds.y1;
        if (width * width < height * height * 2.3703704f) {
            if (player_count > 1) {
                if (player_count >= 3 && (player_count != 3 || player_index != 0))
                    return 6;
                else
                    return 5;
            } else {
                return 2;
            }
        } else {
            if (player_count > 1) {
                if (player_count >= 3 && (player_count != 3 || player_index != 0))
                    return 4;
                else
                    return 1;
            } else {
                return 0;
            }
        }
    }
}

// @address 0x472440 chud_draw_composite
extern "C" void chud_draw_composite(int width, int height)
{
    long shader_count = chud_globals->shader_count;
    s_tag_reference *vertex_shader = 0;
    if (shader_count > 20)
        vertex_shader = &chud_globals->shaders[20].vertex_shader;
    s_tag_reference *pixel_shader = 0;
    if (shader_count > 20)
        pixel_shader = &chud_globals->shaders[20].pixel_shader;
    long vertex_shader_index = vertex_shader ? vertex_shader->index : -1;
    long pixel_shader_index = pixel_shader ? pixel_shader->index : -1;
    c_rasterizer::set_vertex_shader(
        c_rasterizer_vertex_shader::get(vertex_shader_index), _vertex_type_chud_simple, _transfer_vector_vertex_none, _entry_point_albedo);
    c_rasterizer::set_pixel_shader(c_rasterizer_pixel_shader::get(pixel_shader_index), _entry_point_albedo);
    if (g_rasterizer_alpha_blend_mode != 1) {
        D3DDevice *device = g_rasterizer_device;
        g_rasterizer_alpha_blend_mode = 1;
        D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
        D3DDevice_SetRenderState_BlendOp(device, 0);
        D3DDevice_SetRenderState_SrcBlend(device, 1);
        D3DDevice_SetRenderState_DestBlend(device, 1);
    }
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_off);
    D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 0);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
    c_rasterizer::set_aliased_surface_as_texture(3, (c_rasterizer::e_surface)58);
    c_rasterizer::draw_fullscreen_quad(width, height);
}

// 0x472580

// @address 0x472658 chud_post_grenade_pickup
extern "C" void chud_post_grenade_pickup(long user_index, long grenade_index)
{
    if (user_index >= 0 && user_index < 4 && !game_is_cooperative()) {
        c_chud_impulse_manager::get(user_index)->picked_up_grenade(grenade_index);
    }
}

// 0x472720


// @address 0x4728e0 chud_set_texture_cam_enabled

// 0x472968
// @address 0x472968 ?chud_get_widget_collection_and_definition@@YAXJJJPAPAUs_chud_widget_collection@@PAPAUs_chud_widget_base@@PA_N@Z
void chud_get_widget_collection_and_definition(
    long tag_index, long entry_index, long element_index, s_chud_widget_collection **collection, s_chud_widget_base **widget, bool *text)
{
    *collection = 0;
    *widget = 0;
    if (tag_index == -1)
        return;

    s_chud_definition *definition = (s_chud_definition *)tag_get('chdt', tag_index);
    if (entry_index < 0 || entry_index >= definition->widget_collection_count)
        return;

    s_chud_widget_collection *entry = &(definition->widget_collections)[entry_index];
    int first_count = entry->bitmap_widget_count;
    *collection = entry;

    void *element;
    if (element_index < first_count) {
        element = &entry->bitmap_widgets[element_index].base;
        if (text)
            *text = false;
    } else {
        element = &entry->text_widgets[element_index - first_count].base;
        if (text)
            *text = true;
    }
    *widget = (s_chud_widget_base *)element;
}

// @address 0x1aaea0 ?deallocate@?$t_restricted_allocation_manager@$02$1?main_thread_combine_timing_data@@YAXPAX@Z$0A@$1?__tls_set_g_main_gamestate_timing_data_allocator@@YAX0@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, &main_thread_combine_timing_data, static_cast<void (*)(void *)>(0),
    &__tls_set_g_main_gamestate_timing_data_allocator>::deallocate(void *allocation)
{
    g_main_gamestate_timing_data = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x472a10

static __forceinline void chud_unattached_looping_sound_stop(long sound_index)
{
    game_looping_sound_datum *sound = &((game_looping_sound_datum *)game_looping_sound_data->data)[(unsigned short)sound_index];
    sound->flags.set(1, true);
}
static __forceinline chud_sound_reference *chud_sound_block_get_element(s_tag_block *block, long index)
{
    return &((chud_sound_reference *)block->address)[index];
}
// @address 0x472a10 ?chud_play_sound@@YAXJJPAUs_tag_block@@PAJPAG@Z
void chud_play_sound(long unused, long mask, s_tag_block *block, long *sounds, unsigned short *flags)
{
    for (short i = 0; i < block->count; i++)
    {
        chud_sound_reference *reference = chud_sound_block_get_element(block, i);
        if ((reference->mask & mask) != 0)
        {
            switch (reference->sound.group_tag)
            {
            case 'lsnd':
                if (sounds[i] == -1)
                    sounds[i] = ::unattached_looping_sound_start(reference->sound.index, -1, reference->gain);
                break;
            case 'snd!':
                if (sounds[i] == -1 || ((1 << i) & *flags) == 0)
                {
                    if (sounds[i] != -1)
                        sound_stop_impulse(sounds[i]);
                    sounds[i] = ::unspatialized_impulse_sound_new(reference->sound.index, reference->gain);
                }
                break;
            }
            *flags |= (1 << i);
        }
        else if (sounds[i] != -1)
        {
            if (reference->sound.group_tag == 0x6c736e64u)
                chud_unattached_looping_sound_stop(sounds[i]);
            sounds[i] = -1;
            *flags &= ~(1 << i);
        }
    }
}

// 0x472cf8
// @address 0x472cf8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_user_data_allocator_0@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_0>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}


// 0x472db8
// @address 0x472db8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_user_data_allocator_1@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_1>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x472e60
// @address 0x472e60 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_user_data_allocator_1@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_1>::deallocate(void *allocation)
{
    g_chud_widget_data[1] = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x472e88
// @address 0x472e88 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_user_data_allocator_2@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_2>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x472f30
// @address 0x472f30 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_user_data_allocator_2@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_2>::deallocate(void *allocation)
{
    g_chud_widget_data[2] = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x472f58
// @address 0x472f58 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_user_data_allocator_3@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_3>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x473000
// @address 0x473000 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_user_data_allocator_3@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_3>::deallocate(void *allocation)
{
    g_chud_widget_data[3] = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x473100
// @address 0x473100 chud_allocate_scripting_globals
extern "C" void *chud_allocate_scripting_globals(t_chud_scripting_globals_allocator *allocation,
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = g_restricted_regions[3].add_member(name, type, size, 0, __tls_set_g_chud_scripting_globals_allocator, 0, 0);
    unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
    void *address = g_restricted_address[3] + offset;
    allocation->m_thread_id = get_current_thread_index();
    allocation->m_member_index = member_index;
    g_chud_scripting_globals = (chud_scripting_globals_tls *)address;
    return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}

// 0x4731a0
// @address 0x4731a0 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_scripting_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_scripting_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return chud_allocate_scripting_globals(this, name, "", size, 0);
}

// 0x4731b8
// @address 0x4731b8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_scripting_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_scripting_globals_allocator>::deallocate(void *allocation)
{
    long value = m_member_index;
    g_chud_scripting_globals = 0;
    restricted_region_free_member(3, value);
}

// 0x487820
// @address 0x487820 ?__tls_set_g_chud_state_allocator@@YAXPAX@Z
void __tls_set_g_chud_state_allocator(void *value)
{
    g_chud_cinematic_state = (chud_dispose_state *)value;
}

// 0x4878c8

// @address 0x4878c8 chud_stop_screen_effect
extern "C" void chud_stop_screen_effect(void)
{
    if (g_chud_cinematic_state->index != -1) {
        s_cortana_effect_definition *definition =
            (s_cortana_effect_definition *)tag_get('crte', g_chud_cinematic_state->index);
        g_chud_cinematic_state->index = -1;
        g_camera_script_globals->camera_track.render_main_camera_to_texture = false;
        g_render_texture_camera_globals->enabled = false;
        g_render_texture_camera_globals->dynamic_lights = false;
        if (definition->cinematic_scene.index != -1)
            cinematic_reset_scene_and_show_chud();
    }
}

// 0x487930

// @address 0x487930 chud_start_screen_effect
extern "C" void chud_start_screen_effect(long index)
{
    if (index == -1)
        return;
    s_cortana_effect_definition *definition =
        (s_cortana_effect_definition *)((cache_file_tag_instance *)global_tag_instances)[(unsigned short)index].definition;
    chud_dispose_state *record = g_chud_cinematic_state;
    record->index = index;
    record->ticks = 0;
    record->start_time = game_time_globals->time;
    record->definition_value = definition->frame_count;
    if (definition->cinematic_scene.index == -1)
        return;
    long flag_count = global_scenario->cutscene_flags.count();
    long flag_index = 0;
    if (flag_count > 0) {
        long flag_id = definition->anchor_name;
        do {
            if (global_scenario->cutscene_flags.begin()[flag_index].name == flag_id)
                break;
            flag_index = flag_index + 1;
        } while (flag_index < flag_count);
    }
    if (flag_index != flag_count) {
        g_camera_script_globals->active = true;
        g_camera_script_globals->animation_changed = true;
    }
}

// 0x4879e8
static __forceinline void chud_cycle_color_set(float *color, float r, float g, float b)
{
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

// @address 0x4879e8 chud_compute_cycle_color
extern "C" void chud_compute_cycle_color(float frame, float saturation, float *color)
{
    float seconds = frame * 0.016666668f;
    int sector = (int)(float)floor((double)seconds) % 6;
    float fraction = seconds - (float)(double)sector;
    float complement = (float)(1.0 - saturation);
    float fall = (float)(1.0 - fraction * saturation);
    float rise = (float)(1.0 - (1.0 - fraction) * saturation);
    switch (sector) {
    case 0: chud_cycle_color_set(color, 1.0f, rise, complement); break;
    case 1: chud_cycle_color_set(color, fall, 1.0f, complement); break;
    case 2: chud_cycle_color_set(color, complement, 1.0f, rise); break;
    case 3: chud_cycle_color_set(color, complement, fall, 1.0f); break;
    case 4: chud_cycle_color_set(color, rise, complement, 1.0f); break;
    case 5: chud_cycle_color_set(color, 1.0f, complement, fall); break;
    }
}

// 0x4889d8
// @address 0x4889d8 ?reserve_memory@c_chud_state_allocator@@QAAPAXPBD0IJ@Z
void *c_chud_state_allocator::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x4889d8: value range assumption guides branch lowering
    __assume(alignment_bits == 0);
    long member_index = g_restricted_regions[3].add_member(name, type, size, alignment_bits, __tls_set_g_chud_state_allocator, 0, 0);
    __tls_set_g_chud_state_allocator(restricted_region_get_member_address(3, member_index));
    m_member_index = member_index;
    m_owner_thread_index = get_current_thread_index();
    return restricted_region_get_member_address(3, member_index);
}

// 0x488a78
// @address 0x488a78 ?allocate@c_chud_state_allocator@@UAAPAXIPBD@Z
void *c_chud_state_allocator::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x488a90
// @address 0x488a90 ?deallocate@c_chud_state_allocator@@UAAXPAX@Z
void c_chud_state_allocator::deallocate(void *allocation)
{
    __tls_set_g_chud_state_allocator(0);
    restricted_region_free_member(3, m_member_index);
}

// 0x4e4618

typedef char s_chud_curve_follower_size[(sizeof(s_chud_curve_follower) == 0x34) ? 1 : -1];
typedef char s_chud_curve_follower_secondary_offset[(offsetof(s_chud_curve_follower, secondary) == 0x28) ? 1 : -1];

static __forceinline s_chud_curve_key_3d *chud_curve_get_key_3d(s_chud_curve_3d *c, int i)
{
#ifdef HALO3_64BIT
    return (s_chud_curve_key_3d *)c->keys + i;
#else
    s_chud_curve_key_3d **data = &c->keys;
    s_chud_curve_key_3d *&ref = *data;
    return ref + i;
#endif
}
static __forceinline void chud_curve_interpolate_3d(const real_point3d *a, const real_point3d *b, float t, real_point3d *out)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float dz = b->z - a->z;
    out->x = dx * t + a->x;
    out->y = dy * t + a->y;
    out->z = dz * t + a->z;
}
static __forceinline int chud_curve_find_key_3d(s_chud_curve_3d *c, unsigned int frame)
{
    int i = 0;
    int count = c->count;
    if (count > 0) {
        s_chud_curve_key_3d *key = c->keys;
        do {
            if (key->frame >= frame) break;
            ++i;
            ++key;
        } while (i < count);
    }
    return i;
}

// 0x4e44b8
// @address 0x4e44b8 chud_compute_curve_value_3d
// match: 0x4e44b8: pointer __restrict qualifier prevents alias reload
extern "C" bool chud_compute_curve_value_3d(void *curve, unsigned long frame, bool smooth, real_point3d *__restrict out)
{
    s_chud_curve_3d *c = (s_chud_curve_3d *)curve;
    bool result = false;
    int i = chud_curve_find_key_3d(c, frame);
    if (i != 0 && i != c->count) {
        s_chud_curve_key_3d *key = chud_curve_get_key_3d(c, i);
        unsigned int start = key[-1].frame;
        unsigned int span = key->frame - start;
        if ((int)span > 1) {
            float t = (float)(frame - start) / (float)span;
            if (smooth) {
                t = (float)(1.0 - ((float)::cos((double)(t * 3.1415927f)) + 1.0f) * 0.5f);
            }
            chud_curve_interpolate_3d(&key[-1].value, &key->value, t, out);
            return true;
        } else {
            *out = key->value;
            result = true;
        }
    }
    return result;
}

static __forceinline s_chud_curve_key_2d *chud_curve_get_key_2d(s_chud_curve_2d *c, int i)
{
#ifdef HALO3_64BIT
    return (s_chud_curve_key_2d *)c->keys + i;
#else
    s_chud_curve_key_2d **data = &c->keys;
    s_chud_curve_key_2d *&ref = *data;
    return ref + i;
#endif
}
static __forceinline void chud_curve_interpolate_2d(const real_point2d *a, const real_point2d *b, float t, real_point2d *out)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    out->x = dx * t + a->x;
    out->y = dy * t + a->y;
}
static __forceinline int chud_curve_find_key_2d(s_chud_curve_2d *c, unsigned int frame)
{
    int i = 0;
    int count = c->count;
    if (count > 0) {
        s_chud_curve_key_2d *key = c->keys;
        do {
            if (key->frame >= frame) break;
            ++i;
            ++key;
        } while (i < count);
    }
    return i;
}

// 0x4e42a0
// @address 0x4e42a0 chud_compute_curve_value_2d
// match: 0x4e42a0: pointer __restrict qualifier prevents alias reload
extern "C" bool chud_compute_curve_value_2d(s_chud_curve_2d *c, unsigned int frame, unsigned char smooth, real_point2d *__restrict out)
{
    bool result = false;
    int i = chud_curve_find_key_2d(c, frame);
    if (i != 0 && i != c->count) {
        s_chud_curve_key_2d *key = chud_curve_get_key_2d(c, i);
        unsigned int start = key[-1].frame;
        unsigned int span = key->frame - start;
        if ((int)span > 1) {
            float t = (float)(frame - start) / (float)span;
            if (smooth) {
                t = (float)(1.0 - ((float)::cos((double)(t * 3.1415927f)) + 1.0f) * 0.5f);
            }
            chud_curve_interpolate_2d(&key[-1].value, &key->value, t, out);
            return true;
        } else {
            *out = key->value;
            result = true;
        }
    }
    return result;
}

// @address 0x4e4618 chud_compute_follower_value_3d
extern "C" void chud_compute_follower_value_3d(s_chud_curve_follower *follower, unsigned long frame, real_point3d *position,
    real_point3d *offset, real_point3d *secondary)
{
    *position = follower->position;
    bool smooth = (follower->flags & 1) != 0;
    if (follower->curve.count != 0) {
        real_point3d delta;
        if (chud_compute_curve_value_3d(follower->curve.address, frame, smooth, &delta)) {
            position->x = position->x + delta.x;
            position->y = position->y + delta.y;
            position->z = position->z + delta.z;
        }
    }
    if (offset != 0)
        *offset = follower->offset;
    if (secondary != 0)
        *secondary = follower->secondary;
}


// 0x4e43e8
typedef char s_chud_curve_follower_2d_size[(sizeof(s_chud_curve_follower_2d) == 0x30) ? 1 : -1];
typedef char s_chud_curve_follower_2d_secondary_offset[(offsetof(s_chud_curve_follower_2d, secondary) == 0x24) ? 1 : -1];

// @address 0x4e43e8 chud_compute_follower_value_2d
extern "C" void chud_compute_follower_value_2d(s_chud_curve_follower_2d *follower, unsigned long frame, real_point2d *position,
    real_point3d *offset, real_point3d *secondary)
{
    *position = follower->position;
    bool smooth = (follower->flags & 1) != 0;
    if (follower->curve.count != 0) {
        real_point2d delta;
        if (chud_compute_curve_value_2d((s_chud_curve_2d *)follower->curve.address, frame, smooth, &delta)) {
            position->x = position->x + delta.x;
            position->y = position->y + delta.y;
        }
    }
    if (offset != 0)
        *offset = follower->offset;
    if (secondary != 0)
        *secondary = follower->secondary;
}

struct s_chud_persistent_display_state;


static __forceinline void chud_strip_set_point2d(real_point2d *point, float x, float y)
{
    point->x = x;
    point->y = y;
}


static __forceinline void chud_strip_add_quad(s_widget_geometry *geometry, const s_chud_strip_point *points, long index, float u0,
    float u1, float side)
{
    const s_chud_strip_point *a = &points[index];
    const s_chud_strip_point *b = &points[index + 1];
    long quad_index = geometry->character_count++;
    s_widget_geometry_character *quad = &geometry->characters[quad_index];
    quad->vertices[0].position = a->position;
    chud_strip_set_point2d(&quad->vertices[0].texcoord, u0, 1.0f);
    float a_width = a->width;
    point_from_line2d(&a->position, &a->normal, a_width * side, &quad->vertices[1].position);
    chud_strip_set_point2d(&quad->vertices[1].texcoord, u0, 0.0f);
    float b_width = b->width;
    point_from_line2d(&b->position, &b->normal, b_width * side, &quad->vertices[2].position);
    chud_strip_set_point2d(&quad->vertices[2].texcoord, u1, 0.0f);
    quad->vertices[3].position = b->position;
    chud_strip_set_point2d(&quad->vertices[3].texcoord, u1, 1.0f);
}

// @address 0x487e10 chud_build_strip_geometry
extern "C" void chud_build_strip_geometry(const s_chud_strip_point *points, s_widget_geometry *geometry)
{
    geometry->character_count = 0;
    for (long i = 0; i < 31; i++)
    {
        float u0 = (float)i / 31.0f;
        float u1 = (float)(i + 1) / 31.0f;
        chud_strip_add_quad(geometry, points, i, u0, u1, -1.0f);
        chud_strip_add_quad(geometry, points, i, u0, u1, 1.0f);
    }
}

static __forceinline void chud_find_widget_collection_and_definition(
    long tag_index, long collection_index, long widget_index, s_chud_widget_collection **collection, s_chud_widget_base **widget)
{
    *collection = 0;
    *widget = 0;
    if (tag_index != -1)
    {
        s_chud_definition *definition = (s_chud_definition *)tag_get('chdt', tag_index);
        if (collection_index >= 0 && collection_index < definition->widget_collection_count)
        {
            *collection = &definition->widget_collections[collection_index];
            *widget = (*collection)->get_widget_by_index(widget_index);
        }
    }
}

// @address 0x135740 ?compute_desired_widget_state@c_chud_update_user_data@@QAAJJJJJJPA_N@Z
long c_chud_update_user_data::compute_desired_widget_state(
    long user_index, long collection_tag_index, long collection_index, long widget_index, long object_index, bool *hidden)
{
    long state = 0;
    // match: preserve the write summary so callers reload widget state.
    this->user_index = this->user_index;
    *hidden = false;
    s_chud_widget_collection *collection;
    s_chud_widget_base *widget;
    chud_find_widget_collection_and_definition(collection_tag_index, collection_index, widget_index, &collection, &widget);
    if (collection && widget)
    {
        if (widget_definition_type_is_impulse(widget, collection, object_index))
        {
            long count;
            if (widget_definition_should_be_impulse(user_index, widget, collection, object_index, &count) &&
                widget_definition_should_be_active(widget, collection, object_index))
                state = 6;
        }
        else if (widget_definition_should_be_active(widget, collection, object_index))
        {
            if (widget_definition_should_be_flashing(widget, collection, object_index))
                state = 3;
            else
                state = 2;
        }
        if (state != 0 && widget_definition_should_be_hidden(widget, collection, object_index))
            *hidden = true;
    }
    return state;
}

// @address 0x130bf0 chud_get_saved_film_progress
extern "C" void chud_get_saved_film_progress(chud_saved_film_progress *progress)
{
    long length = g_saved_film_work_globals.playback_length;
    // match: 0x130bf0: plain peek() load swaps two registers at +0x61/+0x6d; the tick is read as a volatile long
    long tick = const_cast<const volatile long &>(g_saved_film_playback_tick.value);
    progress->duration = game_ticks_to_seconds(length);
    progress->position = game_ticks_to_seconds(tick);
    progress->marker_count = saved_film_get_marker_count(g_saved_film_work_globals.entries);
    progress->progress = 0.0f;
    for (long i = 0; i < 10; i++)
        progress->markers[i] = 0.0f;
    float *marker = progress->markers;
    long earliest = length;
    for (long i = 0; i < 16; i++)
    {
        s_saved_film_work_entry *entry = &g_saved_film_work_globals.entries[i];
        if (entry->active)
        {
            if (entry->flags[0])
                *marker++ = game_ticks_to_seconds(entry->value) / progress->duration;
            if (!entry->flags[0] && entry->value < earliest)
                earliest = entry->value;
        }
    }
    float fraction = length > 0 ? (float)tick / (float)length : 0.0f;
    progress->buffered = fraction;
    progress->progress = fraction;
    if (earliest != length)
        progress->progress = length > 0 ? game_ticks_to_seconds(earliest) / progress->duration : 0.0f;
    progress->rate = 0.0f;
    progress->active = false;
}

// @address 0x7c1bec g_chud_turn_yaw_wrap
float g_chud_turn_yaw_wrap = 6.28318548f;
// @address 0x7c1bf0 g_chud_turn_yaw_limit
float g_chud_turn_yaw_limit = 3.14159274f;

static __forceinline float chud_angle_difference(float a, float b)
{
    float half_circle = k_player_control_half_turn;
    float full_circle = k_player_control_full_turn;
    float difference = a - b;
    float delta = difference;
    if (delta >= half_circle)
        delta -= full_circle;
    if (delta <= -half_circle)
        delta += full_circle;
    return delta;
}

// @address 0x136f08 chud_compute_limited_direction
extern "C" void chud_compute_limited_direction(const real_vector3d *desired, const real_vector3d *current,
    const real_euler_angles2d *reference, float maximum_angle, real_vector3d *result, float *turn_direction)
{
    *result = *desired;
    real_euler_angles2d current_angles;
    float yaw_delta;
    float pitch_difference;
    {
        real_euler_angles2d desired_angles;
        euler_angles2d_from_vector3d(&desired_angles, desired);
        euler_angles2d_from_vector3d(&current_angles, current);
        if (reference)
        {
            real_vector3d reference_vector;
            vector3d_from_euler_angles2d(&reference_vector, reference);
            if (dot_product3d(current, &reference_vector) > (float)cos(10.0 * 0.017453292f))
                current_angles = *reference;
        }
        yaw_delta = chud_angle_difference(desired_angles.yaw, current_angles.yaw);
        pitch_difference = desired_angles.pitch - current_angles.pitch;
    }
    float turn_yaw_delta = yaw_delta;
    float pitch_delta = pitch_difference;
    if (turn_direction)
    {
        float previous_direction = *turn_direction;
        if ((float)fabs(yaw_delta) > g_chud_turn_yaw_limit - 0.5235988f)
        {
            if (previous_direction > 0.0f && yaw_delta < 0.0f)
                turn_yaw_delta = yaw_delta + g_chud_turn_yaw_wrap;
            else if (previous_direction < 0.0f && yaw_delta > 0.0f)
                turn_yaw_delta = yaw_delta - g_chud_turn_yaw_wrap;
        }
        *turn_direction = turn_yaw_delta > 0.0f ? 1.0f : -1.0f;
    }
    float magnitude = sqrtf(turn_yaw_delta * turn_yaw_delta + pitch_delta * pitch_delta);
    if (magnitude > maximum_angle)
    {
        float scale = maximum_angle / magnitude;
        real_euler_angles2d limited_angles;
        limited_angles.yaw = current_angles.yaw + turn_yaw_delta * scale;
        limited_angles.pitch = current_angles.pitch + pitch_delta * scale;
        vector3d_from_euler_angles2d(result, &limited_angles);
    }
}

static __forceinline unsigned long chud_random_value(unsigned long *seed)
{
    return random_seed_next(seed) >> 16;
}

static __forceinline float chud_random_fraction(unsigned long *seed)
{
    return (float)chud_random_value(seed) * k_random_word_normalization;
}

static __forceinline void chud_damage_flash_get_angles(long unit_index, const real_point3d *position, real_euler_angles2d *angles)
{
    real_point3d head_position;
    real_vector3d direction;
    unit_get_head_position(unit_index, &head_position);
    vector_from_points3d(position, &head_position, &direction);
    normalize3d(&direction);
    euler_angles2d_from_vector3d(angles, &direction);
}

// @address 0x73043c g_chud_cinematic_strip_tuning
s_chud_cinematic_strip_tuning g_chud_cinematic_strip_tuning = { -1.5f, 1.0f, 0.17f, 0.03f, -0.1f, 0.05f, 0.8f, 0.6f, 0.1f, 0.2f };


static __forceinline float chud_strip_scales(long strip_index, float *center_scale, float *width_scale)
{
    short function = 10;
    float bias = 1.0f;
    float half = 0.5f;
    float strip_spacing = 53.4070778f;
    float center_offset = 21.9911499f;
    float width_offset = 28.274334f;
    chud_dispose_state *cinematic = g_chud_cinematic_state;
    long ticks = cinematic->ticks;
    float wave_speed = g_chud_cinematic_strip_tuning.wave_speed;
    long rate = game_time_globals->ticks_per_second;
    float phase = (float)strip_index * strip_spacing + (float)ticks * wave_speed / (float)rate;
    *center_scale = (periodic_function_evaluate(function, phase + center_offset) + bias) * half;
    *width_scale = (periodic_function_evaluate(function, phase + width_offset) + bias) * half;
    return phase;
}


// @address 0x12bc50 chud_get_cinematic_fov_scale
extern "C" float chud_get_cinematic_fov_scale(void)
{
    float one = 1.0f;
    float result = one;
    chud_dispose_state *cinematic = g_chud_cinematic_state;
    if (cinematic->index != -1)
    {
        s_cortana_effect_definition *definition =
            (s_cortana_effect_definition *)tag_get('crte', cinematic->index);
        if (definition->postprocessing.s_tag_block::count != 0)
        {
            s_cortana_effect_postprocessing *reference = (s_cortana_effect_postprocessing *)definition->postprocessing.address;
            if (reference != 0 && reference->field_of_view.count != 0)
            {
                real_point2d position;
                real_point3d scratch;
                chud_compute_follower_value_2d((s_chud_curve_follower_2d *)reference->field_of_view.address, cinematic->ticks, &position,
                    &scratch, &scratch);
                result = position.x;
            }
        }
    }
    return result;
}

// @address 0x166568 chud_get_cinematic_fade
extern "C" float chud_get_cinematic_fade(void)
{
    float one = 1.0f;
    float result = one;
    chud_dispose_state *cinematic = g_chud_cinematic_state;
    if (cinematic->index != -1)
    {
        s_cortana_effect_definition *definition =
            (s_cortana_effect_definition *)tag_get('crte', cinematic->index);
        if (definition->postprocessing.s_tag_block::count != 0)
        {
            s_cortana_effect_postprocessing *reference = (s_cortana_effect_postprocessing *)definition->postprocessing.address;
            if (reference != 0 && reference->hud_brightness.count != 0)
            {
                real_point2d position;
                real_point3d scratch;
                chud_compute_follower_value_2d((s_chud_curve_follower_2d *)reference->hud_brightness.address, cinematic->ticks, &position,
                    &scratch, &scratch);
                result = position.x;
            }
        }
    }
    return result;
}

// @address 0x487b68 chud_compute_strip_points
extern "C" void chud_compute_strip_points(long strip_index, s_chud_strip_point *points)
{
    float center_scale;
    float width_scale;
    float phase = chud_strip_scales(strip_index, &center_scale, &width_scale);
    for (long i = 0; i < 32; i++)
    {
        s_chud_strip_point *point = &points[i];
        float t = (float)i / 31.0f;
        float center = g_chud_cinematic_strip_tuning.center_start +
            (g_chud_cinematic_strip_tuning.center_end - g_chud_cinematic_strip_tuning.center_start) * t * center_scale;
        float width_t = (float)pow((double)t, 0.5);
        float wave_phase = g_chud_cinematic_strip_tuning.wave_length * t + phase;
        float y = (periodic_function_evaluate(10, wave_phase + 1.0f) - 0.5f) *
                (g_chud_cinematic_strip_tuning.vertical_amplitude_start +
                    (g_chud_cinematic_strip_tuning.vertical_amplitude_end - g_chud_cinematic_strip_tuning.vertical_amplitude_start) * t) +
            center;
        float x = (periodic_function_evaluate(10, wave_phase) - 0.5f) *
            (g_chud_cinematic_strip_tuning.horizontal_amplitude_start +
                (g_chud_cinematic_strip_tuning.horizontal_amplitude_end - g_chud_cinematic_strip_tuning.horizontal_amplitude_start) * t);
        point->position.x = x;
        point->position.y = y;
        point->width = g_chud_cinematic_strip_tuning.width_start +
            (g_chud_cinematic_strip_tuning.width_end - g_chud_cinematic_strip_tuning.width_start) * width_t * width_scale;
    }
    for (long i = 0; i < 32; i++)
    {
        real_vector2d direction;
        if (i == 0)
            vector_from_points2d(&points[i + 1].position, &points[i].position, &direction);
        else if (i == 31)
            vector_from_points2d(&points[i].position, &points[i - 1].position, &direction);
        else
            vector_from_points2d(&points[i + 1].position, &points[i - 1].position, &direction);
        if (normalize2d(&direction) == 0.0f)
            direction = k_real_math_identity_matrix_storage.left.xy;
        points[i].normal.i = -direction.j;
        points[i].normal.j = direction.i;
    }
}


// @address 0x487fd0 chud_draw_cinematic_strips
extern "C" void chud_draw_cinematic_strips(void)
{
    chud_dispose_state *cinematic = g_chud_cinematic_state;
    if (cinematic->index != -1)
    {
        s_cortana_effect_definition *definition =
            (s_cortana_effect_definition *)tag_get('crte', cinematic->index);
        if (definition->gravemind.count != 0)
        {
            s_chud_cinematic_curve_set *reference = (s_chud_cinematic_curve_set *)definition->gravemind.address;
            if (reference != 0 && reference->channels[2].count != 0)
            {
                real_point2d position;
                real_point3d scratch;
                chud_compute_follower_value_2d((s_chud_curve_follower_2d *)reference->channels[2].address, cinematic->ticks, &position,
                    &scratch, &scratch);
                if (position.y > 0.0f && chud_globals != 0 && chud_globals->cinematic_strip_bitmap.index != -1 &&
                    chud_globals->cinematic_strip_resource.index != -1)
                {
                    for (long strip_index = 0; strip_index < 50; strip_index++)
                    {
                        s_widget_geometry geometry;
                        geometry.texture_handle = -1;
                        s_chud_cinematic_strip strip;
                        chud_compute_strip_points(strip_index, strip.points);
                        chud_build_strip_geometry(strip.points, &geometry);
                        chud_draw_cinematic_strip(strip_index, 50, position.x, position.y, &geometry);
                    }
                }
            }
        }
    }
}
static __forceinline const s_observer_camera_result *chud_user_observer_camera(long user_index)
{
    return &g_observer_globals->blocks[user_index].camera;
}
static __forceinline rectangle2d *chud_set_rectangle2d(rectangle2d *rectangle, short x0, short y0, short x1, short y1)
{
    rectangle->x0 = x0;
    rectangle->y0 = y0;
    rectangle->x1 = x1;
    rectangle->y1 = y1;
    return rectangle;
}

struct s_chud_persistent_display_state;

// @address 0x135490 ?update@c_chud_cortana_effect@@QAAXJ@Z
void c_chud_cortana_effect::update(long user_index)
{
 for (long i = 0; i < 2; i++)
 {
  if (m_sounds[i] == 0)
   m_sounds[i] = -1;
  if (m_secondary_sounds[i] == 0)
   m_secondary_sounds[i] = -1;
 }
 object_marker marker;
 if (active && object_index != -1 && global_scenario != 0 &&
  object_try_and_get_and_verify_type(object_index, -1) != 0 &&
  internal_object_get_markers_by_string_id(object_index, marker_name, &marker, 1, false) != 0)
 {
  m_position = marker.matrix.position;
  m_desired_scale = 1.0f;
 }
 else
  m_desired_scale = 0.0f;
 if (m_scale != m_desired_scale)
 {
  m_scale = m_scale + PIN(m_desired_scale - m_scale, -0.06666667f, 0.06666667f);
 }
 if (m_scale > 0.0f)
 {
  const s_chud_cortana_definition *cortana = (const s_chud_cortana_definition *)tag_block_get_element_with_size(
      &chud_globals->cortana_definitions, 0, sizeof(s_chud_cortana_definition));
  const s_chud_cortana_suck_definition *suck = (const s_chud_cortana_suck_definition *)tag_block_get_element_with_size(
      &cortana->suck_definitions, 0, sizeof(s_chud_cortana_suck_definition));
  const real_point3d *camera = &chud_user_observer_camera(user_index)->position;
  float distance = sqrtf(distance_squared3d(camera, &m_position)) / radius;
  m_pixel_constant_tail = PIN(distance / suck->maximum_distance, 0.0f, 1.0f);
  compute_effect_config(user_index);
  update_noise(m_effect_blend);
 }
 else
 {
  for (long i = 0; i < 2; i++)
  {
   if (m_sounds[i] != -1)
   {
    chud_unattached_looping_sound_stop(m_sounds[i]);
    m_sounds[i] = -1;
   }
   if (m_secondary_sounds[i] != -1)
   {
    chud_unattached_looping_sound_stop(m_secondary_sounds[i]);
    m_secondary_sounds[i] = -1;
   }
  }
  m_state0 = 0;
  m_state1 = 0;
 }
}

struct c_chud_messaging_manager;
    __forceinline void c_chud_messaging_manager::update(long user_index)
    {
        chud_post_action_response(user_index);
        chud_messaging_manager_update_messages(this, user_index);
        update_medals(user_index);
        update_state_messages(user_index);
    }

    

static __forceinline void chud_directional_damage_reset(c_chud_directional_damage *damage)
{
    for (long i = 0; i < 4; i++)
    {
        s_directional_damage *entry = &damage->entries[i];
        entry->tracker.m_valid = false;
        entry->tracker.m_turn_direction = 0.0f;
        entry->start_time = 0;
        entry->end_time = 0;
        entry->rotation = 0.0f;
        entry->alpha = 0.0f;
        entry->scale = 0.0f;
    }
}

static __forceinline void chud_impulse_manager_reset(c_chud_impulse_manager *manager)
{
    manager->ammo.count = 0;
    for (long i = 0; i < 4; i++)
        manager->grenade_counts[i] = 0;
}


static __forceinline void chud_persistent_display_state_reset(s_chud_persistent_display_state *data)
{
    memset(data, 0, sizeof(*data));
    data->cortana_effect.m_desired_scale = 0.0f;
    data->cortana_effect.m_scale = 0.0f;
    data->cortana_effect.m_sounds[0] = -1;
    data->cortana_effect.radius = 1.0f;
    long *indices = &data->cortana_effect.m_sound_handles[1];
    indices[0] = -1;
    indices[1] = -1;
    indices[2] = -1;
    for (long i = 1; i >= 0; i--)
        data->cortana_effect.m_states[i] = 0;
    data->widget_list_heads[0] = -1;
    data->widget_list_heads[1] = -1;
}

__forceinline void c_chud_persistent_user_data::initialize_sounds(long user_index)
{
    memset(m_tail_indices, 255, sizeof(m_tail_indices));
    m_tail_flags = 0;
}

__forceinline long c_chud_persistent_user_data::get_active_chud_definition_index(long definition_index, long weapon_index)
{
    long result = -1;
    for (long i = 0; i < 8; i++)
    {
        if (m_weapons[i].definition_index == definition_index && m_weapons[i].object_index == weapon_index)
        {
            result = i;
            break;
        }
    }
    return result;
}

__forceinline long c_chud_persistent_user_data::add_active_chud_definition_index(long definition_index, long weapon_index,
    bool *reset_required)
{
    long i;
    for (i = 0; i < 8; i++)
    {
        if (m_weapons[i].definition_index == -1 && m_weapons[i].object_index == -1)
            break;
    }
    if (i >= 8)
    {
        reset();
        data_delete_all((s_data_array *)g_chud_widget_data[m_user_index]);
        *reset_required = true;
        i = 0;
    }
    m_weapons[i].definition_index = definition_index;
    m_weapons[i].object_index = weapon_index;
    return i;
}

__forceinline void c_chud_persistent_user_data::active_chud_definition_notify_widget_created(long active_definition_index)
{
    m_weapons[active_definition_index].count++;
}

__forceinline void c_chud_persistent_user_data::active_chud_definition_notify_widget_deleted(long active_definition_index)
{
    if (--m_weapons[active_definition_index].count == 0)
    {
        m_weapons[active_definition_index].definition_index = -1;
        m_weapons[active_definition_index].object_index = -1;
    }
}

__forceinline void c_chud_persistent_user_data::update_sounds(long user_index, c_chud_update_user_data *update_data)
{
    unsigned long sound_flags = update_data->get_sound_flags();
    chud_table_row *row = &chud_globals->rows[chud_get_user_skin_index(user_index)];
    if (((1UL << ((s_controllers *)g_director_globals)->slots[user_index].perspective) & 3) == 0)
        sound_flags = 0;
    chud_play_sound(user_index, sound_flags, &row->sounds, m_tail_indices, &m_tail_flags);
}

// @address 0x5bec98 ?chud_dispose@@YAXXZ
void chud_dispose(void)
{
}

void chud_dispose_from_old_map(void)
{
    for (long user_index = 0; user_index < 4; ++user_index) {
        g_chud_widget_data[user_index]->valid = false;
        s_chud_persistent_display_state *user_data = &c_chud_manager::x_persistent_user_data[user_index].m_display_state;
        user_data->cortana_effect.m_desired_scale = 0.0f;
        user_data->cortana_effect.m_scale = 0.0f;
        user_data->cortana_effect.m_sounds[0] = -1;
        user_data->cortana_effect.radius = 1.0f;
        user_data->cortana_effect.m_sounds[1] = -1;
        user_data->cortana_effect.m_secondary_sounds[0] = -1;
        user_data->cortana_effect.m_secondary_sounds[1] = -1;
        user_data->cortana_effect.m_state1 = 0;
        user_data->cortana_effect.m_state0 = 0;
    }
    chud_globals = 0;
    g_chud_cinematic_state->address = 0;
    g_chud_cinematic_state->index = -1;
}

extern "C" void chud_set_texture_cam_enabled(bool value)
{
    for (long user_index = inl_players_first_active_user(); user_index != -1; user_index = players_next_active_user(user_index)) {
        c_chud_manager::x_persistent_user_data[user_index].m_flags[0] = value;
    }
}

typedef char chud_persistent_user_size[(sizeof(c_chud_persistent_user_data) == 0x2880) ? 1 : -1];
typedef char chud_persistent_fade_offset[(offsetof(c_chud_persistent_user_data, m_cinematic_fade) == 0x238) ? 1 : -1];
typedef char chud_persistent_sensor_offset[(offsetof(c_chud_persistent_user_data, m_motion_sensor) == 0x244) ? 1 : -1];
typedef char chud_persistent_widget_head_offset[(offsetof(c_chud_persistent_user_data,
    m_display_state.widget_list_heads) == 0x1580) ? 1 : -1];
typedef char chud_persistent_navpoint_offset[(offsetof(c_chud_persistent_user_data, m_navpoint_manager) == 0x1634) ? 1 : -1];
typedef char chud_persistent_messaging_offset[(offsetof(c_chud_persistent_user_data, m_messaging_manager) == 0x1fb0) ? 1 : -1];

__forceinline c_chud_impulse_manager *c_chud_impulse_manager::get(long user_index)
{
    return &c_chud_manager::x_persistent_user_data[user_index].m_impulse_manager;
}

// @address 0x472788 chud_set_cutscene_title
extern "C" void chud_set_cutscene_title(const wchar_t *text, float seconds)
{
    for (long user_index = inl_players_first_active_user(); user_index != -1; user_index = players_next_active_user(user_index)) {
        s_chud_persistent_display_state *update = &c_chud_manager::x_persistent_user_data[user_index].m_display_state;
        if (text != 0) {
            update->flash_ticks = game_seconds_to_ticks_round(seconds);
            nl_3193::ustrnzcpy(update->cutscene_title, text, 256);
        } else {
            update->flash_ticks = 0;
        }
    }
}

// @address 0x472580 chud_post_ammo_pickup
extern "C" void chud_post_ammo_pickup(long user_index, long definition_index, short count)
{
    if (user_index >= 0 && user_index < 4 && !game_is_cooperative()) {
        chud_ammo_impulses *ammo = &c_chud_manager::x_persistent_user_data[user_index].m_impulse_manager.ammo;
        for (long i = 0; i < 3; ++i) {
            if (i >= ammo->count) {
                ammo->impulses[i].definition_index = definition_index;
                ammo->impulses[i].count = count;
                ++ammo->count;
                return;
            }
            if (ammo->impulses[i].definition_index == definition_index) {
                ammo->impulses[i].count += count;
                return;
            }
        }
    }
}

void chud_motion_sensor_invalidate(long user_index)
{
    if (user_index >= 0 && user_index < 4) {
        c_chud_manager::x_persistent_user_data[user_index].m_motion_sensor.reset();
    }
}

// @address 0x118dd8 ?chud_set_player_training_text@@YAXJPA_W@Z
void chud_set_player_training_text(long player_index, wchar_t *text)
{
    if (player_index >= 0 && player_index < 4) {
        s_chud_persistent_display_state *persistent_data;
        persistent_data = &c_chud_manager::x_persistent_user_data[player_index].m_display_state;
        if (text != 0) {
            persistent_data->display_active = true;
            nl_3193::ustrnzcpy(persistent_data->player_training_text, text, 256);
        } else {
            persistent_data->display_active = false;
        }
    }
}

typedef char chud_persistent_motion_sensor_offset[(offsetof(c_chud_persistent_user_data, m_motion_sensor) == 0x244) ? 1 : -1];
typedef char chud_persistent_messaging_offset[(offsetof(c_chud_persistent_user_data, m_messaging_manager) == 0x1fb0) ? 1 : -1];
typedef char chud_update_hud_state_offset[(offsetof(c_chud_update_user_data, hud_state) == 0x484) ? 1 : -1];
typedef char chud_runtime_widget_size[(sizeof(s_chud_runtime_widget_datum) == 0x18) ? 1 : -1];
typedef char chud_persistent_display_size[(sizeof(s_chud_persistent_display_state) == 0x72c) ? 1 : -1];
typedef char chud_persistent_user_size[(sizeof(c_chud_persistent_user_data) == 0x2880) ? 1 : -1];
typedef char chud_persistent_display_offset[(offsetof(c_chud_persistent_user_data, m_display_state) == 0xf08) ? 1 : -1];
typedef char chud_display_tracker_offset[(offsetof(s_chud_persistent_display_state, point_tracker) == 0x524) ? 1 : -1];
typedef char chud_display_constants_offset[(offsetof(s_chud_persistent_display_state, curved_hud_constants) == 0x6e8) ? 1 : -1];
typedef char chud_display_resolution_offset[(offsetof(s_chud_persistent_display_state, resolution) == 0x728) ? 1 : -1];

/* ---- 0x12cdf0: per-user chud update -------------------------------------------- */
static __forceinline unsigned __int64 user_interface_get_player_hash_bits(e_controller_index controller_index, long bit_count)
{
    // match: 0x12cdf0: controller indices are limited to the four local controllers.
    __assume((unsigned long)controller_index < 4);
    unsigned __int64 hash_bits = 0xcafebeefcafebeefui64;
    c_controller_interface *controller = controller_get(controller_index);
    if (controller)
        hash_bits = controller->get_hash_bits(bit_count);
    return hash_bits;
}
static __forceinline c_chud_persistent_user_data *chud_persistent_user_data_get(long user_index)
{
    return &c_chud_manager::x_persistent_user_data[user_index];
}
static __forceinline s_chud_runtime_widget_datum *chud_widget_datum_get(long user_index, long widget_index)
{
    return datum_get<s_chud_runtime_widget_datum>(g_chud_widget_data[user_index], widget_index);
}

// @address 0x12cdf0
void c_chud_persistent_user_data::update(long user_index, float dt)
{
    long elapsed_msec = (long)(dt * 1000.0f);
    c_chud_update_user_data update_data;
    chud_update_user_data_build(&update_data, user_index, m_unit_index, m_flags[2]);
    m_flags[2] = false;

    c_static_array<unsigned long, 8> widget_flags[8];
    for (long i = 0; i < NUMBEROF(widget_flags); i++)
        widget_flags[i].set_all(0);

    update_from_update_data(user_index, &update_data);
    update_sounds(user_index, &update_data);

    long controller_index = user_interface_controller_get_rumble_enabled(user_index);
    if (controller_index == -1)
        controller_index = controller_get_first_signed_in_controller();
    if (controller_index != -1)
        m_player_hash_bits = user_interface_get_player_hash_bits((e_controller_index)controller_index, 30);

    m_directional_damage.update(user_index, dt);
    m_cinematic_fade.update();
    m_navpoint_manager.update(user_index);
    m_motion_sensor.update(true, &m_navpoint_manager);
    m_messaging_manager.update(user_index);
    chud_persistent_user_data_get(user_index)->m_display_state.cortana_effect.update(user_index);

    long widget_list_heads[16];
    long widget_list_tails[16];
    memset(widget_list_heads, -1, sizeof(widget_list_heads));
    memset(widget_list_tails, -1, sizeof(widget_list_tails));

    unsigned long active_definition_flags = 0;
    for (long i = 0; i < 8; i++)
    {
        long definition_index = chud_persistent_user_data_get(user_index)->m_weapons[i].definition_index;
        if (definition_index != -1 && update_data.contains_chud_definition_index(definition_index))
            active_definition_flags |= FLAG(i);
    }

    long widget_index;
    {
        s_data_array *widget_data = g_chud_widget_data[user_index];
        long absolute_index = data_next_absolute_index(widget_data, 0);
        widget_index = datum_absolute_index_to_index(widget_data, absolute_index);
    }
    while (widget_index != NONE)
    {
        s_chud_runtime_widget_datum *widget = chud_widget_datum_get(user_index, widget_index);
        chud_persistent_weapon_entry *definition_entry = &chud_persistent_user_data_get(user_index)->m_weapons[widget->widget_index];
        long anchor = 3;
        bool hidden = false;
        long desired_state = 0;
        if ((active_definition_flags & FLAG(widget->widget_index)) != 0)
        {
            desired_state = update_data.compute_desired_widget_state(user_index, definition_entry->definition_index,
                widget->collection_index, widget->definition_index, definition_entry->object_index, &hidden);
        }
        long state = update_data.compute_actual_widget_state(definition_entry->definition_index, widget->collection_index,
            widget->definition_index, definition_entry->object_index, desired_state, widget->state, widget->state_time);

        s_chud_definition *definition = (s_chud_definition *)tag_get('chdt', definition_entry->definition_index);
        if (widget->collection_index >= 0 && widget->collection_index < definition->widget_collection_count)
        {
            s_chud_widget_collection *collection = &(definition->widget_collections)[widget->collection_index];
            long runtime_index = collection->get_widget_runtime_index(widget->definition_index);
            if (runtime_index != -1)
                widget_flags[widget->widget_index].get_elements()[runtime_index >> 5] |= FLAG(runtime_index & 31);
            else
                state = 0;
        }
        else
        {
            state = 0;
        }

        if (state == 0)
        {
            chud_persistent_user_data_get(user_index)->active_chud_definition_notify_widget_deleted(widget->widget_index);
            datum_delete((s_data_array *)g_chud_widget_data[user_index], widget_index);
        }
        else
        {
            s_chud_widget_collection *collection;
            s_chud_widget_base *widget_definition;
            bool text;
            chud_get_widget_collection_and_definition(definition_entry->definition_index, widget->collection_index, widget->definition_index,
                &collection, &widget_definition, &text);
            if (widget_definition)
            {
                anchor = widget_definition->anchor;
                if (anchor == 6)
                    anchor = collection->anchor;
            }

            long impulse_count;
            if (state == 6 && c_chud_update_user_data::widget_definition_should_be_impulse(user_index, widget_definition, collection,
                definition_entry->object_index, &impulse_count))
            {
                widget->state_time = 0;
                widget->external_inputs[0] = (float)impulse_count;
            }

            if (state != widget->state)
            {
                widget->state_time = 0;
                widget->state = (short)state;
            }
            else
            {
                if (!game_time_is_paused())
                    widget->state_time += (short)((32767 - widget->state_time) > elapsed_msec ? elapsed_msec : (32767 - widget->state_time));
                if (collection && widget_definition)
                {
                    long duration = widget_definition->compute_state_duration_msec(widget->state, true);
                    if (duration > 0)
                        widget->state_time = (short)(widget->state_time % (2 * duration));
                }
            }

            if (hidden)
                widget->flags |= FLAG(0);
            else
                widget->flags &= ~FLAG(0);

            if ((widget_definition->flags & FLAG(1)) == 0 || widget->state == 2)
                update_widget_external_inputs(user_index, widget, &update_data);

            long list_index = (text ? 1 : 0) + 2 * anchor;
            if (list_index >= 0 && list_index < NUMBEROF(widget_list_heads))
            {
                if (widget_list_tails[list_index] == -1)
                {
                    widget_list_tails[list_index] = widget_index;
                    widget_list_heads[list_index] = widget_index;
                    widget->next_index = -1;
                }
                else
                {
                    widget->next_index = widget_list_heads[list_index];
                    widget_list_heads[list_index] = widget_index;
                }
            }
        }
        s_data_array *widget_data = g_chud_widget_data[user_index];
        long absolute_index = data_next_absolute_index(widget_data, DATUM_INDEX_TO_ABSOLUTE_INDEX(widget_index + 1));
        widget_index = datum_absolute_index_to_index(widget_data, absolute_index);
    }

    for (long info_index = update_data.get_next_definition_info_index(-1);
        info_index != -1;
        info_index = update_data.get_next_definition_info_index(info_index))
    {
        definition_info *info = update_data.get_definition_info(info_index);
        long active_definition_index = get_active_chud_definition_index(info->definition_index, info->index);
        s_chud_definition *definition = (s_chud_definition *)tag_get('chdt', info->definition_index);
        long collection_count = definition->widget_collection_count;
        for (long collection_index = 0; collection_index < collection_count; collection_index++)
        {
            s_chud_widget_collection *collection = &(definition->widget_collections)[collection_index];
            long widget_count = collection->get_widget_count();
            for (long widget_index = 0; widget_index < widget_count; widget_index++)
            {
                bool exists = active_definition_index != -1
                    ? ((widget_flags[active_definition_index].get_elements()[collection->get_widget_runtime_index(widget_index) >> 5]
                        & FLAG(collection->get_widget_runtime_index(widget_index) & 31)) != 0)
                    : false;
                if (!exists)
                {
                    bool hidden = false;
                    long desired_state = update_data.compute_desired_widget_state(user_index, info->definition_index,
                        collection_index, widget_index, info->index, &hidden);
                    long state = update_data.compute_actual_widget_state(info->definition_index, collection_index,
                        widget_index, info->index, desired_state, 0, 0);
                    if (state != 0)
                    {
                        long anchor = 3;
                        if (active_definition_index == -1)
                        {
                            bool reset_required = false;
                            active_definition_index = add_active_chud_definition_index(info->definition_index, info->index, &reset_required);
                            if (reset_required)
                            {
                                memset(widget_list_heads, -1, sizeof(widget_list_heads));
                                memset(widget_list_tails, -1, sizeof(widget_list_tails));
                            }
                        }

                        chud_persistent_weapon_entry *definition_entry = &chud_persistent_user_data_get(user_index)->m_weapons[active_definition_index];
                        s_chud_widget_collection *widget_collection;
                        s_chud_widget_base *widget_definition;
                        bool text;
                        chud_get_widget_collection_and_definition(definition_entry->definition_index, collection_index, widget_index,
                            &widget_collection, &widget_definition, &text);
                        if (widget_definition)
                        {
                            anchor = widget_definition->anchor;
                            if (anchor == 6)
                                anchor = widget_collection->anchor;
                        }

                        long new_widget_index = datum_new((s_data_array *)g_chud_widget_data[user_index]);
                        if (new_widget_index != -1)
                        {
                            s_chud_runtime_widget_datum *widget = chud_widget_datum_get(user_index, new_widget_index);
                            widget->widget_index = (signed char)active_definition_index;
                            widget->collection_index = (signed char)collection_index;
                            widget->definition_index = (signed char)widget_index;
                            widget->state = (short)state;
                            widget->state_time = 0;
                            if (hidden)
                                widget->flags |= FLAG(0);
                            else
                                widget->flags &= ~FLAG(0);
                            if ((widget_definition->flags & FLAG(4)) != 0 && update_data.hud_reset)
                                widget->state_time = 10000;
                            for (long input_index = 0; input_index < 2; input_index++)
                                widget->external_inputs[input_index] = 0.0f;
                            chud_persistent_user_data_get(user_index)->active_chud_definition_notify_widget_created(widget->widget_index);

                            long impulse_count;
                            if (state == 6 && c_chud_update_user_data::widget_definition_should_be_impulse(user_index, widget_definition, widget_collection,
                                definition_entry->object_index, &impulse_count))
                            {
                                widget->external_inputs[0] = (float)impulse_count;
                            }
                            update_widget_external_inputs(user_index, widget, &update_data);

                            long list_index = (text ? 1 : 0) + 2 * anchor;
                            if (list_index >= 0 && list_index < NUMBEROF(widget_list_heads))
                            {
                                if (widget_list_tails[list_index] == -1)
                                {
                                    widget_list_tails[list_index] = new_widget_index;
                                    widget_list_heads[list_index] = new_widget_index;
                                    widget->next_index = -1;
                                }
                                else
                                {
                                    widget->next_index = widget_list_heads[list_index];
                                    widget_list_heads[list_index] = new_widget_index;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    s_chud_persistent_display_state *display_state = &chud_persistent_user_data_get(m_user_index)->m_display_state;
    long head = -1;
    long tail = -1;
    long anchor7_head = -1;
    long anchor7_tail = -1;
    for (long list_index = 0; list_index < NUMBEROF(widget_list_heads); list_index++)
    {
        bool not_anchor7 = (list_index >> 1) != 7;
        long *merged_head = not_anchor7 ? &head : &anchor7_head;
        long *merged_tail = not_anchor7 ? &tail : &anchor7_tail;
        long list_head = widget_list_heads[list_index];
        long list_tail = widget_list_tails[list_index];
        if (list_head != -1)
        {
            if (*merged_head == -1)
            {
                *merged_head = list_head;
                *merged_tail = list_tail;
            }
            else
            {
                chud_widget_datum_get(user_index, *merged_tail)->next_index = list_head;
                *merged_tail = list_tail;
            }
        }
    }
    display_state->widget_list_heads[0] = head;
    display_state->widget_list_heads[1] = anchor7_head;
    chud_impulse_manager_reset(&m_impulse_manager);
}
// @address 0x471440 ?reset@c_chud_persistent_user_data@@QAAXXZ
void c_chud_persistent_user_data::reset()
{
    for (long i = 0; i < 8; i++)
    {
        m_weapons[i].count = 0;
        m_weapons[i].definition_index = -1;
        m_weapons[i].object_index = -1;
    }
    chud_directional_damage_reset(&m_directional_damage);
    m_cinematic_fade.set_fade(1.0f, 0.0f);
    m_motion_sensor.reset();
    m_navpoint_manager.reset();
    chud_impulse_manager_reset(&m_impulse_manager);
    m_messaging_manager.reset();
    m_flags[0] = false;
    m_flags[1] = false;
    m_flags[2] = false;
    s_chud_persistent_display_state *update_data = &m_display_state;
    chud_persistent_display_state_reset(update_data);
    update_data->perspective_opacity = ((s_controllers *)g_director_globals)->slots[m_user_index].perspective == 0 ? 1.0f : 0.0f;
    update_data->perspective_changed = false;
}

// @address 0x4713a8 chud_persistent_user_data_initialize
extern "C" void chud_persistent_user_data_initialize(c_chud_persistent_user_data *data, long user_index)
{
    data->m_user_index = user_index;
    data->m_unit_index = -1;
    c_chud_motion_sensor *sensor = &data->m_motion_sensor;
    sensor->m_user_index = user_index;
    sensor->m_tracking_distance = 10.0f;
    sensor->m_sensor_range = 8.0f;
    sensor->m_sensor_scale = 0.125f;
    for (long i = 0; i < 20; i++)
        sensor->m_tracked_entities[i].object_index = -1;
    for (long i = 0; i < 10; i++)
        sensor->m_contacts[i].active = 0;
    sensor->m_valid_index = 0;
    sensor->m_tracking_flags = 0;
    memset(data->m_tail_indices, 255, sizeof(data->m_tail_indices));
    data->m_tail_flags = 0;
    data->reset();
}

static __forceinline void chud_scripting_initialize_for_new_map(chud_script_flags *scripting)
{
    scripting->enabled = true;
    scripting->items[0] = true;
    scripting->items[1] = true;
    scripting->items[2] = true;
    scripting->items[3] = true;
    scripting->items[4] = true;
    scripting->items[5] = true;
    scripting->items[6] = true;
    scripting->items[7] = true;
}

// @address 0x471f70 ?chud_initialize_for_new_map@@YAXXZ
void chud_initialize_for_new_map(void)
{
    s_game_globals *game_globals = global_game_globals;
    s_game_globals_interface_tags *interface_tags =
        game_globals->interface_tags.count != 0
            ? (s_game_globals_interface_tags *)game_globals->interface_tags.address
            : 0;
    if (interface_tags->chud_globals.index != -1)
        chud_globals = (s_chud_globals_definition *)tag_get('chgd', interface_tags->chud_globals.index);
    else
        chud_globals = 0;
    for (long user_index = 0; user_index < 4; ++user_index)
    {
        data_make_valid(g_chud_widget_data[user_index]);
        chud_persistent_user_data_initialize(&c_chud_manager::x_persistent_user_data[user_index],
            user_index);
        c_chud_manager::x_persistent_user_data[user_index].initialize_sounds(user_index);
    }
    chud_scripting_initialize_for_new_map(&g_chud_scripting_globals->scripting);
    memset(&g_chud_cinematic_state->index, 0, sizeof(g_chud_cinematic_state->index));
    g_chud_cinematic_state->index = -1;
}


static __forceinline void chud_set_real_vector4d(float *vector, float i, float j, float k, float l)
{
    vector[0] = i;
    vector[1] = j;
    vector[2] = k;
    vector[3] = l;
}

// @address 0x131118 ?update_from_update_data@c_chud_persistent_user_data@@QAAXJPAVc_chud_update_user_data@@@Z
void c_chud_persistent_user_data::update_from_update_data(long user_index, c_chud_update_user_data *update_data)
{
    long unit_index = update_data->unit_index;
    bool hud_changed = false;
    s_chud_persistent_display_state *display = &c_chud_manager::x_persistent_user_data[user_index].m_display_state;

    if (m_unit_index != unit_index)
    {
        s_chud_globals_definition *table = chud_globals;
        if (table && user_index >= 0 && user_index < 4)
        {
            chud_table_row *row = &table->rows[chud_get_user_skin_index(user_index)];
            chud_play_sound(user_index, 0, &row->sounds, m_tail_indices, &m_tail_flags);
        }
        reset();
        data_delete_all((s_data_array *)g_chud_widget_data[m_user_index]);
        m_unit_index = unit_index;
        hud_changed = true;
        update_data->hud_reset = true;
    }

    if (update_data->unit_index != -1)
    {
        float shield_vitality = update_data->shield_vitality;
        float last_shield_vitality = m_last_shield_vitality;
        if (last_shield_vitality > shield_vitality)
        {
            long recharge_ticks = m_shield_recharge_ticks;
            if (recharge_ticks < 0 || recharge_ticks > 1)
                m_shield_update_time = game_time_globals->time;
            if (game_ticks_to_seconds(game_time_globals->time - m_shield_update_time) < 0.5f)
            {
                m_shield_recharge_ticks = 0;
            }
            else
            {
                m_last_shield_vitality = shield_vitality;
                m_shield_recharge_ticks = (long)(game_ticks_to_seconds(game_time_globals->time - m_shield_update_time) + (float)recharge_ticks);
                m_shield_update_time = game_time_globals->time;
            }
        }
        else
        {
            m_last_shield_vitality = shield_vitality;
            if (last_shield_vitality < shield_vitality)
                m_shield_recharge_ticks = -1;
            else if (m_shield_recharge_ticks > 0)
                m_shield_recharge_ticks = (long)(game_ticks_to_seconds(game_time_globals->time - m_shield_update_time) + (float)m_shield_recharge_ticks);
            m_shield_update_time = game_time_globals->time;
        }
    }
    else
    {
        m_last_shield_vitality = 0.0f;
        m_shield_recharge_ticks = -1;
        m_shield_update_time = game_time_globals->time;
    }

    if (update_data->unit_index != -1)
    {
        long target_index = -1;
        bool has_target = false;
        if (update_data->autoaim_target_type != -1 && update_data->autoaim_target_index != -1)
        {
            target_index = update_data->autoaim_target_index;
            has_target = true;
        }
        if (has_target)
        {
            real_point3d camera_position = chud_user_observer_camera(user_index)->position;
            real_point3d target_position;
            object_get_origin(target_index, &target_position);
            real_vector3d offset;
            vector_from_points3d(&target_position, &camera_position, &offset);
            m_target_height = (target_position.z - camera_position.z) * 3.048f;
            m_target_distance = sqrtf(dot_product3d(&offset, &offset)) * 3.048f;
        }
    }

    if (update_data->unit_index != -1)
    {
        chud_table_row *row = &chud_globals->rows[chud_get_user_skin_index(user_index)];
        if (update_data->alternate_state)
            display->weapon_state_value = row->alternate_value;
        else if (update_data->has_no_secondary_weapons())
            display->weapon_state_value = row->no_secondary_weapon_value;
        else
            display->weapon_state_value = row->secondary_weapon_value;
        if (update_data->highlight_index != -1)
            display->highlight_value = row->highlight_value;
        else
            display->highlight_value = 0.0f;
    }

    long score_count = update_data->saved_film_progress.marker_count;
    chud_set_real_vector4d(&display->scores[0], update_data->get_score(score_count, 0), update_data->get_score(score_count, 1),
        update_data->get_score(score_count, 2), update_data->get_score(score_count, 3));
    chud_set_real_vector4d(&display->scores[4], update_data->get_score(score_count, 4), update_data->get_score(score_count, 5),
        update_data->get_score(score_count, 6), update_data->get_score(score_count, 7));
    chud_set_real_vector4d(&display->scores[8], update_data->get_score(score_count, 8), update_data->get_score(score_count, 9), -1.0f, -1.0f);
    chud_set_real_vector4d(&display->external_input_718, update_data->saved_film_progress.active ? update_data->saved_film_progress.rate : 2.0f,
        update_data->saved_film_progress.progress, update_data->saved_film_progress.buffered, -1.0f);

    nl_3193::ustrnzcpy(display->text_410, update_data->hud_state.sudden_death_text, 20);
    nl_3193::ustrnzcpy(display->text_438, update_data->hud_state.flag_return_text, 32);
    nl_3193::ustrnzcpy(display->text_478, update_data->hud_state.talker_name, 32);
    nl_3193::ustrnzcpy(display->text_4b8, update_data->hud_state.variant_name, 32);
    display->icon_4fc = update_data->hud_state.own_color;
    display->icon_500 = update_data->hud_state.rival_color;
    display->icon_504 = update_data->hud_state.flag_return_color;

    bool visible = update_data->hud_state.leading;
    if (display->visible_initialized)
    {
        float *opacity = &display->visible_opacity;
        *opacity += visible ? 0.06666667f : -0.06666667f;
        display->visible_opacity = PIN(display->visible_opacity, 0.0f, 1.0f);
    }
    else
    {
        display->visible_initialized = true;
        display->visible_opacity = visible ? 1.0f : 0.0f;
    }

    display->count_text_width = 0;
    if (update_data->hud_state.stats_valid)
    {
        wchar_t count_text[128];
        usnzprintf(count_text, 128, L"%d", update_data->hud_state.own_score);
        float width = chud_draw_get_string_width(6, count_text);
        display->count_text_width = (long)maximum((float)display->count_text_width, width);
    }
    if (update_data->hud_state.rival_valid)
    {
        wchar_t count_text[128];
        usnzprintf(count_text, 128, L"%d", update_data->hud_state.rival_score);
        float width = chud_draw_get_string_width(6, count_text);
        display->count_text_width = (long)maximum((float)display->count_text_width, width);
    }

    chud_metagame_state *color_data = &update_data->metagame_state;
    for (long color_index = 0; color_index < 4; color_index++)
    {
        display->color_group = color_data->player_count;
        display->color_labels[color_index] = color_data->color_labels[color_index];
        display->colors[color_index].alpha = 1.0f;
        real_rgb_color color;
        display->colors[color_index].rgb = *player_profile_get_rgb_color(&color, color_data->teams[color_index]);
    }

    float perspective_step = 1.0f / ((float)game_time_globals->ticks_per_second * 0.5f);
    if (((s_controllers *)g_director_globals)->slots[user_index].perspective == 0)
        display->perspective_opacity += perspective_step;
    else
        display->perspective_opacity -= perspective_step;
    display->perspective_opacity = chud_user_observer_camera(user_index)->view_value;

    display->perspective_changed = false;
    if (chud_scripting_is_element_enabled(&g_chud_scripting_globals->scripting, 8, user_index) &&
        !chud_scripting_is_element_enabled(&g_chud_scripting_globals->scripting, 7, user_index))
        display->perspective_changed = true;

    if (unit_index != -1)
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 3);
        long equipment_index = unit->unit.equipment_object_index;
        display->equipment_opacity = 0.0f;
        if (equipment_index != -1)
        {
            unit_datum *equipment = (unit_datum *)object_get_and_verify_type(equipment_index, 0x800);
            s_equipment_definition *definition = (s_equipment_definition *)tag_get('eqip', equipment->object.definition_index);
            if (definition->equipment_types[7].count != 0)
            {
                float fraction = equipment_active_fraction(equipment_index);
                if (fraction > 0.0001f)
                {
                    if (fraction < 0.05f)
                    {
                        display->equipment_opacity = 1.0f - (0.05f - fraction) * 20.0f;
                    }
                    else
                    {
                        float remaining = 1.0f - fraction;
                        if (remaining < 0.05f)
                            display->equipment_opacity = (0.05f - remaining) * 20.0f;
                        else
                            display->equipment_opacity = 1.0f;
                    }
                }
            }
        }
    }

    bool weapon_zoomed = false;
    long weapon_slots[4] = { 0, 1, 3, 4 };
    for (long slot_index = 0; slot_index < 4; slot_index++)
    {
        definition_info *definition = &update_data->definitions[weapon_slots[slot_index]];
        if (definition->index != -1)
        {
            weapon_state *weapon = &definition->weapon;
            if (weapon->locked || weapon->locking)
            {
                weapon_zoomed = true;
                display->point_tracker_position = weapon->target_position;
                break;
            }
        }
    }
    display->weapon_zoomed = weapon_zoomed;

    if (!hud_changed)
    {
        if (!update_data->has_primary_weapons() || update_data->autoaim_target_type != -1 || !m_flags[1])
            hud_changed = true;
    }
    update_data->hud_changed = hud_changed;
    m_flags[1] = update_data->has_primary_weapons();
    if (display->flash_ticks > 0)
        display->flash_ticks--;
    display->resolution = chud_get_layout_index(user_index);
}

static __forceinline unsigned long chud_game_campaign_skulls()
{
    game_options *options = &game_globals->options;
    return options->game_mode == 1 ? options->campaign_skulls : 0;
}

static __forceinline bool chud_game_skull_is_active(long skull)
{
    return ((chud_game_campaign_skulls() >> skull) & 1) != 0;
}

static __forceinline const c_game_variant *chud_game_multiplayer_variant()
{
    game_options *options = &game_globals->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}

static __forceinline const void *chud_game_variant_get_engine_variant(const c_game_variant *variant, long engine_type)
{
    return variant->engine_type == engine_type ? variant->m_variant_storage : 0;
}

// @address 0x131d00 ?get_sound_flags@c_chud_update_user_data@@QAAKXZ
unsigned long c_chud_update_user_data::get_sound_flags()
{
    float low_shield_threshold = 0.25f;
    unsigned long flags = 0;
    if (!inl_cinematic_in_progress() && unit_index != -1)
    {
        float last_shield_vitality = c_chud_manager::x_persistent_user_data[user_index].m_last_shield_vitality;
        if (last_shield_vitality != -1.0f && game_engine_player_has_shield(player_index))
        {
            if (state_flags & 0x400)
                flags |= 1;
            else
                flags &= ~1;
            if (last_shield_vitality > shield_vitality)
                flags |= 2;
            else
                flags &= ~2;
            if (!chud_game_skull_is_active(1))
            {
                if (shield_vitality < low_shield_threshold && shield_vitality > 0.0f)
                    flags |= 4;
                else
                    flags &= ~4;
                if (shield_vitality == 0.0f)
                    flags |= 8;
                else
                    flags &= ~8;
            }
        }
        long weapon_index = unit_inventory_get_weapon_or_none(unit_index, ((unit_datum *)object_get_and_verify_type(unit_index,
            8195))->unit.current_weapon_index);
        if (weapon_index != -1)
        {
            weapon_state *weapon = &definitions[weapon_is_support_weapon(weapon_index) ? 3 : 0].weapon;
            if (weapon->locking)
                flags |= 0x100;
            else
                flags &= ~0x100;
            if (weapon->locked)
                flags |= 0x200;
            else
                flags &= ~0x200;
        }
        if (weapon_ready_sound)
            flags |= 0x400;
        else
            flags &= ~0x400;
        if (weapon_overheat_sound)
            flags |= 0x800;
        else
            flags &= ~0x800;
        if (game_is_multiplayer())
        {
            const c_game_variant *variant = chud_game_multiplayer_variant();
            const void *vip_variant = chud_game_variant_get_engine_variant(variant, 6);
            const void *juggernaut_variant = chud_game_variant_get_engine_variant(variant, 7);
            const void *infection_variant = chud_game_variant_get_engine_variant(variant, 10);
            bool vip = vip_variant && (hud_state.ctf_flags & 1);
            bool juggernaut = juggernaut_variant && (hud_state.ctf_flags & 1);
            bool zombie = infection_variant && (hud_state.ctf_flags & 1);
            bool last_man = infection_variant && (bool)((hud_state.ctf_flags >> 1) & 1);
            if (vip)
                flags |= 0x1000;
            else
                flags &= ~0x1000;
            if (juggernaut)
                flags |= 0x2000;
            else
                flags &= ~0x2000;
            if (zombie)
                flags |= 0x4000;
            else
                flags &= ~0x4000;
            if (last_man)
                flags |= 0x8000;
            else
                flags &= ~0x8000;
        }
        if (game_is_multiplayer() && shield_vitality >= 1.0f)
            flags &= ~2;
    }
    return flags;
}


// @address 0x8c29c5 g_chud_metagame_debug_enabled
bool g_chud_metagame_debug_enabled;
// @address 0x119a410 g_chud_debug_medal_counter
long g_chud_debug_medal_counter;

static __forceinline bool chud_campaign_metagame_enabled()
{
    return game_globals->options.campaign_metagame_enabled && !(g_campaing_metagame_globals->flags & 0x10)
        && !(g_campaing_metagame_globals->flags & 0x20);
}

static __forceinline const s_campaign_metagame_definition *chud_campaign_metagame_definition_get()
{
    const s_campaign_metagame_definition *definition = 0;
    if (global_game_globals && global_game_globals->campaign_metagame.count() == 1)
        definition = global_game_globals->campaign_metagame.begin();
    return definition;
}

static __forceinline void chud_metagame_post_debug_medal(long medal_value)
{
    if (campaign_metagame_is_enabled())
    {
        c_chud_messaging_manager *manager = &c_chud_manager::x_persistent_user_data->m_messaging_manager;
        long result;
        s_chud_messaging_handle handle = { chud_globals };
        if (chud_messaging_clamp_index(handle, medal_value, &result))
        {
            s_chud_medal_message *medal = &manager->m_medals[chud_messaging_manager_get_new_medal_index(manager)];
            medal->active = true;
            medal->medal_value = medal_value;
            medal->shown = false;
            medal->time = game_time_globals->time;
            medal->target = 0.0f;
            medal->value = 0.0f;
        }
    }
}

// @address 0x130610 chud_get_metagame_state
extern "C" bool chud_get_metagame_state(long user_index, chud_metagame_state *state)
{
    long player_index = player_index_from_user_index_render(user_index);
    const s_campaign_metagame_definition *definition = chud_campaign_metagame_definition_get();
    memset(state, 0, sizeof(*state));
    if (chud_campaign_metagame_enabled() && player_index != -1)
    {
        state->player_count = 1;
        if (game_globals->options.game_mode == 1 && definition && definition->style_multipliers.count() == 8)
        {
            c_chud_data_iterator<player_datum> iterator;
            iterator.begin(player_data);
            while (iterator.next())
            {
                long index = iterator.get_index();
                s_campaing_metagame_player *owner_slot = campaign_metagame_try_and_get_owner_slot(g_campaing_metagame_globals, index, 1);
                s_campaign_metagame_player_slot *player_slot = campaign_metagame_try_and_get_player_slot(&g_campaign_metagame_player_globals, index, 1);
                long slot;
                if (index == player_index)
                    slot = 0;
                else
                    slot = state->player_count++;
                long score = owner_slot->score - player_slot->score + owner_slot->awarded_bonus;
                state->emblems[slot] = iterator.get_datum()->configuration.client.appearance.emblem;
                state->teams[slot] = iterator.get_datum()->configuration.client.appearance.primary_color;
                state->scores[slot] = score;
                state->local[slot] = false;
                if (slot == 0)
                {
                    state->time = (float)g_campaign_metagame_player_globals.ticks * game_time_globals->tick_length / 60.0f;
                    state->survival_time = owner_slot->pending;
                    if (owner_slot->timer2 > 0)
                    {
                        state->survival_time += owner_slot->last_delta;
                        state->scores[slot] -= owner_slot->last_delta;
                    }
                    state->multiplier = campaign_metagame_get_score_scale();
                    state->score_scale = campaign_metagame_get_time_multiplier(g_campaign_metagame_player_globals.ticks, 0);
                    state->bonus_round = owner_slot->timer0 > 0;
                    state->bonus_round_index = owner_slot->points;
                    state->bonus_round_timer = owner_slot->timer1 > 0;
                }
            }
            if (game_globals->options.campaign_metagame_scoring == 1)
            {
                long first = 0;
                for (long i = 1; i < state->player_count; i++)
                    state->scores[first] += state->scores[i];
                state->player_count = 1;
            }
        }
    }
    if (g_chud_metagame_debug_enabled)
    {
        state->local[0] = true;
        state->local[1] = false;
        state->local[2] = false;
        state->scores[1] = -456;
        state->time = 123.45f;
        state->player_count = 3;
        state->scores[0] = 123;
        state->multiplier = 3.0f;
        state->scores[2] = 0x786450;
        for (long i = 0; i < 4; i++)
        {
            state->emblems[i].background_color = 3;
            state->emblems[i].background = 4;
            state->emblems[i].flags = 0;
            state->emblems[i].foreground = 5;
            state->emblems[i].primary_color = 6;
            state->emblems[i].secondary_color = 7;
        }
        long first_player = player_index_from_user_index_render(inl_next_active_user(-1));
        if (first_player != -1)
            state->emblems[0] = player_get(first_player)->configuration.client.appearance.emblem;
        switch ((game_time_globals->time % (game_time_globals->ticks_per_second * 3)) / game_time_globals->ticks_per_second)
        {
        case 0:
            state->survival_time = 123;
            break;
        case 1:
            state->survival_time = 234;
            break;
        case 2:
            state->survival_time = 0;
            break;
        }
        if (game_time_globals->time % 70 == 0)
        {
            g_chud_debug_medal_counter++;
            switch (g_chud_debug_medal_counter % 3)
            {
            case 0:
                chud_metagame_post_debug_medal(0);
                break;
            case 1:
                chud_metagame_post_debug_medal(1);
                break;
            }
        }
    }
    return state->player_count > 0;
}


static __forceinline real_euler_angles2d *chud_player_control_get_facing_angles(long user_index)
{
    return &player_control_globals->states[user_index].action.look;
}
static __forceinline c_chud_persistent_user_data *chud_external_user_state_get(long user_index)
{
    return &c_chud_manager::x_persistent_user_data[user_index];
}

static __forceinline void *chud11_tag_block_get_element(const s_tag_block *block, long index, long element_size)
{
    return (char *)block->address + index * element_size;
}
static __forceinline s_chud_widget_base *chud11_widget_definition_collection_get_widget(s_chud_widget_collection *collection, long index)
{
    if (index < collection->bitmap_widget_count)
        return &collection->bitmap_widgets[index].base;
    index -= collection->bitmap_widget_count;
    return &collection->text_widgets[index].base;
}
static __forceinline long chud11_persistent_user_data_get_widget_anchor_type(s_chud_widget_base *widget,
    s_chud_widget_collection *collection)
{
    long result = -1;
    if (widget->placement.count != 0)
    {
        s_chud_widget_placement_data *element = (s_chud_widget_placement_data *)widget->placement.address;
        if (element != 0)
        {
            result = element->anchor;
            if ((unsigned long)result == 12)
            {
                if (collection->placement.count != 0)
                {
                    s_chud_widget_placement_data *collection_element = (s_chud_widget_placement_data *)collection->placement.address;
                    if (collection_element != 0)
                        result = collection_element->anchor;
                }
            }
        }
    }
    return result;
}
// @address 0x136338 ?update_widget_external_inputs@c_chud_persistent_user_data@@QAAXJPAUs_chud_runtime_widget_datum@@PAVc_chud_update_user_data@@@Z
void c_chud_persistent_user_data::update_widget_external_inputs(long user_index, s_chud_runtime_widget_datum *widget,
    c_chud_update_user_data *update_data)
{
    chud_persistent_weapon_entry *entry = &c_chud_manager::x_persistent_user_data[user_index].m_weapons[widget->widget_index];
    s_chud_definition *definition = (s_chud_definition *)tag_get(1667785844, entry->definition_index);
    s_chud_widget_collection *collection = &definition->widget_collections[widget->collection_index];
    s_chud_widget_base *widget_entry = chud11_widget_definition_collection_get_widget(collection, widget->definition_index);
    s_chud_widget_render_data *inputs;
    if (widget_entry->render_data.count != 0)
        inputs = (s_chud_widget_render_data *)widget_entry->render_data.address;
    else
        inputs = 0;
    long anchor_type = chud11_persistent_user_data_get_widget_anchor_type(widget_entry, collection);
    if (inputs != 0)
    {
        float *output = &widget->external_inputs[0];
        short *input_index = inputs->external_inputs;
        unsigned long remaining = 2;
        do
        {
            float value = *output;
            *output = update_data->evaluate_external_input((short)*input_index, entry->object_index, anchor_type, value);
            --remaining;
            ++input_index;
            ++output;
        } while (remaining != 0);
    }
    if ((widget_entry->flags & 4) != 0 && widget->state == 2)
        widget->state_time = 0;
}
// @address 0x1364b0 ?evaluate_external_input@c_chud_update_user_data@@QAAMJJJM@Z
float c_chud_update_user_data::evaluate_external_input(long input_type, long weapon_index, long state_index, float value)
{
    float result = 1.0f;
    weapon_state *weapon = find_weapon_state(weapon_index);
    switch (input_type)
    {
    case _chud_external_input_zero:
        result = 0.0f;
        break;
    case _chud_external_input_one:
        result = 1.0f;
        break;
    case _chud_external_input_time_byte:
        {
            float byte_value = (float)(unsigned char)game_time_get();
            result = (float)(byte_value / 255.0);
        }
        break;
    case _chud_external_input_time_modulo_100:
    {
        long time = game_time_get();
        result = (float)(time - time / 100 * 100);
        break;
    }
    case _chud_external_input_shield_vitality:
        result = MIN(MAX(shield_vitality, 0.0f), 1.0f) * 255.0f;
        break;
    case _chud_external_input_overshield_vitality_1:
        result = MIN(MAX(shield_vitality - 1.0f, 0.0f), 1.0f) * 255.0f;
        break;
    case _chud_external_input_overshield_vitality_2:
        result = MIN(MAX(shield_vitality - 2.0f, 0.0f), 1.0f) * 255.0f;
        break;
    case _chud_external_input_overshield_vitality_3:
        result = MIN(MAX(shield_vitality - 3.0f, 0.0f), 1.0f) * 255.0f;
        break;
    case _chud_external_input_recent_shield_vitality:
        result = MIN(MAX(chud_external_user_state_get(user_index)->m_last_shield_vitality, 0.0f), 1.0f) * 255.0f;
        break;
    case _chud_external_input_recent_overshield_vitality_1:
        result = MIN(MAX(chud_external_user_state_get(user_index)->m_last_shield_vitality - 1.0f, 0.0f), 1.0f) * 255.0f;
        break;
    case _chud_external_input_recent_overshield_vitality_2:
        result = MIN(MAX(chud_external_user_state_get(user_index)->m_last_shield_vitality - 2.0f, 0.0f), 1.0f) * 255.0f;
        break;
    case _chud_external_input_recent_overshield_vitality_3:
        result = MIN(MAX(chud_external_user_state_get(user_index)->m_last_shield_vitality - 3.0f, 0.0f), 1.0f) * 255.0f;
        break;
    case _chud_external_input_weapon_rounds_loaded:
        if (weapon)
            result = weapon->rounds_loaded;
        break;
    case _chud_external_input_weapon_rounds_total:
        if (weapon)
            result = weapon->rounds_total;
        break;
    case _chud_external_input_weapon_heat:
        if (weapon)
            result = weapon->heat * 255.0f;
        break;
    case _chud_external_input_weapon_age:
        if (weapon)
            result = weapon->age;
        break;
    case _chud_external_input_animation_value:
        result = value;
        break;
    case _chud_external_input_zoom_level:
        result = zoom_level;
        if (autoaim_flags & 1)
            result = -1.0f;
        break;
    case _chud_external_input_selected_grenade:
        if (state_index >= 6 && state_index <= 9)
            result = state_index - 6 == selected_grenade_index ? 1.0f : 0.0f;
        break;
    case _chud_external_input_grenade_count:
        if (state_index >= 6 && state_index <= 9)
            result = (float)grenade_counts[state_index - 6];
        break;
    case _chud_external_input_weapon_charge:
        if (weapon)
            result = weapon->charge;
        break;
    case _chud_external_input_weapon_lock_fraction:
        if (weapon)
            result = weapon->lock_fraction;
        break;
    case _chud_external_input_own_score:
        result = (float)hud_state.own_score;
        break;
    case _chud_external_input_rival_score:
        result = (float)hud_state.rival_score;
        break;
    case _chud_external_input_score_to_win:
        result = (float)hud_state.score_to_win;
        break;
    case _chud_external_input_flag_return_progress:
        result = hud_state.flag_return_progress;
        break;
    case _chud_external_input_facing_yaw:
        result = chud_player_control_get_facing_angles(user_index)->yaw / 6.2831855f;
        break;
    case _chud_external_input_facing_pitch:
        result = (chud_player_control_get_facing_angles(user_index)->pitch + 1.5707964f) / 3.1415927f;
        break;
    case _chud_external_input_target_distance:
        result = chud_external_user_state_get(user_index)->m_target_distance;
        break;
    case _chud_external_input_target_height:
        result = chud_external_user_state_get(user_index)->m_target_height;
        break;
    case _chud_external_input_budget_fraction:
        result = hud_state.budget_fraction;
        break;
    case _chud_external_input_budget_remaining:
        result = hud_state.budget - hud_state.budget_used;
        break;
    case _chud_external_input_saved_film_buffered:
        result = saved_film_progress.buffered;
        break;
    case _chud_external_input_saved_film_duration:
        result = saved_film_progress.duration;
        break;
    case _chud_external_input_saved_film_position:
        result = saved_film_progress.position;
        break;
    case _chud_external_input_saved_film_rate:
        result = saved_film_progress.rate;
        break;
    case _chud_external_input_saved_film_progress:
        result = saved_film_progress.progress;
        break;
    case _chud_external_input_metagame_time:
        result = metagame_state.time;
        break;
    case _chud_external_input_metagame_survival_time:
        result = (float)metagame_state.survival_time;
        break;
    case _chud_external_input_metagame_score_0:
        result = (float)metagame_state.scores[0];
        break;
    case _chud_external_input_metagame_score_1:
        result = (float)metagame_state.scores[1];
        break;
    case _chud_external_input_metagame_score_2:
        result = (float)metagame_state.scores[2];
        break;
    case _chud_external_input_metagame_score_3:
        result = (float)metagame_state.scores[3];
        break;
    case _chud_external_input_metagame_score_scale:
        result = metagame_state.score_scale;
        break;
    case _chud_external_input_metagame_multiplier:
        result = metagame_state.multiplier;
        break;
    case _chud_external_input_metagame_bonus_round_index:
        result = (float)metagame_state.bonus_round_index;
        break;
    case _chud_external_input_energy_fraction:
        result = energy_fraction;
        break;
    case _chud_external_input_external_count:
        result = (float)external_count_6dc;
        break;
    case _chud_external_input_external_value_0:
        result = (float)chud_get_external_type_value(external_types_6e0[0]);
        break;
    case _chud_external_input_external_value_1:
        result = (float)chud_get_external_type_value(external_types_6e0[1]);
        break;
    case _chud_external_input_external_value_2:
        result = (float)chud_get_external_type_value(external_types_6e0[2]);
        break;
    default:
        break;
    }
    return result;
}

// match: 0x49b610: precise blocks the 1.0f/denominator reciprocal and keeps per-component mul-then-div order
#pragma float_control(precise, on, push)
// @address 0x49b610 chud_compute_projection_point
extern "C" void chud_compute_projection_point(c_curved_hud_test *data, float first, float second, real_point2d *point)
{
    real_vector3d first_direction;
    vectors_interpolate_toward(&data->m_shader_constants[0].xyz, &data->m_shader_constants[1].xyz,
        first,
        &first_direction);
    real_vector3d second_direction;
    vectors_interpolate_toward(&data->m_shader_constants[3].xyz, &data->m_shader_constants[2].xyz,
        first,
        &second_direction);
    real_vector3d direction;
    vectors_interpolate_toward(&first_direction, &second_direction, second, &direction);
    normalize3d(&direction);
    float distance = data->m_distance;
    float y_scale = data->m_projection_scale.j;
    float x_scale = data->m_projection_scale.i;
    float denominator = distance + direction.k;
    float x = x_scale * direction.i / denominator;
    float y = y_scale * direction.j / denominator;
    point->x = x;
    point->y = y;
}
// match: 0x49b610: pops the precise block so later code uses default float contraction
#pragma float_control(pop)

// @address 0x471cb0 chud_post_damage_flash
extern "C" void chud_post_damage_flash(s_equipment_slot *flashes, long user_index, long flash_index, c_chud_directional_damage *directional_damage)
{
    s_equipment_slot *flash = &flashes[flash_index];
    union { long ticks; s_damage_flash_elapsed_ticks value; } elapsed;
    elapsed.ticks = game_time_globals->time - flash->start_time;
    s_damage_flash_elapsed_ticks elapsed_ticks = elapsed.value;
    unsigned long seed = (elapsed_ticks.value << 4) ^ flash->identifier;
    long random = chud_random_value(&seed);
    if (!game_time_is_paused() && (random & 4) && ((random & 0x10) || (random & 0x40)) && (random & 0x100) && (random & 0x200) &&
        (random & 0x800) && user_index != -1)
    {
        long unit_index = player_mapping_globals->unit_indices[user_index];
        if (unit_index != -1)
        {
            s_chud_globals_definition *table = chud_globals;
            if (table)
            {
                chud_table_row *row = &table->rows[chud_get_user_skin_index(user_index)];
                if (row->damage_flash_effect.index != -1)
                {
                    real_euler_angles2d angles;
                    {
                        real_point3d head_position;
                        real_vector3d direction;
                        unit_get_head_position(unit_index, &head_position);
                        vector_from_points3d(&flash->center, &head_position, &direction);
                        s_damage_flash_direction_argument normal_direction = { &direction };
                        ((float (*)(s_damage_flash_direction_argument))normalize3d)(normal_direction);
                        euler_angles2d_from_vector3d(&angles, &direction);
                    }
                    angles.yaw += chud_random_fraction(&seed) * 0.2617994f - 0.2617994f;
                    angles.pitch += chud_random_fraction(&seed) * 0.2617994f - 0.2617994f;
                    angles.pitch = PIN(angles.pitch, -1.5707964f, 1.5707964f);
                    {
                        real_vector3d forward;
                        vector3d_from_euler_angles2d(&forward, &angles);
#if defined(_XBOX) && !defined(__clang__)
                        // match: 0x471cb0: shared constant-pool base register addresses both k_damage_owner_unknown and the 1.0f load
                        s_damage_flash_owner_argument owner = { (const s_damage_owner *)&k_real_constant_pool_one[7497] };
#else
                        s_damage_flash_owner_argument owner = { &k_damage_owner_unknown };
#endif
                        float one = real_constant_pool_one();
                        ((void (*)(long, long, long, s_damage_flash_owner_argument, const real_vector3d *, float, float, long,
                            bool))player_effect_start)(
                            unit_index, user_index, row->damage_flash_effect.index, owner, &forward, one, one, 1, true);
                        if (row->damage_flash_sound.index != -1)
                            unspatialized_impulse_sound_new(row->damage_flash_sound.index, one);
                    }
                }
            }
        }
    }
}

static __forceinline long get_widget_animation_duration(s_chud_widget_base *base, long state)
{
    if (base->animations.count && base->animations.address)
    {
        s_chud_widget_animation_data *animation = (s_chud_widget_animation_data *)base->animations.address;
        return animation->compute_type_duration_msec(state - 1, false);
    }
    return 0;
}

// @address 0x135fe8 ?compute_actual_widget_state@c_chud_update_user_data@@QAAJJJJJJJJ@Z
long c_chud_update_user_data::compute_actual_widget_state(
    long tag, long collection_index, long widget_index, long unused, long desired, long previous, long elapsed)
{
    // match: preserve the write summary so callers reload widget state.
    user_index = user_index;
    long result = desired;
    s_chud_widget_collection *collection = 0;
    s_chud_widget_base *base = 0;
    if (tag != -1)
    {
        s_chud_definition *definition = TAG_GET('chdt', s_chud_definition, tag);
        if (collection_index >= 0 && collection_index < definition->widget_collection_count)
        {
            collection = &definition->widget_collections[collection_index];
            if (widget_index < collection->bitmap_widget_count)
                base = &collection->bitmap_widgets[widget_index].base;
            else
                base = &collection->text_widgets[widget_index - collection->bitmap_widget_count].base;
        }
    }
    if (collection && base) {
        long duration = base->compute_state_duration_msec(previous, false);
        switch (desired) {
        case 0:
            switch (previous) {
            case 2:
            case 3:
                if (get_widget_animation_duration(base, 5)) result = 5;
                break;
            case 1:
            case 4:
            case 5:
            case 6:
                if (elapsed < duration) result = previous;
                break;
            }
            break;
        case 2:
            switch (previous) {
            case 0:
                if (unit_changed && get_widget_animation_duration(base, 1)) result = 1;
                else if (get_widget_animation_duration(base, 4)) result = 4;
                break;
            case 1:
            case 4:
            case 5:
            case 6:
                if (elapsed < duration && duration != -1) result = previous;
                break;
            }
            break;
        }
        if ((base->flags & 8) && hud_changed) {
            if (result == 4) result = 2;
            else if (result == 5) result = 0;
        }
    }
    if (result == 2) {
        if ((base && (base->flags & 1)) || (collection && (collection->flags & 1))) result = 0;
    }
    return result;
}

// 0x488100


// @address 0x488100 chud_set_cinematic_shader_constants
extern "C" void chud_set_cinematic_shader_constants(void)
{
    chud_dispose_state *cinematic = g_chud_cinematic_state;
    s_cortana_effect_definition *definition =
        TAG_GET('crte', s_cortana_effect_definition, cinematic->index);
    int enabled;
    union u_shader_constant
    {
        real_vector4d vector;
        real_point3d point;
    };
    real_vector4d origin;
    u_shader_constant constants[16];
    real_point2d position;

    memset(constants, 0, sizeof(constants));
    origin.x = 1.0f;
    origin.y = 1.0f;
    origin.z = 1.0f;
    origin.w = 1.0f;
    constants[0].vector = origin;
    constants[2].vector = origin;
    constants[15].vector = origin;
    enabled = 0;

    if (definition->postprocessing.s_tag_block::count != 0)
    {
        s_cortana_effect_postprocessing *reference = (s_cortana_effect_postprocessing *)definition->postprocessing.address;
        if (reference != 0)
        {
            if (reference->hue.count != 0)
                chud_compute_follower_value_3d((s_chud_curve_follower *)reference->hue.address, (g_chud_cinematic_state)->ticks,
                    &constants[1].point, 0, 0);
            if (reference->saturation.count != 0)
                chud_compute_follower_value_3d((s_chud_curve_follower *)reference->saturation.address, (g_chud_cinematic_state)->ticks,
                    &constants[0].point, 0, 0);
        }
    }
    if (definition->gravemind.count != 0)
    {
        s_chud_cinematic_curve_set *reference = (s_chud_cinematic_curve_set *)definition->gravemind.address;
        if (reference != 0)
        {
            if (reference->channels[0].count != 0)
                chud_compute_follower_value_3d((s_chud_curve_follower *)reference->channels[0].address, (g_chud_cinematic_state)->ticks,
                    &constants[2].point, 0, 0);
            if (reference->channels[1].count != 0)
                chud_compute_follower_value_3d((s_chud_curve_follower *)reference->channels[1].address, (g_chud_cinematic_state)->ticks,
                    &constants[14].point, &constants[12].point, &constants[13].point);
            if (reference->channels[3].count != 0)
                chud_compute_follower_value_3d((s_chud_curve_follower *)reference->channels[3].address, (g_chud_cinematic_state)->ticks,
                    &constants[15].point, 0, 0);
        }
    }
    if (definition->cortana.count != 0)
    {
        s_chud_cinematic_curve_set *reference = (s_chud_cinematic_curve_set *)definition->cortana.address;
        if (reference != 0)
        {
            if (reference->channels[0].count != 0)
                chud_compute_follower_value_2d((s_chud_curve_follower_2d *)reference->channels[0].address, (g_chud_cinematic_state)->ticks,
                    &constants[5].point.xy, &constants[3].point, &constants[4].point);
            if (reference->channels[1].count != 0)
                chud_compute_follower_value_3d((s_chud_curve_follower *)reference->channels[1].address, (g_chud_cinematic_state)->ticks,
                    &constants[8].point, &constants[6].point, &constants[7].point);
            if (reference->channels[2].count != 0)
            {
                chud_compute_follower_value_2d((s_chud_curve_follower_2d *)reference->channels[2].address, (g_chud_cinematic_state)->ticks,
                    &position, &constants[9].point, &constants[10].point);
                chud_compute_cycle_color(position.x, position.y, constants[11].vector.n);
                enabled = 1;
            }
        }
    }
    D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, 8, &enabled, 1);
    D3DDevice_SetPixelShaderConstantFN(g_rasterizer_device, 57, constants[0].vector.n, 16, ((unsigned __int64)31) << 45);
}

// @address 0x488618 chud_set_cinematic_textures
extern "C" void chud_set_cinematic_textures(void)
{
    c_rasterizer::set_aliased_surface_as_texture(0, (c_rasterizer::e_surface)67);
    c_rasterizer::set_aliased_surface_as_texture(1, (c_rasterizer::e_surface)17);
    c_rasterizer::set_sampler_address_mode(
        1, (c_rasterizer::e_sampler_address_mode)(3), (c_rasterizer::e_sampler_address_mode)(3), (c_rasterizer::e_sampler_address_mode)(3));
    long tag_index = chud_globals->cinematic_strip_bitmap.index;
    if (tag_index != -1) {
        c_rasterizer_texture_ref texture_ref;
        void *entry;
        texture_ref.clear();
        ::bitmap_group_get_xenon_hardware_format(tag_index, 0, &texture_ref, (bitmap_data const **)((void **)&entry), (__int64 *)(0));
        c_rasterizer::set_sampler_address_mode(2, (c_rasterizer::e_sampler_address_mode)(0), (c_rasterizer::e_sampler_address_mode)(0),
            (c_rasterizer::e_sampler_address_mode)(0));
        long texture_value = texture_ref.get_datum_ref();
        // match: preserve the accepted compiler barrier before binding the sampler reference.
        void __cdecl _ReadWriteBarrier(void);
        _ReadWriteBarrier();
        texture_ref = c_rasterizer_texture_ref(texture_value);
        rasterizer_set_sampler_texture_reference(2, texture_ref);
        if (g_rasterizer_sampler_filter_modes[2] != 2) {
            D3DDevice *device = g_rasterizer_device;
            g_rasterizer_sampler_filter_modes[2] = 2;
            rasterizer_device_set_sampler_filter_modes(device, 2, 2, 2);
        }
    }
}

// @address 0x8977a0 g_chud_draw_globals

static __forceinline void chud_screen_set_point3d(real_point3d *point, float x, float y, float z)
{
    point->x = x;
    point->y = y;
    point->z = z;
}
static __forceinline void chud_screen_set_rectangle2d(real_rectangle2d *bounds, float x0, float x1, float y0, float y1)
{
    bounds->x0 = x0;
    bounds->x1 = x1;
    bounds->y0 = y0;
    bounds->y1 = y1;
}
static __forceinline void chud_screen_set_vector2d(real_vector2d *vector, float i, float j)
{
    vector->i = i;
    vector->j = j;
}
// @rdata 0x8203385c
static const float chud_screen_wave_amplitude = 0.2f;
static __forceinline void chud_screen_texture_bounds(real_rectangle2d *bounds, const real_vector2d *wave, const real_vector2d *scale)
{
    bounds->x0 = -(wave->i * chud_screen_wave_amplitude);
    bounds->x1 = wave->i * chud_screen_wave_amplitude + scale->i;
    bounds->y0 = -(wave->j * chud_screen_wave_amplitude);
    bounds->y1 = wave->j * chud_screen_wave_amplitude + scale->j;
}

// @address 0x4886f0 chud_render_cinematic_effect
extern "C" void chud_render_cinematic_effect(void)
{
    real_point3d position = *global_origin3d;
    real_vector3d up = *global_up3d;
    real_vector3d left = *global_left3d;
    s_cortana_effect_definition *definition = (s_cortana_effect_definition *)tag_get('crte', g_chud_cinematic_state->index);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)67);
    if (g_rasterizer_alpha_blend_mode != 0)
    {
        g_rasterizer_alpha_blend_mode = 0;
        D3DDevice_SetRenderState_AlphaBlendEnable(g_rasterizer_device, 0);
    }
    real_point3d adjustments = *global_origin3d;
    union
    {
        real_rgb_color filter;
        real_point3d saturation;
    };
    filter.red = 1.0f;
    filter.green = 1.0f;
    filter.blue = 1.0f;
    real_rgb_color offset = { 0.0f, 0.0f, 0.0f };
    if (definition->postprocessing.s_tag_block::count)
    {
        s_cortana_effect_postprocessing *reference = (s_cortana_effect_postprocessing *)definition->postprocessing.address;
        if (reference)
        {
            if (reference->hue.count)
                chud_compute_follower_value_3d((s_chud_curve_follower *)reference->hue.address, (g_chud_cinematic_state)->ticks,
                    &adjustments, 0, 0);
            if (reference->saturation.count)
                chud_compute_follower_value_3d(
                    (s_chud_curve_follower *)reference->saturation.address, (g_chud_cinematic_state)->ticks, &saturation, 0, 0);
        }
    }
    rasterizer_set_hue_saturation_constants(g_screenshot_hue_saturation, adjustments.x, adjustments.y, adjustments.z, &filter,
        &offset);
    float screen_width = g_chud_draw_globals.width;
    float screen_height = g_chud_draw_globals.height;
    chud_screen_set_point3d(&position, screen_width * 0.5f, screen_height * 0.5f, 0.0f);
    real_rectangle2d bounds;
    float horizontal_radius = screen_width * 0.8f;
    float vertical_radius = screen_height * 0.8f;
    real_vector2d texture_scale;
    chud_screen_set_vector2d(&texture_scale, (float)(screen_width / 128.0), (float)(screen_height / 128.0));
    float phase = (float)(g_chud_cinematic_state)->ticks * g_chud_cinematic_strip_tuning.wave_speed
        / (float)game_time_globals->ticks_per_second;
    chud_screen_set_rectangle2d(&bounds, -horizontal_radius, horizontal_radius, -vertical_radius, vertical_radius);
    real_vector2d wave;
    wave.i = periodic_function_evaluate(10, phase + 97.38937f) - 0.5f;
    wave.j = periodic_function_evaluate(10, phase + 84.823006f) - 0.5f;
    real_rectangle2d texture_bounds;
    chud_screen_texture_bounds(&texture_bounds, &wave, &texture_scale);
    chud_draw_cortana_effect_background(&bounds, &position, &up, &left, &texture_bounds, chud_set_cinematic_textures, 22);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)1);
}

// @address 0x12f920 ?compute_weapon_update@c_chud_update_user_data@@IAAXJJPBUs_aim_assist_targeting_result@@@Z
void c_chud_update_user_data::compute_weapon_update(long weapon_index, long slot, const s_aim_assist_targeting_result *targeting)
{
    unit_datum *weapon = (unit_datum *)object_get_and_verify_type(weapon_index, 8195);
    weapon_state *update = &definitions[slot].weapon;
    weapon_interface_state interface_state;
    weapon_build_weapon_interface_state(weapon_index, &interface_state);
    definitions[slot].definition_index =
        TAG_GET('weap', weapon_definition, weapon->object.definition_index)
            ->chud_interface.index;
    definitions[slot].index = weapon_index;
    update->heat = interface_state.heat;
    update->age = interface_state.age == 3.402823466e+38f ? 3.402823466e+38f : interface_state.age * 100.0f;
    update->analog_value = interface_state.analog_value;
    update->overheated = (float)interface_state.overheated;
    update->charge = interface_state.charge;
    update->rounds_loaded = (float)interface_state.magazines[0].rounds_loaded;
    update->rounds_inventory = (float)interface_state.magazines[0].rounds_loaded_maximum;
    update->rounds_total = interface_state.magazines[0].rounds_inventory == -1 ? 3.402823466e+38f : (float)interface_state.magazines[0].rounds_inventory;
    update->magazine = (float)interface_state.magazines[0].rounds_inventory_maximum;
    update->magazine_active = interface_state.magazines[0].active;
    update->control_flag_1 = interface_state.control_flag_1;
    update->tracking = interface_state.tracking;
    update->locking = interface_state.locking;
    update->locked = interface_state.locked;
    update->lock_fraction = interface_state.lock_fraction;
    update->target_position = interface_state.target_position;
    update->active_flags_0 = 0;
    update->active_flags_2 = 0;
    update->active_flags_4 = 0;
    switch (slot) {
    case 0:
        update->active_flags_2 |= (1 << 0);
        break;
    case 1:
        update->active_flags_2 |= (1 << 1);
        break;
    case 2:
        update->active_flags_2 |= (1 << 2);
        break;
    case 3:
        update->active_flags_2 |= (1 << 0);
        break;
    case 4:
        update->active_flags_2 |= (1 << 0);
        break;
    }
    if ((autoaim_flags & (1 << 0)) != 0)
        update->active_flags_4 |= (1 << 1);
    else if (interface_state.charged_plasma_pistol && weapons_aim_assist_targeting_is_valid_for_plasma_pistol_lock(targeting) && targeting->lock_amount >= 1.0f)
        update->active_flags_4 |= (1 << 7);
    else if ((autoaim_flags & (1 << 1)) != 0)
        update->active_flags_4 |= (1 << 5);
    else if ((autoaim_flags & (1 << 2)) != 0)
        update->active_flags_4 |= (1 << 4);
    else if (update->tracking)
        update->active_flags_4 |= (1 << 6);
    else if ((autoaim_flags & (1 << 4)) != 0)
        update->active_flags_4 |= (1 << 3);
    else if (zoom_level < 1.0f)
        update->active_flags_4 |= (1 << 0);
    else
        update->active_flags_4 |= (1 << 2);
    if ((update->active_flags_4 & (1 << 6)) == 0)
        update->active_flags_4 |= (1 << 8);
    else
        update->active_flags_4 &= ~(1 << 8);
    if (update->locking)
        update->active_flags_6 |= (1 << 0);
    else if (update->locked)
        update->active_flags_6 |= (1 << 1);
    if ((update->active_flags_6 & (1 << 1)) == 0)
        update->active_flags_6 |= (1 << 2);
    else
        update->active_flags_6 &= ~(1 << 2);
    if (definitions[slot].definition_index != -1) {
        s_chud_definition *definition = TAG_GET('chdt', s_chud_definition, definitions[slot].definition_index);
        if (update->rounds_loaded <= (float)definition->rounds_loaded_warning && !update->magazine_active)
            update->flashingFlags |= (1 << 0);
        else if (update->rounds_loaded == 0.0f && !update->magazine_active)
            update->flashingFlags |= (1 << 1);
        if (update->rounds_total <= (float)definition->rounds_total_warning)
            update->flashingFlags |= (1 << 2);
        else if (update->rounds_total == 0.0f)
            update->flashingFlags |= (1 << 3);
        if (update->age <= (float)definition->age_warning)
            update->flashingFlags |= (1 << 4);
        else if (update->age == 0.0f)
            update->flashingFlags |= (1 << 5);
        if (update->overheated != 0.0f)
            update->flashingFlags |= (1 << 6);
    }
}


// @address 0x472b88 chud_can_show_action_prompt
extern "C" long chud_can_show_action_prompt(unsigned long user_index)
{
    long result = 0;
    if (user_index <= 3 && user_index != -1) {
        long player_index = player_mapping_globals->player_indices[user_index];
        if (player_index != -1) {
            if ((unsigned short)player_control_globals->states[user_index].action.zoom == 0xffff
                || ((s_controllers *)g_director_globals)->slots[user_index].perspective != 0) {
                if (chud_scripting_is_element_enabled(&g_chud_scripting_globals->scripting, 6, user_index))
                    result = 1;
            }
            bool alive = true;
            if (game_engine_running() && player_get(player_index)->traits.get_sensor_traits()->m_motion_tracker == 1)
                alive = false;
            if (!alive)
                result = 0;
            if (player_get(player_index)->unit_index == -1)
                result = 0;
        }
    }
    bool skull_active = chud_game_skull_is_active(4);
    long final_result = 0;
    if (!skull_active)
        final_result = result;
    return final_result;
}


// @address 0x18e438 ?render_debug@c_chud_cortana_effect@@QAAXJ@Z
void c_chud_cortana_effect::render_debug(long user_index)
{
    if (m_scale > 0.0f && chud_cortana_debug)
    {
        real_point3d position = chud_user_observer_camera(user_index)->position;
        real_vector3d forward = chud_user_observer_camera(user_index)->forward;
        real_vector3d direction;
        vector_from_points3d(&position, &m_position, &direction);
        normalize3d(&direction);
        float distance = sqrtf(distance_squared3d(&position, &m_position));
        float facing = 0.0f > dot_product3d(&direction, &forward) ? 0.0f : dot_product3d(&direction, &forward);
        rectangle2d bounds;
        chud_set_rectangle2d(&bounds, 100, 150, 1000, 1000);
        char text[2048];
        text[0] = '\0';
        csnzappendf(text, sizeof(text), "current power= %f|n", m_scale);
        csnzappendf(text, sizeof(text), "distance= %.3f, facing= %.3f|n", distance, facing);
        csnzappendf(text, sizeof(text), "effect blend= %.3f|n", m_effect_blend);
        csnzappendf(text, sizeof(text),
            "noise a velocity      = %.3f : %.3f|nnoise a scale x       = %.3f : %.3f|nnoise a scale y       = %.3f : %.3f|n"
            "noise b velocity      = %.3f : %.3f|nnoise b scale x       = %.3f : %.3f|nnoise b scale y       = %.3f : %.3f|n"
            "noise pix. threshold  = %.3f : %.3f|nnoise pix. persistence= %.3f : %.3f|nnoise pix. velocity   = %.3f : %.3f|n"
            "noise pix. turbulence = %.3f : %.3f|nnoise trans. scale x  = %.3f : %.3f|nnoise trans. scale y  = %.3f : %.3f|n",
            m_noise_bounds.lower, m_noise_bounds.upper,
            m_vertex_bounds[0].lower, m_vertex_bounds[0].upper,
            m_vertex_bounds[1].lower, m_vertex_bounds[1].upper,
            m_vertex_bounds[2].lower, m_vertex_bounds[2].upper,
            m_vertex_bounds[3].lower, m_vertex_bounds[3].upper,
            m_vertex_bounds[4].lower, m_vertex_bounds[4].upper,
            m_pixel_bounds[0].lower, m_pixel_bounds[0].upper,
            m_pixel_bounds[1].lower, m_pixel_bounds[1].upper,
            m_pixel_bounds[2].lower, m_pixel_bounds[2].upper,
            m_pixel_bounds[3].lower, m_pixel_bounds[3].upper,
            m_pixel_bounds[4].lower, m_pixel_bounds[4].upper,
            m_pixel_bounds[5].lower, m_pixel_bounds[5].upper);
        c_rasterizer_draw_string draw_string;
        c_font_cache font_cache;
        draw_string.set_bounds(&bounds);
        draw_string.c_draw_string::draw(&font_cache, text);
    }
}

static __forceinline float chud_scaled_fade(long user_index, float base_scale)
{
    return chud_get_scoreboard_fade(user_index) * chud_get_cinematic_fade() * base_scale;
}
// @address 0x8c48db g_chud_text_render_target_dirty
bool g_chud_text_render_target_dirty;
// @address 0x166220 chud_draw_text_widgets
extern "C" void chud_draw_text_widgets(long user_index)
{
    g_chud_text_render_target_dirty = 0;
    if (!chud_user_should_draw(user_index) || chud_globals == 0 || (unsigned long)user_index > 3)
        return;

    float scale = c_chud_manager::x_persistent_user_data[user_index].m_cinematic_fade.get_fade();
    if (!chud_initialize_draw_state(user_index, chud_scaled_fade(user_index, scale), 1, 0))
        return;

    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)58);
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)0);

    real_vector4d color;
    color.i = 0.5f;
    color.j = 0.5f;
    color.k = 0.0f;
    color.l = 0.0f;

    float z_scale = 1.0f;
    union { float as_float; unsigned long as_dword; } slope_scale_bias = { g_rasterizer_depth_bias };
    union { float as_float; unsigned long as_dword; } depth_bias = { g_rasterizer_depth_bias };
    if (g_rasterizer_use_floating_point_z_buffer)
        z_scale = -1.0f;
    if (g_rasterizer_current_z_buffer_mode != 2 || g_rasterizer_use_floating_point_z_buffer != g_rasterizer_current_z_buffer_floating_point)
    {
        g_rasterizer_current_z_buffer_mode = 2;
        g_rasterizer_current_z_buffer_floating_point = g_rasterizer_use_floating_point_z_buffer;
        D3DDevice_SetRenderState_ZEnable(g_rasterizer_device, 0);
        slope_scale_bias.as_float *= z_scale;
        depth_bias.as_float *= z_scale;
        D3DDevice_SetRenderState_SlopeScaleDepthBias(g_rasterizer_device, slope_scale_bias.as_dword);
        D3DDevice_SetRenderState_DepthBias(g_rasterizer_device, depth_bias.as_dword);
    }

    D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 0);
    D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
    if (g_rasterizer_alpha_blend_mode != 1)
    {
        D3DDevice *device = g_rasterizer_device;
        g_rasterizer_alpha_blend_mode = 1;
        rasterizer_set_device_alpha_blend_mode(device, 1);
    }
    c_rasterizer::clearf(1, &color, 0.0f, 0);

    long index = data_next_index((s_data_array *)g_chud_widget_data[user_index], -1);
    if (index != -1)
    {
        do
        {
            s_chud_runtime_widget_datum *widget =
                &((s_chud_runtime_widget_datum *)((s_data_array *)g_chud_widget_data[user_index])->data)[(unsigned short)index];
            long tag_index =
                c_chud_manager::x_persistent_user_data[user_index].m_weapons[widget->widget_index].definition_index;
            chud_draw_widget(user_index, widget, tag_index, 1);
            index = data_next_index((s_data_array *)g_chud_widget_data[user_index], index);
        } while (index != -1);
    }

    if (g_chud_text_render_target_dirty)
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)58, 0, 0, 0, 0);
}

// @address 0x18eab8 chud_draw_curved_hud
extern "C" void chud_draw_curved_hud(int user_index)
{

    if (chud_can_draw_for_user(user_index) != 0
        && chud_globals != 0
        && (unsigned int)user_index <= 3
        && chud_initialize_draw_state(user_index, 1.0f, 0, 1) != 0)
    {
        c_chud_persistent_user_data *persistent = c_chud_manager::x_persistent_user_data + user_index;
        long index = persistent->m_display_state.widget_list_heads[1];
        if (index != -1)
        {
            do
            {
                s_chud_runtime_widget_datum *widget =
                    &((s_chud_runtime_widget_datum *)((s_data_array *)g_chud_widget_data[user_index])->data)[(unsigned short)index];
                long tag_index =
                    c_chud_manager::x_persistent_user_data[user_index].m_weapons[widget->widget_index].definition_index;
                chud_draw_widget(user_index, widget, tag_index, 0);
                index = widget->next_index;
            } while (index != -1);
        }
    }
}

/* ---- 0x12db38: build the per-user chud update data ------------------------------- */
static __forceinline long unit_get_current_weapon_verified(long unit_index)
{
    unit_datum *unit = OBJECT_GET(unit_index, unit_datum, _object_mask_unit);
    long weapon_index = -1;
    if (unit->unit.current_weapon_index != -1)
        weapon_index = unit->unit.weapon_object_indices[unit->unit.current_weapon_index];
    return weapon_index;
}
static __forceinline long chud_unit_get_dual_weapon(long unit_index)
{
    unit_datum *unit = OBJECT_GET(unit_index, unit_datum, _object_mask_unit);
    long weapon_index = -1;
    if (unit->unit.current_weapon_indices[1] != -1)
        weapon_index = unit->unit.weapon_object_indices[unit->unit.current_weapon_indices[1]];
    return weapon_index;
}
static __forceinline short chud_unit_grenade_count(long unit_index, short grenade_index)
{
    short count;
    if (grenade_index != -1)
        count = OBJECT_GET(unit_index, unit_datum, _object_mask_unit)->unit.grenade_counts[grenade_index];
    else
        count = 0;
    return count;
}
static __forceinline long chud_hud_interface_index(const c_typed_tag_block<s_tag_reference> *block)
{
    short index = (short)MIN(block->count() - 1, 0);
    long result;
    if (index < 0)
    {
        result = -1;
    }
    else
    {
        const s_tag_reference *reference = &(*block)[index & 0x0fffffff];
        result = reference->index;
    }
    return result;
}
static __forceinline int chud_game_engine_index_out_of_range(long index)
{
    if (index < 1)
        return 1;
    if (index > 10)
        return 1;
    return 0;
}
static __forceinline void chud_definition_set(definition_info *definition, long tag_index)
{
    definition->index = -1;
    definition->definition_index = tag_index;
}
static __forceinline c_game_engine *chud_game_engine_get_validated()
{
    long index = game_engine_globals->game_engine_index;
    if (!chud_game_engine_index_out_of_range(index) && game_engines[index])
        return game_engines[index];
    return 0;
}
static __forceinline bool chud_game_session_active()
{
    game_globals_storage *game = game_globals;
    return game && (game->initializing || game->map_initialized);
}
static __forceinline bool chud_voice_available()
{
    s_voice_interface_globals *voice = &g_voice_interface_globals;
    bool available = false;
    if (voice->initialized)
    {
        e_voice_lifecycle lifecycle = voice_get_lifecycle();
        if (lifecycle > _voice_lifecycle_none && lifecycle <= _voice_lifecycle_in_game_pending)
            available = true;
    }
    return available;
}
static __forceinline void chud_bit_flag_set(unsigned short &flags, long bit, bool value)
{
    if (value)
        flags |= (unsigned short)FLAG(bit);
    else
        flags &= (unsigned short)~FLAG(bit);
}
static __forceinline void chud_bit_flag_set(long &flags, long bit, bool value)
{
    if (value)
        flags |= FLAG(bit);
    else
        flags &= ~FLAG(bit);
}
static __forceinline long verified_equipment_remaining_charges(long equipment_index)
{
    equipment_datum *equipment = OBJECT_GET(equipment_index, equipment_datum, _object_mask_equipment);
    short charges = ((s_equipment_definition *)tag_get('eqip', equipment->item.object.definition_index))->max_charges;
    long remaining = -1;
    if (charges != -1)
        remaining = charges - equipment->charges_used;
    return remaining;
}
static __forceinline int chud_grenade_type_valid(long grenade_type)
{
    if (grenade_type < 0)
        return 0;
    if (grenade_type >= 4)
        return 0;
    return 1;
}

// @address 0x12db38 chud_update_user_data_build
extern "C" c_chud_update_user_data *chud_update_user_data_build(c_chud_update_user_data *update_data, long user_index, long last_unit_index, bool force)
{
    memset(update_data, 0, sizeof(*update_data));
    update_data->user_index = user_index;
    update_data->player_index = -1;
    update_data->unit_index = -1;
    for (long i = 0; i < 12; i++)
    {
        update_data->definitions[i].definition_index = -1;
        update_data->definitions[i].index = -1;
    }
    chud_table_row *row = &chud_globals->rows[chud_get_user_skin_index(user_index)];
    update_data->player_index = player_index_from_user_index_render(user_index);
    if (update_data->player_index != -1)
    {
    player_datum *player = player_get(update_data->player_index);
    if (force || player->unit_index != last_unit_index)
        update_data->unit_changed = true;
    if (player->unit_index != -1)
    {
        bool weapons_hidden = false;
        const long unit_index = player->unit_index;
        unit_datum *unit = OBJECT_GET(player->unit_index, unit_datum, _object_mask_unit);
        update_data->unit_index = unit_index;
        const s_aim_assist_targeting_result *targeting = &unit->unit.targeting;
        unit_interface_state interface_state;
        unit_build_unit_interface_state(player->unit_index, &interface_state);
        update_data->unit_valid = true;
        update_data->zoom_level = targeting->primary_autoaim_level;
        update_data->secondary_zoom_level = targeting->secondary_autoaim_level;
        update_data->body_vitality = unit->object.body_vitality;
        update_data->shield_vitality = unit->object.shield_vitality;
        update_data->selected_grenade_index = OBJECT_GET(player->unit_index, unit_datum, _object_mask_unit)->unit.current_grenade_index;
        for (long grenade_index = 0; grenade_index < 4; grenade_index++)
            update_data->grenade_counts[grenade_index] = chud_unit_grenade_count(player->unit_index, (short)grenade_index);
        update_data->primary_weapon = interface_state.primary_weapon;
        update_data->alternate_state = interface_state.dual_weapons;
        long zoom_level;
        if (((s_controllers *)g_director_globals)->slots[user_index].perspective == 0)
            zoom_level = OBJECT_GET(player->unit_index, unit_datum, _object_mask_unit)->unit.zoom_level;
        else
            zoom_level = -1;
        update_data->autoaim_target_type = zoom_level;
        update_data->seat_flag = interface_state.seat_flag;
        update_data->frag_grenade_count = OBJECT_GET(player->unit_index, unit_datum, _object_mask_unit)->unit.grenade_counts[0];
        update_data->plasma_grenade_count = OBJECT_GET(player->unit_index, unit_datum, _object_mask_unit)->unit.grenade_counts[1];
        update_data->state_flags = unit->object.damage_flags;
        update_data->zoom_unavailable = false;
        const long &current_unit_index = player->unit_index;
        if (zoom_level != -1 && unit_get_current_weapon_verified(player->unit_index) != -1)
        {
            if (weapon_rotate_zoom_level(unit_get_current_weapon_verified(current_unit_index), -1) == -1)
                update_data->zoom_unavailable = true;
        }
        update_data->can_show_action_prompt = (unsigned char)chud_can_show_action_prompt(user_index);
        update_data->has_shield = game_engine_player_has_shield(update_data->player_index);
        update_data->weapon_ready_sound = (bool)((player->flags >> 14) & 1);
        update_data->weapon_overheat_sound = (bool)((player->flags >> 15) & 1);
        update_data->highlight_index = OBJECT_GET(update_data->unit_index, unit_datum, _object_mask_unit)->unit.equipment_object_index;
        if (update_data->highlight_index != -1 && verified_equipment_remaining_charges(update_data->highlight_index) == 0)
            update_data->highlight_index = -1;
        unit_definition *definition = (unit_definition *)tag_get('unit', unit->object.definition_index);
        update_data->definitions[5].definition_index = chud_hud_interface_index(&definition->hud_interfaces);
        update_data->definitions[5].index = -1;
        chud_scripting_globals_tls *scripting_globals = g_chud_scripting_globals;
        if (!chud_scripting_is_element_enabled(&scripting_globals->scripting, 7, user_index) && update_data->grenade_counts[2] > 0)
            scripting_globals->scripting.items[6] = true;
        if (!chud_scripting_is_element_enabled(&scripting_globals->scripting, 8, user_index) && update_data->grenade_counts[3] > 0)
            scripting_globals->scripting.items[7] = true;
        update_data->study_autoaim(player->unit_index);
        if (update_data->highlight_index != -1)
        {
            unit_datum *equipment = OBJECT_GET(update_data->highlight_index, unit_datum, _object_mask_unit);
            long hud_interface_index = ((s_equipment_definition *)tag_get('eqip', equipment->object.definition_index))->hud_interface_index;
            chud_definition_set(&update_data->definitions[7], hud_interface_index);
        }
        // match: 0x12db38: qualifying the guard read keeps the parent lookup load in r8 at 0x12e018 after the seat check.
        const volatile object_datum &object = unit->object;
        if (object.parent_object_index != -1 && unit->unit.parent_seat_index != -1)
        {
            unit_datum *parent = OBJECT_GET(unit->object.parent_object_index, unit_datum, _object_mask_unit);
            unit_definition *parent_definition = (unit_definition *)tag_get('unit', parent->object.definition_index);
            unit_seat *seat = &parent_definition->seats.begin()[unit->unit.parent_seat_index];
            update_data->definitions[6].definition_index = chud_hud_interface_index(&seat->hud_interfaces);
            update_data->definitions[6].index = -1;
            if (!(bool)((seat->flags >> 5) & 1))
                weapons_hidden = true;
            if ((bool)((seat->flags >> 3) & 1))
            {
                long weapon_index = unit_get_current_weapon_verified(unit->object.parent_object_index);
                if (weapon_index == -1)
                {
                    for (long child_index = parent->object.first_child_object_index; child_index != -1; )
                    {
                        unit_datum *child = OBJECT_GET(child_index, unit_datum, _object_mask_unit);
                        if ((bool)((child->object.object_flags >> 26) & 1)
                            && (FLAG(child->object.object_type) & 0x1003) != 0
                            && (bool)((((unit_definition *)tag_get('obje', child->object.definition_index))->flags >> 26) & 1))
                        {
                            weapon_index = unit_inventory_get_weapon_or_none(child_index,
                                OBJECT_GET(child_index, unit_datum, _object_mask_unit)->unit.current_weapon_index);
                            if (weapon_index != -1)
                                break;
                        }
                        child_index = child->object.next_object_index;
                    }
                }
                if (weapon_index != -1)
                {
                    update_data->compute_weapon_update(weapon_index, 4, targeting);
                    update_data->definitions[6].index = update_data->definitions[4].index;
                }
            }
        }
        if (!weapons_hidden && unit_get_current_weapon_verified(player->unit_index) != -1)
        {
            long weapon_index = unit_get_current_weapon_verified(current_unit_index);
            update_data->compute_weapon_update(weapon_index, weapon_is_support_weapon(weapon_index) ? 3 : 0, targeting);
        }
        if (!weapons_hidden && chud_unit_get_dual_weapon(player->unit_index) != -1)
            update_data->compute_weapon_update(chud_unit_get_dual_weapon(current_unit_index), 1,
                targeting);
        long backpack_weapon_index = chud_compute_unit_backpack_weapon_index(player->unit_index);
        if (!weapons_hidden && backpack_weapon_index != -1)
            update_data->compute_weapon_update(backpack_weapon_index, 2, targeting);
    }

    if (game_globals->options.game_mode == 2)
    {
        game_engine_get_hud_interface_state(user_index, &update_data->hud_state);
        update_data->definitions[8].definition_index = row->interface_references[0].index;
        update_data->definitions[8].index = -1;
        if (!g_chud_network_status_override)
        {
            long *network_status = update_data->network_status;
            network_status[0] = 0;
            network_status[1] = 0;
            network_status[2] = 0;
            network_status[3] = 0;
            simulation_world_build_network_status(network_status);
        }
    }
    else if (game_globals->options.game_mode == 1 && chud_get_metagame_state(user_index, &update_data->metagame_state))
    {
        chud_definition_set(&update_data->definitions[8], row->interface_references[1].index);
    }
    chud_definition_set(&update_data->definitions[9], row->interface_references[3].index);
    chud_saved_film_progress *saved_film_progress = &update_data->saved_film_progress;
    memset(saved_film_progress, 0, sizeof(*saved_film_progress));
    chud_get_saved_film_progress(saved_film_progress);
    chud_update_progress_timer(saved_film_progress);
    chud_definition_set(&update_data->definitions[10], row->interface_references[2].index);
    if (game_globals->options.game_mode == 2)
    {
        c_game_engine *game_engine = chud_game_engine_get_validated();
        update_data->definitions[11].definition_index = game_engine ? game_engine->get_message_chud_reference() : -1;
        update_data->definitions[11].index = -1;
    }

    if (game_globals->options.game_mode == 2)
    {
        long game_engine_index = game_engine_globals->game_engine_index;
        const bool has_teams = !chud_game_engine_index_out_of_range(game_engine_index)
            && game_engines[game_engine_index] && (game_engine_globals->flags & 8) != 0;
        if (has_teams)
            update_data->active_flags[0] |= 8;
        else
            update_data->active_flags[0] |= 4;
        switch (update_data->hud_state.game_type)
        {
        case 1: update_data->active_flags[0] |= 0x10; break;
        case 2: update_data->active_flags[0] |= 0x20; break;
        case 3: update_data->active_flags[0] |= 0x40; break;
        case 4: update_data->active_flags[0] |= 0x80; break;
        case 6: update_data->active_flags[0] |= 0x800; break;
        case 7: update_data->active_flags[0] |= 0x100; break;
        case 8: update_data->active_flags[0] |= 0x200; break;
        case 9: update_data->active_flags[0] |= 0x400; break;
        case _game_engine_infection: update_data->active_flags[0] |= 0x1000; break;
        case 5: update_data->active_flags[0] |= 0x4000; break;
        }
        c_game_engine *game_engine = chud_game_engine_get_validated();
        long team_designator;
        if (game_engine)
            team_designator = game_engine->get_player_team_designator(update_data->player_index);
        else
            team_designator = game_engine_team_index_to_team_designator((e_game_team)player->configuration.host.team_index);
        const c_game_variant *variant = current_game_variant();
        bool symmetric = variant ? variant->get_rules_are_symmetric() : false;
        const void *vip_variant = chud_game_variant_get_engine_variant(variant, 6);
        const void *juggernaut_variant = chud_game_variant_get_engine_variant(variant, 7);
        const void *infection_variant = chud_game_variant_get_engine_variant(variant, 10);
        bool vip = vip_variant && (bool)((update_data->hud_state.ctf_flags >> 0) & 1);
        bool juggernaut = juggernaut_variant && (bool)((update_data->hud_state.ctf_flags >> 0) & 1);
        bool infection = infection_variant && (bool)((update_data->hud_state.ctf_flags >> 0) & 1);
        bool special_role = vip || juggernaut || infection;
        if (!special_role)
        {
            if (!symmetric)
            {
                SET_BIT(update_data->active_flags[2], 0, team_designator != 0);
                SET_BIT(update_data->active_flags[2], 1, team_designator == 0);
            }
            else
            {
                update_data->active_flags[2] |= 4;
            }
        }
        else
        {
            update_data->active_flags[2] |= 8;
            if (vip && !symmetric)
                update_data->active_flags[2] |= 0x10;
        }
    }
    else
    {
        bool multiple_players = false;
        if (game_globals->options.game_mode == 1)
            multiple_players = players_get_active_and_in_game_count(true) > 1;
        if (multiple_players)
            update_data->active_flags[0] |= 2;
        else
            update_data->active_flags[0] |= 1;
    }

    long voice_state;
    long voice_talking;
    voice_get_player_voice_state((unsigned short)update_data->player_index, &voice_state, &voice_talking);
    SET_BIT(update_data->active_flags[2], 5, voice_state == 0);
    SET_BIT(update_data->active_flags[2], 6, voice_state == 1 || voice_state == 2);
    SET_BIT(update_data->active_flags[2], 7, voice_state == 3 && voice_talking == 0);
    SET_BIT(update_data->active_flags[2], 8, voice_state == 3 && voice_talking == 1);
    SET_BIT(update_data->active_flags[2], 9, voice_state == 4 && voice_talking == 0);
    SET_BIT(update_data->active_flags[2], 10, voice_state == 4 && voice_talking == 2);
    switch ((unsigned long)chud_get_user_skin_key(user_index))
    {
    case 0: update_data->active_flags[1] |= 1; break;
    case 1: update_data->active_flags[1] |= 2; break;
    case 2: update_data->active_flags[1] |= 4; break;
    }
    chud_bit_flag_set(update_data->active_flags[0], 15, chud_game_session_active() && game_globals->options.game_playback != 0);
    update_data->active_flags[3] |= FLAG(chud_get_layout_index(user_index));
    if (player->unit_index != -1 && game_engine_is_player_in_edit_mode(update_data->player_index, 0, 0))
        update_data->active_flags[6] |= FLAG(update_data->hud_state.editing_state) | 0x10;

    SET_BIT(update_data->active_flags[4], 0, update_data->hud_state.stats_valid);
    SET_BIT(update_data->active_flags[4], 1, update_data->hud_state.rival_valid);
    SET_BIT(update_data->active_flags[4], 2, wcslen(update_data->hud_state.variant_name) != 0);
    SET_BIT(update_data->active_flags[4], 3, update_data->hud_state.talker_valid);
    SET_BIT(update_data->active_flags[4], 4, update_data->hud_state.flag_return_visible);
    SET_BIT(update_data->active_flags[4], 5, update_data->hud_state.sudden_death_active || update_data->hud_state.sudden_death);
    SET_BIT(update_data->active_flags[4], 6, update_data->hud_state.team_in_hill);
    SET_BIT(update_data->active_flags[4], 7, update_data->hud_state.enemy_in_hill);
    bool show_time_remaining = update_data->hud_state.show_time_remaining;
    if (!show_time_remaining)
    {
        unsigned long hud_flags = update_data->hud_state.ctf_flags;
        SET_BIT(update_data->active_flags[4], 8, (bool)((hud_flags >> 0) & 1));
        SET_BIT(update_data->active_flags[4], 9, (bool)((hud_flags >> 1) & 1));
        SET_BIT(update_data->active_flags[4], 10, (bool)((hud_flags >> 2) & 1));
    }
    SET_BIT(update_data->active_flags[4], 11, show_time_remaining);
    SET_BIT(update_data->active_flags[4], 12, g_simulation_authority_enabled || g_chud_network_status_override);

    c_chud_persistent_user_data *persistent = c_chud_manager::x_persistent_user_data;
    bool training_active = persistent[user_index].m_flags[0];
    if (g_chud_cinematic_state->index != -1)
        training_active = false;
    SET_BIT(update_data->active_flags[5], 0, training_active);
    if (update_data->autoaim_target_type != -1 && update_data->autoaim_target_index != -1)
        update_data->active_flags[5] |= 2;
    c_chud_persistent_user_data *user_persistent = &persistent[update_data->user_index];
    SET_BIT(update_data->active_flags[5], 4, user_persistent->m_display_state.display_active);
    SET_BIT(update_data->active_flags[5], 5, user_persistent->m_display_state.flash_ticks > 0);

    game_globals_storage *game = game_globals;
    float motion_tracker_range;
    if (game->options.game_mode == 2)
        motion_tracker_range = player->traits.m_sensor_traits.get_motion_tracker_range();
    else
        motion_tracker_range = 25.0f;
    update_data->energy_fraction = motion_tracker_range;
    SET_BIT(update_data->active_flags[7], 0, fabsf(motion_tracker_range - 10.0f) < 0.0001f);
    SET_BIT(update_data->active_flags[7], 1, fabsf(motion_tracker_range - 25.0f) < 0.0001f);
    SET_BIT(update_data->active_flags[7], 2, fabsf(motion_tracker_range - 75.0f) < 0.0001f);
    SET_BIT(update_data->active_flags[7], 3, fabsf(motion_tracker_range - 150.0f) < 0.0001f);
    if (game->options.game_mode == 1)
    {
        long player_count = update_data->metagame_state.player_count;
        if (player_count > 0)
        {
            SET_BIT(update_data->active_flags[7], 5, update_data->metagame_state.local[0]);
            SET_BIT(update_data->active_flags[7], 6, player_count > 1);
            SET_BIT(update_data->active_flags[7], 7, update_data->metagame_state.local[1]);
            SET_BIT(update_data->active_flags[7], 8, player_count > 2);
            SET_BIT(update_data->active_flags[7], 9, update_data->metagame_state.local[2]);
            SET_BIT(update_data->active_flags[7], 10, player_count > 3);
            SET_BIT(update_data->active_flags[7], 11, update_data->metagame_state.local[3]);
            SET_BIT(update_data->active_flags[7], 12, update_data->metagame_state.survival_time != 0);
            SET_BIT(update_data->active_flags[7], 13, update_data->metagame_state.bonus_round);
            SET_BIT(update_data->active_flags[7], 14, update_data->metagame_state.bonus_round_timer);
        }
    }

    if (update_data->unit_index != -1)
    {
        bool has_maximum_shield = player->traits.m_shield_vitality_traits.get_maximum_shield_vitality() > 0.0f;
        switch (update_data->autoaim_target_type)
        {
        case -1: update_data->active_flags[9] |= 1; break;
        case 0: update_data->active_flags[9] |= 2; break;
        case 1: update_data->active_flags[9] |= 4; break;
        }
        if (update_data->definitions[3].index != -1)
            update_data->active_flags[10] |= 8;
        else if (update_data->definitions[1].index != -1)
            update_data->active_flags[10] |= 4;
        else if (update_data->definitions[0].index != -1)
            update_data->active_flags[10] |= 2;
        else
            update_data->active_flags[10] |= 1;
        if (update_data->can_show_action_prompt)
            update_data->active_flags[11] |= 1;
        else
            update_data->active_flags[11] |= 2;
        long grenade_index = update_data->selected_grenade_index;
        if (chud_grenade_type_valid(grenade_index) && update_data->grenade_counts[grenade_index] > 0)
            update_data->active_flags[11] |= FLAG(grenade_index + 2);
        if (update_data->zoom_unavailable)
            update_data->active_flags[11] |= 0x40;
        else
            update_data->active_flags[11] |= 0x80;
        if (((s_controllers *)g_director_globals)->slots[user_index].perspective == 1)
            update_data->active_flags[11] |= 0x200;
        else
            update_data->active_flags[11] |= 0x100;
        long player_index = update_data->player_index;
        if (player_index != -1 && chud_voice_available() && voice_player_is_talking((unsigned short)player_index))
            update_data->active_flags[11] |= 0x400;
        player_index = update_data->player_index;
        if (player_index != -1 && chud_voice_available())
        {
            bool incoming = false;
            if (g_voice_interface_globals.initialized)
                incoming = g_voice_observer.get_player_push_to_talk_mask(player_index & 0xffff) != 0;
            if (incoming)
                update_data->active_flags[11] |= 0x800;
        }
        float shield_vitality = update_data->shield_vitality;
        if (shield_vitality > 3.0f)
            update_data->active_flags[11] |= 0x4000;
        if (shield_vitality > 2.0f)
            update_data->active_flags[11] |= 0x2000;
        if (shield_vitality > 1.0f)
            update_data->active_flags[11] |= 0x1000;
        SET_BIT(update_data->active_flags[11], 15, has_maximum_shield);
        if (has_maximum_shield && shield_vitality == 0.0f)
            update_data->flashingFlags |= 2;
        else if (has_maximum_shield && shield_vitality <= 0.25f)
            update_data->flashingFlags |= 1;
        long grenade_type = 0;
        do
        {
            chud_bit_flag_set(update_data->flashingFlags, grenade_type + 2, update_data->grenade_counts[grenade_type] == 0);
        } while (++grenade_type < 4);
    }
    else
    {
        update_data->active_flags[9] |= 1;
        update_data->active_flags[11] |= 0x200;
    }
    bool zoom_visible;
    if (update_data->unit_index != -1)
        zoom_visible = update_data->autoaim_target_type != -1;
    else
        zoom_visible = false;
    SET_BIT(update_data->flags, 0, zoom_visible);
    SET_BIT(update_data->flags, 1, !update_data->has_no_secondary_weapons());
    if (update_data->definitions[2].index == -1)
        update_data->flags |= 4;
    }
    return update_data;
}

// 0x4722f8
// @address 0x4722f8 chud_draw_damage_indicators
extern "C" bool chud_draw_damage_indicators(long user_index)
{
    bool result = false;
    if (chud_user_should_draw(user_index) && chud_globals != 0 && (unsigned int)user_index <= 3)
    {
        float base_scale = c_chud_manager::x_persistent_user_data[user_index].m_cinematic_fade.get_fade();
        if (chud_initialize_draw_state((int)user_index, chud_scaled_fade(user_index, base_scale), 0, 0) != 0)
        {
            c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)0);
            c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)25);
            c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
            if (g_rasterizer_alpha_blend_mode != 0)
            {
                g_rasterizer_alpha_blend_mode = 0;
                D3DDevice_SetRenderState_AlphaBlendEnable(g_rasterizer_device, 0);
            }
            result =
                chud_draw_directional_damage_overlay(c_chud_manager::x_persistent_user_data[user_index]
                .m_directional_damage.entries, user_index);
            if (result)
                c_rasterizer::resolve_surface((c_rasterizer::e_surface)25, 0, 0, 0, 0);
            c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)3);
        }
    }
    return result;
}

// @address 0x186930 ?chud_draw_screen@@YAXJ@Z
void chud_draw_screen(long user_index)
{
    if (chud_user_should_draw(user_index) && chud_globals != 0 && (unsigned int)user_index <= 3)
    {
        float fade_scale = c_chud_manager::x_persistent_user_data[user_index].m_cinematic_fade.get_fade();
        float fade = chud_get_scoreboard_fade(user_index) * chud_get_cinematic_fade() * fade_scale;
        if (chud_initialize_draw_state(user_index, fade, 1, 0) != 0)
        {
            long widget_index = c_chud_manager::x_persistent_user_data[user_index].m_display_state.widget_list_heads[0];
            if (widget_index != -1)
            {
                do
                {
                    s_chud_runtime_widget_datum *widget = &(
                        (s_chud_runtime_widget_datum *)((s_data_array *)g_chud_widget_data[user_index])->data)[(unsigned short)widget_index];
                    chud_draw_widget(user_index, widget,
                        c_chud_manager::x_persistent_user_data[user_index]
                        .m_weapons[widget->widget_index].definition_index, 0);
                    widget_index = widget->next_index;
                } while (widget_index != -1);
            }
            if ((unsigned char)chud_can_show_action_prompt((unsigned long)user_index) != 0)
                c_chud_manager::x_persistent_user_data[user_index].m_motion_sensor.draw();
            bool draw_widgets = true;
            if (players_globals->suppressed != 0)
                draw_widgets = false;
            if (draw_widgets)
                chud_draw_navpoints(&c_chud_manager::x_persistent_user_data[user_index].m_navpoint_manager, user_index);
            c_chud_manager::x_persistent_user_data[user_index].m_messaging_manager.draw(user_index);
            c_chud_manager::x_persistent_user_data[user_index].m_display_state.cortana_effect.render_debug(user_index);
        }
    }
    if (g_chud_cinematic_state->index != -1 && chud_globals != 0 && (unsigned int)user_index <= 3
        && chud_initialize_draw_state(user_index, real_constant_pool_one(), 1, 0) != 0 && g_chud_cinematic_state->index != -1)
    {
        chud_set_cinematic_shader_constants();
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)67);
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)67, 0, 0, 0, 0);
        c_chud_cortana_effect::begin_frame();
        chud_draw_cinematic_strips();
        c_rasterizer::resolve_surface((c_rasterizer::e_surface)17, 0, 0, 0, 0);
        chud_render_cinematic_effect();
    }
}

// @address 0x167d18 chud_cortana_effect_render
extern "C" void chud_cortana_effect_render(c_chud_cortana_effect *effect, long user_index)
{
    int enabled;
    int disabled;
    real_point2d screen_position;
    float x, y;
    if (!(effect->m_scale <= 0.0f)) {
        c_curved_hud_test curved_hud;
        c_chud_point_tracker *tracker = &effect->m_tracker;
        tracker->m_valid = true;
        const real_point3d *position = &effect->m_position;
        tracker->m_world_position = *position;
        chud_point_tracker_update_screen_position(tracker, 0.0f);
        screen_position = effect->m_tracker.m_screen_position;
        curved_hud.inverse_transform(&screen_position, &x, &y);
        effect->set_constants(effect->m_effect_blend, x, y);
        c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)17);
        c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)0);
        c_rasterizer::clear(15, 0, 1.0f, 0);
        if (g_rasterizer_alpha_blend_mode != 3) {
            g_rasterizer_alpha_blend_mode = 3;
            D3DDevice *device = g_rasterizer_device;
            D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
            D3DDevice_SetRenderState_BlendOp(device, 0);
            D3DDevice_SetRenderState_SrcBlend(device, 6);
            D3DDevice_SetRenderState_DestBlend(device, 7);
        }
        D3DDevice_SetRenderState_ColorWriteEnable(g_rasterizer_device, 15);
        enabled = 1;
        D3DDevice_SetVertexShaderConstantB(g_rasterizer_device, 7, &enabled, 1);
        D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, 7, &enabled, 1);
    } else {
        if (g_rasterizer_alpha_blend_mode != 3) {
            g_rasterizer_alpha_blend_mode = 3;
            rasterizer_set_device_alpha_blend_mode(g_rasterizer_device, 3);
        }
        disabled = 0;
        D3DDevice_SetVertexShaderConstantB(g_rasterizer_device, 7, &disabled, 1);
        D3DDevice_SetPixelShaderConstantB(g_rasterizer_device, 7, &disabled, 1);
        c_chud_point_tracker *tracker = &effect->m_tracker;
        tracker->m_valid = false;
        tracker->m_world_position = *global_origin3d;
    }
}

// @address 0x11db074 g_chud_projection_normalized_scale
real_point2d g_chud_projection_normalized_scale;
// @address 0x11db07c g_chud_projection_top_center
real_point2d g_chud_projection_top_center;
// @address 0x11db084 g_chud_projection_bottom_right
real_point2d g_chud_projection_bottom_right;
// @address 0x11db08c g_chud_projection_top_left
real_point2d g_chud_projection_top_left;

static __forceinline void chud_draw_set_vertex_texture_select(long sampler_index, const c_rasterizer_texture_ref *a,
    const c_rasterizer_texture_ref *b)
{
    rasterizer_set_sampler_texture_reference(sampler_index + 20, *(sampler_index >= 0 ? b : a));
}
static __forceinline void chud_draw_scale_camera_bounds(
    real_rectangle2d *rectangle, const rectangle2d *bounds, const render_camera *camera, const real_rectangle2d *reference)
{
    set_real_rectangle2d(rectangle,
        reference->x1 * ((real)bounds->x0 / (real)camera->fullscreen_pixel_bounds.x1),
        reference->x1 * ((real)bounds->x1 / (real)camera->fullscreen_pixel_bounds.x1),
        reference->y1 * ((real)bounds->y0 / (real)camera->fullscreen_pixel_bounds.y1),
        reference->y1 * ((real)bounds->y1 / (real)camera->fullscreen_pixel_bounds.y1));
}
static __forceinline void chud_draw_scale_bounds(
    real_rectangle2d *rectangle, const rectangle2d *bounds, const rectangle2d *fullscreen_bounds, const real_rectangle2d *reference)
{
    set_real_rectangle2d(rectangle,
        reference->x1 * ((real)bounds->x0 / (real)fullscreen_bounds->x1),
        reference->x1 * ((real)bounds->x1 / (real)fullscreen_bounds->x1),
        reference->y1 * ((real)bounds->y0 / (real)fullscreen_bounds->y1),
        reference->y1 * ((real)bounds->y1 / (real)fullscreen_bounds->y1));
}
// @address 0x1665f8 chud_initialize_draw_state
extern "C" unsigned char chud_initialize_draw_state(long user_index, float global_hud_alpha, bool draw_cortana_effect, bool curved_hud)
{
    if (!chud_globals)
        return false;
    chud_table_row *skin = &chud_globals->rows[chud_get_user_skin_index(user_index)];
    if (skin->entry_count < 1)
        return false;

    s_chud_persistent_display_state *persistent_user_data =
        &c_chud_manager::x_persistent_user_data[user_index].m_display_state;
    const render_camera *rasterizer_camera = c_player_view::get_current()->get_rasterizer_camera();

    g_chud_draw_globals.user_index = user_index;
    g_chud_draw_globals.skin_index = chud_get_user_skin_index(user_index);
    g_chud_draw_globals.resolution = persistent_user_data->resolution;
    g_chud_draw_globals.curvature_info = chud_get_layout_entry(user_index, g_chud_draw_globals.resolution);
    g_chud_draw_globals.origin = g_chud_draw_globals.curvature_info->origin;
    g_chud_draw_globals.pixel_bounds = rasterizer_camera->window_title_safe_pixel_bounds;

    real inset_x = (real)(g_chud_draw_globals.pixel_bounds.x1 - g_chud_draw_globals.pixel_bounds.x0) *
        g_chud_draw_globals.curvature_info->pixel_bounds_inset.i * 0.5f;
    real inset_y = (real)(g_chud_draw_globals.pixel_bounds.y1 - g_chud_draw_globals.pixel_bounds.y0) *
        g_chud_draw_globals.curvature_info->pixel_bounds_inset.j * 0.5f;
    g_chud_draw_globals.pixel_bounds.x0 += (long)inset_x;
    g_chud_draw_globals.pixel_bounds.y0 += (long)inset_y;
    g_chud_draw_globals.pixel_bounds.x1 -= (long)inset_x;
    g_chud_draw_globals.pixel_bounds.y1 -= (long)inset_y;

    set_real_rectangle2d(&g_chud_draw_globals.display_bounds,
        (real)rasterizer_camera->display_pixel_bounds.x0,
        (real)rasterizer_camera->display_pixel_bounds.x1,
        (real)rasterizer_camera->display_pixel_bounds.y0,
        (real)rasterizer_camera->display_pixel_bounds.y1);
    const rectangle2d *fullscreen_pixel_bounds = &rasterizer_camera->fullscreen_pixel_bounds;
    const rectangle2d *fullscreen_title_safe_pixel_bounds = &rasterizer_camera->fullscreen_title_safe_pixel_bounds;
    set_real_rectangle2d(&g_chud_draw_globals.display_safe_bounds,
        g_chud_draw_globals.display_bounds.x1 *(
            (real)rasterizer_camera->fullscreen_title_safe_pixel_bounds.x0 / (real)rasterizer_camera->fullscreen_pixel_bounds.x1),
        g_chud_draw_globals.display_bounds.x1 *(
            (real)rasterizer_camera->fullscreen_title_safe_pixel_bounds.x1 / (real)rasterizer_camera->fullscreen_pixel_bounds.x1),
        g_chud_draw_globals.display_bounds.y1 *(
            (real)rasterizer_camera->fullscreen_title_safe_pixel_bounds.y0 / (real)rasterizer_camera->fullscreen_pixel_bounds.y1),
        g_chud_draw_globals.display_bounds.y1 * ((real)rasterizer_camera->fullscreen_title_safe_pixel_bounds.y1 / 
            (real)rasterizer_camera->fullscreen_pixel_bounds.y1));
    set_real_rectangle2d(&g_chud_draw_globals.display_window_bounds,
        g_chud_draw_globals.display_bounds.x1 *((real)(rasterizer_camera->window_pixel_bounds.x0 + rasterizer_camera->window_offset.x) /
            (real)rasterizer_camera->fullscreen_pixel_bounds.x1),
        g_chud_draw_globals.display_bounds.x1 *((real)(rasterizer_camera->window_pixel_bounds.x1 + rasterizer_camera->window_offset.x) /
            (real)rasterizer_camera->fullscreen_pixel_bounds.x1),
        g_chud_draw_globals.display_bounds.y1 *((real)(rasterizer_camera->window_pixel_bounds.y0 + rasterizer_camera->window_offset.y) /
            (real)rasterizer_camera->fullscreen_pixel_bounds.y1),
        g_chud_draw_globals.display_bounds.y1 * ((real)
            (rasterizer_camera->window_pixel_bounds.y1 + rasterizer_camera->window_offset.y) / (real)rasterizer_camera->fullscreen_pixel_bounds.y1));
    chud_draw_scale_bounds(&g_chud_draw_globals.display_window_safe_bounds, &g_chud_draw_globals.pixel_bounds, fullscreen_pixel_bounds,
        &g_chud_draw_globals.display_bounds);
    set_real_rectangle2d(&g_chud_draw_globals.virtual_bounds,
        (real)rasterizer_camera->fullscreen_pixel_bounds.x0,
        (real)rasterizer_camera->fullscreen_pixel_bounds.x1,
        (real)rasterizer_camera->fullscreen_pixel_bounds.y0,
        (real)rasterizer_camera->fullscreen_pixel_bounds.y1);
    chud_draw_scale_camera_bounds(&g_chud_draw_globals.virtual_safe_bounds, fullscreen_title_safe_pixel_bounds, rasterizer_camera,
        &g_chud_draw_globals.virtual_bounds);
    set_real_rectangle2d(&g_chud_draw_globals.virtual_window_bounds,
        g_chud_draw_globals.virtual_bounds.x1 *((real)(rasterizer_camera->window_pixel_bounds.x0 + rasterizer_camera->window_offset.x) /
            (real)rasterizer_camera->fullscreen_pixel_bounds.x1),
        g_chud_draw_globals.virtual_bounds.x1 *((real)(rasterizer_camera->window_pixel_bounds.x1 + rasterizer_camera->window_offset.x) /
            (real)rasterizer_camera->fullscreen_pixel_bounds.x1),
        g_chud_draw_globals.virtual_bounds.y1 *((real)(rasterizer_camera->window_pixel_bounds.y0 + rasterizer_camera->window_offset.y) /
            (real)rasterizer_camera->fullscreen_pixel_bounds.y1),
        g_chud_draw_globals.virtual_bounds.y1 * ((real)
            (rasterizer_camera->window_pixel_bounds.y1 + rasterizer_camera->window_offset.y) / (real)rasterizer_camera->fullscreen_pixel_bounds.y1));
    chud_draw_scale_camera_bounds(&g_chud_draw_globals.virtual_window_safe_bounds, &g_chud_draw_globals.pixel_bounds, rasterizer_camera,
        &g_chud_draw_globals.virtual_bounds);

    g_chud_draw_globals.width = (real)g_chud_draw_globals.curvature_info->virtual_width;
    g_chud_draw_globals.height = (real)g_chud_draw_globals.curvature_info->virtual_height;
    g_chud_draw_globals.global_hud_alpha = global_hud_alpha;

    if (g_chud_draw_globals.curvature_info->safe_bounds_scale.i != 0.0f && g_chud_draw_globals.curvature_info->safe_bounds_scale.j != 0.0f)
    {
        real margin_x = (1.0f - g_chud_draw_globals.curvature_info->safe_bounds_scale.i) * 0.5f;
        real margin_y = (1.0f - g_chud_draw_globals.curvature_info->safe_bounds_scale.j) * 0.5f;

        real display_width = g_chud_draw_globals.display_bounds.x1 - g_chud_draw_globals.display_bounds.x0;
        real display_height = g_chud_draw_globals.display_bounds.y1 - g_chud_draw_globals.display_bounds.y0;
        set_real_rectangle2d(&g_chud_draw_globals.display_safe_bounds,
            display_width * margin_x,
            display_width * (1.0f - margin_x),
            display_height * margin_y,
            display_height * (1.0f - margin_y));
        g_chud_draw_globals.display_window_safe_bounds.x0 = g_chud_draw_globals.display_window_bounds.x0 >
                g_chud_draw_globals.display_safe_bounds.x0
            ? g_chud_draw_globals.display_window_bounds.x0
            : g_chud_draw_globals.display_safe_bounds.x0;
        g_chud_draw_globals.display_window_safe_bounds.y0 = g_chud_draw_globals.display_window_bounds.y0 >
                g_chud_draw_globals.display_safe_bounds.y0
            ? g_chud_draw_globals.display_window_bounds.y0
            : g_chud_draw_globals.display_safe_bounds.y0;
        g_chud_draw_globals.display_window_safe_bounds.x1 = g_chud_draw_globals.display_window_bounds.x1 >
                g_chud_draw_globals.display_safe_bounds.x1
            ? g_chud_draw_globals.display_safe_bounds.x1
            : g_chud_draw_globals.display_window_bounds.x1;
        g_chud_draw_globals.display_window_safe_bounds.y1 = g_chud_draw_globals.display_window_bounds.y1 >
                g_chud_draw_globals.display_safe_bounds.y1
            ? g_chud_draw_globals.display_safe_bounds.y1
            : g_chud_draw_globals.display_window_bounds.y1;

        real virtual_width = g_chud_draw_globals.virtual_bounds.x1 - g_chud_draw_globals.virtual_bounds.x0;
        real virtual_height = g_chud_draw_globals.virtual_bounds.y1 - g_chud_draw_globals.virtual_bounds.y0;
        set_real_rectangle2d(&g_chud_draw_globals.virtual_safe_bounds,
            virtual_width * margin_x,
            virtual_width * (1.0f - margin_x),
            virtual_height * margin_y,
            virtual_height * (1.0f - margin_y));
        g_chud_draw_globals.virtual_window_safe_bounds.x0 = g_chud_draw_globals.virtual_window_bounds.x0 >
                g_chud_draw_globals.virtual_safe_bounds.x0
            ? g_chud_draw_globals.virtual_window_bounds.x0
            : g_chud_draw_globals.virtual_safe_bounds.x0;
        g_chud_draw_globals.virtual_window_safe_bounds.y0 = g_chud_draw_globals.virtual_window_bounds.y0 >
                g_chud_draw_globals.virtual_safe_bounds.y0
            ? g_chud_draw_globals.virtual_window_bounds.y0
            : g_chud_draw_globals.virtual_safe_bounds.y0;
        g_chud_draw_globals.virtual_window_safe_bounds.x1 = g_chud_draw_globals.virtual_window_bounds.x1 >
                g_chud_draw_globals.virtual_safe_bounds.x1
            ? g_chud_draw_globals.virtual_safe_bounds.x1
            : g_chud_draw_globals.virtual_window_bounds.x1;
        g_chud_draw_globals.virtual_window_safe_bounds.y1 = g_chud_draw_globals.virtual_window_bounds.y1 >
                g_chud_draw_globals.virtual_safe_bounds.y1
            ? g_chud_draw_globals.virtual_safe_bounds.y1
            : g_chud_draw_globals.virtual_window_bounds.y1;

        real pixel_x0 = g_chud_draw_globals.virtual_safe_bounds.x0 - (real)rasterizer_camera->window_offset.x;
        g_chud_draw_globals.pixel_bounds.x0 =
            (short)((real)g_chud_draw_globals.pixel_bounds.x0 > pixel_x0 ? (real)g_chud_draw_globals.pixel_bounds.x0 : pixel_x0);
        real pixel_y0 = g_chud_draw_globals.virtual_safe_bounds.y0 - (real)rasterizer_camera->window_offset.y;
        g_chud_draw_globals.pixel_bounds.y0 =
            (short)((real)g_chud_draw_globals.pixel_bounds.y0 > pixel_y0 ? (real)g_chud_draw_globals.pixel_bounds.y0 : pixel_y0);
        real pixel_x1 = g_chud_draw_globals.virtual_safe_bounds.x1 - (real)rasterizer_camera->window_offset.x;
        g_chud_draw_globals.pixel_bounds.x1 =
            (short)((real)g_chud_draw_globals.pixel_bounds.x1 > pixel_x1 ? pixel_x1 : (real)g_chud_draw_globals.pixel_bounds.x1);
        real pixel_y1 = g_chud_draw_globals.virtual_safe_bounds.y1 - (real)rasterizer_camera->window_offset.y;
        g_chud_draw_globals.pixel_bounds.y1 =
            (short)((real)g_chud_draw_globals.pixel_bounds.y1 > pixel_y1 ? pixel_y1 : (real)g_chud_draw_globals.pixel_bounds.y1);
    }

    g_chud_draw_globals.curvature_amount = curved_hud ? 0.0f : persistent_user_data->curvature_amount;
    c_curved_hud_test curved_hud_test;
    curved_hud_test.set_constants(user_index);
    if (curved_hud)
    {
        g_chud_draw_globals.curvature_amount = persistent_user_data->curvature_amount;
        c_curved_hud_test projection;
        chud_compute_projection_point(&projection, 0.0f, 1.0f, &g_chud_projection_top_left);
        chud_compute_projection_point(&projection, 1.0f, 0.0f, &g_chud_projection_bottom_right);
        chud_compute_projection_point(&projection, 0.5f, 1.0f, &g_chud_projection_top_center);
        real width = g_chud_projection_bottom_right.x - g_chud_projection_top_left.x;
        real width_divisor = width < 0.0001f ? 0.0001f : width;
        g_chud_projection_normalized_scale.x = (g_chud_projection_top_center.x - g_chud_projection_top_left.x) / width_divisor;
        real height = g_chud_projection_bottom_right.y - g_chud_projection_top_left.y;
        real height_divisor = height < 0.0001f ? 0.0001f : height;
        g_chud_projection_normalized_scale.y = (g_chud_projection_top_center.y - g_chud_projection_top_left.y) / height_divisor;
        g_chud_draw_globals.curvature_pixel_scale.i = g_chud_projection_normalized_scale.x * g_chud_draw_globals.width;
        g_chud_draw_globals.curvature_pixel_scale.j = g_chud_projection_normalized_scale.y * g_chud_draw_globals.height;
        g_chud_draw_globals.curvature_amount = 0.0f;
    }

    if (skin->curvature_bitmap.index != -1)
    {
        c_rasterizer_texture_ref texture_ref;
        const bitmap_data *bitmap;
        bitmap_group_get_xenon_hardware_format(skin->curvature_bitmap.index, 0, &texture_ref, &bitmap, 0);
        c_rasterizer_texture_ref sampler_texture_ref = texture_ref;
        rasterizer_set_sampler_texture_reference(1, sampler_texture_ref);
        c_rasterizer::set_sampler_address_mode(1, c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap,
            c_rasterizer::_sampler_address_wrap);
        chud_draw_set_sampler_filter_mode(1, 2);
    }
    if (persistent_user_data->point_tracker_enabled)
    {
        c_chud_point_tracker *point_tracker = &persistent_user_data->point_tracker;
        point_tracker->m_valid = true;
        point_tracker->m_world_position = persistent_user_data->point_tracker_position;
        chud_point_tracker_update_screen_position(point_tracker, 1.0f);
    }
    if (skin->curvature_vertex_bitmap.index != -1)
    {
        c_rasterizer_texture_ref texture_ref;
        const bitmap_data *bitmap;
        bitmap_group_get_xenon_hardware_format(skin->curvature_vertex_bitmap.index, 0, &texture_ref, &bitmap, 0);
        c_rasterizer_texture_ref vertex_texture_ref = texture_ref;
        c_rasterizer_texture_ref vertex_sampler_texture_ref = vertex_texture_ref;
        chud_draw_set_vertex_texture_select(2, &vertex_texture_ref, &vertex_sampler_texture_ref);
        chud_draw_set_sampler_filter_mode(22, 1);
        c_rasterizer::set_sampler_address_mode(22, c_rasterizer::_sampler_address_wrap, c_rasterizer::_sampler_address_wrap,
            c_rasterizer::_sampler_address_wrap);
        chud_draw_set_sampler_filter_mode(22, 1);
    }
    if (draw_cortana_effect)
    {
        chud_cortana_effect_render(&persistent_user_data->cortana_effect, user_index);
    }
    else
    {
        if (g_rasterizer_alpha_blend_mode != 3)
        {
            g_rasterizer_alpha_blend_mode = 3;
            D3DDevice *device = g_rasterizer_device;
            D3DDevice_SetRenderState_AlphaBlendEnable(device, 1);
            D3DDevice_SetRenderState_BlendOp(device, 0);
            D3DDevice_SetRenderState_SrcBlend(device, 6);
            D3DDevice_SetRenderState_DestBlend(device, 7);
        }
        BOOL vertex_shader_value = 0;
        BOOL pixel_shader_value = 0;
        g_rasterizer_device->SetVertexShaderConstantB(7, &vertex_shader_value, 1);
        g_rasterizer_device->SetPixelShaderConstantB(7, &pixel_shader_value, 1);
    }
    if (curved_hud)
        c_rasterizer::set_pixel_shader_constant(33, 4, persistent_user_data->curved_hud_constants);
    c_rasterizer::begin_high_quality_blend();
    c_rasterizer::set_z_buffer_mode(c_rasterizer::_z_buffer_mode_off);
    D3DDevice_SetRenderState_CullMode(g_rasterizer_device, 0);
    return true;
}

// @address 0x1321a8
void c_chud_directional_damage::update(long user_index, float dt)
{
    if (user_index != -1)
    {
        long unit_index = player_mapping_globals->unit_indices[user_index];
        if (unit_index != -1)
        {
            real_point3d head_position;
            unit_get_head_position(unit_index, &head_position);
            long flash_index = chud_find_slot_containing_point(g_chud_scripting_globals->equipment_slots, &head_position, true);
            if (flash_index != -1)
                chud_post_damage_flash((s_equipment_slot *)g_chud_scripting_globals, user_index, flash_index, this);
        }
    }
    for (long index = 0; index < 4; index++)
    {
        s_directional_damage *damage = &entries[index];
        long time = game_time_get();
        if (damage->end_time > time)
        {
            const s_observer_camera_result *camera = chud_user_observer_camera(user_index);
            damage->transition = transition_function_evaluate(damage->type,
                1.0f - (float)(time - damage->start_time) / (float)(damage->end_time - damage->start_time));
            real_point3d point;
            point_from_line3d(&camera->position, &damage->direction, -1.0f, &point);
            damage->tracker.update(&point, true);
            damage->rotation = damage->transition;
            float cosine = (float)cos(15.0 * 0.017453292f);
            float facing = dot_product3d(&damage->direction, &camera->forward);
            float scale;
            if (facing < -cosine)
                scale = 0.0f;
            else
                scale = MIN(facing + 1.0f, 1.0f);
            damage->rotation = damage->rotation * scale;
            if (damage->rotation > 0.0f)
            {
                real_vector3d desired;
                negate_vector3d(&damage->direction, &desired);
                real_vector3d result;
                chud_compute_limited_direction(
                    &desired, &camera->forward, chud_player_control_get_facing_angles(user_index), 0.43633232f, &result, &damage->scale);
                real_vector3d side;
                cross_product3d(&camera->forward, &camera->up, &side);
                real_vector2d local;
                local.j = dot_product3d(&result, &side);
                local.i = dot_product3d(&camera->up, &result);
                damage->alpha = -(float)atan2(-local.j, local.i);
            }
        }
    }
}

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_0> t_chud_user_data_allocator_0;
t_chud_user_data_allocator_0 chud_user_data_allocator_0;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_1> t_chud_user_data_allocator_1;
t_chud_user_data_allocator_1 chud_user_data_allocator_1;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_2> t_chud_user_data_allocator_2;
t_chud_user_data_allocator_2 chud_user_data_allocator_2;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_3> t_chud_user_data_allocator_3;
t_chud_user_data_allocator_3 chud_user_data_allocator_3;
t_chud_scripting_globals_allocator chud_scripting_globals_allocator;

// @address 0x7f1956 g_chud_network_status_override
bool g_chud_network_status_override;
// @address 0x7f1957 g_simulation_authority_enabled
bool g_simulation_authority_enabled;
// @address 0x8c48d9 chud_cortana_debug
bool chud_cortana_debug;
// @address 0x8c48dc chud_globals
s_chud_globals_definition *chud_globals;
extern "C" {
c_chud_state_allocator g_chud_state_allocator;
}


// @address 0x11df6f8 g_chud_cinematic_state
__declspec(thread) chud_dispose_state *g_chud_cinematic_state;
// @address 0x11df75c g_chud_scripting_globals
__declspec(thread) chud_scripting_globals_tls *g_chud_scripting_globals;

// @address 0x8c46bc g_rasterizer_use_floating_point_z_buffer
