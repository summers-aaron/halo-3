#ifndef INTERFACE_USER_INTERFACE_NETWORKING_H
#define INTERFACE_USER_INTERFACE_NETWORKING_H

#include "shell/shell_types.h"
#include "interface/gui_network_session_advertisement_types.h"
#include "networking/session/network_session_class.h"

// String ids the networking UI posts alerts and dialogs by (sets 2 and 3 of the string_ids table,
// decoded from mainmenu.map; dialog names double as c_dialog_result_message dialog names).
enum e_user_interface_networking_string_id
{
    _user_interface_networking_string_alpha_sign_in_failed_no_split_screen = 0x20002,
    _user_interface_networking_string_booted_from_game = 0x20009,
    _user_interface_networking_string_booted_from_session = 0x2000a,
    _user_interface_networking_string_campaign_start_failed_unknown_reason = 0x2000d,
    _user_interface_networking_string_join_failed_campaign_in_progress = 0x2001d,
    _user_interface_networking_string_join_failed_failed_to_find_session = 0x2001e,
    _user_interface_networking_string_join_failed_film_in_progress = 0x2001f,
    _user_interface_networking_string_join_failed_game_not_open = 0x20020,
    _user_interface_networking_string_join_failed_generic_error = 0x20021,
    _user_interface_networking_string_join_failed_host_timed_out = 0x20022,
    _user_interface_networking_string_join_failed_host_version_too_low = 0x20023,
    _user_interface_networking_string_join_failed_party_member_not_online_enabled = 0x20024,
    _user_interface_networking_string_join_failed_not_enough_space = 0x20025,
    _user_interface_networking_string_join_failed_peer_version_too_low = 0x20026,
    _user_interface_networking_string_join_failed_target_is_matchmaking = 0x20028,
    _user_interface_networking_string_join_failed_unable_to_connect_closed_nat = 0x20029,
    _user_interface_networking_string_join_failed_unable_to_connect_moderate_nat = 0x2002a,
    _user_interface_networking_string_join_failed_unable_to_connect_open_nat = 0x2002b,
    _user_interface_networking_string_join_failed_unable_to_connect_party_open_nat = 0x2002c,
    _user_interface_networking_string_join_failed_unable_to_connect_party_moderate_nat = 0x2002d,
    _user_interface_networking_string_join_failed_unable_to_connect_party_strict_nat = 0x2002e,
    _user_interface_networking_string_no_profile_sent_to_main_menu = 0x20039,
    _user_interface_networking_string_file_successfully_added_to_library = 0x2003b,
    _user_interface_networking_string_file_successfully_added_to_file_share = 0x20045,
    _user_interface_networking_string_sign_in_failed_game_full = 0x20061,
    _user_interface_networking_string_sign_in_failed_generic = 0x20062,
    _user_interface_networking_string_sign_in_game_in_campaign = 0x20063,
    _user_interface_networking_string_sign_in_game_in_matchmaking = 0x20064,
    _user_interface_networking_string_sign_in_game_in_theater = 0x20065,
    _user_interface_networking_string_film_clip_available_in_theater = 0x2006e,
    _user_interface_networking_string_game_variant_available_in_custom_games = 0x2006f,
    _user_interface_networking_string_map_available_in_custom_games_and_forge = 0x20070,
    _user_interface_networking_string_screenshot_available_in_screenshots = 0x20071,
    _user_interface_networking_string_film_clip_uploaded_to_file_share = 0x20075,
    _user_interface_networking_string_game_variant_uploaded_to_file_share = 0x20076,
    _user_interface_networking_string_map_uploaded_to_file_share = 0x20077,
    _user_interface_networking_string_screenshot_uploaded_to_file_share = 0x20078,
    _user_interface_networking_string_sign_in_user_content_not_permitted = 0x2008a,
    _user_interface_networking_string_join_failed_user_content_not_permitted = 0x2008b,
    _user_interface_networking_string_not_signed_in_bring_up_guide = 0x30000,
    _user_interface_networking_string_not_signed_in_bring_up_guide_or_use_temporary_account = 0x30001,
    _user_interface_networking_string_confirm_the_use_of_a_temporary_account = 0x30002,
    _user_interface_networking_string_sign_in_failed_online_permissions_1_player = 0x30008,
    _user_interface_networking_string_sign_in_failed_online_permissions_2_player = 0x30009,
    _user_interface_networking_string_sign_in_failed_online_permissions_3_player = 0x3000a,
    _user_interface_networking_string_sign_in_failed_online_permissions_4_player = 0x3000b,
    _user_interface_networking_string_join_party_from_party_leader = 0x3001a,
    _user_interface_networking_string_join_party_from_party_member = 0x3001b,
    _user_interface_networking_string_join_party_from_party_leader_in_campaign = 0x30042,
    _user_interface_networking_string_join_party_from_party_member_in_campaign = 0x30043,
    _user_interface_networking_string_join_party_from_party_leader_in_match = 0x30044,
    _user_interface_networking_string_join_party_from_party_member_in_match = 0x30045,
    _user_interface_networking_string_join_party_from_party_leader_in_match_lose_skill = 0x30046,
    _user_interface_networking_string_join_party_from_party_member_in_match_lose_skill = 0x30047,
    _user_interface_networking_string_join_party_from_party_leader_in_match_lose_skill_and_xp = 0x30048,
    _user_interface_networking_string_join_party_from_party_member_in_match_lose_skill_and_xp = 0x30049,
    _user_interface_networking_string_join_party_from_party_leader_in_match_lose_xp = 0x3004a,
    _user_interface_networking_string_join_party_from_party_member_in_match_lose_xp = 0x3004b,
    _user_interface_networking_string_join_party_from_party_leader_in_theater = 0x3004c,
    _user_interface_networking_string_join_party_from_party_member_in_theater = 0x3004d,
    _user_interface_networking_string_join_party_while_in_local_game = 0x3004e,
    _user_interface_networking_string_join_party_while_in_local_campaign = 0x3004f,
    _user_interface_networking_string_join_party_while_in_local_theater = 0x30050,
};

