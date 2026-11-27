// objectslibxbox/weapons.obj: from the records 0x98b50 0x99200 0x9a030 0x9a130 0x9a228 0xa05d8 0xc0500 0xc0898 0xc88e0 0xd8248 0xdf848 0xefcb8 0xfe428 0x100748 0x100858 0x1009f0 0x101498 0x110800 0x12bb80 0x12fd70 0x1301a8 0x1302f0 0x130378 0x3497f8 0x349888 0x349958 0x349a08 0x349bd8 0x349e00 0x34a1d8 0x34a308 0x34a6d8 0x34a880 0x34a978 0x34a9d8 0x34aa38 0x34ac10 0x34acb0 0x34ad70 0x34adc8 0x34ae28 0x34ae98 0x34b050 0x34b158 0x34b1b8 0x34b318 0x34b418 0x34b4c8 0x34b628 0x34b6b0 0x34b7d8 0x34b8c0 0x34b940 0x34ba40 0x34bac8 0x34bb00 0x34bc18 0x34bcf0 0x34bf00 0x34c250 0x34c410 0x34c730 0x34c9f8 0x34cbd0 0x34cd38 0x34cdf0 0x34cf10 0x34cfa8 0x34d090 0x34d358 0x34d3b8 0x34d410 0x34e188 0x34e2f8 0x34e3d8 0x34e498 0x34e510 0x34e608 0x34e700 0x34e840 0x34e8b8 0x34e9d8 0x34ea98 0x34eba0 0x350250 0x350348 0x3503e8 0x350678 0x3508a0 0x350958 0x350b88 0x350e30 0x351028 0x351120 0x3511e8 0x351268 0x3512d0 0x3513e8 0x351508 0x351588 0x3516c0 0x43a638
// @flags /TP /O2 /Oc
#include "math/matrix_math_types.h"
#include "objects/object_header.h"

#include "cseries/cseries_wrapped.h"
#include "items/weapon_definition_flags.h"
#include "items/weapon_definitions.h"
#include "math/power.h"
#include "tag_files/tag_types.h"
#include "objects/object_types.h"
#include "animations/animation_id.h"
#include "objects/object_iterator.h"
#include "objects/object_datum.h"
#include "ai/weapon_datum_types.h"
#include "animations/animation_search_path_iterator.h"
#include "objects/object_placement.h"
#include "units/unit_control.h"
#include "units/unit_definitions.h"
#include "units/unit_types.h"
#include "units/action_request.h"
#include "math/real_math.h"
#include "math/point_math.h"
#include "animations/animation_manager_storage.h"
#include "memory/tls_globals.h"
#include "memory/data_types.h"
#include "units/motor_enums.h"
#include "game/game_time_types.h"
#include "game/game_time_inline.h"
#include "game/game_globals_storage.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "game/game_engine_simulation_externs.h"

#include "motor/action_system.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "sound/game_sound.h"
#include "units/units.h"
#include "ai/actor_combat.h"
#include "effects/effects.h"
#include "game/aim_assist.h"
#include "game/game_engine_assault.h"
#include "game/game_engine_notifications.h"
#include "game/player_control.h"
#include "interface/first_person_weapons.h"
#include "items/items.h"
#include "models/model_types.h"
#include "effects/player_effects.h"
#include "game/player_machine.h"
#include "items/projectiles.h"
#include "items/weapons.h"
#include "math/periodic_functions.h"
#include "math/random_math.h"
#include "motor/motor_system.h"
#include "units/motor_task.h"
#include "math/real_math_types.h"
#include "memory/tls_data_arrays.h"
#include "objects/damage.h"
#include "objects/damage_owner.h"
#include "physics/collisions.h"
#include "sound/sound_manager.h"
#include "game/game_globals.h"
#include "math/matrix_math.h"
#include "animations/mixing_board/channels/animation_channel.h"
#include "animations/animation_manager.h"
#include "game/players.h"
#include "objects/object_early_movers.h"
#include "motor/motor_animation.h"
#include "networking/network_configuration_types.h"
#include "ai/ai_sound_volume_types.h"
#include "ai/sound_types.h"
#include "items/weapon_interface_magazine_state_types.h"
#include "objects/weapon_first_person_interface_definition_types.h"
#include "ai/ai.h"
#include "cache/cache_file_globals.h"
#include "networking/network_configuration.h"
#include "physics/havok_component.h"
#include "physics/havok_component_class.h"
#include "physics/havok_component_datum_access.h"
#include "objects/object_header_access.h"
#include "tag_files/tag_groups.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "items/weapon_definition_access.h"
#include "math/real_constant_pool.h"
#include "ai/ai_math_constants.h"
#include "tag_files/tag_index.h"
#include "effects/effect_creation.h"
#include "camera/director.h"
#include "game/player_in_game_iterator.h"
#include "memory/static_arrays.h"
#include "math/random_math_range.h"
#include "cseries/bit_operations.h"
#include "objects/object_markers_inline.h"
#include "game/game_globals_inline.h"
#include "game/game_time_inline.h"
#include "cseries/bit_test.h"
#include "objects/object_type_access.h"
#include "math/real_math_trigonometry.h"
#include "game/game_variant_inline.h"
#include "tag_files/string_ids.h"

enum e_weapon_state
{
    _weapon_state_put_away = 10
};
#include <new>
class c_tag_index;
struct effect_vector_field;
// @address 0x9b278 ?rotate_vector_about_axis@@YAPATreal_vector3d@@PAT1@PBT1@MM@Z
// @address 0x1a35d0 ?sound_stop_impulse@@YAXJ@Z
// @address 0x1538e0 ?weapon_get_deterministic_first_person_animation_duration_ticks@@YAFJJF@Z
// @address 0x350678 ?weapon_magazine_set_state@@YAXJFF@Z
// @address 0x43a638 weapon_interrupt_reload_and_clear_timer
// @address 0xa05d8 ?weapon_reloading@@YA_NJJPAF@Z
// @address 0x34e3d8 ?weapon_trigger_can_begin_charging@@YA_NJF@Z
// @address 0x9a228 ?weapon_is_tracking@@YA_NJ@Z
// @address 0xdf848 ?playing@c_animation_channel@@QBA_NXZ
// @address 0x100748 ?weapon_is_dual_wielded@@YA_NJ@Z
// @address 0x1301a8 ?weapons_aim_assist_targeting_is_valid_for_plasma_pistol_lock@@YA_NPBUs_aim_assist_targeting_result@@@Z
// @address 0x130378 ?weapon_get_rounds_inventory@@YAJJJ_N@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x31c7e0 ?object_try_and_get_unsafe_and_verify_type@@YAPAXJK@Z
// @address 0xc88e0 ?weapon_estimate_time_to_target@@YAMJFM@Z
// @address 0x100858 ?weapon_barrel_recover@@YAXJF_N@Z
// @address 0x34b8c0 ?weapon_is_age_based@@YA_NJ@Z
// @address 0x34e8b8 ?weapon_barrel_finish_recover@@YAXJF@Z
// @address 0x350348 ?weapon_barrel_start_ejection_port@@YAXJF_N@Z
// @address 0x350e30 ?weapon_update_first_person_emulation@@YA_NJ@Z
// @address 0x8087a8 ?g_network_configuration@@3Us_network_configuration@@A
// @address 0x3513e8 ?weapon_can_be_stowed@@YA_NJJ@Z
// @address 0x351508 ?weapon_stow@@YAXJJ@Z
// @address 0x3512d0 ?weapon_has_infinite_ammo@@YA_NJ@Z
// @address 0x34bf00 ?weapon_barrel_can_fire_this_tick@@YA_NJFPIA_N00@Z
// @address 0x3497f8 ??1c_animation_manager@@QAA@XZ
// @address 0x34ad70 ?weapon_is_zoomable@@YA_NJ@Z
// @address 0x99200 ?weapon_compute_movement_penalty@@YAMJ_N0@Z
// @address 0x12bb80 ?weapon_rotate_zoom_level@@YAFJF@Z
// @address 0x1302f0 ?weapon_send_message_to_unit@@YAXJW4e_unit_messages@@@Z
// @address 0x34b050 weapon_interrupt_reload
// @address 0x34cd38 ?weapon_magazine_finish_reload@@YAXJF_N@Z
// @address 0xe2c20 ?object_wake@@YAXJ@Z
// @address 0x350250 ?projectile_distribute@@YAXPATreal_vector3d@@0FMFF@Z
// @address 0x28ff10 ?set_goal@c_animation_manager@@QAA_NJJJJJK@Z
// @address 0x3259f0 ?unit_handle_weapon_state_change@@YAXJJJ@Z
// @address 0x3503e8 ?weapon_set_state@@YA_NJJ_N@Z
// @address 0x350958 ?weapon_setup_first_person_emulation@@YA_NJJ@Z
// @address 0x28e8c0 ?find_impulse_or_state_animation_id@c_animation_manager@@QBA?AVc_animation_id@@JJJ@Z
// @address 0x11a000 ?weapon_definition_get_first_person_interface_by_character@@YAPBUweapon_first_person_interface_definition@@PBUweapon_definition@@J@Z
// @address 0x3aa930 ?simulation_action_weapon_state_update@@YAXJ@Z
// @address 0xfcf30 ?internal_object_get_markers_by_string_id@@YAFJJPAUobject_marker@@F_N@Z
// @address 0x34cbd0 ?weapon_magazine_update_reload@@YAXJF@Z
// @address 0x110a98 ?object_detach@@YAXJ@Z
// @address 0x34b6b0 ?weapon_get_trigger_position@@YA_NJFPATreal_point3d@@@Z
// @address 0xc0500 ?weapon_set_initial_rounds_from_scenario_weapon@@YAXJPBUs_scenario_weapon@@@Z
// @address 0x110800 ?weapons_fire_barrels@@YAXXZ
// @address 0x349a08 ?weapon_new@@YA_NJPAUobject_placement_data@@PA_N@Z
// @address 0x34a1d8 ?weapon_barrel_add_fire_error@@YAXJJ@Z
// @address 0x34a6d8 ?weapon_ready@@YAXJ_N0@Z
// @address 0x34a880 ?weapon_put_away@@YA_NJ_N0@Z
// @address 0x34acb0 ?weapon_can_be_fired@@YA_NJ@Z
// @address 0x34ae98 ?weapon_reload@@YA_NJJ@Z
// @address 0x34b1b8 ?weapon_steal_goodies@@YAXJJ@Z
// @address 0x34b318 ?weapon_set_total_rounds@@YAXJPAF@Z
// @address 0x34b4c8 ?weapon_set_current_amount@@YAXJM@Z
// @address 0x34b7d8 ?weapon_report_kill@@YAXJJ@Z
// @address 0x34c250 ?weapon_barrel_check_for_reload@@YA_NJJ@Z
// @address 0x34c9f8 ?weapon_magazine_start_or_continue_reload@@YA_NJF@Z
// @address 0x34cfa8 ?weapon_trigger_fully_charged@@YAXJF@Z
// @address 0x34e2f8 ?weapon_trigger_recover@@YAXJF_N@Z
// @address 0x34e498 ?weapon_trigger_begin_tracking@@YAXJF@Z
// @address 0x34e700 ?weapon_trigger_begin_charging@@YAXJF@Z
// @address 0x34ea98 ?weapon_reset@@YAXJ@Z
// @address 0x351588 ?weapon_should_set_targeting_data@@YA_NJPAUs_projectile_targeting_data@@@Z
// @address 0xdb468 ??0c_animation_channel@@QAA@XZ
// @address 0x28e078 ??0c_animation_manager@@QAA@XZ
// @address 0x28e1a8 ?reset_graph@c_animation_manager@@QAA_NJJ_N@Z
// @address 0xed738 ?setup_animation_channel_by_index@c_animation_manager@@QBA_NPAVc_animation_channel@@Vc_animation_id@@K@Z
// @address 0xdf7d8 ?update_animation_channel@c_animation_manager@@QBA_NPAVc_animation_channel@@P6AXPBUs_animation_event_data@@J@ZJ@Z
// @address 0x2345b0 ?effect_stop@@YAXJ_N@Z
// @address 0xd83c0 ?unit_get_weapon_dual_error_theta@@YAMJJ@Z
// @address 0x34e510 ?weapon_trigger_begin_locked@@YAXJF@Z
// @address 0x34e608 ?weapon_trigger_begin_locking@@YAXJF@Z
#include "cseries/crt_xdk.h"
#include <math.h>
#include <string.h>
/* object headers and the object header array (object_header_data): as tu/objectslibxbox/motor_animation.cpp declares them */

#include "math/real_math_constants.h"
// @address 0x3aa90 k_real_math_origin3d


/* tag instances: the definition pointer is at +4 (each use casts it to its definition type) */


/* the object datum: the fields these functions use; tu/objectslibxbox/units.cpp's unit_datum names the same offsets 0x10, 0x9a, 0x25a and 0x260 */




/* one ammunition item a magazine accepts (its rounds and the item's definition) */
typedef char weapons_magazine_tag_stride[(sizeof(void *) != 4 || sizeof(s_weapon_magazine_definition) == 0x80) ? 1 : -1];
typedef char weapons_trigger_tag_stride[(sizeof(void *) != 4 || sizeof(s_weapon_trigger_definition) == 0x90) ? 1 : -1];
typedef char weapons_barrel_tag_stride[(sizeof(void *) != 4 || sizeof(s_weapon_barrel_definition) == 0x134) ? 1 : -1];
typedef char weapons_ready_damage_index[(sizeof(void *) != 4 || offsetof(weapon_definition, ready_damage_effect) == 0x1e0) ? 1 : -1];
typedef char weapons_depleted_effect_index[(sizeof(void *) != 4 || offsetof(weapon_definition, depleted_effect) == 0x4ac) ? 1 : -1];


#include "objects/object_definitions.h"

#include "units/biped_definitions.h"

/* the owner unit: its player, seat, and weapons, as tu/objectslibxbox/units.cpp's unit_datum places them */


#include "animations/animation_event_data.h"


#include "scenario/scenario_weapon_types.h"

#include "units/unit_messages.h"


/* the interface (hud) state of one weapon */
#include "items/weapon_interface_state_types.h"

/* the timer definitions and the game-time globals weapon_trigger_get_charged_fraction reads */


/* the slot array of an object datum weapon_try_and_get_magazine_value reads: 0x18-byte slots at 0x234 */

/* the aim targeting and the game-time reads weapon_owner_autoaimed_on_lockable_target and weapon_tracked_on_something_trackable use */
#include "models/model_target_types.h"

#include "camera/director_globals_types.h"



#include "simulation/game_interface/simulation_game_projectiles.h"


/* object globals (tls object_header_data sibling slot): warthog chaingun light state at +0x24a */
#include "objects/object_runtime_globals_types.h"
#include "objects/object_runtime_tls.h"


/* ---- weapon_barrel_fire: callees and the inlined helpers it expands ---- */


#include "game/game_engine_scoring_globals.h"

#include "interface/chud/chud_navpoints_runtime.h"

#include "items/weapon_fire_types.h"

#include "items/projectile_definitions.h"

#include "units/vehicle_definitions.h"

#include "ai/actor_datum.h"

#include "items/projectile_types.h"

// match: 0x34eba0: spelling these accesses through projectile_datum::projectile flips the error-axis rotate FP load order (lfs 240/244/248(r1), 27 words)
struct weapons_create_projectile : public object_datum, public _projectile_datum
{
    using _projectile_datum::flags;
};
typedef char weapons_create_projectile_size[(sizeof(weapons_create_projectile) == sizeof(projectile_datum)) ? 1 : -1];
typedef char weapons_create_projectile_flags_offset[(sizeof(void *) != 4 || ((offsetof(weapons_create_projectile, flags) == 0x160))) ? 1 : -1];
typedef char weapons_create_projectile_parent_offset[(sizeof(void *) != 4 || ((offsetof(weapons_create_projectile, parent_unit_index) == 0x174))) ? 1 : -1];
typedef char weapons_create_projectile_weapon_offset[(sizeof(void *) != 4 || ((offsetof(weapons_create_projectile, weapon_index) == 0x184))) ? 1 : -1];
typedef char weapons_create_projectile_collision_object_offset[
    (sizeof(void *) != 4 || ((offsetof(weapons_create_projectile, collision_object) == 0x188))) ? 1 : -1];
typedef char weapons_create_projectile_collision_node_offset[
    (sizeof(void *) != 4 || ((offsetof(weapons_create_projectile, collision_node) == 0x18c))) ? 1 : -1];
typedef char weapons_create_projectile_collision_position_offset[
    (sizeof(void *) != 4 || ((offsetof(weapons_create_projectile, collision_position) == 0x190))) ? 1 : -1];
typedef char weapons_create_projectile_damage_offset[(sizeof(void *) != 4 || ((offsetof(weapons_create_projectile, damage_scale) == 0x1d8))) ? 1 : -1];
typedef char weapons_create_projectile_shot_offset[(sizeof(void *) != 4 || ((offsetof(weapons_create_projectile, shot_id) == 0x1f4))) ? 1 : -1];


#include "objects/damage_data.h"

// match: 0x34eba0: the owner player/actor/targeting reads before the gunner walk and the primary-weapon
// lookup read through this view; spelling all of them through unit_datum::unit recolors fr0/fr10-fr13 in the
// error-axis rotate (27 words); the primary-weapon lookup through owner->unit drops 28 bytes of reloads.
struct weapons_unit_datum_flat_view : public object_datum, public _unit_datum {};
typedef char weapons_flat_view_size[(sizeof(weapons_unit_datum_flat_view) == sizeof(unit_datum)) ? 1 : -1];








// @address 0x2251c k_weapon_pitch_throttling_constants
// @address 0x2251c k_real_zero_storage

/* game time (tls game_time_globals) and game globals (tls tls_game_globals()): the fields these functions use */


/* Initialize the base fields of an animation channel. */
static __forceinline void weapons_initialize_base(c_channel_base *channel)
{
    channel->m_parent_slider_index = -1;
    channel->m_parent_slider_type = _mixing_board_parent_slider_none;
    channel->m_enabled = true;
    channel->clear();
}
/* Set a channel frame position from authored time. */
// match: 0xfe428: c_channel_base::get_animation with an unsigned index test; valid()'s signed test swaps two adds in weapon_update.
static __forceinline const c_model_animation *channel_animation_lookup(const c_channel_base *channel)
{
    const c_model_animation *animation = 0;
    if ((unsigned short)channel->m_animation_id.m_animation_index < 0x8000) {
        const c_model_animation_graph *graph = TAG_GET('jmad', const c_model_animation_graph, channel->m_graph_index);
        animation = graph->get_animation(channel->m_animation_id);
    }
    return animation;
}
static __forceinline void weapons_set_frame_position_from_authored_time(c_animation_channel *channel, float authored_time)
{
    (channel)->set_frame_position(authored_time * channel->get_authored_rate());
}
/* initialization followed by disabling the channel,
   which this unit expands into the manager destructor and calls out of line
   in weapon_update_first_person_emulation */
static __forceinline void weapons_inl_channel_destroy(c_animation_channel *channel)
{
    (channel)->initialize();
    weapons_initialize_base(channel);
    channel->m_enabled = false;
}
static __forceinline bool entry_valid(const c_animation_channel *entry)
{
    return entry->m_enabled != 0 && entry->m_graph_index != -1 && (unsigned short)entry->m_animation_id.m_animation_index < 0x8000;
}


static __forceinline weapon_datum *weapon_get(long object_index)
{
    return (weapon_datum *)object_header_get(object_index)->datum;
}
static __forceinline long seat_object(long unit_index, long seat)
{
  weapon_datum *unit = weapon_get(unit_index);
  long seat_index = ((unit_datum *)unit)->unit.current_weapon_indices[seat];
  long result = -1;
  if (seat_index != -1)
    result = ((unit_datum *)unit)->unit.weapon_object_indices[seat_index];
  return result;
}


static __forceinline s_weapon_magazine_definition *magazine_definition_get(weapon_definition *definition, long magazine_index)
{
    return &((s_weapon_magazine_definition *)definition->magazines.address)[magazine_index];
}
static __forceinline s_weapon_barrel *weapon_barrel_get(weapon_datum *weapon, long barrel_index)
{
    return &weapon->barrels[barrel_index];
}
static __forceinline s_weapon_trigger_definition *trigger_definition_get(weapon_definition *definition, long trigger_index)
{
    return &definition->triggers[trigger_index];
}

static __forceinline long weapon_owner_unit_index_get(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    long result = -1;
    if (weapon->item.owner_relation)
        result = weapon->item.unit_object_index;
    return result;
}
static __forceinline float real_floor(float value)
{
    return (float)floor((double)value);
}
static __forceinline bool campaign_skull_available(long flavor)
{
    unsigned long skulls = game_is_campaign() ? tls_game_globals()->options.active_skulls : 0;
    return ((skulls >> flavor) & 1) != 0;
}


static __forceinline long weapons_pitch_active_barrel_index(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    long barrel_index;
    for (barrel_index = definition->barrel_count - 1; barrel_index >= 0; barrel_index--) {
        if (weapon_barrel_get(weapon, barrel_index)->state_1 == 1)
            break;
    }
    if (barrel_index == -1)
        barrel_index = 0;
    return barrel_index;
}

static __forceinline long weapons_pitch_throttling_active_barrel_index(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    long barrel_index = definition->barrel_count - 1;
    if (barrel_index >= 0) {
        unsigned char *active = (unsigned char *)&weapon->barrels[barrel_index].state_1;
        do {
            if (*active == 1)
                break;
            barrel_index--;
            active -= sizeof(s_weapon_barrel);
        } while (barrel_index >= 0);
    }
    if (barrel_index == -1)
        barrel_index = 0;
    return barrel_index;
}

// @address 0x98b50 ?weapon_get_pitch_throttling@@YAMJ@Z
float weapon_get_pitch_throttling(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    long barrel_index = weapons_pitch_throttling_active_barrel_index(weapon_index);
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x98b50: 130.0f and 0.0f are read off one k_weapon_pitch_throttling_constants base held in r30;
    // plain literals load a base per constant and the function grows by 12 bytes
    float result = k_weapon_pitch_throttling_constants[7594];
#else
    float result = 130.0f;
#endif
    if (definition->barrel_count > barrel_index) {
        s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
        s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
#if defined(_XBOX) && !defined(__clang__)
        float zero = k_weapon_pitch_throttling_constants[0];
#else
        float zero = 0.0f;
#endif
        double exponent = barrel_definition->pitch_exponent;
        if (exponent == zero)
            exponent = 1.0;
        float t = (float)pow(barrel->firing_error_fraction, (float)exponent);
        float pitch_min = barrel_definition->pitch_min;
        if (pitch_min == zero)
#if defined(_XBOX) && !defined(__clang__)
            pitch_min = k_weapon_pitch_throttling_constants[7594];
#else
            pitch_min = 130.0f;
#endif
        float pitch_max = barrel_definition->pitch_max;
        if (pitch_max == zero)
#if defined(_XBOX) && !defined(__clang__)
            pitch_max = k_weapon_pitch_throttling_constants[7594];
#else
            pitch_max = 130.0f;
#endif
        result = pitch_min + (pitch_max - pitch_min) * t;
    }
    return result;
}

float weapon_compute_movement_penalty(long weapon_index, bool forward, bool moving)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    float penalty = forward ? definition->movement_penalty_forward : definition->movement_penalty_sideways;
    long penalty_type = definition->movement_penalty_type;
    if (penalty_type != 1) {
        if (penalty_type == 2 && weapon_reloading(weapon_index, -1, 0) && !moving)
            penalty = 0.0f;
    } else if (!moving) {
        penalty = 0.0f;
    }
    return penalty;
}

bool weapon_is_melee_only(long weapon_index)
{
    bool result = false;
    if (weapon_index != -1) {
        weapon_datum *weapon = weapon_get(weapon_index);
        weapon_definition *definition = weapon_definition_get(weapon->definition_index);
        result = definition->m_flags.test(_weapon_definition_melee_only);
    }
    return result;
}

float weapon_get_zoom_magnification(long weapon_index, short zoom_level)
{
    float magnification = 1.0f;
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    if (zoom_level >= 0 && zoom_level < definition->zoom_level_count) {
        float zoom_fraction = definition->zoom_level_count > 1 ? (float)zoom_level / (float)(definition->zoom_level_count - 1) : 0.0f;
        float minimum = definition->zoom_magnification_minimum > 0.0f ? definition->zoom_magnification_minimum : 1.0f;
        float maximum = definition->zoom_magnification_maximum > 0.0f ? definition->zoom_magnification_maximum : 1.0f;
        magnification = minimum * power(maximum / minimum, zoom_fraction);
    }
    return magnification;
}

bool weapon_is_tracking(long object_index)
{
  bool result = false;
  weapon_datum *object = (weapon_datum *)object_header_get(object_index)->datum;
  weapon_definition *definition = weapon_definition_get(object->definition_index);
  for (long i = 0; i < definition->trigger_count; i++) {
    char state = object->triggers[i].state;
    if (state == 3 || state == 7 || state == 5 || state == 6 || state == 2 && definition->tracking_type)
      result = true;
  }
  return result;
}


bool weapon_reloading(long weapon_index, long magazine_index, short *rounds_reloaded)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool result = false;
    short rounds = -1;
    for (long index = 0; index < definition->magazines.s_tag_block::count; index++)
    {
        if ((magazine_index == -1 || magazine_index == index) &&
            weapon_try_and_get_magazine_value(weapon_index, (short)index, &rounds))
        {
            result = true;
            break;
        }
    }
    if (rounds_reloaded)
        *rounds_reloaded = rounds;
    return result;
}

float weapon_estimate_time_to_target(long weapon_index, short barrel_index, float distance)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    float result = 0.0f;
    if (barrel_index >= 0 && barrel_index < definition->barrel_count) {
        s_weapon_barrel_definition *barrel = &definition->barrels[barrel_index];
        if (barrel->projectile_index != -1)
            result = projectile_estimate_time_to_target(barrel->projectile_index, distance);
    }
    return result;
}

// 0xd8248

static __forceinline s_weapon_trigger *weapon_get_trigger(weapon_datum *weapon, short trigger_index)
{
    return &weapon->triggers[trigger_index];
}

void weapon_owner_update(long weapon_index, c_flags<e_weapon_control_flags, unsigned short, 8> control_flags, float analog_trigger)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    weapon->control_flags = control_flags.get_unsafe();
    weapon->analog_trigger = (unsigned char)(analog_trigger * 255.0f);
    if (definition->trigger_count > 0) {
        s_weapon_trigger_definition *trigger_definition = trigger_definition_get(definition, 0);
        s_weapon_trigger *trigger = weapon_get_trigger(weapon, 0);
        if (trigger_definition->behavior == 5 && trigger->state != 6) {
            long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
            if (owner_unit_index != -1) {
                unit_datum *unit = (unit_datum *)weapon_get(owner_unit_index);
                long target_index;
                char model_target_index;
                if (unit->unit.player_index != -1 &&
                    ::weapon_owner_autoaimed_on_lockable_target(owner_unit_index, (long *)(&target_index), (char *)(&model_target_index)))
                {
                    weapon->tracked_object_index = target_index;
                    weapon->current_state_ticks = model_target_index;
                    weapon->target_time = game_time_globals->time;
                }
            }
        }
        if (definition->tracking_type == 1 && trigger_definition->behavior == 1) {
            long projectile_owner_index = ::weapon_get_projectile_owner_unit_index(weapon_index);
            if (projectile_owner_index != -1 && ((unit_datum *)weapon_get(projectile_owner_index))->unit.player_index != -1)
                object_wake(weapon_index);
        }
    }
    if (control_flags.get_unsafe() & 0x19f)
        object_wake(weapon_index);
}

inline bool c_animation_channel::playing() const
{
    bool result = entry_valid(this) && (m_playback_flags.get_unsafe() & 1) != 0 && (m_state_flags.get_unsafe() & 1) == 0 &&
        (m_state_flags.get_unsafe() & 4) == 0;
    return result;
}

bool weapon_is_support_weapon_or_must_be_readied(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    return definition->m_flags.test(_weapon_definition_support_weapon) || definition->m_flags.test(_weapon_definition_must_be_readied);
}






