#ifndef GAME_DYNAMIC_SPAWN_INFLUENCER_COLLECTION_TYPES_H
#define GAME_DYNAMIC_SPAWN_INFLUENCER_COLLECTION_TYPES_H

#include "game/dynamic_spawn_influencer_types.h"

struct s_dynamic_spawn_influencer_collection
{
    short player_index;
    volatile short count;
    s_dynamic_spawn_influencer influencers[222];
};

#endif // GAME_DYNAMIC_SPAWN_INFLUENCER_COLLECTION_TYPES_H
