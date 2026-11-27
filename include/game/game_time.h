#ifndef GAME_GAME_TIME_H
#define GAME_GAME_TIME_H

// Functions defined in tu/blamlibxbox/game_time.cpp.

enum e_game_time_holiday;
extern "C" e_game_time_holiday game_time_compute_holiday(void);

float game_time_get_safe_in_seconds();
void game_time_set_rate_scale_direct(float rate_scale);
void __tls_set_g_game_time_globals_allocator(void *allocation);
bool game_time_update(float world_seconds_elapsed, float *game_seconds_elapsed_out, long *game_ticks_elapsed_out);
extern "C" bool game_time_is_paused(void);
extern "C" void game_time_update_water_ripples(void);
extern "C" int game_time_add_water_ripple(int key, int period, float amplitude);
extern "C" void game_time_set_pause_reason(bool set, long reason);

struct s_flags_iterator;

extern "C" int flags_iterator_next(s_flags_iterator *iterator);
extern "C" void game_time_update_pause_state(void);
extern "C" unsigned char game_time_get_film_ticks_remaining(long *out);

extern "C" const long k_game_time_controller_pause_reasons[];

void game_time_initialize(void);
void game_time_dispose(void);
void game_time_initialize_for_new_map(void);
void game_time_dispose_from_old_map(void);
#endif // GAME_GAME_TIME_H
