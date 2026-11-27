#ifndef INTERFACE_USER_INTERFACE_DIALOG_GLOBALS_H
#define INTERFACE_USER_INTERFACE_DIALOG_GLOBALS_H
#include <stddef.h>
#include "networking/transport/transport_security_types.h"

struct s_user_interface_networking_globals {
    short minutes_spent_in_current_state;
    short minutes_remaining_in_current_state;
    unsigned long time_entered_current_state;
    bool join_in_progress;
    long last_join_error;
    union { bool start_pending; signed char loading; };
    long start_state;
    long start_controller;
    bool start_cancelled;
    long start_game_when_ready_user_count;
    long local_user_state;
    long controller;
    long screen_name;
    long error;
    long dialog;
    bool sign_in_lock;
    unsigned long last_request_time;
    unsigned char sign_in_required;
    unsigned char sign_in_requested;
    long dialog_name;
    unsigned char join_as_friend;
    long remote_session_class;
    s_transport_secure_identifier session_id;
    s_transport_secure_address host_address;
    s_transport_secure_key session_key;
    bool show_teams;
};
extern "C" s_user_interface_networking_globals g_user_interface_networking_globals;
typedef char ui_networking_join_offset[(offsetof(s_user_interface_networking_globals, join_in_progress) == 0x8) ? 1 : -1];
typedef char ui_networking_start_offset[(offsetof(s_user_interface_networking_globals, start_state) == 0x14) ? 1 : -1];
typedef char ui_networking_start_user_count_offset[(offsetof(s_user_interface_networking_globals, start_game_when_ready_user_count) == 0x20) ? 1 : -1];
typedef char ui_networking_controller_offset[(offsetof(s_user_interface_networking_globals, controller) == 0x28) ? 1 : -1];
typedef char ui_networking_session_offset[(offsetof(s_user_interface_networking_globals, session_id) == 0x50) ? 1 : -1];
typedef char ui_networking_size[(sizeof(s_user_interface_networking_globals) == 0x90) ? 1 : -1];
#endif