#define WEAPONS_UPDATE_TRIGGER_SET_STATE(weapon_index, trigger_index, state_val, timer_val) \
    do { \
        weapon_datum *weapon = (weapon_datum *)object_get(weapon_index); \
        weapon->triggers[trigger_index].timer = (timer_val); \
        weapon->triggers[trigger_index].state = (signed char)(state_val); \
    } while (0)

#define WEAPONS_UPDATE_TRIGGER_SET_STATE_H(weapon_index, trigger_index, state_val, timer_val) \
    do { \
        s_object_header *header = object_header_get(weapon_index); \
        weapon_datum *weapon = (weapon_datum *)header->datum; \
        weapon_datum *weapon2 = (weapon_datum *)header->datum; \
        weapon->triggers[trigger_index].timer = (timer_val); \
        weapon2->triggers[trigger_index].state = (signed char)(state_val); \
    } while (0)

static __forceinline void weapons_update_trigger_overcharged(long weapon_index, short trigger_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_trigger_definition *trigger_definition = trigger_definition_get(definition, trigger_index);
    switch (trigger_definition->overcharged_action) {
    case 1:
        weapon_detonate(weapon_index);
        break;
    case 2:
        weapon_trigger_release_charge(weapon_index, trigger_index);
        break;
    }
}


static __forceinline void weapons_inl_reload_clear_empty_click_bit(long weapon_index, short barrel_index)
{
    if (barrel_index < 0)
        return;
    if (barrel_index >= 2)
        return;
    s_weapon_barrel *barrel = weapon_barrel_get(weapon_get(weapon_index), barrel_index);
    if (barrel->state_1 == 1)
        return;
    barrel->flags &= 0xffbf;
}

static __forceinline void weapons_inl_reload_set_fire_bit(long weapon_index, short barrel_index, bool set)
{
    if (barrel_index == -1)
        return;
    s_weapon_barrel *barrel = weapon_barrel_get(weapon_get(weapon_index), barrel_index);
    if (set)
        barrel->flags |= 1;
    else
        barrel->flags &= 0xfffe;
}


static __forceinline void weapons_barrel_idle(long weapon_index, short barrel_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
    barrel->active = 0;
    barrel->value = 0;
    barrel->state_1 = 0;
    if (!test_bit(barrel_definition->flags, 11)) {
        if (barrel_index != -1) {
            s_weapon_barrel *fire_barrel = weapon_barrel_get(weapon_get(weapon_index), barrel_index);
            fire_barrel->flags &= 0xfffe;
        }
    }
}

static void weapon_handle_primary_frame_event(long weapon_index);
static void weapon_handle_state_finished(long weapon_index, bool looping);

// @address 0xfe428 ?weapon_update@@YA_NJ@Z
bool weapon_update(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool dual_wielded = weapon_is_dual_wielded(weapon_index);
    float dual_error_theta = unit_get_weapon_dual_error_theta(weapon_owner_unit_index_get(weapon_index), weapon_index);
    bool weapon_disabled = false;
    long player_index = -1;
    bool result = false;

    if (bit_test(weapon->item.object.physics_flags, 13) && weapon->item.object.havok_component_index != -1) {
        c_havok_component *havok_component = havok_component_get(weapon->item.object.havok_component_index);
        havok_component_update_phantom_state(havok_component, true);
        result = true;
    }

    if (definition->m_flags.test(_weapon_definition_dual_wield_error_simple))
        dual_error_theta = dual_wielded ? 1.0f : 0.0f;

    long projectile_owner_unit_index = weapon_get_projectile_owner_unit_index(weapon_index);
    if (projectile_owner_unit_index != -1) {
        unit_datum *owner_unit = (unit_datum *)weapon_get(projectile_owner_unit_index);
        if (owner_unit->unit.player_index != -1)
            player_index = owner_unit->unit.player_index;
    }

    if (weapon->tracked_object_index != -1) {
        weapon_datum *target = (weapon_datum *)object_try_and_get_and_verify_type(weapon->tracked_object_index, 0xffffffff);
        if (!target || bit_test(target->item.object.damage_flags, 2))
            weapon->tracked_object_index = -1;
    }

    result |= weapon_update_first_person_emulation(weapon_index);

    if ((unsigned short)weapon_get(weapon_index)->item.object.animation.offset != 0xffff) {
        c_animation_manager *animation_manager = (c_animation_manager *)((char *)weapon + (short)weapon->item.object.animation.offset);
        const c_model_animation *state_animation = channel_animation_lookup(&animation_manager->m_state_channel);
        const c_animation_channel *state_channel = animation_manager->get_state_channel();
        animation_manager->update_state_animation(object_animation_callback, weapon_index, 0, 0, 0);
        const c_flags<e_animation_frame_event_types, unsigned short, 13> &event_flags = state_channel->get_event_flags();
        if (event_flags.test(_animation_frame_event_type_primary_keyframe)) {
            weapon_handle_primary_frame_event(weapon_index);
        } else {
            bool reached_end = state_channel->reached_last_frame();
            bool looping = state_animation && state_animation->frame_count > 1;
            if (reached_end || !looping)
                weapon_handle_state_finished(weapon_index, looping);
        }
        if (state_channel->valid() && state_channel->playing() && state_animation->frame_count > 1)
            result = true;
    }

    if (!weapon->weapon_flags.test(5)) {
        if (weapon->turn_on_timer != 0)
            weapon->turn_on_timer--;
        else
            weapon->weapon_flags.set(5, true);
        result = true;
    }

    if (!weapon->weapon_flags.test(8)) {
        if (weapon->ready_for_use_timer != 0)
            weapon->ready_for_use_timer--;
        else
            weapon->weapon_flags.set(8, true);
        result = true;
    }

    // match: 0xfe428: volatile parent read keeps the parent load in place in the caller.
    const volatile datum_index &parent_object_index = weapon->item.object.parent_object_index;
    if (definition->m_flags.test(_weapon_definition_detonates_when_dropped) && parent_object_index == -1
        && bit_test(weapon->item.object.flags, 8))
        item_detonate(weapon_index);

    if ((weapon->control_flags & 1) != 0) {
        SET_BIT(weapon->flags, 4, false);
        if (weapon->f1a0 == 0.0f)
            weapon->f1a0 = 1.0f;
        else
            weapon->f1a0 = 0.0f;
    }

    if (weapon->current_power != weapon->f1a0) {
        bool turning_on = weapon->current_power < weapon->f1a0;
        if (!weapon->weapon_flags.test(4)) {
            long effect_index = turning_on ? definition->turn_on_effect : definition->turn_off_effect;
            if (effect_index != -1)
                weapon_effect_new(weapon_index, -1, effect_index, 0.0f, 0.0f);
            weapon->weapon_flags.set(4, true);
        }
        if (turning_on) {
            weapon->current_power += definition->turn_on_speed * game_tick_length();
            weapon->current_power = weapon->current_power > weapon->f1a0 ? weapon->f1a0 : weapon->current_power;
        } else {
            weapon->current_power -= definition->turn_off_speed * game_tick_length();
            weapon->current_power = MAX(weapon->current_power, weapon->f1a0);
        }
        result = true;
    }

    if (weapon->heat > 0.0f) {
        bool barrel_active = false;
        bool barrel_heats = false;
        bool block_heat_loss = false;
        for (short barrel_index = 0; barrel_index < definition->barrel_count; barrel_index++) {
            s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
            s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
            if (barrel->state_1 == 1 || barrel->value != 0) {
                barrel_active = true;
                if (barrel_definition->heat_recovery_threshold > 0.0f)
                    barrel_heats = true;
            }
        }
        if (weapon->heat >= definition->overheated_threshold) {
            if ((unsigned short)definition->reload_interrupt_mode == 7 && barrel_active && barrel_heats) {
                block_heat_loss = true;
            } else if (!weapon->weapon_flags.test(1)) {
                weapon->weapon_flags.set(1, true);
                long owner_index = weapon_owner_unit_index_get(weapon_index);
                if (owner_index != -1)
                    unit_unzoom(owner_index, false);
                if ((short)(unsigned short)definition->reload_interrupt_mode == 3 || (short)(unsigned short)definition->reload_interrupt_mode == 7) {
                    if (weapon->weapon_flags.test(3))
                        weapon->weapon_flags.set(3, false);
                }
                weapon_send_message_to_unit(weapon_index, _unit_message_overheat_enter);
                damage_effect_new_on_weapon(weapon_index, definition->overheated_damage_effect);
            }
        }
        if (!block_heat_loss && weapon->f198 <= 0.0f) {
            float loss_rate = (weapon->weapon_flags.test(1) && definition->overheated_heat_loss_per_second > 0.0f)
                ? definition->overheated_heat_loss_per_second
                : definition->heat_loss_per_second;
            float heat_delta = loss_rate * game_tick_length();
            if (definition->heat_loss_age_scale > 0.0f)
                heat_delta *= 1.0f - weapon_get_age(weapon_index) * definition->heat_loss_age_scale;
            weapon->heat -= heat_delta;
            if (weapon->heat < 0.0f)
                weapon->heat = 0.0f;
            if (weapon->weapon_flags.test(1) && !weapon->weapon_flags.test(2)) {
                short exit_ticks = weapon_get_deterministic_first_person_animation_duration_ticks(weapon_index, 117, 1);
                exit_ticks = MAX(exit_ticks, 1);
                if (weapon->heat - (float)exit_ticks * heat_delta <= definition->heat_recovery_threshold) {
                    weapon->weapon_flags.set(2, true);
                    weapon_send_message_to_unit(weapon_index, _unit_message_overheat_exit);
                }
            }
        }
        if (weapon->weapon_flags.test(1) && weapon->heat < definition->heat_recovery_threshold) {
            weapon->weapon_flags.set(1, false);
            weapon->weapon_flags.set(2, false);
        }
        result = true;
    }

    if (definition->overheated_effect != -1) {
        bool want_effect = weapon->weapon_flags.test(1) && weapon->item.owner_relation != 2;
        bool has_effect = weapon->overheated_effect_index != -1;
        if (want_effect != has_effect) {
            if (want_effect) {
                weapon->overheated_effect_index = weapon_effect_new(weapon_index, -1, definition->overheated_effect, 0.0f, 0.0f);
            } else {
                effect_delete_internal(weapon->overheated_effect_index, true);
                weapon->overheated_effect_index = -1;
            }
        }
    }

    weapon->f198 = 0.0f;

    if (weapon->time_184 > 0) {
        weapon->time_184--;
        result = true;
    }

    if (weapon->ready_ticks > 0) {
        weapon->ready_ticks--;
        result = true;
    }

    if (weapon->prediction_ticks != 0) {
        weapon->prediction_ticks--;
        if (weapon->prediction_ticks == 0) {
            weapon_datum *w = weapon_get(weapon_index);
            weapon_definition *def = weapon_definition_get(w->definition_index);
            w->age = w->predicted_age;
            for (short magazine_index = 0; magazine_index < def->magazines.s_tag_block::count; magazine_index++) {
                s_weapon_magazine *magazine = &w->magazines[magazine_index];
                magazine->rounds_loaded = magazine->predicted_rounds_loaded;
                magazine->rounds_inventory = magazine->predicted_rounds_inventory;
            }
            w->prediction_ticks = 0;
        } else {
            result = true;
        }
    }

    bool trigger_held[3] = {
        (bool)((weapon->control_flags >> 3) & 1),
        (bool)((weapon->control_flags >> 4) & 1),
        false
    };
    bool trigger_down[3] = {
        (bool)((weapon->control_flags >> 1) & 1),
        (bool)((weapon->control_flags >> 2) & 1),
        bit_test(weapon->control_flags, 8)
    };

    weapon_datum *busy_weapon = weapon_get(weapon_index);
    bool is_busy = false;
    if (bit_test(busy_weapon->control_flags, 5) ||
        busy_weapon->ready_ticks > 0 ||
        busy_weapon->time_184 > 0 ||
        weapon_get(weapon_index)->weapon_flags.test(1))
    {
        is_busy = true;
    }
    if (is_busy) {
        weapon->trigger_ticks = 0;
        weapon_disabled = true;
        result = true;
    }

    for (short magazine_index = 0; magazine_index < definition->magazines.s_tag_block::count; magazine_index++)
        result |= weapon_magazine_update(weapon_index, magazine_index);

    for (short trigger_index = 0; trigger_index < definition->trigger_count; trigger_index++) {
        s_weapon_trigger_definition *trigger_definition = &definition->triggers[trigger_index];
        s_weapon_trigger *trigger = &weapon->triggers[trigger_index];
        bool trigger_is_down = false;
        bool trigger_is_held = false;

        if (trigger->timer > 0) {
            trigger->timer--;
            result = true;
        }
        if (trigger->mispredict_ticks != 0) {
            trigger->mispredict_ticks--;
            result = true;
        }
        if (definition->tracking_type == 1 && trigger_definition->behavior == 1 && player_index != -1)
            result = true;

        if (!weapon_disabled) {
            trigger_is_down = trigger_down[trigger_definition->input];
            trigger_is_held = trigger_held[trigger_definition->input];
            if ((trigger->flags & 128) != 0)
                trigger_is_down = true;
        }

        switch (trigger->state) {
        case 0:
            switch (trigger_definition->behavior) {
            case 0:
                if ((trigger_is_down || trigger_is_held) && trigger_definition->primary_barrel != -1 &&
                    weapon_barrel_can_fire(weapon_index, trigger_definition->primary_barrel, false))
                {
                    if ((trigger->flags & 32) == 0) {
                        long mispredict_ticks = game_seconds_to_ticks_round(g_network_configuration.trigger_mispredict_time);
                        SET_BIT(trigger->flags, 5, true);
                        trigger->mispredict_ticks = (unsigned char)MIN(mispredict_ticks, 255);
                    }
                    weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                } else {
                    SET_BIT(trigger->flags, 5, false);
                }
                if (!trigger_is_down || trigger_is_held)
                    weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->primary_barrel);
                break;

            case 1:
                if (trigger_is_down && (trigger->flags & 1) != 0) {
                    if (trigger_definition->primary_barrel != -1 && weapon_barrel_can_fire(weapon_index, trigger_definition->primary_barrel, false)) {
                        weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                        SET_BIT(trigger->flags, 0, false);
                    }
                } else if (!trigger_is_down) {
                    SET_BIT(trigger->flags, 0, true);
                    weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->primary_barrel);
                }
                if (definition->tracking_type == 1 && player_index != -1) {
                    long target_index;
                    char model_target_index;
                    if (weapon_owner_autoaimed_on_lockable_target(weapon_get_projectile_owner_unit_index(weapon_index), &target_index, &model_target_index)) {
                        weapon->tracked_object_index = target_index;
                        weapon->current_state_ticks = model_target_index;
                        weapon_trigger_begin_locked(weapon_index, trigger_index);
                    }
                }
                break;

            case 4:
                if (trigger_is_down && (trigger->flags & 1) != 0) {
                    bool use_secondary = bit_test(weapon->control_flags, 7);
                    if (!use_secondary && trigger_definition->primary_barrel != -1 &&
                        weapon_barrel_can_fire(weapon_index, trigger_definition->primary_barrel, false))
                    {
                        weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                        SET_BIT(trigger->flags, 0, false);
                    }
                    if (use_secondary && trigger_definition->barrel_index != -1 &&
                        weapon_barrel_can_fire(weapon_index, trigger_definition->barrel_index, false))
                    {
                        weapon_barrel_set_fire_bit(weapon_index, trigger_definition->barrel_index, true);
                        SET_BIT(trigger->flags, 0, false);
                    }
                } else if (!trigger_down[trigger_definition->input]) {
                    SET_BIT(trigger->flags, 0, true);
                    weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->primary_barrel);
                    weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->barrel_index);
                }
                break;

            case 5:
                if (trigger_is_down && (trigger->flags & 1) != 0 && weapon_barrel_can_fire(weapon_index, 0, false)) {
                    if ((trigger->flags & 512) == 0) {
                        long tracking_ticks = game_seconds_to_ticks_round(trigger_definition->tracking_time);
                        trigger->tracking_ticks = (unsigned char)MIN(tracking_ticks, 255);
                        SET_BIT(trigger->flags, 9, true);
                    } else if (trigger->tracking_ticks != 0) {
                        trigger->tracking_ticks--;
                    }
                    SET_BIT(trigger->flags, 8, true);
                    if (player_index != -1) {
                        long target_index;
                        char model_target_index;
                        if (weapon_tracked_on_something_trackable(weapon_index, &target_index, &model_target_index)) {
                            weapon_trigger_begin_locking(weapon_index, trigger_index);
                            SET_BIT(trigger->flags, 8, false);
                        }
                    } else if (trigger_definition->barrel_index != -1 && weapon_barrel_can_fire(weapon_index, trigger_definition->barrel_index, false)) {
                        weapon_barrel_set_fire_bit(weapon_index, trigger_definition->barrel_index, true);
                        SET_BIT(trigger->flags, 0, false);
                    }
                } else if (!trigger_is_down) {
                    if ((trigger->flags & 256) != 0 && trigger->tracking_ticks > 0 && player_index != -1 &&
                        trigger_definition->primary_barrel != -1 &&
                        weapon_barrel_can_fire(weapon_index, trigger_definition->primary_barrel, false))
                        weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                    SET_BIT(trigger->flags, 0, true);
                    SET_BIT(trigger->flags, 8, false);
                    SET_BIT(trigger->flags, 9, false);
                    weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->primary_barrel);
                    weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->barrel_index);
                }
                break;

            case 2:
                if (!weapon_disabled) {
                    if (weapon->analog_trigger > 0 || weapon->previous_analog_trigger > 0) {
                        bool fire;
                        bool alt;
                        bool clear;
                        weapon_trigger_determine_analog_autofire_behavior(weapon_index, trigger_index, &fire, &alt, &clear);
                        if (fire) {
                            SET_BIT(trigger->flags, 3, true);
                            switch (trigger_definition->primary_action) {
                            case 0:
                                if (trigger_definition->primary_barrel != -1 && weapon_barrel_can_fire(weapon_index, trigger_definition->primary_barrel, false))
                                    weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                                break;
                            case 3:
                                if (trigger_definition->barrel_index != -1 && weapon_barrel_can_fire(weapon_index, trigger_definition->barrel_index, false))
                                    weapon_barrel_set_fire_bit(weapon_index, trigger_definition->barrel_index, true);
                                break;
                            case 1:
                                if (weapon_trigger_can_begin_charging(weapon_index, trigger_index))
                                    weapon_trigger_begin_charging(weapon_index, trigger_index);
                                break;
                            case 2:
                                weapon_trigger_begin_tracking(weapon_index, trigger_index);
                                break;
                            }
                        }
                        if (alt) {
                            SET_BIT(trigger->flags, 4, true);
                            switch (trigger_definition->secondary_action) {
                            case 0:
                                if (trigger_definition->barrel_index != -1 && weapon_barrel_can_fire(weapon_index, trigger_definition->barrel_index, false))
                                    weapon_barrel_set_fire_bit(weapon_index, trigger_definition->barrel_index, true);
                                break;
                            case 3:
                                if (trigger_definition->primary_barrel != -1 && weapon_barrel_can_fire(weapon_index, trigger_definition->primary_barrel, false))
                                    weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                                break;
                            case 1:
                                if (weapon_trigger_can_begin_charging(weapon_index, trigger_index))
                                    weapon_trigger_begin_charging(weapon_index, trigger_index);
                                break;
                            case 2:
                                if (player_index != -1)
                                    weapon_trigger_begin_tracking(weapon_index, trigger_index);
                                else
                                    weapon_trigger_begin_locked(weapon_index, trigger_index);
                                break;
                            }
                        }
                        if (clear) {
                            SET_BIT(trigger->flags, 3, false);
                            SET_BIT(trigger->flags, 4, false);
                            weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->primary_barrel);
                            weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->barrel_index);
                        }
                    } else if (trigger_is_held) {
                        if (trigger_definition->secondary_action == 1 && weapon_trigger_can_begin_charging(weapon_index, trigger_index)) {
                            weapon_trigger_begin_charging(weapon_index, trigger_index);
                            SET_BIT(trigger->flags, 6, true);
                        }
                    } else {
                        SET_BIT(trigger->flags, 3, false);
                        SET_BIT(trigger->flags, 4, false);
                        weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->primary_barrel);
                        weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->barrel_index);
                    }
                } else {
                    weapon->previous_analog_trigger = 0;
                    weapon->analog_trigger = 0;
                    weapon->trigger_direction = 0;
                }
                break;

            case 3:
            case 7:
                if ((trigger_is_down || trigger_is_held) && weapon_trigger_can_begin_charging(weapon_index, trigger_index)) {
                    if (trigger_is_held)
                        SET_BIT(trigger->flags, 6, true);
                    weapon_trigger_begin_charging(weapon_index, trigger_index);
                } else {
                    weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->primary_barrel);
                }
                break;

            case 6:
                if ((trigger_is_down || trigger_is_held) && weapon_trigger_can_begin_charging(weapon_index, trigger_index) && (trigger->flags & 1) != 0) {
                    if (trigger_is_held)
                        SET_BIT(trigger->flags, 6, true);
                    weapon_trigger_begin_charging(weapon_index, trigger_index);
                    SET_BIT(trigger->flags, 0, false);
                } else if ((trigger_is_down || trigger_is_held) && (trigger->flags & 1) != 0) {
                    if (weapon_has_infinite_ammo(weapon_index) && trigger_definition->barrel_index != -1)
                        weapon_barrel_set_fire_bit(weapon_index, trigger_definition->barrel_index, true);
                } else if (!trigger_is_down && !trigger_is_held) {
                    SET_BIT(trigger->flags, 0, true);
                    weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->primary_barrel);
                    if (trigger_definition->barrel_index != -1)
                        weapon_barrel_clear_empty_click_bit(weapon_index, trigger_definition->barrel_index);
                }
                break;
            }
            break;

        case 1:
            if (trigger->timer != 0) {
                if (trigger_definition->charging_damage_response != -1 && player_index != -1) {
                    float charged_fraction = weapon_trigger_get_charged_fraction(weapon_index, trigger_index);
                    long owner_unit_index = weapon_get_owner_unit_index(weapon_index);
                    player_effect_start(owner_unit_index, -1, trigger_definition->charging_damage_response, damage_owner_unknown(),
                        global_up3d, 0.0f, charged_fraction, 0, false);
                }
                if (trigger_is_down || trigger_is_held) {
                    if (trigger_definition->behavior == 6)
                        weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                } else {
                    if ((trigger->flags & 64) != 0) {
                        SET_BIT(trigger->flags, 6, false);
                    } else if (trigger_definition->primary_barrel != -1) {
                        weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                    }
                    SET_BIT(trigger->flags, 0, true);
                    weapon_trigger_recover(weapon_index, trigger_index, false);
                }
            } else {
                weapon_trigger_fully_charged(weapon_index, trigger_index);
            }
            break;

        case 2:
            if (trigger_definition->charging_age_degeneration > 0.0f && !::game_engine_player_has_infinite_ammo(weapon_index)) {
                weapon->age += trigger_definition->charging_age_degeneration / (float)game_tick_rate();
                // match: 0xfe428: written out; PIN() here swaps two adds in the barrel loop.
                weapon->age = (weapon->age > 0.0f ? weapon->age : 0.0f) < 1.0f ? (weapon->age > 0.0f ? weapon->age : 0.0f) : 1.0f;
                if (weapon_has_infinite_ammo(weapon_index) && (trigger->flags & 64) == 0) {
                    weapon->age = 1.0f;
                    weapon_trigger_release_charge(weapon_index, trigger_index);
                }
                weapon_set_state_update_timer(weapon_index);
                simulation_action_weapon_state_update(weapon_index);
                result = true;
            }
            if (trigger_definition->charging_damage_response != -1 && player_index != -1) {
                float charged_fraction = weapon_trigger_get_charged_fraction(weapon_index, trigger_index);
                long owner_unit_index = weapon_get_owner_unit_index(weapon_index);
                player_effect_start(owner_unit_index, -1, trigger_definition->charging_damage_response, damage_owner_unknown(), global_up3d,
                    0.0f, charged_fraction, 0, false);
            }
            if ((trigger->flags & 64) == 0) {
                if (!trigger_is_down && !weapon_disabled) {
                    long min_delta = trigger_definition->cancelled_trigger_throw;
                    long delta = weapon->previous_analog_trigger - weapon->analog_trigger;
                    if (delta > min_delta || (delta == 0 && weapon->analog_trigger == 0) || trigger_definition->behavior != 2) {
                        weapon_trigger_release_charge(weapon_index, trigger_index);
                        weapon->previous_analog_trigger = 0;
                        weapon->analog_trigger = 0;
                    } else {
                        weapon_trigger_recover(weapon_index, trigger_index, true);
                        weapon_send_message_to_unit(weapon_index, _unit_message_idle);
                        weapon->trigger_direction = -1;
                    }
                }
                if (trigger->timer == 0)
                    weapons_update_trigger_overcharged(weapon_index, trigger_index);
            } else {
                bool release = !trigger_is_held && !weapon_disabled;
                if ((unsigned short)definition->reload_interrupt_mode == 3)
                    release = weapon->weapon_flags.test(1) | release;
                if (release) {
                    weapon_trigger_recover(weapon_index, trigger_index, false);
                    SET_BIT(trigger->flags, 6, false);
                }
            }
            if (trigger_definition->behavior == 2) {
                weapon->previous_analog_trigger = weapon->analog_trigger;
                result = true;
            }
            break;

        case 3:
        case 7:
            if (!trigger_is_down && !weapon_disabled) {
                if (trigger->state != 7 && (trigger_definition->behavior == 5 || trigger->tracking_ticks > 0) &&
                    trigger_definition->primary_barrel != -1 &&
                    weapon_barrel_can_fire(weapon_index, trigger_definition->primary_barrel, false))
                {
                    weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                    SET_BIT(trigger->flags, 0, false);
                }
                weapon_trigger_recover(weapon_index, trigger_index, false);
            } else {
                if (trigger->tracking_ticks != 0)
                    trigger->tracking_ticks--;
                long target_index;
                char model_target_index;
                if (weapon_tracked_on_something_trackable(weapon_index, &target_index, &model_target_index))
                    weapon_trigger_begin_locking(weapon_index, trigger_index);
            }
            break;

        case 5:
            if (!trigger_is_down && !weapon_disabled) {
                if (trigger_definition->primary_barrel != -1 && (trigger_definition->behavior == 5 || trigger->tracking_ticks > 0) &&
                    weapon_barrel_can_fire(weapon_index, trigger_definition->primary_barrel, false))
                {
                    weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                    SET_BIT(trigger->flags, 0, false);
                }
                weapon_trigger_recover(weapon_index, trigger_index, false);
            } else {
                if (trigger->tracking_ticks != 0)
                    trigger->tracking_ticks--;
                long target_index;
                char model_target_index;
                if (weapon_tracked_on_something_trackable(weapon_index, &target_index, &model_target_index)) {
                    if (weapon->tracked_object_index == target_index) {
                        weapon->current_state_ticks = model_target_index;
                        if (trigger->timer == 0)
                            weapon_trigger_begin_locked(weapon_index, trigger_index);
                    } else {
                        weapon_trigger_begin_locking(weapon_index, trigger_index);
                        weapon->tracked_object_index = target_index;
                        weapon->current_state_ticks = model_target_index;
                    }
                    if (weapon->tracked_object_index != -1) {
                        long pad0 = weapon_index, pad1 = weapon_index, pad2 = weapon_index, pad3 = weapon_index, pad4 = weapon_index, pad5 = weapon_index;
                        long pad6 = weapon_index, pad7 = weapon_index, pad8 = weapon_index, pad9 = weapon_index, pad10 = weapon_index,
                             pad11 = weapon_index, pad12 = weapon_index, pad13 = weapon_index, pad14 = weapon_index, pad15 = weapon_index,
                             pad16 = weapon_index, pad17 = weapon_index;
                        weapon_datum *target = (weapon_datum *)object_try_and_get_and_verify_type(weapon->tracked_object_index, 0xffffffff);
                        if (target && ((1 << target->item.object.object_type) & 0x1003) != 0)
                            unit_notify_of_tracking_or_locking(weapon->tracked_object_index, -1, 2);
                    }
                } else {
                    WEAPONS_UPDATE_TRIGGER_SET_STATE(weapon_index, trigger_index, 0, 0);
                }
            }
            break;

        case 6:
        {
            bool continue_locked = false;
            if (trigger_definition->behavior == 1) {
                if (trigger_is_down && (trigger->flags & 1) != 0) {
                    if (trigger_definition->primary_barrel != -1 && weapon_barrel_can_fire(weapon_index, trigger_definition->primary_barrel, false)) {
                        weapon_barrel_set_fire_bit(weapon_index, trigger_definition->primary_barrel, true);
                        SET_BIT(trigger->flags, 0, false);
                    }
                    continue_locked = true;
                } else if (!trigger_is_down) {
                    SET_BIT(trigger->flags, 0, true);
                    weapons_inl_reload_clear_empty_click_bit(weapon_index, trigger_definition->primary_barrel);
                    continue_locked = true;
                } else {
                    continue_locked = true;
                }
            } else {
                if (!trigger_is_down && !weapon_disabled) {
                    if (trigger_definition->barrel_index != -1 && weapon_barrel_can_fire(weapon_index, trigger_definition->barrel_index, false))
                        weapon_barrel_set_fire_bit(weapon_index, trigger_definition->barrel_index, true);
                    weapon_trigger_recover(weapon_index, trigger_index, false);
                } else {
                    continue_locked = true;
                }
            }
            if (continue_locked) {
                long owner_unit_index = weapon_get_owner_unit_index(weapon_index);
                if (trigger->tracking_ticks != 0)
                    trigger->tracking_ticks--;
                long target_index;
                char model_target_index;
                if (weapon_owner_autoaimed_on_lockable_target(owner_unit_index, &target_index, &model_target_index)) {
                    weapon->tracked_object_index = target_index;
                    weapon->current_state_ticks = model_target_index;
                    weapon_trigger_begin_locked(weapon_index, trigger_index);
                }
                if (weapon->tracked_object_index != -1) {
                    weapon_datum *target = (weapon_datum *)object_try_and_get_and_verify_type(weapon->tracked_object_index, 0xffffffff);
                    if (target) {
                        if (((1 << target->item.object.object_type) & 0x1003) != 0)
                            unit_notify_of_tracking_or_locking(weapon->tracked_object_index, -1, 4);
                        if (bit_test(target->item.object.damage_flags, 2))
                            WEAPONS_UPDATE_TRIGGER_SET_STATE_H(weapon_index, trigger_index, 0, 0);
                    }
                } else {
                    WEAPONS_UPDATE_TRIGGER_SET_STATE_H(weapon_index, trigger_index, 0, 0);
                }
                if (trigger->timer == 0 && player_index != -1) {
                    weapon->tracked_object_index = -1;
                    WEAPONS_UPDATE_TRIGGER_SET_STATE(weapon_index, trigger_index, 0, 0);
                }
            }
            break;
        }
        }

        if (trigger->state != 0 || trigger_is_down || trigger_is_held)
            result = true;

        if (trigger->charging_effect_index != -1) {
            float charged_fraction = weapon_trigger_get_charged_fraction(weapon_index, trigger_index);
            effect_ping(trigger->charging_effect_index, trigger_definition->charging_effect, charged_fraction, 0.0f, false);
        }
    }

    for (short barrel_index = 0; barrel_index < definition->barrel_count; barrel_index++) {
        s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
        s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
        bool barrel_firing = false;

        if (barrel->ejection_port > 0.0f) {
            barrel->ejection_port -= barrel_definition->ejection_port_recovery_rate * game_tick_length();
            if (barrel->ejection_port <= 0.0f)
                barrel->ejection_port = 0.0f;
            result = true;
        }

        if (barrel->illumination_fraction > 0.0f) {
            barrel->illumination_fraction -= barrel_definition->illumination_recovery_rate * game_tick_length();
            if (barrel->illumination_fraction <= 0.0f)
                barrel->illumination_fraction = 0.0f;
            result = true;
        }

        if (barrel->ticks > 0) {
            barrel->ticks--;
            result = true;
        }

        if (barrel->state_1 == 0 && barrel->recovery_delay_ticks != 0) {
            barrel->recovery_delay_ticks--;
            result = true;
            if (barrel->recovery_delay_ticks == 0)
                weapon_barrel_recover(weapon_index, barrel_index, false);
        }

        switch (barrel->state_1) {
        case 0:
            SET_BIT(barrel->flags, 7, false);
            if (weapon_disabled) {
                SET_BIT(barrel->flags, 0, false);
            } else if ((barrel->flags & 1) != 0) {
                bool blocked = false;
                if (bit_test(barrel_definition->flags, 9) || bit_test(barrel_definition->flags, 10)) {
                    if (definition->barrel_count > 1) {
                        short other_barrel_index = barrel_index == 0;
                        s_weapon_barrel *other_barrel = &weapon->barrels[other_barrel_index];
                        if (other_barrel->state_1 == 1 && bit_test(barrel_definition->flags, 9))
                            blocked = true;
                        if (other_barrel->state_1 == 4 && bit_test(barrel_definition->flags, 10))
                            blocked = true;
                    }
                }
                if (!blocked)
                    barrel->state_1 = 1;
                result = true;
            } else if (weapon_barrel_check_for_reload(weapon_index, barrel_index)) {
                result = true;
            }
            if (barrel->state_1 != 1)
                break;

        case 1:
        {
            bool recover = false;
            bool exhausted = false;
            bool flag = false;
            if (weapon_disabled) {
                weapon_barrel_recover(weapon_index, barrel_index, flag);
            } else if (weapon_barrel_can_fire_this_tick(weapon_index, barrel_index, &recover, &exhausted, &flag)) {
                if (recover && (barrel->flags & 1) == 0) {
                    weapon_barrel_recover(weapon_index, barrel_index, flag);
                } else {
                    barrel_firing = true;
                    SET_BIT(barrel->flags, 0, false);
                    weapon_barrel_fire(weapon_index, barrel_index, false);
                }
            } else if (exhausted) {
                weapon_barrel_recover(weapon_index, barrel_index, flag);
            } else if (recover && (barrel->flags & 1) == 0) {
                weapon_barrel_recover(weapon_index, barrel_index, flag);
            } else {
                barrel_firing = true;
                barrel->active = (char)(barrel->active + 1 > 127 ? 127 : barrel->active + 1);
            }
            break;
        }

        case 3:
            if (weapon_barrel_check_for_reload(weapon_index, barrel_index))
                result = true;

        case 2:
            if (barrel->ticks == 0)
                weapon_barrel_finish_recover(weapon_index, barrel_index);
            break;

        case 4:
            if (barrel->ticks == 0)
                weapons_barrel_idle(weapon_index, barrel_index);
            break;
        }

        if (barrel->state_1 != 0 || barrel->ticks > 0)
            result = true;

        if (barrel_firing) {
            barrel->rate_fraction += barrel_definition->rate_of_fire_acceleration * game_tick_length();
            if (barrel->rate_fraction > 1.0f)
                barrel->rate_fraction = 1.0f;
            if (barrel_definition->firing_effect_threshold != 0.0f && (barrel->flags & 8) == 0 &&
                barrel->rate_fraction > barrel_definition->firing_effect_threshold)
            {
                long effect_object_index = weapon_get_effect_object_index(weapon_index);
                object_set_model_state_property_per_region(effect_object_index, -1, 0, -1, false);
                SET_BIT(barrel->flags, 3, true);
            }
        } else {
            if (barrel->rate_fraction > 0.0f) {
                barrel->rate_fraction -= barrel_definition->rate_of_fire_deceleration * game_tick_length();
                if (barrel->rate_fraction < 0.0f)
                    barrel->rate_fraction = 0.0f;
                result = true;
            }
            if ((barrel->flags & 8) != 0 && barrel->rate_fraction < barrel_definition->firing_effect_threshold) {
                long effect_object_index = weapon_get_effect_object_index(weapon_index);
                object_set_model_state_property_per_region(effect_object_index, -1, 0, 0, false);
                SET_BIT(barrel->flags, 3, false);
            }
        }

        if (barrel->rate_fraction > 0.0f) {
            float current_noise = (float)weapon->analog_value / 255.0f;
            float barrel_noise = barrel->rate_fraction / 7.0f;
            if (barrel_definition->firing_noise_scale > 0.0f)
                barrel_noise = barrel_definition->firing_noise_scale * barrel_noise;
            current_noise += barrel_noise;
            weapon->analog_value = (unsigned char)(unsigned long)(current_noise * 255.0f);
        }

        if (barrel_firing) {
            weapon_barrel_add_fire_error(weapon_index, barrel_index);
            result = true;
        } else if (barrel->firing_error_fraction > 0.0f) {
            float decay_rate = dual_error_theta * barrel_definition->dual_error_decay_rate + (1.0f - dual_error_theta) * barrel_definition->error_decay_rate_b;
            barrel->firing_error_fraction -= decay_rate * game_tick_length();
            barrel->firing_error_fraction = MAX(barrel->firing_error_fraction, 0.0f);
            result = true;
        }

        if (!barrel_firing && barrel->error > 0.0f) {
            barrel->error -= barrel_definition->error_decay_rate * game_tick_length();
            barrel->error = MAX(barrel->error, 0.0);
            result = true;
        }
    }

    return result;
}

