// blamlibxbox/players.obj: from the records 0x8b370 0x8b418 0x98870 0x993f0 0xa06a0 0xa06d0 0xa0738 0xa08c0 0xa0a58 0xa1970 0xa33d8 0xa3830 0xa70c0 0xa7970 0xa79f8 0xa8208 0xa82b8 0xa8458 0xa86b0 0xd5478 0xd5bc8 0xf68c0 0xfc278 0x111498 0x117850 0x117ff8 0x131c80 0x1d7568 0x1d7578 0x1d7628 0x1d7670 0x1d7760 0x1d77c8 0x1d7868 0x1d7ae0 0x1d7c38 0x1d7cc0 0x1d7fd8 0x1d8130 0x1d8198 0x1d82b8 0x1d87d8 0x1d8af0 0x1d8bc0 0x1d8e38 0x1d91d0 0x1d9370 0x1d9478 0x1d9960 0x1d9aa0 0x1d9b70 0x1d9bd8 0x1d9c98 0x1d9df8 0x1d9ec8 0x1da0b8 0x1da3e0 0x1da4a0 0x1da9a0 0x1dab48 0x1dabd0 0x1dad90 0x1dae10 0x1db0e0 0x1dc288 0x1dc410 0x1dc448 0x1dc610 0x1dc728 0x1dc878 0x1dcd40 0x1dd280 0x1dd388 0x1dd460 0x1dd5e0 0x1dd7d8 0x1dd8d8 0x1dd9d0 0x1ddbd8 0x1ddd78 0x1dddf8 0x1de068 0x1de180 0x1de270 0x1de3a0 0x1de918 0x1dead0 0x1debe8 0x1dee60 0x1def00 0x1df078 0x1df158 0x1df410 0x1df5d8 0x1df760 0x1df898 0x1dfaf0 0x1e0460 0x1e0650 0x1e0738 0x1e0818 0x1e08b8 0x1e0958 0x1e0978 0x1e0a18 0x1e0a30 0x1f71c8 0x1f71d8 0x1f7290 0x1f7370 0x1f7410 0x1f7478 0x1f74f0 0x1f7568 0x1f7648 0x1f7700 0x1f7750 0x1f7890 0x1f7930 0x1f7948 0x201440 0x282418 0x2824a0 0x2e11e0 0x380c30 0x380da8 0x3811e8 0x381688 0x381788 0x491908 0x50ab20
// @flags /TP /O2 /Oc
#include "game/player_control_types.h"
#include "motor/player_equipment_action_state.h"
#include "game/player_control_deterministic_globals.h"
#include "math/real_constant_pool_access.h"
#include "ai/ai_player_storage.h"
#include "cseries/global_real_argb_colors.h"
#include "interface/chud/chud_manager.h"
#include "game/player_control_globals.h"
#include "game/player_training_tls.h"
#include "memory/tls_globals.h"
#include "effects/player_effect_tls.h"
#include "effects/player_effect_globals_types.h"
#include "interface/chud/chud_user_data_types.h"
#include "units/weapon_addition_method_types.h"
#include "game/game_globals_storage.h"
#include "game/game_globals.h"
#include "game/game_time_types.h"
#include "math/real_math_globals.h"
#include "input/input_globals.h"
#include "units/unit_seat.h"
#include "math/matrix_math_types.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_types.h"
#include "game/player_traits.h"
#include "game/player_types.h"
#include "objects/object_iterator.h"
#include "units/unit_types.h"
#include "units/vehicle_datum.h"
#include "items/item_types.h"
#include "units/unit_definitions.h"
#include "bink/bink_playback_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "physics/havok_constants_types.h"
#include "units/unit_control.h"
#include "units/vehicle_types.h"
#include "units/action_request.h"
#include "units/unit_passenger_iterator.h"
#include "shell/shell.h"
#include "ai/ai_point3d.h"
#include "game/game_options.h"
#include "game/game_variant_inline.h"
#include "game/game_simulation_predicates.h"
#include "game/player_configuration.h"
#include "game/player_interaction.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "objects/object_types.h"
#include "simulation/player_action.h"
#include "units/motor_task.h"
#include "items/weapon_definitions.h"
#include "items/projectile_definitions.h"
#include "ai/weapon_datum_types.h"
#include "game/game_engine_scoring_globals.h"
#include "camera/observer_globals_types.h"
#include "cseries/cseries.h"
#include "game/player_mapping_inlines.h"
#include "units/unit_inventory_inline.h"
#include "math/integer_math.h"
#include "game/game_time_access.h"
#include "math/real_math_trigonometry.h"
#include "motor/mover_inline.h"
#include "cseries/bit_operations.h"
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include "ai/actors.h"
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "game/players_globals.h"
#include "main/main_game_externs.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "simulation/simulation.h"
#include "sound/game_sound.h"
#include "units/units.h"
#include "ai/ai.h"
#include "ai/ai_player.h"
#include "ai/ai_reference_frame.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "camera/observer.h"
#include "devices/devices.h"
#include "game/game_engine_notifications.h"
#include "game/game_engine_player_traits.h"
#include "game/game_engine_sandbox.h"
#include "game/game_engine_spawn_influencer.h"
#include "game/game_engine_spawning.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "game/aim_assist.h"
#include "game/player_control.h"
#include "game/player_training.h"
#include "interface/chud/chud.h"
#include "interface/first_person_weapons.h"
#include "items/weapons.h"
#include "main/main.h"
#include "motor/motor_system.h"
#include "motor/mover.h"
#include "objects/damage.h"
#include "objects/damage_owner.h"
#include "objects/model_customization.h"
#include "objects/multiplayer_game_objects.h"
#include "objects/object_early_movers.h"
#include "physics/bsp3d.h"
#include "physics/havok.h"
#include "scenario/scenario_trigger_volumes.h"
#include "units/bipeds.h"
#include "units/vehicles.h"
#include "ai/actor_combat.h"
#include "effects/effects.h"
#include "interface/user_interface_controller.h"
#include "items/equipment.h"
#include "items/projectiles.h"
#include "motor/weapon_actions.h"
#include "memory/data.h"
#include "bink/bink_playback.h"
#include "cutscene/cinematics.h"
#include "camera/following_camera.h"
#include "structures/structure_bsp_definitions.h"
#include "interface/controller_types.h"
#include "memory/data_types.h"
#include "ai/ai_player_types.h"
#include "animations/animation_manager.h"
#include "game/game_engine_default.h"
#include "game/player_masks_types.h"
#include "simulation/simulation_queue_player_event_types.h"
#include "simulation/simulation_queue_events.h"
#include "game/game_types.h"
#include "cache/cache_files.h"
#include "game/game_allegiance.h"
#include "networking/network_configuration.h"
#include "simulation/game_interface/simulation_game_events.h"
#include "simulation/simulation_queue_global_events.h"
#include "game/player_mapping_types.h"
#include "cseries/data_mining_event_types.h"
#include "game/players_dependencies.h"
#include "interface/chud/chud_message_context_types.h"
#include "structures/cluster_bit_vectors.h"
#include "interface/chud/chud_navpoint_types.h"
#include "items/equipment_definitions.h"
#include "items/item_definitions.h"
#include "objects/objects.h"
#include "camera/director.h"
#include "math/real_math_functions.h"
#include "math/matrix_math_functions.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "game/game_engine_list.h"
#include "interface/chud/chud_messaging.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_definitions.h"
#include "scenario/scenario_game_state.h"
#include "memory/data_absolute_index.h"
#include "memory/datum_index.h"
#include "cubemaps/cubemap_sample.h"
#include "scenario/cluster_reference_helpers.h"
#include "objects/object_reference_types.h"
#include "objects/object_header.h"
#include "objects/object_type_access.h"
#include "game/player_access.h"
#include "game/player_in_game_iterator.h"
#include "game/player_with_unit_iterator.h"
#include "objects/damage_globals.h"
#include "saved_games/saved_film_snippet_internals.h"
#include "camera/director_storage_types.h"
#include "units/unit_facing.h"
#include "scenario/scenario_light_types.h"
#include "objects/object_placement.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
#include "units/vehicle_type_component_internals.h"
#include "units/biped_types.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "tag_files/tag_groups.h"
#include "game/player_iterator.h"
#include "ai/cs_scenario_definitions.h"
#include "simulation/simulation_queue_player_event_data_types.h"
#include "interface/user_interface_utilities.h"
#include "networking/network_configuration_types.h"
#include "interface/chud/chud_globals_definitions.h"
#include "game/player_training_types.h"
#include "units/unit_camera.h"
#include "units/unit_weapon_pickup_result.h"
#include "game/player_control_constants.h"
#include "simulation/simulation_update_types.h"
#include "game/player_information_definition.h"
#include "effects/rumble_globals_types.h"
#include <new>
#include "tag_files/tag_index.h"
#include "effects/effect_creation.h"
#include "units/vehicle_type_human_plane_exit.h"
#include "units/motor_request.h"
#include "units/unit_messages.h"
#include "units/vehicle_definitions.h"
#include "scenario/scenario_player_starting_location.h"
#include "scenario/scenario_starting_profile.h"
#include "game/game_globals_inline.h"


// @address 0x993f0 ?player_build_aiming_vector_from_facing@@YAXJPBTreal_euler_angles2d@@PATreal_vector3d@@@Z
// @address 0x1d9df8 ?players_any_are_near_death@@YA_NPAJ@Z
// @address 0x1dad90 ?next@c_player_with_unit_iterator@@QAA_NXZ
// @address 0x1def00 ?players_build_persistent_player_options@@YAXPAUgame_options@@@Z
// @address 0x1df078 ?players_store_persistent_weapon@@YAXJPAUs_persistent_campaign_weapon@@@Z
// @address 0xa7970 ?next@c_player_in_game_iterator@@QAA_NXZ
// @address 0xa8208 ?players_joined_in_progress_and_waiting_to_respawn@@YA_NXZ
// @address 0xa8458 ?player_force_spawn@@YAXJ@Z
// @address 0xa82b8 player_function_get_function_value
// @address 0x131c80 player_profile_get_rgb_color
// @address 0x1d7568 ?__tls_set_g_players_globals_allocator@@YAXPAX@Z
// @address 0x1d9b70 ?player_try_and_get_player_index_from_absolute_player_index@@YAJJ@Z
// @address 0x1dc410 players_is_zone_transition_active
// @address 0x1ddd78 ?player_interaction_exists@@YA_NJKPBUs_player_interaction@@@Z
// @address 0x1de3a0 ?player_handle_interaction_held@@YA_NJPBUs_player_interaction@@@Z
// @address 0x1e0818 player_is_on_local_machine
// @address 0x1e08b8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x1e0958 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x1e0978 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_players_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x1d7760 ?players_dispose_from_old_map@@YAXXZ
// @address 0x1df5d8 ?players_arm_unit_from_persistent_campaign_data@@YA_NPBUs_persistent_campaign_player@@J@Z
// @address 0x1f7478 players_set_unit_for_player_views
// @address 0x1f74f0 players_set_controller_mapping
// @address 0x1d91d0 ?player_set_unit_index@@YAXJJ@Z
// @address 0x1d7fd8 ?player_set_configuration@@YAXJPBUs_player_configuration@@@Z
// @address 0x2824a0 players_is_object_player_local

template<typename datum_type> class c_player_data_iterator {
public:
    datum_type *datum;
    s_data_iterator iterator;
    __forceinline void begin(s_data_array *data)
    {
        iterator.absolute_index = NONE;
        iterator.index = NONE;
        iterator.data = data;
    }
    __forceinline bool next()
    {
        datum = (datum_type *)::data_iterator_next(&iterator);
        return datum != 0;
    }
    __forceinline datum_type *get_datum() const { return datum; }
};

struct s_remap_local_identity {
    long controller_index;
    s_machine_identifier machine_identifier;
    s_player_identifier player_identifier;
};

#pragma pack(push, 2)
struct s_remap_local_header {
    short user_index;
    s_remap_local_identity identity;
};
#pragma pack(pop)

struct s_remap_header_pointer {
    const s_remap_local_header *value;
    long user_index;
};

class c_output_user_iterator {
    long m_player_index;
    long m_user_index;
public:
    __forceinline c_output_user_iterator(long player_index)
        : m_player_index(player_index), m_user_index(NONE) {}
    __forceinline bool next()
    {
        if (m_player_index != NONE)
            m_user_index = ((long)::director_get_perspective(m_player_index, m_user_index));
        return m_user_index != NONE;
    }
    __forceinline long get_index() const { return m_user_index; }
};

class c_players_iterator : public c_data_iterator<player_datum> {
public:
    __forceinline void begin()
    {
        m_iterator.data = player_data;
        m_iterator.absolute_index = NONE;
        m_iterator.index = NONE;
    }
};


struct s_weapon_swap_request : action_request {
    __forceinline s_weapon_swap_request() { memset(this, 0, sizeof(*this)); }
};

struct s_exit_vehicle_event {
    long seat_index;
    bool detach;
};


struct s_equipment_event {
    short type;
    long definition_index;
};

struct s_spawn_restore_context
{
    long unit_index;
};

typedef struct s_player_unit_index { unsigned long value; } s_player_unit_index;

struct s_leave_player_prefix
{
    short identifier;
    unsigned long flags;
    s_player_identifier player_identifier;
    long left_game_time;
    s_machine_identifier machine;
    short machine_index;
    short user_index;
    long controller_index;
    s_cluster_reference cluster_reference;
    long unit_index;
    long dead_unit_index;
};

enum e_director_perspective;


typedef char players_character_representation_size[(sizeof(s_player_representation) == 0x54) ? 1 : -1];


/* ---- math ---- */


/* ---- data arrays ---- */
/* data.inl helpers (ground truth: src/d/sub_d4c50.cpp) */


namespace {
static __forceinline player_datum *player_try_and_get(const s_data_array *array, long player_index)
{
    player_datum *result = 0;
    if (player_index != NONE) {
        long absolute_index = (unsigned short)player_index;
        long identifier = player_index >> 16;
        if (absolute_index < array->first_unallocated) {
            player_datum *player = (player_datum *)(array->data + absolute_index * array->size);
            if (player->identifier != 0 && player->identifier == identifier)
                result = player;
        }
    }
    return result;
}
}
// @address 0x98870 ?players_get_active_and_in_game_count@@YAJ_N@Z
long players_get_active_and_in_game_count(bool include_respawning)
{
    long player_count = 0;
    c_player_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (!TEST_BIT(player->flags, 1) && TEST_BIT(player->flags, 0))
            player_count++;
    }
    return player_count;
}

namespace {
static const real_vector3d k_forward3d = { 1.0f, 0.0f, 0.0f };



}

// @address 0x993f0 ?player_build_aiming_vector_from_facing@@YAXJPBTreal_euler_angles2d@@PATreal_vector3d@@@Z
void player_build_aiming_vector_from_facing(long player_index, const real_euler_angles2d *facing, real_vector3d *aiming_vector)
{
    player_datum *player = player_get(player_index);
    if (player->unit_index != NONE && mover_has_variable_camera_up(player->unit_index)) {
        object_datum *object = object_get(player->unit_index);
        real_vector3d left;
        real_vector3d forward;
        cross_product3d(&object->up, &k_forward3d, &left);
        if (normalize3d(&left) == 0.0f)
            left = *global_up3d;
        cross_product3d(&left, &object->up, &forward);
        *aiming_vector = forward;
        rotate_vector_about_axis(aiming_vector, &left, sine(-facing->pitch), cosine(-facing->pitch));
        rotate_vector_about_axis(aiming_vector, &object->up, sine(facing->yaw), cosine(facing->yaw));
    } else {
        vector3d_from_euler_angles2d(aiming_vector, facing);
    }
    if (player->unit_index != NONE)
        ::unit_adjust_first_person_camera(player->unit_index, aiming_vector);
}

long player_index_from_user_index(long user_index)
{
    long player_index = NONE;
    if (user_index != NONE)
        player_index = player_mapping_globals->player_indices[user_index];
    return player_index;
}

namespace {
static __forceinline void clear_interaction(s_player_interaction *interaction)
{
    memset(interaction, 0, sizeof(*interaction));
    interaction->object_index = NONE;
}
static __forceinline void clear_action_context(s_player_action_context *context)
{
    memset(context, 0, sizeof(*context));
    clear_interaction(&context->interaction);
    context->melee_target_unit = NONE;
}
}
void player_action_clear(player_action *action)
{
    s_aim_assist_targeting_result *targeting = &action->aim_assist;
    memset(action, 0, sizeof(*action));
    action->unit_flags = 255;
    action->weapons.set_identifier = NONE;
    action->weapons.weapon_indices[0] = NONE;
    action->weapons.weapon_indices[1] = NONE;
    action->grenade = NONE;
    action->zoom = NONE;
    aim_assist_targeting_clear(targeting);
    clear_action_context(&action->action_context);
}

namespace {
static __forceinline long next_active_user(long user_index)
{
    long result = NONE;
    if (user_index == NONE)
        user_index = 0;
    else
        user_index++;
    for (long index = user_index; index < 4; index++) {
        if (player_mapping_globals->player_indices[index] != NONE) {
            result = index;
            break;
        } else {
            continue;
        }
    }
    return result;
}
}

// @address 0xa06d0 ?player_control_set_facing@@YAXJPBTreal_vector3d@@@Z
void player_control_set_facing(long player_index, const real_vector3d *facing)
{
    s_player_control_user_state *user = &((s_player_control_globals *)player_control_globals)->users[player_index];
    euler_angles2d_from_vector3d(&user->action.look, facing);
    if (user->action.look.yaw < 0.0f)
        user->action.look.yaw += 6.2831855f;
    player_control_update_state_actions(player_index);
}

// @address 0xa3830 players_test_targeting_bounds
bool players_test_targeting_bounds(s_aim_assist_targeting_result *record)
{
  bool valid = true;
  unsigned long flags = record->flags;
  if ((flags & 1) && (record->primary_autoaim_level > real_constant_pool_zero() || record->secondary_autoaim_level > real_constant_pool_zero()))
    valid = false;
  valid = valid && record->primary_autoaim_level >= real_constant_pool_zero() && record->primary_autoaim_level <= real_constant_pool_one();
  valid = valid && record->secondary_autoaim_level >= real_constant_pool_zero() && record->secondary_autoaim_level <= real_constant_pool_one();
  if (record->primary_autoaim_level == real_constant_pool_zero() && record->secondary_autoaim_level == real_constant_pool_zero() && (flags & 2) == 0)
    return valid;
  return valid && record->target_object_index != -1;
}

bool c_player_in_game_iterator::next()
{
    m_iterator.m_datum = (player_datum *)::data_iterator_next(&m_iterator.m_iterator);
    while (m_iterator.m_datum && TEST_BIT(m_iterator.m_datum->flags, 1))
        m_iterator.m_datum = (player_datum *)::data_iterator_next(&m_iterator.m_iterator);
    return m_iterator.m_datum != 0;
}
bool players_joined_in_progress_and_waiting_to_respawn(void)
{
    bool result = false;
    c_player_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (!TEST_BIT(player->flags, 1) && TEST_BIT(player->flags, 0) && TEST_BIT(player->flags, 13)) {
            result = true;
            break;
        }
    }
    return result;
}
void player_force_spawn(long player_index)
{
    player_datum *player = player_try_and_get(player_data, player_index);
    if (game_engine_player_is_out_of_lives(player_index)) {
        player->lives = 1;
        ::simulation_action_game_engine_player_update((short)player_index, 0x200);
    }
    player->respawn_timer = 0;
    player->respawn_seconds = 0;
    ::simulation_action_game_engine_player_update((short)player_index, 0x800);
}
// @address 0xa86b0 ?players_update_for_checkpoint@@YAXXZ
void players_update_for_checkpoint(void)
{
    c_player_in_game_iterator iterator;
    iterator.m_iterator.m_iterator.absolute_index = NONE;
    iterator.m_iterator.m_iterator.index = NONE;
    iterator.m_iterator.m_iterator.data = player_data;
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (TEST_BIT(player->flags, 13))
            player->flags &= ~FLAG(13);
    }
}
long players_first_active_user(void)
{
    return next_active_user(NONE);
}

long players_next_active_user(long user_index)
{
    return next_active_user(user_index);
}


// @address 0x8b418 players_get_active_user_count
// A count clamped to 1..4, or 1 when the feature is not active.
int players_get_active_user_count(void)
{
  bool enabled = false;
  game_globals_storage *globals = game_globals;
  if (globals != 0 && globals->in_progress != 0 && globals->initializing == 0 && globals->map_active != 0)
    enabled = true;
  if (enabled) {
    bool in_progress = false;
    s_cinematic_globals_definition *cinematic = (s_cinematic_globals_definition *)cinematic_globals;
    if (cinematic != 0)
      in_progress = cinematic->in_progress;
    if (!in_progress && players_globals->suppressed == 0) {
      short count = player_mapping_globals->local_player_count;
      if (count > 1) {
        if (count < 4)
          return count;
        return 4;
      }
    }
  }
  return 1;
}


// The user a controller drives (from the controllers' user mapping after the four controllers), or 4 (none) in the
// main menu, with no output users, or for no controller.
// @address 0x8b370 player_get_user_from_controller
long player_get_user_from_controller(long controller_index)
{
    long user_index = 4;
    if (game_globals->options.game_mode != 3 && player_mapping_globals->local_player_count > 0 && controller_index != NONE && controller_index != 255) {
        user_index = controller_globals().controller_windows[controller_index];
        if (user_index < 0 || user_index >= players_get_active_user_count())
            user_index = 4;
    }
    return user_index;
}

// The value of a player attribute (field - 0x274) for the player at player_index: a float, and whether the
// field is known. An unknown player or field stores 0 and false.
bool player_function_get_function_value(long player_index, long field, float *value, bool *valid)
{
    bool found = false;
    float result = 0.0f;
    if (player_index != NONE) {
        player_datum *player = &((player_datum *)player_data->data)[(unsigned short)player_index];
        found = true;
        switch (field - 0x274) {
        case 0: result = game_globals->options.game_mode == 2 ? 1.0f : 0.0f; break;
        case 1: result = (float)player->configuration.client.appearance.foreground_emblem; break;
        case 2: result = (float)player->configuration.client.appearance.background_emblem; break;
        case 3: result = (player->configuration.client.appearance.emblem_flags & 1) ? 0.0f : 1.0f; break;
        case 4: result = (player->configuration.client.appearance.emblem_flags & 2) ? 1.0f : 0.0f; break;
        case 5: result = (player->configuration.client.appearance.emblem_flags & 4) ? 1.0f : 0.0f; break;
        case 6: result = (float)player->configuration.client.appearance.emblem_primary_color; break;
        case 7: result = (float)player->configuration.client.appearance.emblem_secondary_color; break;
        case 8: result = (float)player->configuration.client.appearance.emblem_background_color; break;
        default: found = false;
        }
    }
    if (found) {
        *value = result;
        *valid = true;
    } else {
        *value = 0.0f;
        *valid = false;
    }
    return found;
}

// @address 0xd5478 ?players_compute_combined_pvs@@YAXPAUs_game_cluster_bit_vectors@@_NPAV?$c_static_array@V?$c_static_array@J$0BAA@@@$0BA@@@@Z

void players_compute_combined_pvs(s_game_cluster_bit_vectors *pvs, bool use_observers, c_static_array<c_static_array<long, 256>, 16> *cluster_activation)
{
    s_cluster_reference cluster_references[16];
    long cluster_reference_count = 0;
    c_player_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (use_observers) {
            long player_index = iterator.iterator.index;
            for (long user_index = player_get_first_output_user(player_index); user_index != NONE;
                user_index = ((long)::director_get_perspective(player_index, user_index)))
                cluster_references[cluster_reference_count++] = observer_get_globals()->blocks[user_index].camera_result.location.cluster_reference;
        } else {
            cluster_references[cluster_reference_count++] = player->cluster_reference;
        }
    }
    memset(pvs, 0, sizeof(*pvs));
    for (long i = 0; i < cluster_reference_count; i++) {
        if (cluster_reference_valid(&cluster_references[i])) {
            s_game_cluster_bit_vectors cluster_pvs;
            structure_bsp_compute_cluster_active_pvs(cluster_references[i], &cluster_pvs);
            game_clusters_or(pvs, &cluster_pvs, pvs);
        }
    }
}

// @address 0xd5bc8 player_get_first_output_user
// The first bit (of four) set in the mask of a datum's absolute index, or -1.
long player_get_first_output_user(long index)
{
  long result = -1;
  unsigned char mask = player_mapping_globals->player_masks.masks[datum_index_to_absolute_index(index)];
  if (mask) {
    for (long i = 0; i < 4; i++) {
      if ((1 << i) & mask) {
        result = i;
        break;
      }
    }
  }
  return result;
}

// The globals' color at index (white when there is no scenario or the index
// is out of range). Read the scenario color before storing the default.
// match: 0x131c80: __restrict lets the default-color copy complete before the scenario branch.
real_rgb_color *player_profile_get_rgb_color(real_rgb_color * __restrict color, long index)
{
  void *scenario = global_scenario;
  *color = global_real_argb_colors[_real_argb_white].rgb;
  if (scenario) {
    s_game_globals *globals = ((s_game_globals *)global_game_globals);
    if (index >= 0 && index < globals->player_colors.count())
      *color = globals->player_colors[index];
  }
  return color;
}

// @address 0x11df660 player_data
__declspec(thread) s_data_array *player_data;
// @address 0x11df67c players_globals
__declspec(thread) s_players_globals *players_globals;

void __tls_set_g_players_globals_allocator(void *value)
{
    players_globals = (s_players_globals *)value;
}

void players_dispose(void)
{
    if (player_mapping_globals)
        player_mapping_globals = 0;
    if (player_data)
        player_data = 0;
    if (players_globals)
        players_globals = 0;
}


// The players globals block (players_globals, 284 bytes), as the functions that reset it use it.


static __forceinline real_vector3d *players_set_vector3d(real_vector3d *v, float i, float j, float k)
{
    v->i = i; v->j = j; v->k = k; return v;
}

static __forceinline void players_copy_vector3d(real_vector3d *to, const real_vector3d *from)
{
    *to = *from;
}

// @address 0x1d7670 ?players_initialize_for_new_map@@YAXXZ
void players_initialize_for_new_map(void)
{
    s_players_globals *globals;
    if (main_game_globals.reset_in_progress) {
        globals = players_globals;
        memset(globals->transient_state, 0, sizeof(globals->transient_state));
        globals->transient_count = 0;
        globals->respawn_unsafe = false;
    } else {
        globals = players_globals;
        memset(globals, 0, sizeof(*globals));
        players_reset_mapping_globals();
        real_vector3d *forward = &globals->camera_forward;
        players_set_vector3d(&globals->camera_position, 3.4028234663852886e38f, 3.4028234663852886e38f, 3.4028234663852886e38f);
        players_copy_vector3d(forward, global_forward3d);
        globals->camera_user_index = NONE;
        globals->camera_controller_index = NONE;
        globals->camera_unit_index = NONE;
        globals->camera_object_index = NONE;
        globals->camera_target_index = NONE;
        globals->respawn_failure = 0;
        s_data_array *data = player_data;
        data->valid = true;
        data_delete_all(data);
        ::ai_players_reset();
        players_globals->local_machine_index = NONE;
    }
}

void players_dispose_from_old_map(void)
{
    if (!main_game_globals.reset_in_progress) {
        player_data->valid = false;
        ::ai_players_reset();
        players_globals->count = 0;
        players_reset_mapping_globals();
    }
}

// @address 0x1d8198 ?player_copy_object_appearance@@YAXJJ@Z
void player_copy_object_appearance(long player_index, long object_index)
{
    player_datum *player = player_get(player_index);
    object_datum *object = object_get(object_index);
    if (TEST_BIT(0x1003, object->object_type))
        ((unit_datum *)object)->unit.team = (e_game_team)player->configuration.host.team_index;
    long team = game_is_multiplayer() ? (game_engine_has_teams() ? player->configuration.host.team_index : NONE) : 0;
    real_rgb_color colors[4];
    bool valid;
    ::game_engine_get_change_colors(player_index, &player->configuration.client.appearance,
        (e_game_team)team, (real_rgb_color *)colors, (bool *)(&valid));
    if (valid) {
        object_set_base_change_color_by_index(object_index, 0, &colors[0]);
        object_set_base_change_color_by_index(object_index, 1, &colors[1]);
        object_set_base_change_color_by_index(object_index, 2, &colors[2]);
    } else {
        object_set_initial_change_colors(object_index, 0, 0);
    }
}

static __forceinline void players_director_notify_player_index_changed(long player_index)
{
    for (long user_index = 0; user_index < 4; user_index++)
    {
        g_director_globals->directors[user_index].get_director()->handle_deleted_player(player_index);
    }
}

// @address 0x1d87d8 ?player_swap@@YAXJJ@Z
void player_swap(long absolute_player_index1, long absolute_player_index2)
{
    player_datum *player1 = (player_datum *)datum_try_and_get_absolute(player_data, absolute_player_index1);
    player_datum *player2 = (player_datum *)datum_try_and_get_absolute(player_data, absolute_player_index2);
    player_datum player1_copy;
    player_datum player2_copy;
    long player1_index = NONE;
    long player1_new_index = NONE;
    long player2_index = NONE;
    long player2_new_index = NONE;

    if (player1)
    {
        player1_index = datum_absolute_index_to_index(player_data, absolute_player_index1);
        memcpy(&player1_copy, player1, sizeof(player_datum));
        ::datum_delete(player_data, player1_index);
    }
    if (player2)
    {
        player2_index = datum_absolute_index_to_index(player_data, absolute_player_index2);
        memcpy(&player2_copy, player2, sizeof(player_datum));
        ::datum_delete(player_data, player2_index);
    }
    if (player1_index != NONE)
    {
        player1_new_index = datum_new_at_absolute_index(player_data, absolute_player_index2);
        player_datum *player = (player_datum *)player_get(player1_new_index);
        memcpy(player, &player1_copy, sizeof(player_datum));
        player->identifier = (unsigned short)(player1_new_index >> 16);
    }
    if (player2_index != NONE)
    {
        player2_new_index = datum_new_at_absolute_index(player_data, absolute_player_index1);
        player_datum *player = (player_datum *)player_get(player2_new_index);
        memcpy(player, &player2_copy, sizeof(player_datum));
        player->identifier = (unsigned short)(player2_new_index >> 16);
    }

    ::game_engine_player_indices_swapped(absolute_player_index1, absolute_player_index2);
    if (player1_index != NONE)
    {
        game_engine_player_changed_indices(player1_index, player1_new_index);
        objects_handle_deleted_player(player1_index);
        players_director_notify_player_index_changed(player1_index);
    }
    if (player2_index != NONE)
    {
        game_engine_player_changed_indices(player2_index, player2_new_index);
        objects_handle_deleted_player(player2_index);
        players_director_notify_player_index_changed(player2_index);
    }
}

long player_try_and_get_player_index_from_absolute_player_index(long absolute_player_index)
{
    long player_index = NONE;
    if (datum_try_and_get_absolute(player_data, absolute_player_index))
        player_index = datum_absolute_index_to_index(player_data, absolute_player_index);
    return player_index;
}

namespace {
static __forceinline bool players_campaign_death_disabled()
{
    unsigned long flags;
    if (game_globals->options.game_mode == 1)
        flags = game_globals->options.primary_skulls;
    else
        flags = 0;
    return (bool)((flags >> 1) & 1);
}
}
// @address 0x1d9bd8 ?player_find_player_character_unit_and_variant_info@@YAXJPAJ0@Z
void player_find_player_character_unit_and_variant_info(long player_index, long * unit_definition_index, long * variant_index)
{
    long definition = NONE;
    long variant = NONE;
    long character = game_get_player_character_type(player_index);
    if (character >= 0 && character < ((s_game_globals *)global_game_globals)->player_representations.count()) {
        s_player_representation *element = &((s_player_representation *)((s_game_globals *)global_game_globals)->player_representations.address)[character];
        definition = element->third_person_unit.index;
        variant = element->third_person_variant;
        if (game_globals->options.game_mode == 1 && character == 4 && player_get(player_index)->campaign_player_slot == 2)
            variant = 1;
    }
    if (unit_definition_index != 0)
        *unit_definition_index = definition;
    if (variant_index != 0)
        *variant_index = variant;
}
bool players_any_are_near_death(long *unit_index)
{
    bool result = false;
    if (!players_campaign_death_disabled()) {
        c_player_data_iterator<player_datum> iterator;
        iterator.begin(player_data);
        while (iterator.next()) {
            player_datum *player = iterator.get_datum();
            if (player->unit_index != NONE) {
                object_datum *unit = object_get(player->unit_index);
                if ((bool)((unit->damage_flags >> 3) & 1)) {
                    *unit_index = player->unit_index;
                    result = true;
                }
            }
        }
    }
    return result;
}
// @address 0x1da0b8 player_teleport_for_zone_switch
extern "C" void player_teleport_for_zone_switch(long player_index, long unit_index, const real_point3d *position, const real_vector3d *forward, bool special)
{
    player_datum *player = player_get(player_index);
    if (player->unit_index == NONE)
        return;
    bool updated = false;
    object_datum *unit_object = object_get(player->unit_index);
    if (unit_object->parent_object_index != NONE) {
        long child_index = unit_object->parent_object_index;
        long root_index;
        do {
            root_index = child_index;
            child_index = object_get(child_index)->parent_object_index;
        } while (child_index != NONE);
        long source_index = unit_index;
        long source_root_index = NONE;
        if (source_index != NONE) {
            do {
                source_root_index = source_index;
                source_index = object_get(source_index)->parent_object_index;
            } while (source_index != NONE);
        }
        if (root_index != source_root_index) {
            long unit_child_index = player->unit_index;
            long unit_root_index;
            do {
                unit_root_index = unit_child_index;
                unit_child_index = object_get(unit_child_index)->parent_object_index;
            } while (unit_child_index != NONE);
            if (special && TEST_BIT(1 << object_get_type(unit_root_index), 1)) {
                bool usable = true;
                if (vehicle_get_type(unit_root_index) == 5)
                    usable = false;
                if (usable) {
                    object_set_position_direct(unit_root_index, 0, forward, global_up3d, 0, false);
                    if (player_fix_unit_position(player_index, NONE, position, false, true))
                        updated = true;
                }
            }
            if (!updated)
                action_submit_event(player->unit_index, 20);
        }
    }
    bool result;
    if (!updated && unit_object->parent_object_index == NONE) {
        object_set_position_direct(player->unit_index, 0, forward, global_up3d, 0, false);
        unit_set_facing(player->unit_index, forward, forward);
        result = player_fix_unit_position(player_index, unit_index, position, true, true);
    }
    else {
        result = true;
    }
    if (!result) {
        long unit = player->unit_index;
        if (unit != NONE) {
            long other_index = unit_index;
            long other_root_index = NONE;
            if (other_index != NONE) {
                do {
                    other_root_index = other_index;
                    other_index = object_get(other_index)->parent_object_index;
                } while (other_index != NONE);
            }
            long child = unit;
            long parent_root;
            do {
                parent_root = child;
                child = object_get(child)->parent_object_index;
            } while (child != NONE);
            s_location location;
            real_point3d source_center;
            object_get_location(other_root_index, &location);
            object_get_center_of_mass(other_root_index, &source_center);
            real_vector3d offset;
            {
                real_point3d unit_center;
                real_point3d origin;
                object_get_center_of_mass(parent_root, &unit_center);
                object_get_origin(parent_root, &origin);
                offset.i = origin.x - unit_center.x;
                offset.j = origin.y - unit_center.y;
                offset.k = origin.z - unit_center.z;
            }
            {
                real_point3d new_position;
                new_position.x = source_center.x + offset.i;
                new_position.y = source_center.y + offset.j;
                new_position.z = source_center.z + offset.k;
                object_set_position_direct(parent_root, &new_position, 0, 0, &location, false);
            }
            player_update_unit_after_teleport(player_index, unit_index, true);
            return;
        }
        player->coop_respawn_delay_ticks = 0;
        player->coop_respawn_pending = true;
        player->coop_respawn_initial = true;
    }
}

// @address 0x1da3e0 player_move_unit_to_position
extern "C" bool player_move_unit_to_position(long player_index, long unit_index, bool teleported, const real_point3d *position)
{
    player_datum *player = player_get(player_index);
    long unit = player->unit_index;
    if (object_get(unit)->parent_object_index != NONE)
        action_submit_event(unit, 20);
    real_point3d point;
    point.x = position->x + 0.0f * g_havok_constants.minimum_radius;
    point.y = position->y + 0.0f * g_havok_constants.minimum_radius;
    point.z = position->z + 1.0f * g_havok_constants.minimum_radius;
    return player_fix_unit_position(player_index, unit_index, &point, teleported, false);
}
// @address 0x1dab48 player_increment_teleport_counter
void player_increment_teleport_counter(unsigned long player_index)
{
  player_datum *player = player_get(player_index);
  if (player->unit_index != -1) {
    unit_datum *object = (unit_datum *)object_get(player->unit_index);
    object->unit.unit_input_flags = player->teleport_counter;
    ::simulation_action_object_update(player->unit_index, 0x80000000);
  }
  player->teleport_counter = (player->teleport_counter + 1) & 15;
}

// @address 0x1dabd0 ?player_find_best_spawn_location@@YA_NJPATreal_point3d@@PAM1_N2@Z
// match: 0x1dabd0: pointer __restrict qualifier prevents alias reload
bool player_find_best_spawn_location(long player_index, real_point3d *location, float *__restrict facing, float *pitch, bool single_player, bool initial_spawn)
{
    bool result = false;
    if (game_is_multiplayer()) {
        long object_index = game_engine_find_best_player_respawn_object_index(player_index, false);
        if (object_index != NONE) {
            real_matrix4x3 matrix;
            long unit_definition_index;
            {
                real_vector3d forward;
                real_vector3d up;
                multiplayer_get_spawn_object_position_and_facing_vectors(object_index, location, &forward, &up);
                matrix4x3_rotation_from_vectors(&matrix, &forward, &up);
            }
            matrix.position = *location;
            {
                real_euler_angles3d angles;
                following_camera_deterministic(&matrix, &angles);
                *facing = angles.yaw;
            }
            *pitch = 0.0f;
            ::player_find_player_character_unit_and_variant_info(player_index, (long *)(&unit_definition_index), (long *)(0));
            if (unit_definition_index != NONE) {
                game_engine_get_spawn_object_location(object_index, unit_definition_index, location);
                result = true;
            }
        }
    } else {
        player_datum *player = player_get(player_index);
        scenario *scenario = global_scenario;
        long starting_location_index = NONE;
        long unit_definition_index;
        ::player_find_player_character_unit_and_variant_info(player_index, (long *)(&unit_definition_index), (long *)(0));
        for (long i = 0; i < scenario->player_starting_locations.count; i++) {
            s_scenario_player_starting_location *starting_location =
                (s_scenario_player_starting_location *)((char *)scenario->player_starting_locations.address +
                    i * sizeof(s_scenario_player_starting_location));
            if (game_engine_unit_can_spawn_at_location_unobstructed(unit_definition_index, &starting_location->position, false)) {
                starting_location_index = i;
                break;
            }
        }
        if (starting_location_index != NONE) {
            player->respawn_counter++;
            s_scenario_player_starting_location *starting_location =
                (s_scenario_player_starting_location *)((char *)global_scenario->player_starting_locations.address +
                    starting_location_index * sizeof(s_scenario_player_starting_location));
            *location = starting_location->position;
            *facing = starting_location->facing;
            *pitch = starting_location->pitch;
            result = true;
        }
    }
    return result;
}
bool c_player_with_unit_iterator::next()
{
    datum = (player_datum *)::data_iterator_next(&iterator);
    while (datum && datum->unit_index == NONE)
        datum = (player_datum *)::data_iterator_next(&iterator);
    return datum != 0;
}
// The globals block (players_globals): the network's machine identifier, valid flag and two more bytes.
bool players_is_zone_transition_active(void)
{
    s_players_globals *globals = players_globals;
    return globals->old_zone != NONE || globals->new_zone != NONE;
}

