#ifndef NETWORKING_ONLINE_ONLINE_GUIDE_XENON_H
#define NETWORKING_ONLINE_ONLINE_GUIDE_XENON_H

#include "shell/shell_types.h"

// Functions defined in tu/networkinglibxbox/online_guide_xenon.cpp.

void online_guide_update_gamercard_dynamic_actions();
void online_guide_handle_custom_gamercard(bool closing);
void online_guide_update_get_game_details(void);
extern "C" void online_guide_handle_dynamic_action(void);
extern "C" void online_guide_update_service_record(void);

struct s_network_squad_status_data;


class c_string_verify_task;

extern "C" void string_verify_task_set_strings(c_string_verify_task *task, long language, const wchar_t **strings, long count);
void online_guide_set_dynamic_actions(bool show_details, bool allow_join);
extern "C" bool online_guide_can_join(void);

struct s_compose_text;
struct s_service_record_identity;
struct s_transport_secure_identifier;

unsigned long online_guide_show_gamer_card_ui(e_controller_index controller_index, unsigned __int64 xuid);
unsigned long online_guide_show_friends_ui(e_controller_index controller_index);
void online_guide_show_file_share_recommendation(e_controller_index controller_index, unsigned __int64 xuid, long value,
    const char *name, const s_service_record_identity *identity, const s_compose_text *texts);
void online_guide_show_player_details_for_player(e_controller_index controller_index, const s_transport_secure_identifier *session_id);

class c_virtual_keyboard_task;
class c_online_service_record_manager;
extern "C" c_virtual_keyboard_task *g_virtual_keyboard_task_instance;
extern c_online_service_record_manager g_online_service_record_manager;

#endif // NETWORKING_ONLINE_ONLINE_GUIDE_XENON_H