bool weapon_is_dual_wielded(long object_index)
{
  bool result = false;
  weapon_datum *object = weapon_get(object_index);
  // match: 0x100748: volatile parent reads keep the two seat lookups from sharing one parent load.
  const volatile datum_index &parent_object_index = object->item.object.parent_object_index;
  long parent_index = parent_object_index;
  if (parent_index != -1) {
    weapon_datum *parent = weapon_get(parent_index);
    if ((1 << parent->item.object.object_type) & 0x1003) {
      bool has_seats = false;
      if ((unsigned char)((unit_datum *)parent)->unit.current_weapon_indices[0] != 0xff
          && (unsigned char)((unit_datum *)parent)->unit.current_weapon_indices[1] != 0xff)
        has_seats = true;
      result = has_seats && (object_index == seat_object(parent_object_index, 0)
          || object_index == seat_object(parent_object_index, 1));
    }
  }
  return result;
}

void weapon_barrel_recover(long weapon_index, short barrel_index, bool flag)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
    float recovery_ticks = game_seconds_to_ticks_real(barrel_definition->recovery_time);
    float unrecovered_ticks = (1.0f - barrel_definition->recovery_fraction) * recovery_ticks;
    long whole_ticks = (long)real_floor(recovery_ticks);
    long ticks = (long)real_floor(unrecovered_ticks);
    barrel->recovery_fraction += recovery_ticks - (float)whole_ticks;
    if (barrel->recovery_fraction >= 1.0f) {
        float carry = real_floor(barrel->recovery_fraction);
        barrel->recovery_fraction -= carry;
        ticks += (long)carry;
    }
    barrel->state_1 = (flag != 0) + 2;
    barrel->ticks = (unsigned short)ticks;
    barrel->recovery_delay_ticks = 0;
}

long weapon_get_owner_unit_inventory_index(long weapon_index)
{
    long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
    long owner_unit_inventory_index = -1;
    if (owner_unit_index != -1) {
        weapon_datum *unit = weapon_get(owner_unit_index);
        for (long inventory_index = 0; inventory_index < 4; ++inventory_index) {
            if (((unit_datum *)unit)->unit.weapon_object_indices[inventory_index] == weapon_index) {
                owner_unit_inventory_index = inventory_index;
                break;
            }
        }
    }
    return owner_unit_inventory_index;
}

// 0x12bb80
short weapon_rotate_zoom_level(long weapon_index, short zoom_level)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    if (!weapon_reloading(weapon_index, -1, 0)) {
        if (zoom_level >= 0 && zoom_level < definition->zoom_level_count - 1)
            zoom_level++;
        else if (zoom_level == definition->zoom_level_count - 1)
            zoom_level = -1;
        else
            zoom_level = 0;
    }
    return zoom_level;
}

static const float weapons_interface_infinite_age = 3.402823466e38f;

static __forceinline s_weapon_magazine *weapons_interface_get_magazine(weapon_datum *weapon, short magazine_index)
{
    return &weapon->magazines[magazine_index];
}
static __forceinline bool weapons_interface_magazine_is_busy(long weapon_index, short magazine_index)
{
    s_weapon_magazine *magazine = weapons_interface_get_magazine(weapon_get(weapon_index), magazine_index);
    bool result = false;
    switch (magazine->state) {
    case 1:
    case 2:
    case 3:
    case 4:
        result = true;
        break;
    case 5:
    case 6:
        result = true;
    }
    return result;
}
static __forceinline long weapons_interface_get_rounds_loaded(long weapon_index, short magazine_index)
{
    return weapons_interface_get_magazine(weapon_get(weapon_index), magazine_index)->rounds_loaded;
}

// 0x12fd70
// @address 0x12fd70 ?weapon_build_weapon_interface_state@@YAXJPAUweapon_interface_state@@@Z
// match: 0x12fd70: without __restrict values are re-read after the stores through state (48 bytes longer, frame 0x170 -> 0x160)
void weapon_build_weapon_interface_state(long weapon_index, weapon_interface_state *__restrict state)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    memset(state, 0, sizeof(weapon_interface_state));
    state->heat = weapon->heat;
    state->age = 1.0f - weapon_get_age(weapon_index);
    state->charge = 0.0f;
    state->analog_value = (float)weapon->analog_value * 0.003921569f;
    state->overheated = weapon->weapon_flags.test(1);
    state->control_flag_1 = ((weapon->control_flags >> 1) & 1) != 0;
    if (definition->trigger_count > 0)
        state->charge = ::weapon_trigger_get_charged_fraction(weapon_index, 0);
    state->magazine_count = (short)definition->magazines.s_tag_block::count;
    for (short magazine_index = 0; magazine_index < definition->magazines.s_tag_block::count; magazine_index++) {
        s_weapon_magazine *magazine = weapons_interface_get_magazine(weapon, magazine_index);
        s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, magazine_index);
        state->magazines[magazine_index].active = weapons_interface_magazine_is_busy(weapon_index, magazine_index);
        state->magazines[magazine_index].idle = magazine->state == 0;
        state->magazines[magazine_index].rounds_loaded = (short)weapons_interface_get_rounds_loaded(weapon_index, magazine_index);
        state->magazines[magazine_index].rounds_loaded_maximum = magazine_definition->rounds_loaded_maximum;
        state->magazines[magazine_index].rounds_inventory = (short)weapon_get_rounds_inventory(weapon_index, magazine_index, false);
        state->magazines[magazine_index].rounds_inventory_maximum = magazine_definition->rounds_inventory_maximum;
    }
    if (definition->trigger_count > 0) {
        s_weapon_trigger_definition *trigger_definition = trigger_definition_get(definition, 0);
        s_weapon_trigger *trigger = weapon_get_trigger(weapon, 0);
        bool lock_on = trigger_definition->behavior == 5;
        bool charged_lock_on = trigger_definition->behavior == 1 && definition->tracking_type == 1;
        if (weapon->magazines[0].rounds_loaded > 0 && (lock_on || charged_lock_on)) {
            if (lock_on && trigger->state == 0) {
                long target_index;
                unsigned char model_target_index;
                if (::weapon_tracked_on_something_trackable(weapon_index, (long *)(&target_index), (char *)(&model_target_index)))
                    state->tracking = true;
            }
            else if (trigger->state == 5 || trigger->state == 6) {
                if (weapon->tracked_object_index != -1) {
                    weapon_datum *target = (weapon_datum *)object_try_and_get_and_verify_type(weapon->tracked_object_index, 0xffffffff);
                    if (target && weapon->current_state_ticks != -1) {
                        s_object_definition *target_definition =
                            TAG_GET('obje', s_object_definition, target->definition_index);
                        long model_index = target_definition->model.index;
                        if (model_index != -1) {
                            s_model_definition *model =
                                TAG_GET('hlmt', s_model_definition, model_index);
                            if (weapon->current_state_ticks >= 0 && weapon->current_state_ticks < model->targets.count) {
                                object_marker marker;
                                if (object_get_markers_by_string_id(
                                        weapon->tracked_object_index,
                                            ((s_model_target *)model->targets.address)[weapon->current_state_ticks].marker_name, &marker, 1) == 1)
                                    state->target_position = marker.matrix.position;
                            }
                        }
                    }
                }
                if (trigger->state == 5) {
                    long lock_ticks = game_seconds_to_ticks_round(trigger_definition->locked_time);
                    state->locking = true;
                    if (lock_ticks != 0)
                        state->lock_fraction = 1.0f - (float)trigger->timer / (float)lock_ticks;
                    else
                        state->lock_fraction = 0.0f;
                }
                else {
                    state->locked = true;
                }
            }
        }
    }
    if (weapon_is_charged_plasma_pistol(weapon_index, false))
        state->charged_plasma_pistol = true;
    if (::game_engine_player_has_infinite_ammo(weapon_index)) {
        state->age = weapons_interface_infinite_age;
        state->magazines[0].rounds_inventory = -1;
    }
}

bool weapons_aim_assist_targeting_is_valid_for_plasma_pistol_lock(const s_aim_assist_targeting_result *reference)
{
    bool result = false;
    if (reference->object_index != -1) {
        weapon_datum *object = (weapon_datum *)object_try_and_get_and_verify_type(reference->object_index, 0xffffffff);
        if (object) {
            bool alive = object->item.object.shield_vitality > k_real_zero_storage && object->item.object.maximum_shield_vitality > k_real_zero_storage;
            s_object_definition *definition =
                TAG_GET('obje', s_object_definition, object->definition_index);
            if ((1 << object->item.object.object_type) & 1) {
                long flags = ((biped_definition *)definition)->autoaim_flags;
                if ((flags & 2) && alive || (flags & 4))
                    result = true;
            }
            long child_index = definition->model.index;
            if (child_index != -1) {
                long entry_index = reference->entry_index;
                if (entry_index != -1) {
                    s_model_definition *child = TAG_GET('hlmt', s_model_definition, child_index);
                    if (entry_index < child->targets.count) {
                        long flags = ((s_model_target *)child->targets.address)[reference->entry_index].flags;
                        if ((flags & 2) && alive || (flags & 0x10))
                            return true;
                    }
                }
            }
        }
    }
    return result;
}

static __forceinline long weapons_message_get_owner_unit_index(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    long result = -1;
    if (weapon->item.owner_relation)
        result = weapon->item.unit_object_index;
    return result;
}

static __forceinline long weapons_message_unit_inventory_get_weapon(long unit_index, short inventory_index)
{
    weapon_datum *unit = weapon_get(unit_index);
    long result = -1;
    signed char weapon_slot = ((unit_datum *)unit)->unit.current_weapon_indices[inventory_index];
    if (weapon_slot != -1)
        result = ((unit_datum *)unit)->unit.weapon_object_indices[weapon_slot];
    return result;
}

// 0x1302f0
void weapon_send_message_to_unit(long weapon_index, e_unit_messages message)
{
    long unit_index = weapons_message_get_owner_unit_index(weapon_index);
    if (unit_index != -1) {
        long parameter = weapon_index - weapons_message_unit_inventory_get_weapon(unit_index, 0) != 0;
        unit_handle_message_from_weapon(unit_index, weapon_index, message, parameter);
    }
}

long weapon_get_rounds_inventory(long datum_index, long slot, bool exclude_linked)
{
    weapon_datum *self = (weapon_datum *)object_get(datum_index);
    long total = self->magazines[slot].rounds_inventory;
    long parent = self->item.unit_object_index;
    if (parent != -1 && exclude_linked == 0) {
        unsigned int count = 4;
        long *children = ((unit_datum *)object_get(parent))->unit.weapon_object_indices;
        do {
            long child = *children;
            if (child != -1 && child != datum_index) {
                weapon_datum *other = (weapon_datum *)object_get(child);
                if (other->definition_index == self->definition_index) {
                    total = other->magazines[slot].rounds_inventory + total;
                }
            }
            count = count - 1;
            children = children + 1;
        } while (count != 0);
    }
    return total;
}


// 0x3497f8: the animation manager weapon_setup_first_person_emulation keeps on its stack
c_animation_manager::~c_animation_manager()
{
    weapons_inl_channel_destroy(&m_transition_channel);
    weapons_inl_channel_destroy(&m_state_channel);
}

bool weapon_destroy_barrel(long weapon_index, long barrel_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool result = false;
    if (definition->barrel_count > barrel_index) {
        s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
        barrel->flags |= 4;
        result = true;
    }
    return result;
}

bool weapon_damage_barrel(long weapon_index, long barrel_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool result = false;
    if (definition->barrel_count > barrel_index) {
        s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
        barrel->flags |= 32;
        result = true;
    }
    return result;
}

// 0x34aa38

bool weapon_take_inventory_rounds(long weapon_index, long magazine_index, long round_count)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    long rounds_available = weapon_get_rounds_inventory(weapon_index, magazine_index, false);
    bool result = false;
    if (rounds_available >= round_count) {
        long total_rounds_taken = 0;
        long weapon_indices[4];
        long weapon_rounds[4];
        memset(weapon_indices, -1, sizeof(weapon_indices));
        memset(weapon_rounds, 0, sizeof(weapon_rounds));
        if (!::game_engine_player_has_infinite_ammo(weapon_index)) {
            weapon_definition *definition = weapon_definition_get(weapon->definition_index);
            if (magazine_index >= 0 && magazine_index < definition->magazines.s_tag_block::count) {
                s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
                long rounds_taken = round_count > magazine->rounds_inventory ? magazine->rounds_inventory : round_count;
                magazine->rounds_inventory -= rounds_taken;
                total_rounds_taken += rounds_taken;
                simulation_action_weapon_state_update(weapon_index);
            }
        }
        if (total_rounds_taken > 0)
            object_wake(weapon_index);
        if (weapon->item.unit_object_index != -1 && total_rounds_taken < round_count) {
            unit_datum *unit = (unit_datum *)weapon_get(weapon->item.unit_object_index);
            for (long weapon_slot = 0; weapon_slot < 4; weapon_slot++) {
                long other_weapon_index = unit->unit.weapon_object_indices[weapon_slot];
                if (other_weapon_index != -1 && other_weapon_index != weapon_index) {
                    weapon_datum *other_weapon = weapon_get(other_weapon_index);
                    if (other_weapon->definition_index == weapon->definition_index) {
                        s_weapon_magazine *magazine = &other_weapon->magazines[magazine_index];
                        long rounds_taken = round_count - total_rounds_taken > magazine->rounds_inventory
                            ? magazine->rounds_inventory
                            : round_count - total_rounds_taken;
                        if (rounds_taken > 0) {
                            magazine->rounds_inventory -= rounds_taken;
                            total_rounds_taken += rounds_taken;
                            weapon_indices[weapon_slot] = other_weapon_index;
                            weapon_rounds[weapon_slot] = rounds_taken;
                            simulation_action_weapon_state_update(other_weapon_index);
                        }
                    }
                }
            }
        }
        result = true;
    }
    return result;
}


static __forceinline bool game_simulation_predicted(void)
{
    bool result = false;
    if (tls_game_globals()->options.simulation_type == 4)
        result = true;
    return result;
}

// @address 0x34ac10 weapon_set_state_update_timer
void weapon_set_state_update_timer(long weapon_index)
{
    if (game_simulation_predicted()) {
        weapon_datum *weapon = weapon_get(weapon_index);
        long ticks = (long)((float)game_time_globals->ticks_per_second * g_network_configuration.weapon_ticks_seconds + 0.5f);
        if (ticks >= 255)
            ticks = 255;
        weapon->prediction_ticks = (unsigned char)ticks;
    }
}

bool weapon_is_zoomable(long index)
{
  s_object_header *header = object_header_get(index);
  weapon_datum *object = (weapon_datum *)header->datum;
  long handle = object->definition_index;
  weapon_definition *definition = weapon_definition_get(handle);
  return definition->zoom_level_count > 0;
}

bool weapon_counts_towards_inventory_limit(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    return !definition->m_flags.test(_weapon_definition_does_not_count_towards_inventory_limit) && !definition->m_flags.test(_weapon_definition_support_weapon);
}

bool weapon_uses_charging_melee(long weapon_index)
{
    bool result = false;
    if (weapon_index != -1) {
        weapon_datum *weapon = weapon_get(weapon_index);
        weapon_definition *definition = weapon_definition_get(weapon->definition_index);
        if (definition->trigger_count > 0) {
            s_weapon_trigger_definition *trigger = trigger_definition_get(definition, 0);
            result = trigger->behavior == 7;
        }
    }
    return result;
}

// 0x34b050 (no dev name)
extern "C" void weapon_interrupt_reload(long weapon_index, bool reload_magazines)
{
    if (weapon_reloading(weapon_index, -1, 0)) {
        weapon_datum *weapon = weapon_get(weapon_index);
        weapon_definition *definition = weapon_definition_get(weapon->definition_index);
        if (reload_magazines) {
            for (short magazine_index = 0; magazine_index < definition->magazines.s_tag_block::count; magazine_index++) {
                s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
                if (magazine->state == 1 && magazine->fire_id >= 0) {
                    weapon_magazine_update_reload(weapon_index, magazine_index);
                    weapon_magazine_finish_reload(weapon_index, magazine_index, true);
                }
            }
        }
        weapon_stop_first_person_sound(weapon_index, 12, true);
        weapon_reset(weapon_index);
        weapon_send_message_to_unit(weapon_index, _unit_message_idle);
    }
}

bool weapon_can_be_dual_wielded(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool result = definition->m_flags.test(_weapon_definition_can_be_dual_wielded) || definition->m_flags.test(_weapon_definition_can_only_be_dual_wielded);
    return result;
}

void weapon_set_initial_rounds(long weapon_index, short magazine_index, short loaded_rounds, short total_rounds)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    if (definition->magazines.s_tag_block::count > 0) {
        s_weapon_magazine_definition *magazine = magazine_definition_get(definition, 0);
        short loaded = loaded_rounds < 0 ? 0 : loaded_rounds;
        short inventory = total_rounds - loaded < 0 ? 0 : total_rounds - loaded;
        weapon->magazines[0].rounds_loaded = loaded > magazine->rounds_loaded_maximum ? magazine->rounds_loaded_maximum : loaded;
        weapon->magazines[0].rounds_inventory = inventory > magazine->rounds_inventory_maximum ? magazine->rounds_inventory_maximum : inventory;
    }
}

long weapon_get_owner_unit_index(long weapon_index)
{
    return weapon_owner_unit_index_get(weapon_index);
}

long weapon_barrel_get_marker_name(long weapon_index, long barrel_index)
{
    long result = 0;
    if ((unsigned long)barrel_index <= 1) {
        result = 202 + (barrel_index != 0);
        weapon_datum *weapon = weapon_get(weapon_index);
        weapon_definition *definition = weapon_definition_get(weapon->definition_index);
        if (barrel_index < definition->barrel_count) {
            s_weapon_barrel_definition *barrel = &definition->barrels[barrel_index];
            if (barrel->marker_name != 0 && barrel->marker_name != -1)
                result = barrel->marker_name;
        }
    }
    return result;
}

bool weapon_get_trigger_position(long weapon_index, short barrel_index, real_point3d *gun_position)
{
    bool result = false;
    object_marker markers[64];
    long object_index = weapon_get_effect_object_index(weapon_index);
    long marker_name = weapon_barrel_get_marker_name(weapon_index, barrel_index);
    short marker_count = object_get_markers_by_string_id(object_index, marker_name, markers, 64);
    if (marker_count > 0)
    {
        *gun_position = *global_origin3d;
        for (short marker_index = 0; marker_index < marker_count; marker_index++)
        {
            const real_point3d *marker_position = &markers[marker_index].matrix.position;
            gun_position->x = marker_position->x + gun_position->x;
            gun_position->y = marker_position->y + gun_position->y;
            gun_position->z = marker_position->z + gun_position->z;
        }
        float scale = 1.0f / (float)marker_count;
        gun_position->x = scale * gun_position->x;
        gun_position->y = scale * gun_position->y;
        gun_position->z = scale * gun_position->z;
        result = true;
    }
    return result;
}

bool weapon_is_age_based(long definition_index)
{
    bool result = false;
    weapon_definition *definition = weapon_definition_get(definition_index);
    if (definition->magazines.s_tag_block::count == 0) {
        result = true;
    } else {
        for (short barrel_index = 0; barrel_index < definition->barrel_count; barrel_index++) {
            s_weapon_barrel_definition *barrel = &definition->barrels[barrel_index];
            if (barrel->amount_a > 0.0f || barrel->amount_b > 0.0f) {
                result = true;
                break;
            }
        }
    }
    return result;
}