bool player_interaction_exists(long unused, unsigned long type_mask, struct s_player_interaction const * reference)
{
  long object_index = reference->object_index;
  return object_try_and_get_unsafe_and_verify_type(object_index, (unsigned long)-1)
      ? (((unsigned long)1 << ((s_object_header *)object_header_data->data)[(unsigned short)object_index].object_type) & type_mask) != 0
      : false;
}

void players_build_persistent_player_options(game_options *options)
{
    s_players_globals *globals = players_globals;
    options->machine_valid_mask = globals->machine_valid_mask;
    memcpy(options->machines, globals->machines, sizeof(options->machines));
    options->local_machine_exists = globals->network_valid;
    options->local_machine = globals->machine;
    long player_count = 0;
    memset(options->players, 0, sizeof(options->players));
    c_player_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        if ((unsigned long)player_count < 16) {
            player_datum *player = iterator.get_datum();
            game_player_options *player_options = &options->players[player_count];
            player_count++;
            player_options->valid = true;
            player_options->left_game = TEST_BIT(player->flags, 1);
            player_options->player_identifier = player->player_identifier;
            player_options->machine_identifier = player->machine;
            player_options->user_index = player->user_index;
            player_options->controller_index = player->controller_index;
            player_options->configuration = player->configuration;
        } else {
            break;
        }
    }
}
namespace {
static __forceinline void *players_player_tag_block_get(const s_tag_block *block, long index, long size)
{
    return (char *)block->address + index * size;
}
}
void players_store_persistent_weapon(long weapon_index, s_persistent_campaign_weapon *persistent_weapon)
{
    weapon_datum *weapon = (weapon_datum *)object_get(weapon_index);
    weapon_definition *definition = (weapon_definition *)tag_get('weap', weapon->item.object.definition_index);
    long rounds = 0;
    if (definition->barrel_count > 0) {
        s_weapon_barrel_definition *magazine = (s_weapon_barrel_definition *)players_player_tag_block_get((const s_tag_block
            *)&definition->barrel_count, 0, 288);
        if (magazine->projectile_index != NONE) {
            projectile_definition *equipment = (projectile_definition *)tag_get('proj', magazine->projectile_index);
            rounds = equipment->reporting_type;
        } else if (magazine->damage_effect_index != NONE) {
            rounds = magazine->damage_reporting_type;
        }
    }
    if (rounds == 0)
        rounds = definition->reporting_type;
    persistent_weapon->rounds = (short)rounds;
    persistent_weapon->age = (short)(long)((1.0f - weapon->age) * 100.0f + 0.5f);
    if (definition->magazines.s_tag_block::count > 0) {
        weapon_datum *magazine = weapon;
        persistent_weapon->loaded_rounds = magazine->magazines[0].rounds_inventory;
        persistent_weapon->inventory_rounds = magazine->magazines[0].rounds_loaded;
    }
}
// @address 0x1df410 ?players_build_weapon_from_persistent_campaign_data@@YAJPBUs_persistent_campaign_weapon@@JPA_N@Z
long players_build_weapon_from_persistent_campaign_data(const s_persistent_campaign_weapon *persistent_weapon, long owner_index, bool *failed)
{
    const s_persistent_campaign_weapon * persistent_weapon_loc = persistent_weapon;
    long weapon_index = NONE;
    long type = persistent_weapon_loc->rounds;
    if (type != 0) {
        scenario *scenario_view = global_scenario;
        long count = scenario_view->weapon_palette.count;
        long definition_index = NONE;
        for (long i = 0; i < count; i++) {
            long candidate = ((scenario_object_palette_entry *)scenario_view->weapon_palette.address)[i].object.index;
            if (candidate != NONE) {
                weapon_definition *definition = (weapon_definition *)tag_get('weap', candidate);
                if (definition->barrel_count > 0) {
                    s_weapon_barrel_definition *magazine = (s_weapon_barrel_definition *)definition->barrels;
                    if (magazine->projectile_index != NONE) {
                        projectile_definition *equipment = (projectile_definition *)tag_get('proj', magazine->projectile_index);
                        if (type == equipment->reporting_type) {
                            definition_index = candidate;
                            break;
                        }
                    } else if (magazine->damage_effect_index != NONE) {
                        if (type == magazine->damage_reporting_type) {
                            definition_index = candidate;
                            break;
                        }
                    }
                }
                if (type == definition->reporting_type) {
                    definition_index = candidate;
                    break;
                }
            }
        }
        if (definition_index != NONE) {
            object_placement_data placement;
            object_placement_data_new(&placement, definition_index, owner_index, 0);
            weapon_index = object_new(&placement);
            if (weapon_index != NONE) {
                weapon_datum *object = (weapon_datum *)object_get(weapon_index);
                weapon_definition *definition = (weapon_definition *)tag_get('weap', object->item.object.definition_index);
                if (definition->magazines.s_tag_block::count > 0) {
                    object->magazines[0].rounds_loaded = persistent_weapon_loc->inventory_rounds;
                    object->magazines[0].rounds_inventory = persistent_weapon_loc->loaded_rounds;
                }
                object->age = (float)(100 - persistent_weapon_loc->age) * 0.01f;
                if (object->age < 0.01f)
                    object->age = 0.0f;
                if (object->age > 0.99f)
                    object->age = 1.0f;
            } else {
                *failed = true;
            }
        } else if (persistent_weapon_loc->rounds != 0) {
            *failed = true;
        }
    }
    return weapon_index;
}

// the input-driven target of a player; no edit-mode object or a weak stick clears it.
// @address 0x1e0650 player_update_input_target
void player_update_input_target(long player_index, player_action *input)
{
    player_datum *player = player_get(player_index);
    long object_index = NONE;
    if (game_engine_is_player_in_edit_mode(player_index, &object_index, 0) && object_index != NONE && input->primary > 0.1f) {
        if (input->byte)
            player->input_extra = input->extra_input;
        player->input_target = input->target;
        player->input_active = 1;
        input->target.a = 0;
        input->target.b = 0;
    } else {
        player->input_active = 0;
    }
}


// 0x1e0738: every in-game player whose unit is attached (at any depth) under the object
// @address 0x1e0738 players_detach_units_from_object
void players_detach_units_from_object(long object_index)
{
    if (object_index != NONE && object_get(object_index)->first_child_object_index != NONE) {
        c_player_in_game_iterator iterator;
        iterator.begin();
        while (iterator.next()) {
            long unit_index = iterator.get_datum()->unit_index;
            if (unit_index != NONE) {
                for (long parent_index = object_get(unit_index)->parent_object_index; parent_index != NONE;
                    parent_index = object_get(parent_index)->parent_object_index)
                {
                    if (parent_index == object_index) {
                        unit_detach_and_reset_motor_program(unit_index);
                        break;
                    }
                }
            }
        }
    }
}
static __forceinline bool players_get_machine_identifier(s_machine_identifier *machine)
{
    s_players_globals *globals = players_globals;
    if (globals->network_valid)
        memcpy(machine, &globals->machine, sizeof(*machine));
    return globals->network_valid;
}
extern "C" bool player_is_on_local_machine(unsigned short player_index)
{
    bool result = false;
    s_machine_identifier machine;
    if (players_get_machine_identifier(&machine))
        result = memcmp(&((player_datum *)player_data->data)[player_index].machine, &machine, sizeof(machine)) == 0;
    return result;
}

// Registers a member of field layout 3 (see c_restricted_memory::add_member) and publishes its per-thread address. One of a family
// of identical allocators that differ only in the layout index, the update callback and the thread-local handle.


class c_restricted_memory;


template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_data_allocator>::allocate(unsigned int size, const char *name)
{
  int member_index = g_restricted_regions[3].add_member(name, "", size, 0, __tls_set_g_actor_data_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  data_set_new_base_address(&player_data, (s_data_array *)(g_restricted_address[3] + offset));
  m_member_index = member_index;
  m_thread_id = g_registered_thread_index;
  return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}


// @address 0x1e0958 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_data_allocator>::deallocate(void *memory)
{
    player_data = 0;
    restricted_region_free_member(3, m_member_index);
}

template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_players_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long flags)
{
  int member_index = g_restricted_regions[3].add_member(name, type, size, 0, (field_proc_t)__tls_set_g_players_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = g_restricted_address[3] + offset;
  int owner = g_registered_thread_index;
  m_member_index = member_index;
  m_thread_id = owner;
  players_globals = (s_players_globals *)address;
  return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}

// The reservation of field layout 3 under an unnamed type.
// @address 0x1e0a18 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_players_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_players_globals_allocator>::allocate(unsigned int size, const char *name)
{
  return reserve_memory(name, "", size, 0);
}


// @address 0x201440 player_get_team_index
int player_get_team_index(unsigned int index)
{
    int result = -1;
    if (!game_is_cooperative() && game_engine_has_teams() && index != 0xffffffff) {
        player_datum *base = (player_datum *)player_data->data;
        result = base[datum_index_to_absolute_index(index)].configuration.host.team_index;
    }
    return result;
}

// @address 0x1e0a30 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_players_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_players_globals_allocator>::deallocate(void *memory)
{
    int value = m_member_index;
    players_globals = 0;
    restricted_region_free_member(3, value);
}

#define FLAG_TEST(flags, bit) ((bool)(((flags) >> (bit)) & 1))
/* motor_system_in_contact_with_ground (bipeds only) */
static __forceinline bool motor_system_in_contact_with_ground(long object_index)
{
    bool result = false;
    object_datum *object = object_get(object_index);
    if (object->object_type == 0)
        result = !TEST_BIT(((biped_datum *)object)->biped.physics.m_flags, 1);
    return result;
}
/* object_get_attachment_root: climb while the object is attached (flag 26) */
static __forceinline long object_get_attachment_root(long object_index)
{
    do {
        object_datum *object = object_get(object_index);
        if (!FLAG_TEST(object->flags, 26))
            break;
        object_index = object->parent_object_index;
    } while (object_index != NONE);
    return object_index;
}
// match: 0x1d9ec8: aggregate load preserves the unit-index register allocation.
static __forceinline s_player_unit_index player_unit_index_get(player_datum *player)
{
    return *(s_player_unit_index *)&player->unit_index;
}
// @address 0x1d9ec8 players_any_are_in_the_air
bool players_any_are_in_the_air(long *out_unit_index)
{
    c_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (player->unit_index != NONE) {
            unsigned long object_index = player_unit_index_get(player).value;
            long ultimate_parent_index = NONE;
            object_datum *unit = object_get(object_index);
            while (object_index != NONE) {
                ultimate_parent_index = object_index;
                object_index = object_get(object_index)->parent_object_index;
            }
            object_datum *ultimate_parent = object_get(ultimate_parent_index);
            if (FLAG_TEST(ultimate_parent->flags, 18)) {
                *out_unit_index = player->unit_index;
                return true;
            }
            if (unit->parent_object_index == NONE) {
                if (unit->object_type == 0) {
                    if (motor_system_in_airborne_state(player->unit_index) || !motor_system_in_contact_with_ground(player->unit_index)) {
                        *out_unit_index = player->unit_index;
                        return true;
                    }
                } else if (unit->object_type == 1 && ((vehicle_datum *)unit)->vehicle.airborne_ticks) {
                    *out_unit_index = player->unit_index;
                    return true;
                }
            } else {
                long root_object_index = object_get_attachment_root(unit->parent_object_index);
                if (TEST_BIT(FLAG(1), object_get_type(root_object_index))) {
                    vehicle_datum *vehicle = (vehicle_datum *)object_get(root_object_index);
                    if (((c_vehicle_type_component *)vehicle->vehicle.type_state)
                            ->kills_riders_at_terminal_velocity(root_object_index) &&
                        vehicle->vehicle.airborne_ticks)
                    {
                        *out_unit_index = root_object_index;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// @address 0x1dae10 ?player_prepare_action@@YAXJPAUplayer_action@@@Z
void player_prepare_action(long player_index, struct player_action * action)
{
    player_datum *player = player_get(player_index);
    unsigned long original_buttons = action->flags;
    unsigned short original_triggers = action->action_flags;
    action->flags &= ~(player->suppressed_buttons & 0x78400074);
    action->action_flags &= ~(player->previous_triggers & 0x1000);
    player->suppressed_buttons = (player->suppressed_buttons & ~0x78400074u) | (original_buttons & 0x78400074);
    player->previous_triggers = (player->previous_triggers & ~0x1000) | (original_triggers & 0x1000);
    bool primary = TEST_BIT(action->action_flags,0);
    bool secondary = TEST_BIT(action->action_flags,4);
    bool tertiary = TEST_BIT(action->action_flags,8);
    bool was_primary = TEST_BIT(player->previous_triggers,0);
    bool was_secondary = TEST_BIT(player->previous_triggers,4);
    bool was_tertiary = TEST_BIT(player->previous_triggers,8);
    SET_BIT(action->action_flags,2,primary && !was_primary);
    SET_BIT(action->action_flags,3,!primary && was_primary);
    SET_BIT(player->previous_triggers,0,primary);
    SET_BIT(action->action_flags,6,secondary && !was_secondary);
    SET_BIT(action->action_flags,7,!secondary && was_secondary);
    SET_BIT(player->previous_triggers,4,secondary);
    SET_BIT(action->action_flags,10,tertiary && !was_tertiary);
    SET_BIT(action->action_flags,11,!tertiary && was_tertiary);
    SET_BIT(player->previous_triggers,8,tertiary);
    if (player->flags & 16) {
        action->action_flags &= ~15;
        SET_BIT(player->flags,4,primary);
    }
    if (player->flags & 32) {
        action->action_flags &= ~240;
        SET_BIT(player->flags,5,secondary);
    }
    if (player->flags & 64) {
        action->action_flags &= ~3840;
        SET_BIT(player->flags,6,secondary);
    }
}

// @address 0x1dee60 players_death_status_calculate
// match: 0x1dee60: pointer __restrict qualifier prevents alias reload
void players_death_status_calculate(unsigned char * __restrict a, unsigned char * __restrict b)
{
    c_player_with_unit_iterator it;
    *b = 1;
    it.iterator.absolute_index = -1;
    s_data_array *d = player_data;
    *a = 0;
    it.iterator.data = d;
    it.iterator.index = -1;
    it.datum = (player_datum *)::data_iterator_next(&it.iterator);
    while (it.datum) {
        if (it.datum->unit_index != -1) {
            *b = 0;
        } else if (it.datum->flags & 8) {
            *b = 0;
        } else {
            *a = 1;
        }
        it.datum = (player_datum *)::data_iterator_next(&it.iterator);
    }
}

// whether a player belongs to this machine.
static __forceinline const s_machine_identifier *players_local_machine_identifier()
{
    s_game_options *options = &game_globals->options;
    return &options->local_machine;
}

// @address 0x282418 players_is_player_local
extern "C" bool players_is_player_local(long player_index)
{
    s_machine_identifier machine;
    memcpy(&machine, players_local_machine_identifier(), sizeof(machine));
    return memcmp(&player_get(player_index)->machine, &machine, sizeof(machine)) == 0;
}

// whether an object's player belongs to this machine.

bool players_is_object_player_local(long object_index)
{
    long player_index = ((unit_datum *)object_get(object_index))->unit.player_index;
    return player_index != NONE && players_is_player_local(player_index);
}

// @address 0x491908 players_is_object_campaign_companion
bool players_is_object_campaign_companion(long object_index)
{
    bool result = false;
    object_datum *object = object_get(object_index);
    if (((unit_datum *)object)->unit.player_index == NONE && ((unit_datum *)object)->unit.actor_index != NONE &&
        !game_team_is_enemy((e_game_team)((unit_datum *)object)->unit.team, _game_team_player)) {
        s_game_globals *globals = ((s_game_globals *)global_game_globals);
        if (globals && globals->player_representations.count() > 1) {
            long reference_index = globals->player_representations[1].third_person_unit.index;
            if (reference_index != NONE) {
                s_object_definition *reference = (s_object_definition *)tag_get(0, reference_index);
                s_object_definition *definition = (s_object_definition *)tag_get(0, object->definition_index);
                if (reference->model.index != NONE && definition->model.index != NONE) {
                    if (((s_model_definition *)tag_get(0, definition->model.index))->render_model_index == ((s_model_definition *)tag_get(0,
                        reference->model.index))->render_model_index)
                        result = true;
                }
            }
        }
    }
    return result;
}

// cseries csmemset

// The thread-local handle of the player mapping globals' allocator: publishes the reserved address.
// @address 0x1f71c8 ?__tls_set_g_player_mapping_globals_allocator@@YAXPAX@Z
void __tls_set_g_player_mapping_globals_allocator(void *address)
{
    player_mapping_globals = (s_user_globals *)address;
}

// players_globals: cleared to its no-user, no-controller state.
// @address 0x1f71d8 players_reset_mapping_globals
void players_reset_mapping_globals(void)
{
    s_user_globals *player_mapping = player_mapping_globals;
    player_mapping->player_user_count = 0;
    player_mapping->player_controller_count = 0;
    for (long user_index = 0; user_index < 4; user_index++)
        player_mapping->player_user_mapping[user_index] = NONE;
    for (long user_index = 0; user_index < 4; user_index++)
        player_mapping->player_controller_mapping[user_index] = NONE;
    for (long controller_index = 0; controller_index < 4; controller_index++)
        player_mapping->user_controller_mapping[controller_index] = NONE;
    csmemset(player_mapping->player_input_controller_mapping, NONE, sizeof(player_mapping->player_input_controller_mapping));
    csmemset(player_mapping->player_input_user_mapping, NONE, sizeof(player_mapping->player_input_user_mapping));
    player_mapping->local_player_count = 0;
    for (long user_index = 0; user_index < 4; user_index++)
        player_mapping->player_indices[user_index] = NONE;
    for (long user_index = 0; user_index < 4; user_index++)
        player_mapping->unit_indices[user_index] = NONE;
    csmemset(&player_mapping->player_masks, 0, sizeof(player_mapping->player_masks));
}

// The next user index after user_index (the first when it is NONE) that has a player, or NONE.
// @address 0x1f7410 players_get_next_user_with_player
long players_get_next_user_with_player(long user_index)
{
    long result = NONE;
    long first_user_index = user_index == NONE ? 0 : user_index + 1;
    for (long index = first_user_index; index < 4; index++) {
        if (player_mapping_globals->player_user_mapping[index] != NONE) {
            result = index;
            break;
        }
    }
    return result;
}


void players_set_unit_for_player_views(int index, int value)
{
    int active_index = player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(index)];
    if (active_index != -1) {
        players_set_controller_mapping(active_index, value);
    }

    int child_index = -1;
    while (true) {
        if (index != -1) {
            child_index = ((long)::director_get_perspective(index, child_index));
        }
        if (child_index == -1)
            break;
        players_set_user_unit(child_index, value);
    }
}


// @address 0x1f7648 players_remove_local_player
void players_remove_local_player(long player_index)
{
    s_user_globals *globals = player_mapping_globals;
    if (globals->player_masks.masks[datum_index_to_absolute_index(player_index)] != 0) {
        long controller_index = -1;
        while (true) {
            if (player_index != -1)
                controller_index = ((long)::director_get_perspective(player_index, controller_index));
            if (controller_index == -1)
                break;
            long occupant = globals->player_indices[controller_index];
            if (occupant != -1) {
                players_set_user_unit(controller_index, -1);
                globals = player_mapping_globals;
                globals->player_masks.masks[datum_index_to_absolute_index(occupant)] &= ~(1 << controller_index);
                globals->player_indices[controller_index] = -1;
                globals->local_player_count--;
            }
        }
    }
}

void players_set_controller_mapping(int active_index, int value)
{
    if (value == player_mapping_globals->player_controller_mapping[active_index])
        return;
    if (value != -1) {
        players_set_controller_mapping(active_index, -1);
        player_mapping_globals->player_controller_mapping[active_index] = value;
        player_control_set_user_unit(active_index, value);
        return;
    }
    player_control_set_user_unit(active_index, -1);
    player_mapping_globals->player_controller_mapping[active_index] = -1;
}

static __forceinline long &players_controller_array_element(long *array, unsigned long byte_offset) { return *(long *)((unsigned char *)array + byte_offset); }
// @address 0x1f7568 players_set_local_player
extern "C" void players_set_local_player(long input_user_index, long player_index)
{
    unsigned long player_offset = (input_user_index + offsetof(s_user_globals, player_indices) / sizeof(long)) * sizeof(long);
    s_user_globals *mapping = player_mapping_globals;
    long previous_player_index = players_controller_array_element(mapping->player_indices, player_offset - offsetof(s_user_globals, player_indices));
    if (previous_player_index != player_index) {
        if (player_index == NONE) {
            players_set_user_unit(input_user_index, NONE);
            mapping = player_mapping_globals;
            mapping->player_masks.masks[(unsigned short)previous_player_index] &= ~(1 << input_user_index);
            players_controller_array_element(mapping->player_indices, player_offset - offsetof(s_user_globals, player_indices)) = NONE;
            mapping->local_player_count--;
        } else {
            long absolute_player_index = datum_index_to_absolute_index(player_index);
            player_datum *player = &((player_datum *)player_data->data)[absolute_player_index];
            players_set_local_player(input_user_index, NONE);
            mapping = player_mapping_globals;
            players_controller_array_element(mapping->player_indices, player_offset - offsetof(s_user_globals, player_indices)) = player_index;
            mapping->player_masks.masks[absolute_player_index] |= 1 << input_user_index;
            mapping->local_player_count++;
            players_set_user_unit(input_user_index, player->unit_index);
        }
    }
}


// @address 0x1f7700 player_iterate_output_users
bool player_iterate_output_users(player_iterator *iterator)
{
    if (iterator->player_index != NONE) {
        iterator->controller_index = ((long)::director_get_perspective(iterator->player_index, iterator->controller_index));
    }
    return iterator->controller_index != NONE;
}

struct control;

static __forceinline void players_control_set_unit(long user_index, long unit_index)
{
    if (user_index != NONE) {
        s_player_control_globals *globals = (s_player_control_globals *)player_control_globals;
        s_player_control_state *state = &globals->states[user_index];
        if (!globals->flag || state->unit_index != unit_index) {
            state->unit_index = unit_index;
            player_control_set_action_from_unit(unit_index, &state->action);
        }
    }
}

// @address 0x1f7750 players_set_user_unit
void players_set_user_unit(int user_index, int unit_index)
{
    long previous_unit_index = player_mapping_globals->unit_indices[user_index];
    if (unit_index != previous_unit_index) {
        if (unit_index != NONE) {
            players_set_user_unit(user_index, NONE);
            player_mapping_globals->unit_indices[user_index] = unit_index;
            players_control_set_unit(user_index, unit_index);
            first_person_weapon_attach(user_index, unit_index, true);
            game_engine_mark_user_unit_changed(user_index, unit_index);
            effect_update_first_person_user_flags(unit_index, user_index, true);
        } else {
            players_control_set_unit(user_index, NONE);
            first_person_weapon_attach(user_index, NONE, true);
            effect_update_first_person_user_flags(previous_unit_index, user_index, false);
            player_mapping_globals->unit_indices[user_index] = NONE;
        }
    }
}


// The reservation of players_globals in field layout 3.
// @address 0x1f7890 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_mapping_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_mapping_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long flags)
{
  int member_index = g_restricted_regions[3].add_member(name, type, size, 0, __tls_set_g_player_mapping_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = g_restricted_address[3] + offset;
  int owner = g_registered_thread_index;
  m_member_index = member_index;
  m_thread_id = owner;
  player_mapping_globals = (s_user_globals *)address;
  return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}

// The allocate of player_mapping_globals' allocator (its vtable's slot): a reservation under no type.
// @address 0x1f7930 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_mapping_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_mapping_globals_allocator>::allocate(unsigned int size, const char *name)
{
  return reserve_memory(name, "", size, 0);
}

// The deallocate of player_mapping_globals' allocator: clears the thread's handle and frees the member.
// @address 0x1f7948 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_mapping_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_mapping_globals_allocator>::deallocate(void *object)
{
    // match: 0x1f7948: pointer __restrict qualifier prevents alias reload
    t_restricted_allocation_manager * __restrict self = this;
    player_mapping_globals = 0;
    restricted_region_free_member(3, self->m_member_index);
}


// @address 0x1d7578 ?players_initialize@@YAXXZ
void players_initialize(void)
{
    player_data = data_new("players", 16, sizeof(player_datum), 0, (c_allocation_base *)&g_player_data_allocator);
    players_globals = (s_players_globals *)g_players_globals_allocator.reserve_memory("players globals", 0, 284, 0);
    player_mapping_globals = (s_user_globals *)g_player_mapping_globals_allocator.reserve_memory("player mapping globals", 0, sizeof(s_user_globals), 0);
    players_reset_mapping_globals();
}


// The unit's facing vectors player_set_facing writes.
// @address 0x1d9370 ?player_set_facing@@YAXJPBTreal_vector3d@@@Z
void player_set_facing(long player_index, const real_vector3d *facing)
{
    player_datum *player = player_get(player_index);
    if (!game_simulation::game_is_predicted()) {
        if (player->unit_index != NONE) {
            unit_datum *unit = (unit_datum *)object_get(player->unit_index);
            unit->unit.desired_facing_vector = *facing;
            unit->unit.desired_aiming_vector = *facing;
            unit->unit.desired_looking_vector = *facing;
            ::simulation_action_object_update(player->unit_index, 0x10000);
        }
    }
    long user_index = player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player_index)];
    if (user_index != NONE)
        player_control_set_facing(user_index, facing);
}

// The zones a zone state has loaded.
static __forceinline bool players_mask_contains(unsigned long mask, unsigned long bits)
{
    return (mask & bits) == bits;
}

// Whether switching zone_state to the zone set zone_set_index has to load anything it does not have.
// @address 0x117ff8 players_is_zone_loading_required
bool players_is_zone_loading_required(const s_scenario_game_state *zone_state, long zone_set_index)
{
    const s_scenario_zone_set *zone_set = &((s_scenario_zone_set *)global_scenario->zone_sets.address)[zone_set_index];
    bool result = true;
    if (zone_state->zone_set_index == zone_set_index ||
        (players_mask_contains(zone_state->active_structure_bsp_mask, zone_set->bsp_active_mask) &&
        players_mask_contains(zone_state->active_designer_zone_mask, zone_state->active_designer_zone_mask & ~(zone_set_index != NONE ?
            zone_set->designer_zone_forbidden_mask : 0)) &&
        players_mask_contains(zone_state->touched_cinematic_zone_mask, zone_set->cinematic_zones)))
        result = false;
    return result;
}


// chud.cpp's chud_submit_navpoint
static __forceinline void players_chud_submit_navpoint(long user_index, s_chud_navpoint *navpoint)
{
    if (user_index >= 0 && user_index < 4)
        c_chud_manager::x_persistent_user_data[user_index].m_navpoint_manager.submit_navpoint(navpoint);
}



// @address 0x2e11e0 ?submit_nav_points@c_game_engine@@UBAXJJ@Z
void c_game_engine::submit_nav_points(long user_index, long target_player_index) const
{
    long user_player_index = player_index_from_user_index_render(user_index);
    if (user_player_index != NONE) {
        c_player_in_game_iterator iterator;
        iterator.begin();
        while (iterator.next()) {
            long player_index = iterator.get_index();
            bool should_draw = should_draw_nav_point(user_index, player_index);
            if (should_draw || player_index == target_player_index && target_player_index != user_player_index) {
                s_chud_navpoint navpoint;
                if (build_player_nav_point(user_index, player_index, target_player_index, !should_draw, &navpoint))
                    players_chud_submit_navpoint(user_index, &navpoint);
            }
        }
    }
}


static __forceinline short players_game_engine_index_get(void)
{
    short game_engine_index = 0;
    const c_game_variant *variant = current_game_variant();
    if (variant)
        game_engine_index = (short)variant->game_engine_index;
    return game_engine_index;
}
static __forceinline long players_sandbox_editor_object_get(long player_index)
{
    long object_index = NONE;
    long editor_index = game_engine_globals->sandbox.owners[datum_index_to_absolute_index(player_index)];
    if (editor_index != NONE)
        object_index = game_engine_globals->map_info.objects[editor_index].object_index;
    return object_index;
}

static __forceinline long players_input_user_get(long player_index)
{
    return player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player_index)];
}
static __forceinline const c_game_engine *current_game_engine_inline(void)
{
    long game_engine_index = game_engine_globals->game_engine_index;
    if (game_engine_index < 1)
        return 0;
    if (game_engine_index > 10)
        return 0;
    return game_engines[game_engine_index];
}
// @address 0xa08c0 ?game_engine_is_player_in_edit_mode@@YA_NJPAJ0@Z
bool game_engine_is_player_in_edit_mode(long player_index, long *object_index, long *editor_index)
{
    bool result = false;
    if (player_index != NONE && current_game_engine_inline() && game_globals->options.game_mode == 2 &&
        !game_engine_player_is_awaiting_respawn(player_index) && players_game_engine_index_get() == 5)
    {
        result = game_engine_sandbox_is_player_in_edit_mode(player_index);
        if (result && object_index)
            *object_index = players_sandbox_editor_object_get(player_index);
        if (editor_index && players_input_user_get(player_index) != NONE)
            *editor_index = game_engine_globals->sandbox.handle_slots[datum_index_to_absolute_index(player_index)];
    }
    return result;
}

// The armaments a campaign player carries into the next level.

// moves a player onto another unit, releasing the unit it held before.
static __forceinline void players_set_point3d(real_point3d *p, float x, float y, float z) { p->x = x; p->y = y; p->z = z; }
void player_set_unit_index(long player_index, long unit_index)
{
    player_datum *player = (player_datum *)player_get(player_index);
    if (player->unit_index == unit_index)
        return;
    if (player->unit_index != NONE) {
        unit_datum *previous = ((unit_datum *)object_get(player->unit_index));
        players_set_unit_for_player_views(player_index, NONE);
        previous->unit.player_index = NONE;
        previous->unit.controller_player_index = player_index;
        ::simulation_action_object_update(player->unit_index, 0x4000);
        ::unit_set_actively_controlled(player->unit_index, 0);
        player->unit_index = NONE;
    }
    float reset = 65536.0f;
    player->suppressed_buttons = 0;
    player->previous_triggers = 0;
    player->sprint_ticks = 0;
    player->sprint_recovery_ticks = 0;
    players_set_point3d(&player->position, reset, reset, reset);
    if (unit_index == NONE)
        return;
    unit_datum *next = ((unit_datum *)object_get(unit_index));
    next->unit.player_index = player_index;
    next->unit.controller_player_index = NONE;
    ::simulation_action_object_update(unit_index, 0x4000);
    ::unit_set_actively_controlled(unit_index, 1);
    long pending = player->dead_unit_index;
    player->unit_index = unit_index;
    if (pending != NONE && pending != unit_index) {
        unit_datum *pending_datum = ((unit_datum *)object_get(pending));
        if (pending_datum->object.recycling_flags & 2)
            pending_datum->object.garbage_time = game_time_globals->time;
    }
    player->dead_unit_index = NONE;
    players_set_unit_for_player_views(player_index, unit_index);
    unit_dialogue_set_variant(unit_index, player->configuration.client.appearance.flags & 1);
    if (current_game_engine() != 0)
        next->object.shield_vitality = player->traits.m_shield_vitality_traits.get_maximum_shield_vitality();
    if (game_globals->options.game_mode == 2) {
        player->melee_remaining_ticks = 2;
        player->grenade_recovery_ticks = 2;
    }
}

// @address 0x1df158 ?players_store_persistent_campaign_data_on_game_won@@YAXPAUs_campaign_armaments@@@Z
void players_store_persistent_campaign_data_on_game_won(s_campaign_armaments *armaments)
{
    long player_count = 0;
    memset(armaments, 0, sizeof(*armaments));
    c_player_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        if (player_count >= 4)
            break;
        player_datum *player = iterator.get_datum();
        s_persistent_campaign_player *armaments_player = &armaments->players[player_count++];
        long unit_index = player->unit_index;
        if (unit_index != NONE && armaments_player) {
            unit_datum *unit = (unit_datum *)object_get(unit_index);
            long primary_weapon_index = NONE;
            long secondary_weapon_index = NONE;
            long backpack_weapon_index = NONE;
            if ((unsigned short)unit->unit.parent_seat_index != 0xffff) {
                for (long pass = 0; pass < 2; pass++) {
                    long game_time = game_time_get();
                    long best_age = 0x7fffffff;
                    long best_weapon_index = NONE;
                    for (long weapon_slot = 0; weapon_slot < 4; weapon_slot++) {
                        long age = game_time - unit->unit.weapon_ages[weapon_slot];
                        if (unit->unit.weapon_object_indices[weapon_slot] != NONE &&
                            (pass == 0 || unit->unit.weapon_object_indices[weapon_slot] != primary_weapon_index) &&
                            age < best_age) {
                            best_age = age;
                            best_weapon_index = unit->unit.weapon_object_indices[weapon_slot];
                        }
                    }
                    if (pass == 0)
                        primary_weapon_index = best_weapon_index;
                    else
                        backpack_weapon_index = best_weapon_index;
                }
            } else {
                primary_weapon_index = unit_inventory_get_weapon_or_none(unit_index, unit->unit.current_weapon_index);
                secondary_weapon_index = unit_inventory_get_weapon_or_none(unit_index, unit->unit.desired_weapon_index);
                backpack_weapon_index = ::unit_get_current_backpack_weapon(unit_index);
            }
            if (primary_weapon_index != NONE)
                players_store_persistent_weapon(primary_weapon_index, &armaments_player->primary_weapon);
            else
                armaments_player->primary_weapon.rounds = 0;
            if (secondary_weapon_index != NONE)
                players_store_persistent_weapon(secondary_weapon_index, &armaments_player->secondary_weapon);
            else
                armaments_player->secondary_weapon.rounds = 0;
            if (backpack_weapon_index != NONE)
                players_store_persistent_weapon(backpack_weapon_index, &armaments_player->backpack_weapon);
            else
                armaments_player->backpack_weapon.rounds = 0;
            for (long grenade_index = 0; grenade_index < 4; grenade_index++)
                armaments_player->grenade_counts[grenade_index] = unit->unit.grenade_counts[grenade_index];
            armaments_player->valid = true;
        } else {
            memset(armaments_player, 0, sizeof(*armaments_player));
        }
    }
}

bool players_arm_unit_from_persistent_campaign_data(struct s_persistent_campaign_player const * persistent_player, long unit_index)
{
  unit_datum *unit;
  unsigned int primary_weapon_index;
  unsigned int secondary_weapon_index;
  unsigned int backpack_weapon_index;
  unsigned char inventory_value;
  int grenade_index;
  bool success;
  unsigned char weapon_build_failed [8];

  success = false;
  if (*((unsigned char *)persistent_player) != '\0') {
    unit = (unit_datum *)object_get(unit_index);
    ::unit_delete_all_weapons(((unsigned int)unit_index));
    weapon_build_failed[0] = '\0';
    primary_weapon_index = (unsigned int)players_build_weapon_from_persistent_campaign_data(&persistent_player->primary_weapon, unit_index,
        (bool *)weapon_build_failed);
    secondary_weapon_index = (unsigned int)players_build_weapon_from_persistent_campaign_data(&persistent_player->secondary_weapon, unit_index,
        (bool *)weapon_build_failed);
    backpack_weapon_index = (unsigned int)players_build_weapon_from_persistent_campaign_data(&persistent_player->backpack_weapon, unit_index,
        (bool *)weapon_build_failed);
    grenade_index = 0;
    success = weapon_build_failed[0] == '\0';
    do {
      unit->unit.grenade_counts[grenade_index] = 0;
      inventory_value = ((unsigned char *)persistent_player)[grenade_index + 0x1a];
      unit->unit.grenade_counts[grenade_index] = inventory_value;
      if (((unsigned char)unit->unit.desired_grenade_index == 255) && ((signed char)inventory_value > 0)) {
        unit->unit.current_grenade_index = (signed char)grenade_index;
        unit->unit.desired_grenade_index = (signed char)grenade_index;
      }
      grenade_index = grenade_index + 1;
    } while (grenade_index < 4);
    if (((int)primary_weapon_index != -1) && (inventory_value = ::unit_add_weapon_to_inventory(((unsigned int)unit_index), primary_weapon_index,
        _weapon_addition_method_silent), inventory_value == '\0')) {
      ::object_delete(primary_weapon_index);
      success = false;
    }
    if (((int)backpack_weapon_index != -1) && (inventory_value = ::unit_add_weapon_to_inventory(((unsigned int)unit_index),
        backpack_weapon_index, _weapon_addition_method_silent), inventory_value == '\0')) {
      ::object_delete(backpack_weapon_index);
      success = false;
    }
    if (((int)secondary_weapon_index != -1) && (inventory_value = ::unit_add_weapon_to_inventory(((unsigned int)unit_index),
        secondary_weapon_index, _weapon_addition_method_silent_dual_wield), inventory_value == '\0')) {
      ::object_delete(secondary_weapon_index);
      success = false;
    }
  }
  return success;
}


static __forceinline bool players_color_index_valid(signed char color_index)
{
    return color_index >= NONE && color_index < 32;
}

static __forceinline bool players_emblem_flags_valid(unsigned char flags)
{
    bool valid = true;
    valid = valid && (flags & ~7) == 0;
    return valid;
}

// @address 0x1e0460 player_appearance_valid
bool player_appearance_valid(const s_player_appearance *appearance)
{
    bool valid = true;
    unsigned long color_index = 0;
    for (;;) {
        valid = valid && players_color_index_valid(appearance->change_colors[color_index]);
        if (++color_index >= sizeof(appearance->change_colors))
            break;
    }
    valid = valid && appearance->player_model_choice >= 0 && appearance->player_model_choice < 2;
    valid = valid && appearance->foreground_emblem < 80;
    valid = valid && appearance->background_emblem < 48;
    for (long model_index = 0; model_index < 2; model_index++) {
        long permutation_index = 0;
        for (;;) {
            valid = valid && appearance->model_permutations[model_index][permutation_index] < 32;
            if (++permutation_index >= 4)
                break;
        }
    }
    valid = valid && players_emblem_flags_valid(appearance->emblem_flags);
    valid = valid && appearance->emblem_primary_color >= 0 && appearance->emblem_primary_color < 32;
    valid = valid && appearance->emblem_secondary_color >= 0 && appearance->emblem_secondary_color < 32;
    valid = valid && appearance->emblem_background_color >= 0 && appearance->emblem_background_color < 32;
    valid = valid && appearance->emblem_pad == 0;
    return valid;
}


static __forceinline float point_distance3d(const real_point3d *a, const real_point3d *b)
{
    real_vector3d v;
    vector_from_points3d(a, b, &v);
    return sqrtf(v.j * v.j + v.k * v.k + v.i * v.i);
}

// The distance from a scenario point to the nearest living player unit, in quarter units (0x7fff when none).
// @address 0x50ab20 players_get_nearest_player_distance
short players_get_nearest_player_distance(const cs_point *scenario_point)
{
    short closest_distance = 0x7fff;
    real_point3d position;
    ::ai_point_get_position(&scenario_point->position, &position);
    c_player_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (TEST_BIT(player->flags, 0) && !TEST_BIT(player->flags, 1) && player->unit_index != NONE) {
            real_point3d origin;
            object_get_origin(player->unit_index, &origin);
            float distance = point_distance3d(&position, &origin) * 4.0f;
            if (distance < closest_distance)
                closest_distance = (short)distance;
        }
    }
    return closest_distance;
}

