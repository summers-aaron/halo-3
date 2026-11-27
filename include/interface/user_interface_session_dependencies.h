#ifndef INCLUDE_INTERFACE_USER_INTERFACE_SESSION_DEPENDENCIES_H
#define INCLUDE_INTERFACE_USER_INTERFACE_SESSION_DEPENDENCIES_H

#include "main/game_preferences_game_setup.h"

#include "game/game_types.h"
#include "cseries/language_types.h"
#include "cseries/string_literal_pool.h"
#include "networking/online/online_cached_user.h"
class c_game_variant;
struct s_network_session_host_owner;
struct s_saved_game_item_metadata;
class c_game_engine_base_variant;
struct s_saved_film_description;
extern "C" long user_interface_session_get_controller_index(short player_index);
extern "C" long network_session_get_join_refuse_reason(s_network_session_host_owner *session);
extern "C" void wchar_string_to_utf8_string(const wchar_t *source, char *dest, long count, long *remaining);
extern "C" void game_engine_set_map_info_name(s_saved_game_item_metadata *info, const wchar_t *name);
extern "C" void gui_screen_game_options_set_variant_name(c_game_engine_base_variant *variant, const wchar_t *name);
extern "C" void game_engine_variant_set_description(c_game_engine_base_variant *variant, const char *description);
extern "C" bool user_interface_squad_set_film(const s_saved_film_description *source, bool enabled);
extern "C" void game_engine_variant_initialize_default(c_game_variant *variant, e_game_engine_type type);

struct s_player_identifier;

typedef const s_player_identifier *(*user_interface_session_identifier_provider)(long);
user_interface_session_identifier_provider network_session_interface_identifier_provider();

#endif // INCLUDE_INTERFACE_USER_INTERFACE_SESSION_DEPENDENCIES_H
