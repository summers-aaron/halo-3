#ifndef AI_SQUAD_DEFINITIONS_H
#define AI_SQUAD_DEFINITIONS_H
#include <stddef.h>
#include "ai/ai_point3d.h"
#include "tag_files/tag_types.h"
__declspec(align(4)) struct squads_group_definition {
    char name[32];
    short parent_index;
    __declspec(align(4)) short objective;
};

typedef char squads_group_definition_size[(sizeof(squads_group_definition) == 0x28) ? 1 : -1];
typedef char squads_group_definition_objective_offset[(offsetof(squads_group_definition, objective) == 0x24) ? 1 : -1];

struct actor_starting_location_definition;
struct fire_team_definition {
    short difficulty_mask;
    short spare_after_difficulty_mask;
    short initial_count;
    short upgrade;
    union { short character; short character_index; };
    short primary_weapon;
    short secondary_weapon;
    short grenade;
    short equipment;
    short vehicle_type;
    long vehicle_variant;
    char script_name[32];
    short script_index;
    long activity;
    short activity_type;
    short activity_variant;
    short patrol_point_count;
    s_tag_block patrol_block;
    union {
        s_tag_block starting_locations;
        struct {
            union { long location_count; long count; };
            TAG_POINTER(actor_starting_location_definition) locations;
            long location_definition;
        };
    };
};
typedef fire_team_definition s_new_location;
struct squad_definition {
    char name[32];
    unsigned long flags;
    short team;
    short parent_index;
    short zone_index;
    short trigger_index;
    short objective;
    short task;
    union {
        s_tag_block fire_team_block;
        struct {
            union { long locations_count; long count; };
            union
            {
                TAG_POINTER(s_new_location) locations;
                TAG_POINTER(fire_team_definition) fire_teams;
                TAG_POINTER(fire_team_definition) squads;
            };
            long locations_definition;
        };
    };
    short editor_folder_index;
};
struct actor_starting_location_definition
{
    union { long name; short flags; };
    long starting_location_name;
    union { c_ai_point3d position; c_ai_point3d point; };
    real_euler_angles2d facing;
    float roll;
    long initial_movement_mode;
    short character;
    short primary_weapon;
    short secondary_weapon;
    char spare_after_secondary_weapon[0x30 - 0x2e];
    short vehicle_type;
    union { short seat_type; short priority; };
    short grenade_type;
    short initial_state;
    long actor_variant;
    long vehicle_variant;
    float initial_movement_distance;
    union { short emitter_palette_index; unsigned short emitter_index; };
    short giant_index;
    short biped_index;
    short movement_mode;
    char script_name[32];
    short script_index;
    short equipment_palette_index;
    long activity_name;
    short activity_type;
    short activity_variant_index;
    short activity_variant;
    short patrol_point_count;
    s_tag_block patrol_block;
};
typedef char starting_location_priority_offset[(offsetof(actor_starting_location_definition, priority) == 0x32) ? 1 : -1];
typedef char squad_definition_editor_folder_offset
    [(offsetof(squad_definition, editor_folder_index) == 0x3c) ? 1 : -1];
typedef char squad_definition_header_size[(sizeof(squad_definition) == 64) ? 1 : -1];
typedef char fire_team_definition_header_size[(sizeof(fire_team_definition) == 96) ? 1 : -1];
typedef char starting_location_definition_header_size[(sizeof(actor_starting_location_definition) == 136) ? 1 : -1];
typedef char fire_team_patrol_count_offset[(offsetof(fire_team_definition, patrol_point_count) == 68) ? 1 : -1];
typedef char fire_team_patrol_block_offset[(offsetof(fire_team_definition, patrol_block) == 72) ? 1 : -1];
typedef char starting_location_patrol_count_offset[(offsetof(actor_starting_location_definition, patrol_point_count) == 122) ? 1 : -1];
typedef char starting_location_patrol_block_offset[(offsetof(actor_starting_location_definition, patrol_block) == 124) ? 1 : -1];
typedef char actor_starting_location_definition_initial_movement_mode_offset
    [(offsetof(actor_starting_location_definition, initial_movement_mode) == 0x24) ? 1 : -1];
typedef char actor_starting_location_definition_initial_state_offset[(offsetof(actor_starting_location_definition, initial_state) == 0x36) ? 1 : -1];
typedef char actor_starting_location_definition_equipment_palette_index_offset
    [(offsetof(actor_starting_location_definition, equipment_palette_index) == 0x6e) ? 1 : -1];
typedef char actor_starting_location_definition_activity_name_offset[(offsetof(actor_starting_location_definition, activity_name) == 0x70) ? 1 : -1];
typedef char fire_team_definition_activity_offset[(offsetof(fire_team_definition, activity) == 0x3c) ? 1 : -1];
typedef char starting_location_starting_location_name_offset[(offsetof(actor_starting_location_definition, starting_location_name) == 0x4) ? 1 : -1];
typedef char starting_location_primary_weapon_offset[(offsetof(actor_starting_location_definition, primary_weapon) == 0x2a) ? 1 : -1];
typedef char starting_location_secondary_weapon_offset[(offsetof(actor_starting_location_definition, secondary_weapon) == 0x2c) ? 1 : -1];
typedef char starting_location_vehicle_type_offset[(offsetof(actor_starting_location_definition, vehicle_type) == 0x30) ? 1 : -1];
typedef char starting_location_seat_type_offset[(offsetof(actor_starting_location_definition, seat_type) == 0x32) ? 1 : -1];
typedef char starting_location_grenade_type_offset[(offsetof(actor_starting_location_definition, grenade_type) == 0x34) ? 1 : -1];
typedef char starting_location_actor_variant_offset[(offsetof(actor_starting_location_definition, actor_variant) == 0x38) ? 1 : -1];
typedef char starting_location_vehicle_variant_offset[(offsetof(actor_starting_location_definition, vehicle_variant) == 0x3c) ? 1 : -1];
typedef char starting_location_initial_movement_distance_offset[(offsetof(actor_starting_location_definition, initial_movement_distance) == 0x40) ? 1 : -1];
typedef char starting_location_emitter_palette_index_offset[(offsetof(actor_starting_location_definition, emitter_palette_index) == 0x44) ? 1 : -1];
typedef char starting_location_giant_index_offset[(offsetof(actor_starting_location_definition, giant_index) == 0x46) ? 1 : -1];
typedef char starting_location_biped_index_offset[(offsetof(actor_starting_location_definition, biped_index) == 0x48) ? 1 : -1];
typedef char starting_location_movement_mode_offset[(offsetof(actor_starting_location_definition, movement_mode) == 0x4a) ? 1 : -1];

typedef char fire_team_activity_type_offset
    [(offsetof(fire_team_definition, activity_type) == 0x40) ? 1 : -1];
typedef char squad_trigger_index_offset
    [(offsetof(squad_definition, trigger_index) == 0x2a) ? 1 : -1];
typedef char starting_location_activity_type_offset
    [(offsetof(actor_starting_location_definition, activity_type) == 0x74) ? 1 : -1];
typedef char starting_location_activity_variant_index_offset
    [(offsetof(actor_starting_location_definition, activity_variant_index) == 0x76) ? 1 : -1];

#endif