static __forceinline bool players_point2d_valid(const real_point2d *point)
{
    return valid_real(point->x) && valid_real(point->y);
}
static __forceinline bool players_interaction_valid(const s_player_interaction *interaction)
{
    bool valid = true;
    valid = valid && interaction != 0;
    valid = valid && interaction->type < 11;
    if (valid && interaction->type != 0)
        valid = valid && interaction->object_index != NONE;
    return valid;
}
static __forceinline bool players_action_context_valid(const s_player_action_context *context)
{
    bool valid = true;
    valid = valid && context != 0;
    valid = valid && players_interaction_valid(&context->interaction);
    return valid;
}

// Whether a player action (as it arrives from a client) holds only values in range.
// @address 0xa33d8 player_is_action_valid
bool player_is_action_valid(player_action *action)
{
    bool valid = true;
    valid = valid && action != 0;
    valid = valid && action->unit_valid >= 0 && action->unit_valid < 2;
    if (action->unit_valid == 1)
        valid = valid && action->unit_flags < 16;
    else
        valid = valid && action->unit_flags == 255;
    valid = valid && valid_real(action->facing.pitch);
    valid = valid && valid_real(action->facing.yaw);
    valid = valid && valid_real(action->primary);
    valid = valid && valid_real(action->secondary);
    valid = valid && action->primary >= 0.0f && action->primary <= 1.0f;
    valid = valid && players_point2d_valid(&action->move);
    valid = valid && action->move.x >= -1.0f && action->move.x <= 1.0f;
    valid = valid && action->move.y >= -1.0f && action->move.y <= 1.0f;
    valid = valid && (action->weapons.weapon_indices[0] == NONE || action->weapons.weapon_indices[0] >= 0 && action->weapons.weapon_indices[0] < 4);
    valid = valid && (action->weapons.weapon_indices[1] == NONE || action->weapons.weapon_indices[1] >= 0 && action->weapons.weapon_indices[1] < 4);
    valid = valid && (action->weapons.weapon_indices[0] == NONE || action->weapons.weapon_indices[0] != action->weapons.weapon_indices[1]);
    valid = valid && (action->grenade == NONE || action->grenade >= 0 && action->grenade < 4);
    valid = valid && (action->zoom == NONE || action->zoom >= 0 && action->zoom < 4);
    valid = valid && (action->action_flags & ~0x3fffUL) == 0;
    valid = valid && players_test_targeting_bounds(&action->aim_assist);
    return valid && players_action_context_valid(&action->action_context);
}

// Whether a zone transition is possible without loading a different destination.
// @address 0x1dc610 players_is_zone_transition_valid
bool players_is_zone_transition_valid(long transition_index, long source_index, long unused, long previous_index)
{
    bool result = false;
    scenario *scenario_definition = global_scenario;
    if (transition_index != NONE) {
        short zone_set_index =
            ((s_scenario_zone_set_switch_trigger_volume *)scenario_definition->zone_set_switch_trigger_volumes.address)[transition_index].begin_zone_set;
        s_scenario_game_state zone_state;
        zone_state.flags = 1;
        zone_state.zone_set_index = global_zone_set_index;
        zone_state.active_structure_bsp_mask = g_active_structure_bsp_mask;
        zone_state.active_designer_zone_mask = g_active_designer_zone_mask;
        zone_state.touched_structure_bsp_mask = g_touched_structure_bsp_mask;
        zone_state.active_cinematic_zone_mask = g_active_cinematic_zone_mask;
        zone_state.touched_cinematic_zone_mask = g_touched_cinematic_zone_mask;
        if (!players_is_zone_loading_required(&zone_state, zone_set_index))
            result = true;
    } else if (previous_index != NONE) {
        if (((s_scenario_zone_set_switch_trigger_volume *)scenario_definition->zone_set_switch_trigger_volumes.address)[previous_index].commit_zone_set ==
            ((s_scenario_zone_set_switch_trigger_volume *)scenario_definition->zone_set_switch_trigger_volumes.address)[source_index].commit_zone_set)
            result = true;
    } else {
        result = false;
    }
    return result;
}


// @address 0x1d9960 ?players_update_activation@@YAXXZ
void players_update_activation(void)
{
    if (!game_simulation::game_is_predicted()) {
        c_player_data_iterator<player_datum> iterator;
        iterator.begin(player_data);
        while (iterator.next()) {
            unsigned long flags = iterator.get_datum()->flags;
            player_datum *p = iterator.get_datum();
            if (!(flags & 2)) {
                bool active = (flags & 1) != 0;
                bool queue_event = false;
                if (game_is_distributed()) {
                    if (!game_is_cooperative()) {
                        active = simulation_get_machine_bandwidth_events(&p->machine);
                        queue_event = true;
                    }
                } else active = true;
                if (((flags & 1) != 0) != active) {
                    if (queue_event) {
                        s_simulation_queue_player_event_data data;
                        memset(&data.respawn_target_absolute_player_index, 0, sizeof(data.respawn_target_absolute_player_index));
                        data.active = active;
                        simulation_queue_player_event_insert(_simulation_queue_player_event_active, iterator.iterator.index, &data);
                    } else {
                        if (active) p->flags = flags | 1; else p->flags = flags & ~1;
                        if (active) game_engine_notify_player_activated(iterator.iterator.index);
                    }
                }
            }
        }
    }
}

// @address 0x1d7cc0 players_remap_cooperative_users
void players_remap_cooperative_users(int value)
{
    if (!game_is_cooperative())
        return;
    if ((unsigned char)value != 0) {
        c_player_with_unit_iterator it;
        it.iterator.absolute_index = -1;
        it.iterator.index = -1;
        it.iterator.data = player_data;
        it.datum = (player_datum *)data_iterator_next(&it.iterator);
        if (it.datum != 0) {
            do {
                long player_index = it.iterator.index;
                players_set_input_user(player_index, -1);
                long absolute_player_index = datum_index_to_absolute_index(player_index);
                s_user_globals *globals = player_mapping_globals;
                long user_index = globals->player_input_controller_mapping[absolute_player_index];
                if (user_index != -1) {
                    globals->user_controller_mapping[user_index] = -1;
                    globals->player_input_controller_mapping[absolute_player_index] = -1;
                    globals->player_controller_count--;
                }
                it.datum = (player_datum *)data_iterator_next(&it.iterator);
            } while (it.datum != 0);
        }
        s_user_globals *globals = player_mapping_globals;
        for (long controller = 0; controller < 4; controller++) {
            long occupant = globals->player_indices[controller];
            if (occupant != -1) {
                players_set_user_unit(controller, -1);
                globals = player_mapping_globals;
                globals->player_masks.masks[datum_index_to_absolute_index(occupant)] &= ~(1 << controller);
                globals->player_indices[controller] = -1;
                globals->local_player_count--;
            }
        }
    }
    if (g_saved_film_timer_globals.player_remap_enabled != 0) {
        long map[4];
        memset(map, -1, sizeof(map));
        c_player_with_unit_iterator it;
        it.iterator.absolute_index = -1;
        it.iterator.index = -1;
        it.iterator.data = player_data;
        it.datum = (player_datum *)data_iterator_next(&it.iterator);
        if (it.datum != 0) {
            s_players_globals *globals = players_globals;
            do {
                if (it.datum->machine_index == globals->local_machine_index)
                    map[it.datum->user_index] = it.iterator.index;
                it.datum = (player_datum *)data_iterator_next(&it.iterator);
            } while (it.datum != 0);
        }
        long user = 0;
        long *slot = map;
        do {
            players_set_local_player(user, *slot);
            user++;
            slot++;
        } while (user < 4);
        return;
    }
    long fallback = -1;
    s_players_globals *globals = players_globals;
    if (globals->local_machine_index != -1) {
        c_player_in_game_iterator iterator;
        iterator.begin();
        while (iterator.next()) {
            if (iterator.get_datum()->machine_index == globals->local_machine_index) {
                fallback = iterator.get_index();
                break;
            }
        }
    }
    if (fallback == -1) {
        c_player_in_game_iterator iterator;
        iterator.begin();
        if (iterator.next())
            fallback = iterator.get_index();
    }
    for (long user = 0; user < 4; user++) {
        if (user_interface_controller_get_rumble_enabled(user) != -1) {
            if (player_index_from_user_index_render(user) == -1)
                players_set_local_player(user, fallback);
        }
    }
    if ((unsigned short)player_mapping_globals->local_player_count == 0)
        players_set_local_player(0, fallback);
}


// @address 0x1d77c8 ?players_remap_local_players_from_flags@@YAXJ@Z
void players_remap_local_players_from_flags(long flags)
{
    if (TEST_BIT(flags, 2)) {
        s_machine_identifier local_machine_identifier;
        long local_player_count;
        game_player_options local_players[16];
        user_interface_build_local_machine_and_players(&local_machine_identifier, &local_player_count, local_players);
        players_remap_local_players(&local_machine_identifier, local_players);
    }
    if (game_is_cooperative())
        players_remap_cooperative_users(true);
}

void player_set_configuration(long player_index, const s_player_configuration *player_configuration)
{
    player_datum *player = player_get(player_index);
    s_player_configuration previous;
    previous = player->configuration;
    player->desired_configuration = *player_configuration;
    s_player_configuration configuration = *player_configuration;
    long game_mode = game_globals->options.game_mode;
    if (game_mode == 1)
        configuration.host.team_index = game_mode;
    signed char mode = configuration.client.appearance.player_model_choice;
    mode = (mode > 0 ? mode : 0) < 2 ? (mode > 0 ? mode : 0) : 2;
    configuration.client.appearance.player_model_choice = mode;
    signed char model = configuration.client.bungienet_user_role;
    if (model >= 0 && model < 9)
        configuration.client.bungienet_user_role = 4;
    game_engine_player_prepare_to_change_team(player_index, configuration.host.team_index);
    player->configuration = configuration;
    wcsncmp(previous.host.name, player->configuration.host.name, 16);
    long previous_team = previous.host.team_index;
    if (previous_team != player->configuration.host.team_index)
        game_engine_player_changed_teams(player_index, (e_game_team)previous_team, (e_game_team)player->configuration.host.team_index);
    if (player->unit_index != -1)
        ::player_copy_object_appearance(player_index, player->unit_index);
}

// @address 0x1de270 player_handle_flip_interaction
unsigned char player_handle_flip_interaction(long player_index, const s_player_interaction *interaction)
{
    unsigned char result = false;
    player_datum *player = player_get(player_index);
    if (interaction->type == 10 && ::player_interaction_exists(player_index, 2, interaction)) {
        unsigned char predicted = false;
        if (game_globals->options.game_simulation == 4)
            predicted = true;
        if (predicted) {
            long unit_index = player->unit_index;
            if (simulation_query_local_predictive_control(unit_index, 0)) {
                long references[2];
                references[1] = unit_index;
                references[0] = interaction->object_index;
                simulation_event_generate(_simulation_event_type_vehicle_flip, 2, references, -1, 0, 0,
                    g_network_configuration.event_relevance[_simulation_event_type_vehicle_flip].delay_msec);
            }
        } else {
            action_request request;
            memset(&request, 0, sizeof(request));
            request.type = 21;
            request.unit_index = player->unit_index;
            action_submit(interaction->object_index, &request);
        }
        result = true;
    }
    return result;
}
// @address 0x1de3a0 ?player_handle_interaction_held@@YA_NJPBUs_player_interaction@@@Z
static __forceinline void players_training_device_touched(long device_index, long player_index)
{
    long input_user_index = player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player_index)];
    if (input_user_index != -1)
        ((s_player_training_entry_data *)player_training_globals)[input_user_index].device_touched_ticks = game_time_globals->ticks_per_second;
}
static __forceinline int players_user_index_out_of_range(long user_index) { if (user_index < 0) return 1; if (user_index >= 4) return 1; return 0; }
bool player_handle_interaction_held(long player_index, const s_player_interaction *interaction)
{
    player_datum *player = player_get(player_index);
    bool result = false;
    // Function-scope requests: each switch arm's request keeps its own stack slot (112/176/240).
    action_request seat_request;
    action_request interact_request;
    action_request pickup_request;
    switch (interaction->type) {
    case 3: {
        if (!::player_interaction_exists(player_index, 400, interaction))
            break;
        bool predicted = result;
        if (game_globals->options.game_simulation == 4)
            predicted = true;
        if (predicted) {
            long unit_index = player->unit_index;
            if (simulation_query_local_predictive_control(unit_index, 0)) {
                long references[2];
                references[1] = unit_index;
                references[0] = interaction->object_index;
                simulation_event_generate(_simulation_event_type_device_touch, 2, references, -1, 0, 0,
                g_network_configuration.event_relevance[_simulation_event_type_device_touch].delay_msec);
            }
            players_training_device_touched(interaction->object_index, player_index);
        } else {
            device_touched(interaction->object_index, player->unit_index);
            players_training_device_touched(interaction->object_index, player_index);
        }
        result = true;
        break;
    }
    case 4:
    case 5:
    case 8: {
        if (!::player_interaction_exists(player_index, 4099, interaction))
            break;
        short seat_index = interaction->seat_index;
        object_datum *object = object_get(interaction->object_index);
        unit_definition *definition =
            (unit_definition *)tag_get(NONE, object->definition_index);
        if (seat_index >= 0 && seat_index < definition->seats.count()) {
            bool can_enter; long evict_seat;
            { long out_seat_index = -1;
            bool flag_b = false;
            bool flag_a = false;
            can_enter = unit_can_enter_seat(player->unit_index, interaction->object_index, seat_index, &out_seat_index, &flag_b, &flag_a);
            evict_seat = out_seat_index;
            }
            if (can_enter) {
                bool predicted = result;
                if (game_globals->options.game_simulation == 4)
                    predicted = true;
                if (predicted) {
                    long unit_index = player->unit_index;
                    if (simulation_query_local_predictive_control(unit_index, 0)) {
                        long references[2];
                        references[0] = unit_index;
                        references[1] = interaction->object_index;
                        long seat_payload = seat_index;
                        simulation_event_generate(_simulation_event_type_unit_enter_vehicle, 2, references, -1, 4, &seat_payload,
                            g_network_configuration.event_relevance[_simulation_event_type_unit_enter_vehicle].delay_msec);
                    }
                } else {
                    memset(&seat_request, 0, sizeof(seat_request));
                    seat_request.type = 17;
                    seat_request.unit_index = interaction->object_index;
                    seat_request.seat_index = seat_index;
                    seat_request.flag_a = false;
                    seat_request.flag_b = false;
                    action_submit(player->unit_index, &seat_request);
                }
                result = true;
            } else if (evict_seat != -1) {
                object_datum *occupant = object_get(evict_seat);
                bool predicted = result;
                if (game_globals->options.game_simulation == 4)
                    predicted = true;
                if (!predicted) {
                    if (((unit_datum *)occupant)->unit.actor_index != -1)
                        ai_try_vehicle_eviction(((unit_datum *)occupant)->unit.actor_index, player->unit_index, true);
                }
                result = true;
            }
        }
        break;
    }
    case 6: {
        long perspective = -1;
        s_chud_globals_definition *chud = ((s_chud_globals_definition *)chud_globals);
        for (;;) {
            if (player_index != NONE)
                perspective = ((long)::director_get_perspective(player_index, perspective));
            if (perspective == -1)
                break;
            if (!players_user_index_out_of_range(perspective) && chud != 0) {
                long sound_index = chud->rows[chud_get_user_skin_index(perspective)].equipment_pickup_sound.index;
                if (sound_index != -1) {
                    unspatialized_impulse_sound_new(sound_index, 1.0f);
                    chud = ((s_chud_globals_definition *)chud_globals);
                }
            }
        }
        result = true;
        break;
    }
    case 7: {
        if (!::player_interaction_exists(player_index, 1, interaction))
            break;
        bool predicted = result;
        if (game_globals->options.game_simulation == 4)
            predicted = true;
        if (predicted)
            break;
        memset(&interact_request, 0, sizeof(interact_request));
        interact_request.unit_index = player_index;
        interact_request.type = 45;
        result = action_submit(interaction->object_index, &interact_request);
        break;
    }
    case 2: {
        if (!::player_interaction_exists(player_index, 8, interaction))
            break;
        long object_index = interaction->object_index;
        long unit_index = player->unit_index;
        if (!unit_can_pickup_equipment(unit_index, object_index))
            break;
        bool predicted = result;
        if (game_globals->options.game_simulation == 4)
            predicted = true;
        if (predicted) {
            if (simulation_query_local_predictive_control(unit_index, 1)) {
                long references[2];
                references[0] = unit_index;
                references[1] = object_index;
                unsigned short payload; memset(&payload, 0, sizeof(payload));
                simulation_event_generate(_simulation_event_type_unit_equipment_pickup, 2, references, -1, 2, &payload,
                    g_network_configuration.event_relevance[_simulation_event_type_unit_equipment_pickup].delay_msec);
            }
            result = true;
        } else {
            memset(&pickup_request, 0, sizeof(pickup_request));
            pickup_request.unit_index = object_index;
            pickup_request.type = 58;
            result = action_submit(player->unit_index, &pickup_request);
        }
        break;
    }
    }
    return result;
}


// @address 0x1f7370 players_set_input_controller
void players_set_input_controller(long player_index, long controller_index)
{
    unsigned long controller_offset = ((unsigned short)player_index + offsetof(s_user_globals, player_input_controller_mapping) / sizeof(long)) * sizeof(long);
    s_user_globals *mapping = player_mapping_globals;
    long previous_controller = players_controller_array_element(
        mapping->player_input_controller_mapping, controller_offset - offsetof(s_user_globals, player_input_controller_mapping));
    if (previous_controller != controller_index) {
        if (controller_index == NONE) {
            mapping->user_controller_mapping[previous_controller] = NONE;
            players_controller_array_element(mapping->player_input_controller_mapping,
                controller_offset - offsetof(s_user_globals, player_input_controller_mapping)) = NONE;
            mapping->player_controller_count--;
            return;
        }
        players_set_input_controller(player_index, NONE);
        mapping = player_mapping_globals;
        players_controller_array_element(mapping->player_input_controller_mapping,
            controller_offset - offsetof(s_user_globals, player_input_controller_mapping)) = controller_index;
        mapping->user_controller_mapping[controller_index] = player_index;
        mapping->player_controller_count++;
    }
}


// @address 0x1dd280 player_compute_aim_score
static __forceinline real_point3d players_aim_camera_value(long unit_index)
{
  real_point3d position;
  unit_get_camera_position(unit_index, &position);
  return position;
}
float player_compute_aim_score(long p0, long p1)
{
  unit_datum *object = ((unit_datum *)object_get(p0));
  real_point3d head = players_aim_camera_value(p0);
  real_point3d position = ((unit_datum *)object_get(p1))->object.bounding_center;
  real_vector3d direction;
  direction.i = position.x - head.x;
  direction.j = position.y - head.y;
  direction.k = position.z - head.z;
  float distance = normalize3d(&direction);
  float result = distance * distance;
  float dot = dot_product3d(&object->unit.aiming_vector, &direction);
  if (distance < real_constant_pool_one()) {
    if (dot > 0.93969262f) {
      result = (real_constant_pool_one() - (dot - 0.93969262f) * 16.5817184f) * result;
    }
  }
  return result;
}

// @address 0x1dd388 player_consider_interaction
extern "C" bool player_consider_interaction(long player_index, const s_player_interaction *interaction, s_player_action_context *context)
{
    player_datum *player = player_get(player_index);
    bool result;
    if (context->interaction.type < interaction->type) {
        result = true;
    } else if (context->interaction.type == interaction->type) {
        float current = player_compute_aim_score(player->unit_index, context->interaction.object_index);
        result = player_compute_aim_score(player->unit_index, interaction->object_index) < current;
    } else {
        result = false;
    }
    if (result) {
        context->interaction = *interaction;
        if (simulation_query_local_predictive_control(player->unit_index, 1))
            simulation_request_projectile_supercombine(interaction->object_index);
    }
    return result;
}

// @address 0x1dd460 player_consider_vehicle_seat
extern "C" bool player_consider_vehicle_seat(long player_index, long object_index, s_player_action_context *context)
{
    player_datum *player = player_get(player_index);
    bool result = false;
    if (player->unit_index != object_index
        && (unsigned char)((object_get(object_index)->damage_flags >> 2) & 1) == 0
        && (object_get_type(object_index) != 1 || !vehicle_is_flipped(object_index))
        && !TEST_BIT(player->flags, 10))
    {
        short seat_type = 0;
        long vehicle_index = NONE;
        short seat_index = NONE;
        {
            bool exit = false;
            float distance = 3.4028234663852886e38f;
            unit_consider_nearby_seats(player->unit_index, object_index, &vehicle_index, &seat_index, &seat_type, &distance, &exit);
        }
        if (seat_type != 0)
        {
            s_player_interaction interaction;
            memset(&interaction, 0, sizeof(interaction));
            switch (seat_type)
            {
            case 3: interaction.type = 5; break;
            case 1: interaction.type = 4; break;
            case 4: interaction.type = 8; break;
            case 2: interaction.type = 6; break;
            }
            interaction.object_index = vehicle_index;
            interaction.seat_index = seat_index;
            result = player_consider_interaction(player_index, &interaction, context);
        }
    }
    return result;
}

static __forceinline float players_flipped_vehicle_distance_squared(const real_point3d *a, const real_point3d *b)
{
    float dy = b->y - a->y;
    float dz = b->z - a->z;
    float dx = b->x - a->x;
    float dz_squared = dz * dz;
    float dist_squared = dz_squared + dy * dy;
    dist_squared += dx * dx;
    return dist_squared;
}
// @address 0x1dd5e0 player_consider_flipped_vehicle
extern "C" bool player_consider_flipped_vehicle(long player_index, long object_index, s_player_action_context *context)
{
    player_datum *player = player_get(player_index);
    vehicle_datum *target = (vehicle_datum *)object_get(object_index);
    bool result = false;
    if ((unsigned char)((target->object.damage_flags >> 2) & 1) == 0) {
        if (vehicle_is_flipped(object_index)) {
            long unit_index = player->unit_index;
            vehicle_datum *unit = (vehicle_datum *)object_get(unit_index);
            if ((unsigned char)((target->vehicle.flags >> 5) & 1) == 0 && target->unit.driver_object_index == NONE) {
                float radius = unit->object.bounding_radius + target->object.bounding_radius;
                if (players_flipped_vehicle_distance_squared(&unit->object.bounding_center, &target->object.bounding_center) < radius * radius
                    && unit_can_access_object(unit_index, object_index)) {
                    s_player_interaction interaction = {};
                    interaction.object_index = object_index;
                    interaction.type = 10;
                    result = player_consider_interaction(player_index, &interaction, context);
                }
            }
        }
        if (TEST_BIT(1 << object_get_type(player->unit_index), 0)) {
            c_multi_unit_passenger_iterator iterator;
            iterator.begin(object_index);
            if (iterator.next()) {
                do {
                    long passenger_index = iterator.m_passenger_index;
                    if (TEST_BIT(1 << object_get_type(passenger_index), 0)
                        && game_team_is_enemy((e_game_team)player->configuration.host.team_index, (e_game_team)((unit_datum
                            *)object_get(passenger_index))->unit.team)) {
                        context->melee_target_unit = biped_calculate_better_melee_target(player->unit_index, passenger_index, context->melee_target_unit);
                    }
                } while (iterator.next());
            }
        }
    }
    return result;
}

// @address 0x1dd8d8 player_consider_device
extern "C" bool player_consider_device(long player_index, long device_index, s_player_action_context *context)
{
    player_datum *player = player_get(player_index);
    object_datum *device = (object_datum *)object_get(device_index);
    unit_datum *unit = ((unit_datum *)object_get(player->unit_index));
    bool result = false;
    real_point3d camera_position;
    unit_get_camera_position(player->unit_index, &camera_position);
    if (fast_vector_intersects_sphere(&camera_position, &unit->unit.aiming_vector, &device->bounding_center, device->bounding_radius)
        && device_allow_interaction(device_index, &camera_position, &unit->unit.aiming_vector)
        && (device->owner_team_index == NONE || player->configuration.host.team_index == device->owner_team_index))
    {
        s_player_interaction interaction;
        memset(&interaction, 0, sizeof(interaction));
        interaction.type = 3;
        interaction.object_index = device_index;
        result = player_consider_interaction(player_index, &interaction, context);
    }
    return result;
}

// @address 0xa0738 ?unit_get_camera_info@@YAXJPAUs_unit_camera_info@@@Z


static __forceinline unit_datum *players_camera_object_get(long index,long mask){return (unit_datum *)object_get(index);}
static __forceinline long players_camera_current_weapon(long index)
{
    unit_datum *u = players_camera_object_get(index, 8195);
    long result = -1;
    if (u->unit.current_weapon_index != -1)
        result = u->unit.weapon_object_indices[u->unit.current_weapon_index];
    return result;
}
void unit_get_camera_info(long index,s_unit_camera_info *out){
 out->unit_index=index;out->seat_index=-1;out->camera=0;out->weapon_index=-1;
 if(index!=-1){
  unit_datum *u=players_camera_object_get(index,8195);
  unit_get_camera_position(index,&out->position);
  if(u->unit.parent_seat_index!=-1){
   unit_datum *parent=players_camera_object_get(u->object.parent_object_index,8195);
   unit_definition *definition=(unit_definition*)tag_get('unit',parent->object.definition_index);
   unit_seat *seat=&definition->seats[u->unit.parent_seat_index];
   out->unit_index=u->object.parent_object_index;out->camera=&seat->camera;out->seat_index=u->unit.parent_seat_index;
   if(((unsigned char)((seat->flags>>4)&1)) && ((unsigned char)((seat->flags>>2)&1)))object_get_center_of_mass(out->unit_index,&out->position);
  }else{unit_definition *definition=(unit_definition*)tag_get('unit',u->object.definition_index);out->camera=&definition->camera;}
  long weapon=players_camera_current_weapon(out->unit_index);
  if (weapon != -1)
  {
      unit_datum *w = players_camera_object_get(weapon, 4);
      weapon_definition *definition = (weapon_definition *)tag_get('weap', w->object.definition_index);
      if ((unsigned char)((definition->flags >> 13) & 1))
          out->weapon_index = weapon;
  }
 }
}


static __forceinline float players_normalize_facing(real_vector3d *vector, const real_matrix4x3 *identity)
{
    return normalize3d_with_default(vector, &identity->forward);
}


// @address 0x1da9a0 player_update_unit_after_teleport
extern "C" void player_update_unit_after_teleport(long player_index, long source_index, bool teleported)
{
    player_datum *player = player_get(player_index);
    long root_index = NONE;
    long object_index = player->unit_index;
    while (object_index != NONE) {
        root_index = object_index;
        object_index = object_get(object_index)->parent_object_index;
    }
    object_set_velocities(root_index, global_zero_vector3d, global_zero_vector3d);
    c_output_user_iterator iterator(player_index); while(iterator.next()) chud_motion_sensor_invalidate(iterator.get_index());
    if (source_index != NONE) {
        {
            real_vector3d facing = object_get(source_index)->forward;
            player_set_facing(player_index, &facing);
        }
        if (TEST_BIT(1 << object_get_type(source_index), 0) && TEST_BIT(1 << object_get_type(player->unit_index), 0)) {
            real_point3d position;
            object_get_center_of_mass(source_index, &position);
            real_vector3d velocity, angular_velocity;
            if (object_get_early_mover_local_space_velocity(source_index, &position, &velocity, &angular_velocity, false, false))
            {
                object_in_early_mover_join(source_index, player->unit_index);
                object_set_velocities(player->unit_index, &velocity, 0);
            }
        }
    }
    if (teleported) player->flags |= 0x800;
}


static __forceinline void players_control_vectors(
    long player_index, const real_vector3d *aiming, const real_point2d *throttle, real_vector3d *out_aiming, real_vector3d *out_throttle)
{
    real_vector3d aiming_copy = *aiming;
    real_point2d throttle_copy = *throttle;
    memcpy(out_aiming, &aiming_copy, sizeof(aiming_copy));
    out_throttle->i = throttle_copy.x;
    out_throttle->j = throttle_copy.y;
    out_throttle->k = 0.0f;
}
static __forceinline s_object_header &players_control_header_element(s_object_header *headers, unsigned long byte_offset)
{
    return *(s_object_header *)((unsigned char *)headers + byte_offset);
}
// @address 0x1dc288 ?player_submit_control@@YAXJJPAUplayer_action@@@Z
void player_submit_control(long player_index, long unit_index, player_action *action)
{
    real_vector3d aiming;
    player_build_aiming_vector_from_facing(player_index, &action->facing, &aiming);
    real_vector3d facing, throttle;
    players_control_vectors(player_index, &aiming, &action->move, &facing, &throttle);
    unit_control_data control;
    unit_control_clear(&control);
    control.aiming_vector = aiming;
    control.looking_vector = aiming;
    control.facing_vector = aiming;
    control.control_flags = action->flags;
    control.throttle = throttle;
    if (action->extra_short & 1) control.throttle.k = 1.0f;
    if (action->extra_short & 2) control.throttle.k -= 1.0f;
    control.primary_trigger = action->primary;
    control.secondary_trigger = action->secondary;
    control.weapon_set = action->weapons;
    control.grenade_index = action->grenade;
    control.animation_state = 138;
    control.zoom_level = action->zoom;
    control.aiming_speed = 0;
    memcpy(&control.targeting, &action->aim_assist, sizeof(control.targeting));
    unsigned long unit_offset = ((unsigned long)unit_index << 4) & 0xffff0UL;
    control.control_flags |= 0x10000;
    unit_datum *unit = (unit_datum *)players_control_header_element((s_object_header *)object_header_data->data, unit_offset).datum;
    SET_BIT(unit->unit.unit_flags, 28, action->velocity_exceeds_motion_tracker_threshold);
    unit_control(unit_index, &control);
}


static __forceinline bool players_transition_enabled(long index)
{
    bool enabled = false;
    if ((unsigned long)index <= 349)
        enabled = !TEST_BIT(players_globals->disabled[index >> 5], index & 31);
    return enabled;
}
// @address 0x1dc448 player_find_zone_transition
// match: 0x1dc448: pointer __restrict qualifier prevents alias reload
extern "C" bool player_find_zone_transition(long object_index, long *__restrict transition_index, long *__restrict destination_index)
{
    bool result = false;
    if (object_index != NONE)
    {
        long destination = NONE;
        long transition = NONE;
        scenario *definition = global_scenario;
        s_scenario_game_state zone_state;
        zone_state.flags = 1;
        long zone_set_index = global_zone_set_index;
        zone_state.zone_set_index = zone_set_index;
        zone_state.active_structure_bsp_mask = g_active_structure_bsp_mask;
        zone_state.active_designer_zone_mask = g_active_designer_zone_mask;
        zone_state.touched_structure_bsp_mask = g_touched_structure_bsp_mask;
        zone_state.active_cinematic_zone_mask = g_active_cinematic_zone_mask;
        zone_state.touched_cinematic_zone_mask = g_touched_cinematic_zone_mask;
        for (long index = 0; index < definition->zone_set_switch_trigger_volumes.count; index++) {
            if (players_transition_enabled(index)) {
                s_scenario_zone_set_switch_trigger_volume *entry =
                    &((s_scenario_zone_set_switch_trigger_volume *)definition->zone_set_switch_trigger_volumes.address)[index];
                const short &volume = entry->trigger_volume;
                if (volume != NONE) {
                    short destination_zone = entry->commit_zone_set;
                    if (destination_zone != NONE) {
                        if (destination_zone != zone_set_index || !scenario_zone_set_is_active(destination_zone)) {
                            if (scenario_trigger_volume_test_object(volume, object_index)) {
                                destination = index;
                                continue;
                            }
                        }
                    }
                    short next_zone = entry->begin_zone_set;
                    if (next_zone != NONE && players_is_zone_loading_required(&zone_state, next_zone) &&
                        scenario_trigger_volume_test_object(entry->trigger_volume, object_index))
                        transition = index;
                }
            }
        }
        if (destination != NONE || transition != NONE) {
            *transition_index = transition;
            *destination_index = destination;
            result = true;
        }
    }
    return result;
}


static __forceinline void players_set_camo_maximum(long unit_index, float maximum)
{
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    unit->unit.active_camouflage_target = maximum <= 0.0f ? 0.0f : (maximum < 1.0f ? maximum : 1.0f);
    ::simulation_action_object_update(unit_index, 0x8000000);
}

// @address 0x1debe8 ?player_update_invisibility@@YAXJ@Z
static __forceinline bool players_camo_active(long unit_index)
{
    unsigned long flags = ((unit_datum *)object_get(unit_index))->unit.unit_flags;
    return (bool)(_rotl(flags, 29) & 1) || (bool)(_rotl(flags, 28) & 1);
}
static __forceinline unsigned char players_camo_held(long unit_index)
{
    unsigned long flags = ((unit_datum *)object_get(unit_index))->unit.unit_flags;
    return (flags >> 4) & 1;
}
void player_update_invisibility(long player_index)
{
    player_datum *player = player_get(player_index);
    long unit_index = player->unit_index;
    bool active = players_camo_active(unit_index);
    unsigned char held = players_camo_held(unit_index);
    bool enable = false;
    bool disable = false;
    bool hold = false;
    float maximum = 1.0f;
    if (player->invisibility_time > 0 && !((float)(game_time_globals->time - player->invisibility_time) * game_time_globals->tick_length <
        player->invisibility_duration))
        disable = true;
    if (game_globals->options.game_mode == 2 && game_engine_running()) {
        player = player_get(player_index); switch (player->traits.m_appearance_traits.m_active_camo) {
        case 1: disable |= active; break;
        case 2: enable = !active || held; maximum = 0.5f; disable = false; break;
        case 3: enable = !active || held; maximum = 0.67f; disable = false; break;
        case 4: hold = true; enable = !active || held != 1; disable = false; break;
        }
    }
    if (enable) {
        unit_active_camouflage_enable(unit_index, 4.0f, hold);
        players_set_camo_maximum(player->unit_index, maximum);
    } else if (disable) {
        unit_active_camouflage_disable(unit_index, 4.0f);
    }
}


static __forceinline unit_datum *players_pickup_outer_unit(unsigned long index)
{
    return (unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)index].datum;
}
static __forceinline unit_datum *players_pickup_get_unit(const long &index)
{
    return (unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)index].datum;
}
static __forceinline unsigned long players_pickup_weapon_flags(long index)
{
    unit_datum *weapon = players_pickup_get_unit(index);
    return ((weapon_definition *)tag_get(0, weapon->object.definition_index))->flags;
}
static __forceinline bool players_pickup_support_weapon(long index)
{
    return (bool)(_rotl(players_pickup_weapon_flags(index), 32-13) & 1);
}
static __forceinline bool players_pickup_must_be_readied(long index)
{
    return (bool)(_rotl(players_pickup_weapon_flags(index), 32-3) & 1);
}
static __forceinline bool players_pickup_counts(long index)
{
    unit_datum *weapon = players_pickup_get_unit(index);
    weapon_definition *definition=(weapon_definition *)tag_get(0, weapon->object.definition_index);
    return !(bool)(_rotl(definition->flags,28) & 1) && !(bool)(_rotl(definition->flags,19) & 1);
}
static __forceinline long players_pickup_inventory_weapon(long unit_index, short slot)
{
    unit_datum *unit = players_pickup_get_unit(unit_index);
    long weapon = NONE;
    if (slot != NONE) weapon = unit->unit.weapon_object_indices[slot];
    return weapon;
}

// @address 0x1df760 ?players_handle_deleted_object@@YAXJ@Z
void players_handle_deleted_object(long player_index)
{
    if (game_engine_sandbox_is_active()) {
        long object_index = NONE;
        if (game_engine_is_player_in_edit_mode(player_index, &object_index, 0)) {
            if (object_index != NONE) {
                object_datum *object = (object_datum *)object_get(object_index);
                if ((unsigned short)object->map_variant_index != 0xffff) {
                    bool b = !g_havok_memory_allocator_locked && g_registered_thread_index == 1 && restricted_region_is_available(3);
                    bool nb = !b;
                    // match: 0x1df760: volatile pointer cast preserves memory access order
                    game_engine_sandbox_stop_editing_object((short)*(volatile unsigned short *)&object->map_variant_index, 0.0f, nb);
                }
            }
            long x = game_engine_sandbox_find_player_original_unit(player_index);
            if (x != NONE) {
                bool v = false;
                if (game_globals->options.game_simulation == 4)
                    v = true;
                if (!v)
                    object_delete(x);
            }
            game_engine_sandbox_toggle_player_mode(player_index);
        }
    }
}
static __forceinline long players12_engine_index_out_of_range(long game_engine_index)
{
    if (game_engine_index < 1)
        return 1;
    if (game_engine_index > 10)
        return 1;
    return 0;
}
// @address 0x1d9c98 player_handle_death
extern "C" void player_handle_death(long player_index)
{
    unsigned char predicted;
    long zero;
    long perspective;
    zero = 0;
    long offset = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index) * 0x370;
    player_datum *player = (player_datum *)(player_data->data + offset);
    long unit_index = player->unit_index;
    player->ticks_without_unit = zero;
    player->dead_unit_index = unit_index;
    game_engine_spawn_influencer_record_player_death(player_index);
    players_handle_deleted_object(player_index);
    player_set_unit_index(player_index, NONE);
    if (game_globals->options.game_mode == 2)
    {
        player_datum *same_player = (player_datum *)(player_data->data + offset);
        same_player->powerup_times[0] = zero;
        same_player->powerup_times[1] = zero;
        same_player->powerup_times[2] = zero;
        long game_engine_index = game_engine_globals->game_engine_index;
        if (!players12_engine_index_out_of_range(game_engine_index) && game_engines[game_engine_index] != 0)
        {
            perspective = NONE;
            for (;;) {
                if (player_index != NONE)
                    perspective = ((long)::director_get_perspective(player_index, perspective));
                if (perspective == NONE)
                    break;
                c_game_engine *engine = (c_game_engine *)current_game_engine();
                engine->player_hump(perspective);
            }
        }
    }
    game_globals_storage *globals = game_globals;
    memset(player->achievement_damage_info, 0, sizeof(player->achievement_damage_info));
    predicted = zero;
    player->last_vehicle_exit_time = zero;
    player->last_vehicle_exit_index = NONE;
    if (globals->options.game_simulation == 4)
        predicted = 1;
    if ((unsigned char)predicted == 0)
        return;
    const game_options *options = &globals->options;
    const c_game_variant *variant = 0;
    if (options->game_mode == 2)
        variant = &options->game_variant;
    player->respawn_seconds = variant->respawn_time_seconds;
}
// @address 0x111498 players_handle_object_deletion
extern "C" void players_handle_object_deletion(long player_index)
{
    object_datum *object = object_get(player_index);
    if (((1 << object->object_type) & 0x1003) != 0) {
        c_player_data_iterator<player_datum> player_iterator;
        player_iterator.begin(player_data);
        while (player_iterator.next()) {
            player_datum *player = player_iterator.get_datum();
            if (player->unit_index == player_index)
                player_handle_death(player_iterator.iterator.index);
            if (player->last_shot_at_object_index == player_index)
                player->last_shot_at_object_index = NONE;
            if (player->dead_unit_index == player_index)
                player->dead_unit_index = NONE;
            if (player->next_unit_index == player_index)
                player->next_unit_index = NONE;
        }
    }
    // Clear targeting information for the deleted object.
    s_player_control_globals *control_globals = player_control_globals;
    for (unsigned int user_index = 0; user_index < 4; user_index++) {
        if (control_globals->users[user_index].action.aim_assist.target_object_index == player_index)
            aim_assist_targeting_clear(&control_globals->users[user_index].action.aim_assist);
    }
    for (unsigned int state_index = 0; state_index < 4; state_index++) {
        if (control_globals->states[state_index].action.aim_assist.target_object_index == player_index)
            aim_assist_targeting_clear(&control_globals->states[state_index].action.aim_assist);
    }
}

