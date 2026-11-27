// networkinglibxbox/simulation_game_action.obj: from the records 0xa8988 0xf4228 0xf6098 0xfcc10 0x3a9430 0x3a94c8 0x3a9548 0x3a9638 0x3a96c0 0x3a97c0 0x3a9880 0x3a9970 0x3a9a60 0x3a9b20 0x3a9c10 0x3a9cb0 0x3a9dd0 0x3a9ed0 0x3a9fd8 0x3aa0a0 0x3aa190 0x3aa2c0 0x3aa418 0x3aa4f0 0x3aa5d0 0x3aa6a0 0x3aa7f8 0x3aa930 0x3aaa10 0x3aae90 0x3aafc0 0x3ab0a8 0x3ab290 0x3ab458 0x3ab750 0x3ab838 0x3ab8d0 0x3ab968 0x3aba70 0x3abb70 0x3abc40 0x3abd38 0x3abe70 0x3abf38 0x3ac000 0x3ac110 0x3ac1a0 0x3ac280 0x3ac360 0x3ac530 0x3ac700 0x3ac7d8 0x3ac8e8 0x3ac9d8 0x3acaf0 0x3acc08 0x3acca8 0x3acd80 0x3ace30 0x3acf00 0x3acff8 0x3ad0a0 0x3ad178
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "simulation/simulation_distributed_world_types.h"
#include "items/weapon_fire_types.h"
#include "math/real_constant_pool_interpolation.h"
#include "simulation/game_interface/simulation_game_weapons.h"
#include "simulation/game_interface/simulation_game_projectiles.h"
#include "simulation/simulation_queue_event_types.h"
#include "game/game.h"
#include "memory/data.h"
#include "simulation/simulation_node_space_encoding_types.h"
#include "simulation/simulation_entity_types.h"
#include "objects/object_reference_types.h"
#include "simulation/simulation_globals.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "game/player_types.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "items/item_types.h"
#include "items/weapon_definitions.h"
#include "ai/weapon_datum_types.h"
#include "game/game_simulation_predicates.h"
#include "simulation/simulation_damage_aftermath_types.h"
#include "objects/damage_section_response_types.h"
#include "objects/object_types.h"
#include "objects/object_placement.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "units/unit_types.h"
#include "units/unit_inventory_inline.h"
#include "game/game_engine_simulation.h"
#include "items/weapons.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/game_interface/simulation_game_interface.h"
#include "simulation/game_interface/simulation_game_objects.h"
#include "simulation/simulation_gamestate_entities.h"
#include "units/vehicles.h"
#include "game/game_engine_sandbox.h"
#include "math/matrix_math.h"
#include "physics/collision_types.h"
#include "game/game_engine_event_data.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_events.h"
#include "simulation/simulation_event_handler_types.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ppcintrinsics.h>
#include "memory/tls_data_arrays.h"
#include "units/biped_types.h"
#include "game/game_engine_scoring_globals.h"
#include "simulation/simulation_game_action_services.h"
#include "simulation/simulation_world_types.h"
#include "structures/structure_bsp_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/scenario_tag.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "game/materials.h"
#include "simulation/game_interface/simulation_game_engine_player_types.h"
#include "objects/object_definitions.h"
#include "objects/damage_effect_definitions.h"
#include "objects/damage_aftermath_types.h"
#include "networking/replication/replication_motion.h"
#include "simulation/game_interface/simulation_game_units_melee_types.h"
#include "cseries/cseries.h"
#include "memory/datum_index.h"
#include "objects/object_node_matrices_inline.h"

// @rdata 0x8202490c pooled
static const float k_object_orientation_threshold = 0.0001f;
// @rdata 0x820271bc pooled
static const float k_object_orientation_fraction = 0.35f;
// @rdata 0x820271b8 pooled
static const float k_object_position_fraction = 0.45f;

struct s_damage_section_response_data
{
    long section_index;
    struct s_response
    {
        long response_index;
        e_damage_section_response_type type;
        __forceinline s_response() { memset(this, 0, sizeof(*this)); }
    } response;
};

struct s_projectile_hit { signed char type; unsigned char index; };

struct s_simulation_pickup_ammo_event_data
{
    short pickup_type;
    long definition_index;
    short amount;
};

struct s_simulation_pickup_item_definition
{
    short type;
};

struct s_pickup_equipment_data
{
    short type;
    struct s_equipment
    {
        long definition_index;
        long unknown;
        __forceinline s_equipment() { memset(this, 0, sizeof(*this)); }
    } equipment;
};

struct s_exit_vehicle_data { long seat_index; unsigned long flags; };

struct s_weapon_effect_event_data
{
    long type;
    real_point3d position;
    __forceinline s_weapon_effect_event_data() { memset(&position, 0, sizeof(position)); }
};

struct s_pickup_event_payload
{
    short type;
    long value;
    long extra;
};

struct s_simulation_grenade_initiate_event_data
{
    unsigned short type;
    long grenade_type;
};

struct s_game_action_melee_event_data
{
    long type;
    real_point3d position;
    real_vector3d direction;
};

struct s_simulation_reload_event_data
{
    unsigned char type;
    bool successful;
    bool empty;
};

struct s_empty_click_event_data
{
    long unit_inventory_index;
    long definition_index;
};

struct s_put_away_weapon_event_data
{
    short set_index;
    short weapon_index;
    long definition_index;
};

struct s_game_action_grenade_release_data
{
    short hand;
    long grenade_type;
    real_point3d origin;
    real_vector3d direction;
    bool has_target;
    long entity_gamestate_index;
    long entity_index;
    long node_index;
    real_point3d position;
};


typedef char simulation_breakable_surface_key_count_offset[(offsetof(structure_bsp,
    structure_physics.breakable_surface_key_table) == 0x2bc) ? 1 : -1];

struct s_game_engine_event_data;
struct s_aim_assist_targeting_result;
struct s_player_predicted_aim_assist;
enum e_player_editor_request_type;
union real_point3d;
// @address 0xa8988 ?simulation_action_object_update@@YAXJK@Z
// @address 0xfcc10 ?simulation_action_object_delete@@YAXJ@Z
// @address 0x3a94c8 ?simulation_query_local_predictive_control@@YA_NJJ@Z
// @address 0x3a9548 ?simulation_query_predictive_control@@YA_NJJPAJ@Z
// @address 0x3a97c0 ?simulation_action_game_engine_globals_update@@YAXK@Z
// @address 0x3a9880 ?simulation_action_game_engine_globals_delete@@YAXXZ
// @address 0x3a9a60 ?simulation_action_game_statborg_update@@YAXK@Z
// @address 0x3a9b20 ?simulation_action_game_statborg_delete@@YAXXZ
// @address 0x3a9cb0 simulation_action_sandbox_group_update
// @address 0x3a9dd0 simulation_action_sandbox_groups_delete
// @address 0x3a9fd8 ?simulation_action_game_engine_player_update@@YAXFK@Z
// @address 0x3aa0a0 ?simulation_action_game_engine_player_delete@@YAXF@Z
// @address 0x3aa418 simulation_action_object_latch_update
// @address 0x3aa5d0 ?simulation_action_actor_died@@YAXJ@Z
// @address 0x3aa930 ?simulation_action_weapon_state_update@@YAXJ@Z
union real_vector3d;
struct s_data_array;
// @address 0x3acc08 ?simulation_disable_object_interpolation@@YAXJ@Z
// @address 0x3acd80 simulation_object_get_interpolation_data
// @address 0x3ace30 ?simulation_get_object_interpolated_position@@YA_NJPATreal_point3d@@@Z
// @address 0x3acf00 ?simulation_get_object_interpolated_orientation@@YA_NJPATreal_vector3d@@0@Z
// @address 0x3acff8 simulation_object_set_interpolation_data
// @address 0xf4228 ?simulation_query_object_is_predicted@@YA_NJ@Z
// @address 0x3bf000 ?create_actor@c_simulation_world@@QAAJJJ@Z
// @address 0x409838 ?entity_capture_creation_data@c_simulation_entity_database@@QAAXJ@Z
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x8087a8 ?g_network_configuration@@3Us_network_configuration@@A
// @address 0x29f7c0 ?game_engine_globals_get_gamestate_index@@YAJXZ
// @address 0x29f860 ?game_engine_globals_get_player_gamestate_index@@YAJF@Z
// @address 0x29f760 ?game_engine_globals_get_simulation_entity_type@@YA?AW4e_simulation_entity_type@@XZ
// @address 0x29f810 ?game_engine_globals_get_statborg_gamestate_index@@YAJXZ
// @address 0xa7940 ?game_is_authoritative@@YA_NXZ
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0x101498 ?weapon_get_owner_unit_inventory_index@@YAJJ@Z
// @address 0x10fcb0 ?quaternions_interpolate@@YAXPBUreal_quaternion@@0MPAU1@@Z
// @address 0x10fd50 ?quaternion_normalize@@YAXPAUreal_quaternion@@@Z
// @address 0x31c9e8 ?object_attach_gamestate_entity@@YAXJJ@Z
// @address 0x3272c8 ?unit_set_actively_controlled@@YAXJ_N@Z
// @address 0x3af0d0 ?simulation_gamestate_entity_create@@YAJXZ
// @address 0x3d1ec8 ?simulation_entity_create@@YAJW4e_simulation_entity_type@@JJ@Z
// @address 0x3d1fb0 ?simulation_entity_update@@YAXJJK@Z
// @address 0x3d2050 ?simulation_entity_latch_update@@YAXJJK@Z
// @address 0x40a4d8 ?simulation_event_generate@@YAXW4e_simulation_event_type@@JPBJJJPBXJ@Z
// @address 0x40a6c8 ?simulation_event_generate_exclusive@@YAXW4e_simulation_event_type@@JPBJJJPBXJ@Z
// @address 0x40a800 ?simulation_event_generate_exclusive_list@@YAXW4e_simulation_event_type@@JPBJPAJJJPBXJ@Z
// @address 0x40aac0 ?simulation_object_get_authoritative_entity_internal@@YAJJ_N@Z
// @address 0x40abb8 ?simulation_object_get_predictive_entity_internal@@YAJJ_N@Z
// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x905c8 ?game_is_distributed@@YA_NXZ
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z
// @address 0x9d0d0 ?matrix3x3_rotation_from_quaternion@@YAPAUreal_matrix3x3@@PAU1@PBUreal_quaternion@@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xade58 ?matrix4x3_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0xdb610 ?matrix3x3_rotation_to_quaternion@@YAPAUreal_quaternion@@PBUreal_matrix3x3@@PAU1@@Z
// @address 0xf25a0 ?matrix4x3_inverse@@YAXPBUreal_matrix4x3@@PAU1@@Z
// @address 0xf26b0 ?quaternion_to_angle_and_vector@@YAXPBUreal_quaternion@@PAMPATreal_vector3d@@@Z
// @address 0x31cac0 ?object_detach_gamestate_entity@@YAXJJ@Z
// @address 0x40a638 ?simulation_event_generate_for_simulation_queue@@YAXW4e_simulation_event_type@@JPBJJPAX@Z
// @address 0x354898 ?vehicle_get_ultimate_parent@@YAJJ@Z