// @address 0x34b940 ?weapon_age@@YAXJM@Z
void weapon_age(long weapon_index, float age_delta)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool infinite_ammo;
    if (::game_engine_player_has_infinite_ammo(weapon_index))
        infinite_ammo = true;
    else
        infinite_ammo = false;
    if ((weapon->item.inventory_flags & 2) != 0 && weapon->age < 1.0f && !infinite_ammo)
    {
        weapon->age += age_delta;
        if (!(weapon->age + 0.0001f < 1.0f))
        {
            weapon->age = 1.0f;
            weapon_effect_new(weapon_index, -1, definition->depleted_effect, 0.0f, 0.0f);
            long effect_object_index = weapon_get_effect_object_index(weapon_index);
            object_set_model_state_property_per_region(effect_object_index, -1, 3, -1, false);
        }
        weapon_set_state_update_timer(weapon_index);
        simulation_action_weapon_state_update(weapon_index);
    }
}

long weapon_get_effect_object_index(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    weapon_datum *object = (weapon_datum *)header->datum;
    s_object_definition *definition = TAG_GET('obje', s_object_definition, object->definition_index);
    long result = object_index;
    bool hidden = (header->flags & 16) != 0 || (object->item.object.flags & 1) != 0;
    if (hidden || definition->model.index == -1) {
        // match: 0x34ba40: volatile parent read preserves the inlined effect-owner load.
        const volatile datum_index &parent_object_index = object->item.object.parent_object_index;
        long parent = parent_object_index;
        if (parent != -1)
            result = parent;
    }
    return result;
}

void weapon_barrel_set_fire_bit(long weapon_index, short barrel_index, bool set)
{
    if (barrel_index != -1) {
        weapon_datum *weapon = weapon_get(weapon_index);
        s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
        if (set)
            barrel->flags |= 1;
        else
            barrel->flags &= 0xfffe;
    }
}

void weapon_barrel_clear_empty_click_bit(long weapon_index, short barrel_index)
{
    if (barrel_index >= 0 && barrel_index < 2) {
        weapon_datum *weapon = weapon_get(weapon_index);
        s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
        if (barrel->state_1 != 1)
            barrel->flags &= 0xffbf;
    }
}


// @address 0x34bb00 ?weapon_get_projectile_owner_unit_index@@YAJJ@Z
long weapon_get_projectile_owner_unit_index(long weapon_index)
{
    s_object_header *object_headers = (s_object_header *)object_header_data->data;
    weapon_datum *weapon = (weapon_datum *)object_headers[datum_index_to_absolute_index(weapon_index)].datum;
    int result = -1;
    // match: 0xfe428: volatile owner read (with the weapon_state read) keeps weapon_update's add at 0x820feec4 as r27,r10,r29 (r27,r29,r10 without it).
    const volatile datum_index &parent_object_index = weapon->item.object.parent_object_index;
    int owner_index = parent_object_index;
    if (owner_index != -1) {
        unit_datum *owner = (unit_datum *)object_try_and_get_and_verify_type(owner_index, 4099);
        if (owner) {
            unit_definition *definition = TAG_GET('unit', unit_definition, owner->object.definition_index);
            if (bit_test(definition->flags, 26) &&
                owner->object.parent_object_index != -1 &&
                (1 << object_headers[datum_index_to_absolute_index(owner->object.parent_object_index)].object_type & 4099) != 0) {
                unit_datum *parent = (unit_datum *)object_headers[datum_index_to_absolute_index(owner->object.parent_object_index)].datum;
                result = owner->object.parent_object_index;
                if (parent->unit.gunner_object_index != -1)
                    result = parent->unit.gunner_object_index;
            } else {
                result = owner_index;
                if (owner->unit.gunner_object_index != -1)
                    result = owner->unit.gunner_object_index;
            }
        }
    }
    return result;
}

const float weapons_timer_real_one = 1.0f;
const float weapons_timer_real_zero = 0.0f;


// @address 0x34bc18 ?weapon_trigger_get_charged_fraction@@YAMJF@Z
float weapon_trigger_get_charged_fraction(long object_index, short timer_index)
{
  weapon_datum *object = weapon_get(object_index);
  weapon_definition *definition = weapon_definition_get(object->definition_index);
  float result = weapons_timer_real_zero;
  if (definition->trigger_count > timer_index) {
    s_weapon_trigger *timer = &object->triggers[timer_index];
    s_weapon_trigger_definition *timer_definitions = definition->triggers;
    switch (timer->state) {
    case 1:
      result = weapons_timer_real_one - (game_time_globals->tick_length * (float)timer->timer) / timer_definitions[timer_index].charging_time;
      break;
    case 2:
      result = weapons_timer_real_one;
      break;
    }
  }
  return result;
}


static __forceinline bool weapons_barrel_is_busy(long weapon_index, short barrel_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
    bool result = barrel->state_1 == 4 || barrel->state_1 == 2 || barrel->state_1 == 3;
    return result;
}

// match: 0x34bf00: without __restrict the bool stores re-read the weapon datum (extra lwz 0x44/0xc) and save one more GPR (r21)
bool weapon_barrel_can_fire_this_tick(long weapon_index, short barrel_index, bool *__restrict fire, bool *__restrict exhausted, bool *__restrict blocked)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    bool can_fire = false;
    bool waiting = false;
    float rate_fraction = barrel->rate_fraction;
    float rounds_per_second = barrel_definition->rounds_per_second_lower +
        rate_fraction * (barrel_definition->rounds_per_second_upper - barrel_definition->rounds_per_second_lower);
    float ticks_per_round = rounds_per_second > 0.0001f ? (float)game_time_globals->ticks_per_second / rounds_per_second : 0.0f;
    if (definition->heat_rate_scale > 0.0f)
        ticks_per_round = (1.0f + weapon->age * definition->heat_rate_scale) * ticks_per_round;
    if ((bit_test(barrel_definition->flags, 9) || bit_test(barrel_definition->flags, 10)) && definition->barrel_count > 1) {
        short other_barrel_index = barrel_index == 0;
        s_weapon_barrel *other_barrel = weapon_barrel_get(weapon, other_barrel_index);
        if (other_barrel->state_1 == 1 && bit_test(barrel_definition->flags, 9))
            waiting = true;
        if (weapons_barrel_is_busy(weapon_index, other_barrel_index) && bit_test(barrel_definition->flags, 10))
            waiting = true;
    }
    if (!waiting && (barrel->value == 0 || (float)barrel->active + 1.0f >= ticks_per_round))
        can_fire = true;
    if (barrel_definition->shots_minimum != 0 && barrel->value < barrel_definition->shots_minimum) {
        *fire = false;
        *exhausted = false;
    } else if (barrel_definition->shots_maximum != 0 && barrel->value >= barrel_definition->shots_maximum) {
        *fire = false;
        *exhausted = true;
        can_fire = false;
    } else {
        *fire = true;
        *exhausted = false;
    }
    if (barrel->flags & 64) {
        can_fire = false;
        *exhausted = true;
        *blocked = true;
    }
    if (can_fire && bit_test(weapon->control_flags, 6)) {
        can_fire = false;
        *fire = true;
        *exhausted = false;
    }
    if (can_fire) {
        long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
        if (owner_unit_index != -1) {
            long primary_weapon_index = seat_object(owner_unit_index, 0);
            long secondary_weapon_index = seat_object(owner_unit_index, 1);
            long partner_weapon_index = -1;
            if (weapon_index == primary_weapon_index)
                partner_weapon_index = secondary_weapon_index;
            if (weapon_index == secondary_weapon_index)
                partner_weapon_index = primary_weapon_index;
            if (partner_weapon_index != -1) {
                weapon_datum *partner = weapon_get(partner_weapon_index);
                if (partner->definition_index == weapon->definition_index) {
                    s_weapon_barrel *partner_barrel = weapon_barrel_get(partner, barrel_index);
                    if (partner_barrel->state_1 == 1 && partner_barrel->active == 0)
                        can_fire = false;
                }
            }
        }
    }
    if (can_fire && ticks_per_round < 1.0f && ticks_per_round > 0.0001f)
        barrel->fire_time = barrel->fire_time + 1.0f / ticks_per_round;
    return can_fire;
}

static __forceinline s_weapon_magazine *weapons_message_get_magazine(weapon_datum *weapon, short magazine_index)
{
    return &weapon->magazines[magazine_index];
}

// 0x34cd38
void weapon_magazine_finish_reload(long weapon_index, short magazine_index, bool interrupted)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    s_weapon_magazine *magazine = weapons_message_get_magazine(weapon, magazine_index);
    if (!interrupted && magazine->state == 3)
        weapon_send_message_to_unit(weapon_index, _unit_message_reload_finished);
    else if (interrupted || magazine->state != 4)
        weapon_send_message_to_unit(weapon_index, _unit_message_idle);
    ::weapon_magazine_set_state(weapon_index, magazine_index, 5);
}

// @address 0x34cf10 weapon_try_and_get_magazine_value
bool weapon_try_and_get_magazine_value(long object_index, short slot_index, short *value)
{
    bool result = false;
    short slot_value = -1;
    weapon_datum *object = (weapon_datum *)weapon_get(object_index);
    s_weapon_magazine *slot = &object->magazines[slot_index];
    switch (slot->state) {
    case 1:
    case 2:
    case 3:
        slot_value = slot->fire_id;
    case 4:
    case 5:
    case 6:
        result = true;
    }
    if (value != 0)
        *value = slot_value;
    return result;
}


// @address 0x351028 ?weapon_owner_autoaimed_on_lockable_target@@YA_NJPAJPAD@Z
// match: 0x351028: without __restrict the function grows 8 bytes and the header-lookup rlwinm moves ahead of li -1
bool weapon_owner_autoaimed_on_lockable_target(long index, long * __restrict target_index, char * __restrict model_target_index)
{
    unit_datum *unit = (unit_datum *)object_get(index);
    const s_aim_assist_targeting_result *aim = &unit->unit.targeting;
    bool result = false;
    *target_index = -1;
    *model_target_index = -1;
    if (aim->target_object_index != -1 && aim->primary_autoaim_level >= 1.0f) {
        unit_datum *object = (unit_datum *)object_try_and_get_unsafe_and_verify_type(aim->target_object_index, ~0UL);
        if (object && aim->model_target != -1) {
            const s_object_definition *def = TAG_GET('obje', const s_object_definition, object->object.definition_index);
            long model_index = def->model.index;
            if (model_index != -1) {
                const s_model_definition *m = TAG_GET('hlmt', const s_model_definition, model_index);
                if (aim->model_target >= 0 && aim->model_target < m->targets.count) {
                    const s_model_target *t = &((s_model_target *)m->targets.address)[aim->model_target];
                    if (t->flags & 1) {
                        result = true;
                        *target_index = aim->target_object_index;
                        *model_target_index = (char)aim->model_target;
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x351120 ?weapon_tracked_on_something_trackable@@YA_NJPAJPAD@Z
// match: 0xfe428: inlined into weapon_update; without __restrict its frame shrinks 0x190 -> 0x180 and 52 stack-slot offsets move
bool weapon_tracked_on_something_trackable(long weapon_index, long * __restrict tracked_target_index, char * __restrict tracked_model_target_index)
{
    bool result = false;
    game_time_globals_definition *globals = game_time_globals;
    weapon_datum *object = weapon_get(weapon_index);
    if (globals->time - object->target_time < (long)((float)globals->ticks_per_second + 0.5f)) {
        long target_index = object->tracked_object_index;
        if (target_index != -1 && object_try_and_get_unsafe_and_verify_type(target_index, ~0UL) != 0) {
            unsigned char state = object->current_state_ticks;
            *tracked_target_index = target_index;
            *((unsigned char *)tracked_model_target_index) = state;
            result = true;
        }
    }
    return result;
}

bool weapon_is_charged_plasma_pistol(long weapon_index, bool any_state)
{
    bool result = false;
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    if (definition->trigger_count > 0) {
        s_weapon_trigger_definition *trigger = trigger_definition_get(definition, 0);
        if ((any_state || weapon->triggers[0].state == 2) && trigger->behavior == 2 && trigger->secondary_action == 1)
            result = true;
    }
    return result;
}


/* natural 0x351268: the object's value at 0x190, or 0 when game_engine_player_has_infinite_ammo holds for it */
// @address 0x351268 ?weapon_get_age@@YAMJ@Z
float weapon_get_age(long object_index)
{
    if (!::game_engine_player_has_infinite_ammo(object_index))
        return weapon_get(object_index)->age;
    return 0.0f;
}



struct effect_vector_field;

// @address 0x34e188 ?weapon_effect_new@@YAJJJJMM@Z
long weapon_effect_new(long weapon_index, long old_effect_index, long definition_index, float scale_a, float scale_b)
{
    long effect_index = -1;
    if (definition_index != -1)
    {
        long effect_object_index = weapon_get_effect_object_index(weapon_index);
        long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
        switch (tag_get_group_tag(definition_index))
        {
        case 'effe':
            effect_index = old_effect_index;
            if (!effect_ping(old_effect_index, definition_index, scale_a, scale_b, true))
            {
                long effect_storage;
                c_tag_index *effect = new (&effect_storage) c_tag_index(definition_index);
                effect_index = effect_new_from_object(*effect, 0, 0, effect_object_index, scale_a, scale_b, 0, 0, 1);
            }
            break;
        case 'snd!':
            object_impulse_sound_new(owner_unit_index, definition_index, -1, global_origin3d, global_forward3d, scale_a);
            break;
        }
    }
    return effect_index;
}


bool weapon_trigger_can_begin_charging(long weapon_index, short trigger_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_trigger_definition *trigger_definition = trigger_definition_get(definition, trigger_index);
    bool result = true;
    if (trigger_definition->barrel_index != -1 && trigger_definition->barrel_index < definition->barrel_count)
    {
        s_weapon_barrel *barrel = weapon_barrel_get(weapon, trigger_definition->barrel_index);
        if (barrel->state_1 != 0)
            result = false;
    }
    if (weapon_has_infinite_ammo(weapon_index))
        result = false;
    return result;
}

// @address 0x34e840 ?weapon_barrel_idle@@YAXJF@Z
void weapon_barrel_idle(long weapon_index, short barrel_index)
{
    weapon_datum *object = (weapon_datum *)object_header_get(weapon_index)->datum;
    s_weapon_barrel_definition *definitions = weapon_definition_get(object->definition_index)->barrels;
    s_weapon_barrel *slot = &object->barrels[barrel_index];
    s_weapon_barrel_definition *sd = &definitions[barrel_index];
    slot->active = 0;
    slot->value = 0;
    slot->state_1 = 0;
    if (!(unsigned char)((sd->flags >> 11) & 1))
        weapon_barrel_set_fire_bit(weapon_index, barrel_index, false);
}

static __forceinline float weapons_inl_seconds_to_ticks(float seconds)
{
    return (float)game_time_globals->ticks_per_second * seconds;
}

void weapon_barrel_finish_recover(long weapon_index, short barrel_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
    float recovery_ticks_real = weapons_inl_seconds_to_ticks(barrel_definition->recovery_time);
    long recovery_time = (long)floor(recovery_ticks_real);
    float idle_ticks = weapons_inl_seconds_to_ticks(barrel_definition->recovery_time * (1.0f - barrel_definition->recovery_fraction));
    long idle_time = (long)floor(idle_ticks);
    long remaining_time = recovery_time - idle_time;
    barrel->state_1 = 4;
    barrel->ticks = (short)remaining_time;
    if (remaining_time == 0)
        weapon_barrel_idle(weapon_index, barrel_index);
}




void projectile_distribute(real_vector3d *vector, real_vector3d *axis, short distribution_function, float spread_angle,
    short projectile_index, short projectile_count)
{
    float offset;
    if (projectile_count & 1) {
        if (projectile_index == 0) {
            offset = 0.0f;
        } else {
            short index = projectile_index - 1;
            short step = (index & 1) ? (index >> 1) : -(index >> 1);
            offset = (float)step;
        }
    } else {
        short step = projectile_index >> 1;
        offset = (float)step - 0.5f;
        if (projectile_index & 1)
            offset = -offset;
    }
    float angle = offset * spread_angle;
    float angle_cosine = cosine(angle);
    float angle_sine = sine(angle);
    if (distribution_function == 1)
        rotate_vector_about_axis(vector, axis, angle_sine, angle_cosine);
}

void weapon_barrel_start_ejection_port(long weapon_index, short barrel_index, bool flag)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    if (barrel_definition->ejection_port_threshold > 0.0f
        && (bit_test(barrel_definition->flags, 4) && flag
            || !bit_test(barrel_definition->flags, 4) && !flag))
        barrel->ejection_port = 1.0f;
}


static __forceinline long weapons_stowed_marker_name_get(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    return definition->stowed_marker_name;
}

bool weapon_can_be_stowed(long weapon_index, long unit_index)
{
    bool result = false;
    weapon_datum *weapon = weapon_get(weapon_index);
    object_marker marker;
    if (internal_object_get_markers_by_string_id(weapon_index, 40, &marker, 1, false) == 1)
    {
        weapon_definition *definition = weapon_definition_get(weapon->definition_index);
        if (definition->stowed_marker_name != -1 && definition->stowed_marker_name != 0 &&
            internal_object_get_markers_by_string_id(unit_index, definition->stowed_marker_name, &marker, 1, false) == 1)
        {
            result = true;
        }
    }
    if (result)
    {
        unit_datum *unit = (unit_datum *)weapon_get(unit_index);
        if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1)
        {
            weapon_datum *parent = weapon_get(unit->object.parent_object_index);
            unit_definition *parent_definition =
                TAG_GET('unit', unit_definition, parent->definition_index);
            if (bit_test(parent_definition->seats[unit->unit.parent_seat_index].flags, 30))
                return false;
        }
    }
    return result;
}

void weapon_stow(long weapon_index, long unit_index)
{
    long marker_name = weapons_stowed_marker_name_get(weapon_index);
    weapon_datum *weapon = weapon_get(weapon_index);
    if (weapon->item.object.parent_object_index != -1)
        object_detach(weapon_index);
    weapon->item.owner_relation = 3;
    object_attach_to_marker_with_type(unit_index, marker_name, weapon_index, 40);
}

bool weapon_has_infinite_ammo(long weapon_index)
{
    bool result = false;
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    if (weapon_get_age(weapon_index) >= 1.0f) {
        result = true;
        if ((unsigned short)definition->reload_interrupt_mode == 7) {
            bool barrel_active = false;
            bool barrel_heats = false;
            for (short barrel_index = 0; barrel_index < definition->barrel_count; barrel_index++) {
                if (weapon->barrels[barrel_index].value != 0) {
                    barrel_active = true;
                    if (definition->barrels[barrel_index].heat_recovery_threshold > 0.0f)
                        barrel_heats = true;
                }
            }
            if (barrel_active && barrel_heats)
                result = false;
        }
    }
    return result;
}


// @address 0x34c730 ?weapon_trigger_determine_analog_autofire_behavior@@YAXJJPA_N00@Z
void weapon_trigger_determine_analog_autofire_behavior(
    // match: 0x34c730: without __restrict the function grows 12 bytes and saves one fewer GPR (frame 0xc0 -> 0xb0)
    long weapon_index, long trigger_index, bool *__restrict fire, bool *__restrict alt, bool *__restrict clear)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_trigger *trigger = &weapon->triggers[trigger_index];
    s_weapon_trigger_definition *trigger_definition = &definition->triggers[trigger_index];
    long autofire_ticks = (long)(trigger_definition->autofire_time * (float)game_time_globals->ticks_per_second + 0.5f);
    long autofire_threshold = (unsigned char)(255.0f * trigger_definition->autofire_threshold);
    *fire = false;
    *alt = false;
    *clear = false;
    if (weapon->analog_trigger < weapon->previous_analog_trigger)
    {
        if (weapon->trigger_direction == 1)
        {
            long delta = weapon->previous_analog_trigger - weapon->trigger_reference;
            if (delta > 20)
                *fire = true;
            weapon->trigger_direction = -1;
            weapon->trigger_reference = weapon->previous_analog_trigger;
            weapon->trigger_ticks = 0;
        }
    }
    else if (weapon->analog_trigger == weapon->previous_analog_trigger)
    {
        if (weapon->trigger_direction == 1)
        {
            long delta = weapon->analog_trigger - weapon->trigger_reference;
            weapon->trigger_ticks = (signed char)(weapon->trigger_ticks + 1 > 255 ? 255 : weapon->trigger_ticks + 1);
            if (weapon->trigger_ticks >= autofire_ticks)
            {
                if (delta < autofire_threshold && weapon->analog_trigger < 250)
                    *fire = true;
                else
                    *alt = true;
            }
        }
    }
    else
    {
        if (weapon->trigger_direction == -1)
        {
            long delta = weapon->trigger_reference - weapon->previous_analog_trigger;
            if (delta > 20)
                *clear = true;
            weapon->trigger_reference = weapon->previous_analog_trigger;
            weapon->trigger_direction = 1;
            weapon->trigger_ticks = 0;
        }
        else if (weapon->trigger_direction == 0)
        {
            weapon->trigger_reference = 0;
            weapon->trigger_direction = 1;
            weapon->trigger_ticks = 0;
        }
        else
        {
            long delta = weapon->analog_trigger - weapon->trigger_reference;
            weapon->trigger_ticks = (signed char)(weapon->trigger_ticks + 1 > 255 ? 255 : weapon->trigger_ticks + 1);
            if (weapon->trigger_ticks >= autofire_ticks)
            {
                if (delta < autofire_threshold && weapon->analog_trigger < 250)
                    *fire = true;
                else
                    *alt = true;
            }
        }
    }
    bool useful = true;
    if (weapon_has_infinite_ammo(weapon_index))
        useful = false;
    if (!useful && *alt)
    {
        *alt = false;
        *fire = true;
    }
    if (weapon->analog_trigger == 0 && !*fire && !*alt)
        *clear = true;
    if (!*clear && *fire && ((trigger->flags & 8) != 0 || (trigger->flags & 16) != 0))
        *fire = false;
    if (!*clear && *alt && ((trigger->flags & 16) != 0 || ((trigger_definition->flags & 1) != 0 && (trigger->flags & 8) != 0)))
        *alt = false;
    weapon->previous_analog_trigger = weapon->analog_trigger;
    weapon->analog_trigger = 0;
}

void weapon_magazine_update_reload(long weapon_index, short magazine_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, magazine_index);
    if (magazine_definition->flags & 1)
        magazine->rounds_loaded = 0;
    bool infinite = ::game_engine_player_has_infinite_ammo(weapon_index);
    short rounds;
    if (infinite)
        rounds = (short)(magazine->rounds_loaded + magazine_definition->rounds_reload);
    else
    {
        long inventory = weapon_get_rounds_inventory(weapon_index, magazine_index, false);
        rounds = (short)(magazine->rounds_loaded + (magazine_definition->rounds_reload > inventory ? inventory : magazine_definition->rounds_reload));
    }
    rounds = rounds > magazine_definition->rounds_loaded_maximum ? magazine_definition->rounds_loaded_maximum : rounds;
    if (!infinite && (weapon->item.inventory_flags & 2))
        weapon_take_inventory_rounds(weapon_index, magazine_index, rounds - magazine->rounds_loaded);
    magazine->rounds_loaded = rounds;
    weapon_set_state_update_timer(weapon_index);
    weapon->time_184 = weapon->time_184 > magazine->time ? weapon->time_184 : magazine->time;
    magazine->fire_id = -1;
    magazine->chamber_id = -1;
    simulation_action_weapon_state_update(weapon_index);
}


static __forceinline long unit_current_weapon_object_get(unit_datum *unit)
{
    long weapon_index = -1;
    if (unit->unit.current_weapon_index != -1)
        weapon_index = unit->unit.weapon_object_indices[unit->unit.current_weapon_index];
    return weapon_index;
}

bool weapon_setup_first_person_emulation(long weapon_index, long animation_name)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool result = false;

    weapon->first_person_emulation.animation_id.clear();
    weapon->first_person_emulation.authored_time = 0.0f;
    weapon->first_person_emulation.animation_graph_index = -1;
    weapon->first_person_emulation.animation_name = -1;
    weapon->first_person_emulation.secondary = 0;
    weapon_stop_first_person_sound(weapon_index, animation_name, false);
    if (weapon->item.unit_object_index != -1) {
        unit_datum *unit = (unit_datum *)weapon_get(weapon->item.unit_object_index);
        if (unit->unit.player_index != -1) {
            const weapon_first_person_interface_definition *interface_definition =
                weapon_definition_get_first_person_interface_by_character(
                    (const weapon_definition *)definition, game_get_player_character_type(unit->unit.player_index));
            if (interface_definition && interface_definition->first_person_animations.index != -1) {
                c_animation_manager manager;
                if (manager.reset_graph(interface_definition->first_person_animations.index, -1, true)) {
                    long impulse = 226;
                    unit_datum *owner = (unit_datum *)weapon_get(weapon->item.unit_object_index);
                    bool dual_wielding = false;
                    if ((unsigned char)owner->unit.current_weapon_index != 255 && (unsigned char)owner->unit.secondary_weapon_index != 255)
                        dual_wielding = true;
                    if (dual_wielding)
                        impulse = 486;
                    c_animation_id animation_id = manager.find_impulse_or_state_animation_id(impulse, 226, animation_name);
                    if (animation_id.m_animation_index >= 0) {
                        weapon->first_person_emulation.animation_graph_index = interface_definition->first_person_animations.index;
                        weapon->first_person_emulation.animation_id = animation_id;
                        weapon->first_person_emulation.authored_time = 0.0f;
                        weapon->first_person_emulation.animation_name = animation_name;
                        weapon->first_person_emulation.secondary = unit_current_weapon_object_get((unit_datum *)weapon_get(
                                                                       weapon->item.unit_object_index)) != weapon_index;
                        object_wake(weapon_index);
                        if (first_person_weapon_is_deterministic_animation(animation_name)) {
                            long ticks = weapon_get_deterministic_first_person_animation_duration_ticks(weapon_index, animation_name, 1);
                            weapon->ready_ticks = weapon->ready_ticks > ticks ? weapon->ready_ticks : ticks;
                        }
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

enum e_director_perspective;

// @address 0x350b88 ?emulated_first_person_animation_callback@@YAXPBUs_animation_event_data@@J@Z
void emulated_first_person_animation_callback(const s_animation_event_data *event_data, long weapon_index)
{
    if (weapon_index == -1)
        return;
    s_object_header *headers = (s_object_header *)object_header_data->data;
    weapon_datum *weapon = (weapon_datum *)headers[datum_index_to_absolute_index(weapon_index)].datum;
    if (!weapon->item.owner_relation)
        return;
    long owner_unit_index = weapon->item.unit_object_index;
    if (owner_unit_index == -1)
        return;
    unit_datum *unit = (unit_datum *)headers[datum_index_to_absolute_index(owner_unit_index)].datum;
    long player_index = unit->unit.player_index;
    bool in_first_person = false;
    long perspective = -1;
    do {
        if (player_index != -1)
            perspective = ((long)::director_get_perspective(player_index, perspective));
        if (perspective == -1)
            break;
        if (tls_director_globals()->slots[perspective].perspective == 0) {
            in_first_person = true;
            break;
        }
    } while (tls_director_globals()->slots[perspective].perspective != 0);
    if (in_first_person)
        return;
    if (event_data->event_type == 1 && event_data->definition_index != -1) {
        if (((event_data->flags) & (1 << 3)) != 0)
            return;
        long marker_name = event_data->marker_name;
        object_marker marker;
        if (marker_name == -1 || marker_name == 146
            || internal_object_get_markers_by_string_id(weapon_index, marker_name, &marker, 1, false) < 1) {
            marker.node_index = 0;
            marker.node_to_marker.position = *global_origin3d;
            marker.node_to_marker.forward = *global_forward3d;
        }
        bool play_sound = true;
        if (((event_data->flags) & (1 << 6)) != 0
            && weapon_is_age_based(weapon_get(weapon_index)->definition_index)
            && weapon_has_infinite_ammo(weapon_index))
            play_sound = false;
        if (!play_sound)
            return;
        weapon->first_person_sound_index = object_impulse_sound_new(weapon_index, event_data->definition_index, marker.node_index,
            &marker.node_to_marker.position, &marker.node_to_marker.forward, 1.0f);
        weapon->first_person_sound_animation_name = weapon->first_person_emulation.animation_name;
        return;
    }
    if (event_data->event_type == 2) {
        if (event_data->definition_index == -1)
            return;
        s_damage_owner damage_owner;
        damage_compute_damage_owner_from_object_index(weapon_index, &damage_owner);
        long marker_name = event_data->marker_name;
        if (marker_name != -1 && marker_name != 0) {
            ::effect_new_attached_from_marker_name(event_data->definition_index, &damage_owner, 104,
                weapon_index, (unsigned short)-1, marker_name, 0.0f, 0.0f, 0,
                0, 0, 0);
            return;
        }
        effect_new_from_object(event_data->definition_index, &damage_owner, 104, weapon_index, 0.0f, 0.0f, 0, 0, 0);
    }
}

static __forceinline void weapons_inl_clear_first_person_emulation(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    float authored_time = k_real_zero_storage;
    weapon->first_person_emulation.animation_id.clear();
    weapon->first_person_emulation.authored_time = authored_time;
    weapon->first_person_emulation.animation_graph_index = -1;
    weapon->first_person_emulation.animation_name = -1;
    weapon->first_person_emulation.secondary = 0;
}

// @address 0x3508a0 weapon_stop_first_person_sound
extern "C" void weapon_stop_first_person_sound(long weapon_index, long animation_name, bool unknown)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    long sound_index = weapon->first_person_sound_index;
    bool stop = sound_index != -1;
    if (!unknown) {
        long animation = weapon->first_person_sound_animation_name;
        if (animation == weapon->first_person_emulation.animation_name)
            stop = false;
        if (animation == 0x249 || animation == 0x24a || animation == 0x24b || animation == 113)
            stop = false;
    }
    if (stop) {
        sound_stop_impulse(sound_index);
        weapon->first_person_sound_index = -1;
        weapon->first_person_sound_animation_name = -1;
    }
}

bool weapon_update_first_person_emulation(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    bool result = false;
    if (weapon->first_person_emulation.animation_graph_index != -1
        && weapon->first_person_emulation.animation_id.valid())
    {
        c_animation_manager animation_manager;
        if (animation_manager.reset_graph(weapon->first_person_emulation.animation_graph_index, -1, true))
        {
            c_animation_channel animation_channel;
            if (animation_manager.setup_animation_channel_by_index(&animation_channel,
                    weapon->first_person_emulation.animation_id,
                    weapon->first_person_emulation.secondary
                        ? ((1 << 0) | (1 << 9) | (1 << 13) | (1 << 14))
                        : ((1 << 0) | (1 << 9) | (1 << 12) | (1 << 14))))
            {
                weapons_set_frame_position_from_authored_time(&animation_channel, weapon->first_person_emulation.authored_time);
                animation_manager.update_animation_channel(&animation_channel, emulated_first_person_animation_callback, weapon_index);
                result = !animation_channel.playback_complete();
                weapon->first_person_emulation.authored_time = animation_channel.get_authored_time();
            }
            animation_channel_initialize_and_disable(&animation_channel);
        }
        if (!result)
            weapons_inl_clear_first_person_emulation(weapon_index);
    }
    return result;
}

/* natural-source functions added for the new records */
struct s_predicted_weapon_fire_data;


void weapon_set_initial_rounds_from_scenario_weapon(long weapon_index, const s_scenario_weapon *scenario_weapon)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    if (definition->magazines.s_tag_block::count > 0) {
        s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, 0);
        short rounds_loaded = scenario_weapon->rounds_loaded > 0 ? scenario_weapon->rounds_loaded
            : (magazine_definition->rounds_loaded_maximum > magazine_definition->rounds_total_initial
                ? magazine_definition->rounds_total_initial : magazine_definition->rounds_loaded_maximum);
        short rounds_total = scenario_weapon->rounds_total > 0 ? scenario_weapon->rounds_total
            : magazine_definition->rounds_total_initial;
        if (campaign_skull_available(5)) {
            rounds_loaded /= 2;
            rounds_total /= 2;
        }
        weapon_set_initial_rounds(weapon_index, 0, rounds_loaded, rounds_total);
    }
}

void weapons_fire_barrels()
{
    c_object_iterator<weapon_datum> iterator;
    iterator.begin(4, 2);
    while (iterator.next()) {
        weapon_datum *weapon = iterator.get_datum();
        if ((weapon->flags & 1) != 0)
            weapon_fire_barrels(iterator.get_index(), 0, 0, false, -1);
    }
}

// 0x349888

// @address 0x349888 ?weapon_fire_barrels@@YAXJPBUs_predicted_weapon_fire_data@@J_NJ@Z
void weapon_fire_barrels(long weapon_index, const s_predicted_weapon_fire_data *predicted_fire_data, long parent_unit_index, bool force,
    long forced_barrel_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    for (long barrel_index = 0; barrel_index < definition->barrel_count; barrel_index++) {
        s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
        if (barrel_index == forced_barrel_index || (barrel->flags & 2) != 0) {
            weapon_barrel_create_projectiles(weapon_index, (short)barrel_index, predicted_fire_data, parent_unit_index, force);
            barrel->flags &= ~2;
        }
    }
    weapon->weapon_flags.set(0, false);
}

static __forceinline void weapons_inl_reset_first_person_emulation(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon->first_person_emulation.animation_id.clear();
    weapon->first_person_emulation.authored_time = 0.0f;
    weapon->first_person_emulation.animation_graph_index = -1;
    weapon->first_person_emulation.animation_name = -1;
    weapon->first_person_emulation.secondary = 0;
}

// @address 0x349958 ?weapon_place@@YAJJPBUs_scenario_weapon@@@Z
long weapon_place(long weapon_index, const s_scenario_weapon *scenario_weapon)
{
    weapon_datum *weapon = (weapon_datum *)object_get_and_verify_type(weapon_index, _object_mask_weapon);
    weapon_set_initial_rounds_from_scenario_weapon(weapon_index, scenario_weapon);
    object_set_at_rest(weapon_index, (scenario_weapon->flags & 1) != 0);
    object_set_invalid_for_recycling(weapon_index, true);
    if (!((unsigned char)((scenario_weapon->flags >> 2) & 1)))
        weapon->item.inventory_flags = weapon->item.inventory_flags | 4;
    else
        weapon->item.inventory_flags = weapon->item.inventory_flags & 251;
    if (!(scenario_weapon->flags & 1))
        weapon->item.object.position.z = weapon->item.object.position.z + 0.05f;
    object_placement_update_multiplayer_item_flags(weapon_index);
    return weapon_index;
}

bool weapon_new(long weapon_index, object_placement_data *placement, bool *out)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    short i;

    weapon->weapon_state = 0;
    weapon->overheated_effect_index = -1;
    weapon->f1a0 = 0.0f;
    weapon->current_power = 0.0f;
    weapon_set_state(weapon_index, 0, true);
    weapon->tracked_object_index = -1;
    weapon->current_state_ticks = -1;
    weapon->target_time = 0;
    weapon->trigger_direction = 0;
    weapon->trigger_ticks = 0;
    weapon->trigger_reference = 0;
    weapon->previous_analog_trigger = 0;
    for (i = 0; i < definition->magazines.s_tag_block::count; i++) {
        s_weapon_magazine *magazine = &weapon->magazines[i];
        s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, i);
        magazine->rounds_loaded = magazine_definition->rounds_total_initial > magazine_definition->rounds_loaded_maximum
            ? magazine_definition->rounds_loaded_maximum : magazine_definition->rounds_total_initial;
        magazine->rounds_inventory = magazine_definition->rounds_total_initial - magazine->rounds_loaded > magazine_definition->rounds_inventory_maximum
            ? magazine_definition->rounds_inventory_maximum : magazine_definition->rounds_total_initial - magazine->rounds_loaded;
    }
    for (i = 0; i < definition->trigger_count; i++)
        weapon->triggers[i].charging_effect_index = -1;
    for (i = 0; i < definition->barrel_count; i++) {
        s_weapon_barrel *barrel = &weapon->barrels[i];
        barrel->active = 127;
        barrel->firing_effect_index = -1;
    }
    weapon->multiplayer_weapon_identifier = -1;
    weapon->simulation_weapon_identifier = (short)weapon_index;
    weapons_inl_reset_first_person_emulation(weapon_index);
    weapon->first_person_sound_index = -1;
    weapon->first_person_sound_animation_name = -1;
    return true;
}

void weapon_barrel_add_fire_error(long weapon_index, long barrel_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);

    if (definition->barrel_count > barrel_index) {
        s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
        s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
        bool dual_wielded = weapon_is_dual_wielded(weapon_index);
        long owner_unit_index = -1;
        if (weapon->item.owner_relation)
            owner_unit_index = weapon->item.unit_object_index;
        float theta = unit_get_weapon_dual_error_theta(owner_unit_index, weapon_index);
        if (definition->m_flags.test(_weapon_definition_dual_wield_error_simple))
            theta = dual_wielded ? 1.0f : 0.0f;
        barrel->error = barrel->error + barrel_definition->error_rate * game_tick_length();
        barrel->error = barrel->error > 1.0f ? 1.0 : barrel->error;
        float rate = theta * barrel_definition->dual_error_rate + (1.0f - theta) * barrel_definition->error_rate_b;
        barrel->firing_error_fraction = barrel->firing_error_fraction + rate * game_tick_length();
        barrel->firing_error_fraction = barrel->firing_error_fraction > 1.0f ? 1.0f : barrel->firing_error_fraction;
    }
}

struct s_projectile_targeting_data;


// 0x34a308

// @address 0x34a308 ?weapon_handle_potential_inventory_item@@YA_NJJJJPAF@Z
// match: 0x34a308: without __restrict rounds_picked_up moves to r24 and the prologue gains a li r8,0 (19 words)
bool weapon_handle_potential_inventory_item(long weapon_index, long item_index, long unit_index, long player_index, short *__restrict rounds_picked_up)
{
    bool item_has_rounds = false;
    bool result = false;
    *rounds_picked_up = 0;
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    weapon_datum *item = weapon_get(item_index);
    s_object_definition *item_definition = TAG_GET('obje', s_object_definition, item->definition_index);
    for (short magazine_index = 0; magazine_index < definition->magazines.s_tag_block::count; magazine_index++) {
        s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, magazine_index);
        s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
        short rounds_available = magazine_definition->rounds_inventory_maximum - magazine->rounds_inventory;
        if (rounds_available > 0) {
            s_weapon_magazine *item_magazine = 0;
            short rounds = 0;
            if (weapon->definition_index == item->definition_index) {
                item_magazine = &item->magazines[magazine_index];
                if (((1 << item_definition->object_type) & 4) && weapon_can_be_dual_wielded(item_index))
                    rounds = item_magazine->rounds_inventory > rounds_available ? rounds_available : item_magazine->rounds_inventory;
                else
                    rounds = item_magazine->rounds_inventory + item_magazine->rounds_loaded > rounds_available
                        ? rounds_available
                        : item_magazine->rounds_inventory + item_magazine->rounds_loaded;
                result = rounds > 0;
            }
            else {
                for (short ammunition_index = 0; ammunition_index < magazine_definition->ammunition_count; ammunition_index++) {
                    s_magazine_ammunition *ammunition = &magazine_definition->ammunition[ammunition_index];
                    if (ammunition->definition_index == item->definition_index) {
                        rounds = ammunition->rounds > rounds_available ? rounds_available : ammunition->rounds;
                        result = true;
                        break;
                    }
                }
            }
            if (rounds > 0 && !game_is_predicted()) {
                if (item_magazine) {
                    short inventory_rounds = item_magazine->rounds_inventory > rounds ? rounds : item_magazine->rounds_inventory;
                    short loaded_rounds = item_magazine->rounds_loaded > rounds - inventory_rounds ? rounds - inventory_rounds : item_magazine->rounds_loaded;
                    item_magazine->rounds_inventory -= inventory_rounds;
                    item_magazine->rounds_loaded -= loaded_rounds;
                    if (item_magazine->rounds_inventory > 0 || item_magazine->rounds_loaded > 0)
                        item_has_rounds = true;
                }
                magazine->rounds_inventory += rounds;
                *rounds_picked_up += rounds;
                weapon_set_state_update_timer(weapon_index);
            }
        }
    }
    if (result) {
        object_wake(weapon_index);
        object_wake(item_index);
        if (game_simulation_predicted()) {
            simulation_request_projectile_supercombine(item_index);
            result = false;
        }
        else {
            bool item_is_weapon = ((1 << item_definition->object_type) & 4) != 0;
            bool dual_wielded = item_is_weapon ? weapon_can_be_dual_wielded(item_index) : false;
            if (!item_has_rounds && !dual_wielded)
                object_delete(item_index);
            if ((1 << item_definition->object_type) & 4) {
                if (player_get_first_output_user(player_index) != -1 && definition->pickup_sound != -1)
                    unspatialized_impulse_sound_new(definition->pickup_sound, 1.0f);
            }
            else {
                projectile_play_pickup_sound(player_index, item->definition_index);
            }
            simulation_action_pickup_ammo(unit_index, item->definition_index, weapon->definition_index, *rounds_picked_up);
            simulation_action_weapon_state_update(weapon_index);
            if ((1 << item_definition->object_type) & 4)
                simulation_action_object_update(item_index, 0x100000);
        }
    }
    return result;
}