static __forceinline int players14_invalid_controller(long controller_index)
{
    if (controller_index < 0) return 1;
    if (controller_index >= 4) return 1;
    return false;
}
static __forceinline bool players14_iterator_next(player_iterator *iterator)
{
    if (iterator->player_index != NONE)
        iterator->controller_index = ((long)::director_get_perspective(iterator->player_index, iterator->controller_index));
    return iterator->controller_index != NONE;
}
// @address 0x1dead0 player_pickup_equipment
extern "C" void player_pickup_equipment(long player_index, long equipment_index)
{
    player_datum *player = player_get(player_index);
    object_datum *equipment = object_get(equipment_index);
    action_request request;
    memset(&request, 0, sizeof(request));
    request.unit_index = equipment_index;
    request.type = 58;
    action_submit(player->unit_index, &request);
    projectile_play_pickup_sound(player_index, equipment->definition_index);
    player_iterator iterator;
    iterator.player_index = player_index;
    iterator.controller_index = NONE;
    while (players14_iterator_next(&iterator)) {
        if (player_mapping_globals->local_player_count < 2 && !players14_invalid_controller(iterator.controller_index)) {
            s_item_definition *definition =
                (s_item_definition *)tag_get(NONE, equipment->definition_index);
            chud_messaging_post(iterator.controller_index, definition->picked_up_message, _chud_message_context_self);
        }
    }
    simulation_action_pickup_powerup(player->unit_index, equipment->definition_index);
}

static __forceinline s_equipment_definition *players_powerup_definition_get(const object_datum *object)
{
    return (s_equipment_definition *)tag_get(NONE, object->definition_index);
}

// @address 0x1de918 player_pickup_powerup
extern "C" void player_pickup_powerup(long player_index, long object_index)
{
    player_datum *player = player_get(player_index);
    s_object_header *header = object_header_get(object_index);
    object_datum *object = (object_datum *)header->datum;
    long *type = (long *)players_powerup_definition_get(object)->equipment_types[1].address;
    if (TEST_BIT(header->flags, 4))
        return;
    player->powerup_and_respawn_times[*type] = game_time_globals->time;
    s_event datamine;
    data_mining_event_initialize(&datamine, 0, "powerup picked up", 1, "game");
    player_iterator iterator;
    iterator.player_index = player_index;
    iterator.controller_index = NONE;
    while (players14_iterator_next(&iterator)) {
        if (player_mapping_globals->local_player_count < 2 && !players14_invalid_controller(iterator.controller_index))
            chud_messaging_post(iterator.controller_index, players_powerup_definition_get(object)->item.picked_up_message, _chud_message_context_self);
    }
    projectile_play_pickup_sound(player_index, object->definition_index);
    if (*type == 0 && !game_simulation::game_is_predicted()) {
        long unit = player->unit_index;
        if (unit != NONE)
            object_get(unit)->shield_stun_ticks = 0;
    }
    ::object_destroy_internal(object_index, NONE, NONE, 0, 0);
    simulation_action_pickup_powerup(player->unit_index, object->definition_index);
}


// @address 0x1f7290 players_set_input_user
extern "C" void players_set_input_user(long player_index, long input_user_index)
{
    // match: 0x1f7290: value range assumption guides branch lowering
    long index = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index); __assume(index >= 0);
    player_datum *player = player_get(player_index);
    unsigned long user_offset = (index + offsetof(s_user_globals, player_input_user_mapping) / sizeof(long)) * sizeof(long);
    s_user_globals *mapping = player_mapping_globals;
    long previous_input_user_index = players_controller_array_element(
        mapping->player_input_user_mapping, user_offset - offsetof(s_user_globals, player_input_user_mapping));
    if (previous_input_user_index != input_user_index) {
        if (input_user_index == NONE) {
            if (mapping->player_controller_mapping[previous_input_user_index] != NONE) {
                player_control_set_user_unit(previous_input_user_index, NONE);
                mapping = player_mapping_globals;
                mapping->player_controller_mapping[previous_input_user_index] = NONE;
            }
            mapping->player_user_mapping[previous_input_user_index] = NONE;
            players_controller_array_element(mapping->player_input_user_mapping, user_offset - offsetof(s_user_globals, player_input_user_mapping)) = NONE;
            mapping->player_user_count--;
            return;
        }
        players_set_input_user(player_index, NONE);
        mapping = player_mapping_globals;
        players_controller_array_element(
            mapping->player_input_user_mapping, user_offset - offsetof(s_user_globals, player_input_user_mapping)) = input_user_index;
        mapping->player_user_mapping[input_user_index] = player_index;
        mapping->player_user_count++;
        players_set_controller_mapping(input_user_index, player->unit_index);
    }
}
static __forceinline s_players_globals *players_global_data_get() { return players_globals; }
static __forceinline void players_globals_reset_bsp_switch(s_players_globals *globals)
{
    real_vector3d *forward = &globals->forward;
    globals->position.x = 3.4028234e+38f;
    globals->position.y = 3.4028234e+38f;
    globals->position.z = 3.4028234e+38f;
    *forward = *global_forward3d;
    globals->old_zone = -1;
    globals->new_zone = -1;
    globals->player_index = -1;
    globals->unit_index = -1;
}
// @address 0x1d7868 ?player_positions_initialize_for_new_structure_bsp@@YAXK@Z
void player_positions_initialize_for_new_structure_bsp(unsigned long)
{
    long zone_index = -1;
    if (players_global_data_get()->new_zone != -1)
        zone_index = players_global_data_get()->new_zone;
    else if (players_global_data_get()->old_zone != -1)
        zone_index = players_global_data_get()->old_zone;
    players_global_data_get()->pending = false;
    if (zone_index != -1) {
        player_datum *teleport_player = player_get(players_global_data_get()->player_index);
        if (players_global_data_get()->count > 1) {
            c_players_iterator iterator;
            iterator.begin();
            while (iterator.next()) {
                long player_index = iterator.get_index();
                player_datum *player = player_get(player_index);
                if (player->unit_index != -1 && teleport_player->unit_index != -1 && player_index != players_global_data_get()->player_index) {
                    long unit = player->unit_index;
                    long teleport_unit = teleport_player->unit_index;
                    bool teleport = true;
                    real_point3d center;
                    s_location player_location, teleport_location;
                    long transition_index, destination_index;
                    object_get_center_of_mass(unit, &center);
                    scenario_location_from_point(&player_location, &center);
                    object_get_location(teleport_unit, &teleport_location);
                    if ((unsigned char)player_location.cluster_reference.bsp_index != 255 &&
                        (unsigned char)teleport_location.cluster_reference.bsp_index != 255)
                    {
                        teleport = false;
                        if (player_find_zone_transition(unit, &transition_index, &destination_index) &&
                            !players_is_zone_transition_valid(transition_index, destination_index, players_global_data_get()->old_zone,
                                players_global_data_get()->new_zone))
                            teleport = true;
                    }
                    if (teleport) {
                        scenario *map = global_scenario;
                        bool special = false;
                        if (zone_index >= 0 && zone_index < map->zone_set_switch_trigger_volumes.count)
                            special = (((s_scenario_zone_set_switch_trigger_volume *)map->zone_set_switch_trigger_volumes.address)[zone_index].flags & 1);
                        player_teleport_for_zone_switch(player_index, teleport_player->unit_index, &players_global_data_get()->position,
                            &players_global_data_get()->forward, special);
                    }
                }
            }
        }
        players_globals_reset_bsp_switch(players_global_data_get());
    }
    c_players_iterator iterator;
    iterator.begin();
    while (iterator.next())
        cluster_reference_invalidate(&iterator.get_datum()->cluster_reference);
}

// @address 0x5bec98 ?players_initialize_for_new_structure_bsp@@YAXK@Z
void players_initialize_for_new_structure_bsp(unsigned long)
{
}

// @address 0x5bec98 ?players_dispose_from_old_structure_bsp@@YAXK@Z
void players_dispose_from_old_structure_bsp(unsigned long)
{
}

// @address 0x5bec98 ?player_positions_initialize@@YAXXZ
void player_positions_initialize(void)
{
}

// @address 0x5bec98 ?player_positions_dispose@@YAXXZ
void player_positions_dispose(void)
{
}

// @address 0x5bec98 ?player_positions_initialize_for_new_map@@YAXXZ
void player_positions_initialize_for_new_map(void)
{
}

// @address 0x5bec98 ?player_positions_dispose_from_old_map@@YAXXZ
void player_positions_dispose_from_old_map(void)
{
}

// @address 0x5bec98 ?player_positions_dispose_from_old_structure_bsp@@YAXK@Z
void player_positions_dispose_from_old_structure_bsp(unsigned long)
{
}

// @address 0x380c30 player_update_equipment_use
extern "C" void player_update_equipment_use(long object_index, bool flag)
{
    bool create_equipment = true;
    unit_datum *unit = ((unit_datum *)object_get(object_index));
    s_player_equipment_action_state *action = (s_player_equipment_action_state *)((char *)unit + unit->unit.action_offset);
    if (action->interruptible == 0) {
        long equipment = unit->unit.equipment_object_index;
        if (equipment != -1) {
            equipment_datum *child = (equipment_datum *)object_get(equipment);
            s_equipment_definition *tag = (s_equipment_definition *)tag_get(NONE, child->item.object.definition_index);
            if (tag->max_charges == 0)
                create_equipment = false;
            else
                child->charges_used++;
        } else if (flag) {
            long object_indices[32];
            long object_count = objects_in_sphere(3, 8, &unit->object.location, &unit->object.bounding_center,
                unit->object.bounding_radius * 2.0f, object_indices, 32);
            long found_object_index = -1;
            for (long i = 0; i < object_count; i++) {
                equipment_datum *candidate =
                    (equipment_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_indices[i])].datum;
                if (candidate->item.object.definition_index == action->definition_index && candidate->item.last_unit_object_index == object_index) {
                    found_object_index = object_indices[i];
                    break;
                }
            }
            if (found_object_index != -1) {
                object_delete(found_object_index);
                create_equipment = true;
            } else
                create_equipment = false;
        } else
            create_equipment = false;
    }
    if (create_equipment && action->definition_index != -1)
        unit_throw_grenade_create(object_index, action->definition_index, 0);
}

static __forceinline bool game_initializing_or_map_active()
{
    game_globals_storage *globals = game_globals;
    return globals && (globals->initializing || globals->map_active);
}
typedef char players_game_playback_offset[(offsetof(game_globals_storage, options) + offsetof(game_options, game_playback) == 0x140) ? 1 : -1];
static __forceinline bool players_game_has_active_structure_bsp()
{
    return game_initializing_or_map_active() && (unsigned short)game_globals->options.game_playback != 0;
}
// @address 0x1d8e38 ?players_set_local_machine@@YAXPBUs_machine_identifier@@@Z
void players_set_local_machine(const s_machine_identifier *machine_identifier)
{
    bool machine_valid = machine_identifier != 0;
    long local_machine_index = NONE;
    c_data_iterator<player_datum> iterator;
    if (machine_identifier != 0) {
        s_players_globals *scan_globals = players_globals;
        for (long i = 0; i < 16; i++) {
            if ((1 << i) & scan_globals->machine_valid_mask) {
                if (memcmp(machine_identifier, &scan_globals->machines[i], sizeof(s_machine_identifier)) == 0)
                    local_machine_index = i;
            }
        }
    }
    s_players_globals *globals = players_globals;
    if (machine_valid != globals->network_valid
        || (machine_valid && memcmp(machine_identifier, &globals->machine, sizeof(s_machine_identifier)) != 0)
        || local_machine_index != globals->local_machine_index) {
        if (!players_game_has_active_structure_bsp()) {
            iterator.begin(player_data);
            while (iterator.next()) {
                long player_index = iterator.get_index();
                unsigned long user_offset = (DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)
                    + offsetof(s_user_globals, player_input_user_mapping) / sizeof(long)) * sizeof(long);
                s_user_globals *mapping = player_mapping_globals;
                long input_user = players_controller_array_element(
                    mapping->player_input_user_mapping, user_offset - offsetof(s_user_globals, player_input_user_mapping));
                if (input_user != NONE) {
                    if (mapping->player_controller_mapping[input_user] != NONE) {
                        player_control_set_user_unit(input_user, NONE);
                        mapping = player_mapping_globals;
                        mapping->player_controller_mapping[input_user] = NONE;
                    }
                    mapping->player_user_mapping[input_user] = NONE;
                    players_controller_array_element(
                        mapping->player_input_user_mapping, user_offset - offsetof(s_user_globals, player_input_user_mapping)) = NONE;
                    mapping->player_user_count--;
                }
                long controller_index = mapping->player_input_controller_mapping[datum_index_to_absolute_index(player_index)];
                if (controller_index != NONE) {
                    mapping->user_controller_mapping[controller_index] = NONE;
                    mapping->player_input_controller_mapping[datum_index_to_absolute_index(player_index)] = NONE;
                    mapping->player_controller_count--;
                }
                players_remove_local_player(player_index);
            }
        }
        globals = players_globals;
        globals->network_valid = machine_valid;
        globals->local_machine_index = local_machine_index;
        if (machine_valid) {
            memcpy(&globals->machine, machine_identifier, sizeof(s_machine_identifier));
        } else {
            csmemset(&globals->machine, 0, sizeof(globals->machine));
        }
        if (!players_game_has_active_structure_bsp() && globals->local_machine_index != NONE) {
            iterator.begin(player_data);
            while (iterator.next()) {
                player_datum *player = iterator.get_datum();
                if (player->machine_index == players_globals->local_machine_index) {
                    long player_index = iterator.get_index();
                    players_set_input_user(player_index, player->user_index);
                    players_set_input_controller(player_index, player->controller_index);
                    players_set_local_player(player->user_index, player_index);
                }
            }
        }
    }
}


static __forceinline e_controller_index players_remap_next_controller(e_controller_index controller_index)
{
    e_controller_index result = _controller_none;
    if ((unsigned long)controller_index <= 2)
        result = (e_controller_index)(controller_index + 1);
    return result;
}

static __forceinline long players_remap_get_input_user(long player_index)
{
    unsigned long absolute_player_index = (unsigned short)player_index;
    s_user_globals *mapping = player_mapping_globals;
    return mapping->player_input_user_mapping[absolute_player_index];
}

static __forceinline long players_remap_get_input_controller(long player_index)
{
    s_user_globals *mapping = player_mapping_globals;
    return mapping->player_input_controller_mapping[(unsigned short)player_index];
}


static __forceinline void *players_remap_get_absolute(s_data_array *data, long absolute_index)
{
    return data->data + absolute_index * data->size;
}

static __forceinline long players_remap_absolute_index_to_index(s_data_array *data, long absolute_index)
{
    long result = NONE;
    if (absolute_index != NONE)
    {
        s_datum_header *datum = (s_datum_header *)players_remap_get_absolute(data, absolute_index);
        result = (datum->identifier << 16) | absolute_index;
    }
    return result;
}

static __forceinline player_datum *players_remap_try_player(long absolute_index)
{
    player_datum *player = 0;
    const s_data_array *data = player_data;
    if (absolute_index != NONE && absolute_index >= 0 && absolute_index < data->first_unallocated)
    {
        player = (player_datum *)(data->data + absolute_index * data->size);
        if (player->identifier == 0)
            player = 0;
    }
    return player;
}

static __forceinline void players_remap_detach_player(long player_index)
{
    long absolute_player_index = (unsigned short)player_index;
    s_user_globals *mapping = player_mapping_globals;
    long input_user = mapping->player_input_user_mapping[absolute_player_index];
    if (input_user != NONE)
    {
        if (mapping->player_controller_mapping[input_user] != NONE)
        {
            player_control_set_user_unit(input_user, NONE);
            mapping = player_mapping_globals;
            mapping->player_controller_mapping[input_user] = NONE;
        }
        mapping->player_user_mapping[input_user] = NONE;
        mapping->player_input_user_mapping[absolute_player_index] = NONE;
        mapping->player_user_count--;
    }
    long controller_index = mapping->player_input_controller_mapping[absolute_player_index];
    if (controller_index != NONE)
    {
        mapping->user_controller_mapping[controller_index] = NONE;
        mapping->player_input_controller_mapping[absolute_player_index] = NONE;
        mapping->player_controller_count--;
    }
}


static __forceinline void players_remap_collect_fallback(long &machine_index, long *player_controllers, long *player_indices,
    s_player_identifier *player_identifiers, long &local_player_count, unsigned long &player_mask)
{
    c_player_in_game_iterator iterator;
    iterator.begin();
    if (!iterator.next())
        return;
    machine_index = iterator.get_datum()->machine_index;
    if (machine_index == NONE)
        return;
            for (long player_absolute_index = 0; player_absolute_index < 16; player_absolute_index++)
            {
                player_datum *player = players_remap_try_player(player_absolute_index);
                if (player && player->machine_index == machine_index)
                {
                    player_indices[player_absolute_index] = players_remap_absolute_index_to_index(player_data, player_absolute_index);
                    player_controllers[player_absolute_index] = player->controller_index;
                    player_identifiers[player_absolute_index] = player->player_identifier;
                    local_player_count++;
                    SET_BIT(player_mask, player_absolute_index, true);
                }
            }
}

static __forceinline void players_remap_collect_local_options(const game_player_options *local_player, long *user_to_controller,
    s_player_identifier *user_identifiers, unsigned long &local_user_mask, unsigned long &local_controller_mask)
{
    if (!local_player->valid)
        return;
    if (local_player->left_game)
        return;
    long user_index = local_player->user_index;
    long controller_index = local_player->controller_index;
    user_to_controller[user_index] = controller_index;
    user_identifiers[user_index] = local_player->player_identifier;
    SET_BIT(local_user_mask, user_index, true);
    SET_BIT(local_controller_mask, controller_index, true);
}

// @address 0x1dfaf0 players_remap_local_players
extern "C" void players_remap_local_players(s_machine_identifier *local_machine_identifier, game_player_options *local_players)
{
    long local_machine_index = NONE;
    unsigned long local_user_mask = 0;
    unsigned long local_controller_mask = 0;
    long matched_player_count = 0;
    unsigned long player_used_mask = 0;
    long local_player_count = 0;
    unsigned long player_mask = 0;
    unsigned long matched_user_mask = 0;
    long user_to_player[4];
    long user_to_controller[4];
    s_player_identifier user_identifiers[4];
    long player_controllers[16];
    long player_indices[16];
    s_player_identifier player_identifiers[16];
    memset(user_identifiers, 0, sizeof(user_identifiers));
    for (long user_index = 0; user_index < 4; user_index++)
    {
        user_to_player[user_index] = NONE;
        user_to_controller[user_index] = NONE;
    }
    for (long player_absolute_index = 0; player_absolute_index < 16; player_absolute_index++)
    {
        player_datum *player = players_remap_try_player(player_absolute_index);
        if (player)
        {
            long player_index = players_remap_absolute_index_to_index(player_data, player_absolute_index);
            if (players_remap_get_input_user(player_index) != NONE)
            {
                player_indices[player_absolute_index] = player_index;
                player_controllers[player_absolute_index] = players_remap_get_input_controller(player_index);
                player_identifiers[player_absolute_index] = player->player_identifier;
                local_player_count++;
                SET_BIT(player_mask, player_absolute_index, true);
            }
            {
                long absolute_player_index = (unsigned short)player_index;
                s_user_globals *mapping = player_mapping_globals;
                long input_user = mapping->player_input_user_mapping[absolute_player_index];
                if (input_user != NONE)
                {
                    if (mapping->player_controller_mapping[input_user] != NONE)
                    {
                        player_control_set_user_unit(input_user, NONE);
                        mapping = player_mapping_globals;
                        mapping->player_controller_mapping[input_user] = NONE;
                    }
                    mapping->player_user_mapping[input_user] = NONE;
                    mapping->player_input_user_mapping[absolute_player_index] = NONE;
                    mapping->player_user_count--;
                }
                long controller_index = mapping->player_input_controller_mapping[absolute_player_index];
                if (controller_index != NONE)
                {
                    mapping->user_controller_mapping[controller_index] = NONE;
                    mapping->player_input_controller_mapping[absolute_player_index] = NONE;
                    mapping->player_controller_count--;
                }
            }
            players_remove_local_player(player_index);
        }
        else
        {
            player_indices[player_absolute_index] = NONE;
            player_controllers[player_absolute_index] = NONE;
            memset(&player_identifiers[player_absolute_index], 0, sizeof(s_player_identifier));
        }
    }
    if (local_player_count == 0)
    {
        players_remap_collect_fallback(local_machine_index, player_controllers, player_indices, player_identifiers, local_player_count, player_mask);
        // match: 0x1dfaf0: the zero offset keeps the nonzero player-count branch targeting the options loop at 0x1dfe3c.
        local_players = local_players + (long)_rotl(0, 0);
    }
    for (long local_player_index = 0; local_player_index < 16; local_player_index += 4)
    {
        players_remap_collect_local_options(&local_players[local_player_index], user_to_controller, user_identifiers, local_user_mask, local_controller_mask);
        const game_player_options *local_player = &local_players[local_player_index + 1];
        if (local_player->valid)
        {
            s_remap_header_pointer header;
            header.value = (const s_remap_local_header *)&local_player->user_index;
            // match: 0x1dfaf0: a pointer value folds the controller load at 0x1dfed4 into 2(r11) and sinks the identity address past left_game.
            const s_remap_local_identity *const &identity = &header.value->identity;
            if (!local_player->left_game)
            {
                header.user_index = local_player->user_index;
                const s_remap_local_identity *const &copy_identity = &header.value->identity;
                long user_index = header.user_index;
                long controller_index = identity->controller_index;
                user_to_controller[user_index] = controller_index;
                user_identifiers[user_index] = copy_identity->player_identifier;
                SET_BIT(local_user_mask, user_index, true);
                SET_BIT(local_controller_mask, controller_index, true);
            }
        }
        players_remap_collect_local_options(
            &local_players[local_player_index + 2], user_to_controller, user_identifiers, local_user_mask, local_controller_mask);
        players_remap_collect_local_options(
            &local_players[local_player_index + 3], user_to_controller, user_identifiers, local_user_mask, local_controller_mask);
    }
    for (long match_pass = 0; match_pass < 3; match_pass++)
    {
        for (long user_index = 0; user_index < 4; user_index++)
        {
            if (TEST_BIT(local_user_mask, user_index) && !TEST_BIT(matched_user_mask, user_index))
            {
                for (long player_absolute_index = 0; player_absolute_index < 16; player_absolute_index++)
                {
                    if (TEST_BIT(player_mask, player_absolute_index) && !TEST_BIT(player_used_mask, player_absolute_index))
                    {
                        bool matches;
                        switch (match_pass)
                        {
                        case 0:
                            matches = memcmp(&player_identifiers[player_absolute_index], &user_identifiers[user_index],
                                          sizeof(s_player_identifier)) == 0;
                            break;
                        case 1:
                            matches = player_controllers[player_absolute_index] == user_to_controller[user_index];
                            break;
                        default:
                            matches = true;
                            break;
                        }
                        if (matches)
                        {
                            user_to_player[user_index] = player_absolute_index;
                            user_identifiers[user_index] = player_identifiers[player_absolute_index];
                            matched_player_count++;
                            SET_BIT(matched_user_mask, user_index, true);
                            SET_BIT(player_used_mask, player_absolute_index, true);
                            break;
                        }
                    }
                }
            }
        }
    }
    if (matched_player_count < local_player_count)
    {
        for (long player_absolute_index = 0; player_absolute_index < 16; player_absolute_index++)
        {
            if (TEST_BIT(player_mask, player_absolute_index) && !TEST_BIT(player_used_mask, player_absolute_index))
            {
                for (long user_index = 0; user_index < 4; user_index++)
                {
                    if (!TEST_BIT(local_user_mask, user_index))
                    {
                    e_controller_index controller_index = _controller_none;
                    for (long controller_pass = 0; controller_pass < 3 && controller_index == _controller_none; controller_pass++)
                    {
                        for (e_controller_index candidate = _controller0; candidate != _controller_none; candidate = players_remap_next_controller(candidate))
                        {
                            if (!TEST_BIT(local_controller_mask, candidate)
                                && (controller_pass >= 1 || candidate == player_controllers[player_absolute_index])
                                && (controller_pass >= 2 || input_globals.gamepad_connected[(short)candidate]))
                            {
                                controller_index = candidate;
                                break;
                            }
                        }
                    }
                    user_to_controller[user_index] = controller_index;
                    user_identifiers[user_index] = player_identifiers[player_absolute_index];
                    user_to_player[user_index] = player_absolute_index;
                    SET_BIT(player_used_mask, player_absolute_index, true);
                    SET_BIT(local_user_mask, user_index, true);
                    SET_BIT(matched_user_mask, user_index, true);
                    SET_BIT(local_controller_mask, controller_index, true);
                    break;
                    }
                }
            }
        }
    }
    if (local_machine_index != NONE)
    {
        s_players_globals *globals = players_globals;
        globals->local_machine_index = local_machine_index;
        globals->network_valid = true;
    }
    s_players_globals *globals = players_globals;
    if (globals->network_valid)
    {
        globals->network_valid = true;
        globals->machine = *local_machine_identifier;
        if (globals->local_machine_index != NONE)
        {
            globals->machines[globals->local_machine_index] = *local_machine_identifier;
            for (long player_absolute_index = 0; player_absolute_index < 16; player_absolute_index++)
            {
                player_datum *player = players_remap_try_player(player_absolute_index);
                if (player && player->machine_index == globals->local_machine_index)
                    player->machine = globals->machine;
            }
        }
    }
    for (long user_index = 0; user_index < 4; user_index++)
    {
        if (TEST_BIT(matched_user_mask, user_index))
        {
            long controller_index = user_to_controller[user_index];
            // match: 0x1dfaf0: const reference binding preserves temporary lifetime
            const unsigned __int64 &wide_player_index = (unsigned long)user_to_player[user_index];
            // match: 0x1dfaf0: const reference binding preserves temporary lifetime
            const unsigned long &absolute_player_index = (unsigned long)wide_player_index;
            long player_index = player_indices[absolute_player_index];
            player_datum *player = player_get(player_index);
            player->user_index = (short)user_index;
            player->controller_index = controller_index;
            players_set_input_user(player_index, user_index);
            players_set_input_controller(player_index, controller_index);
            players_set_local_player(user_index, player_index);
        }
    }
}


// @address 0x1de180 ?player_try_to_drop_weapon@@YA_NJ_N@Z

bool player_try_to_drop_weapon(long player_index, bool primary)
{
    player_datum *player = player_get(player_index);
    bool success = false;
    if (player->unit_index != NONE)
    {
        long weapon_set_index = !primary;
        s_player_unit_index unit_index = player_unit_index_get(player);
        unit_datum *unit = (unit_datum *)object_get(unit_index.value);
        long weapon_index = unit_inventory_get_weapon_or_none(unit_index.value, (&unit->unit.current_weapon_index)[weapon_set_index]);
        if (weapon_index != NONE)
        {
            if (game_simulation::game_is_predicted())
            {
                simulation_request_drop_weapon(player->unit_index, (short)weapon_set_index);
                success = true;
            }
            else
            {
                success = action_submit_event(player->unit_index, primary ? 46 : 47);
            }
        }
    }
    return success;
}


// match: 0x1de068: direct header lookup preserves the reference-parameter load.
static __forceinline unit_datum *players_put_away_unit_get(const long &unit_index)
{
    return (unit_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(unit_index)].datum;
}
static __forceinline long players_put_away_inventory_weapon(long unit_index, short inventory_index)
{
    unit_datum *unit = players_put_away_unit_get(unit_index);
    long weapon_index = NONE;
    if (inventory_index != NONE)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}

// @address 0x1de068 ?player_try_to_put_away_weapon@@YA_NJ_N@Z
bool player_try_to_put_away_weapon(long player_index, bool primary)
{
    player_datum *player = player_get(player_index);
    bool success = false;
    if (player->unit_index != NONE)
    {
        long weapon_set_index = !primary;
        long weapon_index = players_put_away_inventory_weapon(
            player->unit_index, players_put_away_unit_get(player->unit_index)->unit.current_weapon_indices[weapon_set_index]);
        if (weapon_index != NONE)
        {
            unit_weapon_pickup_result pickup_result;
            if (unit_can_pickup_weapon(player->unit_index, weapon_index, _weapon_addition_method_none, &pickup_result)
                && pickup_result.primary[weapon_set_index])
            {
                if (game_simulation::game_is_predicted())
                {
                    simulation_request_put_away_weapon(player->unit_index, (short)weapon_set_index);
                    success = true;
                }
                else if (::unit_put_away_weapon(player->unit_index, (short)weapon_set_index, false))
                {
                    success = true;
                }
            }
        }
    }
    return success;
}
// @address 0x1d8bc0 ?players_set_machines@@YAXKPBUs_machine_identifier@@@Z
void players_set_machines(unsigned long new_machine_valid_mask, const s_machine_identifier *new_machine_identifiers)
{
    unsigned long old_machine_valid_mask = players_globals->machine_valid_mask;
    s_machine_identifier old_machine_identifiers[16];
    memcpy(old_machine_identifiers, players_globals->machines, sizeof(old_machine_identifiers));
    long new_machine_indices[16];
    memset(new_machine_indices, NONE, sizeof(new_machine_indices));
    for (long machine_index = 0; machine_index < 16; machine_index++) {
        if (TEST_BIT(old_machine_valid_mask, machine_index)) {
            for (long new_machine_index = 0; new_machine_index < 16; new_machine_index++) {
                if (TEST_BIT(new_machine_valid_mask, new_machine_index)
                    && memcmp(&new_machine_identifiers[new_machine_index], &old_machine_identifiers[machine_index], sizeof(s_machine_identifier)) == 0) {
                    new_machine_indices[machine_index] = new_machine_index;
                    break;
                }
            }
        }
    }
    c_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (player->machine_index != NONE) {
            if (new_machine_indices[player->machine_index] == NONE)
                player_leave_game_internal(iterator.get_index());
        }
    }
    s_players_globals *globals = players_globals;
    globals->machine_valid_mask = new_machine_valid_mask;
    memcpy(globals->machines, new_machine_identifiers, sizeof(globals->machines));
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (player->machine_index != NONE) {
            long new_machine_index = new_machine_indices[player->machine_index];
            if (new_machine_index != NONE)
                player->machine_index = (short)new_machine_index;
        }
    }
    if (globals->network_valid) {
        long local_machine_index = NONE;
        for (long machine_index = 0; machine_index < 16; machine_index++) {
            if (TEST_BIT(new_machine_valid_mask, machine_index)
                && memcmp(&new_machine_identifiers[machine_index], &globals->machine, sizeof(s_machine_identifier)) == 0) {
                local_machine_index = machine_index;
                break;
            }
        }
        if (local_machine_index != NONE) {
            if (globals->local_machine_index == NONE) {
                players_set_local_machine(&globals->machine);
                players_remap_cooperative_users(true);
            } else {
                globals->local_machine_index = local_machine_index;
            }
        }
    }
}

// @address 0x1d82b8 ?player_reset@@YAXJ_N0PBUgame_player_options@@@Z
void player_reset(long player_index, bool new_player, bool initial_spawn, const game_player_options *options)
{
    player_datum *player = player_get(player_index);
    long machine_index = NONE;
    if (!options->left_game) {
        s_players_globals *globals = players_globals;
        for (long i = 0; i < 16; i++) {
            if ((globals->machine_valid_mask & (1 << i)) != 0 &&
                memcmp(&options->machine_identifier, &globals->machines[i], sizeof(s_machine_identifier)) == 0) {
                machine_index = i;
                break;
            }
        }
    }
    {
        c_player_data_iterator<player_datum> iterator;
        iterator.iterator.data = player_data;
        iterator.iterator.absolute_index = NONE;
        iterator.iterator.index = NONE;
        if (iterator.next()) { do {} while (iterator.next()); }
    }
    player->flags = 0;
    if (new_player && !(game_globals->options.game_mode == 1 && initial_spawn))
        player->flags = 8;
    else
        player->flags = 0;
    SET_BIT(player->flags, 13, initial_spawn);
    player->left_game_time = NONE;
    player->player_identifier = options->player_identifier;
    player->machine_index = (short)machine_index;
    player->machine = options->machine_identifier;
    player->user_index = options->user_index;
    player->controller_index = options->controller_index;
    player->unit_index = NONE;
    player->dead_unit_index = NONE;
    player->next_unit_index = NONE;
    player->teleport_counter = false;
    player->illegal_position_ticks = false;
    cluster_reference_invalidate(&player->cluster_reference);
    player->last_shot_at_object_index = NONE;
    player->sprint_recovery_ticks = 0;
    player->sprint_ticks = 0;
    player->grenade_recharge_ticks = 0;
    player->grace_ticks = 0;
    player->coop_respawn_pending = false;
    player->coop_respawn_initial = false;
    player->coop_respawn_delay_ticks = 0;
    player->ticks_without_unit = 0;
    player->tracking_object_index = NONE;
    player->tracking_model_index = NONE;
    player->tracking_ticks = 0;
    memset(&player->target_player_index, 0, offsetof(player_datum, achievement_kill_counters) - offsetof(player_datum, target_player_index));
    player->respawn_target_player_index = NONE;
    player->target_absolute_player_index = NONE;
    player->target_player_index = NONE;
    player->respawn_player_index = NONE;
    player->ticks_with_unit = NONE;
    player->lives = NONE;
    player->last_death_round_ticks = NONE;
    {
        c_player_traits *traits = &player->traits;
        traits->m_shield_vitality_traits.initialize();
        traits->m_weapon_traits.initialize();
        traits->m_movement_traits.initialize();
        traits->m_appearance_traits.initialize();
        traits->m_sensor_traits.initialize();
    }
    memset(player->powerup_times, 0, sizeof(player->powerup_times));
    if (new_player)
        player->respawn_counter = 0;
    player->achievement_kill_counters[0] = false;
    player->achievement_kill_counters[1] = false;
    memset(player->achievement_damage_info, 0, sizeof(player->achievement_damage_info));
    player->last_vehicle_exit_index = NONE;
    player->last_vehicle_exit_time = 0;
    for (unsigned long i = 0; i < sizeof(player->shots) / sizeof(player->shots[0]); ++i)
        player->shots[i].signed_identifier = NONE;
    player->campaign_player_slot = NONE;
    if (game_globals->options.game_mode == 1) {
        unsigned long used_slots = 0;
        c_player_in_game_iterator iterator;
        iterator.begin();
        while (iterator.next()) {
            if (iterator.get_index() != player_index) {
                signed char slot = iterator.get_datum()->campaign_player_slot;
                if (slot >= 0 && slot < 4)
                    used_slots |= 1 << slot;
            }
        }
        for (long i = 0; i < 4; ++i) {
            if ((used_slots & (1 << i)) == 0) {
                player->campaign_player_slot = (char)i;
                break;
            }
        }
    }
    player_set_configuration(player_index, &options->configuration);
    if (player->machine_index != NONE) {
        s_players_globals *globals = players_globals;
        if (player->machine_index == globals->local_machine_index) {
            if (players_game_has_active_structure_bsp()) {
                bool first_local_player = true;
                c_player_in_game_iterator iterator;
                iterator.begin();
                while (iterator.next()) {
                    if (iterator.get_index() != player_index && iterator.get_datum()->machine_index == globals->local_machine_index) {
                        first_local_player = false;
                        break;
                    }
                }
                if (first_local_player && !options->left_game)
                    players_remap_cooperative_users(1);
            } else {
                players_set_input_user(player_index, options->user_index);
                players_set_input_controller(player_index, options->controller_index);
                players_set_local_player(options->user_index, player_index);
            }
        }
    }
    if (options->left_game) {
        player->flags |= 2;
        player->left_game_time = game_time_globals->time;
    }
}

// @address 0x1d7c38 ?player_new@@YAJJPBUgame_player_options@@_N@Z
long player_new(long player_array_index, const game_player_options *options, bool initial_spawn)
{
    long player_index = datum_new_at_absolute_index(player_data, player_array_index);
    if (player_index != NONE) {
        player_reset(player_index, true, initial_spawn, options);
        if (!options->left_game) {
            players_globals->count++;
            simulation_player_joined_game(player_index);
            ai_player_state_create(player_index);
            game_engine_player_added(player_index);
        }
    }
    return player_index;
}

// @address 0x1d8130 ?player_rejoined_game@@YAXJPBUgame_player_options@@_N@Z
void player_rejoined_game(long player_index, const game_player_options *options, bool initial_spawn)
{
    player_reset(player_index, false, initial_spawn, options);
    players_globals->count++;
    simulation_player_joined_game(player_index);
    game_engine_player_rejoined(player_index);
    ai_player_state_create(player_index);
}

// game_player_options as players_detach_from_map builds it on the stack: the configuration's
// client and host halves construct themselves before the whole block is cleared.
// @address 0x1d7ae0 ?players_detach_from_map@@YAXXZ
void players_detach_from_map()
{
    c_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (!TEST_BIT(player->flags, 1)) {
            game_player_options options;
            memset(&options, 0, sizeof(options));
            options.valid = true;
            options.machine_identifier = player->machine;
            options.user_index = player->user_index;
            options.controller_index = player->controller_index;
            options.player_identifier = player->player_identifier;
            memcpy(&options.configuration, &player->desired_configuration, sizeof(options.configuration));
            long player_index = iterator.get_index();
            player_leave_game_internal(player_index);
            player_reset(player_index, false, false, (const game_player_options *)&options);
            players_globals->count++;
            simulation_player_joined_game(player_index);
            game_engine_player_rejoined(player_index);
            ai_player_state_create(player_index);
        }
        SET_BIT(player->flags, 3, true);
    }
}


