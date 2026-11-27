#ifndef GAME_MAP_VARIANT_OBJECT_PROPERTIES_TYPES_H
#define GAME_MAP_VARIANT_OBJECT_PROPERTIES_TYPES_H

#include <stddef.h>
#include "cseries/cseries_wrapped.h"

enum e_scenario_game_engine {
    k_scenario_game_engine_count = 10
};

enum e_variant_placement_flags {
    _variant_placement_flag0_bit = 0,
    _variant_placement_flag1_bit = 1,
    _variant_placement_symmetric_bit = 2,
    _variant_placement_asymmetric_bit = 3,
    k_variant_placement_flags_count = 6
};
enum e_variant_team_designator {
    _variant_team_designator_neutral = 9,
    k_variant_team_designator_count = 10
};

enum e_multiplayer_object_boundary_shape {
    _multiplayer_object_boundary_shape_unused = 0,
    _multiplayer_object_boundary_shape_sphere = 1,
    _multiplayer_object_boundary_shape_cylinder = 2,
    _multiplayer_object_boundary_shape_box = 3,
    k_multiplayer_object_boundary_shape_count = 4
};
struct s_variant_multiplayer_object_properties_definition {
    s_variant_multiplayer_object_properties_definition() { initialize_to_default(); }
    void initialize_to_default() {
        game_engine_flags.clear();
        placement_flags.clear();
        placement_flags.set(_variant_placement_asymmetric_bit, true);
        placement_flags.set(_variant_placement_symmetric_bit, true);
        boundary_shape = _multiplayer_object_boundary_shape_unused;
        spawn_rate = 0;
        boundary_negative_height = 0.0f;
        boundary_positive_height = 0.0f;
        boundary_box_length = 0.0f;
        boundary_width = 0.0f;
        spare_clips = 0;
        team_designator = _variant_team_designator_neutral;
        teleporter_channel = 0;
    }

    c_flags<e_scenario_game_engine, unsigned short, k_scenario_game_engine_count> game_engine_flags;
    c_flags<e_variant_placement_flags, unsigned char, k_variant_placement_flags_count> placement_flags;
    c_enum<e_variant_team_designator, unsigned char, 0, k_variant_team_designator_count> team_designator;
    unsigned char spare_clips;
    unsigned char spawn_rate;
    char teleporter_channel;                    // signed teleporter channel; known values include 14, 18, 19, 20, 23
    c_enum<e_multiplayer_object_boundary_shape, char, 0, k_multiplayer_object_boundary_shape_count> boundary_shape;
    float boundary_width;
    float boundary_box_length;
    float boundary_positive_height;
    float boundary_negative_height;
};

typedef char variant_multiplayer_properties_size[(sizeof(s_variant_multiplayer_object_properties_definition) == 0x18) ? 1 : -1];
typedef char variant_multiplayer_properties_channel_offset[(offsetof(s_variant_multiplayer_object_properties_definition, teleporter_channel) == 6) ? 1 : -1];
typedef char variant_multiplayer_properties_boundary_offset[(offsetof(s_variant_multiplayer_object_properties_definition, boundary_width) == 8) ? 1 : -1];

#endif // GAME_MAP_VARIANT_OBJECT_PROPERTIES_TYPES_H