static __forceinline bool simulation_game_action_is_distributed_inline()
{
    bool result = false;
    long simulation = game_simulation::game_simulation_get();
    if (simulation >= 4 && simulation <= 5)
        result = true;
    return result;
}


// @address 0x3a9430 ?simulation_query_object_placement@@YA_NPBUobject_placement_data@@@Z


static __forceinline bool simulation_game_action_is_authoritative()
{
    return !game_simulation::game_is_predicted();
}
// @address 0x3a9638 ?simulation_query_predictive_event@@YA_NJJPAJ@Z

static __forceinline bool simulation_game_action_is_server_inline()
{
    bool result = true;
    long simulation = game_simulation::game_simulation_get();
    if (simulation == 2 || simulation == 4)
        result = false;
    return result;
}

static __forceinline bool simulation_game_action_mode_is_4_or_5(signed char mode)
{
    bool result = false;
    switch (mode)
    {
    case 4:
    case 5:
        result = true;
        break;
    }
    return result;
}

static __forceinline bool simulation_game_action_mode_not_2_or_4(signed char mode)
{
    bool result = true;
    switch (mode)
    {
    case 2:
    case 4:
        result = false;
        break;
    }
    return result;
}

static __forceinline bool simulation_game_action_not_idle(int mode)
{
    bool result = true;
    if (mode == 2 || mode == 4)
        result = false;
    return result;
}

static __forceinline bool simulation_game_action_in_range(int mode)
{
    bool result = false;
    if (mode >= 4 && mode <= 5)
        result = true;
    return result;
}


// @address 0xfcc10 ?simulation_action_object_delete@@YAXJ@Z


// @address 0x3aa0a0 ?simulation_action_game_engine_player_delete@@YAXF@Z


// @address 0x3a9880 ?simulation_action_game_engine_globals_delete@@YAXXZ


// @address 0x3a9b20 ?simulation_action_game_statborg_delete@@YAXXZ

class c_simulation_world;


// @address 0x41c560 ?send_event@c_simulation_event_handler@@QBAXW4e_simulation_event_type@@JPAJKJPBXJ@Z


static __forceinline c_simulation_world *simulation_game_action_get_world()
{
    return simulation_globals_get()->world;
}
static __forceinline void simulation_game_action_set_object_index(long gamestate_index, long object_index)
{
    s_simulation_gamestate_entity_datum *entity = (s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data;
    entity = &entity[datum_index_to_absolute_index(gamestate_index)];
    entity->object_index = object_index;
}
// @address 0x3a9970 ?simulation_action_game_statborg_create@@YAXXZ
// @address 0x3a9ed0 ?simulation_action_game_engine_player_create@@YAXF@Z
// @address 0x3aa190 ?simulation_action_object_create@@YAXJ@Z

static __forceinline bool simulation_game_action_is_cinematic(long object_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
    bool result = (object->flags_d6 & 2) != 0;
    return result;
}
static __forceinline long simulation_game_action_ultimate_parent(long object_index)
{
    long result = -1;
    while (object_index != -1)
    {
        result = object_index;
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
        object_index = object->parent_object_index;
    }
    return result;
}
// @address 0x3aa2c0 ?simulation_action_object_create_build_entity_types@@YAXJJJPAJPAW4e_simulation_entity_type@@0@Z
// @address 0x3aa4f0 ?simulation_action_actor_create@@YAXJ@Z


static __forceinline long simulation_game_action_first_active_bsp()
{
    unsigned long mask = g_active_structure_bsp_mask;
    return 31 - _CountLeadingZeros(mask & -mask);
}
static __forceinline long simulation_game_action_breakable_group_count(long bsp_index)
{
    structure_bsp *bsp =
        (structure_bsp *)global_tag_instances[
            datum_index_to_absolute_index(
                ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[bsp_index].structure_bsp.index)].definition;
    long count = bsp->structure_physics.breakable_surface_key_table.count;
    long groups = count / 32;
    if (count % 32 > 0)
        groups++;
    return groups;
}
// @address 0x3aa6a0 ?simulation_action_breakable_surfaces_create@@YAXXZ




// @address 0x3aafc0 ?simulation_action_damage_section_response@@YAXJJJW4e_damage_section_response_type@@@Z

// @address 0x3aac8 k_real_math_identity_matrix_storage
// @address 0x2251c k_real_zero_storage
// @address 0x22838 k_simulation_object_interpolation_constant_pool


static __forceinline short simulation_game_action_material_index(const c_global_material_type *material)
{
    return material->m_index;
}
static __forceinline bool simulation_game_action_world_is_distributed(c_simulation_world *world)
{
    return world->m_world_type == 7 || world->m_world_type == 8;
}
static __forceinline c_simulation_event_handler *simulation_game_action_event_handler(c_simulation_world *world)
{
    return &world->m_distributed_world->event_handler;
}
// @address 0x3ab290 ?simulation_action_projectile_detonate@@YAXJKPBVc_global_material_type@@PBTreal_vector3d@@PBUs_projectile_hit@@_N@Z
// @address 0x3ab750 ?simulation_action_pickup_ammo@@YAXJJJJ@Z
void simulation_action_pickup_ammo(long unit_index, long item_definition_index, long weapon_definition_index, long amount)
{
    if (unit_index != -1 && item_definition_index != -1 && weapon_definition_index != -1)
    {
        unit_datum * unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        if (unit->unit.player_index != -1)
        {
            const s_simulation_pickup_item_definition *item_definition =
                (const s_simulation_pickup_item_definition *)global_tag_instances[datum_index_to_absolute_index(item_definition_index)].definition;
            long references[1];
            references[0] = unit_index;
            s_simulation_pickup_ammo_event_data event_data;
            event_data.pickup_type = 0;
            memset(&event_data.definition_index, 0,
                sizeof(event_data) - offsetof(s_simulation_pickup_ammo_event_data, definition_index));
            if (((1 << item_definition->type) & 4) != 0)
                event_data.definition_index = item_definition_index;
            else
                event_data.definition_index = weapon_definition_index;
            event_data.pickup_type = 1;
            event_data.amount = amount;
            simulation_event_generate_exclusive(_simulation_event_type_unit_pickup, 1, references, unit->unit.player_index,
                sizeof(event_data), &event_data, g_network_configuration.event_relevance[_simulation_event_type_unit_pickup].delay_msec);
        }
    }
}
// @address 0x3ab838 ?simulation_action_pickup_equipment@@YAXJJ@Z

// @address 0x3ab968 ?simulation_action_unit_exit_vehicle@@YAXJ@Z

static __forceinline long simulation_game_action_absolute_index_to_index(const s_data_array *array, long absolute_index)
{
    long index = -1;
    if (absolute_index != -1)
        index = ((unsigned long)*(unsigned short *)(array->data + array->size * absolute_index) << 16) | absolute_index;
    return index;
}
static __forceinline long simulation_game_action_player_index(long absolute_index)
{
    return simulation_game_action_absolute_index_to_index(player_data, absolute_index);
}
// @address 0x3ac700 ?simulation_request_player_event@@YAXPBUs_player_editor_request_event_data@@@Z

static __forceinline float simulation_game_action_distance_squared(const real_point3d *point, const real_point3d *other)
{
    float x = point->x - other->x;
    float z = point->z - other->z;
    float y = point->y - other->y;
    return y * y + (z * z + x * x);
}
// @address 0x3ac9d8 ?simulation_request_projectile_supercombine@@YAXJ@Z


// @address 0x3acaf0 ?simulation_update_object_interpolation@@YAXJ@Z
// @address 0x3acca8 ?simulation_set_object_orientation_for_interpolation@@YAXJPBTreal_vector3d@@0@Z

// @address 0x3acc08 ?simulation_disable_object_interpolation@@YAXJ@Z

// @address 0x3a9cb0 simulation_action_sandbox_group_update

// @address 0x3a9dd0 simulation_action_sandbox_groups_delete

void simulation_action_object_update(long handle, unsigned long value)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_mode_is_4_or_5(mode) && simulation_game_action_mode_not_2_or_4(mode) && !game_is_cooperative())
    {
        object_datum *object = (object_datum *)object_get_and_verify_type(handle, ~0ul);
        long index = object->gamestate_index;
        if (index != -1)
        {
            long other =
                ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)[datum_index_to_absolute_index(index)].entity_index;
            if (other != -1)
                simulation_entity_update(other, handle, value);
        }
    }
}

bool simulation_query_object_is_predicted(long object_index)
{
    bool result = false;
    if (game_simulation::game_is_predicted())
    {
        unit_datum *object = (unit_datum *)object_get_and_verify_type(object_index, ~0ul);
        result = object->object.simulation_entity_index != -1;
    }
    return result;
}

void simulation_action_object_delete(long object_index)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode))
    {
        if (simulation_game_action_in_range(mode))
        {
            object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
            long index = object->gamestate_index;
            if (index != -1)
            {
                if (!game_is_cooperative())
                {
                    long other =
                        ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)[datum_index_to_absolute_index(index)].entity_index;
                    if (other != -1)
                        simulation_entity_delete(other, object_index, index);
                }
                ::datum_delete(g_simulation_gamestate_entity_data, object->gamestate_index);
                object_detach_gamestate_entity(object_index, object->gamestate_index);
            }
        }
    }
}

bool simulation_query_object_placement(const object_placement_data *placement)
{
    if (placement->definition_index == -1)
        return false;
    if (game_simulation::game_is_predicted())
        return ((long)::simulation_entity_type_from_object_creation(placement->definition_index, -1, false)) == -1;
    return true;
}

bool simulation_query_local_predictive_control(long object_index, long control_type)
{
    bool result = false;
    long controlling_player_index;
    if (game_simulation::game_is_predicted() && simulation_query_predictive_control(object_index, control_type, &controlling_player_index))
        result = player_is_on_local_machine(controlling_player_index);
    return result;
}

bool simulation_query_predictive_control(long object_index, long control_type, long *controlling_player_index)
{
    long player_index = -1;
    if (simulation_game_action_is_distributed_inline())
    {
        unit_datum *object = (unit_datum *)object_get_and_verify_type(object_index, ~0ul);
        if ((1 << object->object.identifier.get_type()) & 0x1003)
        {
            unit_datum *unit = object;
            player_index = unit->unit.player_index;
            switch (control_type)
            {
            case 0:
                if (player_index == -1 && unit->unit.driver_object_index != -1)
                {
                    unit_datum *primary = (unit_datum *)object_get_and_verify_type(unit->unit.driver_object_index, 0x1003);
                    player_index = primary->unit.player_index;
                }
                break;
            case 1:
                if (player_index == -1 && unit->unit.gunner_object_index != -1)
                {
                    unit_datum *secondary = (unit_datum *)object_get_and_verify_type(
                        unit->unit.gunner_object_index, 0x1003);
                    player_index = secondary->unit.player_index;
                }
                break;
            }
        }
        else if (((1 << object->object.identifier.get_type()) & 12) && control_type == 1 && object->unit.actor_index != -1)
        {
            unit_datum *controller = (unit_datum *)object_get_and_verify_type(object->unit.actor_index, 0x1003);
            player_index = controller->unit.player_index;
        }
    }
    if (controlling_player_index)
        *controlling_player_index = player_index;
    return player_index != -1;
}

