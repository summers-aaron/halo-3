#include "simulation/game_interface/simulation_game_units_melee_types.h"
#include "models/model_types.h"
#include "objects/object_definitions.h"
// networkinglibxbox/simulation_game_units.obj: from the records 0x42d380 0x42d550 0x42d568 0x42e600 0x42e608 0x42e618 0x42e628 0x42e630 0x42e640 0x42e650 0x42e658 0x42e668 0x42e678 0x42e680 0x42e690 0x42e6e0 0x42e6e8 0x42e6f8 0x42e748 0x42e750 0x42e760 0x42e778 0x42e788 0x42e798 0x42e7a0 0x42e7b0 0x42e7c0 0x42e7c8 0x42e7d8 0x42e7e8 0x42e7f0 0x42e800 0x42e810 0x42e818 0x42e828 0x42e860 0x42e868 0x42e878 0x42e888 0x42e890 0x42e8a0 0x42e8a8 0x42e8b8 0x42e8c8 0x42e8d8 0x42e908 0x42ea58 0x42eb88 0x42f238 0x42f2a8 0x42f440 0x42fad0 0x42fbe0 0x42fc68 0x42fc70 0x42fce0 0x42fd70 0x42fe78 0x42fee8 0x430040 0x430738 0x430fd0 0x4311b0 0x4311b8 0x4311c8 0x431228 0x4313d0 0x431430 0x431470 0x4314d0 0x431590 0x4315f0 0x431608 0x431648 0x431838 0x431868 0x4318c8 0x431950 0x431a30 0x431c10 0x431c18 0x431c48 0x431ca8 0x431de8 0x431fb0 0x431ff0 0x4322d0 0x4322d8 0x432338 0x4324e0 0x432678 0x432b28 0x432b88 0x432bd8 0x432c80 0x432f30 0x432f38 0x432f40 0x432fa0 0x433018 0x433090 0x433120 0x433180 0x4332a8 0x433308 0x433320 0x433360 0x433400 0x433420 0x433480 0x4334f8 0x4335a0 0x433880 0x4338e0 0x4338f8 0x433940 0x433a08 0x433a68 0x709f28 0x70c7f0
// @flags /TP /O2 /Oc
// @address 0x2afd40 ?compare_unit_vectors@c_bitstream@@SA_NPBTreal_vector3d@@0@Z
#include "simulation/simulation_queue_event_types.h"
#include "simulation/game_interface/simulation_game_units.h"
#include "simulation/game_interface/simulation_game_map_variant.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "simulation/simulation_entity_types.h"
#include "networking/replication/replication_entity.h"
#include "networking/simulation/simulation_entity_definition.h"
#include "tag_files/tag_index.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "memory/data_types.h"
#include "memory/datum_index_inlines.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "memory/tls_globals.h"
#include "math/real_math_types.h"
#include "game/game_time_types.h"
#include "game/game_time_inline.h"
#include "objects/object_types.h"
#include "game/player_types.h"
#include "cseries/cseries_wrapped.h"
#include "cseries/bit_operations.h"
#include "objects/object_datum.h"
#include "units/motor_enums.h"
#include "units/vehicle_types.h"
#include "units/unit_seat.h"
#include "units/unit_control.h"
#include "units/unit_types.h"
#include "objects/object_placement.h"
#include "ai/actors.h"
#include "motor/action_system.h"
#include "objects/objects.h"
#include "sound/game_sound.h"
#include "units/units.h"
#include "units/unit_inventory_inline.h"
#include "ai/squads.h"
#include "cache/cache_files.h"
#include "game/aim_assist.h"
#include "motor/motor_system.h"
#include "objects/damage_owner.h"
#include "objects/model_customization.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/game_interface/simulation_game_internal.h"
#include "simulation/game_interface/simulation_game_objects.h"
#include "simulation/simulation_encoding.h"
#include "simulation/simulation_gamestate_entities.h"
#include "game/player_control.h"
#include "interface/chud/chud.h"
#include "items/equipment.h"
#include "items/projectiles.h"
#include "items/weapons.h"
#include "game/game_engine_teleporters.h"
#include "memory/bitstream.h"
#include "game/players.h"
#include "game/player_access.h"
#include "simulation/simulation_priority.h"
#include "units/vehicles.h"
#include "motor/motor_animation.h"
#include "game/player_types.h"
#include "game/player_types.h"
#include "units/unit_seat.h"
#include "units/action_request.h"
#include "game/materials_definitions.h"
#include "networking/player_configuration_encoding_types.h"
#include "scenario/scenario.h"
#include "units/unit_definitions.h"
#include "math/real_math_functions.h"
#include "math/real_math.h"
#include "math/real_math_globals.h"
#include "physics/breakable_surfaces.h"
#include "game/game_engine_util.h"
#include "tag_files/tag_groups.h"
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "networking/network_game_definitions.h"
#include "interface/chud/chud_message_context_types.h"
#include "cseries/cseries.h"
#include "networking/replication/replication_event.h"
#include <stdlib.h>
#include <string.h>
#include <ppcintrinsics.h>
#include "objects/object_header.h"
#include "models/model_variant_types.h"
#include "motor/player_melee_action_state.h"
#include "game/materials.h"
#include "game/game_globals.h"
#include "game/game_globals_storage.h"
#include "game/material_constants.h"
#include "render/render_objects_globals.h"
#include "effects/effect_creation.h"
#include "units/vehicle_type_alien_fighter.h"
#include "units/vehicle_datum.h"
#include "game/player_iterator.h"
#include "items/weapon_definitions.h"
#include "game/player_mapping_types.h"
#include "interface/chud/chud_messaging.h"
#include <math.h>
#include "ai/actor_datum.h"
#include "ai/weapon_datum_types.h"
#include "game/game_types.h"
#include "units/unit_facing.h"
union real_point3d;
class c_bitstream;
union real_rectangle3d;

// @address 0x431608 ?event_payload_decode@c_simulation_unit_board_vehicle_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x431838 ?minimum_required_bits@c_simulation_unit_grenade_initiate_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x431868 ?write_description_to_string@c_simulation_unit_grenade_initiate_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
// @address 0x4318c8 ?event_payload_encode@c_simulation_unit_grenade_initiate_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x431950 ?event_payload_decode@c_simulation_unit_grenade_initiate_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x431c18 ?minimum_required_bits@c_simulation_unit_grenade_release_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x431c48 ?write_description_to_string@c_simulation_unit_grenade_release_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
// @address 0x432b88 ?event_payload_encode@c_simulation_unit_melee_initiate_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x432bd8 ?event_payload_decode@c_simulation_unit_melee_initiate_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_melee_initiate_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x42d380 ?can_add_to_recent_list@c_game_engine_base_variant@@UBA_NXZ
// @address 0x42d380 ?can_clear_change_request_desired@c_network_session_parameter_base@@MAA_NXZ
// @address 0x42d380 ?can_set_peer_no_longer_requires_update@c_network_session_parameter_base@@MAA_NJ@Z
// @address 0x42d380 ?parameter_data_ready_for_consumption@c_network_session_parameter_base@@MBA_NXZ
// @address 0x42e680 ?event_type_name@c_simulation_unit_grenade_initiate_event_definition@@UAAPBDXZ
// @address 0x42e7a0 ?event_type_name@c_simulation_unit_melee_clang_event_definition@@UAAPBDXZ
// @address 0x42e7b0 ?event_requirements@c_simulation_unit_melee_clang_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x48d94 ??_7c_simulation_unit_melee_clang_event_definition@@6B@
// @address 0x48eac ??_7c_simulation_unit_equipment_pickup_event_definition@@6B@



class c_bitstream;
struct s_simulation_unit_weapon_state {
    long definition_index;
    short multiplayer_weapon_identifier;
    short rounds_loaded;
    short rounds_inventory;
    short rounds_total;
    float age;
};
struct s_simulation_unit_state_data : s_simulation_object_state_data {
    short control_values[5];
    long parent_vehicle_entity_index;
    short parent_seat_index;
    real_vector3d desired_aiming_vector;
    s_unit_weapon_set desired_weapon_set;
    s_simulation_unit_weapon_state weapons[4];
    unsigned char grenade_counts[4];
    bool active_camo_active;
    float active_camo_maximum;
    float active_camo_regrowth;
    float active_camo_level;
    long parent_vehicle_gamestate_index;
    long equipment_definition_index;
    long equipment_index;
    signed char map_editor_flags;
    bool hud_info_a;
    bool hud_info_b;
    unsigned char control_context;
};
typedef char simulation_unit_state_size[(sizeof(s_simulation_unit_state_data) == 0x128) ? 1 : -1];
typedef char simulation_unit_state_weapons_offset[(offsetof(s_simulation_unit_state_data, weapons) == 0xc4) ? 1 : -1];
typedef char simulation_unit_state_grenades_offset[(offsetof(s_simulation_unit_state_data, grenade_counts) == 0x104) ? 1 : -1];

#include "simulation/game_interface/simulation_game_object_entity_definition.h"
#include "simulation/game_interface/simulation_game_object_creation_inline.h"
struct s_simulation_unit_exit_vehicle_event_data {
    long parent_seat_index;
    bool detach_on_exit;
};
struct s_simulation_unit_grenade_initiate_event_data {
    short throw_type;
    long grenade_definition_index;
};
struct s_simulation_unit_melee_initiate_event_data {
    short melee_type;
    short prediction_type;
};
struct s_simulation_unit_board_vehicle_event_data {
    long boarding_seat_index;
};
struct s_simulation_unit_enter_vehicle_event_data {
    long seat_index;
};
struct s_simulation_vehicle_trick_event_data {
    long vehicle_trick_type;
};
struct s_simulation_unit_pickup_event_data {
    unsigned short pickup_type;
    long definition_index;
    short count;
};
struct s_simulation_unit_grenade_release_event_data {
    short throw_type;
    long grenade_type;
    real_point3d position;
    real_vector3d aiming_vector;
    bool has_entity;
    long entity_gamestate_index;
    long entity_index;
    long target_model_index;
    real_vector3d lead_vector;
};
struct s_simulation_unit_equipment_use_event_data {
    long equipment_definition_index;
};
struct s_simulation_unit_creation_data : s_simulation_object_creation_data {
    s_player_appearance appearance;
    long model_variant;
    long team_index;
};
struct s_simulation_unit_creation_state : s_simulation_object_state_data {
    short player_absolute_index;
};
// @address 0x40d178 ?object_update_decode@c_simulation_object_entity_definition@@KA_N_NPAKPAUs_simulation_object_state_data@@PAVc_bitstream@@0@Z
// @address 0x40d840 ?object_state_compare@c_simulation_object_entity_definition@@KA_NPAUs_simulation_object_state_data@@0@Z
// @address 0x42e600 ?event_type@c_simulation_unit_enter_vehicle_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x42e608 ?event_type_name@c_simulation_unit_enter_vehicle_event_definition@@UAAPBDXZ
// @address 0x3fc550 ?payload_size@c_simulation_unit_enter_vehicle_event_definition@@UAAJXZ
// @address 0x42e630 ?event_type_name@c_simulation_unit_exit_vehicle_event_definition@@UAAPBDXZ
// @address 0x42e650 ?payload_size@c_simulation_unit_exit_vehicle_event_definition@@UAAJXZ
// @address 0x42e658 ?event_type_name@c_simulation_unit_board_vehicle_event_definition@@UAAPBDXZ
// @address 0x3fc550 ?payload_size@c_simulation_unit_board_vehicle_event_definition@@UAAJXZ
// @address 0x42e6e0 ?event_type@c_simulation_unit_grenade_release_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x42e748 ?event_type@c_simulation_unit_melee_initiate_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x42e750 ?event_type_name@c_simulation_unit_melee_initiate_event_definition@@UAAPBDXZ
// @address 0x42e778 ?event_type_name@c_simulation_unit_melee_damage_event_definition@@UAAPBDXZ
// @address 0x42e798 ?event_type@c_simulation_unit_melee_clang_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x42e7c0 ?event_type@c_simulation_vehicle_flip_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x42e7c8 ?event_type_name@c_simulation_vehicle_flip_event_definition@@UAAPBDXZ
// @address 0x42e7e8 ?event_type@c_simulation_vehicle_trick_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x42e7f0 ?event_type_name@c_simulation_vehicle_trick_event_definition@@UAAPBDXZ
// @address 0x42e810 ?event_type@c_simulation_unit_pickup_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x42e818 ?event_type_name@c_simulation_unit_pickup_event_definition@@UAAPBDXZ
// @address 0x42e860 ?event_type@c_simulation_unit_equipment_use_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x42e868 ?event_type_name@c_simulation_unit_equipment_use_event_definition@@UAAPBDXZ
// @address 0x431430 ?event_payload_encode@c_simulation_unit_exit_vehicle_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x4315f0 ?event_payload_encode@c_simulation_unit_board_vehicle_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x431c10 ?payload_size@c_simulation_unit_grenade_release_event_definition@@UAAJXZ
// @address 0x431c10 ?payload_size@c_simulation_unit_melee_damage_event_definition@@UAAJXZ
// @address 0x433180 ?apply_game_event@c_simulation_vehicle_flip_event_definition@@UAA_NJPBJJPBX@Z
// @address 0x433308 ?event_payload_encode@c_simulation_vehicle_trick_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x433320 ?event_payload_decode@c_simulation_vehicle_trick_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x433400 ?minimum_required_bits@c_simulation_unit_pickup_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x433480 ?event_payload_encode@c_simulation_unit_pickup_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z


class c_bitstream;
class c_replication_outgoing_event;

namespace {
static __forceinline long player_appearance_required_bits() { return 60; }
}


struct cache_file_tag_instance;
// @address 0xedff8 ?action_submit@@YA_NJPBUaction_request@@@Z
struct action_request;

// @address 0x42d550 ?entity_creation_encode@c_simulation_map_variant_group_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
void c_simulation_map_variant_group_entity_definition::entity_creation_encode(long size, const void *creation_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update)
{
    packet->write_integer("integer", *static_cast<const long *>(creation_data), 32);
}

// @address 0x42d568 ?entity_creation_decode@c_simulation_map_variant_group_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_map_variant_group_entity_definition::entity_creation_decode(long size, void *creation_data,
    c_bitstream *packet, bool final_update)
{
    *static_cast<long *>(creation_data) = packet->read_integer("integer", 32);
    return true;
}

// 0x42e600
e_simulation_event_type c_simulation_unit_enter_vehicle_event_definition::event_type()
{
    return _simulation_event_type_unit_enter_vehicle;
}

// 0x42e608
const char *c_simulation_unit_enter_vehicle_event_definition::event_type_name()
{
    return "unit_enter_vehicle";
}

