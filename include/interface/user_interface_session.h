#ifndef INTERFACE_USER_INTERFACE_SESSION_H
#define INTERFACE_USER_INTERFACE_SESSION_H

#include "shell/shell_types.h"

#include "networking/session/network_session_types.h"
#include "game/game_types.h"

// Functions defined in tu/interfacelibxbox/user_interface_session.cpp.

class c_game_variant;
class c_map_variant;
struct s_network_squad_status_data;
struct s_player_configuration;
struct s_player_identifier;
struct s_saved_film_description;

bool user_interface_squad_exists(void);
const c_game_variant *user_interface_game_settings_get_game_variant(void);
struct s_saved_film_description const * user_interface_game_settings_get_film(void);
bool user_interface_squad_is_booting_allowed(void);
long user_interface_squad_get_countdown_timer(void);
long user_interface_squad_get_player_index(const s_player_identifier *identifier);
long user_interface_squad_get_player_count(void);
long user_interface_squad_get_local_player_count(void);
long user_interface_squad_get_player_at_local_controller(e_controller_index controller_index);
e_network_join_refuse_reason user_interface_squad_is_joinable(void);
long user_interface_squad_get_leader_player_index(void);
bool user_interface_squad_is_local_player(long player_index);
bool user_interface_group_is_local_player(long player_index);
bool user_interface_squad_is_player_valid(long player_index);
bool user_interface_group_is_player_valid(long player_index);
bool user_interface_session_is_player_valid(long player_index);
const s_player_identifier *user_interface_squad_get_player_identifier(long player_index);
const s_player_configuration *user_interface_squad_get_player_data(long player_index);
const s_player_configuration *user_interface_session_get_player_data(long session_player_index);
bool user_interface_squad_in_or_after_countdown(void);
long user_interface_session_player_get_player_index(const s_player_identifier *player_identifier);
bool user_interface_interactive_session_is_group(void);
bool user_interface_remote_squad_contains_local_peer(const s_network_squad_status_data *squad);
extern "C" bool user_interface_session_is_local_peer_player(long player_index);
extern "C" __int64 user_interface_session_is_silver_or_gold_live(long player_index);
extern "C" const s_player_identifier *user_interface_session_is_local_player(long player_index);
extern "C" unsigned __int64 user_interface_session_player_get_usable_player_xuid(long player_index);
extern "C" unsigned __int64 user_interface_session_get_player_xuid_if_usable(long player_index);
extern "C" bool user_interface_interactive_session_is_squad(void);
extern "C" unsigned __int64 user_interface_session_find_player_xuid(const s_player_identifier *identifier);
extern "C" void user_interface_session_update_string_verification();
extern "C" bool user_interface_group_exists(void);
extern "C" long user_interface_group_get_player_index(const s_player_identifier *identifier);
extern "C" bool user_interface_squad_can_user_join(long user_index);
extern "C" bool parse_xml_lobby_film_party_leader_requirement(void *context, wchar_t *output, long capacity);

struct s_network_session;

extern "C" bool user_interface_session_set_name(const wchar_t *name, const wchar_t *session_name);
extern "C" bool user_interface_group_try_and_get_session(s_network_session **session);
extern "C" bool user_interface_squad_set_game_variant(const c_game_variant *source, bool enabled);
extern "C" unsigned char user_interface_squad_set_map_variant(const c_map_variant *source, bool enabled);
extern "C" char *user_interface_game_settings_try_and_get_map_variant(void);
extern "C" long user_interface_squad_get_join_mode(void);

class c_network_session;
struct s_player_profile_summary;

extern "C" bool user_interface_session_get_player_profile_summary(e_controller_index controller_index, s_player_profile_summary *out);
long user_interface_session_get_player_at_local_controller(enum e_controller_index controller_index);
e_controller_index user_interface_squad_get_controller_index(long player_index);
e_campaign_difficulty_level user_interface_game_settings_get_campaign_difficulty();
bool user_interface_squad_set_map(e_campaign_id campaign_id, e_map_id map_id);
bool user_interface_squad_start_countdown_timer(e_controller_index controller_index, long countdown_timer, long minimum_timer);
bool user_interface_squad_stop_countdown_timer(e_controller_index controller_index, long countdown_timer, bool keep_countdown);
bool user_interface_get_interactive_session(c_network_session **session);
extern "C" bool user_interface_session_get_usable_player_xuid(c_network_session **session);
long user_interface_session_get_player_index(struct s_player_identifier const * identifier, class c_network_session const * session);

enum e_network_game_start_mode;
enum e_network_session_class;
enum e_session_game_start_status;
enum e_session_game_start_error;
enum e_session_game_mode;
class c_network_session_parameters;

extern "C" const char k_user_interface_session_source_file[];
enum e_session_game_start_status user_interface_get_session_game_start_status(enum e_session_game_start_error *, unsigned long *);
e_session_game_mode user_interface_get_session_game_mode(void);
bool user_interface_squad_set_start_mode(enum e_network_game_start_mode);
void user_interface_create_new_squad(e_network_session_class session_class);
bool user_interface_session_parameters_are_valid_for_ui_game_mode(class c_network_session_parameters const *);

#endif // INTERFACE_USER_INTERFACE_SESSION_H
