#ifndef GAME_CAMPAIGN_METAGAME_H
#define GAME_CAMPAIGN_METAGAME_H

#include "game/campaign_metagame_level_types.h"

extern "C" c_level campaign_metagame_get_highest_level(void);

// Functions defined in tu/blamlibxbox/campaign_metagame.cpp.

extern "C" void campaign_metagame_grant_map_achievement(long player_index, long key);
extern "C" void campaign_metagame_update_scores_before_revert(void);
extern "C" void campaign_metagame_update_scores_after_revert(void);
extern "C" void campaign_metagame_reset_finished(void);
extern "C" void campaign_metagame_handle_game_won(void);
extern "C" long campaign_metagame_get_highest_completed_difficulty(long controller_index);
void campaign_metagame_update();
extern "C" bool campaign_metagame_is_enabled(void);

struct s_campaing_metagame_player;
struct s_campaing_metagame_globals;
#include "game/campaign_metagame_player_types.h"

extern "C" void campaign_metagame_initialize_for_new_map(void);
extern "C" void campaign_metagame_dispose_from_old_map(void);
extern "C" void campaign_metagame_update_object_score(long object_index, short score_delta);
extern "C" void campaign_metagame_set_profile_progress_bit(long object_index, short p1);
extern "C" void campaign_metagame_set_profile_bit_and_grant_achievement(long object_index, short p1);
extern "C" int campaign_metagame_compute_medal(unsigned char flags);
extern "C" float campaign_metagame_get_time_multiplier(long ticks, float *time);
extern "C" float campaign_metagame_get_player_count_scale(void);
extern "C" float campaign_metagame_get_skull_bonus(void);
extern "C" float campaign_metagame_get_score_scale(void);
extern "C" void campaign_metagame_update_player_score(long player_index, float points);
extern "C" s_campaing_metagame_player *campaign_metagame_try_and_get_owner_slot(s_campaing_metagame_globals *globals, long index,
    unsigned char allocate);
extern "C" s_campaign_metagame_player_slot *campaign_metagame_try_and_get_player_slot(s_campaign_metagame_player_globals *globals,
    long player_index, int allocate);
extern "C" void campaign_metagame_initialize_atexit(void);
extern "C" void campaign_metagame_dispose_atexit(void);

// Named allocator objects owned by this unit.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
void __tls_set_g_campaing_metagame_allocator(void *allocation);
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_campaing_metagame_allocator> g_campaing_metagame_allocator;
extern "C" void campaign_metagame_set_disabled_flag(unsigned int flags);

void campaign_metagame_initialize(void);
void campaign_metagame_dispose(void);

#endif // GAME_CAMPAIGN_METAGAME_H