void weapon_ready(long weapon_index, bool skip_effects, bool instant)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);

    weapon_reset(weapon_index);
    weapon_set_state(weapon_index, 9, true);
    if (!skip_effects) {
        weapon_effect_new(weapon_index, -1, definition->ready_effect, 0.0f, 0.0f);
        damage_effect_new_on_weapon(weapon_index, definition->ready_damage_effect);
    }
    if (instant) {
        weapon->ready_ticks = 0;
    } else {
        weapon->weapon_flags.set(5, false);
        weapon->turn_on_timer = (unsigned char)(game_seconds_to_ticks_round(definition->ready_duration) < 255
            ? game_seconds_to_ticks_round(definition->ready_duration) : 255);
        weapon->weapon_flags.set(8, false);
        weapon->ready_for_use_timer = (unsigned char)(game_seconds_to_ticks_round(definition->ready_duration_b) < 255
            ? game_seconds_to_ticks_round(definition->ready_duration_b) : 255);
        weapon->ready_ticks = weapon_get_deterministic_first_person_animation_duration_ticks(weapon_index, 38, 1);
    }
}

bool weapon_put_away(long weapon_index, bool unused, bool force)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool result = false;
    long trigger_index;
    long barrel_index;

    for (trigger_index = 0; trigger_index < definition->trigger_count; trigger_index++)
        weapon_trigger_recover(weapon_index, (short)trigger_index, false);
    for (barrel_index = 0; barrel_index < definition->barrel_count; barrel_index++)
        weapon_barrel_recover(weapon_index, (short)barrel_index, false);
    if (weapon_set_state(weapon_index, _weapon_state_put_away, force)) {
        weapon->ready_ticks = weapon_get_deterministic_first_person_animation_duration_ticks(weapon_index, _string_id_put_away, 1);
        result = true;
    }
    return result;
}

bool weapon_can_be_fired(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool result = true;
    if (weapon_has_infinite_ammo(weapon_index)) {
        result = false;
    } else if (definition->magazines.s_tag_block::count > 0) {
        s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, 0);
        if (magazine_definition->rounds_loaded_maximum > 0 &&
            weapon->magazines[0].rounds_loaded == 0 &&
            weapon->magazines[0].rounds_inventory == 0 &&
            !::game_engine_player_has_infinite_ammo(weapon_index))
            result = false;
    }
    return result;
}

bool weapon_reload(long weapon_index, long magazine_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool result = false;

    if (magazine_index >= 0 && magazine_index < definition->magazines.s_tag_block::count &&
        weapon_magazine_start_or_continue_reload(weapon_index, (short)magazine_index))
        result = true;
    if (result) {
        long trigger_index;
        long barrel_index;
        object_wake(weapon_index);
        for (trigger_index = 0; trigger_index < definition->trigger_count; trigger_index++)
            weapon_trigger_recover(weapon_index, (short)trigger_index, false);
        for (barrel_index = 0; barrel_index < definition->barrel_count; barrel_index++) {
            weapons_inl_reload_clear_empty_click_bit(weapon_index, (short)barrel_index);
            if ((unsigned short)definition->reload_interrupt_mode != 1)
                weapons_barrel_idle(weapon_index, (short)barrel_index);
        }
    }
    return result;
}

void weapon_steal_goodies(long weapon_index, long victim_weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_datum *victim = weapon_get(victim_weapon_index);

    object_wake(weapon_index);
    object_wake(victim_weapon_index);
    if (weapon->definition_index == victim->definition_index) {
        weapon_definition *definition = weapon_definition_get(weapon->definition_index);
        long magazine_index;
        for (magazine_index = 0; magazine_index < definition->magazines.s_tag_block::count; magazine_index++) {
            s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, magazine_index);
            s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
            s_weapon_magazine *victim_magazine = &victim->magazines[magazine_index];
            if (magazine->rounds_inventory < magazine_definition->rounds_inventory_maximum && victim_magazine->rounds_inventory > 0) {
                long room = magazine_definition->rounds_inventory_maximum - magazine->rounds_inventory;
                long taken = victim_magazine->rounds_inventory > room ? room : victim_magazine->rounds_inventory;
                magazine->rounds_inventory = magazine->rounds_inventory + taken;
                victim_magazine->rounds_inventory = victim_magazine->rounds_inventory - taken;
            }
            if (magazine->rounds_inventory < magazine_definition->rounds_inventory_maximum && victim_magazine->rounds_loaded > 0) {
                long room = magazine_definition->rounds_inventory_maximum - magazine->rounds_inventory;
                long taken = victim_magazine->rounds_loaded > room ? room : victim_magazine->rounds_loaded;
                magazine->rounds_inventory = magazine->rounds_inventory + taken;
                victim_magazine->rounds_loaded = victim_magazine->rounds_loaded - taken;
            }
        }
        if (weapon->age > victim->age) {
            float age = weapon->age;
            weapon->age = victim->age;
            victim->age = age;
        }
    }
}

void weapon_set_total_rounds(long weapon_index, short *rounds_array)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    for (short magazine_index = 0; magazine_index < definition->magazines.s_tag_block::count; magazine_index++) {
        s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
        s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, magazine_index);
        short rounds = rounds_array[magazine_index];
        magazine->rounds_loaded = magazine->rounds_loaded > rounds ? rounds : magazine->rounds_loaded;
        magazine->rounds_inventory = rounds - magazine->rounds_loaded > magazine_definition->rounds_inventory_maximum
            ? magazine_definition->rounds_inventory_maximum : rounds - magazine->rounds_loaded;
    }
    simulation_action_weapon_state_update(weapon_index);
    object_wake(weapon_index);
}

void weapon_set_current_amount(long weapon_index, float amount)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool age_based = weapon_is_age_based(weapon_get(weapon_index)->definition_index);
    amount = MIN(MAX(amount, 0.0f), 1.0f);
    if (age_based) {
        weapon->age = 1.0f - amount;
    } else if (definition->magazines.s_tag_block::count > 0) {
        s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, 0);
        s_weapon_magazine *magazine = &weapon->magazines[0];
        short rounds_loaded = (short)(long)(magazine_definition->rounds_loaded_maximum * amount + 0.5f);
        short rounds_inventory = magazine->rounds_inventory + (magazine->rounds_loaded - rounds_loaded);
        magazine->rounds_inventory = MIN(MAX(rounds_inventory, 0), magazine_definition->rounds_inventory_maximum);
        magazine->rounds_loaded = rounds_loaded;
    }
    object_wake(weapon_index);
    simulation_action_weapon_state_update(weapon_index);
}

void weapon_report_kill(long weapon_index, long victim_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);

    if (definition->m_flags.test(_weapon_definition_weapon_ages)) {
        unsigned long *victim = (unsigned long *)object_try_and_get_and_verify_type(victim_index, 0x1003);
        if (victim) {
            float scale = 1.0f;
            if (tls_game_globals()->options.game_mode == 1) {
                switch ((TAG_GET('unit', unit_definition, *victim))->default_team) {
                case 4:
                case 5:
                    scale = 0.25f;
                    break;
                }
            }
            weapon_age(weapon_index, scale * 0.1f);
            simulation_action_weapon_state_update(weapon_index);
        }
    }
}


bool weapon_barrel_check_for_reload(long weapon_index, long barrel_index)
{
    weapon_datum *object = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(object->definition_index);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    bool result = false;
    short magazine_index = barrel_definition->magazine_index;
    long magazine_index_int = magazine_index;

    if (magazine_index_int != -1) {
        s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, barrel_definition->magazine_index);
        s_weapon_magazine *magazine = &object->magazines[barrel_definition->magazine_index];
        short rounds_loaded = magazine->rounds_loaded;
        if ((rounds_loaded < barrel_definition->minimum_rounds && !test_bit(barrel_definition->flags, 2)) ||
            rounds_loaded < barrel_definition->rounds_per_shot || rounds_loaded == 0) {
            long rounds = weapon_get_rounds_inventory(weapon_index, magazine_index_int, false);
            if ((rounds > 0 || ::game_engine_player_has_infinite_ammo(weapon_index)) &&
                rounds_loaded < magazine_definition->rounds_loaded_maximum) {
                if (!test_bit(object->control_flags, 5) && object->item.object.parent_object_index != -1)
                    weapon_submit_reload_action(weapon_index, magazine_index, false);
                result = true;
            } else {
                long trigger_index;
                long barrel;
                for (trigger_index = 0; trigger_index < definition->trigger_count; trigger_index++)
                    weapon_trigger_recover(weapon_index, (short)trigger_index, false);
                for (barrel = 0; barrel < definition->barrel_count; barrel++) {
                    s_weapon_barrel *barrel_state = &object->barrels[barrel];
                    if (barrel_state->state_1 == 1)
                        weapon_barrel_recover(weapon_index, (short)barrel, true);
                }
            }
        }
    }
    return result;
}

static __forceinline bool weapons_magazine_state_compatible(short current, short target)
{
    bool result = false;
    switch (current) {
    case 0:
    case 5:
        result = true;
        break;
    case 4:
        if (target == 3)
            result = true;
        break;
    }
    return result;
}

static __forceinline bool weapons_weapon_can_start_reload(long weapon_index)
{
    weapon_datum *object = weapon_get(weapon_index);
    return !object->triggers[0].state && !object->triggers[1].state && (short)object->weapon_state <= 2;
}

bool weapon_magazine_start_or_continue_reload(long weapon_index, short magazine_index)
{
    weapon_datum *object = weapon_get(weapon_index);
    s_weapon_magazine *state = &object->magazines[magazine_index];
    weapon_definition *definition = weapon_definition_get(object->definition_index);
    s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, magazine_index);
    bool result = false;
    long target_state;

    if ((short)(unsigned short)definition->reload_interrupt_mode == 1) {
        if (state->state == 3 || state->state == 4)
            target_state = 3;
        else
            target_state = 2;
    } else {
        target_state = 1;
    }
    if (((short)(unsigned short)definition->reload_interrupt_mode != 1 || state->pending_reload <= 0) &&
        weapons_magazine_state_compatible(state->state, (short)target_state) &&
        weapons_weapon_can_start_reload(weapon_index)) {
        long inventory = weapon_get_rounds_inventory(weapon_index, magazine_index, false);
        if (inventory > 0 || ::game_engine_player_has_infinite_ammo(weapon_index)) {
            if (state->rounds_loaded < magazine_definition->rounds_loaded_maximum) {
                weapon_set_state(weapon_index, magazine_index + 5, false);
                ::weapon_magazine_set_state(weapon_index, magazine_index, (short)target_state);
                weapon_effect_new(weapon_index, -1, magazine_definition->reload_effect, 0.0f, 0.0f);
                result = true;
            }
        }
    }
    return result;
}

// @address 0x34cdf0 ?weapon_magazine_start_chamber@@YAXJF@Z
void weapon_magazine_start_chamber(long weapon_index, short magazine_index)
{
    weapon_datum *object = weapon_get(weapon_index);
    s_weapon_magazine *state = &object->magazines[magazine_index];

    if (weapons_magazine_state_compatible(state->state, 6) && weapons_weapon_can_start_reload(weapon_index)) {
        weapon_definition *definition = weapon_definition_get(object->definition_index);
        s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, magazine_index);
        weapon_effect_new(weapon_index, -1, magazine_definition->chamber_effect, 0.0f, 0.0f);
        weapon_set_state(weapon_index, magazine_index + 3, false);
        ::weapon_magazine_set_state(weapon_index, magazine_index, 6);
    }
}

static __forceinline void weapons_trigger_set_state(long weapon_index, short trigger_index, short state, short timer)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon->triggers[trigger_index].state = (signed char)state;
    weapon->triggers[trigger_index].timer = timer;
}

void weapon_trigger_fully_charged(long weapon_index, short trigger_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_trigger_definition *trigger_definition = trigger_definition_get(definition, trigger_index);

    weapons_trigger_set_state(weapon_index, trigger_index, 2, (short)game_seconds_to_ticks_round(trigger_definition->charged_time));
    weapon_set_state(weapon_index, trigger_index + 7, true);
    weapon_send_message_to_unit(weapon_index, _unit_message_fully_charged);
}


// @address 0x34d090
void weapon_trigger_release_charge(long weapon_index, short trigger_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_trigger_definition *trigger_definition = trigger_definition_get(definition, trigger_index);
    if (trigger_definition->release_effect != -1)
        weapon_effect_new(weapon_index, -1, trigger_definition->release_effect, 0.0f, 0.0f);
    if (trigger_definition->release_damage != -1)
        damage_effect_new_on_weapon(weapon_index, trigger_definition->release_damage);
    if (trigger_definition->release_time > 0.0f)
        weapons_trigger_set_state(weapon_index, trigger_index, 4, (short)game_seconds_to_ticks_round(trigger_definition->release_time));
    else
    {
        if (trigger_definition->barrel_index != -1)
            weapon_barrel_set_fire_bit(weapon_index, trigger_definition->barrel_index, true);
        if (trigger_definition->behavior == 7)
        {
            float minimum_distance = 0.9f;
            s_object_header *entries = (s_object_header *)object_header_data->data;
            weapon_datum *weapon_data = (weapon_datum *)entries[datum_index_to_absolute_index(weapon_index)].datum;
            if (weapon_data->item.owner_relation)
            {
            if (weapon_data->item.unit_object_index != -1)
            {
                long unit_index = weapon_data->item.unit_object_index;
                unit_datum *unit = (unit_datum *)entries[datum_index_to_absolute_index(unit_index)].datum;
                s_aim_assist_targeting_result *targeting = &unit->unit.targeting;
                bool lunge = false;
                long target_index = -1;
                if (targeting->primary_autoaim_level > 0.0f && object_try_and_get_unsafe_and_verify_type(targeting->target_object_index, 0x1003))
                {
                    target_index = targeting->target_object_index;
                    real_point3d target_origin;
                    real_point3d unit_origin;
                    object_get_origin(targeting->target_object_index, &target_origin);
                    object_get_origin(unit_index, &unit_origin);
                    if (distance_squared3d(&target_origin, &unit_origin) > minimum_distance * minimum_distance)
                        lunge = true;
                }
                action_request request = {};
                memset(&request, 0, sizeof(request));
                request.type = 60;
                request.melee_player.type = (lunge != false) + 1;
                request.melee_player.target = target_index;
                if (lunge && simulation_query_local_predictive_control(unit_index, 0))
                    request.melee_player.weapon_slot = 2;
                else
                    request.melee_player.weapon_slot = 0;
                action_submit(unit_index, &request);
            }
            }
        }
        weapon_trigger_recover(weapon_index, trigger_index, true);
    }
}

static __forceinline void weapons_trigger_idle(long weapon_index, short trigger_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    weapon->triggers[trigger_index].timer = 0;
    weapon->triggers[trigger_index].state = 0;
    if ((unsigned short)definition->reload_interrupt_mode == 7)
        weapon_send_message_to_unit(weapon_index, _unit_message_idle);
}