// 0x42d380
bool c_simulation_unit_melee_initiate_event_definition::event_can_be_transmitted(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// 0x42e680
const char *c_simulation_unit_grenade_initiate_event_definition::event_type_name()
{
    return "unit_throw_initiate";
}

long c_simulation_unit_enter_vehicle_event_definition::payload_size()
{
    return sizeof(s_simulation_unit_enter_vehicle_event_data);
}

// 0x42e630
const char *c_simulation_unit_exit_vehicle_event_definition::event_type_name()
{
    return "unit_exit_vehicle";
}

// 0x42e650
long c_simulation_unit_exit_vehicle_event_definition::payload_size()
{
    return sizeof(s_simulation_unit_exit_vehicle_event_data);
}

// 0x42e658
const char *c_simulation_unit_board_vehicle_event_definition::event_type_name()
{
    return "unit_board_vehicle";
}

long c_simulation_unit_board_vehicle_event_definition::payload_size()
{
    return sizeof(s_simulation_unit_board_vehicle_event_data);
}

// 0x42e6e0
e_simulation_event_type c_simulation_unit_grenade_release_event_definition::event_type()
{
    return _simulation_event_type_unit_grenade_release;
}

// @address 0x42e6e8 ?event_type_name@c_simulation_unit_grenade_release_event_definition@@UAAPBDXZ
const char *c_simulation_unit_grenade_release_event_definition::event_type_name()
{
    return "unit_throw_release";
}

// 0x42e748
e_simulation_event_type c_simulation_unit_melee_initiate_event_definition::event_type()
{
    return _simulation_event_type_unit_melee_initiate;
}

// 0x42e750
const char *c_simulation_unit_melee_initiate_event_definition::event_type_name()
{
    return "unit_melee_initiate";
}

// 0x42e778
const char *c_simulation_unit_melee_damage_event_definition::event_type_name()
{
    return "unit_melee_damage";
}

// 0x42e798
e_simulation_event_type c_simulation_unit_melee_clang_event_definition::event_type()
{
    return _simulation_event_type_unit_melee_clang;
}

// 0x42e7a0
const char * c_simulation_unit_melee_clang_event_definition::event_type_name()
{
    return "unit_melee_clang";
}

// @address 0x429c18 ?payload_size@c_simulation_unit_melee_clang_event_definition@@UAAJXZ
long c_simulation_unit_melee_clang_event_definition::payload_size() { return 28; }
// @address 0x42d298 ?number_of_entity_references@c_simulation_unit_melee_clang_event_definition@@UAAJXZ
long c_simulation_unit_melee_clang_event_definition::number_of_entity_references() { return 0; }
// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_melee_clang_event_definition@@UAA_NXZ
bool c_simulation_unit_melee_clang_event_definition::reference_delays_entity_deletion() { return false; }
// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_melee_clang_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_melee_clang_event_definition::event_can_be_transmitted(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return true; }

s_simulation_event_requirements c_simulation_unit_melee_clang_event_definition::event_requirements(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 0;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 67;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_unit_melee_clang_event_definition@@UAAXJPAX@Z
void c_simulation_unit_melee_clang_event_definition::prepare_event_data_for_gameworld(long, void *) {}

// 0x42e7c0
e_simulation_event_type c_simulation_vehicle_flip_event_definition::event_type()
{
    return _simulation_event_type_vehicle_flip;
}

// 0x42e7c8
const char *c_simulation_vehicle_flip_event_definition::event_type_name()
{
    return "vehicle_flip";
}

// 0x42e7e8
e_simulation_event_type c_simulation_vehicle_trick_event_definition::event_type()
{
    return _simulation_event_type_vehicle_trick;
}

// 0x42e7f0
const char *c_simulation_vehicle_trick_event_definition::event_type_name()
{
    return "vehicle_trick";
}

// 0x42e810
e_simulation_event_type c_simulation_unit_pickup_event_definition::event_type()
{
    return _simulation_event_type_unit_pickup;
}

// 0x42e818
const char *c_simulation_unit_pickup_event_definition::event_type_name()
{
    return "unit_pickup";
}

e_simulation_event_type c_simulation_unit_equipment_use_event_definition::event_type()
{
    return _simulation_event_type_unit_equipment_use;
}

// 0x42e868
const char *c_simulation_unit_equipment_use_event_definition::event_type_name()
{
    return "unit_equipment_use";
}

// @address 0x42e888 ?event_type@c_simulation_unit_equipment_pickup_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_unit_equipment_pickup_event_definition::event_type()
{
    return _simulation_event_type_unit_equipment_pickup;
}

// @address 0x42e890 ?event_type_name@c_simulation_unit_equipment_pickup_event_definition@@UAAPBDXZ
const char *c_simulation_unit_equipment_pickup_event_definition::event_type_name()
{
    return "unit_equipment_pickup";
}

// @address 0x3fc598 ?payload_size@c_simulation_unit_equipment_pickup_event_definition@@UAAJXZ
long c_simulation_unit_equipment_pickup_event_definition::payload_size() { return 2; }
// @address 0x3fc598 ?number_of_entity_references@c_simulation_unit_equipment_pickup_event_definition@@UAAJXZ
long c_simulation_unit_equipment_pickup_event_definition::number_of_entity_references() { return 2; }
// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_equipment_pickup_event_definition@@UAA_NXZ
bool c_simulation_unit_equipment_pickup_event_definition::reference_delays_entity_deletion() { return false; }
// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_equipment_pickup_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_equipment_pickup_event_definition::event_can_be_transmitted(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return true; }
// @address 0x42d298 ?minimum_required_bits@c_simulation_unit_equipment_pickup_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_unit_equipment_pickup_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return 0; }
// @address 0x4311b8 ?calculate_relevance@c_simulation_unit_equipment_pickup_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_equipment_pickup_event_definition::calculate_relevance(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}
// @address 0x42e7d8 ?event_requirements@c_simulation_unit_equipment_pickup_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_unit_equipment_pickup_event_definition::event_requirements(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 0;
    return requirements;
}
// @address 0x5bec98 ?event_payload_encode@c_simulation_unit_equipment_pickup_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_unit_equipment_pickup_event_definition::event_payload_encode(long, const void *, c_bitstream *, bool) {}
// @address 0x42d380 ?event_payload_decode@c_simulation_unit_equipment_pickup_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_unit_equipment_pickup_event_definition::event_payload_decode(long, void *, c_bitstream *, bool) { return true; }
// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_unit_equipment_pickup_event_definition@@UAAXJPAX@Z
void c_simulation_unit_equipment_pickup_event_definition::prepare_event_data_for_gameworld(long, void *) {}

// 0x42e8a0
// @address 0x42e8a0 ?entity_type@c_simulation_unit_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_unit_entity_definition::entity_type()
{
    return _simulation_entity_type_unit;
}

// 0x42e8a8
// @address 0x42e8a8 ?entity_type_name@c_simulation_unit_entity_definition@@UAAPBDXZ
const char *c_simulation_unit_entity_definition::entity_type_name()
{
    return "unit";
}

// 0x42e8b8
// @address 0x42e8b8 ?state_data_size@c_simulation_unit_entity_definition@@UAAJXZ
long c_simulation_unit_entity_definition::state_data_size()
{
    return sizeof(s_simulation_unit_state_data);
}

// @address 0x42d280 ?update_flag_count@c_simulation_unit_entity_definition@@UAAJXZ
long c_simulation_unit_entity_definition::update_flag_count()
{
    return 32;
}

// @address 0x42e8c8 ?initial_update_mask@c_simulation_unit_entity_definition@@UAAKXZ
unsigned long c_simulation_unit_entity_definition::initial_update_mask()
{
    return 0xB7FFDB3F;
}

// 0x42e908
// @address 0x42e908 ?build_object_creation_data@c_simulation_unit_entity_definition@@UAAXJJPAX@Z
void c_simulation_unit_entity_definition::build_object_creation_data(long object_index, long creation_data_size, void *creation_data)
{
    s_simulation_unit_creation_data *unit_creation = (s_simulation_unit_creation_data *)creation_data;
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(object_index, 8195);
    memset(unit_creation, 0, sizeof(*unit_creation));
    object_build_creation_data(object_index, unit_creation);
    player_appearance_initialize(&unit_creation->appearance);
    if (unit->unit.player_index != -1) {
        const player_datum *player = player_get(unit->unit.player_index);
        unit_creation->appearance = player->configuration.client.appearance;
    } else {
        const s_object_definition *model = (const s_object_definition *)tag_get('obje', unit->object.definition_index);
        if (unit->object.variant_index != -1 && model->model.index != -1) {
            const s_model_definition *render_model = (const s_model_definition *)tag_get('hlmt', model->model.index);
            const s_model_variant *variant = &((s_model_variant *)render_model->variants.address)[unit->object.variant_index];
            unit_creation->model_variant = variant->name;
        } else {
            unit_creation->model_variant = 0;
        }
    }
    unit_creation->team_index = unit->unit.team;
}

// 0x42ea58
// @address 0x42ea58 ?build_baseline_state_data@c_simulation_unit_entity_definition@@UAA_NJPBXJPAX@Z
bool c_simulation_unit_entity_definition::build_baseline_state_data(long creation_data_size, const void *creation_data, long state_data_size, void *state_data)
{
    s_simulation_unit_state_data *state = (s_simulation_unit_state_data *)state_data;
    bool result = false;
    memset(state, 0, sizeof(*state));
    if (object_build_baseline_state_data((const s_simulation_object_creation_data *)creation_data, (s_simulation_object_state_data *)state_data)) {
        state->control_values[0] = -1;
        state->control_values[1] = -1;
        state->control_values[2] = -1;
        state->control_values[3] = -1;
        state->control_values[4] = -1;
        state->parent_vehicle_entity_index = -1;
        state->parent_seat_index = -1;
        state->desired_aiming_vector = state->forward;
        state->desired_weapon_set.set_identifier = -1;
        state->desired_weapon_set.weapon_indices[0] = -1;
        state->desired_weapon_set.weapon_indices[1] = -1;
        for (unsigned int i = 0; i < 4; i++) {
            state->weapons[i].definition_index = -1;
            state->weapons[i].multiplayer_weapon_identifier = -1;
            state->weapons[i].rounds_loaded = 0;
        }
        int definition_index = ((const s_simulation_object_creation_data *)creation_data)->definition_index;
        if (definition_index != -1) {
            const unit_definition *definition = (const unit_definition *)tag_get('unit', definition_index);
            short t = definition->grenade_type;
            if (t >= 0 && t < 4) {
                short k = definition->grenade_count;
                if (k >= 0)
                    state->grenade_counts[t] = k;
            }
        }
        state->equipment_definition_index = -1;
        state->equipment_index = -1;
        state->active_camo_maximum = 1.0f;
        result = true;
    }
    return result;
}

// 0x42f238
// @address 0x42f238 ?prepare_state_data_for_gameworld@c_simulation_unit_entity_definition@@UAAXKJPAX@Z
void c_simulation_unit_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long state_data_size, void *state_data)
{
    s_simulation_unit_state_data *state = (s_simulation_unit_state_data *)state_data;
    unsigned long object_update_mask = update_mask & 0x3fff;
    if (object_update_mask) {
        if (object_update_mask & (1 << 11))
            state->parent_gamestate_index = simulation_entity_get_gamestate_index(state->parent_entity_index);
    }
    if (update_mask & 0x8000)
        state->parent_vehicle_gamestate_index = simulation_entity_get_gamestate_index(state->parent_vehicle_entity_index);
    else
        state->parent_vehicle_gamestate_index = -1;
}

// 0x42fbe0
// @address 0xa0538 ?action_executing@@YA_NJW4e_action@@@Z

static __forceinline unit_datum *simulation_unit_promote_datum(long object_index)
{
    return (unit_datum *)object_get(object_index);
}

static __forceinline void simulation_unit_promote_detach(long object_index)
{
    unit_datum *unit = simulation_unit_promote_datum(object_index);
    unit->unit.simulation_entity_index = -1;
    unit->unit.simulation_actor_index = -1;
    unit->unit.simulation_player_index = -1;
    unit->unit.simulation_player_identifier = -1;
    unit->unit.simulation_actor_squad_index = -1;
    unit->unit.simulation_actor_variant_index = -1;
    unit->unit.simulation_actor_starting_location_index = -1;
}

// @address 0x42fad0 ?promote_object_to_authority@c_simulation_unit_entity_definition@@UAA_NJ@Z
bool c_simulation_unit_entity_definition::promote_object_to_authority(long object_index)
{
    unit_datum *unit = simulation_unit_promote_datum(object_index);
    if (unit->unit.simulation_actor_datum_index != -1)
    {
        long actor_index = -1;
        long character_index = -1;
        long squad_index = unit->unit.simulation_actor_squad_index;
        ::squad_get_actor_character_definition_index((short)squad_index, (short)unit->unit.simulation_actor_variant_index,
            (short)unit->unit.simulation_actor_starting_location_index, (long *)(&character_index));
        if (character_index != -1)
            actor_index = ::actor_create(object_index, character_index, squad_index, false);
        if (actor_index != -1)
            ::squad_set_actor_starting_location(actor_index, (short)unit->unit.simulation_actor_squad_index,
                (short)unit->unit.simulation_actor_variant_index, (short)unit->unit.simulation_actor_starting_location_index);
    }
    if (action_executing(object_index, _action_melee_player))
    {
        s_player_melee_action_state *control = (s_player_melee_action_state *)object_header_block_get(unit, &unit->unit.action);
        if (control->animation != -1)
            control->weapon_slot = false;
    }
    simulation_unit_promote_detach(object_index);
    return true;
}

// @address 0x42fbe0 ?entity_can_be_created@c_simulation_unit_entity_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_entity_definition::entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry)
{
 bool result=true;
 const s_simulation_object_creation_data *creation=(const s_simulation_object_creation_data *)entity->creation_data;
 const s_simulation_unit_state_data *state=(const s_simulation_unit_state_data *)entity->state_data;
 if(state->parent_vehicle_entity_index!=-1) {
  c_simulation_view_telemetry_provider *provider=telemetry->provider;
  if(!provider->entity_is_active(state->parent_vehicle_entity_index)) result=false;
 }
 result=result && object_can_be_created(creation,state,telemetry);
 return result;
}

// 0x42fc68
// @address 0x42fc68 ?creation_minimum_required_bits@c_simulation_unit_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_unit_entity_definition::creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry)
{
    return object_creation_required_bits() + player_appearance_required_bits() + 38;
}

// 0x42fc70
// @address 0x42fc70 ?write_creation_description_to_string@c_simulation_unit_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_unit_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer)
{
    float relevance = 0.0f;
    simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
    csnzprintf(buffer, buffer_size, "unit creation: relevance=%5.3f", relevance);
}

// 0x42fe78
// @address 0x42fe78 ?rotate_entity_indices@c_simulation_unit_entity_definition@@UAAXPBUs_simulation_entity@@@Z
void c_simulation_unit_entity_definition::rotate_entity_indices(const s_simulation_entity *entity)
{
    s_simulation_unit_state_data *state_data = (s_simulation_unit_state_data *)entity->state_data;
    int handle = state_data->parent_entity_index;
    if (handle != -1) {
        int salt = ((unsigned int)handle >> 28) + 1;
        salt %= 16;
        state_data->parent_entity_index = (salt << 28) | (handle & 0x3FF);
    }

    handle = state_data->parent_vehicle_entity_index;
    if (handle == -1)
        return;

    int salt = ((unsigned int)handle >> 28) + 1;
    salt %= 16;
    state_data->parent_vehicle_entity_index = (salt << 28) | (handle & 0x3FF);
}

// 0x42fee8
// @address 0x42fee8 ?update_minimum_required_bits@c_simulation_unit_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_unit_entity_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update)
{
    unsigned long update_mask = update->update_mask;
    long minimum_bits;
    object_calculate_update_requirements(entity, update, &minimum_bits);
    minimum_bits += 18;
    if ((update_mask & 0x4000) != 0) {
        long bits = 58;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x8000) != 0) {
        long bits = 53;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x10000) != 0) {
        long bits = 51;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x20000) != 0) {
        long bits = 43;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x3c0000) != 0) {
        long bits = 50;
        bits += simulation_definition_table_index_bits();
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x3c00000) != 0) {
        long bits = 58;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x4000000) != 0) {
        long bits = 64;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x8000000) != 0) {
        long bits = 39;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x10000000) != 0) {
        long bits = 47;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x20000000) != 0) {
        long bits = 35;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x40000000) != 0) {
        long bits = 40;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    if ((update_mask & 0x80000000) != 0) {
        long bits = 4;
        minimum_bits = minimum_bits > bits ? bits : minimum_bits;
    }
    return minimum_bits;
}

// 0x4311b8
// @address 0x4311b8 ?calculate_relevance@c_simulation_unit_melee_damage_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_melee_damage_event_definition::calculate_relevance(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(
        this, event, relevance, telemetry, 0);
}

// 0x4311c8
// @address 0x4311c8 ?write_description_to_string@c_simulation_unit_enter_vehicle_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_unit_enter_vehicle_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "unit enter vehicle: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x4313d0
// @address 0x4313d0 ?write_description_to_string@c_simulation_unit_exit_vehicle_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_unit_exit_vehicle_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "unit exit vehicle: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x431430
void c_simulation_unit_exit_vehicle_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_exit_vehicle_event_data *event_data = (const s_simulation_unit_exit_vehicle_event_data *)payload;
    packet->write_integer("parent-seat-index", event_data->parent_seat_index, 6);
    packet->write_bool("detach-on-exit", event_data->detach_on_exit);
}

// 0x431470
// @address 0x431470 ?event_payload_decode@c_simulation_unit_exit_vehicle_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_unit_exit_vehicle_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_unit_exit_vehicle_event_data *event_data = (s_simulation_unit_exit_vehicle_event_data *)payload;
    event_data->parent_seat_index = packet->read_integer("parent-seat-index", 6);
    event_data->detach_on_exit = packet->read_bool("detach-on-exit");
    return true;
}

// 0x431590
// @address 0x431590 ?write_description_to_string@c_simulation_unit_board_vehicle_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_unit_board_vehicle_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "unit board vehicle: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x4315f0
void c_simulation_unit_board_vehicle_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_board_vehicle_event_data *event_data = (const s_simulation_unit_board_vehicle_event_data *)payload;
    packet->write_integer("boarding-seat-index", event_data->boarding_seat_index, 6);
}

// 0x431608
bool c_simulation_unit_board_vehicle_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_unit_board_vehicle_event_data *event_data = (s_simulation_unit_board_vehicle_event_data *)payload;
    event_data->boarding_seat_index = packet->read_integer("boarding-seat-index", 6);
    return true;
}