// @address 0x381788 player_reset_equipment_throw
extern "C" void player_reset_equipment_throw(long p0)
{
    s_object_header *header = object_header_get(p0);
    unit_datum *datum = (unit_datum *)header->datum;
    short offset = datum->unit.action_offset;
    unsigned char *target = (unsigned char *)datum + offset;
    player_throw_equipment(p0, 1, 0, false);
    target[6] = 0;
}

// @address 0x381688 ?player_teleport@@YA_NJJPBTreal_point3d@@@Z
bool player_teleport(long p0, long p1, union real_point3d const *p2)
{
    bool result = true;
    s_object_header *header = object_header_get(p0);
    unit_datum *datum = (unit_datum *)header->datum;
    s_player_equipment_action_state *teleport = (s_player_equipment_action_state *)((char *)datum + datum->unit.action_offset);
    char state = teleport->active;
    char counter = ++teleport->released;
    switch (state)
    {
    case 1:
        if (counter >= 2)
            player_update_equipment_use(p0, 0);
        break;
    case 2:
        if (counter >= teleport->threshold)
            player_throw_equipment(p0, 0, 0, false);
        break;
    case 3:
        if (!motor_animation_running(p0, 35))
        {
            player_throw_equipment(p0, 1, 0, false);
            teleport->active = 0;
            result = false;
        }
        break;
    }
    return result;
}

static __forceinline bool players_object_is_multiplayer_cinematic_object(long object_index)
{
    object_datum *object = object_get(object_index);
    return TEST_BIT(object->flags_d6, 1);
}
// @address 0x1dcd40 player_handle_nearby_item
extern "C" void player_handle_nearby_item(long player_index, long object_index)
{
    player_datum *player = player_get(player_index);
    if (player->unit_index == NONE)
        return;
    if (players_object_is_multiplayer_cinematic_object(object_index))
        return;
    unit_datum *unit = (unit_datum *)object_get(player->unit_index);
    item_datum *item = (item_datum *)object_get(object_index);
    bool attached_usable = false;
    long game_mode = game_globals->options.game_mode;
    if (item->object.parent_object_index != NONE) {
        if ((unsigned char)((item->object.physics_flags >> 4) & 1) != 0 || ((1UL << object_get_type(item->object.parent_object_index)) & 0x14c2) != 0)
            attached_usable = true;
    }
    if (item->object.parent_object_index != NONE && !attached_usable)
        return;
    if (item->toss_unit_index == player->unit_index)
        return;
    float radius_sum = unit->object.bounding_radius + 0.4f;
    if (distance_squared3d(&unit->object.bounding_center, &item->object.bounding_center) >= radius_sum * radius_sum)
        return;
    if (game_mode == 2 && !unit_can_access_object(player->unit_index, object_index))
        return;
    long last_weapon = NONE;
    for (long slot_offset = 0; slot_offset < 4; slot_offset++) {
        long weapon_slot = (long)unit->unit.current_weapon_indices[0];
        if (weapon_slot < 0)
            weapon_slot = 0;
        long weapon_index = unit->unit.weapon_object_indices[(weapon_slot + slot_offset) % 4];
        if (weapon_index != NONE) {
            short rounds_picked_up;
            if (weapon_handle_potential_inventory_item(weapon_index, object_index, player->unit_index, player_index, &rounds_picked_up)
                && rounds_picked_up > 0) {
                player_iterator iterator;
                iterator.player_index = player_index;
                last_weapon = weapon_index;
                iterator.controller_index = NONE;
                while (player_iterate_output_users(&iterator))
                    chud_post_ammo_pickup(iterator.controller_index, object_get(weapon_index)->definition_index, rounds_picked_up);
            }
            if (last_weapon != NONE && last_weapon != weapon_index
                && object_get(last_weapon)->definition_index == object_get(weapon_index)->definition_index)
                object_wake(weapon_index);
        }
    }
    item_datum *type_datum = (item_datum *)object_try_and_get_and_verify_type(object_index, 8);
    if (type_datum != 0) {
        long definition_index = type_datum->object.definition_index;
        if (game_get_grenade_type_index_from_item_defintion(definition_index) != NONE) {
            bool can_pickup;
            if (game_globals->options.game_mode == 2)
                can_pickup = player->traits.m_weapon_traits.m_weapon_pickup != 2;
            else
                can_pickup = true;
            if (can_pickup) {
                if (game_simulation::game_is_predicted()) {
                    simulation_request_projectile_supercombine(object_index);
                } else if (unit_add_grenade_to_inventory(player->unit_index, object_index)) {
                    player_iterator iterator;
                    iterator.player_index = player_index;
                    iterator.controller_index = NONE;
                    while (player_iterate_output_users(&iterator))
                        chud_post_grenade_pickup(iterator.controller_index, type_datum->object.definition_index);
                }
            }
        } else {
            long unit_index = player->unit_index;
            if (unit_can_pickup_equipment(unit_index, object_index)) {
                long equipment_index = ((unit_datum *)object_get(unit_index))->unit.equipment_object_index;
                if (equipment_definition_get_type(definition_index, 0) == 1) {
                    if (game_simulation::game_is_predicted())
                        simulation_request_projectile_supercombine(object_index);
                    else
                        player_pickup_powerup(player_index, object_index);
                } else if (equipment_index == NONE) {
                    if (game_simulation::game_is_predicted())
                        simulation_request_projectile_supercombine(object_index);
                    else
                        player_pickup_equipment(player_index, object_index);
                }
            }
        }
    }
    if (TEST_BIT(1 << object_get_type(object_index), 2)) {
        unit_weapon_pickup_result pickup_result;
        if (unit_can_pickup_weapon(player->unit_index, object_index, _weapon_addition_method_none, &pickup_result)
            && pickup_result.primary[0]
            && player_should_auto_pickup_weapon(player_index, object_index)) {
            if (game_simulation::game_is_predicted()) {
                simulation_action_pickup_weapon(player->unit_index, object_index, 3);
            } else {
                s_weapon_swap_request swap_request;
                memset(&swap_request, 0, sizeof(swap_request));
                swap_request.type = 44;
                swap_request.weapon = object_index;
                swap_request.mode = 3;
                action_submit(player->unit_index, &swap_request);
            }
        }
    }
    game_engine_player_nearby_item(player_index, object_index);
}
// @address 0x1dc728 ?player_examine_nearby_objects@@YAXJ@Z
void player_examine_nearby_objects(long player_index)
{
    player_datum *player = player_get(player_index);
    if (player->unit_index != NONE)
    {
        object_datum *unit = object_get(player->unit_index);
        if (unit->parent_object_index == NONE)
        {
            long object_indices[64];
            long object_count = objects_in_sphere(0, 0x14ce, &unit->location, &unit->bounding_center,
                unit->bounding_radius + 0.4f, object_indices, sizeof(object_indices) / sizeof(object_indices[0]));
            for (long index = 0; index < object_count; index++)
            {
                long object_index = object_indices[index];
                object_datum *object = object_get(object_index);
                long object_type = object->object_type;
                switch (object_type)
                {
                case 2:
                case 3:
                    player_handle_nearby_item(player_index, object_index);
                    break;
                }
                if ((1UL << object_type) & 0x14c2)
                {
                    for (long child_index = object->first_child_object_index;
                         child_index != NONE && object_count < sizeof(object_indices) / sizeof(object_indices[0]);
                         child_index = object_get(child_index)->next_object_index)
                    {
                        object_indices[object_count++] = child_index;
                    }
                }
            }
        }
    }
}

// @address 0x1dd7d8 player_consider_biped
extern "C" bool player_consider_biped(long player_index, long biped_index, s_player_action_context *context)
{
    player_datum *player = player_get(player_index);
    unit_datum *biped = (unit_datum *)object_get(biped_index);
    bool result = false;
    if (TEST_BIT(1 << object_get_type(player->unit_index), 0) && (unsigned char)((biped->object.damage_flags >> 2) & 1) == 0)
    {
        if (game_team_is_enemy((e_game_team)player->configuration.host.team_index, (e_game_team)biped->unit.team))
        {
            context->melee_target_unit = biped_calculate_better_melee_target(player->unit_index, biped_index, context->melee_target_unit);
        }
        else if (biped_can_trade_weapon_with_player(player_index, biped_index))
        {
            s_player_interaction interaction;
            memset(&interaction, 0, sizeof(interaction));
            interaction.type = 7;
            interaction.object_index = biped_index;
            result = player_consider_interaction(player_index, &interaction, context);
        }
    }
    return result;
}

// @address 0x1d9aa0 player_get_teleport_source_pose
extern "C" void player_get_teleport_source_pose(long player_index, real_point3d *position, real_vector3d *forward)
{
    float height_offset = 0.2f;
    player_datum *player = player_get(player_index);
    object_get_center_of_mass(player->unit_index, position);
    object_get_orientation(player->unit_index, forward, NULL);
    forward->k = 0.0f;
    normalize3d_with_default(forward, &global_identity4x3->forward);
    if (TEST_BIT(1 << object_get_type(player->unit_index), 0)) {
        float height;
        float radius;
        ::biped_get_physics_pill_from_definition(player->unit_index, position, (float *)(&height), (float *)(&radius));
        height_offset = radius;
    }
    position->z += height_offset;
}

#ifndef __cplusplus
typedef unsigned char bool;
#define true 1
#define false 0
#endif
// @address 0x1d8af0 player_delete
extern "C" void player_delete(long player_index)
{
    player_set_unit_index(player_index, NONE);
    players_set_input_user(player_index, NONE);
    long absolute_player_index = datum_index_to_absolute_index(player_index);
    s_user_globals *mapping = player_mapping_globals;
    long controller_index = mapping->player_input_controller_mapping[absolute_player_index];
    if (controller_index != NONE) {
        mapping->user_controller_mapping[controller_index] = NONE;
        mapping->player_input_controller_mapping[absolute_player_index] = NONE;
        mapping->player_controller_count--;
    }
    players_remove_local_player(player_index);
    players_remap_cooperative_users(0);
    datum_delete(player_data, player_index);
    game_engine_player_deleted(player_index);
    objects_handle_deleted_player(player_index);
    long offset = 0;
    do
    {
        s_director_storage *storage = (s_director_storage *)((char *)g_director_globals + offset);
        storage->get_director()->handle_deleted_player(player_index);
        offset += sizeof(s_director_storage);
    } while (offset < (long)sizeof(g_director_globals->directors));
}

// @address 0x1dddf8 ?player_try_to_swap_weapons@@YA_NJ_NPBUs_player_interaction@@@Z
bool player_try_to_swap_weapons(long player_index, bool primary, const s_player_interaction *interaction)
{
    player_datum *player = player_get(player_index);
    bool result = false;
    if ((interaction->type == 1 || interaction->type == 9) && player_interaction_exists(player_index, 4, interaction)) {
        long weapon_index = interaction->object_index;
        unit_weapon_pickup_result pickup;
        if (((unit_datum *)object_get(weapon_index))->unit.actor_index == NONE &&
            unit_can_pickup_weapon(player->unit_index, weapon_index, _weapon_addition_method_none, &pickup)) {
            if (!game_simulation::game_is_predicted()) {
                long weapon_set_index = !primary;
                unit_datum *unit = (unit_datum *)object_get(player->unit_index);
                long current_weapon = unit_inventory_get_weapon_or_none(player->unit_index, (&unit->unit.current_weapon_index)[weapon_set_index]);
                if (current_weapon != NONE && players_pickup_must_be_readied(current_weapon))
                    player_try_to_drop_weapon(player_index, primary);
            }
            long method = NONE;
            if (primary) {
                if (pickup.primary[0])
                    method = 3;
                else if (pickup.backpack[0])
                    method = 4;
            } else {
                if (pickup.primary[1])
                    method = 5;
                else if (pickup.backpack[1])
                    method = 6;
            }
            if (method != NONE) {
                if (game_simulation::game_is_predicted()) {
                    simulation_action_pickup_weapon(player->unit_index, weapon_index, method);
                    result = true;
                } else {
                    s_weapon_swap_request swap_request;
                    memset(&swap_request, 0, sizeof(swap_request));
                    swap_request.type = 44;
                    swap_request.weapon = weapon_index;
                    swap_request.mode = method;
                    if (action_submit(player->unit_index, &swap_request))
                        result = true;
                }
            }
        }
    }
    return result;
}

// match: 0x1db0e0: retain bool return; canonical unsigned char adds return normalization.


static __forceinline long players_submit_unit_inventory_get_weapon(long unit_index, short inventory_index)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    long weapon_index = NONE;
    if (inventory_index != NONE)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}
static __forceinline weapon_definition *players_get_weapon_object_definition(long weapon_index)
{
    return (weapon_definition *)tag_get(0, ((unit_datum *)object_get(weapon_index))->object.definition_index);
}
static __forceinline bool players_submit_seat_flag(long vehicle_index, short seat_index, long bit)
{
    unit_definition *definition = (unit_definition *)tag_get(0, ((unit_datum *)object_get(vehicle_index))->object.definition_index);
    return FLAG_TEST(definition->seats[seat_index].flags, bit);
}
static __forceinline bool players_submit_weapon_flag(long weapon_index, long bit)
{
    return FLAG_TEST(players_get_weapon_object_definition(weapon_index)->flags, bit);
}
static __forceinline bool players_submit_weapon_allows_lunge(long weapon_index)
{
    bool result = false;
    if (weapon_index != NONE)
        result = players_get_weapon_object_definition(weapon_index)->melee_damage_parameters[5].damage.index != NONE;
    return result;
}
static __forceinline bool players_submit_weapon_charges_melee(long weapon_index)
{
    bool result = false;
    if (weapon_index != NONE)
        result = players_submit_weapon_flag(weapon_index, 28);
    return result;
}
static __forceinline float players_submit_distance_squared(const real_point3d *a, const real_point3d *b)
{
    real_vector3d vector;
    vector.i = b->x - a->x;
    vector.j = b->y - a->y;
    vector.k = b->z - a->z;
    return vector.i * vector.i + vector.j * vector.j + vector.k * vector.k;
}
// @rdata 0x82024264
static const float k_player_tracking_target_timeout_seconds = 5.0f;
static __forceinline long players_submit_seconds_to_ticks(float seconds)
{
    return (long)(seconds * (float)game_time_globals->ticks_per_second + 0.5f);
}
static __forceinline void player_suppress_action(long player_index, long action_type)
{
    player_datum *player = player_get(player_index);
    switch (action_type) {
    case 0:
        player->flags |= FLAG(4);
        break;
    case 1: {
        long user_index = player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player_index)];
        player->flags |= FLAG(5);
        if (user_index != NONE) {
            s_player_control_user_state *state = &((s_player_control_globals *)player_control_globals)->users[user_index];
            if (state->switch_weapon_held)
                state->switch_weapon_consumed = true;
            state->action.weapons.set_identifier = NONE;
        }
        break;
    }
    case 2:
        player->flags |= FLAG(6);
        break;
    }
}
static __forceinline void player_use_equipment(long player_index)
{
    player_datum *player = player_get(player_index);
    long unit_index = player->unit_index;
    long equipment_index = ((unit_datum *)object_get(unit_index))->unit.equipment_object_index;
    if (equipment_index != NONE)
        equipment_activate(equipment_index, unit_index, 0);
}
static __forceinline void simulation_request_unit_exit_vehicle(long unit_index, bool detach)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    if (simulation_query_local_predictive_control(unit_index, 0)) {
        long object_indices[2];
        s_exit_vehicle_event event_data;
        object_indices[0] = unit_index;
        object_indices[1] = unit->object.parent_object_index;
        event_data.seat_index = unit->unit.parent_seat_index;
        event_data.detach = detach;
        simulation_event_generate(_simulation_event_type_unit_exit_vehicle, 2, object_indices, NONE, sizeof(event_data), &event_data,
            g_network_configuration.event_relevance[_simulation_event_type_unit_exit_vehicle].delay_msec);
    }
}

static __forceinline float point_magnitude2d(const real_point2d *vector)
{
    return sqrtf(vector->x * vector->x + vector->y * vector->y);
}
// @address 0x1db0e0 ?player_submit_actions@@YAXJJPAUplayer_action@@@Z
void player_submit_actions(long player_index, long unit_index, player_action *action)
{
    player_datum *player = player_get(player_index);
    unit_datum *unit = ((unit_datum *)object_get(unit_index));

    if (TEST_BIT(action->action_flags, 2) && player_handle_flip_interaction(player_index, &action->action_context.interaction)) {
        player_suppress_action(player_index, 0);
        action->action_flags &= ~FLAG(2);
    }
    if (TEST_BIT(action->action_flags, 1) && player_handle_interaction_held(player_index, &action->action_context.interaction)) {
        player_suppress_action(player_index, 0);
        action->action_flags &= ~FLAG(1);
    }
    if (TEST_BIT(action->action_flags, 1) && player_try_to_swap_weapons(player_index, true, &action->action_context.interaction)) {
        player_suppress_action(player_index, 0);
        action->action_flags &= ~FLAG(1);
    }
    if (TEST_BIT(action->action_flags, 5) && player_try_to_swap_weapons(player_index, false, &action->action_context.interaction)) {
        player_suppress_action(player_index, 1);
        action->action_flags &= ~FLAG(1);
    }

    if (TEST_BIT(action->action_flags, 11)) {
        unsigned long put_away_mask = 0;
        unsigned long drop_mask = 0;
        bool success = false;
        for (long weapon_set_index = 0; weapon_set_index < 2; weapon_set_index++) {
            long weapon_index = players_submit_unit_inventory_get_weapon(
                unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_set.weapon_indices[weapon_set_index]);
            if (weapon_index != NONE) {
                if (weapon_is_support_weapon_or_must_be_readied(weapon_index)) {
                    drop_mask |= FLAG(weapon_set_index);
                } else if (weapon_set_index == 1) {
                    drop_mask |= FLAG(1);
                    put_away_mask |= FLAG(1);
                }
            }
        }
        if (TEST_BIT(put_away_mask, 0) && player_try_to_put_away_weapon(player_index, true))
            success = true;
        else if (TEST_BIT(drop_mask, 0) && player_try_to_drop_weapon(player_index, true))
            success = true;
        else if (TEST_BIT(put_away_mask, 1) && player_try_to_put_away_weapon(player_index, false))
            success = true;
        else if (TEST_BIT(drop_mask, 1) && player_try_to_drop_weapon(player_index, false))
            success = true;
        if (success) {
            player_suppress_action(player_index, 2);
            action->action_flags &= ~FLAG(11);
        }
    }

    if (TEST_BIT(action->action_flags, 2)
        && players_submit_unit_inventory_get_weapon(unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_set.weapon_indices[0]) != NONE) {
        action->action_flags &= ~FLAG(2);
        action->flags |= FLAG(29);
    }
    if (TEST_BIT(action->action_flags, 6)
        && players_submit_unit_inventory_get_weapon(unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_set.weapon_indices[1]) != NONE) {
        action->action_flags &= ~FLAG(6);
        action->flags |= FLAG(30);
    }

    if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE) {
        if (players_submit_seat_flag(unit->object.parent_object_index, unit->unit.parent_seat_index, 11) && !mover_is_in_entering_state(unit_index)) {
            bool exited = false;
            bool predicted = false;
            if (game_globals->options.game_simulation == 4)
                predicted = true;
            if (!predicted)
                exited = action_submit_event(unit_index, 16);
            if (!exited && TEST_BIT(action->flags, 28))
                action_submit_event(unit_index, 56);
        }
        if (TEST_BIT(action->action_flags, 2) || TEST_BIT(action->action_flags, 1)) {
            bool can_exit = true;
            if (mover_is_in_entering_state(unit_index) && motor_animation_base_channel_time(unit_index, NULL) < 1.0f)
                can_exit = false;
            if (can_exit) {
                bool success;
                bool predicted = false;
                if (game_globals->options.game_simulation == 4)
                    predicted = true;
                if (predicted) {
                    simulation_request_unit_exit_vehicle(unit_index, false);
                    success = true;
                } else {
                    success = action_submit_event(unit_index, 13);
                }
                if (success) {
                    player_suppress_action(player_index, 0);
                    action->action_flags &= ~(FLAG(2) | FLAG(1));
                }
            }
        }
    }

    if (TEST_BIT(action->flags, 4))
        player_use_equipment(player_index);

    unsigned long buttons = action->flags;
    if (TEST_BIT(buttons, 5) || TEST_BIT(buttons, 6)) {
        long weapon_index = players_submit_unit_inventory_get_weapon(unit_index, ((unit_datum
            *)object_get(unit_index))->unit.current_weapon_set.weapon_indices[0]);
        bool melee_weapon = weapon_index != NONE && players_submit_weapon_flag(weapon_index, 24);
        bool lunge_weapon = weapon_index != NONE && players_submit_weapon_allows_lunge(weapon_index);
        bool melee = false;
        bool charged = false;
        long target_index = NONE;
        bool lunge = false;
        bool set_recovery = false;
        bool melee_ready = true;
        if (melee_weapon) {
            bool weapon_ready = false;
            if (TEST_BIT(((weapon_datum *)((unit_datum *)object_get(weapon_index)))->flags_bytes[BIG_ENDIAN_VIEW_INDEX(0, 2)], 0))
                weapon_ready = true;
            melee_ready = weapon_ready && player->melee_recovery_ticks == 0;
            if (!weapon_can_be_fired(weapon_index) && TEST_BIT(buttons, 6))
                action->flags = (buttons & ~FLAG(6)) | FLAG(5);
        }
        if (unit->unit.current_weapon_set.weapon_indices[0] == NONE
            || unit->unit.current_weapon_set.weapon_indices[0] == unit->unit.desired_weapon_set.weapon_indices[0]) {
            if (TEST_BIT(action->flags, 6) && melee_weapon && melee_ready) {
                if (!weapon_uses_charging_melee(weapon_index)) {
                    long melee_target_index = unit_get_melee_target(unit_index);
                    melee = true;
                    if (players_submit_weapon_charges_melee(weapon_index))
                        charged = true;
                    if (melee_target_index != NONE) {
                        bool can_lunge = true;
                        if (weapon_has_infinite_ammo(weapon_index))
                            can_lunge = false;
                        if (can_lunge) {
                            real_point3d target_position;
                            real_point3d unit_position;
                            object_get_origin(melee_target_index, &target_position);
                            object_get_origin(unit_index, &unit_position);
                            if (lunge_weapon || players_submit_distance_squared(&target_position, &unit_position) > 0.9f * 0.9f) {
                                charged = false;
                                lunge = true;
                                set_recovery = true;
                                target_index = melee_target_index;
                            }
                        }
                    }
                }
            } else if (TEST_BIT(action->flags, 5) && melee_ready) {
                melee = true;
            }
        }
        if (melee) {
            action_request request;
            memset(&request, 0, sizeof(request));
            long request_target_index = NONE;
            if (lunge && target_index != NONE) {
                request_target_index = target_index;
            } else {
                long object_index = action->action_context.melee_target_unit;
                if (object_try_and_get_unsafe_and_verify_type(object_index, 0x1003))
                    request_target_index = object_index;
            }
            memset(&request, 0, sizeof(request));
            request.type = 60;
            if (lunge)
                request.melee_player.type = 2;
            else
                request.melee_player.type = charged ? 3 : 1;
            request.melee_player.target = request_target_index;
            if (simulation_query_local_predictive_control(unit_index, 0))
                request.melee_player.weapon_slot = lunge ? 2 : 1;
            else
                request.melee_player.weapon_slot = 0;
            if (action_submit(unit_index, (const action_request *)&request) && set_recovery)
                player->melee_recovery_ticks = (unsigned char)players_submit_seconds_to_ticks(1.1f);
        }
        if (TEST_BIT(action->flags, 5) && weapon_index != NONE && players_submit_weapon_flag(weapon_index, 13))
            player_try_to_drop_weapon(player_index, true);
        if (TEST_BIT(action->flags, 5) && unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE
            && players_submit_seat_flag(unit->object.parent_object_index, unit->unit.parent_seat_index, 22)) {
            bool predicted = false;
            if (game_globals->options.game_simulation == 4)
                predicted = true;
            if (predicted)
                simulation_request_unit_exit_vehicle(unit_index, true);
            else
                action_submit_event(unit_index, 14);
        }
    }

    if (players_submit_unit_inventory_get_weapon(unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_set.weapon_indices[0]) != NONE) {
        long weapon_index = players_submit_unit_inventory_get_weapon(unit_index, ((unit_datum
            *)object_get(unit_index))->unit.current_weapon_set.weapon_indices[0]);
        unsigned long flags = players_get_weapon_object_definition(weapon_index)->flags;
        if (FLAG_TEST(flags, 12) || FLAG_TEST(flags, 13))
            action->flags &= ~FLAG(28);
    }
    if (TEST_BIT(action->flags, 28))
        action_submit_event(unit_index, 56);

    long current_weapon_index = players_submit_unit_inventory_get_weapon(unit_index, ((unit_datum
        *)object_get(unit_index))->unit.current_weapon_set.weapon_indices[0]);
    if (current_weapon_index != NONE && players_submit_weapon_flag(current_weapon_index, 3)) {
        action->weapons = unit->unit.desired_weapon_set;
        if (TEST_BIT(action->flags, 18) || TEST_BIT(action->flags, 28))
            player_try_to_drop_weapon(player_index, true);
    }

    if (unit->object.parent_object_index == NONE && !game_is_multiplayer()) {
        bool sprinting = false;
        s_player_control_constants *information = (s_player_control_constants *)global_game_globals->player_control.address;
        float recovery_rate = information->sprint_recovery_rate;
        long sprint_ticks = players_submit_seconds_to_ticks(information->sprint_ticks_seconds);
        long recovery_ticks = players_submit_seconds_to_ticks(information->sprint_recovery_seconds);
        float throttle_angle = (float)atan2(action->move.y, action->move.x);
        if (point_magnitude2d(&action->move) > information->sprint_minimum_throttle
            && throttle_angle > information->sprint_maximum_angle * -0.017453292f
            && throttle_angle < information->sprint_maximum_angle * 0.017453292f)
            sprinting = true;
        float recovered = (float)player->sprint_recovery_ticks * recovery_rate;
        if ((float)player->sprint_ticks - recovered < 0.0f) {
            player->sprint_ticks = 0;
            player->sprint_recovery_ticks = 0;
        } else if ((float)player->sprint_ticks - recovered >= (float)(recovery_ticks + sprint_ticks)) {
            player->sprint_ticks = (short)(recovery_ticks + sprint_ticks);
            player->sprint_recovery_ticks = 0;
        }
        unsigned long action_buttons = action->flags;
        bool soft_ping = unit_animation_is_running_damage_overlay(unit_index)
            | (action->zoom != unit->unit.zoom_level)
            | FLAG_TEST(action_buttons, 30) | FLAG_TEST(action_buttons, 29) | FLAG_TEST(action_buttons, 0);
        bool hard_ping = false;
        unit_datum *animation_unit = ((unit_datum *)object_get(unit_index));
        c_animation_manager *animation = (c_animation_manager *)((char *)animation_unit + animation_unit->object.animation.offset);
        if (FLAG_TEST(animation->m_flags, 6) && animation->m_current_goal.state == 0x94)
            hard_ping = true;
        bool interrupted = FLAG_TEST(action_buttons, 28) | FLAG_TEST(action_buttons, 19) | FLAG_TEST(action_buttons, 18)
            | FLAG_TEST(action_buttons, 5) | FLAG_TEST(action_buttons, 4) | FLAG_TEST(action_buttons, 0) | hard_ping;
        if (soft_ping || interrupted) {
            player->sprint_ticks = (short)MAX(0, player->sprint_ticks - (long)((float)player->sprint_recovery_ticks * recovery_rate + 0.5f));
            player->sprint_recovery_ticks = 0;
        }
        if (soft_ping) {
            player->sprint_ticks = (short)(player->sprint_ticks > sprint_ticks ? sprint_ticks : player->sprint_ticks);
        }
        if (interrupted)
            player->sprint_ticks = 0;
        if (unit->object.object_type != 0 || !motor_system_in_airborne_state(unit_index)) {
            if (sprinting)
                player->sprint_ticks++;
            else
                player->sprint_recovery_ticks++;
        }
    } else {
        player->sprint_ticks = 0;
        player->sprint_recovery_ticks = 0;
    }

    long primary_weapon_index = players_submit_unit_inventory_get_weapon(unit_index, ((unit_datum
        *)object_get(unit_index))->unit.current_weapon_set.weapon_indices[0]);
    long secondary_weapon_index = players_submit_unit_inventory_get_weapon(
        unit_index, ((unit_datum *)object_get(unit_index))->unit.current_weapon_set.weapon_indices[1]);
    if ((primary_weapon_index != NONE && weapon_is_tracking(primary_weapon_index))
        || (secondary_weapon_index != NONE && weapon_is_tracking(secondary_weapon_index))) {
        if (action->aim_assist.object_index != NONE && !TEST_BIT(action->aim_assist.flags, 1)) {
            player->tracking_object_index = action->aim_assist.object_index;
            player->tracking_model_index = (short)action->aim_assist.model_index;
            player->tracking_ticks = 0;
        } else if (player->tracking_object_index != NONE) {
            player->tracking_ticks++;
            if ((float)player->tracking_ticks * game_time_globals->tick_length > k_player_tracking_target_timeout_seconds) {
                player->tracking_object_index = NONE;
                player->tracking_model_index = NONE;
                player->tracking_ticks = 0;
            }
        }
    } else {
        player->tracking_object_index = NONE;
        player->tracking_model_index = NONE;
        player->tracking_ticks = 0;
    }
}


// Unit-datum fields player_leave_game_internal touches.


// match: 0x1df898: the direct machine member preserves the aligned word/halfword
// clear; the canonical anonymous-union alias makes c2 emit a six-byte loop.


// @address 0x1df898 ?player_leave_game_internal@@YAXJ@Z
void player_leave_game_internal(long player_index)
{
    s_leave_player_prefix *player = (s_leave_player_prefix *)player_get(player_index);
    long lock = tag_resources_lock_game();
    players_handle_deleted_object(player_index);
    if (player->unit_index != NONE) {
        long unit_index = player->unit_index;
        unit_datum *unit = ((unit_datum *)object_get(unit_index));
        unit->object.damage_flags |= 0x1000;
        object_wake(unit_index);
    }
    if (game_simulation::game_is_predicted()) {
        c_object_iterator<unit_datum> iterator;
        iterator.begin(0x1003, 0);
        while (iterator.next()) {
            unit_datum *unit = iterator.get_datum();
            if (unit->unit.simulation_player_index == player_index)
                unit->unit.simulation_player_index = NONE;
        }
    }
    player_set_unit_index(player_index, NONE);
    if (player->dead_unit_index != NONE) {
        unit_datum *unit = ((unit_datum *)object_get(player->dead_unit_index));
        if (TEST_BIT(unit->object.recycling_flags, 1))
            unit->object.garbage_time = game_time_get();
    }
    player->dead_unit_index = NONE;
    simulation_delete_world_player(player_index);
    player->machine_index = NONE;
    memset(&player->machine, 0, sizeof(player->machine));
    player->user_index = NONE;
    player->controller_index = NONE;
    players_set_input_user(player_index, NONE);
    long absolute_player_index = datum_index_to_absolute_index(player_index);
    s_user_globals *mapping = player_mapping_globals;
    long input_user_index = mapping->player_input_controller_mapping[absolute_player_index];
    if (input_user_index != NONE) {
        mapping->user_controller_mapping[input_user_index] = NONE;
        mapping->player_input_controller_mapping[absolute_player_index] = NONE;
        mapping->player_controller_count--;
    }
    players_remove_local_player(player_index);
    player->flags |= 2;
    player->left_game_time = game_time_get();
    player->flags &= ~1;
    players_globals->count--;
    game_engine_player_left(player_index);
    if (game_globals->options.game_mode == 1) {
        unsigned long ai_count = 4;
        ai_player_state *ai_player = tls_ai_player_states();
        do {
            if (ai_player->player_index == player_index)
                ai_player->player_index = NONE;
            ai_count--;
            ai_player++;
        } while (ai_count != 0);
    }
    players_remap_cooperative_users(false);
    tag_resources_unlock_game(lock);
}


// @rdata 0x820242d4
static const float k_players_coop_respawn_delay_seconds = 3.0f;
static __forceinline long players_update_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)game_time_globals->ticks_per_second + 0.5f);
}
static __forceinline bool players_update_bink_playing(void)
{
    return bink_globals.playing;
}
static __forceinline long players_update_respawn_time_seconds(long player_index)
{
    return current_game_variant()->respawn_time_seconds;
}
static __forceinline unit_datum *players_update_unit_get(long unit_index)
{
    return (unit_datum *)object_get(unit_index);
}
static __forceinline void players_update_unit_control_maintain(long unit_index)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    unit_control_data control;
    unit_control_clear(&control);
    control.facing_vector = unit->unit.desired_facing_vector;
    control.aiming_vector = unit->unit.desired_aiming_vector;
    control.looking_vector = unit->unit.desired_looking_vector;
    unit_control(unit_index, &control);
}
// @address 0xa70c0 ?players_update_before_game@@YAXPBUsimulation_update@@@Z
void players_update_before_game(const simulation_update *update)
{
    if (players_globals->pending)
        player_positions_initialize_for_new_structure_bsp(0);

    players_update_activation();
    g_player_control_globals_deterministic->action_test_flags = update->action_test_flags;

    if (game_is_cooperative()) {
        for (long user_index = 0; user_index < 4; user_index++) {
            long player_index = NONE;
            if (user_index != NONE)
                player_index = player_mapping_globals->player_indices[user_index];
            if (player_index != NONE) {
                short absolute_index = (short)player_index;
                if ((1 << absolute_index) & update->player_valid_mask)
                    player_control_set_external_action(user_index, &update->actions[absolute_index]);
            }
        }
    }

    long multiplayer_player_indices[16];
    long multiplayer_player_indices_count = 0;
    c_player_data_iterator<player_datum> player_iterator;
    player_iterator.begin(player_data);
    while (player_iterator.next()) {
        player_datum *player = player_iterator.get_datum();
        unsigned long player_flags = player->flags;
        if (!TEST_BIT(player_flags, 1) && TEST_BIT(player_flags, 0) && player->unit_index == NONE) {
            if (game_globals->options.game_mode == 2) {
                long player_index = player_iterator.iterator.index;
                if (game_is_authoritative() && game_engine_should_spawn_player(player_index)) {
                    multiplayer_player_indices[multiplayer_player_indices_count] = player_index;
                    multiplayer_player_indices_count++;
                }
            } else if (game_globals->options.game_mode == 1 && !game_is_predicted() && !TEST_BIT(player_flags, 13)) {
                if (TEST_BIT(player_flags, 3)) {
                    player_spawn(player_iterator.iterator.index, 0, 0);
                } else if (game_campaign_is_multiplayer() && game_is_coop_respawn_allowed()) {
                    player_datum *respawn_player = player_get(player_iterator.iterator.index);
                    if (!respawn_player->coop_respawn_pending) {
                        respawn_player->coop_respawn_pending = 1;
                        respawn_player->coop_respawn_initial = 0;
                        respawn_player->coop_respawn_delay_ticks = players_update_seconds_to_ticks_round(k_players_coop_respawn_delay_seconds);
                    }
                }
            }
        }
    }

    while (multiplayer_player_indices_count > 0) {
        long spawn_index = 0;
        if (multiplayer_player_indices_count > 1)
            spawn_index = _random(tls_random_seed(), __FILE__, __FUNCTION__, __LINE__) % multiplayer_player_indices_count;
        if (game_engine_should_spawn_player(multiplayer_player_indices[spawn_index])) {
            long teleport_object = NONE;
            bool teleport_valid = false;
            real_point3d teleport_point;
            if (current_game_engine() != 0)
                current_game_engine()->player_about_to_spawn(multiplayer_player_indices[spawn_index]);
            game_engine_assemble_player_traits(multiplayer_player_indices[spawn_index]);
            bool spawned = player_spawn(multiplayer_player_indices[spawn_index], teleport_valid ? &teleport_point : 0, 0);
            if (spawned && teleport_object != NONE)
                player_teleport(multiplayer_player_indices[spawn_index], teleport_object, &teleport_point);
            if (spawned && current_game_engine() != 0)
                current_game_engine()->player_just_spawned(multiplayer_player_indices[spawn_index]);
        } else {
            player_datum *respawn_player = player_get(multiplayer_player_indices[spawn_index]);
            if (TEST_BIT(respawn_player->flags, 3))
                respawn_player->respawn_timer = players_update_seconds_to_ticks_round(
                    (float)players_update_respawn_time_seconds(multiplayer_player_indices[spawn_index]));
        }
        if (spawn_index != multiplayer_player_indices_count - 1)
            multiplayer_player_indices[spawn_index] = multiplayer_player_indices[multiplayer_player_indices_count - 1];
        multiplayer_player_indices_count--;
    }

    game_engine_update_player_sitting_out();
    players_globals->respawn_failure = 0;
    player_iterator.begin(player_data);
    while (player_iterator.next())
        player_update_coop_respawn(player_iterator.iterator.index);

    player_iterator.begin(player_data);
    while (player_iterator.next()) {
        player_datum *player = player_iterator.get_datum();
        long object_index = player->unit_index;
        if (object_index == NONE)
            object_index = player->dead_unit_index;
        if (object_index != NONE) {
            long parent_index = NONE;
            while (object_index != NONE) {
                parent_index = object_index;
                object_index = object_get(object_index)->parent_object_index;
            }
            object_datum *parent = object_get(parent_index);
            if ((unsigned char)parent->location.cluster_reference.bsp_index != 255)
                player->cluster_reference = parent->location.cluster_reference;
        }
    }

    player_iterator.begin(player_data);
    while (player_iterator.next()) {
        long player_index = player_iterator.iterator.index;
        player_datum *player = player_iterator.get_datum();
        player_action *action = 0;
        unsigned short absolute_index = (unsigned short)player_index;
        if (((1 << absolute_index) & update->player_valid_mask) && player_is_action_valid((player_action *)&update->actions[absolute_index]))
            action = (player_action *)&update->actions[absolute_index];
        if (action != 0 && player->unit_index != NONE) {
            unit_datum *unit = ((unit_datum *)object_get(player->unit_index));
            if (TEST_BIT(unit->unit.unit_flags, 0)) {
                if (TEST_BIT(action->action_flags, 12) && (cinematic_can_skip() || players_update_bink_playing())) {
                    s_cinematic_globals_definition *cinematic_globals = (s_cinematic_globals_definition *)::cinematic_globals;
                    if (!cinematic_globals->skip) {
                        cinematic_globals->skip = true;
                        cinematic_globals->start_time = game_time_globals->time;
                        cinematic_globals->skip_end_time = game_time_globals->time + 10;
                    }
                    s_event datamine;
                    data_mining_event_initialize(&datamine, 0, "cinematic skipped", 1, "cinematics");
                }
                if (players_globals->inhibited == 0) {
                    player_action local_action;
                    memcpy(&local_action, action, sizeof(local_action));
                    player_update_input_target(player_index, &local_action);
                    if (players_globals->mostly_inhibited != 0) {
                        long context = local_action.unit_valid;
                        unsigned char mode = local_action.unit_flags;
                        real_euler_angles2d facing = local_action.facing;
                        player_action_clear(&local_action);
                        local_action.unit_valid = context;
                        local_action.unit_flags = mode;
                        local_action.facing = facing;
                    }
                    player_prepare_action(player_index, &local_action);
                    player_training_examine_action(player_index, &local_action);
                    if (!game_engine_is_player_in_edit_mode(player_index, 0, 0))
                        player_submit_actions(player_index, player->unit_index, &local_action);
                    if (local_action.unit_valid == 1 && local_action.unit_flags == unit->unit.unit_input_flags)
                        player_submit_control(player_index, player->unit_index, &local_action);
                } else if (unit->unit.actor_index == NONE) {
                    players_update_unit_control_maintain(player->unit_index);
                }
            }
        }
    }
}