bool simulation_query_predictive_event(long object_index, long control_type, long *controlling_player_index)
{
    bool result;
    if (simulation_game_action_is_authoritative())
    {
        result = true;
        simulation_query_predictive_control(object_index, control_type, controlling_player_index);
    }
    else
    {
        result = simulation_query_local_predictive_control(object_index, control_type);
        *controlling_player_index = -1;
    }
    return result;
}

// @address 0x3a96c0 ?simulation_action_game_engine_globals_create@@YAXXZ
void simulation_action_game_engine_globals_create(void)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode) && simulation_game_action_in_range(mode))
    {
        long gamestate_index = simulation_gamestate_entity_create();
        game_engine_globals->gamestate_index = gamestate_index;
        simulation_game_action_set_object_index(gamestate_index, -1);
        if (!game_is_cooperative())
        {
            e_simulation_entity_type entity_type = game_engine_globals_get_simulation_entity_type();
            if (entity_type != _simulation_entity_type_none)
            {
                long entity_index = simulation_entity_create(entity_type, -1, gamestate_index);
                if (entity_index != -1)
                {
                    c_simulation_entity_database *database = &simulation_game_action_get_world()->m_distributed_world->m_entity_database;
                    database->entity_capture_creation_data(entity_index);
                }
            }
        }
    }
}

void simulation_action_game_engine_globals_update(unsigned long flags)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode))
    {
        if (simulation_game_action_in_range(mode))
        {
            if (!game_is_cooperative())
            {
                long index = game_engine_globals_get_gamestate_index();
                if (index != -1)
                {
                    long target =
                        ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)[datum_index_to_absolute_index(index)].entity_index;
                    if (target != -1)
                        simulation_entity_update(target, -1, flags);
                }
            }
        }
    }
}

void simulation_action_game_engine_globals_delete(void)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode))
    {
        if (simulation_game_action_in_range(mode))
        {
            long gamestate_index = game_engine_globals_get_gamestate_index();
            if (gamestate_index != -1)
            {
                if (!game_is_cooperative())
                {
                    long entity_index =
                        ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)
                            [datum_index_to_absolute_index(gamestate_index)].entity_index;
                    if (entity_index != -1)
                        simulation_entity_delete(entity_index, -1, gamestate_index);
                }
                ::datum_delete(g_simulation_gamestate_entity_data, gamestate_index);
            }
            game_engine_globals->gamestate_index = -1;
        }
    }
}

void simulation_action_game_statborg_create(void)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode) && simulation_game_action_in_range(mode))
    {
        long gamestate_index = simulation_gamestate_entity_create();
        game_engine_globals->statborg_gamestate_index = gamestate_index;
        simulation_game_action_set_object_index(gamestate_index, -1);
        if (!game_is_cooperative())
        {
            long entity_index = simulation_entity_create(_simulation_entity_type_game_engine_statborg, -1, gamestate_index);
            if (entity_index != -1)
            {
                c_simulation_world *world = simulation_game_action_get_world();
                c_simulation_entity_database *database = &world->m_distributed_world->m_entity_database;
                database->entity_capture_creation_data(entity_index);
            }
        }
    }
}

void simulation_action_game_statborg_update(unsigned long player_mask)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode))
    {
        if (simulation_game_action_in_range(mode))
        {
            if (!game_is_cooperative())
            {
                long index = game_engine_globals_get_statborg_gamestate_index();
                if (index != -1)
                {
                    long target =
                        ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)[datum_index_to_absolute_index(index)].entity_index;
                    if (target != -1)
                        simulation_entity_update(target, -1, player_mask);
                }
            }
        }
    }
}

void simulation_action_game_statborg_delete(void)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode))
    {
        if (simulation_game_action_in_range(mode))
        {
            long gamestate_index = game_engine_globals_get_statborg_gamestate_index();
            if (gamestate_index != -1)
            {
                if (!game_is_cooperative())
                {
                    long entity_index =
                        ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)
                            [datum_index_to_absolute_index(gamestate_index)].entity_index;
                    if (entity_index != -1)
                        simulation_entity_delete(entity_index, -1, gamestate_index);
                }
                ::datum_delete(g_simulation_gamestate_entity_data, gamestate_index);
            }
            game_engine_globals->statborg_gamestate_index = -1;
        }
    }
}

// @address 0x3a9c10 simulation_action_sandbox_group_create
extern "C" void simulation_action_sandbox_group_create(long group_index)
{
    long gamestate_index = simulation_gamestate_entity_create();
    game_engine_globals->map_info.m_simulation_indices[group_index] = gamestate_index;
    simulation_game_action_set_object_index(gamestate_index, group_index);
    if (!game_is_cooperative())
    {
        long entity_index = simulation_entity_create(_simulation_entity_type_map_variant, group_index, gamestate_index);
        if (entity_index != -1)
        {
            c_simulation_entity_database *database = &simulation_game_action_get_world()->m_distributed_world->m_entity_database;
            database->entity_capture_creation_data(entity_index);
        }
    }
}

extern "C" void simulation_action_sandbox_group_update(long surface_index, unsigned long flags, bool immediate)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode))
    {
        if (simulation_game_action_in_range(mode))
        {
            if (game_engine_sandbox_is_active() && !game_is_cooperative())
            {
                long group_index = surface_index / 8;
                long group_surface_index = surface_index % 8;
                long gamestate_index = game_engine_globals->map_info.m_simulation_indices[group_index];
                if (gamestate_index != -1)
                {
                    long entity_index =
                        ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)
                            [datum_index_to_absolute_index(gamestate_index)].entity_index;
                    if (entity_index != -1)
                    {
                        if (immediate)
                            simulation_entity_update(entity_index, group_index, flags << (group_surface_index * 4));
                        else
                            simulation_entity_latch_update(entity_index, group_index, flags << (group_surface_index * 4));
                    }
                }
            }
        }
    }
}

extern "C" void simulation_action_sandbox_groups_delete(void)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode))
    {
        if (simulation_game_action_in_range(mode))
        {
            for (long group_index = 0; group_index < 80; group_index++)
            {
                long gamestate_index = game_engine_globals->map_info.m_simulation_indices[group_index];
                if (gamestate_index != -1)
                {
                    if (!game_is_cooperative())
                    {
                        long entity_index =
                            ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)
                                [datum_index_to_absolute_index(gamestate_index)].entity_index;
                        if (entity_index != -1)
                            simulation_entity_delete(entity_index, group_index, gamestate_index);
                    }
                    ::datum_delete(g_simulation_gamestate_entity_data, gamestate_index);
                }
                game_engine_globals->map_info.m_simulation_indices[group_index] = -1;
            }
        }
    }
}

void simulation_action_game_engine_player_create(short player_absolute_index)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode) && simulation_game_action_in_range(mode))
    {
        long gamestate_index = simulation_gamestate_entity_create();
        game_engine_globals->player_gamestate_indices[player_absolute_index] = gamestate_index;
        simulation_game_action_set_object_index(gamestate_index, -1);
        if (!game_is_cooperative())
        {
            long entity_index = simulation_entity_create(_simulation_entity_type_game_engine_player, -1, gamestate_index);
            if (entity_index != -1)
            {
                c_simulation_entity_database *database = &simulation_game_action_get_world()->m_distributed_world->m_entity_database;
                database->entity_capture_creation_data(entity_index);
            }
        }
    }
}

void simulation_action_game_engine_player_update(short handle, unsigned long mask)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode))
    {
        if (simulation_game_action_in_range(mode))
        {
            if (!game_is_cooperative())
            {
                long index = game_engine_globals_get_player_gamestate_index(handle);
                if (index != -1)
                {
                    long target =
                        ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)[datum_index_to_absolute_index(index)].entity_index;
                    if (target != -1)
                        simulation_entity_update(target, 0xffffffff, mask);
                }
            }
        }
    }
}

void simulation_action_game_engine_player_delete(short player_index)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode))
    {
        if (simulation_game_action_in_range(mode))
        {
            long gamestate_index = game_engine_globals_get_player_gamestate_index(player_index);
            if (gamestate_index != -1)
            {
                if (!game_is_cooperative())
                {
                    long entity_index =
                        ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)
                            [datum_index_to_absolute_index(gamestate_index)].entity_index;
                    if (entity_index != -1)
                        simulation_entity_delete(entity_index, -1, gamestate_index);
                }
                ::datum_delete(g_simulation_gamestate_entity_data, gamestate_index);
            }
            game_engine_globals->player_gamestate_indices[player_index] = -1;
        }
    }
}

void simulation_action_object_create(long object_index)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode) && simulation_game_action_in_range(mode))
    {
        long entity_object_indices[4];
        long entity_count = 0;
        e_simulation_entity_type entity_types[4];
        simulation_action_object_create_build_entity_types(object_index, -1, 4, &entity_count, entity_types, entity_object_indices);
        if (entity_count > 0)
        {
            for (long index = 0; index < entity_count; index++)
            {
                long entity_object_index = entity_object_indices[index];
                long gamestate_index = simulation_gamestate_entity_create();
                object_attach_gamestate_entity(entity_object_index, gamestate_index);
                simulation_game_action_set_object_index(gamestate_index, entity_object_index);
                if (!game_is_cooperative())
                {
                    long entity_index = simulation_entity_create(entity_types[index], entity_object_indices[index], gamestate_index);
                    if (entity_index != -1)
                    {
                        c_simulation_entity_database *database =
                            &simulation_game_action_get_world()->m_distributed_world->m_entity_database;
                        database->entity_capture_creation_data(entity_index);
                    }
                }
            }
        }
    }
}

void simulation_action_object_create_build_entity_types(long object_index, long parent_object_index, long maximum_entity_count,
    long *entity_count, e_simulation_entity_type *entity_types, long *entity_object_indices)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
    s_object_header *header = &((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)];
    bool cinematic = simulation_game_action_is_cinematic(object_index);
    bool parent_cinematic = simulation_game_action_is_cinematic(simulation_game_action_ultimate_parent(object_index));
    if (!(header->flags & 16) && object->gamestate_index == -1 && !cinematic && !parent_cinematic)
    {
        bool dynamic = (bool)(_rotl(object->recycling_flags, 31) & 1);
        e_simulation_entity_type type = (e_simulation_entity_type)((long)::simulation_entity_type_from_object_creation(
            object->definition_index, parent_object_index, dynamic));
        if (type != -1 && *entity_count < maximum_entity_count)
        {
            entity_types[*entity_count] = type;
            entity_object_indices[*entity_count] = object_index;
            (*entity_count)++;
            long child_index = object->first_child_object_index;
            while (child_index != -1)
            {
                object_datum *child = (object_datum *)object_get_and_verify_type(child_index, ~0ul);
                if ((bool)(_rotl(child->flags, 6) & 1))
                    simulation_action_object_create_build_entity_types(
                        child_index, object_index, maximum_entity_count, entity_count, entity_types, entity_object_indices);
                child_index = child->next_object_index;
            }
        }
    }
}

