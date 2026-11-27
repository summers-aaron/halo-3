#ifndef OBJECTS_LIGHT_GLOBALS_H
#define OBJECTS_LIGHT_GLOBALS_H

#include "structures/cluster_partition_types.h"

struct lights_game_globals_definition
{
    bool can_use_flashlights;
    long can_use_flashlights_time;
    bool enabled;
    unsigned long enabled_lights[13];
};

typedef char lights_game_globals_size[(sizeof(lights_game_globals_definition) == 64) ? 1 : -1];

struct s_nondeterministic_light_player_state
{
    unsigned char fade;
    unsigned char cooldown;
};

struct s_nondeterministic_light_data
{
    long visibility_mark;
    long light_index;
    float intensity;
    float specular_fade;
    s_nondeterministic_light_player_state players[4];
};

// March 2007 TLS: ?lights_game_globals@@3PAUlights_game_globals_definition@@A,
// ?light_data@@3PAUs_data_array@@A, ?light_cluster_partition@@3Ucluster_partition@@A,
// ?g_nondeterministic_light_data@@3PAUs_nondeterministic_light_data@@A (all C++ linkage).
extern __declspec(thread) lights_game_globals_definition *lights_game_globals;
extern __declspec(thread) s_data_array *light_data;
extern __declspec(thread) cluster_partition light_cluster_partition;
extern __declspec(thread) s_nondeterministic_light_data *g_nondeterministic_light_data;

typedef char nondeterministic_light_data_size[(sizeof(s_nondeterministic_light_data) == 24) ? 1 : -1];

#endif // OBJECTS_LIGHT_GLOBALS_H