// Functions defined in tu/interfacelibxbox/user_interface_networking.cpp.

class c_game_results;

bool user_interface_networking_get_closed(void);
e_gui_network_session_advertisement_mode user_interface_networking_get_session_advertisement(void);
bool user_interface_networking_build_is_compatible(long type, long version, long build);
long user_interface_networking_get_current_party_size(void);
bool user_interface_networking_set_closed(bool closed);
const c_game_results *user_interface_networking_get_final_game_results();
void user_interface_networking_join_update(void);
bool user_interface_networking_should_show_teams(void);
void user_interface_networking_start_game_when_ready_update(void);
void user_interface_network_dialogs_update();

class c_dialog_result_message;
bool user_interface_networking_handle_join_dialog_result(const c_dialog_result_message *message);

extern "C" bool user_interface_networking_is_throttled(void);
extern "C" long user_interface_networking_get_current_location(void);
extern "C" bool user_interface_networking_dialogs_handle_results(const c_dialog_result_message *message);
extern "C" bool user_interface_networking_set_session_advertisement(long advertisement);
extern "C" bool user_interface_networking_set_pre_game_state(void);
extern "C" void user_interface_initialize_coop_game_settings(int unused_context, int controller_index);
e_controller_index user_interface_networking_get_saved_game_controller(void);

struct s_transport_secure_address;
struct s_transport_secure_identifier;
struct s_transport_secure_key;

void user_interface_join_remote_session(bool from_title, e_network_session_class session_class,
    const s_transport_secure_identifier *session_id, const s_transport_secure_address *host_address,
    const s_transport_secure_key *key);

#include "networking/session/network_session_types.h"
#include "networking/session/network_session_boot_reason_types.h"
void user_interface_networking_notify_booted_from_session(e_network_session_type session_type, e_network_session_boot_reason reason);
#endif // INTERFACE_USER_INTERFACE_NETWORKING_H