extern "C" void simulation_action_object_latch_update(long object_index)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_mode_is_4_or_5(mode) && simulation_game_action_mode_not_2_or_4(mode) && !game_is_cooperative())
    {
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
        long index = object->gamestate_index;
        if (index != -1)
        {
            long entity =
                ((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)[datum_index_to_absolute_index(index)].entity_index;
            if (entity != -1)
                simulation_entity_latch_update(entity, object_index, 2);
        }
    }
}

void simulation_action_actor_create(long unit_index)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode) && simulation_game_action_in_range(mode))
    {
        unit_datum * unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        simulation_action_object_create(unit_index);
        c_simulation_world *world = simulation_game_action_get_world();
        unit->unit.simulation_actor_datum_index = world->create_actor(-1, unit_index);
        unit_set_actively_controlled(unit_index, true);
        simulation_action_object_update(unit_index, 16384);
    }
}

void simulation_action_actor_died(long object_index)
{
    if (simulation_game_action_is_distributed_inline())
    {
        unit_datum *object = (unit_datum *)object_get_and_verify_type(object_index, 8195);
        if (object->unit.simulation_actor_datum_index != -1)
        {
            c_simulation_actor *actor = &simulation_globals_get()->world->m_actors[object->unit.simulation_actor_datum_index];
            actor->destroy();
            object->unit.simulation_actor_datum_index = -1;
            unit_set_actively_controlled(object_index, false);
            simulation_action_object_update(object_index, 0x4000);
        }
    }
}

void simulation_action_breakable_surfaces_create(void)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_not_idle(mode) && simulation_game_action_in_range(mode))
    {
        long group_count = simulation_game_action_breakable_group_count(simulation_game_action_first_active_bsp());
        for (long group_index = 0; group_index < group_count; group_index++)
        {
            long gamestate_index = simulation_gamestate_entity_create();
            simulation_game_action_set_object_index(gamestate_index, group_index);
            if (game_is_distributed() && !game_is_cooperative())
            {
                long entity_index = simulation_entity_create(_simulation_entity_type_breakable_surface_group, group_index, gamestate_index);
                if (entity_index != -1)
                {
                    c_simulation_world *world = simulation_game_action_get_world();
                    c_simulation_entity_database *database = &world->m_distributed_world->m_entity_database;
                    database->entity_capture_creation_data(entity_index);
                }
            }
        }
    }
}

void simulation_action_weapon_state_update(long weapon_index)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode))
    {
        if (simulation_game_action_not_idle(mode) && !game_is_cooperative())
        {
            long object_index = -1;
            item_datum *weapon = (item_datum *)object_get_and_verify_type(weapon_index, ~0ul);
            if (weapon->owner_relation != 0)
                object_index = weapon->unit_object_index;
            long inventory_index = weapon_get_owner_unit_inventory_index(weapon_index);
            if (object_index != -1 && (unsigned long)inventory_index <= 3)
                simulation_action_object_update(object_index, 1 << (inventory_index + 22));
        }
    }
}

void simulation_action_damage_section_response(long object_index,
    long section_index,
    long response_index,
    e_damage_section_response_type response_type)
{
    if (simulation_game_action_is_distributed_inline() && simulation_game_action_is_server_inline() && !game_is_cooperative())
    {
        long entity_index = simulation_object_get_authoritative_entity_internal(object_index, false);
        if (entity_index != -1)
        {
            s_damage_section_response_data event_data;
            event_data.section_index = 0;
            long references[1];
            references[0] = object_index;
            event_data.section_index = section_index;
            event_data.response.response_index = response_index;
            event_data.response.type = response_type;
            simulation_event_generate(_simulation_event_type_damage_section_response, 1, references, -1, sizeof(event_data), &event_data,
                g_network_configuration.event_relevance[_simulation_event_type_damage_section_response].delay_msec);
        }
    }
}


// @address 0x3ab0a8 ?simulation_action_projectile_attached@@YAXJJFPBTreal_point3d@@PBUs_location@@@Z
void simulation_action_projectile_attached(
    long projectile_index, long parent_object_index, short parent_node_index, const real_point3d *position, const s_location *location)
{
    if (simulation_game_action_is_distributed_inline() && simulation_game_action_is_server_inline() && !game_is_cooperative())
    {
        s_object_header *headers = ((s_object_header *)object_header_data->data);
        object_datum *projectile = (object_datum *)headers[(unsigned short)projectile_index].datum;
        if (projectile->gamestate_index != -1)
        {
            s_simulation_projectile_attached_event_data event_data;
            memset(&event_data.node_index, 0,
                sizeof(event_data) - offsetof(s_simulation_projectile_attached_event_data, node_index));
            if (parent_object_index != -1)
            {
                object_datum *parent = (object_datum *)headers[(unsigned short)parent_object_index].datum;
                if (parent->gamestate_index == -1 && parent->parent_object_index != -1 && headers[(unsigned short)parent->parent_object_index].object_type == 0)
                {
                    parent_object_index = parent->parent_object_index;
                    parent_node_index = parent->parent_node_index;
                }
            }
            long references[2];
            references[0] = projectile_index;
            references[1] = parent_object_index;
            event_data.node_index = parent_node_index;
            event_data.attached_to_object = parent_object_index != -1;
            if (event_data.attached_to_object)
            {
                object_datum *parent = (object_datum *)headers[datum_index_to_absolute_index(parent_object_index)].datum;
                real_matrix4x3 *matrices = (real_matrix4x3 *)((char *)parent + parent->node_matrices_offset);
                real_matrix4x3 inverse;
                matrix4x3_inverse(&matrices[event_data.node_index], &inverse);
                matrix4x3_transform_point(&inverse, position, &event_data.position);
                event_data.position_encoding_type = 0;
            }
            else
            {
                event_data.position = *position;
            }
            if (location)
                memcpy(event_data.location, location, sizeof(event_data.location));
            else
                location_invalidate((s_location *)event_data.location);
            simulation_event_generate(_simulation_event_type_projectile_attached, 2, references, -1, sizeof(event_data), &event_data,
                g_network_configuration.event_relevance[_simulation_event_type_projectile_attached].delay_msec);
        }
    }
}

void simulation_action_projectile_detonate(long projectile_index, unsigned long flags, const c_global_material_type *hit_material_type,
    const real_vector3d *hit_normal, const s_projectile_hit *hit, bool force)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && simulation_game_action_not_idle(mode) && !game_is_cooperative())
    {
        object_datum *projectile = (object_datum *)object_get_and_verify_type(projectile_index, 32);
        if (projectile->gamestate_index != -1 || force)
        {
            s_simulation_projectile_effect_event_data event_data;
            memset(&event_data, 0, sizeof(event_data));
            event_data.definition_index = projectile->definition_index;
            object_get_origin(projectile_index, &event_data.position);
            real_vector3d up;
            ::object_get_orientation(projectile_index, &event_data.forward, &up);
            event_data.flags = flags;
            if (flags & 4)
            {
                event_data.material_index = simulation_game_action_material_index(hit_material_type);
                event_data.material_normal = *hit_normal;
                event_data.has_material_effect = hit->type != -1;
                if (event_data.has_material_effect)
                    event_data.material_effect_index = hit->index;
            }
            c_simulation_world *world = simulation_game_action_get_world();
            if (simulation_game_action_world_is_distributed(world))
            {
                // match: 0x3ab0a8: removal is locally identical but renumbers TU IL, flipping detonate's relevance-address schedule (lwz r9/addis r3,r9,1)
                c_simulation_event_handler * __restrict event_handler = simulation_game_action_event_handler(world);
                event_handler->send_event(_simulation_event_type_projectile_effect, 0, 0, ~0ul, sizeof(event_data), &event_data,
                    g_network_configuration.event_relevance[_simulation_event_type_projectile_effect].delay_msec);
            }
        }
    }
}


#define F6098_MIN(a, b) ((a) < (b) ? (a) : (b))
#define F6098_MAX(a, b) ((a) > (b) ? (a) : (b))
#define F6098_PIN(x, lo, hi) F6098_MIN(F6098_MAX(x, lo), hi)
inline real_vector3d *simulation_game_action_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *vector)
{
    vector->i = b->x - a->x;
    vector->j = b->y - a->y;
    vector->k = b->z - a->z;
    return vector;
}
// @address 0xf6098 ?simulation_action_damage_aftermath@@YAXJPBUs_damage_aftermath_result_data@@@Z
void simulation_action_damage_aftermath(long object_index, const s_damage_aftermath_result_data *result)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode))
    {
        if (simulation_game_action_not_idle(mode))
        {
            if (!game_is_cooperative())
            {
        unit_datum *object = (unit_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
        s_object_definition *definition =
            (s_object_definition *)global_tag_instances[datum_index_to_absolute_index(object->object.definition_index)].definition;
        s_damage_effect_definition *damage_definition = (s_damage_effect_definition *)global_tag_instances[
            datum_index_to_absolute_index(result->damage_definition_index)].definition;
        long object_type = ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type;
        long owner_player_index = -1;
        bool send_event = false;
        if (((1 << 0) | (1 << 1) | (1 << 12)) & (1 << object_type))
        {
            owner_player_index = object->unit.player_index;
        }
        if ((((1 << 0) | (1 << 1) | (1 << 12)) & (1 << object_type)) || ((1 << 10) & (1 << object_type)))
        {
            if (definition->acceleration_scale * damage_definition->damage.instantaneous_acceleration > 0.0001f)
            {
                send_event = true;
            }
            else if ((((1 << 0) | (1 << 1) | (1 << 12)) & (1 << object_type)) &&
                (!(bool)(_rotl(object->object.damage_flags, 30) & 1) || ((result->flags & (1 << 0)) != 0)) &&
                (result->body_damage > 0.0f || result->shield_damage > 0.0f))
            {
                send_event = true;
            }
            else if (owner_player_index != -1 && result->owner_player_index != -1 && result->owner_player_index != owner_player_index)
            {
                send_event = true;
            }
        }
        if (send_event && simulation_object_get_authoritative_entity_internal(object_index, false) != -1)
        {
            long object_indices[2];
            object_indices[0] = object_index;
            if (result->damage_owner_object_index != -1 && object_try_and_get_and_verify_type(result->damage_owner_object_index,
                (unsigned long)-1))
                object_indices[1] = result->damage_owner_object_index;
            else
                object_indices[1] = -1;
            s_simulation_damage_aftermath_event_data event_data;
            memset(&event_data, 0, sizeof(event_data));
            event_data.damage_definition_index = result->damage_definition_index;
            event_data.damage_owner_player_index = (unsigned short)result->owner_player_index;
            event_data.direction = result->direction;
            if (normalize3d(&event_data.direction) > 0.0001f)
                event_data.direction_valid = true;
            real_point3d origin;
            ::object_get_origin(object_index, &origin);
            // match: 0xf6098: pointer __restrict qualifier prevents alias reload
            const s_damage_aftermath_result_data * __restrict epicenter_source = result;
            simulation_game_action_vector_from_points(&origin, &epicenter_source->epicenter, &event_data.epicenter_direction);
            event_data.epicenter_distance = normalize3d(&event_data.epicenter_direction);
            if ((float)fabs(event_data.epicenter_distance - 0.0f) >= 0.0001f)
            {
                event_data.epicenter_distance = F6098_PIN(event_data.epicenter_distance, 0.0f, 25.0f);
                event_data.epicenter_valid = true;
            }
            event_data.scale = F6098_PIN(result->scale, 0.0f, 2.0f);
            event_data.shake_scale = F6098_PIN(result->shake_scale, 0.0f, 2.0f);
            event_data.flags = result->flags;
            event_data.ping_type = result->ping_type;
            event_data.body_damage = F6098_PIN(result->body_damage, 0.0f, 16.0f);
            event_data.shield_damage = F6098_PIN(result->shield_damage, 0.0f, 3.0f);
            event_data.body_part = (short)result->body_part;
            event_data.node_index = result->node_index;
            event_data.damage_reporting_info = result->reporting_info;
            event_data.special_death_type = result->response_metadata;
            simulation_event_generate(_simulation_event_type_damage_aftermath, 2, object_indices, -1, sizeof(event_data), &event_data,
                g_network_configuration.event_relevance[_simulation_event_type_damage_aftermath].delay_msec);
            }
        }
    }
}
}

