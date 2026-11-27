#ifndef INTERFACE_USER_INTERFACE_CONTROLLER_H
#define INTERFACE_USER_INTERFACE_CONTROLLER_H

// Functions defined in tu/interfacelibxbox/user_interface_controller.cpp.

void user_interface_controller_initialize(void);
void user_interface_controller_update(void);
extern "C" long user_interface_controller_get_rumble_enabled(long user_index);
extern "C" unsigned char user_interface_controller_has_blocked_communications(void);
extern "C" unsigned char user_interface_controller_has_friends_only_communications(void);

class c_player_profile_interface;
enum e_controller_index;
struct s_player_identifier;
struct s_player_configuration;

extern "C" bool user_interface_controller_test_button_hold(e_controller_index controller_index, unsigned char button_action, unsigned short duration);
void user_interface_controller_update_network_properties(e_controller_index controller_index);
const wchar_t *user_interface_controller_get_player_profile_name(e_controller_index controller_index);
bool user_interface_controller_voice_allowed(e_controller_index controller_index);
bool user_interface_controller_voice_is_for_friends_only(e_controller_index controller_index);
void user_interface_controller_update_profile_error(e_controller_index controller_index);
void user_interface_controller_reset(e_controller_index controller_index);
void user_interface_controller_get_player_configuration(e_controller_index controller_index, s_player_identifier *identifier,
    s_player_configuration *configuration);

extern "C" void user_interface_controller_clear_signin_cache(void);
void event_manager_update(unsigned long milliseconds);
extern "C" long user_interface_controller_get_campaign_progress(c_player_profile_interface *configuration_profile);

e_controller_index user_interface_controller_get_first_xbox_live_signed_in_non_guest(void);

extern "C" long levels_get_main_campaign_map_count(long campaign_id);

#endif // INTERFACE_USER_INTERFACE_CONTROLLER_H
