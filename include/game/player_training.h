#ifndef GAME_PLAYER_TRAINING_H
#define GAME_PLAYER_TRAINING_H

#include "cache/restricted_memory_regions.h"

// Functions defined in tu/blamlibxbox/player_training.cpp.


struct player_action;

void player_training_examine_action(long player_index, struct player_action const * action);
void player_training_notify_kill(long player_index, long object_index, unsigned char damage_reporting_info);
void player_training_load_profiles(void);
void player_training_update_player(long player_index);
void player_training_handle_saved_game(long saved_game_index);
bool unit_in_driving_seat(long unit_index, long seat_index, bool include_any_driver);
void __tls_set_g_player_training_globals_allocator(void *globals);
extern "C" void player_training_set_user_bit(int bit);
bool training_is_completed(long user_index, long training_index);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_training_globals_allocator> g_player_training_globals_allocator;

template<> void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_training_globals_allocator>::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits);
template<> void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_training_globals_allocator>::allocate(unsigned int size, const char *name);
template<> void t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_training_globals_allocator>::deallocate(void *allocation);

void player_training_initialize();
void player_training_dispose();
void player_training_initialize_for_new_map();
void player_training_dispose_from_old_map();

#endif // GAME_PLAYER_TRAINING_H
