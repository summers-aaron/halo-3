#ifndef SOUND_GAME_SOUND_TRANSMISSION_TYPES_H
#define SOUND_GAME_SOUND_TRANSMISSION_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "sound/sound_environment_parameters.h"
#include "objects/object_reference_types.h"
#include "scenario/scenario_zone_set_audibility.h"

struct game_sound_transmission_slot { unsigned short *cluster_map; long count; };


struct game_sound_transmission_portal_indices { short bsp_index : 6; short portal_index : 10; };


struct s_game_sound_transmission_source {
    long listener_index;
    real_point3d obstruction_point;
    real_point3d reverse_obstruction_point;
    float obstruction_fraction;
    float reverse_obstruction_fraction;
    real_point3d position;
    union {
        s_sound_environment_parameters environment;
        struct {
            float obstruction;
            float portal_obstruction;
            float portal_propagation;
            float water_depth;
            short water_material;
            float transmission_multiplier;
        };
    };
    s_location location;
    bool portals_ignored;
    bool has_obstruction_points;
    bool no_obstruction;
    bool use_audibility;
};

struct s_game_sound_obstruction_parameters
{
    long count;
    s_game_sound_transmission_source sources[256];
    __forceinline bool full() const { return count == 256; }
    __forceinline void push() { ++count; }
    __forceinline s_game_sound_transmission_source *get_top() { return &sources[count - 1]; }
};

struct game_sound_transmission_map { const scenario_zone_set_audibility *valid; long cluster_count; game_sound_transmission_slot bsps[16]; };



typedef char game_sound_transmission_source_size[(sizeof(s_game_sound_transmission_source) == 0x50) ? 1 : -1];
typedef char game_sound_environment_offset[(offsetof(s_game_sound_transmission_source, environment) == 0x30) ? 1 : -1];
typedef char game_sound_multiplier_offset[(offsetof(s_game_sound_transmission_source, transmission_multiplier) == 0x44) ? 1 : -1];
typedef char game_sound_no_obstruction_offset[(offsetof(s_game_sound_transmission_source, no_obstruction) == 0x4e) ? 1 : -1];
typedef char game_sound_obstruction_size[(sizeof(s_game_sound_obstruction_parameters) == 0x5004) ? 1 : -1];
#endif