struct effect_vector_field;
struct bsp3d;
static const float players_half_second = 0.5f;
static __forceinline long players_after_game_seconds_to_ticks_round(float seconds)
{
    float ticks = (float)game_time_globals->ticks_per_second * seconds;
    return (long)(ticks + players_half_second);
}
static __forceinline long object_get_topmost_parent(long object_index)
{
    long parent_index;
    do {
        parent_index = object_index;
        object_index = object_get(object_index)->parent_object_index;
    } while (object_index != NONE);
    return parent_index;
}
static __forceinline void unit_kill_direct(long unit_index)
{
    if (unit_index != NONE) {
        object_get(unit_index)->damage_flags |= 0x40;
        object_wake(unit_index);
    }
}
static __forceinline long players_first_active_bsp(void)
{
    return lowest_bit_set(g_active_structure_bsp_mask);
}
static __forceinline long players_next_active_bsp(long index)
{
    return lowest_bit_set(g_active_structure_bsp_mask & ~(unsigned long)((1i64 << (index + 1)) - 1));
}

// @address 0x117850 ?players_update_after_game@@YAXPBUsimulation_update@@@Z
void players_update_after_game(const struct simulation_update *)
{
    long transition = NONE;
    long destination = NONE;
    long teleport_player_index = NONE;

    player_training_load_profiles();

    c_player_data_iterator<player_datum> player_iterator;
    player_iterator.begin(player_data);
    while (player_iterator.next()) {
        player_datum *player = player_iterator.get_datum();
        long player_index = player_iterator.iterator.index;

        if (TEST_BIT(player->flags, 11)) {
            s_player_information_definition *respawn_effect =
                (s_player_information_definition *)global_game_globals->player_information.address;
            long effect_index = respawn_effect->respawn_effect.index;
            if (player->unit_index != NONE && effect_index != NONE) {
                long tag_storage;
                c_tag_index *tag = new (&tag_storage) c_tag_index(effect_index);
                effect_new_from_object(*tag, NULL, 0, player->unit_index, 0.0f, 0.0f, NULL, NULL, 1);
            }
            player->flags &= ~FLAG(11);
        }

        if (player->unit_index != NONE && !game_is_predicted()) {
            long game_time = game_time_globals->time;
            bool outside_world = (game_time + (short)player_index) % 16 == 0 || player->illegal_position_ticks != 0;
            player->illegal_position_ticks = (unsigned char)((outside_world && (unsigned char)unit_calculate_is_in_illegal_position(player->unit_index) != 0) ?
                MIN(player->illegal_position_ticks + 1, 254) : 0);
            if (player->illegal_position_ticks > players_after_game_seconds_to_ticks_round(players_half_second))
                unit_kill_direct(player->unit_index);
        } else {
            player->illegal_position_ticks = 0;
        }

        if (TEST_BIT(player->flags, 2) && player->unit_index != NONE) {
            if (!game_is_predicted())
                player->flags &= ~FLAG(2);
        } else if (player->death_timer > 0 && --player->death_timer == 0) {
            simulation_action_game_engine_player_update((short)player_index, 128);
            long perspective = NONE;
            for (;;) {
                if (player_index != NONE)
                    perspective = (long)::director_get_perspective(player_index, perspective);
                if (perspective == NONE)
                    break;
                s_rumble_player *rumble = &g_rumble_globals->players[perspective];
                rumble->transient_rumble.x = 0.0f;
                rumble->transient_rumble.y = 0.0f;
            }
        }

        if (player->melee_recovery_ticks > 0)
            player->melee_recovery_ticks--;
        if (player->melee_remaining_ticks > 0)
            player->melee_remaining_ticks--;
        if (player->grenade_recovery_ticks > 0)
            player->grenade_recovery_ticks--;

        if (!game_is_predicted()) {
            if (player->vehicle_ejection_ticks > 0) {
                player->vehicle_ejection_ticks--;
                if (player->vehicle_ejection_ticks == 0) {
                    player->flags &= ~FLAG(10);
                    simulation_action_game_engine_player_update((short)player_index, 0x1000);
                }
            }
        }

        if (player->unit_index != NONE)
            player_update_invisibility(player_index);

        if (player->unit_index == NONE)
            player->ticks_without_unit++;
        else
            player->ticks_without_unit = 0;

        if (!game_is_predicted()) {
            if (player->unit_index != NONE) {
                if (player->tracking_warning_ticks != 0) {
                    player->flags |= FLAG(14);
                    player->tracking_warning_ticks--;
                } else {
                    player->flags &= ~FLAG(14);
                }
                if (player->locking_warning_ticks != 0) {
                    player->flags |= FLAG(15);
                    player->locking_warning_ticks--;
                } else {
                    player->flags &= ~FLAG(15);
                }
                simulation_action_object_update(player->unit_index, 0x20000000);
            } else {
                player->tracking_warning_ticks = 0;
                player->locking_warning_ticks = 0;
                player->flags &= ~FLAG(14);
                player->flags &= ~FLAG(15);
            }
        }

        if (player->unit_index != NONE) {
            bool no_flag = true;
            long parent_index = object_get_topmost_parent(player->unit_index);
            if ((bool)((object_get(parent_index)->flags >> 18) & 1))
                no_flag = false;
            if (!players_is_zone_transition_active() && no_flag) {
                long new_transition_index = NONE;
                long new_destination_index = NONE;
                if (player_find_zone_transition(player->unit_index, &new_transition_index, &new_destination_index) &&
                    destination == NONE) {
                    if (new_destination_index != NONE) {
                        destination = new_destination_index;
                        transition = NONE;
                        teleport_player_index = player_index;
                    } else if (transition == NONE && new_transition_index != NONE) {
                        transition = new_transition_index;
                        destination = NONE;
                        teleport_player_index = player_index;
                    }
                }
            }
        }

        if (!game_engine_is_player_in_edit_mode(player_index, NULL, NULL))
            player_examine_nearby_objects(player_index);
        player_training_update_player(player_index);
    }

    scenario *scenario_definition = global_scenario;
    if (destination != NONE) {
        s_scenario_zone_set_switch_trigger_volume *zone_transition =
            &((s_scenario_zone_set_switch_trigger_volume *)scenario_definition->zone_set_switch_trigger_volumes.address)[destination];
        players_global_data_get()->new_zone = (signed char)destination;
        players_global_data_get()->switch_count = 0;
        players_global_data_get()->player_index = teleport_player_index;
        players_global_data_get()->zone_set_index = global_zone_set_index;
        player_get_teleport_source_pose(teleport_player_index, &players_global_data_get()->position, &players_global_data_get()->forward);
        main_switch_zone_set(zone_transition->commit_zone_set);
        players_global_data_get()->pending = true;
    } else if (transition != NONE) {
        s_scenario_zone_set_switch_trigger_volume *zone_transition =
            &((s_scenario_zone_set_switch_trigger_volume *)scenario_definition->zone_set_switch_trigger_volumes.address)[transition];
        players_global_data_get()->old_zone = (signed char)transition;
        players_global_data_get()->switch_count = 0;
        players_global_data_get()->player_index = teleport_player_index;
        players_global_data_get()->zone_set_index = global_zone_set_index;
        player_get_teleport_source_pose(teleport_player_index, &players_global_data_get()->position, &players_global_data_get()->forward);

        s_players_globals *switch_globals = players_global_data_get();
        unsigned long bsp_mask = 0;
        long unit_index = player_get(switch_globals->player_index)->unit_index;
        real_point3d center = ((object_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(unit_index)].datum)->bounding_center;
        for (long bsp_index = players_first_active_bsp(); bsp_index != NONE; bsp_index = players_next_active_bsp(bsp_index)) {
            if (bsp3d_test_point(global_bsp3d_get(bsp_index), 0, &center) != NONE)
                bsp_mask |= 1 << bsp_index;
        }
        long zone_set_index = zone_transition->begin_zone_set;
        unsigned long zone_set_mask = zone_set_index != NONE ? ((s_scenario_zone_set *)global_scenario->zone_sets.address)[zone_set_index].bsp_active_mask : 0;
        if ((zone_set_mask & bsp_mask) != 0) {
            main_set_active_zone_set(zone_set_index);
            switch_globals->pending = true;
        }
    }

    s_players_globals *globals = players_global_data_get();
    if (globals->player_index != NONE) {
        short switch_count = globals->switch_count + 1;
        globals->switch_count = switch_count;
        if (switch_count > 12) {
            globals->old_zone = NONE;
            globals->new_zone = NONE;
            globals->player_index = NONE;
            globals->zone_set_index = NONE;
            globals->switch_count = 0;
        }
    }
}
// @rdata 0x820361a8..0x820391c0
static const real_vector3d players_spawn_random_directions[1026] = {
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { -1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f },
    { 0.0665189996f, 0.0f, 0.997784972f },
    { 0.141421005f, 0.0f, 0.989950001f },
    { 0.224859998f, 0.0f, 0.974390984f },
    { 0.316228002f, 0.0f, 0.948683023f },
    { 0.413803011f, 0.0f, 0.910365999f },
    { 0.514496028f, 0.0f, 0.857492983f },
    { 0.613941014f, 0.0f, 0.789352f },
    { 0.707107008f, 0.0f, 0.707107008f },
    { 0.789352f, 0.0f, 0.613941014f },
    { 0.857492983f, 0.0f, 0.514496028f },
    { 0.910365999f, 0.0f, 0.413803011f },
    { 0.948683023f, 0.0f, 0.316228002f },
    { 0.974390984f, 0.0f, 0.224859998f },
    { 0.989950001f, 0.0f, 0.141421005f },
    { 0.997784972f, 0.0f, 0.0665189996f },
    { 0.0f, 0.0665189996f, 0.997784972f },
    { 0.0f, 0.141421005f, 0.989950001f },
    { 0.0f, 0.224859998f, 0.974390984f },
    { 0.0f, 0.316228002f, 0.948683023f },
    { 0.0f, 0.413803011f, 0.910365999f },
    { 0.0f, 0.514496028f, 0.857492983f },
    { 0.0f, 0.613941014f, 0.789352f },
    { 0.0f, 0.707107008f, 0.707107008f },
    { 0.0f, 0.789352f, 0.613941014f },
    { 0.0f, 0.857492983f, 0.514496028f },
    { 0.0f, 0.910365999f, 0.413803011f },
    { 0.0f, 0.948683023f, 0.316228002f },
    { 0.0f, 0.974390984f, 0.224859998f },
    { 0.0f, 0.989950001f, 0.141421005f },
    { 0.0f, 0.997784972f, 0.0665189996f },
    { 0.0710669979f, 0.0710669979f, 0.994937003f },
    { 0.151620001f, 0.0758100003f, 0.985527992f },
    { 0.0758100003f, 0.151620001f, 0.985527992f },
    { 0.241747007f, 0.0805820003f, 0.966987014f },
    { 0.162221f, 0.162221f, 0.973327994f },
    { 0.0805820003f, 0.241747007f, 0.966987014f },
    { 0.340503007f, 0.0851259977f, 0.936381996f },
    { 0.259160995f, 0.172774002f, 0.950254977f },
    { 0.172774002f, 0.259160995f, 0.950254977f },
    { 0.0851259977f, 0.340503007f, 0.936381996f },
    { 0.445434988f, 0.089087002f, 0.890870988f },
    { 0.365148008f, 0.182574004f, 0.912871003f },
    { 0.276172012f, 0.276172012f, 0.920575023f },
    { 0.182574004f, 0.365148008f, 0.912871003f },
    { 0.089087002f, 0.445434988f, 0.890870988f },
    { 0.552344978f, 0.0920569971f, 0.82851702f },
    { 0.476731002f, 0.190693006f, 0.858115971f },
    { 0.388514012f, 0.291386008f, 0.874157012f },
    { 0.291386008f, 0.388514012f, 0.874157012f },
    { 0.190691993f, 0.476731002f, 0.858115971f },
    { 0.0920569971f, 0.552344978f, 0.82851702f },
    { 0.655610025f, 0.0936589986f, 0.749269009f },
    { 0.588347971f, 0.196116f, 0.784465015f },
    { 0.505075991f, 0.303045988f, 0.808121979f },
    { 0.408248007f, 0.408248007f, 0.816497028f },
    { 0.303045988f, 0.505075991f, 0.808121979f },
    { 0.196116f, 0.588347971f, 0.784465015f },
    { 0.0936589986f, 0.655610025f, 0.749269009f },
    { 0.749269009f, 0.0936589986f, 0.655610025f },
    { 0.693103015f, 0.198029995f, 0.693103015f },
    { 0.618852973f, 0.30942601f, 0.721994996f },
    { 0.527046025f, 0.421636999f, 0.737864971f },
    { 0.421636999f, 0.527046025f, 0.737864971f },
    { 0.30942601f, 0.618852973f, 0.721994996f },
    { 0.198028997f, 0.693103015f, 0.693103015f },
    { 0.0936589986f, 0.749269009f, 0.655610025f },
    { 0.82851702f, 0.0920569971f, 0.552344978f },
    { 0.784465015f, 0.196116f, 0.588347971f },
    { 0.721994996f, 0.30942601f, 0.618852973f },
    { 0.639602005f, 0.426400989f, 0.639602005f },
    { 0.539164007f, 0.539164007f, 0.646996975f },
    { 0.426400989f, 0.639602005f, 0.639602005f },
    { 0.30942601f, 0.721994996f, 0.618852973f },
    { 0.196116f, 0.784465015f, 0.588347971f },
    { 0.0920569971f, 0.82851702f, 0.552344978f },
    { 0.890870988f, 0.089087002f, 0.445434988f },
    { 0.858115971f, 0.190693006f, 0.476731002f },
    { 0.808121979f, 0.303045988f, 0.505075991f },
    { 0.737864971f, 0.421636999f, 0.527046025f },
    { 0.646996975f, 0.539164007f, 0.539164007f },
    { 0.539164007f, 0.646996975f, 0.539164007f },
    { 0.421636999f, 0.737864971f, 0.527046025f },
    { 0.303045988f, 0.808121979f, 0.505075991f },
    { 0.190693006f, 0.858115971f, 0.476731002f },
    { 0.089087002f, 0.890870988f, 0.445434988f },
    { 0.936381996f, 0.0851259977f, 0.340503007f },
    { 0.912871003f, 0.182574004f, 0.365148008f },
    { 0.874157012f, 0.291386008f, 0.388514012f },
    { 0.816497028f, 0.408248007f, 0.408248007f },
    { 0.737864971f, 0.527046025f, 0.421636999f },
    { 0.639602005f, 0.639602005f, 0.426400989f },
    { 0.527046025f, 0.737864971f, 0.421636999f },
    { 0.408248007f, 0.816497028f, 0.408248007f },
    { 0.291386008f, 0.874157012f, 0.388514012f },
    { 0.182574004f, 0.912871003f, 0.365148008f },
    { 0.0851259977f, 0.936381996f, 0.340503007f },
    { 0.966988027f, 0.0805820003f, 0.241747007f },
    { 0.950254977f, 0.172774002f, 0.259160995f },
    { 0.920575023f, 0.276172012f, 0.276172012f },
    { 0.874157012f, 0.388514012f, 0.291386008f },
    { 0.808121979f, 0.505075991f, 0.303045988f },
    { 0.721994996f, 0.618852973f, 0.30942601f },
    { 0.618852973f, 0.721994996f, 0.30942601f },
    { 0.505075991f, 0.808121979f, 0.303045988f },
    { 0.388514012f, 0.874157012f, 0.291386008f },
    { 0.276172012f, 0.920575023f, 0.276172012f },
    { 0.172774002f, 0.950254977f, 0.259160995f },
    { 0.0805820003f, 0.966988027f, 0.241747007f },
    { 0.985526979f, 0.0758100003f, 0.151620001f },
    { 0.973329008f, 0.162221f, 0.162221f },
    { 0.950254977f, 0.259160995f, 0.172774002f },
    { 0.912871003f, 0.365148008f, 0.182574004f },
    { 0.858115971f, 0.476731002f, 0.190693006f },
    { 0.784465015f, 0.588347971f, 0.196116f },
    { 0.693103015f, 0.693103015f, 0.198029995f },
    { 0.588347971f, 0.784465015f, 0.196116f },
    { 0.476731002f, 0.858115971f, 0.190693006f },
    { 0.365148008f, 0.912871003f, 0.182574004f },
    { 0.259160995f, 0.950254977f, 0.172774002f },
    { 0.162221f, 0.973329008f, 0.162221f },
    { 0.0758100003f, 0.985526979f, 0.151620001f },
    { 0.994937003f, 0.0710669979f, 0.0710669979f },
    { 0.985526979f, 0.151620001f, 0.0758100003f },
    { 0.966988027f, 0.241747007f, 0.0805820003f },
    { 0.936381996f, 0.340503007f, 0.0851259977f },
    { 0.890870988f, 0.445434988f, 0.089087002f },
    { 0.82851702f, 0.552344978f, 0.0920569971f },
    { 0.749269009f, 0.655610025f, 0.0936589986f },
    { 0.655610025f, 0.749269009f, 0.0936589986f },
    { 0.552344978f, 0.82851702f, 0.0920569971f },
    { 0.445434988f, 0.890870988f, 0.089087002f },
    { 0.340503007f, 0.936381996f, 0.0851259977f },
    { 0.241747007f, 0.966988027f, 0.0805820003f },
    { 0.151620001f, 0.985526979f, 0.0758100003f },
    { 0.0710669979f, 0.994937003f, 0.0710669979f },
    { 0.0665189996f, 0.997784972f, 0.0f },
    { 0.141421005f, 0.989950001f, 0.0f },
    { 0.224859998f, 0.974390984f, 0.0f },
    { 0.316228002f, 0.948683023f, 0.0f },
    { 0.413803011f, 0.910365999f, 0.0f },
    { 0.514496028f, 0.857492983f, 0.0f },
    { 0.613941014f, 0.789352f, 0.0f },
    { 0.707107008f, 0.707107008f, 0.0f },
    { 0.789352f, 0.613941014f, 0.0f },
    { 0.857492983f, 0.514496028f, 0.0f },
    { 0.910365999f, 0.413803011f, 0.0f },
    { 0.948683023f, 0.316228002f, 0.0f },
    { 0.974390984f, 0.224859998f, 0.0f },
    { 0.989950001f, 0.141421005f, 0.0f },
    { 0.997784972f, 0.0665189996f, 0.0f },
    { 0.0f, -0.0665189996f, 0.997784972f },
    { 0.0f, -0.141421005f, 0.989950001f },
    { 0.0f, -0.224859998f, 0.974390984f },
    { 0.0f, -0.316228002f, 0.948683023f },
    { 0.0f, -0.413803011f, 0.910365999f },
    { 0.0f, -0.514496028f, 0.857492983f },
    { 0.0f, -0.613941014f, 0.789352f },
    { 0.0f, -0.707107008f, 0.707107008f },
    { 0.0f, -0.789352f, 0.613941014f },
    { 0.0f, -0.857492983f, 0.514496028f },
    { 0.0f, -0.910365999f, 0.413803011f },
    { 0.0f, -0.948683023f, 0.316228002f },
    { 0.0f, -0.974390984f, 0.224859998f },
    { 0.0f, -0.989950001f, 0.141421005f },
    { 0.0f, -0.997784972f, 0.0665189996f },
    { 0.0710669979f, -0.0710669979f, 0.994937003f },
    { 0.0758100003f, -0.151620001f, 0.985527992f },
    { 0.151620001f, -0.0758100003f, 0.985527992f },
    { 0.0805820003f, -0.241747007f, 0.966987014f },
    { 0.162221f, -0.162221f, 0.973327994f },
    { 0.241747007f, -0.0805820003f, 0.966987014f },
    { 0.0851259977f, -0.340503007f, 0.936381996f },
    { 0.172774002f, -0.259160995f, 0.950254977f },
    { 0.259160995f, -0.172774002f, 0.950254977f },
    { 0.340503007f, -0.0851259977f, 0.936381996f },
    { 0.089087002f, -0.445434988f, 0.890870988f },
    { 0.182574004f, -0.365148008f, 0.912871003f },
    { 0.276172012f, -0.276172012f, 0.920575023f },
    { 0.365148008f, -0.182574004f, 0.912871003f },
    { 0.445434988f, -0.089087002f, 0.890870988f },
    { 0.0920569971f, -0.552344978f, 0.82851702f },
    { 0.190693006f, -0.476731002f, 0.858115971f },
    { 0.291386008f, -0.388514012f, 0.874157012f },
    { 0.388514012f, -0.291386008f, 0.874157012f },
    { 0.476731002f, -0.190691993f, 0.858115971f },
    { 0.552344978f, -0.0920569971f, 0.82851702f },
    { 0.0936589986f, -0.655610025f, 0.749269009f },
    { 0.196116f, -0.588347971f, 0.784465015f },
    { 0.303045988f, -0.505075991f, 0.808121979f },
    { 0.408248007f, -0.408248007f, 0.816497028f },
    { 0.505075991f, -0.303045988f, 0.808121979f },
    { 0.588347971f, -0.196116f, 0.784465015f },
    { 0.655610025f, -0.0936589986f, 0.749269009f },
    { 0.0936589986f, -0.749269009f, 0.655610025f },
    { 0.198029995f, -0.693103015f, 0.693103015f },
    { 0.30942601f, -0.618852973f, 0.721994996f },
    { 0.421636999f, -0.527046025f, 0.737864971f },
    { 0.527046025f, -0.421636999f, 0.737864971f },
    { 0.618852973f, -0.30942601f, 0.721994996f },
    { 0.693103015f, -0.198028997f, 0.693103015f },
    { 0.749269009f, -0.0936589986f, 0.655610025f },
    { 0.0920569971f, -0.82851702f, 0.552344978f },
    { 0.196116f, -0.784465015f, 0.588347971f },
    { 0.30942601f, -0.721994996f, 0.618852973f },
    { 0.426400989f, -0.639602005f, 0.639602005f },
    { 0.539164007f, -0.539164007f, 0.646996975f },
    { 0.639602005f, -0.426400989f, 0.639602005f },
    { 0.721994996f, -0.30942601f, 0.618852973f },
    { 0.784465015f, -0.196116f, 0.588347971f },
    { 0.82851702f, -0.0920569971f, 0.552344978f },
    { 0.089087002f, -0.890870988f, 0.445434988f },
    { 0.190693006f, -0.858115971f, 0.476731002f },
    { 0.303045988f, -0.808121979f, 0.505075991f },
    { 0.421636999f, -0.737864971f, 0.527046025f },
    { 0.539164007f, -0.646996975f, 0.539164007f },
    { 0.646996975f, -0.539164007f, 0.539164007f },
    { 0.737864971f, -0.421636999f, 0.527046025f },
    { 0.808121979f, -0.303045988f, 0.505075991f },
    { 0.858115971f, -0.190693006f, 0.476731002f },
    { 0.890870988f, -0.089087002f, 0.445434988f },
    { 0.0851259977f, -0.936381996f, 0.340503007f },
    { 0.182574004f, -0.912871003f, 0.365148008f },
    { 0.291386008f, -0.874157012f, 0.388514012f },
    { 0.408248007f, -0.816497028f, 0.408248007f },
    { 0.527046025f, -0.737864971f, 0.421636999f },
    { 0.639602005f, -0.639602005f, 0.426400989f },
    { 0.737864971f, -0.527046025f, 0.421636999f },
    { 0.816497028f, -0.408248007f, 0.408248007f },
    { 0.874157012f, -0.291386008f, 0.388514012f },
    { 0.912871003f, -0.182574004f, 0.365148008f },
    { 0.936381996f, -0.0851259977f, 0.340503007f },
    { 0.0805820003f, -0.966988027f, 0.241747007f },
    { 0.172774002f, -0.950254977f, 0.259160995f },
    { 0.276172012f, -0.920575023f, 0.276172012f },
    { 0.388514012f, -0.874157012f, 0.291386008f },
    { 0.505075991f, -0.808121979f, 0.303045988f },
    { 0.618852973f, -0.721994996f, 0.30942601f },
    { 0.721994996f, -0.618852973f, 0.30942601f },
    { 0.808121979f, -0.505075991f, 0.303045988f },
    { 0.874157012f, -0.388514012f, 0.291386008f },
    { 0.920575023f, -0.276172012f, 0.276172012f },
    { 0.950254977f, -0.172774002f, 0.259160995f },
    { 0.966988027f, -0.0805820003f, 0.241747007f },
    { 0.0758100003f, -0.985526979f, 0.151620001f },
    { 0.162221f, -0.973329008f, 0.162221f },
    { 0.259160995f, -0.950254977f, 0.172774002f },
    { 0.365148008f, -0.912871003f, 0.182574004f },
    { 0.476731002f, -0.858115971f, 0.190693006f },
    { 0.588347971f, -0.784465015f, 0.196116f },
    { 0.693103015f, -0.693103015f, 0.198029995f },
    { 0.784465015f, -0.588347971f, 0.196116f },
    { 0.858115971f, -0.476731002f, 0.190693006f },
    { 0.912871003f, -0.365148008f, 0.182574004f },
    { 0.950254977f, -0.259160995f, 0.172774002f },
    { 0.973329008f, -0.162221f, 0.162221f },
    { 0.985526979f, -0.0758100003f, 0.151620001f },
    { 0.0710669979f, -0.994937003f, 0.0710669979f },
    { 0.151620001f, -0.985526979f, 0.0758100003f },
    { 0.241747007f, -0.966988027f, 0.0805820003f },
    { 0.340503007f, -0.936381996f, 0.0851259977f },
    { 0.445434988f, -0.890870988f, 0.089087002f },
    { 0.552344978f, -0.82851702f, 0.0920569971f },
    { 0.655610025f, -0.749269009f, 0.0936589986f },
    { 0.749269009f, -0.655610025f, 0.0936589986f },
    { 0.82851702f, -0.552344978f, 0.0920569971f },
    { 0.890870988f, -0.445434988f, 0.089087002f },
    { 0.936381996f, -0.340503007f, 0.0851259977f },
    { 0.966988027f, -0.241747007f, 0.0805820003f },
    { 0.985526979f, -0.151620001f, 0.0758100003f },
    { 0.994937003f, -0.0710669979f, 0.0710669979f },
    { 0.997784972f, -0.0665189996f, 0.0f },
    { 0.989950001f, -0.141421005f, 0.0f },
    { 0.974390984f, -0.224859998f, 0.0f },
    { 0.948683023f, -0.316228002f, 0.0f },
    { 0.910365999f, -0.413803011f, 0.0f },
    { 0.857492983f, -0.514496028f, 0.0f },
    { 0.789352f, -0.613941014f, 0.0f },
    { 0.707107008f, -0.707107008f, 0.0f },
    { 0.613941014f, -0.789352f, 0.0f },
    { 0.514496028f, -0.857492983f, 0.0f },
    { 0.413803011f, -0.910365999f, 0.0f },
    { 0.316228002f, -0.948683023f, 0.0f },
    { 0.224859998f, -0.974390984f, 0.0f },
    { 0.141421005f, -0.989950001f, 0.0f },
    { 0.0665189996f, -0.997784972f, 0.0f },
    { -0.0665189996f, 0.0f, 0.997784972f },
    { -0.141421005f, 0.0f, 0.989950001f },
    { -0.224859998f, 0.0f, 0.974390984f },
    { -0.316228002f, 0.0f, 0.948683023f },
    { -0.413803011f, 0.0f, 0.910365999f },
    { -0.514496028f, 0.0f, 0.857492983f },
    { -0.613941014f, 0.0f, 0.789352f },
    { -0.707107008f, 0.0f, 0.707107008f },
    { -0.789352f, 0.0f, 0.613941014f },
    { -0.857492983f, 0.0f, 0.514496028f },
    { -0.910365999f, 0.0f, 0.413803011f },
    { -0.948683023f, 0.0f, 0.316228002f },
    { -0.974390984f, 0.0f, 0.224859998f },
    { -0.989950001f, 0.0f, 0.141421005f },
    { -0.997784972f, 0.0f, 0.0665189996f },
    { -0.0710669979f, -0.0710669979f, 0.994937003f },
    { -0.151620001f, -0.0758100003f, 0.985527992f },
    { -0.0758100003f, -0.151620001f, 0.985527992f },
    { -0.241747007f, -0.0805820003f, 0.966987014f },
    { -0.162221f, -0.162221f, 0.973327994f },
    { -0.0805820003f, -0.241747007f, 0.966987014f },
    { -0.340503007f, -0.0851259977f, 0.936381996f },
    { -0.259160995f, -0.172774002f, 0.950254977f },
    { -0.172774002f, -0.259160995f, 0.950254977f },
    { -0.0851259977f, -0.340503007f, 0.936381996f },
    { -0.445434988f, -0.089087002f, 0.890870988f },
    { -0.365148008f, -0.182574004f, 0.912871003f },
    { -0.276172012f, -0.276172012f, 0.920575023f },
    { -0.182574004f, -0.365148008f, 0.912871003f },
    { -0.089087002f, -0.445434988f, 0.890870988f },
    { -0.552344978f, -0.0920569971f, 0.82851702f },
    { -0.476731002f, -0.190693006f, 0.858115971f },
    { -0.388514012f, -0.291386008f, 0.874157012f },
    { -0.291386008f, -0.388514012f, 0.874157012f },
    { -0.190691993f, -0.476731002f, 0.858115971f },
    { -0.0920569971f, -0.552344978f, 0.82851702f },
    { -0.655610025f, -0.0936589986f, 0.749269009f },
    { -0.588347971f, -0.196116f, 0.784465015f },
    { -0.505075991f, -0.303045988f, 0.808121979f },
    { -0.408248007f, -0.408248007f, 0.816497028f },
    { -0.303045988f, -0.505075991f, 0.808121979f },
    { -0.196116f, -0.588347971f, 0.784465015f },
    { -0.0936589986f, -0.655610025f, 0.749269009f },
    { -0.749269009f, -0.0936589986f, 0.655610025f },
    { -0.693103015f, -0.198029995f, 0.693103015f },
    { -0.618852973f, -0.30942601f, 0.721994996f },
    { -0.527046025f, -0.421636999f, 0.737864971f },
    { -0.421636999f, -0.527046025f, 0.737864971f },
    { -0.30942601f, -0.618852973f, 0.721994996f },
    { -0.198028997f, -0.693103015f, 0.693103015f },
    { -0.0936589986f, -0.749269009f, 0.655610025f },
    { -0.82851702f, -0.0920569971f, 0.552344978f },
    { -0.784465015f, -0.196116f, 0.588347971f },
    { -0.721994996f, -0.30942601f, 0.618852973f },
    { -0.639602005f, -0.426400989f, 0.639602005f },
    { -0.539164007f, -0.539164007f, 0.646996975f },
    { -0.426400989f, -0.639602005f, 0.639602005f },
    { -0.30942601f, -0.721994996f, 0.618852973f },
    { -0.196116f, -0.784465015f, 0.588347971f },
    { -0.0920569971f, -0.82851702f, 0.552344978f },
    { -0.890870988f, -0.089087002f, 0.445434988f },
    { -0.858115971f, -0.190693006f, 0.476731002f },
    { -0.808121979f, -0.303045988f, 0.505075991f },
    { -0.737864971f, -0.421636999f, 0.527046025f },
    { -0.646996975f, -0.539164007f, 0.539164007f },
    { -0.539164007f, -0.646996975f, 0.539164007f },
    { -0.421636999f, -0.737864971f, 0.527046025f },
    { -0.303045988f, -0.808121979f, 0.505075991f },
    { -0.190693006f, -0.858115971f, 0.476731002f },
    { -0.089087002f, -0.890870988f, 0.445434988f },
    { -0.936381996f, -0.0851259977f, 0.340503007f },
    { -0.912871003f, -0.182574004f, 0.365148008f },
    { -0.874157012f, -0.291386008f, 0.388514012f },
    { -0.816497028f, -0.408248007f, 0.408248007f },
    { -0.737864971f, -0.527046025f, 0.421636999f },
    { -0.639602005f, -0.639602005f, 0.426400989f },
    { -0.527046025f, -0.737864971f, 0.421636999f },
    { -0.408248007f, -0.816497028f, 0.408248007f },
    { -0.291386008f, -0.874157012f, 0.388514012f },
    { -0.182574004f, -0.912871003f, 0.365148008f },
    { -0.0851259977f, -0.936381996f, 0.340503007f },
    { -0.966988027f, -0.0805820003f, 0.241747007f },
    { -0.950254977f, -0.172774002f, 0.259160995f },
    { -0.920575023f, -0.276172012f, 0.276172012f },
    { -0.874157012f, -0.388514012f, 0.291386008f },
    { -0.808121979f, -0.505075991f, 0.303045988f },
    { -0.721994996f, -0.618852973f, 0.30942601f },
    { -0.618852973f, -0.721994996f, 0.30942601f },
    { -0.505075991f, -0.808121979f, 0.303045988f },
    { -0.388514012f, -0.874157012f, 0.291386008f },
    { -0.276172012f, -0.920575023f, 0.276172012f },
    { -0.172774002f, -0.950254977f, 0.259160995f },
    { -0.0805820003f, -0.966988027f, 0.241747007f },
    { -0.985526979f, -0.0758100003f, 0.151620001f },
    { -0.973329008f, -0.162221f, 0.162221f },
    { -0.950254977f, -0.259160995f, 0.172774002f },
    { -0.912871003f, -0.365148008f, 0.182574004f },
    { -0.858115971f, -0.476731002f, 0.190693006f },
    { -0.784465015f, -0.588347971f, 0.196116f },
    { -0.693103015f, -0.693103015f, 0.198029995f },
    { -0.588347971f, -0.784465015f, 0.196116f },
    { -0.476731002f, -0.858115971f, 0.190693006f },
    { -0.365148008f, -0.912871003f, 0.182574004f },
    { -0.259160995f, -0.950254977f, 0.172774002f },
    { -0.162221f, -0.973329008f, 0.162221f },
    { -0.0758100003f, -0.985526979f, 0.151620001f },
    { -0.994937003f, -0.0710669979f, 0.0710669979f },
    { -0.985526979f, -0.151620001f, 0.0758100003f },
    { -0.966988027f, -0.241747007f, 0.0805820003f },
    { -0.936381996f, -0.340503007f, 0.0851259977f },
    { -0.890870988f, -0.445434988f, 0.089087002f },
    { -0.82851702f, -0.552344978f, 0.0920569971f },
    { -0.749269009f, -0.655610025f, 0.0936589986f },
    { -0.655610025f, -0.749269009f, 0.0936589986f },
    { -0.552344978f, -0.82851702f, 0.0920569971f },
    { -0.445434988f, -0.890870988f, 0.089087002f },
    { -0.340503007f, -0.936381996f, 0.0851259977f },
    { -0.241747007f, -0.966988027f, 0.0805820003f },
    { -0.151620001f, -0.985526979f, 0.0758100003f },
    { -0.0710669979f, -0.994937003f, 0.0710669979f },
    { -0.0665189996f, -0.997784972f, 0.0f },
    { -0.141421005f, -0.989950001f, 0.0f },
    { -0.224859998f, -0.974390984f, 0.0f },
    { -0.316228002f, -0.948683023f, 0.0f },
    { -0.413803011f, -0.910365999f, 0.0f },
    { -0.514496028f, -0.857492983f, 0.0f },
    { -0.613941014f, -0.789352f, 0.0f },
    { -0.707107008f, -0.707107008f, 0.0f },
    { -0.789352f, -0.613941014f, 0.0f },
    { -0.857492983f, -0.514496028f, 0.0f },
    { -0.910365999f, -0.413803011f, 0.0f },
    { -0.948683023f, -0.316228002f, 0.0f },
    { -0.974390984f, -0.224859998f, 0.0f },
    { -0.989950001f, -0.141421005f, 0.0f },
    { -0.997784972f, -0.0665189996f, 0.0f },
    { -0.0710669979f, 0.0710669979f, 0.994937003f },
    { -0.0758100003f, 0.151620001f, 0.985527992f },
    { -0.151620001f, 0.0758100003f, 0.985527992f },
    { -0.0805820003f, 0.241747007f, 0.966987014f },
    { -0.162221f, 0.162221f, 0.973327994f },
    { -0.241747007f, 0.0805820003f, 0.966987014f },
    { -0.0851259977f, 0.340503007f, 0.936381996f },
    { -0.172774002f, 0.259160995f, 0.950254977f },
    { -0.259160995f, 0.172774002f, 0.950254977f },
    { -0.340503007f, 0.0851259977f, 0.936381996f },
    { -0.089087002f, 0.445434988f, 0.890870988f },
    { -0.182574004f, 0.365148008f, 0.912871003f },
    { -0.276172012f, 0.276172012f, 0.920575023f },
    { -0.365148008f, 0.182574004f, 0.912871003f },
    { -0.445434988f, 0.089087002f, 0.890870988f },
    { -0.0920569971f, 0.552344978f, 0.82851702f },
    { -0.190693006f, 0.476731002f, 0.858115971f },
    { -0.291386008f, 0.388514012f, 0.874157012f },
    { -0.388514012f, 0.291386008f, 0.874157012f },
    { -0.476731002f, 0.190691993f, 0.858115971f },
    { -0.552344978f, 0.0920569971f, 0.82851702f },
    { -0.0936589986f, 0.655610025f, 0.749269009f },
    { -0.196116f, 0.588347971f, 0.784465015f },
    { -0.303045988f, 0.505075991f, 0.808121979f },
    { -0.408248007f, 0.408248007f, 0.816497028f },
    { -0.505075991f, 0.303045988f, 0.808121979f },
    { -0.588347971f, 0.196116f, 0.784465015f },
    { -0.655610025f, 0.0936589986f, 0.749269009f },
    { -0.0936589986f, 0.749269009f, 0.655610025f },
    { -0.198029995f, 0.693103015f, 0.693103015f },
    { -0.30942601f, 0.618852973f, 0.721994996f },
    { -0.421636999f, 0.527046025f, 0.737864971f },
    { -0.527046025f, 0.421636999f, 0.737864971f },
    { -0.618852973f, 0.30942601f, 0.721994996f },
    { -0.693103015f, 0.198028997f, 0.693103015f },
    { -0.749269009f, 0.0936589986f, 0.655610025f },
    { -0.0920569971f, 0.82851702f, 0.552344978f },
    { -0.196116f, 0.784465015f, 0.588347971f },
    { -0.30942601f, 0.721994996f, 0.618852973f },
    { -0.426400989f, 0.639602005f, 0.639602005f },
    { -0.539164007f, 0.539164007f, 0.646996975f },
    { -0.639602005f, 0.426400989f, 0.639602005f },
    { -0.721994996f, 0.30942601f, 0.618852973f },
    { -0.784465015f, 0.196116f, 0.588347971f },
    { -0.82851702f, 0.0920569971f, 0.552344978f },
    { -0.089087002f, 0.890870988f, 0.445434988f },
    { -0.190693006f, 0.858115971f, 0.476731002f },
    { -0.303045988f, 0.808121979f, 0.505075991f },
    { -0.421636999f, 0.737864971f, 0.527046025f },
    { -0.539164007f, 0.646996975f, 0.539164007f },
    { -0.646996975f, 0.539164007f, 0.539164007f },
    { -0.737864971f, 0.421636999f, 0.527046025f },
    { -0.808121979f, 0.303045988f, 0.505075991f },
    { -0.858115971f, 0.190693006f, 0.476731002f },
    { -0.890870988f, 0.089087002f, 0.445434988f },
    { -0.0851259977f, 0.936381996f, 0.340503007f },
    { -0.182574004f, 0.912871003f, 0.365148008f },
    { -0.291386008f, 0.874157012f, 0.388514012f },
    { -0.408248007f, 0.816497028f, 0.408248007f },
    { -0.527046025f, 0.737864971f, 0.421636999f },
    { -0.639602005f, 0.639602005f, 0.426400989f },
    { -0.737864971f, 0.527046025f, 0.421636999f },
    { -0.816497028f, 0.408248007f, 0.408248007f },
    { -0.874157012f, 0.291386008f, 0.388514012f },
    { -0.912871003f, 0.182574004f, 0.365148008f },
    { -0.936381996f, 0.0851259977f, 0.340503007f },
    { -0.0805820003f, 0.966988027f, 0.241747007f },
    { -0.172774002f, 0.950254977f, 0.259160995f },
    { -0.276172012f, 0.920575023f, 0.276172012f },
    { -0.388514012f, 0.874157012f, 0.291386008f },
    { -0.505075991f, 0.808121979f, 0.303045988f },
    { -0.618852973f, 0.721994996f, 0.30942601f },
    { -0.721994996f, 0.618852973f, 0.30942601f },
    { -0.808121979f, 0.505075991f, 0.303045988f },
    { -0.874157012f, 0.388514012f, 0.291386008f },
    { -0.920575023f, 0.276172012f, 0.276172012f },
    { -0.950254977f, 0.172774002f, 0.259160995f },
    { -0.966988027f, 0.0805820003f, 0.241747007f },
    { -0.0758100003f, 0.985526979f, 0.151620001f },
    { -0.162221f, 0.973329008f, 0.162221f },
    { -0.259160995f, 0.950254977f, 0.172774002f },
    { -0.365148008f, 0.912871003f, 0.182574004f },
    { -0.476731002f, 0.858115971f, 0.190693006f },
    { -0.588347971f, 0.784465015f, 0.196116f },
    { -0.693103015f, 0.693103015f, 0.198029995f },
    { -0.784465015f, 0.588347971f, 0.196116f },
    { -0.858115971f, 0.476731002f, 0.190693006f },
    { -0.912871003f, 0.365148008f, 0.182574004f },
    { -0.950254977f, 0.259160995f, 0.172774002f },
    { -0.973329008f, 0.162221f, 0.162221f },
    { -0.985526979f, 0.0758100003f, 0.151620001f },
    { -0.0710669979f, 0.994937003f, 0.0710669979f },
    { -0.151620001f, 0.985526979f, 0.0758100003f },
    { -0.241747007f, 0.966988027f, 0.0805820003f },
    { -0.340503007f, 0.936381996f, 0.0851259977f },
    { -0.445434988f, 0.890870988f, 0.089087002f },
    { -0.552344978f, 0.82851702f, 0.0920569971f },
    { -0.655610025f, 0.749269009f, 0.0936589986f },
    { -0.749269009f, 0.655610025f, 0.0936589986f },
    { -0.82851702f, 0.552344978f, 0.0920569971f },
    { -0.890870988f, 0.445434988f, 0.089087002f },
    { -0.936381996f, 0.340503007f, 0.0851259977f },
    { -0.966988027f, 0.241747007f, 0.0805820003f },
    { -0.985526979f, 0.151620001f, 0.0758100003f },
    { -0.994937003f, 0.0710669979f, 0.0710669979f },
    { -0.0665189996f, 0.997784972f, 0.0f },
    { -0.141421005f, 0.989950001f, 0.0f },
    { -0.224859998f, 0.974390984f, 0.0f },
    { -0.316228002f, 0.948683023f, 0.0f },
    { -0.413803011f, 0.910365999f, 0.0f },
    { -0.514496028f, 0.857492983f, 0.0f },
    { -0.613941014f, 0.789352f, 0.0f },
    { -0.707107008f, 0.707107008f, 0.0f },
    { -0.789352f, 0.613941014f, 0.0f },
    { -0.857492983f, 0.514496028f, 0.0f },
    { -0.910365999f, 0.413803011f, 0.0f },
    { -0.948683023f, 0.316228002f, 0.0f },
    { -0.974390984f, 0.224859998f, 0.0f },
    { -0.989950001f, 0.141421005f, 0.0f },
    { -0.997784972f, 0.0665189996f, 0.0f },
    { -0.997784972f, 0.0f, -0.0665189996f },
    { -0.989950001f, 0.0f, -0.141421005f },
    { -0.974390984f, 0.0f, -0.224859998f },
    { -0.948683023f, 0.0f, -0.316228002f },
    { -0.910365999f, 0.0f, -0.413803011f },
    { -0.857492983f, 0.0f, -0.514496028f },
    { -0.789352f, 0.0f, -0.613941014f },
    { -0.707107008f, 0.0f, -0.707107008f },
    { -0.613941014f, 0.0f, -0.789352f },
    { -0.514496028f, 0.0f, -0.857492983f },
    { -0.413803011f, 0.0f, -0.910365999f },
    { -0.316228002f, 0.0f, -0.948683023f },
    { -0.224859998f, 0.0f, -0.974390984f },
    { -0.141421005f, 0.0f, -0.989950001f },
    { -0.0665189996f, 0.0f, -0.997784972f },
    { 0.0f, 0.997784972f, -0.0665189996f },
    { 0.0f, 0.989950001f, -0.141421005f },
    { 0.0f, 0.974390984f, -0.224859998f },
    { 0.0f, 0.948683023f, -0.316228002f },
    { 0.0f, 0.910365999f, -0.413803011f },
    { 0.0f, 0.857492983f, -0.514496028f },
    { 0.0f, 0.789352f, -0.613941014f },
    { 0.0f, 0.707107008f, -0.707107008f },
    { 0.0f, 0.613941014f, -0.789352f },
    { 0.0f, 0.514496028f, -0.857492983f },
    { 0.0f, 0.413803011f, -0.910365999f },
    { 0.0f, 0.316228002f, -0.948683023f },
    { 0.0f, 0.224859998f, -0.974390984f },
    { 0.0f, 0.141421005f, -0.989950001f },
    { 0.0f, 0.0665189996f, -0.997784972f },
    { -0.0710669979f, 0.0710669979f, -0.994937003f },
    { -0.151620001f, 0.0758100003f, -0.985527992f },
    { -0.0758100003f, 0.151620001f, -0.985527992f },
    { -0.241747007f, 0.0805820003f, -0.966987014f },
    { -0.162221f, 0.162221f, -0.973327994f },
    { -0.0805820003f, 0.241747007f, -0.966987014f },
    { -0.340503007f, 0.0851259977f, -0.936381996f },
    { -0.259160995f, 0.172774002f, -0.950254977f },
    { -0.172774002f, 0.259160995f, -0.950254977f },
    { -0.0851259977f, 0.340503007f, -0.936381996f },
    { -0.445434988f, 0.089087002f, -0.890870988f },
    { -0.365148008f, 0.182574004f, -0.912871003f },
    { -0.276172012f, 0.276172012f, -0.920575023f },
    { -0.182574004f, 0.365148008f, -0.912871003f },
    { -0.089087002f, 0.445434988f, -0.890870988f },
    { -0.552344978f, 0.0920569971f, -0.82851702f },
    { -0.476731002f, 0.190693006f, -0.858115971f },
    { -0.388514012f, 0.291386008f, -0.874157012f },
    { -0.291386008f, 0.388514012f, -0.874157012f },
    { -0.190691993f, 0.476731002f, -0.858115971f },
    { -0.0920569971f, 0.552344978f, -0.82851702f },
    { -0.655610025f, 0.0936589986f, -0.749269009f },
    { -0.588347971f, 0.196116f, -0.784465015f },
    { -0.505075991f, 0.303045988f, -0.808121979f },
    { -0.408248007f, 0.408248007f, -0.816497028f },
    { -0.303045988f, 0.505075991f, -0.808121979f },
    { -0.196116f, 0.588347971f, -0.784465015f },
    { -0.0936589986f, 0.655610025f, -0.749269009f },
    { -0.749269009f, 0.0936589986f, -0.655610025f },
    { -0.693103015f, 0.198029995f, -0.693103015f },
    { -0.618852973f, 0.30942601f, -0.721994996f },
    { -0.527046025f, 0.421636999f, -0.737864971f },
    { -0.421636999f, 0.527046025f, -0.737864971f },
    { -0.30942601f, 0.618852973f, -0.721994996f },
    { -0.198028997f, 0.693103015f, -0.693103015f },
    { -0.0936589986f, 0.749269009f, -0.655610025f },
    { -0.82851702f, 0.0920569971f, -0.552344978f },
    { -0.784465015f, 0.196116f, -0.588347971f },
    { -0.721994996f, 0.30942601f, -0.618852973f },
    { -0.639602005f, 0.426400989f, -0.639602005f },
    { -0.539164007f, 0.539164007f, -0.646996975f },
    { -0.426400989f, 0.639602005f, -0.639602005f },
    { -0.30942601f, 0.721994996f, -0.618852973f },
    { -0.196116f, 0.784465015f, -0.588347971f },
    { -0.0920569971f, 0.82851702f, -0.552344978f },
    { -0.890870988f, 0.089087002f, -0.445434988f },
    { -0.858115971f, 0.190693006f, -0.476731002f },
    { -0.808121979f, 0.303045988f, -0.505075991f },
    { -0.737864971f, 0.421636999f, -0.527046025f },
    { -0.646996975f, 0.539164007f, -0.539164007f },
    { -0.539164007f, 0.646996975f, -0.539164007f },
    { -0.421636999f, 0.737864971f, -0.527046025f },
    { -0.303045988f, 0.808121979f, -0.505075991f },
    { -0.190693006f, 0.858115971f, -0.476731002f },
    { -0.089087002f, 0.890870988f, -0.445434988f },
    { -0.936381996f, 0.0851259977f, -0.340503007f },
    { -0.912871003f, 0.182574004f, -0.365148008f },
    { -0.874157012f, 0.291386008f, -0.388514012f },
    { -0.816497028f, 0.408248007f, -0.408248007f },
    { -0.737864971f, 0.527046025f, -0.421636999f },
    { -0.639602005f, 0.639602005f, -0.426400989f },
    { -0.527046025f, 0.737864971f, -0.421636999f },
    { -0.408248007f, 0.816497028f, -0.408248007f },
    { -0.291386008f, 0.874157012f, -0.388514012f },
    { -0.182574004f, 0.912871003f, -0.365148008f },
    { -0.0851259977f, 0.936381996f, -0.340503007f },
    { -0.966988027f, 0.0805820003f, -0.241747007f },
    { -0.950254977f, 0.172774002f, -0.259160995f },
    { -0.920575023f, 0.276172012f, -0.276172012f },
    { -0.874157012f, 0.388514012f, -0.291386008f },
    { -0.808121979f, 0.505075991f, -0.303045988f },
    { -0.721994996f, 0.618852973f, -0.30942601f },
    { -0.618852973f, 0.721994996f, -0.30942601f },
    { -0.505075991f, 0.808121979f, -0.303045988f },
    { -0.388514012f, 0.874157012f, -0.291386008f },
    { -0.276172012f, 0.920575023f, -0.276172012f },
    { -0.172774002f, 0.950254977f, -0.259160995f },
    { -0.0805820003f, 0.966988027f, -0.241747007f },
    { -0.985526979f, 0.0758100003f, -0.151620001f },
    { -0.973329008f, 0.162221f, -0.162221f },
    { -0.950254977f, 0.259160995f, -0.172774002f },
    { -0.912871003f, 0.365148008f, -0.182574004f },
    { -0.858115971f, 0.476731002f, -0.190693006f },
    { -0.784465015f, 0.588347971f, -0.196116f },
    { -0.693103015f, 0.693103015f, -0.198029995f },
    { -0.588347971f, 0.784465015f, -0.196116f },
    { -0.476731002f, 0.858115971f, -0.190693006f },
    { -0.365148008f, 0.912871003f, -0.182574004f },
    { -0.259160995f, 0.950254977f, -0.172774002f },
    { -0.162221f, 0.973329008f, -0.162221f },
    { -0.0758100003f, 0.985526979f, -0.151620001f },
    { -0.994937003f, 0.0710669979f, -0.0710669979f },
    { -0.985526979f, 0.151620001f, -0.0758100003f },
    { -0.966988027f, 0.241747007f, -0.0805820003f },
    { -0.936381996f, 0.340503007f, -0.0851259977f },
    { -0.890870988f, 0.445434988f, -0.089087002f },
    { -0.82851702f, 0.552344978f, -0.0920569971f },
    { -0.749269009f, 0.655610025f, -0.0936589986f },
    { -0.655610025f, 0.749269009f, -0.0936589986f },
    { -0.552344978f, 0.82851702f, -0.0920569971f },
    { -0.445434988f, 0.890870988f, -0.089087002f },
    { -0.340503007f, 0.936381996f, -0.0851259977f },
    { -0.241747007f, 0.966988027f, -0.0805820003f },
    { -0.151620001f, 0.985526979f, -0.0758100003f },
    { -0.0710669979f, 0.994937003f, -0.0710669979f },
    { 0.0f, -0.997784972f, -0.0665189996f },
    { 0.0f, -0.989950001f, -0.141421005f },
    { 0.0f, -0.974390984f, -0.224859998f },
    { 0.0f, -0.948683023f, -0.316228002f },
    { 0.0f, -0.910365999f, -0.413803011f },
    { 0.0f, -0.857492983f, -0.514496028f },
    { 0.0f, -0.789352f, -0.613941014f },
    { 0.0f, -0.707107008f, -0.707107008f },
    { 0.0f, -0.613941014f, -0.789352f },
    { 0.0f, -0.514496028f, -0.857492983f },
    { 0.0f, -0.413803011f, -0.910365999f },
    { 0.0f, -0.316228002f, -0.948683023f },
    { 0.0f, -0.224859998f, -0.974390984f },
    { 0.0f, -0.141421005f, -0.989950001f },
    { 0.0f, -0.0665189996f, -0.997784972f },
    { -0.0710669979f, -0.0710669979f, -0.994937003f },
    { -0.0758100003f, -0.151620001f, -0.985527992f },
    { -0.151620001f, -0.0758100003f, -0.985527992f },
    { -0.0805820003f, -0.241747007f, -0.966987014f },
    { -0.162221f, -0.162221f, -0.973327994f },
    { -0.241747007f, -0.0805820003f, -0.966987014f },
    { -0.0851259977f, -0.340503007f, -0.936381996f },
    { -0.172774002f, -0.259160995f, -0.950254977f },
    { -0.259160995f, -0.172774002f, -0.950254977f },
    { -0.340503007f, -0.0851259977f, -0.936381996f },
    { -0.089087002f, -0.445434988f, -0.890870988f },
    { -0.182574004f, -0.365148008f, -0.912871003f },
    { -0.276172012f, -0.276172012f, -0.920575023f },
    { -0.365148008f, -0.182574004f, -0.912871003f },
    { -0.445434988f, -0.089087002f, -0.890870988f },
    { -0.0920569971f, -0.552344978f, -0.82851702f },
    { -0.190693006f, -0.476731002f, -0.858115971f },
    { -0.291386008f, -0.388514012f, -0.874157012f },
    { -0.388514012f, -0.291386008f, -0.874157012f },
    { -0.476731002f, -0.190691993f, -0.858115971f },
    { -0.552344978f, -0.0920569971f, -0.82851702f },
    { -0.0936589986f, -0.655610025f, -0.749269009f },
    { -0.196116f, -0.588347971f, -0.784465015f },
    { -0.303045988f, -0.505075991f, -0.808121979f },
    { -0.408248007f, -0.408248007f, -0.816497028f },
    { -0.505075991f, -0.303045988f, -0.808121979f },
    { -0.588347971f, -0.196116f, -0.784465015f },
    { -0.655610025f, -0.0936589986f, -0.749269009f },
    { -0.0936589986f, -0.749269009f, -0.655610025f },
    { -0.198029995f, -0.693103015f, -0.693103015f },
    { -0.30942601f, -0.618852973f, -0.721994996f },
    { -0.421636999f, -0.527046025f, -0.737864971f },
    { -0.527046025f, -0.421636999f, -0.737864971f },
    { -0.618852973f, -0.30942601f, -0.721994996f },
    { -0.693103015f, -0.198028997f, -0.693103015f },
    { -0.749269009f, -0.0936589986f, -0.655610025f },
    { -0.0920569971f, -0.82851702f, -0.552344978f },
    { -0.196116f, -0.784465015f, -0.588347971f },
    { -0.30942601f, -0.721994996f, -0.618852973f },
    { -0.426400989f, -0.639602005f, -0.639602005f },
    { -0.539164007f, -0.539164007f, -0.646996975f },
    { -0.639602005f, -0.426400989f, -0.639602005f },
    { -0.721994996f, -0.30942601f, -0.618852973f },
    { -0.784465015f, -0.196116f, -0.588347971f },
    { -0.82851702f, -0.0920569971f, -0.552344978f },
    { -0.089087002f, -0.890870988f, -0.445434988f },
    { -0.190693006f, -0.858115971f, -0.476731002f },
    { -0.303045988f, -0.808121979f, -0.505075991f },
    { -0.421636999f, -0.737864971f, -0.527046025f },
    { -0.539164007f, -0.646996975f, -0.539164007f },
    { -0.646996975f, -0.539164007f, -0.539164007f },
    { -0.737864971f, -0.421636999f, -0.527046025f },
    { -0.808121979f, -0.303045988f, -0.505075991f },
    { -0.858115971f, -0.190693006f, -0.476731002f },
    { -0.890870988f, -0.089087002f, -0.445434988f },
    { -0.0851259977f, -0.936381996f, -0.340503007f },
    { -0.182574004f, -0.912871003f, -0.365148008f },
    { -0.291386008f, -0.874157012f, -0.388514012f },
    { -0.408248007f, -0.816497028f, -0.408248007f },
    { -0.527046025f, -0.737864971f, -0.421636999f },
    { -0.639602005f, -0.639602005f, -0.426400989f },
    { -0.737864971f, -0.527046025f, -0.421636999f },
    { -0.816497028f, -0.408248007f, -0.408248007f },
    { -0.874157012f, -0.291386008f, -0.388514012f },
    { -0.912871003f, -0.182574004f, -0.365148008f },
    { -0.936381996f, -0.0851259977f, -0.340503007f },
    { -0.0805820003f, -0.966988027f, -0.241747007f },
    { -0.172774002f, -0.950254977f, -0.259160995f },
    { -0.276172012f, -0.920575023f, -0.276172012f },
    { -0.388514012f, -0.874157012f, -0.291386008f },
    { -0.505075991f, -0.808121979f, -0.303045988f },
    { -0.618852973f, -0.721994996f, -0.30942601f },
    { -0.721994996f, -0.618852973f, -0.30942601f },
    { -0.808121979f, -0.505075991f, -0.303045988f },
    { -0.874157012f, -0.388514012f, -0.291386008f },
    { -0.920575023f, -0.276172012f, -0.276172012f },
    { -0.950254977f, -0.172774002f, -0.259160995f },
    { -0.966988027f, -0.0805820003f, -0.241747007f },
    { -0.0758100003f, -0.985526979f, -0.151620001f },
    { -0.162221f, -0.973329008f, -0.162221f },
    { -0.259160995f, -0.950254977f, -0.172774002f },
    { -0.365148008f, -0.912871003f, -0.182574004f },
    { -0.476731002f, -0.858115971f, -0.190693006f },
    { -0.588347971f, -0.784465015f, -0.196116f },
    { -0.693103015f, -0.693103015f, -0.198029995f },
    { -0.784465015f, -0.588347971f, -0.196116f },
    { -0.858115971f, -0.476731002f, -0.190693006f },
    { -0.912871003f, -0.365148008f, -0.182574004f },
    { -0.950254977f, -0.259160995f, -0.172774002f },
    { -0.973329008f, -0.162221f, -0.162221f },
    { -0.985526979f, -0.0758100003f, -0.151620001f },
    { -0.0710669979f, -0.994937003f, -0.0710669979f },
    { -0.151620001f, -0.985526979f, -0.0758100003f },
    { -0.241747007f, -0.966988027f, -0.0805820003f },
    { -0.340503007f, -0.936381996f, -0.0851259977f },
    { -0.445434988f, -0.890870988f, -0.089087002f },
    { -0.552344978f, -0.82851702f, -0.0920569971f },
    { -0.655610025f, -0.749269009f, -0.0936589986f },
    { -0.749269009f, -0.655610025f, -0.0936589986f },
    { -0.82851702f, -0.552344978f, -0.0920569971f },
    { -0.890870988f, -0.445434988f, -0.089087002f },
    { -0.936381996f, -0.340503007f, -0.0851259977f },
    { -0.966988027f, -0.241747007f, -0.0805820003f },
    { -0.985526979f, -0.151620001f, -0.0758100003f },
    { -0.994937003f, -0.0710669979f, -0.0710669979f },
    { 0.997784972f, 0.0f, -0.0665189996f },
    { 0.989950001f, 0.0f, -0.141421005f },
    { 0.974390984f, 0.0f, -0.224859998f },
    { 0.948683023f, 0.0f, -0.316228002f },
    { 0.910365999f, 0.0f, -0.413803011f },
    { 0.857492983f, 0.0f, -0.514496028f },
    { 0.789352f, 0.0f, -0.613941014f },
    { 0.707107008f, 0.0f, -0.707107008f },
    { 0.613941014f, 0.0f, -0.789352f },
    { 0.514496028f, 0.0f, -0.857492983f },
    { 0.413803011f, 0.0f, -0.910365999f },
    { 0.316228002f, 0.0f, -0.948683023f },
    { 0.224859998f, 0.0f, -0.974390984f },
    { 0.141421005f, 0.0f, -0.989950001f },
    { 0.0665189996f, 0.0f, -0.997784972f },
    { 0.0710669979f, -0.0710669979f, -0.994937003f },
    { 0.151620001f, -0.0758100003f, -0.985527992f },
    { 0.0758100003f, -0.151620001f, -0.985527992f },
    { 0.241747007f, -0.0805820003f, -0.966987014f },
    { 0.162221f, -0.162221f, -0.973327994f },
    { 0.0805820003f, -0.241747007f, -0.966987014f },
    { 0.340503007f, -0.0851259977f, -0.936381996f },
    { 0.259160995f, -0.172774002f, -0.950254977f },
    { 0.172774002f, -0.259160995f, -0.950254977f },
    { 0.0851259977f, -0.340503007f, -0.936381996f },
    { 0.445434988f, -0.089087002f, -0.890870988f },
    { 0.365148008f, -0.182574004f, -0.912871003f },
    { 0.276172012f, -0.276172012f, -0.920575023f },
    { 0.182574004f, -0.365148008f, -0.912871003f },
    { 0.089087002f, -0.445434988f, -0.890870988f },
    { 0.552344978f, -0.0920569971f, -0.82851702f },
    { 0.476731002f, -0.190693006f, -0.858115971f },
    { 0.388514012f, -0.291386008f, -0.874157012f },
    { 0.291386008f, -0.388514012f, -0.874157012f },
    { 0.190691993f, -0.476731002f, -0.858115971f },
    { 0.0920569971f, -0.552344978f, -0.82851702f },
    { 0.655610025f, -0.0936589986f, -0.749269009f },
    { 0.588347971f, -0.196116f, -0.784465015f },
    { 0.505075991f, -0.303045988f, -0.808121979f },
    { 0.408248007f, -0.408248007f, -0.816497028f },
    { 0.303045988f, -0.505075991f, -0.808121979f },
    { 0.196116f, -0.588347971f, -0.784465015f },
    { 0.0936589986f, -0.655610025f, -0.749269009f },
    { 0.749269009f, -0.0936589986f, -0.655610025f },
    { 0.693103015f, -0.198029995f, -0.693103015f },
    { 0.618852973f, -0.30942601f, -0.721994996f },
    { 0.527046025f, -0.421636999f, -0.737864971f },
    { 0.421636999f, -0.527046025f, -0.737864971f },
    { 0.30942601f, -0.618852973f, -0.721994996f },
    { 0.198028997f, -0.693103015f, -0.693103015f },
    { 0.0936589986f, -0.749269009f, -0.655610025f },
    { 0.82851702f, -0.0920569971f, -0.552344978f },
    { 0.784465015f, -0.196116f, -0.588347971f },
    { 0.721994996f, -0.30942601f, -0.618852973f },
    { 0.639602005f, -0.426400989f, -0.639602005f },
    { 0.539164007f, -0.539164007f, -0.646996975f },
    { 0.426400989f, -0.639602005f, -0.639602005f },
    { 0.30942601f, -0.721994996f, -0.618852973f },
    { 0.196116f, -0.784465015f, -0.588347971f },
    { 0.0920569971f, -0.82851702f, -0.552344978f },
    { 0.890870988f, -0.089087002f, -0.445434988f },
    { 0.858115971f, -0.190693006f, -0.476731002f },
    { 0.808121979f, -0.303045988f, -0.505075991f },
    { 0.737864971f, -0.421636999f, -0.527046025f },
    { 0.646996975f, -0.539164007f, -0.539164007f },
    { 0.539164007f, -0.646996975f, -0.539164007f },
    { 0.421636999f, -0.737864971f, -0.527046025f },
    { 0.303045988f, -0.808121979f, -0.505075991f },
    { 0.190693006f, -0.858115971f, -0.476731002f },
    { 0.089087002f, -0.890870988f, -0.445434988f },
    { 0.936381996f, -0.0851259977f, -0.340503007f },
    { 0.912871003f, -0.182574004f, -0.365148008f },
    { 0.874157012f, -0.291386008f, -0.388514012f },
    { 0.816497028f, -0.408248007f, -0.408248007f },
    { 0.737864971f, -0.527046025f, -0.421636999f },
    { 0.639602005f, -0.639602005f, -0.426400989f },
    { 0.527046025f, -0.737864971f, -0.421636999f },
    { 0.408248007f, -0.816497028f, -0.408248007f },
    { 0.291386008f, -0.874157012f, -0.388514012f },
    { 0.182574004f, -0.912871003f, -0.365148008f },
    { 0.0851259977f, -0.936381996f, -0.340503007f },
    { 0.966988027f, -0.0805820003f, -0.241747007f },
    { 0.950254977f, -0.172774002f, -0.259160995f },
    { 0.920575023f, -0.276172012f, -0.276172012f },
    { 0.874157012f, -0.388514012f, -0.291386008f },
    { 0.808121979f, -0.505075991f, -0.303045988f },
    { 0.721994996f, -0.618852973f, -0.30942601f },
    { 0.618852973f, -0.721994996f, -0.30942601f },
    { 0.505075991f, -0.808121979f, -0.303045988f },
    { 0.388514012f, -0.874157012f, -0.291386008f },
    { 0.276172012f, -0.920575023f, -0.276172012f },
    { 0.172774002f, -0.950254977f, -0.259160995f },
    { 0.0805820003f, -0.966988027f, -0.241747007f },
    { 0.985526979f, -0.0758100003f, -0.151620001f },
    { 0.973329008f, -0.162221f, -0.162221f },
    { 0.950254977f, -0.259160995f, -0.172774002f },
    { 0.912871003f, -0.365148008f, -0.182574004f },
    { 0.858115971f, -0.476731002f, -0.190693006f },
    { 0.784465015f, -0.588347971f, -0.196116f },
    { 0.693103015f, -0.693103015f, -0.198029995f },
    { 0.588347971f, -0.784465015f, -0.196116f },
    { 0.476731002f, -0.858115971f, -0.190693006f },
    { 0.365148008f, -0.912871003f, -0.182574004f },
    { 0.259160995f, -0.950254977f, -0.172774002f },
    { 0.162221f, -0.973329008f, -0.162221f },
    { 0.0758100003f, -0.985526979f, -0.151620001f },
    { 0.994937003f, -0.0710669979f, -0.0710669979f },
    { 0.985526979f, -0.151620001f, -0.0758100003f },
    { 0.966988027f, -0.241747007f, -0.0805820003f },
    { 0.936381996f, -0.340503007f, -0.0851259977f },
    { 0.890870988f, -0.445434988f, -0.089087002f },
    { 0.82851702f, -0.552344978f, -0.0920569971f },
    { 0.749269009f, -0.655610025f, -0.0936589986f },
    { 0.655610025f, -0.749269009f, -0.0936589986f },
    { 0.552344978f, -0.82851702f, -0.0920569971f },
    { 0.445434988f, -0.890870988f, -0.089087002f },
    { 0.340503007f, -0.936381996f, -0.0851259977f },
    { 0.241747007f, -0.966988027f, -0.0805820003f },
    { 0.151620001f, -0.985526979f, -0.0758100003f },
    { 0.0710669979f, -0.994937003f, -0.0710669979f },
    { 0.0710669979f, 0.0710669979f, -0.994937003f },
    { 0.0758100003f, 0.151620001f, -0.985527992f },
    { 0.151620001f, 0.0758100003f, -0.985527992f },
    { 0.0805820003f, 0.241747007f, -0.966987014f },
    { 0.162221f, 0.162221f, -0.973327994f },
    { 0.241747007f, 0.0805820003f, -0.966987014f },
    { 0.0851259977f, 0.340503007f, -0.936381996f },
    { 0.172774002f, 0.259160995f, -0.950254977f },
    { 0.259160995f, 0.172774002f, -0.950254977f },
    { 0.340503007f, 0.0851259977f, -0.936381996f },
    { 0.089087002f, 0.445434988f, -0.890870988f },
    { 0.182574004f, 0.365148008f, -0.912871003f },
    { 0.276172012f, 0.276172012f, -0.920575023f },
    { 0.365148008f, 0.182574004f, -0.912871003f },
    { 0.445434988f, 0.089087002f, -0.890870988f },
    { 0.0920569971f, 0.552344978f, -0.82851702f },
    { 0.190693006f, 0.476731002f, -0.858115971f },
    { 0.291386008f, 0.388514012f, -0.874157012f },
    { 0.388514012f, 0.291386008f, -0.874157012f },
    { 0.476731002f, 0.190691993f, -0.858115971f },
    { 0.552344978f, 0.0920569971f, -0.82851702f },
    { 0.0936589986f, 0.655610025f, -0.749269009f },
    { 0.196116f, 0.588347971f, -0.784465015f },
    { 0.303045988f, 0.505075991f, -0.808121979f },
    { 0.408248007f, 0.408248007f, -0.816497028f },
    { 0.505075991f, 0.303045988f, -0.808121979f },
    { 0.588347971f, 0.196116f, -0.784465015f },
    { 0.655610025f, 0.0936589986f, -0.749269009f },
    { 0.0936589986f, 0.749269009f, -0.655610025f },
    { 0.198029995f, 0.693103015f, -0.693103015f },
    { 0.30942601f, 0.618852973f, -0.721994996f },
    { 0.421636999f, 0.527046025f, -0.737864971f },
    { 0.527046025f, 0.421636999f, -0.737864971f },
    { 0.618852973f, 0.30942601f, -0.721994996f },
    { 0.693103015f, 0.198028997f, -0.693103015f },
    { 0.749269009f, 0.0936589986f, -0.655610025f },
    { 0.0920569971f, 0.82851702f, -0.552344978f },
    { 0.196116f, 0.784465015f, -0.588347971f },
    { 0.30942601f, 0.721994996f, -0.618852973f },
    { 0.426400989f, 0.639602005f, -0.639602005f },
    { 0.539164007f, 0.539164007f, -0.646996975f },
    { 0.639602005f, 0.426400989f, -0.639602005f },
    { 0.721994996f, 0.30942601f, -0.618852973f },
    { 0.784465015f, 0.196116f, -0.588347971f },
    { 0.82851702f, 0.0920569971f, -0.552344978f },
    { 0.089087002f, 0.890870988f, -0.445434988f },
    { 0.190693006f, 0.858115971f, -0.476731002f },
    { 0.303045988f, 0.808121979f, -0.505075991f },
    { 0.421636999f, 0.737864971f, -0.527046025f },
    { 0.539164007f, 0.646996975f, -0.539164007f },
    { 0.646996975f, 0.539164007f, -0.539164007f },
    { 0.737864971f, 0.421636999f, -0.527046025f },
    { 0.808121979f, 0.303045988f, -0.505075991f },
    { 0.858115971f, 0.190693006f, -0.476731002f },
    { 0.890870988f, 0.089087002f, -0.445434988f },
    { 0.0851259977f, 0.936381996f, -0.340503007f },
    { 0.182574004f, 0.912871003f, -0.365148008f },
    { 0.291386008f, 0.874157012f, -0.388514012f },
    { 0.408248007f, 0.816497028f, -0.408248007f },
    { 0.527046025f, 0.737864971f, -0.421636999f },
    { 0.639602005f, 0.639602005f, -0.426400989f },
    { 0.737864971f, 0.527046025f, -0.421636999f },
    { 0.816497028f, 0.408248007f, -0.408248007f },
    { 0.874157012f, 0.291386008f, -0.388514012f },
    { 0.912871003f, 0.182574004f, -0.365148008f },
    { 0.936381996f, 0.0851259977f, -0.340503007f },
    { 0.0805820003f, 0.966988027f, -0.241747007f },
    { 0.172774002f, 0.950254977f, -0.259160995f },
    { 0.276172012f, 0.920575023f, -0.276172012f },
    { 0.388514012f, 0.874157012f, -0.291386008f },
    { 0.505075991f, 0.808121979f, -0.303045988f },
    { 0.618852973f, 0.721994996f, -0.30942601f },
    { 0.721994996f, 0.618852973f, -0.30942601f },
    { 0.808121979f, 0.505075991f, -0.303045988f },
    { 0.874157012f, 0.388514012f, -0.291386008f },
    { 0.920575023f, 0.276172012f, -0.276172012f },
    { 0.950254977f, 0.172774002f, -0.259160995f },
    { 0.966988027f, 0.0805820003f, -0.241747007f },
    { 0.0758100003f, 0.985526979f, -0.151620001f },
    { 0.162221f, 0.973329008f, -0.162221f },
    { 0.259160995f, 0.950254977f, -0.172774002f },
    { 0.365148008f, 0.912871003f, -0.182574004f },
    { 0.476731002f, 0.858115971f, -0.190693006f },
    { 0.588347971f, 0.784465015f, -0.196116f },
    { 0.693103015f, 0.693103015f, -0.198029995f },
    { 0.784465015f, 0.588347971f, -0.196116f },
    { 0.858115971f, 0.476731002f, -0.190693006f },
    { 0.912871003f, 0.365148008f, -0.182574004f },
    { 0.950254977f, 0.259160995f, -0.172774002f },
    { 0.973329008f, 0.162221f, -0.162221f },
    { 0.985526979f, 0.0758100003f, -0.151620001f },
    { 0.0710669979f, 0.994937003f, -0.0710669979f },
    { 0.151620001f, 0.985526979f, -0.0758100003f },
    { 0.241747007f, 0.966988027f, -0.0805820003f },
    { 0.340503007f, 0.936381996f, -0.0851259977f },
    { 0.445434988f, 0.890870988f, -0.089087002f },
    { 0.552344978f, 0.82851702f, -0.0920569971f },
    { 0.655610025f, 0.749269009f, -0.0936589986f },
    { 0.749269009f, 0.655610025f, -0.0936589986f },
    { 0.82851702f, 0.552344978f, -0.0920569971f },
    { 0.890870988f, 0.445434988f, -0.089087002f },
    { 0.936381996f, 0.340503007f, -0.0851259977f },
    { 0.966988027f, 0.241747007f, -0.0805820003f },
    { 0.985526979f, 0.151620001f, -0.0758100003f },
    { 0.994937003f, 0.0710669979f, -0.0710669979f },
};
// @rdata-end
__declspec(selectany) const real_vector3d *const global_random_directions = players_spawn_random_directions;
static __forceinline float players_spawn_get_random_scale() { return 0.2f; }
// @address 0x3ccc0 k_player_fix_position_search_directions
#if !defined(_XBOX) || defined(__clang__)
const real_vector3d k_player_fix_position_search_directions[9] =
{
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { 0.70710677f, -0.70710677f, 0.0f },
    { 0.70710677f, 0.70710677f, 0.0f },
    { 0.57735026f, 0.57735026f, 0.57735026f },
    { 0.57735026f, 0.57735026f, -0.57735026f },
    { 0.57735026f, -0.57735026f, 0.57735026f },
    { 0.57735026f, -0.57735026f, -0.57735026f },
};
#endif
// @address 0x1da4a0 player_fix_unit_position
extern "C" bool player_fix_unit_position(long player_index, long unit_index, const real_point3d *position, bool single_pass, bool teleported)
{
    bool result = false;
    player_datum *player = player_get(player_index);
    long player_unit_index = player->unit_index;
    object_datum *player_unit_object = object_get(player_unit_index);
    long child_index = player_unit_index;
    long root_index = NONE;
    if (child_index != NONE) {
        do {
            root_index = child_index;
            child_index = object_get(child_index)->parent_object_index;
        } while (child_index != NONE);
    }
    object_set_position_direct(root_index, position, 0, 0, 0, false);
    if (player_unit_object->havok_component_index == NONE)
        havok_component_aquire(player_unit_index, true, true);
    do {
    if (unit_index != NONE) {
        long target_child_index = unit_index;
        long target_root_index;
        do {
            target_root_index = target_child_index;
            target_child_index = object_get(target_child_index)->parent_object_index;
        } while (target_child_index != NONE);
        if (target_root_index != unit_index) {
            long parent_child_index = player_unit_index;
            long parent_root_index = NONE;
            if (parent_child_index != NONE) {
                do {
                    parent_root_index = parent_child_index;
                    parent_child_index = object_get(parent_child_index)->parent_object_index;
                } while (parent_child_index != NONE);
            }
            long spawn_child_index = unit_index;
            do {
                unit_index = spawn_child_index;
                spawn_child_index = object_get(spawn_child_index)->parent_object_index;
            } while (spawn_child_index != NONE);
            object_datum *unit_object = object_get(unit_index);
            float clearance;
            bool is_vehicle = TEST_BIT(1 << object_get_type(parent_root_index), 1);
            if (is_vehicle)
                clearance = 2.0f;
            else
                clearance = 0.35f;
            real_vector3d facing = unit_object->linear_velocity;
            if (!(facing.i * facing.i + facing.j * facing.j > 0.0f)) {
                if (unit_object->forward.k < 0.70710677f)
                    facing = unit_object->forward;
                else
                    facing = unit_object->up;
            }
            float height = unit_object->bounding_radius + 0.6f;
            facing.i = -facing.i;
            facing.j = -facing.j;
            facing.k = -0.0f;
            normalize3d(&facing);
            real_matrix4x3 matrix;
            matrix4x3_rotation_from_vectors(&matrix, &facing, global_up3d);
            float placement_scale = 1.0f;
            float random_scale = players_spawn_get_random_scale();
            matrix.position = unit_object->bounding_center;
            long which = unit_index == NONE;
            while (which < 2) {
                long target_unit_index = which == 0 ? unit_index : NONE;
                unsigned long table_offset = 0;
                const real_vector3d *direction = k_player_fix_position_search_directions;
                do {
                    if (result)
                        break;
                    float x = direction->i * height;
                    float y = direction->j * height;
                    float z = direction->k * height;
                    real_point3d spawn_point;
                    spawn_point.x = matrix.forward.i * x + (matrix.left.i * y + matrix.up.i * z) + matrix.position.x;
                    spawn_point.y = matrix.forward.j * x + (matrix.left.j * y + matrix.up.j * z) + matrix.position.y;
                    spawn_point.z = matrix.forward.k * x + (matrix.left.k * y + matrix.up.k * z) + matrix.position.z;
                    result = unit_fix_position(parent_root_index, target_unit_index, 0, &spawn_point, 0, clearance, placement_scale, false);
                    if (!result) {
                        float saved_z = spawn_point.z;
                        float saved_y = spawn_point.y;
                        float saved_x = spawn_point.x;
                        long attempt = 0;
                        do {
                            if (result)
                                break;
                            // match: 0x1da4a0: advancing the TLS seed directly swaps the z-direction fmuls operands.
                            unsigned long next = *tls_random_seed();
                            random_seed_next(&next);
                            *tls_random_seed() = next;
                            short index = (short)(((next >> 16) * 1026) >> 16);
                            real_vector3d random_direction = players_spawn_random_directions[index];
                            real_point3d candidate_point;
                            candidate_point.x = random_direction.i * random_scale + saved_x;
                            candidate_point.y = random_direction.j * random_scale + saved_y;
                            candidate_point.z = random_direction.k * random_scale + saved_z;
                            result = unit_fix_position(parent_root_index, target_unit_index, 0, &candidate_point, 0, clearance, placement_scale, false);
                            attempt++;
                        } while (attempt < 8);
                    }
                    table_offset += 12;
                    direction++;
                } while (table_offset < 108);
                if (single_pass)
                    break;
                which++;
                if (!result)
                    continue;
                break;
            }
            break;
        }
    }
    {
        long fallback_child_index = player_unit_index;
        long fallback_root_index = NONE;
        if (fallback_child_index != NONE) {
            do {
                fallback_root_index = fallback_child_index;
                fallback_child_index = object_get(fallback_child_index)->parent_object_index;
            } while (fallback_child_index != NONE);
        }
        result = unit_fix_position(fallback_root_index, unit_index, 0, position, 0, 0.0f, 1.0f, false);
    }
    } while (0);
    cluster_reference_invalidate(&player->cluster_reference);
    if (result)
        player_update_unit_after_teleport(player_index, unit_index, teleported);
    return result;
}