struct s_aim_assist_targeting_result;
struct weapon_definition;
struct s_simulation_action_projectile_fire_result;

void simulation_action_pickup_equipment(long unit_index, long equipment_definition_index)
{
    if (unit_index != -1 && equipment_definition_index != -1)
    {
        // match: 0x3ab838: keeps the unit pointer in r31 for the player_index check and references store (removal recolors it to r30)
        unit_datum * __restrict unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        if (unit->unit.player_index != -1)
        {
            long references[1];
            references[0] = unit_index;
            s_pickup_equipment_data event_data;
            event_data.type = 0;
            event_data.equipment.definition_index = equipment_definition_index;
            event_data.type = 2;
            simulation_event_generate_exclusive(_simulation_event_type_unit_pickup, 1, references, unit->unit.player_index,
                sizeof(event_data), &event_data, g_network_configuration.event_relevance[_simulation_event_type_unit_pickup].delay_msec);
        }
    }
}

void simulation_action_unit_exit_vehicle(long unit_index)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && simulation_game_action_not_idle(mode) && !game_is_cooperative())
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1)
        {
            long references[2];
            references[0] = unit_index;
            references[1] = unit->object.parent_object_index;
            s_exit_vehicle_data event_data;
            event_data.seat_index = unit->unit.parent_seat_index;
            memset(&event_data.flags, 0, sizeof(event_data.flags));
            simulation_event_generate(_simulation_event_type_unit_exit_vehicle, 2, references, -1, sizeof(event_data), &event_data,
                g_network_configuration.event_relevance[_simulation_event_type_unit_exit_vehicle].delay_msec);
        }
    }
}

void simulation_request_player_event(const s_player_editor_request_event_data *event_data)
{
    player_datum *player =
        (&((player_datum *)player_data->data)[datum_index_to_absolute_index(simulation_game_action_player_index(event_data->request_player_index))]);
    bool valid = true;
    if (player->unit_index == -1)
    {
        valid = false;
    }
    if (valid)
    {
        c_simulation_world *world = simulation_game_action_get_world();
        if (simulation_game_action_world_is_distributed(world))
        {
            // match: 0x3ac700: keeps the relevance-address computation on r8 with the delay load first (removal rebuilds it through r3 and sinks the load)
            c_simulation_event_handler * __restrict event_handler = simulation_game_action_event_handler(world);
            event_handler->send_event(_simulation_event_type_player_editor_request,
                0,
                0,
                ~0ul,
                120,
                event_data,
                g_network_configuration.event_relevance[_simulation_event_type_player_editor_request].delay_msec);
        }
    }
}


// @address 0x3ac8e8 ?simulation_request_player_respawn_target@@YAXJJ@Z
void simulation_request_player_respawn_target(long player_index, long target_index)
{
    bool send = true;
    if ((&((player_datum *)player_data->data)[datum_index_to_absolute_index(player_index)])->unit_index != -1)
    {
        send = false;
    }
    if (target_index != -1 && (&((player_datum *)player_data->data)[datum_index_to_absolute_index(target_index)])->unit_index == -1)
    {
        send = false;
    }
    if (send != 0)
    {
        unsigned short request[2];
        request[0] = (unsigned short)player_index;
        request[1] = (unsigned short)target_index;
        c_simulation_world *world = simulation_game_action_get_world();
        if (simulation_game_action_world_is_distributed(world))
        {
            // match: 0x3ac8e8: pointer __restrict qualifier prevents alias reload
            c_simulation_event_handler * __restrict event_handler = simulation_game_action_event_handler(world);
            event_handler->send_event(_simulation_event_type_player_respawn_request, 0, 0, ~0ul, sizeof(request), request,
                g_network_configuration.event_relevance[_simulation_event_type_player_respawn_request].delay_msec);
        }
    }
}

void simulation_request_projectile_supercombine(long projectile_index)
{
    if (simulation_query_object_is_predicted(projectile_index))
    {
        object_datum *projectile = (object_datum *)object_get_and_verify_type(projectile_index, 32);
        if (projectile->simulation_object_index != -1)
        {
            if ((float)(game_time_globals->time - projectile->simulation_object_index) * game_time_globals->tick_length >=
                g_network_configuration.supercombine_minimum_time)
            {
                float distance = g_network_configuration.supercombine_minimum_distance;
                real_point3d position;
                object_get_origin(projectile_index, &position);
                if (simulation_game_action_distance_squared(&position, &projectile->simulation_update_position) > distance * distance)
                {
                    long event_data;
                    memset(&event_data, 0, sizeof(event_data));
                    simulation_event_generate(_simulation_event_type_object_refresh, 1, &projectile_index, -1, sizeof(event_data), &event_data,
                        g_network_configuration.event_relevance[_simulation_event_type_object_refresh].delay_msec);
                    projectile->simulation_object_index = game_time_globals->time;
                }
            }
        }
    }
}

void simulation_update_object_interpolation(long object_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
    if (object->flags_d6 & 1)
    {
        if (object->parent_object_index != -1)
            simulation_disable_object_interpolation(object_index);
        else
        {
            bool updated = false;
            s_simulation_object_interpolation_data entry = { 0 };
            s_simulation_object_interpolation_data *data = &entry;
            simulation_object_get_interpolation_data(object_index, &entry);
            if (data->flags & 1)
            {
                simulation_update_object_interpolated_position(object_index, data);
                updated = true;
            }
            if (data->flags & 2)
            {
                simulation_update_object_interpolated_orientation(object_index, data);
                updated = true;
            }
            if (updated)
                simulation_object_set_interpolation_data(object_index, &entry);
            if (!updated || data->flags == 0)
            {
                unsigned char *flags = &object->flags_d6;
                *flags = (unsigned char)(*flags & ~1);
            }
        }
    }
}

void simulation_disable_object_interpolation(long object_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
    if (object->flags_d6 & 1)
    {
        s_simulation_object_interpolation_data entry = { 0 };
        s_simulation_object_interpolation_data *data = &entry;
        simulation_object_get_interpolation_data(object_index, &entry);
        unsigned char *flags = &object->flags_d6;
        *flags = (unsigned char)(*flags & ~1);
        data->flags = 0;
        simulation_object_set_interpolation_data(object_index, &entry);
    }
}

void simulation_set_object_orientation_for_interpolation(long object_index, const real_vector3d *forward, const real_vector3d *up)
{
    s_simulation_object_interpolation_data entry = { 0 };
    s_simulation_object_interpolation_data *data = &entry;
    if (simulation_object_get_interpolation_data(object_index, &entry))
    {
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
        unsigned char *flags = &object->flags_d6;
        *flags = (unsigned char)(*flags | 1);
        data->flags |= 2;
        data->forward = *forward;
        data->up = *up;
        simulation_object_set_interpolation_data(object_index, &entry);
    }
}

extern "C" unsigned char simulation_object_get_interpolation_data(long object_index, s_simulation_object_interpolation_data *out)
{
    bool result = false;
    bool active = false;
    if ((unsigned int)(game_simulation::game_simulation_get() - 4) <= 1) active = true;
    if (active)
    {
        s_object_header *header = &((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)];
        if (header->object_type == 0)
        {
            biped_datum *object = (biped_datum *)header->datum;
            if (object->object.gamestate_index != -1 && object->biped.simulation_interpolation.size > 0)
            {
                result = 1;
                *out = *(s_simulation_object_interpolation_data *)((char *)object + object->biped.simulation_interpolation.offset);
            }
        }
    }
    return result;
}

bool simulation_get_object_interpolated_position(long object_index, real_point3d *point)
{
    bool result = false;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
    if (object->parent_object_index == -1 && (object->flags_d6 & 1))
    {
        s_simulation_object_interpolation_data entry = { 0 };
        s_simulation_object_interpolation_data *data = &entry;
        if ((bool)simulation_object_get_interpolation_data(object_index, &entry) && (data->flags & 1))
        {
            *point = data->position;
            result = true;
        }
    }
    return result;
}

bool simulation_get_object_interpolated_orientation(long object_index, real_vector3d *vector, real_vector3d *vector2)
{
    bool result = false;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
    if (object->parent_object_index == -1 && (object->flags_d6 & 1))
    {
        s_simulation_object_interpolation_data entry = { 0 };
        s_simulation_object_interpolation_data *data = &entry;
        if ((bool)simulation_object_get_interpolation_data(object_index, &entry) && (data->flags & 2))
        {
            *vector = data->forward;
            *vector2 = data->up;
            result = true;
        }
    }
    return result;
}

extern "C" void simulation_object_set_interpolation_data(long object_index,
    const s_simulation_object_interpolation_data *interpolation_data)
{
    if (simulation_game_action_is_distributed_inline() &&
        ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].object_type == 0)
    {
        biped_datum *object = (biped_datum *)object_get_and_verify_type(object_index, 1);
        if (object->object.gamestate_index != -1 && object->biped.simulation_interpolation.size > 0)
            *(s_simulation_object_interpolation_data *)((char *)object + object->biped.simulation_interpolation.offset) =
                *interpolation_data;
    }
}


// @address 0x3aa7f8 ?simulation_action_multiplayer_event@@YAXPBUs_game_engine_event_data@@@Z
void simulation_action_multiplayer_event(const s_game_engine_event_data *source)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && simulation_game_action_not_idle(mode) && !game_is_cooperative())
    {
        s_game_engine_event_data event = *source;
        if (event.flags != -1)
            event.flags = (unsigned short)event.flags;
        if (event.cause_player_index != -1)
            event.cause_player_index = (unsigned short)event.cause_player_index;
        if (event.effect_player_index != -1)
            event.effect_player_index = (unsigned short)event.effect_player_index;
        long count = 0;
        long players[16];
        ::notify_audience_member_of_event((s_game_engine_event_data const *)source,
            (long *)(players),
            16,
            (s_game_engine_event_data const *)&count);
        if (count > 0)
            simulation_event_generate_exclusive_list(
                _simulation_event_type_game_engine,
                    0,
                    0,
                    players,
                    count,
                    sizeof(event),
                    &event,
                    g_network_configuration.event_relevance[_simulation_event_type_game_engine].delay_msec);
    }
}