void weapon_trigger_recover(long weapon_index, short trigger_index, bool force)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    s_weapon_trigger *state = &weapon->triggers[trigger_index];

    weapons_trigger_idle(weapon_index, trigger_index);
    if (force) {
        state->flags &= 0xfff7;
        state->flags &= 0xffef;
    }
    if (state->charging_effect_index != -1) {
        effect_stop(state->charging_effect_index, true);
        state->charging_effect_index = -1;
    }
}

void weapon_trigger_begin_tracking(long weapon_index, short trigger_index)
{
    long node = -1;
    weapon_datum *object = weapon_get(weapon_index);

    if (object->item.owner_relation)
        node = object->item.unit_object_index;
    object->triggers[trigger_index].state = 3;
    object->triggers[trigger_index].timer = 0;
    if (node != -1)
        ::unit_unzoom(node, 1);
}

void weapon_trigger_begin_locked(long weapon_index, short trigger_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_trigger_definition *trigger_definition = trigger_definition_get(definition, trigger_index);
    long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
    long ticks = game_seconds_to_ticks_round(trigger_definition->lock_time);

    weapons_trigger_set_state(weapon_index, trigger_index, 6, (short)ticks);
    if (owner_unit_index != -1)
        ::unit_unzoom(owner_unit_index, true);
}

void weapon_trigger_begin_locking(long weapon_index, short trigger_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_trigger_definition *trigger_definition = trigger_definition_get(definition, trigger_index);
    long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
    long ticks = game_seconds_to_ticks_round(trigger_definition->locked_time);

    weapons_trigger_set_state(weapon_index, trigger_index, 5, (short)ticks);
    if (owner_unit_index != -1)
        ::unit_unzoom(owner_unit_index, true);
}

void weapon_trigger_begin_charging(long weapon_index, short trigger_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_trigger_definition *trigger_definition = trigger_definition_get(definition, trigger_index);
    s_weapon_trigger *state = &weapon->triggers[trigger_index];
    long owner = weapon_owner_unit_index_get(weapon_index);

    state->charging_effect_index = weapon_effect_new(weapon_index, -1, trigger_definition->charging_effect, 0.0f, 0.0f);
    damage_effect_new_on_weapon(weapon_index, trigger_definition->charging_damage);
    weapons_trigger_set_state(weapon_index, trigger_index, 1, (short)game_seconds_to_ticks_round(trigger_definition->charging_time));
    if (owner != -1)
        ::unit_unzoom(owner, true);
}

void weapon_reset(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    short trigger_index;
    short magazine_index;

    for (trigger_index = 0; trigger_index < definition->trigger_count; trigger_index++) {
        s_weapon_trigger *trigger = &weapon->triggers[trigger_index];
        trigger->state = 0;
        trigger->timer = 0;
    }
    for (magazine_index = 0; magazine_index < definition->magazines.s_tag_block::count; magazine_index++)
        ::weapon_magazine_set_state(weapon_index, magazine_index, 0);

    weapon_datum *reset_weapon = weapon_get(weapon_index);
    reset_weapon->first_person_emulation.animation_id.clear();
    reset_weapon->first_person_emulation.authored_time = 0.0f;
    reset_weapon->first_person_emulation.animation_graph_index = -1;
    reset_weapon->first_person_emulation.animation_name = -1;
    reset_weapon->first_person_emulation.secondary = 0;
    object_wake(weapon_index);
}

bool weapon_should_set_targeting_data(long weapon_index, s_projectile_targeting_data *targeting_data)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool result = true;
    short mode = (short)definition->tracking_type;

    if (mode != 0 && targeting_data->object_index != -1) {
        object_datum *object = (object_datum *)object_try_and_get_and_verify_type(targeting_data->object_index, 0xffffffff);
        if (object && targeting_data->model_target_index != -1) {
            long model = TAG_GET('obje', s_object_definition, object->definition_index)->model.index;
            if (model != -1) {
                s_model_definition *model_definition = TAG_GET('hlmt', s_model_definition, model);
                if (targeting_data->model_target_index >= 0 && targeting_data->model_target_index < model_definition->targets.count) {
                    s_model_target *node = &((s_model_target *)model_definition->targets.address)[targeting_data->model_target_index];
                    switch (mode) {
                    case 1:
                        result = (node->flags & 1) != 0;
                        break;
                    case 2:
                        result = (node->flags & 2) != 0 || (node->flags & 0x10) != 0;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

static void weapon_handle_primary_frame_event(long weapon_index)
{
    switch ((short)weapon_get(weapon_index)->weapon_state) {
    case 3:
        weapon_barrel_start_ejection_port(weapon_index, 0, true);
        break;
    case 4:
        weapon_barrel_start_ejection_port(weapon_index, 1, true);
        break;
    }
}

static void weapon_handle_state_finished(long weapon_index, bool looping)
{
    weapon_datum *w = weapon_get(weapon_index);
    bool restart = looping;
    long next_state = (short)w->weapon_state;
    switch ((short)w->weapon_state) {
    case 7:
    case 8:
    {
        bool trigger_busy = w->triggers[0].state == 1 || w->triggers[0].state == 2;
        if (trigger_busy) {
            restart = false;
            next_state = 0;
        }
        break;
    }
    case _weapon_state_put_away:
        restart = false;
        break;
    default:
        next_state = 0;
        break;
    }
    // match: 0xfe428: volatile state read (with the owner read) keeps weapon_update's add at 0x820feec4 as r27,r10,r29 (r27,r29,r10 without it).
    const volatile short &current_state = w->weapon_state;
    if (restart || next_state != current_state)
        weapon_set_state(weapon_index, next_state, true);
}

static __forceinline bool weapons_state_change_allowed(long current_state, long state)
{
    bool result = false;
    if ((unsigned long)current_state >= 1) {
        if ((unsigned long)current_state < 3)
            result = state >= current_state;
    } else {
        result = true;
    }
    return result;
}

static __forceinline bool weapons_is_stowed(long weapon_index)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    bool result = false;
    if (weapon->item.owner_relation == 3)
        result = true;
    return result;
}

static __forceinline bool weapons_unit_is_dual_wielding(long unit_index)
{
    weapon_datum *unit = weapon_get(unit_index);
    bool result = false;
    if ((unsigned char)((unit_datum *)unit)->unit.current_weapon_indices[0] != 255
        && (unsigned char)((unit_datum *)unit)->unit.current_weapon_indices[1] != 255)
        result = true;
    return result;
}

bool weapon_set_state(long weapon_index, long state, bool force)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    bool result = false;
    object_wake(weapon_index);
    if (force || weapons_state_change_allowed((short)weapon->weapon_state, state)) {
        weapon_datum *object = weapon_get(weapon_index);
        if ((unsigned short)object->item.object.animation.offset != 0xffff) {
            c_animation_manager *manager = (c_animation_manager *)((char *)weapon + (short)weapon->item.object.animation.offset);
            long playback_flags = 29;
            long animation_state;
            switch (state) {
            case 0:
                animation_state = _string_id_idle;
                playback_flags = 31;
                break;
            case 1:
                animation_state = _string_id_fire_1;
                break;
            case 2:
                animation_state = _string_id_fire_2;
                break;
            case 3:
                animation_state = _string_id_chamber_1;
                break;
            case 4:
                animation_state = _string_id_chamber_2;
                break;
            case 5:
            case 6:
                animation_state = _string_id_reload_1;
                break;
            case 7:
            case 8:
                animation_state = _string_id_charged_2;
                break;
            case 9:
                animation_state = _string_id_ready;
                break;
            case _weapon_state_put_away:
                animation_state = _string_id_put_away;
                break;
            default:
                // match: 0x3503e8: without it the switch gains a cmplwi 10/bgt bound check and animation_state spills (frame 0xb0 -> 0xc0)
                __assume(0);
            }
            long flags = 130;
            if (animation_state == _string_id_idle) {
                flags = 150;
                long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
                if (owner_unit_index == -1 || weapons_is_stowed(weapon_index))
                    animation_state = _string_id_disabled;
            }
            long weapon_class = _string_id_default;
            long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
            if (owner_unit_index != -1 && weapons_unit_is_dual_wielding(owner_unit_index))
                weapon_class = _string_id_dual;
            if (manager->set_goal(_string_id_current, weapon_class, _string_id_default, animation_state, flags, playback_flags)) {
                weapon->weapon_state = (unsigned short)state;
            } else if (weapon_class == _string_id_dual &&
                manager->set_goal(_string_id_current, _string_id_default, _string_id_default, animation_state, flags, playback_flags)) {
                weapon->weapon_state = (unsigned short)state;
            }
        }
        long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
        if (owner_unit_index != -1)
            unit_handle_weapon_state_change(owner_unit_index, weapon_index, state);
        result = true;
    }
    return result;
}

// 0x350678: magazine state change and its unit message

static __forceinline long weapons_magazine_message(short state, bool empty)
{
    long result = -1;
    switch (state) {
    case 1:
        result = empty ? 7 : 8;
        break;
    case 2:
        result = 9;
        break;
    case 3:
        result = empty ? 10 : 11;
        break;
    case 4:
        break;
    default:
        // match: 0x350678: without it the inlined state switch compiles to a cmpwi/beq chain instead of the bctr jump table
        __assume(0);
    }
    return result;
}

void weapon_magazine_set_state(long weapon_index, short magazine_index, short state)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, magazine_index);
    bool empty = magazine->rounds_loaded == 0;
    short reload = -1;
    short ticks = -1;
    short remaining = -1;
    long message = -1;

    switch (state) {
    case 0:
    case 5:
        break;
    case 6:
        ticks = (short)game_seconds_to_ticks_round(magazine_definition->chamber_time);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        message = weapons_magazine_message(state, empty);
        if (message != -1) {
            long animation = ::first_person_weapon_state_from_message((short)message);
            ticks = weapon_get_deterministic_first_person_animation_duration_ticks(weapon_index, animation, 0);
            if (state == 2) {
                reload = -1;
            } else {
                reload = weapon_get_deterministic_first_person_animation_duration_ticks(weapon_index, animation, 3);
                remaining = weapon_get_deterministic_first_person_animation_duration_ticks(weapon_index, animation, 1);
                if (remaining > 0)
                    remaining = (short)((remaining - reload) > 0 ? remaining - reload : 0);
            }
        }
        break;
    default:
        // match: 0x350678: without it the state switch compiles to a cmpwi/ble chain instead of the bctr jump table
        __assume(0);
    }
    magazine->state = state;
    magazine->ticks = ticks;
    magazine->fire_id = reload;
    magazine->chamber_id = reload;
    magazine->time = remaining > 0 ? remaining : 0;
    if (message != -1)
        weapon_send_message_to_unit(weapon_index, (e_unit_messages)message);
}

extern "C" void weapon_interrupt_reload_and_clear_timer(long weapon_index, long unused, bool clear_timer)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    short ticks_per_second = game_time_globals->ticks_per_second;
    long threshold = (long)((float)ticks_per_second * 0.1f + 0.5f);
    short rounds_reloaded;
    if (weapon_reloading(weapon_index, -1, &rounds_reloaded)) {
        bool reload_magazines = false;
        if (rounds_reloaded >= 0 && rounds_reloaded <= threshold)
            reload_magazines = true;
        weapon_interrupt_reload(weapon_index, reload_magazines);
    }
    if (clear_timer) {
        if (weapon->time_184 > 0 && weapon->time_184 <= threshold)
            weapon->time_184 = 0;
    }
}


// @address 0xc0898 ?weapon_aim@@YA_NJFPBTreal_point3d@@0PAM_NPATreal_vector3d@@111PA_N@Z
bool weapon_aim(long weapon_index, short barrel_index, const real_point3d *origin, const real_point3d *target, float *maximum_time_scale,
    bool high_arc, real_vector3d *aim_vector, float *unused, float *result_distance, float *result_speed, bool *linear)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    bool result = false;
    if (barrel_index >= 0 && barrel_index < definition->barrel_count) {
        long projectile_index = definition->barrels[barrel_index].projectile_index;
        if (projectile_index != -1) {
            projectile_aim(projectile_index, origin, target, NULL, NULL, maximum_time_scale, NULL, high_arc, aim_vector, result_speed, NULL,
                result_distance, linear);
            return true;
        }
        result = projectile_aim_linear(1.0f, origin, target, aim_vector, result_speed, NULL, result_distance);
        if (linear)
            *linear = true;
    }
    return result;
}

static __forceinline bool weapons_reload_is_distributed(long simulation_type)
{
    bool result = false;
    switch (simulation_type) {
    case 4:
    case 5:
        result = true;
        break;
    }
    return result;
}
static __forceinline bool weapons_reload_is_local(s_object_header *entries, weapon_datum *object)
{
    bool result = false;
    if (weapons_reload_is_distributed(tls_game_globals()->options.simulation_type)) {
        long type_mask = 1L << object->item.object.object_type;
        long player_index = -1;
        if (type_mask & 0x1003) {
            unit_datum *unit = (unit_datum *)object;
            player_index = unit->unit.player_index;
            if (player_index == -1 && unit->unit.gunner_object_index != -1)
                player_index = ((unit_datum *)entries[datum_index_to_absolute_index(unit->unit.gunner_object_index)].datum)->unit.player_index;
        } else if (type_mask & 12) {
            if (object->item.unit_object_index != -1)
                player_index = ((unit_datum *)entries[datum_index_to_absolute_index(object->item.unit_object_index)].datum)->unit.player_index;
        }
        if (player_index != -1)
            result = player_is_on_local_machine(player_index);
    }
    return result;
}
static __forceinline long weapons_reload_get_weapon(unit_datum *unit, short inventory_index)
{
    long result = -1;
    if (inventory_index != -1)
        result = unit->unit.weapon_object_indices[inventory_index];
    return result;
}
// @address 0x349bd8 weapon_submit_reload_action
extern "C" bool weapon_submit_reload_action(long weapon_index, short magazine_index, bool forced)
{
    s_object_header *entries = (s_object_header *)object_header_data->data;
    weapon_datum *weapon = (weapon_datum *)entries[datum_index_to_absolute_index(weapon_index)].datum;
    long unit_index = -1;
    if (weapon->item.owner_relation)
        unit_index = weapon->item.unit_object_index;
    bool allowed = true;
    bool result = false;
    if (weapons_reload_is_distributed(tls_game_globals()->options.simulation_type) && !forced)
        allowed = weapons_reload_is_local(entries, weapon);
    if (allowed && unit_index != -1) {
        unit_datum *unit = (unit_datum *)entries[datum_index_to_absolute_index(unit_index)].datum;
        bool primary = weapon_index == unit_current_weapon_object_get(unit);
        bool secondary = weapon_index == weapons_reload_get_weapon(unit, unit->unit.secondary_weapon_index);
        if (primary || secondary) {
            bool submitted = false;
            if (magazine_index == 0)
                submitted = action_submit_event(unit_index, primary ? 61 : 63);
            else if (magazine_index == 1)
                submitted = action_submit_event(unit_index, primary ? 62 : 64);
            if (submitted) {
                simulation_action_unit_reload(unit_index, primary, secondary);
                result = true;
            }
        }
    }
    return result;
}



// match: 0x349e00: calling weapon_submit_reload_action directly swaps r24/r25 between the reload result and the weapon index (5 words)
static __forceinline bool weapons_owner_submit_reload_action(long weapon_index, short magazine_index, bool forced)
{
    return weapon_submit_reload_action(weapon_index, magazine_index, forced);
}
// @address 0x349e00 ?weapon_magazine_update@@YA_NJJ@Z
bool weapon_magazine_update(long weapon_index, long magazine_index)
{
    bool result = false;
    weapon_datum *weapon = weapon_get(weapon_index);
    s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, magazine_index);
    if (magazine_definition->rounds_recharged > 0 && magazine->rounds_loaded < magazine_definition->rounds_loaded_maximum) {
        short rounds = (short)(magazine_definition->rounds_recharged / game_tick_rate());
        magazine->rounds_recharge_remainder += magazine_definition->rounds_recharged % game_tick_rate();
        if (magazine->rounds_recharge_remainder >= game_tick_rate()) {
            rounds++;
            magazine->rounds_recharge_remainder -= game_tick_rate();
        }
        if (rounds > 0) {
            magazine->rounds_loaded += rounds;
            if (magazine->rounds_loaded > magazine_definition->rounds_loaded_maximum)
                magazine->rounds_loaded = magazine_definition->rounds_loaded_maximum;
            weapon_set_state_update_timer(weapon_index);
            simulation_action_weapon_state_update(weapon_index);
        }
        result = true;
    }
    if (magazine->ticks > 0) {
        magazine->ticks--;
        result = true;
    }
    if (magazine->pending_reload > 0) {
        magazine->pending_reload--;
        result = true;
    }
    if (magazine->fire_id > 0) {
        magazine->fire_id--;
        result = true;
    }
    if (magazine->state != 0)
        result = true;
    switch (magazine->state) {
    case 1:
    case 2:
    case 3:
        if (magazine->fire_id > magazine->ticks)
            magazine->fire_id = magazine->ticks;
        if (magazine->fire_id == 0)
            weapon_magazine_update_reload(weapon_index, (short)magazine_index);
        if (magazine->ticks == 0) {
            bool infinite_ammo = ::game_engine_player_has_infinite_ammo(weapon_index);
            long rounds_inventory = weapon_get_rounds_inventory(weapon_index, magazine_index, false);
            bool reloading = false;
            bool finished = false;
            if (magazine->state == 1) {
                ::weapon_magazine_set_state(weapon_index, (short)magazine_index, 0);
                finished = true;
            } else {
                ::weapon_magazine_set_state(weapon_index, (short)magazine_index, 4);
            }
            bool barrel0_firing = test_bit(weapon->control_flags, 1) && weapon_barrel_can_fire(weapon_index, 0, true);
            bool barrel1_firing = test_bit(weapon->control_flags, 2) && weapon_barrel_can_fire(weapon_index, 1, true);
            if (!finished && (rounds_inventory > 0 || infinite_ammo) &&
                magazine->rounds_loaded < magazine_definition->rounds_loaded_maximum &&
                !test_bit(magazine_definition->flags, 0) && !barrel0_firing && !barrel1_firing)
                reloading = weapons_owner_submit_reload_action(weapon_index, (short)magazine_index, magazine->state == 4);
            if (!reloading)
                weapon_magazine_finish_reload(weapon_index, (short)magazine_index, barrel0_firing | barrel1_firing);
        }
        break;
    case 5:
        weapon_magazine_start_chamber(weapon_index, (short)magazine_index);
        break;
    case 6:
        if (magazine->ticks == 0)
            ::weapon_magazine_set_state(weapon_index, (short)magazine_index, 0);
        break;
    }
    return result;
}

// @address 0x34e9d8 ?weapon_detonate@@YAXJ@Z
void weapon_detonate(long weapon_index)
{
    if (!game_simulation_predicted())
    {
        weapon_definition *definition = weapon_definition_get(object_get(weapon_index)->definition_index);
        weapon_effect_new(weapon_index, -1, definition->detonation_effect.index, 0.0f, 0.0f);
        damage_effect_new_on_weapon(weapon_index, definition->detonation_damage_effect.index);
        object_delete(weapon_index);
    }
}

static __forceinline bool weapons_item_in_hand(long object_index)
{
    bool result = false;
    if (weapon_get(object_index)->item.owner_relation == 1)
        result = true;
    return result;
}
static __forceinline bool weapons_weapon_is_stowed(long object_index)
{
    bool result = false;
    if (weapon_get(object_index)->item.owner_relation == 3)
        result = true;
    return result;
}



static __forceinline float game_engine_bomb_get_arming_theta(long weapon_index)
{
    if (current_game_variant()->engine_type == 9)
        return assault_bomb_get_arming_theta(weapon_index);
    return 0.0f;
}

// @address 0x1009f0 ?weapon_compute_function_value@@YA_NJJJPAMPA_N1@Z
bool weapon_compute_function_value(long object_index, long function_name, long argument, float *value, bool *active, bool *deterministic)
{
    weapon_datum *weapon = weapon_get(object_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    float function_value = 0.0f;
    bool result = true;
    switch (function_name) {
    case _string_id_ready:
        if (weapons_item_in_hand(object_index))
            function_value = 1.0f;
        break;
    case _string_id_active_camouflage:
    {
        long owner_index = weapon_owner_unit_index_get(object_index);
        if (owner_index != -1)
            unit_compute_function_value(owner_index, _string_id_active_camouflage, argument, &function_value, active, deterministic);
        break;
    }
    case _string_id_turned_on:
        if (!weapons_weapon_is_stowed(object_index)
            && (!weapons_item_in_hand(object_index) || weapon->weapon_flags.test(5))
            && weapon_get_age(object_index) < 1.0f)
            function_value = 1.0f;
        break;
    case _string_id_turning_on:
        if (weapons_item_in_hand(object_index) && !weapon->weapon_flags.test(5) && weapon_get_age(object_index) < 1.0f)
            function_value = 1.0f;
        break;
    case _string_id_heat:
        function_value = weapon->heat;
        break;
    case _string_id_overheated:
        if (weapon->weapon_flags.test(1) && definition->heat_recovery_threshold != 1.0f)
            function_value = (weapon->heat - definition->heat_recovery_threshold) / (1.0f - definition->heat_recovery_threshold);
        break;
    case _string_id_illumination:
    {
        long barrel_index;
        for (barrel_index = 0; barrel_index < definition->barrel_count; barrel_index++) {
            float barrel_value = weapon->barrels[barrel_index].illumination_fraction;
            if (function_value <= barrel_value)
                function_value = barrel_value;
        }
        long trigger_index;
        for (trigger_index = 0; trigger_index < definition->trigger_count; trigger_index++) {
            s_weapon_trigger_definition *trigger_definition = &definition->triggers[trigger_index];
            if (trigger_definition->charging_time > 0.0f) {
                float charged_value = weapon_trigger_get_charged_fraction(object_index, (short)trigger_index) * trigger_definition->charged_illumination;
                if (function_value <= charged_value)
                    function_value = charged_value;
            }
            if (weapon->triggers[trigger_index].state == 2) {
                float active_value = trigger_definition->charged_illumination + (1.0f - trigger_definition->charged_illumination) * weapon->f198;
                if (function_value <= active_value)
                    function_value = active_value;
            }
        }
        float heat_value = definition->heat_illumination * weapon->heat;
        if (function_value <= heat_value)
            function_value = heat_value;
        break;
    }
    case _string_id_warthog_chaingun_illumination:
        if (!tls_object_runtime_globals()->warthog_chaingun_light_disabled)
            return weapon_compute_function_value(object_index, _string_id_illumination, argument, value, active, deterministic);
        function_value = 0.0f;
        break;
    case _string_id_primary_ammunition:
    case _string_id_secondary_ammunition:
    {
        long magazine_index = (function_name != _string_id_primary_ammunition);
        if (magazine_index >= definition->magazines.s_tag_block::count)
            break;
        short maximum_rounds = definition->magazines[magazine_index].rounds_loaded_maximum;
        if (maximum_rounds == 0)
            break;
        function_value = (float)weapon->magazines[magazine_index].rounds_loaded / (float)maximum_rounds;
        break;
    }
    case _string_id_primary_ammunition_ones:
        if (definition->magazines.s_tag_block::count > 0) {
            short rounds = weapon->magazines[0].rounds_loaded;
            function_value = (float)(rounds % 10) * 0.1f;
        }
        break;
    case _string_id_primary_ammunition_tens:
        if (definition->magazines.s_tag_block::count > 0) {
            short rounds = weapon->magazines[0].rounds_loaded;
            function_value = (float)((rounds / 10) % 10) * 0.1f;
        }
        break;
    case _string_id_primary_ejection_port:
    case _string_id_secondary_ejection_port:
    {
        long barrel_index = (function_name != _string_id_primary_ejection_port);
        if (barrel_index < definition->barrel_count)
            function_value = weapon->barrels[barrel_index].ejection_port;
        break;
    }
    case _string_id_primary_rate_of_fire:
    case _string_id_secondary_rate_of_fire:
    {
        long barrel_index = (function_name != _string_id_primary_rate_of_fire);
        if (barrel_index < definition->barrel_count)
            function_value = weapon->barrels[barrel_index].rate_fraction;
        break;
    }
    case _string_id_primary_barrel_firing:
    case _string_id_secondary_barrel_firing:
    case _string_id_primary_barrel_firing_single:
    case _string_id_primary_barrel_firing_dual_left:
    case _string_id_primary_barrel_firing_dual_right:
    {
        long barrel_index = (function_name == _string_id_secondary_barrel_firing);
        bool barrel_active = true;
        if (function_name == _string_id_primary_barrel_firing_single || function_name == _string_id_primary_barrel_firing_dual_left ||
            function_name == _string_id_primary_barrel_firing_dual_right) {
            long owner_index = weapon_owner_unit_index_get(object_index);
            if (owner_index != -1) {
                long primary_weapon = seat_object(owner_index, 0);
                long secondary_weapon = seat_object(owner_index, 1);
                switch (function_name) {
                case _string_id_primary_barrel_firing_single:
                    barrel_active = secondary_weapon == -1 && primary_weapon == object_index;
                    break;
                case _string_id_primary_barrel_firing_dual_left:
                    barrel_active = secondary_weapon == object_index && primary_weapon != -1;
                    break;
                case _string_id_primary_barrel_firing_dual_right:
                    barrel_active = secondary_weapon != -1 && primary_weapon == object_index;
                    break;
                }
            }
        }
        if (barrel_index < definition->barrel_count) {
            if (barrel_active && weapon->barrels[barrel_index].state_1 == 1)
                function_value = 1.0f;
            else
                function_value = 0.0f;
        }
        break;
    }
    case _string_id_primary_firing:
    case _string_id_secondary_firing:
    {
        long barrel_index = (function_name != _string_id_primary_firing);
        if (barrel_index < definition->barrel_count) {
            function_value = weapon->barrels[barrel_index].rate_fraction;
            if (game_time_globals->time - weapon->game_time_last_fired > 1)
                function_value = 0.0f;
        }
        break;
    }
    case _string_id_primary_charged:
    case _string_id_secondary_charged:
    {
        long trigger_index = (function_name != _string_id_primary_charged);
        if (trigger_index < definition->trigger_count)
            function_value = weapon_trigger_get_charged_fraction(object_index, (short)trigger_index);
        break;
    }
    case _string_id_power:
        function_value = weapon->current_power;
        break;
    case _string_id_age:
        function_value = weapon_get_age(object_index);
        break;
    case _string_id_primary_trigger_down:
        if (test_bit(weapon->control_flags, 1) && weapon_barrel_can_fire(object_index, 0, false))
            function_value = 1.0f;
        break;
    case _string_id_secondary_trigger_down:
        if (test_bit(weapon->control_flags, 2))
            function_value = 1.0f;
        break;
    case _string_id_barrel_spin:
        function_value = (float)weapon->analog_value * 0.003921569f;
        break;
    case _string_id_barrel_spin_rate:
        if (definition->barrel_count > 0)
            function_value = weapon->barrels[0].rate_fraction;
        break;
    case _string_id_weapon_battery_empty:
        function_value = weapon_get_age(object_index) >= 1.0f ? 1.0f : 0.0f;
        break;
    case _string_id_bomb_arming_amount:
        function_value = 0.0f;
        if (game_is_multiplayer() && weapon->multiplayer_weapon_identifier != -1 && game_engine_weapon_is_bomb(object_index))
            function_value = game_engine_bomb_get_arming_theta(object_index);
        break;
    case _string_id_bomb_is_unarmed:
    case _string_id_bomb_is_arming:
    case _string_id_bomb_is_armed:
        function_value = 0.0f;
        if (game_is_multiplayer() && weapon->multiplayer_weapon_identifier != -1 && game_engine_weapon_is_bomb(object_index)) {
            float arming_theta = game_engine_bomb_get_arming_theta(object_index);
            if (function_name == _string_id_bomb_is_unarmed && arming_theta == 0.0f)
                function_value = 1.0f;
            else if (function_name == _string_id_bomb_is_arming && arming_theta > 0.0f && arming_theta < 1.0f)
                function_value = 1.0f;
            else if (function_name == _string_id_bomb_is_armed && arming_theta == 1.0f)
                function_value = 1.0f;
        }
        break;
    case _string_id_flashlight_intensity:
    {
        function_value = 0.0f;
        long owner_index = weapon_owner_unit_index_get(object_index);
        if (owner_index != -1 && !weapons_weapon_is_stowed(object_index)) {
            weapon_datum *owner = weapon_get(owner_index);
            unit_definition *owner_definition =
                TAG_GET('unit', unit_definition, owner->definition_index);
            if (test_bit(~owner_definition->flags, 17)) {
                bool function_active;
                function_value = object_get_function_value(owner_index, _string_id_integrated_light_power, argument, &function_value, &function_active);
            }
        }
        break;
    }
    case _string_id_stowed:
        function_value = weapons_weapon_is_stowed(object_index) ? 1.0f : 0.0f;
        break;
    default:
        result = false;
        break;
    }
    if (result) {
        function_value = PIN(function_value, 0.0f, 1.0f);
        *value = function_value;
        *active = function_value > 0.0f;
    }
    return result;
}

// Unplaced ODR use: /GL emits the `inline` COMDAT (f i) body of playing() only when referenced in this TU.
typedef bool (c_animation_channel::*weapons_animation_channel_playing_member)() const;
weapons_animation_channel_playing_member weapons_animation_channel_playing_odr_use() { return &c_animation_channel::playing; }
static __forceinline const s_weapon_barrel *weapons_can_fire_barrel_get(const weapon_datum *weapon, short barrel_index)
{
    return &weapon->barrels[barrel_index];
}
static __forceinline bool weapons_barrel_is_recovering(long weapon_index,short barrel_index)
{
    weapon_datum *weapon=weapon_get(weapon_index);
    const s_weapon_barrel *barrel=weapons_can_fire_barrel_get(weapon,barrel_index);
    bool result=barrel->state_1==2 || barrel->state_1==3;
    return result;
}

static __forceinline s_weapon_barrel_definition *weapons_can_fire_barrel_definition_get(const weapon_definition *definition,long barrel_index)
{
 return &definition->barrels[barrel_index];
}
static __forceinline bool weapons_can_fire_unit_is_boosting(long unit_index)
{
 bool boosting=false;
 if(unit_compute_boost_fraction(unit_index)>0.0f) boosting=true;
 return boosting;
}
static __forceinline const s_weapon_magazine *weapons_can_fire_magazine_get(const weapon_datum *weapon,short magazine_index)
{
 return &weapon->magazines[magazine_index];
}
static __forceinline short weapons_can_fire_rounds_loaded(const weapon_datum *weapon,short magazine_index)
{
 return weapon->magazines[magazine_index].rounds_loaded;
}
static __forceinline const s_weapon_trigger *weapons_can_fire_trigger_get(const weapon_datum *weapon,long trigger_index)
{
 return &weapon->triggers[trigger_index];
}
// @address 0x34c410 ?weapon_barrel_can_fire@@YA_NJF_N@Z
bool weapon_barrel_can_fire(long weapon_index, short barrel_index, bool ignore_reload)
{
    bool result = false;
    const weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    if (barrel_index >= 0 && barrel_index < definition->barrel_count)
    {
        const s_weapon_barrel *barrel = weapons_can_fire_barrel_get(weapon, barrel_index);
        s_weapon_barrel_definition *barrel_definition = weapons_can_fire_barrel_definition_get(definition,barrel_index);
        long owner_index = weapon_owner_unit_index_get(weapon_index);
        long parent_index;
        if (!((definition->m_flags.test(_weapon_definition_cannot_fire_at_maximum_age) && weapon_has_infinite_ammo(weapon_index) && (barrel->flags & 64)) ||
              (definition->m_flags.test(_weapon_definition_cannot_fire_if_parent_dead) &&
               (parent_index=weapon->item.object.parent_object_index)!=-1 &&
               bit_test(((object_datum *)object_header_get(parent_index)->datum)->damage_flags,2))) &&
            !(barrel->flags & 4) && !weapons_barrel_is_recovering(weapon_index,barrel_index))
        {
            do {
                if (definition->m_flags.test(_weapon_definition_cannot_fire_while_boosting) && owner_index != -1 &&
                    ((1 << object_header_get(owner_index)->object_type) & 2))
                {
                    if (weapons_can_fire_unit_is_boosting(owner_index)) break;
                }
                if (!weapon->weapon_flags.test(1))
                {
                    if (definition->trigger_count == 1 && definition->barrel_count == 2 &&
                        trigger_definition_get(definition, 0)->behavior == 5 && weapons_barrel_is_recovering(weapon_index, !barrel_index))
                        break;
                    if (definition->trigger_count == 1 && trigger_definition_get(definition, 0)->behavior == 1 &&
                        !(weapons_can_fire_trigger_get(weapon, 0)->flags & 1))
                        break;
                    short magazine_index = barrel_definition->magazine_index;
                    if (magazine_index != -1)
                    {
                        const s_weapon_magazine *magazine = weapons_can_fire_magazine_get(weapon,magazine_index);
                        bool enough_rounds = magazine->rounds_loaded >= barrel_definition->rounds_per_shot &&
                            (magazine->rounds_loaded >= barrel_definition->minimum_rounds || bit_test(barrel_definition->flags,2));
                        if (!enough_rounds && (ignore_reload || (barrel->flags & 64)))
                            break;
                        if ((weapon->time_184 > 0 || magazine->fire_id > 0) && !ignore_reload)
                        {
                            if ((unsigned short)definition->reload_interrupt_mode == 1 && enough_rounds)
                            {
                                if (magazine->state != 0)
                                    weapon_magazine_finish_reload(weapon_index, magazine_index, true);
                                result = true;
                            }
                        }
                        else
                            result = true;
                    }
                    else
                        result = true;
                }
            } while (0);
        }
    }
    return result;
}

static __forceinline float weapons_bcf0_firing_duration(long weapon_index, short barrel_index, float ticks_per_second)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
    float rate = barrel_definition->rounds_per_second_lower +
        (barrel_definition->rounds_per_second_upper - barrel_definition->rounds_per_second_lower) * barrel->rate_fraction;
    float duration = !(rate <= 0.0001f) ? ticks_per_second / rate : 0.0f;
    if (!(definition->heat_rate_scale <= 0.0f))
        duration = (weapon->age * definition->heat_rate_scale + 1.0f) * duration;
    return duration;
}

