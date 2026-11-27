#ifndef NETWORKING_LOGIC_NETWORK_SESSION_INTERFACE_H
#define NETWORKING_LOGIC_NETWORK_SESSION_INTERFACE_H

#include "game/game_types.h"
#include "networking/session/network_session_types.h"
#include "shell/shell_types.h"

// Functions defined in tu/networkinglibxbox/network_session_interface.cpp.

class c_map_variant;
struct s_network_squad_status_data;
struct s_saved_film_description;
enum e_gui_game_mode;
enum e_network_session_class;
enum e_network_game_start_mode;
enum e_network_game_privacy;

e_network_game_start_mode network_squad_session_get_start_mode(void);
e_network_game_privacy network_squad_session_get_privacy_mode(void);

e_gui_game_mode network_life_cycle_squad_session_get_ui_game_mode(void);

bool network_life_cycle_in_matchmaking(void);
e_network_session_class network_squad_session_get_session_class(void);
long network_squad_session_get_maximum_player_count(void);
bool network_squad_session_set_maximum_player_count(long maximum_player_count);
long network_group_session_set_disable_host_election(long disable_host_election);
bool network_squad_session_get_matchmaking_tip(long a, long index);
bool network_squad_session_can_set_game_settings(void);
bool network_squad_session_set_map_variant(const c_map_variant *map_variant);
bool network_squad_session_set_film(const s_saved_film_description *film);
long network_squad_session_get_countdown_timer(void);
unsigned short network_squad_session_get_hopper(void);
bool network_squad_session_build_status(s_network_squad_status_data *game_status);

struct s_map_variant_validation;
struct s_network_quality_data;
struct s_network_session;
struct s_network_session_peer;
struct s_network_session_player;

extern "C" long network_squad_session_get_peer_count(void);
extern "C" bool network_squad_session_get_membership(long *a, long *local_peer_index, long *c, long *leader_peer_index,
	long *peer_count, unsigned long *peer_valid_mask, const s_network_session_peer **peers, long *player_count, unsigned long *player_valid_mask,
	const s_network_session_player **players);
bool map_variant_is_valid(const s_map_variant_validation *variant);
extern "C" long network_squad_session_get_host_player_index(void);
extern "C" bool network_group_session_get_membership(long *a, long *local_peer_index, long *c, long *d, long *e,
	unsigned long *f, const s_network_session_peer **g, long *player_count, unsigned long *player_valid_mask,
	const s_network_session_player **players);
extern "C" bool network_squad_session_set_game_options(long a, long b, const void *game_options);
extern "C" long network_squad_session_get_campaign_difficulty(void);
extern "C" bool network_squad_session_delegate_leadership(long peer_index);
extern "C" void network_session_interface_set_player_and_session_names(const wchar_t *name, const wchar_t *description);
long network_session_interface_get_user_team_index(long user_index);
extern "C" bool network_session_interface_get_network_quality(s_network_quality_data *quality, long *nat_type,
	long *value_a, long *value_b);
extern "C" bool network_session_get_membership(const s_network_session *session, long *update_number, long *local_peer_index, long *host_peer_index,
	long *leader_peer_index, long *peer_count, unsigned long *peer_valid_mask, const s_network_session_peer **peers,
	long *player_count, unsigned long *player_valid_mask, const s_network_session_player **players);
extern "C" long network_life_cycle_session_get_primary_skulls(void);
extern "C" long network_life_cycle_session_get_global_map_precache_status(void);
extern "C" long network_life_cycle_session_get_game_variant(void);
extern "C" bool network_squad_session_end_game(void);
extern "C" void network_session_interface_set_controller_desired_team(long controller_index, long desired_team_index);
extern "C" void network_session_interface_remove_local_user(long user_index);
extern "C" void network_session_update_team_indices(void);
extern "C" void network_session_interface_update();
void network_session_interface_update_local_state();

struct s_player_identifier;

bool network_session_interface_get_local_user_identifier(long index, s_player_identifier *key, bool any_state);

struct s_player_configuration;

void network_session_interface_set_local_user_properties(long user_index, e_controller_index controller_index,
	const s_player_configuration *configuration, unsigned long voice_settings);

e_network_join_refuse_reason network_squad_session_is_joinable(void);
bool network_squad_session_set_coop_game_options( e_campaign_id campaign_id, e_map_id map_id, const char *scenario_path,
	short insertion_point, const void *campaign_options);
void network_squad_session_get_map(enum e_campaign_id * _sym_p0, enum e_map_id * _sym_p1, char * name);
bool network_squad_session_set_map(e_campaign_id campaign_id, e_map_id map_id, const char *scenario_path);
bool network_squad_session_set_campaign_difficulty(e_campaign_difficulty_level campaign_difficulty);
bool network_session_interface_get_local_user_properties( long user_index, e_controller_index *controller_index,
	s_player_configuration *configuration, unsigned long *voice_settings);
void network_session_set_player_failure_reason(long player_index, e_network_join_refuse_reason reason);
bool network_life_cycle_session_get_map(e_campaign_id *campaign_id, e_map_id *map_id);

enum e_session_network_message;

void network_session_interface_handle_message(e_session_network_message message);

class c_network_session;
void network_session_update_properties(c_network_session *session);

class c_network_session_parameter_saved_film_game_options;
extern "C" bool network_session_game_options_set(c_network_session_parameter_saved_film_game_options *parameter, long a, long b, const void *game_options);

#endif // NETWORKING_LOGIC_NETWORK_SESSION_INTERFACE_H