// @address 0x3aae90 ?simulation_action_weapon_effect@@YAXJPBTreal_point3d@@J@Z
void simulation_action_weapon_effect(long weapon_index, const real_point3d *position, long effect_type)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && simulation_game_action_not_idle(mode) && !game_is_cooperative())
    {
        item_datum *weapon = (item_datum *)object_get_and_verify_type(weapon_index, 4);
        if (weapon->object.simulation_entity_index != -1)
        {
            weapon_definition *definition =
                (weapon_definition *)global_tag_instances[datum_index_to_absolute_index(weapon->object.definition_index)].definition;
            if (definition->multiplayer_weapon_type == 1)
            {
                long references[1];
                references[0] = weapon_index;
                s_weapon_effect_event_data event_data;
                event_data.type = effect_type;
                event_data.position = *position;
                simulation_event_generate(_simulation_event_type_weapon_effect, 1, references, -1, sizeof(event_data), &event_data,
                    g_network_configuration.event_relevance[_simulation_event_type_weapon_effect].delay_msec);
            }
        }
    }
}


// @address 0x3ab8d0 simulation_action_pickup_powerup
extern "C" void simulation_action_pickup_powerup(long object_index, long value)
{
    if (object_index != -1 && value != -1)
    {
        unit_datum *object = (unit_datum *)object_get_and_verify_type(object_index, ~0ul);
        if (object->unit.player_index != -1)
        {
            s_pickup_event_payload event_data = { 0 };
            event_data.value = value;
            long references[1] = { object_index };
            event_data.type = 3;
            simulation_event_generate_exclusive(_simulation_event_type_unit_pickup, 1, references, object->unit.player_index,
                sizeof(event_data), &event_data, g_network_configuration.event_relevance[_simulation_event_type_unit_pickup].delay_msec);
        }
    }
}

// @address 0x3aba70 ?simulation_action_unit_board_vehicle@@YAXJ@Z
void simulation_action_unit_board_vehicle(long unit_index)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && simulation_game_action_not_idle(mode) && !game_is_cooperative())
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        if (unit->object.parent_object_index != -1 && unit->unit.parent_seat_index != -1)
        {
            long references[2];
            references[0] = unit_index;
            references[1] = unit->object.parent_object_index;
            long event_data = unit->unit.parent_seat_index;
            simulation_event_generate(_simulation_event_type_unit_board_vehicle, 2, references, -1, sizeof(event_data), &event_data,
                g_network_configuration.event_relevance[_simulation_event_type_unit_board_vehicle].delay_msec);
        }
    }
}


// @address 0x3abb70 ?simulation_action_unit_grenade_initiate@@YAXJF@Z
void simulation_action_unit_grenade_initiate(long unit_index, short grenade_type)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && !game_is_cooperative())
    {
        if (grenade_type >= 0 && grenade_type < 4)
        {
            long controlling_player_index;
            if (simulation_query_predictive_event(unit_index, 1, &controlling_player_index))
            {
                s_simulation_grenade_initiate_event_data event_data;
                event_data.type = 0;
                event_data.grenade_type = grenade_type;
                simulation_event_generate(_simulation_event_type_unit_grenade_initiate,
                    1,
                    &unit_index,
                    controlling_player_index,
                    sizeof(event_data),
                    &event_data, g_network_configuration.event_relevance[_simulation_event_type_unit_grenade_initiate].delay_msec);
            }
        }
    }
}

// @address 0x3abc40 ?simulation_action_unit_melee_initiate@@YAXJJJJ@Z
void simulation_action_unit_melee_initiate(long unit_index, long melee_index, long melee_type, long attack_type)
{
    long mode = game_simulation::game_simulation_get();
    if (simulation_game_action_in_range(mode) && !game_is_cooperative())
    {
        long controlling_player_index;
        if (simulation_query_predictive_event(unit_index, 1, &controlling_player_index))
        {
            short melee_event_type;
            if (attack_type >= 1u && attack_type != 1u)
            {
                if (attack_type >= 3u)
                    return;
                bool predicted = false;
                if (mode == 4)
                    predicted = true;
                melee_event_type = predicted ? 0 : 2;
            }
            else
            {
                melee_event_type = 2;
            }
            long references[2];
            references[0] = unit_index;
            references[1] = melee_index;
            short event_data[2];
            event_data[0] = (short)melee_type;
            event_data[1] = melee_event_type;
            simulation_event_generate(_simulation_event_type_unit_melee_initiate,
                2,
                references,
                controlling_player_index,
                sizeof(event_data),
                event_data,
                g_network_configuration.event_relevance[_simulation_event_type_unit_melee_initiate].delay_msec);
        }
    }
}


// @address 0x3abd38 simulation_action_unit_melee_clang
extern "C" void simulation_action_unit_melee_clang(long type, const real_point3d *position, const real_vector3d *direction)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && simulation_game_action_not_idle(mode) && !game_is_cooperative())
    {
        s_game_action_melee_event_data data;
        memset(&data, 0, sizeof(data));
        data.type = type;
        data.position = *position;
        data.direction = *direction;
        if (simulation_game_action_world_is_distributed(simulation_game_action_get_world()))
        {
            c_simulation_world *world = simulation_game_action_get_world();
            // match: 0x3abd38: pointer __restrict qualifier prevents alias reload
            c_simulation_event_handler *__restrict handler = simulation_game_action_event_handler(world);
            handler->send_event(_simulation_event_type_unit_melee_clang, 0, 0, ~0ul, sizeof(data), &data,
                g_network_configuration.event_relevance[_simulation_event_type_unit_melee_clang].delay_msec);
        }
    }
}


// @address 0x3abe70 ?simulation_action_unit_reload@@YAXJ_N0@Z
void simulation_action_unit_reload(long unit_index, bool successful, bool empty)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && !game_is_cooperative())
    {
        long controlling_player_index;
        if (simulation_query_predictive_event(unit_index, 1, &controlling_player_index))
        {
            s_simulation_reload_event_data event_data = { 0 };
            event_data.type = 0;
            event_data.successful = successful;
            event_data.empty = empty;
            simulation_event_generate(_simulation_event_type_weapon_reload, 1, &unit_index, controlling_player_index, sizeof(event_data),
                &event_data, g_network_configuration.event_relevance[_simulation_event_type_weapon_reload].delay_msec);
        }
    }
}

// @address 0x3abf38 simulation_action_unit_reload_interrupt
extern "C" void simulation_action_unit_reload_interrupt(long unit_index, bool successful, bool empty)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && !game_is_cooperative())
    {
        long controlling_player_index;
        if (simulation_query_predictive_event(unit_index, 1, &controlling_player_index))
        {
            s_simulation_reload_event_data event_data = { 0 };
            event_data.type = 1;
            event_data.successful = successful;
            event_data.empty = empty;
            simulation_event_generate(_simulation_event_type_weapon_reload, 1, &unit_index, controlling_player_index, sizeof(event_data),
                &event_data, g_network_configuration.event_relevance[_simulation_event_type_weapon_reload].delay_msec);
        }
    }
}


// @address 0x3ac000 ?simulation_action_weapon_empty_click@@YAXJ@Z
void simulation_action_weapon_empty_click(long weapon_index)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && !game_is_cooperative())
    {
        item_datum *weapon = (item_datum *)object_get_and_verify_type(weapon_index, 4);
        long controlling_player_index;
        long unit_index = -1;
        if (weapon->owner_relation != 0)
            unit_index = weapon->unit_object_index;
        long references[1];
        references[0] = unit_index;
        if (unit_index != -1)
        {
            long inventory_index = weapon_get_owner_unit_inventory_index(weapon_index);
            if (inventory_index != -1 && simulation_query_predictive_event(unit_index, 1, &controlling_player_index))
            {
                s_empty_click_event_data event_data;
                event_data.unit_inventory_index = inventory_index;
                event_data.definition_index = weapon->object.definition_index;
                simulation_event_generate(_simulation_event_type_weapon_empty_click,
                    1,
                    references,
                    controlling_player_index,
                    sizeof(event_data),
                    &event_data, g_network_configuration.event_relevance[_simulation_event_type_unit_melee_initiate].delay_msec);
            }
        }
    }
}


// @address 0x3ac1a0 simulation_request_put_away_weapon
extern "C" void simulation_request_put_away_weapon(long unit_index, short set_index)
{
    if (simulation_query_local_predictive_control(unit_index, 1))
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        long weapon_index = unit_inventory_get_weapon_or_none(unit_index,
            ((unit_datum *)object_get_and_verify_type(unit_index, 8195))->unit.current_weapon_set.weapon_indices[set_index]);
        if (weapon_index != -1)
        {
            s_put_away_weapon_event_data event_data;
            event_data.set_index = set_index;
            event_data.weapon_index = unit->unit.current_weapon_set.weapon_indices[set_index];
            event_data.definition_index = ((object_datum *)object_get_and_verify_type(weapon_index, 4))->definition_index;
            simulation_event_generate(_simulation_event_type_weapon_put_away, 1, &unit_index, -1, sizeof(event_data), &event_data,
                g_network_configuration.event_relevance[_simulation_event_type_weapon_put_away].delay_msec);
        }
    }
}

// @address 0x3ac280 simulation_request_drop_weapon
extern "C" void simulation_request_drop_weapon(long unit_index, short set_index)
{
    if (simulation_query_local_predictive_control(unit_index, 1))
    {
        unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
        long weapon_index = unit_inventory_get_weapon_or_none(unit_index,
            ((unit_datum *)object_get_and_verify_type(unit_index, 8195))->unit.current_weapon_set.weapon_indices[set_index]);
        if (weapon_index != -1)
        {
            s_put_away_weapon_event_data event_data;
            event_data.set_index = set_index;
            event_data.weapon_index = unit->unit.current_weapon_set.weapon_indices[set_index];
            event_data.definition_index = ((object_datum *)object_get_and_verify_type(weapon_index, 4))->definition_index;
            simulation_event_generate(_simulation_event_type_weapon_drop, 1, &unit_index, -1, sizeof(event_data), &event_data,
                g_network_configuration.event_relevance[_simulation_event_type_weapon_drop].delay_msec);
        }
    }
}



static __forceinline real_matrix3x3 *simulation_game_action_matrix3x3_from_forward_and_up(
    real_matrix3x3 *matrix, const real_vector3d *forward, const real_vector3d *up)
{
    matrix->forward = *forward;
    cross_product3d(up, forward, &matrix->left);
    matrix->up = *up;
    return matrix;
}

static __forceinline void simulation_game_action_quaternions_interpolate_and_normalize(
    const real_quaternion *a, const real_quaternion *b, float t, real_quaternion *result)
{
    quaternions_interpolate(a, b, t, result);
    quaternion_normalize(result);
}