// 0x431838
long c_simulation_unit_grenade_initiate_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
    const s_simulation_unit_grenade_initiate_event_data *event_data = (const s_simulation_unit_grenade_initiate_event_data *)event->get_payload_data();
    long minimum_bits = 1;
    switch (event_data->throw_type) {
    case 0: minimum_bits += 2; break;
    case 1: minimum_bits += simulation_definition_table_index_bits(); break;
    }
    return minimum_bits;
}

// 0x431868
void c_simulation_unit_grenade_initiate_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "unit-throw-initiate: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x4318c8
void c_simulation_unit_grenade_initiate_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_grenade_initiate_event_data *event_data = (const s_simulation_unit_grenade_initiate_event_data *)payload;
    packet->write_integer("unit-throw-type", event_data->throw_type, 1);
    switch (event_data->throw_type) {
    case 0: packet->write_integer("grenade-type", event_data->grenade_definition_index, 2); break;
    case 1: simulation_write_definition_index("unit-throw-definition-index", packet, event_data->grenade_definition_index); break;
    }
}

// 0x431950
bool c_simulation_unit_grenade_initiate_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_unit_grenade_initiate_event_data *event_data = (s_simulation_unit_grenade_initiate_event_data *)payload;
    bool result = true;
    event_data->throw_type = (short)packet->read_integer("unit-throw-type", 1);
    if (event_data->throw_type >= 0 && event_data->throw_type < 2) {
        switch (event_data->throw_type) {
        case 0: event_data->grenade_definition_index = packet->read_integer("grenade-type", 2); break;
        case 1: event_data->grenade_definition_index = simulation_read_definition_index("unit-throw-definition-index", packet); break;
        }
        if (event_data->throw_type == 0)
            result = event_data->grenade_definition_index >= 0 && event_data->grenade_definition_index < 4;
        else
            result = tag_try_and_get_unsafe('item', event_data->grenade_definition_index) != 0;
    } else {
        result = false;
    }
    return result;
}

// 0x431c10
long c_simulation_unit_grenade_release_event_definition::payload_size()
{
    return sizeof(s_simulation_unit_grenade_release_event_data);
}

// 0x431c18
long c_simulation_unit_grenade_release_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
    const s_simulation_unit_grenade_initiate_event_data *event_data = (const s_simulation_unit_grenade_initiate_event_data *)event->get_payload_data();
    long minimum_bits = 68;
    switch (event_data->throw_type) {
    case 0: minimum_bits += 2; break;
    case 1: minimum_bits += simulation_definition_table_index_bits(); break;
    }
    return minimum_bits;
}

// 0x431c48
void c_simulation_unit_grenade_release_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "unit-throw-release: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x431fb0
// @address 0x431fb0 ?prepare_event_data_for_gameworld@c_simulation_unit_grenade_release_event_definition@@UAAXJPAX@Z
void c_simulation_unit_grenade_release_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
    s_simulation_unit_grenade_release_event_data *grenade = (s_simulation_unit_grenade_release_event_data *)payload;
    grenade->entity_gamestate_index = -1;
    if (grenade->has_entity)
        grenade->entity_gamestate_index = simulation_entity_get_gamestate_index(grenade->entity_index);
}

long c_simulation_unit_melee_damage_event_definition::payload_size()
{
    return sizeof(s_simulation_unit_melee_damage_event_data);
}

// 0x4322d8
// @address 0x4322d8 ?write_description_to_string@c_simulation_unit_melee_damage_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_unit_melee_damage_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "unit-melee-damage: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x432b28
// @address 0x432b28 ?write_description_to_string@c_simulation_unit_melee_initiate_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_unit_melee_initiate_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "unit-melee-initiate: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x432b88
void c_simulation_unit_melee_initiate_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_melee_initiate_event_data *event_data = (const s_simulation_unit_melee_initiate_event_data *)payload;
    packet->write_integer("melee-type", event_data->melee_type, 2);
    packet->write_integer("prediction-type", event_data->prediction_type, 2);
}

// 0x432bd8
bool c_simulation_unit_melee_initiate_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_unit_melee_initiate_event_data *event_data = (s_simulation_unit_melee_initiate_event_data *)payload;
    bool result = true;
    event_data->melee_type = (short)packet->read_integer("melee-type", 2);
    event_data->prediction_type = (short)packet->read_integer("prediction-type", 2);
    result = result && event_data->melee_type > 0 && event_data->melee_type < 4;
    result = result && event_data->prediction_type >= 0 && event_data->prediction_type < 3;
    return result;
}

union real_point3d;
union real_rectangle3d;


// @address 0x432f30 ?minimum_required_bits@c_simulation_unit_melee_clang_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_unit_melee_clang_event_definition::minimum_required_bits(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *)
{
    return 67;
}

namespace {
}

// @address 0x432f38 ?calculate_relevance@c_simulation_unit_melee_clang_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_melee_clang_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float maximum_distance)
{
    return simulation_calculate_position_relevance(&((const s_simulation_unit_melee_clang_event_data *)event)->position, maximum_distance, telemetry);
}

// @address 0x432f40 ?write_description_to_string@c_simulation_unit_melee_clang_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_unit_melee_clang_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float maximum_distance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "unit-melee-clang: relevance=%5.3f", calculate_relevance(event, telemetry, maximum_distance));
}

// 0x433120
// @address 0x433120 ?write_description_to_string@c_simulation_vehicle_flip_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_vehicle_flip_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "vehicle-flip relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x433180
bool c_simulation_vehicle_flip_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    bool result = false;
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 2);
    long vehicle_index = simulation_gamestate_entity_try_and_get_object_index(references[1], 0x1003);
    if (unit_index != -1 && vehicle_index != -1) {
        object_datum *vehicle = (object_datum *)object_get(vehicle_index);
        object_datum *unit = (object_datum *)object_get(unit_index);
        if (!vehicle->m_damage_flags.test(_object_damage_body_depleted_bit) &&
            !unit->m_damage_flags.test(_object_damage_body_depleted_bit)) {
            if (distance_squared3d(&vehicle->bounding_center, &unit->bounding_center) <= 16.0f) {
                action_request request;
                memset(&request, 0, sizeof(request));
                request.target_index = vehicle_index;
                request.type = 21;
                result = action_submit(unit_index, (const action_request *)&request);
            }
        }
    }
    return result;
}

// 0x4332a8
// @address 0x4332a8 ?write_description_to_string@c_simulation_vehicle_trick_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_vehicle_trick_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "vehicle-trick: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x433308
void c_simulation_vehicle_trick_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_vehicle_trick_event_data *event_data = (const s_simulation_vehicle_trick_event_data *)payload;
    packet->write_integer("vehicle-trick-type", event_data->vehicle_trick_type, 2);
}

// 0x433320
bool c_simulation_vehicle_trick_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_vehicle_trick_event_data *event_data = (s_simulation_vehicle_trick_event_data *)payload;
    event_data->vehicle_trick_type = packet->read_integer("vehicle-trick-type", 2);
    return true;
}

// 0x433400
long c_simulation_unit_pickup_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
    const s_simulation_unit_pickup_event_data *event_data = (const s_simulation_unit_pickup_event_data *)event->get_payload_data();
    long minimum_bits = 3;
    minimum_bits += simulation_definition_table_index_bits();
    if (event_data->pickup_type == 1)
        minimum_bits += 16;
    return minimum_bits;
}

// 0x433420
// @address 0x433420 ?write_description_to_string@c_simulation_unit_pickup_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_unit_pickup_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "unit-pickup : relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x433480
void c_simulation_unit_pickup_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_pickup_event_data *event_data = (const s_simulation_unit_pickup_event_data *)payload;
    packet->write_integer("unit-pickup-type", (short)event_data->pickup_type, 3);
    simulation_write_definition_index("unit-pickup-definition-index", packet, event_data->definition_index);
    if (event_data->pickup_type == 1)
        packet->write_raw_data("count", &event_data->count, 16);
}


static __forceinline void simulation_unit_read_raw_data(c_bitstream *stream, const char *name, void *buffer, long size_in_bits)
{
    unsigned char *data = (unsigned char *)buffer;
    for (; size_in_bits >= 64; size_in_bits -= 64, data += 8)
        *(unsigned __int64 *)data = bitstream_read_window_bits(stream, 64);
    if (size_in_bits > 0)
    {
        unsigned __int64 value = bitstream_read_window_bits(stream, size_in_bits) << (64 - size_in_bits);
        for (unsigned long byte_count = (unsigned long)size_in_bits / 8; byte_count != 0; --byte_count)
        {
            *data++ = (unsigned char)(value >> 56);
            value <<= 8;
        }
        if (size_in_bits % 8)
            *data = (unsigned char)(value >> 56);
    }
}

// @address 0x4334f8 ?event_payload_decode@c_simulation_unit_pickup_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_unit_pickup_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_unit_pickup_event_data *event_data = (s_simulation_unit_pickup_event_data *)payload;
    event_data->pickup_type = (unsigned short)packet->read_integer("unit-pickup-type", 3);
    event_data->definition_index = simulation_read_definition_index("unit-pickup-definition-index", packet);
    if (event_data->pickup_type == 1)
        simulation_unit_read_raw_data(packet, "count", &event_data->count, 16);
    return true;
}

// 0x433880
// @address 0x433880 ?write_description_to_string@c_simulation_unit_equipment_use_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_unit_equipment_use_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
  csnzprintf(buffer, buffer_size, "unit-equipment-use : relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x4338e0
// @address 0x4338e0 ?event_payload_encode@c_simulation_unit_equipment_use_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_unit_equipment_use_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_equipment_use_event_data *event_data = (const s_simulation_unit_equipment_use_event_data *)payload;
    simulation_write_definition_index("equipment-definition-index", packet, event_data->equipment_definition_index);
}

// 0x4338f8
// @address 0x4338f8 ?event_payload_decode@c_simulation_unit_equipment_use_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_unit_equipment_use_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_unit_equipment_use_event_data *event_data = (s_simulation_unit_equipment_use_event_data *)payload;
    event_data->equipment_definition_index = simulation_read_definition_index("equipment-definition-index", packet);
    return true;
}

// @address 0x433a08 ?write_description_to_string@c_simulation_unit_equipment_pickup_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_unit_equipment_pickup_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "unit-equipment-pickup : relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// @address 0x433a68 ?apply_game_event@c_simulation_unit_equipment_pickup_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_equipment_pickup_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 0x1003);
    long equipment_index = simulation_gamestate_entity_try_and_get_object_index(references[1], 8);
    if (unit_index != -1 && equipment_index != -1 && unit_can_pickup_equipment(unit_index, equipment_index)) {
        action_request request;
        memset(&request, 0, sizeof(request));
        request.target_index = equipment_index;
        request.type = 58;
        action_submit(unit_index, (const action_request *)&request);
    }
    return false;
}


// @address 0x42fd70 ?entity_creation_decode@c_simulation_unit_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_unit_entity_definition::entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool flag)
{
    s_simulation_unit_creation_data *data = (s_simulation_unit_creation_data *)creation_data;
    bool object_valid = object_creation_decode(data, packet, flag);
    bool appearance_valid = player_appearance_decode(packet, &data->appearance, _player_configuration_encoding_full);
    unsigned char *buffer = (unsigned char *)&data->model_variant;
    unsigned __int64 value = bitstream_read_window_bits(packet, 32) << 32;
    unsigned long bytes = 4;
    do {
        *buffer++ = (unsigned char)(value >> 56);
        value <<= 8;
        bytes--;
    } while (bytes != 0);
    if (packet->read_bool("team-valid"))
        data->team_index = packet->read_integer("team", 5);
    else
        data->team_index = -1;
    bool result = !packet->overflowed();
    result = result && object_valid;
    result = result && appearance_valid;
    return result;
}



// @address 0x42fce0 ?entity_creation_encode@c_simulation_unit_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
void c_simulation_unit_entity_definition::entity_creation_encode(long creation_data_size, const void *creation_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_creation_data *data = (const s_simulation_unit_creation_data *)creation_data;
    object_creation_encode(data, packet, quantize);
    player_appearance_encode(packet, &data->appearance, _player_configuration_encoding_full);
    packet->write_raw_data("variant-name", &data->model_variant, 32);
    packet->write_bool("team-valid", data->team_index != -1);
    if (data->team_index != -1)
        packet->write_integer("team", data->team_index, 5);
}


namespace {
}

// @address 0x4314d0 ?apply_game_event@c_simulation_unit_exit_vehicle_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_exit_vehicle_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_simulation_unit_exit_vehicle_event_data *event_data = (const s_simulation_unit_exit_vehicle_event_data *)payload;
    bool result = false;
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 0x1003);
    long vehicle_index = simulation_gamestate_entity_try_and_get_object_index(references[1], 0x1003);
    if (unit_index != -1 && vehicle_index != -1) {
        unit_datum *unit = (unit_datum *)object_get(unit_index);
        if (unit->object.parent_object_index != -1 && unit->object.parent_object_index == vehicle_index) {
            if (unit->unit.parent_seat_index != -1 && unit->unit.parent_seat_index == event_data->parent_seat_index) {
                action_submit_event(unit_index, (event_data->detach_on_exit != 0) + 13);
                result = true;
            }
        }
    }
    return result;
}