static __forceinline bool weapons_bcf0_barrel_is_recovering(s_weapon_barrel const *barrel)
{
    bool result = barrel->state_1 == 2 || barrel->state_1 == 3;
    return result;
}
// @address 0x34bcf0 weapon_barrel_is_ready_to_fire
// match: 0x34bcf0: without __restrict on shots and the recovery pointers the function grows 16 bytes (extsh barrel_index hoists, GPRs shift)
extern "C" bool weapon_barrel_is_ready_to_fire(long weapon_index, short barrel_index, long *__restrict shots,
    // match: 0x34bcf0: see above; recovery_ticks/recovery_duration are part of the same 16-byte change
    long *__restrict recovery_ticks, long *__restrict recovery_duration, long *firing_ticks, long *firing_duration)
{
    bool result = true;
    weapon_datum *weapon = weapon_get(weapon_index);
    s_weapon_barrel *barrel = weapon_barrel_get(weapon_get(weapon_index), barrel_index);
    float rate_threshold = g_network_configuration.weapon_ticks_threshold;
    float ticks_per_second = (float)game_time_globals->ticks_per_second;
    float minimum = game_seconds_to_ticks_real(g_network_configuration.weapon_ticks_minimum);
    if (shots) *shots = barrel->value;
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    float recovery = (1.0f - barrel_definition->recovery_fraction) * (barrel_definition->recovery_time * ticks_per_second);
    if (weapons_bcf0_barrel_is_recovering(weapon_barrel_get(weapon, barrel_index)) && !(recovery - minimum < 0.0001f))
    {
        if (!((float)(short)barrel->ticks < (1.0f - rate_threshold) * recovery))
        {
            result = false;
            if (recovery_ticks) *recovery_ticks = (short)barrel->ticks;
            if (recovery_duration) *recovery_duration = (long)recovery;
        }
    }
    float duration = weapons_bcf0_firing_duration(weapon_index, barrel_index, ticks_per_second);
    if (barrel->value != 0 && !(duration - minimum < 0.0001f) && !((float)barrel->active + 1.0f >= duration * rate_threshold))
    {
        result = false;
        if (firing_ticks) *firing_ticks = barrel->active;
        if (firing_duration) *firing_duration = (long)duration;
    }
    return result;
}


static __forceinline long weapons_unit_get_ultimate_weapon_control_unit_index(long unit_index)
{
    long result = ((unit_datum *)weapon_get(unit_index))->unit.gunner_object_index;
    if (result != -1)
    {
        long control_unit_index;
        do
        {
            control_unit_index = result;
            result = ((unit_datum *)weapon_get(control_unit_index))->unit.gunner_object_index;
        } while (result != -1);
        result = control_unit_index;
    }
    return result;
}
static __forceinline long object_ultimate_parent_get(long object_index)
{
    long result = -1;
    while (object_index != -1)
    {
        result = object_index;
        object_index = ((unit_datum *)weapon_get(object_index))->object.parent_object_index;
    }
    return result;
}
static __forceinline void weapons_unit_interrupt_motor_task(long unit_index, e_action action)
{
    c_motor_task *task = motor_task_find_by_action(unit_index, action);
    if (task)
        motor_system_interrupted(unit_index, task, true);
}
static __forceinline void weapons_campaign_player_fired_weapon(long player_index)
{
    if (game_is_campaign() && game_campaign_is_multiplayer())
    {
        c_player_in_game_iterator iterator;
        iterator.begin();
        long waypoint_index = 0;
        while (iterator.next())
        {
            if (iterator.get_index() == player_index)
                game_engine_fade_set_state(&chud_navpoint_globals()->player_waypoints[waypoint_index], 1);
            waypoint_index++;
        }
    }
}
static __forceinline long weapons_fire_weapon_type(const weapon_definition *definition)
{
    return (short)(unsigned short)definition->reload_interrupt_mode;
}
static __forceinline s_weapon_magazine *weapons_fire_magazine_get(weapon_datum *weapon, short magazine_index)
{
    return &weapon->magazines[magazine_index];
}
static __forceinline void weapons_player_control_permanent_impulse(long user_index, const real_euler_angles2d *impulse)
{
    player_control_modify_desired_angles(user_index, impulse->yaw, impulse->pitch, 0.0f, 0.0f);
    player_control_update_state_actions(user_index);
}

// @address 0x34d410 ?weapon_barrel_fire@@YAXJF_N@Z
void weapon_barrel_fire(long weapon_index, short barrel_index, bool force)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
    long damage_effect_index = -1;
    long rider_damage_effect_index = -1;
    long firing_effect_index = -1;
    float scale_b = 0.0f;
    float scale_a = 0.0f;
    bool fired = false;
    bool misfire = false;
    bool first_shot = barrel->value == 0;
    long rounds = barrel_definition->minimum_rounds;
    long extra_shots = 0;

    object_wake(weapon_index);
    if (barrel->fire_time >= 1.0f)
    {
        extra_shots = (long)real_floor(barrel->fire_time);
        rounds = rounds * (extra_shots + 1);
    }

    if (barrel_definition->magazine_index != -1)
    {
        s_weapon_magazine_definition *magazine_definition = magazine_definition_get(definition, barrel_definition->magazine_index);
        s_weapon_magazine *magazine = weapons_fire_magazine_get(weapon, barrel_definition->magazine_index);
        short rounds_loaded = magazine->rounds_loaded;
        if (rounds_loaded < rounds && rounds_loaded >= barrel_definition->minimum_rounds && !force)
        {
            extra_shots = 0;
            rounds = barrel_definition->minimum_rounds;
        }
        if ((rounds_loaded >= rounds || bit_test(barrel_definition->flags, 2) || force) &&
            !(definition->m_flags.test(_weapon_definition_cannot_fire_at_maximum_age) && !bit_test(barrel_definition->flags, 14) &&
              weapon_has_infinite_ammo(weapon_index) && !force) &&
            (rounds_loaded >= barrel_definition->rounds_per_shot || force) &&
            !(definition->m_flags.test(_weapon_definition_cannot_fire_while_boosting) && owner_unit_index != -1 &&
              ((1 << object_get_type(owner_unit_index)) & 2) && weapons_can_fire_unit_is_boosting(owner_unit_index)) &&
            !(barrel->flags & 4))
        {
            if (rounds > 0 && (weapon->item.inventory_flags & 2))
            {
                magazine->rounds_loaded = rounds_loaded - (short)rounds;
                weapon_set_state_update_timer(weapon_index);
            }
            if (magazine->rounds_loaded <= 0)
                magazine->rounds_loaded = 0;
            else if (magazine_definition->flags & 2)
                weapon_magazine_set_state(weapon_index, barrel_definition->magazine_index, 5);
            if ((unsigned short)definition->reload_interrupt_mode == 1)
                magazine->pending_reload = (short)game_seconds_to_ticks_round(1.0f);
            fired = true;
        }
    }
    else
    {
        if (!(definition->m_flags.test(_weapon_definition_cannot_fire_at_maximum_age) && !bit_test(barrel_definition->flags, 14) &&
              weapon_has_infinite_ammo(weapon_index) && !force))
            fired = true;
    }
    if (fired)
        simulation_action_weapon_state_update(weapon_index);

    if (barrel_definition->firing_effect_count > 0)
    {
        if (barrel->current_firing_effect_shots <= 0)
        {
            short last_firing_effect = barrel->current_firing_effect;
            short firing_effect_block_index = bit_test(barrel_definition->flags, 1) ?
                _random(tls_random_seed(), __FILE__, __FUNCTION__, __LINE__) % barrel_definition->firing_effect_count : last_firing_effect;
            do
            {
                if (barrel->firing_effect_mask == (1 << barrel_definition->firing_effect_count) - 1)
                    barrel->firing_effect_mask = 0;
                do
                {
                    firing_effect_block_index++;
                    if (firing_effect_block_index >= barrel_definition->firing_effect_count)
                        firing_effect_block_index = 0;
                } while (barrel->firing_effect_mask & (1 << firing_effect_block_index));
                s_barrel_firing_effect *firing_effect = &barrel_definition->firing_effects[firing_effect_block_index];
                barrel->current_firing_effect = firing_effect_block_index;
                barrel->firing_effect_mask |= 1 << firing_effect_block_index;
                if (firing_effect->shot_count_lower_bound == firing_effect->shot_count_upper_bound)
                    barrel->current_firing_effect_shots = firing_effect->shot_count_lower_bound;
                else
                    barrel->current_firing_effect_shots = random_range(
                        tls_random_seed(), firing_effect->shot_count_lower_bound, firing_effect->shot_count_upper_bound);
            } while (barrel->current_firing_effect_shots <= 0 && firing_effect_block_index != last_firing_effect);
        }
        barrel->current_firing_effect_shots--;
        s_barrel_firing_effect *firing_effect = &barrel_definition->firing_effects[barrel->current_firing_effect];
        if (definition->misfire_start > 0.0f && definition->misfire_start < 1.0f)
        {
            float heat = weapon_get_age(weapon_index);
            if (heat > definition->misfire_start && !bit_test(barrel_definition->flags, 14))
            {
                float misfire_probability = definition->misfire_chance * (heat - definition->misfire_start) / (1.0f - definition->misfire_start);
                if (_real_random(tls_random_seed(), __FILE__, __FUNCTION__, __LINE__) < misfire_probability)
                    misfire = true;
            }
        }
        short effect_type;
        if (!fired)
        {
            effect_type = 2;
            scale_a = 1.0f;
            scale_b = 0.0f;
            barrel->flags |= 0x40;
            if (barrel_definition->prediction_type != 1)
                simulation_action_weapon_empty_click(weapon_index);
        }
        else
        {
            scale_a = barrel->rate_fraction;
            if (misfire)
            {
                effect_type = 1;
                scale_b = 0.0f;
            }
            else
            {
                effect_type = 0;
                scale_b = definition->overheated_threshold == 0.0f ? 0.0f : weapon->heat / definition->overheated_threshold;
                if (!first_shot && firing_effect->effects[3].index != -1)
                    effect_type = 3;
            }
        }
        firing_effect_index = firing_effect->effects[effect_type].index;
        damage_effect_index = firing_effect->damage_effects[effect_type].index;
        rider_damage_effect_index = firing_effect->rider_damage_effects[effect_type].index;
    }

    if (fired)
    {
        if (extra_shots != 0)
            barrel->fire_time -= (float)extra_shots;
        barrel->value = barrel->value + 1 + (short)extra_shots;
        barrel->extra_shots = (short)extra_shots;
        barrel->shot_count++;
        weapon->game_time_last_fired = game_time_globals->time;
        if ((unsigned short)definition->reload_interrupt_mode == 1 && owner_unit_index != -1)
        {
            weapons_unit_interrupt_motor_task(owner_unit_index, _action_primary_weapon_primary_reload);
            weapons_unit_interrupt_motor_task(owner_unit_index, _action_primary_weapon_secondary_reload);
            weapons_unit_interrupt_motor_task(owner_unit_index, _action_secondary_weapon_primary_reload);
            weapons_unit_interrupt_motor_task(owner_unit_index, _action_secondary_weapon_secondary_reload);
        }
        e_unit_messages message;
        if (misfire)
            message = (e_unit_messages)((barrel_index != 0) + 3);
        else
            message = (e_unit_messages)((barrel_index != 0) + 1);
        weapon_send_message_to_unit(weapon_index, message);
        weapon_barrel_start_ejection_port(weapon_index, barrel_index, false);
        if (barrel_definition->ejection_port_recovery_time > 0.0f)
            barrel->illumination_fraction = 1.0f;
        long projectile_owner_index = weapon_get_projectile_owner_unit_index(weapon_index);
        if (projectile_owner_index != -1)
        {
            unit_datum *projectile_owner = (unit_datum *)weapon_get(projectile_owner_index);
            if (projectile_owner->unit.player_index != -1)
            {
                if (game_is_multiplayer())
                    game_engine_player_set_fade_state(projectile_owner->unit.player_index, 1);
                else if (game_campaign_is_multiplayer())
                    weapons_campaign_player_fired_weapon(projectile_owner->unit.player_index);
            }
        }
        if (owner_unit_index != -1 && ((unit_datum *)weapon_get(owner_unit_index))->unit.player_index != -1)
            unit_active_camouflage_ding(owner_unit_index, definition->active_camo_ding, definition->active_camo_regrowth_rate);
        weapon->heat += barrel_definition->heat_recovery_threshold;
        if (!(weapon->item.inventory_flags & 2) && weapon->heat > definition->overheated_threshold)
            weapon->heat = definition->overheated_threshold;
        else if (weapon->heat > 1.0f)
            weapon->heat = 1.0f;
        if (!definition->m_flags.test(_weapon_definition_weapon_ages))
        {
            float age = barrel_definition->amount_b;
            if (game_is_multiplayer() || age == 0.0f)
                age = barrel_definition->amount_a;
            if (age > 0.0f)
            {
                if (bit_test(barrel_definition->flags, 15))
                    age = first_shot ? (float)barrel_definition->shots_minimum * age : 0.0f;
                weapon_age(weapon_index, age);
            }
        }
        weapon_set_state(weapon_index, barrel_index ? 2 : 1, false);
        if (!misfire)
        {
            barrel->flags |= 2;
            weapon->weapon_flags.set(0, true);
            long firing_noise = barrel_definition->firing_noise;
            if (owner_unit_index != -1 && ((1 << object_get_type(owner_unit_index)) & 0x1003))
                ai_handle_unit_effect(owner_unit_index, (e_sound_type)1, (e_ai_sound_volume)firing_noise);
            if (firing_noise == 2)
                tls_object_runtime_globals()->last_loud_weapon_fire_time = game_time_globals->time;
        }
        if (owner_unit_index != -1 && damage_effect_index != -1)
        {
            long ultimate_control_unit_index = weapons_unit_get_ultimate_weapon_control_unit_index(owner_unit_index);
            long control_unit_index;
            if (ultimate_control_unit_index == -1)
                control_unit_index = owner_unit_index;
            else
                control_unit_index = ultimate_control_unit_index;
            unit_cause_self_damage(control_unit_index, damage_effect_index, 1.0f);
            long parent_index;
            if (rider_damage_effect_index != -1 && (parent_index = object_ultimate_parent_get(control_unit_index)) != -1 &&
                object_get_type(parent_index) == 1)
            {
                c_multi_unit_passenger_iterator iterator;
                iterator.begin(parent_index);
                while (iterator.next())
                {
                    if (iterator.passenger_index != control_unit_index &&
                        ((unit_datum *)weapon_get(iterator.passenger_index))->unit.player_index != -1)
                        unit_cause_self_damage(iterator.passenger_index, rider_damage_effect_index, 1.0f);
                }
            }
        }
        if ((weapons_fire_weapon_type(definition) == 3 || weapons_fire_weapon_type(definition) == 7) && barrel_index == 1)
            weapon->weapon_flags.set(3, true);
        if (owner_unit_index != -1)
        {
            long player_index = ((unit_datum *)weapon_get(owner_unit_index))->unit.player_index;
            long user_index = -1;
            while (true)
            {
                if (player_index != -1)
                    user_index = (long)::director_get_perspective(player_index, user_index);
                if (user_index == -1)
                    break;
                float recoil = barrel_definition->firing_recoil_minimum +
                    transition_function_evaluate(barrel_definition->firing_recoil_function, barrel->error) *
                    (barrel_definition->firing_recoil_maximum - barrel_definition->firing_recoil_minimum);
                real_euler_angles2d impulse;
                impulse.yaw = 0.0f;
                impulse.pitch = recoil;
                weapons_player_control_permanent_impulse(user_index, &impulse);
            }
            unit_unzoom(owner_unit_index, true);
        }
    }

    if (weapon->heat > definition->heat_detonation_threshold)
    {
        if (_real_random(tls_random_seed(), __FILE__, __FUNCTION__, __LINE__) < definition->heat_detonation_fraction)
            weapon_detonate(weapon_index);
    }
    barrel->active = 0;
    if (!bit_test(barrel_definition->flags, 15) || !(barrel->flags & 0x80))
    {
        if (bit_test(barrel_definition->flags, 12))
        {
            c_tag_index effect_index;
            c_tag_index *effect_index_address = &effect_index;
            new (effect_index_address) c_tag_index(firing_effect_index);
            long marker_index = barrel->shot_count;
            effect_create_on_object(*effect_index_address, 0, weapon_get_effect_object_index(weapon_index), scale_a, scale_b, 0, 0, 1, marker_index);
        }
        else
            barrel->firing_effect_index = weapon_effect_new(weapon_index, barrel->firing_effect_index, firing_effect_index, scale_a, scale_b);
    }
    barrel->flags |= 0x80;
}


// @address 0x3516c0 weapon_replace_with_replacement
extern "C" long weapon_replace_with_replacement(long weapon_index)
{
    long result_index = weapon_index;
    unsigned long absolute_index = (unsigned short)weapon_index;
    object_datum *weapon = (object_datum *)((s_object_header *)object_header_data->data)[absolute_index].datum;
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    if (!game_simulation_predicted() && definition->replacement_definition_index != NONE)
    {
        object_placement_data placement;
        object_placement_data_new(&placement, definition->replacement_definition_index, NONE, 0);
        placement.position = weapon->position;
        placement.forward = weapon->forward;
        placement.up = weapon->up;
        placement.linear_velocity = weapon->linear_velocity;
        placement.angular_velocity = weapon->angular_velocity;
        weapon_index = object_new(&placement);
        if (weapon_index != NONE)
        {
            object_delete(result_index);
            result_index = weapon_index;
        }
    }
    return result_index;
}



static __forceinline bool weapons_create_game_is_distributed()
{
    bool distributed = false;
    if ((unsigned long)(game_globals->options.simulation_type - 4) <= 1)
        distributed = true;
    return distributed;
}

static __forceinline bool weapons_create_unit_is_dual_wielding(long unit_index)
{
    unit_datum *unit = (unit_datum *)weapon_get(unit_index);
    bool result = false;
    if (unit->unit.current_weapon_set.indices[0] != 0xff && unit->unit.current_weapon_set.indices[1] != 0xff)
        result = true;
    return result;
}

static __forceinline unsigned char weapons_create_reporting_info(unsigned char type, unsigned char modifiers)
{
    unsigned char reporting_info;
    reporting_info = (reporting_info & 3) | (type << 2);
    reporting_info = (reporting_info & ~3) | modifiers;
    return reporting_info;
}

static __forceinline bool weapons_create_early_mover_velocity(long unit_index, real_vector3d *velocity, real_vector3d *axis)
{
    real_point3d center_of_mass;
    object_get_center_of_mass(unit_index, &center_of_mass);
    return object_get_early_mover_local_space_velocity(unit_index, &center_of_mass, velocity, axis, true, false);
}


// @rdata 0x82024fd4
static const float weapons_create_degrees_to_radians = 0.017453292f;
// @rdata 0x8202490c
static const float weapons_create_epsilon = 0.0001f;

static inline s_simulation_action_projectile_fire_result &weapons_create_result_at(
    c_static_array<s_simulation_action_projectile_fire_result, 16> *results, long index)
{
    return (*results)[index];
}

static __forceinline float weapons_create_dot_jk(const real_vector3d *a, const real_vector3d *b)
{
    float forward_k = b->k;
    float velocity_k = a->k;
    float forward_j = b->j;
    float velocity_j = a->j;
    float j_product = velocity_j * forward_j;
    float result = velocity_k * forward_k + j_product;
    return result;

}

static __forceinline void weapons_create_scale_jk(const real_vector3d *vector, float scale, real_vector3d *result)
{
    result->j = vector->j * scale;
    result->k = vector->k * scale;
}

static __forceinline float weapons_create_projection_jk(const real_vector3d *a, const real_vector3d *b)
{
    float forward_k = b->k;
    float velocity_k = a->k;
    float forward_j = b->j;
    float velocity_j = a->j;
    float k_product = velocity_k * forward_k;
    float result = velocity_j * forward_j + k_product;
    return result;
}

static __forceinline float weapons_create_projection_scale(float forward_i, float velocity_i, const real_vector3d *forward, const real_vector3d *velocity)
{
    return forward_i * velocity_i + weapons_create_projection_jk(forward, velocity);
}

static __forceinline real_vector3d *weapons_create_perpendicular3d(const real_vector3d *vector, real_vector3d *result)
{
    real_vector3d *(*const make_perpendicular)(const real_vector3d *, real_vector3d *) = perpendicular3d;
    return make_perpendicular(vector, result);
}

static inline float weapons_create_normalize3d(real_vector3d *vector)
{
    float length = magnitude3d(vector);
    const float &difference = fabs(length - 0.0f);
    if (!(difference < k_real_epsilon))
        scale_vector3d(vector, 1.0f / length, vector);
    else
        length = 0.0f;
    return length;
}

static __forceinline float weapons_create_rotation_cross_j(const real_vector3d *vector, const real_vector3d *axis)
{
    return vector->k * axis->i - vector->i * axis->k;
}

static __forceinline real_vector3d *weapons_create_rotate_vector(
    real_vector3d *vector, const real_vector3d *axis, float sine, float cosine, float scale, float cross_j)
{
    float cross_i = vector->j * axis->k - vector->k * axis->j;
    float cross_k = vector->i * axis->j - vector->j * axis->i;
    vector->i = scale * axis->i + vector->i * cosine - cross_i * sine;
    vector->j = axis->j * scale + vector->j * cosine - cross_j * sine;
    vector->k = axis->k * scale + vector->k * cosine - cross_k * sine;
    return vector;
}


static __forceinline void weapons_create_mark_result(bool *flag);
static __forceinline unsigned long weapons_create_fire_event_value(bool generate_fire_event);
static __forceinline bool weapons_create_vehicle_inherits_parent(const vehicle_definition *definition);
static __forceinline void weapons_create_rotate_error(
    real_vector3d *vector, const real_vector3d *axis, float sine, float cosine, float cosine_complement, float cross_j);


typedef char weapons_flat_view_physics_flags[(offsetof(weapons_unit_datum_flat_view, physics_flags) ==
                                                 offsetof(unit_datum, object.physics_flags))
        ? 1
        : -1];
typedef char weapons_flat_view_actor_index[
    (sizeof(void *) != 4 || offsetof(weapons_unit_datum_flat_view, actor_index) == offsetof(unit_datum, unit.actor_index)) ? 1 : -1];
