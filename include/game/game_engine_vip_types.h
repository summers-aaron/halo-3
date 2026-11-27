#ifndef GAME_GAME_ENGINE_VIP_TYPES_H
#define GAME_GAME_ENGINE_VIP_TYPES_H

#include "game/game_engine_area_set_types.h"
#include "memory/static_arrays.h"
#include "cseries/cseries_wrapped.h"

class c_destination_zone : public c_area {
public:
    __forceinline c_destination_zone() { team_designator_flags.clear(); }
    void set_selected(bool selected);
    __forceinline void clear_team_designators() { team_designator_flags.clear(); }
    __forceinline bool has_team_visited(e_multiplayer_team_designator team_designator) const { return (team_designator_flags & (1 << team_designator)) != 0; }
    void team_visited(e_multiplayer_team_designator team_designator);
    c_flags<e_multiplayer_team_designator, unsigned char, 8> team_designator_flags;
};

struct s_vip_globals
{
    c_static_array<long, 8> vip_player_indices;
    long defending_team_index;
    c_static_array<s_multiplayer_object_boundary_geometry_data, 8> influence_boundaries;
    c_area_set<c_destination_zone, 12> destination_zones;
};

#include "game/game_engine_scoring_globals.h"

__forceinline s_vip_globals &s_game_engine_globals::vip()
{
    return *(s_vip_globals *)variant_globals;
}

typedef char vip_player_indices_offset[(offsetof(s_game_engine_globals, variant_globals) == 0xe18c) ? 1 : -1];
typedef char vip_destination_zones_offset[(offsetof(s_vip_globals, destination_zones) == 0x2a4) ? 1 : -1];

#endif
