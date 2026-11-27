#ifndef NETWORKING_ONLINE_ONLINE_CONSTANTS_H
#define NETWORKING_ONLINE_ONLINE_CONSTANTS_H

static __forceinline long online_constant_get_game_mode_by_xlast_index(long index)
{
    return index + 23;
}
static __forceinline long online_constant_get_game_type(bool ranked)
{
    if (ranked) return 4;
    else return 3;
}

enum e_online_data_type { _online_data_type_none };
extern e_online_data_type k_online_property_data_type_map[];

#endif
