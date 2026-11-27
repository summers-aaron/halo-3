#ifndef INCLUDE_NETWORKING_LOGIC_NETWORK_BANHAMMER_CONTROLLER_TYPES_H
#define INCLUDE_NETWORKING_LOGIC_NETWORK_BANHAMMER_CONTROLLER_TYPES_H

#include "input/input.h"
#include "networking/logic/network_banhammer.h"
#include "networking/logic/storage/network_http_blf_simple_downloader.h"
#include "networking/logic/network_banhammer_remote_data_types.h"
#include "networking/logic/network_quality_statistics_types.h"
#include "interface/files_machine_banhammer_messages_types.h"
#include "interface/files_user_banhammer_messages_types.h"
#include "interface/files_user_recent_players_types.h"

struct s_network_banhammer_controller_downloaders    // 0x2fa4
{
    c_http_stored_buffer_downloader<2769> user_file;
    c_http_blf_simple_downloader<s_files_user_recent_players> recent_players;

    s_network_banhammer_controller_downloaders();
    __forceinline ~s_network_banhammer_controller_downloaders() {}
};

struct s_network_banhammer_repeated_play_list
{
    long entry_count;
    s_network_banhammer_repeated_play_entry entries[384];
};

struct s_network_banhammer_controller_data {   // 0xfd8
    unsigned char flags;
    unsigned __int64 xuid;
    wchar_t name[16];
    unsigned long ban_flags;
    unsigned long ban_types;
    unsigned long cheats;
    unsigned long reported;
    long repeated_play_index;
    s_network_banhammer_repeated_play_list repeated_play_list;

    unsigned long user_file_refresh_time;
    unsigned long user_file_request_time;
    gamepad_state activity_gamepad_state;
    unsigned long last_activity_time;
    gamepad_state session_gamepad_state;
    unsigned long session_start_time;
    long highest_skill;
    long session_count;

    __forceinline s_network_banhammer_controller_data() : flags(0) {}
};
typedef char banhammer_controller_highest_skill_offset[(offsetof(s_network_banhammer_controller_data, highest_skill) == 0xfd0) ? 1 : -1];

struct s_network_banhammer_map_signatures {    // 0x5178
    bool valid;
    bool disabled;
    long last_retry_count;
    long count;
    unsigned char signatures[128][256];
};
struct s_network_banhammer_globals_base
{
    __forceinline s_network_banhammer_globals_base(s_network_banhammer_globals *self)
    {
        // match: 0x4009a0: folds this to the global address so members use absolute addressing instead of r3-relative
        __assume(self == &g_network_banhammer_globals);
    }
};
struct s_network_banhammer_globals : s_network_banhammer_globals_base {
    unsigned char flags;
    unsigned long cheats;
    unsigned long ban_flags;
    unsigned long ban_types;
    s_network_banhammer_controller_data controllers[4];
    s_network_banhammer_remote_data remotes[64];
    long machine_file_index;
    long machine_file_time;
    s_network_banhammer_map_signatures map_signatures;
    c_http_blf_simple_downloader<s_files_machine_banhammer_messages> machine_messages_downloader;
    c_http_stored_buffer_downloader<2161> machine_file_downloader;
    c_http_blf_simple_downloader<s_files_user_banhammer_messages> messages_downloader;
    s_network_banhammer_controller_downloaders controller_downloaders[4];
    c_http_stored_buffer_downloader<1024> network_statistics_upload_downloader;
    s_network_quality_statistics network_statistics;

    s_network_banhammer_globals();
    __forceinline ~s_network_banhammer_globals() {}
};

#endif