// @address 0x3ad178 simulation_update_object_interpolated_orientation
extern "C" void simulation_update_object_interpolated_orientation(long object_index, s_simulation_object_interpolation_data *data)
{
    s_simulation_object_interpolation_data *interpolation_data = data;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 8195);
    float angle1 = 0.0f;
    float angle2 = 0.0f;
    real_matrix3x3 matrix1;
    real_matrix3x3 matrix2;
    real_quaternion q1;
    real_quaternion q2;
    real_vector3d axis;
    simulation_game_action_matrix3x3_from_forward_and_up(&matrix1, &interpolation_data->forward, &interpolation_data->up);
    simulation_game_action_matrix3x3_from_forward_and_up(&matrix2, &object->forward, &object->up);
    matrix3x3_rotation_to_quaternion(&matrix1, &q1);
    matrix3x3_rotation_to_quaternion(&matrix2, &q2);
    quaternion_to_angle_and_vector(&q1, &angle1, &axis);
    quaternion_to_angle_and_vector(&q2, &angle2, &axis);
    if (angle2 - angle1 > k_object_orientation_threshold)
    {
        real_quaternion q;
        real_matrix3x3 matrix;
        simulation_game_action_quaternions_interpolate_and_normalize(&q1, &q2, k_object_orientation_fraction, &q);
        matrix3x3_rotation_from_quaternion(&matrix, &q);
        interpolation_data->forward = matrix.forward;
        interpolation_data->up = matrix.up;
    }
    else
    {
        interpolation_data->forward = object->forward;
        interpolation_data->up = object->up;
        interpolation_data->flags &= ~2;
    }
}


static __forceinline bool simulation_game_action_grenade_type_valid(short hand, long grenade_type)
{
    bool valid = false;
    switch (hand)
    {
    case 0:
        valid = grenade_type >= 0 && grenade_type < 4;
        break;
    case 1:
        valid = grenade_type != -1;
        break;
    }
    return valid;
}

// @address 0x3ac360 simulation_request_unit_grenade_release
extern "C" void simulation_request_unit_grenade_release(
    long unit_index, short hand, long grenade_type, const real_point3d *origin, const real_vector3d *direction)
{
    if (simulation_query_local_predictive_control(unit_index, 1) && simulation_game_action_grenade_type_valid(hand, grenade_type))
    {
        s_object_header *headers = ((s_object_header *)object_header_data->data);
        // match: 0x3ac360: pointer __restrict qualifier prevents alias reload
        s_aim_assist_targeting_result *__restrict aim =
            &((unit_datum *)headers[datum_index_to_absolute_index(unit_index)].datum)->unit.targeting;
        s_game_action_grenade_release_data data;
        memset(&data, 0, sizeof(data));
        data.hand = hand;
        data.grenade_type = grenade_type;
        data.origin = *origin;
        data.direction = *direction;
        data.entity_index = -1;
        data.entity_gamestate_index = -1;
        data.node_index = -1;
        if ((aim->flags & 4) && aim->object_index != -1 && object_try_and_get_and_verify_type(aim->object_index, ~0ul))
        {
            object_datum *object = (object_datum *)headers[datum_index_to_absolute_index(aim->object_index)].datum;
            if (object->gamestate_index != -1)
            {
                s_simulation_gamestate_entity_datum *entity =
                    &((s_simulation_gamestate_entity_datum *)g_simulation_gamestate_entity_data->data)[datum_index_to_absolute_index(object->gamestate_index)];
                if (entity->entity_index != -1)
                {
                    data.has_target = true;
                    data.node_index = aim->model_target_index;
                    data.entity_index = entity->entity_index;
                    memcpy(&data.position, &aim->lead_vector, sizeof(data.position));
                }
            }
        }
        simulation_event_generate(_simulation_event_type_unit_grenade_release, 1, &unit_index, -1, sizeof(data), &data,
            g_network_configuration.event_relevance[_simulation_event_type_unit_grenade_release].delay_msec);
    }
}


// @address 0x3ac530 simulation_action_unit_melee_damage
extern "C" void simulation_action_unit_melee_damage(long unit_index,
    long damage_definition_index,
    long clang_damage_definition_index,
    const s_simulation_unit_melee_damage_target *damage_target)
{
    if (simulation_query_local_predictive_control(unit_index, 1))
    {
        s_data_array *headers = object_header_data;
        unit_datum *unit = (unit_datum *)((s_object_header *)headers->data)[datum_index_to_absolute_index(unit_index)].datum;
        long references[2];
        references[0] = unit_index;
        references[1] = -1;
        s_simulation_unit_melee_damage_event_data event_data;
        memset(&event_data, 0, sizeof(event_data));
        s_simulation_unit_melee_damage_event_data *event = (s_simulation_unit_melee_damage_event_data *)object_get_origin(unit_index, &event_data.position);
        // match: 0x3ac530: value range assumption guides branch lowering
        __assume(event == &event_data);
        event->direction.i = -unit->unit.aiming_vector.i;
        event->direction.j = -unit->unit.aiming_vector.j;
        event->direction.k = -unit->unit.aiming_vector.k;
        event->damage_definition_index = damage_definition_index;
        event->clang_damage_definition_index = clang_damage_definition_index;
        event->breakable_surface_index = -1;
        event->breakable_surface_set_index = -1;
        event->instanced_geometry_instance_index = -1;
        event->surface_index = -1;
        event->material_index = -1;
        event->global_material_index = -1;
        event->damage_reporting_info = 12;
        if (damage_target)
        {
            event_data.position = damage_target->position;
            event_data.direction = damage_target->direction;
            event_data.material_index = damage_target->global_material_index;
            event_data.scale = damage_target->damage_scale;
            event_data.damage_reporting_info = damage_target->hit_material_flags;
            if (damage_target->model_region_index != -1 && damage_target->model_region_index >= 0 && damage_target->model_region_index < 32)
                event_data.global_material_index = damage_target->model_region_index;
            if (damage_target->object_index != -1)
            {
                long object_index = damage_target->object_index;
                s_data_array *object_headers = headers;
                object_datum *object =
                    (object_datum *)((s_object_header *)object_headers->data)[datum_index_to_absolute_index(object_index)].datum;
                if (object->gamestate_index != -1)
                    references[1] = object_index;
            }
            else if ((unsigned short)damage_target->breakable_surface_instance_index != 0xffff)
            {
                event_data.breakable_surface_set_index = damage_target->breakable_surface_index;
                event_data.breakable_surface_index = damage_target->breakable_surface_instance_index;
                event_data.instanced_geometry_instance_index = damage_target->breakable_surface_bsp_index;
                event_data.surface_index = damage_target->breakable_surface_cluster_index;
            }
        }
        simulation_event_generate(_simulation_event_type_unit_melee_damage, 2, references, -1, sizeof(event_data), &event_data,
            g_network_configuration.event_relevance[_simulation_event_type_unit_melee_damage].delay_msec);
    }
}




// @address 0x3ac7d8 ?simulation_request_player_editor@@YAXJW4e_player_editor_request_type@@_NJ@Z
void simulation_request_player_editor(long player_index, enum e_player_editor_request_type request_type, bool unknown, long target_index)
{
    bool valid = true;
    if ((&((player_datum *)player_data->data)[datum_index_to_absolute_index(player_index)])->unit_index == -1)
    {
        valid = false;
    }
    if (!valid)
        return;
    s_player_editor_request_event_data event_data;
    event_data.event_type = 1;
    event_data.request_player_index = (short)player_index;
    event_data.flags = true;
    event_data.variant_index = target_index;
    if (simulation_game_action_world_is_distributed(simulation_game_action_get_world()))
    {
        c_simulation_world *world = simulation_game_action_get_world();
        // match: 0x3ac7d8: pointer __restrict qualifier prevents alias reload
        c_simulation_event_handler * __restrict event_handler = simulation_game_action_event_handler(world);
        event_handler->send_event(_simulation_event_type_player_editor_request,
            0,
            0,
            ~0ul,
            sizeof(event_data),
            &event_data,
            g_network_configuration.event_relevance[_simulation_event_type_player_editor_request].delay_msec);
    }
}


// @address 0x3ac110 simulation_action_pickup_weapon
extern "C" void simulation_action_pickup_weapon(long unit_index, long weapon_index, long desired_weapon_index)
{
    if (simulation_query_local_predictive_control(unit_index, 1))
    {
        long references[2];
        references[0] = unit_index;
        references[1] = weapon_index;
        s_simulation_weapon_pickup_event_data event_data;
        event_data.unit_add_weapon_mode = desired_weapon_index;
        memcpy(&event_data.weapon_set,
            &((unit_datum *)object_get_and_verify_type(unit_index, 8195))->unit.current_weapon_set_storage, sizeof(s_unit_weapon_set));
        simulation_event_generate(_simulation_event_type_weapon_pickup, 2, references, -1, sizeof(event_data), &event_data,
            g_network_configuration.event_relevance[_simulation_event_type_weapon_pickup].delay_msec);
    }
}


static __forceinline void simulation_game_action_position_delta(const real_point3d *a, const real_point3d *b, real_vector3d *result)
{
    result->i = b->x - a->x;
    result->j = b->y - a->y;
    result->k = b->z - a->z;
}
static __forceinline float simulation_game_action_magnitude(const real_vector3d *v)
{
    return sqrtf((v->k * v->k + v->j * v->j) + v->i * v->i);
}
static __forceinline void simulation_game_action_points_interpolate(const real_point3d *a,
    const real_point3d *b,
    float fraction,
    real_point3d *out)
{
    const float inverse_fraction = 1.0f - fraction;
    out->x = inverse_fraction * a->x + fraction * b->x;
    const float by = fraction * b->y;
    out->y = by + inverse_fraction * a->y;
    const float bz = fraction * b->z;
    out->z = bz + inverse_fraction * a->z;
}
// @address 0x3ad0a0 simulation_update_object_interpolated_position
extern "C" void simulation_update_object_interpolated_position(long object_index,
    s_simulation_object_interpolation_data *interpolation_data)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, ~0ul);
    real_vector3d delta;
    simulation_game_action_position_delta(&interpolation_data->position, &object->position, &delta);
    if (!(simulation_game_action_magnitude(&delta) <= 0.05f))
    {
        const float fraction = k_object_position_fraction;
        simulation_game_action_points_interpolate(&interpolation_data->position,
            &object->position,
            fraction,
            &interpolation_data->position);
    }
    else
    {
        interpolation_data->position = object->position;
        interpolation_data->flags &= ~1ul;
    }
}


