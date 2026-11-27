#ifndef GAME_GAME_H
#define GAME_GAME_H

// Functions defined in tu/blamlibxbox/game.cpp.

struct game_options;
struct s_cluster_reference;
struct s_date_and_time;
struct s_game_cluster_bit_vectors;
enum e_game_simulation_type;

void game_simulation_set(e_game_simulation_type simulation_type);

bool game_is_cooperative(void);
bool game_is_distributed(void);
bool game_is_predicted(void);
extern "C" void game_options_validate_active_variants(void);
bool game_is_authoritative(void);
bool game_campaign_is_multiplayer(void);
short game_difficulty_level_get(void);
bool game_test_cluster_activation(const s_cluster_reference *cluster_reference);
void game_clusters_or(const s_game_cluster_bit_vectors *a, const s_game_cluster_bit_vectors *b, s_game_cluster_bit_vectors *out);
void game_lost(bool lost);
void game_initialize(void);
void game_dispose_from_old_map(void);
void game_initialize_for_new_map(const game_options *options);
void game_create_players();
bool game_options_verify(const game_options *options, char *error_string, long error_string_length);
bool game_options_match(const game_options *options1, const game_options *options2);
void game_options_setup_default_players(long player_count, game_options *options);
void game_finish();
void game_time_get_date_and_time(s_date_and_time *date_and_time);
void game_won(void);
void game_tick();

class c_bitstream;

extern "C" bool game_is_map_active(void);
extern "C" void game_options_initialize_default(game_options *options);
extern "C" void game_options_validate_variants(game_options *options);
extern "C" void game_options_encode(c_bitstream *packet, const game_options *options);
extern "C" bool game_campaign_has_multiple_players(void);
extern "C" bool game_is_film_playback_active();
extern "C" bool game_is_coop_respawn_allowed();
extern "C" float game_time_get_holiday(void);
extern "C" void game_saved_film_start_timer(void);
extern "C" void game_saved_film_reset_playback_speed(void);
extern "C" void game_record_campaign_completion(void);
extern "C" void game_update_ticks(long count, float *output);

struct simulation_update;

extern "C" void game_mark_map_completed_at_difficulty(short campaign_difficulty);
extern "C" void game_pvs_update();
void game_tick_pulse_random_seed_deterministic(struct simulation_update const * _sym_p0);
void __tls_set_g_game_globals_allocator(void *value);


// Shared declarations used by game.cpp.
extern bool g_default_playtest_mode;
extern "C" const float k_game_time_july_seventh;
enum e_game_create_mode
{
    _game_create_mode_lock,
    _game_create_mode_dont_lock,

    k_game_create_mode_count
};
void game_create_objects(e_game_create_mode mode);
void game_start(e_game_create_mode mode);

#endif // GAME_GAME_H