typedef char weapons_flat_view_player_index[
    (sizeof(void *) != 4 || offsetof(weapons_unit_datum_flat_view, player_index) == offsetof(unit_datum, unit.player_index)) ? 1 : -1];
typedef char weapons_flat_view_targeting[
    (sizeof(void *) != 4 || offsetof(weapons_unit_datum_flat_view, targeting) == offsetof(unit_datum, unit.targeting)) ? 1 : -1];
typedef char weapons_flat_view_weapon_set[
    (sizeof(void *) != 4 || offsetof(weapons_unit_datum_flat_view, current_weapon_set) == offsetof(unit_datum, unit.current_weapon_set)) ? 1 : -1];
typedef char weapons_flat_view_weapons[
    (sizeof(void *) != 4 || offsetof(weapons_unit_datum_flat_view, weapon_object_indices) == offsetof(unit_datum, unit.weapon_object_indices)) ? 1 : -1];
typedef char weapons_flat_view_gunner[
    (sizeof(void *) != 4 || offsetof(weapons_unit_datum_flat_view, gunner_object_index) == offsetof(unit_datum, unit.gunner_object_index)) ? 1 : -1];

// @address 0x34eba0
void weapon_barrel_create_projectiles(
    long weapon_index, short barrel_index, const s_predicted_weapon_fire_data *predicted_fire_data, long predicted_fire_count, bool force)
{
    weapon_datum *weapon = weapon_get(weapon_index);
    s_weapon_barrel *barrel = weapon_barrel_get(weapon, barrel_index);
    long first_person_offset_index = 0;
    weapon_definition *definition = weapon_definition_get(weapon->definition_index);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    long owner_unit_index = weapon_owner_unit_index_get(weapon_index);
    long projectile_owner_unit_index = weapon_get_projectile_owner_unit_index(weapon_index);
    unsigned long random_seed = weapon_index;
    long random_marker_index = NONE;
    bool create_projectiles = true;
    bool replicate_projectiles = false;
    bool generate_fire_event = false;
    short shot_id = NONE;
    bool dual_wielded = weapon_is_dual_wielded(weapon_index);
    float dual_error = unit_get_weapon_dual_error_theta(owner_unit_index, weapon_index);
    long result_count = 0;
    c_static_array<s_simulation_action_projectile_fire_result, 16> results;
    memset(&results, 0, sizeof(results));
    if (owner_unit_index != NONE)
    {
        unit_datum *owner = (unit_datum *)weapon_get(owner_unit_index);
        if (weapons_create_unit_is_dual_wielding(owner_unit_index))
        {
            long other_weapon_index = NONE;
            short inventory_index = owner->unit.current_weapon_set.weapon_indices[1];
            if (inventory_index != NONE)
                other_weapon_index = owner->unit.weapon_object_indices[inventory_index];
            first_person_offset_index = (weapon_index == other_weapon_index) + 1;
        }
    }
    if (definition->m_flags.test(_weapon_definition_dual_wield_error_simple))
        dual_error = dual_wielded ? 1.0f : 0.0f;
    if (weapons_create_game_is_distributed())
    {
        switch (barrel_definition->prediction_type)
        {
        default:
            generate_fire_event = true;
            replicate_projectiles = true;
            create_projectiles = force && !game_simulation_predicted();
            break;
        case 1:
            break;
        case 2:
            generate_fire_event = true;
            break;
        }
        if (generate_fire_event && !game_simulation_predicted() && !force)
            create_projectiles = false;
    }
    long marker_name = weapon_barrel_get_marker_name(weapon_index, barrel_index);
    object_marker markers[64];
    short marker_count = object_get_markers_by_string_id(weapon_get_effect_object_index(weapon_index), marker_name, markers, 64);
    if (marker_count == 0)
        marker_count = 1;
    if (!bit_test(barrel_definition->flags, 3))
        marker_count = 1;
    real_point3d center;
    if (marker_count > 1)
    {
        center = *global_origin3d;
        for (short marker_index = 0; marker_index < marker_count; marker_index++)
            add_points3d(&markers[marker_index].matrix.position, &center, &center);
        scale_point3d(&center, 1.0f / marker_count, &center);
    }
    else if (marker_count > 0)
        center = markers[0].matrix.position;
    if (TEST_BIT(barrel->flags, 5))
        random_marker_index = _random(&random_seed, __FILE__, __FUNCTION__, __LINE__) % marker_count;
    if (barrel_definition->projectile_index != NONE && projectile_owner_unit_index != NONE && create_projectiles)
    {
        unit_datum *owner = (unit_datum *)weapon_get(projectile_owner_unit_index);
        if (owner->unit.player_index != NONE)
        {
            ::projectile_definition *projectile_definition = (::projectile_definition *)weapon_definition_get(
                barrel_definition->projectile_index);
            unsigned char reporting_info = weapons_create_reporting_info(projectile_definition->reporting_type, 0);
            shot_id = game_engine_notify_shot_fired(owner->unit.player_index, reporting_info);
        }
    }
    for (short marker_index = 0; marker_index < marker_count; marker_index++)
    {
        barrel = weapon_barrel_get(weapon, barrel_index);
        real_vector3d velocity = *global_zero_vector3d;
        bool early_mover_velocity = false;
        float forward_velocity = 0.0f;
        float error = 0.0f;
        float projectile_velocity = 0.0f;
        long crate_index = NONE;
        bool player_fired = false;
        bool fire = true;
        if (marker_count > 1 && bit_test(barrel_definition->flags, 12))
            fire = barrel->shot_count % marker_count == marker_index;
        if (fire)
        {
        s_projectile_targeting_data targeting;
        targeting.object_index = NONE;
        targeting.model_target_index = NONE;
        real_point3d position = markers[marker_index].matrix.position;
        real_vector3d forward = markers[marker_index].matrix.forward;
        real_vector3d early_velocity;
        real_vector3d early_axis;
        long projectile_definition_index;
        long secondary_projectile_index;
        short projectile_count;
        if (barrel_index == 0 && weapon->queued_projectile_count > 0)
        {
            s_weapon_barrel_definition *other_barrel = &definition->barrels[1];
            projectile_definition_index = other_barrel->projectile_index;
            crate_index = other_barrel->crate_index;
            projectile_count = weapon->queued_projectile_count;
            projectile_count *= barrel_definition->projectile_count;
            weapon->queued_projectile_count = 0;
            secondary_projectile_index = other_barrel->secondary_projectile_index;
            if (secondary_projectile_index != NONE && barrel->value > 1)
                projectile_definition_index = secondary_projectile_index;
        }
        else
        {
            projectile_definition_index = barrel_definition->projectile_index;
            secondary_projectile_index = barrel_definition->secondary_projectile_index;
            crate_index = barrel_definition->crate_index;
            projectile_count = barrel_definition->projectile_count;
            if (secondary_projectile_index != NONE && barrel->value > 1)
                projectile_definition_index = secondary_projectile_index;
        }
        if (!bit_test(barrel_definition->flags, 6) && owner_unit_index != NONE)
        {
            // match: 0x34eba0: without the const-ref header temp the owner lookup add swaps to r6,r25 and the error-axis FP block recolors (28 words)
            s_object_header *const &object_headers = (s_object_header *)object_header_data->data;
            unit_datum *owner = (unit_datum *)object_headers[datum_index_to_absolute_index(owner_unit_index)].datum;
            long control_unit_index = owner_unit_index;
            unit_definition *owner_definition = (unit_definition *)weapon_definition_get(owner->object.definition_index);
            if (!bit_test(owner->object.damage_flags, 2))
            {
                long player_index = ((weapons_unit_datum_flat_view *)owner)->player_index;
                long actor_index = ((weapons_unit_datum_flat_view *)owner)->actor_index;
                const s_aim_assist_targeting_result *aim_assist = &((weapons_unit_datum_flat_view *)owner)->targeting;
                bool snap_to_aim_line = bit_test(owner_definition->flags, 3);
                bool use_aiming_vector = true;
                real_point3d *offset = 0;
                if (player_index != NONE && VALID_INDEX(first_person_offset_index, barrel_definition->first_person_offset_count))
                    offset = &barrel_definition->first_person_offsets[first_person_offset_index];
                if (player_index != NONE)
                    player_fired = true;
                unit_datum *control_unit = owner;
                while (control_unit->unit.gunner_object_index !=
                    NONE)
                {
                    const long &next_control_index =
                        +control_unit->unit.gunner_object_index;
                    control_unit = (unit_datum *)weapon_get(next_control_index);
                    control_unit_index = next_control_index;
                    player_index = control_unit->unit.player_index;
                    actor_index = control_unit->unit.actor_index;
                    aim_assist = &control_unit->unit.targeting;
                }
                if (actor_index != NONE && datum_get<actor_datum>(actor_data, actor_index)->combat_aim_mode == 4)
                    use_aiming_vector = false;
                if (owner->unit.gunner_object_index !=
                    NONE)
                    use_aiming_vector = false;
                unit_adjust_projectile_ray(control_unit_index, &position, &forward, &velocity, offset, snap_to_aim_line, use_aiming_vector, true);
                early_mover_velocity = bit_test(owner->object.physics_flags,
                    3) &&
                    weapons_create_early_mover_velocity(control_unit_index, &early_velocity, &early_axis);
                forward_velocity = velocity.i * forward.i + weapons_create_dot_jk(&velocity, &forward);
                if (forward_velocity < 0.0f)
                    forward_velocity = 0.0f;
                if (bit_test(barrel_definition->flags, 3) ||
                    owner->unit.player_index !=
                        NONE)
                {
                    if (bit_test(barrel_definition->flags, 3))
                        position = markers[marker_index].matrix.position;
                    real_point3d camera_position;
                    real_point3d projectile_position;
                    unit_get_camera_position(owner_unit_index, &camera_position);
                    if (collision_find_good_projectile_point(&camera_position, &position, &projectile_position, object_get_root_object(owner_unit_index), NONE))
                        position = projectile_position;
                }
                if (marker_count == 1)
                    center = position;
                if (player_index != NONE)
                {
                    long primary_weapon_index = NONE;
                    short inventory_index =
                        ((weapons_unit_datum_flat_view *)weapon_get(owner_unit_index))->current_weapon_set
                            .weapon_indices[0];
                    if (inventory_index != NONE)
                        primary_weapon_index =
                            ((weapons_unit_datum_flat_view *)weapon_get(owner_unit_index))->weapon_object_indices
                                    [inventory_index];
                    bool primary = primary_weapon_index == weapon_index;
                    if (bit_test(barrel_definition->flags, 0) && definition->tracking_type != 1 &&
                        aim_assist->target_object_index != NONE && aim_assist->primary_autoaim_level > 0.0f)
                    {
                        bool allow_tracking = true;
                        if (weapon_is_charged_plasma_pistol(weapon_index, true) && barrel_index == 1 &&
                            !weapons_aim_assist_targeting_is_valid_for_plasma_pistol_lock(aim_assist))
                            allow_tracking = false;
                        if (allow_tracking)
                        {
                            targeting.object_index = aim_assist->target_object_index;
                            targeting.model_target_index = (short)aim_assist->model_target;
                        }
                    }
                    else if (weapon->tracked_object_index != NONE && !bit_test(barrel_definition->flags, 16))
                    {
                        if (bit_test(barrel_definition->flags, 13) || definition->tracking_type == 1)
                        {
                            targeting.object_index = weapon->tracked_object_index;
                            targeting.model_target_index = weapon->current_state_ticks;
                        }
                    }
                    else
                        player_aim_projectile(player_index, aim_assist, projectile_definition_index, primary, &position, &center, &forward);
                }
                else if (actor_index != NONE)
                {
                    bool aimed;
                    if (bit_test(barrel_definition->flags, 8))
                        aimed = actor_aim_projectile(actor_index, &center, &forward, &projectile_velocity, &error, &targeting.object_index);
                    else
                        aimed = actor_aim_projectile(actor_index, &position, &forward, &projectile_velocity, &error, &targeting.object_index);
                    if (bit_test(barrel_definition->flags, 13) && targeting.object_index != NONE)
                    {
                        s_object_header *target_header = object_header_get(targeting.object_index);
                        e_object_type type = (e_object_type)target_header->object_type;
                        bool remove_tracking = false;
                        if (type == 0)
                        {
                            if ((unsigned short)((unit_datum *)target_header->datum)->unit.parent_seat_index == 0xffff)
                                remove_tracking = true;
                        }
                        else if (type != 1)
                            remove_tracking = true;
                        if (remove_tracking)
                        {
                            targeting.object_index = NONE;
                            targeting.model_target_index = NONE;
                        }
                    }
                    if (!aimed)
                        continue;
                }
            }
        }
        if (TEST_BIT(barrel->flags, 5) && marker_index == random_marker_index)
        {
            float minimum_error = barrel_definition->direction_error_lower * weapons_create_degrees_to_radians;
            float maximum_error = barrel_definition->direction_error_upper * weapons_create_degrees_to_radians;
            real_vector3d error_axis = markers[marker_index].matrix.up;
            float error_angle = _real_random_range(&random_seed, minimum_error, maximum_error);
            float rotation_angle = _real_random_range(&random_seed, 0.0f, 6.2831855f);
            float rotation_cosine = (float)cos(rotation_angle);
            float rotation_sine = (float)sin(rotation_angle);
            float error_cosine;
            float error_sine;
            float rotation_cross_j = weapons_create_rotation_cross_j(&error_axis, &forward);
            float rotation_scale = dot_product3d(&forward, &error_axis) * (1.0f - rotation_cosine);
            weapons_create_rotate_vector(&error_axis, &forward, rotation_sine, rotation_cosine, rotation_scale, rotation_cross_j);
            error_cosine = (float)cos(error_angle);
            error_sine = (float)sin(error_angle);
            float error_cross_j = weapons_create_rotation_cross_j(&forward, &error_axis);
            weapons_create_rotate_error(&forward, &error_axis, error_sine, error_cosine, 1.0f - error_cosine, error_cross_j);
        }
        if ((unsigned short)barrel->extra_shots != 0)
        {
            projectile_count *= barrel->extra_shots + 1;
            barrel->extra_shots = 0;
        }
        real_vector3d first_projectile_direction;
        if (projectile_definition_index != NONE || crate_index != NONE)
        {
            for (short projectile_number = 0; projectile_number < projectile_count; projectile_number++)
            {
                float projection_scale = 0.0f;
                object_placement_data placement;
                bool tracer = false;
                s_damage_owner damage_owner;
                if (projectile_owner_unit_index != NONE)
                    damage_compute_damage_owner_from_object_index(projectile_owner_unit_index, &damage_owner);
                else
                    damage_owner = *damage_owner_unknown();
                object_placement_data_new(&placement, crate_index != NONE ? crate_index : projectile_definition_index,
                    projectile_owner_unit_index, &damage_owner);
                placement.position = position;
                placement.forward = forward;
                if (player_fired)
                    placement.flags |= 4;
                if (barrel->rate_fraction == 0.0f || barrel->projectile_counter++ >= barrel_definition->tracer_frequency)
                {
                    tracer = true;
                    barrel->projectile_counter = 0;
                }
                if (error == 0.0f)
                {
                    float error_fraction = barrel->firing_error_fraction;
                    float dual_lower = barrel_definition->dual_error_lower == 0.0f ? barrel_definition->error_lower : barrel_definition->dual_error_lower;
                    float dual_upper = barrel_definition->dual_error_upper == 0.0f ? barrel_definition->error_upper : barrel_definition->dual_error_upper;
                    float lower = dual_error * dual_lower + (1.0f - dual_error) * barrel_definition->error_lower;
                    float upper = dual_error * dual_upper + (1.0f - dual_error) * barrel_definition->error_upper;
                    error = lower + (upper - lower) * error_fraction;
                }
                if (!(bit_test(barrel_definition->flags, 5) && bit_test(weapon->control_flags, 7)))
                {
                    float minimum = dual_error * barrel_definition->dual_error_minimum + (1.0f - dual_error) * barrel_definition->error_minimum;
                    _random_vector_in_cone3d(TLS_RANDOM_SEED, "weapon-random-error", "..\\items\\weapons.cpp", 0x1c85, &placement.forward,
                        minimum, error, &placement.forward);
                }
                if (bit_test(barrel_definition->flags, 7))
                {
                    if (projectile_number == 0)
                        first_projectile_direction = placement.forward;
                    else
                        placement.forward = first_projectile_direction;
                }
                weapons_create_normalize3d(weapons_create_perpendicular3d(&placement.forward, &placement.up));
                projectile_distribute(&placement.forward, &placement.up, barrel_definition->distribution_function,
                    barrel_definition->spread_angle, projectile_number, projectile_count);
                scale_vector3d(&placement.forward, forward_velocity, &placement.linear_velocity);
                if (early_mover_velocity)
                {
                    real_vector3d projection;
                    real_vector3d inherited_velocity;
                    projection_scale = weapons_create_projection_scale(placement.forward.i, early_velocity.i, &placement.forward, &early_velocity);
                    scale_vector3d(&placement.forward, projection_scale, &projection);
                    subtract_vectors3d(&early_velocity, &projection, &inherited_velocity);
                    add_vectors3d(&placement.linear_velocity, &inherited_velocity, &placement.linear_velocity);
                }
                if (projectile_velocity > 0.0f)
                {
                    ::projectile_definition *projectile_definition =
                        (::projectile_definition *)weapon_definition_get(barrel_definition->projectile_index);
                    if (projectile_velocity < projectile_definition->initial_velocity)
                    {
                        float multiplier = 1.0f;
                        if (projectile_owner_unit_index != NONE)
                        {
                            if (((unit_datum *)weapon_get(projectile_owner_unit_index))->unit.player_index == NONE)
                                multiplier = projectile_definition_get_velocity_multiplier(barrel_definition->projectile_index);
                            else
                                multiplier = 1.0f;
                        }
                        point_from_line3d(&placement.linear_velocity, &placement.forward,
                            projectile_velocity - projectile_definition->initial_velocity * multiplier, &placement.linear_velocity);
                    }
                }
                if (create_projectiles)
                {
                    long projectile_index = NONE;
                    long prediction_index = 0;
                    if (predicted_fire_count > 0)
                    {
                      const s_predicted_weapon_fire_data *prediction = predicted_fire_data;
                      do
                      {
                        if (prediction->impact_valid && prediction->projectile_id == projectile_number)
                        {
                            vector_from_points3d(&placement.position, &prediction->impact_position, &placement.forward);
                            weapons_create_normalize3d(&placement.forward);
                            real_vector3d *up = weapons_create_perpendicular3d(&placement.forward, &placement.up);
                            float up_length = magnitude3d(up);
                            const float &up_difference = fabs(up_length - 0.0f);
                            if (!(up_difference < k_real_epsilon))
                            {
                                up_length = 1.0f / up_length;
                                scale_vector3d(up, up_length, up);
                            }
                            break;
                        }
                        prediction_index++;
                        prediction++;
                      } while (prediction_index < predicted_fire_count);
                    }
                    if (crate_index != NONE)
                    {
                        point_from_line3d(&placement.linear_velocity, &placement.forward, barrel_definition->crate_velocity, &placement.linear_velocity);
                        if (simulation_query_object_placement(&placement))
                        {
                            long crate_object_index = object_new(&placement);
                            if (crate_object_index != NONE)
                                simulation_action_object_create(crate_object_index);
                        }
                    }
                    else
                    {
                        projectile_index = object_new(&placement);
                    }
                    if (projectile_index != NONE)
                    {
                            s_object_header *projectile_header = object_header_get(projectile_index);
                            weapons_create_projectile *projectile;
                            float damage_scale = barrel_definition->dual_damage_scale * dual_error + (1.0f - dual_error);
                            if (((object_datum *)weapon)->parent_object_index != NONE)
                            {
                              long parent_index = ((object_datum *)weapon)->parent_object_index;
                              long child_index = weapon_index;
                              const cache_file_tag_instance *parent_tag_instances = global_tag_instances;
#if defined(_XBOX) && !defined(__clang__)
                              // match: 0x34eba0: without the rotate the following lis/lwz pair takes r10 instead of r6
                              projectile = (weapons_create_projectile *)((s_object_header *)_rotl((unsigned long)projectile_header, 0))->datum;
#else
                              projectile = (weapons_create_projectile *)projectile_header->datum;
#endif
                              // match: 0x34eba0: without the rotate the parent test reuses r11 and drops a mr r9 (4 bytes shorter)
                              if ((long)_rotl((unsigned long)((object_datum *)weapon)->parent_object_index, 0) != NONE)
                              while (parent_index != NONE)
                              {
                                s_object_header *parent_header = object_header_get(parent_index);
                                const unsigned long &type = parent_header->object_type;
                                long type_mask = 1 << type;
                                if (!(type_mask & 0x1003))
                                    break;
                                // match: 0x34eba0: without the rotate the header add/lbz/slw that build type_mask swap r10 and r11
                                type_mask = (long)_rotl((unsigned long)type_mask, 0);
                                object_datum *parent = (object_datum *)parent_header->datum;
                                if (type_mask & 2)
                                {
                                    if (weapons_create_vehicle_inherits_parent(
                                            (vehicle_definition *)parent_tag_instances[datum_index_to_absolute_index(parent->definition_index)].definition))
                                    {
                                        projectile->parent_unit_index = child_index;
                                        break;
                                    }
                                }
                                else if (type_mask & 0x1000)
                                {
                                    projectile->parent_unit_index = child_index;
                                    break;
                                }
                                child_index = parent_index;
                                parent_index = parent->parent_object_index;
                                // match: 0x34eba0: without the rotate the loop exit becomes bne back-edge + b exit instead of beq exit + b back-edge
                                if ((long)_rotl((unsigned long)parent_index, 0) == NONE)
                                    break;
                              }
                            }
                            else
                                projectile = (weapons_create_projectile *)projectile_header->datum;
                            if (bit_test(barrel_definition->flags, 18))
                                projectile->weapon_index = weapon_index;
                            projectile->damage_scale = damage_scale;
                            projectile->shot_id = shot_id;
                            if (projectile_velocity > 0.0f)
                                projectile->flags.m_storage |= 0x100;
                            if (weapon_should_set_targeting_data(weapon_index, &targeting))
                                ((weapons_create_projectile *)weapon_get(projectile_index))->targeting_data = targeting;
                            if (!tracer)
                                ((weapons_create_projectile *)weapon_get(projectile_index))->flags.m_storage &= ~2;
                            if (replicate_projectiles)
                                simulation_action_object_create(projectile_index);
                            bool initial_seed_valid = false;
                            unsigned short initial_seed = 0;
                            for (long prediction_index = 0; prediction_index < predicted_fire_count; prediction_index++)
                            {
                                const s_predicted_weapon_fire_data *prediction = &predicted_fire_data[prediction_index];
                                if (prediction->conical_valid && prediction->projectile_id == projectile_number)
                                {
                                    initial_seed = prediction->conical_random_seed;
                                    initial_seed_valid = true;
                                    break;
                                }
                            }
                            bool actual_seed_valid = false;
                            unsigned short actual_seed = 0;
                            projectile_initial_update(projectile_index, initial_seed_valid, initial_seed, &actual_seed_valid, &actual_seed);
                            s_simulation_action_projectile_fire_result result;
                            s_simulation_action_projectile_fire_result *valid_result;
                            if (!weapons_create_fire_event_value(generate_fire_event))
                                valid_result = NULL;
                            else
                            {
                                memset(&result.impact_valid, 0, sizeof(result) - sizeof(result.projectile_id));
                                result.projectile_id = projectile_number;
                                if (actual_seed_valid)
                                {
                                    weapons_create_mark_result(&result.conical_valid);
                                    result.conical_random_seed = actual_seed;
                                }
                                if (barrel_definition->prediction_type == 2)
                                {
                                    ::projectile_definition *projectile_definition =
                                        (::projectile_definition *)weapon_definition_get(projectile->definition_index);
                                    if (bit_test(projectile_definition->targeting_flags, 5) &&
                                        projectile_definition->conical_distribution.count == 0 &&
                                        bit_test(projectile->flags.m_storage, 11) && bit_test(projectile->flags.m_storage, 13))
                                    {
                                        long hit_object_index = projectile->collision_object;
                                        object_datum *hit_object = (object_datum *)weapon_get(hit_object_index);
                                        if (hit_object->gamestate_index != NONE)
                                        {
                                          long node_index = projectile->collision_node;
                                          long node_count = hit_object->node_matrices.size / sizeof(real_matrix4x3);
                                          if (VALID_INDEX(node_index, node_count))
                                          {
                                            const real_matrix4x3 *node_matrices =
                                                (const real_matrix4x3 *)((unsigned char *)hit_object + hit_object->node_matrices.offset);
                                            real_matrix4x3 inverse;
                                            matrix4x3_inverse(&node_matrices[node_index], &inverse);
                                            matrix4x3_transform_point(&inverse, &projectile->collision_position, &result.impact_position);
                                            result.impact_node_index = node_index;
                                            result.impact_object_index = hit_object_index;
                                            weapons_create_mark_result(&result.impact_valid);
                                          }
                                        }
                                    }
                                }
                                valid_result = result.impact_valid || result.conical_valid ? &result : NULL;
                            }
                            if (valid_result)
                            {
                                // match: 0x34eba0: without it the stack reloads of 156/172/152(r1) reorder ahead of the result check
                                const long &result_index = (long)_rotl((unsigned long)result_count, 0);
                                if (result_index < results.get_count())
                                {
                                    weapons_create_result_at(&results, result_index) = result;
                                    result_count = result_index + 1;
                                }
                            }
                    }
                }
            }
        }
        if (!game_is_distributed() || (force && !game_is_predicted()))
        {
            if (barrel_definition->damage_effect_index != NONE)
            {
                s_damage_data damage;
                damage_data_new(&damage, barrel_definition->damage_effect_index);
                damage.flags |= 8;
                damage.origin = position;
                damage.position = position;
                damage.object_index = owner_unit_index;
                damage.scale[0] = 1.0f;
                damage.cone_direction = forward;
                damage.reporting_info = (unsigned char)(barrel_definition->damage_reporting_type << 2);
                object_get_location(weapon_index, &damage.location);
                if (owner_unit_index != NONE)
                    damage_compute_damage_owner_from_object_index(owner_unit_index, &damage.damage_owner);
                area_of_effect_cause_damage(&damage, NONE, owner_unit_index);
            }
        }
        }
    }
    if (generate_fire_event)
    {
        bool simulation_queue = !game_simulation_predicted() && !force;
        bool fire = false;
        bool exhausted = false;
        bool blocked = false;
        weapon_barrel_can_fire_this_tick(weapon_index, barrel_index, &fire, &exhausted, &blocked);
        bool secondary = fire || exhausted;
        simulation_action_weapon_fire(weapon_index, barrel_index, secondary, results.get_elements(), result_count, simulation_queue);
    }
}


static __forceinline unsigned long weapons_create_fire_event_value(bool generate_fire_event)
{
    // match: 0x34eba0: the const-ref temporary fixes the spill-slot order (r18 at 188(r1), r24 at 152(r1)); a plain return grows the function 4 bytes
    const unsigned long &value = (unsigned char)generate_fire_event;
    return value;
}

static __forceinline bool weapons_create_vehicle_inherits_parent(const vehicle_definition *definition)
{
    return bit_test(definition->flags, 11);
}

static __forceinline void weapons_create_mark_result(bool *flag)
{
    // match: 0x34eba0: without the rotate the constant 1 stays live in r22 across the result stores instead of li 1 before each stb
    const unsigned long &valid = _rotl(1, 0);
    *(unsigned char *)flag = (unsigned char)valid;
}

static __forceinline void weapons_create_rotate_error(
    real_vector3d *vector, const real_vector3d *axis, float sine, float cosine, float cosine_complement, float cross_j)
{
    float scale = dot_product3d(axis, vector) * cosine_complement;
    float cross_i = vector->j * axis->k - vector->k * axis->j;
    float cross_k = vector->i * axis->j - vector->j * axis->i;
    vector->i = scale * axis->i + (vector->i * cosine) - cross_i * sine;
    vector->j = (axis->j * scale) + vector->j * cosine - cross_j * sine;
    vector->k = (axis->k * scale) + vector->k * cosine - cross_k * sine;
}
