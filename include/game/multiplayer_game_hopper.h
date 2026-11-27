#ifndef GAME_MULTIPLAYER_GAME_HOPPER_H
#define GAME_MULTIPLAYER_GAME_HOPPER_H

// Functions defined in tu/blamlibxbox/multiplayer_game_hopper.cpp.

class c_hopper_configuration;
struct s_calculated_player_statistics;
struct s_game_hopper_picked_game_collection;
struct s_queried_player_global_statistics;
struct s_queried_player_hopper_statistics;
struct utf8;

const c_hopper_configuration *multiplayer_game_hoppers_get_hopper_configuration(unsigned short hopper_identifier);
bool multiplayer_game_hoppers_pick_random_game_collection(long player_count, long mode, struct s_game_hopper_picked_game_collection * collection);
void multiplayer_game_hopper_build_calculated_stats(const s_queried_player_global_statistics *global_statistics,
    const s_queried_player_hopper_statistics *hopper_statistics, s_calculated_player_statistics *calculated_statistics);
long multiplayer_game_hopper_compute_skill_match_delta(long index);
long multiplayer_game_hopper_compute_quality_adjusted_skill_update_weight(long skill_update_weight, long game_quality);
const utf8 *multiplayer_game_hopper_get_description(unsigned short hopper_identifier);
extern "C" long multiplayer_game_hopper_get_load_status(void);
extern "C" bool multiplayer_game_hopper_set_active_hopper_and_request_game_set(unsigned short hopper_identifier, bool choose_fallback, const void *membership);
extern "C" long multiplayer_game_hopper_get_next_rank_experience(long experience, long experience_penalty, long highest_skill);
extern "C" void multiplayer_game_hopper_client_compute_repeated_play_adjustment_weight(long controller_index, unsigned short hopper_identifier,
    long recent_limit, long forbid_count, long penalty_start, long penalty_end,
    const unsigned __int64 *player_identifiers, long player_count, long *weight_out, bool *allowed_out);
extern "C" long multiplayer_game_hopper_get_next_rank_skill(long experience, long experience_penalty, long highest_skill);
extern "C" void multiplayer_game_hopper_update(void);
long multiplayer_game_hopper_compute_skill_level(float skill_mean, float skill_deviation, long experience);
extern "C" void multiplayer_game_hopper_compute_experience_rank(long score, long baseline, long level_points,
    long *difference_out, long *rank_out, long *grade_out);
long multiplayer_game_hopper_compute_skill_update_weight(long skill_delta, long skill);
extern "C" bool multiplayer_game_hopper_is_hopper_visible(unsigned short identifier, const void *membership);

class c_network_session_membership;
struct multiplayer_hopper_check;

long multiplayer_game_hopper_compute_match_quality(const c_network_session_membership *membership);
void multiplayer_game_hopper_get_players_status(const c_hopper_configuration *hopper,
    const c_network_session_membership *session_membership, multiplayer_hopper_check *check);

enum e_session_game_start_error;

extern "C" e_session_game_start_error multiplayer_game_hopper_compute_start_error(bool check_hopper, bool hopper_valid);
extern "C" long g_hopper_last_manifest_version;

struct s_multiplayer_game_hopper_global_storage;
extern s_multiplayer_game_hopper_global_storage g_multiplayer_game_hopper_globals;

#endif // GAME_MULTIPLAYER_GAME_HOPPER_H