// @address 0x431648 ?apply_game_event@c_simulation_unit_board_vehicle_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_board_vehicle_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_simulation_unit_board_vehicle_event_data *event_data = (const s_simulation_unit_board_vehicle_event_data *)payload;
    bool result = false;
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 0x1003);
    if (unit_index != -1) {
        unit_datum *unit = (unit_datum *)object_get(unit_index);
        long vehicle_index = simulation_gamestate_entity_try_and_get_object_index(references[1], 2);
        if (!unit->object.m_damage_flags.test(_object_damage_body_depleted_bit) && vehicle_index != -1) {
            if (event_data->boarding_seat_index != -1 && !action_executing(unit_index, _action_vehicle_board)) {
                unit_datum *vehicle = (unit_datum *)object_get(vehicle_index);
                unit_definition *vehicle_definition = (unit_definition *)tag_get('vehi', vehicle->object.definition_index);
                if (event_data->boarding_seat_index < vehicle_definition->seats.count()) {
                    unit_seat *seat = &vehicle_definition->seats[event_data->boarding_seat_index];
                    if ((bool)(_rotl(seat->flags, 21) & 1) && seat->index != -1) {
                        bool board = false;
                        if (unit->object.parent_object_index == -1) {
                            board = true;
                        } else if (unit->object.parent_object_index != vehicle_index || unit->unit.parent_seat_index != event_data->boarding_seat_index) {
                            action_submit_event(unit_index, 20);
                            board = true;
                        }
                        if (vehicle->object.m_damage_flags.test(_object_damage_body_depleted_bit))
                            board = false;
                        if (board) {
                            long occupant_index = ::unit_seat_get_occupant(vehicle_index, (short)event_data->boarding_seat_index);
                            if (occupant_index != -1 && occupant_index != unit_index)
                                action_submit_event(occupant_index, 20);
                            action_request request;
                            memset(&request, 0, sizeof(request));
                            request.type = 17;
                            request.unit_index = vehicle_index;
                            request.seat_index = (short)event_data->boarding_seat_index;
                            request.flag_a = false;
                            request.flag_b = false;
                            action_submit(unit_index, (const action_request *)&request);
                        }
                        if (unit->object.parent_object_index == vehicle_index && unit->unit.parent_seat_index == event_data->boarding_seat_index) {
                            action_submit_event(unit_index, 16);
                            result = true;
                        }
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x431228 ?apply_game_event@c_simulation_unit_enter_vehicle_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_enter_vehicle_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_simulation_unit_enter_vehicle_event_data *event_data = (const s_simulation_unit_enter_vehicle_event_data *)payload;
    bool result = false;
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 0x1003);
    long parent_index = simulation_gamestate_entity_try_and_get_object_index(references[1], 0x1003);
    if (unit_index != -1 && parent_index != -1) {
        unit_datum *unit = (unit_datum *)object_get(unit_index);
        unit_datum *parent = (unit_datum *)object_get(parent_index);
        bool can_enter;
        long occupant_index;
        if (!unit->object.m_damage_flags.test(_object_damage_body_depleted_bit) &&
            unit->object.parent_object_index == -1 &&
            !parent->object.m_damage_flags.test(_object_damage_body_depleted_bit)) {
            unit_definition *parent_definition =
                (unit_definition *)tag_get('unit', parent->object.definition_index);
            if (event_data->seat_index >= 0 &&
                event_data->seat_index < parent_definition->seats.count() &&
                unit_can_enter_seat(unit_index, parent_index, (short)event_data->seat_index, &occupant_index, &can_enter, 0)) {
                if (distance_squared3d(&unit->object.bounding_center, &parent->object.bounding_center) < 16.0f) {
                    action_request request;
                    memset(&request, 0, sizeof(request));
                    memset(&request, 0, sizeof(request));
                    request.type = 17;
                    request.unit_index = parent_index;
                    request.seat_index = (short)event_data->seat_index;
                    result = action_submit(unit_index, (const action_request *)&request);
                }
            }
        }
    }
    return result;
}

namespace {
}

const float k_simulation_game_units_quantize_maximum = 1.0f;
const float k_simulation_game_units_quantize_minimum = 0.0f;
// @address 0x432338 ?event_payload_encode@c_simulation_unit_melee_damage_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_unit_melee_damage_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_melee_damage_event_data *event_data = (const s_simulation_unit_melee_damage_event_data *)payload;
    simulation_write_quantized_position(packet, &event_data->position, 12, false, 0);
    packet->write_integer("direction", bitstream_encode_unit_vector(&event_data->direction), 19);
    simulation_write_definition_index("damage-definition-index", packet, event_data->damage_definition_index);
    packet->write_bool("clang-damage-definition-exists", event_data->clang_damage_definition_index != -1);
    if (event_data->clang_damage_definition_index != -1)
        simulation_write_definition_index("clang-damage-definition-index", packet, event_data->clang_damage_definition_index);
    packet->write_bool("breakable-surface-exists", event_data->breakable_surface_index != -1);
    if (event_data->breakable_surface_index != -1) {
        packet->write_integer("breakable-surface-index", event_data->breakable_surface_index, 8);
        packet->write_integer("breakable-surface-set-index", event_data->breakable_surface_set_index, 11);
        packet->write_bool("instanced-geometry-instance-exists", event_data->instanced_geometry_instance_index != -1);
        if (event_data->instanced_geometry_instance_index != -1)
            packet->write_integer("instanced-geometry-instance-index", event_data->instanced_geometry_instance_index, 12);
        packet->write_integer("surface-index", event_data->surface_index, 17);
    }
    packet->write_integer("material-index", event_data->material_index + 1, 8);
    packet->write_integer("global-material-index", event_data->global_material_index + 1, 6);
    packet->write_integer("scale", quantize_real(event_data->scale, 0.0f, k_simulation_game_units_quantize_maximum, 8, false, false), 8);
    packet->m_encoded_value_bits += 8;
    packet->write_integer("damage-reporting-info", event_data->damage_reporting_info, 8);
}

namespace {
static __forceinline long simulation_game_units_unit_get_weapon(const unit_datum *unit, signed char weapon_index)
{
    long weapon_object_index = -1;
    if (weapon_index != -1)
        weapon_object_index = unit->unit.weapon_object_indices[weapon_index];
    return weapon_object_index;
}
static __forceinline void simulation_game_units_unit_set_desired_grenade_type(long unit_index, short grenade_type)
{
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    unit->unit.desired_grenade_index = (signed char)grenade_type;
    unit->unit.current_grenade_index = unit->unit.desired_grenade_index;
}
}

// @address 0x431a30 ?apply_game_event@c_simulation_unit_grenade_initiate_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_grenade_initiate_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_simulation_unit_grenade_initiate_event_data *event_data = (const s_simulation_unit_grenade_initiate_event_data *)payload;
    bool result = false;
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 1);
    if (unit_index != -1) {
        unit_datum *unit = (unit_datum *)object_get(unit_index);
        if (!unit->object.m_damage_flags.test(_object_damage_body_depleted_bit)) {
            long current_weapon_index = simulation_game_units_unit_get_weapon(unit, unit->unit.current_weapon_index);
            long desired_weapon_index = simulation_game_units_unit_get_weapon(unit, unit->unit.desired_weapon_index);
            if (current_weapon_index != -1)
                weapon_interrupt_reload_and_clear_timer(current_weapon_index, 1, false);
            if (desired_weapon_index != -1)
                weapon_interrupt_reload_and_clear_timer(desired_weapon_index, 1, false);
            action_request grenade_request;
            action_request throw_request;
            switch (event_data->throw_type) {
            case 0: {
                memset(&grenade_request, 0, sizeof(grenade_request));
                simulation_game_units_unit_set_desired_grenade_type(unit_index, (short)event_data->grenade_definition_index);
                memset(&grenade_request, 0, sizeof(grenade_request));
                grenade_request.type = 56;
                grenade_request.grenade_initiate.initiate = true;
                action_submit(unit_index, (const action_request *)&grenade_request);
                result = true;
                break;
            }
            case 1: {
                memset(&throw_request, 0, sizeof(throw_request));
                memset(&throw_request, 0, sizeof(throw_request));
                throw_request.type = 57;
                throw_request.throw_item.definition_index = event_data->grenade_definition_index;
                throw_request.throw_item.initiate = true;
                action_submit(unit_index, (const action_request *)&throw_request);
                result = true;
                break;
            }
            }
        }
    }
    return result;
}

// @address 0x431ca8 ?event_payload_encode@c_simulation_unit_grenade_release_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_unit_grenade_release_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_grenade_release_event_data *event_data = (const s_simulation_unit_grenade_release_event_data *)payload;
    packet->write_integer("unit-throw-type", event_data->throw_type, 1);
    switch (event_data->throw_type) {
    case 0: packet->write_integer("grenade-type", event_data->grenade_type, 2); break;
    case 1: simulation_write_definition_index("unit-throw-definition-index", packet, event_data->grenade_type); break;
    }
    simulation_write_quantized_position(packet, &event_data->position, 16, false, 0);
    packet->write_integer("aiming-vector", bitstream_encode_unit_vector(&event_data->aiming_vector), 19);
    packet->write_bool("target-valid", event_data->has_entity);
    if (event_data->has_entity) {
        if (quantize) {
            replication_entity_index_encode(packet, event_data->entity_index);
        } else {
            packet->write_integer("target-gamestate-index", event_data->entity_gamestate_index, 32);
        }
        packet->write_bool("target-model-valid", event_data->target_model_index != -1);
        if (event_data->target_model_index != -1)
            packet->write_integer("model-target", event_data->target_model_index, 5);
        packet->write_vector("lead-vector", &event_data->lead_vector, 0.01f, 15.0f, 8);
    }
}

// @address 0x431ff0 ?apply_game_event@c_simulation_unit_grenade_release_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_grenade_release_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_simulation_unit_grenade_release_event_data *event_data = (const s_simulation_unit_grenade_release_event_data *)payload;
    bool result = false;
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 1);
    if (unit_index != -1) {
        unit_datum *unit = (unit_datum *)object_get(unit_index);
        bool can_throw = true;
        bool forced = false;
        if (unit->object.m_damage_flags.test(_object_damage_body_depleted_bit)) {
            if (unit->object.flags_d6 & (1 << 3)) {
                can_throw = false;
            } else {
                forced = true;
                ((unit_datum *)object_get(unit_index))->object.flags_d6 |= (1 << 3);
            }
        }
        if (can_throw) {
            union { real_vector3d aim_direction; real_point3d target_position; };
            bool aim_valid = false;
            if (event_data->has_entity && event_data->entity_gamestate_index != -1) {
                long target_object_index = simulation_gamestate_entity_get_object_index(event_data->entity_gamestate_index);
                if (target_object_index != -1) {
                    ::aim_assist_get_target_position(target_object_index, event_data->target_model_index, &target_position);
                    aim_direction.i = target_position.x + event_data->lead_vector.i - event_data->position.x;
                    aim_direction.j = target_position.y + event_data->lead_vector.j - event_data->position.y;
                    aim_direction.k = target_position.z + event_data->lead_vector.k - event_data->position.z;
                    aim_valid = normalize3d(&aim_direction) > 0.0f;
                }
            }
            action_request grenade_request;
            action_request throw_request;
            switch (event_data->throw_type) {
            case 0: {
                memset(&grenade_request, 0, sizeof(grenade_request));
                simulation_game_units_unit_set_desired_grenade_type(unit_index, (short)event_data->grenade_type);
                memset(&grenade_request, 0, sizeof(grenade_request));
                grenade_request.type = 56;
                grenade_request.grenade_initiate.release = true;
                grenade_request.grenade_initiate.forced = forced;
                grenade_request.grenade_initiate.position = event_data->position;
                grenade_request.grenade_initiate.direction = aim_valid ? aim_direction : event_data->aiming_vector;
                action_submit(unit_index, (const action_request *)&grenade_request);
                result = true;
                break;
            }
            case 1: {
                memset(&throw_request, 0, sizeof(throw_request));
                memset(&throw_request, 0, sizeof(throw_request));
                throw_request.type = 57;
                throw_request.throw_item.definition_index = event_data->grenade_type;
                throw_request.throw_item.release = true;
                throw_request.throw_item.forced = forced;
                throw_request.throw_item.position = event_data->position;
                throw_request.throw_item.direction = aim_valid ? aim_direction : event_data->aiming_vector;
                action_submit(unit_index, (const action_request *)&throw_request);
                result = true;
                break;
            }
            }
        }
    }
    return result;
}


// @address 0x4324e0 ?event_payload_decode@c_simulation_unit_melee_damage_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_unit_melee_damage_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_unit_melee_damage_event_data *event_data = (s_simulation_unit_melee_damage_event_data *)payload;
    ::simulation_read_quantized_position(packet, &event_data->position, 12, 0);
    long direction = packet->read_integer("direction", 19);
    bitstream_decode_unit_vector(direction, &event_data->direction);
    event_data->damage_definition_index = simulation_read_definition_index("damage-definition-index", packet);
    if (packet->read_bool("clang-damage-definition-exists"))
        event_data->clang_damage_definition_index = simulation_read_definition_index("clang-damage-definition-index", packet);
    else
        event_data->clang_damage_definition_index = -1;
    if (packet->read_bool("breakable-surface-exists")) {
        event_data->breakable_surface_index = (short)packet->read_integer("breakable-surface-index", 8);
        event_data->breakable_surface_set_index = (short)packet->read_integer("breakable-surface-set-index", 11);
        if (packet->read_bool("instanced-geometry-instance-exists"))
            event_data->instanced_geometry_instance_index = packet->read_integer("instanced-geometry-instance-index", 12);
        else
            event_data->instanced_geometry_instance_index = -1;
        event_data->surface_index = packet->read_integer("surface-index", 17);
    } else {
        event_data->breakable_surface_index = -1;
        event_data->breakable_surface_set_index = -1;
        event_data->instanced_geometry_instance_index = -1;
        event_data->surface_index = -1;
    }
    event_data->material_index = packet->read_integer("material-index", 8) - 1;
    event_data->global_material_index = packet->read_integer("global-material-index", 6) - 1;
    event_data->scale = dequantize_real(packet->read_integer("scale", 8), 0.0f, k_simulation_game_units_quantize_maximum, 8, false);
    event_data->damage_reporting_info = (unsigned char)packet->read_integer("damage-reporting-info", 8);
    return true;
}

class c_motor_task;

namespace {
static __forceinline void simulation_game_units_interrupt_action(long unit_index, long action)
{
    c_motor_task *task = motor_task_find_by_action(unit_index, (e_action)action);
    if (task)
        motor_system_interrupted(unit_index, task, true);
}
}

// @address 0x432c80 ?apply_game_event@c_simulation_unit_melee_initiate_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_melee_initiate_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_simulation_unit_melee_initiate_event_data *event_data = (const s_simulation_unit_melee_initiate_event_data *)payload;
    long target_index = -1;
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 1);
    if (references[1] != -1)
        target_index = simulation_gamestate_entity_try_and_get_object_index(references[1], 0x1003);
    if (unit_index != -1) {
        unit_datum *unit = (unit_datum *)object_get(unit_index);
        long melee_recovery_ticks = 0;
        long minimum_recovery_ticks = game_seconds_to_ticks_round(0.2f);
        if (unit->unit.player_index != -1)
            melee_recovery_ticks = player_get(unit->unit.player_index)->melee_remaining_ticks;
        if (!unit->object.m_damage_flags.test(_object_damage_body_depleted_bit) && event_data->prediction_type != 1) {
            bool predicted = false;
            if (game_globals->options.game_simulation == 4)
                predicted = true;
            if (predicted || melee_recovery_ticks < minimum_recovery_ticks) {
                action_request request;
                memset(&request, 0, sizeof(request));
                unit_datum *target_unit = (unit_datum *)object_get(unit_index);
                long current_weapon_index = simulation_game_units_unit_get_weapon(target_unit, target_unit->unit.current_weapon_index);
                long desired_weapon_index = simulation_game_units_unit_get_weapon(target_unit, target_unit->unit.desired_weapon_index);
                if (current_weapon_index != -1)
                    weapon_interrupt_reload_and_clear_timer(current_weapon_index, 1, false);
                if (desired_weapon_index != -1)
                    weapon_interrupt_reload_and_clear_timer(desired_weapon_index, 1, false);
                simulation_game_units_interrupt_action(unit_index, 61);
                simulation_game_units_interrupt_action(unit_index, 62);
                simulation_game_units_interrupt_action(unit_index, 63);
                simulation_game_units_interrupt_action(unit_index, 64);
                memset(&request, 0, sizeof(request));
                request.type = 60;
                request.melee_player.type = event_data->melee_type;
                request.melee_player.weapon_slot = event_data->prediction_type;
                request.melee_player.flag = true;
                request.melee_player.mode = true;
                request.melee_player.target = target_index;
                action_submit(unit_index, (const action_request *)&request);
            }
        }
    }
    return false;
}



// @address 0x432fa0 ?event_payload_encode@c_simulation_unit_melee_clang_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_unit_melee_clang_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_melee_clang_event_data *event_data = (const s_simulation_unit_melee_clang_event_data *)payload;
    simulation_write_definition_index("clang-effect-index", packet, event_data->definition_index);
    simulation_write_quantized_position(packet, &event_data->position, 12, false, 0);
    packet->write_integer("direction", bitstream_encode_unit_vector(&event_data->direction), 19);
}

// @address 0x433018 ?event_payload_decode@c_simulation_unit_melee_clang_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_unit_melee_clang_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_unit_melee_clang_event_data *event_data = (s_simulation_unit_melee_clang_event_data *)payload;
    event_data->definition_index = simulation_read_definition_index("clang-effect-index", packet);
    ::simulation_read_quantized_position(packet, &event_data->position, 12, 0);
    long direction = packet->read_integer("direction", 19);
    bitstream_decode_unit_vector(direction, &event_data->direction);
    return true;
}

// @address 0x433090 ?apply_game_event@c_simulation_unit_melee_clang_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_melee_clang_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_simulation_unit_melee_clang_event_data *event_data = (const s_simulation_unit_melee_clang_event_data *)payload;
    bool result = false;
    long definition_index = event_data->definition_index;
    if (tag_try_and_get_unsafe('effe', definition_index)) {
        effect_new_unattached_from_point(definition_index, &event_data->position, &event_data->direction, global_up3d, 1, 1, NULL, NULL);
        result = true;
    }
    return result;
}

// @address 0x433360 ?apply_game_event@c_simulation_vehicle_trick_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_vehicle_trick_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_simulation_vehicle_trick_event_data *event_data = (const s_simulation_vehicle_trick_event_data *)payload;
    bool result = false;
    long vehicle_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 2);
    if (vehicle_index != -1) {
        if (event_data->vehicle_trick_type >= 0 && event_data->vehicle_trick_type < k_vehicle_trick_type_count) {
            if (vehicle_get_type(vehicle_index) == _vehicle_type_alien_fighter) {
                vehicle_datum *vehicle = (vehicle_datum *)object_get(vehicle_index);
                vehicle->vehicle.get_alien_fighter()->start_trick(vehicle_index, (e_vehicle_trick_type)event_data->vehicle_trick_type);
            }
            result = true;
        }
    }
    return result;
}


namespace {
static __forceinline void simulation_game_units_write_quantized_real(c_bitstream *packet, const char *name, float value, float minimum,
    float maximum, long size_in_bits, bool exact_midpoint, bool exact_endpoints)
{
    packet->write_integer(name, quantize_real(value, minimum, maximum, size_in_bits, exact_midpoint, exact_endpoints), size_in_bits);
    packet->m_encoded_value_bits += size_in_bits;
}
static __forceinline void simulation_game_units_weapon_set_encode(c_bitstream *packet, const s_unit_weapon_set *weapon_set)
{
    packet->write_integer("set-index", weapon_set->set_identifier + 1, 5);
    packet->write_integer("primary-weapon-index", weapon_set->weapon_indices[0] + 1, 3);
    packet->write_integer("secondary-weapon-index", weapon_set->weapon_indices[1] + 1, 3);
}
}


