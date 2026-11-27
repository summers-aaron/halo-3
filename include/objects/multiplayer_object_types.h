#ifndef OBJECTS_MULTIPLAYER_OBJECT_TYPES_H
#define OBJECTS_MULTIPLAYER_OBJECT_TYPES_H

#include <stddef.h>
#include "game/map_variant.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"

// Multiplayer object data uses twelve flag bits.
enum e_multiplayer_object_data_flags
{
    _multiplayer_object_only_visible_in_editor_bit = 0,
    _multiplayer_object_valid_initial_player_spawn_bit = 1,
    _multiplayer_object_spawn_at_start = 6,
    _multiplayer_object_spawn_on_disturbance = 7,
    _multiplayer_object_data_flag_selected = 8,
    _multiplayer_object_in_teleporter = 9,
    _multiplayer_object_teleported = 10,
    k_multiplayer_object_data_flags_count = 12
};
enum e_multiplayer_object_type
{
    _multiplayer_object_type_none = 0,
    _multiplayer_object_type_weapon = 1,
    _multiplayer_object_type_grenade = 2,
    _multiplayer_object_type_powerup = 4,
    _multiplayer_object_type_equipment = 5,
    _multiplayer_object_type_light_land_vehicle = 6,
    _multiplayer_object_type_heavy_land_vehicle = 7,
    _multiplayer_object_type_flying_vehicle = 8,
    _multiplayer_object_type_teleporter_2way = 9,
    _multiplayer_object_type_teleporter_sender = 10,
    _multiplayer_object_type_teleporter_receiver = 11,
    _multiplayer_object_type_spawn_point = 12,
    _multiplayer_object_type_respawn_zone = 13,
    _multiplayer_object_type_oddball_ball_spawn_point = 14,
    _multiplayer_object_type_flag_or_bomb_spawn_point = 15,
    _multiplayer_object_type_target_spawn_location = 16,
    _multiplayer_object_type_flag_return_or_bomb_goal_area = 17,
    _multiplayer_object_type_koth_hill = 18,
    _multiplayer_object_type_infection_haven = 19,
    _multiplayer_object_type_territory = 20,
    _multiplayer_object_type_vip_boundary = 21,
    _multiplayer_object_type_vip_destination = 22,
    _multiplayer_object_type_juggernaut_destination = 23,
    k_multiplayer_object_type_count = 24
};
enum e_multiplayer_object_remapping_policy { _multiplayer_object_remapping_policy_none = 0 };
enum e_teleporter_channel { _teleporter_channel_default = 0 };
enum e_game_engine_symmetric_placement {
    _game_engine_symmetric_placement_default = 0,
    _game_engine_symmetric_placement_symmetric = 1,
    _game_engine_symmetric_placement_asymmetric = 2
};
enum e_multiplayer_object_boundary_passability {
    _multiplayer_object_boundary_passability_disallow_players = 0,
    _multiplayer_object_boundary_passability_light_land_vehicles = 1,
    _multiplayer_object_boundary_passability_heavy_land_vehicles = 2,
    _multiplayer_object_boundary_passability_flying_vehicles = 3,
    _multiplayer_object_boundary_passability_projectiles = 4,
    k_multiplayer_object_boundary_passability_count = 5
};
enum e_multiplayer_object_spawn_timer_mode { _multiplayer_object_spawn_timer_default = 0 };
enum e_multiplayer_object_placement_spawn_flags
{
    _multiplayer_object_placement_spawn_at_start = 0,
    _multiplayer_object_placement_spawn_on_disturbance = 1
};

struct s_multiplayer_object_properties_definition;
struct s_scenario_multiplayer_object_properties;

struct s_multiplayer_object_properties
{
    c_flags_no_init<e_multiplayer_object_data_flags, short, 12> flags;
    c_enum<e_multiplayer_object_type, char, 0, 24> object_type;
    c_enum<e_multiplayer_object_boundary_shape, char, 0, 4> boundary_shape;
    c_enum<e_multiplayer_object_remapping_policy, char, 0, 3> remapping_policy;
    c_enum<e_teleporter_channel, unsigned char, 0, 26> teleporter_channel;
    c_flags_no_init<e_multiplayer_object_boundary_passability, unsigned char, 5> passability;
    c_enum<e_game_engine_symmetric_placement, unsigned char, 0, 3> symmetry;
    c_flags_no_init<e_scenario_game_engine, unsigned short, 10> game_engine_flags;
    c_enum<e_multiplayer_team_designator, unsigned short, 0, 9> team_designator;
    signed char spawn_sequence;
    unsigned char minimum_count;
    signed char maximum_count;
    c_enum<e_multiplayer_object_spawn_timer_mode, char, 0, 2> spawn_timer_mode;
    short spawn_time;
    unsigned short abandon_time;
    float box_width;
    float box_length;
    float box_height;
    float box_top;
    union { float boundary_size; float initial_spawn_weight; };
    union { float boundary_rotation; float spawn_weight_adjustment; };
    float boundary_inner_radius;
    float boundary_outer_radius;
    long boundary_center_marker;
    long spawn_marker_name;
    long boundary_effect;
    long boundary_material;
    long standard_shader;
    long opaque_shader;

