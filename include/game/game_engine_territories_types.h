#ifndef GAME_GAME_ENGINE_TERRITORIES_TYPES_H
#define GAME_GAME_ENGINE_TERRITORIES_TYPES_H

#include "game/game_engine_area_types.h"
#include "game/game_engine_scoring_globals.h"

struct s_territory_data : c_area
{
    unsigned short capture_progress[8];
    unsigned char capturing_teams[8];
    long team_designator;
    long flags;
    __forceinline s_territory_data()
    {
        team_designator = 8;
        for (long index = 0; index < 8; ++index)
            capturing_teams[index] = 0;
        for (long index = 0; index < 8; ++index)
            capture_progress[index] = 0;
        flags = 0;
    }
    long get_object_index() const { return m_object_index; }
};
struct s_territory_area_set
{
    s_territory_data areas[8];
    long count;
    long maximum_count, types, area_flags;
    __forceinline s_territory_area_set()
    {
        count = 0;
        types = 0;
        area_flags = 0;
    }
    s_territory_data *begin() { return areas; }
    s_territory_data *end() { return areas + count; }
    long get_area_count() const { return count; }
    s_territory_data &operator[](long index) { return areas[index]; }
    const s_territory_data &operator[](long index) const { return areas[index]; }
};
struct s_territories_globals : s_territory_area_set
{
    long defending_team;
    long sudden_death_timer, ending_timer;
    __forceinline s_territories_globals()
    {
        defending_team = -1;
        sudden_death_timer = -1;
        ending_timer = -1;
    }
};

typedef char territory_data_size[(sizeof(s_territory_data) == 0x84) ? 1 : -1];
typedef char territories_globals_size[(sizeof(s_territories_globals) == 0x43c) ? 1 : -1];

__forceinline s_territories_globals &s_game_engine_globals::territories()
{
    return *(s_territories_globals *)variant_globals;
}

#endif
