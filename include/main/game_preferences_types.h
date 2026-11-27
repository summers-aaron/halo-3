#ifndef MAIN_GAME_PREFERENCES_TYPES_H
#define MAIN_GAME_PREFERENCES_TYPES_H

#include "main/game_preferences_game_setup.h"
#include "interface/gui_screen_pregame_lobby_mapeditor_types.h"

#include <stddef.h>
#include "tag_files/file_last_modification_date.h"
#include "networking/logic/storage/network_http_request_hash.h"
#include "main/game_preferences.h"
#include "networking/session/network_session_parameters.h"
#include "multithreading/synchronized_value.h"

struct s_game_preferences_snapshot
{
    int type;
    union { char data[64064]; int words[16016]; s_game_preferences_color color; };
};
struct s_global_preferences_data
{
    __forceinline s_global_preferences_data() : mode_settings(s_gui_game_setup_storage::_file_image) {}

    long version;
    long last_language;
    long font_language;
    s_file_last_modification_date font_modification_date;
    long map_count;
    long betrayal_count;
    long ejection_count;
    long last_menu_selection;
    s_gui_game_setup_storage mode_settings;
    e_gui_network_session_advertisement_mode xbox_live_private_privacy_setting;
    long brightness_setting;
    long subtitle_setting;
    long saved_game_file_name_counters[14];
};
struct s_game_preferences_file_image
{
    s_game_preferences_file_image();

    s_network_http_request_hash hash;
    s_global_preferences_data data;
    char file_alignment[0x2d000 - 0x2c1a8];
};
struct s_global_preferences
{
    bool initialized;
    bool dirty;
    void *file_handle;
    s_game_preferences_file_image primary_file;
    s_game_preferences_file_image shadow_file;
    long async_task_id;
    c_synchronized_long io_completion;
    bool async_write_pending;
};
typedef char preferences_map_count_offset[(offsetof(s_global_preferences_data, map_count) == 0x14) ? 1 : -1];
typedef char preferences_data_size[(sizeof(s_global_preferences_data) == 0x2c190) ? 1 : -1];
typedef char preferences_file_data_offset[(offsetof(s_game_preferences_file_image, data) == 0x18) ? 1 : -1];
typedef char preferences_file_size[(sizeof(s_game_preferences_file_image) == 0x2d000) ? 1 : -1];
typedef char preferences_primary_offset[(sizeof(void *) != 4 || ((offsetof(s_global_preferences, primary_file) == 8))) ? 1 : -1];
typedef char preferences_shadow_offset[(sizeof(void *) != 4 || ((offsetof(s_global_preferences, shadow_file) == 0x2d008))) ? 1 : -1];
typedef char preferences_globals_size[(sizeof(void *) != 4 || ((sizeof(s_global_preferences) == 0x5a018))) ? 1 : -1];
typedef char s_global_preferences_sync_offset[(sizeof(void *) != 4 || ((offsetof(s_global_preferences, io_completion) == 0x5a00c))) ? 1 : -1];


static __forceinline const s_game_preferences_mode_settings *game_preferences_get_mode_settings()
{
    return &global_preferences_get_game_setup_storage()->serialized_settings;
}

// Per-thread preferences published by c_thread_globals_lock.
extern __declspec(thread) s_global_preferences *thread_global_preferences;

#endif // MAIN_GAME_PREFERENCES_TYPES_H