    void initialize_from_object_definition_and_scenario_placement(long definition_index,
        const s_multiplayer_object_properties_definition *definition,
        const s_scenario_multiplayer_object_properties *placement);
    void validate_and_report(long definition_index);
};


typedef char multiplayer_object_initial_spawn_weight_offset[(offsetof(s_multiplayer_object_properties, initial_spawn_weight) == 0x24) ? 1 : -1];
typedef char multiplayer_object_spawn_adjustment_offset[(offsetof(s_multiplayer_object_properties, spawn_weight_adjustment) == 0x28) ? 1 : -1];

struct s_multiplayer_object_properties_definition
{
    c_flags_no_init<e_scenario_game_engine, unsigned short, 10> game_engine_flags;
    c_enum<e_multiplayer_object_type, char, 0, 24> object_type;
    c_flags_no_init<e_multiplayer_object_boundary_passability, unsigned char, 5> passability;
    c_flags_no_init<e_multiplayer_object_data_flags, short, 12> flags;
    c_enum<e_multiplayer_object_boundary_shape, char, 0, 4> boundary_shape;
    c_enum<e_multiplayer_object_spawn_timer_mode, char, 0, 2> spawn_timer_mode;
    short spawn_time;
    unsigned short abandon_time;
    float box_width;
    float box_length;
    float box_height;
    float box_top;
    float boundary_size;
    float boundary_inner_radius;
    float boundary_outer_radius;
    long boundary_center_marker;
    long spawn_marker_name;
    s_tag_reference boundary_effect;
    long boundary_material;
    s_tag_reference standard_shader;
    s_tag_reference opaque_shader;
    s_tag_reference sphere_standard_shader;
    s_tag_reference sphere_opaque_shader;
    s_tag_reference cylinder_standard_shader;
    s_tag_reference cylinder_opaque_shader;
    s_tag_reference box_standard_shader;
    s_tag_reference box_opaque_shader;
};

struct s_scenario_multiplayer_object_properties
{
    e_game_engine_symmetric_placement symmetry;
    c_flags<e_scenario_game_engine, unsigned short, 10> game_engine_flags;
    c_enum<e_multiplayer_team_designator, unsigned short, 0, 9> team_designator;
    signed char spawn_sequence;
    unsigned char minimum_count;
    signed char maximum_count;
    c_flags<e_multiplayer_object_placement_spawn_flags, char, 2> spawn_flags;
    short spawn_time;
    unsigned short abandon_time;
    c_enum<e_multiplayer_object_remapping_policy, char, 0, 3> remapping_policy;
    c_enum<e_multiplayer_object_boundary_shape, char, 0, 4> boundary_shape;
    c_enum<e_teleporter_channel, unsigned char, 0, 26> teleporter_channel;
    char spare_before_parent_name_index[3];
    short parent_name_index;
    char spare_before_box_width[8];
    float box_width;
    float box_length;
    float box_height;
    float box_top;
    float boundary_size;
};

struct s_multiplayer_object_boundary_geometry_data
{
    long type;
    long shader;
    float width;
    float depth;
    float height;
    float neg_height;
    real_matrix4x3 matrix;
    float radius;
};

typedef char multiplayer_object_properties_size[(sizeof(s_multiplayer_object_properties) == 0x4c) ? 1 : -1];
typedef char multiplayer_object_properties_definition_size[(sizeof(s_multiplayer_object_properties_definition) == 0xc4) ? 1 : -1];
typedef char scenario_multiplayer_object_properties_size[(sizeof(s_scenario_multiplayer_object_properties) == 0x34) ? 1 : -1];
typedef char scenario_multiplayer_parent_name_offset[(offsetof(s_scenario_multiplayer_object_properties, parent_name_index) == 0x16) ? 1 : -1];
typedef char multiplayer_object_spawn_sequence_offset[(offsetof(s_multiplayer_object_properties, spawn_sequence) == 0xc) ? 1 : -1];
typedef char multiplayer_object_boundary_size[(sizeof(s_multiplayer_object_boundary_geometry_data) == 0x50) ? 1 : -1];
typedef char multiplayer_object_boundary_matrix_offset[(offsetof(s_multiplayer_object_boundary_geometry_data, matrix) == 0x18) ? 1 : -1];

#endif // OBJECTS_MULTIPLAYER_OBJECT_TYPES_H
