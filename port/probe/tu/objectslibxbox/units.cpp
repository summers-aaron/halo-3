// objectslibxbox/units.obj: from the records 0x99650 0x99d70 0x9a2c8 0x9a6a8 0xa02a0 0xa0328 0xa1a18 0xa1cb8 0xa1d78 0xa1fd8 0xa8730 0xa8828 0xadef0 0xadf48 0xb5d30 0xbf368 0xc04a8 0xc17e8 0xd7830 0xd8058 0xd8120 0xd83c0 0xd84b0 0xd8688 0xd9170 0xd9308 0xd9470 0xd9970 0xd9cc8 0xda138 0xda2e8 0xda370 0xda660 0xe83c0 0xf52c8 0x1031d8 0x110d00 0x11e598 0x122b60 0x12ba38 0x12f438 0x12f520 0x130598 0x1dcbf0 0x20d080 0x321558 0x3216a8 0x3216e8 0x321898 0x3220e0 0x322150 0x322390 0x322500 0x322f40 0x3231b0 0x323978 0x323bc8 0x323d58 0x323f00 0x323fd8 0x324270 0x3243d8 0x3244b8 0x324588 0x324878 0x324a88 0x324c20 0x324f70 0x325000 0x325178 0x325260 0x325780 0x325898 0x3259f0 0x325ba8 0x325f28 0x3269b8 0x326df8 0x327258 0x3272c8 0x3273c0 0x327440 0x327610 0x327738 0x327838 0x3278a0 0x327ab0 0x327b38 0x327bb0 0x327e80 0x327fa0 0x328108 0x328220 0x3282f0 0x328378 0x328558 0x328738 0x3287f0 0x328908 0x329008 0x329148 0x3291b0 0x329390 0x329a20 0x329b30 0x329bf8 0x32a060 0x32a180 0x32a300 0x32a6b0 0x32a728 0x32a7f8 0x32a908 0x32a940 0x32abd8 0x32b010 0x32b298 0x32b440 0x32b820 0x32b890 0x32b900 0x32c598 0x32c7b0 0x32cb90 0x32ccc8 0x32cfe0 0x32d1d8 0x32d330 0x32d370 0x32d620 0x32d770 0x32d940 0x32dae0 0x32db50 0x32dbb8 0x32dc58 0x32dda8 0x32dfe0 0x32e0e0 0x32e1e0 0x32e278 0x32e350 0x32e440 0x32e608 0x32e728 0x32e838 0x32e920 0x32ead0 0x348600 0x353218 0x35be58 0x566488
#include "game/players_global_data.h"
#include "math/real_math_constant_pool.h"
// @address 0x32938 k_real_constants_minus_one
#include "physics/havok.h"
#include "ai/character_behavior_properties.h"
// @flags /TP /O2 /Oc
#include "game/player_control_globals.h"
#include "game/player_training_tls.h"
#include "objects/damage.h"
#include "objects/damage_reporting_types.h"
#include "objects/object_soft_ceiling.h"
#include "physics/havok_constants_types.h"
#include "physics/havok_group_types.h"
#include "units/weapon_addition_method_types.h"
#include "game/game_engine_list.h"
#include "math/matrix_math_functions.h"
#include "math/matrix_math.h"
#include "simulation/simulation_queue_event_types.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "physics/collision_types.h"
#include "math/real_math.h"
#include "math/real_math_trigonometry.h"
#include "math/real_constant_pool_access.h"
#include "math/matrix_math_types.h"
#include "cseries/cseries_wrapped.h"
#include "memory/static_arrays.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"
#include "animations/animation_id.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include "objects/object_iterator.h"
#include "objects/object_placement.h"
#include "units/unit_seat.h"
#include "units/unit_control.h"
#include "units/unit_types.h"
#include "units/unit_animation_types.h"
#include "units/unit_definitions.h"
#include "items/weapon_definition_access.h"
#include "units/biped_types.h"
#include "units/unit_passenger_iterator.h"
#include "units/motor_enums.h"
#include "units/action_request.h"
#include "units/motor_program_state.h"
#include "units/motor_task.h"
#include "ai/actor_globals.h"
#include "ai/actor_datum.h"
#include "game/game_time_types.h"
#include "ai/actors.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "motor/action_system.h"
#include "units/vehicle_auto_control.h"
#include "units/unit_animation_key_frame_status_types.h"
#include "scenario/scenario_tag.h"
#include "ai/sector_ref.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "sound/game_sound.h"
#include "units/units.h"
// @address 0x73d504 g_unit_entering_state_names
// @address 0x74b910 g_havok_monitor_globals
#include "interface/first_person_weapon_tls.h"
#include "ai/actor_stimulus.h"
#include "ai/ai.h"
#include "ai/ai_vehicles.h"
#include "camera/first_person_camera.h"
#include "devices/devices.h"
#include "devices/machine_datum.h"
#include "game/game_engine_notifications.h"
#include "interface/first_person_weapons.h"
#include "items/equipment.h"
#include "items/items.h"
#include "items/weapons.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "models/models.h"
#include "motor/motor_system.h"
#include "motor/mover.h"
#include "objects/damage_owner.h"
#include "objects/object_early_movers.h"
#include "physics/breakable_surfaces.h"
#include "physics/collisions.h"
#include "physics/havok_utilities.h"
#include "units/bipeds.h"
#include "units/unit_animation.h"
#include "units/unit_dialogue.h"
#include "units/vehicles.h"
#include "interface/chud/chud.h"
#include "items/projectiles.h"
#include "units/vehicle_type_component.h"
#include "objects/object_datum.h"
#include "ai/character_definitions.h"
#include "game/game_allegiance.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "tag_files/string_ids.h"
#include "objects/object_header.h"
#include "ai/unit_dialogue_types.h"
#include "motor/mover_action_state.h"
#include "game/game_engine_default.h"
#include "networking/network_configuration_types.h"
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_globals.h"
#include "units/unit_seat_source.h"
#include "game/player_masks_types.h"
#include "game/materials_definitions.h"
#include "units/unit_interface_state_types.h"
#include "units/scenario_unit_datum_types.h"
#include "cache/cache_files.h"
#include "game/game_globals.h"
#include "networking/network_configuration.h"
#include "simulation/game_interface/simulation_game_events.h"
#include "physics/havok_component.h"
#include "game/player_mapping_types.h"
#include "cache/cache_file_globals.h"
#include "units/vehicle_type_turret.h"
#include "items/equipment_definitions.h"
#include "items/weapon_definition_flags.h"
#include "physics/havok_ray_test_types.h"
#include "units/unit_seat_flags.h"
#include "ai/behavior_posture_info_types.h"
#include "game/player_in_game_iterator.h"
#include "objects/damage_data.h"
#include "objects/damage_aftermath_types.h"
#include "motor/mixing_board_component_types.h"
#include "units/unit_inventory_types.h"
#include "effects/material_effect_types.h"
#include "effects/player_effects.h"
#include "effects/effect_creation.h"
#include "effects/effects.h"
#include "interface/first_person_weapons_externs.h"
#include "text/unicode_inline.h"
#include "tag_files/tag_groups.h"
#include "memory/tls_globals.h"
#include "game/game_engine_scoring_globals.h"
#include "ai/character_weapon_properties.h"
#include "physics/character_physics_mode_dead.h"
#include "physics/character_physics_mode_melee.h"
#include "physics/physics_model_definition_types.h"
#include "units/biped_definitions.h"

#include "camera/observer_globals_types.h"
#include "memory/data.h"
#include "ai/weapon_datum_types.h"
#include "items/item_types.h"
#include "units/vehicle_datum.h"
#include "camera/director.h"
#include "physics/havok_component_class.h"
#include "animations/animation_manager.h"
#include "animations/mixing_board/mixing_board_definition.h"
template<long k_bits> class t_short_fixed;
struct unit_passenger_iterator;
struct euler_axis_plan;
struct s_game_sound_deterministic_permutation;
#include "sound/game_sound_impulse.h"
// @address 0xae678 ?normalize2d@@YAMPATreal_vector2d@@@Z
// @address 0xa05d8 ?weapon_reloading@@YA_NJJPAF@Z
// @address 0x34acb0 ?weapon_can_be_fired@@YA_NJ@Z
// @address 0x3512d0 ?weapon_has_infinite_ammo@@YA_NJ@Z
// @address 0xe2c20 ?object_wake@@YAXJ@Z
// @address 0x323978 ?unit_update_active_camouflage@@YA_NJ@Z
// @address 0x3259f0 ?unit_handle_weapon_state_change@@YAXJJJ@Z
// @address 0x32e278 unit_update_dual_wield_weapons
// @address 0x34b158 ?weapon_can_be_dual_wielded@@YA_NJ@Z
// @address 0x325780 ?unit_check_ping_thresholds@@YAXJMMPAJ@Z
// @address 0x325898 ?unit_compute_ping_type@@YAJJ_N00000MM@Z
// @address 0x32e1e0 unit_get_boarding_progress
// @address 0xd9470 ?unit_weapon_next_index@@YAFJJFF@Z
// @address 0x32cb90 ?unit_start_running_blindly@@YAXJ@Z
// @address 0x3291b0 ?unit_can_use_weapon_in_seat@@YA_NJJJ@Z
// @address 0x328378 ?unit_can_pickup_equipment@@YA_NJJ@Z
// @address 0xadef0 ?unit_get_head_position@@YAXJPATreal_point3d@@@Z
// @address 0xadf48 ?unit_get_body_position@@YA_NJPATreal_point3d@@@Z
// @address 0xa02a0 ?unit_inventory_get_must_be_readied_weapon@@YAFJ@Z
// @address 0xa1a18 ?unit_inventory_next_grenade@@YAFJFF@Z
// @address 0xa1cb8 ?begin@c_multi_unit_passenger_iterator@@QAAXJ@Z
// @address 0xa1d78 ?next@c_multi_unit_passenger_iterator@@QAA_NXZ
// @address 0xb5d30 ?unit_seat_get_occupant@@YAJJF@Z
// @address 0xda2e8 ?unit_compute_boost_fraction@@YAMJ@Z
// @address 0xda370 unit_vehicle_is_flying
// @address 0xe83c0 ?limit3d@@YA_NPATreal_vector3d@@M@Z
// @address 0xf52c8 ?unit_override_damage_info_material_type@@YA?AVc_global_material_type@@JV1@@Z
// @address 0x1031d8 ?unit_passenger_iterator_next@@YAPAUunit_datum@@PAUunit_passenger_iterator@@@Z
// @address 0x12f438 ?unit_build_unit_interface_state@@YAXJPAUunit_interface_state@@@Z
// @address 0x12f520 chud_scripting_is_element_enabled
// @address 0x130598 ?unit_get_current_backpack_weapon@@YAJJ@Z
// @address 0x327838 ?unit_is_dying@@YA_NJ@Z
// @address 0x327ab0 ?unit_get_driver_seat@@YAFJ@Z
// @address 0x329a20 ?unit_test_pickup_with_mode@@YA_NJPBUunit_weapon_pickup_result@@W4e_weapon_addition_method@@PAF@Z
// @address 0x32e920 ?unit_get_all_seats_internal@@YAXJPAUunit_seat_source@@PAFFE@Z
// @address 0x327b38 ?unit_seat_test_flag@@YA_NJFW4e_unit_seat_flags@@@Z
// @address 0x353218 unit_dialogue_set_variant
// @address 0x9a2c8 ?unit_get_active_primary_weapon@@YAJJPAJ@Z
// @address 0x20d080 ?set_wchar@?$c_static_string@$0CA@@@QAAXPB_W@Z
// @address 0x324f70 ?unit_get_current_seat@@YAPAUunit_seat@@J@Z
// @address 0x329148 ?unit_has_weapon_definition_index@@YA_NJJ@Z
// @address 0x32a908 ?unit_handle_deleted_player@@YAXJJ@Z
// @address 0x32d330 ?unit_detach_simulation_entity@@YAXJ@Z
// @address 0x3269b8 ?unit_record_damage@@YAXJMF_NJW4e_game_team@@JE@Z
// @address 0x3244b8 ?unit_has_seat_animation@@YA_NJJF@Z
// @address 0x32dbb8 ?unit_active_camouflage_ding@@YAXJMM@Z
// @address 0x32e0e0 ?unit_correct_dual_wield_state@@YAXJJ_N00@Z
// @address 0x12ba38 ?unit_get_zoom_magnification@@YAMJF@Z
// @address 0x329008 ?unit_put_away_weapon@@YA_NJF_N@Z
// @address 0x32e440 ?data_mine_add_player_death_with_killer_data@@YAXPAVc_datamine@@JPBUs_damage_aftermath_result_data@@@Z
// @address 0x32e608 ?data_mine_insert_sp_actor_death@@YAXJPBUs_damage_aftermath_result_data@@@Z
#include <ppcintrinsics.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define FLAG_TEST(field, bit) ((bool)(((field) >> (bit)) & 1))


/* ---- math ---- */


/* ---- tag instances and definitions ---- */
#include "game/player_information_definition.h"
#include "game/campaign_metagame_definitions.h"
// This unit reads the shield-stun value following the 0x44-byte scoring definition.
struct s_emp_stun_globals
{
    s_campaign_metagame_definition scoring;
    float shield_stun_time;
};
typedef char emp_stun_shield_stun_offset[(offsetof(s_emp_stun_globals, shield_stun_time) == 0x44) ? 1 : -1];
/* 20 bytes; the seat and definition blocks both hold one */



/* ---- object headers: the data array at object_header_data ---- */

/* ---- other game data ---- */
#include "motor/player_melee_action_state.h"
#include "units/unit_passenger_iterator_types.h"
#include "interface/chud/chud_scripting_globals_types.h"
#include "units/unit_weapon_pickup_result.h"


#include "math/real_constant_pool.h"


/* functions outside this unit */



#include "game/player_control_types.h"



#include "units/unit_inventory_inline.h"
#include "motor/motor_animation_state.h"
#include "game/game_time_inline.h"
#include "game/game_time_conversion.h"
#include "units/unit_facing.h"
#include "units/vehicle_definitions.h"
#include <new>
#include "tag_files/tag_index.h"
#include "game/players_globals_types.h"
#include "objects/light_globals.h"
#include "game/player_training_types.h"
#include "game/player_training_globals.h"
#include "objects/object_definitions.h"
#include "objects/damage_effect_definitions.h"
#include "effects/material_effects.h"
#include "objects/damage_response_types.h"
#include "cseries/data_mining_event_types.h"
#include "models/model_types.h"
#include "units/dialogue_definitions.h"
#include "units/unit_player_melee.h"
#include "math/real_math_constants.h"
#include "units/motor_request.h"
#include "cseries/crt_xdk.h"
#include "objects/damage_inline.h"
#include "items/item_definitions.h"
#include "interface/chud/chud_globals_definitions.h"
#include "interface/chud/chud_navpoints_runtime.h"
#include "units/units_runtime_dependencies.h"
#include "scenario/scenario_starting_profile.h"
#include "items/weapon_control_flags.h"
#include "interface/first_person_weapon_runtime_types.h"
#include "ai/actor_posture.h"
#include "physics/collision_vector_query.h"
#include "objects/object_broadphase_globals.h"
#include "units/vehicle_type_component_internals.h"
#include "models/model_damage_info.h"
#include "objects/object_acceleration.h"
#include "game/game_globals_inline.h"
#include "game/aim_assist.h"
#include "game/player_access.h"
#include "game/game_engine_get_inline.h"
#include "scenario/cluster_reference_helpers.h"
#include "objects/object_hierarchy.h"
#include "networking/simulation/simulation_accessors.h"
#include "memory/data_inline.h"
#include "game/game_simulation_predicates.h"
#include "objects/object_type_access.h"
#include "objects/object_animation_manager_inline.h"

struct s_seat_acceleration_entry { float seconds; float scale; };

struct euler_axis_plan
{
    bool stationary;
    float position;
    float velocity;
    float acceleration;
    float accel_t;
    float coast_t;
    float deceleration;
    float decel_t;
};


struct s_units_dialogue_state {
    long tag;
    long value;
    long other;
};





struct s_units_new_animation_state
{
    char prefix[0x30];
    long last_animation_index;
};

struct s_units_new_dialogue_state
{
    long tag;
    char prefix[0x48 - 4];
    long last_sound_index;
};


struct s_unit_died_dialogue_state {
    char prefix[14];
    signed char value;
};

// The dialogue block at unit->unit.dialogue_offset carries the live tag/value/variant triple while the unit
// speaks and exposes the death-gating value at byte 14 once it dies (unit_died reads ->died.value).
union u_unit_dialogue_state
{
    s_units_dialogue_state live;
    s_unit_died_dialogue_state died;
};

union s_melee_damage_reporting_info
{
    // type is the high six bits and modifier the low two (damage_reporting_info_get_type: reporting_info >> 2;
    // damage.cpp: reporting_info & ~3 | 1); MSB-first on the console, LSB-first on x86.
    struct
    {
#if defined(_XBOX) && !defined(__clang__)
        unsigned char type : 6;
        unsigned char modifier : 2;
#else
        unsigned char modifier : 2;
        unsigned char type : 6;
#endif
    } fields;
    unsigned char value;
};

struct s_pickup_weapon_event
{
    short type;
    struct s_weapon
    {
        long definition_index;
        long unknown;
        __forceinline s_weapon() { memset(this, 0, sizeof(*this)); }
    } weapon;
};

enum {
    _units_pickup_object_mask_weapon = 0x4,
    _units_pickup_object_mask_unit = 0x2003,
    _units_pickup_weapon_tag = 0x77656170   /* 'weap' */
};




static __forceinline void units_player_handle_weapon_put_away(long player_index, short weapon_set_index)
{
    long control_index = player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player_index)];
    if (control_index != NONE) {
        s_player_control_user_state *control = &((s_player_control_globals *)player_control_globals)->users[control_index];
        if (control->switch_weapon_held)
            control->switch_weapon_consumed = true;
        control->action.weapons.set_identifier = NONE;
    }
}

#define units_action_request_new(request, request_type) (memset((request), 0, sizeof(*(request))), (request)->type = (request_type))



/* ---- helpers ---- */
static __forceinline unit_datum *unit_get(long unit_index)
{
    return (unit_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(unit_index)].datum;
}
static __forceinline c_animation_manager *object_get_motor(long object_index)
{
    unit_datum *object = unit_get(object_index);
    return (c_animation_manager *)((char *)object + object->object.animation.offset);
}


static __forceinline void *unit_damage_datum_try_and_get(const s_data_array *data, long index)
{
    void *result = 0;
    if (index != NONE) {
        long absolute_index = (unsigned short)index;
        long identifier = index >> 16;
        if (absolute_index < data->first_unallocated) {
            char *datum = data->data + absolute_index * data->size;
            short datum_identifier = *(short *)datum;
            if (datum_identifier != 0 && datum_identifier == identifier)
                result = datum;
        }
    }
    return result;
}
static __forceinline unit_datum *unit_damage_try_get(long object_index, unsigned long type_mask)
{
    unit_datum *result = 0;
    s_object_header *header = (s_object_header *)unit_damage_datum_try_and_get((s_data_array *)object_header_data, object_index);
    if (header && (type_mask & (1 << header->object_type)) != 0)
        result = (unit_datum *)header->datum;
    return result;
}

static __forceinline bool unit_damage_ai_handle_killing_spree(long unit_index, short count)
{
    bool result = false;
    if (ai_globals->enabled) {
        unit_datum *unit = unit_get(unit_index);
        short threshold = (short)(unit->unit.player_index != NONE ? 5 : 3);
        if (count >= threshold)
            result = true;
    }
    return result;
}

/* ---- game globals and action requests (shared by the functions below) ---- */

/* Declarations shared by the natural additions. */
// @address 0xed230 ?mover_is_in_entering_state@@YA_NJ@Z

static __forceinline bool units_object_is_being_deleted(long index)
{
    return (object_header_get(index)->flags & 16) != 0;
}


// @address 0xedff8 ?action_submit@@YA_NJPBUaction_request@@@Z
// @address 0xe9a90 ?character_definition_vitality_properties_get@@YAPAUcharacter_vitality_properties@@J@Z
// @address 0x110a98 ?object_detach@@YAXJ@Z
// @address 0x9cb80 ?object_get_velocities@@YAXJPATreal_vector3d@@0@Z
// @address 0x329bf8 ?unit_seat_acceleration_update@@YAXJ@Z
// @address 0x34ad70 ?weapon_is_zoomable@@YA_NJ@Z

// @address 0x34b6b0 ?weapon_get_trigger_position@@YA_NJFPATreal_point3d@@@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x322500 ?unit_update_weapons@@YA_NJ@Z
// @address 0xd9638 ?unit_dialogue_update@@YAXJ@Z
// @address 0xa00b0 ?angle_between_normals3d@@YAMPBTreal_vector3d@@0@Z
// @address 0xa7860 ?game_is_predicted@@YA_NXZ
// @address 0x31ac78 ?object_disconnect_from_physics@@YAXJ@Z

// 0x99650 natural source



bool unit_adjust_first_person_camera(long unit_index, real_vector3d *vector)
{
    unit_datum *unit = unit_get(unit_index);
    bool result = false;
    if (unit->unit.player_index != NONE && unit->unit.parent_seat_index != NONE)
    {
        unit_datum *parent = unit_get(unit->object.parent_object_index);
        unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
        unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
        if (!FLAG_TEST(seat->flags, 4))
        {
            real_vector3d forward;
            real_vector3d up;
            real_matrix4x3 matrix;
            real_vector3d right = { 0.0f, -1.0f, 0.0f };
            real_vector3d down = { 0.0f, 0.0f, -1.0f };
            ::object_get_orientation(unit->object.parent_object_index, &forward, &up);
            cross_product3d(&up, &right, &matrix.forward);
            if (normalize3d(&matrix.forward) == 0.0f)
            {
                cross_product3d(&up, &down, &matrix.forward);
                normalize3d(&matrix.forward);
            }
            matrix4x3_rotation_from_vectors(&matrix, &matrix.forward, &up);
            matrix4x3_transform_normal(&matrix, vector, vector);
            result = true;
        }
    }
    return result;
}

// 0x99d70 natural source
// @address 0x99d70 ?unit_get_aiming_unit_index@@YAJJ@Z
long unit_get_aiming_unit_index(long unit_index)
{
    if (unit_index != NONE)
    {
        unit_datum *unit = unit_get(unit_index);
        if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE)
        {
            unit_datum *parent = unit_get(unit->object.parent_object_index);
            unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
            unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
            if (FLAG_TEST(seat->flags, 0) || FLAG_TEST(seat->flags, 3))
                unit_index = unit->object.parent_object_index;
            long child_index = parent->object.first_child_object_index;
            while (child_index != NONE)
            {
                unit_datum *child = (unit_datum *)object_try_and_get_and_verify_type(child_index, 0x1003);
                if (child)
                {
                    unit_definition *child_definition = (unit_definition *)tag_get(0, child->object.definition_index);
                    if (FLAG_TEST(child_definition->flags, 27) && FLAG_TEST(child->object.object_flags, 26))
                    {
                        unit_index = child_index;
                        break;
                    }
                }
                child_index = unit_get(child_index)->object.next_object_index;
            }
        }
    }
    return unit_index;
}

// 0x9a2c8 natural source
long unit_get_active_primary_weapon(long unit_index, long *owner_unit_index)
{
    long result = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index);
    if (result != NONE) {
        if (owner_unit_index)
            *owner_unit_index = unit_index;
    } else {
        unit_datum *unit = unit_get(unit_index);
        long child_index = unit->object.first_child_object_index;
        while (child_index != NONE) {
            unit_datum *child = unit_get(child_index);
            if (((1 << child->object.object_type) & 0x1003) != 0) {
                unit_datum *child_unit = child;
                unit_definition *definition = (unit_definition *)tag_get(0, child_unit->object.definition_index);
                if (FLAG_TEST(definition->flags, 27)) {
                    result = unit_get_active_primary_weapon(child_index, owner_unit_index);
                    if (result != NONE)
                        break;
                }
            }
            child_index = child->object.next_object_index;
        }
    }
    return result;
}

// 0x9a6a8 natural source
// @address 0x9a6a8 ?unit_get_camera_position@@YAXJPATreal_point3d@@@Z
static __forceinline long units_camera_motor_animation_get_state(long object_index)
{
    unit_datum *unit = unit_get(object_index);
    c_animation_manager *motor = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
    long state = motor->get_state_name();
    return state;
}
void unit_get_camera_position(long unit_index, real_point3d *position)
{
    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    bool success = true;
    if (unit->object.parent_object_index == NONE && !(bool)(_rotl(unit->object.damage_flags, 30) & 1) && unit->object.object_type == 0)
    {
        biped_estimate_camera_position(unit_index, 0, 1, 0, 0, 0, 0, position);
    }
    else if (unit->object.parent_object_index == NONE)
    {
        bool found = false;
        if (unit->unit.gunner_object_index != NONE)
        {
            unit_datum *gunner = unit_get(unit->unit.gunner_object_index);
            if (gunner->unit.parent_seat_index != NONE)
            {
                unit_seat *seat = &definition->seats[gunner->unit.parent_seat_index];
                object_marker marker;
                internal_object_get_markers_by_string_id(unit_index, seat->marker_name, &marker, 1, false);
                *position = marker.matrix.position;
                found = true;
            }
        }
        if (!found)
        {
            object_marker marker;
            internal_object_get_markers_by_string_id(unit_index, 157, &marker, 1, false);
            *position = marker.matrix.position;
        }
    }
    else
    {
        object_get_center_of_mass(unit->object.parent_object_index, position);
        if (unit->unit.parent_seat_index != NONE)
        {
            unit_datum *parent = unit_get(unit->object.parent_object_index);
            unit_definition *parent_definition = (unit_definition *)tag_get(0, parent->object.definition_index);
            unit_seat *seat = &parent_definition->seats[unit->unit.parent_seat_index];
            if (parent->object.object_type != 1 || seat->camera_marker_name != 0)
            {
                object_marker marker;
                internal_object_get_markers_by_string_id(unit->object.parent_object_index, seat->camera_marker_name, &marker, 1, false);
                *position = marker.matrix.position;
            }
            if (unit->object.object_type == 0 && !FLAG_TEST(seat->flags, 4))
            {
                bool use_head = mover_is_in_entering_state(unit_index);
                if (!use_head)
                {
                    long state = units_camera_motor_animation_get_state(unit_index);
                    if (state == 185 || state == 85 || state == 1190 || state == 184)
                        use_head = true;
                }
                if (use_head)
                {
                    object_marker marker;
                    if (internal_object_get_markers_by_string_id(unit_index, 157, &marker, 1, false) > 0)
                        *position = marker.matrix.position;
                }
            }
        }
    }
}

short unit_inventory_get_must_be_readied_weapon(long object_index)
{
  short result = -1;
  unit_datum *object = unit_get(object_index);
  for (long i = 0; i < 4; i++) {
    long attachment_index = object->unit.attachments[i];
    if (attachment_index != -1) {
      unit_datum *attachment = unit_get(attachment_index);
      bool flag =
          (((weapon_definition *)tag_get(0, attachment->object.definition_index))
                  ->flags >>
              3) &
          1;
      if (flag) {
        result = (short)i;
        break;
      }
    }
  }
  return result;
}

// 0xa0328 natural source: unit_can_zoom

static __forceinline bool unit_seat_allows_zoom(const unit_seat *seat)
{
    bool result = (seat->flags & (1 << 3)) != 0;
    return result;
}

bool unit_can_zoom(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    bool can_zoom = true;
    if (unit->unit.player_index == NONE)
        can_zoom = false;
    else if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE)
    {
        unit_datum *parent = unit_get(unit->object.parent_object_index);
        unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
        unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
        if (!unit_seat_allows_zoom(seat))
            can_zoom = false;
    }
    else if (action_executing(unit_index, _action_melee_ai) || action_executing(unit_index, _action_melee_player))
        can_zoom = false;
    else if (action_executing(unit_index, _action_grenade) || action_executing(unit_index, _action_equipment))
        can_zoom = false;
    else if (action_executing(unit_index, _action_primary_weapon_switch) || action_executing(unit_index, _action_secondary_weapon_switch))
        can_zoom = false;
    else if (action_executing(unit_index, _action_primary_weapon_primary_reload) || action_executing(unit_index, _action_primary_weapon_secondary_reload) ||
             action_executing(unit_index, _action_secondary_weapon_primary_reload) || action_executing(unit_index, _action_secondary_weapon_secondary_reload))
        can_zoom = false;
    if (can_zoom)
    {
        for (long hand_index = 0; hand_index < 2; hand_index++)
        {
            long weapon_index = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_indices[hand_index]);
            if (weapon_index != NONE && weapon_reloading(weapon_index, NONE, 0))
            {
                can_zoom = false;
                break;
            }
        }
    }
    return can_zoom;
}

short unit_inventory_next_grenade(long unit_index, short current, short direction)
{
  unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
  short result = -1;
  if (current == -1)
    current = 0;
  short index = current;
  do {
    if (unit->unit.grenade_counts[index] > 0) {
      result = index;
      if (index != current || direction == 0)
        break;
    }
    if (direction < 0)
      index = index == 0 ? 3 : index - 1;
    else
      index = index == 3 ? 0 : index + 1;
  } while (index != current);
  return result;
}

void c_multi_unit_passenger_iterator::begin(long unit_index)
{
    c_multi_unit_passenger_iterator *iterator = this;
    unit_datum *unit = unit_get(unit_index);
    bool again;
    do {
        again = false;
        if (unit->object.parent_object_index != NONE
            && (bool)((unit->object.object_flags >> 26) & 1)
            && !(bool)((((unit_definition *)tag_get(0, unit->object.definition_index))->flags >> 19) & 1)
            && ((1 << unit_get(unit->object.parent_object_index)->object.object_type) & 0x1003)) {
            unit_index = unit->object.parent_object_index;
            unit = unit_get(unit_index);
            again = true;
        }
    } while (again);
    long next_object_index = unit->object.first_child_object_index;
    iterator->m_root_unit_index = unit_index;
    iterator->m_unit_index = iterator->m_root_unit_index;
    iterator->m_next_object_index = next_object_index;
}

bool c_multi_unit_passenger_iterator::next()
{
    c_multi_unit_passenger_iterator *iterator = this;
    bool result = false;
    if (iterator->m_root_unit_index == iterator->m_unit_index) {
        if (iterator->m_next_object_index != NONE) {
            // match: 0xa1d78: restrict prevents alias reloads across iterator writes
            s_object_header *__restrict headers = (s_object_header *)object_header_data->data;
            while (iterator->m_next_object_index != NONE) {
                long object_index = iterator->m_next_object_index;
                // match: 0xa1d78: restrict prevents alias reloads across iterator writes
                unit_datum *__restrict object = (unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
                signed char type = object->object.object_type;
                iterator->m_next_object_index = object->object.next_object_index;
                if ((1 << type) & 0x1003) {
                    // match: 0xa1d78: restrict prevents alias reloads across iterator writes
                    unit_datum *__restrict unit = object;
                    result = true;
                    iterator->m_passenger_index = object_index;
                    iterator->m_seat_index = unit->unit.parent_seat_index;
                    iterator->m_parent_index = object->object.parent_object_index;
                    break;
                }
            }
        }
        if (!result) {
            // match: 0xa1d78: restrict prevents alias reloads across iterator writes
            s_object_header *__restrict headers = (s_object_header *)object_header_data->data;
            long object_index = ((unit_datum *)headers[datum_index_to_absolute_index(iterator->m_root_unit_index)].datum)->object.first_child_object_index;
            while (object_index != NONE) {
                // match: 0xa1d78: restrict prevents alias reloads across iterator writes
                unit_datum *__restrict object = (unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
                if (((1 << object->object.object_type) & 0x1003) && FLAG_TEST(object->object.object_flags, 26))
                    break;
                object_index = object->object.next_object_index;
            }
            iterator->m_unit_index = object_index;
            if (iterator->m_unit_index != NONE)
                iterator->m_next_object_index = ((unit_datum *)headers[datum_index_to_absolute_index(iterator->m_unit_index)].datum)
                    ->object.first_child_object_index;
        }
    }
    if (iterator->m_root_unit_index != iterator->m_unit_index) {
        while (iterator->m_unit_index != NONE) {
            if (iterator->m_next_object_index != NONE) {
                // match: 0xa1d78: restrict prevents alias reloads across iterator writes
                s_object_header *__restrict headers = (s_object_header *)object_header_data->data;
                while (iterator->m_next_object_index != NONE) {
                    long object_index = iterator->m_next_object_index;
                    // match: 0xa1d78: restrict prevents alias reloads across iterator writes
                    unit_datum *__restrict object = (unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
                    signed char type = object->object.object_type;
                    iterator->m_next_object_index = object->object.next_object_index;
                    if ((1 << type) & 0x1003) {
                        // match: 0xa1d78: restrict prevents alias reloads across iterator writes
                        unit_datum *__restrict unit = object;
                        result = true;
                        iterator->m_passenger_index = object_index;
                        iterator->m_seat_index = unit->unit.parent_seat_index;
                        iterator->m_parent_index = object->object.parent_object_index;
                        break;
                    }
                }
            }
            if (result)
                break;
            {
                // match: 0xa1d78: restrict prevents alias reloads across iterator writes
                s_object_header *__restrict headers = (s_object_header *)object_header_data->data;
                long object_index = ((unit_datum *)headers[datum_index_to_absolute_index(iterator->m_unit_index)].datum)->object.next_object_index;
                while (object_index != NONE) {
                    // match: 0xa1d78: restrict prevents alias reloads across iterator writes
                    unit_datum *__restrict object = (unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
                    if (((1 << object->object.object_type) & 0x1003) && FLAG_TEST(object->object.object_flags, 26))
                        break;
                    object_index = object->object.next_object_index;
                }
                iterator->m_unit_index = object_index;
                if (iterator->m_unit_index != NONE)
                    iterator->m_next_object_index = ((unit_datum *)headers[datum_index_to_absolute_index(iterator->m_unit_index)].datum)
                        ->object.first_child_object_index;
            }
        }
    }
    return result;
}

// 0xa8730 natural source
// @address 0xa8730 ?unit_control_clear@@YAXPAUunit_control_data@@@Z
void unit_control_clear(unit_control_data *control)
{
    memset(control, 0, sizeof(*control));
    control->animation_state = 138;
    control->aiming_speed = 0;
    control->weapon_set.set_identifier = NONE;
    control->weapon_set.weapon_indices[0] = NONE;
    control->weapon_set.weapon_indices[1] = NONE;
    control->grenade_index = NONE;
    control->zoom_level = NONE;
    control->facing_vector = k_real_math_identity_matrix_storage.forward;
    control->aiming_vector = k_real_math_identity_matrix_storage.forward;
    control->looking_vector = k_real_math_identity_matrix_storage.forward;
    aim_assist_targeting_clear(&control->targeting);
}

// 0xa8828 natural source
// @address 0xa8828 ?unit_control@@YAXJPBUunit_control_data@@@Z
void unit_control(long unit_index, const unit_control_data *control)
{
    unit_datum *unit = unit_get(unit_index);
    unit->unit.throttle = control->throttle;
    unit->unit.primary_trigger = control->primary_trigger;
    unit->unit.secondary_trigger = control->secondary_trigger;
    unit->unit.aiming_speed = (signed char)control->aiming_speed;
    if (control->weapon_set.set_identifier == unit->unit.desired_weapon_set_identifier)
        *(s_unit_weapon_set *)&unit->unit.desired_weapon_set_identifier = control->weapon_set;
    if (control->grenade_index != NONE)
        unit->unit.desired_grenade_index = (signed char)control->grenade_index;
    unit->unit.desired_zoom_level = (signed char)control->zoom_level;
    unit->unit.control_flags = control->control_flags;
    unit->unit.desired_looking_vector = control->looking_vector;
    unit->unit.desired_aiming_vector = control->aiming_vector;
    unit->unit.facing_vector = control->facing_vector;
    unit->unit.animation_state = control->animation_state;
    unit->unit.gaze_position = control->gaze_position;
    memcpy(&unit->unit.targeting, &control->targeting, sizeof(control->targeting));
    ::simulation_action_object_update(unit_index, 0x30000);
}

// 0xadef0 natural source
void unit_get_head_position(long unit_index, real_point3d *position)
{
    object_marker marker;
    internal_object_get_markers_by_string_id(unit_index, 157, &marker, 1, false);
    *position = marker.matrix.position;
}

// 0xadf48 natural source
bool unit_get_body_position(long unit_index, real_point3d *position)
{
    object_marker marker;
    bool found = internal_object_get_markers_by_string_id(unit_index, 177, &marker, 1, false) > 0;
    *position = marker.matrix.position;
    return found;
}

// the unit attached to the parent in the given seat, or -1
long unit_seat_get_occupant(long parent_index, short seat_index)
{
    long child_index = ((unit_datum *)object_get(parent_index))->object.first_child_object_index;
    while (child_index != -1) {
        unit_datum *child = (unit_datum *)object_get(child_index);
        if (((1 << child->object.object_type) & 0x1003) != 0 && child->unit.parent_seat_index == seat_index)
            break;
        child_index = child->object.next_object_index;
    }
    return child_index;
}


// 0xbf368 natural source
// @address 0xbf368 ?unit_get_aim_position@@YA_NJPATreal_point3d@@@Z
bool unit_get_aim_position(long unit_index, real_point3d *position)
{
    object_marker marker;
    if (internal_object_get_markers_by_string_id(unit_index, 972, &marker, 1, false) == 1)
    {
        *position = marker.matrix.position;
        return true;
    }
    else if (unit_get_body_position(unit_index, position))
        return true;
    else if (unit_get_gun_position(unit_index, true, position))
        return true;
    else
    {
        object_get_origin(unit_index, position);
        return true;
    }
}

// 0xc04a8 natural source
// @address 0xc04a8 ?unit_get_gun_position@@YA_NJ_NPATreal_point3d@@@Z
bool unit_get_gun_position(long unit_index, bool primary, real_point3d *position)
{
    long weapon_index = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index);
    bool result = false;
    if (weapon_index != NONE)
        result = weapon_get_trigger_position(weapon_index, 0, position);
    return result;
}


// @address 0xda660 ?unit_post_update@@YA_NJ@Z
bool unit_post_update(long unit_index)
{
    unit_animation_update_mixing_board(unit_index);
    return false;
}

// 0xd7830 natural source
// @address 0xd7830 ?unit_update@@YA_NJ@Z
bool unit_update(long unit_index)
{
    bool changed = false;
    changed |= unit_update_control(unit_index);
    changed |= unit_update_weapons(unit_index);
    {
        long equipment_index = unit_get(unit_index)->unit.equipment_object_index;
        if (equipment_index != NONE)
        {
            if (equipment_remaining_charges(equipment_index) == 0 && equipment_active_fraction(equipment_index) == 0.0f)
                unit_delete_current_equipment(unit_index);
            else
                equipment_update(equipment_index, unit_index);
        }
    }
    changed |= unit_update_aiming(unit_index);
    changed |= unit_update_seats(unit_index);
    changed |= unit_update_active_camouflage(unit_index);
    unit_dialogue_update(unit_index);
    unit_update_team_index(unit_index);
    unit_update_active_camouflage_fade(unit_index);
    unit_update_vision_mode(unit_index);
    return changed;
}

// @address 0xd8058 unit_set_facing

void unit_set_facing(long unit_index, const real_vector3d *a, const real_vector3d *b)
{
    unit_datum *unit = unit_get(unit_index);
    unit->unit.desired_aiming_vector = *a;
    unit->unit.desired_looking_vector = *b;
    ::simulation_action_object_update(unit_index, 0x10000);
}

// @address 0xd8120 ?unit_delete@@YAXJ@Z
void unit_delete(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    unit->unit.desired_weapon_indices[0] = NONE;
    unit->unit.desired_weapon_indices[1] = NONE;
    unit->unit.current_weapon_index = NONE;
    unit->unit.desired_weapon_index = NONE;
    unit_delete_all_weapons_internal(unit_index);
    unit_delete_current_equipment(unit_index);
}

// A 0..1 ratio of the object's +0x3e0 value: 0 when it is not positive; against its +0x3e4 limit, or the definition's
// 0xd83c0 natural source
// @address 0xd83c0 ?unit_get_weapon_dual_error_theta@@YAMJJ@Z
float unit_get_weapon_dual_error_theta(long unit_index, long weapon_index)
{
    float theta = 1.0f;
    if (unit_index != NONE && weapon_index != NONE)
    {
        unit_datum *unit = unit_get(unit_index);
        if (unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index) == weapon_index)
        {
            if (unit->unit.dual_error_ticks[1] > 0)
            {
                float seconds = game_ticks_to_seconds((float)unit->unit.dual_error_ticks[1]);
                theta = seconds / 0.3f;
                theta = PIN(theta, 0.0f, 1.0f);
            }
            else
                theta = 0.0f;
        }
        else if (unit->unit.dual_error_ticks[0] > 0)
        {
            float seconds = game_ticks_to_seconds((float)unit->unit.dual_error_ticks[0]);
            theta = seconds / 0.3f;
            theta = PIN(theta, 0.0f, 1.0f);
        }
        else
            theta = 0.0f;
    }
    return theta;
}

// 0xd9308 natural source
// @address 0xd9308 ?unit_has_seat_entrance_animation@@YA_NJJF@Z
static __forceinline bool units_motor_animation_test_mode_and_state(long unit_index, long mode, long state)
{
    const c_animation_manager *manager = (const c_animation_manager *)object_get_motor(unit_index);
    c_animation_id animation_id = manager->find_goal_animation_id(mode, 226, 226, state, 0);
    return animation_id.valid();
}
bool unit_has_seat_entrance_animation(long unit_index, long parent_index, short seat_index)
{
    unit_datum *parent = unit_get(parent_index);
    unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
    unit_seat *seats = definition->seats.begin();
    bool found = false;
    for (long index = 0; index < 5; index++)
    {
        if (units_motor_animation_test_mode_and_state(unit_index, seats[seat_index].label, g_unit_entering_state_names[index]))
        {
            found = true;
            break;
        }
    }
    return found;
}

float unit_compute_boost_fraction(long unit_index)
{
  float result = real_constant_pool_zero();
  unit_datum *object = unit_get(unit_index);
  float value = object->unit.boost_time;
  if (!(value <= real_constant_pool_zero())) {
    float limit = object->unit.boost_decay_time;
    unit_definition *definition = (unit_definition *)tag_get(0, object->object.definition_index);
    if (limit == real_constant_pool_zero()) {
      limit = definition->boost_peak_time;
      if (value >= limit)
        result = real_constant_pool_one();
      else
        result = value / limit;
    }
    else if (!(value <= limit)) {
      result = (value - limit) / definition->boost_decay_time;
    }
  }
  return result;
}

unsigned char unit_vehicle_is_flying(long object_index)
{
    bool result = false;
    if (object_index != NONE)
    {
        s_object_header *header = object_header_get(object_index);
        if (((1 << header->object_type) & 2) != 0)
        {
            vehicle_definition *definition =
                (vehicle_definition *)tag_get(0, ((unit_datum *)header->datum)->object.definition_index);
            result = (definition->flags >> 11) & 1;
        }
    }
    return result;
}

inline bool limit3d(real_vector3d *vector, float maximum)
{
  float magnitude_squared = magnitude_squared3d(vector);
  if (magnitude_squared > maximum * maximum) {
    float scale = maximum / sqrtf(magnitude_squared);
    scale_vector3d(vector, scale, vector);
    return true;
  }
  return false;
}

typedef bool (*units_limit_function)(real_vector3d *, float);
units_limit_function units_limit_odr_use() { return &limit3d; }

typedef char units_equipment_invincibility_offset[(offsetof(s_equipment_definition,
    equipment_types) + 7 * sizeof(s_tag_block) == 0x218) ? 1 : -1];

c_global_material_type unit_override_damage_info_material_type(long unit_index, c_global_material_type material)
{
 if (unit_index != -1) {
  s_object_header *header = object_header_get(unit_index);
  if (((1 << header->object_type) & 0x1003) != 0) {
   long equipment_index = ((unit_datum *)header->datum)->unit.equipment_object_index;
   if (equipment_index != -1) {
    short override_index = -1;
    bool overridden = false;
    s_equipment_definition *definition = (s_equipment_definition *)tag_get(0, unit_get(equipment_index)->object.definition_index);
    if (definition->equipment_types[7].count != 0 && !(equipment_active_fraction(equipment_index) == 0.0f)) {
     overridden = true;
     override_index = ((s_equipment_type_invincibility *)definition->equipment_types[7].address)->material_type.m_index;
    }
    if (overridden)
     material.m_index = override_index;
   }
  }
 }
 return material;
}

// the next object on the iterator's list whose type is 0, 1 or 12
// match: 0x1031d8: restrict on the iterator prevents alias reloads across list writes
struct unit_datum * unit_passenger_iterator_next(struct unit_passenger_iterator * __restrict iterator)
{
    unit_datum *result = 0;
    if (iterator->next_object_index != -1) {
        while (iterator->next_object_index != -1) {
            int object_index = iterator->next_object_index;
            unit_datum *object = (unit_datum *)object_get(object_index);
            char type = object->object.object_type;
            iterator->next_object_index = object->object.next_object_index;
            if ((1 << type) & 0x1003) {
                iterator->seat_index = object->unit.parent_seat_index;
                iterator->passenger_index = object_index;
                result = object;
                break;
            }
    }
    }
    return result;
}

float unit_get_zoom_magnification(long unit_index, short zoom_level)
{
    float magnification = 1.0f;
    long weapon_index = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index);
    if (weapon_index != NONE && weapon_rotate_zoom_level(weapon_index, NONE) != NONE) {
        magnification = ::weapon_get_zoom_magnification(weapon_index, zoom_level);
    } else {
        bool weapon_flag = false;
        if (weapon_index != NONE) {
            weapon_definition *definition = (weapon_definition *)tag_get(0, unit_get(weapon_index)->object.definition_index);
            weapon_flag = FLAG_TEST(definition->flags, 16);
        }
        if (!weapon_flag) {
            s_player_information_definition *information = (s_player_information_definition *)tag_block_get_element_with_size(
                &((s_game_globals *)global_game_globals)->player_information, 0, sizeof(s_player_information_definition));
            if (zoom_level != NONE) {
                float fraction;
                if (information->zoom_level_count == 1)
                    fraction = 1.0f;
                else
                    fraction = (float)zoom_level / (float)(information->zoom_level_count - 1);
                magnification = information->zoom_magnification_min + fraction * (information->zoom_magnification_max - information->zoom_magnification_min);
            }
        }
    }
    return magnification;
}

// match: 0x12f438: restrict on the state out-pointer prevents alias reloads across the fill
void unit_build_unit_interface_state(long unit_index, struct unit_interface_state * __restrict state)
{
    unit_datum *unit = unit_get(unit_index);
    memset(state, 0, sizeof(*state));
    if (unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index) != NONE) {
        if (unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.desired_weapon_index) != NONE)
            state->dual_weapons = true;
        else
            state->primary_weapon = true;
    }
    if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE) {
        unit_datum *parent = unit_get(unit->object.parent_object_index);
        unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
        unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
        state->seat_flag = !FLAG_TEST(seat->flags, 5);
    }
}

// Whether the flags are enabled and, for an index 1-8, that item is too.
extern "C" bool chud_scripting_is_element_enabled(const chud_script_flags *flags, short index)
{
  bool result = flags->enabled;
  switch (index) {
  case 1:
    result = flags->enabled && flags->items[0];
    break;
  case 2:
    result = flags->enabled && flags->items[1];
    break;
  case 3:
    result = flags->enabled && flags->items[2];
    break;
  case 4:
    result = flags->enabled && flags->items[3];
    break;
  case 5:
    result = flags->enabled && flags->items[4];
    break;
  case 6:
    result = flags->enabled && flags->items[5];
    break;
  case 7:
    result = flags->enabled && flags->items[6];
    break;
  case 8:
    result = flags->enabled && flags->items[7];
    break;
  }
  return result;
}

long unit_get_current_backpack_weapon(long unit_index)
{
  long result = -1;
  unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
  for (long i = 0; i < 4; i++) {
    if (i != unit->unit.current_weapon_index && i != unit->unit.desired_weapon_index && unit->unit.weapon_object_indices[i] != -1) {
      result = unit->unit.weapon_object_indices[i];
      break;
    }
  }
  return result;
}

// 0x20d080 natural source
template<>
void c_static_string<32>::set_wchar(const wchar_t *string)
{
    wchar_string_to_ascii_string(string, m_string, 32, 0);
}

// 0x3216a8 natural source
// @address 0x3216a8 ?unit_create_children@@YAXJ@Z
void unit_create_children(long unit_index)
{
    unit_add_initial_weapons(unit_index);
    action_submit_event(unit_index, 54);
    unit_update_driver_and_gunner(unit_index);
}

struct effect_vector_field;

// @address 0x3216e8 unit_apply_emp_stun
extern "C" void unit_apply_emp_stun(long unit_index, bool discharge_shield)
{
    s_emp_stun_globals *emp_globals = NULL;
    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    if (global_game_globals && ((s_game_globals *)global_game_globals)->campaign_metagame.count() == 1)
        emp_globals = (s_emp_stun_globals *)((s_game_globals *)global_game_globals)->campaign_metagame.address;
    long stun_ticks = game_seconds_to_ticks_round(definition->emp_stun_time);
    if (emp_globals && unit->object.maximum_shield_vitality != 0.0f && unit->object.shield_vitality == 0.0f && discharge_shield)
        unit->unit.emp_shield_stun_ticks = (short)game_seconds_to_ticks_round(emp_globals->shield_stun_time);
    if (stun_ticks > 0)
    {
        long old_stun_ticks = unit->unit.stun_ticks;
        unit->unit.stun_ticks = (short)stun_ticks;
        if (unit->object.object_type == 1)
            simulation_action_object_update(unit_index, 0x10000);
        for (long index = 0; index < definition->seat_accelerations.count; index++)
            unit->unit.seat_acceleration_fractions[index] = 0.0f;
        if (old_stun_ticks == 0 && definition->emp_effect.index != NONE)
        {
            long effect_storage;
            c_tag_index *effect = new (&effect_storage) c_tag_index(definition->emp_effect.index);
            effect_new_from_object(*effect, damage_owner_unknown(), 0, unit_index, 0.0f, 0.0f, NULL, NULL, 1);
        }
        object_wake(unit_index);
    }
}

// 0x321898 natural source
static __forceinline float game_tick_length();
static __forceinline s_object_header *units_control_datum_get(s_data_array *array, long index)
{
    long absolute_index = (unsigned short)index;
    s_object_header *data = (s_object_header *)array->data;
    return data + absolute_index;
}
static __forceinline unit_definition *units_control_definition_get(long definition_index)
{
    return (unit_definition *)((cache_file_tag_instance *)global_tag_instances)[(unsigned short)definition_index].definition;
}
static __forceinline unit_datum *units_control_get(long unit_index)
{
    return (unit_datum *)units_control_datum_get(object_header_data, unit_index)->datum;
}
static __forceinline long units_control_motor_animation_get_state(long unit_index)
{
    unit_datum *unit = units_control_get(unit_index);
    c_animation_manager *motor = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
    long state = NONE;
    if (motor->valid_goals())
        state = motor->get_state_name();
    return state;
}
static __forceinline bool units_control_player_input_enabled()
{
    return !players_globals->inhibited;
}
static __forceinline bool units_control_vehicle_auto_control(long vehicle_index)
{
    bool result = false;
    vehicle_datum *vehicle = (vehicle_datum *)units_control_get(vehicle_index);
    c_vehicle_type_turret *turret = (c_vehicle_type_turret *)vehicle_get_auto_control(vehicle->vehicle.type_state, vehicle_index);
    if (turret)
        result = turret->auto_control(vehicle_index);
    return result;
}
static __forceinline bool units_control_driven_by_ai(long unit_index)
{
    unit_datum *unit = (unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)unit_index].datum;
    if (unit->unit.driver_object_index != NONE)
        unit = (unit_datum *)((s_object_header *)object_header_data->data)[(unsigned short)unit->unit.driver_object_index].datum;
    return unit->unit.actor_index != NONE;
}

// @address 0x321898 ?unit_update_control@@YA_NJ@Z
bool unit_update_control(long unit_index)
{
    bool result = false;
    unit_datum *unit = units_control_get(unit_index);
    unit_definition *definition = units_control_definition_get(unit->object.definition_index);
    bool driver_updated = false;
    bool gunner_updated = false;
    unit_update_predicted_controller(unit_index);
    if (unit->unit.m_unit_flags.test(_unit_flag_running_blindly)) {
        unit_running_blind(unit_index, &unit->unit.facing_vector);
        unit->unit.desired_aiming_vector = unit->unit.facing_vector;
        unit->unit.desired_looking_vector = unit->unit.desired_aiming_vector;
        simulation_action_object_update(unit_index, 0x10000);
        unit->unit.throttle = *global_forward3d;
        unit->unit.control_flags = 0;
        result = true;
        driver_updated = true;
    } else if (unit->unit.m_unit_flags.test(_unit_flag_actively_controlled)) {
        result = true;
        driver_updated = true;
        gunner_updated = true;
    } else {
        real_vector3d forward;
        object_get_orientation(unit_index, &forward, 0);
        unit->unit.desired_looking_vector = forward;
        unit->unit.desired_aiming_vector = forward;
        unit->unit.facing_vector = forward;
        simulation_action_object_update(unit_index, 0x10000);
        unit->unit.throttle = *global_zero_vector3d;
        unit->unit.control_flags = 0;
    }
    if (!FLAG_TEST(definition->flags, 11)) {
        bool suppress_control = unit->unit.m_unit_flags.test(_unit_flag_suppress_control);
        if (unit->unit.persistent_control_ticks > 0) {
            unsigned long forced_flags = unit->unit.forced_control_flags;
            unsigned long control_flags = unit->unit.control_flags;
            unit->unit.control_flags = forced_flags | control_flags;
            if (unit->unit.forced_control_flags & 0x40000) {
                long pulse_ticks = (long)((float)game_time_globals->ticks_per_second * 0.25f + 0.5f);
                if (unit->unit.persistent_control_ticks % pulse_ticks == 0)
                    unit->unit.control_flags |= 0x40000;
                else
                    unit->unit.control_flags &= ~0x40000;
                unit->unit.primary_trigger = 1.0f;
            } else {
                unit->unit.primary_trigger = 0.0f;
            }
            if (--unit->unit.persistent_control_ticks == 0)
                unit->unit.forced_control_flags = 0;
            result = true;
            gunner_updated = true;
        }
        if (!suppress_control) {
            bool auto_control = false;
            if ((unsigned char)unit->object.object_type == 1)
                auto_control = units_control_vehicle_auto_control(unit_index);
            if (unit->unit.driver_object_index != NONE && !FLAG_TEST(unit->object.damage_flags, 2) && !auto_control) {
                unit_datum *driver = units_control_get(unit->unit.driver_object_index);
                long state = units_control_motor_animation_get_state(unit->unit.driver_object_index);
                bool copy_control = true;
                if (mover_is_in_entering_state(unit->unit.driver_object_index)) {
                    e_unit_animation_key_frame_status key_frame = unit_animation_get_key_frame_status(unit->unit.driver_object_index, true);
                    if (driver->unit.player_index != NONE && units_control_player_input_enabled())
                        copy_control = key_frame != 2;
                    else
                        copy_control = key_frame == 1;
                } else if (state == 85 || state == 185 || state == 1190) {
                    e_unit_animation_key_frame_status key_frame = unit_animation_get_key_frame_status(unit->unit.driver_object_index, true);
                    copy_control = key_frame == 2;
                }
                if (copy_control) {
                    unit->unit.control_flags |= driver->unit.control_flags & 0x3ffff;
                    unit->unit.facing_vector = driver->unit.facing_vector;
                    unit->unit.throttle = driver->unit.throttle;
                }
                driver_updated = true;
            }
            if (unit->unit.gunner_object_index != NONE && !FLAG_TEST(unit->object.damage_flags, 2) && !auto_control &&
                unit_animation_allows_gunner_control(unit_index))
            {
                bool copy_aiming;
                unit_datum *gunner = units_control_get(unit->unit.gunner_object_index);
                long state = units_control_motor_animation_get_state(unit->unit.gunner_object_index);
                bool entering = false;
                bool exiting = false;
                bool copy_control;
                if (mover_is_in_entering_state(unit->unit.gunner_object_index)) {
                    e_unit_animation_key_frame_status key_frame = unit_animation_get_key_frame_status(unit->unit.gunner_object_index, true);
                    entering = key_frame == 2 || key_frame == 0;
                } else if (state == 85 || state == 185 || state == 1190) {
                    e_unit_animation_key_frame_status key_frame = unit_animation_get_key_frame_status(unit->unit.gunner_object_index, true);
                    exiting = key_frame == 1 || key_frame == 0;
                }
                if (gunner->unit.player_index != NONE && units_control_player_input_enabled()) {
                    copy_aiming = !exiting;
                    copy_control = !exiting && !entering;
                } else {
                    copy_aiming = !exiting && !entering;
                    copy_control = !exiting && !entering;
                }
                if (copy_aiming) {
                    unit->unit.desired_aiming_vector = gunner->unit.desired_aiming_vector;
                    unit->unit.desired_looking_vector = unit->unit.desired_aiming_vector;
                    simulation_action_object_update(unit_index, 0x10000);
                }
                if (copy_control) {
                    unit->unit.control_flags |= gunner->unit.control_flags & 0xfffc0000;
                    unit->unit.control_flags |= gunner->unit.control_flags & 0x20;
                    unit->unit.primary_trigger = gunner->unit.primary_trigger;
                }
                gunner_updated = true;
            }
            if (unit->unit.control_flags & 0xfffc0000) {
                unit->unit.last_weapon_control_time = game_time_get();
                result = true;
            }
            if (driver_updated || gunner_updated) {
                for (long child_index = unit->object.first_child_object_index; child_index != NONE;) {
                    s_object_header *header = object_header_get(child_index);
                    unit_datum *child = (unit_datum *)header->datum;
                    if (((1 << child->object.object_type) & 0x1003) &&
                        ((driver_updated && child->unit.driver_object_index == unit_index) ||
                         (gunner_updated && child->unit.gunner_object_index == unit_index)))
                        header->flags |= 2;
                    child_index = child->object.next_object_index;
                }
            }
        }
    }
    bool trick = (unsigned char)unit->object.object_type == 1 && vehicle_is_running_trick(unit_index);
    unsigned char boost = trick || (unit->unit.control_flags & 0x800) != 0;
    if (FLAG_TEST(definition->flags, 28) && (boost || unit->unit.boost_time > 0.0f)) {
        if (unit->unit.boost_decay_time == 0.0f) {
            float peak_time = definition->boost_peak_time + definition->boost_dead_time;
            if (!boost || (unit->unit.boost_time += game_tick_length()) >= peak_time) {
                float decay_time;
                float fraction;
                if (unit->unit.boost_time < peak_time) {
                    decay_time = unit->unit.boost_time < definition->boost_peak_time ?
                        definition->boost_decay_time * unit->unit.boost_time / definition->boost_peak_time : definition->boost_decay_time;
                    fraction = definition->boost_fraction * (unit->unit.boost_time + decay_time) / (peak_time + definition->boost_decay_time);
                } else {
                    decay_time = definition->boost_decay_time;
                    fraction = definition->boost_fraction;
                }
                if (units_control_driven_by_ai(unit_index))
                    fraction = 0.0001f;
                unit->unit.boost_time = decay_time + fraction;
                unit->unit.boost_decay_time = fraction;
            }
        } else {
            unit->unit.boost_time -= game_tick_length();
            if (unit->unit.boost_time <= 0.0f) {
                unit->unit.boost_time = 0.0f;
                unit->unit.boost_decay_time = 0.0f;
            }
        }
        result = true;
    }
    return result;
}

// 0x322150 natural source
// @address 0x322150 ?unit_update_vision_mode@@YAXJ@Z
void unit_update_vision_mode(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    if ((unit->unit.player_index != NONE || unit->unit.actor_index != NONE) && game_globals->options.game_mode == 1)
    {
        if (unit->unit.control_flags & 4)
        {
            if (unit->unit.m_unit_flags.test(_unit_flag_vision_mode))
            {
                unit_arbiter_active_camouflage_play_sound(unit_index, 1);
                unit->unit.m_unit_flags.set(_unit_flag_vision_mode, false);
            }
            else
            {
                unit_arbiter_active_camouflage_play_sound(unit_index, 0);
                unit->unit.m_unit_flags.set(_unit_flag_vision_mode, true);
                unit->unit.vision_mode_timer = 4.0f;
            }
        }
        if (unit->unit.m_unit_flags.test(_unit_flag_vision_mode))
        {
            if (unit->unit.vision_mode_timer > 0.0f)
            {
                unit->unit.vision_mode_timer -= game_time_globals->tick_length;
                if (unit->unit.vision_mode_timer <= 0.0f)
                    unit->unit.vision_mode_timer = 0.0f;
            }
            if (unit->object.parent_object_index != NONE || (bool)(_rotl(unit->object.damage_flags, 30) & 1) ||
                (lights_game_globals->can_use_flashlights == 0 && unit->unit.vision_mode_timer == 0.0f))
                unit->unit.m_unit_flags.set(_unit_flag_vision_mode, false);
            if (unit->unit.vision_mode_fraction != 1.0f)
            {
                unit->unit.vision_mode_fraction += 1.0f / (float)game_seconds_to_ticks_round(0.2f);
                if (unit->unit.vision_mode_fraction > 1.0f)
                    unit->unit.vision_mode_fraction = 1.0f;
            }
        }
        else if (unit->unit.vision_mode_fraction != 0.0f)
        {
            unit->unit.vision_mode_fraction -= 1.0f / (float)game_seconds_to_ticks_round(0.8f);
            if (unit->unit.vision_mode_fraction < 0.0f)
                unit->unit.vision_mode_fraction = 0.0f;
        }
    }
}

// 0x322f40 natural source
// @address 0x322f40 ?unit_update_seats@@YA_NJ@Z
bool unit_update_seats(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    bool success = true;
    bool changed = false;
    if (!FLAG_TEST(definition->flags, 11))
    {
        unit_update_predicted_vehicle(unit_index);
        unit_seat_acceleration_update(unit_index);
        if (unit->unit.m_unit_flags.test(_unit_flag_exit_vehicle_immediate) && !game_simulation::game_is_predicted()
            && unit->object.parent_object_index != NONE &&
            unit->unit.parent_seat_index != NONE)
        {
            unit->unit.m_unit_flags.set(_unit_flag_exit_vehicle_immediate, false);
            action_submit_event(unit_index, 20);
        }
        for (long index = 0; index < definition->seat_accelerations.count; index++)
        {
            s_seat_acceleration_entry *acceleration = (s_seat_acceleration_entry *)tag_block_get_element_with_size(
                &definition->seat_accelerations, index, sizeof(s_seat_acceleration_entry));
            bool active = false;
            switch (index)
            {
            case 0: active = unit->unit.driver_object_index != NONE || FLAG_TEST(unit->unit.unit_flags, 1); break;
            case 1: active = unit->unit.gunner_object_index != NONE && unit->unit.gunner_object_index != unit->unit.driver_object_index; break;
            }
            if (FLAG_TEST(unit->object.damage_flags, 2))
                active = false;
            if (unit->unit.stun_ticks > 0)
                active = false;
            if (active)
            {
                if (unit->unit.seat_acceleration_fractions[index] != 1.0f)
                {
                    if (acceleration->seconds > 0.0f)
                        unit->unit.seat_acceleration_fractions[index] += 1.0f / game_seconds_to_ticks_real(acceleration->seconds);
                    else
                        unit->unit.seat_acceleration_fractions[index] = 1.0f;
                    if (unit->unit.seat_acceleration_fractions[index] > 1.0f)
                        unit->unit.seat_acceleration_fractions[index] = 1.0f;
                    changed = true;
                }
            }
            else if (unit->unit.seat_acceleration_fractions[index] != 0.0f)
            {
                if (acceleration->seconds > 0.0f)
                    unit->unit.seat_acceleration_fractions[index] -= 1.0f / game_seconds_to_ticks_real(acceleration->seconds);
                else
                    unit->unit.seat_acceleration_fractions[index] = 0.0f;
                if (unit->unit.seat_acceleration_fractions[index] < 0.0f)
                    unit->unit.seat_acceleration_fractions[index] = 0.0f;
                changed = true;
            }
        }
    }
    return changed;
}

// 0x323f00 natural source
// @address 0x323f00 ?unit_test_seat_entrance_markers@@YA_NJJF@Z
bool unit_test_seat_entrance_markers(long unit_index, long target_index, short seat_index)
{
    bool found = false;
    unit_datum *target = unit_get(target_index);
    unit_definition *definition = (unit_definition *)tag_get(0, target->object.definition_index);
    unit_seat *seat = &definition->seats[seat_index];
    object_marker markers[4];
    long count = internal_object_get_markers_by_string_id(target_index, seat->entry_marker_name, markers, 4, false);
    for (long index = 0; !found && index < count; index++)
    {
        if (unit_can_use_seat_entry_marker(unit_index, target_index, seat_index, &markers[index].matrix.position,
            &markers[index].matrix.forward))
            found = true;
    }
    return found;
}

// 0x3244b8 natural source

bool unit_has_seat_animation(long unit_index, long parent_index, short seat_index)
{
    unit_datum *parent = unit_get(parent_index);
    unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
    bool result = false;
    if (seat_index >= 0 && seat_index < definition->seats.count()) {
        if ((unsigned char)unit_get(unit_index)->object.object_type == 1)
            result = true;
        else if (motor_animation_test_mode(unit_index, definition->seats[seat_index].label))
            result = true;
    }
    return result;
}

// 0x324a88 natural source
// @address 0x324a88 ?unit_rotate_zoom_level@@YAFJF@Z
short unit_rotate_zoom_level(long unit_index, short zoom_level)
{
    unit_datum *unit = unit_get(unit_index);
    long weapon_index = unit_inventory_try_get_weapon(unit_index, unit->unit.current_weapon_index);
    bool weapon_flag = false;
    short result = NONE;
    if (weapon_index != NONE)
    {
        weapon_definition *definition = (weapon_definition *)tag_get(0, unit_get(weapon_index)->object.definition_index);
        weapon_flag = FLAG_TEST(definition->flags, 16);
    }
    else
    {
        unit_datum *unit = unit_get(unit_index);
        if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE)
        {
            unit_datum *parent = unit_get(unit->object.parent_object_index);
            unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
            unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
            if (FLAG_TEST(seat->flags, 3))
                {
                long parent_index = unit_get(unit_index)->object.parent_object_index;
                weapon_index = unit_inventory_try_get_weapon(parent_index, unit_get(parent_index)->unit.current_weapon_index);
            }
        }
    }
    if (weapon_index != NONE && weapon_is_zoomable(weapon_index))
        result = weapon_rotate_zoom_level(weapon_index, zoom_level);
    else if (!weapon_flag)
    {
        s_player_information_definition *information = (s_player_information_definition *)tag_block_get_element_with_size(
            &((s_game_globals *)global_game_globals)->player_information, 0, sizeof(s_player_information_definition));
        if (information->zoom_level_count > 0 && zoom_level + 1 < information->zoom_level_count)
            result = (short)(zoom_level + 1);
    }
    return result;
}

// 0x324f70 natural source
unit_seat *unit_get_current_seat(long unit_index)
{
    unit_seat *result = 0;
    unit_datum *unit = unit_get(unit_index);
    if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE) {
        unit_datum *parent = unit_get(unit->object.parent_object_index);
        if (((1 << parent->object.object_type) & 0x1003) != 0) {
            unit_datum *parent_unit = parent;
            unit_definition *definition = (unit_definition *)tag_get(0, parent_unit->object.definition_index);
            result = &definition->seats[unit->unit.parent_seat_index];
        }
    }
    return result;
}
static __forceinline unit_seat *units_seat_element_get(unit_definition *definition, short seat_index)
{
    return (unit_seat *)((char *)definition->seats.begin() + seat_index * sizeof(unit_seat));
}

// 0x325000 natural source
// @address 0x325000 ?unit_can_use_seat_entry_marker@@YA_NJJFPBTreal_point3d@@PBTreal_vector3d@@@Z
bool unit_can_use_seat_entry_marker(long unit_index, long target_index, short seat_index, const real_point3d *position,
    const real_vector3d *forward)
{
    bool result = false;
    unit_datum *unit = unit_get(unit_index);
    unit_datum *target = unit_get(target_index);
    unit_definition *definition = (unit_definition *)tag_get(0, target->object.definition_index);
    unit_seat *seat = &definition->seats[seat_index];
    union { real_vector3d direction; real_vector3d unit_velocity; } scratch;
    real_point3d camera;
    unit_get_camera_position(unit_index, &camera);
    vector_from_points3d(&camera, position, &scratch.direction);
    float distance = normalize3d(&scratch.direction);
    if (distance < seat->entry_radius)
    {
        float unit_angle = angle_between_normals3d(&unit->unit.aiming_vector, &scratch.direction);
        float marker_angle = 3.1415927f - angle_between_normals3d(forward, &scratch.direction);
        if (unit_angle <= seat->entry_unit_angle && marker_angle <= seat->entry_marker_angle)
        {
            object_get_velocities(unit_index, &scratch.unit_velocity, 0);
            real_vector3d target_velocity;
            object_get_velocities(target_index, &target_velocity, 0);
            real_vector3d relative_velocity;
            subtract_vectors3d(&scratch.unit_velocity, &target_velocity, &relative_velocity);
            float speed = magnitude3d(&relative_velocity);
            if (speed <= seat->entry_max_velocity)
                result = true;
        }
    }
    return result;
}

// 0x325178 natural source
bool unit_seat_is_being_boarded(long unit_index, short seat_index)
{
    bool result = false;
    unit_definition *definition = (unit_definition *)tag_get(0, unit_get(unit_index)->object.definition_index);
    c_multi_unit_passenger_iterator iterator;
    iterator.begin(unit_index);
    while (iterator.next() && !result)
    {
        if (iterator.m_parent_index == unit_index && iterator.m_seat_index != NONE)
        {
            unit_seat *seat = units_seat_element_get(definition, iterator.m_seat_index);
            if (FLAG_TEST(seat->flags, 11) && (short)seat->linked_seat == seat_index)
                result = true;
        }
    }
    return result;
}

// 0x325780 natural source
// match: 0x325898: restrict on the output formal keeps the output writes in place.
__declspec(noalias) void unit_check_ping_thresholds(long unit_index, float shield_damage, float body_damage, long *__restrict result)
{
    unit_datum *unit = unit_get(unit_index);
    bool done = false;
    *result = 0;
    if (unit->unit.actor_index != NONE) {
        actor_datum *actor = datum_get<actor_datum>(actor_data, unit->unit.actor_index);
        character_vitality_properties *thresholds = character_definition_vitality_properties_get(actor->character_index);
        if (thresholds) {
            done = true;
            if (shield_damage > thresholds->hard_ping_shield_threshold || body_damage > thresholds->hard_ping_body_threshold)
                *result = 2;
            else if (shield_damage > thresholds->soft_ping_shield_threshold || body_damage > thresholds->soft_ping_body_threshold)
                *result = 1;
        }
    }
    if (!done) {
        unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
        if (body_damage > definition->hard_ping_threshold)
            *result = 2;
        else if (body_damage > definition->soft_ping_threshold || shield_damage > definition->soft_ping_threshold)
            *result = 1;
    }
}


// 0x325898 natural source
long unit_compute_ping_type(long unit_index, bool killed, bool a, bool b, bool c, bool d, bool e, float damage, float shield_damage)
{
    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    bool success = true;
    long result = 0;
    if (killed)
    {
        if (a)
            result = 6;
        else if (b)
            result = 5;
        else if (!e && definition->ping_threshold > 0.0f && unit->object.current_body_damage + shield_damage > definition->ping_threshold)
            result = 7;
        else
            result = 3;
    }
    else if (!FLAG_TEST(unit->object.damage_flags, 2))
    {
        unit_check_ping_thresholds(unit_index, damage, shield_damage, &result);
        if (d)
            result = 2;
        else if (c)
            result = result > 1 ? 1 : result;
        if (FLAG_TEST(unit->unit.unit_flags, 5) || unit->unit.player_index != NONE)
            result = result > 1 ? 1 : result;
    }
    return result;
}

// 0x3259f0 natural source
void unit_handle_weapon_state_change(long unit_index, long weapon_index, long state)
{
    if (state == 0)
        return;

    long action = NONE;
    unit_datum *unit = unit_get(unit_index);
    bool current_match = (unit_inventory_try_get_weapon(unit_index, unit->unit.current_weapon_index) - weapon_index == 0);
    bool desired_match = (unit_inventory_try_get_weapon(unit_index, unit->unit.desired_weapon_index) - weapon_index == 0);
    if (current_match) {
        switch (state - 1) {
        case 2: action = 68; break;
        case 3: action = 69; break;
        case 0: action = 66; break;
        case 1: action = 67; break;
        case 6: action = 70; break;
        case 7: action = 71; break;
        default: return;
        }
    } else {
        if (!desired_match)
            return;
        switch (state - 1) {
        case 2: action = 74; break;
        case 3: action = 75; break;
        case 0: action = 72; break;
        case 1: action = 73; break;
        case 6: action = 76; break;
        case 7: action = 77; break;
        default: return;
        }
    }
    action_submit_event(unit_index, action);
    unit_datum *target = unit_get(unit_index);
    if (target->unit.gunner_object_index != NONE) {
        action_submit_event(target->unit.gunner_object_index, action);
    }
}

// 0x3269b8 natural source


static __forceinline bool unit_damage_flag_test(unsigned long flags, long bit)
{
    return ((flags >> bit) & 1) != 0;
}
void unit_record_damage(long unit_index, float damage, short damage_type, bool killed, long player_index, e_game_team team,
    long object_index, unsigned char flags)
{
    unit_datum *unit = unit_get(unit_index);
    bool found = false;
    unsigned long time = game_time_get();
    for (long i = 0; i < 4; i++) {
        s_unit_damage_record *record = &unit->unit.damage_records[i];
        if ((player_index != NONE && record->player_index == player_index) ||
            (object_index != NONE && record->object_index == object_index)) {
            float damage_amount = damage;
            record->damage = damage_amount + record->damage;
            record->time = time;
            unit->unit.damage_record_flags[i] = flags;
            found = true;
        }
    }
    if (!found) {
        short record_index = NONE;
        for (long i = 0; i < 4; i++) {
            if (unit->unit.damage_records[i].time == NONE) {
                record_index = (short)i;
                break;
            }
        }
        if (record_index == NONE) {
            short maximum_damage_index = 0;
            for (long i = 1; i < 4; i++) {
                if (unit->unit.damage_records[i].damage > unit->unit.damage_records[maximum_damage_index].damage)
                    maximum_damage_index = (short)i;
            }
            record_index = NONE;
            for (long i = 0; i < 4; i++) {
                if (i != maximum_damage_index && (record_index == NONE ||
                    (unsigned long)unit->unit.damage_records[i].time < (unsigned long)unit->unit.damage_records[record_index].time))
                    record_index = (short)i;
            }
        }
        unit->unit.damage_records[record_index].object_index = object_index;
        unit->unit.damage_records[record_index].player_index = player_index;
        unit->unit.damage_records[record_index].damage = damage;
        unit->unit.damage_records[record_index].time = time;
        unit->unit.damage_record_flags[record_index] = flags;
    }
    if (!killed || team == _game_team_none)
        return;
    const e_game_team victim_team = unit->unit.team;
    if (game_team_is_enemy(victim_team, team)) {
        unit_datum *attacker = 0;
        long attacker_index = NONE;
        if (player_index != NONE) {
            player_datum *player = (player_datum *)unit_damage_datum_try_and_get(player_data, player_index);
            if (player && player->unit_index != NONE) {
                attacker = unit_get(player->unit_index);
                attacker_index = player->unit_index;
            }
        }
        if (!attacker) {
            attacker_index = object_index;
            attacker = unit_damage_try_get(object_index, 0x1003);
        }
        if (attacker) {
            long seat_unit_index = damage_type == 9 ? attacker->unit.driver_object_index : attacker->unit.gunner_object_index;
            if (seat_unit_index != NONE) {
                attacker_index = seat_unit_index;
                attacker = unit_get(seat_unit_index);
            }
            if (!unit_damage_flag_test(attacker->object.damage_flags, 2)) {
                long now = game_time_get();
                if (attacker->unit.killing_spree_time == NONE || game_ticks_to_seconds((float)(now - attacker->unit.killing_spree_time)) > 4.0f)
                    attacker->unit.killing_spree_count = 0;
                attacker->unit.killing_spree_count++;
                attacker->unit.killing_spree_time = now;
                if (unit_damage_ai_handle_killing_spree(attacker_index, attacker->unit.killing_spree_count))
                    attacker->unit.killing_spree_count = 0;
            }
        }
    }
}

// @address 0x327258 unit_get_melee_target
extern "C" long unit_get_melee_target(long unit_index)
{
    long result = NONE;
    unit_datum *unit = unit_get(unit_index);
    if (unit->unit.targeting.primary_autoaim_level >= 1.0f)
    {
        long object_index = unit->unit.targeting.target_object_index;
        if (object_try_and_get_unsafe_and_verify_type(object_index, 0x1003) != NULL)
            result = object_index;
    }
    return result;
}

// 0x327610 natural source
// @address 0x327610 ?any_unit_is_dangerous@@YA_NPAJ@Z
bool any_unit_is_dangerous(long *unit_index)
{
    c_object_iterator<unit_datum> iterator;
    iterator.begin(0x1003, 1);
    while (iterator.next())
    {
        long index = iterator.get_index();
        unit_datum *unit = iterator.get_datum();
        if (!FLAG_TEST(unit->object.object_flags, 18) && !FLAG_TEST(unit->object.object_flags, 7))
        {
            if (action_executing(index, _action_grenade) || action_executing(index, _action_equipment) || action_executing(index, _action_melee_ai) ||
                action_executing(index, _action_melee_player))
            {
                *unit_index = iterator.get_index();
                return true;
            }
        }
    }
    return false;
}

bool unit_is_dying(long unit_index)
{
    long state = motor_animation_get_state(unit_index);
    return state == 79 || state == 80;
}

// The index of the first element of the object's definition with flag bit 2 set, or -1. The bit is a
// and the result a variable preset to -1.
short unit_get_driver_seat(long object_index)
{
  unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
  unit_definition *definition = (unit_definition *)tag_get(0, object->object.definition_index);
  short result = -1;
  for (short i = 0; i < definition->seats.count(); i++) {
    bool set = ((definition->seats[i].flags >> 2) & 1);
    if (set) {
      result = i;
      break;
    }
  }
  return result;
}

// 0x327bb0 natural source
// @address 0x327bb0 ?unit_update_driver_and_gunner@@YAXJ@Z
void unit_update_driver_and_gunner(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    bool success = true;
    long old_driver = unit->unit.driver_object_index;
    unit->unit.driver_object_index = NONE;
    unit->unit.gunner_object_index = NONE;
    if ((bool)(_rotl(definition->flags, 6) & 1) && unit->object.parent_object_index != NONE && unit->unit.actor_index == NONE &&
        ((1 << object_get_type(unit->object.parent_object_index)) & 0x1003) != 0 &&
        !units_object_is_being_deleted(unit->object.parent_object_index))
        unit->unit.gunner_object_index = unit->object.parent_object_index;
    for (long pass = 0; pass < 2; pass++)
    {
        long child_index = unit->object.first_child_object_index;
        while (child_index != NONE)
        {
            unit_datum *object = unit_get(child_index);
            if (!units_object_is_being_deleted(child_index) && ((1 << object->object.object_type) & 0x1003) != 0)
            {
                unit_datum *child = object;
                if (child->unit.parent_seat_index != NONE)
                {
                    unit_seat *seat = &definition->seats[child->unit.parent_seat_index];
                    bool boarding = (bool)(_rotl(seat->flags, 21) & 1);
                    if (!boarding || (bool)(_rotl(seat->flags, 19) & 1))
                    {
                        bool driver = (bool)(_rotl(seat->flags, 30) & 1);
                        bool gunner = (bool)(_rotl(seat->flags, 29) & 1);
                        if ((bool)(_rotl(seat->flags, 14) & 1) && child->unit.player_index != NONE)
                            gunner = true;
                        bool active;
                        if (pass == 0)
                            active = !boarding && !action_executing(child_index, _action_vehicle_ejection);
                        else
                            active = boarding && action_executing(child_index, _action_vehicle_board);
                        if (boarding && (short)seat->linked_seat != NONE)
                        {
                            unit_seat *boarded_seat = &definition->seats[(short)seat->linked_seat];
                            if ((bool)(_rotl(boarded_seat->flags, 30) & 1))
                                driver = true;
                            if ((bool)(_rotl(boarded_seat->flags, 29) & 1))
                                gunner = true;
                        }
                        if (active)
                        {
                            if (driver && !(bool)(_rotl(unit->unit.unit_flags, 31) & 1) && unit->unit.driver_object_index == NONE)
                            {
                                unit->unit.driver_object_index = child_index;
                                if (gunner && unit->unit.gunner_object_index == NONE)
                                    unit->unit.gunner_object_index = child_index;
                            }
                            else if (gunner && (unit->unit.gunner_object_index == NONE
                                || unit->unit.gunner_object_index == unit->unit.driver_object_index))
                                unit->unit.gunner_object_index = child_index;
                        }
                    }
                }
                else
                {
                    unit_definition *child_definition = (unit_definition *)tag_get(0, child->object.definition_index);
                    if ((bool)(_rotl(child_definition->flags, 7) & 1) &&
                        (unit->unit.gunner_object_index == NONE || unit->unit.gunner_object_index == unit->unit.driver_object_index))
                        unit->unit.gunner_object_index = child_index;
                }
                unit_update_driver_and_gunner(child_index);
            }
            child_index = object->object.next_object_index;
        }
    }
    if (old_driver != unit->unit.driver_object_index)
        ::ai_handle_driver_change(unit_index);
}

// 0x328108 natural source
// @address 0x328108 ?unit_add_equipment_to_inventory@@YAXJJ@Z
static __forceinline void units_player_notify_equipment_pickup(long player_index)
{
    long user_index = player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player_index)];
    if (user_index != NONE)
        ((s_player_training_entry_data *)player_training_globals)[user_index].equipment_pickup_ticks = (signed char)game_time_globals->ticks_per_second;
}
void unit_add_equipment_to_inventory(long unit_index, long equipment_index)
{
    unit_datum *unit = unit_get(unit_index);
    unit_datum *equipment = unit_get(equipment_index);
    if (unit->unit.player_index != NONE)
        units_player_notify_equipment_pickup(unit->unit.player_index);
    bool connected = FLAG_TEST(equipment->object.physics_flags, 7);
    if (connected)
        object_disconnect_from_physics(equipment_index);
    if (equipment->object.parent_object_index != NONE)
        object_detach(equipment_index);
    ::item_in_unit_inventory(equipment_index, unit_index);
    ::item_hide_inventory(equipment_index);
    unit->unit.equipment_object_index = equipment_index;
    unit->unit.equipment_pickup_time = game_time_globals->time;
    ::simulation_action_object_update(unit_index, 0x10000000);
    if (unit->unit.player_index != NONE)
        projectile_play_pickup_sound(unit->unit.player_index, unit_get(equipment_index)->object.definition_index);
}

// 0x329148 natural source
bool unit_has_weapon_definition_index(long unit_index, long weapon_definition_index)
{
    unit_datum *unit = unit_get(unit_index);
    bool result = false;
    for (long i = 0; i < 4; i++) {
        long weapon_index = unit->unit.weapon_object_indices[i];
        if (weapon_index != NONE && unit_get(weapon_index)->object.definition_index == weapon_definition_index) {
            result = true;
            break;
        }
    }
    return result;
}

bool unit_test_pickup_with_mode(long unit_index, struct unit_weapon_pickup_result const *pickup_result,
    enum e_weapon_addition_method _sym_p2, short *motor_resource_mask)
{
    short mask = 0;
    bool result = false;
    switch (((long)_sym_p2)) {
    case 0:
    case 1:
        result = pickup_result->primary[0] && unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index) == NONE;
        mask = 4;
        break;
    case 3:
        result = pickup_result->primary[0];
        mask = 4;
        break;
    case 4:
        result = pickup_result->backpack[0];
        mask = 4;
        break;
    case 5:
        result = pickup_result->primary[1];
        mask = 8;
        break;
    case 6:
        result = pickup_result->backpack[1];
        mask = 8;
        break;
    }
    *motor_resource_mask = mask;
    return result;
}

// @address 0x329b30 unit_seat_acceleration_reset_history
extern "C" void unit_seat_acceleration_reset_history(long unit_index, const real_point3d *position)
{
    unit_datum *unit = unit_get(unit_index);
    unit->unit.seat_acceleration_history[0] = *position;
    unit->unit.seat_acceleration_history[1] = *position;
    unit->unit.seat_acceleration_history[2] = *position;
    unit->unit.seat_acceleration_history[3] = *position;
    unit->unit.seat_acceleration_history[4] = *position;
    unit->unit.seat_acceleration_history[5] = *position;
}



void unit_get_all_seats_internal(long object_index, unit_seat_source *seats, short *count, short max, unsigned char force)
{
    s_object_header *header = object_header_get(object_index);
    unit_datum *object = (unit_datum *)header->datum;
    if (*count >= max)
        return;
    if (((1 << header->object_type) & 0x1003) != 0) {
        if (force != 0 || (unsigned char)((object->object.damage_flags >> 2) & 1) == 0) {
            int index = 0;
            unit_definition *definition =
                (unit_definition *)tag_get(0, object->object.definition_index);
            if (definition->seats.count() > 0) {
                for (; index < definition->seats.count(); index++) {
                    if (*count >= max)
                        break;
                    seats[*count].seat_index = index;
                    seats[*count].unit_index = object_index;
                    seats[*count].definition = &definition->seats[index];
                    (*count)++;
                }
            }
        }
    }
    if (*count >= max)
        return;
    int child = object->object.first_child_object_index;
    if (child == -1)
        return;
    do {
        unit_datum *child_object = (unit_datum *)object_header_get(child)->datum;
        if (((1 << child_object->object.object_type) & 0x1003) != 0 && (unsigned char)((child_object->object.object_flags >> 26) & 1) != 0)
            ::unit_get_all_seats_internal(child, seats, count, max, force);
        child = child_object->object.next_object_index;
    } while (child != -1);
}
// @address 0x32a6b0 ?unit_inventory_cycle_weapon_set_identifier@@YAXJ@Z
void unit_inventory_cycle_weapon_set_identifier(long object_index)
{
    bool skip = false;
    if (game_globals->options.game_simulation == 4)
        skip = true;
    if (skip)
        return;
    unit_datum *object = (unit_datum *)object_header_get(object_index)->datum;
    short next = (object->unit.weapon_set_identifier + 1) & 15;
    if ((unsigned short)next == 0xffff)
        next = 0;
    object->unit.weapon_set_identifier = next;
    object->unit.desired_weapon_set_identifier = next;
    ::simulation_action_object_update(object_index, 0x20000);
}

// 0x32a7f8 natural source
// @address 0x32a7f8 ?unit_drop_grenades@@YAXJ@Z
static __forceinline void units_add_item_to_inventory(long unit_index, long item_index)
{
    ::item_in_unit_inventory(item_index, unit_index);
    ::item_hide_inventory(item_index);
}
void unit_drop_grenades(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    for (long grenade_index = 0; grenade_index < 4; grenade_index++)
    {
        s_grenade_definition *definition = ((s_game_globals *)global_game_globals)->grenades.count()
            ? &((s_game_globals *)global_game_globals)->grenades[grenade_index]
            : 0;
        if (definition)
        {
            while (unit->unit.grenade_counts[grenade_index] > 0)
            {
                if (!game_is_predicted())
                {
                    object_placement_data placement;
                    object_placement_data_new(&placement, definition->equipment.index, unit_index, 0);
                    long item_index = ::object_new(&placement);
                    if (item_index != NONE)
                    {
                        units_add_item_to_inventory(unit_index, item_index);
                        unit_drop_item(unit_index, item_index, _unit_drop_type_drop);
                    }
                }
                unit->unit.grenade_counts[grenade_index]--;
            }
        }
    }
}

// 0x327e80 natural source
// @address 0x327e80 ?unit_add_initial_weapons@@YAXJ@Z
void unit_add_initial_weapons(long unit_index)
{
    unit_definition *definition = (unit_definition *)tag_get(0, unit_get(unit_index)->object.definition_index);
    for (long i = 0; i < definition->initial_weapons.count; i++)
    {
        s_tag_reference *reference = &((s_tag_reference *)definition->initial_weapons.address)[i];
        if (reference->index == NONE)
            continue;
        object_placement_data placement;
        object_placement_data_new(&placement, reference->index, unit_index, 0);
        long object_index = object_new(&placement);
        if (object_index != NONE
            && ((game_globals->options.game_mode == 2
                    && unit_has_weapon_definition_index(unit_index, unit_get(object_index)->object.definition_index))
                || !unit_add_weapon_to_inventory(unit_index, object_index, _weapon_addition_method_silent)))
        {
            object_delete(object_index);
        }
    }
}

// 0x32a908 natural source
void unit_handle_deleted_player(long unit_index, long player_index)
{
    unit_datum *unit = unit_get(unit_index);
    if (unit->unit.simulation_player_index == player_index)
        unit->unit.simulation_player_index = NONE;
}

static __forceinline s_global_material_definition *units_melee_get_material(c_global_material_type material)
{
    s_global_material_definition *result = 0;
    if (material.m_index != NONE && material.m_index >= 0 && material.m_index < ((s_game_globals *)global_game_globals)->material_count)
        result = &((s_global_material_definition *)((s_game_globals *)global_game_globals)->materials.address)[material.m_index];
    return result;
}

// @address 0x32a940 unit_cause_melee_effects
extern "C" void unit_cause_melee_effects(long unit_index, long definition_index, long target_index,
    e_sweetener_size sweetener_size, c_global_material_type material, const real_vector3d *normal,
    const real_point3d *position, const real_vector3d *direction)
{
    long weapon_index = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index);
    bool sound_played = false;
    long effects[7];
    if (weapon_index != NONE)
    {
        weapon_datum *weapon = (weapon_datum *)unit_get(weapon_index);
        struct weapon_definition *weapon_definition = (struct weapon_definition *)tag_get(0, weapon->definition_index);
        long material_effects = weapon_definition->item.object.material_effects.index;
        if (weapon_is_melee_only(weapon_index) && weapon_definition->full_age_material_effects != NONE && !(weapon->age < 1.0f))
            material_effects = weapon_definition->full_age_material_effects;
        if (material_effects != NONE)
        {
            long root_index = weapon_index;
            long parent_index;
            do
            {
                parent_index = root_index;
                root_index = unit_get(root_index)->object.parent_object_index;
            } while (root_index != NONE);
            unit_datum *root = unit_get(parent_index);
            material_effect_find_effects(false, position, material_effects, k_global_material_type_invalid, material, true,
                _effect_type_melee, sweetener_size, effects);
            effect_spawn_impact_effects(effects, position, normal, direction, &root->object.location, 0);
            if (weapon_is_melee_only(weapon_index))
                sound_played = game_sound_new_material_effect_sounds(
                    effects, position, normal, direction, (const long *)&root->object.location, unit_index, 1.0f);
        }
    }
    if (!sound_played)
    {
        long sound_index = units_melee_get_material(material)->sound_sweetener_melee[sweetener_size].index;
        if (target_index != NONE)
        {
            s_object_definition *target_definition = (s_object_definition *)tag_get(0, unit_get(target_index)->object.definition_index);
            if (target_definition->melee_sound.index != NONE)
                sound_index = target_definition->melee_sound.index;
        }
        if (sound_index != NONE)
            object_impulse_sound_new(unit_index, sound_index, NONE, global_origin3d, global_forward3d, 1.0f);
        if (definition_index != NONE)
        {
            s_damage_effect_definition *definition = (s_damage_effect_definition *)tag_get(0, definition_index);
            if (definition->sound.index != NONE)
                object_impulse_sound_new(unit_index, definition->sound.index, NONE, global_origin3d, global_forward3d, 1.0f);
        }
    }
}

// 0x32b010 natural source
// @address 0x32b010 unit_get_melee_effect_ids
// match: 0x32b010: pointer __restrict qualifier prevents alias reload
extern "C" void unit_get_melee_effect_ids(long object_index, long type, long *__restrict a, long *__restrict b, long *__restrict c,
    // match: 0x32b010: output pointers do not alias.
    long *__restrict d)
{
    long parent_index = NONE;
    s_weapon_melee_damage *first;
    s_weapon_melee_damage *second;
    unit_datum *object = (unit_datum *)object_get(object_index);
    signed char weapon_slot = object->unit.current_weapon_index;
    unit_definition *definition = (unit_definition *)tag_get(0, object->object.definition_index);
    if (weapon_slot != NONE)
        parent_index = object->unit.weapon_object_indices[weapon_slot];
    *a = NONE;
    *b = NONE;
    *c = NONE;
    *d = NONE;
    if (parent_index != NONE)
    {
        first = 0;
        second = 0;
        weapon_datum *parent = (weapon_datum *)object_get(parent_index);
        struct weapon_definition *parent_definition = (struct weapon_definition *)tag_get(0, parent->definition_index);
        bool full = FLAG_TEST(parent_definition->flags, 20) && parent->age >= real_constant_pool_one();
        if (full)
        {
            first = &parent_definition->melee_damage_parameters[4];
        }
        else
        {
            switch (type)
            {
            case _string_id_melee_2ndhit:
                first = &parent_definition->melee_damage_parameters[1];
                break;
            case _string_id_melee_3rdhit:
                first = &parent_definition->melee_damage_parameters[2];
                break;
            case _string_id_melee_lunge:
            case _string_id_melee_lunge_airborne:
            case _string_id_melee_lunge_unaimed:
                first = &parent_definition->melee_damage_parameters[3];
                break;
            case _string_id_melee:
            case _string_id_melee_1sthit:
            case _string_id_melee_strike_1:
            case _string_id_melee_strike_2:
            case _string_id_melee_strike_3:
            case _string_id_melee_strike_4:
                first = &parent_definition->melee_damage_parameters[0];
                break;
            }
            second = &parent_definition->melee_damage_parameters[5];
        }
        if (first)
        {
            *a = first->damage.index;
            *b = first->response.index;
        }
        if (second)
        {
            *c = second->damage.index;
            *d = second->response.index;
        }
        if (*a == NONE)
            *a = parent_definition->melee_damage;
        if (*b == NONE)
            *b = parent_definition->melee_response;
        if (*a == NONE)
            *a = definition->melee_damage.index;
    }
}

// @address 0x32b820 unit_update_melee_action_state
extern "C" void unit_update_melee_action_state(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    s_player_melee_action_state *action = (s_player_melee_action_state *)((char *)unit + unit->unit.action_offset);
    if (action_executing(unit_index, _action_melee_player))
        action->silent = 1;
    action->last_melee_time = game_time_get();
}

// 0x32b890 natural source
static __forceinline signed char units_char_max(signed char a, signed char b)
{
    return a > b ? a : b;
}
// @address 0x32b890 unit_set_player_melee_remaining
extern "C" void unit_set_player_melee_remaining(long unit_index, long value)
{
    long player_index = unit_get(unit_index)->unit.player_index;
    if (player_index == NONE)
        return;
    player_datum *player = (player_datum *)(player_data->data + (player_index & 0xffff) * sizeof(player_datum));
    if (value > 127)
        value = 127;
    player->melee_remaining_ticks = units_char_max(player->melee_remaining_ticks, (signed char)value);
}

// 0x32d1d8 natural source
// @address 0x32d1d8 ?unit_show_weapon_from_inventory@@YAXJJW4e_unit_hand@@@Z
void unit_show_weapon_from_inventory(long weapon_index, long unit_index, e_unit_hand hand)
{
    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    unit_datum *weapon = unit_get(weapon_index);
    struct weapon_definition *weapon_definition = (struct weapon_definition *)tag_get(0, weapon->object.definition_index);
    long parent_marker;
    if (hand == 0)
        parent_marker = definition->hand_markers[0];
    else
        parent_marker = definition->hand_markers[1];
    long child_marker = definition->weapon_marker;
    object_marker marker;
    if (child_marker == NONE || child_marker == 0 || internal_object_get_markers_by_string_id(weapon_index, child_marker, &marker, 1, true) == 0)
        child_marker = weapon_definition->marker_name;
    if (weapon->unit.kind == 3)
    {
        ::item_unstow_inventory(weapon_index, unit_index, parent_marker, child_marker);
        return;
    }
    ::item_show_inventory_unattached(weapon_index, unit_index);
    unit_get(weapon_index)->unit.kind = 1;
    object_attach_to_marker_with_type(unit_index, parent_marker, weapon_index, child_marker);
    if (FLAG_TEST(((unit_definition *)tag_get(0, unit_get(unit_index)->object.definition_index))->flags, 5))
        ::object_set_hidden(weapon_index, true);
}

// 0x32d330 natural source
void unit_detach_simulation_entity(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    unit->unit.simulation_entity_index = NONE;
    unit->unit.simulation_actor_index = NONE;
    unit->unit.simulation_player_index = NONE;
    unit->unit.simulation_player_identifier = NONE;
    unit->unit.simulation_actor_identifier = NONE;
    unit->unit.simulation_control_index = NONE;
    unit->unit.simulation_control_identifier = NONE;
}

#define units_min(a, b) ((a) < (b) ? (a) : (b))
#define units_max(a, b) ((a) > (b) ? (a) : (b))

static __forceinline void units_weapon_set_age(long weapon_index, float age)
{
    weapon_datum *weapon = (weapon_datum *)unit_get(weapon_index);
    weapon->predicted_age = age;
    if (!weapon->predicted)
        weapon->age = age;
}
static __forceinline void units_weapon_set_magazine_rounds(long weapon_index, short rounds_loaded, short rounds_inventory)
{
    weapon_datum *weapon = (weapon_datum *)unit_get(weapon_index);
    struct weapon_definition *definition = (struct weapon_definition *)tag_get(0, weapon->definition_index);
    s_weapon_magazine_definition *magazine = &definition->magazines[0];
    short loaded = units_min(units_max(rounds_loaded, 0), magazine->rounds_loaded_maximum);
    short inventory = units_min(units_max(rounds_inventory, 0), magazine->rounds_inventory_maximum);
    s_weapon_magazine *magazine_state = &weapon->magazines[0];
    magazine_state->predicted_rounds_loaded = loaded;
    magazine_state->predicted_rounds_inventory = inventory;
    if (!weapon->predicted)
    {
        magazine_state->rounds_loaded = loaded;
        magazine_state->rounds_inventory = inventory;
    }
}

// @address 0x32d620 ?unit_apply_predicted_weapon_state@@YAXJJ@Z
void unit_apply_predicted_weapon_state(long unit_index, long weapon_slot)
{
    unit_datum *unit = unit_get(unit_index);
    long weapon_index = unit->unit.weapon_object_indices[weapon_slot];
    if (weapon_index != NONE)
    {
        weapon_datum *weapon = (weapon_datum *)unit_get(weapon_index);
        struct weapon_definition *definition = (struct weapon_definition *)tag_get(0, weapon->definition_index);
        units_weapon_set_age(weapon_index, unit->unit.predicted_weapon_states[weapon_slot].age);
        if (definition->magazines.count() > 0)
            units_weapon_set_magazine_rounds(weapon_index, unit->unit.predicted_weapon_states[weapon_slot].rounds_loaded,
                unit->unit.predicted_weapon_states[weapon_slot].rounds_inventory);
    }
}


// 0x32d770 natural source
// @address 0x32d770 ?unit_update_predicted_vehicle@@YAXJ@Z
void unit_update_predicted_vehicle(long unit_index)
{
    if (!game_simulation::game_is_predicted())
        return;
    unit_datum *unit = unit_get(unit_index);
    if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE)
    {
        if (unit->object.parent_object_index != unit->unit.predicted_vehicle_index
            || unit->unit.parent_seat_index != unit->unit.predicted_vehicle_seat_index)
        {
            if (action_executing(unit_index, _action_vehicle_board) && unit->unit.predicted_vehicle_index == NONE)
            {
                c_motor_task *task = motor_task_find_by_action(unit_index, _action_vehicle_board);
                if (task != 0)
                    motor_system_interrupted(unit_index, task, true);
            }
            if (!action_executing(unit_index, _action_vehicle_exit) && !action_executing(unit_index, _action_vehicle_exit_immediate)
                && !action_executing(unit_index, _action_vehicle_ejection))
            {
                action_submit_event(unit_index, 20);
                game_engine_record_vehicle_exit(unit_index, unit->object.parent_object_index);
            }
        }
    }
    if (unit->object.parent_object_index == NONE && unit->unit.predicted_vehicle_index != NONE)
    {
        // match: 0x32d770: pointer __restrict qualifier prevents alias reload
        const unit_datum * __restrict predicted_unit = unit;
        action_request request;
        units_action_request_new(&request, 17);
        request.seat.parent_index = predicted_unit->unit.predicted_vehicle_index;
        request.seat.seat_index = predicted_unit->unit.predicted_vehicle_seat_index;
        unsigned char executing = (unsigned char)action_executing(unit_index, _action_vehicle_board);
        request.seat.flag_a = executing != 0;
        request.seat.flag_b = true;
        action_submit(unit_index, &request);
    }
}

// 0x32d940 natural source

static __forceinline void units_simulation_request_player_detach(long unit_index)
{
    player_handle_death(unit_get(unit_index)->unit.player_index);
}
static __forceinline void units_simulation_request_actor_delete(const unit_datum *unit, long unit_index)
{
    if (unit->unit.simulation_actor_datum_index != NONE)
        simulation_action_actor_died(unit_index);
}
static __forceinline void units_simulation_request_player_attach(long unit_index, long player_index)
{
    player_datum *player = player_get(player_index);
    if (!((player->flags >> 1) & 1) && player->unit_index == NONE)
    {
        units_simulation_request_actor_delete(unit_get(unit_index), unit_index);
        player_set_unit_index(player_index, unit_index);
        player_spawn(player_index, 0, 0);
    }
}
static __forceinline void units_simulation_request_actor_create(long unit_index, long actor_index)
{
    unit_datum *unit = unit_get(unit_index);
    if (unit->unit.player_index == NONE)
    {
        if (((c_simulation_world *)simulation_get_world())->m_actors[actor_index].m_simulation_actor_index == NONE)
        {
            unit->unit.simulation_actor_datum_index = simulation_get_world()->create_actor(actor_index, unit_index);
            unit_set_actively_controlled(unit_index, true);
        }
    }
}
// @address 0x32d940 ?unit_update_predicted_controller@@YAXJ@Z
void unit_update_predicted_controller(long unit_index)
{
    if (game_simulation::game_is_predicted())
    {
        unit_datum *unit = unit_get(unit_index);
        if (unit->unit.player_index != unit->unit.simulation_player_index)
        {
            if (unit->unit.player_index != NONE)
                units_simulation_request_player_detach(unit_index);
            long player_index = unit->unit.simulation_player_index;
            if (player_index != NONE && datum_try_and_get(player_data, player_index))
                units_simulation_request_player_attach(unit_index, player_index);
        }
        if (unit->unit.simulation_actor_datum_index != unit->unit.simulation_player_identifier)
        {
            units_simulation_request_actor_delete(unit, unit_index);
            long actor_index = unit->unit.simulation_player_identifier;
            if (actor_index != NONE)
                units_simulation_request_actor_create(unit_index, actor_index);
        }
    }
}

// @address 0x32dae0 unit_active_camouflage_enable
extern "C" void unit_active_camouflage_enable(long unit_index, float transition_time, bool hold)
{
    unit_datum *unit = unit_get(unit_index);
    unit->unit.m_unit_flags.set(_unit_flag_active_camouflage, true);
    if (hold)
        unit->unit.m_unit_flags.set(_unit_flag_active_camouflage_hold, true);
    float tick_length = game_tick_length();
    transition_time = transition_time > tick_length ? transition_time : tick_length;
    unit->unit.active_camouflage_regrowth = 1.0f / transition_time;
    ::simulation_action_object_update(unit_index, 0x8000000);
}

// @address 0x32db50 ?unit_active_camouflage_disable@@YAXJM@Z
void unit_active_camouflage_disable(long unit_index, float transition_time)
{
    unit_datum *unit = unit_get(unit_index);
    unit->unit.m_unit_flags.set(_unit_flag_active_camouflage, false);
    unit->unit.m_unit_flags.set(_unit_flag_active_camouflage_hold, false);
    float tick_length = game_tick_length();
    transition_time = transition_time > tick_length ? transition_time : tick_length;
    unit->unit.active_camouflage_regrowth = 1.0f / transition_time;
    ::simulation_action_object_update(unit_index, 0x8000000);
}

// 0x32dbb8 natural source

void unit_active_camouflage_ding(long unit_index, float amount, float regrowth_seconds)
{
    unit_datum *unit = unit_get(unit_index);
    if (regrowth_seconds == 0.0f)
        regrowth_seconds = 4.0f;
    if (regrowth_seconds <= game_time_globals->tick_length)
        regrowth_seconds = game_time_globals->tick_length;
    unit->unit.active_camouflage_regrowth =
        (1.0f / regrowth_seconds > unit->unit.active_camouflage_regrowth) ? unit->unit.active_camouflage_regrowth : 1.0f / regrowth_seconds;
    unit->unit.active_camouflage -= amount;
    if ((bool)((unit->unit.unit_flags >> 3) & 1) && unit->unit.active_camouflage < 0.05f)
        unit->unit.active_camouflage = 0.05f;
    ::simulation_action_object_update(unit_index, 0x8000000);
}

// 0x32dc58 natural source
// @address 0x32dc58 ?unit_evict_parent_seat_occupant@@YA_NJ_N0@Z
bool unit_evict_parent_seat_occupant(long unit_index, bool force, bool immediate)
{
    unit_datum *unit = unit_get(unit_index);
    bool handled = false;
    if (unit->object.parent_object_index != NONE)
    {
        unit_datum *parent_object = unit_get(unit->object.parent_object_index);
        if (((1 << parent_object->object.object_type) & 0x1003) != 0)
        {
            long parent_index = unit->object.parent_object_index;
            unit_datum *parent = parent_object;
            unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
            if (unit->unit.parent_seat_index != NONE)
            {
                unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
                if (seat->linked_seat != 0xffff)
                {
                    long occupant_index = unit_seat_get_occupant(unit->object.parent_object_index, seat->linked_seat);
                    if (occupant_index != NONE && occupant_index != unit_index)
                    {
                        action_request request;
                        units_action_request_new(&request, 15);
                        request.evict.flag = false;
                        request.evict.value = immediate;
                        if (action_submit(occupant_index, &request))
                        {
                            unit_cause_player_melee_attack(unit_index, 185, 0, 1.0f);
                            game_engine_record_vehicle_exit(occupant_index, unit->object.parent_object_index);
                            handled = true;
                        }
                    }
                }
            }
            if (handled)
                unit_update_driver_and_gunner(parent_index);
        }
    }
    return false;
}

// 0x32dda8 natural source
// @address 0x32dda8 ?unit_get_acceleration_origin_and_data@@YA_NJPAUreal_matrix4x3@@PAUunit_seat_acceleration@@@Z
bool unit_get_acceleration_origin_and_data(long unit_index, real_matrix4x3 *matrix, unit_seat_acceleration *acceleration)
{
    bool result = false;
    unit_datum *unit = unit_get(unit_index);
    if (unit->object.parent_object_index != NONE)
    {
        if (unit->unit.parent_seat_index != NONE)
        {
            // match: 0x32dda8: unit_get() here moves the parent-index load below the seat-index test
            unit_datum *parent = (unit_datum *)((s_object_header *)object_header_data->data)
                [DATUM_INDEX_TO_ABSOLUTE_INDEX(unit->object.parent_object_index)].datum;
            unit_definition *parent_definition = (unit_definition *)tag_get(0, parent->object.definition_index);
            unit_seat *seat = units_seat_element_get(parent_definition, unit->unit.parent_seat_index);
            object_marker marker;
            int parent_index_value = unit->object.parent_object_index;
            int marker_name_value = seat->marker_name;
            if (internal_object_get_markers_by_string_id((unit->object.parent_object_index), marker_name_value, &marker, 1, false) != 0)
            {
                *matrix = marker.matrix;
                object_get_origin(unit->object.parent_object_index, &matrix->position);
                *acceleration = seat->acceleration;
                result = true;
            }
        }
        else
        {
            unit_datum *parent = unit_get(unit->object.parent_object_index);
            unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
            real_matrix4x3 *node_matrix = (real_matrix4x3 *)((char *)parent + parent->object.node_matrices_offset + unit->object.parent_node_index * 52);
            real_vector3d forward, up;
            matrix4x3_transform_vector(node_matrix, &unit->object.forward, &forward);
            matrix4x3_transform_vector(node_matrix, &unit->object.up, &up);
            real_point3d origin;
            object_get_origin(unit->object.parent_object_index, &origin);
            matrix4x3_rotation_from_vectors(matrix, &forward, &up);
            matrix->position = origin;
            *acceleration = definition->acceleration;
            result = true;
        }
    }
    else
    {
        unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
        matrix4x3_rotation_from_vectors(matrix, &unit->object.forward, &unit->object.up);
        matrix->position = unit->object.position;
        *acceleration = definition->acceleration;
        result = true;
    }
    return result;
}

// 0x32dfe0 natural source
// @address 0x32dfe0 ?unit_seat_try_opening@@YAXJJ_N00@Z
void unit_seat_try_opening(long unit_index, long seat_index, bool a, bool b, bool opening)
{
    if (unit_index != NONE && seat_index != NONE)
    {
        unit_datum *unit = unit_get(unit_index);
        unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
        unit_seat *seat = &definition->seats[seat_index];
        if (FLAG_TEST(seat->flags, 15))
        {
            bool open = FLAG_TEST(seat->flags, 25) || FLAG_TEST(seat->flags, 29);
            if (open == opening)
            {
                action_request request;
                memset(&request, 0, sizeof(request));
                units_action_request_new(&request, 18);
                request.evict.flag = false;
                request.evict.value = false;
                action_submit(unit_index, &request);
            }
        }
    }
}

// 0x32e0e0 natural source

void unit_correct_dual_wield_state(long unit_index, long seat_index, bool a, bool b, bool opening)
{
    if (unit_index != NONE && seat_index != NONE) {
        unit_datum *unit = unit_get(unit_index);
        unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
        unit_seat *seat = &definition->seats[seat_index];
        if (FLAG_TEST(seat->flags, 15)) {
            bool seat_open = FLAG_TEST(seat->flags, 25) || FLAG_TEST(seat->flags, 28);
            if (seat_open == opening) {
                action_request request;
                memset(&request, 0, sizeof(request));
                memset(&request, 0, sizeof(request));
                request.type = 19;
                request.evict.flag = false;
                request.evict.value = false;
                ::action_submit(unit_index, &request);
            }
        }
    }
}


// 0x32e278 natural source
static __forceinline bool units_has_two_weapons(long unit_index)
{
    bool result = false;
    unit_datum *unit = unit_get(unit_index);
    if (unit->unit.current_weapon_index != NONE && unit->unit.desired_weapon_index != NONE)
        result = true;
    return result;
}

extern "C" void unit_update_dual_wield_weapons(long unit_index)
{
    bool has_two = false;
    unit_datum *unit = unit_get(unit_index);
    has_two = units_has_two_weapons(unit_index);
    if (has_two) {
        long primary_weapon = unit_inventory_try_get_weapon(unit_index, unit->unit.current_weapon_index);
        long secondary_weapon = unit_inventory_try_get_weapon(unit_index, unit->unit.desired_weapon_index);
        if (!weapon_can_be_dual_wielded(primary_weapon) || !weapon_can_be_dual_wielded(secondary_weapon)) {
            action_submit_event(unit_index, 47);
        }
    }
}

// 0x32e440 natural source
class c_datamine;
static __forceinline const char *data_mine_get_tag_name(long index) { return "unknown"; }

void data_mine_add_player_death_with_killer_data(c_datamine *datamine, long dead_unit, const s_damage_aftermath_result_data *result_data)
{
    c_static_string<32> killer("unknown");
    real_point3d position;
    position.x = 0.0f;
    memset(&position.y, 0, sizeof(position.y) + sizeof(position.z));
    c_static_string<32> weapon("unknown");
    c_static_string<32> vehicle("unknown");
    long killer_unit = result_data->damage_owner_object_index;
    if (killer_unit != NONE) {
        unit_datum *u = (unit_datum *)object_try_and_get_and_verify_type(killer_unit, 4099);
        if (u) unit_get_body_position(killer_unit, &position);
        if (killer_unit == dead_unit) killer.set("suicide");
        else if (game_globals->options.game_mode == 2) {
            if (result_data->owner_player_index != NONE) {
                player_datum *p = (player_datum *)datum_try_and_get(player_data, result_data->owner_player_index);
                if (p) killer.set_wchar(p->configuration.host.name);
            }
        } else if (u && u->unit.actor_index != NONE) {
            unit_definition *d = (unit_definition *)tag_get(0, u->object.definition_index);
            if (d && d->model.index != NONE) killer.set(data_mine_get_tag_name(d->model.index));
        }
        if (u) {
            vehicle.set("none");
            if ((unsigned short)u->unit.parent_seat_index != 0xffff && u->object.parent_object_index != NONE)
                vehicle.set(data_mine_get_tag_name(u->object.parent_object_index));
        }
        if (result_data->damage_definition_index != NONE) weapon.set(data_mine_get_tag_name(result_data->damage_definition_index));
    }
}

// 0x32e608 natural source
void data_mine_insert_sp_actor_death(long victim_unit_index, const s_damage_aftermath_result_data *damage_aftermath_result_data)
{
    if (victim_unit_index != NONE && damage_aftermath_result_data != NULL) {
        unit_datum *victim = (unit_datum *)object_try_and_get_and_verify_type(victim_unit_index, 4099);
        if (victim && victim->unit.actor_index != NONE) {
            long killer_index = damage_aftermath_result_data->damage_owner_object_index;
            if (killer_index != NONE) {
                unit_datum *killer = (unit_datum *)object_try_and_get_and_verify_type(killer_index, 4099);
                if (killer) {
                    real_point3d killer_position = {0.0f}, victim_position = {0.0f};
                    unit_get_body_position(killer_index, &killer_position);
                    unit_get_body_position(victim_unit_index, &victim_position);
                    if (killer->unit.player_index != NONE) {
                        c_datamine mine(0, "actor death by player", 2, "units", "campaign");
                    } else if (killer->unit.actor_index != 0) {
                        c_datamine mine(0, "actor death by actor", 2, "units", "campaign");
                    }
                }
            }
        }
    }
}


// 0x323978 natural source
bool unit_update_active_camouflage(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    bool changed = false;
    c_multi_unit_passenger_iterator iterator;

    if (unit->unit.damage_stun_ticks > 0) {
        if (--unit->unit.damage_stun_ticks == 0)
            unit->unit.look_difficulty_scale = 0.0f;
        changed = 1;
    }

    if (unit->unit.damage_timer > 0) {
        if (--unit->unit.damage_timer == 0) {
            ai_handle_damage(unit_index, unit->unit.damaging_object_index, unit->unit.damage_type, 0, unit->unit.damage, 0, 1);
            unit->unit.damage = 0.0f;
            unit->unit.damage_type = 0;
            unit->unit.damaging_object_index = NONE;
        }
        changed = 1;
    }

    if (unit->unit.stun_ticks > 0) {
        bool skip = false;
        if (game_globals->options.game_simulation == 4)
            skip = true;
        if (skip == 0) {
            unit->unit.stun_ticks--;
            if (unit->object.object_type == 1)
                ::simulation_action_object_update(unit_index, 0x10000);
            changed = 1;
        }
    }

    if (unit->unit.emp_shield_stun_ticks > 0)
        unit->unit.emp_shield_stun_ticks--;

    if ((bool)(_rotl(unit->unit.flags400, 31) & 1) &&
        unit->unit.actor_index == NONE &&
        unit->unit.driver_object_index == NONE &&
        !(bool)(_rotl(unit->object.damage_flags, 30) & 1))
    {
        iterator.begin(unit_index);
        if (iterator.next()) {
            do {
                if (iterator.m_seat_index != NONE && iterator.m_parent_index != NONE) {
                    unit_datum *parent = unit_get(iterator.m_parent_index);
                    unit_definition *parent_definition = (unit_definition *)tag_get(0, parent->object.definition_index);
                    unit_seat *seat = units_seat_element_get(parent_definition, iterator.m_seat_index);
                    if ((bool)(_rotl(seat->flags, 21) & 1)) {
                        action_submit_event(iterator.m_passenger_index, 20);
                    }
                }
            } while (iterator.next());
        }
        if (unit_index != NONE) {
            unit_datum *target = unit_get(unit_index);
            target->object.damage_flags |= 64;
            object_wake(unit_index);
        }
    }

    return changed;
}

// moves the unit's active camouflage toward its target at the regrowth rate, and the second fade toward 0 or 1
// @address 0x323bc8 unit_update_active_camouflage_fade
extern "C" void unit_update_active_camouflage_fade(long unit_index)
{
    bool changed = false;
    unit_datum *unit = unit_get(unit_index);
    if (FLAG_TEST(unit->unit.unit_flags, 3)) {
        bool rising = unit->unit.active_camouflage < unit->unit.active_camouflage_target;
        float target = unit->unit.active_camouflage_target;
        unit->unit.active_camouflage += game_tick_length() * unit->unit.active_camouflage_regrowth * (rising ? 1.0f : -1.0f);
        if ((rising && unit->unit.active_camouflage > target) || (!rising && unit->unit.active_camouflage < target)) {
            unit->unit.active_camouflage = target;
            changed = true;
        }
    } else {
        unit->unit.active_camouflage -= game_tick_length() * unit->unit.active_camouflage_regrowth;
        if (unit->unit.active_camouflage < 0.0f) {
            unit->unit.active_camouflage = 0.0f;
            changed = true;
        }
    }
    if (changed)
        ::simulation_action_object_update(unit_index, 0x8000000);
    if (FLAG_TEST(unit->unit.unit_flags, 4)) {
        unit->unit.active_camouflage_secondary_fade += game_tick_length() * 0.33333334f;
        if (unit->unit.active_camouflage_secondary_fade > 1.0f)
            unit->unit.active_camouflage_secondary_fade = 1.0f;
    } else {
        unit->unit.active_camouflage_secondary_fade -= game_tick_length() * 0.33333334f;
        if (unit->unit.active_camouflage_secondary_fade < 0.0f)
            unit->unit.active_camouflage_secondary_fade = 0.0f;
    }
}


// @address 0x323d58 ?unit_update_team_index@@YAXJ@Z
void unit_update_team_index(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    if (flag_test(unit->object.damage_flags, 2L) == 0)
    {
        long player = unit->unit.player_index;
        if (player != NONE)
        {
            unit->unit.team = (e_game_team)datum_get<player_datum>(player_data, player)->configuration.host.team_index;
            return;
        }
        long object = unit->unit.actor_index;
        if (object != NONE)
        {
            unit->unit.team = (e_game_team)datum_get<actor_datum>(actor_data, object)->team;
            return;
        }
        else if (unit->object.object_type == 1)
        {
            long tr28 = NONE;
            long tr27 = NONE;
            c_multi_unit_passenger_iterator iterator;
            iterator.begin(unit_index);
            if (iterator.next())
            {
                while (tr28 == NONE || tr27 == NONE)
                {
                    short seat = (short)iterator.m_seat_index;
                    unit_datum *passenger = unit_get(iterator.m_passenger_index);
                    if (seat != NONE)
                    {
                        long parent = iterator.m_parent_index;
                        if (parent != NONE)
                        {
                            unit_definition *definition = (unit_definition *)tag_get(0, unit_get(parent)->object.definition_index);
                            if (flag_test(definition->seats[seat].flags, 11L))
                                tr27 = passenger->unit.team;
                            else
                                tr28 = passenger->unit.team;
                        }
                    }
                    if (!iterator.next())
                        break;
                }
            }
            if (tr28 == NONE && tr27 == NONE)
                unit->unit.team = (e_game_team)NONE;
            else if (tr28 != tr27 && tr27 != NONE)
                unit->unit.team = (e_game_team)NONE;
            else
                unit->unit.team = (e_game_team)tr28;
            return;
        }
        else
        {
            return;
        }
    }
}

// 0x324270 natural source

// @address 0x324270 ?unit_euler_axis_couple@@YAXPAUeuler_axis_plan@@0MM@Z
void unit_euler_axis_couple(struct euler_axis_plan * plan1, struct euler_axis_plan * plan2, float max_velocity, float acceleration)
{
    euler_axis_plan *adjust_plan = NULL;
    float t_extension;

    if (!plan1->stationary && !plan2->stationary)
    {
        float t1 = plan1->accel_t + plan1->coast_t + plan1->decel_t;
        float t2 = plan2->accel_t + plan2->coast_t + plan2->decel_t;
        if (plan1->accel_t > 0.0f && t1 < t2)
        {
            adjust_plan = plan1;
            t_extension = t2 - t1;
        }
        else if (plan2->accel_t > 0.0f && t2 < t1)
        {
            adjust_plan = plan2;
            t_extension = t1 - t2;
        }
    }

    if (adjust_plan)
    {
        float a = acceleration;
        float b = acceleration * (adjust_plan->coast_t + t_extension);
        float c = -t_extension * (float)fabs(adjust_plan->velocity + adjust_plan->acceleration * adjust_plan->accel_t);
        float disc = b * b - 4.0f * a * c;
        float new_accel_t = (-b + square_root(disc)) / (2.0f * a);
        new_accel_t = new_accel_t > (adjust_plan->accel_t > adjust_plan->decel_t ? adjust_plan->decel_t : adjust_plan->accel_t)
            ? (adjust_plan->accel_t > adjust_plan->decel_t ? adjust_plan->decel_t : adjust_plan->accel_t)
            : new_accel_t;
        if (new_accel_t > 0.0f)
        {
            float v = adjust_plan->velocity + adjust_plan->acceleration * (adjust_plan->accel_t - new_accel_t);
            if ((float)fabs(v) < 0.0001f)
            {
                adjust_plan->stationary = true;
                adjust_plan->accel_t = 0.0f;
                adjust_plan->decel_t = 0.0f;
                adjust_plan->coast_t = 0.0f;
            }
            else
            {
                float distance = 2.0f * v * new_accel_t + adjust_plan->acceleration * new_accel_t * new_accel_t;
                float coast_t = distance / v;
                adjust_plan->accel_t -= new_accel_t;
                adjust_plan->decel_t -= new_accel_t;
                adjust_plan->coast_t = coast_t;
            }
        }
    }
}

// 0x3243d8 natural source
bool unit_euler_axis_doplan(euler_axis_plan *plan, float time, float position, float *position_out, float velocity, float *velocity_out)
{
    bool done = plan->stationary;
    float new_position = position;
    float new_velocity = velocity;

    if (!done)
    {
        float remaining = time;
        if (remaining > 0.0f && plan->accel_t > 0.0f)
        {
            float t = remaining > plan->accel_t ? plan->accel_t : remaining;
            new_position = new_position + (new_velocity * t + plan->acceleration * t * t / 2.0f);
            new_velocity = new_velocity + plan->acceleration * t;
            remaining = remaining - t;
        }
        if (remaining > 0.0f && plan->coast_t > 0.0f)
        {
            float t = remaining > plan->coast_t ? plan->coast_t : remaining;
            new_position = new_position + new_velocity * t;
            remaining = remaining - t;
        }
        if (remaining > 0.0f && plan->decel_t > 0.0f)
        {
            float t = remaining > plan->decel_t ? plan->decel_t : remaining;
            new_position = new_position + (new_velocity * t + plan->deceleration * t * t / 2.0f);
            new_velocity = new_velocity + plan->deceleration * t;
            remaining = remaining - t;
        }
        if (remaining > 0.0f)
            done = true;
    }
    *position_out = new_position;
    *velocity_out = new_velocity;
    return done;
}

bool unit_seat_test_flag(long unit_index, short seat_index, e_unit_seat_flags bit)
{
    bool result = false;
    unit_definition *definition = (unit_definition *)tag_get(0, unit_get(unit_index)->object.definition_index);
    if (seat_index >= 0 && (int)seat_index < definition->seats.count())
        result = (definition->seats[seat_index].flags & 1 << (unsigned int)bit) != 0;
    return result;
}

// 0x3282f0 natural source
void unit_delete_current_equipment(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    long equipment_index = unit->unit.equipment_object_index;
    if (equipment_index != NONE)
    {
        unit_drop_item(unit_index, equipment_index, _unit_drop_type_delete);
        unit->unit.equipment_object_index = NONE;
        unit->unit.equipment_pickup_time = NONE;
        ::simulation_action_object_update(unit_index, 0x10000000);
        long datum_index = unit->unit.actor_index;
        if (datum_index != NONE)
            datum_get<actor_datum>(actor_data, datum_index)->equipment_index = NONE;
    }
}

// 0x3272c8 natural source
void unit_set_actively_controlled(long unit_index, bool actively_controlled)
{
    unit_datum *unit = unit_get(unit_index);
    if (!(unit->unit.actor_index == NONE && unit->unit.simulation_actor_datum_index == NONE && unit->unit.player_index == NONE))
        actively_controlled = true;
    if (flag_test(unit->object.damage_flags, 2L))
        actively_controlled = false;
    unit->unit.m_unit_flags.set(_unit_flag_actively_controlled, actively_controlled);
    if (actively_controlled)
        unit->unit.unit_flags |= 1;
    else
        unit->unit.unit_flags &= ~1u;
    long *weapon_index = unit->unit.weapon_object_indices;
    unsigned int remaining = 4;
    do
    {
        if (*weapon_index != NONE)
            item_update_player_inventory(*weapon_index);
        remaining = remaining - 1;
        weapon_index = weapon_index + 1;
    } while (remaining != 0);
    ::unit_update_driver_and_gunner(unit_index);
}

// @address 0x3273c0 unit_is_player_controlled
extern "C" bool unit_is_player_controlled(long unit_index)
{
    bool result = false;
    if (unit_index != NONE) {
        unit_datum *unit = unit_get(unit_index);
        if (unit->unit.player_index != NONE && FLAG_TEST(unit->unit.unit_flags, 1))
            result = true;
        else {
            long driver_index = unit->unit.driver_object_index;
            if (driver_index != NONE)
                result = unit_get(driver_index)->unit.player_index != NONE;
        }
    }
    return result;
}

// 0x328220 natural source
void unit_drop_current_equipment(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    long equipment_index = unit->unit.equipment_object_index;
    if (equipment_index != NONE)
    {
        bool skip = false;
        if (game_globals->options.game_simulation == 4)
            skip = true;
        if (!skip)
        {
            if (equipment_remaining_charges(equipment_index) == 0)
            {
                unit_delete_current_equipment(unit_index);
                return;
            }
            unit_datum *equipment = unit_get(equipment_index);
            unit_drop_item(unit_index, equipment_index, _unit_drop_type_drop);
            if (!flag_test(equipment->object.physics_flags, 7L))
                ::object_reconnect_to_physics(equipment_index);
            unit->unit.equipment_object_index = NONE;
            unit->unit.equipment_pickup_time = NONE;
            ::simulation_action_object_update(unit_index, 0x10000000);
        }
    }
}

// 0x328738 natural source
void unit_delete_all_weapons_internal(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    for (int weapon_index = 0; weapon_index < 4; weapon_index++)
    {
        if (unit->unit.weapon_object_indices[weapon_index] != NONE)
        {
            short index = (short)weapon_index;
            unit_datum *datum = unit_get(unit_index);
            long object_index = datum->unit.weapon_object_indices[index];
            datum->unit.weapon_object_indices[index] = NONE;
            unit_drop_item(unit_index, object_index, _unit_drop_type_delete);
            ::simulation_action_object_update(unit_index, (1 << (index + 22)) | (1 << (index + 18)));
            unit_inventory_cycle_weapon_set_identifier(unit_index);
        }
    }
}

// @address 0x3287f0 ?unit_delete_all_weapons@@YAXJ@Z
void unit_delete_all_weapons(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    action_request drop_request;
    action_request stow_request;
    unit->unit.desired_weapon_indices[0] = NONE;
    if (unit->unit.current_weapon_index != NONE)
    {
        memset(&drop_request, 0, sizeof(drop_request));
        units_action_request_new(&drop_request, 54);
        drop_request.evict.flag = true;
        action_submit(unit_index, &drop_request);
    }
    unit->unit.desired_weapon_indices[1] = NONE;
    if (unit->unit.desired_weapon_index != NONE)
    {
        memset(&stow_request, 0, sizeof(stow_request));
        units_action_request_new(&stow_request, 55);
        stow_request.evict.flag = true;
        action_submit(unit_index, &stow_request);
    }
    unit_delete_all_weapons_internal(unit_index);
}


bool unit_put_away_weapon(long unit_index, short weapon_set_index, bool flag)
{
    unit_datum *unit = unit_get(unit_index);
    bool result = false;
    if (unit->unit.current_weapon_indices[weapon_set_index] != NONE && unit->unit.desired_weapon_indices[weapon_set_index] != NONE) {
        action_request request;
        memset(&request, 0, sizeof(request));
        unit->unit.desired_weapon_indices[weapon_set_index] = NONE;
        units_action_request_new(&request, (weapon_set_index != 0) + 54);
        request.evict.flag = flag;
        if (::action_submit(unit_index, &request)) {
            if (weapon_set_index != 0)
                unit_inventory_cycle_weapon_set_identifier(unit_index);
            if (unit->unit.player_index != NONE)
                units_player_handle_weapon_put_away(unit->unit.player_index, weapon_set_index);
            result = true;
        }
    }
    return result;
}

// 0x32a060 natural source
void unit_drop_item(long unit_index, long object_index, e_unit_drop_type drop_type)
{
    unit_datum *unit = unit_get(unit_index);
    unit_datum *object = unit_get(object_index);
    unit_definition *definition = (unit_definition *)tag_get(0, object->object.definition_index);
    if (FLAG_TEST(unit->unit.unit_flags, 15))
        drop_type = _unit_drop_type_delete;
    if (definition->model.index == NONE)
        drop_type = _unit_drop_type_delete;
    if (object->unit.kind == 3)
        ::item_unstow_inventory(object_index, unit_index, 0, 0);
    else if (object->unit.kind == 2)
        ::item_show_inventory_unattached(object_index, unit_index);
    ::item_in_unit_inventory(object_index, NONE);
    if ((int)drop_type == 1)
    {
        object_delete(object_index);
        return;
    }
    int value = 0;
    bool flag = true;
    switch ((int)drop_type)
    {
    case 2:
        value = 583;
        flag = false;
        break;
    case 3:
        value = 464;
        flag = false;
        break;
    }
    unit_toss_item(unit_index, object_index, value, 1.0f, flag);
}

// 0x32a728 natural source
void unit_inventory_set_weapon_index(long unit_index, short inventory_index, long object_index, e_unit_drop_type unit_drop_type)
{
    unit_datum *unit = unit_get(unit_index);
    if (object_index != NONE)
    {
        ::item_in_unit_inventory(object_index, unit_index);
        ::item_hide_inventory(object_index);
        unit->unit.weapon_object_indices[inventory_index] = object_index;
        unit->unit.weapon_ages[inventory_index] = 0;
    }
    else
    {
        object_index = unit->unit.weapon_object_indices[inventory_index];
        unit->unit.weapon_object_indices[inventory_index] = NONE;
        if ((int)unit_drop_type != 4)
            unit_drop_item(unit_index, object_index, unit_drop_type);
    }
    ::simulation_action_object_update(unit_index, (1 << (inventory_index + 22)) | (1 << (inventory_index + 18)));
    unit_inventory_cycle_weapon_set_identifier(unit_index);
}

// 0x32e728 natural source
// @address 0x32e728 ?unit_placement_try_to_put_unit_in_seat_using_marker_name@@YA_NJJJ@Z
bool unit_placement_try_to_put_unit_in_seat_using_marker_name(long unit_index, long parent_index, long marker_name)
{
    bool success = false;
    long seat_index = NONE;
    long seat_parent_index = NONE;
    unit_seat_source sources[64];
    short seat_count = 0;
    ::unit_get_all_seats_internal(parent_index, sources, &seat_count, 64, false);
    for (short index = 0; seat_index == NONE && index < seat_count; index++)
    {
        unit_seat_source *source = &sources[index];
        if (source->definition->marker_name == marker_name)
        {
            seat_index = source->seat_index;
            seat_parent_index = source->unit_index;
        }
    }
    if (seat_index != NONE && seat_parent_index != NONE)
    {
        action_request request;
        units_action_request_new(&request, 17);
        request.seat.parent_index = seat_parent_index;
        request.seat.seat_index = (short)seat_index;
        request.seat.flag_a = true;
        request.seat.flag_b = false;
        if (action_submit(unit_index, &request))
            success = true;
    }
    return success;
}

static __forceinline u_unit_dialogue_state *units_get_dialogue_state(unit_datum *unit)
{
    return (u_unit_dialogue_state *)((char *)unit + unit->unit.dialogue_offset);
}

void unit_dialogue_set_variant(long object_index, unsigned char alternate)
{
    unit_datum *unit = unit_get(object_index);
    u_unit_dialogue_state *entry = units_get_dialogue_state(unit);
    unit_definition *unit_def = (unit_definition *)tag_get(0, unit->object.definition_index);
    s_model_definition *definition = (s_model_definition *)tag_get(0, unit_def->model.index);
    long tag;
    if (alternate != 0) {
        tag = definition->alternate_dialogue.index;
        if (tag == -1)
            tag = definition->default_dialogue.index;
    } else {
        tag = definition->default_dialogue.index;
    }
    if (tag == -1)
        return;
    u_unit_dialogue_state *target = units_get_dialogue_state(unit_get(object_index));
    dialogue_definition *val_def = (dialogue_definition *)tag_get(0, tag);
    long value = val_def->mission_dialogue_designator;
    target->live.tag = tag;
    target->live.value = value;
    entry->live.other = definition->dialogue_variant;
}


// ---- NEW ----
float unit_get_boarding_progress(long object_index)
{
    float result = real_constant_pool_one();
    if (object_index != NONE) {
        c_animation_manager *motor = object_get_motor(object_index);
        if (motor->get_state_name() == 96)
            result = animation_channel_get_playback_ratio((const c_animation_channel *)motor);
        else if (motor->get_state_name() == 97)
            result = real_constant_pool_one() - animation_channel_get_playback_ratio((const c_animation_channel *)motor);
    }
    return result;
}

// 0x328378 natural source

static __forceinline bool units_player_weapon_pickup_allowed(long player_index)
{
    player_datum *player = (player_datum *)(player_data->data + (player_index & 0xffff) * sizeof(player_datum));
    return player->traits.get_weapons_traits()->get_weapon_pickup_allowed();
}

static __forceinline bool units_equipment_should_replace(long definition_index, long equipment_index, long current_equipment_index)
{
    item_datum *current = (item_datum *)unit_get(current_equipment_index);
    if (definition_index == current->object.definition_index) {
        if (equipment_remaining_charges(equipment_index) <= equipment_remaining_charges(current_equipment_index))
            return false;
    }
    return true;
}

bool unit_can_pickup_equipment(long unit_index, long equipment_index)
{
    s_object_header *headers = (s_object_header *)object_header_data->data;
    item_datum *equipment = (item_datum *)object_get(equipment_index);
    long definition_index = equipment->object.definition_index;
    long equipment_type = equipment_definition_get_type(definition_index, 0);
    unit_datum *unit = (unit_datum *)headers[datum_index_to_absolute_index(unit_index)].datum;
    long current_equipment_index = unit->unit.equipment_object_index;
    bool result = equipment_type == 1 || (equipment_type >= 0 && equipment_type < 9 && equipment_remaining_charges(equipment_index) != 0);
    result = result && equipment->owner_relation == 0;
    result = result && !action_executing(unit_index, _action_equipment);
    if (result) {
        long player_index = unit->unit.player_index;
        if (equipment_type == 1) {
            result = game_globals->options.game_mode == 2 && game_engine_running();
        } else {
            if (current_equipment_index != NONE) {
                item_datum *current_equipment = (item_datum *)headers[datum_index_to_absolute_index(current_equipment_index)].datum;
                __int64 decision;
                if (definition_index != current_equipment->object.definition_index ||
                    equipment_remaining_charges(equipment_index) > equipment_remaining_charges(current_equipment_index))
                    decision = 1;
                else
                    decision = 0;
                bool better = (bool)decision;
                result = better && equipment_active_fraction(current_equipment_index) == 0.0f;
            }
            if (result && player_index != NONE)
                result = units_player_weapon_pickup_allowed(player_index);
        }
    }
    return result;
}

// 0x3291b0 natural source

static bool units_weapon_must_be_readied(long weapon_index)
{
    object_datum *weapon = (object_datum *)unit_get(weapon_index);
    struct weapon_definition *definition = (struct weapon_definition *)tag_get(0, weapon->definition_index);
    return FLAG_TEST(definition->flags, 3);
}


bool unit_can_use_weapon_in_seat(long unit_index, long seat_label, long weapon_index)
{
    unit_datum *unit = unit_get(unit_index);
    object_datum *weapon = (object_datum *)unit_get(weapon_index);
    struct weapon_definition *definition = (struct weapon_definition *)tag_get(0, weapon->definition_index);
    bool readied_or_support = units_weapon_must_be_readied(weapon_index) || weapon_is_support_weapon(weapon_index);
    bool must_be_readied = false;
    bool seat_blocks_weapon;
    bool seat_is_gunner;
    long default_label;
    if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE) {
        unit_datum *parent = unit_get(unit->object.parent_object_index);
        unit_definition *parent_definition = (unit_definition *)tag_get(0, parent->object.definition_index);
        unit_seat *seat = &parent_definition->seats[unit->unit.parent_seat_index];
        default_label = seat->label;
        seat_blocks_weapon = !FLAG_TEST(seat->flags, 5);
        seat_is_gunner = FLAG_TEST(seat->flags, 2) || FLAG_TEST(seat->flags, 3);
    } else {
        default_label = 226;
        seat_blocks_weapon = false;
        seat_is_gunner = false;
        must_be_readied = readied_or_support;
    }
    if (seat_label == NONE || seat_label == 226)
        seat_label = default_label;
    bool result;
    if (((1 << unit->object.object_type) & 2) != 0) {
        result = true;
    } else if (weapon_is_support_weapon(weapon_index) && seat_is_gunner) {
        result = false;
    } else if (seat_blocks_weapon) {
        result = false;
    } else {
        result = unit_animation_test_weapon(unit_index, seat_label, definition->animation_weapon_class,
            definition->animation_weapon_type, must_be_readied);
    }
    if (unit->unit.player_index != NONE && FLAG_TEST(definition->flags, 29))
        result = false;
    if (result && weapon->owner_team_index != NONE)
        result = unit->unit.team - weapon->owner_team_index == 0;
    return result;
}

// 0x32cb90 natural source




void unit_start_running_blindly(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    if (!unit->unit.m_unit_flags.test(_unit_flag_running_blindly)) {
        unit->unit.m_unit_flags.set(_unit_flag_running_blindly, true);
        union { real_vector3d vector; real_euler_angles2d angles; } scratch;
        float maximum;
        if (unit->unit.actor_index != NONE && actor_get_running_blind_vector(unit->unit.actor_index, &scratch.vector)) {
            unit->unit.running_blind_angle = 0.0f;
            maximum = 25.0f * 0.017453292f;
        } else {
            euler_angles2d_from_vector3d(&scratch.angles, &unit->object.forward);
            float yaw = scratch.angles.yaw;
            if (yaw > 3.1415927f)
                yaw -= 6.2831855f;
            unit->unit.running_blind_angle = yaw;
            maximum = 100.0f * 0.017453292f;
        }
        float random = _real_random_range(TLS_RANDOM_SEED, -maximum, maximum);
        unit->unit.running_blind_angle += random;
    }
}

static const real_vector3d k_units_up3d = { 0.0f, 0.0f, 1.0f };



// @address 0x32ccc8 ?unit_running_blind@@YAXJPATreal_vector3d@@@Z
void unit_running_blind(long unit_index, real_vector3d *run_vector)
{
    unit_datum *unit = unit_get(unit_index);
    bool actor_vector = false;
    if (unit->unit.actor_index != NONE && actor_get_running_blind_vector(unit->unit.actor_index, run_vector))
        actor_vector = true;
    else
        *run_vector = *global_forward3d;

    float positive_scale = 1.0f;
    float negative_scale = 1.0f;
    if (actor_vector)
    {
        float angle_left = 0.7853982f - unit->unit.running_blind_angle;
        float angle_right = 0.7853982f + unit->unit.running_blind_angle;
        positive_scale = positive_scale > (angle_left / (0.7853982f * 0.3f)) ? (angle_left / (0.7853982f * 0.3f)) : positive_scale;
        negative_scale = negative_scale > (angle_right / (0.7853982f * 0.3f)) ? (angle_right / (0.7853982f * 0.3f)) : negative_scale;
    }
    float rate_left = 6.2831855f - unit->unit.running_blind_rate;
    float rate_right = 6.2831855f + unit->unit.running_blind_rate;
    positive_scale = positive_scale > (rate_left / (6.2831855f * 0.3f)) ? (rate_left / (6.2831855f * 0.3f)) : positive_scale;
    negative_scale = negative_scale > (rate_right / (6.2831855f * 0.3f)) ? (rate_right / (6.2831855f * 0.3f)) : negative_scale;

    float change;
    if (positive_scale < negative_scale)
    {
        if (positive_scale < -1.0f)
            change = -(6.2831855f / 10.0f);
        else
            change = _real_random_range(TLS_RANDOM_SEED, -(6.2831855f / 10.0f), (6.2831855f / 10.0f) * units_min(positive_scale, 1.0f));
    }
    else
    {
        if (negative_scale < -1.0f)
            change = 6.2831855f / 10.0f;
        else
            change = _real_random_range(TLS_RANDOM_SEED, -(6.2831855f / 10.0f) * units_min(negative_scale, 1.0f), 6.2831855f / 10.0f);
    }

    unit->unit.running_blind_rate += change;
    unit->unit.running_blind_angle += game_tick_length() * unit->unit.running_blind_rate;
    if (unit->unit.running_blind_angle < -3.1415927f)
        unit->unit.running_blind_angle += 6.2831855f;
    else if (unit->unit.running_blind_angle > 3.1415927f)
        unit->unit.running_blind_angle -= 6.2831855f;

    rotate_vector_about_axis(run_vector, &k_units_up3d, (float)sin(unit->unit.running_blind_angle),
        (float)cos(unit->unit.running_blind_angle));
}

// 0xd9470 natural source

short unit_weapon_next_index(long unit_index, long seat_label, short current_index, short direction)
{
    unit_datum *unit = unit_get(unit_index);
    short result = NONE;
    bool best_readied = false;
    long best_age = 0;
    if (current_index == NONE)
        current_index = 0;
    else
        current_index = (current_index + direction + 4) % 4;
    short index = current_index;
    do {
        if (unit->unit.weapon_object_indices[index] != NONE &&
            unit_can_use_weapon_in_seat(unit_index, seat_label, unit->unit.weapon_object_indices[index]) &&
            index != unit->unit.desired_weapon_index)
        {
            long age = unit->unit.weapon_ages[index];
            bool readied = units_weapon_must_be_readied(unit->unit.weapon_object_indices[index]) ||
                weapon_is_support_weapon(unit->unit.weapon_object_indices[index]);
            bool take = false;
            if (result == NONE) {
                take = true;
            } else if (!best_readied) {
                if (readied)
                    take = true;
                else if (direction == 0)
                    take = age > best_age;
            }
            if (take) {
                result = index;
                best_age = age;
                best_readied = readied;
            }
        }
        if (direction < 0)
            index = (index + 3) % 4;
        else
            index = (index + 1) % 4;
    } while (index != current_index);
    return result;
}

static __forceinline float dot_product3d(const real_vector3d *a, const real_vector3d *b);
static __forceinline real_point3d *point_from_line3d(const real_point3d *point, const real_vector3d *vector, float scale, real_point3d *result);







// @address 0x324588 ?unit_adjust_projectile_ray@@YAXJPATreal_point3d@@PATreal_vector3d@@10_N22@Z
void unit_adjust_projectile_ray(long unit_index, real_point3d *origin, real_vector3d *forward, real_vector3d *velocity,
    real_point3d *offset, bool snap_to_aim_line, bool use_aiming_vector, bool find_good_point)
{
    unit_datum *unit = unit_get(unit_index);
    real_point3d aim_origin;
    if (*(long *)((char *)unit + unit->unit.motor_offset) == 6)
        unit_get_head_position(unit_index, &aim_origin);
    else
        unit_get_camera_position(unit_index, &aim_origin);

    if (use_aiming_vector)
        *forward = unit->unit.aiming_vector;

    if (snap_to_aim_line)
    {
        real_vector3d to_origin;
        vector_from_points3d(&aim_origin, origin, &to_origin);
        point_from_line3d(&aim_origin, forward, to_origin.i * forward->i + (to_origin.j * forward->j + to_origin.k * forward->k), origin);
    }

    if (offset)
    {
        real_vector3d left;
        real_vector3d up;
        cross_product3d(global_up3d, forward, &left);
        if (normalize3d(&left) == 0.0f)
            left = *global_left3d;
        cross_product3d(forward, &left, &up);
        normalize3d(&up);
        point_from_line3d(origin, forward, offset->x, origin);
        point_from_line3d(origin, &left, offset->y, origin);
        point_from_line3d(origin, &up, offset->z, origin);
    }

    real_point3d good_point;
    if (collision_find_good_projectile_point(&aim_origin, origin, &good_point, object_get_root_object(unit_index), NONE))
        *origin = good_point;

    object_get_velocities(inl_object_get_ultimate_parent(unit_index), velocity, 0);
}



// @address 0x324878 ?unit_unzoom@@YAXJ_N@Z
void unit_unzoom(long unit_index, bool unknown)
{
    unit_datum *unit = unit_get(unit_index);
    long weapon_index = unit_inventory_try_get_weapon(unit_index, unit->unit.current_weapon_index);
    bool can_zoom;
    if (weapon_index == NONE)
        can_zoom = false;
    else
        can_zoom = weapon_rotate_zoom_level(weapon_index, NONE) != NONE;
    bool any_user = false;
    bool was_zoomed = false;
    if (!unknown || !can_zoom) {
        unit_datum *zoom = unit;
        long user_index = NONE;
        long player_index;
        was_zoomed = zoom->unit.zoom_level != NONE;
        zoom->unit.zoom_transition_timer = real_constant_pool_zero();
        zoom->unit.zoom_level = NONE;
        zoom->unit.desired_zoom_level = NONE;
        player_index = unit_get(unit_index)->unit.player_index;
        for (;;) {
            if (player_index != NONE)
                user_index = ((long)::director_get_perspective(player_index, user_index));
            if (user_index == NONE)
                break;
            s_player_control_user_state *state = &((s_player_control_globals *)player_control_globals)->users[user_index];
            any_user = true;
            state->action.zoom = NONE;
            state->zoom_held = false;
        }
    }
    if (was_zoomed && any_user) {
        long sound = NONE;
        if (can_zoom && !unknown) {
            if (weapon_index == NONE)
                return;
            sound = ((struct weapon_definition *)tag_get(0, unit_get(weapon_index)->object.definition_index))->zoom_out_sound;
        } else {
            long index;
            s_game_globals *globals;
            index = unit->unit.player_index != NONE ? game_get_player_character_type(unit->unit.player_index) : 0;
            if (index < 0)
                return;
            globals = (s_game_globals *)global_game_globals;
            if (index >= globals->player_representations.count())
                return;
            sound = globals->player_representations[index].binoculars_zoom_out_sound.index;
        }
        if (sound != NONE) {
            c_game_sound_impulse_parameters request(sound, real_constant_pool_one());
            request.submit_unattached_object_sound(unit_index, 157);
        }
    }
}


// 0x3220e0 natural source
// @address 0x3220e0 ?unit_arbiter_active_camouflage_play_sound@@YAXJF@Z
void unit_arbiter_active_camouflage_play_sound(long unit_index, short sound)
{
    long definition_index =
        ((s_player_information_definition *)((s_game_globals *)global_game_globals)->player_information.address)->active_camouflage_sounds[sound].index;
    if (definition_index != NONE) {
        c_game_sound_impulse_parameters request(definition_index, 1.0f);
        request.submit_unattached_object_sound(unit_index, 0x9d);
    }
}

static __forceinline float units_physics_offset_component(float origin, const float &offset)
{
    return offset + origin;
}
// @address 0x122b60 unit_calculate_is_in_illegal_position
extern "C" int unit_calculate_is_in_illegal_position(int unit_index)
{
    long parent_index = inl_object_get_ultimate_parent(unit_index);
    s_object_header *header = object_header_get(parent_index);
    object_datum *object = header->datum;
    int result = 0;
    if (cluster_reference_valid(&header->cluster_reference)
        && (header->flags & 1) != 0
        && (header->flags & 64) != 0
        && object->m_physics_flags.test(_object_physics_connected_bit)
        && object->havok_component_index != NONE)
    {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);
        long rigid_body_index = component->get_object_postion_controlling_rigid_body_index();
        if (rigid_body_index != NONE
            && !component->get_rigid_body_is_fixed(rigid_body_index)
            && !component->get_rigid_body_is_keyframed(rigid_body_index))
        {
            s_collision_test_flags flags;
            flags.object_flags = 0;
            flags.collision_flags = 0x14809;
            real_point3d center_of_mass;
            component->get_rigid_body_center_of_mass(rigid_body_index, &center_of_mass);
            if (collision_test_point(flags, &center_of_mass, parent_index, unit_index, 0))
            {
                center_of_mass.z += 0.2f;
                if (collision_test_point(flags, &center_of_mass, parent_index, unit_index, 0))
                    result = 1;
            }
        }
    }
    return result;
}

// whether the player should take the weapon automatically: an unarmed player always does, multiplayer asks the
// game engine, otherwise only a player without a backpack weapon who is not already dual wielding
// @address 0x1dcbf0 ?player_should_auto_pickup_weapon@@YA_NJJ@Z
bool player_should_auto_pickup_weapon(long player_index, long weapon_index)
{
    player_datum *player = player_get(player_index);
    if (unit_weapon_next_index(player->unit_index, NONE, NONE, 0) == NONE && !weapon_is_support_weapon_or_must_be_readied(weapon_index))
        return true;
    if (game_globals->options.game_mode == 2)
    {
        bool result = false;
        c_game_engine *engine = game_engine_get();
        if (engine)
            result = engine->should_auto_pickup_weapon(player_index, weapon_index);
        return result;
    }
    long unit_index = player->unit_index;
    if (unit_get_current_backpack_weapon(unit_index) == NONE)
    {
        unit_datum *unit = unit_get(unit_index);
        bool dual_wielding = false;
        if (unit->unit.current_weapon_set.indices[0] != 0xff && unit->unit.current_weapon_set.indices[1] != 0xff)
            dual_wielding = true;
        if (!dual_wielding)
            return true;
    }
    return false;
}

// Clears every reference the unit holds to an object being deleted: its driver and gunner, its
// inventory weapons (cycling the weapon set), equipment, predicted vehicle, dialogue subject and
// its action's object and target (with unit_dialogue_handle_object_deleted and
// unit_action_handle_deleted_object inlined).
// @address 0x110d00 ?unit_handle_deleted_object@@YAXJJ@Z
void unit_handle_deleted_object(long unit_index, long object_index)
{
    unit_datum *unit = unit_get(unit_index);
    bool current_weapon_deleted = false;
    if (unit->unit.driver_object_index == object_index)
        unit->unit.driver_object_index = NONE;
    if (unit->unit.gunner_object_index == object_index)
        unit->unit.gunner_object_index = NONE;
    for (long slot = 0; slot < 4; slot++)
    {
        if (unit->unit.weapon_object_indices[slot] == object_index)
        {
            if (slot == unit->unit.current_weapon_set.weapon_indices[0])
            {
                current_weapon_deleted = true;
                unit->unit.current_weapon_set.weapon_indices[0] = NONE;
            }
            if (slot == unit->unit.current_weapon_set.weapon_indices[1])
                unit->unit.current_weapon_set.weapon_indices[1] = NONE;
            short inventory_index = (short)slot;
            // match: 0x110d00: volatile header reload makes the slot-clearing stwx take the slot offset first
            unit_datum *inventory_unit = (unit_datum *)((s_object_header *)*(char *volatile *)&object_header_data->data)
                [DATUM_INDEX_TO_ABSOLUTE_INDEX(unit_index)].datum;
            inventory_unit->unit.weapon_object_indices[inventory_index] = NONE;
            ::simulation_action_object_update(unit_index,
                (1 << (inventory_index + 22)) | (1 << (inventory_index + 18)));
            unit_inventory_cycle_weapon_set_identifier(unit_index);
        }
    }
    if (current_weapon_deleted && unit->unit.current_weapon_set.indices[0] == 0xff)
        unit->unit.desired_weapon_set.indices[0] =
            unit_weapon_next_index(unit_index, NONE, (signed char)unit->unit.current_weapon_set.indices[0], 0);
    if (unit->unit.equipment_object_index == object_index)
    {
        unit->unit.equipment_object_index = NONE;
        unit->unit.equipment_pickup_time = NONE;
    }
    if (unit->unit.predicted_vehicle_index == object_index)
    {
        unit->unit.predicted_vehicle_index = NONE;
        unit->unit.predicted_vehicle_seat_index = NONE;
    }
    unit_datum *dialogue_unit = unit_get(unit_index);
    s_unit_dialogue *dialogue =
        (s_unit_dialogue *)object_header_block_get(dialogue_unit, &dialogue_unit->unit.dialogue_reference);
    if (dialogue->current.type > 0)
    {
        if (dialogue->current.information.subject == object_index)
            dialogue->current.information.subject = NONE;
        if (dialogue->current.information.cause == 3 && dialogue->current.information.extra0 == object_index)
            dialogue->current.information.extra0 = NONE;
    }
    unit_datum *action_unit = unit_get(unit_index);
    mover_state *action = (mover_state *)object_header_block_get(action_unit, &action_unit->unit.action);
    if (action->object == object_index)
    {
        action->object = NONE;
        action->action = NONE;
    }
    if (action->task == object_index)
        action->task = NONE;
}

// 0xd84b0 natural source
// @address 0xd84b0 ?unit_inventory_drop_weapon@@YAXJFW4e_unit_drop_type@@_N@Z
void unit_inventory_drop_weapon(long unit_index, short inventory_index, e_unit_drop_type drop_type, bool p3)
{
    unit_datum *unit = unit_get(unit_index);
    long weapon_index = unit->unit.weapon_object_indices[inventory_index];
    if (weapon_index != NONE)
    {
        if (game_simulation::game_is_predicted())
        {
            drop_type = _unit_drop_type_delete;
        }
        else
        {
            bool useful = true;
            if (weapon_has_infinite_ammo(weapon_index))
                useful = false;
            if (!useful)
            {
                drop_type = _unit_drop_type_delete;
            }
            else if (game_globals->options.game_mode == 2 && !weapon_can_be_fired(weapon_index) && !weapon_can_be_dual_wielded(weapon_index))
            {
                drop_type = _unit_drop_type_delete;
            }
        }
        unit_inventory_set_weapon_index(unit_index, inventory_index, NONE, drop_type);
        for (long i = 0; i < 2; i++)
        {
            if (inventory_index == unit->unit.desired_weapon_indices[i])
            {
                if (p3)
                    unit->unit.desired_weapon_indices[i] = (signed char)unit_weapon_next_index(unit_index, -1,
                        unit->unit.current_weapon_indices[i], 0);
                else
                    unit->unit.desired_weapon_indices[i] = NONE;
            }
        }
    }
}

// 0x322390 natural source
// @address 0x322390 ?unit_notify_of_tracking_or_locking@@YAXJJF@Z
static __forceinline void units_player_notify_of_tracking_or_locking(long player_index, short flags)
{
    if (game_simulation::game_is_predicted())
        return;
    if ((flags & 1))
        return;
    player_datum *player = player_get(player_index);
    int ticks = (int)game_seconds_to_ticks_round(0.666f);
    if (((flags >> 2) & 1))
    {
        player->locking_warning_ticks = ticks;
        return;
    }
    if (!((flags >> 1) & 1))
        return;
    player->tracking_warning_ticks = ticks;
}
void unit_notify_of_tracking_or_locking(long unit_index, long object_index, short flags)
{
    unit_datum *unit = unit_get(unit_index);
    if (unit->unit.player_index != NONE)
    {
        units_player_notify_of_tracking_or_locking(unit->unit.player_index, flags);
    }
    else
    {
        long actor_index = unit->unit.actor_index;
        if (actor_index != NONE)
            actor_stimulus_tracking_or_locking(actor_index, object_index, flags);
    }
    long child_index = unit->object.first_child_object_index;
    while (child_index != NONE)
    {
        unit_datum *child = unit_get(child_index);
        if (((1 << child->object.object_type) & 0x1003) != 0)
            unit_notify_of_tracking_or_locking(child_index, object_index, flags);
        child_index = child->object.next_object_index;
    }
}

/* ---- 0x327440: is the unit unaware of a stimulus from the given direction? ---- */
// @address 0x79dad0 k_unit_assassination_angle
float k_unit_assassination_angle = 1.30899692f;

static __forceinline bool units_actor_is_unsuspecting(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = false;
    if (actor->behavior_mode == 1)
    {
        result = true;
    }
    else if (actor->danger_ticks >= 0)
    {
        if (actor->danger_ticks < game_seconds_to_ticks_round(0.2f))
        {
            result = true;
        }
    }
    return result;
}
// @address 0x327440 unit_can_be_assassinated
bool unit_can_be_assassinated(long unit_index, const real_point3d *point, const real_vector3d *direction)
{
    unit_datum *unit = (unit_datum *)object_try_and_get_and_verify_type(unit_index, 0x1003);
    bool result = false;
    if (unit != 0 && unit->object.object_type == 0)
    {
        unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
        if ((definition->flags_high & 1) == 0)
        {
            if (unit->unit.actor_index != NONE && units_actor_is_unsuspecting(unit->unit.actor_index))
            {
                result = true;
            }
            else
            {
                real_point3d body_position;
                unit_get_body_position(unit_index, &body_position);
                real_vector2d to_point;
                vector_from_points2d(&body_position.xy, &point->xy, &to_point);
                real_vector2d facing = unit->unit.facing;
                if (normalize2d(&to_point) != 0.0f)
                {
                    if (normalize2d(&facing) != 0.0f)
                    {
                        float dot = facing.i * to_point.i + to_point.j * facing.j;
                        float limit = (float)cos(k_unit_assassination_angle);
                        result = dot < -limit;
                    }
                }
            }
        }
    }
    return result;
}

// 0x327738 natural source
// @address 0x327738 ?unit_can_see_point@@YA_NJPBTreal_point3d@@M@Z
// @address 0x9a590 ?first_person_camera_deterministic@@YAXJPATreal_point3d@@PATreal_vector3d@@@Z
bool unit_can_see_point(long unit_index, const real_point3d *point, float angle)
{
    bool result = false;
    if (unit_index != NONE)
    {
        unit_datum *unit = unit_get(unit_index);
        real_point3d position;
        real_vector3d forward;
        real_vector3d direction;
        if (unit->unit.player_index != NONE)
            first_person_camera_deterministic(unit_index, &position, &forward);
        else
        {
            unit_get_head_position(unit_index, &position);
            forward = unit->unit.looking_vector;
        }
        // match: 0x327738: restrict on the target pointer preserves the direction schedule
        const real_point3d *__restrict target = point;
        vector_from_points3d(&position, target, &direction);
        normalize3d(&direction);
        float dot = dot_product3d(&forward, &direction);
        if (dot > (float)cos(angle))
            result = true;
    }
    return result;
}

// @address 0x32e350 unit_drop_unused_support_weapons
extern "C" void unit_drop_unused_support_weapons(long unit_index)
{
    if (!game_simulation::game_is_predicted())
    {
        unit_datum *unit = unit_get(unit_index);
        for (long i = 0; i < 4; i++)
        {
            long weapon_index = unit->unit.weapon_object_indices[i];
            if (weapon_index != NONE
                && unit->unit.current_weapon_indices[0] != NONE
                && i != unit->unit.current_weapon_indices[0]
                && i != unit->unit.current_weapon_indices[1]
                && i != unit->unit.desired_weapon_indices[0]
                && i != unit->unit.desired_weapon_indices[1]
                && ::weapon_is_support_weapon_or_must_be_readied(weapon_index))
                unit_inventory_drop_weapon(unit_index, (short)i, _unit_drop_type_drop, false);
        }
    }
}

// @address 0xd9170 ?unit_get_best_seat_entrance_animation@@YAJJJFPATreal_point3d@@0@Z
// match: 0xd9170: time-favoring optimization preserves the entrance-animation frame layout
#pragma optimize("t", on)
long unit_get_best_seat_entrance_animation(long actor_unit_index, long unit_index, short seat_index, real_point3d *start, real_point3d *entrance)
{
    float best_distance = 3.4028234e+38f;
    s_object_header *headers = (s_object_header *)object_header_data->data;
    long seat_offset = (long)seat_index * 228L;
    unit_definition *definition = (unit_definition *)tag_get(0,
        ((unit_datum *)headers[datum_index_to_absolute_index(unit_index)].datum)->object.definition_index);
    unit_seat *seats = definition->seats.begin();
    unit_seat *seat = (unit_seat *)((char *)seats + seat_offset);
    unit_datum *actor_unit = (unit_datum *)headers[datum_index_to_absolute_index(actor_unit_index)].datum;
    // match: 0xd9170: value range assumption guides branch lowering
    __assume(actor_unit_index != NONE);
    bool flag = !FLAG_TEST(seat->flags, 5);
    object_marker marker;
    long best = NONE;
    if (internal_object_get_markers_by_string_id(unit_index, seat->marker_name, &marker, 1, false) != 0)
    {
        if (start)
            *start = marker.matrix.position;
        for (long state_index = 0; state_index < 5; state_index++)
        {
            real_matrix4x3 seat_matrix;
            if (unit_animation_build_root_matrix_by_seat_and_state(actor_unit_index, seat->label,
                    g_unit_entering_state_names[state_index], flag, 1, &seat_matrix))
            {
                real_matrix4x3 result;
                matrix4x3_compute_product(&marker.matrix, &seat_matrix, &result);
                float distance = distance_squared3d(&actor_unit->object.bounding_center, &result.position);
                if (distance < best_distance || best == NONE)
                {
                    best = g_unit_entering_state_names[state_index];
                    best_distance = distance;
                    if (entrance)
                        *entrance = result.position;
                }
            }
        }
    }
    return best;
}



// @address 0x32b298 unit_validate_synchronized_melee_damage
extern "C" void unit_validate_synchronized_melee_damage(long attacker_unit_index, long attacker_effect_index, long attacker_clang_index,
    const s_damage_owner *damage_owner, const s_unit_player_melee_damage_target *melee_damage_target, bool *out_synchronized,
    bool *out_clang)
{
    int unit_data_index;
    s_object_header *header_entry;
    unit_datum *datum;
    s_player_melee_action_state *seat_base;
    unsigned char synchronized_valid;
    unsigned char clang_valid;
    int defender_effect_index[1];
    int defender_clang_index[1];
    int defender_clang_response[1];
    int defender_response_index[1];
    real_vector3d separation_direction;
    real_point3d attacker_center;
    real_point3d target_head_position;
    unit_data_index = melee_damage_target->object_index;
    synchronized_valid = 0;
    clang_valid = 0;
    if (unit_data_index != NONE)
    {
        header_entry = object_header_get(unit_data_index);
        if (((1 << header_entry->object_type) & 1) != 0)
        {
            if ((unsigned char)action_executing(unit_data_index, _action_melee_player) != 0)
            {
                datum = (unit_datum *)header_entry->datum;
                seat_base = (s_player_melee_action_state *)((char *)datum + datum->unit.action_offset);
                if (!((unsigned int)seat_base->silent != 0))
                {
                    if (datum->unit.player_index != NONE)
                    {
                        if (!(((biped_datum *)datum)->biped.physics.get_mode() != c_character_physics_component::_mode_melee))
                        {
                            if (((biped_datum *)datum)->biped.physics.get_mode_melee()->get_target_unit_index() == attacker_unit_index)
                                synchronized_valid = 1;
                        }
                        else
                        {
                            object_get_center_of_mass(attacker_unit_index, &attacker_center);
                            unit_get_head_position(unit_data_index, &target_head_position);
                            separation_direction.i = attacker_center.i - target_head_position.x;
                            separation_direction.j = attacker_center.j - target_head_position.y;
                            separation_direction.k = attacker_center.k - target_head_position.z;
                            normalize3d(&separation_direction);
                            if (dot_product3d(&datum->unit.aiming_vector, &separation_direction) > k_real_seven_tenths_storage)
                                synchronized_valid = 1;
                        }
                    }
                }
                if ((unsigned char)synchronized_valid != 0)
                {
                    if (attacker_clang_index != NONE)
                    {
                        unit_get_melee_effect_ids(unit_data_index, seat_base->animation, (long *)defender_effect_index,
                            (long *)defender_response_index, (long *)defender_clang_index, (long *)defender_clang_response);
                        if (*(int *)defender_effect_index == attacker_effect_index
                            && *(int *)defender_clang_index == attacker_clang_index)
                            clang_valid = 1;
                    }
                }
            }
        }
    }
    *(unsigned char *)out_synchronized = synchronized_valid;
    *(unsigned char *)out_clang = clang_valid;
}

// @address 0x327fa0 ?unit_add_grenade_to_inventory@@YA_NJJ@Z
static __forceinline void units_player_notify_grenade_pickup(long player_index)
{
    long user_index = player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player_index)];
    if (user_index != NONE)
        ((s_player_training_entry_data *)player_training_globals)[user_index].grenade_pickup_ticks = (signed char)game_time_globals->ticks_per_second;
}
bool unit_add_grenade_to_inventory(long unit_index, long equipment_index)
{
    unit_datum *equipment = unit_get(equipment_index);
    long grenade_index = game_get_grenade_type_index_from_item_defintion(equipment->object.definition_index);
    unit_datum *unit = unit_get(unit_index);
    bool result = false;
    if (grenade_index != NONE)
    {
        s_grenade_definition *definition = ((s_game_globals *)global_game_globals)->grenades.count()
            ? &((s_game_globals *)global_game_globals)->grenades[grenade_index]
            : 0;
        if (unit->unit.grenade_counts[grenade_index] < definition->maximum_count)
        {
            if (unit->unit.player_index != NONE)
                units_player_notify_grenade_pickup(unit->unit.player_index);
            if (!game_simulation::game_is_predicted())
            {
                unit->unit.grenade_counts[grenade_index]++;
                ::simulation_action_object_update(unit_index, 0x4000000);
                simulation_action_pickup_equipment(unit_index, equipment->object.definition_index);
                if (unit->unit.player_index != NONE)
                    projectile_play_pickup_sound(unit->unit.player_index, unit_get(equipment_index)->object.definition_index);
                object_delete(equipment_index);
                result = true;
            }
        }
    }
    return result;
}

// @address 0x35be58 unit_cause_random_idle_action
extern "C" void unit_cause_random_idle_action(long unit_index)
{
    if (unit_index == NONE)
        return;
    if ((unsigned char)action_executing(unit_index, _action_custom_animation))
        return;
    if ((unsigned char)unit_is_player_controlled(unit_index))
        return;
    unit_datum *unit = unit_get(unit_index);
    if ((_rotl(unit->object.damage_flags, 30) & 1) != 0)
        return;
    if (unit->object.parent_object_index != NONE)
        return;
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    action_request request;
    memset(&request, 0, sizeof(request));
    request.type = _action_ping;
    unsigned long random = random_seed_next(TLS_RANDOM_SEED);
    unsigned long variant_base = random >> 16;
    unsigned short variant = (unsigned short)((variant_base * 10) >> 16);
    request.ping.region = (signed char)variant;
    real_vector3d *direction = &request.ping.direction;
    *direction = unit->object.forward;
    request.ping.has_direction = 1;
    float one = 1.0f;
    if ((_rotl(definition->flags, 23) & 1) == 0) {
        unsigned long sign_random = random_seed_next(TLS_RANDOM_SEED);
        float sign;
        if ((sign_random & 0x80000000) != 0)
            sign = one;
        else
            sign = k_real_constants_minus_one[0];
        direction->i = sign * unit->object.forward.i;
        direction->j = sign * unit->object.forward.j;
        direction->k = sign * unit->object.forward.k;
    } else {
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x35be58: the direction table (0x820361a8) is addressed at +0x3870 off the k_real_constants_minus_one
        // base register held for the -1.0 sign; naming the table loads its own base and swaps r26/r27
        const real_vector3d *table = (const real_vector3d *)&k_real_constants_minus_one[3612];
#else
        const real_vector3d *table = k_random_direction_table;
#endif
        unsigned long index_random = random_seed_next(TLS_RANDOM_SEED);
        short index = (short)(((index_random >> 16) * 1026) >> 16);
        *direction = table[index];
    }
    request.ping.type = 2;
    if (!action_submit(unit_index, &request))
        return;
    short type_offset = unit->object.animation.offset;
    long animation_offset = unit->unit.animation_storage.offset;
    c_mixing_board_definition *board = 0;
    char *manager = (char *)unit + type_offset;
    unsigned long manager_type = ((c_animation_manager *)manager)->m_mixing_board_type;
    if (manager_type <= 2)
        board = &((c_mixing_board_definition *)g_mixing_board_definition_list)[manager_type];
    c_animation_channel *channel = (c_animation_channel *)(((g_mixing_board_component_sizes[1] + 88) << 1) + board->component_types[1].offset + manager);
    unsigned char valid = (unsigned char)(channel->m_enabled != 0 && channel->m_graph_index != NONE
        && (unsigned short)channel->m_animation_id.m_animation_index < 0x8000);
    if (valid == 0)
        return;
    if ((unsigned char)((const c_animation_channel *)channel)->playing() == 0)
        return;
    unsigned char flag = (unsigned char)((*(unsigned short *)((char *)unit + animation_offset) >> 9) & 1);
    if (flag == 0)
        return;
    unsigned long rate_random = random_seed_next(TLS_RANDOM_SEED);
    float value = ((float)(rate_random >> 16) * 1.5259022e-05f + one) * 0.5f;
    if (!(value > 0.0001f))
        value = 0.0001f;
    channel->m_playback_speed = value;
}

// 0xc17e8 natural source



// @address 0xc17e8 ?unit_clip_vector_to_aiming_screen_bounds@@YA_NJPATreal_vector3d@@_N@Z
bool unit_clip_vector_to_aiming_screen_bounds(long unit_index, real_vector3d *vector, bool secondary)
{
    bool clipped = false;
    unit_datum *unit = unit_get(unit_index);
    s_unit_animation *animation = (s_unit_animation *)((char *)unit + unit->unit.animation_storage.offset);
    const unit_aiming_screen_bounds *bounds;
    bool enabled;
    if (secondary)
    {
        bounds = &animation->secondary_aiming_screen_bounds;
        enabled = animation->flags.test(_unit_animation_secondary_aiming_screen_bounds_bit);
    }
    else
    {
        bounds = &animation->primary_aiming_screen_bounds;
        enabled = animation->flags.test(_unit_animation_primary_aiming_screen_bounds_bit);
    }
    if (enabled)
    {
        real_vector3d relative_forward;
        real_vector3d relative_up;
        ::object_get_orientation(unit_index, &relative_forward, &relative_up);
        if (secondary)
        {
            real_vector3d relative_left;
            relative_forward = unit->unit.aiming_vector;
            normalize3d(&relative_forward);
            cross_product3d(&relative_up, &relative_forward, &relative_left);
            normalize3d(&relative_left);
            cross_product3d(&relative_forward, &relative_left, &relative_up);
            if (normalize3d(&relative_up) == 0.0f)
            {
                perpendicular3d(&relative_forward, &relative_up);
                normalize3d(&relative_up);
            }
        }
        real_matrix4x3 matrix;
        matrix4x3_from_point_and_vectors(&matrix, &k_real_math_origin3d, &relative_forward, &relative_up);
        normalize3d(&matrix.left);
        real_vector3d relative_vector;
        matrix4x3_inverse_transform_normal(&matrix, vector, &relative_vector);
        real_euler_angles2d relative_aiming_angles;
        euler_angles2d_from_vector3d(&relative_aiming_angles, &relative_vector);
        if (relative_aiming_angles.yaw < bounds->yaw.lower)
        {
            relative_aiming_angles.yaw = bounds->yaw.lower;
            clipped = true;
        }
        else if (relative_aiming_angles.yaw > bounds->yaw.upper)
        {
            relative_aiming_angles.yaw = bounds->yaw.upper;
            clipped = true;
        }
        if (relative_aiming_angles.pitch < bounds->pitch.lower)
        {
            relative_aiming_angles.pitch = bounds->pitch.lower;
            clipped = true;
        }
        else if (relative_aiming_angles.pitch > bounds->pitch.upper)
        {
            relative_aiming_angles.pitch = bounds->pitch.upper;
            clipped = true;
        }
        if (clipped)
        {
            real_vector3d relative_vector;
            vector3d_from_euler_angles2d(&relative_vector, &relative_aiming_angles);
            matrix4x3_transform_normal(&matrix, &relative_vector, vector);
        }
    }
    return clipped;
}

// @address 0x3278a0 ?unit_detach_from_parent@@YAXJ@Z
void unit_detach_from_parent(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    if (unit->object.parent_object_index != -1) {
        if (unit->unit.parent_seat_index != -1) {
            c_motor_request request;
            request.setup_exit(_action_none, false, false, false, true);
            request.set_force(true);
            e_motor_result motor_result = motor_system_submit(unit_index, &request);
            // March units.cpp:6864 asserts motor_result == _motor_immediate.
        } else {
            real_point3d parent_origin, unit_origin;
            object_get_origin(unit->object.parent_object_index, &parent_origin);
            object_get_origin(unit_index, &unit_origin);
            real_vector3d direction = {unit_origin.x - parent_origin.x, unit_origin.y - parent_origin.y, unit_origin.z - parent_origin.z};
            if (normalize3d(&direction) == 0.0f) direction = unit->object.forward;
            real_vector3d separation = {direction.i * 0.6f, direction.j * 0.6f, direction.k * 0.6f};
            object_detach_immediate(unit_index);
            real_point3d position = unit->object.position;
            s_location location;
            for (short i = 0; i < 150; ++i) {
                scenario_location_from_point(&location, &position);
                if (location.cluster_reference.bsp_index != -1) break;
                position.z += 0.05f;
            }
            const real_vector3d *old_velocity = &unit->object.linear_velocity;
            real_vector3d velocity = {old_velocity->i + separation.i, old_velocity->j + separation.j, old_velocity->k + separation.k};
            object_set_position_direct(unit_index, &position, 0, 0, 0, false);
            object_set_velocities(unit_index, &velocity, global_zero_vector3d);
            object_set_hidden(unit_index, false);
        }
    }
}


// @address 0x32ead0 unit_detach_and_reset_motor_program
extern "C" void unit_detach_and_reset_motor_program(long unit_index)
{
    if (unit_index != -1)
    {
        unit_datum *unit = unit_get(unit_index);
        if (unit->object.parent_object_index != -1)
        {
            motor_task_interrupt(unit_index, 15, 0);
            if (unit->unit.parent_seat_index != -1)
                motor_animation_exit_seat_reset_velocity(unit_index, 0, 0, false, 0, 0);
            if (unit->object.parent_object_index != -1)
                object_detach(unit_index);

            s_object_header *header = object_header_get(unit_index);
            if (((1 << header->object_type) & 1) != 0)
            {
                unit_datum *biped = (unit_datum *)header->datum;
                c_motor_program_state *motor = (c_motor_program_state *)((char *)biped + biped->unit.motor_offset);
                if (motor->program != 1)
                    motor->set_program(unit_index, _motor_program_airborne);
            }
        }
    }
}



static __forceinline void units_object_flags_set(unsigned long *flags, long bit, bool value)
{
    if (value)
        *flags |= 1UL << bit;
    else
        *flags &= ~(1UL << bit);
}


// @address 0xa1fd8 ?unit_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool unit_new(long unit_index, object_placement_data *placement, bool *out_of_memory)
{
    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    bool success = true;
    unit->unit.equipment_object_index = NONE;
    unit->unit.equipment_pickup_time = NONE;
    memset(unit->unit.weapon_object_indices, NONE, sizeof(unit->unit.weapon_object_indices));
    unit->unit.desired_weapon_set_identifier = 0;
    unit->unit.weapon_set_identifier = 0;
    unit->unit.desired_weapon_index = -1;
    unit->unit.current_weapon_index = -1;
    unit->unit.desired_weapon_indices[1] = -1;
    unit->unit.desired_weapon_indices[0] = -1;
    unit->unit.current_grenade_index = -1;
    unit->unit.desired_grenade_index = -1;
    unit->unit.zoom_level = -1;
    unit->unit.desired_zoom_level = -1;
    unit->unit.player_index = NONE;
    unit->unit.controller_player_index = NONE;
    unit->unit.actor_index = NONE;
    unit->unit.simulation_actor_datum_index = NONE;
    unit->unit.parent_seat_index = -1;
    unit->unit.driver_object_index = NONE;
    unit->unit.gunner_object_index = NONE;
    unit->unit.simulation_entity_index = NONE;
    unit->unit.simulation_actor_index = -1;
    unit->unit.simulation_player_index = NONE;
    unit->unit.simulation_player_identifier = NONE;
    unit->unit.simulation_actor_identifier = NONE;
    unit->unit.simulation_control_index = NONE;
    unit->unit.simulation_control_identifier = NONE;
    unit->unit.unit_input_flags = 0;
    unit->unit.persistent_control_ticks = 0;
    unit->unit.damage_type = 0;
    unit->unit.damage_timer = 0;
    unit->unit.damage = 0.0f;
    unit->unit.damaging_object_index = NONE;
    unit->unit.time_of_death = NONE;
    unit->unit.last_vehicle_index = NONE;
    unit->unit.last_vehicle_exit_time = NONE;
    unit->unit.seat_exit_queued = 0;
    unit->unit.last_squad_index = -1;
    unit->unit.flaming_death_attacker_object_index = NONE;
    unit->unit.killing_spree_count = 0;
    unit->unit.killing_spree_time = NONE;
    unit->unit.left_eye_marker = -1;
    unit->unit.right_eye_marker = -1;
    unit->unit.boost_time = 0.0f;
    unit->unit.boost_decay_time = 0.0f;
    unit->unit.last_enemies_moving_nearby_tick = NONE;
    unit->unit.last_damage_time = NONE;
    unit->unit.soft_ceiling_contact_time = NONE;
    unit->unit.last_in_phantom_volume_time = NONE;
    unit->unit.active_camouflage_target = 1.0f;
    unit->unit.seat_acceleration_reset_time = NONE;
    aim_assist_targeting_clear(&unit->unit.targeting);
    for (unsigned long i = 0; i < 4; ++i)
    {
        unit->unit.damage_records[i].time = NONE;
        unit->unit.damage_records[i].damage = 0.0f;
        unit->unit.damage_records[i].object_index = NONE;
        unit->unit.damage_records[i].player_index = NONE;
        unit->unit.damage_record_flags[i] = 0;
    }
    success = success && object_has_animation_manager(unit_index);
    if (success)
    {
        success = success && ::object_header_block_allocate(unit_index, 0x3f8, 68, 2);
        success = success && ::object_header_block_allocate(unit_index, 0x3fc, 92, 2);
        if (success)
        {
            unit = unit_get(unit_index);
            s_units_new_animation_state *animation = (s_units_new_animation_state *)((char *)unit + unit->unit.animation_storage.offset);
            animation->last_animation_index = NONE;
            unit_animation_reset(unit_index);
            s_units_new_dialogue_state *dialogue = (s_units_new_dialogue_state *)((char *)unit + unit->unit.dialogue_offset);
            dialogue->tag = NONE;
            dialogue->last_sound_index = NONE;
            unit_dialogue_set_variant(unit_index, 0);
            unit->unit.looking_vector = unit->object.forward;
            unit->unit.desired_looking_vector = unit->unit.looking_vector;
            unit->unit.aiming_vector = unit->object.forward;
            unit->unit.desired_aiming_vector = unit->unit.aiming_vector;
            unit->unit.facing_vector = unit->object.forward;
            unit->unit.animation_state = 0x8a;
            if (definition->grenade_type >= 0 && definition->grenade_type < 4 && definition->grenade_count >= 0)
                unit->unit.grenade_counts[definition->grenade_type] = (signed char)definition->grenade_count;
            units_object_flags_set(&unit->object.object_flags, 12, true);
            unit->unit.team = (e_game_team)placement->owner_team;
            if (game_is_campaign() && (unit->unit.team == 0 || unit->unit.team == NONE))
                unit->unit.team = (e_game_team)definition->default_team;
            motor_animation_set_mode_weapons_and_state(unit_index, 1, 0xe2, 0xe2, 1, 0.0f, 2, 0x1d, false);
            unit->unit.left_eye_marker = (signed char)model_find_node(definition->model.index, 0x101);
            unit->unit.right_eye_marker = (signed char)model_find_node(definition->model.index, 0x102);
            unit->unit.flags400 = 0;
            unit->unit.seat_acceleration_mask = 0;
            if (definition->seat_accelerations.count > 0)
                unit->unit.seat_acceleration_mask = (signed char)((1 << definition->seat_accelerations.count) - 1);
            if (placement->parent_object_index != NONE)
            {
                long marker = placement->parent_marker;
                if (marker != NONE && marker != 0
                    && ((1 << object_header_get(placement->parent_object_index)->object_type) & 0x1003) != 0)
                    unit_placement_try_to_put_unit_in_seat_using_marker_name(unit_index, placement->parent_object_index, marker);
            }
        }
        else
            *out_of_memory = true;
    }
    return success;
}
// match: noalias declaration preserves caller live registers

static const float k_melee_impulse_scale = 1.6f;
static const float k_uber_dash_minimum_speed = 4.0f;


static __forceinline bool units_object_damage_flags_test(const unit_datum *object, long bit)
{
    return ((object->object.damage_flags >> bit) & 1) != 0;
}

static __forceinline void machine_try_to_open_with_damage(long machine_index)
{
    s_machine_datum *machine = (s_machine_datum *)unit_get(machine_index);
    if (((machine->machine_flags.m_storage >> _machine_flag_5) & 1))
        device_set_actual_position(machine_index, 1.0f);
}





static __forceinline void units_set_death_node_acceleration(unit_datum *biped, long node_index, const real_vector3d *acceleration)
{
    c_character_physics_mode_dead_datum *dead = ((biped_datum *)biped)->biped.physics.get_mode_dead();
    dead->m_death_node_index = node_index;
    dead->m_death_time = game_time_get();
    dead->m_death_node_acceleration = *acceleration;
}

/* ---- 0x325f28 unit_damage_aftermath_apply support ---- */
struct effect_vector_field;
// @address 0x3d53c k_units_datamine_multiplayer
// @rdata 0x8203d53c..0x8203d548
const char k_units_datamine_multiplayer[] = "multiplayer";
// @rdata-end

static __forceinline bool unit_secondary_skull_test(long flavor)
{
    unsigned long skulls = game_globals->options.game_mode == 1 ? game_globals->options.secondary_skulls : 0;
    return ((skulls >> flavor) & 1) != 0;
}
static __forceinline void units_object_get_damage_owner(long object_index, s_damage_owner *owner)
{
    *owner = unit_get(object_index)->object.damage_owner;
}

// @address 0x325f28 ?unit_damage_aftermath_apply@@YAXJPBUs_damage_aftermath_result_data@@@Z
void unit_damage_aftermath_apply(long unit_index, const s_damage_aftermath_result_data *result)
{
    unit_datum *unit = unit_get(unit_index);
    s_damage_effect_definition *definition = (s_damage_effect_definition *)tag_get(0, result->damage_definition_index);

    bool killed = (result->flags & 1) != 0;
    float total_damage = result->shield_damage + result->body_damage;
    if (result->response_metadata != 1 || !unit_secondary_skull_test(3)) {
    }
    else {
        long actor_index = unit->unit.actor_index;
        if (actor_index != NONE && datum_get<actor_datum>(actor_data, actor_index)->type == 2) {
            s_game_globals *globals = (s_game_globals *)global_game_globals;
            s_grenade_definition *grenade;
            if (globals->grenades.count() != 0)
                grenade = &globals->grenades[1];
            else
                grenade = 0;
            s_item_definition *item = (s_item_definition *)tag_get(0, grenade->equipment.index);
            s_damage_owner owner;
            units_object_get_damage_owner(unit_index, &owner);
            if (item->detonation_damage_effect.index != NONE) {
                s_damage_data damage_data;
                real_point3d origin;
                object_get_origin(unit_index, &origin);
                damage_data_new(&damage_data, item->detonation_damage_effect.index);
                damage_data.scale[0] = 1.0f;
                damage_data.owner = owner;
                damage_data.reporting_info = 16;
                object_get_location(unit_index, &damage_data.location);
                damage_data.position = origin;
                damage_data.origin = origin;
                area_of_effect_cause_damage(&damage_data, NONE, NONE);
            }
            if (chud_globals != NULL && chud_globals->death_effect.index != NONE)
                effect_new_from_object(chud_globals->death_effect.index, &owner, 0, unit_index, 0.0f, 0.0f, NULL, NULL, 1);
            else
                effect_new_from_object(item->detonating_effect.index, &owner, 0, unit_index, 0.0f, 0.0f, NULL, NULL, 1);
        }
    }
    long flags = (result->flags >> 3) & 1;
    long player_index = unit_get(unit_index)->unit.player_index;
    long user_index = NONE;
    while (true) {
        if (player_index != NONE)
            user_index = (long)director_get_perspective(player_index, user_index);
        if (user_index == NONE)
            break;
        real_vector3d direction = result->direction;
        if (!((result->flags >> 2) & 1)) {
            long owner_object = result->damage_owner_object_index;
            if (owner_object != NONE && owner_object != unit_index) {
                real_point3d gun_position;
                void *object = object_try_and_get_and_verify_type(owner_object, NONE);
                if (object != NULL && (((1 << ((unit_datum *)object)->object.object_type) & 0x1003) != 0)
                    && unit_get_gun_position(owner_object, true, &gun_position)) {
                    vector_from_points3d(&gun_position, &observer_get_globals()->blocks[user_index].camera_result.position, &direction);
                    normalize3d(&direction);
                }
            }
        }
        player_effect_start(unit_index, user_index, result->damage_definition_index, (const s_damage_owner *)&result->owner_player_index,
            &direction, result->scale, result->shake_scale, flags, false);
    }
    float health = unit->object.body_damage + unit->object.shield_damage;
    if (health > 0.0f) {
        unit->unit.damage_type = definition->damage.category;
        long timer_ticks = (long)((float)game_time_globals->ticks_per_second * 1.5f + 0.5f);
        unit->unit.damage_timer = (short)timer_ticks;
        if (unit->unit.damage > health)
            health = unit->unit.damage;
        unit->unit.damage = health;
        if (result->damage_owner_object_index != NONE)
            unit->unit.damaging_object_index = result->damage_owner_object_index;
    }
    if (result->body_damage > 0.0f || result->shield_damage > 0.0f)
        unit_active_camouflage_ding(unit_index, definition->damage.active_camouflage_damage, 0.0f);
    bool submitted = false;
    if (result->ping_type > 0) {
        action_request request;
        memset(&request, 0, sizeof(request));
        request.type = 7;
        request.ping.region = (signed char)result->body_part;
        request.ping.direction = result->direction;
        request.ping.has_direction = 1;
        request.ping.type = (signed char)result->ping_type;
        submitted = action_submit(unit_index, &request);
    }
    if (!submitted && killed)
        motor_task_interrupt(unit_index, 15, 0);
    long owner_player = result->owner_player_index;
    long damage_owner_object = result->damage_owner_object_index;
    long unit_player = unit->unit.player_index;
    bool has_player = unit_player != NONE;
    bool valid_owner = owner_player != NONE && unit_player != NONE;
    if (valid_owner && result->shield_damage + result->body_damage > 0.0001f)
        game_engine_player_damaged_player(owner_player, damage_owner_object, unit_player, (result->flags >> 4) & 1);
    if (game_globals->options.game_mode == 1 && has_player && game_campaign_is_multiplayer()) {
        c_player_in_game_iterator iterator;
        iterator.begin();
        long waypoint_index = 0;
        while (iterator.next()) {
            if (iterator.get_index() == unit_player)
                game_engine_fade_set_state((s_game_engine_fade_entry *)&chud_navpoint_globals()->waypoints[waypoint_index], 3);
            waypoint_index++;
        }
    }
    if (result->owner_player_index != NONE || result->damage_owner_object_index != NONE)
        unit_record_damage(unit_index, total_damage, definition->damage.category, killed, result->owner_player_index,
            (e_game_team)result->owner_team, result->damage_owner_object_index, result->reporting_info);
    if (!((result->flags >> 5) & 1) && ((result->flags & 1) || result->body_damage > 0.0f || result->shield_damage > 0.0f))
        unit_make_damage_sound(unit_index, result->damage_definition_index, killed, ((result->flags >> 6) & 1) != 0,
            unit->object.current_body_damage, unit->object.current_shield_damage);
    if (result->body_damage > 0.0f || result->shield_damage > 0.0f)
        unit_unzoom(unit_index, false);
    long owner_object = result->damage_owner_object_index;
    void *try_object = object_try_and_get_and_verify_type(owner_object, 0x1003);
    if (unit->unit.actor_index != NONE || unit->unit.player_index != NONE
        || (try_object != NULL && ((unit_datum *)try_object)->unit.actor_index != NONE)) {
        short material = definition->damage.category;
        switch ((result->reporting_info >> 2) - _damage_reporting_type_banshee) {
        case _damage_reporting_type_banshee - _damage_reporting_type_banshee:
        case _damage_reporting_type_ghost - _damage_reporting_type_banshee:
        case _damage_reporting_type_mongoose - _damage_reporting_type_banshee:
        case _damage_reporting_type_scorpion - _damage_reporting_type_banshee:
        case _damage_reporting_type_spectre_driver - _damage_reporting_type_banshee:
        case _damage_reporting_type_warthog_driver - _damage_reporting_type_banshee:
        case _damage_reporting_type_wraith - _damage_reporting_type_banshee:
        case _damage_reporting_type_wraith_antiinfantry - _damage_reporting_type_banshee:
        case _damage_reporting_type_tank - _damage_reporting_type_banshee:
            if ((result->reporting_info & 3) == 3)
                material = 9;
            break;
        default:
            break;
        }
        if (killed) {
            if (ai_globals->enabled)
                ((void (*)(long, long, short))ai_handle_death)(unit_index, owner_object, material);
        }
        else if (!(bool)(_rotl(unit->object.damage_flags, 30) & 1)) {
            ai_handle_damage(unit_index, owner_object, material, result->flags, total_damage, &result->direction, false);
        }
    }
    bool simulated = false;
    if (game_globals->options.game_simulation == 4)
        simulated = true;
    if (!simulated && unit->unit.actor_index != NONE && ((result->flags >> 2) & 1) && result->distance < definition->ai_stun_radius) {
        float radius = definition->ai_stun_radius;
        float stun = (radius - result->distance) / radius
            * (definition->ai_stun_bounds.upper - definition->ai_stun_bounds.lower) + definition->ai_stun_bounds.lower;
        ai_handle_stun(unit_index, stun, &result->direction);
    }
    if (unit->unit.player_index != NONE) {
        float response = definition->damage.stun;
        if (response > 0.0f && game_globals->options.game_mode == 2) {
            s_player_information_definition *info = (s_player_information_definition *)tag_block_get_element_with_size(
                &((s_game_globals *)global_game_globals)->player_information, 0, sizeof(s_player_information_definition));
            float scale = result->scale;
            float stun_increment = scale * response;
            float maximum_stun = definition->damage.maximum_stun * scale;
            if (stun_increment < 0.0f)
                stun_increment = 0.0f;
            if (maximum_stun < 0.0f)
                maximum_stun = 0.0f;
            if (maximum_stun >= 1.0f)
                maximum_stun = 1.0f;
            if (unit->unit.look_difficulty_scale < maximum_stun) {
                unit->unit.look_difficulty_scale += stun_increment;
                if (unit->unit.look_difficulty_scale > maximum_stun)
                    unit->unit.look_difficulty_scale = maximum_stun;
            }
            short a = (short)game_seconds_to_ticks_round(definition->damage.stun_time);
            short b = (short)game_seconds_to_ticks_round(info->stun_penalty_minimum_time);
            short c = (short)game_seconds_to_ticks_round(info->stun_penalty_maximum_time);
            if (unit->unit.damage_stun_ticks < b)
                unit->unit.damage_stun_ticks = b;
            unit->unit.damage_stun_ticks += a;
            if (unit->unit.damage_stun_ticks > c)
                unit->unit.damage_stun_ticks = c;
        }
    }
    if (((result->flags >> 10) & 1)) {
        bool shielded = true;
        if (result->shield_damage == 0.0f)
            shielded = false;
        unit_apply_emp_stun(unit_index, shielded);
    }
    if (killed) {
        if (game_globals->options.game_mode == 2) {
            bool authoritative = false;
            if (game_globals->options.game_simulation == 4)
                authoritative = true;
            if (!authoritative) {
                long player = unit->unit.player_index;
                if (player != NONE) {
                    player_datum *p = (player_datum *)datum_try_and_get(player_data, player);
                    if (p != NULL && p->unit_index != NONE) {
                        c_datamine mine(0, "death", 2, "units", k_units_datamine_multiplayer);
                        data_mine_add_player_death_with_killer_data(&mine, p->unit_index, result);
                    }
                }
            }
        }
        if (game_globals->options.game_mode == 1) {
            if (unit->unit.actor_index != NONE)
                data_mine_insert_sp_actor_death(unit_index, result);
            if (unit->unit.player_index != NONE && unit_index != NONE) {
                c_datamine mine(0, "death", 2, "units", "campaign");
                data_mine_add_player_death_with_killer_data(&mine, unit_index, result);
            }
        }
        unit_died(unit_index, (short)result->node_index, &result->direction, false, (result->flags >> 9) & 1);
    }
}

// @address 0x32e838 ?unit_perform_detach_and_damage@@YAXJJ@Z
void unit_perform_detach_and_damage(long unit_index, long object_index)
{
    unit_definition *definition = (unit_definition *)tag_get(0, unit_get(object_index)->object.definition_index);
    if (definition->detached_weapon.index != NONE)
    {
        object_placement_data placement;
        object_placement_data_new(&placement, definition->detached_weapon.index, unit_index, 0);
        long weapon_index = object_new(&placement);
        if (weapon_index != NONE)
            unit_add_weapon_to_inventory(unit_index, weapon_index, _weapon_addition_method_silent);
    }
    if (definition->detachment_damage.index != NONE)
    {
        s_damage_data damage_data;
        damage_data_new(&damage_data, definition->detachment_damage.index);
        damage_compute_damage_owner_from_object_index(unit_index, &damage_data.owner);
        object_cause_damage_simple(&damage_data, object_index);
    }
}

// @address 0x32c7b0 ?unit_cause_player_melee_damage@@YAXJJPBUs_damage_owner@@PBUs_unit_player_melee_damage_target@@@Z
void unit_cause_player_melee_damage(
    long unit_index, long damage_effect_index, const s_damage_owner *damage_owner, const s_unit_player_melee_damage_target *damage_target)
{
    unit_datum *unit = unit_get(unit_index);
    s_damage_data damage_data;
    damage_data_new(&damage_data, damage_effect_index);
    damage_data.reporting_info = damage_target->hit_material_flags;
    damage_data.object_index = unit_index;
    if (damage_target->lunge)
        damage_data.flags |= 0x8000;
    else
        damage_data.flags &= ~0x8000;
    damage_data.flags |= 0x30000;
    damage_data.flags |= 1;
    damage_data.location = unit->object.location;
    damage_data.owner = *damage_owner;
    damage_data.origin = damage_target->position;
    damage_data.position = unit->object.bounding_center;
    damage_data.direction = unit->unit.aiming_vector;
    damage_data.initial_direction = unit->unit.aiming_vector;
    damage_data.scale[0] = damage_target->damage_scale;
    damage_data.material_type = damage_target->material_type;
    damage_data.model_region_index = damage_target->model_region_index;
    if (damage_target->object_index != NONE)
    {
        unit_datum *target = unit_get(damage_target->object_index);
        bool was_dead = units_object_damage_flags_test(target, 2);
        if (target->object.object_type == 7)
            machine_try_to_open_with_damage(damage_target->object_index);
        object_cause_damage_simple(&damage_data, damage_target->object_index);
        if (!was_dead && !damage_target->lunge && units_object_damage_flags_test(target, 2) &&
            ((1 << object_header_get(damage_target->object_index)->object_type) & 1))
        {
            target = unit_get(damage_target->object_index);
            bool uber_dash = false;
            if (((1 << object_header_get(unit_index)->object_type) & 1))
            {
                unit_datum *attacker = unit_get(unit_index);
                uber_dash = ((biped_datum *)attacker)->biped.physics.m_mode == 6
                    && (*(unsigned long *)((biped_datum *)attacker)->biped.physics.m_storage & 1);
            }
            if (((biped_datum *)target)->biped.physics.m_mode == 3)
            {
                real_vector3d velocity;
                object_get_velocities(unit_index, &velocity, 0);
                real_vector3d direction = velocity;
                real_vector3d impulse;
                if (dot_product3d(&velocity, &unit->unit.aiming_vector) > 0.0001f && normalize3d(&direction) != 0.0f)
                {
                    impulse = velocity;
                    point_from_line3d(&impulse, &direction, k_melee_impulse_scale, &impulse);
                }
                else
                {
                    scale_vector3d(&unit->unit.aiming_vector, k_melee_impulse_scale, &impulse);
                }
                // match: 0x32c7b0: measuring the copy starts the squared length with j*j at 0x32cac0 and adds i*i last.
                const real_vector3d unnormalized_impulse = impulse;
                if (uber_dash && magnitude_squared3d(&unnormalized_impulse) < k_uber_dash_minimum_speed * k_uber_dash_minimum_speed)
                {
                    normalize3d(&impulse);
                    scale_vector3d(&impulse, k_uber_dash_minimum_speed, &impulse);
                }
                target->unit.death_state = 2;
                units_set_death_node_acceleration(target, 0, &impulse);
            }
        }
    }
    else if (damage_target->breakable_surface_index != NONE)
    {
        breakable_surface_damage(damage_target->breakable_surface_set_index, damage_target->breakable_surface_bsp_index,
            damage_target->breakable_surface_index, damage_target->breakable_surface_instance_index, &damage_data,
            damage_target->breakable_surface_cluster_index);
    }
}

// @address 0x32b440 unit_handle_synchronized_melee_damage
extern "C" void unit_handle_synchronized_melee_damage(long unit_index, long damage_effect_index, long clang_effect_index,
    const s_damage_owner *damage_owner, const s_unit_player_melee_damage_target *damage_target, bool synchronized, bool clang)
{
    s_object_header *headers = (s_object_header *)object_header_data->data;
    unit_datum *unit = (unit_datum *)headers[datum_index_to_absolute_index(unit_index)].datum;
    if (synchronized && !game_simulation::game_is_predicted())
    {
        s_unit_player_melee_damage_target target = *damage_target;
        long target_index = target.object_index;
        bool damage_resolved = false;
        unit_datum *target_unit = unit_get(target_index);
        s_player_melee_action_state *action = (s_player_melee_action_state *)((char *)target_unit + target_unit->unit.action_offset);
        action->silent = 1;
        action->last_melee_time = game_time_get();
        long melee_type = action->animation;
        unit_set_player_melee_remaining(target_index, units_max(0, action->total_ticks - action->ticks));
        long target_damage_effect_index;
        long target_unknown;
        long target_clang_effect_index;
        long target_clang_unknown;
        unit_get_melee_effect_ids(target_index, melee_type, &target_damage_effect_index, &target_unknown, &target_clang_effect_index,
            &target_clang_unknown);
        s_damage_owner target_damage_owner;
        damage_compute_damage_owner_from_object_index(target_index, &target_damage_owner);
        long effect_index = clang ? clang_effect_index : damage_effect_index;
        long target_effect_index = clang ? target_clang_effect_index : target_damage_effect_index;
        if (effect_index != NONE)
        {
            target.lunge = true;
            unit_cause_player_melee_damage(unit_index, effect_index, damage_owner, &target);
        }
        if (target_effect_index != NONE)
        {
            target.lunge = true;
            target.object_index = unit_index;
            unit_cause_player_melee_damage(target_index, target_effect_index, &target_damage_owner, &target);
        }
        if (target_unit->object.body_vitality <= 0.0f || unit->object.body_vitality <= 0.0f)
        {
            bool unit_survives = false;
            bool target_attacks = false;
            bool target_survives = false;
            bool unit_attacks = false;
            if (target_unit->object.body_vitality > 0.0f || unit->object.body_vitality > 0.0f)
            {
                if (target_unit->object.body_vitality <= 0.0f)
                    unit_attacks = true;
                else
                    target_attacks = true;
            }
            else if (clang)
            {
                unit_survives = true;
                target_survives = true;
            }
            else
            {
                bool unit_wins = unit->object.body_vitality >= target_unit->object.body_vitality;
                if (unit_wins)
                {
                    unit_survives = true;
                    unit_attacks = true;
                }
                else
                {
                    target_survives = true;
                    target_attacks = true;
                }
            }
            if (unit_survives)
                unit->object.body_vitality = 0.0001f;
            if (target_survives)
                target_unit->object.body_vitality = 0.0001f;
            if (target_attacks && target_effect_index != NONE)
            {
                target.object_index = unit_index;
                target.lunge = false;
                unit_cause_player_melee_damage(target_index, target_effect_index, &target_damage_owner, &target);
                damage_resolved = true;
            }
            if (unit_attacks && effect_index != NONE)
            {
                target.object_index = target_index;
                target.lunge = false;
                unit_cause_player_melee_damage(unit_index, effect_index, damage_owner, &target);
                damage_resolved = true;
            }
        }
        if (clang && !damage_resolved)
        {
            long weapon_index = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index);
            if (weapon_index != NONE)
            {
                struct weapon_definition *definition = (struct weapon_definition *)tag_get(0, 
                    unit_get(weapon_index)->object.definition_index);
                if (definition->clang_effect != NONE)
                {
                    real_point3d unit_camera;
                    real_point3d target_camera;
                    real_point3d midpoint;
                    unit_get_camera_position(unit_index, &unit_camera);
                    unit_get_camera_position(target_index, &target_camera);
                    midpoint.x = (unit_camera.x + target_camera.x) * 0.5f;
                    midpoint.y = (unit_camera.y + target_camera.y) * 0.5f;
                    midpoint.z = (unit_camera.z + target_camera.z) * 0.5f;
                    effect_new_unattached_from_point(definition->clang_effect, &midpoint, &unit->unit.aiming_vector,
                        &k_real_math_identity_matrix_storage.up, 1, 1, NULL, NULL);
                    simulation_action_unit_melee_clang(definition->clang_effect, &midpoint, &unit->unit.aiming_vector);
                }
            }
        }
    }
    else
    {
        unit_cause_player_melee_damage(unit_index, damage_effect_index, damage_owner, damage_target);
    }
}

static __forceinline bool units_seat_predicted(const game_globals_storage *game)
{
    bool result = false;
    if (game->options.game_simulation == 4) result = true;
    return result;
}
static __forceinline bool units_seat_in_air(long index)
{
    bool result = false;
    if (unit_get(index)->object.object_type == 0)
        result = motor_system_in_airborne_state(index);
    return result;
}
static __forceinline float units_seat_pin(float value)
{
    return (value > -1.0f ? value : -1.0f) < 1.0f ? (value > -1.0f ? value : -1.0f) : 1.0f;
}
static __forceinline void units_seat_line(const real_vector3d *p, const real_vector3d *v, float t, real_vector3d *out)
{
    out->i = p->i + v->i * t;
    out->j = p->j + v->j * t;
    out->k = p->k + v->k * t;
}
static __forceinline void units_seat_add(const real_vector3d *a, const real_vector3d *b, real_vector3d *out)
{
    out->i = a->i + b->i; out->j = a->j + b->j; out->k = a->k + b->k;
}
static __forceinline void units_seat_line(const real_vector3d *p, const real_point3d *v, float t, real_vector3d *out)
{
    out->i = p->i + v->x * t;
    out->j = p->j + v->y * t;
    out->k = p->k + v->z * t;
}
static __forceinline void units_seat_add(const real_vector3d *a, const real_point3d *b, real_vector3d *out)
{
    out->i = a->i + b->x; out->j = a->j + b->y; out->k = a->k + b->z;
}
static __forceinline void units_seat_scale(const real_vector3d *a, float scale, real_vector3d *out)
{
    out->i = a->i * scale; out->j = a->j * scale; out->k = a->k * scale;
}
static __forceinline float units_seat_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->i*b->i + a->j*b->j + a->k*b->k;
}
static __forceinline void units_seat_cross(const real_vector3d *a, const real_vector3d *b, real_vector3d *out)
{
float k = a->i*b->j-a->j*b->i;
float i = a->j*b->k-a->k*b->j;
float j = a->k*b->i-a->i*b->k;
out->i=i; out->j=j; out->k=k;
}
// @address 0x329bf8 ?unit_seat_acceleration_update@@YAXJ@Z
void unit_seat_acceleration_update(long index)
{
    unit_datum *unit = unit_get(index);
    unit->unit.m_unit_flags.set(_unit_flag_exit_vehicle_immediate, false);
    unit->unit.m_unit_flags.set(_unit_flag_acceleration_stun, false);
    bool disabled = (bool)((unit->object.damage_flags >> 2) & 1) && !units_seat_in_air(index);
    real_matrix4x3 matrix;
    unit_seat_acceleration parameters;
    real_vector3d value;
    if (unit_get_acceleration_origin_and_data(index, &matrix, &parameters) && !disabled) {
        unit_seat_acceleration *p = (unit_seat_acceleration *)&parameters;
        long cursor = unit->unit.seat_acceleration_history_index;
        if (unit->unit.m_unit_flags.test(_unit_flag_reset_seat_acceleration)) {
            unit_seat_acceleration_reset_history(index, &matrix.position);
            unit->unit.m_unit_flags.set(_unit_flag_reset_seat_acceleration, false);
        }
        game_globals_storage *game = game_globals;
        if (game->options.game_mode == 2 && unit->unit.seat_acceleration_reset_time != -1
            && game_time_globals->time-unit->unit.seat_acceleration_reset_time <= 30)
            unit_seat_acceleration_reset_history(index, &matrix.position);
        const real_point3d *old = &unit->unit.seat_acceleration_history[cursor];
        const real_point3d *middle = &unit->unit.seat_acceleration_history[(cursor+3)%6];
        const real_point3d *current = &matrix.position;
        float period = game_time_globals->tick_length * 3.0f;
        real_vector3d acceleration;
        memcpy(&acceleration, old, sizeof(acceleration));
        units_seat_line(&acceleration, middle, -2.0f, &acceleration);
        units_seat_add(&acceleration, current, &acceleration);
        units_seat_scale(&acceleration, (1.0f/period)*(1.0f/period), &acceleration);
        unit->unit.seat_acceleration_history[cursor] = matrix.position;
        unit->unit.seat_acceleration_history_index = (short)((cursor+1)%6);
        real_vector3d left;
        units_seat_cross(&matrix.up, &matrix.forward, &left);
        value.i = units_seat_dot(&acceleration, &matrix.forward) * p->forward_scale;
        value.j = units_seat_dot(&acceleration, &left) * p->side_scale;
        value.k = units_seat_dot(&acceleration, &matrix.up) * p->up_scale;
        float x = fabsf(value.i), y = fabsf(value.j), z = fabsf(value.k);
        float largest = x > (y > z ? y : z) ? x : (y > z ? y : z);
        if (p->stun_threshold > 0.0f && largest > p->stun_threshold)
            unit->unit.m_unit_flags.set(_unit_flag_acceleration_stun, true);
        if (p->eject_threshold > 0.0f && largest > p->eject_threshold && unit->object.parent_object_index != -1
            && unit->unit.parent_seat_index != -1 && units_seat_predicted(game))
            unit->unit.m_unit_flags.set(_unit_flag_exit_vehicle_immediate, true);
        value.i = units_seat_pin(value.i); value.j = units_seat_pin(value.j); value.k = units_seat_pin(value.k);
    } else {
        value = *global_zero_vector3d;
    }
    real_vector3d delta;
    subtract_vectors3d(&value, &unit->unit.seat_acceleration, &delta);
    units_seat_line(&unit->unit.seat_acceleration, &delta, 0.2f, &unit->unit.seat_acceleration);
}

// @address 0x328558 ?unit_add_starting_profile_equipment@@YAXJF_N0@Z
void unit_add_starting_profile_equipment(long p0, short p1, bool p2, bool p3)
{
    if (p0 != NONE)
    {
        if (p1 != NONE)
        {
            s_scenario_starting_profile *profile = (s_scenario_starting_profile *)global_scenario->starting_profiles.address + p1;
            unit_datum *unit = unit_get(p0);
            if (p2)
            {
                unit_delete_all_weapons(p0);
                unit->object.shield_vitality = 1.0f;
                unit->object.body_vitality = 1.0f;
                for (long i = 0; i < 4; i++)
                {
                    unit->unit.grenade_counts[i] = 0;
                }
            }
            for (long i = 0; i < 2; i++)
            {
                s_scenario_starting_profile_weapon *weapon = &profile->weapons[i];
                if (weapon->weapon.index != NONE)
                {
                    object_placement_data placement;
                    object_placement_data_new(&placement, weapon->weapon.index, p0, 0);
                    long placed = ::object_new(&placement);
                    if (placed != NONE)
                    {
                        weapon_datum *placed_datum = (weapon_datum *)object_header_get(placed)->datum;
                        struct weapon_definition *definition = (struct weapon_definition *)tag_get(0, placed_datum->definition_index);
                        if (definition->magazines.count() > 0)
                        {
                            weapon_set_initial_rounds(placed, 0, weapon->rounds_loaded, weapon->rounds_inventory);
                        }
                        if (p3)
                        {
                            placed_datum->item.inventory_flags = placed_datum->item.inventory_flags | 16;
                        }
                        if (!unit_add_weapon_to_inventory(p0, placed, _weapon_addition_method_silent))
                        {
                            object_delete(placed);
                        }
                    }
                }
            }
            unit->object.shield_vitality = unit->object.shield_vitality - profile->shield_vitality;
            unit->object.body_vitality = unit->object.body_vitality - profile->body_vitality;
            for (long i = 0; i < 4; i++)
            {
                signed char count = (signed char)(profile->grenade_counts[i] + unit->unit.grenade_counts[i]);
                unit->unit.grenade_counts[i] = (unsigned char)count;
                if (unit->unit.current_grenade_index == NONE && count > 0)
                {
                    unit->unit.current_grenade_index = (signed char)i;
                    unit->unit.desired_grenade_index = (signed char)i;
                }
            }
        }
    }
}


// @address 0x323fd8 ?unit_euler_axis_buildplan@@YAXMMMMPAUeuler_axis_plan@@@Z
// match: 0x323fd8: pointer __restrict qualifier prevents alias reload
void unit_euler_axis_buildplan(float position, float velocity, float maximum_velocity, float acceleration, euler_axis_plan *__restrict plan)
{
    plan->position = position;
    plan->velocity = velocity;
    plan->stationary = fabs(plan->position) < 0.001f && fabs(plan->velocity) < 0.001f;
    if (plan->stationary)
    {
        plan->acceleration = 0.0f;
        plan->accel_t = 0.0f;
        plan->coast_t = 0.0f;
        plan->deceleration = 0.0f;
        plan->decel_t = 0.0f;
        return;
    }
    float stop_time = fabs(plan->velocity) / acceleration;
    bool positive_velocity = plan->velocity > 0.0f;
    if (plan->position + (stop_time / 2.0f) * plan->velocity / 2.0f < 0.0f)
    {
        unit_euler_axis_buildplan(-position, -velocity, maximum_velocity, acceleration, plan);
        plan->position *= -1.0f;
        plan->velocity *= -1.0f;
        plan->acceleration *= -1.0f;
        plan->deceleration *= -1.0f;
        return;
    }
    float distance = plan->position + stop_time * (plan->velocity / 2.0f);
    if (distance < 0.0f)
    {
        plan->acceleration = 0.0f;
        plan->accel_t = 0.0f;
        plan->deceleration = plan->velocity * plan->velocity / (2.0f * plan->position);
        plan->decel_t = -plan->velocity / plan->deceleration;
    }
    else
    {
        float time;
        if (positive_velocity)
            time = square_root(distance / acceleration);
        else
        {
            float negative_acceleration = -acceleration;
            float b = 2.0f * plan->velocity;
            float c = distance;
            float discriminant = b * b - 4.0f * negative_acceleration * c;
            float first_root = (-b - square_root(discriminant)) / (2.0f * negative_acceleration);
            float second_root = (-b + square_root(discriminant)) / (2.0f * negative_acceleration);
            if (!(first_root >= 0.0f && (second_root < 0.0f || first_root < second_root)))
            {
                time = 0.0f > second_root ? 0.0f : second_root;
            }
            else time = first_root;
        }
        float limited_time;
        if (maximum_velocity > 0.0f)
        {
            if (positive_velocity)
                limited_time = maximum_velocity / acceleration;
            else limited_time = (maximum_velocity + plan->velocity) / acceleration;
            limited_time = 0.0f > limited_time ? 0.0f : limited_time;
            if (time <= limited_time) limited_time = time;
        }
        else limited_time = time;
        float initial_acceleration = -acceleration;
        plan->deceleration = acceleration;
        plan->acceleration = initial_acceleration;
        if (positive_velocity)
        {
            plan->accel_t = limited_time + stop_time;
            plan->decel_t = limited_time;
        }
        else
        {
            plan->accel_t = limited_time;
            plan->decel_t = limited_time + stop_time;
        }
        if (limited_time < time)
        {
            float coasting_velocity = plan->velocity + plan->acceleration * plan->accel_t;
            float coasting_distance = 2.0f * coasting_velocity * (time - limited_time) - acceleration * (time - limited_time) * (time - limited_time);
            plan->coast_t = coasting_distance / coasting_velocity;
            return;
        }
    }
    plan->coast_t = 0.0f;
}

// @address 0x32a180 unit_force_drop_inventory_weapon
extern "C" void unit_force_drop_inventory_weapon(long unit_index, long weapon_index, bool unknown, long zero)
{
    unit_datum *unit = unit_get(unit_index);
    long inventory_index = -1;
    action_request current_request;
    action_request desired_request;
    for (long i = 0; i < 4; i++)
    {
        if (unit->unit.weapon_object_indices[i] == weapon_index)
        {
            inventory_index = i;
            break;
        }
    }
    if (inventory_index == unit->unit.current_weapon_index)
    {
        memset(&current_request, 0, sizeof(current_request));
        memset(&current_request, 0, sizeof(current_request));
        current_request.type = 46;
        current_request.evict.flag = (unknown == 0);
        current_request.evict.value = (unsigned char)zero;
        action_submit(unit_index, &current_request);
        return;
    }
    if (inventory_index == unit->unit.desired_weapon_index)
    {
        memset(&desired_request, 0, sizeof(desired_request));
        memset(&desired_request, 0, sizeof(desired_request));
        desired_request.type = 47;
        desired_request.evict.flag = (unknown == 0);
        desired_request.evict.value = (unsigned char)zero;
        action_submit(unit_index, &desired_request);
        return;
    }
    unit_inventory_drop_weapon(unit_index, (short)inventory_index, _unit_drop_type_drop, unknown);
}


// @address 0x32d370 ?unit_set_weapon_type@@YAXJJJFFPBUs_unit_weapon_set@@@Z
void unit_set_weapon_type(long unit_index, long inventory_index, long definition_index, short unknown, short control_flags,
    const s_unit_weapon_set *unit_weapon_set)
{
    long unit_index_loc = unit_index;
    unit_datum *unit = unit_get(unit_index_loc);
    long weapon_index = NONE;
    long weapon_definition_index = NONE;
    short weapon_multiplayer_identifier = NONE;
    short weapon_simulation_identifier = NONE;
    int weapon_count = 0;

    if (unit_index_loc != NONE)
    {
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
        if (weapon_index != NONE)
        {
            weapon_datum *weapon = (weapon_datum *)unit_get(weapon_index);
            weapon_definition_index = weapon->definition_index;
            weapon_multiplayer_identifier = weapon->multiplayer_weapon_identifier;
            weapon_simulation_identifier = weapon->simulation_weapon_identifier;
        }
        if (unit->unit.weapon_object_indices[0] != NONE)
            weapon_count++;
        if (unit->unit.weapon_object_indices[1] != NONE)
            weapon_count++;
        if (unit->unit.weapon_object_indices[2] != NONE)
            weapon_count++;
        if (unit->unit.weapon_object_indices[3] != NONE)
            weapon_count++;
        if (weapon_definition_index == definition_index && (short)weapon_multiplayer_identifier == unknown
            && (short)weapon_simulation_identifier == control_flags)
            return;
        if (weapon_index != NONE)
        {
            unit_force_drop_inventory_weapon(unit_index_loc, weapon_index, false, 1);
            weapon_index = NONE;
        }
        if (definition_index != NONE)
        {
            object_placement_data placement;
            object_placement_data_new(&placement, definition_index, unit_index_loc, 0);
            weapon_index = ::object_new(&placement);
            if (weapon_index != NONE)
            {
                ((weapon_datum *)unit_get(weapon_index))->simulation_weapon_identifier = control_flags;
                if (unknown != NONE)
                    game_engine_multiplayer_weapon_register(weapon_index, unknown);
                unit_inventory_set_weapon_index(unit_index_loc, (short)inventory_index, weapon_index, (e_unit_drop_type)NONE);
                item_show_inventory_unattached(weapon_index, unit_index_loc);
                weapon_stow(weapon_index, unit_index_loc);
                unit_apply_predicted_weapon_state(unit_index_loc, inventory_index);
                long player_index = unit->unit.player_index;
                if (player_index != NONE)
                    units_player_handle_weapon_put_away(player_index, NONE);
            }
        }
        unit_datum *desired_unit = unit_get(unit_index_loc);
        desired_unit->unit.desired_weapon_set_identifier = unit_weapon_set->set_identifier;
        short set_identifier = desired_unit->unit.desired_weapon_set_identifier;
        *(short *)desired_unit->unit.desired_weapon_indices = *(short *)unit_weapon_set->weapon_indices;
        desired_unit->unit.weapon_set_identifier = set_identifier;
        if (weapon_count == 0 && weapon_index != NONE && unit->unit.desired_weapon_indices[0] == inventory_index)
        {
            action_request request;
            memset(&request, 0, sizeof(request));
            memset(&request, 0, sizeof(request));
            request.data[0] = 1;
            request.data[2] = 1;
            request.type = 54;
            action_submit(unit_index_loc, &request);
        }
    }
}









// @address 0xd8688 ?unit_euler_aiming_update@@YAXPBUreal_matrix4x3@@MPATreal_vector3d@@PBT2@M1PBTreal_rectangle2d@@MMPAM@Z
void unit_euler_aiming_update(const real_matrix4x3 *matrix, float yaw, real_vector3d *aiming_vector, const real_vector3d *target_vector,
    float dt, real_vector3d *angular_velocity, const real_rectangle2d *input_aiming_bounds, float angular_velocity_limit,
    float angular_acceleration_limit, float *output_yaw)
{
    real_rectangle2d aiming_bounds = *input_aiming_bounds;
    real_vector3d initial_aiming = *aiming_vector;
    aiming_bounds.x0 = PIN(aiming_bounds.x0, -6.2831855f, 0.0f);
    aiming_bounds.x1 = PIN(aiming_bounds.x1, 0.0f, 6.2831855f);
    float yaw_span = (aiming_bounds.x1 - aiming_bounds.x0);
    bool full_circle = yaw_span >= 6.2830853f;
    if (full_circle)
    {
        aiming_bounds.x0 = PIN((aiming_bounds.x1 - 6.2831855f), -6.2831855f, 0.0f);
        yaw_span = 6.2831855f;
    }
    real_vector3d local_aiming;
    real_vector3d local_target;
    if (matrix)
    {
        matrix4x3_inverse_transform_normal(matrix, aiming_vector, &local_aiming);
        matrix4x3_inverse_transform_normal(matrix, target_vector, &local_target);
    }
    else
    {
        local_aiming = *aiming_vector;
        local_target = *target_vector;
    }
    real_euler_angles2d current_angles;
    real_euler_angles2d desired_angles;
    euler_angles2d_from_vector3d(&current_angles, &local_aiming);
    euler_angles2d_from_vector3d(&desired_angles, &local_target);
    bool unclamped = true;
    if (full_circle)
    {
        if (desired_angles.yaw < aiming_bounds.x0)
            desired_angles.yaw += 6.2831855f;
        else if (desired_angles.yaw > aiming_bounds.x1)
            desired_angles.yaw -= 6.2831855f;
        if (output_yaw)
            *output_yaw = desired_angles.yaw;
    }
    else
    {
        float high_yaw;
        float low_yaw;
        if (desired_angles.yaw > 0.0f)
        {
            high_yaw = desired_angles.yaw;
            low_yaw = desired_angles.yaw - 6.2831855f;
        }
        else
        {
            high_yaw = desired_angles.yaw + 6.2831855f;
            low_yaw = desired_angles.yaw;
        }
        float high_error = fabs(high_yaw - yaw);
        float low_error = fabs(low_yaw - yaw);
        if (high_error < low_error)
            desired_angles.yaw = high_yaw;
        else
            desired_angles.yaw = low_yaw;
        if (output_yaw)
            *output_yaw = desired_angles.yaw;
        if (desired_angles.yaw < aiming_bounds.x0)
        {
            desired_angles.yaw = aiming_bounds.x0;
            unclamped = false;
        }
        else if (desired_angles.yaw > aiming_bounds.x1)
        {
            desired_angles.yaw = aiming_bounds.x1;
            unclamped = false;
        }
    }
    if (desired_angles.pitch < aiming_bounds.y0)
    {
        desired_angles.pitch = aiming_bounds.y0;
        unclamped = false;
    }
    else if (desired_angles.pitch > aiming_bounds.y1)
    {
        desired_angles.pitch = aiming_bounds.y1;
        unclamped = false;
    }
    real_vector3d desired_vector;
    if (unclamped)
    {
        desired_vector = *target_vector;
    }
    else
    {
        vector3d_from_euler_angles2d(&desired_vector, &desired_angles);
        if (matrix)
        {
            matrix4x3_transform_normal(matrix, &desired_vector, &desired_vector);
            normalize3d(&desired_vector);
        }
    }
    real_vector3d rotation_axis = *angular_velocity;
    float axis_length = normalize3d(&rotation_axis);
    real_euler_angles2d velocity;
    if (axis_length != 0.0f)
    {
        real_vector3d rotated_vector = local_aiming;
        // match: 0xd8688: binding the float product by reference keeps dt in f16 through both trig calls.
        const float &scaled_angle = axis_length * dt;
        rotate_vector_about_axis(&rotated_vector, &rotation_axis, sine(scaled_angle), cosine(scaled_angle));
        real_euler_angles2d rotated_angles;
        euler_angles2d_from_vector3d(&rotated_angles, &rotated_vector);
        velocity.yaw = (rotated_angles.yaw - current_angles.yaw) / dt;
        velocity.pitch = (rotated_angles.pitch - current_angles.pitch) / dt;
    }
    else
    {
        velocity.yaw = 0.0f;
        velocity.pitch = 0.0f;
    }
    real_euler_angles2d angle_error;
    angle_error.yaw = (current_angles.yaw - desired_angles.yaw);
    angle_error.pitch = (current_angles.pitch - desired_angles.pitch);
    if (full_circle)
    {
        if (angle_error.yaw > 3.1415927f)
            angle_error.yaw -= 6.2831855f;
        else if (angle_error.yaw < -3.1415927f)
            angle_error.yaw += 6.2831855f;
    }
    euler_axis_plan yaw_plan;
    euler_axis_plan pitch_plan;
    unit_euler_axis_buildplan(angle_error.yaw, velocity.yaw, angular_velocity_limit, angular_acceleration_limit, &yaw_plan);
    unit_euler_axis_buildplan(angle_error.pitch, velocity.pitch, angular_velocity_limit, angular_acceleration_limit, &pitch_plan);
    unit_euler_axis_couple(&yaw_plan, &pitch_plan, angular_velocity_limit, angular_acceleration_limit);
    real_euler_angles2d new_velocity;
    bool yaw_done = unit_euler_axis_doplan(&yaw_plan, dt, angle_error.yaw, &angle_error.yaw, velocity.yaw, &new_velocity.yaw);
    bool pitch_done = unit_euler_axis_doplan(&pitch_plan, dt, angle_error.pitch, &angle_error.pitch, velocity.pitch, &new_velocity.pitch);
    bool done = yaw_done && pitch_done;
    if (done)
    {
        *aiming_vector = desired_vector;
        *angular_velocity = *global_zero_vector3d;
    }
    else
    {
        real_euler_angles2d new_angles;
        new_angles.yaw = (desired_angles.yaw + angle_error.yaw);
        new_angles.pitch = (desired_angles.pitch + angle_error.pitch);
        if (full_circle)
        {
            if (new_angles.yaw < aiming_bounds.x0)
            {
                float wrapped_yaw = (aiming_bounds.x0 - new_angles.yaw);
                wrapped_yaw = fmod(wrapped_yaw, 6.2831855f);
                new_angles.yaw = aiming_bounds.x1 - wrapped_yaw;
            }
            else if (new_angles.yaw > aiming_bounds.x1)
            {
                float wrapped_yaw = (new_angles.yaw - aiming_bounds.x1);
                wrapped_yaw = fmod(wrapped_yaw, 6.2831855f);
                new_angles.yaw = aiming_bounds.x0 + wrapped_yaw;
            }
        }
        new_angles.yaw = PIN(new_angles.yaw, aiming_bounds.x0, aiming_bounds.x1);
        new_angles.pitch = PIN(new_angles.pitch, aiming_bounds.y0, aiming_bounds.y1);
        real_vector3d new_vector;
        vector3d_from_euler_angles2d(&new_vector, &new_angles);
        real_euler_angles2d predicted_angles;
        predicted_angles.yaw = (new_angles.yaw + dt * new_velocity.yaw);
        predicted_angles.pitch = (new_angles.pitch + dt * new_velocity.pitch);
        real_vector3d predicted_vector;
        vector3d_from_euler_angles2d(&predicted_vector, &predicted_angles);
        float alignment = ((dot_product3d(&new_vector, &predicted_vector)));
        alignment = PIN(alignment, -1.0f, 1.0f);
        cross_product3d(&new_vector, &predicted_vector, angular_velocity);
        normalize3d(angular_velocity);
        float rotation_angle = arccosine(alignment) / dt;
        if (rotation_angle > angular_velocity_limit)
            rotation_angle = angular_velocity_limit;
        scale_vector3d(angular_velocity, rotation_angle, angular_velocity);
        if (matrix)
        {
            matrix4x3_transform_normal(matrix, &new_vector, aiming_vector);
            normalize3d(aiming_vector);
        }
        else
        {
            *aiming_vector = new_vector;
        }
    }
    if (full_circle)
    {
        real_vector3d relative_aiming;
        real_vector3d relative_target;
        real_vector3d relative_initial;
        if (matrix)
        {
            matrix4x3_inverse_transform_normal(matrix, aiming_vector, &relative_aiming);
            matrix4x3_inverse_transform_normal(matrix, target_vector, &relative_target);
            matrix4x3_inverse_transform_normal(matrix, &initial_aiming, &relative_initial);
            normalize3d(&relative_aiming);
            normalize3d(&relative_target);
            normalize3d(&relative_initial);
        }
        else
        {
            relative_initial = initial_aiming;
            relative_aiming = *aiming_vector;
            relative_target = *target_vector;
        }
        real_euler_angles2d initial_angles;
        real_euler_angles2d aiming_angles;
        real_euler_angles2d target_angles;
        euler_angles2d_from_vector3d(&initial_angles, &relative_initial);
        euler_angles2d_from_vector3d(&aiming_angles, &relative_aiming);
        euler_angles2d_from_vector3d(&target_angles, &relative_target);
        float target_delta = target_angles.yaw - initial_angles.yaw;
        if (target_delta > 3.1415927f)
            target_delta -= 6.2831855f;
        else if (target_delta < -3.1415927f)
            target_delta += 6.2831855f;
        float remaining_delta = target_angles.yaw - aiming_angles.yaw;
        if (remaining_delta > 3.1415927f)
            remaining_delta -= 6.2831855f;
        else if (remaining_delta < -3.1415927f)
            remaining_delta += 6.2831855f;
        float travelled_delta = aiming_angles.yaw - initial_angles.yaw;
        if (travelled_delta > 3.1415927f)
            travelled_delta -= 6.2831855f;
        else if (travelled_delta < -3.1415927f)
            travelled_delta += 6.2831855f;
        bool target_positive = target_delta >= 0.0f;
        bool remaining_positive = remaining_delta >= 0.0f;
        bool travelled_positive = travelled_delta >= 0.0f;
        bool adjusted = false;
        if (target_positive != travelled_positive)
        {
            aiming_angles.yaw = initial_angles.yaw;
            adjusted = true;
        }
        else if (target_positive != remaining_positive)
        {
            aiming_angles.yaw = target_angles.yaw;
            adjusted = true;
        }
        if (adjusted)
        {
            vector3d_from_euler_angles2d(&relative_aiming, &aiming_angles);
            normalize3d(&relative_aiming);
            if (matrix)
            {
                matrix4x3_transform_normal(matrix, &relative_aiming, aiming_vector);
                normalize3d(aiming_vector);
            }
            else
            {
                *aiming_vector = relative_aiming;
            }
        }
    }
}



static __forceinline long units_weapon_update_find_user(long unit_index)
{
    for (long user_index = 0; user_index < 4; user_index++)
        if (tls_first_person_weapon_data()[user_index].unit_index == unit_index)
            return user_index;
    return NONE;
}
static __forceinline bool units_weapon_update_get_machine(s_machine_identifier *identifier)
{
    bool valid = players_globals->network_valid;
    if (valid)
        memcpy(identifier, &players_globals->machine, sizeof(*identifier));
    return valid;
}
static __forceinline bool units_weapon_update_is_remote(long player_index)
{
    bool remote = true;
    if (player_index != NONE)
    {
        s_machine_identifier machine;
        if (units_weapon_update_get_machine(&machine))
        {
            player_datum *player = player_get(player_index);
            if (!memcmp(&player->machine, &machine, sizeof(machine)))
                remote = false;
        }
    }
    return remote;
}
static __forceinline bool units_weapon_update_can_interrupt_reload(long weapon_index)
{
    struct weapon_definition *definition = (struct weapon_definition *)tag_get(0, unit_get(weapon_index)->object.definition_index);
    bool result = false;
    if (definition->reload_interrupt_mode == 1)
        result = true;
    return result;
}
static __forceinline long units_weapon_update_grenade_sound_index(s_chud_globals_definition *chud, long user_index, long grenade_index)
{
    if (user_index < 0)
        return NONE;
    if (user_index >= 4)
        return NONE;
    if ((unsigned long)grenade_index > 3 || !chud)
        return NONE;
    chud_table_row *skin = &chud->rows[chud_get_user_skin_index(user_index)];
    return skin->grenade_pickup_sounds[grenade_index].index;
}
static __forceinline bool units_weapon_update_positive(signed char ticks)
{
    return ticks > 0;
}
static __forceinline long units_weapon_update_representation_index(long player_index)
{
    if (player_index != NONE)
        return game_get_player_character_type(player_index);
    return 0;
}
static __forceinline long units_weapon_update_get_zoom_level(long unit_index, const unit_datum *unit)
{
    if (unit->unit.desired_zoom_level != NONE && unit_can_zoom(unit_index))
        return unit->unit.desired_zoom_level;
    return NONE;
}
static __forceinline void units_weapon_update_zoom_sound(long unit_index, unit_datum *unit)
{
    long weapon_index = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index);
    long sound_index;
    long zoom_level_count;
    if (weapon_index != NONE && weapon_is_zoomable(weapon_index))
    {
        struct weapon_definition *weapon_definition = (struct weapon_definition *)tag_get(0, unit_get(weapon_index)->object.definition_index);
        sound_index = unit->unit.zoom_level == NONE ? weapon_definition->zoom_out_sound : weapon_definition->zoom_in_sound;
        zoom_level_count = weapon_definition->zoom_level_count;
    }
    else
    {
        long representation = units_weapon_update_representation_index(unit->unit.player_index);
        if (representation < 0)
            return;
        s_game_globals *globals = (s_game_globals *)global_game_globals;
        if (representation < globals->player_representations.count())
        {
            s_player_representation *player_representation = &globals->player_representations[representation];
            s_player_information_definition *information = (s_player_information_definition *)globals->player_information.address;
            if (unit->unit.zoom_level == NONE)
                sound_index = player_representation->binoculars_zoom_out_sound.index;
            else
                sound_index = player_representation->binoculars_zoom_in_sound.index;
            zoom_level_count = information->zoom_level_count;
        }
        else
            return;
    }
    if (sound_index != NONE)
    {
        float scale = 1.0f;
        if (unit->unit.zoom_level != NONE && zoom_level_count > 1)
            scale = (float)unit->unit.zoom_level / (float)(zoom_level_count - 1);
        c_game_sound_impulse_parameters parameters(sound_index, scale);
        parameters.submit_unattached_object_sound(unit_index, 157);
    }
}
bool unit_update_weapons(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    bool changed = false;
    if (!FLAG_TEST(definition->flags, 11))
    {
        if (unit->unit.weapon_change_ticks > 0)
        {
            if (--unit->unit.weapon_change_ticks == 0)
            {
                action_submit_event(unit_index, 47);
                action_submit_event(unit_index, 46);
            }
            changed = true;
        }
        if (unit->unit.dual_error_ticks[0])
        {
            unit->unit.dual_error_ticks[0]--;
            changed = true;
        }
        if (unit->unit.dual_error_ticks[1])
        {
            unit->unit.dual_error_ticks[1]--;
            changed = true;
        }
        if (FLAG_TEST(unit->unit.unit_flags, 1))
        {
            if (unit->object.parent_object_index == NONE && (unit->unit.control_flags & (1 << 14)) != 0)
            {
                unit_datum *animated_unit = unit_get(unit_index);
                c_animation_manager *manager = (c_animation_manager *)((char *)animated_unit + animated_unit->object.animation.offset);
                t_short_fixed<13> lowered = manager->get_slider_value_from_chain(3, _mixing_board_parent_slider_regular);
                if (lowered.m_value != 8192)
                {
                    unit_animation_lower_weapon(unit_index, 0.5f);
                    long user_index = units_weapon_update_find_user(unit_index);
                    if (user_index >= 0 && user_index < 4)
                    {
                        first_person_weapon_set_user_state(user_index, 0);
                        tls_first_person_weapon_data()[user_index].flags |= 0x200;
                    }
                    changed = true;
                }
            }
            else if (!unit_animation_weapon_raised(unit_index))
            {
                unit_animation_raise_weapon(unit_index, 0.5f);
                long user_index = units_weapon_update_find_user(unit_index);
                if (user_index >= 0 && user_index < 4)
                {
                    tls_first_person_weapon_data()[user_index].flags &= ~0x200;
                    first_person_weapon_raise(user_index, 0);
                }
                changed = true;
            }
        }
    }
    if (!FLAG_TEST(definition->flags, 10) && !FLAG_TEST(unit->object.damage_flags, 2))
    {
        if ((unit->object.inhibited_flags & 8) != 0)
        {
            action_submit_event(unit_index, 47);
            action_submit_event(unit_index, 46);
        }
        else
        {
            for (long weapon_number = 0; weapon_number < 2; weapon_number++)
            {
                long current = unit->unit.current_weapon_indices[weapon_number];
                long desired = unit->unit.desired_weapon_indices[weapon_number];
                if (current != NONE)
                    unit->unit.weapon_ages[current] = game_time_get();
                if (desired != current)
                {
                    action_request request = {};
                    memset(&request, 0, sizeof(request));
                    if (game_globals->options.game_mode == 2 && game_is_predicted())
                    {
                        if (units_weapon_update_is_remote(unit->unit.player_index))
                        {
                            request.data[0] = true;
                            request.data[1] = true;
                        }
                    }
                    /* a select on weapon_number would make it a strength-reduced induction variable */
                    if (weapon_number == 0)
                        request.type = 54;
                    else
                        request.type = 55;
                    action_submit(unit_index, &request);
                }
            }
        }
        if (((unit->unit.control_flags & (1 << 29)) != 0) || ((unit->unit.control_flags & (1 << 30)) != 0))
        {
            bool primary = ((unit->unit.control_flags & (1 << 29)) != 0) ? action_submit_event(unit_index, 61) : false;
            bool secondary = ((unit->unit.control_flags & (1 << 30)) != 0) ? action_submit_event(unit_index, 63) : false;
            if (unit->unit.player_index != NONE && (primary || secondary))
                simulation_action_unit_reload(unit_index, primary, secondary);
        }
        if (unit->unit.desired_grenade_index != unit->unit.current_grenade_index)
        {
            short grenade_index = unit_inventory_next_grenade(unit_index, unit->unit.desired_grenade_index, 0);
            if (grenade_index != NONE && grenade_index != unit->unit.current_grenade_index)
            {
                long user_index = NONE;
                long player_index = unit_get(unit_index)->unit.player_index;
                for (;;)
                {
                    s_chud_globals_definition *chud = ((s_chud_globals_definition *)chud_globals);
                    for (;;)
                    {
                        if (player_index != NONE)
                            user_index = (long)::director_get_perspective(player_index, user_index);
                        if (user_index == NONE)
                            break;
                        long sound_index = units_weapon_update_grenade_sound_index(chud, user_index, grenade_index);
                        if (sound_index != NONE)
                        {
                            unspatialized_impulse_sound_new(sound_index, 1.0f);
                            break;
                        }
                    }
                    if (user_index == NONE)
                        break;
                }
                unit->unit.current_grenade_index = (signed char)grenade_index;
            }
        }
        short zoom_level = units_weapon_update_get_zoom_level(unit_index, unit);
        if (unit->unit.zoom_level != zoom_level)
        {
            unit->unit.zoom_level = (signed char)zoom_level;
            if (unit->unit.zoom_level == NONE)
                unit->unit.zoom_transition_timer = 0.0f;
            units_weapon_update_zoom_sound(unit_index, unit);
        }
    }
    if (!FLAG_TEST(definition->flags, 11))
    {
        /* both locals are referenced ahead of weapon_number: c2 orders the indexed adds by symbol number */
        unit_datum *inventory_unit = unit;
        const signed char *current_indices = 0;
        long weapon_number = 0;
        current_indices = unit->unit.current_weapon_indices;
        for (; weapon_number < 2; weapon_number++)
        {
            bool primary = weapon_number == 0;
            if (current_indices[weapon_number] != NONE)
            {
                inventory_unit = (unit_datum *)object_header_get(unit_index)->datum;
                long weapon_index = NONE;
                if (inventory_unit->unit.current_weapon_indices[weapon_number] != NONE)
                    weapon_index = inventory_unit->unit.weapon_object_indices[inventory_unit->unit.current_weapon_indices[weapon_number]];
                e_motor_resource resource = primary ? _units_primary_weapon_resource : _units_secondary_weapon_resource;
                bool busy = motor_animation_resource_busy(unit_index, resource);
                bool autofire = false;
                if (unit->unit.persistent_control_ticks > 0)
                    busy = false;
                if (unit->unit.player_index != NONE)
                {
                    player_datum *player = player_get(unit->unit.player_index);
                    bool primary_fire = units_weapon_update_positive(player->primary_fire_ticks);
                    bool secondary_fire = units_weapon_update_positive(player->secondary_fire_ticks);
                    autofire = primary_fire | secondary_fire;
                }
                if (busy)
                {
                    c_motor_task *owner = motor_animation_get_resource_owner(unit_index, resource);
                    if (owner && owner->action == _action_primary_weapon_primary_reload && units_weapon_update_can_interrupt_reload(weapon_index))
                        busy = false;
                }
                c_flags<e_weapon_control_flags, unsigned short, 8> flags;
                flags.clear();
                unsigned long control_flags = unit->unit.control_flags;
                if (primary)
                {
                    flags.set(_weapon_control_trigger_down_0, FLAG_TEST(control_flags, 18));
                    flags.set(_weapon_control_trigger_down_1, FLAG_TEST(control_flags, 19));
                    flags.set(_weapon_control_trigger_held_0, FLAG_TEST(control_flags, 20));
                    flags.set(_weapon_control_trigger_held_1, FLAG_TEST(control_flags, 21));
                    flags.set(_weapon_control_integrated_light, FLAG_TEST(control_flags, 22));
                }
                else
                {
                    flags.set(_weapon_control_trigger_down_0, FLAG_TEST(control_flags, 23));
                    flags.set(_weapon_control_trigger_down_1, FLAG_TEST(control_flags, 24));
                    flags.set(_weapon_control_trigger_held_0, FLAG_TEST(control_flags, 25));
                    flags.set(_weapon_control_trigger_held_1, FLAG_TEST(control_flags, 26));
                    flags.set(_weapon_control_integrated_light, FLAG_TEST(control_flags, 27));
                }
                flags.set(_weapon_control_trigger_down_2, FLAG_TEST(control_flags, 31));
                flags.set(_weapon_control_busy, busy);
                flags.set(_weapon_control_autofire, autofire);
                flags.set(_weapon_control_use_secondary, unit->unit.zoom_level != NONE);
                weapon_owner_update(weapon_index, flags, primary ? unit->unit.primary_trigger : unit->unit.secondary_trigger);
            }
        }
        unit_update_dual_wield_weapons(unit_index);
        unit_drop_unused_support_weapons(unit_index);
    }
    return changed;
}

/* ---- 0x3231b0 unit_update_aiming (appended after the accepted bodies so their symbol order is untouched) ---- */
// @address 0x239e0 k_behavior_posture_info

static __forceinline bool units_parent_seat_blocks_aiming_clip(long unit_index)
{
    bool result = false;
    if (unit_index != NONE)
    {
        unit_datum *unit = unit_get(unit_index);
        if (unit->object.parent_object_index != NONE)
        {
            unit_datum *parent = unit_get(unit->object.parent_object_index);
            if (((1 << parent->object.object_type) & 0x1003) != 0 && unit->unit.parent_seat_index != NONE)
            {
                unit_definition *parent_definition = (unit_definition *)tag_get(0, parent->object.definition_index);
                result = FLAG_TEST(parent_definition->seats[unit->unit.parent_seat_index].flags, 6);
            }
        }
    }
    return result;
}

static __forceinline bool units_should_clip_aiming(long unit_index)
{
    bool result = true;
    unit_datum *unit = unit_get(unit_index);
    if (unit->unit.player_index != NONE)
    {
        if (unit->object.parent_object_index == NONE || units_parent_seat_blocks_aiming_clip(unit_index))
            result = false;
    }
    return result;
}


// @address 0x3231b0 ?unit_update_aiming@@YA_NJ@Z
bool unit_update_aiming(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    bool result = false;
    if (!FLAG_TEST(definition->flags, 10) && !FLAG_TEST(unit->object.damage_flags, 2))
    {
        real_vector3d old_aiming = unit->unit.aiming_vector;
        real_vector3d old_looking = unit->unit.looking_vector;
        float scale = unit->unit.aiming_speed == 1 ? definition->aiming_scale : 1.0f;
        s_unit_animation *animation = (s_unit_animation *)((char *)unit + unit->unit.animation_storage.offset);
        float aim_angle = 0.0f;
        float aim_max_velocity = definition->aiming_velocity_maximum * scale;
        float aim_max_acceleration = definition->aiming_acceleration_maximum * scale;
        if (aim_max_velocity == 0.0f && aim_max_acceleration == 0.0f)
        {
            unit->unit.aiming_vector = unit->unit.desired_aiming_vector;
            if (units_should_clip_aiming(unit_index))
                unit_clip_vector_to_aiming_screen_bounds(unit_index, &unit->unit.aiming_vector, false);
            unit->unit.aiming_velocity = *global_zero_vector3d;
        }
        else if (animation->flags.test(_unit_animation_aiming_euler_bit))
        {
            if (animation->flags.test(_unit_animation_primary_aiming_screen_bounds_bit))
            {
                real_matrix4x3 matrix;
                object_get_orientation(unit_index, &matrix.forward, &matrix.up);
                if (unit->unit.actor_index != NONE)
                {
                    long posture = actor_get_posture(unit->unit.actor_index);
                    if (posture != 0 && k_behavior_posture_info[posture].peek_type == 2)
                        negate_vector3d(&matrix.forward, &matrix.forward);
                }
                matrix.scale = 1.0f;
                cross_product3d(&matrix.up, &matrix.forward, &matrix.left);
                matrix.position = *global_origin3d;
                real_vector3d direction;
                if ((unit->unit.control_flags & 0x10000) != 0)
                {
                    direction = unit->unit.desired_aiming_vector;
                    direction.k = 0.0f;
                    if (normalize3d(&direction) == 0.0f)
                        direction = matrix.forward;
                }
                else
                {
                    direction = unit->unit.facing_vector;
                }
                float angle = signed_angle_between_normals3d(&matrix.forward, &direction, &matrix.up);
                unit_euler_aiming_update(&matrix, angle, &unit->unit.aiming_vector, &unit->unit.desired_aiming_vector,
                    game_time_globals->tick_length, &unit->unit.aiming_velocity,
                    (const real_rectangle2d *)&animation->primary_aiming_screen_bounds,
                    aim_max_velocity, aim_max_acceleration, &aim_angle);
            }
            else
            {
                object_get_orientation(unit_index, &unit->unit.aiming_vector, NULL);
                unit->unit.aiming_velocity = *global_zero_vector3d;
            }
        }
        else
        {
            angular_accelerate_to_position(&unit->unit.aiming_vector, &unit->unit.desired_aiming_vector,
                game_time_globals->tick_length, &unit->unit.aiming_velocity, aim_max_velocity, aim_max_acceleration);
        }

        float aim_change;
        float look_change;
        if (definition->aiming_velocity_maximum == 0.0f)
        {
            aim_change = 0.0f;
            look_change = 0.0f;
        }
        else
        {
            float aim_tick_length = game_tick_length();
            aim_change = angle_between_vectors3d(&unit->unit.aiming_vector, &old_aiming) /
                (definition->aiming_velocity_maximum * aim_tick_length);
            aim_change = PIN(aim_change, 0.0f, 1.0f);
            real_vector3d old_horizontal = old_aiming;
            real_vector3d new_horizontal = unit->unit.aiming_vector;
            old_horizontal.k = 0.0f;
            new_horizontal.k = 0.0f;
            normalize3d_with_default(&old_horizontal, global_up3d);
            normalize3d_with_default(&new_horizontal, global_up3d);
            float look_tick_length = game_tick_length();
            look_change = angle_between_vectors3d(&old_horizontal, &new_horizontal) /
                (definition->aiming_velocity_maximum * look_tick_length);
            look_change = PIN(look_change, 0.0f, 1.0f);
        }
        unit->unit.aiming_change = (unsigned char)(aim_change * 255.0f);
        unit->unit.looking_change = (unsigned char)(look_change * 255.0f);

        float look_max_velocity = definition->looking_velocity_maximum * scale;
        float look_max_acceleration = definition->looking_acceleration_maximum * scale;
        if (look_max_velocity == 0.0f && look_max_acceleration == 0.0f)
        {
            unit->unit.looking_vector = unit->unit.desired_looking_vector;
            unit_clip_vector_to_aiming_screen_bounds(unit_index, &unit->unit.looking_vector, true);
            unit->unit.looking_velocity = *global_zero_vector3d;
        }
        else if (animation->flags.test(_unit_animation_looking_euler_bit))
        {
            if (animation->flags.test(_unit_animation_secondary_aiming_screen_bounds_bit))
            {
                real_matrix4x3 matrix;
                matrix.scale = 1.0f;
                object_get_orientation(unit_index, &matrix.forward, &matrix.up);
                matrix.forward = unit->unit.aiming_vector;
                cross_product3d(&matrix.up, &matrix.forward, &matrix.left);
                cross_product3d(&matrix.forward, &matrix.left, &matrix.up);
                matrix.position = *global_origin3d;
                unit_euler_aiming_update(&matrix, aim_angle, &unit->unit.looking_vector, &unit->unit.desired_looking_vector,
                    game_time_globals->tick_length, &unit->unit.looking_velocity,
                    (const real_rectangle2d *)&animation->secondary_aiming_screen_bounds,
                    look_max_velocity, look_max_acceleration, NULL);
            }
            else
            {
                unit->unit.looking_vector = unit->unit.aiming_vector;
                unit->unit.looking_velocity = *global_zero_vector3d;
            }
        }
        else
        {
            angular_accelerate_to_position(&unit->unit.looking_vector, &unit->unit.desired_looking_vector,
                game_time_globals->tick_length, &unit->unit.looking_velocity, look_max_velocity, look_max_acceleration);
        }

        if (fabs(old_aiming.i - unit->unit.aiming_vector.i) >= k_real_epsilon ||
            fabs(old_aiming.j - unit->unit.aiming_vector.j) >= k_real_epsilon ||
            fabs(old_aiming.k - unit->unit.aiming_vector.k) >= k_real_epsilon ||
            fabs(old_looking.i - unit->unit.looking_vector.i) >= k_real_epsilon ||
            fabs(old_looking.j - unit->unit.looking_vector.j) >= k_real_epsilon ||
            fabs(old_looking.k - unit->unit.looking_vector.k) >= k_real_epsilon)
        {
            result = true;
            object_set_requires_motion(unit_index);
        }
    }
    return result;
}

// ---- unit_can_enter_seat (0x325260) support types ----

typedef char units_player_vehicle_usage_offset[(offsetof(player_datum, traits.m_movement_traits.m_vehicle_usage) == 0x2e2) ? 1 : -1];

// @address 0x325260 ?unit_can_enter_seat@@YA_NJJFPAJPA_N1@Z
bool unit_can_enter_seat(long unit_index, long parent_index, short seat_index, long *occupant_index, bool *boarding, bool *unknown)
{
    unit_datum *parent = unit_get(parent_index);
    unit_definition *definition = (unit_definition *)tag_get(0, parent->object.definition_index);
    unit_seat *seat = units_seat_element_get(definition, seat_index);
    unit_datum *unit = unit_get(unit_index);
    unit_datum *target = parent;
    long top_index = parent_index;
    while (top_index != NONE)
    {
        unit_datum *current = unit_get(top_index);
        if (!FLAG_TEST(current->object.object_flags, 26))
            break;
        top_index = current->object.parent_object_index;
    }
    if (((1 << object_header_get(top_index)->object_type) & 0x1003) != 0)
        target = unit_get(top_index);
    bool occupied = false;
    long occupant = NONE;
    bool can_enter = true;
    bool linked = FLAG_TEST(seat->flags, 11);
    long linked_seat_index = linked ? (short)seat->linked_seat : NONE;
    bool enemy_in_linked = false;
    bool linked_action = false;
    bool linked_boarding = false;
    bool enemy_present = false;
    if (unit_index == parent_index)
        can_enter = false;
    else if (unit->object.parent_object_index != NONE)
        can_enter = false;
    else if (unit->unit.player_index != NONE && FLAG_TEST(target->unit.unit_flags, 12))
        can_enter = false;
    else if (unit->unit.player_index == NONE && FLAG_TEST(seat->flags, 17))
        can_enter = false;
    else if (unit->unit.player_index != NONE && FLAG_TEST(seat->flags, 16))
        can_enter = false;
    else if (linked && parent->object.shield_vitality > 0.0f)
        can_enter = false;
    else if (action_executing(parent_index, _action_phase))
        can_enter = false;
    {
    c_multi_unit_passenger_iterator iterator;
    iterator.begin(parent_index);
    while (iterator.next())
    {
        long passenger_index = iterator.m_passenger_index;
        unit_datum *passenger = unit_get(passenger_index);
        if (iterator.m_seat_index == seat_index && iterator.m_parent_index == parent_index)
        {
            occupied = true;
            occupant = passenger_index;
        }
        else if (linked && passenger->unit.parent_seat_index == linked_seat_index && iterator.m_parent_index == parent_index)
        {
            enemy_in_linked = linked && game_team_is_enemy(unit->unit.team, passenger->unit.team);
            if (enemy_in_linked)
            {
                linked_boarding = unit->unit.actor_index == NONE && unit_seat_is_being_boarded(parent_index, (short)linked_seat_index);
                linked_action = action_executing(passenger_index, _action_vehicle_exit);
            }
        }
        else if ((unsigned short)passenger->unit.parent_seat_index != 0xffff && unit->unit.team != NONE &&
            game_team_is_enemy(unit->unit.team, passenger->unit.team))
            enemy_present = true;
    }
    }
    if (can_enter)
    {
        if (!linked)
        {
            if (occupied)
                can_enter = false;
            else if (enemy_present && !(game_globals->options.game_mode == 2))
                can_enter = false;
        }
        else if (occupied)
            can_enter = false;
        else if (linked_seat_index == NONE)
        {
            if (FLAG_TEST(seat->flags, 14) && !enemy_present)
                can_enter = false;
        }
        else if (!enemy_in_linked || linked_action || linked_boarding)
            can_enter = false;
    }
    if (occupant_index)
        *occupant_index = occupant;
    if (boarding)
        *boarding = linked;
    if (can_enter && unit->unit.player_index != NONE)
    {
        long ultimate_index = inl_object_get_ultimate_parent(parent_index);
        collision_result collision;
        collision.global_material.m_index = NONE;
        real_point3d unit_center;
        real_point3d parent_center;
        real_vector3d direction;
        object_get_center_of_mass(unit_index, &unit_center);
        object_get_center_of_mass(ultimate_index, &parent_center);
        vector_from_points3d(&unit_center, &parent_center, &direction);
        if (collision_test_vector(k_unit_enter_seat_collision_flags, false, &unit_center, &direction, unit_index, parent_index, NONE,
            &collision))
            can_enter = false;
    }
    if (can_enter && unit->unit.player_index != NONE && game_globals->options.game_mode == 2)
    {
        player_datum *player = (player_datum *)(player_data->data +
            (unit->unit.player_index & 0xffff) * sizeof(player_datum));
        switch ((long)player->traits.m_movement_traits.m_vehicle_usage)
        {
        case 1:
            can_enter = false;
            break;
        case 2:
            can_enter = !FLAG_TEST(seat->flags, 2) && !FLAG_TEST(seat->flags, 3);
            break;
        }
        if (!can_enter && unknown)
            *unknown = true;
    }
    return can_enter;
}


// @address 0x11e598 ?unit_estimate_position@@YAXJFPBTreal_point3d@@PBTreal_vector3d@@11PAT1@@Z
void unit_estimate_position(long object_index, short estimate_mode, const real_point3d *body_position, const real_vector3d *vector1,
    const real_vector3d *vector2, const real_vector3d *vector3, real_point3d *estimated_position)
{
    unit_datum *unit = unit_get(object_index);
    if (unit->object.parent_object_index == NONE && !(bool)(_rotl(unit->object.damage_flags, 30) & 1) && unit->object.object_type == 0)
    {
        biped_estimate_camera_position(object_index, estimate_mode, vector1 == NULL, body_position, vector1, vector2, vector3,
            estimated_position);
        return;
    }

    bool found = false;
    long pathfinding_index;
    real_point3d origin;
    if (unit->object.object_type == 0 && unit->object.parent_object_index != NONE)
    {
        unit_datum *parent = unit_get(unit->object.parent_object_index);
        if (parent->object.object_type == 1)
        {
            vehicle_find_pathfinding_location(unit->object.parent_object_index, NULL, &pathfinding_index, NULL, &origin, NULL, NULL);
            if (pathfinding_index != NONE)
                found = true;
        }
    }
    if (!found)
        object_get_origin(object_index, &origin);

    switch (estimate_mode)
    {
    case 3:
        if (unit_get_gun_position(object_index, true, estimated_position))
            break;
        // fall through
    case 1:
    case 2:
        unit_get_head_position(object_index, estimated_position);
        break;
    default:
        unit_get_camera_position(object_index, estimated_position);
        break;
    }

    real_vector3d body_to_estimated;
    vector_from_points3d(&origin, body_position, &body_to_estimated);
    point_from_line3d(estimated_position, &body_to_estimated, 1.0f, estimated_position);
}


// @rdata 0x82021694
static const float units_seat_maximum_distance = 3.4028234663852886e+38f;

// @address 0x324c20 ?unit_consider_nearby_seats@@YAXJJPAJPAF1PAMPA_N@Z
void unit_consider_nearby_seats(long unit_index, long target_index, long *out_unit, short *out_seat_a, short *out_seat_b,
    float *out_distance, bool *out_flag)
{
    unit_seat_source sources[64];
    unit_datum *unit = unit_get(unit_index);
    unit_datum *target = unit_get(target_index);
    long seat_limit;
    {
        short seat_count = 0;
        ::unit_get_all_seats_internal(target_index, sources, &seat_count, 64, false);
        if (unit->unit.player_index != NONE && flag_test(target->unit.unit_flags, 12L))
            return;
        seat_limit = seat_count;
    }
    if (seat_limit <= 0)
        return;
    float one = 1.0f;
    float one_and_half = 1.5f;
    for (short i = 0; i < seat_limit; i++)
    {
        long seat_unit = sources[i].unit_index;
        const unit_seat *seat = sources[i].definition;
        short seat_idx = sources[i].seat_index;
        unit_datum *seat_unit_datum = unit_get(seat_unit);
        if (seat->label == NONE)
            continue;
        if (!unit_test_seat_entrance_markers(unit_index, seat_unit, seat_idx))
            continue;
        long eviction_unit = NONE;
        bool enter_flag = false;
        bool direct_flag = false;
        short priority = 0;
        if (unit_can_enter_seat(unit_index, seat_unit, seat_idx, &eviction_unit, &enter_flag, &direct_flag))
            priority = (short)(3 + (enter_flag != 0));
        else if (direct_flag)
            priority = 2;
        else if (eviction_unit != NONE)
        {
            long occupant = unit_get(eviction_unit)->unit.actor_index;
            if (occupant != NONE && ai_try_vehicle_eviction(occupant, unit_index, false))
                priority = 1;
        }
        if (priority > 1)
        {
            if (flag_test(seat->flags, 9L) && seat_unit_datum->unit.driver_object_index == NONE)
                priority = 0;
            else if (!motor_animation_test_mode(unit_index, seat->label))
                priority = 0;
        }
        if (priority <= 0)
            continue;
        float scale = one;
        float best = units_seat_maximum_distance;
        bool seat_flag = ((seat->flags >> 2) & 1) != 0;
        bool take = false;
        real_point3d entry_a;
        real_point3d entry_b;
        if (unit_get_best_seat_entrance_animation(unit_index, seat_unit, seat_idx, &entry_a, &entry_b) != NONE)
        {
            float dx = unit->object.bounding_center.x - entry_b.x;
            float dz = unit->object.bounding_center.z - entry_b.z;
            float dy = unit->object.bounding_center.y - entry_b.y;
            best = sqrtf(dy * dy + (dz * dz + dx * dx));
        }
        float dy = unit->object.bounding_center.y - entry_a.y;
        float dz = unit->object.bounding_center.z - entry_a.z;
        float dx = unit->object.bounding_center.x - entry_a.x;
        float distance = sqrtf(dx * dx + (dz * dz + dy * dy));
        if (best > distance)
            best = distance;
        if (*out_flag && !seat_flag)
            scale = one_and_half;
        if (*(unsigned short *)out_seat_a == 0xffff)
            take = true;
        else if (priority > *out_seat_b)
            take = true;
        else if (best * scale < *out_distance)
            take = true;
        if (take)
        {
            *out_distance = best;
            *out_unit = seat_unit;
            *out_seat_a = seat_idx;
            *out_seat_b = priority;
            *out_flag = seat_flag;
        }
    }
}


// @address 0x321558 ?unit_place@@YAXJPAUs_scenario_unit_datum@@@Z
void unit_place(long unit_index, s_scenario_unit_datum *scenario_unit)
{
    unit_datum *unit = unit_get(unit_index);
    if (scenario_unit->body_vitality > 0.0f)
        unit->object.body_vitality = scenario_unit->body_vitality;
    if ((scenario_unit->flags & 4) != 0)
        unit->unit.m_unit_flags.set(_unit_flag_not_enterable_by_player, true);
    object_set_invalid_for_recycling(unit_index, true);
    unit->object.physics_reconnect_time = -0x8000;
    if ((scenario_unit->flags & 1) != 0)
    {
        unit->unit.m_unit_flags.set(_unit_flag_placed_dead, true);
        s_destroy_routine_response response;
        memset(&response, 0, sizeof(response));
        response.silent = true;
        object_kill(unit_index, 0, damage_owner_unknown(), &response, true, 0);
        unit_delete_all_weapons(unit_index);
        unit_delete_current_equipment(unit_index);
        memset(unit->unit.grenade_counts, 0, sizeof(unit->unit.grenade_counts));
        unit_died(unit_index, NONE, NULL, true, false);
        object_compute_node_matrices(unit_index);
        if ((bool)(_rotl(unit->object.physics_flags, 23) & 1) && unit->object.parent_object_index == NONE)
            object_header_get(unit_index)->flags &= 0xFD;
    }
}

static void units_physics_get_center(const c_havok_component *component, real_point3d *center)
{
    const phys_rigid_body *body = component->m_rigid_bodies[0].m_rigid_body;
    center->x = body->get_contact_position()(0);
    center->y = body->get_contact_position()(1);
    center->z = body->get_contact_position()(2);
}

// @address 0x348600 unit_clamp_point_to_physics_shape
extern "C" void unit_clamp_point_to_physics_shape(long object_index, real_point3d *point, float margin)
{
    unit_datum *unit = unit_get(object_index);
    biped_definition *definition = (biped_definition *)tag_get(0, unit->object.definition_index);
    long component_index = unit->object.havok_component_index;
    s_character_physics_definition *physics = &definition->physics;
    // match: 0x348600: value range assumption guides branch lowering
    __assume(physics != 0);
    if (component_index == NONE || unit->object.parent_object_index != NONE || !flag_test(unit->object.physics_flags, 7L) ||
        flag_test(physics->flags, 0L))
        return;
    c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, component_index);
    if (flag_test(component->m_flags, 8L) || component->m_rigid_bodies.getSize() <= 0 || !(component->m_rigid_bodies[0].m_flags & 2))
        return;
    const void *first_shape = physics->pills.count ? (const void *)&TAG_BLOCK_GET_ELEMENT(&physics->pills, 0,
        physics_capsule_record)->shape : (const void *)&TAG_BLOCK_GET_ELEMENT(&physics->spheres, 0, physics_sphere_record)->shape;
    const void *second_shape = physics->pills.count ? (const void *)&TAG_BLOCK_GET_ELEMENT(&physics->pills, 1,
        physics_capsule_record)->shape : (const void *)&TAG_BLOCK_GET_ELEMENT(&physics->spheres, 1, physics_sphere_record)->shape;
    float radius = physics->radius;
    float upper = 3.4028234663852886e38f;
    float lower = -3.4028234663852886e38f;
    if (!(margin < radius))
        margin = radius;
    real_point3d pivot;
    pivot.z = 0.0f;
    union {
        real_point3d center;
        real_vector3d direction;
    } offset;
    units_physics_get_center(component, &offset.center);
    if (component->m_rigid_bodies[0].m_rigid_body->get_collider()->get_shape() == first_shape) {
        upper = physics->height_standing + offset.center.z - physics->radius;
        lower = physics->radius + offset.center.z;
    } else if (component->m_rigid_bodies[0].m_rigid_body->get_collider()->get_shape() == second_shape) {
        upper = physics->height_crouching + offset.center.z - physics->radius;
        lower = physics->radius + offset.center.z;
    }
    pivot = offset.center;
    pivot.z = PIN(point->z, lower, upper);
    vector_from_points3d(&pivot, point, &offset.direction);
    if (limit3d(&offset.direction, radius - margin)) {
        point->x = units_physics_offset_component(pivot.x, offset.direction.i);
        point->y = units_physics_offset_component(pivot.y, offset.direction.j);
        point->z = units_physics_offset_component(pivot.z, offset.direction.k);
    }
}


// @rdata 0x8202a1a4
static const float k_unit_died_camouflage_duration = 4.0f;
static __forceinline long unit_died_seconds_to_ticks(long seconds)
{
    return game_time_globals->ticks_per_second * seconds;
}

// @address 0x326df8 ?unit_died@@YAXJJPBTreal_vector3d@@_N1@Z
void unit_died(long unit_index, long damage_type, const real_vector3d *direction, bool alternate, bool force_weapon_drop)
{
    unit_datum *unit = unit_get(unit_index);
    if (((1 << unit->object.object_type) & 1))
        biped_died(unit_index, damage_type, direction, alternate, false);
    unit_animation_raise_weapon(unit_index, 0.0f);
    fpw_first_person *fp_entries = tls_first_person_weapon_data();
    long user_index = units_weapon_update_find_user(unit_index);
    if (user_index >= 0) {
        if (user_index < 4) {
            fpw_first_person *entry = &fp_entries[user_index];
            if ((unsigned char)first_person_weapon_has_custom_animation(unit_index))
                first_person_weapon_clear_custom_animation(unit_index);
            entry->flags &= 0xfffffc4f;
            first_person_weapon_raise((unsigned long)user_index, -1);
        }
    }
    if (unit->unit.player_index != -1) {
        unit->unit.m_unit_flags.set(_unit_flag_vision_mode, false);
        unit->unit.vision_mode_fraction = 0.0f;
    }
    if (unit->unit.player_index != -1)
        object_set_garbage(unit_index, true, unit_died_seconds_to_ticks(35));
    else
        object_set_garbage(unit_index, true, unit_died_seconds_to_ticks(10));
    if (unit->unit.player_index != -1)
        player_handle_death(unit->unit.player_index);
    if (unit->unit.simulation_actor_datum_index != -1)
        simulation_action_actor_died(unit_index);
    if (unit->unit.actor_index != -1) {
        actor_datum *actors = (actor_datum *)actor_data->data;
        unit->unit.last_squad_index = actors[datum_index_to_absolute_index(unit->unit.actor_index)].squad_index;
        actor_died(unit->unit.actor_index);
        unit->unit.actor_index = -1;
    }
    unit->unit.time_of_death = game_time_get();
    unit->unit.m_unit_flags.set(_unit_flag_actively_controlled, false);
    unit_active_camouflage_disable(unit_index, k_unit_died_camouflage_duration);
    unit->unit.control_flags = 0;
    unit_datum *dialogue_unit = unit_get(unit_index);
    signed char dialogue_value = units_get_dialogue_state(dialogue_unit)->died.value;
    if (dialogue_value > 0 && dialogue_value < 15)
        unit_dialogue_interrupt(unit_index);
    for (long i = 0; i < 2; i++) {
        if (unit->unit.current_weapon_indices[i] != -1) {
            c_flags<e_weapon_control_flags, unsigned short, 8> no_control;
            no_control.clear();
            long weapon_index = -1;
            unit_datum *weapon_unit = unit_get(unit_index);
            signed char weapon_slot = weapon_unit->unit.current_weapon_indices[i];
            if (weapon_slot != -1)
                weapon_index = weapon_unit->unit.weapon_object_indices[weapon_slot];
            weapon_owner_update(weapon_index, no_control, 0.0f);
        }
    }
    unit_datum *flag_unit = unit_get(unit_index);
    flag_unit->unit.unit_flags &= 0xffefffff;
    if (unit->object.parent_object_index != -1)
        object_detach(unit_index);
    unit_datum *drop_unit = unit_get(unit_index);
    for (long i = 0; i < 4; i++) {
        if (i != drop_unit->unit.current_weapon_indices[0] && i != drop_unit->unit.current_weapon_indices[1])
            unit_inventory_drop_weapon(unit_index, (short)i, _unit_drop_type_drop, false);
    }
    unit_drop_current_equipment(unit_index);
    unit_drop_grenades(unit_index);
    if (unit->unit.weapon_change_ticks == 0 || force_weapon_drop) {
        action_request request;
        memset(&request, 0, sizeof(request));
        memset(&request, 0, sizeof(request));
        request.type = 47;
        action_submit(unit_index, &request);
        action_request request2;
        memset(&request2, 0, sizeof(request2));
        memset(&request2, 0, sizeof(request2));
        request2.type = 46;
        action_submit(unit_index, &request2);
    }
    c_motor_task *task;
    task = motor_task_find_by_action(unit_index, _action_melee_ai);
    if (task != 0)
        motor_system_interrupted(unit_index, task, true);
    task = motor_task_find_by_action(unit_index, _action_melee_player);
    if (task != 0)
        motor_system_interrupted(unit_index, task, true);
    task = motor_task_find_by_action(unit_index, _action_grenade);
    if (task != 0)
        motor_system_interrupted(unit_index, task, true);
    task = motor_task_find_by_action(unit_index, _action_equipment);
    if (task != 0)
        motor_system_interrupted(unit_index, task, true);
}

static __forceinline bool units_aftermath_flag_test(unsigned long value, long bit)
{
    c_flags_no_init<e_unit_flags, unsigned long, 31> flags;
    flags.m_storage = value;
    return flags.test((e_unit_flags)bit);
}

static __forceinline c_animation_manager *units_aftermath_get_animation_manager(biped_datum *object)
{
    long offset = object->object.animation.offset;
    return (c_animation_manager *)((char *)object + offset);
}
static __forceinline bool units_aftermath_test_mode_and_state(biped_datum *object, long mode, long state)
{
    return units_aftermath_get_animation_manager(object)->find_goal(mode, mode, mode, state, 0);
}
// @address 0x325ba8 ?unit_damage_aftermath_compute@@YAXJJPBUs_damage_data@@PAUs_damage_aftermath_result_data@@@Z
void unit_damage_aftermath_compute(long index, long damage_flags, const s_damage_data *damage, s_damage_aftermath_result_data *result)
{
    biped_datum *object = (biped_datum *)object_header_get(index)->datum;
    unit_definition *definition = (unit_definition *)tag_get(0,
        ((biped_datum *)object_header_get(index)->datum)->object.definition_index);
    bool biped_mask = units_aftermath_flag_test(1 << object_header_get(index)->object_type, 0);
    long *model_damage = 0;
    s_damage_effect_definition *damage_definition = (s_damage_effect_definition *)tag_get(0, result->damage_definition_index);
    struct damage_definition *damage_info = &damage_definition->damage;
    bool killed = (result->flags & 1) != 0;
    bool heavy = false;
    bool airborne = false;
    if (definition->model.index != -1) {
        s_model_definition *model = (s_model_definition *)tag_get(0, definition->model.index);
        if (model->damage_info.count > 0) model_damage = (long *)model->damage_info.address;
    }
    heavy = killed && damage_info->instantaneous_acceleration >= 10.0f;
    if (killed) {
        airborne = units_aftermath_test_mode_and_state(object, 226, 79);
        airborne = airborne && biped_mask != 0 && motor_system_in_airborne_state(index);
    }
    if (damage_info->side_effect == 3 && biped_mask != 0 &&
        units_aftermath_flag_test(((biped_definition *)tag_get(0, object->object.definition_index))->flags, 10))
    {
        c_motor_request request;
        request.setup_stun(_action_none);
        motor_system_submit(index, &request);
    }
    unsigned long ignore_ping = damage->flags & 0x10;
    bool no_ping = false;
    bool blocked = false;
    bool reaction = false;
    bool special = false;
    result->ping_type = 0;
    if (ignore_ping || (damage_info->flags & 0x10) || units_aftermath_flag_test(object->unit.unit_flags, 18)) no_ping = true;
    long type_mask = 1 << object_header_get(index)->object_type;
    if (((type_mask & 1) && ((biped_datum *)object_header_get(index)->datum)->biped.physics.get_mode() == 2)
        || (type_mask & 2)) blocked = true;
    else if ((damage_flags & 2) || (damage_flags & 0x80) || ((damage_flags & 8) && model_damage && (*model_damage & 0x400))) reaction = true;
    else {
        if (object->unit.player_index != -1) blocked = true;
        if (units_aftermath_flag_test(definition->flags, 7) && !(damage_info->flags & 4)) blocked = true;
        if (units_aftermath_flag_test(object->unit.unit_flags, 5)) blocked = true;
    }
    if ((damage_info->flags & 0x4000) && result->body_damage > 0.0f) reaction = true;
    bool ping_reaction = reaction;
    if (killed && !heavy && !airborne && (damage_info->flags & 0x200000) && !ignore_ping
        && !units_aftermath_flag_test(object->unit.unit_flags, 18)) {
        no_ping = false; ping_reaction = false; blocked = true; special = true;
    }
    if (!no_ping) {
        result->ping_type = unit_compute_ping_type(index, killed, heavy, airborne, blocked, ping_reaction, special,
            object->object.current_shield_damage + result->shield_damage, object->object.current_body_damage + result->body_damage);
    }
}


static __forceinline long units_access_get_ultimate_parent(long object_index)
{
    long parent_index = object_index;
    if (object_index != NONE) {
        do {
            object_index = parent_index;
            parent_index = ((unit_datum *)object_header_get(object_index)->datum)->object.parent_object_index;
        } while (parent_index != NONE);
    }
    return object_index;
}

// @address 0xd9cc8 ?unit_can_access_object@@YA_NJJ@Z
bool unit_can_access_object(long unit_index, long object_index)
{
    bool can_access = true;
    if (((1 << object_get_type(unit_index)) & 1) && ((1 << object_get_type(object_index)) & 0xC) != 0) {
        float sink_depth = ((scenario *)global_scenario)->soft_surfaces[0].maximum_sink_depth[4];
        collision_result collision;
        collision.global_material.m_index = NONE;
        real_point3d pill_points[3];
        float pill_height;
        float pill_radius;
        biped_get_physics_pill_from_definition(unit_index, &pill_points[0], &pill_height, &pill_radius);
        real_point3d top_points[3];
        top_points[0] = pill_points[0];
        top_points[0].z = pill_points[0].z + pill_height;
        real_point3d object_points[3];
        bool object_attached;
        if (units_access_get_ultimate_parent(object_index) != object_index) {
            object_get_origin(object_index, &object_points[0]);
            object_attached = true;
        } else {
            object_get_center_of_mass(object_index, &object_points[0]);
            object_attached = false;
        }
        real_point3d raised_point = object_points[0];
        raised_point.z = object_points[0].z + sink_depth;
        if (collision_test_line(k_unit_access_collision_flags, &top_points[0], &object_points[0], unit_index, object_index, &collision) &&
            collision_test_line(k_unit_access_collision_flags, &pill_points[0], &object_points[0], unit_index, object_index, &collision) &&
            (sink_depth == 0.0f || object_attached ||
             collision_test_line(k_unit_access_collision_flags, &top_points[0], &raised_point, unit_index, object_index, &collision))) {
            real_vector3d direction;
            real_vector3d perpendicular;
            vector_from_points3d(&pill_points[0], &object_points[0], &direction);
            cross_product3d(&direction, global_up3d, &perpendicular);
            if (normalize3d(&perpendicular) == 0.0f)
                perpendicular = *global_right3d;
            float pill_offset = 0.075f;
            point_from_line3d(&pill_points[0], &perpendicular, pill_offset, &pill_points[1]);
            point_from_line3d(&pill_points[0], &perpendicular, -pill_offset, &pill_points[2]);
            point_from_line3d(&top_points[0], &perpendicular, pill_offset, &top_points[1]);
            point_from_line3d(&top_points[0], &perpendicular, -pill_offset, &top_points[2]);
            collision_result object_collision;
            object_collision.global_material.m_index = NONE;
            float object_offset = 0.05f;
            s_collision_test_flags object_flags = { 0x24801, 0 };
            point_from_line3d(&object_points[0], &perpendicular, object_offset, &object_points[1]);
            point_from_line3d(&object_points[0], &perpendicular, -object_offset, &object_points[2]);
            if (collision_test_line(object_flags, &object_points[0], &object_points[1], unit_index, object_index, &object_collision))
                object_points[1] = object_collision.point;
            if (collision_test_line(object_flags, &object_points[0], &object_points[2], unit_index, object_index, &object_collision))
                object_points[2] = object_collision.point;
            if (collision_test_line(k_unit_access_collision_flags, &top_points[1], &object_points[1], unit_index, object_index, &collision) &&
                collision_test_line(k_unit_access_collision_flags, &top_points[2], &object_points[2], unit_index, object_index, &collision) &&
                collision_test_line(k_unit_access_collision_flags, &pill_points[1], &object_points[1], unit_index, object_index, &collision) &&
                collision_test_line(k_unit_access_collision_flags, &pill_points[2], &object_points[2], unit_index, object_index, &collision))
                can_access = false;
        }
    }
    return can_access;
}



// @address 0xd9970 ?unit_fix_position@@YA_NJJPBTreal_point3d@@0PAT1@MM_N@Z
bool unit_fix_position(long unit_index, long p1, real_point3d const *point, real_point3d const *point2, real_point3d *output_point,
    float minimum_search_radius, float maximum_search_radius, bool search_everywhere)
{
    bool result = false;
    s_object_header *header = object_header_get(unit_index);
    unit_datum *unit = (unit_datum *)header->datum;
    if (g_havok_monitor_globals.initialized_for_new_map)
    {
        if (unit->object.parent_object_index == NONE)
        {
            bool reset_on_exit = false;
            if (unit->object.havok_component_index == NONE)
            {
                object_rebuild_havok(unit_index);
                reset_on_exit = true;
            }
            if (unit->object.havok_component_index != NONE)
            {
                long component_index = unit->object.havok_component_index;
                int component_slot = component_index & 0xffff;
                c_havok_component *component =
                    &((c_havok_component *)g_havok_component_data->data)[component_slot];
                long body_slot = component->m_object_postion_controlling_rigid_body_index;
                if (body_slot < 0 || body_slot >= component->get_rigid_body_count())
                    body_slot = -1;
                short rigid_body_index = (short)body_slot;
                if (rigid_body_index != NONE)
                {
                    real_point3d origin;
                    real_point3d center;
                    real_vector3d velocity;
                    real_point3d raycast_center;
                    s_location location;
                    const real_point3d *override_point = 0;
                    float search_radius = maximum_search_radius;
                    object_get_origin(unit_index, &origin);
                    object_get_center_of_mass(unit_index, &center);
                    velocity.i = center.x - origin.x;
                    velocity.j = center.y - origin.y;
                    velocity.k = center.z - origin.z;
                    const real_point3d *desired_point = point2 ? point2 : &origin;
                    center = *desired_point;
                    if (point != 0)
                        override_point = point;
                    else if (unit->unit.player_index != NONE && p1 != NONE)
                    {
                        long root = p1;
                        long leaf = p1;
                        do
                        {
                            leaf = root;
                            root = unit_get(root)->object.parent_object_index;
                        } while (root != NONE);
                        if (p1 == leaf)
                        {
                            s_object_header *parent_header = object_header_get(p1);
                            unit_datum *parent_unit = (unit_datum *)parent_header->datum;
                            if (flag_test(parent_unit->object.physics_flags, 7L) && ((1 << parent_header->object_type) & 0x1003) != 0)
                                override_point = object_get_center_of_mass(p1, &raycast_center);
                        }
                    }
                    for (short tries = 0; tries < 3; tries++)
                    {
                        if (component->rigid_body_calculate_fixed_position(rigid_body_index,
                            (header->object_type ? _havok_group_vehicle_physics : _havok_group_biped),
                                &center, &velocity, &origin, &location, minimum_search_radius, search_radius, search_everywhere,
                                override_point, p1))
                        {
                            if (output_point != 0)
                            {
                                *output_point = origin;
                            }
                            result = true;
                            break;
                        }
                        search_radius = search_radius * 2.0f;
                    }
                    if (result)
                    {
                        object_set_position_direct(unit_index, &origin, 0, 0, &location, false);
                        object_set_at_rest(unit_index, false);
                    }
                }
            }
            if (reset_on_exit && (header->flags & 1) == 0)
                object_reset(unit_index);
        }
    }
    else if (point2 != 0)
    {
        object_set_position_direct(unit_index, point2, 0, 0, 0, false);
        if (output_point != 0)
        {
            *output_point = *point2;
        }
    }
    return result;
}

 

static __forceinline bool units_game_simulation_is_predicted()
{
    bool predicted = false;
    if (game_globals->options.game_simulation == 4)
        predicted = true;
    return predicted;
}



// @rdata 0x8203385c
static const float k_units_melee_attack_ray_offset = 0.2f;
static __forceinline bool units_melee_attack_seat_allows_damage(const unit_seat *seat)
{
    return (seat->flags >> 11) & 1;
}
struct s_model_damage_info;
static __forceinline c_global_material_type units_melee_attack_material(short index)
{
    c_global_material_type material;
    material.m_index = index;
    return material;
}
static __forceinline void units_melee_attack_material_initialize(c_global_material_type *material)
{
    material->m_index = NONE;
}
static __forceinline void units_melee_attack_target_initialize(s_unit_player_melee_damage_target *target)
{
    units_melee_attack_material_initialize(&target->material_type);
    memset(target, 0, sizeof(*target));
}
static __forceinline long units_melee_attack_abs(long value)
{
    return value >= 0 ? value : -value;
}
static __forceinline float units_melee_attack_aiming_i_get(const unit_datum *unit)
{
    const unit_datum *const *unit_reference = &unit;
    return (*unit_reference)->unit.aiming_vector.i;
}
static __forceinline const float &units_melee_attack_bounds_k_get(const real_vector3d *bounds)
{
    const real_vector3d *const *bounds_reference = &bounds;
    return (*bounds_reference)->k;
}
static __forceinline void units_melee_attack_copy_point(const real_point3d *source, real_point3d *destination)
{
    *destination = *source;
}
static __forceinline void units_melee_attack_copy_vector(const real_vector3d *source, real_vector3d *destination)
{
    *destination = *source;
}
static __forceinline float units_melee_attack_dot_normal_point(const real_vector3d *normal, const real_point3d *point)
{
    return normal->i * point->x + (normal->j * point->y + normal->k * point->z);
}
static __forceinline unsigned char units_melee_attack_damage_reporting_info_build(unsigned char type, unsigned char modifier)
{
    s_melee_damage_reporting_info info;
    info.fields.type = type;
    info.fields.modifier = modifier;
    return info.value;
}


// @address 0x32b900 ?unit_cause_player_melee_attack@@YAXJJFM@Z
void unit_cause_player_melee_attack(long unit_index, long animation, short mode, float scale)
{
    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    const real_vector3d *up_vector = global_up3d;
    long weapon_index = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index);
    long target_index = NONE;
    short target_type = NONE;
    unsigned char reporting_info = units_melee_attack_damage_reporting_info_build(3, 0);
    long effect_index = NONE;
    long response_effect_index = NONE;
    long clang_effect_index = NONE;
    long breakable_surface_set_index = NONE;
    long clang_response_effect_index = NONE;
    long breakable_surface_index = NONE;
    long breakable_surface_instance_index = NONE;
    long model_region_index = NONE;
    c_global_material_type material = k_global_material_type_invalid;
    long breakable_surface_bsp_index = NONE;
    long breakable_surface_cluster_index = NONE;
    real_point3d position;
    units_melee_attack_copy_point(&unit->object.position, &position);
    real_vector3d normal;
    units_melee_attack_copy_vector(up_vector, &normal);
    long best_distance = 0x7fffffff;
    float best_fraction;
    bool allied = false;
    bool synchronized = false;
    bool clang = false;
    bool play_response = true;
    if (weapon_index != NONE)
    {
        struct weapon_definition *weapon_definition = (struct weapon_definition *)tag_get(0, 
            unit_get(weapon_index)->object.definition_index);
        reporting_info = units_melee_attack_damage_reporting_info_build(weapon_definition->reporting_type, 0);
        real_point3d camera_position;
        unit_get_camera_position(unit_index, &camera_position);
        const real_vector3d *bounds = &weapon_definition->melee_bounds;
        real_vector3d left;
        real_vector3d up;
        normalize3d(perpendicular3d(&unit->unit.aiming_vector, &left));
        cross_product3d(&unit->unit.aiming_vector, &left, &up);
        scale_vector3d(&left, bounds->i / 2.0f, &left);
        scale_vector3d(&up, bounds->j / 2.0f, &up);
        for (long horizontal = -2; horizontal <= 2; horizontal++)
        {
            for (long vertical = -2; vertical <= 2; vertical++)
            {
                collision_result collision;
                collision.global_material.m_index = NONE;
                real_point3d start = camera_position;
                const float *aiming_j = &unit->unit.aiming_vector.j;
                real_vector3d vector;
                const float &left_k = +left.k;
                vector.i = units_melee_attack_bounds_k_get(bounds) * units_melee_attack_aiming_i_get(unit) +
                    (left.i * (float)horizontal + up.i * (float)vertical);
                vector.j = bounds->k * *aiming_j + (left.j * (float)horizontal + up.j * (float)vertical);
                vector.k = bounds->k * unit->unit.aiming_vector.k + (left_k * (float)horizontal + up.k * (float)vertical);
                if (bounds->k > 0.001f)
                {
                    point_from_line3d(&start, &unit->unit.aiming_vector, -k_units_melee_attack_ray_offset, &start);
                    point_from_line3d(&start, &vector, k_units_melee_attack_ray_offset / bounds->k, &start);
                }
                if (unit->object.parent_object_index == NONE && ((1 << definition->object_type) & 1))
                    unit_clamp_point_to_physics_shape(unit_index, &start, (g_havok_constants.minimum_radius));
                if (collision_test_vector(k_projectiles_collision_test_flags, true, &start, &vector, unit_index, NONE, NONE, &collision))
                {
                    long original_hit_index = NONE;
                    long hit_index = NONE;
                    unit_datum *hit_object = 0;
                    long distance = units_melee_attack_abs(horizontal) + units_melee_attack_abs(vertical);
                    if (collision.type == 4)
                    {
                        original_hit_index = collision.object_index;
                        hit_index = original_hit_index;
                        hit_object = unit_get(hit_index);
                        if (hit_object->object.object_type == 2 && hit_object->object.parent_object_index != NONE)
                        {
                            hit_index = hit_object->object.parent_object_index;
                            hit_object = unit_get(hit_index);
                        }
                    }
                    if (hit_index == NONE || !((1 << object_header_get(hit_index)->object_type) & 0x1003))
                    {
                        s_havok_ray_test ray;
                        ray.start = start;
                        ray.vector = vector;
                        if (havok_ray_cast_in_world(&ray, 11, unit->object.havok_component_index, NONE)
                            && ray.object_index != NONE && ((1 << object_header_get(ray.object_index)->object_type) & 0x1003))
                        {
                            hit_index = ray.object_index;
                            collision.object_index = ray.object_index;
                            collision.type = 4;
                            collision.fraction = ray.fraction;
                            float normal_j = ray.normal.j;
                            float normal_k = ray.normal.k;
                            collision.plane.n = ray.normal;
                            point_from_line3d(&ray.start, &ray.vector, ray.fraction, &collision.point);
                            collision.plane.d = ray.normal.i * collision.point.x + (normal_j * collision.point.y + normal_k * collision.point.z);
                            hit_object = unit_get(hit_index);
                        }
                    }
                    switch (collision.type)
                    {
                    case 1:
                    case 3:
                        if (target_index == NONE && distance == 0)
                        {
                            best_distance = 0;
                            breakable_surface_set_index = collision.structure_bsp_index;
                            memcpy(&position, &collision.point, sizeof(position));
                            normal = collision.plane.n;
                            material = units_melee_attack_material(collision.global_material.m_index);
                            if (collision.flags & 8)
                            {
                                breakable_surface_index = collision.breakable_surface;
                                breakable_surface_instance_index = collision.breakable_surface_bit;
                                breakable_surface_bsp_index = collision.instance_index;
                                breakable_surface_cluster_index = collision.surface_index;
                            }
                        }
                        break;
                    case 4:
                    {
                        bool ignore = false;
                        bool hit_allied = false;
                        if ((1 << object_header_get(hit_index)->object_type) & 0x1003)
                        {
                            hit_allied = game_team_is_ally((e_game_team)hit_object->unit.team, (e_game_team)unit->unit.team);
                            if (hit_allied && (unit->unit.actor_index != NONE || horizontal != 0 || vertical != 0))
                                ignore = true;
                        }
                        else
                            ignore = distance > 0;
                        if (!ignore)
                        {
                            short hit_type = hit_object->object.object_type;
                            bool hit_biped = hit_type == 0;
                            bool target_biped = target_type == 0;
                            bool better;
                            if (target_index == NONE)
                                better = true;
                            else if (hit_biped != target_biped)
                                better = hit_biped > target_biped;
                            else
                            {
                                if (distance == best_distance)
                                    better = collision.fraction < best_fraction;
                                else
                                    better = distance < best_distance;
                            }
                            if (better)
                            {
                                breakable_surface_set_index = collision.structure_bsp_index;
                                target_type = hit_type;
                                memcpy(&position, &collision.point, sizeof(position));
                                normal = collision.plane.n;
                                material = units_melee_attack_material(collision.global_material.m_index);
                                target_index = hit_index;
                                model_region_index = hit_index == original_hit_index ? (short)collision.material_index : NONE;
                                best_fraction = collision.fraction;
                                allied = hit_allied;
                                best_distance = distance;
                            }
                        }
                        break;
                    }
                    }
                }
            }
        }
        material = global_material_resolve_underwater_material_proxy(material, &position);
        unit_get_melee_effect_ids(unit_index, animation, &effect_index, &response_effect_index, &clang_effect_index,
            &clang_response_effect_index);
    }
    else if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE)
    {
        unit_seat *seat = units_seat_element_get(
            (unit_definition *)tag_get(0, unit_get(unit->object.parent_object_index)->object.definition_index), unit->unit.parent_seat_index);
        if (units_melee_attack_seat_allows_damage(seat))
        {
            if (animation == 185)
            {
                effect_index = definition->linked_seat_damage.index;
                response_effect_index = definition->linked_seat_response.index;
                if (seat->linked_seat != 0xffff)
                    target_index = unit_seat_get_occupant(unit->object.parent_object_index, seat->linked_seat);
            }
            else
            {
                effect_index = definition->boarding_melee_damage.index;
                response_effect_index = definition->boarding_response.index;
                target_index = unit->object.parent_object_index;
            }
            if (target_index != NONE)
            {
                unit_definition *target_definition = (unit_definition *)tag_get(0, unit_get(target_index)->object.definition_index);
                if (target_definition->model.index != NONE)
                {
                    s_model_definition *model = (s_model_definition *)tag_get(0, target_definition->model.index);
                    if (model->damage_info.count != 0)
                        material = unit_override_damage_info_material_type(unit->object.parent_object_index,
                            ((s_model_damage_info *)model->damage_info.address)->indirect_material_type);
                }
            }
        }
    }
    if (effect_index != NONE && (target_index != NONE || breakable_surface_instance_index != NONE))
    {
        s_unit_player_melee_damage_target target;
        units_melee_attack_target_initialize(&target);
        bool cause_damage = true;
        s_damage_owner owner;
        damage_compute_damage_owner_from_object_index(unit_index, &owner);
        if (target_index != NONE && ((1 << unit_get(target_index)->object.object_type) & 0x1003))
        {
            float shield = unit_get(target_index)->object.shield_vitality;
            if (shield > 0.0f)
            {
                const s_model_damage_info *info = object_get_damage_info(target_index);
                if (info)
                {
                    material = ((const s_model_damage_info *)info)->shield_material_type;
                    scale = shield;
                }
                material = unit_override_damage_info_material_type(target_index, material);
            }
        }
        target.position = position;
        target.normal = normal;
        target.breakable_surface_set_index = breakable_surface_set_index;
        target.object_index = target_index;
        target.breakable_surface_index = (short)breakable_surface_index;
        target.breakable_surface_instance_index = (short)breakable_surface_instance_index;
        target.breakable_surface_bsp_index = breakable_surface_bsp_index;
        target.breakable_surface_cluster_index = breakable_surface_cluster_index;
        target.model_region_index = model_region_index;
        target.material_type = material;
        target.hit_material_flags = reporting_info;
        target.damage_scale = PIN(scale, 0.0f, 1.0f);
        long parent_index = unit->object.parent_object_index;
        target.lunge = false;
        if (parent_index != NONE && unit->unit.parent_seat_index != NONE && allied)
        {
            unit_seat *seat = units_seat_element_get((unit_definition *)tag_get(0, unit_get(parent_index)->object.definition_index),
                unit->unit.parent_seat_index);
            if (!units_melee_attack_seat_allows_damage(seat))
                cause_damage = false;
        }
        unit_validate_synchronized_melee_damage(unit_index, effect_index, clang_effect_index, &owner, &target, &synchronized, &clang);
        if (cause_damage)
        {
            if (mode == 2)
            {
                if (!units_game_simulation_is_predicted())
                    play_response = false;
            }
            else if (mode == 1)
                simulation_action_unit_melee_damage(unit_index, effect_index, clang_effect_index, &target, false);
            else if (!units_game_simulation_is_predicted() && !(unit_get(unit_index)->object.flags_d6 & 2))
                unit_handle_synchronized_melee_damage(unit_index, effect_index, clang_effect_index, &owner, &target, synchronized, clang);
        }
    }
    else if (mode == 1)
        simulation_action_unit_melee_damage(unit_index, NONE, NONE, 0, false);
    if (mode != 2)
        unit_update_melee_action_state(unit_index);
    if (units_melee_get_material(material) && play_response)
        unit_cause_melee_damage_response(unit_index, clang ? clang_effect_index : effect_index,
            clang ? clang_response_effect_index : response_effect_index, target_index, material, &position, &normal);
}

// @address 0x32c598 ?unit_cause_melee_damage_response@@YAXJJJJVc_global_material_type@@PBTreal_point3d@@PBTreal_vector3d@@@Z
void unit_cause_melee_damage_response(long unit_index, long effect_index, long damage_effect_index, long target_index,
    c_global_material_type material, const real_point3d *position, const real_vector3d *direction)
{
    unit_datum *unit = unit_get(unit_index);
    real_point3d melee_position;
    real_vector3d melee_direction;
    if (position)
    {
        melee_position = *position;
        // match: 0x32c598: without this rotate the supplied x coordinate loads into r27 instead of r28.
        effect_index = (int)_rotl(effect_index, 32);
    }
    else
    {
        melee_position = unit->object.bounding_center;
        // match: 0x32c598: without this rotate the TLS base takes r28 and bounding_center.z moves to r25.
        target_index = (int)_rotl(target_index, 32);
    }
    if (direction)
        melee_direction = *direction;
    else
        negate_vector3d(&unit->unit.aiming_vector, &melee_direction);
    e_sweetener_size sweetener_size = _sweetener_size_medium;
    if (target_index != NONE)
        sweetener_size = (e_sweetener_size)(unsigned char)((unit_definition *)tag_get(0, unit_get(target_index)->object.definition_index))->sweetener_size;
    unit_cause_melee_effects(unit_index, effect_index, target_index, sweetener_size, material, &unit->unit.aiming_vector,
        &melee_position, &melee_direction);
    if (damage_effect_index != NONE)
    {
        s_damage_data damage_data;
        if (tag_get_group_tag(damage_effect_index) == 'jpt!' && !units_game_simulation_is_predicted())
        {
            damage_data_new(&damage_data, damage_effect_index);
            damage_data.flags |= 8;
            damage_data.direction = melee_direction;
            damage_data.position = melee_position;
            damage_data.origin = damage_data.position;
            object_cause_damage_simple(&damage_data, unit_index);
        }
        else
        {
            player_effect_start(unit_index, NONE, damage_effect_index, damage_owner_unknown(), &melee_direction, 1.0f, 1.0f, 0, false);
        }
    }
}

// @address 0x32cfe0 unit_cause_self_damage
extern "C" void unit_cause_self_damage(long unit_index, long damage_index, float scale)
{
    unit_datum *unit = unit_get(unit_index);
    unsigned long group_tag = tag_get_group_tag(damage_index);
    if (units_game_simulation_is_predicted() || group_tag == 'drdf')
    {
        s_damage_owner owner = { NONE, NONE, NONE };
        real_vector3d direction = { 0.0f };
        negate_vector3d(&unit->unit.aiming_vector, &direction);
        player_effect_start(unit_index, NONE, damage_index, &owner, &direction, scale, scale, 0, false);
    }
    else if (!units_game_simulation_is_predicted() && group_tag == 'jpt!')
    {
        s_damage_data damage_data;
        damage_data_new(&damage_data, damage_index);
        damage_data.flags |= 8;
        negate_vector3d(&unit->unit.aiming_vector, &damage_data.direction);
        damage_data.position = unit->object.bounding_center;
        damage_data.origin = damage_data.position;
        damage_data.scale[0] = scale;
        object_cause_damage_simple(&damage_data, unit_index);
    }
}
// @address 0xda138 ?vehicle_update@@YA_NJ@Z
bool vehicle_update(long vehicle_index)
{
    vehicle_datum *unit = (vehicle_datum *)unit_get(vehicle_index);
    vehicle_definition *definition = (vehicle_definition *)tag_get(0, unit->object.definition_index);
    float boost = unit_compute_boost_fraction(vehicle_index);
    bool physics_disabled = vehicle_physics_disabled(vehicle_index);
    if (unit->object.parent_object_index == NONE && unit_vehicle_is_flying(vehicle_index) != 0)
        object_update_soft_ceiling(vehicle_index, &unit->vehicle.last_position);
    if (physics_disabled)
        ((c_vehicle_type_component *)((vehicle_datum *)unit_get(vehicle_index))->vehicle.type_state)->deplete_function_variables(vehicle_index);
    real_point3d *center_of_mass = &unit->vehicle.pathfinding_point;
    unit->vehicle.pathfinding_structure_index = NONE;
    unit->vehicle.pathfinding_surface_index = NONE;
    unit->vehicle.pathfinding_instanced_geometry_index = NONE;
    unit->vehicle.pathfinding_object_index = NONE;
    unit->vehicle.pathfinding_bsp_reference = 0xffffffff;
    unit->vehicle.pathfinding_sector.structure_bsp_index = NONE;
    unit->vehicle.pathfinding_sector.sector_index = 0xffff;
    object_get_center_of_mass(vehicle_index, center_of_mass);
    if (FLAG_TEST(unit->object.flags, 8))
        object_in_early_mover_physics_update(vehicle_index);
    if (boost != 0.0f)
    {
        c_multi_unit_passenger_iterator iterator;
        iterator.begin(vehicle_index);
        if (iterator.next())
        {
            do
            {
                long passenger_index = iterator.m_passenger_index;
                vehicle_datum *passenger = (vehicle_datum *)unit_get(passenger_index);
                if (passenger->unit.player_index != NONE)
                {
                    long damage_index;
                    if (unit_seat_test_flag(passenger_index, passenger->unit.parent_seat_index, _unit_seat_driver_bit))
                        damage_index = definition->boost_rider_damage.index;
                    else
                        damage_index = definition->boost_rider_response.index;
                    if (damage_index != NONE)
                        unit_cause_self_damage(passenger_index, damage_index, boost);
                }
            } while (iterator.next());
        }
    }
    return false;
}


static __forceinline unsigned char units_try_use_equipment(long unit_index)
{
    long equipment_index = unit_get(unit_index)->unit.equipment_object_index;
    unsigned char result = false;
    if (equipment_index != NONE)
        result = equipment_activate(equipment_index, unit_index, 0);
    return result;
}

// @address 0x566488 unit_actor_try_use_equipment
extern "C" unsigned char unit_actor_try_use_equipment(long actor_index, long unit_index)
{
    unsigned char result = false;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->unit_index == unit_index && actor->unit_index != NONE) {
        result = units_try_use_equipment(actor->unit_index);
    }
    return result;
}

// @address 0x32abd8 ?unit_cause_ai_melee_attack@@YAXJF@Z
void unit_cause_ai_melee_attack(long unit_index, short melee_type)
{
    if (game_simulation::game_is_predicted())
        return;

    unit_datum *unit = unit_get(unit_index);
    unit_definition *definition = (unit_definition *)tag_get(0, unit->object.definition_index);
    long damage_definition_index;
    switch (melee_type)
    {
    case 1:
        damage_definition_index = definition->landing_melee_damage.index;
        break;
    case 2:
        damage_definition_index = definition->flurry_melee_damage.index;
        break;
    default:
        damage_definition_index = definition->melee_damage.index;
        if (unit->unit.actor_index != NONE)
        {
            character_weapon_properties *properties = actor_weapon_properties_get(unit->unit.actor_index);
            if (properties && properties->melee_damage != NONE)
                damage_definition_index = properties->melee_damage;
        }
        if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE)
        {
            unit_definition *parent_definition = (unit_definition *)tag_get(0,
                unit_get(unit->object.parent_object_index)->object.definition_index);
            if (FLAG_TEST(parent_definition->seats[unit->unit.parent_seat_index].flags, 11))
                damage_definition_index = definition->boarding_melee_damage.index;
        }
        else
        {
            long weapon_index = unit_inventory_try_get_weapon(unit_index, unit->unit.current_weapon_index);
            if (weapon_index != NONE)
            {
                struct weapon_definition *weapon_definition =
                    (struct weapon_definition *)tag_get(0, unit_get(weapon_index)->object.definition_index);
                if (FLAG_TEST(weapon_definition->flags, 15))
                    damage_definition_index = weapon_definition->melee_damage;
            }
        }
        break;
    }
    if (damage_definition_index == NONE)
        return;

    real_vector3d forward = unit->object.forward;
    real_vector3d backward;
    negate_vector3d(&forward, &backward);
    real_point3d position;
    switch (melee_type)
    {
    case 1:
        object_get_origin(unit_index, &position);
        point_from_line3d(&position, global_up3d, 0.1f, &position);
        break;
    case 0:
    {
        object_marker marker;
        if (internal_object_get_markers_by_string_id(unit_index, 10, &marker, 1, false) == 1)
        {
            collision_result collision;
            collision.global_material.m_index = NONE;
            position = marker.matrix.position;
            forward = marker.matrix.forward;
            if (collision_test_line(k_ai_melee_collision_flags, &unit->object.bounding_center, &position, unit_index, NONE, &collision))
                position = unit->object.bounding_center;
            break;
        }
    }
    default:
        position = unit->object.bounding_center;
        break;
    }

    c_animation_channel *channel = motor_animation_resource_get_channel(unit_index, _motor_resource_base);
    long attack_time = game_time_get() - game_seconds_to_ticks_round(channel->get_authored_time());

    s_damage_data damage;
    damage_data_new(&damage, damage_definition_index);
    object_get_location(unit_index, &damage.location);
    damage.reporting_info = 12;
    damage_compute_damage_owner_from_object_index(unit_index, &damage.owner);
    damage.origin = position;
    damage.position = unit->object.bounding_center;
    damage.object_index = unit_index;
    damage.cone_direction = forward;
    damage.damage_identifier = (attack_time & 0xffff) | (unit_index & 0xffff0000);
    damage.flags |= 0x10000;
    long object_index = area_of_effect_cause_damage(&damage, NONE, unit_index);
    if (object_index != NONE)
        damage.material_type = object_get_default_material(object_index);
    if (units_melee_get_material(damage.material_type))
        unit_cause_melee_effects(unit_index, damage_definition_index, NONE, _sweetener_size_small, damage.material_type, &forward,
            &position, &backward);
}

// 0x329390 natural source

static __forceinline void *units_pickup_object_get_and_verify_type(long object_index, unsigned long type_mask)
{
    (void)type_mask;
    return unit_get(object_index);
}
static __forceinline void *units_pickup_tag_get(unsigned long group_tag, long tag_index)
{
    (void)group_tag;
    return tag_get(0, tag_index);
}
static __forceinline struct weapon_definition *units_pickup_weapon_get_definition(long weapon_index)
{
    weapon_datum *weapon = (weapon_datum *)units_pickup_object_get_and_verify_type(weapon_index, _units_pickup_object_mask_weapon);
    return (struct weapon_definition *)units_pickup_tag_get(_units_pickup_weapon_tag, weapon->definition_index);
}
static __forceinline bool units_pickup_weapon_must_be_readied(long weapon_index)
{
    struct weapon_definition *definition = units_pickup_weapon_get_definition(weapon_index);
    bool result = definition->m_flags.test(_weapon_definition_must_be_readied);
    return result;
}
static __forceinline bool units_pickup_weapon_can_be_dual_wielded(long weapon_index)
{
    struct weapon_definition *definition = units_pickup_weapon_get_definition(weapon_index);
    bool result = definition->m_flags.test(_weapon_definition_can_be_dual_wielded)
        || definition->m_flags.test(_weapon_definition_can_only_be_dual_wielded);
    return result;
}
static __forceinline bool units_pickup_weapon_can_only_be_dual_wielded(long weapon_index)
{
    struct weapon_definition *definition = units_pickup_weapon_get_definition(weapon_index);
    bool result = definition->m_flags.test(_weapon_definition_can_only_be_dual_wielded);
    return result;
}
static __forceinline bool units_pickup_weapon_is_age_based(long weapon_index)
{
    weapon_datum *weapon = (weapon_datum *)units_pickup_object_get_and_verify_type(weapon_index, _units_pickup_object_mask_weapon);
    return weapon_is_age_based(weapon->definition_index);
}
static __forceinline float units_pickup_weapon_get_age(long weapon_index)
{
    weapon_datum *weapon = (weapon_datum *)units_pickup_object_get_and_verify_type(weapon_index, _units_pickup_object_mask_weapon);
    return weapon->age;
}
static __forceinline bool units_pickup_game_is_multiplayer()
{
    bool result = game_globals->options.game_mode == 2;
    return result;
}
static __forceinline c_game_engine *units_pickup_current_game_engine()
{
    s_game_engine_globals *globals = game_engine_globals;
    if (globals->game_engine_index >= 1 && globals->game_engine_index <= 10)
        return game_engines[globals->game_engine_index];
    return 0;
}
static __forceinline bool units_pickup_game_engine_allow_weapon_pickup(long player_index, long weapon_index)
{
    bool result = true;
    c_game_engine *engine = units_pickup_current_game_engine();
    if (engine)
        result = engine->allow_weapon_pickup(player_index, weapon_index);
    return result;
}

// @address 0x329390 ?unit_can_pickup_weapon@@YA_NJJW4e_weapon_addition_method@@PAUunit_weapon_pickup_result@@@Z
// match: 0x329390: pointer __restrict qualifier prevents alias reload
bool unit_can_pickup_weapon(long unit_index, long weapon_index, e_weapon_addition_method mode, unit_weapon_pickup_result *__restrict result)
{
    bool can_pickup = false;
    if (unit_can_use_weapon_in_seat(unit_index, NONE, weapon_index))
    {
        unit_datum *unit = (unit_datum *)units_pickup_object_get_and_verify_type(unit_index, _units_pickup_object_mask_unit);
        weapon_datum *weapon = (weapon_datum *)units_pickup_object_get_and_verify_type(
            weapon_index, _units_pickup_object_mask_weapon);
        struct weapon_definition *weapon_definition = (struct weapon_definition *)units_pickup_tag_get(
            _units_pickup_weapon_tag, weapon->definition_index);
        long matching_inventory_index = NONE;
        bool weapon_is_empty = false;
        bool has_matching_rounds = false;
        long matching_rounds = 0;
        bool can_be_dual_wielded = false;
        bool can_only_be_dual_wielded = false;
        bool magazine_is_empty = false;
        bool magazine_has_rounds = false;
        bool infinite_ammo = unit->unit.player_index != NONE && units_pickup_game_is_multiplayer()
            ? game_engine_player_grants_infinite_ammo(unit->unit.player_index)
            : false;
        for (long magazine_index = 0; magazine_index < weapon_definition->magazines.count(); magazine_index++)
        {
            s_weapon_magazine_definition *magazine = (s_weapon_magazine_definition *)tag_block_get_element_with_size(
                &weapon_definition->magazines, magazine_index, sizeof(s_weapon_magazine_definition));
            if (magazine->rounds_total_maximum > 0)
            {
                if (!infinite_ammo && weapon->magazines[magazine_index].rounds_loaded == 0
                    && weapon->magazines[magazine_index].rounds_inventory == 0)
                    magazine_is_empty = true;
                else
                    magazine_has_rounds = true;
            }
        }
        if (magazine_is_empty && !magazine_has_rounds)
            weapon_is_empty = true;
        can_be_dual_wielded = units_pickup_weapon_can_be_dual_wielded(weapon_index);
        can_only_be_dual_wielded = units_pickup_weapon_can_only_be_dual_wielded(weapon_index);
        for (long inventory_index = 0; inventory_index < 4; inventory_index++)
        {
            if (unit->unit.weapon_object_indices[inventory_index] != NONE)
            {
                weapon_datum *inventory_weapon = (weapon_datum *)units_pickup_object_get_and_verify_type(
                    unit->unit.weapon_object_indices[inventory_index], _units_pickup_object_mask_weapon);
                if (weapon->definition_index == inventory_weapon->definition_index ||
                    weapon_definition->ammunition_weapon_definition_index == inventory_weapon->definition_index)
                {
                    if (matching_inventory_index == NONE)
                        matching_inventory_index = inventory_index;
                    for (long magazine_index = 0; magazine_index < weapon_definition->magazines.count(); magazine_index++)
                    {
                        long rounds = weapon_get_rounds_inventory(unit->unit.weapon_object_indices[inventory_index], magazine_index, true);
                        if (rounds > 0)
                        {
                            has_matching_rounds = true;
                            matching_rounds += rounds;
                        }
                    }
                }
            }
        }

        memset(result, 0, sizeof(*result));
        long current_weapon_indices[2];
        for (long weapon_set_index = 0; weapon_set_index < 2; weapon_set_index++)
        {
            result->primary[weapon_set_index] = true;
            result->backpack[weapon_set_index] = true;
            current_weapon_indices[weapon_set_index] = unit_inventory_try_get_weapon(unit_index,
                ((unit_datum *)units_pickup_object_get_and_verify_type(unit_index, _units_pickup_object_mask_unit))
                    ->unit.current_weapon_indices[weapon_set_index]);
            if (current_weapon_indices[weapon_set_index] != NONE)
                (void)units_pickup_weapon_must_be_readied(current_weapon_indices[weapon_set_index]);
            else
                result->backpack[weapon_set_index] = false;
        }

        bool other_hand_occupied[2];
        memset(other_hand_occupied, 0, sizeof(other_hand_occupied));
        if (can_be_dual_wielded)
        {
            if (current_weapon_indices[0] == NONE || !units_pickup_weapon_can_be_dual_wielded(current_weapon_indices[0]))
            {
                result->primary[1] = false;
                result->backpack[1] = false;
            }
            if (can_only_be_dual_wielded)
            {
                result->primary[0] = false;
                result->backpack[0] = false;
            }
            other_hand_occupied[0] = current_weapon_indices[1] != NONE;
            other_hand_occupied[1] = current_weapon_indices[0] != NONE;
        }
        else
        {
            result->primary[1] = false;
            result->backpack[1] = false;
        }

        if (matching_inventory_index != NONE)
        {
            for (long weapon_set_index = 0; weapon_set_index < 2; weapon_set_index++)
            {
                if (!other_hand_occupied[weapon_set_index])
                    result->primary[weapon_set_index] = false;
                if (result->backpack[weapon_set_index])
                {
                    weapon_datum *current_weapon = (weapon_datum *)units_pickup_object_get_and_verify_type(
                        current_weapon_indices[weapon_set_index], _units_pickup_object_mask_weapon);
                    if ((weapon->definition_index != current_weapon->definition_index
                        && weapon_definition->ammunition_weapon_definition_index != current_weapon->definition_index
                        || weapon->age >= current_weapon->age) &&
                        (weapon->definition_index == current_weapon->definition_index
                            || weapon_definition->ammunition_weapon_definition_index == current_weapon->definition_index
                            || !other_hand_occupied[weapon_set_index]))
                    {
                        result->backpack[weapon_set_index] = false;
                        result->primary[weapon_set_index] = false;
                    }
                }
            }
        }

        if (weapon_is_empty)
        {
            for (long weapon_set_index = 0; weapon_set_index < 2; weapon_set_index++)
            {
                if (!has_matching_rounds)
                {
                    other_hand_occupied[weapon_set_index] = false;
                }
                else if (other_hand_occupied[weapon_set_index] && current_weapon_indices[weapon_set_index] != NONE)
                {
                    weapon_datum *current_weapon = (weapon_datum *)units_pickup_object_get_and_verify_type(
                        current_weapon_indices[weapon_set_index], _units_pickup_object_mask_weapon);
                    struct weapon_definition *current_definition = (struct weapon_definition *)units_pickup_tag_get(
                        _units_pickup_weapon_tag, current_weapon->definition_index);
                    if (current_weapon->definition_index == weapon->definition_index)
                    {
                        long rounds = 0;
                        for (long magazine_index = 0; magazine_index < current_definition->magazines.count(); magazine_index++)
                            rounds += weapon_get_rounds_inventory(current_weapon_indices[weapon_set_index], magazine_index, true);
                        if (rounds == matching_rounds)
                            other_hand_occupied[weapon_set_index] = false;
                    }
                }
                if (!other_hand_occupied[weapon_set_index])
                {
                    result->primary[weapon_set_index] = false;
                    result->backpack[weapon_set_index] = false;
                }
            }
        }

        if (unit->unit.actor_index != NONE)
        {
            if (units_pickup_weapon_is_age_based(weapon_index) && units_pickup_weapon_get_age(weapon_index) == 1.0f || weapon_is_empty)
            {
                result->primary[0] = false;
                result->primary[1] = false;
                result->backpack[0] = false;
                result->backpack[1] = false;
            }
        }

        if (units_pickup_weapon_must_be_readied(weapon_index) && !units_pickup_weapon_can_be_dual_wielded(weapon_index) &&
            unit_get_current_backpack_weapon(unit_index) == NONE)
        {
            result->backpack[0] = false;
            result->backpack[1] = false;
        }

        if (unit->unit.player_index != NONE)
            player_approve_pickup_weapon(unit->unit.player_index, weapon_index, result);

        for (long weapon_set_index = 0; weapon_set_index < 2; weapon_set_index++)
        {
            if (result->primary[weapon_set_index] || result->backpack[weapon_set_index])
            {
                can_pickup = true;
                break;
            }
        }

        if (can_pickup && unit->unit.player_index != NONE && mode != 1)
            can_pickup = units_pickup_game_engine_allow_weapon_pickup(unit->unit.player_index, weapon_index);
    }
    return can_pickup;
}


static __forceinline void units_swap_weapons_begin(long unit_index)
{
    if (unit_index != NONE)
        first_person_weapon_suspend_weapon_attachment(unit_index, true);
}
static __forceinline void units_swap_weapons_end(long unit_index)
{
    if (unit_index != NONE)
        first_person_weapon_suspend_weapon_attachment(unit_index, false);
}
static __forceinline short units_inventory_get_free_index(long unit_index)
{
    unit_datum *unit = unit_get(unit_index);
    short inventory_index = NONE;
    for (long index = 0; index < 4; index++)
    {
        long weapon_index = unit->unit.weapon_object_indices[index];
        if (weapon_index == NONE)
        {
            inventory_index = (short)index;
            break;
        }
    }
    return inventory_index;
}
static __forceinline void unit_generate_pickup_weapon_event(long unit_index, long weapon_definition_index)
{
    if (unit_index != NONE && weapon_definition_index != NONE)
    {
        unit_datum *unit = unit_get(unit_index);
        if (unit->unit.player_index != NONE)
        {
            long references[1];
            references[0] = unit_index;
            s_pickup_weapon_event event_data;
            event_data.type = 0;
            event_data.weapon.definition_index = weapon_definition_index;
            event_data.type = 0;
            simulation_event_generate_exclusive(_simulation_event_type_unit_pickup, 1, references, unit->unit.player_index, sizeof(event_data),
                &event_data, g_network_configuration.event_relevance[_simulation_event_type_unit_pickup].delay_msec);
        }
    }
}

// @address 0x328908 ?unit_add_weapon_to_inventory@@YA_NJJW4e_weapon_addition_method@@@Z
bool unit_add_weapon_to_inventory(long unit_index, long object_index, e_weapon_addition_method addition_method)
{
    item_datum *weapon = (item_datum *)unit_get(object_index);
    unit_datum *unit = unit_get(unit_index);
    bool result = false;
    action_request backpack_request;
    action_request dual_wield_request;
    action_request primary_request;
    action_request secondary_request;
    if (!FLAG_TEST(weapon->object.object_flags, 7) && weapon->owner_relation == 0 && unit_can_use_weapon_in_seat(unit_index, NONE, object_index))
    {
        bool success = true;
        units_swap_weapons_begin(unit_index);
        if (addition_method == 2)
        {
            unit_delete_all_weapons(unit_index);
        }
        else if (addition_method == 3 || addition_method == 4)
        {
            long secondary_weapon_index = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.desired_weapon_index);
            if (secondary_weapon_index != NONE && (!weapon_can_be_dual_wielded(object_index)
                || !weapon_can_be_dual_wielded(secondary_weapon_index)))
            {
                bool put_away = false;
                unit_weapon_pickup_result pickup_result;
                if (unit_can_pickup_weapon(unit_index, secondary_weapon_index, addition_method, &pickup_result) && pickup_result.primary[1])
                    put_away = unit_put_away_weapon(unit_index, 1, true);
                if (!put_away)
                    action_submit_event(unit_index, 47);
            }
            if (success && addition_method == 4)
            {
                if (unit_get_current_backpack_weapon(unit_index) != NONE)
                {
                    memset(&backpack_request, 0, sizeof(backpack_request));
                    units_action_request_new(&backpack_request, 46);
                    backpack_request.evict.flag = true;
                    success = action_submit(unit_index, &backpack_request);
                }
                else if (secondary_weapon_index != NONE)
                {
                    success = unit_put_away_weapon(unit_index, 0, true);
                }
                else
                {
                    success = action_submit_event(unit_index, 46);
                }
            }
        }
        else if (addition_method == 5 || addition_method == 6 || addition_method == 7)
        {
            long primary_weapon_index = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index);
            success = primary_weapon_index != NONE && weapon_can_be_dual_wielded(primary_weapon_index)
                && weapon_can_be_dual_wielded(object_index);
            if (success && addition_method == 6)
            {
                memset(&dual_wield_request, 0, sizeof(dual_wield_request));
                units_action_request_new(&dual_wield_request, 47);
                dual_wield_request.evict.flag = true;
                success = action_submit(unit_index, &dual_wield_request);
            }
        }
        if (success && addition_method == 3)
        {
            long primary_weapon_index = unit_inventory_try_get_weapon(unit_index, unit_get(unit_index)->unit.current_weapon_index);
            if (primary_weapon_index != NONE && weapon_is_support_weapon(primary_weapon_index))
                success = action_submit_event(unit_index, 46);
        }
        if (success)
        {
            short weapon_set_index = 0;
            if (addition_method == 5 || addition_method == 6 || addition_method == 7)
                weapon_set_index = 1;
            else
                weapon_set_index = 0;
            unit_weapon_pickup_result pickup_result;
            success = success && unit_can_pickup_weapon(unit_index, object_index, addition_method, &pickup_result);
            success = success && pickup_result.primary[weapon_set_index];
        }
        short inventory_index;
        if (success)
        {
            inventory_index = units_inventory_get_free_index(unit_index);
            success = success && inventory_index != NONE;
        }
        if (success)
        {
            if (weapon->object.parent_object_index != NONE)
                object_detach(object_index);
            unit_inventory_set_weapon_index(unit_index, inventory_index, object_index, (e_unit_drop_type)NONE);
            switch (addition_method)
            {
            case 0:
            case 1:
                unit->unit.desired_weapon_indices[0] = (signed char)unit_weapon_next_index(unit_index, NONE, unit->unit.current_weapon_index, 0);
                if (addition_method == 1 && unit->unit.desired_weapon_indices[0] == inventory_index)
                {
                    memset(&primary_request, 0, sizeof(primary_request));
                    units_action_request_new(&primary_request, 54);
                    primary_request.immediate = true;
                    primary_request.force = true;
                    action_submit(unit_index, &primary_request);
                }
                else if (addition_method == 1)
                {
                    item_show_inventory_unattached(object_index, unit_index);
                    weapon_stow(object_index, unit_index);
                }
                break;
            case 2:
            case 3:
            case 4:
                unit->unit.desired_weapon_indices[0] = (signed char)inventory_index;
                action_submit_event(unit_index, 54);
                break;
            case 5:
            case 6:
                unit->unit.desired_weapon_indices[1] = (signed char)inventory_index;
                action_submit_event(unit_index, 55);
                break;
            case 7:
            {
                unit->unit.desired_weapon_indices[1] = (signed char)inventory_index;
                memset(&secondary_request, 0, sizeof(secondary_request));
                units_action_request_new(&secondary_request, 55);
                secondary_request.immediate = true;
                secondary_request.force = true;
                action_submit(unit_index, &secondary_request);
                break;
            }
            default:
                // match: 0x328908: unreachable hint omits dead branch check
                __assume(0);
            }
            if (unit->unit.player_index != NONE)
                units_player_handle_weapon_put_away(unit->unit.player_index, inventory_index);
            if (addition_method != 1 && addition_method != 7)
                unit_generate_pickup_weapon_event(unit_index, weapon->object.definition_index);
            result = true;
        }
        units_swap_weapons_end(unit_index);
    }
    return result;
}


// match: noalias declaration preserves caller live registers

// @rdata 0x8202a1a4
static const float units_toss_maximum_speed = 4.0f;

static __forceinline long units_toss_accepted_physics_object(long object_index)
{
    long result = NONE;
    unit_datum *object = unit_get(object_index);
    unsigned long flags = object->object.physics_flags;
    if ((bool)((flags >> 3) & 1) && ((bool)((flags >> 1) & 1) || (bool)((flags >> 2) & 1)))
        result = object->object.local_physics_object_index;
    return result;
}

static __forceinline void units_toss_clear_early_mover_flag(unsigned long *flags)
{
    *flags &= ~0x400;
}

// @address 0x32a300
void unit_toss_item(long unit_index, long object_index, long marker_name, float magnitude, bool randomize)
{
    unit_datum *unit = unit_get(unit_index);
    item_datum *item = (item_datum *)unit_get(object_index);
    object_marker marker;
    if (marker_name == NONE || marker_name == 0)
        marker_name = internal_object_get_markers_by_string_id(unit_index, 37, &marker, 1, false) ? 37 : 820;
    object_attach_to_marker_with_type(unit_index, marker_name, object_index, 0);
    object_detach(object_index);
    if (randomize && (bool)((unit->object.damage_flags >> 2) & 1) && ((1 << item->object.object_type) & 4))
        object_index = weapon_replace_with_replacement(object_index);
    item_maintain_z_up(object_index);
    real_vector3d unit_velocity;
    real_point3d center_of_mass;
    real_vector3d early_mover_velocity;
    real_vector3d early_mover_angular_velocity;
    object_get_velocities(unit_index, &unit_velocity, 0);
    object_get_center_of_mass(object_index, &center_of_mass);
    bool early_mover = object_get_early_mover_local_space_velocity(
        object_index, &center_of_mass, &early_mover_velocity, &early_mover_angular_velocity, true, false);
    if (early_mover)
        subtract_vectors3d(&unit_velocity, &early_mover_velocity, &unit_velocity);
    // match: 0x32a300: removing this calculation swaps f9 and f28 in the fmadds at 0x32a564.
    float velocity_squared = magnitude_squared3d(&unit_velocity);
    real_vector3d toss_velocity;
    float random_magnitude;
    if (randomize)
    {
        random_magnitude = _real_random_range(TLS_RANDOM_SEED, 0.8f, 1.2f);
        _random_vector_in_cone3d(TLS_RANDOM_SEED, "item-toss-vector", "..\\units\\units.cpp", 9648, &unit->unit.aiming_vector, 0.0f,
            0.3926991f, &toss_velocity);
        scale_vector3d(&toss_velocity, random_magnitude, &toss_velocity);
    }
    else
    {
        random_magnitude = 1.0f;
        scale_vector3d(&unit->unit.aiming_vector, random_magnitude, &toss_velocity);
    }
    scale_vector3d(&unit->unit.aiming_vector, dot_product3d(&unit_velocity, &unit->unit.aiming_vector), &unit_velocity);
    add_vectors3d(&toss_velocity, &unit_velocity, &toss_velocity);
    if (units_toss_accepted_physics_object(unit_index) == NONE)
        limit3d(&toss_velocity, units_toss_maximum_speed);
    item->toss_unit_index = unit_index;
    units_toss_clear_early_mover_flag(&item->object.physics_flags);
    if (early_mover)
        add_vectors3d(&toss_velocity, &early_mover_velocity, &toss_velocity);
    object_set_velocities(object_index, global_zero_vector3d, global_zero_vector3d);
    if (!((((item_datum *)unit_get(object_index))->inventory_flags >> 2) & 1))
        object_apply_acceleration(object_index, NONE, 0, &toss_velocity, 0);
    if (!simple_fix_havok_object_position(object_index, unit_index, 0.0f, _havok_group_vehicle) &&
        !simulation_query_object_is_predicted(object_index) && game_globals->options.game_mode == 1)
        object_delete(object_index);
}