// @address 0x430040 ?entity_update_encode@c_simulation_unit_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_unit_entity_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *written_update_mask, long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool quantize)
{
    const s_simulation_unit_state_data *state = (const s_simulation_unit_state_data *)state_data;
    bool result = false;
    if (object_update_encode(initial_update, update_mask & 0x3fff, written_update_mask, telemetry,
            (const s_simulation_object_state_data *)state, packet, maximum_bits + 18, ensure_object_position_update_quantization_inside_bsp(), quantize))
    {
        c_entity_update_encode_helper helper;
        if (helper.make_room_for_update(packet, maximum_bits, 14, 18, update_mask & 0xffffc000)) {
            if (helper.write_component_header(14, "control-exists")) {
                packet->write_bool("aiming-speed-exists", state->control_values[0] != -1);
                if (state->control_values[0] != -1)
                    packet->write_integer("aiming-speed", state->control_values[0], 4);
                packet->write_bool("looking-speed-exists", state->control_values[1] != -1);
                if (state->control_values[1] != -1)
                    packet->write_integer("looking-speed", state->control_values[1], 4);
                packet->write_bool("animation-state-exists", state->control_values[2] != -1);
                if (state->control_values[2] != -1)
                    packet->write_integer("animation-state", state->control_values[2], 9);
                packet->write_bool("aiming-mode-exists", state->control_values[3] != -1);
                if (state->control_values[3] != -1)
                    packet->write_integer("aiming-mode", state->control_values[3], 5);
                packet->write_bool("weapon-mode-exists", state->control_values[4] != -1);
                if (state->control_values[4] != -1)
                    packet->write_integer("weapon-mode", state->control_values[4], 5);
            }
            helper.finish_component();
            if (helper.write_component_header(15, "parent-vehicle-exists")) {
                if (quantize) {
                    long parent_vehicle_entity_index = state->parent_vehicle_entity_index;
                    bool parent_vehicle_active = true;
                    if (parent_vehicle_entity_index != -1 && telemetry)
                        parent_vehicle_active = telemetry->provider->entity_is_active(parent_vehicle_entity_index);
                    if (parent_vehicle_active) {
                        packet->write_bool("parent-vehicle-valid", parent_vehicle_entity_index != -1);
                        if (state->parent_vehicle_entity_index != -1) {
                            replication_entity_index_encode(packet, state->parent_vehicle_entity_index);
                            packet->write_integer("parent-seat-index", state->parent_seat_index, 6);
                        }
                    } else {
                        helper.skip_component();
                    }
                } else {
                    packet->write_bool("parent-vehicle-valid", state->parent_vehicle_gamestate_index != -1);
                    if (state->parent_vehicle_gamestate_index != -1) {
                        packet->write_integer("parent-vehicle-gamestate-index", state->parent_vehicle_gamestate_index, 32);
                        packet->write_integer("parent-seat-index", state->parent_seat_index, 6);
                    }
                }
            }
            helper.finish_component();
            if (helper.write_component_header(16, "desired-aiming-vector-exists"))
                packet->write_integer("desired-aiming-vector", bitstream_encode_unit_vector(&state->desired_aiming_vector), 19);
            helper.finish_component();
            if (helper.write_component_header(17, "desired-weapon-set-exists"))
                simulation_game_units_weapon_set_encode(packet, &state->desired_weapon_set);
            helper.finish_component();
            for (long weapon_index = 0; weapon_index < 4; weapon_index++) {
                if (helper.write_component_header(18 + weapon_index, "weapon-type-exists")) {
                    const s_simulation_unit_weapon_state *weapon = &state->weapons[weapon_index];
                    simulation_write_definition_index("weapon-definition-index", packet, weapon->definition_index);
                    packet->write_integer("multiplayer-weapon-identifier", weapon->multiplayer_weapon_identifier + 1, 4);
                    packet->write_integer("rounds-loaded", weapon->rounds_loaded, 12);
                    simulation_game_units_weapon_set_encode(packet, &state->desired_weapon_set);
                }
                helper.finish_component();
                if (helper.write_component_header(22 + weapon_index, "weapon-state-exists")) {
                    const s_simulation_unit_weapon_state *weapon = &state->weapons[weapon_index];
                    packet->write_integer("rounds-inventory", weapon->rounds_inventory, 8);
                    packet->write_integer("rounds-total", weapon->rounds_total, 11);
                    simulation_game_units_write_quantized_real(packet, "age", weapon->age, k_simulation_game_units_quantize_minimum,
                        k_simulation_game_units_quantize_maximum, 7, true, false);
                }
                helper.finish_component();
            }
            if (helper.write_component_header(26, "grenade-counts-exists"))
                packet->write_raw_data("grenade-counts", state->grenade_counts, 32);
            helper.finish_component();
            if (helper.write_component_header(27, "active-camo-exists")) {
                packet->write_bool("active-camo-active", state->active_camo_active);
                simulation_game_units_write_quantized_real(packet, "active-camo-maximum", state->active_camo_maximum,
                    k_simulation_game_units_quantize_minimum, k_simulation_game_units_quantize_maximum, 6, true, false);
                simulation_game_units_write_quantized_real(packet, "active-camo-regrowth", state->active_camo_regrowth,
                    k_simulation_game_units_quantize_minimum, k_simulation_game_units_quantize_maximum, 6, true, false);
                simulation_game_units_write_quantized_real(packet, "active-camo-level", state->active_camo_level,
                    k_simulation_game_units_quantize_minimum, k_simulation_game_units_quantize_maximum, 6, true, false);
            }
            helper.finish_component();
            if (helper.write_component_header(28, "equipment-exists"))
                simulation_write_definition_index("equipment-definition-index", packet, state->equipment_definition_index);
            helper.finish_component();
            if (helper.write_component_header(29, "hud-info-exists")) {
                packet->write_bool("hud-info-a", state->hud_info_a);
                packet->write_bool("hud-info-b", state->hud_info_b);
            }
            helper.finish_component();
            if (helper.write_component_header(30, "map-editor-flags-exist"))
                packet->write_integer("map-editor-flags", state->map_editor_flags, 8);
            helper.finish_component();
            if (helper.write_component_header(31, "control-context-exists"))
                packet->write_integer("control-context", state->control_context, 4);
            helper.finish_component();
            helper.finish_update(written_update_mask);
            result = true;
        }
    }
    return result;
}




// @address 0x430738 ?entity_update_decode@c_simulation_unit_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_unit_entity_definition::entity_update_decode(
    bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool quantize)
{
    s_simulation_unit_state_data *state = (s_simulation_unit_state_data *)state_data;
    bool valid = true;
    valid = object_update_decode(initial_update, update_mask, (s_simulation_object_state_data *)state, packet, quantize) && valid;
    if (packet->read_bool("control-exists")) {
        if (packet->read_bool("aiming-speed-exists")) {
            state->control_values[0] = (short)packet->read_integer("aiming-speed", 4);
            valid = valid && state->control_values[0] >= 0 && state->control_values[0] <= 16;
        } else {
            state->control_values[0] = -1;
        }
        if (packet->read_bool("looking-speed-exists")) {
            state->control_values[1] = (short)packet->read_integer("looking-speed", 4);
            valid = valid && state->control_values[1] >= 0 && state->control_values[1] <= 16;
        } else {
            state->control_values[1] = -1;
        }
        if (packet->read_bool("animation-state-exists")) {
            state->control_values[2] = (short)packet->read_integer("animation-state", 9);
            valid = valid && state->control_values[2] >= 0 && state->control_values[2] < 335;
        } else {
            state->control_values[2] = -1;
        }
        if (packet->read_bool("aiming-mode-exists")) {
            state->control_values[3] = (short)packet->read_integer("aiming-mode", 5);
            valid = valid && state->control_values[3] != -1;
        } else {
            state->control_values[3] = -1;
        }
        if (packet->read_bool("weapon-mode-exists")) {
            state->control_values[4] = (short)packet->read_integer("weapon-mode", 5);
            valid = valid && state->control_values[4] != -1;
        } else {
            state->control_values[4] = -1;
        }
        valid = valid && (state->control_values[0] == -1 || state->control_values[1] == -1);
        *update_mask |= 0x4000;
    }
    if (packet->read_bool("parent-vehicle-exists")) {
        if (packet->read_bool("parent-vehicle-valid")) {
            if (quantize)
                replication_entity_index_decode(packet, &state->parent_vehicle_entity_index);
            else
                state->parent_vehicle_gamestate_index = packet->read_integer("parent-vehicle-gamestate-index", 32);
            state->parent_seat_index = (short)packet->read_integer("parent-seat-index", 6);
        } else {
            state->parent_vehicle_gamestate_index = -1;
            state->parent_vehicle_entity_index = -1;
            state->parent_seat_index = -1;
        }
        *update_mask |= 0x8000;
        if ((quantize && state->parent_vehicle_entity_index == -1) || (!quantize && state->parent_vehicle_gamestate_index == -1))
            valid = valid && state->parent_seat_index == -1;
        else
            valid = valid && state->parent_seat_index >= 0 && state->parent_seat_index < 64;
    }
    if (packet->read_bool("desired-aiming-vector-exists")) {
        long desired_aiming_vector = packet->read_integer("desired-aiming-vector", 19);
        bitstream_decode_unit_vector(desired_aiming_vector, &state->desired_aiming_vector);
        *update_mask |= 0x10000;
        valid = valid && ::valid_real_normal3d(&state->desired_aiming_vector);
    }
    if (packet->read_bool("desired-weapon-set-exists")) {
        state->desired_weapon_set.set_identifier = (short)(packet->read_integer("set-index", 5) - 1);
        state->desired_weapon_set.weapon_indices[0] = (signed char)(packet->read_integer("primary-weapon-index", 3) - 1);
        state->desired_weapon_set.weapon_indices[1] = (signed char)(packet->read_integer("secondary-weapon-index", 3) - 1);
        *update_mask |= 0x20000;
        if (state->desired_weapon_set.weapon_indices[0] != -1 &&
            state->desired_weapon_set.weapon_indices[0] == state->desired_weapon_set.weapon_indices[1])
            valid = false;
    }
    for (long weapon_index = 0; weapon_index < 4; weapon_index++) {
        s_simulation_unit_weapon_state *weapon = &state->weapons[weapon_index];
        if (packet->read_bool("weapon-type-exists")) {
            weapon->definition_index = simulation_read_definition_index("weapon-definition-index", packet);
            weapon->multiplayer_weapon_identifier = (short)(packet->read_integer("multiplayer-weapon-identifier", 4) - 1);
            weapon->rounds_loaded = (short)packet->read_integer("rounds-loaded", 12);
            *update_mask |= 1 << (18 + weapon_index);
            bool weapon_valid = valid && (weapon->definition_index == -1 || weapon->definition_index != -1);
            valid = weapon_valid &
                (weapon->multiplayer_weapon_identifier == -1 ||
                    (weapon->multiplayer_weapon_identifier >= 0 && weapon->multiplayer_weapon_identifier < 9));
            state->desired_weapon_set.set_identifier = (short)(packet->read_integer("set-index", 5) - 1);
            state->desired_weapon_set.weapon_indices[0] = (signed char)(packet->read_integer("primary-weapon-index", 3) - 1);
            state->desired_weapon_set.weapon_indices[1] = (signed char)(packet->read_integer("secondary-weapon-index", 3) - 1);
            if (state->desired_weapon_set.weapon_indices[0] != -1 &&
                state->desired_weapon_set.weapon_indices[0] == state->desired_weapon_set.weapon_indices[1])
                valid = false;
        }
        if (packet->read_bool("weapon-state-exists")) {
            weapon->rounds_inventory = (short)packet->read_integer("rounds-inventory", 8);
            weapon->rounds_total = (short)packet->read_integer("rounds-total", 11);
            weapon->age = dequantize_real(packet->read_integer("age", 7), k_simulation_game_units_quantize_minimum,
                k_simulation_game_units_quantize_maximum, 7, true);
            *update_mask |= 1 << (22 + weapon_index);
        }
    }
    if (packet->read_bool("grenade-counts-exists")) {
        simulation_unit_read_raw_data(packet, "grenade-counts", state->grenade_counts, 32);
        *update_mask |= 1 << 26;
        valid = valid && state->grenade_counts[0] < 128;
        valid = valid && state->grenade_counts[1] < 128;
        valid = valid && state->grenade_counts[2] < 128;
        valid = valid && state->grenade_counts[3] < 128;
    }
    if (packet->read_bool("active-camo-exists")) {
        state->active_camo_active = packet->read_bool("active-camo-active");
        state->active_camo_maximum = dequantize_real(packet->read_integer("active-camo-maximum", 6),
            k_simulation_game_units_quantize_minimum, k_simulation_game_units_quantize_maximum, 6, true);
        state->active_camo_regrowth = dequantize_real(packet->read_integer("active-camo-regrowth", 6),
            k_simulation_game_units_quantize_minimum, k_simulation_game_units_quantize_maximum, 6, true);
        state->active_camo_level = dequantize_real(packet->read_integer("active-camo-level", 6), k_simulation_game_units_quantize_minimum,
            k_simulation_game_units_quantize_maximum, 6, true);
        *update_mask |= 1 << 27;
    }
    if (packet->read_bool("equipment-exists")) {
        state->equipment_definition_index = simulation_read_definition_index("equipment-definition-index", packet);
        *update_mask |= 1 << 28;
    }
    if (packet->read_bool("hud-info-exists")) {
        state->hud_info_a = packet->read_bool("hud-info-a");
        state->hud_info_b = packet->read_bool("hud-info-b");
        *update_mask |= 1 << 29;
    }
    if (packet->read_bool("map-editor-flags-exist")) {
        state->map_editor_flags = (signed char)packet->read_integer("map-editor-flags", 8);
        *update_mask |= 1 << 30;
    }
    if (packet->read_bool("control-context-exists")) {
        state->control_context = (unsigned char)packet->read_integer("control-context", 4);
        *update_mask |= 1 << 31;
    }
    return valid && !packet->overflowed();
}

// @address 0x430fd0 ?entity_state_lossy_compare@c_simulation_unit_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_unit_entity_definition::entity_state_lossy_compare(void *first, void *second, long state_data_size)
{
    s_simulation_unit_state_data *first_state = (s_simulation_unit_state_data *)first;
    s_simulation_unit_state_data *second_state = (s_simulation_unit_state_data *)second;
    bool result = c_simulation_object_entity_definition::object_state_compare((s_simulation_object_state_data *)first, (s_simulation_object_state_data *)second)
        && c_bitstream::compare_unit_vectors(&first_state->desired_aiming_vector, &second_state->desired_aiming_vector);
    second_state->desired_aiming_vector = *global_zero_vector3d;
    first_state->desired_aiming_vector = second_state->desired_aiming_vector;
    for (long weapon_index = 0; weapon_index < 4; weapon_index++) {
        result = result &&
            c_bitstream::compare_quantized_reals(first_state->weapons[weapon_index].age, second_state->weapons[weapon_index].age,
                k_simulation_game_units_quantize_minimum, k_simulation_game_units_quantize_maximum, 7, true, false);
        second_state->weapons[weapon_index].age = 0.0f;
        first_state->weapons[weapon_index].age = 0.0f;
    }
    result = result &&
        c_bitstream::compare_quantized_reals(first_state->active_camo_maximum, second_state->active_camo_maximum,
            k_simulation_game_units_quantize_minimum, k_simulation_game_units_quantize_maximum, 6, true, false);
    second_state->active_camo_maximum = 0.0f;
    first_state->active_camo_maximum = 0.0f;
    result = result &&
        c_bitstream::compare_quantized_reals(first_state->active_camo_regrowth, second_state->active_camo_regrowth,
            k_simulation_game_units_quantize_minimum, k_simulation_game_units_quantize_maximum, 6, true, false);
    second_state->active_camo_regrowth = 0.0f;
    first_state->active_camo_regrowth = 0.0f;
    result = result &&
        c_bitstream::compare_quantized_reals(first_state->active_camo_level, second_state->active_camo_level,
            k_simulation_game_units_quantize_minimum, k_simulation_game_units_quantize_maximum, 6, true, false);
    second_state->active_camo_level = 0.0f;
    first_state->active_camo_level = 0.0f;
    return result;
}

static __forceinline void simulation_game_units_item_message(player_iterator *iterator, long definition_index)
{
    long user_index = iterator->controller_index; if (tls_player_mapping_globals()->local_player_count < 2 && user_index >= 0 && user_index < 4) {
        const struct weapon_definition *definition =
            (const struct weapon_definition *)tag_get('weap', definition_index);
        chud_messaging_post(iterator->controller_index, definition->item.picked_up_message, _chud_message_context_self);
    }
}

