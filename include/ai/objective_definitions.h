#ifndef AI_OBJECTIVE_DEFINITIONS_H
#define AI_OBJECTIVE_DEFINITIONS_H

#include "tag_files/tag_types.h"
#include <stddef.h>

struct squads_area_zone_set_entry {
    short type;
    short flags;
    short firing_positions;
    short zone_index;
    short area_index;
};

typedef char task_area_entry_size[(sizeof(squads_area_zone_set_entry) == 0xa) ? 1 : -1];
typedef char task_area_zone_offset[(offsetof(squads_area_zone_set_entry, zone_index) == 0x6) ? 1 : -1];
typedef char task_area_index_offset[(offsetof(squads_area_zone_set_entry, area_index) == 0x8) ? 1 : -1];

// Scenario objective/tag task layouts shared by objectives.cpp and actors.cpp.
struct s_task
{
    union { unsigned short flags; short signed_flags; };
    short vehicle_inhibit_flags;
    short inhibit_difficulty;
    short movement;
    union { unsigned short type; short follow_mode; short signed_type; };
    short follow_area_index;
    float distance;
    char entry_script[32];
    char command_script[32];
    char exhaustion_script[32];
    short script_index;
    short command_script_index;
    short exhaust_script_index;
    short group_squad_index;
    short dialogue;
    unsigned short runtime_flags;
    s_tag_block pureform_distributions;
    long name;
    short priority;
    short first_child_index;
    short next_sibling_index;
    short parent_index;
    s_tag_block activation_script;
    short activation_script_index;
    short maximum_body_count;
    short filter_flags;
    short filter;
    short minimum_squad_count;
    short capacity;
    short maximum_squad_count;
    short style_index;
    float minimum_strength;
    s_tag_block areas;
    s_tag_block points;
};

struct s_objective
{
    long name;
    short zone_index;
    short first_task_index;
    s_tag_block tasks;
};

typedef char task_follow_area_index_offset[(offsetof(s_task, follow_area_index) == 0xa) ? 1 : -1];
typedef char objective_definition_size[(sizeof(s_objective) == 20) ? 1 : -1];
typedef char task_definition_size[(sizeof(s_task) == 204) ? 1 : -1];
typedef char task_definition_command_script_offset[(offsetof(s_task, command_script_index) == 0x72) ? 1 : -1];
typedef char task_entry_script_offset[(offsetof(s_task, entry_script) == 0x10) ? 1 : -1];
typedef char task_command_script_offset[(offsetof(s_task, command_script) == 0x30) ? 1 : -1];
typedef char task_exhaustion_script_offset[(offsetof(s_task, exhaustion_script) == 0x50) ? 1 : -1];
typedef char task_runtime_flags_offset[(offsetof(s_task, runtime_flags) == 0x7a) ? 1 : -1];
typedef char task_distribution_definition_offset[(offsetof(s_task, pureform_distributions) == 0x7c) ? 1 : -1];
typedef char task_activation_script_offset[(offsetof(s_task, activation_script) == 0x94) ? 1 : -1];
typedef char objective_zone_index_offset[(offsetof(s_objective, zone_index) == 4) ? 1 : -1];

#endif // AI_OBJECTIVE_DEFINITIONS_H
