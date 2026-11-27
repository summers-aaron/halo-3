#ifndef SIMULATION_GAME_ENGINE_PLAYER_TYPES_H
#define SIMULATION_GAME_ENGINE_PLAYER_TYPES_H

#include "game/map_variant.h"
#include "math/real_math_types.h"

struct s_player_editor_request_event_data {
    unsigned int event_type;
    short request_player_index;
    long variant_index;
    real_point3d player_position;
    real_vector3d player_forward, player_up;
    real_point3d position;
    real_vector3d forward, up;
    float distance;
    long edited_object_id;
    unsigned char minimum_count, maximum_count;
    bool flags;
    s_variant_multiplayer_object_properties_definition properties;
};

extern "C" int simulation_game_engine_player_index_from_gamestate_index(int gamestate_index);

#endif