// @address 0x4335a0 ?apply_game_event@c_simulation_unit_pickup_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_pickup_event_definition::apply_game_event(long reference_count, const long *references, long payload_size,
    const void *payload)
{
    const s_simulation_unit_pickup_event_data *event = static_cast<const s_simulation_unit_pickup_event_data *>(payload);
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 0x1003);
    if (unit_index != -1) {
        unit_datum *unit = (unit_datum *)object_get(unit_index);
        long player = unit->unit.player_index;
        if (player != -1) {
            player_iterator iterator = {player, -1};
            if ((short)event->pickup_type == 4 || (event->definition_index != -1 && tag_try_and_get_unsafe('item', event->definition_index))) {
                switch ((short)event->pickup_type) {
                case 0:
                    while (player_iterate_output_users(&iterator)) simulation_game_units_item_message(&iterator, event->definition_index);
                    break;
                case 2:
                    while (player_iterate_output_users(&iterator)) chud_post_grenade_pickup(iterator.controller_index, event->definition_index);
                    projectile_play_pickup_sound(unit->unit.player_index, event->definition_index);
                    break;
                case 1: {
                    const struct weapon_definition *definition =
                        (const struct weapon_definition *)tag_get('weap', event->definition_index);
                    if ((1 << definition->item.object.object_type) & 4) {
                        if (definition->pickup_sound != -1 && player_get_first_output_user(player) != -1)
                            unspatialized_impulse_sound_new(definition->pickup_sound, 1.0f);
                    } else projectile_play_pickup_sound(player, event->definition_index);
                    while (player_iterate_output_users(&iterator)) chud_post_ammo_pickup(iterator.controller_index, event->definition_index, event->count);
                    break;
                }
                case 3:
                    while (player_iterate_output_users(&iterator)) simulation_game_units_item_message(&iterator, event->definition_index);
                    projectile_play_pickup_sound(unit->unit.player_index, event->definition_index);
                    break;
                case 4:
                    teleporter_send_blocked_event(player);
                    break;
                }
            }
        }
    }
    return false;
}

// @address 0x431de8 ?event_payload_decode@c_simulation_unit_grenade_release_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_unit_grenade_release_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_unit_grenade_release_event_data *event_data = (s_simulation_unit_grenade_release_event_data *)payload;
    bool success = true;
    event_data->throw_type = (short)packet->read_integer("unit-throw-type", 1);
    if (event_data->throw_type >= 0 && event_data->throw_type < 2) {
        switch (event_data->throw_type) {
        case 0: event_data->grenade_type = packet->read_integer("grenade-type", 2); break;
        case 1: event_data->grenade_type = simulation_read_definition_index("unit-throw-definition-index", packet); break;
        }
        ::simulation_read_quantized_position(packet, &event_data->position, 16, 0);
        long aiming_quantization = (long)packet->read_integer("aiming-vector", 19);
        bitstream_decode_unit_vector(aiming_quantization, &event_data->aiming_vector);
        event_data->has_entity = packet->read_bool("target-valid");
        if (event_data->has_entity) {
            if (quantize) {
                replication_entity_index_decode(packet, &event_data->entity_index);
            } else {
                event_data->entity_gamestate_index = (long)packet->read_integer("target-gamestate-index", 32);
            }
            if (packet->read_bool("target-model-valid")) {
                event_data->target_model_index = (long)packet->read_integer("model-target", 5);
            } else {
                event_data->target_model_index = -1;
            }
            packet->read_vector("lead-vector", &event_data->lead_vector, 0.01f, 15.0f, 8);
        } else {
            event_data->entity_index = -1;
            event_data->entity_gamestate_index = -1;
            event_data->target_model_index = -1;
        }
        if (event_data->throw_type == 0)
            success = event_data->grenade_type >= 0 && event_data->grenade_type < 4;
        else
            success = tag_try_and_get_unsafe('item', event_data->grenade_type) != 0;
    }
    return success;
}
typedef char simulation_unit_actor_datum_offset[(offsetof(unit_datum, unit.simulation_actor_datum_index) == 0x170) ? 1 : -1];
typedef char simulation_unit_map_editor_offset[(offsetof(unit_datum, unit.map_editor_flags) == 0x368) ? 1 : -1];
typedef char simulation_unit_context_offset[(offsetof(unit_datum, unit.control_context) == 0x208) ? 1 : -1];
typedef char simulation_unit_camo_level_offset[(offsetof(unit_datum, unit.active_camo_level) == 0x364) ? 1 : -1];

static __forceinline long simulation_game_units_inventory_get_weapon(unit_datum *unit, short inventory_index)
{
    long weapon_index = -1;
    if (inventory_index != -1)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}


// @address 0x42eb88 ?handle_object_update@c_simulation_unit_entity_definition@@UAAKJKJPAX@Z
unsigned long c_simulation_unit_entity_definition::handle_object_update(
    long object_index, unsigned long update_mask, long state_data_size, void *state_data_raw)
{
    s_simulation_unit_state_data *state = (s_simulation_unit_state_data *)state_data_raw;
    unit_datum *object = (unit_datum *)object_get_and_verify_type(object_index, 0x2003);
    unsigned long result = 0;
    unsigned long object_update_mask = update_mask & 0x3fff;
    result |= c_simulation_object_entity_definition::object_handle_update(object_index, object_update_mask, (s_simulation_object_state_data *)state);
    if (TEST_BIT(update_mask, 14))
    {
        short player_absolute_index = -1;
        short simulation_actor_index = (short)object->unit.simulation_actor_datum_index;
        short animation_state = -1;
        short weapon_mode = -1;
        short aiming_mode = -1;
        if (object->unit.player_index != -1)
            player_absolute_index = (short)(object->unit.player_index & 0xffff);
        if (object->unit.actor_index != -1)
        {
            actor_datum *actor = datum_get<actor_datum>(actor_data, object->unit.actor_index);
            animation_state = (short)actor->squad_index;
            aiming_mode = actor->cell_index;
            weapon_mode = actor->starting_location_index;
        }
        // a unit controlled by both a player and an actor keeps its current control values
        if ((player_absolute_index == -1 || simulation_actor_index == -1) &&
            (state->control_values[0] != player_absolute_index || state->control_values[1] != simulation_actor_index ||
            state->control_values[2] != animation_state || state->control_values[4] != weapon_mode))
        {
            state->control_values[0] = player_absolute_index;
            state->control_values[1] = simulation_actor_index;
            state->control_values[2] = animation_state;
            state->control_values[3] = aiming_mode;
            state->control_values[4] = weapon_mode;
            SET_BIT(result, 14, true);
        }
    }
    if (TEST_BIT(update_mask, 15))
    {
        long parent_entity_index = -1;
        short parent_seat_index = -1;
        if (object->object.parent_object_index != -1 && (unsigned short)object->unit.parent_seat_index != 0xffff)
        {
            parent_entity_index = simulation_object_get_authoritative_entity_internal(object->object.parent_object_index, false);
            if (parent_entity_index != -1)
                parent_seat_index = object->unit.parent_seat_index;
        }
        if (state->parent_vehicle_entity_index != parent_entity_index || state->parent_seat_index != parent_seat_index)
        {
            state->parent_vehicle_entity_index = parent_entity_index;
            state->parent_seat_index = parent_seat_index;
            SET_BIT(result, 15, true);
        }
    }
    if (TEST_BIT(update_mask, 16))
    {
        if (fabs(state->desired_aiming_vector.i - object->unit.desired_aiming_vector.i) >= 0.0001f ||
            fabs(state->desired_aiming_vector.j - object->unit.desired_aiming_vector.j) >= 0.0001f ||
            fabs(state->desired_aiming_vector.k - object->unit.desired_aiming_vector.k) >= 0.0001f)
        {
            state->desired_aiming_vector = object->unit.desired_aiming_vector;
            SET_BIT(result, 16, true);
        }
    }
    if (TEST_BIT(update_mask, 17))
    {
        if (memcmp(&state->desired_weapon_set, &object->unit.desired_weapon_set, sizeof(s_unit_weapon_set)) != 0)
        {
            // match: 0x42eb88: use the aligned word view to retain the single-word copy.
            memcpy(&state->desired_weapon_set, &object->unit.desired_weapon_set_storage, sizeof(s_unit_weapon_set));
            SET_BIT(result, 17, true);
        }
    }
    for (long weapon_index = 0; weapon_index < NUMBEROF(state->weapons); weapon_index++)
    {
        if (TEST_BIT(update_mask, weapon_index + 18) || TEST_BIT(update_mask, weapon_index + 22))
        {
            long weapon_object_index = unit_inventory_try_get_weapon(object_index, (short)weapon_index);
            long definition_index = -1;
            short multiplayer_identifier = -1;
            short rounds_loaded = 0;
            long rounds_inventory = 0;
            long rounds_total = 0;
            float age = 0.0f;
            if (weapon_object_index != -1)
            {
                weapon_datum *weapon_object = (weapon_datum *)object_get_and_verify_type(weapon_object_index, 4);
                const weapon_definition *weapon_definition =
                    (const ::weapon_definition *)tag_get('weap', weapon_object->definition_index);
                definition_index = weapon_object->definition_index;
                multiplayer_identifier = weapon_object->multiplayer_weapon_identifier;
                rounds_loaded = weapon_object->simulation_weapon_identifier;
                age = weapon_object->age;
                if (weapon_definition->magazines.count() > 0)
                {
                    s_weapon_magazine *magazine = &weapon_object->magazines[0];
                    rounds_inventory = magazine->rounds_loaded;
                    rounds_total = magazine->rounds_inventory;
                }
            }
            if (TEST_BIT(update_mask, weapon_index + 18))
            {
                if (state->weapons[weapon_index].definition_index != definition_index ||
                    state->weapons[weapon_index].multiplayer_weapon_identifier != multiplayer_identifier ||
                    state->weapons[weapon_index].rounds_loaded != rounds_loaded ||
                    memcmp(&state->desired_weapon_set, &object->unit.desired_weapon_set, sizeof(s_unit_weapon_set)) != 0)
                {
                    state->weapons[weapon_index].definition_index = definition_index;
                    state->weapons[weapon_index].multiplayer_weapon_identifier = multiplayer_identifier;
                    state->weapons[weapon_index].rounds_loaded = rounds_loaded;
                    memcpy(&state->desired_weapon_set, &object->unit.desired_weapon_set_storage, sizeof(s_unit_weapon_set));
                    SET_BIT(result, weapon_index + 18, true);
                }
            }
            if (TEST_BIT(update_mask, weapon_index + 22))
            {
                if (state->weapons[weapon_index].rounds_inventory != rounds_inventory ||
                    state->weapons[weapon_index].rounds_total != rounds_total || state->weapons[weapon_index].age != age)
                {
                    state->weapons[weapon_index].rounds_inventory = (short)rounds_inventory;
                    state->weapons[weapon_index].rounds_total = (short)rounds_total;
                    state->weapons[weapon_index].age = age;
                    SET_BIT(result, weapon_index + 22, true);
                }
            }
        }
    }
    if (TEST_BIT(update_mask, 26))
    {
        memcpy(state->grenade_counts, &object->unit.grenade_counts, sizeof(state->grenade_counts));
        SET_BIT(result, 26, true);
    }
    if (TEST_BIT(update_mask, 27))
    {
        bool active_changed = false;
        bool maximum_changed = false;
        bool regrowth_changed = false;
        bool level_changed = false;
        float maximum = PIN(object->unit.active_camo_maximum, 0.0f, 1.0f);
        float regrowth = PIN(object->unit.active_camo_regrowth, 0.0f, 1.0f);
        float level = PIN(object->unit.active_camo_level, 0.0f, 1.0f);
        if (state->active_camo_active != object->unit.m_unit_flags.test(_unit_flag_active_camouflage))
        {
            state->active_camo_active = object->unit.m_unit_flags.test(_unit_flag_active_camouflage);
            active_changed = true;
        }
        if (state->active_camo_maximum != maximum)
        {
            state->active_camo_maximum = maximum;
            maximum_changed = true;
        }
        if (state->active_camo_regrowth != regrowth)
        {
            state->active_camo_regrowth = regrowth;
            regrowth_changed = true;
        }
        if (state->active_camo_level != level)
        {
            state->active_camo_level = level;
            level_changed = true;
        }
        if (active_changed || maximum_changed || regrowth_changed || level_changed)
            SET_BIT(result, 27, true);
    }
    if (TEST_BIT(update_mask, 28))
    {
        long equipment_definition = -1;
        long equipment_index = -1;
        if (object->unit.equipment_object_index != -1)
        {
            equipment_definition =
                ((unit_datum *)object_get_and_verify_type(object->unit.equipment_object_index, 0xffffffff))
                    ->object.definition_index;
            equipment_index = object->unit.equipment_pickup_time;
        }
        if (state->equipment_definition_index != equipment_definition || state->equipment_index != equipment_index)
        {
            state->equipment_definition_index = equipment_definition;
            state->equipment_index = equipment_index;
            SET_BIT(result, 28, true);
        }
    }
    if (TEST_BIT(update_mask, 29))
    {
        bool hud_a = false;
        bool hud_b = false;
        if (object->unit.player_index != -1)
        {
            player_datum *player = player_get(object->unit.player_index);
            hud_a = TEST_BIT(player->flags, 14);
            hud_b = TEST_BIT(player->flags, 15);
        }
        if (state->hud_info_a != hud_a || state->hud_info_b != hud_b)
        {
            state->hud_info_a = hud_a;
            state->hud_info_b = hud_b;
            SET_BIT(result, 29, true);
        }
    }
    if (TEST_BIT(update_mask, 30))
    {
        if (state->map_editor_flags != object->unit.map_editor_flags.get_unsafe())
        {
            state->map_editor_flags = object->unit.map_editor_flags.get_unsafe();
            SET_BIT(result, 30, true);
        }
    }
    if (TEST_BIT(update_mask, 31))
    {
        if (state->control_context != object->unit.control_context.get_unsafe())
        {
            state->control_context = object->unit.control_context.get_unsafe();
            SET_BIT(result, 31, true);
        }
    }
    return result;
}


namespace {
static __forceinline long simulation_game_units_first_bsp_index(unsigned long mask)
{
    unsigned long first_bit = mask & -mask;
    return 31 - _CountLeadingZeros(first_bit);
}
static __forceinline s_global_material_definition *simulation_game_units_melee_get_material(short material_index)
{
    s_global_material_definition *result = 0;
    if (material_index != -1 && material_index >= 0 && material_index < global_game_globals->material_count)
        result = &((s_global_material_definition *)global_game_globals->materials.address)[material_index];
    return result;
}
}