static __forceinline bool simulation_game_action_fire_is_predicted(const game_globals_storage *game)
{
    bool result = false;
    long simulation = (signed char)game->options.simulation_type;
    if (simulation == 4)
        result = true;
    return result;
}
static __forceinline e_simulation_node_space_encoding_type simulation_game_action_fire_node_space(const s_data_array *headers,
    long object_index)
{
    switch (((s_object_header *)headers->data)[datum_index_to_absolute_index(object_index)].object_type)
    {
    case 0:
        return _simulation_node_space_encoding_type_object;
    default:
        return _simulation_node_space_encoding_type_world;
    }
}
// @address 0x3aaa10 simulation_action_weapon_fire
extern "C" void simulation_action_weapon_fire(long weapon_index, long barrel_index, unsigned char fire_mode,
    const s_simulation_action_projectile_fire_result *projectile_results, long result_count, bool simulation_queue)
{
    game_globals_storage *game = tls_game_globals();
    if (simulation_game_action_in_range((signed char)game->options.simulation_type) && !game_is_cooperative())
    {
        s_data_array *headers = object_header_data;
        s_object_header *objects = ((s_object_header *)headers->data);
        weapon_datum *weapon = (weapon_datum *)objects[datum_index_to_absolute_index(weapon_index)].datum;
        long definition_index = weapon->definition_index;
        weapon_definition *definition =
            (weapon_definition *)global_tag_instances[datum_index_to_absolute_index(definition_index)].definition;
        s_weapon_barrel_definition *barrel = &definition->barrels[barrel_index];
        if (barrel->prediction_type != 1)
        {
            long owner_unit_index = NONE;
            if (weapon->item.owner_relation)
                owner_unit_index = weapon->item.unit_object_index;
            long inventory_index = weapon_get_owner_unit_inventory_index(weapon_index);
            long references[2] = {NONE, NONE};
            long aim_owner_index = NONE;
            if (owner_unit_index != NONE)
            {
                if ((unsigned long)inventory_index <= 3)
                {
                    references[0] = owner_unit_index;
                    unit_datum *owner = (unit_datum *)objects[datum_index_to_absolute_index(owner_unit_index)].datum;
                    if (owner->unit.gunner_object_index != NONE)
                        aim_owner_index = owner->unit.gunner_object_index;
                    else
                        aim_owner_index = owner_unit_index;
                }
            }
            else if (inventory_index == NONE)
            {
                references[0] = weapon_index;
            }
            long event_object_index = references[0];
            if (objects[datum_index_to_absolute_index(event_object_index)].object_type == 1)
            {
                unit_datum *vehicle = (unit_datum *)objects[datum_index_to_absolute_index(event_object_index)].datum;
                long primary_control_index = vehicle->unit.gunner_object_index;
                long controlling_player_index = NONE;
                if (primary_control_index != NONE)
                    controlling_player_index =
                        ((unit_datum *)objects[datum_index_to_absolute_index(primary_control_index)].datum)->unit.player_index;
                if (controlling_player_index == NONE)
                    event_object_index = vehicle_get_ultimate_parent(event_object_index);
            }
            if (event_object_index != NONE && simulation_query_predictive_event(event_object_index, 1, &references[1]))
            {
                s_simulation_weapon_fire_event_data event_data;
                csmemset(&event_data, 0, sizeof(event_data));
                event_data.unit_inventory_index = inventory_index;
                event_data.weapon_definition_index = definition_index;
                event_data.barrel_index = barrel_index;
                event_data.primary_trigger_byte = fire_mode;
                event_data.predicted_aim_assist.primary_autoaim_level = 0.0f;
                event_data.predicted_aim_assist.secondary_autoaim_level = 0.0f;
                event_data.predicted_aim_assist.target = NONE;
                event_data.predicted_aim_assist.model_target = NONE;
                event_data.aiming_vector = k_real_math_identity_matrix_storage.forward;
                if (aim_owner_index != NONE)
                {
                    unit_datum *owner =
                        (unit_datum *)((s_object_header *)headers->data)[datum_index_to_absolute_index(aim_owner_index)].datum;
                    simulation_game_build_predicted_aim_assist((const s_aim_assist_targeting_result *)&owner->unit.targeting,
                        &event_data.predicted_aim_assist);
                    owner = (unit_datum *)((s_object_header *)headers->data)[datum_index_to_absolute_index(aim_owner_index)].datum;
                    event_data.aiming_vector = owner->unit.aiming_vector;
                }
                event_data.projectile_count = 0;
                event_data.impact_target_count = 0;
                for (long i = 0; i < result_count; ++i)
                {
                    const s_simulation_action_projectile_fire_result *result = &projectile_results[i];
                    s_simulation_weapon_fire_projectile *projectile = &event_data.projectiles[event_data.projectile_count];
                    if (result->impact_valid)
                    {
                        long entity_index;
                        if (game_is_authoritative())
                            entity_index = simulation_object_get_authoritative_entity_internal(result->impact_object_index, false);
                        else
                            entity_index = simulation_object_get_predictive_entity_internal(result->impact_object_index, false);
                        if (entity_index != NONE)
                        {
                            long target_index = NONE;
                            for (long j = 0; j < event_data.impact_target_count; ++j)
                            {
                                if (event_data.impact_targets[j].entity_index == entity_index)
                                {
                                    target_index = j;
                                    break;
                                }
                            }
                            if (target_index == NONE)
                            {
                                event_data.impact_targets[event_data.impact_target_count].entity_index = entity_index;
                                event_data.impact_targets[event_data.impact_target_count].encoding_type = simulation_game_action_fire_node_space(headers,
                                    result->impact_object_index);
                                target_index = event_data.impact_target_count++;
                            }
                            projectile->impact_valid = true;
                            projectile->target_index = target_index;
                            projectile->object_node_index = result->impact_node_index;
                            projectile->impact_position = result->impact_position;
                        }
                    }
                    if (result->conical_valid)
                    {
                        projectile->conical_valid = true;
                        projectile->conical_random_seed = result->conical_random_seed;
                    }
                    if (projectile->conical_valid || projectile->impact_valid)
                    {
                        projectile->projectile_id = result->projectile_id;
                        ++event_data.projectile_count;
                    }
                }
                bool track_target = (barrel->flags & (1 << 13)) != 0;
                if (track_target && weapon->tracked_object_index != NONE)
                {
                    long entity_index;
                    if (simulation_game_action_fire_is_predicted(game))
                        entity_index = simulation_object_get_predictive_entity_internal(weapon->tracked_object_index, false);
                    else
                        entity_index = simulation_object_get_authoritative_entity_internal(weapon->tracked_object_index, false);
                    if (entity_index != NONE)
                    {
                        long model_target_index = weapon->tracked_model_target_index;
                        event_data.tracked_entity_index = entity_index;
                        event_data.tracked_data_valid = true;
                        event_data.tracked_model_target_index = model_target_index;
                    }
                }
                event_data.generated_from_host_live_fire = simulation_queue;
                if (simulation_queue)
                    simulation_event_generate_for_simulation_queue(_simulation_event_type_weapon_fire,
                        1,
                        references,
                        sizeof(event_data),
                        &event_data);
                else
                    simulation_event_generate(_simulation_event_type_weapon_fire,
                        1,
                        references,
                        references[1],
                        sizeof(event_data),
                        &event_data,
                        g_network_configuration.event_relevance[_simulation_event_type_weapon_fire].delay_msec);
            }
        }
    }
}

static __forceinline long simulation_get_node_space_encoding_type(long object_index)
{
    switch (((s_object_header *)object_header_data->data)[(unsigned short)object_index].object_type)
    {
    case 0:
        return 1;
    default:
        return 0;
    }
}
// @address 0x3ab458 simulation_action_projectile_impact
extern "C" void simulation_action_projectile_impact(bool impact, long projectile_index, float fraction, float velocity,
    const real_vector3d *direction, const collision_result *collision, bool from_impact)
{
    signed char mode = (signed char)tls_game_globals()->options.simulation_type;
    if (simulation_game_action_in_range(mode) && simulation_game_action_not_idle(mode) && !game_is_cooperative())
    {
        s_object_header *headers = ((s_object_header *)object_header_data->data);
        object_datum *projectile = (object_datum *)headers[(unsigned short)projectile_index].datum;
        if (projectile->gamestate_index != -1)
        {
            bool attached = false;
            if (collision->type == 4 && collision->object_index != -1)
            {
                object_datum *object = (object_datum *)headers[(unsigned short)collision->object_index].datum;
                if (object->gamestate_index != -1)
                    attached = true;
            }
            if (attached)
            {
                s_simulation_projectile_object_impact_effect_event_data event_data;
                memset(&event_data.scale, 0, sizeof(event_data) - sizeof(event_data.definition_index));
                long object_index = collision->object_index;
                event_data.definition_index = projectile->definition_index;
                event_data.scale = fraction;
                event_data.error = velocity;
                event_data.direction = *direction;
                event_data.normal = collision->plane.n;
                event_data.material_index = (long)(short)collision->global_material.m_index;
                event_data.node_index = (long)(short)collision->node_index;
                event_data.extra = from_impact;
                event_data.effect = impact;
                event_data.position_encoding_type = simulation_get_node_space_encoding_type(collision->object_index);
                long node_count = 0;
                real_matrix4x3 *node_matrices = object_get_node_matrices(object_index, &node_count);
                real_matrix4x3 *node_matrix = &node_matrices[(short)collision->node_index];
                real_matrix4x3 inverse;
                matrix4x3_inverse(node_matrix, &inverse);
                matrix4x3_transform_point(&inverse, &collision->point, &event_data.position);
                simulation_event_generate(_simulation_event_type_projectile_object_impact_effect,
                    1,
                    &object_index,
                    -1,
                    sizeof(event_data),
                    &event_data,
                    g_network_configuration.event_relevance[_simulation_event_type_projectile_object_impact_effect].delay_msec);
            }
            else
            {
                s_simulation_projectile_impact_effect_event_data event_data;
                memset(&event_data.scale, 0, sizeof(event_data) - sizeof(event_data.definition_index));
                event_data.definition_index = projectile->definition_index;
                event_data.scale = fraction;
                event_data.error = velocity;
                event_data.direction = *direction;
                event_data.normal = collision->plane.n;
                event_data.position = collision->point;
                event_data.material_index = (long)(short)collision->global_material.m_index;
                event_data.extra = from_impact;
                if (simulation_game_action_world_is_distributed(simulation_game_action_get_world()))
                {
                    // match: 0x3ab458: pointer __restrict qualifier prevents alias reload
                    c_simulation_event_handler *__restrict handler =
                        simulation_game_action_event_handler(simulation_game_action_get_world());
                    handler->send_event(_simulation_event_type_projectile_impact_effect, 0, 0, ~0ul, sizeof(event_data), &event_data,
                        g_network_configuration.event_relevance[_simulation_event_type_projectile_impact_effect].delay_msec);
                }
            }
        }
    }
}

typedef char simulation_projectile_result_size[(sizeof(s_simulation_action_projectile_fire_result) == 0x20) ? 1 : -1];
typedef char simulation_projectile_result_impact_offset[(offsetof(s_simulation_action_projectile_fire_result,
    impact_position) == 0x10) ? 1 : -1];
typedef char simulation_projectile_attached_event_size[(sizeof(s_simulation_projectile_object_impact_effect_event_data) == 0x44) ? 1 : -1];
typedef char simulation_projectile_impact_event_size[(sizeof(s_simulation_projectile_impact_effect_event_data) == 0x38) ? 1 : -1];
typedef char simulation_weapon_fire_event_size[(sizeof(s_simulation_weapon_fire_event_data) == 0x33c) ? 1 : -1];
typedef char simulation_editor_event_size[(sizeof(s_player_editor_request_event_data) == 120) ? 1 : -1];
typedef char simulation_grenade_release_event_size[(sizeof(s_game_action_grenade_release_data) == 60) ? 1 : -1];
typedef char simulation_grenade_throw_size[(sizeof(s_simulation_unit_melee_damage_target) == 60) ? 1 : -1];
typedef char simulation_grenade_throw_event_size[(sizeof(s_simulation_unit_melee_damage_event_data) == 60) ? 1 : -1];