static __forceinline short weapon_definition_multiplayer_type(long weapon_index)
{
    item_datum *weapon = (item_datum *)object_get(weapon_index);
    const weapon_definition *definition =
        (const weapon_definition *)tag_get(NONE, weapon->object.definition_index);
    return definition->multiplayer_weapon_type;
}

static const float k_player_weapon_pickup_radius = 0.4f;
static __forceinline float players_weapon_distance_squared(const real_point3d *a, const real_point3d *b)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float dz = b->z - a->z;
    return (dx * dx + dy * dy) + dz * dz;
}
// @address 0x1dd9d0 ?player_consider_weapon_interaction@@YA_NJJPAUs_player_action_context@@@Z
bool player_consider_weapon_interaction(long player_index, long weapon_index, s_player_action_context *context)
{
    player_datum *player = player_get(player_index);
    unit_datum *unit = (unit_datum *)object_get(player->unit_index);
    item_datum *weapon = (item_datum *)object_get(weapon_index);
    bool multiplayer_weapon = weapon_definition_multiplayer_type(weapon_index) != 0;
    float radius = unit->object.bounding_radius + (multiplayer_weapon ? 0.1f : 0.0f) + k_player_weapon_pickup_radius;
    bool result = false;
    if (weapon->owner_relation == 0
        && weapon->toss_unit_index != player->unit_index
        && players_weapon_distance_squared(&unit->object.bounding_center, &weapon->object.bounding_center) < radius * radius
        && !players_object_is_multiplayer_cinematic_object(weapon_index))
    {
        unit_weapon_pickup_result pickup_result;
        if (unit_can_pickup_weapon(player->unit_index, weapon_index, _weapon_addition_method_none, &pickup_result))
        {
            s_player_interaction saved_interaction = context->interaction;
            unsigned long flags = 0;
            if (pickup_result.primary[0])
                SET_BIT(flags, 0, true);
            else if (pickup_result.backpack[0])
                SET_BIT(flags, 1, true);
            if (pickup_result.primary[1])
                SET_BIT(flags, 2, true);
            else if (pickup_result.backpack[1])
                SET_BIT(flags, 3, true);
            s_player_interaction interaction;
            memset(&interaction, 0, sizeof(interaction));
            interaction.type = multiplayer_weapon ? 9 : 1;
            interaction.object_index = weapon_index;
            interaction.weapon_flags = (unsigned char)flags;
            result = player_consider_interaction(player_index, &interaction, context);
            if (result && !unit_can_access_object(player->unit_index, weapon_index))
            {
                result = false;
                context->interaction = saved_interaction;
            }
        }
    }
    return result;
}