// @address 0x432678 ?apply_game_event@c_simulation_unit_melee_damage_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_melee_damage_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_simulation_unit_melee_damage_event_data *event_data = (const s_simulation_unit_melee_damage_event_data *)payload;
    bool result = false;
    long target_index = -1;
    short material_index = -1;
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 1);
    if (references[1] != -1)
        target_index = simulation_gamestate_entity_get_object_index(references[1]);
    long bsp_index = simulation_game_units_first_bsp_index(g_active_structure_bsp_mask);
    long breakable_surface_a = event_data->breakable_surface_set_index;
    long variant_index = event_data->instanced_geometry_instance_index;
    long set_index = event_data->surface_index;
    long breakable_surface_b = event_data->breakable_surface_index;
    if (breakable_surface_b != -1 && breakable_surface_is_valid_set(bsp_index, variant_index, breakable_surface_a, breakable_surface_b, set_index) == 0) {
        breakable_surface_b = -1;
        breakable_surface_a = -1;
        variant_index = -1;
        set_index = -1;
        bsp_index = -1;
    }
    unsigned char reporting_info = event_data->damage_reporting_info;
    if ((unsigned long)(reporting_info & ~3) >= 244)
        reporting_info = reporting_info & 3;
    short event_material = event_data->material_index;
    if (event_material != -1) {
        if (event_material >= 0 && event_material < global_game_globals->material_count)
            material_index = event_material;
    } else {
        material_index = -1;
    }
    s_global_material_definition *checked_material = simulation_game_units_melee_get_material(material_index);
    if (checked_material == 0)
        material_index = k_global_material_type_invalid.m_index;
    if (unit_index != -1) {
        unit_datum *target;
        if (target_index != -1)
            target = (unit_datum *)object_get(target_index);
        else
            target = 0;
        unit_datum *unit = (unit_datum *)object_get(unit_index);
        s_player_melee_action_state *action = (s_player_melee_action_state *)object_header_block_get(unit, &unit->unit.action);
        if (!unit->object.m_damage_flags.test(_object_damage_body_depleted_bit)) {
            long damage_definition_index = event_data->damage_definition_index;
            if (damage_definition_index == -1 || tag_try_and_get_unsafe('jpt!', damage_definition_index) != 0) {
                long clang_damage_definition_index = event_data->clang_damage_definition_index;
                if (clang_damage_definition_index == -1 || tag_try_and_get_unsafe('jpt!', clang_damage_definition_index) != 0) {
                    do {
                    if (target_index != -1) {
                        float reach = target->object.bounding_radius + 2.0f;
                        real_vector3d vector;
                        vector_from_points3d(&unit->object.bounding_center, &target->object.bounding_center, &vector);
                        // c2 sorts this sum by scalarized field id, which follows first direct reference: keep i, k, j
                        if (vector.i * vector.i + vector.k * vector.k + vector.j * vector.j > reach * reach)
                            break;
                    }
                    {
                        if (!action_executing(unit_index, _action_melee_player) || action->silent == 0) {
                            if (action->last_melee_time == -1
                                || game_time_globals->time - action->last_melee_time >= game_seconds_to_ticks_round(0.3f)) {
                                s_simulation_unit_melee_damage_target damage_target;
                                damage_target.global_material_index = -1;
                                memset(&damage_target, 0, sizeof(damage_target));
                                bool valid_a = false;
                                bool valid_b = false;
                                damage_target.breakable_surface_set_index = -1;
                                damage_target.object_index = -1;
                                damage_target.breakable_surface_index = -1;
                                damage_target.breakable_surface_instance_index = -1;
                                damage_target.breakable_surface_bsp_index = -1;
                                damage_target.breakable_surface_cluster_index = -1;
                                damage_target.model_region_index = -1;
                                memset(&damage_target, 0, sizeof(damage_target));
                                s_damage_owner owner;
                                if (unit->unit.player_index == -1 && unit->unit.controller_player_index != -1) {
                                    owner.set(unit_index, unit->unit.controller_player_index, unit->unit.team);
                                } else {
                                    damage_compute_damage_owner_from_object_index(unit_index, &owner);
                                }
                                damage_target.damage_scale = event_data->scale;
                                damage_target.breakable_surface_set_index = bsp_index;
                                damage_target.object_index = target_index;
                                damage_target.position = event_data->position;
                                damage_target.breakable_surface_instance_index = (short)breakable_surface_b;
                                damage_target.breakable_surface_index = (short)breakable_surface_a;
                                damage_target.breakable_surface_bsp_index = variant_index;
                                damage_target.breakable_surface_cluster_index = set_index;
                                damage_target.global_material_index = material_index;
                                damage_target.hit_material_flags = (unsigned char)reporting_info;
                                damage_target.model_region_index = event_data->global_material_index;
                                damage_target.direction = event_data->direction;
                                unit_validate_synchronized_melee_damage(unit_index, damage_definition_index, clang_damage_definition_index,
                                    &owner, &damage_target, &valid_a, &valid_b);
                                s_global_material_definition *material = simulation_game_units_melee_get_material(material_index);

                                long effect_index;
                                if (valid_b)
                                    effect_index = event_data->clang_damage_definition_index;
                                else
                                    effect_index = event_data->damage_definition_index;
                                if (material != 0 && effect_index != -1)
                                {
                                    c_global_material_type material_type;
                                    material_type.m_index = material_index;
                                    unit_cause_melee_damage_response(unit_index, effect_index, -1, target_index, material_type, 0, 0);
                                }
                                unit_handle_synchronized_melee_damage(unit_index, event_data->damage_definition_index,
                                    event_data->clang_damage_definition_index, &owner, &damage_target, valid_a, valid_b);
                            }
                        }
                    }
                    } while (0);
                }
            }
        }
        unit_update_melee_action_state(unit_index);
    }
    return result;
}

static __forceinline const s_datum_header *simulation_game_units_player_try_and_get_absolute(const s_data_array *players, long absolute_index)
{
    if (absolute_index != -1) {
        if (absolute_index >= 0 && absolute_index < players->first_unallocated) {
            s_datum_header *player = (s_datum_header *)(players->data + absolute_index * players->size);
            if (player->identifier != 0)
                return player;
        }
    }
    return 0;
}

static __forceinline long simulation_game_units_player_index_from_absolute_index(long absolute_index)
{
    long index = -1;
    if (absolute_index != -1 && simulation_game_units_player_try_and_get_absolute(player_data, absolute_index))
        index = datum_absolute_index_to_index(player_data, absolute_index);
    return index;
}

// @address 0x42f2a8 ?create_object@c_simulation_unit_entity_definition@@UAAJJPBXPAKJ0@Z
long c_simulation_unit_entity_definition::create_object(long creation_data_size, const void *creation_data, unsigned long *update_mask,
    long initial_state_data_size, const void *initial_state_data)
{
    const s_simulation_unit_creation_data *creation = (const s_simulation_unit_creation_data *)creation_data;
    const s_simulation_unit_creation_state *state = (const s_simulation_unit_creation_state *)initial_state_data;
    object_placement_data placement;
    long object_index = -1;
    if (creation->definition_index != -1 && tag_try_and_get_unsafe('bipd', creation->definition_index)) {
        object_placement_data_new(&placement, creation->definition_index, -1, 0);
        object_setup_placement_data(creation, state, update_mask, &placement);
        long player_index = simulation_game_units_player_index_from_absolute_index(state->player_absolute_index);
        real_rgb_color change_colors[4];
        bool use_change_colors;
        game_engine_get_change_colors(player_index, &creation->appearance, (e_game_team)creation->team_index, change_colors, &use_change_colors);
        if (use_change_colors) {
            placement.active_change_colors |= 15;
            memcpy(placement.change_colors, change_colors, sizeof(change_colors));
        }
        model_customization_get_overrides_for_placement(&creation->appearance, &placement);
        placement.model_variant_index = creation->model_variant;
        object_index = object_create_object(creation, state, update_mask, &placement);
        if (object_index != -1) {
            unit_delete_all_weapons(object_index);
            unit_datum *unit = (unit_datum *)object_get_and_verify_type(object_index, 8195);
            if (unit->unit.player_index == -1)
                unit->unit.team = (e_game_team)creation->team_index;
        }
    }
    return object_index;
}

static __forceinline long simulation_game_units_current_equipment(const unit_datum *unit)
{
    // match: 0x433940: volatile read keeps the lwz r3,640(r5) reload after tag_try_and_get_unsafe (plain read is CSEd, -24B)
    const volatile long &equipment_object_index = unit->unit.equipment_object_index;
    return equipment_object_index;
}
static __forceinline void simulation_game_units_use_current_equipment(long unit_index, const unit_datum *unit)
{
    long equipment_index = simulation_game_units_current_equipment(unit);
    if (equipment_index != -1)
        equipment_activate(equipment_index, unit_index, 1);
}
// @address 0x433940 ?apply_game_event@c_simulation_unit_equipment_use_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_unit_equipment_use_event_definition::apply_game_event(long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_simulation_unit_equipment_use_event_data *event_data = (const s_simulation_unit_equipment_use_event_data *)payload;
    long unit_index = simulation_gamestate_entity_try_and_get_object_index(references[0], 0x1003);
    if (unit_index != -1)
    {
        const unit_datum *unit = (const unit_datum *)object_get_and_verify_type(unit_index, 0x1003);
        long equipment_index = unit->unit.equipment_object_index;
        if (equipment_index != -1)
        {
            long equipment_definition_index = event_data->equipment_definition_index;
            if (equipment_definition_index != -1)
            {
                const unit_datum *equipment = (const unit_datum *)object_get_and_verify_type(
                    equipment_index, 0xffffffff);
                if (equipment->object.definition_index == equipment_definition_index && tag_try_and_get_unsafe('eqip', equipment_definition_index))
                    simulation_game_units_use_current_equipment(unit_index, unit);
            }
        }
    }
    return false;
}

struct s_unit_weapon_set;

static inline void simulation_game_units_set_desired_weapon_set(long unit_index, const s_unit_weapon_set *weapon_set)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 0x2003);
    unit->unit.desired_weapon_set = *weapon_set;
    unit->unit.weapon_set_identifier = unit->unit.desired_weapon_set.set_identifier;
}

static inline void simulation_game_units_set_weapon_state(long unit_index, long weapon_index, long rounds_loaded, long rounds_inventory, float age)
{
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    if (unit_index != -1)
    {
        unit->unit.predicted_weapon_states[weapon_index].rounds_loaded = (short)rounds_loaded;
        unit->unit.predicted_weapon_states[weapon_index].rounds_inventory = (short)rounds_inventory;
        unit->unit.predicted_weapon_states[weapon_index].age = age;
        unit_apply_predicted_weapon_state(unit_index, weapon_index);
    }
}

static __forceinline void simulation_game_units_input_user_from_unit(long unit_index, long *input_user_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 0x2003);
    if (unit->unit.player_index != -1)
        *input_user_index = tls_player_mapping_globals()->player_input_user_mapping[datum_index_to_absolute_index(unit->unit.player_index)];
}

static inline void simulation_game_units_set_predicted_controller(
    long unit_index, long player_index, long control_a, long control_b, long control_c, long control_d)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 0x2003);
    unit->unit.simulation_player_index = player_index;
    unit->unit.simulation_player_identifier = control_a;
    unit->unit.simulation_actor_identifier = control_b;
    unit->unit.simulation_control_index = control_c;
    unit->unit.simulation_control_identifier = control_d;
    if (player_index != -1)
    {
        long previous_unit_index = player_get(player_index)->unit_index;
        if (previous_unit_index != -1 && previous_unit_index != unit_index)
            unit_update_predicted_controller(previous_unit_index);
    }
    unit_update_predicted_controller(unit_index);
}

static inline long simulation_game_units_apply_player_index_convert(const short *absolute_index)
{
    return datum_absolute_index_to_index(player_data, *absolute_index);
}

static __forceinline long simulation_game_units_apply_player_index_from_absolute(s_data_array *players, const short *absolute_index)
{
    long player_index = -1;
    if (simulation_game_units_player_try_and_get_absolute(players, *absolute_index))
        player_index = simulation_game_units_apply_player_index_convert(absolute_index);
    return player_index;
}

// @address 0x42f440 ?apply_object_update@c_simulation_unit_entity_definition@@UAAXJKJPBX@Z
void c_simulation_unit_entity_definition::apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data_raw)
{
    const s_simulation_unit_state_data *state = (const s_simulation_unit_state_data *)state_data_raw;
    unsigned long object_update_mask = update_mask & 0x3fff;
    if (object_update_mask != 0)
        object_apply_update(object_index, object_update_mask, (const s_simulation_object_state_data *)state_data_raw);
    if (TEST_BIT(update_mask, 15))
    {
        long vehicle_object_index = -1;
        short seat_index = state->parent_seat_index;
        if (state->parent_vehicle_gamestate_index != -1)
            vehicle_object_index = simulation_gamestate_entity_get_object_index(state->parent_vehicle_gamestate_index);
        if (vehicle_object_index != -1)
        {
            object_datum *parent = (object_datum *)object_get(vehicle_object_index);
            if (((1 << parent->object_type) & 0x1003) != 0)
            {
                const unit_definition *definition =
                    (const unit_definition *)tag_get('unit', parent->definition);
                if (!(seat_index >= 0 && seat_index < definition->seats.count()))
                {
                    vehicle_object_index = -1;
                    seat_index = -1;
                }
            }
            else
            {
                vehicle_object_index = -1;
                seat_index = -1;
            }
        }
        else if (seat_index != -1)
        {
            seat_index = -1;
        }
        unit_datum *object = (unit_datum *)object_get(object_index);
        object->unit.predicted_vehicle_index = vehicle_object_index;
        object->unit.predicted_vehicle_seat_index = seat_index;
        unit_update_predicted_vehicle(object_index);
    }
    if (TEST_BIT(update_mask, 16))
    {
        unit_set_facing(object_index, &state->desired_aiming_vector, &state->desired_aiming_vector);
    }
    if (TEST_BIT(update_mask, 17))
    {
        simulation_game_units_set_desired_weapon_set(object_index, (const s_unit_weapon_set *)&state->desired_weapon_set);
    }
    for (unsigned long weapon_index = 0; weapon_index < 4; weapon_index++)
    {
        const s_simulation_unit_weapon_state *weapon = &state->weapons[weapon_index];
        if (TEST_BIT(update_mask, weapon_index + 18))
        {
            if (weapon->definition_index == -1 || tag_try_and_get_unsafe('weap', weapon->definition_index) != 0)
                unit_set_weapon_type(object_index, weapon_index, weapon->definition_index, weapon->multiplayer_weapon_identifier, weapon->rounds_loaded,
                    (const s_unit_weapon_set *)&state->desired_weapon_set);
        }
        if (TEST_BIT(update_mask, weapon_index + 22))
        {
            simulation_game_units_set_weapon_state(object_index, weapon_index, weapon->rounds_inventory, weapon->rounds_total, weapon->age);
        }
    }
    if (TEST_BIT(update_mask, 26))
    {
        unit_datum *object = (unit_datum *)object_get(object_index);
        object->unit.grenade_counts[0] = state->grenade_counts[0];
        object->unit.grenade_counts[1] = state->grenade_counts[1];
        object->unit.grenade_counts[2] = state->grenade_counts[2];
        object->unit.grenade_counts[3] = state->grenade_counts[3];
    }
    if (TEST_BIT(update_mask, 27))
    {
        unit_datum *object = (unit_datum *)object_get(object_index);
        object->unit.active_camo_maximum = state->active_camo_maximum;
        object->unit.active_camo_regrowth = state->active_camo_regrowth;
        if (state->active_camo_active)
            unit_active_camouflage_enable(object_index, 4.0f, false);
        else
            unit_active_camouflage_disable(object_index, 4.0f);
        object->unit.active_camo_level = state->active_camo_level;
    }
    if (TEST_BIT(update_mask, 28))
    {
        unit_datum *object = (unit_datum *)object_get(object_index);
        if (object->unit.equipment_object_index != -1)
            unit_delete_current_equipment(object_index);
        long equipment_definition = state->equipment_definition_index;
        if (equipment_definition != -1)
        {
            object_placement_data placement;
            object_placement_data_new(&placement, equipment_definition, object_index, 0);
            long equipment_index = object_new(&placement);
            if (equipment_index != -1)
                unit_add_equipment_to_inventory(object_index, equipment_index);
        }
    }
    if (TEST_BIT(update_mask, 29))
    {
        unit_datum *object = (unit_datum *)object_get(object_index);
        long player_index = object->unit.player_index;
        if (player_index != -1)
        {
            player_datum *player = player_get(player_index);
            if (state->hud_info_a)
                player->flags |= 0x4000;
            else
                player->flags &= ~0x4000;
            if (state->hud_info_b)
                player->flags |= 0x8000;
            else
                player->flags &= ~0x8000;
        }
    }
    if (TEST_BIT(update_mask, 30))
    {
        c_flags<e_unit_map_editor_helper_flags, unsigned char, 1> map_editor_flags;
        map_editor_flags.set_unsafe(state->map_editor_flags);
        unit_datum *object = (unit_datum *)object_get_and_verify_type(object_index, 0x2003);
        bool flags_valid = true;
        flags_valid = flags_valid && map_editor_flags.valid();
        if (flags_valid)
        {
            if (object->unit.map_editor_flags.test(_unit_map_editor_helper_flag_0) != map_editor_flags.test(_unit_map_editor_helper_flag_0))
            {
                long parent_index = object_index;
                if (parent_index != -1)
                {
                    long ancestor_index;
                    do
                    {
                        ancestor_index = parent_index;
                        parent_index = ((unit_datum *)object_get(parent_index))->object.parent_object_index;
                    } while (parent_index != -1);
                    if (ancestor_index != object_index)
                        object_detach(object_index);
                }
                if (!map_editor_flags.test(_unit_map_editor_helper_flag_0))
                {
                    object_activate(object_index);
                    object_set_hidden(object_index, false);
                    if (!(bool)((((object_datum *)object)->physics_flags >> 7) & 1))
                        object_reconnect_to_physics(object_index);
                }
                else
                {
                    object_deactivate(object_index, false);
                    object_set_hidden(object_index, true);
                    if ((bool)((((object_datum *)object)->physics_flags >> 7) & 1))
                        object_disconnect_from_physics(object_index);
                }
            }
            object->unit.map_editor_flags = map_editor_flags;
        }
    }
    if (TEST_BIT(update_mask, 31))
    {
        unit_datum *object = (unit_datum *)object_get(object_index);
        object->unit.control_context.m_storage = state->control_context;
        if (object->unit.player_index != -1)
        {
            long input_user_index;
            simulation_game_units_input_user_from_unit(object_index, &input_user_index);
            player_control_set_user_unit(input_user_index, -1);
            player_control_set_user_unit(input_user_index, object_index);
        }
    }
    if (TEST_BIT(update_mask, 14))
    {
        long control_unit_index = simulation_game_units_apply_player_index_from_absolute(player_data, &state->control_values[0]);
        simulation_game_units_set_predicted_controller(object_index, control_unit_index, state->control_values[1], state->control_values[2],
            state->control_values[3], state->control_values[4]);
    }
}

