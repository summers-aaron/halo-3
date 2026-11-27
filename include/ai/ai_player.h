#ifndef AI_AI_PLAYER_H
#define AI_AI_PLAYER_H

// Functions defined in tu/ailibxbox/ai_player.cpp that other units call.

struct ai_player_state;

void ai_players_reset(void);
void ai_player_state_create(long id);
ai_player_state *ai_player_state_get(long id);
extern "C" void ai_player_update();
extern "C" bool ai_player_has_no_nearby_vehicle(short index);
extern "C" void ai_player_update_vehicle_boarding(long actor_index);
extern "C" void ai_player_update_boarding_dialogue(long actor_index);

union real_vector3d;

extern "C" unsigned char ai_player_try_and_get_combat_position(long object_index, real_vector3d *position);

#include "cache/restricted_memory_regions.h"

void __tls_set_g_ai_players_allocator(void *allocation);
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_players_allocator> ai_player_allocation_manager;
extern ai_player_allocation_manager g_ai_players_allocator;

#endif // AI_AI_PLAYER_H