static __forceinline float players_equipment_distance_squared(const real_point3d *a, const real_point3d *b)
{
    float dz = 0.0f;
    float dy = 0.0f;
    float dx = 0.0f;
    dy = b->y - a->y;
    dz = b->z - a->z;
    dx = b->x - a->x;
    return dx * dx + (dy * dy + dz * dz);
}

static __forceinline item_datum *players_equipment_object_get(s_data_array *const &data, long index)
{
    return (item_datum *)((s_object_header *)data->data)[(unsigned short)index].datum;
}

static __forceinline bool players_consider_equipment_interaction(const long &player_index, const long &equipment_index, s_player_action_context *context)
{
    player_datum *player = player_get(player_index);
    unit_datum *unit = (unit_datum *)object_get(player->unit_index);
    item_datum *equipment = (item_datum *)players_equipment_object_get(object_header_data, equipment_index);
    float radius = unit->object.bounding_radius + 0.4f;
    bool result = false;
    if (equipment->owner_relation == 0
        && equipment->toss_unit_index != player->unit_index
        && players_equipment_distance_squared(&unit->object.bounding_center, &equipment->object.bounding_center) < radius * radius
        && unit->unit.equipment_object_index != NONE)
    {
        long unit_index = player->unit_index;
        if (unit_can_pickup_equipment(unit_index, equipment_index))
        {
            long weapon_index = unit_get_active_primary_weapon(unit_index, 0);
            if (weapon_index == NONE || !players_pickup_support_weapon(weapon_index))
            {
                s_player_interaction saved_interaction = context->interaction;
                s_player_interaction interaction;
                memset(&interaction, 0, sizeof(interaction));
                interaction.object_index = equipment_index;
                interaction.type = 2;
                result = player_consider_interaction(player_index, &interaction, context);
                if (result && !unit_can_access_object(player->unit_index, equipment_index))
                {
                    result = false;
                    context->interaction = saved_interaction;
                }
            }
        }
    }
    return result;
}

// @address 0x1ddbd8 ?player_consider_equipment_interaction@@YA_NJJPAUs_player_action_context@@@Z
bool player_consider_equipment_interaction(long player_index, long equipment_index, s_player_action_context *context)
{
    return players_consider_equipment_interaction(player_index, equipment_index, context);
}

static __forceinline s_players_globals *players_pickup_game_state_get(void)
{
    return players_globals;
}
// match: 0x1dc878: direct header lookup preserves separate unit-index reloads.
static __forceinline unit_datum *players_approve_unit_get(const long &unit_index)
{
    return (unit_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(unit_index)].datum;
}
static __forceinline long players_approve_unit_inventory_get_weapon(long unit_index, short inventory_index)
{
    unit_datum *unit = players_approve_unit_get(unit_index);
    long weapon_index = NONE;
    if (inventory_index != NONE)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}
// @address 0x1dc878 ?player_approve_pickup_weapon@@YAXJJPAUunit_weapon_pickup_result@@@Z
// match: 0x1dc878: pointer __restrict qualifier prevents alias reload
void player_approve_pickup_weapon(long player_index, long weapon_index, unit_weapon_pickup_result *__restrict pickup_result)
{
    player_datum *player = player_get(player_index);
    unit_datum *unit = players_pickup_outer_unit(player->unit_index);
    long counted_weapons = 0;
    long primary_weapons[2];
    long secondary_weapons[2];
    for (long weapon_set_index = 0; weapon_set_index < 2; weapon_set_index++)
    {
        primary_weapons[weapon_set_index] = players_approve_unit_inventory_get_weapon(
            player->unit_index, players_approve_unit_get(player->unit_index)->unit.current_weapon_indices[weapon_set_index]);
        secondary_weapons[weapon_set_index] = players_approve_unit_inventory_get_weapon(
            player->unit_index, players_approve_unit_get(player->unit_index)->unit.desired_weapon_indices[weapon_set_index]);
        if (primary_weapons[weapon_set_index] != NONE && players_pickup_support_weapon(primary_weapons[weapon_set_index]))
        {
            if (players_pickup_support_weapon(weapon_index) || players_pickup_must_be_readied(weapon_index))
            {
                pickup_result->primary[weapon_set_index] = false;
            }
            else
            {
                pickup_result->primary[weapon_set_index] = false;
                pickup_result->backpack[weapon_set_index] = false;
            }
        }
    }
    if (unit->object.parent_object_index != NONE || (players_globals && players_pickup_game_state_get()->input_flag))
    {
        for (long weapon_set_index = 0; weapon_set_index < 2; weapon_set_index++)
        {
            pickup_result->primary[weapon_set_index] = false;
            pickup_result->backpack[weapon_set_index] = false;
        }
    }
    for (long inventory_index = 0; inventory_index < 4; inventory_index++)
    {
        long inventory_weapon = unit->unit.weapon_object_indices[inventory_index];
        if (inventory_weapon != NONE && players_pickup_counts(inventory_weapon))
        {
            bool counts = true;
            if (weapon_can_be_dual_wielded(weapon_index)
                && inventory_weapon == primary_weapons[1]
                && inventory_weapon == secondary_weapons[1])
            {
                counts = false;
            }
            if (counts)
                counted_weapons++;
        }
    }
    if (counted_weapons >= 2)
    {
        for (long weapon_set_index = 0; weapon_set_index < 2; weapon_set_index++)
        {
            if (weapon_set_index == 1)
            {
                if (primary_weapons[weapon_set_index] != NONE && players_pickup_counts(primary_weapons[weapon_set_index]))
                    pickup_result->primary[weapon_set_index] = false;
            }
            else if (weapon_counts_towards_inventory_limit(weapon_index))
            {
                pickup_result->primary[weapon_set_index] = false;
                if (primary_weapons[weapon_set_index] != NONE
                    && !players_pickup_must_be_readied(primary_weapons[weapon_set_index])
                    && !players_pickup_counts(primary_weapons[weapon_set_index]))
                {
                    pickup_result->backpack[weapon_set_index] = false;
                }
            }
        }
    }
}

// @address 0xa0a58 ?player_find_action_context@@YAXJPAUs_player_action_context@@@Z
void player_find_action_context(long player_index, s_player_action_context *out_action_context)
{
    player_datum *player = player_get(player_index);
    clear_action_context(out_action_context);
    if (player->unit_index != NONE)
    {
        object_datum *unit = object_get(player->unit_index);
        if (unit->parent_object_index == NONE)
        {
            float radius = unit->bounding_radius + 0.5f;
            long object_indices[64];
            long object_count = objects_in_sphere(0, 0x15df, &unit->location, &unit->bounding_center,
                radius, object_indices, sizeof(object_indices) / sizeof(object_indices[0]));
            for (long index = 0; index < object_count; index++)
            {
                object_datum *object = object_get(object_indices[index]);
                long object_type = object->object_type;
                bool consider_children = false;
                unsigned long ignore_child_mask = 0xa20;
                if ((1 << object_type) & ignore_child_mask)
                    continue;
                if (((1 << object_type) & 0x184) != 0
                    && !point_in_sphere(&unit->bounding_center, &object->bounding_center, object->bounding_radius + radius))
                    continue;
                switch (object_type)
                {
                case 2:
                    player_consider_weapon_interaction(player_index, object_indices[index], out_action_context);
                    break;
                case 3:
                    player_consider_equipment_interaction(player_index, object_indices[index], out_action_context);
                    break;
                case 4:
                case 8:
                    player_consider_device(player_index, object_indices[index], out_action_context);
                    break;
                case 1:
                    player_consider_flipped_vehicle(player_index, object_indices[index], out_action_context);
                    ignore_child_mask |= 2;
                    consider_children = true;
                    break;
                case 0:
                    player_consider_biped(player_index, object_indices[index], out_action_context);
                    break;
                default:
                    consider_children = true;
                    break;
                }
                if (consider_children)
                {
                    for (long child_index = object->first_child_object_index;
                         child_index != NONE && object_count < sizeof(object_indices) / sizeof(object_indices[0]);)
                    {
                        object_datum *child = object_get(child_index);
                        bool add_child;
                        if ((1 << child->object_type) & ignore_child_mask)
                            add_child = false;
                        else if (((1 << object_type) & 0x184) != 0)
                            add_child = point_in_sphere(
                                &unit->bounding_center, &child->bounding_center, child->bounding_radius + radius);
                        else
                            add_child = true;
                        if (add_child)
                            object_indices[object_count++] = child_index;
                        child_index = child->next_object_index;
                    }
                }
                if (((1 << object_type) & 0x1003) != 0)
                    player_consider_vehicle_seat(player_index, object_indices[index], out_action_context);
            }
        }
    }
}


static __forceinline void players_teleport_get_position(long unit_index, long source_unit_index,
    const s_equipment_spawner_definition *seat, real_point3d *position);


static __forceinline void players_teleport_get_position(long unit_index, long source_unit_index,
    const s_equipment_spawner_definition *seat, real_point3d *position)
{
    // match: 0x380da8: the zero offset preserves the second seat test in r25 at 0x380f0c after the position calls.
    if (seat + (long)_rotl(0, 0))
        vehicle_type_human_plane_find_exit_point(unit_index, seat->spawn_radius, seat->spawn_z_offset, seat->spawn_area_radius, seat->type, position, false);
    else
        object_get_origin(source_unit_index, position);
}

// @address 0x380da8 player_throw_equipment
extern "C" void player_throw_equipment(long unit_handle, long unused, const real_point3d *point, const real_vector3d *direction)
{
    real_vector3d aiming;
    real_vector3d velocity;
    real_vector3d horizontal_aiming;
    real_point3d position;
    real_point3d local_origin;

    unit_datum *unit_object = ((unit_datum *)object_get(unit_handle));
    s_player_equipment_action_state *action =
        (s_player_equipment_action_state *)((char *)unit_object + unit_object->unit.action_offset);

    long source_unit_index = action->source_unit_index;
    if (source_unit_index != NONE && !object_try_and_get_and_verify_type(source_unit_index, 0xffffffff))
        action->source_unit_index = NONE;
    long unit_index = action->source_unit_index;
    if (unit_index == NONE)
        return;

    unit_datum *aiming_unit = ((unit_datum *)object_get(unit_handle));
    const real_vector3d *forward = &aiming_unit->unit.aiming_vector;
    aiming = *forward;
    velocity = *forward;
    horizontal_aiming = *forward;
    horizontal_aiming.k = 0.0f;
    if (normalize3d(&horizontal_aiming) == -1.0f)
        return;

    long definition_index = action->definition_index;
    s_equipment_definition *definition = 0;
    s_equipment_spawner_definition *seat = 0;
    if (definition_index != NONE) {
        definition = (s_equipment_definition *)
            tag_get(NONE, definition_index);
        if (definition->equipment_types[2].count > 0)
            seat = (s_equipment_spawner_definition *)definition->equipment_types[2].address;
    }

    players_teleport_get_position(unit_handle, unit_index, seat, &position);

    if (seat && !simulation_query_local_predictive_control(unit_handle, 1) && action->interruptible == 0) {
        long previous_unit_index = action->source_unit_index;
        if (unit_throw_grenade_create(unit_handle, seat->spawned_object.index, 0) &&
            previous_unit_index != action->source_unit_index)
            object_delete(previous_unit_index);
    }

    long actor_index = unit_object->unit.actor_index;
    if (actor_index != NONE && actor_combat_try_and_get_throw_velocity(actor_index, action->source_unit_index, &velocity)) {
        real_vector3d planar_velocity = velocity;
        planar_velocity.k = 0.0f;
        if (normalize3d(&planar_velocity) > 0.0f) {
            aiming = velocity;
            horizontal_aiming = planar_velocity;
            normalize3d(&aiming);
        }
    }

    else if (seat != 0) {
        real_vector3d inherited_velocity;
        const real_point3d *origin = point;
        if (point == 0)
            origin = object_get_origin(unit_handle, &local_origin);
        unit_get_inherited_velocity(unit_handle, true, origin, &aiming, &inherited_velocity);
        float scale = seat->spawn_velocity;
        velocity.i = aiming.i * scale + inherited_velocity.i;
        velocity.j = aiming.j * scale + inherited_velocity.j;
        velocity.k = aiming.k * scale + inherited_velocity.k;
    }

    long new_object_index = unit_throw_grenade(unit_handle, point, direction, &position, &horizontal_aiming,
        &global_identity4x3->up, &velocity, false, false);
    if (new_object_index != NONE) {
        s_damage_owner owner;
        object_in_early_mover_join(unit_handle, new_object_index);
        damage_compute_damage_owner_from_object_index(unit_handle, &owner);
        object_set_damage_owner(new_object_index, &owner, true);
        if (TEST_BIT(1 << object_get_type(new_object_index), 3)) {
            ((equipment_datum *)((unit_datum *)object_get(new_object_index)))->creator_damage_owner = owner;
            unit_datum *new_unit = ((unit_datum *)object_get(new_object_index));
            unsigned short animation_offset = (unsigned short)new_unit->object.animation.offset;
            if (animation_offset != 0xffff)
                ((c_animation_manager *)((char *)new_unit + (short)animation_offset))->set_goal(
                    226, 226, 226, 710, 130, 29);
        }
        ai_unit_setup_auto_turret(new_object_index, unit_handle);
    }

    if (seat != 0) {
        long effect_index = seat->spawned_effect.index;
        if (effect_index != NONE) {
            c_tag_index effect;
            c_tag_index *effect_pointer = &effect;
            new (effect_pointer) c_tag_index(effect_index);
            effect_new_unattached_from_point(*effect_pointer, &position, &aiming, &global_identity4x3->up,
                1, 1, 0, 0);
        }
        if (definition != 0) {
            long sound_index = definition->sound_index;
            if (sound_index != NONE) {
                long player_index = unit_object->unit.player_index;
                if (player_index != NONE &&
                    player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player_index)] != NONE)
                    unspatialized_impulse_sound_new(sound_index, 1.0f);
            }
        }
    }
}


static __forceinline long players_equipment_weapon_get(long unit_index)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    long weapon_index = NONE;
    short inventory_index = unit->unit.current_weapon_set.weapon_indices[0];
    if (inventory_index != NONE)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}
static __forceinline bool players_equipment_weapon_busy(const weapon_datum *weapon)
{
    return (unsigned long)(weapon->weapon_state - 5) <= 5;
}

// @address 0x3811e8 player_try_to_use_equipment
extern "C" bool player_try_to_use_equipment(long unit_index, const action_request *request)
{
    unit_datum *unit = ((unit_datum *)object_get(unit_index));
    s_player_equipment_action_state *state = (s_player_equipment_action_state *)((char *)unit + unit->unit.action_offset);
    long weapon_index = players_equipment_weapon_get(unit_index);
    bool result = false;
    bool allowed = false;
    if (unit->object.object_type == 0) {
        bool weapon_blocks = false;
        if (weapon_index != NONE) {
            weapon_datum *weapon = (weapon_datum *)object_get(weapon_index);
            weapon_definition *definition =
                (weapon_definition *)tag_get(0, weapon->item.object.definition_index);
            bool prevents_equipment;
            if (FLAG_TEST(definition->secondary_flags, 1))
                prevents_equipment = false;
            else
                prevents_equipment = FLAG_TEST(definition->flags, 6);
            const bool &prevents_equipment_value = prevents_equipment;
            const bool &weapon_busy = players_equipment_weapon_busy(weapon);
            weapon_blocks = weapon_busy | prevents_equipment_value;
        }
        if (!weapon_blocks) {
            if (!TEST_BIT((unit->unit.unit_flags >> 24), 0) || request->throw_item.release)
                allowed = true;
        }
    }
    if (allowed && action_executing(unit_index, _action_grenade))
        allowed = false;
    if (allowed && unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE) {
        unit_definition *definition =
            (unit_definition *)tag_get(0, ((unit_datum *)object_get(unit->object.parent_object_index))->object.definition_index);
        unsigned long flags = definition->seats[unit->unit.parent_seat_index].flags;
        if (!FLAG_TEST(flags, 5) || FLAG_TEST(flags, 11))
            allowed = false;
    }
    if (allowed) {
        if (!request->throw_item.release && request->throw_item.initiate && action_executing(unit_index, _action_equipment)) {
            c_motor_task *task = motor_task_find_by_action(unit_index, _action_equipment);
            if (task)
                motor_system_interrupted(unit_index, task, true);
        }
        if (request->throw_item.release) {
            action_submit_event(unit_index, 47);
            if (action_executing(unit_index, _action_equipment))
                player_throw_equipment(unit_index, 1, 0, 0);
            state->active = true;
            state->released = false;
            state->interruptible = false;
            state->definition_index = request->throw_item.definition_index;
            player_update_equipment_use(unit_index, request->throw_item.forced);
            player_throw_equipment(unit_index, 0, &request->throw_item.position, &request->throw_item.direction);
        } else if (!action_executing(unit_index, _action_equipment)) {
            c_motor_request motor_request;
            action_submit_event(unit_index, 47);
            motor_request.m_valid = true;
            motor_request.action = (e_action)request->action;
            motor_request.m_task = _motor_task_equipment;
            if (motor_system_submit(unit_index, &motor_request) != _motor_result_none) {
                c_animation_channel *channel = motor_animation_task_get_channel(unit_index,
                    motor_task_find_by_action(unit_index, (e_action)request->action));
                float duration = channel->get_playback_duration();
                float primary_key_time = channel->get_authored_primary_key_time();
                state->definition_index = request->throw_item.definition_index;
                state->active = true;
                state->released = false;
                state->interruptible = request->throw_item.initiate;
                state->duration_ticks = (long)((float)game_time_globals->ticks_per_second * duration + 0.5f);
                if (primary_key_time >= 0.0f)
                    state->primary_key_ticks = (long)((float)game_time_globals->ticks_per_second * primary_key_time + 0.5f);
                else
                    state->primary_key_ticks = 0;
                if (unit->object.parent_object_index == NONE) {
                    unit_unzoom(unit_index, false);
                    first_person_weapon_handle_message(unit_index, players_equipment_weapon_get(unit_index),
                        _unit_message_throw_equipment, 0);
                }
                long definition_index = state->definition_index;
                long references[1] = { unit_index };
                bool predicted = false;
                if ((unsigned long)((signed char)game_globals->options.game_simulation - 4) <= 1)
                    predicted = true;
                if (predicted && !game_is_cooperative() && definition_index != NONE) {
                    long controlling_player_index;
                    if (simulation_query_predictive_event(unit_index, 1, &controlling_player_index)) {
                        s_equipment_event event;
                        event.type = 1;
                        event.definition_index = definition_index;
                        simulation_event_generate(_simulation_event_type_unit_grenade_initiate, 1, references,
                            controlling_player_index, sizeof(event), &event,
                g_network_configuration.event_relevance[_simulation_event_type_unit_grenade_initiate].delay_msec);
                    }
                }
                result = true;
            }
        }
    }
    return result;
}


struct s_observer;

static __forceinline void players_spawn_stop_screen_effect(long user_index)
{
    player_effect_datum *effect = &g_player_effect_globals->effects[user_index];
    memset(&effect->flash, 0, sizeof(effect->flash));
    effect->flash_ticks = 0;
    effect->flags &= ~1;
    s_damage_response_screen_tint_definition *values = &effect->tint;
    memset(values, 0, sizeof(effect->tint));
    effect->tint_ticks = 0;
    effect->flags &= 0xfd;
}

template<long k_depth>
static __forceinline unsigned long players_spawn_word_value(unsigned long value)
{
    // match: 0xa79f8: rotate+reference temporary preserves player_spawn register allocation.
    const unsigned long &normalized = _rotl(players_spawn_word_value<k_depth - 1>(value), 0);
    return normalized;
}
template<>
__forceinline unsigned long players_spawn_word_value<0>(unsigned long value)
{
    return value;
}
static __forceinline bool players_spawn_in_limbo(long object_index)
{
    return TEST_BIT(players_spawn_word_value<4>(object_get(object_index)->flags), 7);
}
static __forceinline bool players_spawn_unit_is_dead(long object_index)
{
    return TEST_BIT(object_get(object_index)->damage_flags, 2);
}
static __forceinline long players_spawn_grace_period_seconds()
{
    if (current_game_variant()) {
        long seconds = current_game_variant()->respawn_player_traits_duration_seconds;
        if (seconds > 1)
            return seconds;
    }
    return 1;
}
static __forceinline void players_spawn_set_angles(real_euler_angles2d *angles, float yaw, float pitch)
{
    angles->yaw = yaw;
    angles->pitch = pitch;
}
static __forceinline long players_spawn_seconds_as_ticks(long seconds)
{
    return game_time_globals->ticks_per_second * seconds;
}
// @address 0xa79f8 ?player_spawn@@YA_NJPBTreal_point3d@@PBM@Z
bool player_spawn(long player_index, const real_point3d *position, const float *rotation)
{
    player_datum *player = player_get(player_index);
    bool result = false;
    do {
        switch (game_globals->options.game_mode) {
        case 1: {
            long previous_unit = (long)players_spawn_word_value<2>((unsigned long)player->next_unit_index);
            player->next_unit_index = NONE;
            if (previous_unit != NONE) {
                if (players_spawn_in_limbo(previous_unit))
                    object_set_in_limbo(previous_unit, false);
                if (players_spawn_unit_is_dead(previous_unit)) {
                    object_delete(previous_unit);
                } else {
                    object_activate(previous_unit);
                    object_set_hidden(previous_unit, false);
                    s_spawn_restore_context context = { previous_unit };
                    // match: 0xa79f8: const reference binding preserves temporary lifetime
                    const long &restored_unit = (long)_rotl((unsigned long)context.unit_index, 0);
                    player_set_unit_index(player_index, restored_unit);
                    break;
                }
            }
        }
        // Fall through when there is no surviving campaign unit to restore.
        default: {
            if (game_simulation::game_is_predicted())
                break;
            real_point3d spawn_position;
            float facing_angle;
            real_vector3d facing;
            real_euler_angles2d angles;
            if (!player_find_best_spawn_location(player_index, &spawn_position, &facing_angle, &angles.yaw, true, false))
                continue;
            long unit_definition_index;
            long model_variant_index;
            player_find_player_character_unit_and_variant_info(player_index, &unit_definition_index, &model_variant_index);
            if (unit_definition_index == NONE)
                continue;
            object_placement_data placement;
            bool reused = false;
            long unit_index;
            if (players_globals->camera_target_index != NONE &&
                object_try_and_get_and_verify_type(players_globals->camera_target_index, 0x1003)) {
                if (position)
                    spawn_position = *position;
                vector3d_from_angle(&facing, facing_angle);
                object_set_position_direct(players_globals->camera_target_index, &spawn_position, &facing, global_up3d, 0, false);
                object_set_hidden(players_globals->camera_target_index, false);
                object_activate(players_globals->camera_target_index);
                reused = true;
                unit_index = players_globals->camera_target_index;
                players_globals->camera_target_index = NONE;
            } else {
                object_placement_data_new(&placement, unit_definition_index, NONE, 0);
                float spawn_angle = facing_angle;
                placement.model_variant_index = model_variant_index;
                placement.position = spawn_position;
                vector3d_from_angle(&placement.forward, spawn_angle);
                placement.up = *global_up3d;
                placement.player_index = player_index;
                placement.team_index = player->configuration.host.team_index;
                e_game_team team;
                game_globals_storage *globals = game_globals;
                if (globals->options.game_mode == 2)
                    team = (e_game_team)(game_engine_has_teams() ? player->configuration.host.team_index : NONE);
                else
                    team = _game_team_default;
                real_rgb_color colors[4];
                bool valid;
                const s_player_appearance *appearance = &player->configuration.client.appearance;
                game_engine_get_change_colors(player_index, appearance, team, colors, &valid);
                if (valid) {
                    placement.active_change_colors |= 7;
                    memcpy(placement.change_colors, colors, sizeof(colors));
                }
                if (globals->options.game_mode != 1)
                    model_customization_get_overrides_for_placement(appearance, &placement);
                if (position)
                    placement.position = *position;
                facing = placement.forward;
                unit_index = object_new(&placement);
                if (unit_index == NONE)
                    continue;
                if (angles.yaw != 0.0f) {
                    players_spawn_set_angles(&angles, spawn_angle, angles.yaw);
                    unit_datum *unit = (unit_datum *)object_get(unit_index);
                    vector3d_from_euler_angles2d(&unit->unit.desired_aiming_vector, &angles);
                    facing = unit->unit.desired_aiming_vector;
                }
            }
            if (unit_index == NONE)
                continue;

            player_set_unit_index(player_index, unit_index);
            simulation_action_object_create(unit_index);
            simulation_action_object_update(unit_index, 0x4000);
            player_set_facing(player_index, &facing);
            player_increment_teleport_counter(player_index);
            long game_mode = game_globals->options.game_mode;
            if (game_mode == 2) {
                game_engine_add_starting_equipment(unit_index);
            } else if (!reused) {
                scenario *scenario_definition = global_scenario;
                bool initial_spawn = !TEST_BIT(player->flags, 3);
                unsigned char initial_spawn_flag = initial_spawn;
                bool alternate_model = (player->campaign_player_slot) >= 0 && (player->campaign_player_slot) < 4 && (player->campaign_player_slot) != 0;
                const long &alternate_profile = alternate_model ? 2 : 0;
                const long &initial_profile = initial_spawn_flag != 0;
                long profile_index = initial_profile + alternate_profile;
                bool armed = false;
                if (initial_spawn_flag && !alternate_model && game_mode == 1 &&
                    players_get_active_and_in_game_count(true) > 1) {
                    for (long index = 0; index < scenario_definition->starting_profiles.count; ++index) {
                        const s_scenario_starting_profile *profiles =
                            (const s_scenario_starting_profile *)scenario_definition->starting_profiles.address;
                        if (strncmp(profiles[index].name, "chief_coop_respawn", sizeof(profiles[index].name)) == 0) {
                            profile_index = index;
                            break;
                        }
                    }
                }
                long last_profile = scenario_definition->starting_profiles.count - 1;
                if (profile_index > last_profile)
                    profile_index = last_profile;
                if (!initial_spawn_flag && TEST_BIT(scenario_definition->flags, 5)) {
                    long ordinal = 0;
                    c_player_data_iterator<player_datum> iterator;
                    iterator.begin(player_data);
                    while (iterator.next() && ordinal < 4) {
                        if (iterator.iterator.index == player_index) {
                            const s_persistent_campaign_player *persistent = &game_globals->options.campaign_armaments.players[ordinal];
                            if (persistent && players_arm_unit_from_persistent_campaign_data(persistent, unit_index))
                                armed = true;
                            break;
                        }
                        ++ordinal;
                    }
                }
                if (!armed && profile_index >= 0 && profile_index < scenario_definition->starting_profiles.count)
                    unit_add_starting_profile_equipment(unit_index, (short)profile_index, true, initial_spawn);
            }
            break;
        }
        }
        result = true;
    } while (false);
    if (result) {
        player->invisibility_duration = 0.0f;
        player->invisibility_time = 0;
        player->sprint_ticks = 0;
        player->sprint_recovery_ticks = 0;
        player->flags &= ~0xc000;
        {
            long user_index = NONE;
            for (;;) {
                if (player_index != NONE)
                    user_index = (long)director_get_perspective(player_index, user_index);
                if (user_index == NONE)
                    break;
                chud_motion_sensor_invalidate(user_index);
                players_spawn_stop_screen_effect(user_index);
                observer_clear((s_observer *)&observer_get_globals()->blocks[user_index]);
            }
            long grace_seconds = players_spawn_grace_period_seconds();
            player->grace_ticks = (short)players_spawn_seconds_as_ticks(grace_seconds);
            player->powerup_and_respawn_times[3] = NONE;
            player->b2ae = (char)((player->flags >> 3) & 1);
            if (game_globals->options.game_mode == 1 && TEST_BIT(player->flags, 3) &&
                player->campaign_player_slot != 0 && player->campaign_player_slot != 1) {
                c_player_data_iterator<player_datum> iterator;
                iterator.begin(player_data);
                while (iterator.next()) {
                    player_datum *other = iterator.get_datum();
                    long other_unit_index = other->unit_index;
                    if (other_unit_index != NONE && !TEST_BIT(other->flags, 3)) {
                        real_point3d other_position;
                        object_get_origin(other_unit_index, &other_position);
                        player_fix_unit_position(player_index, other_unit_index, &other_position, true, true);
                        break;
                    }
                }
            }
            if (game_globals->options.game_mode == 1 && game_campaign_is_multiplayer())
                player->flags |= 0x800;
            player->flags &= ~8;
        }
    }
    return result;
}

static __forceinline bool players_coop_cinematic_in_progress()
{
    bool result = false;
    s_cinematic_globals_definition *cinematic = (s_cinematic_globals_definition *)cinematic_globals;
    if (cinematic)
        result = cinematic->in_progress;
    return result;
}
static __forceinline e_vehicle_type players_coop_vehicle_get_type(long object_index)
{
    object_datum *object = object_get(object_index);
    vehicle_definition *definition = (vehicle_definition *)tag_get(0, object->definition_index);
    long result = _vehicle_type_none;
    for (long i = 0; i < k_vehicle_type_count; i++) {
        if (definition->types[i].count != 0) {
            result = i;
            break;
        }
    }
    return (e_vehicle_type)result;
}
static __forceinline bool players_coop_all_players_endangered(unsigned long mask)
{
    bool result = true;
    c_player_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if (player->unit_index != NONE && !TEST_BIT(mask, (short)iterator.iterator.index)) {
            result = false;
            break;
        }
    }
    return result;
}
static __forceinline s_players_globals *players_coop_globals_get()
{
    return players_globals;
}
// @address 0x1d9478 player_update_coop_respawn
extern "C" void player_update_coop_respawn(long player_index)
{
    player_datum *player = player_get(player_index);
    if (player->coop_respawn_delay_ticks > 0 && !players_coop_cinematic_in_progress())
        player->coop_respawn_delay_ticks--;
    long ticks_per_second = game_time_globals->ticks_per_second;
    if (player->coop_respawn_delay_ticks == 3 * ticks_per_second || player->coop_respawn_delay_ticks == 2 * ticks_per_second ||
        player->coop_respawn_delay_ticks == ticks_per_second)
        unspatialized_impulse_sound_new(((s_player_information_definition
            *)global_game_globals->player_information.address)->respawn_countdown_sound.index, 1.0f);
    else if (player->coop_respawn_delay_ticks == 1)
        unspatialized_impulse_sound_new(((s_player_information_definition *)global_game_globals->player_information.address)->respawn_final_sound.index, 1.0f);
    long dangerous_unit_index = NONE;
    if (player->coop_respawn_pending) {
        unsigned long endangered_players = 0;
        if (players_coop_cinematic_in_progress())
            players_coop_globals_get()->respawn_failure = 2;
        else if (players_coop_globals_get()->respawn_unsafe)
            players_coop_globals_get()->respawn_failure = 3;
        else if (!player->coop_respawn_initial) {
            if (player->coop_respawn_delay_ticks > 0)
                players_coop_globals_get()->respawn_failure = 4;
            else if (game_globals->lost)
                players_coop_globals_get()->respawn_failure = 9;
            else if (any_unit_is_dangerous(&dangerous_unit_index))
                players_coop_globals_get()->respawn_failure = 5;
            else {
                actor_endangering_player(true, true, &endangered_players);
                if (endangered_players && players_coop_all_players_endangered(endangered_players))
                    players_coop_globals_get()->respawn_failure = 6;
            }
        }
        if (players_coop_globals_get()->respawn_failure == 0) {
            long unit_index = NONE;
            real_point3d position;
            c_player_data_iterator<player_datum> iterator;
            iterator.begin(player_data);
            while (iterator.next()) {
                player_datum *other_player = iterator.get_datum();
                if (other_player->unit_index != NONE &&
                    !TEST_BIT(endangered_players, (short)iterator.iterator.index)) {
                    long ultimate_parent_index = other_player->unit_index;
                    long parent_index;
                    do {
                        parent_index = ultimate_parent_index;
                        ultimate_parent_index = object_get(ultimate_parent_index)->parent_object_index;
                    } while (ultimate_parent_index != NONE);
                    ultimate_parent_index = parent_index;
                    bool in_air = false;
                    if (ultimate_parent_index == other_player->unit_index) {
                        if (TEST_BIT(1, object_get_type(other_player->unit_index)))
                            in_air = motor_system_in_airborne_state(other_player->unit_index);
                    } else if (TEST_BIT(2, object_get_type(ultimate_parent_index))) {
                        vehicle_datum *vehicle = (vehicle_datum *)object_get(ultimate_parent_index);
                        if (players_coop_vehicle_get_type(ultimate_parent_index) != _vehicle_type_turret)
                            in_air = vehicle->vehicle.airborne_ticks > 0;
                    }
                    if (!in_air) {
                        unit_index = other_player->unit_index;
                        object_get_center_of_mass(unit_index, &position);
                    } else
                        players_coop_globals_get()->respawn_failure = 7;
                }
            }
            if (unit_index != NONE && !TEST_BIT(player->flags, 1) && TEST_BIT(player->flags, 0) &&
                player->unit_index == NONE) {
                bool success = player_spawn(player_index, 0, 0);
                success = success && player_move_unit_to_position(player_index, unit_index, true, &position);
                player->flags |= 0x800;
                if (!success) {
                    players_coop_globals_get()->respawn_failure = 11;
                    if (player->unit_index != NONE) {
                        if (!(bool)((object_get(player->unit_index)->flags >> 7) & 1))
                            object_set_in_limbo(player->unit_index, true);
                        player->next_unit_index = player->unit_index;
                        player_set_unit_index(player_index, NONE);
                    }
                }
                player->coop_respawn_pending = !success;
            }
        }
    } else if (game_globals->options.game_mode == 1 && players_joined_in_progress_and_waiting_to_respawn())
        players_coop_globals_get()->respawn_failure = 10;
}

t_player_data_allocation g_player_data_allocator;
t_players_globals_allocation g_players_globals_allocator;
t_player_mapping_globals_allocation g_player_mapping_globals_allocator;