// @address 0x48b78 ??_7c_simulation_unit_entity_definition@@6B@
// @address 0x709f28 ??__Eg_simulation_unit_entity_definition@@YAXXZ
// @address 0x70c7f0 ??__Fg_simulation_unit_entity_definition@@YAXXZ
// @address 0x7bb484 g_simulation_unit_entity_definition
c_simulation_unit_entity_definition g_simulation_unit_entity_definition;

extern "C" {
// @address 0x48c0c ??_7c_simulation_unit_enter_vehicle_event_definition@@6B@
// @address 0x7bb488 g_simulation_unit_enter_vehicle_event_definition
c_simulation_unit_enter_vehicle_event_definition g_simulation_unit_enter_vehicle_event_definition;
// @address 0x48c44 ??_7c_simulation_unit_exit_vehicle_event_definition@@6B@
// @address 0x7bb48c g_simulation_unit_exit_vehicle_event_definition
c_simulation_unit_exit_vehicle_event_definition g_simulation_unit_exit_vehicle_event_definition;
// @address 0x48c7c ??_7c_simulation_unit_board_vehicle_event_definition@@6B@
// @address 0x7bb490 g_simulation_unit_board_vehicle_event_definition
c_simulation_unit_board_vehicle_event_definition g_simulation_unit_board_vehicle_event_definition;
// @address 0x48cb4 ??_7c_simulation_unit_grenade_initiate_event_definition@@6B@
// @address 0x7bb494 g_simulation_unit_grenade_initiate_event_definition
c_simulation_unit_grenade_initiate_event_definition g_simulation_unit_grenade_initiate_event_definition;
// @address 0x48cec ??_7c_simulation_unit_grenade_release_event_definition@@6B@
// @address 0x7bb498 g_simulation_unit_grenade_release_event_definition
c_simulation_unit_grenade_release_event_definition g_simulation_unit_grenade_release_event_definition;
// @address 0x48d24 ??_7c_simulation_unit_melee_initiate_event_definition@@6B@
// @address 0x7bb49c g_simulation_unit_melee_initiate_event_definition
c_simulation_unit_melee_initiate_event_definition g_simulation_unit_melee_initiate_event_definition;
// @address 0x48d5c ??_7c_simulation_unit_melee_damage_event_definition@@6B@
// @address 0x7bb4a0 g_simulation_unit_melee_damage_event_definition
c_simulation_unit_melee_damage_event_definition g_simulation_unit_melee_damage_event_definition;
// @address 0x7bb4a4 g_simulation_unit_melee_clang_event_definition
c_simulation_unit_melee_clang_event_definition g_simulation_unit_melee_clang_event_definition;
// @address 0x48dcc ??_7c_simulation_vehicle_flip_event_definition@@6B@
// @address 0x7bb4a8 g_simulation_vehicle_flip_event_definition
c_simulation_vehicle_flip_event_definition g_simulation_vehicle_flip_event_definition;
// @address 0x48e04 ??_7c_simulation_vehicle_trick_event_definition@@6B@
// @address 0x7bb4ac g_simulation_vehicle_trick_event_definition
c_simulation_vehicle_trick_event_definition g_simulation_vehicle_trick_event_definition;
// @address 0x48e3c ??_7c_simulation_unit_pickup_event_definition@@6B@
// @address 0x7bb4b0 g_simulation_unit_pickup_event_definition
c_simulation_unit_pickup_event_definition g_simulation_unit_pickup_event_definition;
// @address 0x48e74 ??_7c_simulation_unit_equipment_use_event_definition@@6B@
// @address 0x7bb4b4 g_simulation_unit_equipment_use_event_definition
c_simulation_unit_equipment_use_event_definition g_simulation_unit_equipment_use_event_definition;
// @address 0x7bb4b8 g_simulation_unit_equipment_pickup_event_definition
c_simulation_unit_equipment_pickup_event_definition g_simulation_unit_equipment_pickup_event_definition;
}

typedef char simulation_unit_creation_size[(sizeof(s_simulation_unit_creation_data) == 56) ? 1 : -1];

// @address 0x42e8c0 ?creation_data_size@c_simulation_unit_entity_definition@@UAAJXZ
long c_simulation_unit_entity_definition::creation_data_size()
{
    return 56;
}

// @address 0x42d380 ?entity_type_is_gameworld_object@c_simulation_unit_entity_definition@@UAA_NXZ
bool c_simulation_unit_entity_definition::entity_type_is_gameworld_object()
{
    return true;
}

// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_unit_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_unit_entity_definition::entity_creation_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x5bec98 ?prepare_creation_data_for_gameworld@c_simulation_unit_entity_definition@@UAAXJPAX@Z
void c_simulation_unit_entity_definition::prepare_creation_data_for_gameworld(long size, void *creation_data)
{
}

// @address 0x42e8d8 ?object_required_to_join_game@c_simulation_unit_entity_definition@@UAA_NJ@Z
bool c_simulation_unit_entity_definition::object_required_to_join_game(long object_index)
{
    object_datum *object = (object_datum *)object_get(object_index);
    return !object->m_damage_flags.test(_object_damage_body_depleted_bit);
}

// @address 0x42d298 ?handle_delete_object@c_simulation_unit_entity_definition@@UAA_NJ@Z
bool c_simulation_unit_entity_definition::handle_delete_object(long object_index)
{
    return false;
}

// @address 0x42d298 ?ensure_object_position_update_quantization_inside_bsp@c_simulation_unit_entity_definition@@UAA_NXZ
bool c_simulation_unit_entity_definition::ensure_object_position_update_quantization_inside_bsp()
{
    return false;
}

// @address 0x3fc598 ?number_of_entity_references@c_simulation_unit_enter_vehicle_event_definition@@UAAJXZ
long c_simulation_unit_enter_vehicle_event_definition::number_of_entity_references()
{
    return 2;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_enter_vehicle_event_definition@@UAA_NXZ
bool c_simulation_unit_enter_vehicle_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_enter_vehicle_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_enter_vehicle_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x4311b0 ?minimum_required_bits@c_simulation_unit_enter_vehicle_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_unit_enter_vehicle_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 6;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_unit_enter_vehicle_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_enter_vehicle_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e618 ?event_requirements@c_simulation_unit_enter_vehicle_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_unit_enter_vehicle_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 6;
    return requirements;
}

// @address 0x4315f0 ?event_payload_encode@c_simulation_unit_enter_vehicle_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_unit_enter_vehicle_event_definition::event_payload_encode(long payload_size, const void *payload,
    c_bitstream *packet, bool quantize)
{
    const s_simulation_unit_enter_vehicle_event_data *event_data = (const s_simulation_unit_enter_vehicle_event_data *)payload;
    packet->write_integer("seat-index", event_data->seat_index, 6);
}

// @address 0x431608 ?event_payload_decode@c_simulation_unit_enter_vehicle_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_unit_enter_vehicle_event_definition::event_payload_decode(long payload_size, void *payload,
    c_bitstream *packet, bool quantize)
{
    s_simulation_unit_enter_vehicle_event_data *event_data = (s_simulation_unit_enter_vehicle_event_data *)payload;
    event_data->seat_index = packet->read_integer("seat-index", 6);
    return true;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_unit_enter_vehicle_event_definition@@UAAXJPAX@Z
void c_simulation_unit_enter_vehicle_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x42e628 ?event_type@c_simulation_unit_exit_vehicle_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_unit_exit_vehicle_event_definition::event_type()
{
    return _simulation_event_type_unit_exit_vehicle;
}

// @address 0x3fc598 ?number_of_entity_references@c_simulation_unit_exit_vehicle_event_definition@@UAAJXZ
long c_simulation_unit_exit_vehicle_event_definition::number_of_entity_references()
{
    return 2;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_exit_vehicle_event_definition@@UAA_NXZ
bool c_simulation_unit_exit_vehicle_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_exit_vehicle_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_exit_vehicle_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x4313c8 ?minimum_required_bits@c_simulation_unit_exit_vehicle_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_unit_exit_vehicle_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 7;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_unit_exit_vehicle_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_exit_vehicle_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e640 ?event_requirements@c_simulation_unit_exit_vehicle_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_unit_exit_vehicle_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 7;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_unit_exit_vehicle_event_definition@@UAAXJPAX@Z
void c_simulation_unit_exit_vehicle_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x42e650 ?event_type@c_simulation_unit_board_vehicle_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_unit_board_vehicle_event_definition::event_type()
{
    return _simulation_event_type_unit_board_vehicle;
}

// @address 0x3fc598 ?number_of_entity_references@c_simulation_unit_board_vehicle_event_definition@@UAAJXZ
long c_simulation_unit_board_vehicle_event_definition::number_of_entity_references()
{
    return 2;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_board_vehicle_event_definition@@UAA_NXZ
bool c_simulation_unit_board_vehicle_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_board_vehicle_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_board_vehicle_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42d280 ?minimum_required_bits@c_simulation_unit_board_vehicle_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_unit_board_vehicle_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 32;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_unit_board_vehicle_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_board_vehicle_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e668 ?event_requirements@c_simulation_unit_board_vehicle_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_unit_board_vehicle_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 32;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_unit_board_vehicle_event_definition@@UAAXJPAX@Z
void c_simulation_unit_board_vehicle_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x42e678 ?event_type@c_simulation_unit_grenade_initiate_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_unit_grenade_initiate_event_definition::event_type()
{
    return _simulation_event_type_unit_grenade_initiate;
}

// @address 0x42e650 ?payload_size@c_simulation_unit_grenade_initiate_event_definition@@UAAJXZ
long c_simulation_unit_grenade_initiate_event_definition::payload_size()
{
    return sizeof(s_simulation_unit_grenade_initiate_event_data);
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_unit_grenade_initiate_event_definition@@UAAJXZ
long c_simulation_unit_grenade_initiate_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_grenade_initiate_event_definition@@UAA_NXZ
bool c_simulation_unit_grenade_initiate_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_grenade_initiate_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_grenade_initiate_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_unit_grenade_initiate_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_grenade_initiate_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e690 ?event_requirements@c_simulation_unit_grenade_initiate_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_unit_grenade_initiate_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = c_simulation_unit_grenade_initiate_event_definition::minimum_required_bits(event, telemetry);
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_unit_grenade_initiate_event_definition@@UAAXJPAX@Z
void c_simulation_unit_grenade_initiate_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_unit_grenade_release_event_definition@@UAAJXZ
long c_simulation_unit_grenade_release_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_grenade_release_event_definition@@UAA_NXZ
bool c_simulation_unit_grenade_release_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_grenade_release_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_grenade_release_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_unit_grenade_release_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_grenade_release_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e6f8 ?event_requirements@c_simulation_unit_grenade_release_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_unit_grenade_release_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = c_simulation_unit_grenade_release_event_definition::minimum_required_bits(event, telemetry);
    return requirements;
}

// @address 0x3fc550 ?payload_size@c_simulation_unit_melee_initiate_event_definition@@UAAJXZ
long c_simulation_unit_melee_initiate_event_definition::payload_size()
{
    return sizeof(s_simulation_unit_melee_initiate_event_data);
}

// @address 0x3fc598 ?number_of_entity_references@c_simulation_unit_melee_initiate_event_definition@@UAAJXZ
long c_simulation_unit_melee_initiate_event_definition::number_of_entity_references()
{
    return 2;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_melee_initiate_event_definition@@UAA_NXZ
bool c_simulation_unit_melee_initiate_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x3fc550 ?minimum_required_bits@c_simulation_unit_melee_initiate_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_unit_melee_initiate_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 4;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_unit_melee_initiate_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_melee_initiate_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e760 ?event_requirements@c_simulation_unit_melee_initiate_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_unit_melee_initiate_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 4;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_unit_melee_initiate_event_definition@@UAAXJPAX@Z
void c_simulation_unit_melee_initiate_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x42e770 ?event_type@c_simulation_unit_melee_damage_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_unit_melee_damage_event_definition::event_type()
{
    return _simulation_event_type_unit_melee_damage;
}

// @address 0x3fc598 ?number_of_entity_references@c_simulation_unit_melee_damage_event_definition@@UAAJXZ
long c_simulation_unit_melee_damage_event_definition::number_of_entity_references()
{
    return 2;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_melee_damage_event_definition@@UAA_NXZ
bool c_simulation_unit_melee_damage_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_melee_damage_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_melee_damage_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x4322d0 ?minimum_required_bits@c_simulation_unit_melee_damage_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_unit_melee_damage_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 99;
}

// @address 0x42e788 ?event_requirements@c_simulation_unit_melee_damage_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_unit_melee_damage_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 99;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_unit_melee_damage_event_definition@@UAAXJPAX@Z
void c_simulation_unit_melee_damage_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x42d298 ?payload_size@c_simulation_vehicle_flip_event_definition@@UAAJXZ
long c_simulation_vehicle_flip_event_definition::payload_size()
{
    return 0;
}

// @address 0x3fc598 ?number_of_entity_references@c_simulation_vehicle_flip_event_definition@@UAAJXZ
long c_simulation_vehicle_flip_event_definition::number_of_entity_references()
{
    return 2;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_vehicle_flip_event_definition@@UAA_NXZ
bool c_simulation_vehicle_flip_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_vehicle_flip_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_vehicle_flip_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42d298 ?minimum_required_bits@c_simulation_vehicle_flip_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_vehicle_flip_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 0;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_vehicle_flip_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_vehicle_flip_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e7d8 ?event_requirements@c_simulation_vehicle_flip_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_vehicle_flip_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 0;
    return requirements;
}

// @address 0x5bec98 ?event_payload_encode@c_simulation_vehicle_flip_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_vehicle_flip_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
}

// @address 0x42d380 ?event_payload_decode@c_simulation_vehicle_flip_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_vehicle_flip_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    return true;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_vehicle_flip_event_definition@@UAAXJPAX@Z
void c_simulation_vehicle_flip_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x3fc550 ?payload_size@c_simulation_vehicle_trick_event_definition@@UAAJXZ
long c_simulation_vehicle_trick_event_definition::payload_size()
{
    return sizeof(s_simulation_vehicle_trick_event_data);
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_vehicle_trick_event_definition@@UAAJXZ
long c_simulation_vehicle_trick_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_vehicle_trick_event_definition@@UAA_NXZ
bool c_simulation_vehicle_trick_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_vehicle_trick_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_vehicle_trick_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42d298 ?minimum_required_bits@c_simulation_vehicle_trick_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_vehicle_trick_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 0;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_vehicle_trick_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_vehicle_trick_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e800 ?event_requirements@c_simulation_vehicle_trick_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_vehicle_trick_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 0;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_vehicle_trick_event_definition@@UAAXJPAX@Z
void c_simulation_vehicle_trick_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x42e628 ?payload_size@c_simulation_unit_pickup_event_definition@@UAAJXZ
long c_simulation_unit_pickup_event_definition::payload_size()
{
    return sizeof(s_simulation_unit_pickup_event_data);
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_unit_pickup_event_definition@@UAAJXZ
long c_simulation_unit_pickup_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_pickup_event_definition@@UAA_NXZ
bool c_simulation_unit_pickup_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_pickup_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_pickup_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_unit_pickup_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_pickup_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e828 ?event_requirements@c_simulation_unit_pickup_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_unit_pickup_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = c_simulation_unit_pickup_event_definition::minimum_required_bits(event, telemetry);
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_unit_pickup_event_definition@@UAAXJPAX@Z
void c_simulation_unit_pickup_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x3fc550 ?payload_size@c_simulation_unit_equipment_use_event_definition@@UAAJXZ
long c_simulation_unit_equipment_use_event_definition::payload_size()
{
    return sizeof(s_simulation_unit_equipment_use_event_data);
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_unit_equipment_use_event_definition@@UAAJXZ
long c_simulation_unit_equipment_use_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_unit_equipment_use_event_definition@@UAA_NXZ
bool c_simulation_unit_equipment_use_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_unit_equipment_use_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_unit_equipment_use_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42d280 ?minimum_required_bits@c_simulation_unit_equipment_use_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_unit_equipment_use_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 32;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_unit_equipment_use_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_unit_equipment_use_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x42e878 ?event_requirements@c_simulation_unit_equipment_use_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_unit_equipment_use_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 32;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_unit_equipment_use_event_definition@@UAAXJPAX@Z
void c_simulation_unit_equipment_use_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}
